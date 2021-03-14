#include <metahook.h>
#include "exportfuncs.h"
#include "gl_local.h"
#include "command.h"
#include "parsemsg.h"
#include "qgl.h"

//Error when can't find sig
void Sys_ErrorEx(const char *fmt, ...)
{
	char msg[4096] = { 0 };

	va_list argptr;

	va_start(argptr, fmt);
	_vsnprintf(msg, sizeof(msg), fmt, argptr);
	va_end(argptr);

	if(gEngfuncs.pfnClientCmd)
		gEngfuncs.pfnClientCmd("escape\n");

	MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR);
	TerminateProcess((HANDLE)(-1), 0);
}

char *UTIL_VarArgs(char *format, ...)
{
	va_list argptr;
	static int index = 0;
	static char string[16][1024];

	va_start(argptr, format);
	vsprintf(string[index], format, argptr);
	va_end(argptr);

	char *result = string[index];
	index = (index + 1) % 16;
	return result;
}

cl_enginefunc_t gEngfuncs;
engine_studio_api_t IEngineStudio;
r_studio_interface_t **gpStudioInterface;

shaderapi_t ShaderAPI = 
{
	R_CompileShader,
	R_CompileShaderFile,
	R_CompileShaderFileEx,
	GL_UseProgram,
	GL_EndProgram,
	GL_GetUniformLoc,
	GL_GetAttribLoc,
	GL_Uniform1i,
	GL_Uniform2i,
	GL_Uniform3i,
	GL_Uniform4i,
	GL_Uniform1f,
	GL_Uniform2f,
	GL_Uniform3f,
	GL_Uniform4f,
	GL_VertexAttrib3f,
	GL_VertexAttrib3fv,
	GL_MultiTexCoord2f,
	GL_MultiTexCoord3f
};

engrefapi_t RefAPI = 
{
	GL_Bind,
	GL_SelectTexture,
	GL_DisableMultitexture,
	GL_EnableMultitexture,
	R_DrawBrushModel,
	R_DrawSpriteModel,
	R_GetSpriteAxes,
	R_SpriteColor,
	GlowBlend,
	CL_FxBlend,
	R_CullBox
};

ref_export_t gRefExports =
{
	//common
	R_GetDrawPass,
	//refdef
	R_PushRefDef,
	R_PopRefDef,
	R_GetRefDef,
	//framebuffer
	GL_PushFrameBuffer,
	GL_PopFrameBuffer,
	//texture
	GL_GenTextureRGBA8,
	R_LoadTextureEx,
	GL_LoadTextureEx,
	R_GetCurrentGLTexture,
	GL_UploadDXT,
	LoadDDS,
	LoadImageGeneric,
	SaveImageGeneric,
	//capture screen
	R_GetSCRCaptureBuffer,
	//2d postprocess
	R_BeginFXAA,
	//cloak
	//shader
	ShaderAPI,
	RefAPI
};

HWND GetMainHWND()
{
	//todo
	return 0;
}

void hudGetMousePos(struct tagPOINT *ppt)
{
	gEngfuncs.pfnGetMousePos(ppt);

	int iVideoWidth, iVideoHeight, iBPP;
	bool bWindowed = false;

	g_pMetaHookAPI->GetVideoMode(&iVideoWidth, &iVideoHeight, &iBPP, &bWindowed);

	if ( !bWindowed )
	{
		RECT rectWin;
		GetWindowRect(GetMainHWND(), &rectWin);
		int videoW = iVideoWidth;
		int videoH = iVideoHeight;
		int winW = rectWin.right - rectWin.left;
		int winH = rectWin.bottom - rectWin.top;
		ppt->x *= (float)videoW / winW;
		ppt->y *= (float)videoH / winH;
		ppt->x *= (*windowvideoaspect - 1) * (ppt->x - videoW / 2);
		ppt->y *= (*videowindowaspect - 1) * (ppt->y - videoH / 2);
	}
}

void hudGetMousePosition(int *x, int *y)
{
	gEngfuncs.GetMousePosition(x, y);

	int iVideoWidth, iVideoHeight, iBPP;
	bool bWindowed = false;

	g_pMetaHookAPI->GetVideoMode(&iVideoWidth, &iVideoHeight, &iBPP, &bWindowed);

	if ( !bWindowed )
	{
		RECT rectWin;
		GetWindowRect(GetMainHWND(), &rectWin);
		int videoW = iVideoWidth;
		int videoH = iVideoHeight;
		int winW = rectWin.right - rectWin.left;
		int winH = rectWin.bottom - rectWin.top;
		*x *= (float)videoW / winW;
		*y *= (float)videoH / winH;
		*x *= (*windowvideoaspect - 1) * (*x - videoW / 2);
		*y *= (*videowindowaspect - 1) * (*y - videoH / 2);
	}
}

