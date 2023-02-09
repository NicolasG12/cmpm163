layout(location = 0) in float3 positionAttrib;

layout(location = 0) uniform float4 vparam[7];

void main()
{
	gl_Position.x = dot(vparam[0].xyz, positionAttrib) + vparam[0].w;
	gl_Position.y = dot(vparam[1].xyz, positionAttrib) + vparam[1].w;
	gl_Position.z = dot(vparam[2].xyz, positionAttrib) + vparam[2].w;
	gl_Position.w = dot(vparam[3].xyz, positionAttrib) + vparam[3].w;
}
