//
// This file is part of the Terathon Math Library, by Eric Lengyel.
// Copyright 1999-2023, Terathon Software LLC
//
// This software is distributed under the MIT License.
// Separate proprietary licenses are available from Terathon Software.
//


#ifndef TSCircle3D_h
#define TSCircle3D_h


//# \component	Math Library
//# \prefix		Math/


#include "TSDipole3D.h"


#define TERATHON_CIRCLE3D 1


namespace Terathon
{
	struct ConstCircle3D;


	class Circle3D
	{
		public:

			Plane3D			g;
			Vector3D		v;
			Bivector3D		m;

			TERATHON_API static const ConstCircle3D zero;

			inline Circle3D() = default;

			Circle3D(float gx, float gy, float gz, float gw, float vx, float vy, float vz, float mx, float my, float mz)
			{
				g.Set(gx, gy, gz, gw);
				v.Set(vx, vy, vz);
				m.Set(mx, my, mz);
			}

			Circle3D(const Plane3D& plane, const Vector3D& direction, const Bivector3D& moment)
			{
				g = plane;
				v = direction;
				m = moment;
			}

			Circle3D& Set(float gx, float gy, float gz, float gw, float vx, float vy, float vz, float mx, float my, float mz)
			{
				g.Set(gx, gy, gz, gw);
				v.Set(vx, vy, vz);
				m.Set(mx, my, mz);
				return (*this);
			}

			void Set(float gx, float gy, float gz, float gw, float vx, float vy, float vz, float mx, float my, float mz) volatile
			{
				g.Set(gx, gy, gz, gw);
				v.Set(vx, vy, vz);
				m.Set(mx, my, mz);
			}

			Circle3D& Set(const Plane3D& plane, const Vector3D& direction, const Bivector3D& moment)
			{
				g = plane;
				v = direction;
				m = moment;
				return (*this);
			}

			void Set(const Plane3D& plane, const Vector3D& direction, const Bivector3D& moment) volatile
			{
				g = plane;
				v = direction;
				m = moment;
			}
	};


	inline Circle3D Reverse(const Circle3D& c)
	{
		return (Circle3D(-c.g.x, -c.g.y, -c.g.z, -c.g.w, -c.v.x, -c.v.y, -c.v.z, -c.m.x, -c.m.y, -c.m.z));
	}

	inline Dipole3D Dual(const Circle3D& c)
	{
		return (Dipole3D(c.g.x, c.g.y, c.g.z, c.v.x, c.v.y, c.v.z, c.m.x, c.m.y, c.m.z, -c.g.w));
	}

	inline Circle3D Dual(const Dipole3D& d)
	{
		return (Circle3D(-d.v.x, -d.v.y, -d.v.z, d.p.w, -d.m.x, -d.m.y, -d.m.z, -d.p.x, -d.p.y, -d.p.z));
	}

	inline Plane3D Carrier(const Circle3D& c)
	{
		return (c.g);
	}

	inline Line3D Anticarrier(const Circle3D& c)
	{
		return (Line3D(c.v.x, c.v.y, c.v.z, c.g.x, c.g.y, c.g.z));
	}

	inline Dipole3D Attitude(const Circle3D& c)
	{
		return (Dipole3D(0.0F, 0.0F, 0.0F, c.g.x, c.g.y, c.g.z, c.v.x, c.v.y, c.v.z, 0.0F));
	}

	inline Round3D Center(const Circle3D& c)
	{
		return (Round3D(c.g.y * c.v.z - c.g.z * c.v.y - c.g.x * c.g.w,
		                c.g.z * c.v.x - c.g.x * c.v.z - c.g.y * c.g.w,
						c.g.x * c.v.y - c.g.y * c.v.x - c.g.z * c.g.w,
						c.g.x * c.g.x + c.g.y * c.g.y + c.g.z * c.g.z,
						c.v.x * c.v.x + c.v.y * c.v.y + c.v.z * c.v.z + c.g.x * c.m.x + c.g.y * c.m.y + c.g.z * c.m.z));
	}

