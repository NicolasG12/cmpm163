void WorldManager::ConstructNormalMap(const Color4U* heightMap, Color2S* normalMap, int32 width, int32 height, float scale)
{
	// #HW3 -- Add code to this function that constructs a signed two-channel normal map from the height map passed in through
	// the heightMap parameter. Store the outputs in the space pointed to by the normalMap parameter, which is large enough to
	// hold width x height texels. You can assume that width and height are each powers of two and that the texture is supposed
	// to repeat, which means you need to wrap around when calculating central differences at the edges.
	//
	// The red, green, and blue channels of the four-channel height map all contain the same height values, so you can just pick one.
	// Use Listing 7.5 as a starting place, but modify it to account for the fact that the input here is an 8-bit unsigned integer
	// in the range 0 to 255 instead of a floating-point value in the range 0.0 - 1.0. The outputs for each texel are the
	// x and y components of the normal vector stored as signed 8-bit integers in the range -127 to +127. This corresponds
	// to the floating-point range -1.0 to 1.0, so multiply by 127.0F before converting to an integer.

	for (int32 y = 0; y < height; y++) {
		int32 yp1 = (y + 1) & (height - 1), ym1 = (y - 1) & (height - 1);

		const Color4U* centerRow = heightMap + y * width;
		const Color4U* upperRow = heightMap + yp1 * width;
		const Color4U* lowerRow = heightMap + ym1 * width;

		for (int x = 0; x < width; x++) {
			int32 xp1 = (x + 1) & (width - 1), xm1 = (x - 1) & (width - 1);


			float dx = (((centerRow[xp1].red) - (centerRow[xm1].red)) * 0.5F * scale) / 255;
			float dy = (((lowerRow[x].red) - (upperRow[x].red)) * 0.5F * scale) / 255;

			Vector3D ux = Vector3D(1, 0, dx);
			Vector3D uy = Vector3D(0, 1, dy);

			Vector3D m = Cross(ux, uy);
			m /= Magnitude(m);

			normalMap[x].Set(m.x * 127.0F, m.y * 127.0F);

		}
		normalMap += width;
	}
}