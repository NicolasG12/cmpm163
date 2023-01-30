#ifndef Graphics_h
#define Graphics_h


#include "Base.h"


using namespace Terathon;


// Define the OpenGL enums and types we need that aren't in gl.h.

#define GL_FUNC_ADD								0x8006
#define GL_MIN									0x8007
#define GL_MAX									0x8008
#define GL_TEXTURE_3D							0x806F
#define GL_TEXTURE_CUBE_MAP						0x8513
#define GL_TEXTURE_2D_ARRAY						0x8C1A
#define GL_TEXTURE_RECTANGLE					0x84F5
#define GL_TEXTURE_2D_MULTISAMPLE				0x9100
#define GL_COMPARE_REF_DEPTH_TO_TEXTURE			0x884E
#define GL_TEXTURE_MAX_LEVEL					0x813D
#define GL_TEXTURE_MAX_ANISOTROPY				0x84FE
#define GL_CLAMP_TO_EDGE						0x812F
#define GL_SRGB8_ALPHA8							0x8C43
#define GL_RGBA8_SNORM							0x8F97
#define GL_RG									0x8227
#define GL_RG8									0x822B
#define GL_R8									0x8229
#define GL_RG8_SNORM							0x8F95
#define GL_R8_SNORM								0x8F94
#define GL_RGBA_INTEGER							0x8D99
#define GL_HALF_FLOAT							0x140B
#define GL_RGBA16UI								0x8D76
#define GL_RGBA16F								0x881A
#define GL_RGBA32F								0x8814
#define GL_DEPTH_COMPONENT32F					0x8CAC
#define GL_FRAMEBUFFER							0x8D40
#define GL_COLOR_ATTACHMENT0					0x8CE0
#define GL_COLOR_ATTACHMENT1					0x8CE1
#define GL_DEPTH_ATTACHMENT						0x8D00
#define GL_FRAMEBUFFER_SRGB						0x8DB9
#define GL_VERTEX_SHADER						0x8B31
#define GL_FRAGMENT_SHADER						0x8B30
#define GL_COMPILE_STATUS						0x8B81
#define GL_LINK_STATUS							0x8B82
#define GL_INFO_LOG_LENGTH						0x8B84
#define GL_ARRAY_BUFFER							0x8892
#define GL_ELEMENT_ARRAY_BUFFER					0x8893
#define GL_UNIFORM_BUFFER						0x8A11
#define GL_STATIC_DRAW							0x88E4
#define GL_WRITE_ONLY							0x88B9
#define GL_LOWER_LEFT							0x8CA1
#define GL_UPPER_LEFT							0x8CA2
#define GL_NEGATIVE_ONE_TO_ONE					0x935E
#define GL_ZERO_TO_ONE							0x935F
#define GL_QUERY_RESULT							0x8866
#define GL_TIME_ELAPSED							0x88BF
#define GL_TIMESTAMP							0x8E28
#define GL_TEXTURE_COMPARE_MODE					0x884C
#define GL_TEXTURE_COMPARE_FUNC					0x884D
#define GL_COMPARE_REF_TO_TEXTURE				0x884E

#define WGL_CONTEXT_MAJOR_VERSION_ARB			0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB			0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB			0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB		0x00000001
#define WGL_DRAW_TO_WINDOW_ARB					0x2001
#define WGL_ACCELERATION_ARB					0x2003
#define WGL_SUPPORT_OPENGL_ARB					0x2010
#define WGL_DOUBLE_BUFFER_ARB					0x2011
#define WGL_PIXEL_TYPE_ARB						0x2013
#define WGL_COLOR_BITS_ARB						0x2014
#define WGL_ALPHA_BITS_ARB						0x201B
#define WGL_FULL_ACCELERATION_ARB				0x2027
#define WGL_TYPE_RGBA_ARB						0x202B
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB		0x20A9

typedef char			GLchar;
typedef ptrdiff_t		GLintptr;
typedef ptrdiff_t		GLsizeiptr;
typedef uint64			GLuint64;

// Declare the GL functions we need that aren't in opengl32.lib.
// The actual storage is defined at the top of Graphics.cpp.

#define GLEXTFUNC(type, name, params) extern type (*name)params;
#include "OpenGL.h"


namespace Engine
{
	struct Vertex
	{
		Point3D			position;

		union
		{
			Normal		normal;
			Vector3D	radius;
		};

		union
		{
			Vector4D	tangent;
			ColorRGBA	color;
		};

		Point2D			texcoord;
	};


	struct Triangle
	{
		uint16			index[3];

		void Set(uint32 i0, uint32 i1, uint32 i2)
		{
			index[0] = uint16(i0);
			index[1] = uint16(i1);
			index[2] = uint16(i2);
		}
	};


	struct UniversalParams
	{
		Vector4D		cameraPosition;
		Vector4D		cameraRight;
		Vector4D		cameraDown;
		ColorRGBA		ambientColor;
		ColorRGBA		lightColor;
		Vector4D		lightPosition;
		Vector4D		attenConst;
		Vector4D		fogPlane;
		ColorRGBA		fogColor;
		Vector4D		fogParams;
		Vector4D		shadowParams;
		Vector4D		inverseLightTransform[3];
	};


