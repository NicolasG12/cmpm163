//
// This file is part of the Terathon Math Library, by Eric Lengyel.
// Copyright 1999-2023, Terathon Software LLC
//
// This software is distributed under the MIT License.
// Separate proprietary licenses are available from Terathon Software.
//


#ifndef TSSphere3D_h
#define TSSphere3D_h


//# \component	Math Library
//# \prefix		Math/


#include "TSCircle3D.h"


#define TERATHON_SPHERE3D 1


namespace Terathon
{
	struct ConstSphere3D;


	class Sphere3D
	{
		public:

			float	u, x, y, z, w;

			TERATHON_API static const ConstSphere3D zero;

			inline Sphere3D() = default;

			Sphere3D(float su, float sx, float sy, float sz, float sw)
			{
				u = su;
				x = sx;
				y = sy;
				z = sz;
				w = sw;
			}
	};


	inline const Sphere3D& Reverse(const Sphere3D& s)
	{
		return (s);
	}

	inline Round3D Dual(const Sphere3D& s)
	{
		return (Round3D(-s.x, -s.y, -s.z, s.u, s.w));
	}

	inline Sphere3D Dual(const Round3D& a)
	{
		return (Sphere3D(-a.w, a.x, a.y, a.z, -a.u));
	}

	inline Vector4D Anticarrier(const Sphere3D& s)
	{
		return (Vector4D(-s.x, -s.y, -s.z, s.u));
	}

	inline Circle3D Attitude(const Sphere3D& s)
	{
		return (Circle3D(0.0F, 0.0F, 0.0F, s.u, 0.0F, 0.0F, 0.0F, s.x, s.y, s.z));
	}

	inline Sphere3D Container(const Round3D& a)
	{
		return (Sphere3D(-a.w * a.w, a.x * a.w, a.y * a.w, a.z * a.w, a.w * a.u - a.x * a.x - a.y * a.y - a.z * a.z));
	}

	inline Sphere3D Container(const Dipole3D& d)
	{
		return (Sphere3D(-d.v.x * d.v.x - d.v.y * d.v.y - d.v.z * d.v.z,
		                  d.v.y * d.m.z - d.v.z * d.m.y + d.v.x * d.p.w,
						  d.v.z * d.m.x - d.v.x * d.m.z + d.v.y * d.p.w,
						  d.v.x * d.m.y - d.v.y * d.m.x + d.v.z * d.p.w,
						 -d.m.x * d.m.x - d.m.y * d.m.y - d.m.z * d.m.z - d.v.x * d.p.x - d.v.y * d.p.y - d.v.z * d.p.z));
	}

	inline Sphere3D Container(const Circle3D& c)
	{
		return (Sphere3D(-c.g.x * c.g.x - c.g.y * c.g.y - c.g.z * c.g.z,
		                  c.g.y * c.v.z - c.g.z * c.v.y - c.g.w * c.g.x,
		                  c.g.z * c.v.x - c.g.x * c.v.z - c.g.w * c.g.y,
						  c.g.x * c.v.y - c.g.y * c.v.x - c.g.w * c.g.z,
						  c.g.x * c.m.x + c.g.y * c.m.y + c.g.z * c.m.z - c.g.w * c.g.w));
	}

	inline Sphere3D Container(const Sphere3D& s)
	{
		return (Sphere3D(-s.u * s.u, -s.x * s.u, -s.y * s.u, -s.z * s.u, -s.w * s.u));
	}

	inline Round3D Center(const Sphere3D& s)
	{
		return (Round3D(-s.x * s.u, -s.y * s.u, -s.z * s.u, -s.u * s.u, s.x * s.x + s.y * s.y + s.z * s.z - s.w * s.u));
	}

	inline Point3D FlatCenter(const Sphere3D& s)
	{
		float f = -1.0F / s.u;
		return (Point3D(s.x * f, s.y * f, s.z * f));
	}

	inline Sphere3D Partner(const Sphere3D& s)
	{
		float u2 = s.u * s.u;
		return (Sphere3D(-s.u * u2, -s.x * u2, -s.y * u2, -s.z * u2, (s.w * s.u - s.x * s.x - s.y * s.y - s.z * s.z) * s.u));
	}

	inline float SquaredRadiusNorm(const Sphere3D& s)
	{
		return (s.x * s.x + s.y * s.y + s.z * s.z - s.w * s.u * 2.0F);
	}

	inline float SquaredCenterNorm(const Sphere3D& s)
	{
		return (s.x * s.x + s.y * s.y + s.z * s.z);
	}

	inline float SquaredWeightNorm(const Sphere3D& s)
	{
		return (s.u * s.u);
	}

	inline const Sphere3D& operator ~(const Sphere3D& s)
	{
		return (Reverse(s));
	}

	inline Sphere3D operator -(const Sphere3D& s)
	{
		return (Sphere3D(-s.u, -s.x, -s.y, -s.z, -s.w));
	}

	inline Sphere3D operator *(const Sphere3D& s, float n)
	{
		return (Sphere3D(s.u * n, s.x * n, s.y * n, s.z * n, s.w * n));
	}

	inline Sphere3D operator *(float n, const Sphere3D& s)
	{
		return (Sphere3D(n * s.u, n * s.x, n * s.y, n * s.z, n * s.w));
	}

	inline Sphere3D operator /(const Sphere3D& s, float n)
	{
		n = 1.0F / n;
		return (Sphere3D(s.u * n, s.x * n, s.y * n, s.z * n, s.w * n));
	}

	inline bool operator ==(const Sphere3D& s, const Sphere3D& t)
	{
		return ((s.u == t.u) && (s.x == t.x) && (s.y == t.y) && (s.z == t.z) && (s.w == t.w));
	}

	inline bool operator !=(const Sphere3D& s, const Sphere3D& t)
	{
		return ((s.u != t.u) || (s.x != t.x) || (s.y != t.y) || (s.z != t.z) || (s.w != t.w));
	}

	inline Circle3D Antiwedge(const Sphere3D& s, const Sphere3D& t)
	{
		return (Circle3D(s.u * t.x - s.x * t.u, s.u * t.y - s.y * t.u, s.u * t.z - s.z * t.u, s.u * t.w - s.w * t.u, s.z * t.y - s.y * t.z, s.x * t.z - s.z * t.x, s.y * t.x - s.x * t.y, s.x * t.w - s.w * t.x, s.y * t.w - s.w * t.y, s.z * t.w - s.w * t.z));
	}


	struct ConstSphere3D
	{
		float	su, sx, sy, sz, sw;

		operator const Sphere3D&(void) const
		{
			return (reinterpret_cast<const Sphere3D&>(*this));
		}

		const Sphere3D *operator &(void) const
		{
			return (reinterpret_cast<const Sphere3D *>(this));
		}

		const Sphere3D *operator ->(void) const
		{
			return (reinterpret_cast<const Sphere3D *>(this));
		}
	};
}


#endif
