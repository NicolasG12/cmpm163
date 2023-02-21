//
// This file is part of the Terathon Math Library, by Eric Lengyel.
// Copyright 1999-2023, Terathon Software LLC
//
// This software is distributed under the MIT License.
// Separate proprietary licenses are available from Terathon Software.
//


#ifndef TSRound3D_h
#define TSRound3D_h


//# \component	Math Library
//# \prefix		Math/


#include "TSPlane3D.h"


#define TERATHON_ROUND3D 1


namespace Terathon
{
	struct ConstRound3D;


	class Round3D
	{
		public:

			float	x, y, z, w, u;

			TERATHON_API static const ConstRound3D zero;

			inline Round3D() = default;

			Round3D(float px, float py, float pz, float pw, float pu)
			{
				x = px;
				y = py;
				z = pz;
				w = pw;
				u = pu;
			}

			Round3D(const Point3D& p)
			{
				x = p.x;
				y = p.y;
				z = p.z;
				w = 1.0F;
				u = (p.x * p.x + p.y * p.y + p.z * p.z) * 0.5F;
			}
	};


	inline const Round3D& Reverse(const Round3D& a)
	{
		return (a);
	}

	inline Vector4D Carrier(const Round3D& a)
	{
		return (Vector4D(a.x, a.y, a.z, a.w));
	}

	inline Round3D Center(const Round3D& a)
	{
		return (Round3D(a.x * a.w, a.y * a.w, a.z * a.w, a.w * a.w, a.w * a.u));
	}

	inline Point3D FlatCenter(const Round3D& a)
	{
		float f = 1.0F / a.w;
		return (Point3D(a.x * f, a.y * f, a.z * f));
	}

	inline Round3D Partner(const Round3D& a)
	{
		float w2 = a.w * a.w;
		return (Round3D(a.x * w2, a.y * w2, a.z * w2, a.w * w2, (a.x * a.x + a.y * a.y + a.z * a.z - a.w * a.u) * a.w));
	}

	inline float SquaredRadiusNorm(const Round3D& a)
	{
		return (a.w * a.u * 2.0F - a.x * a.x - a.y * a.y - a.z * a.z);
	}

	inline float SquaredCenterNorm(const Round3D& a)
	{
		return (a.x * a.x + a.y * a.y + a.z * a.z);
	}

	inline float SquaredWeightNorm(const Round3D& a)
	{
		return (a.w * a.w);
	}

	inline const Round3D& operator ~(const Round3D& a)
	{
		return (Reverse(a));
	}

	inline Round3D operator -(const Round3D& a)
	{
		return (Round3D(-a.x, -a.y, -a.z, -a.w, -a.u));
	}

	inline Round3D operator *(const Round3D& a, float n)
	{
		return (Round3D(a.x * n, a.y * n, a.z * n, a.w * n, a.u * n));
	}

	inline Round3D operator *(float n, const Round3D& a)
	{
		return (Round3D(n * a.x, n * a.y, n * a.z, n * a.w, n * a.u));
	}

	inline Round3D operator /(const Round3D& a, float n)
	{
		n = 1.0F / n;
		return (Round3D(a.x * n, a.y * n, a.z * n, a.w * n, a.u * n));
	}

	inline bool operator ==(const Round3D& a, const Round3D& b)
	{
		return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w) && (a.u == b.u));
	}

	inline bool operator !=(const Round3D& a, const Round3D& b)
	{
		return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z) || (a.w != b.w) || (a.u != b.u));
	}


	struct ConstRound3D
	{
		float	px, py, pz, pw, pu;

		operator const Round3D&(void) const
		{
			return (reinterpret_cast<const Round3D&>(*this));
		}

		const Round3D *operator &(void) const
		{
			return (reinterpret_cast<const Round3D *>(this));
		}

		const Round3D *operator ->(void) const
		{
			return (reinterpret_cast<const Round3D *>(this));
		}
	};
}


#endif
