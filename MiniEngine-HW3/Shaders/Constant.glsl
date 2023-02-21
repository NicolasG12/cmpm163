in float3 vertexPosition;
in float3 vertexNormal;
in float4 vertexTangent;
in float2 vertexTexcoord;

layout(location = 32) uniform float4 fparam[2];

out float4 fragmentColor;

void main()
{
	fragmentColor = fparam[0];
}
