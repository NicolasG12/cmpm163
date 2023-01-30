#include "Particles.h"
#include "Controllers.h"
#include "OpenGEX.h"


using namespace Engine;
using namespace OpenGEX;


WorldManager *Engine::worldManager = nullptr;


const uint32 TextGeometry::fontTypeArray[1] = {0};
const uint8 TextGeometry::fontIndexTable[2] = {0, 1};

Slug::FontMap TextGeometry::fontMap =
{
	1, 2, fontTypeArray, fontIndexTable
};


Node::Node(uint32 type)
{
	nodeType = type;
	nodeTransform.SetIdentity();
	nodeController = nullptr;
}

Node::~Node()
{
}

void Node::UpdateNode(void)
{
	const Node *superNode = GetSuperNode();
	if (superNode)
	{
		SetWorldTransform(superNode->GetWorldTransform() * nodeTransform);
	}
	else
	{
		SetWorldTransform(nodeTransform);
	}

	Node *node = GetFirstSubnode();
	while (node)
	{
		node->UpdateNode();
		node = node->GetNextSubnode();
	}
}

Node *Node::FindNode(const char *name)
{
	Node *node = GetFirstSubnode();
	while (node)
	{
		if (node->nodeName == name)
		{
			return (node);
		}

		node = GetNextTreeNode(node);
	}

	return (nullptr);
}


CameraNode::CameraNode(uint32 type) : Node(kNodeCamera)
{
	cameraType = type;
}

CameraNode::~CameraNode()
{
}


FrustumCamera::FrustumCamera(float g, float s, float n) : CameraNode(kCameraFrustum)
{
	projectionDistance = g;
	aspectRatio = s;
	nearDepth = n;
}

FrustumCamera::~FrustumCamera()
{
}

Matrix4D FrustumCamera::CalculateProjectionMatrix(void) const
{
	constexpr float kFrustumEpsilon = 1.0F / 524288.0F;

	Matrix4D	matrix;

	matrix(0,1) = matrix(0,2) = matrix(0,3) = matrix(1,0) = matrix(1,2) = matrix(1,3) = matrix(2,0) = matrix(2,1) = matrix(3,0) = matrix(3,1) = matrix(3,3) = 0.0F;
	matrix(3,2) = 1.0F;

	matrix(0,0) = projectionDistance / aspectRatio;
	matrix(1,1) = -projectionDistance;

	matrix(2,2) = (1.0F - kFrustumEpsilon);
	matrix(2,3) = -nearDepth * (1.0F - kFrustumEpsilon);

	return (matrix);
}

void FrustumCamera::UpdateNode(void)
{
	CameraNode::UpdateNode();

	float g = projectionDistance;
	float s = aspectRatio;

	// Construct the camera-space planes.

	float rx = InverseSqrt(g * g + s * s);
	float ry = InverseSqrt(g * g + 1.0F);

	Plane left(g * rx, 0.0F, s * rx, 0.0F);
	Plane right(-g * rx, 0.0F, s * rx, 0.0F);
	Plane top(0.0F, g * ry, ry, 0.0F);
	Plane bottom(0.0F, -g * ry, ry, 0.0F);
	Plane near(0.0F, 0.0F, 1.0F, -nearDepth);

	// Use the inverse of the camera's object-to-world transformation matrix to
	// transform the planes into world space. Planes are row vectors.

	const Transform4D& transform = GetInverseWorldTransform();

	worldFrustumPlane[0] = left * transform;
	worldFrustumPlane[1] = right * transform;
	worldFrustumPlane[2] = top * transform;
	worldFrustumPlane[3] = bottom * transform;
	worldFrustumPlane[4] = near * transform;
}

bool FrustumCamera::SphereVisible(const Point3D& center, float radius) const
{
	return (true);
}

bool FrustumCamera::BoxVisible(const Transform4D& transform, const Vector3D& size) const
{
	return (true);
}


OrthoCamera::OrthoCamera(float w, float h) : CameraNode(kCameraOrtho)
{
	orthoWidth = w;
	orthoHeight = h;
}

OrthoCamera::~OrthoCamera()
{
}

Matrix4D OrthoCamera::CalculateProjectionMatrix(void) const
{
	Matrix4D	matrix;

	matrix(0,1) = matrix(0,2) = matrix(1,0) = matrix(1,2) = matrix(2,0) = matrix(2,1) = matrix(2,3) = matrix(3,0) = matrix(3,1) = matrix(3,2) = 0.0F;
	matrix(2,2) = matrix(3,3) = 1.0F;

	matrix(0,0) = 2.0F / orthoWidth;
	matrix(0,3) = -1.0F;

	matrix(1,1) = -2.0F / orthoHeight;
	matrix(1,3) = 1.0F;

	return (matrix);
}


LightNode::LightNode(uint32 type, const ColorRGBA& color) : Node(kNodeLight)
{
	lightType = type;
	lightColor = color;
}

LightNode::~LightNode()
{
}

bool LightNode::LightVisible(const FrustumCamera *camera) const
{
	return (true);
}


InfiniteLight::InfiniteLight(const ColorRGBA& color) : LightNode(kLightInfinite, color)
{
}

InfiniteLight::~InfiniteLight()
{
}


PointLight::PointLight(const ColorRGBA& color, float range) : LightNode(kLightPoint, color)
{
	lightRange = range;
}

PointLight::~PointLight()
{
}

