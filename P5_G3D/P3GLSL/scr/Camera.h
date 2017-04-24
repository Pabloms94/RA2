#ifndef _CAMERA_H
#define _CAMERA_H

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Camera
{
	private:
		glm::mat4	proj;
		glm::mat4	view;
		glm::mat4 modelView;
		glm::mat4 modelViewProj;
		glm::mat4 normal;

		float rotation;
		float nearp = 1.0;
		float farp = 500.0;
		float pesoRGB = 0.5;
		float pesoAlpha = 0.5;
		float focalDistance = -25.0;
		float maxDistanceFactor = 0.2;

	public:
		Camera();
		~Camera();

		inline void minusZView() {view[3].z -= 0.5;}
		inline void maxZView() { view[3].z += 0.5; }
		inline void minusXView() { view[3].x -= 0.5; }
		inline void maxXView() { view[3].x += 0.5; }

		inline glm::mat4 GetView() { return view; }
		inline glm::mat4 GetProj() { return proj; }
		inline glm::mat4 GetModelView() { return modelView; }
		inline glm::mat4 GetModelViewProj() { return modelViewProj; }
		inline glm::mat4 GetNormal() { return normal; }

		inline void IncrementRot() { rotation = 0.0523599f; }
		inline void DecrementRot() { rotation = -0.0523599f; }

		void SetModelView(glm::mat4 mv);
		void SetModelViewProj(glm::mat4 mvp);
		void SetNormal(glm::mat4 norm);

		void MoveCamera(unsigned char key);
		void RotateCamera();
		void ResizeAspectRatio(float width, float height);
		bool InitCamera();
};

#endif