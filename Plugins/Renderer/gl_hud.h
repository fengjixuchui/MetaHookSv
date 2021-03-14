#pragma once

#include "enginedef.h"

#define MAX_GAUSSIAN_SAMPLES 16
#define LUMIN1x1_BUFFERS 3
#define DOWNSAMPLE_BUFFERS 2
#define LUMIN_BUFFERS 3
#define BLUR_BUFFERS 3

typedef struct
{
	int program;
	int tex0;
	int rt_w;
	int rt_h;
}pp_fxaa_program_t;

typedef struct
{
	int program;
	int tex;
}pp_downsample_program_t;

typedef struct
{
	int program;
	int texelsize;
	int tex;
}pp_downsample2x2_program_t;

typedef struct
{
	int program;
	int texelsize;
	int tex;
}pp_lumin_program_t;

typedef struct
{
	int program;
	int texelsize;
	int tex;
}pp_luminlog_program_t;

typedef struct
{
	int program;
	int texelsize;
	int tex;
}pp_luminexp_program_t;

typedef struct
{
	int program;
	int curtex;
	int adatex;
	int frametime;
}pp_luminadapt_program_t;

typedef struct
{
	int program;
	int tex;
	int lumtex;
}pp_brightpass_program_t;

typedef struct
{
	int program;
	int tex;
	int du;
}pp_gaussianblurv_program_t, pp_gaussianblurh_program_t;

typedef struct
{
	int program;
	int basetex;
	int blurtex;
	int lumtex;
	int blurfactor;
	int exposure;
	int darkness;
	int gamma;
}pp_tonemap_program_t;

typedef struct
{
	int program;
}depth_linearize_program_t;

typedef struct
{
	int program;
}depth_linearize_msaa_program_t;

typedef struct
{
	int program;
	int texLinearDepth;
	int texRandom;
	int control_RadiusToScreen;
	int control_projOrtho;
	int control_projInfo;
	int control_PowExponent;
	int control_InvQuarterResolution;
	int control_AOMultiplier;
	int control_InvFullResolution;
	int control_NDotVBias;
	int control_NegInvR2;

	int control_Fog;
}hbao_calc_blur_program_t, hbao_calc_blur_fog_program_t;

typedef struct
{
	int program;	
}hbao_blur_program_t, hbao_blur2_program_t;

extern cvar_t *r_hdr;
extern cvar_t *r_hdr_debug;
extern cvar_t *r_ssao;
extern cvar_t *r_ssao_debug;
extern cvar_t *r_ssao_radius;
extern cvar_t *r_ssao_intensity;
extern cvar_t *r_ssao_bias;
extern cvar_t *r_ssao_blur_sharpness;
extern cvar_t *r_ssao_studio_model;
extern cvar_t *r_fxaa;

extern int last_luminance;

void R_BeginHUDQuad(void);
void R_BeginFXAA(int w, int h);
int R_DoSSAO(int sampleIndex);
void R_DoHDR(void);
void R_DoFXAA(void);
void R_BlitToScreen(FBO_Container_t *src);
void R_BlitToFBO(FBO_Container_t *src, FBO_Container_t *dst);
void R_DrawHUDQuad(int w, int h);
void R_DrawHUDQuad_Texture(int tex, int w, int h);