void R_Version_f(void)
{
	gEngfuncs.Con_Printf("Renderer Version:\n%s\n", META_RENDERER_VERSION);
}

void HUD_Init(void)
{
	gExportfuncs.HUD_Init();

	R_Init();

	gEngfuncs.pfnAddCommand("r_version", R_Version_f);
}

int HUD_VidInit(void)
{
	return gExportfuncs.HUD_VidInit();
}

void V_CalcRefdef(struct ref_params_s *pparams)
{
	gExportfuncs.V_CalcRefdef(pparams);

	R_CalcRefdef(pparams);
}

void HUD_DrawNormalTriangles(void)
{
	R_EndRenderGBuffer();

	GL_DisableMultitexture();

	if (!r_draw_pass)
	{
		R_RenderShadowScenes();
	}

	if (!r_refdef->onlyClientDraws)
	{
		if (R_UseMSAA())
		{
			for (int sampleIndex = 0; sampleIndex < max(1, gl_msaa_samples); sampleIndex++)
			{
				if (!R_DoSSAO(sampleIndex))
				{
					break;
				}
			}
		}
		else
		{
			R_DoSSAO(-1);
		}
	}

	//Allow SCClient to write stencil buffer (but not bit 1)?

	qglStencilMask(0xFF);
	qglClear(GL_STENCIL_BUFFER_BIT);
	gExportfuncs.HUD_DrawNormalTriangles();
	qglStencilMask(0);

	//Restore current framebuffer just in case that Sven-Coop client changes it
	
	if (r_draw_pass == r_draw_reflect || r_draw_pass == r_draw_refract)
	{
		qglBindFramebufferEXT(GL_FRAMEBUFFER, s_WaterFBO.s_hBackBufferFBO);
	}
	else
	{
		if (R_UseMSAA())
			qglBindFramebufferEXT(GL_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO);
		else
			qglBindFramebufferEXT(GL_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO);
	}
}

void HUD_DrawTransparentTriangles(void)
{
	if (!r_draw_pass)
	{
		R_FreeDeadWaters();
		for (r_water_t *water = waters_active; water; water = water->next)
		{
			water->free = true;
		}
	}

	gExportfuncs.HUD_DrawTransparentTriangles();
}

