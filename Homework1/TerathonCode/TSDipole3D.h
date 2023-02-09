//
// This file is part of the Terathon Math Library, by Eric Lengyel.
// Copyright 1999-2023, Terathon Software LLC
//
// This software is distributed under the MIT License.
// Separate proprietary licenses are available from Terathon Software.
//


#ifndef TSDipole3D_h
#define TSDipole3D_h


//# \component	Math Library
//# \prefix		Math/


#include "TSRound3D.h"


#define TERATHON_DIPOLE3D 1


namespace Terathon
{
	struct ConstDipole3D;


	class Dipole3D
	{
		public:

			Vector3D		v;
			Bivector3D		m;
			Vector4D		p;

			TERATHON_API static const ConstDipole3D zero;

			inline Dipole3D() = default;

			Dipole3D(float vx, float vy, float vz, float mx, float my, float mz, float px, float py, float pz, float pw)
			{
				v.Set(vx, vy, vz);
				m.Set(mx, my, mz);
				p.Set(px, py, pz, pw);
			}

			Dipole3D(const Vector3D& direction, const Bivector3D& moment, const Vector4D& point)
			{
				v = direction;
				m = moment;
				p = point;
			}

			Dipole3D& Set(float vx, float vy, float vz, float mx, float my, float mz, float px, float py, float pz, float pw)
			{
				v.Set(vx, vy, vz);
				m.Set(mx, my, mz);
				p.Set(px, py, pz, pw);
				return (*this);
			}

			void Set(float vx, float vy, float vz, float mx, float my, float mz, float px, float py, float pz, float pw) volatile
			{
				v.Set(vx, vy, vz);
				m.Set(mx, my, mz);
				p.Set(px, py, pz, pw);
			}

			Dipole3D& Set(const Vector3D& direction, const Bivector3D& moment, const Vector4D& point)
			{
				v = direction;
				m = moment;
				p = point;
				return (*this);
			}

			void Set(const Vector3D& direction, const Bivector3D& moment, const Vector4D& point) volatile
			{
				v = direction;
				m = moment;
				p = point;
			}
	};


	inline Dipole3D Reverse(const Dipole3D& d)
	{
		return (Dipole3D(-d.v.x, -d.v.y, -d.v.z, -d.m.x, -d.m.y, -d.m.z, -d.p.x, -d.p.y, -d.p.z, -d.p.w));
	}

	inline Line3D Carrier(const Dipole3D& d)
	{
		return (Line3D(d.v, d.m));
	}

	inline Plane3D Anticarrier(const Dipole3D& d)
	{
		return (Plane3D(-d.v.x, -d.v.y, -d.v.z, d.p.w));
	}

	inline Round3D Attitude(const Dipole3D& d)
	{
		return (Round3D(d.v.x, d.v.y, d.v.z, 0.0F, d.p.w));
	}

	inline Round3D Center(const Dipole3D& d)
	{
		return (Round3D(d.v.y * d.m.z - d.v.z * d.m.y + d.p.x * d.p.w,
		                d.v.z * d.m.x - d.v.x * d.m.z + d.p.y * d.p.w,
						d.v.x * d.m.y - d.v.y * d.m.x + d.p.z * d.p.w,
						d.v.x * d.v.x + d.v.y * d.v.y + d.v.z * d.v.z,
						d.p.w * d.p.w - d.v.x * d.p.x - d.v.y * d.p.y - d.v.z * d.p.z));
	}

	inline Point3D FlatCenter(const Dipole3D& d)
	{
		float f = 1.0F / (d.v.x * d.v.x + d.v.y * d.v.y + d.v.z * d.v.z);
		return (Point3D((d.v.y * d.m.z - d.v.z * d.m.y + d.p.x * d.p.w) * f, (d.v.z * d.m.x - d.v.x * d.m.z + d.p.y * d.p.w) * f, (d.v.x * d.m.y - d.v.y * d.m.x + d.p.z * d.p.w) * f));
	}

