#include "Controllers.h"


using namespace Engine;


Controller::Controller(Node *node)
{
	targetNode = node;
}

Controller::~Controller()
{
}

void Controller::PreprocessController(void)
{
}

void Controller::MoveController(float deltaTime)
{
}

void Controller::UpdateController(void)
{
}


LightController::LightController(LightNode *node) : Controller(node)
{
	revolutionAxis = RandomUnitBivector3D();
}

LightController::~LightController()
{
}

void LightController::MoveController(float deltaTime)
{
	float angle = deltaTime * 0.2F;
	targetNode->nodeTransform[3] = Matrix3D::MakeRotation(angle, revolutionAxis) * targetNode->nodeTransform[3];
}


SkinController::SkinController(MeshGeometry *mesh) : Controller(mesh)
{
	updateFlag = true;

	skinDataStorage = nullptr;
	transformTable = nullptr;
}

SkinController::~SkinController()
{
	delete[] transformTable;
	delete[] skinDataStorage;
}

void SkinController::PreprocessController(void)
{
	transformTable = new Transform4D[boneNodeArray.GetArrayElementCount()];
}

void SkinController::UpdateController(void)
{
	/*const MeshGeometry *meshGeometry = GetTargetNode();
	int32 totalBoneCount = boneNodeArray.GetArrayElementCount();

	for (int32 k = 0; k < totalBoneCount; k++)
	{
		// ...
	}

	int32 vertexCount = meshGeometry->meshVertexCount;
	const Vertex *bindVertex = meshGeometry->meshVertexArray;

	const SkinData *skinData = GetSkinData();
	volatile Vertex *skinnedVertex = static_cast<volatile Vertex *>(meshGeometry->vertexBuffer[0]->MapBuffer());

	for (int32 a = 0; a < vertexCount; a++)
	{
		int32 boneCount = skinData->boneCount;
		const BoneWeight *boneWeight = skinData->boneWeight;

		// ...

		skinData = skinData->GetNextSkinData();
	}

	meshGeometry->vertexBuffer[0]->UnmapBuffer();*/
}