int HUD_Redraw(float time, int intermission)
{
	if(waters_active && r_water_debug && r_water_debug->value > 0 && r_water_debug->value <= 3)
	{
		qglDisable(GL_BLEND);
		qglDisable(GL_ALPHA_TEST);
		qglColor4f(1, 1, 1, 1);

		qglEnable(GL_TEXTURE_2D);
		int debugTextureID = 0;
		switch ((int)r_water_debug->value)
		{
		case 1:
			debugTextureID = waters_active->reflectmap;
			break;
		case 2:
			debugTextureID = waters_active->refractmap;
			break;
		case 3:
			debugTextureID = waters_active->depthrefrmap;
			qglUseProgramObjectARB(drawdepth.program);
			break;
		case 4:
			debugTextureID = waters_active->depthreflmap;
			qglUseProgramObjectARB(drawdepth.program);
			break;
		default:
			break;
		}

		if (debugTextureID)
		{
			qglBindTexture(GL_TEXTURE_2D, debugTextureID);
			qglBegin(GL_QUADS);
			qglTexCoord2f(0, 1);
			qglVertex3f(0, 0, 0);
			qglTexCoord2f(1, 1);
			qglVertex3f(glwidth / 2, 0, 0);
			qglTexCoord2f(1, 0);
			qglVertex3f(glwidth / 2, glheight / 2, 0);
			qglTexCoord2f(0, 0);
			qglVertex3f(0, glheight / 2, 0);
			qglEnd();

			qglUseProgramObjectARB(0);
		}
	}
	else if(r_shadow_debug && r_shadow_debug->value)
	{
		qglDisable(GL_BLEND);
		qglDisable(GL_ALPHA_TEST);
		qglColor4f(1,1,1,1);

		qglEnable(GL_TEXTURE_2D);
		qglBindTexture(GL_TEXTURE_2D, shadow_depthmap_high);

		qglUseProgramObjectARB(drawdepth.program);

		qglBegin(GL_QUADS);
		qglTexCoord2f(0,1);
		qglVertex3f(0,0,0);
		qglTexCoord2f(1,1);
		qglVertex3f(glwidth/2,0,0);
		qglTexCoord2f(1,0);
		qglVertex3f(glwidth/2,glheight/2,0);
		qglTexCoord2f(0,0);
		qglVertex3f(0,glheight/2,0);
		qglEnd();
		qglEnable(GL_ALPHA_TEST);

		qglUseProgramObjectARB(0);
	}
	
	else if(r_light_debug && r_light_debug->value)
	{
		qglDisable(GL_BLEND);
		qglDisable(GL_ALPHA_TEST);
		qglColor4f(1,1,1,1);

		qglEnable(GL_TEXTURE_2D);
		if(r_light_debug->value == 1)
			qglBindTexture(GL_TEXTURE_2D, s_GBufferFBO.s_hBackBufferTex);
		else if (r_light_debug->value == 2)
			qglBindTexture(GL_TEXTURE_2D, s_GBufferFBO.s_hBackBufferTex2);
		else if (r_light_debug->value == 3)
			qglBindTexture(GL_TEXTURE_2D, s_GBufferFBO.s_hBackBufferTex3);
		else if (r_light_debug->value == 4)
			qglBindTexture(GL_TEXTURE_2D, s_GBufferFBO.s_hBackBufferTex4);
		else if (r_light_debug->value == 5)
			qglBindTexture(GL_TEXTURE_2D, s_GBufferFBO.s_hBackBufferTex5);
		else
		{
			qglUseProgramObjectARB(drawdepth.program);
			qglBindTexture(GL_TEXTURE_2D, s_GBufferFBO.s_hBackBufferDepthTex);
		}

		qglBegin(GL_QUADS);
		qglTexCoord2f(0,1);
		qglVertex3f(0,0,0);
		qglTexCoord2f(1,1);
		qglVertex3f(glwidth/2,0,0);
		qglTexCoord2f(1,0);
		qglVertex3f(glwidth/2,glheight/2,0);
		qglTexCoord2f(0,0);
		qglVertex3f(0,glheight/2,0);
		qglEnd();
		qglEnable(GL_ALPHA_TEST);

		qglUseProgramObjectARB(0);
	}
	else if(r_hdr_debug && r_hdr_debug->value)
	{
		qglDisable(GL_BLEND);
		qglDisable(GL_ALPHA_TEST);
		qglColor4f(1,1,1,1);

		qglEnable(GL_TEXTURE_2D);
		FBO_Container_t *pFBO = NULL;
		switch((int)r_hdr_debug->value)
		{
		case 1:
			pFBO = &s_DownSampleFBO[0];break;
		case 2:
			pFBO = &s_DownSampleFBO[1];break;
		case 3:
			pFBO = &s_BrightPassFBO;break;
		case 4:
			pFBO = &s_BlurPassFBO[0][0];break;
		case 5:
			pFBO = &s_BlurPassFBO[0][1];break;
		case 6:
			pFBO = &s_BlurPassFBO[1][0];break;
		case 7:
			pFBO = &s_BlurPassFBO[1][1];break;
		case 8:
			pFBO = &s_BlurPassFBO[2][0];break;
		case 9:
			pFBO = &s_BlurPassFBO[2][1];break;
		case 10:
			pFBO = &s_BrightAccumFBO;break;
		case 11:
			pFBO = &s_ToneMapFBO;break;
		case 12:
			pFBO = &s_BackBufferFBO; break;
		default:
			break;
		}

		if(pFBO)
		{
			qglBindTexture(GL_TEXTURE_2D, pFBO->s_hBackBufferTex);
			qglBegin(GL_QUADS);
			qglTexCoord2f(0,1);
			qglVertex3f(0,0,0);
			qglTexCoord2f(1,1);
			qglVertex3f(glwidth/2, 0,0);
			qglTexCoord2f(1,0);
			qglVertex3f(glwidth/2, glheight/2,0);
			qglTexCoord2f(0,0);
			qglVertex3f(0, glheight/2,0);
			qglEnd();
		}
	}
	else if (r_ssao_debug && r_ssao_debug->value)
	{
		qglDisable(GL_BLEND);
		qglDisable(GL_ALPHA_TEST);
		qglColor4f(1, 1, 1, 1);

		qglEnable(GL_TEXTURE_2D);
		int texId = 0;
		switch ((int)r_ssao_debug->value)
		{	
		case 1:
			qglUseProgramObjectARB(drawdepth.program);
			texId = s_BackBufferFBO.s_hBackBufferDepthTex; break;
		case 2:
			texId = s_DepthLinearFBO.s_hBackBufferTex; break;
		case 3:
			texId = s_HBAOCalcFBO.s_hBackBufferTex; break;
		case 4:
			texId = s_HBAOCalcFBO.s_hBackBufferTex2; break;
		default:
			break;
		}

		if (texId)
		{
			qglBindTexture(GL_TEXTURE_2D, texId);
			qglBegin(GL_QUADS);
			qglTexCoord2f(0, 1);
			qglVertex3f(0, 0, 0);
			qglTexCoord2f(1, 1);
			qglVertex3f(glwidth / 2, 0, 0);
			qglTexCoord2f(1, 0);
			qglVertex3f(glwidth / 2, glheight / 2, 0);
			qglTexCoord2f(0, 0);
			qglVertex3f(0, glheight / 2, 0);
			qglEnd();

			qglUseProgramObjectARB(0);
		}
	}
	return gExportfuncs.HUD_Redraw(time, intermission);
}