Matrix4D PointLight::CalculateShadowProjectionMatrix(void) const
{
	constexpr float n = 0.1F;

	Matrix4D	matrix;

	matrix(0,1) = matrix(0,3) = matrix(1,0) = matrix(1,3) = matrix(2,0) = matrix(2,1) = matrix(3,0) = matrix(3,1) = matrix(3,3) = 0.0F;
	matrix(3,2) = 1.0F;

	matrix(0,0) = 1.0F;
	matrix(1,1) = 1.0F;

	matrix(0,2) = 0.0F;
	matrix(1,2) = 0.0F;

	matrix(2,2) = lightRange / (lightRange - n);
	matrix(2,3) = -n * lightRange / (lightRange - n);

	return (matrix);
}

Transform4D PointLight::CalculateShadowFaceMatrix(int32 face) const
{
	return (Transform4D::identity);
}

bool PointLight::LightVisible(const FrustumCamera *camera) const
{
	return (camera->SphereVisible(GetWorldPosition(), lightRange));
}

bool PointLight::SphereIlluminated(const Point3D& center, float radius) const
{
	// #LIGHTCULL -- Add code here that determines whether a sphere is illuminated.

	return (true);
}

bool PointLight::BoxIlluminated(const Transform4D& transform, const Vector3D& size) const
{
	// #LIGHTCULL -- Add code here that determines whether a box is illuminated.

	return (true);
}


FogNode::FogNode() : Node(kNodeFog)
{
}

FogNode::~FogNode()
{
}


OccluderNode::OccluderNode(const Vector3D& size) : Node(kNodeOccluder)
{
	occluderSize = size;
}

OccluderNode::~OccluderNode()
{
}

void OccluderNode::CalculateOccluderPlanes(const FrustumCamera *cameraNode)
{
	// #OCCLUDER -- Add code here that calculates the occluder planes for the specific camera position.
	// Store the number of occluder planes in planeCount and the planes themselves in planeArray.

	planeCount = 0;		// If the occluder is not visible (or the camera is inside the occluder), set the plane count to zero.
}

bool OccluderNode::SphereOccluded(const Point3D& center, float radius) const
{
	// #OCCLUDER -- Add code here that determines whether a sphere is occluded.


	return (false);
}

bool OccluderNode::BoxOccluded(const Transform4D& transform, const Vector3D& size) const
{
	// #OCCLUDER -- Add code here that determines whether a box is occluded.


	return (false);
}


BoneNode::BoneNode() : Node(kNodeBone)
{
}

BoneNode::~BoneNode()
{
}


RenderableNode::RenderableNode(uint32 type) : Node(type), Renderable(Renderable::kTypeIndexedTriangleList)
{
}

RenderableNode::~RenderableNode()
{
}

void RenderableNode::EstablishStandardVertexArray(void)
{
	vertexArray = new VertexArray;
	vertexArray->SetAttribBuffer(0, sizeof(Vertex), vertexBuffer[0]);
	vertexArray->SetAttribArray(0, 3, VertexArray::kFormatFloat32, 0);
	vertexArray->SetAttribArray(1, 3, VertexArray::kFormatFloat32, sizeof(Point3D));
	vertexArray->SetAttribArray(2, 4, VertexArray::kFormatFloat32, sizeof(Point3D) + sizeof(Bivector3D));
	vertexArray->SetAttribArray(3, 2, VertexArray::kFormatFloat32, sizeof(Point3D) + sizeof(Bivector3D) + sizeof(Vector4D));
}

void RenderableNode::PrepareToRender(const Matrix4D& viewProjectionMatrix)
{
	if (transformable)
	{
		Matrix4D mvp = viewProjectionMatrix * transformable->GetWorldTransform();

		vertexParam[0] = !mvp.row0;
		vertexParam[1] = !mvp.row1;
		vertexParam[2] = !mvp.row2;
		vertexParam[3] = !mvp.row3;
	}
	else
	{
		vertexParam[0] = !viewProjectionMatrix.row0;
		vertexParam[1] = !viewProjectionMatrix.row1;
		vertexParam[2] = !viewProjectionMatrix.row2;
		vertexParam[3] = !viewProjectionMatrix.row3;
	}

	vertexParam[4] = !GetWorldTransform().row0;
	vertexParam[5] = !GetWorldTransform().row1;
	vertexParam[6] = !GetWorldTransform().row2;

	SetVertexParamCount(7);
}


GeometryNode::GeometryNode(uint32 type) : RenderableNode(kNodeGeometry)
{
	geometryType = type;
	transformable = this;
}

GeometryNode::~GeometryNode()
{
}

bool GeometryNode::GeometryVisible(const FrustumCamera *camera) const
{
	return (true);
}

bool GeometryNode::GeometryOccluded(const OccluderNode *occluder) const
{
	return (false);
}

bool GeometryNode::GeometryIlluminated(const PointLight *light) const
{
	return (true);
}


MeshGeometry::MeshGeometry(int32 vertexCount, int32 triangleCount, Vertex *vertexArray, Triangle *triangleArray) : GeometryNode(kGeometryMesh)
{
	meshVertexCount = vertexCount;
	meshTriangleCount = triangleCount;

	meshVertexArray = vertexArray;
	meshTriangleArray = triangleArray;

	SetVertexCount(vertexCount);
	SetIndexCount(0, triangleCount * 3);

	vertexBuffer[0] = new Buffer(vertexCount * sizeof(Vertex), vertexArray);
	indexBuffer = new Buffer(triangleCount * sizeof(Triangle), triangleArray);

	EstablishStandardVertexArray();
}

MeshGeometry::~MeshGeometry()
{
	delete[] meshTriangleArray;
	delete[] meshVertexArray;
}


