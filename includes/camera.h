#ifndef CAMERA_H
#define CAMERA_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include<vector>

//define possible camera options
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//default camera values

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

class Camera 
{
	private:
		void updateCameraVectors();
		
	public:
		//camera attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		//euler angles;
		float Yaw;
		float Pitch;

		//camera options

		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		Camera(glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),float yaw = YAW, float pitch= PITCH) : Front(glm::vec3(0.0f,0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	    {
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = PITCH;
			updateCameraVectors();
		}

		glm::mat4 GetViewMatrix();
		

		void ProcessKeyboard(Camera_Movement direction, float deltaTime);
		

		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
		

		void ProcessMouseScroll(float yoffset);
		

};



#endif