typedef struct portal_texture_s
{
	struct portal_texture_s *next;
	struct portal_texture_s *prev;
	GLuint gl_texturenum1;
	GLuint gl_texturenum2;
}portal_texture_t;

void __fastcall PortalManager_ResetAll(int pthis, int)
{
	portal_texture_t *ptextures = *(portal_texture_t **)(pthis + 0x9C);

	if (ptextures->next != ptextures)
	{
		do
		{
			//qglDeleteTextures(1, &ptextures->gl_texturenum2);
			ptextures->gl_texturenum2 = 0;
			ptextures = ptextures->next;
		} while (ptextures != *(portal_texture_t **)(pthis + 0x9C) );
	}

	gRefFuncs.PortalManager_ResetAll(pthis, 0);
}

void __fastcall StudioSetupBones(void *pthis, int)
{
	if (R_StudioRestoreBones())
		return;

	gRefFuncs.StudioSetupBones(pthis, 0);

	R_StudioSaveBones();
}

int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio)
{
	DWORD addr;

	//Save StudioAPI Funcs
	gRefFuncs.studioapi_SetupRenderer = pstudio->SetupRenderer;
	gRefFuncs.studioapi_RestoreRenderer = pstudio->RestoreRenderer;
	gRefFuncs.studioapi_StudioDynamicLight = pstudio->StudioDynamicLight;
	gRefFuncs.studioapi_SetupModel = pstudio->StudioSetupModel;

	//Vars in Engine Studio API
	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->GetCurrentEntity, 0x10, "\xA1", 1);
	Sig_AddrNotFound("currententity");
	currententity = *(cl_entity_t ***)(addr + 0x1);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->GetTimes, 0x10, "\xA1\x2A\x2A\x2A\x2A\x89", 6);
	Sig_AddrNotFound("r_framecount");
	r_framecount = *(int **)(addr + 1);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)addr, 0x20, "\xDD\x05\x2A\x2A\x2A\x2A\xDD\x18", 8);
	cl_time = *(double **)(addr + 2);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)(addr + 8) , 0x20, "\xDD\x05\x2A\x2A\x2A\x2A\xDD\x18", 8);
	cl_oldtime = *(double **)(addr + 2);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->SetRenderModel, 0x10, "\xA3", 1);
	Sig_AddrNotFound("r_model");
	r_model = *(model_t ***)(addr + 1);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->StudioSetHeader, 0x10, "\xA3", 1);
	Sig_AddrNotFound("pstudiohdr");
	pstudiohdr = *(studiohdr_t ***)(addr + 1);

	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->SetForceFaceFlags, 0x10, "\xA3", 1);
	Sig_AddrNotFound("g_ForcedFaceFlags");
	g_ForcedFaceFlags = *(int **)(addr + 1);
	
	//call	CL_FxBlend
	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->StudioSetRenderamt, 0x50, "\xE8", 1);
	Sig_AddrNotFound("CL_FxBlend");
	gRefFuncs.CL_FxBlend = (int (*)(cl_entity_t *))GetCallAddress(addr);

	//fstp    r_blend
	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->StudioSetRenderamt, 0x50, "\xD9\x1D", sizeof("\xD9\x1D")-1);
	Sig_AddrNotFound("r_blend");
	r_blend = *(float **)(addr + 2);

