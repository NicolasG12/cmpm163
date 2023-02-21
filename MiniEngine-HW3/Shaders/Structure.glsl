out float4 fragmentColor;

void main()
{
	float w = 1.0 / gl_FragCoord.w;
	float z = asfloat(asuint(w) & 0xFFFFE000U);
	fragmentColor = float4(ddx(w), ddy(w), z, w - z);
}