	inline Point3D FlatCenter(const Circle3D& c)
	{
		float f = 1.0F / (c.g.x * c.g.x + c.g.y * c.g.y + c.g.z * c.g.z);
		return (Point3D((c.g.y * c.v.z - c.g.z * c.v.y - c.g.x * c.g.w) * f, (c.g.z * c.v.x - c.g.x * c.v.z - c.g.y * c.g.w) * f, (c.g.x * c.v.y - c.g.y * c.v.x - c.g.z * c.g.w) * f));
	}

	inline Circle3D Partner(const Circle3D& c)
	{
		float g2 = c.g.x * c.g.x + c.g.y * c.g.y + c.g.z * c.g.z;
		float f = c.g.w * c.g.w - c.v.x * c.v.x + c.v.y * c.v.y + c.v.z * c.v.z - c.g.x * c.m.x - c.g.y * c.m.y - c.g.z * c.m.z;

		return (Circle3D(c.g.x * g2, c.g.y * g2, c.g.z * g2, c.g.w * g2, c.v.x * g2, c.v.y * g2, c.v.z * g2,
		       (c.v.y * c.g.z - c.v.z * c.g.y) * c.g.w + c.g.x * f, (c.v.z * c.g.x - c.v.x * c.g.z) * c.g.w + c.g.y * f, (c.v.x * c.g.y - c.v.y * c.g.x) * c.g.w + c.g.z * f));
	}

	inline float SquaredRadiusNorm(const Circle3D& c)
	{
		return (c.v.x * c.v.x + c.v.y * c.v.y + c.v.z * c.v.z + (c.g.x * c.m.x + c.g.y * c.m.y + c.g.z * c.m.z) * 2.0F - c.g.w * c.g.w);
	}

	inline float SquaredCenterNorm(const Circle3D& c)
	{
		return (c.g.w * c.g.w + c.v.x * c.v.x + c.v.y * c.v.y + c.v.z * c.v.z);
	}

	inline float SquaredWeightNorm(const Circle3D& c)
	{
		return (c.g.x * c.g.x + c.g.y * c.g.y + c.g.z * c.g.z);
	}

	inline Circle3D operator ~(const Circle3D& c)
	{
		return (Reverse(c));
	}

	inline Circle3D operator -(const Circle3D& c)
	{
		return (Circle3D(-c.g.x, -c.g.y, -c.g.z, -c.g.w, -c.v.x, -c.v.y, -c.v.z, -c.m.x, -c.m.y, -c.m.z));
	}

	inline Circle3D operator *(const Circle3D& c, float n)
	{
		return (Circle3D(c.g.x * n, c.g.y * n, c.g.z * n, c.g.w * n, c.v.x * n, c.v.y * n, c.v.z * n, c.m.x * n, c.m.y * n, c.m.z * n));
	}

	inline Circle3D operator *(float n, const Circle3D& c)
	{
		return (Circle3D(n * c.g.x, n * c.g.y, n * c.g.z, n * c.g.w, n * c.v.x, n * c.v.y, n * c.v.z, n * c.m.x, n * c.m.y, n * c.m.z));
	}

	inline Circle3D operator /(const Circle3D& c, float n)
	{
		n = 1.0F / n;
		return (Circle3D(c.g.x * n, c.g.y * n, c.g.z * n, c.g.w * n, c.v.x * n, c.v.y * n, c.v.z * n, c.m.x * n, c.m.y * n, c.m.z * n));
	}

	inline bool operator ==(const Circle3D& c, const Circle3D& o)
	{
		return ((c.g == o.g) && (c.v == o.v) && (c.m == o.m));
	}

	inline bool operator !=(const Circle3D& c, const Circle3D& o)
	{
		return ((c.g != o.g) || (c.v != o.v) || (c.m != o.m));
	}


	struct ConstCircle3D
	{
		float	gx, gy, gz, gw;
		float	vx, vy, vz;
		float	mx, my, mz;

		operator const Circle3D&(void) const
		{
			return (reinterpret_cast<const Circle3D&>(*this));
		}

		const Circle3D *operator &(void) const
		{
			return (reinterpret_cast<const Circle3D *>(this));
		}

		const Circle3D *operator ->(void) const
		{
			return (reinterpret_cast<const Circle3D *>(this));
		}
	};
}


#endif