	inline Dipole3D Partner(const Dipole3D& d)
	{
		float v2 = -d.v.x * d.v.x - d.v.y * d.v.y - d.v.z * d.v.z;
		float f = d.m.x * d.m.x + d.m.y * d.m.y + d.m.z * d.m.z - d.p.w * d.p.w + d.v.x * d.p.x + d.v.y * d.p.y + d.v.z * d.p.z;

		return (Dipole3D(d.v.x * v2, d.v.y * v2, d.v.z * v2, d.m.x * v2, d.m.y * v2, d.m.z * v2,
		       (d.m.y * d.v.z - d.m.z * d.v.y) * d.p.w + d.v.x * f, (d.m.z * d.v.x - d.m.x * d.v.z) * d.p.w + d.v.y * f, (d.m.x * d.v.y - d.m.y * d.v.x) * d.p.w + d.v.z * f, d.p.w * v2));
	}

	inline float SquaredRadiusNorm(const Dipole3D& d)
	{
		return (d.p.w * d.p.w - d.m.x * d.m.x - d.m.y * d.m.y - d.m.z * d.m.z - (d.p.x * d.v.x + d.p.y * d.v.y + d.p.z * d.v.z) * 2.0F);
	}

	inline float SquaredCenterNorm(const Dipole3D& d)
	{
		return (d.m.x * d.m.x + d.m.y * d.m.y + d.m.z * d.m.z + d.p.w * d.p.w);
	}

	inline float SquaredWeightNorm(const Dipole3D& d)
	{
		return (d.v.x * d.v.x + d.v.y * d.v.y + d.v.z * d.v.z);
	}

	inline Dipole3D operator ~(const Dipole3D& d)
	{
		return (Reverse(d));
	}

	inline Dipole3D operator -(const Dipole3D& d)
	{
		return (Dipole3D(-d.v.x, -d.v.y, -d.v.z, -d.m.x, -d.m.y, -d.m.z, -d.p.x, -d.p.y, -d.p.z, -d.p.w));
	}

	inline Dipole3D operator *(const Dipole3D& d, float n)
	{
		return (Dipole3D(d.v.x * n, d.v.y * n, d.v.z * n, d.m.x * n, d.m.y * n, d.m.z * n, d.p.x * n, d.p.y * n, d.p.z * n, d.p.w * n));
	}

	inline Dipole3D operator *(float n, const Dipole3D& d)
	{
		return (Dipole3D(n * d.v.x, n * d.v.y, n * d.v.z, n * d.m.x, n * d.m.y, n * d.m.z, n * d.p.x, n * d.p.y, n * d.p.z, n * d.p.w));
	}

	inline Dipole3D operator /(const Dipole3D& d, float n)
	{
		n = 1.0F / n;
		return (Dipole3D(d.v.x * n, d.v.y * n, d.v.z * n, d.m.x * n, d.m.y * n, d.m.z * n, d.p.x * n, d.p.y * n, d.p.z * n, d.p.w * n));
	}

	inline bool operator ==(const Dipole3D& d, const Dipole3D& f)
	{
		return ((d.v == f.v) && (d.m == f.m) && (d.p == f.p));
	}

	inline bool operator !=(const Dipole3D& d, const Dipole3D& f)
	{
		return ((d.v != f.v) || (d.m != f.m) || (d.p != f.p));
	}


	struct ConstDipole3D
	{
		float	vx, vy, vz;
		float	mx, my, mz;
		float	px, py, pz, pw;

		operator const Dipole3D&(void) const
		{
			return (reinterpret_cast<const Dipole3D&>(*this));
		}

		const Dipole3D *operator &(void) const
		{
			return (reinterpret_cast<const Dipole3D *>(this));
		}

		const Dipole3D *operator ->(void) const
		{
			return (reinterpret_cast<const Dipole3D *>(this));
		}
	};
}


#endif