#define G_CHROMEORIGIN_SIG_SVENGINE "\xD9\x05\x2A\x2A\x2A\x2A\xD9\x1D"
	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->SetChromeOrigin, 0x150, G_CHROMEORIGIN_SIG_SVENGINE, sizeof(G_CHROMEORIGIN_SIG_SVENGINE) - 1);
	Sig_AddrNotFound("g_ChromeOrigin");
	g_ChromeOrigin = *(decltype(g_ChromeOrigin) *)(addr + 8);

	pbonetransform = (float (*)[MAXSTUDIOBONES][3][4])pstudio->StudioGetBoneTransform();
	plighttransform = (float (*)[MAXSTUDIOBONES][3][4])pstudio->StudioGetLightTransform();

	if (g_iEngineType == ENGINE_SVENGINE)
	{
#define PSUBMODEL_SIG_SVENGINE "\xC7\x00\x2A\x2A\x2A\x2A\xC3"
		addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->StudioSetupModel, 0x50, PSUBMODEL_SIG_SVENGINE, sizeof(PSUBMODEL_SIG_SVENGINE) - 1);
		Sig_AddrNotFound("psubmodel");
		psubmodel = *(decltype(psubmodel)*)(addr + 2);

#define R_COLORMIX_SIG_SVENGINE "\xD9\x46\x08\xD9\x1D\x2A\x2A\x2A\x2A\xD9\x46\x0C"
		addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)pstudio->StudioSetupLighting, 0x150, R_COLORMIX_SIG_SVENGINE, sizeof(R_COLORMIX_SIG_SVENGINE) - 1);
		Sig_AddrNotFound("psubmodel");
		r_colormix = *(decltype(r_colormix)*)(addr + 5);
	}

	cl_viewent = gEngfuncs.GetViewModel();

	//Save Studio API
	memcpy(&IEngineStudio, pstudio, sizeof(IEngineStudio));
	gpStudioInterface = ppinterface;

	//InlineHook StudioAPI
	InstallHook(studioapi_SetupRenderer);
	InstallHook(studioapi_RestoreRenderer);
	InstallHook(studioapi_StudioDynamicLight);
	//InstallHook(studioapi_SetupModel);

	cl_sprite_white = IEngineStudio.Mod_ForName("sprites/white.spr", 1);

	cl_shellchrome = IEngineStudio.Mod_ForName("sprites/shellchrome.spr", 1);

	//Fix SvClient Portal Rendering Confliction
	if (g_iEngineType == ENGINE_SVENGINE)
	{
		PUCHAR ClientBase = (PUCHAR)GetModuleHandleA("client.dll");
		if (ClientBase)
		{
			auto ClientSize = g_pMetaHookAPI->GetModuleSize((HMODULE)ClientBase);

#define SVCLIENT_PORTALMANAGER_RESETALL_SIG "\xC7\x45\x2A\xFF\xFF\xFF\xFF\xA3\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x8B\x0D"
		 DWORD addr = (DWORD)
				g_pMetaHookAPI->SearchPattern((void *)ClientBase, ClientSize, SVCLIENT_PORTALMANAGER_RESETALL_SIG, sizeof(SVCLIENT_PORTALMANAGER_RESETALL_SIG) - 1);
			Sig_AddrNotFound("PortalManager_ResetAll");

			gRefFuncs.PortalManager_ResetAll = (decltype(gRefFuncs.PortalManager_ResetAll))GetCallAddress(addr + 12);

			g_pMetaHookAPI->InlineHook(gRefFuncs.PortalManager_ResetAll, PortalManager_ResetAll, (void *&)gRefFuncs.PortalManager_ResetAll);

#define SVCLIENT_STUDIO_SETUP_BONES_SIG "\x83\xEC\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC4\x89\x44\x24\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x8B\x4F\x40"
			gRefFuncs.StudioSetupBones = (decltype(gRefFuncs.StudioSetupBones))g_pMetaHookAPI->SearchPattern((void *)ClientBase, ClientSize, SVCLIENT_STUDIO_SETUP_BONES_SIG, sizeof(SVCLIENT_STUDIO_SETUP_BONES_SIG) - 1);
			Sig_FuncNotFound(StudioSetupBones);

			g_pMetaHookAPI->InlineHook(gRefFuncs.StudioSetupBones, StudioSetupBones, (void *&)gRefFuncs.StudioSetupBones);
		}
	}

	return gExportfuncs.HUD_GetStudioModelInterface(version, ppinterface, pstudio);
}

int HUD_UpdateClientData(client_data_t *pcldata, float flTime)
{
	scr_fov_value = pcldata->fov;
	return gExportfuncs.HUD_UpdateClientData(pcldata, flTime);
}

int HUD_AddEntity(int type, cl_entity_t *ent, const char *model)
{
	return gExportfuncs.HUD_AddEntity(type, ent, model);
}

void HUD_Frame(double time)
{
	return gExportfuncs.HUD_Frame(time);
}