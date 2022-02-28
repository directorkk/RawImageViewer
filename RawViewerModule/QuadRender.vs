varying vec2 texcoord;
varying vec3 position;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	position = gl_Vertex.xyz;
	texcoord = gl_MultiTexCoord0.st;
}