SphereGeometry::SphereGeometry(float radius) : GeometryNode(kGeometrySphere)
{
	constexpr float kTexcoordScaleS = 4.0F;
	constexpr float kTexcoordScaleT = 2.0F;

	sphereRadius = radius;

	float dx = 1.0F / float(xdiv);
	float dz = 1.0F / float(zdiv);

	Vertex *vertex = sphereVertex;

	for (machine i = 0; i < xdiv; i++)
	{
		float s = float(i) * dx;
		Vector2D v = CosSin(s * Math::tau);

		for (machine j = 1; j < zdiv; j++)
		{
			float t = float(j) * dz;
			Vector2D u = CosSin((1.0F - t) * Math::tau_over_2);

			float x = v.x * u.y;
			float y = v.y * u.y;
			float z = u.x;

			vertex->position.Set(x * radius, y * radius, z * radius);
			vertex->normal.Set(x, y, z);
			vertex->tangent.Set(-y, x, z, 1.0F);
			vertex->texcoord.Set(s * kTexcoordScaleS, t * kTexcoordScaleT);
			vertex++;
		}

		vertex->position.Set(0.0F, 0.0F, -radius);
		vertex->normal.Set(0.0F, 0.0F, -1.0F);
		vertex->tangent.Set(-v.y, v.x, 0.0F, 1.0F);
		vertex->texcoord.Set(s * kTexcoordScaleS, 0.0F);
		vertex++;

		vertex->position.Set(0.0F, 0.0F, radius);
		vertex->normal.Set(0.0F, 0.0F, 1.0F);
		vertex->tangent.Set(-v.y, v.x, 0.0F, 1.0F);
		vertex->texcoord.Set(s * kTexcoordScaleS, kTexcoordScaleT);
		vertex++;
	}

	for (machine j = 0; j < zdiv - 1; j++)
	{
		vertex->position = sphereVertex[j].position;
		vertex->normal = sphereVertex[j].normal;
		vertex->tangent = sphereVertex[j].tangent;
		vertex->texcoord.Set(kTexcoordScaleS, float(j + 1) * dz * kTexcoordScaleT);
		vertex++;
	}

	Triangle *triangle = sphereTriangle;

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * (zdiv + 1);

		triangle->Set(k + zdiv - 1, k + zdiv + 1, k);
		triangle++;

		for (machine j = 0; j < zdiv - 2; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
				triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
			}
			else
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
				triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
			}

			triangle += 2;
		}

		triangle->Set(k + zdiv - 2, k + zdiv * 2 - 1, k + zdiv);
		triangle++;
	}

	// Establish vertex buffers in the Renderable base class.

	SetVertexCount(kSphereVertexCount);
	SetIndexCount(0, kSphereTriangleCount * 3);

	vertexBuffer[0] = new Buffer(kSphereVertexCount * sizeof(Vertex), sphereVertex);
	indexBuffer = new Buffer(kSphereTriangleCount * sizeof(Triangle), sphereTriangle);

	EstablishStandardVertexArray();
}

SphereGeometry::~SphereGeometry()
{
}

bool SphereGeometry::GeometryVisible(const FrustumCamera *camera) const
{
	return (camera->SphereVisible(GetWorldPosition(), sphereRadius));
}

bool SphereGeometry::GeometryOccluded(const OccluderNode *occluder) const
{
	return (occluder->SphereOccluded(GetWorldPosition(), sphereRadius));
}

bool SphereGeometry::GeometryIlluminated(const PointLight *light) const
{
	return (light->SphereIlluminated(GetWorldPosition(), sphereRadius));
}


