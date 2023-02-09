#ifndef Particles_h
#define Particles_h


#include "World.h"


namespace Engine
{
	struct Particle
	{
		Particle		*prevParticle;
		Particle		*nextParticle;

		Point3D			position;
		Vector3D		velocity;
		ColorRGBA		color;
		float			radius;
		float			angle;
		float			life;
	};


	struct RotatingParticle : Particle
	{
		float			angularVelocity;
	};


	class ParticlePoolBase
	{
		friend class ParticleSystem;

		private:

			Particle		*particlePool;
			uint32			particleDataSize;

			int32			totalParticleCount;
			int32			activeParticleCount;

			Particle		*firstFreeParticle;
			Particle		*lastFreeParticle;

		public:

			ParticlePoolBase(int32 count, Particle *pool, uint32 size);

			Particle *NewParticle(void);
			void ResetPool(void);
	};


	template <class type = Particle>
	class ParticlePool : public ParticlePoolBase
	{
		public:

			ParticlePool(int32 count, type *pool) : ParticlePoolBase(count, pool, sizeof(type))
			{
			}

			type *NewParticle(void)
			{
				return (static_cast<type *>(ParticlePoolBase::NewParticle()));
			}
	};


	class ParticleSystem : public RenderableNode, public ListElement<ParticleSystem>
	{
		public:

			ParticlePoolBase	*particlePool;

			Particle			*firstUsedParticle;
			Particle			*lastUsedParticle;

		protected:

			float				emitTime;

			ParticleSystem(ParticlePoolBase *pool, const char *textureName);

		public:

			~ParticleSystem();

			void AddParticle(Particle *particle);
			void FreeParticle(Particle *particle);
			void FreeAllParticles(void);

			void RenderParticles(void);

			virtual void MoveParticles(float deltaTime) = 0;
	};


	class ExampleParticleSystem : public ParticleSystem
	{
		private:

			ParticlePool<RotatingParticle>		particlePool;
			RotatingParticle					particleArray[1024];

		public:

			ExampleParticleSystem();
			~ExampleParticleSystem();

			void MoveParticles(float deltaTime) override;
	};
}


#endif
