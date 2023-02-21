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

void main()
{
	// #HW3 -- Add code to this shader to accomplish the following tasks.
	//
	// 1. Use the texture() function to read a sample from the normal texture with the same texture coordinates that were used for the diffuse texture.
	// 2. The normal texture contains only the x and y coordinates of each normal. Reconstitute the z coordinate using Equation (7.44).
	// 3. Calculate the normalized vertex tangent (using the xyz coordinates of vertexTangent).
	// 4. Calculate the bitangent vector using Equation (7.40). The value of sigma is stored in vertexTangent.w.
	// 5. Transform the normal sampled from the normal map into world space using the normalized vertex tangent vector, the calculated bitangent vector, and normalized vertex normal vector.
	// 6. Replace the vertex normal with the transformed sampled normal in the calculations for diffuse and specular shading.

	// These are the material properties provided by the C++ code plus texturing.

	float3 diffuseColor = fparam[0].xyz * texture(diffuseTexture, vertexTexcoord).xyz;
	float3 specularColor = fparam[1].xyz;
	float specularPower = fparam[1].w;

	float3 normal = normalize(vertexNormal);

	// Calculate direction to light, get its squared length, and then normalize it.

	float3 ldir = lightPosition.xyz - vertexPosition;
	float r2 = dot(ldir, ldir);
	ldir *= rsqrt(r2);

	// The following line calculates a softening effect for the specular highlights
	// near the boundary between lit and unlit sides of a geometry. This is not part of
	// the homework assignment, and you do not need to modify this use of the vertex normal.

	float softening = smoothstep(0.0, 1.0, dot(normal, ldir));

	// Calculate direction to camera and halfway vector.

	float3 vdir = normalize(cameraPosition.xyz - vertexPosition);
	float3 hdir = normalize(ldir + vdir);

	// Calculate light attenuation using squared distance to light.

	float atten = saturate(exp(r2 * attenConst.x) * attenConst.y - attenConst.z);

	// Calculate Lambertian diffuse factor sat(n • l) / pi.

	float3 diff = diffuseColor * clamp(dot(normal, ldir), 0.0, 1.0) * 0.3183;

	// Calculate specular factor sat(n • h)^alpha.

	float3 spec = specularColor * (pow(saturate(dot(normal, hdir)), specularPower) * softening);

	// Multiply combined diffse and specular color by attenuated light color.

	fragmentColor.xyz = (diff + spec) * lightColor.xyz * atten;
	fragmentColor.w = 0.0;
}