BoxGeometry::BoxGeometry(const Vector3D& size) : GeometryNode(kGeometryBox)
{
	boxSize = size;

	// Positive x side

	boxVertex[0].position.Set(size.x, 0.0F, 0.0F);
	boxVertex[1].position.Set(size.x, size.y, 0.0F);
	boxVertex[2].position.Set(size.x, size.y, size.z);
	boxVertex[3].position.Set(size.x, 0.0F, size.z);

	boxVertex[0].normal.Set(1.0F, 0.0F, 0.0F);
	boxVertex[1].normal.Set(1.0F, 0.0F, 0.0F);
	boxVertex[2].normal.Set(1.0F, 0.0F, 0.0F);
	boxVertex[3].normal.Set(1.0F, 0.0F, 0.0F);

	boxVertex[0].tangent.Set(0.0F, 1.0F, 0.0F, 1.0F);
	boxVertex[1].tangent.Set(0.0F, 1.0F, 0.0F, 1.0F);
	boxVertex[2].tangent.Set(0.0F, 1.0F, 0.0F, 1.0F);
	boxVertex[3].tangent.Set(0.0F, 1.0F, 0.0F, 1.0F);

	boxVertex[0].texcoord.Set(0.0F, 0.0F);
	boxVertex[1].texcoord.Set(size.y, 0.0F);
	boxVertex[2].texcoord.Set(size.y, size.z);
	boxVertex[3].texcoord.Set(0.0F, size.z);

	boxTriangle[0].index[0] = 0;
	boxTriangle[0].index[1] = 1;
	boxTriangle[0].index[2] = 2;

	boxTriangle[1].index[0] = 0;
	boxTriangle[1].index[1] = 2;
	boxTriangle[1].index[2] = 3;

	// Positive y side

	boxVertex[4].position.Set(size.x, size.y, 0.0F);
	boxVertex[5].position.Set(0.0F, size.y, 0.0F);
	boxVertex[6].position.Set(0.0F, size.y, size.z);
	boxVertex[7].position.Set(size.x, size.y, size.z);

	boxVertex[4].normal.Set(0.0F, 1.0F, 0.0F);
	boxVertex[5].normal.Set(0.0F, 1.0F, 0.0F);
	boxVertex[6].normal.Set(0.0F, 1.0F, 0.0F);
	boxVertex[7].normal.Set(0.0F, 1.0F, 0.0F);

	boxVertex[4].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[5].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[6].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[7].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);

	boxVertex[4].texcoord.Set(0.0F, 0.0F);
	boxVertex[5].texcoord.Set(size.x, 0.0F);
	boxVertex[6].texcoord.Set(size.x, size.z);
	boxVertex[7].texcoord.Set(0.0F, size.z);

	boxTriangle[2].index[0] = 4;
	boxTriangle[2].index[1] = 5;
	boxTriangle[2].index[2] = 6;

	boxTriangle[3].index[0] = 4;
	boxTriangle[3].index[1] = 6;
	boxTriangle[3].index[2] = 7;

	// Negative x side

	boxVertex[8].position.Set(0.0F, size.y, 0.0F);
	boxVertex[9].position.Set(0.0F, 0.0F, 0.0F);
	boxVertex[10].position.Set(0.0F, 0.0F, size.z);
	boxVertex[11].position.Set(0.0F, size.y, size.z);

	boxVertex[8].normal.Set(-1.0F, 0.0F, 0.0F);
	boxVertex[9].normal.Set(-1.0F, 0.0F, 0.0F);
	boxVertex[10].normal.Set(-1.0F, 0.0F, 0.0F);
	boxVertex[11].normal.Set(-1.0F, 0.0F, 0.0F);

	boxVertex[8].tangent.Set(0.0F, -1.0F, 0.0F, 1.0F);
	boxVertex[9].tangent.Set(0.0F, -1.0F, 0.0F, 1.0F);
	boxVertex[10].tangent.Set(0.0F, -1.0F, 0.0F, 1.0F);
	boxVertex[11].tangent.Set(0.0F, -1.0F, 0.0F, 1.0F);

	boxVertex[8].texcoord.Set(0.0F, 0.0F);
	boxVertex[9].texcoord.Set(size.y, 0.0F);
	boxVertex[10].texcoord.Set(size.y, size.z);
	boxVertex[11].texcoord.Set(0.0F, size.z);

	boxTriangle[4].index[0] = 8;
	boxTriangle[4].index[1] = 9;
	boxTriangle[4].index[2] = 10;

	boxTriangle[5].index[0] = 8;
	boxTriangle[5].index[1] = 10;
	boxTriangle[5].index[2] = 11;

	// Negative y side

	boxVertex[12].position.Set(0.0F, 0.0F, 0.0F);
	boxVertex[13].position.Set(size.x, 0.0F, 0.0F);
	boxVertex[14].position.Set(size.x, 0.0F, size.z);
	boxVertex[15].position.Set(0.0F, 0.0F, size.z);

	boxVertex[12].normal.Set(0.0F, -1.0F, 0.0F);
	boxVertex[13].normal.Set(0.0F, -1.0F, 0.0F);
	boxVertex[14].normal.Set(0.0F, -1.0F, 0.0F);
	boxVertex[15].normal.Set(0.0F, -1.0F, 0.0F);

	boxVertex[12].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[13].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[14].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[15].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);

	boxVertex[12].texcoord.Set(0.0F, 0.0F);
	boxVertex[13].texcoord.Set(size.x, 0.0F);
	boxVertex[14].texcoord.Set(size.x, size.z);
	boxVertex[15].texcoord.Set(0.0F, size.z);

	boxTriangle[6].index[0] = 12;
	boxTriangle[6].index[1] = 13;
	boxTriangle[6].index[2] = 14;

	boxTriangle[7].index[0] = 12;
	boxTriangle[7].index[1] = 14;
	boxTriangle[7].index[2] = 15;

	// Positive z side

	boxVertex[16].position.Set(0.0F, 0.0F, size.z);
	boxVertex[17].position.Set(size.x, 0.0F, size.z);
	boxVertex[18].position.Set(size.x, size.y, size.z);
	boxVertex[19].position.Set(0.0F, size.y, size.z);

	boxVertex[16].normal.Set(0.0F, 0.0F, 1.0F);
	boxVertex[17].normal.Set(0.0F, 0.0F, 1.0F);
	boxVertex[18].normal.Set(0.0F, 0.0F, 1.0F);
	boxVertex[19].normal.Set(0.0F, 0.0F, 1.0F);

	boxVertex[16].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[17].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[18].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[19].tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);

	boxVertex[16].texcoord.Set(0.0F, 0.0F);
	boxVertex[17].texcoord.Set(size.x, 0.0F);
	boxVertex[18].texcoord.Set(size.x, size.y);
	boxVertex[19].texcoord.Set(0.0F, size.y);

	boxTriangle[8].index[0] = 16;
	boxTriangle[8].index[1] = 17;
	boxTriangle[8].index[2] = 18;

	boxTriangle[9].index[0] = 16;
	boxTriangle[9].index[1] = 18;
	boxTriangle[9].index[2] = 19;

	// Negative z side

	boxVertex[20].position.Set(size.x, 0.0F, 0.0F);
	boxVertex[21].position.Set(0.0F, 0.0F, 0.0F);
	boxVertex[22].position.Set(0.0F, size.y, 0.0F);
	boxVertex[23].position.Set(size.x, size.y, 0.0F);

	boxVertex[20].normal.Set(0.0F, 0.0F, -1.0F);
	boxVertex[21].normal.Set(0.0F, 0.0F, -1.0F);
	boxVertex[22].normal.Set(0.0F, 0.0F, -1.0F);
	boxVertex[23].normal.Set(0.0F, 0.0F, -1.0F);

	boxVertex[20].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[21].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[22].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);
	boxVertex[23].tangent.Set(-1.0F, 0.0F, 0.0F, 1.0F);

	boxVertex[20].texcoord.Set(0.0F, 0.0F);
	boxVertex[21].texcoord.Set(size.x, 0.0F);
	boxVertex[22].texcoord.Set(size.x, size.y);
	boxVertex[23].texcoord.Set(0.0F, size.y);

	boxTriangle[10].index[0] = 20;
	boxTriangle[10].index[1] = 21;
	boxTriangle[10].index[2] = 22;

	boxTriangle[11].index[0] = 20;
	boxTriangle[11].index[1] = 22;
	boxTriangle[11].index[2] = 23;

	// Establish vertex buffers in the Renderable base class.

	SetVertexCount(24);
	SetIndexCount(0, 36);

	vertexBuffer[0] = new Buffer(24 * sizeof(Vertex), boxVertex);
	indexBuffer = new Buffer(12 * sizeof(Triangle), boxTriangle);

	EstablishStandardVertexArray();
}

