#ifndef Renderer_h__
#define Renderer_h__

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <memory>


#include "Shaders/shader.hpp"
#include "Model/Model.h"
#include "EulerCamera/EulerCamera.h"
#include "Texture/texture.h"
#include "GraphicsDevice/ShaderProgram.h"
#include "Scene/Model3D.h"
#include "ThirdCamera.h"
#include "Avatar.h"

class Renderer
{


	GLuint programID;
	ShaderProgram shader;

	// Transformation
	GLuint MatrixID;
	GLuint ModelMatrixID;

	GLuint AmbientLightID;
	GLuint LightPositionID;
	GLuint EyePositionID;


	glm::vec3 ambientLight;
	glm::vec3 lightPosition;


	std::unique_ptr<Model> mySquare;
	glm::mat4 floorM;

	std::unique_ptr<Model> OriginAxis;
	glm::mat4 AxisMat;


	std::unique_ptr<EulerCamera> myCamera;
	std::unique_ptr<ThirdCamera> Camera;



	std::unique_ptr<Avatar> avatar;

	std::unique_ptr<Model> sq1;
	std::unique_ptr<Model> sq2;
	std::unique_ptr<Model> sq3;
	std::unique_ptr<Model> sq4;
	std::unique_ptr<Model> sq5;
	std::unique_ptr<Model> sq6;

	glm::mat4 m1;
	glm::mat4 m2;
	glm::mat4 m3;
	glm::mat4 m4;
	glm::mat4 m5;
	glm::mat4 m6;

	std::unique_ptr<Texture> t1;
	std::unique_ptr<Texture> t2;
	std::unique_ptr<Texture> t3;
	std::unique_ptr<Texture> t4;
	std::unique_ptr<Texture> t5;
	std::unique_ptr<Texture> t6;
	


	float dt;
	float upMovement = 0.3;

	int fir_Euer_Cam = 1;


public:
	Renderer();
	~Renderer();

	void Initialize();
	void Draw();
	void HandleKeyboardInput(int key);
	void HandleMouse(double deltaX,double deltaY);
	void Update(double deltaTime);
	void Cleanup();
	glm::vec3 cheack_coll(glm::vec3);
	void music();


	void initTransformation();
	void initShader();
	void initPointLight();

//	void CreateSquare();
	void RenderOrigin();
	void createSquare(std::unique_ptr<Model> &Square);

};

#endif // Renderer_h__

