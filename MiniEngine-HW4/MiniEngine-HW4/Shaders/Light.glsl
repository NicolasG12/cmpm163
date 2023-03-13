in float3 vertexPosition;			// The interpolated world-space vertex position.
in float3 vertexNormal;				// The interpolated world-space vertex normal.
in float4 vertexTangent;			// The interpolated world-space vertex tangent in xyz. Handedness in w.
in float2 vertexTexcoord;			// The interpolated texture coordinates.

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

layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D normalTexture;

layout(location = 32) uniform float4 fparam[3];

out float4 fragmentColor;			// The final output color. Set the alpha component (w coordinate) to zero.

float3 ApplyHalfspaceFog(float3 shadedColor, float3 v)
{
	// #HW4 -- Add code here to calculate half-space fog for the lighting pass.
	float fp = dot(fogPlane.xyz, vertexPosition);
	float u1 = fogParams.y * (fogParams.z + fp);
	float u2 = fp * fogParams.w;
	float fv = dot(fogPlane.xyz, v);

	const float kFogEpsilon = 0.0001;

	float x = min(u2, 0.0);
	float tau = 0.5 * fogParams.x * length(v) * (u1 - x * x / (abs(fv) + kFogEpsilon));
	return (shadedColor * exp(tau));
}

void main()
{
	// These are the material properties provided by the C++ code plus texturing.

	float3 diffuseColor = fparam[0].xyz * texture(diffuseTexture, vertexTexcoord).xyz;
	float3 specularColor = fparam[1].xyz;
	float specularPower = fparam[1].w;

	float3	m;

	m.xy = texture(normalTexture, vertexTexcoord).xy;
	m.z = sqrt(1.0F - m.x * m.x - m.y * m.y);

	float3 normal = normalize(vertexNormal);

	float3 tangent = normalize(vertexTangent.xyz);
	float3 bitangent = cross(normal, tangent) * vertexTangent.w;

	m = tangent * m.x + bitangent * m.y + normal * m.z;

	// Calculate direction to light, get its squared length, and then normalize it.

	float3 ldir = lightPosition.xyz - vertexPosition;
	float r2 = dot(ldir, ldir);
	ldir *= rsqrt(r2);

	// The following line calculates a softening effect for the specular highlights
	// near the boundary between lit and unlit sides of a geometry. This is not part of
	// the homework assignment, and you do not need to modify this use of the vertex normal.

	float softening = smoothstep(0.0, 1.0, dot(normal, ldir));

	// Calculate direction to camera and halfway vector.
	// The full-length v is needed later for fog.

	float3 v = cameraPosition.xyz - vertexPosition;
	float3 vdir = normalize(v);
	float3 hdir = normalize(ldir + vdir);

	// Calculate light attenuation using squared distance to light.

	float atten = saturate(exp(r2 * attenConst.x) * attenConst.y - attenConst.z);

	// Calculate Lambertian diffuse factor sat(n • l) / pi.

	float3 diff = diffuseColor * clamp(dot(m, ldir), 0.0, 1.0) * 0.3183;

	// Calculate specular factor sat(n • h)^alpha.

	float3 spec = specularColor * (pow(saturate(dot(m, hdir)), specularPower) * softening);

	// Multiply combined diffse and specular color by attenuated light color.

	float3 shadedColor = (diff + spec) * lightColor.xyz * atten;

	fragmentColor.xyz = ApplyHalfspaceFog(shadedColor, v);
	fragmentColor.w = 0.0;


}