	class Transformable
	{
		private:

			Transform4D		worldTransform;
			Transform4D		inverseWorldTransform;

		public:

			inline Transformable() = default;

			void SetIdentityTransform(void)
			{
				worldTransform.SetIdentity();
				inverseWorldTransform.SetIdentity();
			}

			const Transform4D& GetWorldTransform(void) const
			{
				return (worldTransform);
			}

			const Point3D& GetWorldPosition(void) const
			{
				return (worldTransform.GetTranslation());
			}

			const Transform4D& GetInverseWorldTransform(void) const
			{
				return (inverseWorldTransform);
			}

			void SetWorldTransform(const Transform4D& transform)
			{
				worldTransform = transform;
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldTransform(const Matrix3D& m, const Point3D& p)
			{
				worldTransform.Set(m, p);
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldTransform(const Vector3D& c1, const Vector3D& c2, const Vector3D& c3, const Point3D& c4)
			{
				worldTransform.Set(c1, c2, c3, c4);
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldTransform(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23)
			{
				worldTransform.Set(n00, n01, n02, n03, n10, n11, n12, n13, n20, n21, n22, n23);
				inverseWorldTransform = Inverse(worldTransform);
			}

			void SetWorldPosition(const Point3D& position)
			{
				worldTransform.SetTranslation(position);
				inverseWorldTransform = Inverse(worldTransform);
			}
	};


	class Buffer
	{
		friend class VertexArray;
		friend class GraphicsManager;

		private:

			GLuint			bufferObject;

		public:

			Buffer(uint32 size, const void *data = nullptr);
			~Buffer();

			volatile void *MapBuffer(void);
			void UnmapBuffer(void);

			void BindVertexBuffer(void);
			void BindIndexBuffer(void);
			void BindUniformBuffer(void);
	};


	class VertexArray
	{
		private:

			GLuint			vertexArrayObject;

		public:

			enum
			{
				kFormatFloat32,
				kFormatUint8,
				kFormatCount
			};

			VertexArray();
			~VertexArray();

			void SetAttribBuffer(int32 binding, int32 stride, const Buffer *buffer);
			void SetAttribArray(int32 index, int32 count, int32 format, int32 offset, int32 binding = 0);

			void BindVertexArray();
	};


	class Program : public Shared
	{
		friend class Renderable;

		private:

			GLuint			vertexShaderObject;
			GLuint			fragmentShaderObject;
			GLuint			programObject;

			static const char prologString[];

			struct StringPair
			{
				const char	*string[2];

				StringPair(const char *s1, const char *s2)
				{
					string[0] = s1;
					string[1] = s2;
				}

				operator const char **(void)
				{
					return (string);
				}
			};

			~Program();

		public:

			Program(int32 vertexStringCount, const char **vertexStringArray, int32 fragmentStringCount, const char **fragmentStringArray);
			Program(const char *vertexString, const char *fragmentString);

			void BindProgram(void);
	};


	class Texture : public Shared
	{
		friend class Framebuffer;
		friend class GraphicsManager;

		private:

			GLuint			textureObject;

			~Texture();

		public:

			enum
			{
				kType2D,
				kType3D,
				kTypeCube,
				kType2DArray,
				kTypeRectangle,
				kTypeMultisample,
				kTypeCount
			};

			enum
			{
				kFormatGammaRgba,
				kFormatLinearRgba,
				kFormatSignedRgba,
				kFormatLinearRedGreen,
				kFormatSignedRedGreen,
				kFormatLinearRed,
				kFormatSignedRed,
				kFormatFloat16Rgba,
				kFormatUint16Rgba,
				kFormatDepth,
				kFormatCount
			};

			Texture(int32 type, int32 format, int32 width, int32 height, int32 depth, int32 mipmapCount, const void *image);

			void BindTexture(int32 unit);
	};


	class Font : public Shared
	{
		private:

			File					fontFile;
			const Slug::FontHeader	*fontHeader;

			Texture					*curveTexture;
			Texture					*bandTexture;

			~Font();

		public:

			Font(const char *name);

			const Slug::FontHeader *GetFontHeader(void) const
			{
				return (fontHeader);
			}

			Texture *GetCurveTexture(void) const
			{
				return (curveTexture);
			}

			Texture *GetBandTexture(void) const
			{
				return (bandTexture);
			}
	};


	class Framebuffer
	{
		friend class GraphicsManager;

		private:

			GLuint		framebufferObject;

		public:

			Framebuffer(const Texture *colorTexture, const Texture *depthTexture);
			~Framebuffer();

			void BindFramebuffer(void);
	};


	class Drawable
	{
		friend class Renderable;

		public:

			enum
			{
				kTypeTriangleList,
				kTypeTriangleStrip,
				kTypeIndexedTriangleList
			};

			enum
			{
				kBlendReplace,
				kBlendAlpha,
				kBlendAdd,
				kBlendCount
			};

			enum
			{
				kMaxVertexBufferCount	= 1,
				kMaxProgramCount		= 2,
				kMaxTextureCount		= 4,
				kMaxVertexParamCount	= 16,
				kMaxFragmentParamCount	= 16
			};

		private:

			Drawable			*nextDrawable;

			int32				indexStart;
			int32				indexCount;

			Texture				*renderableTexture[kMaxTextureCount];

		protected:

			void SetNextDrawable(Drawable *drawable)
			{
				nextDrawable = drawable;
			}

		public:

			Drawable();
			~Drawable();

			void SetIndexCount(int32 start, int32 count)
			{
				indexStart = start;
				indexCount = count;
			}

			void SetTexture(int32 index, Texture *texture)
			{
				renderableTexture[index] = texture;
				texture->Retain();
			}
	};


	class Renderable : public Drawable
	{
		private:

			int32				renderType;
			int32				blendMode;

			bool				depthWriteFlag;
			bool				cullFaceFlag;

			int32				vertexCount;

			int32				textureCount;
			int32				vertexParamLocation;
			int32				vertexParamCount;
			int32				fragmentParamLocation;
			int32				fragmentParamCount;

			Program				*renderableProgram[kMaxProgramCount];

		public:

			Transformable		*transformable;
			Buffer				*vertexBuffer[kMaxVertexBufferCount];
			Buffer				*indexBuffer;
			VertexArray			*vertexArray;

			Vector4D			vertexParam[kMaxVertexParamCount];
			Vector4D			fragmentParam[kMaxFragmentParamCount];

			Renderable(int32 type);
			~Renderable();

			void SetBlendMode(int32 mode)
			{
				blendMode = mode;
			}

			void SetDepthWriteFlag(bool flag)
			{
				depthWriteFlag = flag;
			}

			void SetCullFaceFlag(bool flag)
			{
				cullFaceFlag = flag;
			}

			void SetVertexCount(int32 count)
			{
				vertexCount = count;
			}

			void SetTextureCount(int32 count)
			{
				textureCount = count;
			}

			void SetVertexParamLocation(int32 location)
			{
				vertexParamLocation = location;
			}

			void SetVertexParamCount(int32 count)
			{
				vertexParamCount = count;
			}

			void SetFragmentParamLocation(int32 location)
			{
				fragmentParamLocation = location;
			}

			void SetFragmentParamCount(int32 count)
			{
				fragmentParamCount = count;
			}

			void SetProgram(int32 index, Program *program)
			{
				renderableProgram[index] = program;
				program->Retain();
			}

			void RenderColor(int32 programIndex);
			void RenderStructure(void);
			void RenderShadow(void);
	};


	class GraphicsManager
	{
		private:

			int32					screenWidth;
			int32					screenHeight;
			float					viewportWidth;
			float					viewportHeight;
			uint32					frameIndex;

			HDC						deviceContext;
			HGLRC					openglContext;

			Texture					*colorTexture;
			Texture					*depthTexture;
			Framebuffer				*framebuffer;

			Texture					*structureColorTexture;
			Texture					*structureDepthTexture;
			Framebuffer				*structureFramebuffer;
			Program					*structureProgram;

			Texture					*shadowTexture;
			Framebuffer				*shadowFramebuffer;
			Program					*shadowProgram;

			Buffer					*quadIndexBuffer;
			static const Triangle	quadTriangle[32766];

			bool					universalUpdateFlag;
			Buffer					*universalBufferObject;
			UniversalParams			universalParams;

			static LRESULT CALLBACK WglWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
			static bool InitWglExtensions(HINSTANCE instance, PIXELFORMATDESCRIPTOR *formatDescriptor);

			void InitializeOpenGL(void);
			void TerminateOpenGL(void);

		public:

			GraphicsManager(int32 width, int32 height);
			~GraphicsManager();

			bool Initialize(HINSTANCE instance, HWND window);
			void Terminate(HWND window);

			float GetViewportWidth(void) const
			{
				return (viewportWidth);
			}

			float GetViewportHeight(void) const
			{
				return (viewportHeight);
			}

			Program *GetStructureProgram(void) const
			{
				return (structureProgram);
			}

			Program *GetShadowProgram(void) const
			{
				return (shadowProgram);
			}

			Buffer *GetQuadIndexBuffer(void) const
			{
				return (quadIndexBuffer);
			}

			UniversalParams *GetUniversalParams(void)
			{
				universalUpdateFlag = true;
				return (&universalParams);
			}

			void BeginRendering(void);
			void EndRendering(void);

			void BeginColorRendering(void);
			void EndColorRendering(void);

			void BeginStructureRendering(void);
			void EndStructureRendering(void);

			void BeginShadowRendering(void);
			void EndShadowRendering(void);
			void SetShadowFace(int32 face);

			void UpdateUniversalParams(void);
	};


	extern GraphicsManager *graphicsManager;
}


#endif
