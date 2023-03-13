layout(location = 0) in float3 positionAttrib;
layout(location = 1) in float3 normalAttrib;
layout(location = 2) in float4 tangentAttrib;
layout(location = 3) in float2 texcoordAttrib;

layout(location = 0) uniform float4 vparam[7];

out float3 vertexPosition;
out float3 vertexNormal;
out float4 vertexTangent;
out float2 vertexTexcoord;

layout(std140) uniform universal
{
	float4		cameraPosition;				// The world-space position of the camera.
	float4		cameraRight;				// The world-space right direction of the camera.
	float4		cameraDown;					// The world-space down direction of the camera.
	float4		ambientColor;				// The constant ambient color in xyz. The w component is not used.
	float4		lightColor;					// The color of the light in xyz. The w component is not used.
	float4		lightPosition;				// The world-space position of the light source.
	float4		attenConst;					// The constant values for attenuation shown in Listing 8.2.
	float4		fogPlane;					// The world-space fog plane f.
	float4		fogColor;					// The color of the fog. The w component is not used.
	float4		fogParams;					// The fog density in x. The value of m from Equation (8.116) in y. The value dot(f, c) in z. The value sgn(dot(f, c)) in w.
	float4		shadowParams;				// The depth transform (p22, p23) is stored in the x and y components. The shadow offset is stored in the z component. The w component is not used.
	float4		inverseLightTransform[3];	// The first 3 rows of the world-to-light transform.
};

void main()
{
	// Apply model-view-projection matrix to vertex position.

	gl_Position.x = dot(vparam[0].xyz, positionAttrib) + vparam[0].w;
	gl_Position.y = dot(vparam[1].xyz, positionAttrib) + vparam[1].w;
	gl_Position.z = dot(vparam[2].xyz, positionAttrib) + vparam[2].w;
	gl_Position.w = dot(vparam[3].xyz, positionAttrib) + vparam[3].w;

	// Transform position into world space.

	float3		P_wld;

	P_wld.x = dot(vparam[4].xyz, positionAttrib) + vparam[4].w;
	P_wld.y = dot(vparam[5].xyz, positionAttrib) + vparam[5].w;
	P_wld.z = dot(vparam[6].xyz, positionAttrib) + vparam[6].w;

	vertexPosition = P_wld;

	// Transform normal into world space. This assumes object-to-world matrix is orthogonal.

	vertexNormal.x = dot(vparam[4].xyz, normalAttrib);
	vertexNormal.y = dot(vparam[5].xyz, normalAttrib);
	vertexNormal.z = dot(vparam[6].xyz, normalAttrib);

	// Transform tangent into world space.

	vertexTangent.x = dot(vparam[4].xyz, tangentAttrib.xyz);
	vertexTangent.y = dot(vparam[5].xyz, tangentAttrib.xyz);
	vertexTangent.z = dot(vparam[6].xyz, tangentAttrib.xyz);
	vertexTangent.w = tangentAttrib.w;

	// Copy texture coordinates.

	vertexTexcoord = texcoordAttrib;
}