BoxGeometry::~BoxGeometry()
{
}

bool BoxGeometry::GeometryVisible(const FrustumCamera *camera) const
{
	return (camera->BoxVisible(GetWorldTransform(), boxSize));
}

bool BoxGeometry::GeometryOccluded(const OccluderNode *occluder) const
{
	return (occluder->BoxOccluded(GetWorldTransform(), boxSize));
}

bool BoxGeometry::GeometryIlluminated(const PointLight *light) const
{
	return (light->BoxIlluminated(GetWorldTransform(), boxSize));
}


TextGeometry::TextGeometry(const char *text, Font *font, float width, const Slug::LayoutData *data) : GeometryNode(kGeometryText)
{
	static const char vertexMain[] =
	{
		"layout(location = 0) in float4 attrib0;\n"
		"layout(location = 1) in float4 attrib1;\n"
		"layout(location = 2) in float4 attrib2;\n"
		"layout(location = 3) in float4 attrib3;\n"
		"layout(location = 4) in float4 attrib4;\n"

		"layout(location = 0) uniform float4 slug_matrix[5];\n"

		"out float4 vresult_color;\n"
		"out float2 vresult_texcoord;\n"
		"flat out float4 vresult_banding;\n"
		"flat out int4 vresult_glyph;\n"

		"void main()\n"
		"{\n"
			"float2 p;\n"

			"vresult_texcoord = SlugDilate(attrib0, attrib1, attrib2, slug_matrix[0], slug_matrix[1], slug_matrix[3], slug_matrix[4].xy, p);\n"
			"gl_Position.x = p.x * slug_matrix[0].x + p.y * slug_matrix[0].y + slug_matrix[0].w;\n"
			"gl_Position.y = p.x * slug_matrix[1].x + p.y * slug_matrix[1].y + slug_matrix[1].w;\n"
			"gl_Position.z = p.x * slug_matrix[2].x + p.y * slug_matrix[2].y + slug_matrix[2].w;\n"
			"gl_Position.w = p.x * slug_matrix[3].x + p.y * slug_matrix[3].y + slug_matrix[3].w;\n"

			"SlugUnpack(attrib1, attrib3, vresult_banding, vresult_glyph);\n"
			"vresult_color = attrib4;\n"
		"}\n"
	};

	static const char fragmentMain[] =
	{
		"in vec4 vresult_color;\n"
		"in vec2 vresult_texcoord;\n"
		"flat in vec4 vresult_banding;\n"
		"flat in ivec4 vresult_glyph;\n"

		"layout(binding = 0) uniform sampler2D curveTexture;\n"
		"layout(binding = 1) uniform usampler2D bandTexture;\n"

		"layout(location = 0) out float4 fcolor;\n"

		"void main()\n"
		"{\n"
			"fcolor = SlugRender(curveTexture, bandTexture, vresult_texcoord, vresult_color, vresult_banding, vresult_glyph);\n"
		"}\n"
	};

	uint32			vertexIndex;
	uint32			fragmentIndex;
	const char		*vertexStringArray[Slug::kMaxVertexStringCount + 1];
	const char		*fragmentStringArray[Slug::kMaxFragmentStringCount + 1];

	textFont = font;
	font->Retain();

	textWidth = width;
	layoutData = *data;

	if (text)
	{
		SetText(text);
	}

	SetBlendMode(Renderable::kBlendAlpha);
	SetDepthWriteFlag(false);
	SetCullFaceFlag(false);

	SetTextureCount(2);
	SetTexture(0, font->GetCurveTexture());
	SetTexture(1, font->GetBandTexture());

	Slug::GetShaderIndices(0, &vertexIndex, &fragmentIndex);

	vertexStringArray[0] = fragmentStringArray[0] = "#version 450\n";
	int32 vertexStringCount = Slug::GetVertexShaderSourceCode(vertexIndex, &vertexStringArray[1], Slug::kVertexShaderProlog);
	int32 fragmentStringCount = Slug::GetFragmentShaderSourceCode(fragmentIndex, &fragmentStringArray[1], Slug::kFragmentShaderProlog);
	vertexStringArray[vertexStringCount + 1] = vertexMain;
	fragmentStringArray[fragmentStringCount + 1] = fragmentMain;

	Program *program = new Program(vertexStringCount + 2, vertexStringArray, fragmentStringCount + 2, fragmentStringArray);
	SetProgram(0, program);
	program->Release();
}

TextGeometry::~TextGeometry()
{
}

