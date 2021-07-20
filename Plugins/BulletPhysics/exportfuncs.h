#include <metahook.h>
#include <studio.h>
#include <r_studioint.h>
#include "cl_entity.h"

extern cl_enginefunc_t gEngfuncs;
extern cl_exportfuncs_t gExportfuncs;
extern mh_interface_t *g_pInterface;
extern metahook_api_t *g_pMetaHookAPI;
extern mh_enginesave_t *g_pMetaSave;
extern IFileSystem *g_pFileSystem;
extern DWORD g_dwEngineBuildnum;

void R_NewMap(void);
int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion);
int HUD_AddEntity(int type, cl_entity_t *ent, const char *model);
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio);
void HUD_TempEntUpdate(
	double frametime,   // Simulation time
	double client_time, // Absolute time on client
	double cl_gravity,  // True gravity on client
	TEMPENTITY **ppTempEntFree,   // List of freed temporary ents
	TEMPENTITY **ppTempEntActive, // List 
	int(*Callback_AddVisibleEntity)(cl_entity_t *pEntity),
	void(*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp));
void HUD_DrawTransparentTriangles(void);
void HUD_Init(void);
void V_CalcRefdef(struct ref_params_s *pparams);

void Sys_ErrorEx(const char *fmt, ...);

#define GetCallAddress(addr) (addr + (*(int *)((addr)+1)) + 5)
#define Sig_NotFound(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum��%d", #name, g_dwEngineBuildnum);
#define Sig_FuncNotFound(name) if(!gRefFuncs.name) Sig_NotFound(name)
#define Sig_VarNotFound(name) if(!name) Sig_NotFound(name)
#define Sig_AddrNotFound(name) if(!addr) Sig_NotFound(name)
#define SIG_NOT_FOUND(name) Sys_ErrorEx("Could not found: %s\nEngine buildnum��%d", name, g_dwEngineBuildnum);

#define Sig_Length(a) (sizeof(a)-1)
#define Search_Pattern(sig) g_pMetaHookAPI->SearchPattern(g_dwEngineTextBase, g_dwEngineTextSize, sig, Sig_Length(sig));
#define Search_Pattern_Data(sig) g_pMetaHookAPI->SearchPattern(g_dwEngineDataBase, g_dwEngineDataSize, sig, Sig_Length(sig));
#define Search_Pattern_Rdata(sig) g_pMetaHookAPI->SearchPattern(g_dwEngineRdataBase, g_dwEngineRdataSize, sig, Sig_Length(sig));
#define Search_Pattern_From(fn, sig) g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.fn, ((PUCHAR)g_dwEngineTextBase + g_dwEngineTextSize) - (PUCHAR)gRefFuncs.fn, sig, Sig_Length(sig));
#define InstallHook(fn) g_pMetaHookAPI->InlineHook((void *)gPrivateFuncs.fn, fn, (void *&)gPrivateFuncs.fn);