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
	float4		attenConst;					// The range of the light source in the x component. The reciprocal range in the y component. The z and w components are not used.
	float4		fogPlane;					// The world-space fog plane f.
	float4		fogColor;					// The color of the fog. The w component is not used.
	float4		fogParams;					// The fog density in x. The value of m from Equation (8.116) in y. The value dot(f, c) in z. The value sgn(dot(f, c)) in w.
	float4		shadowParams;				// The depth transform (p22, p23) is stored in the x and y components. The shadow offset is stored in the z component. The w component is not used.
	float4		inverseLightTransform[3];	// The first 3 rows of the world-to-light transform.
};

layout(binding = 0) uniform sampler2D diffuseTexture;

layout(location = 32) uniform float4 fparam[3];

out float4 fragmentColor;			// The final output color. Set the alpha component (w coordinate) to zero.

void main()
{
	fragmentColor.w = 0.0;

	// These are the material properties provided by the C++ code plus texturing.

	float3 diffuseColor = fparam[0].xyz * texture(diffuseTexture, vertexTexcoord).xyz;
	float3 specularColor = fparam[1].xyz;
	float specularPower = fparam[1].w;

	// HW #1 -- Add code below that calculates the following items:
	//
	// 1. The unit-length interpolated vertex normal (the vector n).
	float3 n = normalize(vertexNormal);
	// 2. The unit-length direction to the light source (the vector l).
	float3 ldir = lightPosition.xyz - vertexPosition;
	float3 l = normalize(ldir);
	// 3. The unit-length direction to the camera/viewer (the vector v).
	float3 v = normalize(cameraPosition.xyz - vertexPosition);
	// 4. The unit-length halfway direction (the vector h).
	float3 h = normalize(l + v);
	// 5. The squared distance between the interopolated vertex position and the light source.
	float l2 = dot(ldir, ldir);
	// 6. The light attenuation function given by Equation (8.7) for k = 2. The attenConst vector already contains the values shown in Listing 8.2.

	float a = saturate(exp(l2 * attenConst.x) * attenConst.y - attenConst.z);
	// 7. The combined diffuse and specular shading given by Equation (7.26). Do not add ambient light. Assume the albedo rho is 1.0.
	//    You may multiply by 0.3183, which is approximately 1/pi, to account for division by pi in the diffuse component.
	float3 diffuse = 0.3813 * diffuseColor * saturate(dot(n, l));
	float3 specular = specularColor * pow(saturate(dot(n, h)), specularPower);
	float3 ds = (diffuse + specular) * lightColor.xyz;
	// Output the product of the final diffuse/specular color and the attenuated light color to fragmentColor.xyz, replacing the line below.


	fragmentColor.xyz = ds * a;
}