void TextGeometry::SetText(const char *text)
{
	Slug::FontDesc				fontDesc[2];
	Slug::GeometryBuffer		geometryBuffer[2];
	int32						maxVertexCount[2];
	int32						maxTriangleCount[2];
	int32						triangleCount[2];
	volatile Slug::Triangle		*triangleBase[2];
	Slug::LineData				lineData;

	delete indexBuffer;
	delete vertexBuffer[0];

	fontDesc[0].fontHeader = textFont->GetFontHeader();
	fontDesc[0].fontScale = 1.0F;
	fontDesc[0].fontOffset = 0.0F;

	Font *emojiFont = worldManager->GetEmojiFont();
	fontDesc[1].fontHeader = emojiFont->GetFontHeader();
	fontDesc[1].fontScale = 1.0F;
	fontDesc[1].fontOffset = 0.0F;

	Slug::BreakMultiLineTextEx(2, fontDesc, &fontMap, &layoutData, text, textWidth, 0, nullptr, 0, nullptr, 0, nullptr, 1, &lineData);
	Slug::CountMultiLineTextEx(2, fontDesc, &fontMap, &layoutData, text, 0, 1, &lineData, maxVertexCount, maxTriangleCount);

	vertexBuffer[0] = new Buffer((maxVertexCount[0] + maxVertexCount[1]) * sizeof(Slug::Vertex));
	indexBuffer = new Buffer((maxTriangleCount[0] + maxTriangleCount[1]) * sizeof(Slug::Triangle));

	geometryBuffer[0].vertexData = static_cast<volatile Slug::Vertex *>(vertexBuffer[0]->MapBuffer());
	geometryBuffer[0].triangleData = static_cast<volatile Slug::Triangle *>(indexBuffer->MapBuffer());
	geometryBuffer[0].vertexIndex = 0;

	geometryBuffer[1].vertexData = geometryBuffer[0].vertexData + maxVertexCount[0];
	geometryBuffer[1].triangleData = geometryBuffer[0].triangleData + maxTriangleCount[0];
	geometryBuffer[1].vertexIndex = maxVertexCount[0];

	triangleBase[0] = geometryBuffer[0].triangleData;
	triangleBase[1] = geometryBuffer[1].triangleData;

	Slug::BuildMultiLineTextEx(2, fontDesc, &fontMap, &layoutData, text, 0, 1, &lineData, Point2D(0.0F, 0.0F), textWidth, geometryBuffer);

	indexBuffer->UnmapBuffer();
	vertexBuffer[0]->UnmapBuffer();

	triangleCount[0] = int32(geometryBuffer[0].triangleData - triangleBase[0]);
	triangleCount[1] = int32(geometryBuffer[1].triangleData - triangleBase[1]);

	SetIndexCount(0, triangleCount[0] * 3);

	if (triangleCount[1] != 0)
	{
		SetNextDrawable(&emojiDrawable);

		emojiDrawable.SetIndexCount(maxTriangleCount[0] * 3, triangleCount[1] * 3);
		emojiDrawable.SetTexture(0, emojiFont->GetCurveTexture());
		emojiDrawable.SetTexture(1, emojiFont->GetBandTexture());
	}
	else
	{
		SetNextDrawable(nullptr);
	}

	vertexArray = new VertexArray;
	vertexArray->SetAttribBuffer(0, sizeof(Slug::Vertex), vertexBuffer[0]);
	vertexArray->SetAttribArray(0, 4, VertexArray::kFormatFloat32, 0);
	vertexArray->SetAttribArray(1, 4, VertexArray::kFormatFloat32, sizeof(Vector4D));
	vertexArray->SetAttribArray(2, 4, VertexArray::kFormatFloat32, sizeof(Vector4D) * 2);
	vertexArray->SetAttribArray(3, 4, VertexArray::kFormatFloat32, sizeof(Vector4D) * 3);
	vertexArray->SetAttribArray(4, 4, VertexArray::kFormatUint8, sizeof(Vector4D) * 4);
}

void TextGeometry::PrepareToRender(const Matrix4D& viewProjectionMatrix)
{
	GeometryNode::PrepareToRender(viewProjectionMatrix);

	vertexParam[4].Set(graphicsManager->GetViewportWidth(), graphicsManager->GetViewportHeight(), 0.0F, 0.0F);
	SetVertexParamCount(5);
}


WorldManager::WorldManager()
{
	rootNode = new Node(0);

	cameraNode = nullptr;
	fogNode = nullptr;

	ambientColor.Set(0.0F, 0.0F, 0.0F, 0.0F);

	overlayCameraNode = nullptr;

	emojiFont = new Font("Fonts/Emoji.slug");
}

WorldManager::~WorldManager()
{
	emojiFont->Release();

	geometryList.RemoveAllListElements();
	occluderList.RemoveAllListElements();
	lightList.RemoveAllListElements();

	delete rootNode;
	delete overlayCameraNode;
}

void WorldManager::PreprocessWorld(void)
{
	Node *node = rootNode->GetFirstSubnode();
	while (node)
	{
		uint32 type = node->GetNodeType();
		if (type == kNodeGeometry)
		{
			geometryList.AppendListElement(static_cast<GeometryNode *>(node));
		}
		else if (type == kNodeLight)
		{
			lightList.AppendListElement(static_cast<LightNode *>(node));
		}
		else if (type == kNodeCamera)
		{
			if (!cameraNode)
			{
				CameraNode *camera = static_cast<CameraNode *>(node);
				if (camera->GetCameraType() == kCameraFrustum)
				{
					cameraNode = static_cast<FrustumCamera *>(camera);
				}
			}
		}
		else if (type == kNodeFog)
		{
			FogNode *fog = static_cast<FogNode *>(node);
			fogNode = fog;
		}
		else if (type == kNodeOccluder)
		{
			occluderList.AppendListElement(static_cast<OccluderNode *>(node));
		}
		else if (type == kNodeParticleSystem)
		{
			particleSystemList.AppendListElement(static_cast<ParticleSystem *>(node));
		}

		Controller *controller = node->nodeController;
		if (controller)
		{
			controllerList.AppendListElement(controller);
			controller->PreprocessController();
		}

		node = rootNode->GetNextTreeNode(node);
	}

	for (GeometryNode *geometryNode : overlayGeometryList)
	{
		geometryNode->SetWorldTransform(geometryNode->nodeTransform);
	}
}

