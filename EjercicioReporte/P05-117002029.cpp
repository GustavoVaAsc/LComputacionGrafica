/*
Prctica 5: Optimizacin y Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Model Delorean_M;
Model Cofre;
Model Llanta1;
Model Llanta2;
Model Llanta3;
Model Llanta4;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";





void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 1.0f);

	Delorean_M = Model();
	Delorean_M.LoadModel("Models/cuerpo_delorean.fbx");
	Cofre = Model();
	Cofre.LoadModel("Models/cofre.fbx");
	Llanta1 = Model();
	Llanta1.LoadModel("Models/llanta_dd.fbx");
	Llanta2 = Model();
	Llanta2.LoadModel("Models/llanta_di.fbx");
	Llanta3 = Model();
	Llanta3.LoadModel("Models/llanta_td.fbx");
	Llanta4 = Model();
	Llanta4.LoadModel("Models/llanta_ti.fbx");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);


	glm::mat4 model(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	// VARIABLES PARA EL MOVIMIENTO DEL COCHE
	GLfloat movCoche = 0.0f;
	const float velocidadCoche = 1.0f;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// LÓGICA DEL MOVIMIENTO DEL COCHE 
		if (mainWindow.getsKeys()[GLFW_KEY_UP])
		{
			movCoche += velocidadCoche * deltaTime;
			mainWindow.setArticulacion2(mainWindow.getarticulacion2() + 10.0f);
			mainWindow.setArticulacion3(mainWindow.getarticulacion3() + 10.0f);
			mainWindow.setArticulacion4(mainWindow.getarticulacion4() + 10.0f);
			mainWindow.setArticulacion5(mainWindow.getarticulacion5() + 10.0f);
		}

		if (mainWindow.getsKeys()[GLFW_KEY_DOWN])
		{
			movCoche -= velocidadCoche * deltaTime;
			mainWindow.setArticulacion2(mainWindow.getarticulacion2() - 10.0f);
			mainWindow.setArticulacion3(mainWindow.getarticulacion3() - 10.0f);
			mainWindow.setArticulacion4(mainWindow.getarticulacion4() - 10.0f);
			mainWindow.setArticulacion5(mainWindow.getarticulacion5() - 10.0f);
		}


		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMesh();

		//------------*INICIA DIBUJO DE NUESTROS DEMÁS OBJETOS-------------------*

		glm::mat4 modelSinEscala = glm::mat4(1.0);
		modelSinEscala = glm::translate(modelSinEscala, glm::vec3(0.0f, 0.0f, movCoche)); // Aplicamos movimiento
		modelSinEscala = glm::translate(modelSinEscala, glm::vec3(0.0f, -0.1f, 0.0f));
		modelSinEscala = glm::rotate(modelSinEscala, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

		// --- Delorean (cuerpo principal con escala) ---
		color = glm::vec3(0.7f, 0.7f, 0.8f); 
		model = modelSinEscala; 
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f)); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Delorean_M.RenderModel(); 

		// Cofre
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 modelCofre = modelSinEscala; 
		modelCofre = glm::translate(modelCofre, glm::vec3(0.2f, -4.22f, 0.9f)); 
		modelCofre = glm::rotate(modelCofre, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelCofre = glm::scale(modelCofre, glm::vec3(1.2f, 1.2f, 1.2f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelCofre));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Cofre.RenderModel();

		// --- Llantas  ---
		color = glm::vec3(0.1f, 0.1f, 0.1f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Llanta Delantera Derecha (Llanta1)
		glm::mat4 modelLlanta1 = modelSinEscala;
		modelLlanta1 = glm::translate(modelLlanta1, glm::vec3(1.5f, -5.2f, -0.78f)); // Posición ajustada
		modelLlanta1 = glm::rotate(modelLlanta1, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelLlanta1 = glm::scale(modelLlanta1, glm::vec3(0.7f, 0.7f, 0.3f));
		modelLlanta1 = glm::rotate(modelLlanta1, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación manual
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelLlanta1));
		Llanta1.RenderModel();

		// Llanta Delantera Izquierda (Llanta2)
		glm::mat4 modelLlanta2 = modelSinEscala;
		modelLlanta2 = glm::translate(modelLlanta2, glm::vec3(-1.5f, -5.2f, -0.78f)); // Posición ajustada
		modelLlanta2 = glm::rotate(modelLlanta2, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelLlanta2 = glm::scale(modelLlanta2, glm::vec3(0.7f, 0.7f, 0.3f));
		modelLlanta2 = glm::rotate(modelLlanta2, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación manual
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelLlanta2));
		Llanta2.RenderModel();

		// Llanta Trasera Derecha (Llanta3)
		glm::mat4 modelLlanta3 = modelSinEscala;
		modelLlanta3 = glm::translate(modelLlanta3, glm::vec3(1.26f, 3.7f, -0.48f)); // Posición ajustada
		modelLlanta3 = glm::rotate(modelLlanta3, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelLlanta3 = glm::scale(modelLlanta3, glm::vec3(0.8f, 0.8f, 0.4f));
		modelLlanta3 = glm::rotate(modelLlanta3, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación manual
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelLlanta3));
		Llanta3.RenderModel();

		// Llanta Trasera Izquierda (Llanta4)
		glm::mat4 modelLlanta4 = modelSinEscala;
		modelLlanta4 = glm::translate(modelLlanta4, glm::vec3(-1.26f, 3.7f, -0.48f)); // Posición ajustada
		modelLlanta4 = glm::rotate(modelLlanta4, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelLlanta4 = glm::scale(modelLlanta4, glm::vec3(0.8f, 0.8f, 0.4f));
		modelLlanta4 = glm::rotate(modelLlanta4, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación manual
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelLlanta4));
		Llanta4.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}