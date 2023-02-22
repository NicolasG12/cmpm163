bool FrustumCamera::SphereVisible(const Point3D& center, float radius) const
{
	// #HW2 -- Add code here that determines whether a sphere is visible to the camera.
	// The center parameter is the world-space center of the sphere.

	for (auto g : worldFrustumPlane) {
		if (Dot(g, center) <= -radius) {
			return (false);
		}
	}

	return (true);
}

bool FrustumCamera::BoxVisible(const Transform4D& transform, const Vector3D& size) const
{
	// #HW2 -- Add code here that determines whether a box is visible to the camera.
	// The transform parameter is the object-to-world transformation matrix for the box.
	Vector3D h = size * 0.5;
	Point3D p = transform.GetTranslation() + (transform[0] * h.x) + (transform[1] * h.y) + (transform[2] * h.z);
	for (auto g : worldFrustumPlane) {
		float rg = Fabs(Dot(g * h.x, transform[0])) + Fabs(Dot(g * h.y, transform[1])) + Fabs(Dot(g * h.z, transform[2]));
		if (Dot(g, p) <= -rg) return (false);
	}

	return (true);
}

bool PointLight::SphereIlluminated(const Point3D& center, float radius) const
{
	// #HW2 -- Add code here that determines whether a sphere is illuminated.
	// The center parameter is the world-space center of the sphere.
	Vector3D d = center - GetWorldPosition();
	float rSum = radius + lightRange;
	if (Dot(d, d) >= (rSum * rSum)) return (false);


	return (true);
}

bool PointLight::BoxIlluminated(const Transform4D& transform, const Vector3D& size) const
{
	// #HW2 -- Add code here that determines whether a box is illuminated.
	// The transform parameter is the object-to-world transformation matrix for the box.

	// Remember that there is an error in Listing 9.17 in the book, and that you need
	// to divide v•s, v•t, and v•u by the magnitude of v when implementing Equation (9.15).
	Vector3D h = size * 0.5;
	Point3D p = transform.GetTranslation() + (transform[0] * h.x) + (transform[1] * h.y) + (transform[2] * h.z);
	Vector3D v = p - GetWorldPosition();
	float vs = Fabs(Dot(v, transform[0]));
	float vt = Fabs(Dot(v, transform[1]));
	float vu = Fabs(Dot(v, transform[2]));
	float rs = (h.x * vs / Magnitude(v)) + (h.y * vt / Magnitude(v)) + (h.z * vu / Magnitude(v)) + lightRange;
	if (Dot(v, v) >= rs * rs) return (false);

	return (Fmax(vs - h.x, vt - h.y, vu - h.z) < lightRange);
}