void WorldManager::RenderWorld(int32 *ambientDrawCount, int32 *lightDrawCount, int32 *lightSourceCount)
{
	Array<GeometryNode *>		visibleGeometryArray;
	Array<GeometryNode *>		illuminatedGeometryArray;

	int32 pointLightCount = 0;
	int32 illuminatedCount = 0;

	if (cameraNode)
	{
		extern float deltaTime;

		for (Controller *controller : controllerList)
		{
			controller->MoveController(deltaTime);
		}

		rootNode->UpdateNode();

		for (Controller *controller : controllerList)
		{
			if (controller->updateFlag)
			{
				controller->updateFlag = false;
				controller->UpdateController();
			}
		}

		for (ParticleSystem *particleSystem : particleSystemList)
		{
			particleSystem->MoveParticles(deltaTime);
			particleSystem->RenderParticles();
		}

		UniversalParams *params = graphicsManager->GetUniversalParams();
		params->cameraPosition = cameraNode->GetWorldPosition();
		params->cameraRight = cameraNode->GetWorldTransform()[0];
		params->cameraDown = cameraNode->GetWorldTransform()[1];
		params->ambientColor = ambientColor;

		if (fogNode)
		{
			// #FOG -- Add code here to establish the fog plane, color, and other parameters in the UniversalParams structure.


		}

		for (OccluderNode *occluderNode : occluderList)
		{
			occluderNode->CalculateOccluderPlanes(cameraNode);
		}

		Matrix4D viewProjectionMatrix = cameraNode->CalculateProjectionMatrix() * cameraNode->GetInverseWorldTransform();

		// Render structure pass.

		graphicsManager->BeginStructureRendering();

		for (GeometryNode *geometryNode : geometryList)
		{
			if (geometryNode->GeometryVisible(cameraNode))
			{
				for (OccluderNode *occluderNode : occluderList)
				{
					if (geometryNode->GeometryOccluded(occluderNode))
					{
						goto next;
					}
				}

				visibleGeometryArray.AppendArrayElement(geometryNode);

				geometryNode->PrepareToRender(viewProjectionMatrix);
				geometryNode->RenderStructure();

				next:;
			}
		}

		graphicsManager->EndStructureRendering();

		// Render ambient pass.

		graphicsManager->BeginColorRendering();

		for (GeometryNode *geometryNode : visibleGeometryArray)
		{
			geometryNode->RenderColor(0);
		}

		// Render light passes.

		for (const LightNode *lightNode : lightList)
		{
			if (lightNode->GetLightType() == kLightPoint)
			{
				const PointLight *pointLight = static_cast<const PointLight *>(lightNode);

				if (lightNode->LightVisible(cameraNode))
				{
					for (GeometryNode *geometryNode : visibleGeometryArray)
					{
						if (geometryNode->GeometryIlluminated(pointLight))
						{
							illuminatedGeometryArray.AppendArrayElement(geometryNode);
						}
					}

					params = graphicsManager->GetUniversalParams();
					params->lightColor = lightNode->lightColor;
					params->lightPosition = lightNode->GetWorldPosition();

					float r = pointLight->lightRange;
					params->attenConst.Set(-4.0F / (r * r), 1.01865736F, 0.01865736F, 0.0F);

					const Transform4D& M = pointLight->GetInverseWorldTransform();
					params->inverseLightTransform[0] = !M.row0;
					params->inverseLightTransform[1] = !M.row1;
					params->inverseLightTransform[2] = !M.row2;

					for (GeometryNode *geometryNode : illuminatedGeometryArray)
					{
						geometryNode->PrepareToRender(viewProjectionMatrix);
						geometryNode->RenderColor(1);
						illuminatedCount++;
					}

					illuminatedGeometryArray.ClearArray();
					pointLightCount++;
				}
			}
		}

		// Render effect pass.

		for (ParticleSystem *particleSystem : particleSystemList)
		{
			particleSystem->PrepareToRender(viewProjectionMatrix);
			particleSystem->RenderColor(0);
		}
	}
	else
	{
		// If there's no camera, this clears the framebuffer and does nothing else.

		graphicsManager->BeginColorRendering();
	}

	*ambientDrawCount = visibleGeometryArray.GetArrayElementCount();
	*lightDrawCount = illuminatedCount;
	*lightSourceCount = pointLightCount;
}

void WorldManager::RenderOverlay(void)
{
	if (overlayCameraNode)
	{
		Matrix4D projectionMatrix = overlayCameraNode->CalculateProjectionMatrix();

		for (GeometryNode *geometryNode : overlayGeometryList)
		{
			geometryNode->PrepareToRender(projectionMatrix);
			geometryNode->RenderColor(0);
		}
	}

	graphicsManager->EndColorRendering();
}

void WorldManager::ConstructNormalMap(const Color4U *heightMap, Color2S *normalMap, int32 width, int32 height, float scale)
{
}

