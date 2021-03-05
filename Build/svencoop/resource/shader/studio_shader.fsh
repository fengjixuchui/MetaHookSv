#version 130
uniform sampler2D diffuseTex;

varying vec4 worldpos;
varying vec4 normal;
varying vec4 color;

void main(void)
{
	vec4 diffuseColor = texture2D(diffuseTex, gl_TexCoord[0].xy);

#ifdef GBUFFER_ENABLED

	#ifdef TRANSPARENT_ENABLED

		#ifdef STUDIO_NF_ADDITIVE

			gl_FragColor = diffuseColor * color;

		#else

			gl_FragData[0] = diffuseColor;
			gl_FragData[1] = color;

		#endif

	#else

		gl_FragData[0] = diffuseColor;
		gl_FragData[1] = color;
		gl_FragData[2] = worldpos;
		gl_FragData[3] = normal;

	#endif

#else

	gl_FragColor = diffuseColor * color;

#endif
}