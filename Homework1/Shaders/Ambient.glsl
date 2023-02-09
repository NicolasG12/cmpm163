in float3 vertexPosition;
in float3 vertexNormal;
in float4 vertexTangent;
in float2 vertexTexcoord;

layout(std140) uniform universal
{
	float4		cameraPosition;				// The world-space position of the camera.
	float4		cameraRight;				// The world-space right direction of the camera.
	float4		cameraDown;					// The world-space down direction of the camera.
	float4		ambientColor;				// The constant ambient color in xyz. The w component is not used.
	float4		lightColor;					// The color of the light in xyz. The w component is not used.
	float4		lightPosition;				// The world-space position of the light source.
	float4		attenConst;					// The range of the light source in the x component. The reciprocal range in the y component. The z and w components are not used.
	float4		fogPlane;					// The world-space fog plane f.
	float4		fogColor;					// The color of the fog. The w component is not used.
	float4		fogParams;					// The fog density in x. The value of m from Equation (8.116) in y. The value dot(f, c) in z. The value sgn(dot(f, c)) in w.
	float4		shadowParams;				// The depth transform (p22, p23) is stored in the x and y components. The shadow offset is stored in the z component. The w component is not used.
	float4		inverseLightTransform[3];	// The first 3 rows of the world-to-light transform.
};

layout(binding = 0) uniform sampler2D diffuseTexture;

layout(location = 32) uniform float4 fparam[3];

out float4 fragmentColor;

void main()
{
	float3 diffuseColor = fparam[0].xyz;

	// Multiply texture color by ambient light color.

	fragmentColor.xyz = texture(diffuseTexture, vertexTexcoord).xyz * diffuseColor * ambientColor.xyz;
	fragmentColor.w = 0.0;
}
