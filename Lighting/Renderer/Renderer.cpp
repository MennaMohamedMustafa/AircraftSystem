#include "Renderer.h"

#include <gtc/matrix_transform.hpp> 
#include <gtx/transform.hpp>
#include "OBJLoader/objloader.hpp"
#include<iostream>
using namespace std;


Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	Cleanup();
}
void music()
{
	
}
void Renderer::Initialize()
{


	//set camera pos and view point
	Camera = std::unique_ptr<ThirdCamera>(new ThirdCamera());

	avatar  =  std::unique_ptr<Avatar>(new Avatar(glm::vec3(0,0,0),glm::vec3(0,0,1))); //load the model 
	Camera->UpdatePosition(avatar->position, avatar->orintation);

	myCamera =  std::unique_ptr<EulerCamera>(new EulerCamera());            //////////////////?

	myCamera->SetPerspectiveProjection(90.0f, 4.0f / 3.0f, 0.1f, 99999999.0f);
	myCamera->Reset(0.0, 1.0, 5.0,
		0, 0, 0,
		0, 1, 0);
	//	CreateSquare();
	RenderOrigin();
	createSquare(sq1);//floor
	m1 = glm::scale(100.0f, 100.0f, 100.0f)*glm::translate(0.0f, -0.01f, 0.0f)*glm::rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	createSquare(sq2);//sky
	m2 = glm::scale(100.0f, 1.0f, 100.0f)*glm::translate(0.0f, 50.0f, 0.0f)*glm::rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	createSquare(sq3);//front
	m3 = glm::scale(100.0f, 100.0f, 1.0f) *glm::translate(0.0f, 0.1f, -100.0f)*glm::rotate(180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	createSquare(sq4);//right
	m4 = glm::scale(1.0f, 100.0f, 100.0f) *glm::translate(-100.0f, 0.1f, -0.1f)*glm::rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	createSquare(sq5);//left
	m5 = glm::scale(1.0f, 100.0f, 100.0f) *glm::translate(100.0f, 0.1f, -0.1f)*glm::rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	createSquare(sq6);//back
	m6 = glm::scale(100.0f, 100.0f, 1.0f) *glm::translate(0.01f, 0.1f, 100.0f)*glm::rotate(180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	//load of images (textures)
	t1 = std::unique_ptr<Texture>(new Texture("skybox/floor.png", 0)); 
	t2 = std::unique_ptr<Texture>(new Texture("skybox/side2.png", 1));
	t3 = std::unique_ptr<Texture>(new Texture("skybox/side3.png", 2));
	t4 = std::unique_ptr<Texture>(new Texture("skybox/side4.png", 3));
	t5 = std::unique_ptr<Texture>(new Texture("skybox/side5.png", 4));
	t6 = std::unique_ptr<Texture>(new Texture("skybox/side6.png", 5));

	initShader();

	initPointLight();

	initTransformation();
}


void Renderer::Update(double deltaTime)
{
	dt = deltaTime /  10;
	Camera->Look();
	//update the eye position uniform.
	glUniform3fv(EyePositionID,1, &Camera->mvPosition[0]);
}

void Renderer::Draw()
{		
	glClearColor(1.0,1.0,1.0,1.0f);
	shader.UseProgram();

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
//glm::mat4 VP = Camera->Proj * Camera->View;
	glm::mat4 VP;

	if (fir_Euer_Cam == 1)
	{
		VP = myCamera->GetProjectionMatrix() * myCamera->GetViewMatrix();
	}
	else
	{
		VP = Camera->Proj*Camera->View;
	}

	shader.BindVPMatrix(&VP[0][0]);


	//the floor
	//we need to send the model matrix to transform the normals too.
	//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &floorM[0][0]);
//	shader.BindModelMatrix(&floorM[0][0]);
	//mySquare->Draw();


	//Origin Axis
	shader.BindModelMatrix(&AxisMat[0][0]);
	OriginAxis->DrawAxis();


	avatar->Draw(shader);
	//skybox
	t1->Bind();//floor
	shader.BindModelMatrix(&m1[0][0]);
	sq1->Draw();

	t6->Bind();//sky
	shader.BindModelMatrix(&m2[0][0]);
	sq2->Draw();

	t3->Bind(); //front
	shader.BindModelMatrix(&m3[0][0]);
	sq3->Draw();

	t2->Bind();//right
	shader.BindModelMatrix(&m4[0][0]);
	sq4->Draw();

	t4->Bind(); //left
	shader.BindModelMatrix(&m5[0][0]);
	sq5->Draw();

	t5->Bind();
	shader.BindModelMatrix(&m6[0][0]);
	sq6->Draw();





}

void Renderer::Cleanup()
{
	glDeleteProgram(programID);
}

glm::vec3 Renderer::cheack_coll(glm::vec3 pos)
{
	float x = 95;
	if (pos.x >= x)
		pos.x = x - 1;

	if (pos.x <= -x)
		pos.x = -x + 1;

	if (pos.y >= x)
		pos.y = x - 1;

	if (pos.y < 0)
		pos.y = 0;

	if (pos.z >= x)
		pos.z = x - 1;
	
	if (pos.z <= -x)
		pos.z = -x + 1;
	return pos;
}

void Renderer::HandleKeyboardInput(int key)
{
	const float kRotateSpeed = 0.5f;
	const float kMoveSpeed = 0.5f;

	float fRotation = 0.0f;
	glm::vec3 vMovement = glm::vec3(0,0,0);
	glm::vec3 vDirection = avatar->orintation;	
	float	kAvatarMovement = 1.0f;
	float	kAvatarRotation = 0.5f;

/*	cout << "x"<<Camera->mvPosition.x << endl;
	cout << "y"<<Camera->mvPosition.y << endl;
	cout << "Z"<<Camera->mvPosition.z << endl;*/

	switch (key)
	{
		//Moving forward
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
		vMovement += vDirection* (kAvatarMovement);
		break;

		//Moving backword
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
		vMovement -= vDirection* (kAvatarMovement);
		break;

		// Moving right
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
		fRotation -= kAvatarRotation;
		break;

		// Moving left
	case GLFW_KEY_LEFT:
	case GLFW_KEY_A:
		fRotation += kAvatarRotation;
		break;

		// Moving up
	case GLFW_KEY_I:
		Camera->mfDistance -= kMoveSpeed;
		Camera->UpdatePosition(avatar->position, avatar->orintation);
		break;

	case GLFW_KEY_K:
		Camera->mfDistance += kMoveSpeed;
		Camera->UpdatePosition(avatar->position, avatar->orintation);
		break;
	case GLFW_KEY_J:
		Camera->mfAzimuth += kRotateSpeed;
		Camera->UpdatePosition(avatar->position, avatar->orintation);
		break;
	case GLFW_KEY_L:
		Camera->mfAzimuth -= kRotateSpeed;
		Camera->UpdatePosition(avatar->position, avatar->orintation);
		break;
	case GLFW_KEY_Z:
		upMovement = 0.3;
		break;
	case GLFW_KEY_X:
		upMovement = -0.3;
		break;
	case GLFW_KEY_9:
		fir_Euer_Cam = 1;
		break;
	case GLFW_KEY_0:
		fir_Euer_Cam = 2;
		break;
	default:
		break;
	
	
	}
		// Rotate the avatar if necessary.
	if (fRotation != 0.0f)
	{
		// Rotate the Orientation Vector of the avatar to face the correct direction.
		avatar->RotateVectorArbitrary(avatar->orintation , glm::vec3(0,0,0), fRotation, glm::vec3(0,1,0));
		avatar->orintation = glm::normalize(avatar->orintation);
		avatar->UpdateMatrix();
		Camera->UpdatePosition(avatar->position, avatar->orintation);		
	}
	// Move the avatar if necessary.
	if (vMovement != glm::vec3(0,0,0))
	{
		vMovement.y = upMovement;////flyig
		avatar->position += vMovement;
		avatar->position=cheack_coll(avatar->position);
		avatar->UpdateMatrix();
		Camera->UpdatePosition(avatar->position, avatar->orintation);	
	}
}

void Renderer::HandleMouse(double deltaX,double deltaY)
{	
	myCamera->Yaw(deltaX);
	myCamera->Pitch(deltaY);
	myCamera->UpdateViewMatrix();
	//update the eye position uniform.
	glUniform3fv(EyePositionID,1, &myCamera->GetEyePosition()[0]);
}


void Renderer::initTransformation()
{
	//send the eye position to the shaders.
	glUniform3fv(EyePositionID,1, &Camera->mvPosition[0]);
	//////////////////////////////////////////////////////////////////////////

}

void Renderer::initShader()
{
	// Create and compile our GLSL program from the shaders
	//programID = LoadShaders( "SimpleTransformWithColor.vertexshader", "MultiColor.fragmentshader" );
	shader.LoadProgram();

	MatrixID = glGetUniformLocation(shader.programID, "MVP");
	ModelMatrixID = glGetUniformLocation(shader.programID, "ModelMatrix");

	// Use our shader
	//glUseProgram(programID);
	shader.UseProgram();
}

void Renderer::initPointLight()
{

	//////////////////////////////////////////////////////////////////////////
	// Configure the light.

	//setup the light position.
	LightPositionID = glGetUniformLocation(shader.programID,"LightPosition_worldspace");

	lightPosition = glm::vec3(0.0,0.5,0.0);

	glUniform3fv(LightPositionID,1, &lightPosition[0]);

	//setup the ambient light component.
	AmbientLightID = glGetUniformLocation(shader.programID,"ambientLight");
	ambientLight = glm::vec3(0.1,0.1,0.1);
	glUniform3fv(AmbientLightID,1, &ambientLight[0]);

	//setup the eye position.
	EyePositionID = glGetUniformLocation(shader.programID,"EyePosition_worldspace");
	//////////////////////////////////////////////////////////////////////////

}

void Renderer::createSquare(std::unique_ptr<Model> &Square)
{

	//drawing a square.
	Square = std::unique_ptr<Model>(new Model());

	Square->VertexData.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	Square->VertexData.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	Square->VertexData.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	Square->VertexData.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));

	Square->ColorData.push_back(glm::vec3(1.0f, 0.0f, 0.5f));
	Square->ColorData.push_back(glm::vec3(0.0f, 0.0f, 0.5f));
	Square->ColorData.push_back(glm::vec3(0.0f, 0.0f, 0.5f));
	Square->ColorData.push_back(glm::vec3(1.0f, 0.0f, 0.5f));

	Square->UVData.push_back(glm::vec2(0.0f, 0.0f));
	Square->UVData.push_back(glm::vec2(1.0f, 0.0f));
	Square->UVData.push_back(glm::vec2(0.0f, 1.0f));
	Square->UVData.push_back(glm::vec2(1.0f, 1.0f));

	//first triangle.
	Square->IndicesData.push_back(0);
	Square->IndicesData.push_back(1);
	Square->IndicesData.push_back(3);

	//second triangle.
	Square->IndicesData.push_back(1);
	Square->IndicesData.push_back(2);
	Square->IndicesData.push_back(3);

	Square->Initialize();

}




void Renderer::RenderOrigin()
{

	//drawing a square.
	OriginAxis = std::unique_ptr<Model>(new Model());

	OriginAxis->VertexData.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	OriginAxis->VertexData.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

	OriginAxis->VertexData.push_back(glm::vec3( 0.0f,  1.0f, 0.0f));
	OriginAxis->VertexData.push_back(glm::vec3( 0.0f,  0.0f, 1.0f));


	OriginAxis->ColorData.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

	//first line.
	OriginAxis->IndicesData.push_back(0);
	OriginAxis->IndicesData.push_back(1);

	//second line.
	OriginAxis->IndicesData.push_back(0);
	OriginAxis->IndicesData.push_back(2);

	//third line.
	OriginAxis->IndicesData.push_back(0);
	OriginAxis->IndicesData.push_back(3);

	OriginAxis->Initialize();

	AxisMat = glm::scale(200,100,200);
}