varying vec2 texcoord;
varying vec3 position;

uniform sampler2D tex_0;

void main()
{
	texcoord.t = 1.0 - texcoord.t;
	vec4 tex_0_color = texture2D(tex_0, texcoord).rgba;
	
	gl_FragColor = tex_0_color;
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}