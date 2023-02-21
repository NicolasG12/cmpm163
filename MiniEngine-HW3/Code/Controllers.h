#ifndef Controllers_h
#define Controllers_h


#include "World.h"


namespace Engine
{
	struct BoneWeight
	{
		int32				boneIndex;
		float				weight;
	};


	struct SkinData
	{
		int32				boneCount;
		BoneWeight			boneWeight[1];

		const SkinData *GetNextSkinData(void) const
		{
			return (reinterpret_cast<const SkinData *>(boneWeight + boneCount));
		}
	};


	class Controller : public ListElement<Controller>
	{
		protected:

			Node	*targetNode;

			Controller(Node *node);

		public:

			bool	updateFlag;			// Set this flag to cause the UpdateController() function to be called the next time the world is rendered.

			virtual ~Controller();

			virtual void PreprocessController(void);
			virtual void MoveController(float deltaTime);
			virtual void UpdateController(void);
	};


	class LightController : public Controller
	{
		private:

			Bivector3D		revolutionAxis;

		public:

			LightController(LightNode *node);
			~LightController();

			void MoveController(float deltaTime) override;
	};


	class SkinController : public Controller
	{
		public:

			Array<const BoneNode *>		boneNodeArray;
			Array<Transform4D>			inverseBindTransformArray;

			char						*skinDataStorage;
			Transform4D					*transformTable;

			SkinController(MeshGeometry *mesh);
			~SkinController();

			MeshGeometry *GetTargetNode(void) const
			{
				return (static_cast<MeshGeometry *>(targetNode));
			}

			const SkinData *GetSkinData(void) const
			{
				return (reinterpret_cast<SkinData *>(skinDataStorage));
			}

			void PreprocessController(void) override;
			void UpdateController(void) override;
	};
}


#endif
