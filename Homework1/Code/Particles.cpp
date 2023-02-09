#include "Particles.h"


using namespace Engine;


ParticlePoolBase::ParticlePoolBase(int32 count, Particle *pool, uint32 size)
{
	particlePool = pool;
	particleDataSize = size;

	totalParticleCount = count;
	activeParticleCount = 0;

	firstFreeParticle = nullptr;
	lastFreeParticle = nullptr;
}

Particle *ParticlePoolBase::NewParticle(void)
{
	Particle *particle = firstFreeParticle;
	if (particle)
	{
		Particle *next = particle->nextParticle;
		if (next)
		{
			firstFreeParticle = next;
			next->prevParticle = nullptr;
		}
		else
		{
			firstFreeParticle = nullptr;
			lastFreeParticle = nullptr;
		}

		return (particle);
	}

	int32 count = activeParticleCount;
	if (count < totalParticleCount)
	{
		particle = reinterpret_cast<Particle *>(reinterpret_cast<char *>(particlePool) + count * particleDataSize);
		activeParticleCount = count + 1;
		return (particle);
	}

	return (nullptr);
}

void ParticlePoolBase::ResetPool(void)
{
	activeParticleCount = 0;
	firstFreeParticle = nullptr;
	lastFreeParticle = nullptr;
}


ParticleSystem::ParticleSystem(ParticlePoolBase *pool, const char *textureName) : RenderableNode(kNodeParticleSystem)
{
	Color4U			*textureImage;
	Color4U			*mipmapImages;
	Integer2D		imageSize;

	particlePool = pool;
	firstUsedParticle = nullptr;
	lastUsedParticle = nullptr;

	emitTime = 0.0F;

	indexBuffer = graphicsManager->GetQuadIndexBuffer();

	SetBlendMode(Renderable::kBlendAdd);
	SetDepthWriteFlag(false);
	SetCullFaceFlag(false);

	ImportTargaImageFile(textureName, &textureImage, &imageSize);
	int32 mipmapCount = GenerateMipmapImages(Integer3D(imageSize, 1), textureImage, &mipmapImages);

	Texture *texture = new Texture(Texture::kType2D, Texture::kFormatGammaRgba, imageSize.x, imageSize.y, 1, mipmapCount, mipmapImages);
	SetTexture(0, texture);
	SetTextureCount(1);

	ReleaseMipmapImages(mipmapImages);
	ReleaseTargaImageData(textureImage);
}

ParticleSystem::~ParticleSystem()
{
	indexBuffer = nullptr;		// Don't let Renderable base class release index buffer.
}

void ParticleSystem::AddParticle(Particle *particle)
{
	Particle *last = lastUsedParticle;
	if (last)
	{
		last->nextParticle = particle;
		lastUsedParticle = particle;
		particle->prevParticle = last;
		particle->nextParticle = nullptr;
	}
	else
	{
		firstUsedParticle = particle;
		lastUsedParticle = particle;
		particle->prevParticle = nullptr;
		particle->nextParticle = nullptr;
	}
}

void ParticleSystem::FreeParticle(Particle *particle)
{
	Particle *next = particle->nextParticle;
	Particle *prev = particle->prevParticle;

	if (firstUsedParticle == particle)
	{
		firstUsedParticle = next;
	}

	if (lastUsedParticle == particle)
	{
		lastUsedParticle = prev;
	}

	if (next)
	{
		next->prevParticle = prev;
	}

	if (prev)
	{
		prev->nextParticle = next;
	}

	Particle *last = particlePool->lastFreeParticle;
	if (last)
	{
		last->nextParticle = particle;
		particlePool->lastFreeParticle = particle;
		particle->prevParticle = last;
		particle->nextParticle = nullptr;
	}
	else
	{
		particlePool->firstFreeParticle = particle;
		particlePool->lastFreeParticle = particle;
		particle->prevParticle = nullptr;
		particle->nextParticle = nullptr;
	}
}

void ParticleSystem::FreeAllParticles(void)
{
	particlePool->ResetPool();

	firstUsedParticle = nullptr;
	lastUsedParticle = nullptr;
}

void ParticleSystem::RenderParticles(void)
{
	if (!vertexBuffer[0])
	{
		vertexBuffer[0] = new Buffer(particlePool->totalParticleCount * sizeof(Vertex));
		EstablishStandardVertexArray();
	}

	volatile Vertex *vertex = static_cast<volatile Vertex *>(vertexBuffer[0]->MapBuffer());

	int32 count = 0;
	const Particle *particle = firstUsedParticle;
	while (particle)
	{
		float r = particle->radius;
		float t = particle->angle;

		vertex[0].position = particle->position;
		vertex[0].radius.Set(-r, -r, t);
		vertex[0].color = particle->color;
		vertex[0].texcoord.Set(0.0F, 0.0F);

		vertex[1].position = particle->position;
		vertex[1].radius.Set(r, -r, t);
		vertex[1].color = particle->color;
		vertex[1].texcoord.Set(1.0F, 0.0F);

		vertex[2].position = particle->position;
		vertex[2].radius.Set(r, r, t);
		vertex[2].color = particle->color;
		vertex[2].texcoord.Set(1.0F, 1.0F);

		vertex[3].position = particle->position;
		vertex[3].radius.Set(-r, r, t);
		vertex[3].color = particle->color;
		vertex[3].texcoord.Set(0.0F, 1.0F);

		vertex += 4;
		count++;

		particle = particle->nextParticle;
	}

	SetVertexCount(count * 4);
	SetIndexCount(0, count * 6);
	vertexBuffer[0]->UnmapBuffer();
}


ExampleParticleSystem::ExampleParticleSystem() : ParticleSystem(&particlePool, "Particles/Star.tga"), particlePool(1024, particleArray)
{
	//SetBlendMode(Renderable::kBlendAlpha);

	File vertexShader("Shaders/Billboard.glsl");
	File fragmentShader("Shaders/Particle.glsl");

	const char *vertexString = vertexShader.GetData();
	const char *fragmentString = fragmentShader.GetData();
	Program *program = new Program(vertexString, fragmentString);
	SetProgram(0, program);
}

ExampleParticleSystem::~ExampleParticleSystem()
{
}

void ExampleParticleSystem::MoveParticles(float deltaTime)
{
	// Figure out how many new particles to emit.

	constexpr float emitRate = 50.0F;	// Particles per second.

	float time = emitTime + deltaTime;
	int32 emitCount = int32(time * emitRate);
	emitTime = time - float(emitCount) / emitRate;

	for (int32 k = 0; k < emitCount; k++)
	{
		RotatingParticle *particle = particlePool.NewParticle();
		if (!particle) break;

		// A new particle is available. Initialize all of its fields, and add it to the system.

		particle->position.Set(0.0F, 0.0F, 0.0F);
		particle->velocity = RandomUnitVector3D() * (RandomFloat(1.0F) + 1.0F);
		particle->velocity.z = Fabs(particle->velocity.z);
		particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);
		particle->radius = RandomFloat(0.2F) + 0.05F;
		particle->life = RandomFloat(2.0F) + 1.0F;
		particle->angle = RandomFloat(Math::two_pi);

		AddParticle(particle);
	}

	// Now animate all of the particles in the system.

	RotatingParticle *particle = static_cast<RotatingParticle *>(firstUsedParticle);
	while (particle)
	{
		RotatingParticle *next = static_cast<RotatingParticle *>(particle->nextParticle);

		float life = particle->life - deltaTime;
		particle->life = life;

		if (life > 0.0F)
		{
			particle->position += particle->velocity * deltaTime;
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}