void WorldManager::ConstructParallaxMap(const Color4U *heightMap, Color1S *parallaxMap, int32 width, int32 height, float scale)
{
}

Vector2D WorldManager::CalculateParallaxScale(int32 width, int32 height, float scale)
{
	return (Vector2D(0.0F, 0.0F));
}

void WorldManager::BuildWorld(void)
{
	Color4U			*textureImage;
	Color4U			*colorMipmapImages;
	Color2S			*normalMipmapImages;
	Color1S			*parallaxMipmapImages;
	Integer2D		imageSize;

	// Load the stone wall texture, create its mipmaps, and create a Texture object for it.

	ImportTargaImageFile("Textures/StoneWall-diff.tga", &textureImage, &imageSize);
	int32 mipmapCount = GenerateMipmapImages(Integer3D(imageSize, 1), textureImage, &colorMipmapImages);

	Texture *wallDiffuseTexture = new Texture(Texture::kType2D, Texture::kFormatGammaRgba, imageSize.x, imageSize.y, 1, mipmapCount, colorMipmapImages);

	ReleaseMipmapImages(colorMipmapImages);
	ReleaseTargaImageData(textureImage);

	// Load the source for the vertex and fragment shaders, and create Program objects for them.

	File vertexShader("Shaders/Vertex.glsl");
	File ambientFragmentShader("Shaders/Ambient.glsl");
	File lightFragmentShader("Shaders/Light.glsl");

	const char *vertexString = vertexShader.GetData();
	const char *ambientFragmentString = ambientFragmentShader.GetData();
	const char *lightFragmentString = lightFragmentShader.GetData();
	Program *ambientProgram = new Program(vertexString, ambientFragmentString);
	Program *lightProgram = new Program(vertexString, lightFragmentString);

	// Create geometries at random locations.
	// The texture and programs created above are assigned to each geometry.

	for (int32 k = 0; k < 50; k++)
	{
		BoxGeometry *boxGeometry = new BoxGeometry(Vector3D(RandomFloat(0.25F, 1.0F), RandomFloat(0.25F, 1.0F), RandomFloat(0.25F, 1.0F)));
		Vector2D direction = RandomUnitVector2D();
		boxGeometry->nodeTransform.Set(RandomUnitQuaternion().GetRotationMatrix(), Point3D(direction * (RandomFloat(5.0F) + 5.0F), RandomFloat(4.0F)));
		rootNode->AppendSubnode(boxGeometry);

		boxGeometry->SetTexture(0, wallDiffuseTexture);
		boxGeometry->SetTextureCount(1);

		boxGeometry->SetProgram(0, ambientProgram);
		boxGeometry->SetProgram(1, lightProgram);

		boxGeometry->fragmentParam[0].Set(RandomFloat(0.875F) + 0.125F, RandomFloat(0.875F) + 0.125F, RandomFloat(0.875F) + 0.125F, 1.0F);
		boxGeometry->fragmentParam[1].Set(1.0F, 1.0F, 1.0F, 200.0F);
		boxGeometry->SetFragmentParamLocation(32);
		boxGeometry->SetFragmentParamCount(2);
	}

	for (int32 k = 0; k < 10; k++)
	{
		SphereGeometry *sphereGeometry = new SphereGeometry(RandomFloat(0.25F, 1.0F));
		Vector2D direction = RandomUnitVector2D();
		sphereGeometry->nodeTransform.SetTranslation(Point3D(direction * (RandomFloat(5.0F) + 5.0F), RandomFloat(4.0F)));
		rootNode->AppendSubnode(sphereGeometry);

		sphereGeometry->SetTexture(0, wallDiffuseTexture);
		sphereGeometry->SetTextureCount(1);

		sphereGeometry->SetProgram(0, ambientProgram);
		sphereGeometry->SetProgram(1, lightProgram);

		sphereGeometry->fragmentParam[0].Set(RandomFloat(0.875F) + 0.125F, RandomFloat(0.875F) + 0.125F, RandomFloat(0.875F) + 0.125F, 1.0F);
		sphereGeometry->fragmentParam[1].Set(1.0F, 1.0F, 1.0F, 200.0F);
		sphereGeometry->SetFragmentParamLocation(32);
		sphereGeometry->SetFragmentParamCount(2);
	}

	// We can release our local references to the textures and programs here because they are
	// still referenced by all of the geometries and will not be deleted.

	lightProgram->Release();
	ambientProgram->Release();
	wallDiffuseTexture->Release();

	// Set the ambient light color and add some point lights to the world.

	ambientColor.Set(0.05F, 0.05F, 0.05F, 0.0F);

	File constantFragmentShader("Shaders/Constant.glsl");
	const char *constantFragmentString = constantFragmentShader.GetData();
	Program *constantProgram = new Program(vertexString, constantFragmentString);

	// Moving lights

	for (int32 k = 0; k < 2; k++)
	{
		PointLight *pointLight = new PointLight(ColorRGBA(5.0F, 5.0F, 5.0F), 25.0F);
		pointLight->nodeTransform[3] = RandomPointInsideSphere(20.0F);
		pointLight->nodeController = new LightController(pointLight);
		rootNode->AppendSubnode(pointLight);

		SphereGeometry *sphereGeometry = new SphereGeometry(0.125F);
		sphereGeometry->SetProgram(0, constantProgram);
		pointLight->AppendSubnode(sphereGeometry);

		sphereGeometry->fragmentParam[0].Set(1.0F, 1.0F, 1.0F, 1.0F);
		sphereGeometry->SetFragmentParamLocation(32);
		sphereGeometry->SetFragmentParamCount(1);
	}

	constantProgram->Release();
}
