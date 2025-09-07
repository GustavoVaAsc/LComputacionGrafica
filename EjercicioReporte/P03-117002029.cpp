//práctica 3: Modelado Geométrico y Cámara Sintética.
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<MeshColor*> meshColorList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks




void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}


void CrearPiramideTriangular()
{
	GLfloat h = sqrt(2.0f / 3.0f); // altura del tetraedro regular

	// Base: triángulo equilátero
	GLfloat V1[3] = { 0.0f, 0.0f,  sqrt(3.0f) / 3.0f };
	GLfloat V2[3] = { -0.5f, 0.0f, -sqrt(3.0f) / 6.0f };
	GLfloat V3[3] = { 0.5f, 0.0f, -sqrt(3.0f) / 6.0f };

	// Punta arriba
	GLfloat V4[3] = { 0.0f, h, 0.0f };

	GLfloat vertices_piramide[] = {
		// Cara 1
		V1[0], V1[1], V1[2], 0.686f, 0.239f, 1.0f,
		V2[0], V2[1], V2[2], 0.686f, 0.239f, 1.0f,
		V4[0], V4[1], V4[2], 0.686f, 0.239f, 1.0f,

		// Cara 2
		V2[0], V2[1], V2[2], 0.333f, 1.0f, 1.0f,
		V3[0], V3[1], V3[2], 0.333f, 1.0f, 1.0f,
		V4[0], V4[1], V4[2], 0.333f, 1.0f, 1.0f,

		// Cara 3
		V3[0], V3[1], V3[2], 1.0f, 0.231f, 0.580f,
		V1[0], V1[1], V1[2], 1.0f, 0.231f, 0.580f,
		V4[0], V4[1], V4[2], 1.0f, 0.231f, 0.580f,

		// Base
		V1[0], V1[1], V1[2], 0.650f, 0.992f, 0.161f,
		V2[0], V2[1], V2[2], 0.650f, 0.992f, 0.161f,
		V3[0], V3[1], V3[2], 0.650f, 0.992f, 0.161f,
	};

	MeshColor* obj1 = new MeshColor();
	obj1->CreateMeshColor(vertices_piramide, 12);
	meshColorList.push_back(obj1);

	GLfloat aristas_piramide[] = {
		V1[0], V1[1], V1[2], 0,0,0,   V2[0], V2[1], V2[2], 0,0,0,
		V2[0], V2[1], V2[2], 0,0,0,   V3[0], V3[1], V3[2], 0,0,0,
		V3[0], V3[1], V3[2], 0,0,0,   V1[0], V1[1], V1[2], 0,0,0,

		V1[0], V1[1], V1[2], 0,0,0,   V4[0], V4[1], V4[2], 0,0,0,
		V2[0], V2[1], V2[2], 0,0,0,   V4[0], V4[1], V4[2], 0,0,0,
		V3[0], V3[1], V3[2], 0,0,0,   V4[0], V4[1], V4[2], 0,0,0,
	};

	MeshColor* obj2 = new MeshColor();
	obj2->CreateMeshColor(aristas_piramide, 12);
	meshColorList.push_back(obj2);
}


/*
Crear cilindro, cono y esferas con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	glEnable(GL_DEPTH_TEST);
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(20, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();



	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.1f, 0.3f);


	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		uniformView = shaderList[1].getViewLocation();
		uniformColor = shaderList[1].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		

		// ------ CARA AZUL -------

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.3f, -2.5f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f); 
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f); 
		
		// Tetraedro inverso 

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.30f, 0.68f, -2.37f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		glm::quat q1 = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat q2 = glm::angleAxis(glm::radians(39.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat q = q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);
		
		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.6f, 0.3f, -2.5f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro inverso 

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.90f, 0.68f, -2.37f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));


		q1 = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(39.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		q = q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.2f, 0.3f, -2.5f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// SEGUNDA CAPA CARA AZUL
		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.30f, 0.78f, -2.32f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro inverso 

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.60f, 1.16f, -2.19f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		q1 = glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(39.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		q = q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.9f, 0.78f, -2.32f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// ULTIMA CAPA CARA AZUL

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.6f, 1.27f, -2.147f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);


		// CARA MORADA

		// Tetraedro inverso 

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.26f, 0.51f, -2.19f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		q1 = glm::angleAxis(glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(-51.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat q3 = glm::angleAxis(glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		q = q3 * q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.295f, 0.3f, -1.98f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro inverso

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.56f, 0.51f, -1.67f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		q1 = glm::angleAxis(glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(-51.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		q3 = glm::angleAxis(glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		q = q3 * q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.608f, 0.79f, -1.81f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);


		// Tetraedro inverso

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.56f, 1.00f, -2.03f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		q1 = glm::angleAxis(glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(-51.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		q3 = glm::angleAxis(glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		q = q3 * q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.598f, 0.3f, -1.45f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// CARA ROSA

		// Tetraedro normal

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.905f, 0.3f, -1.965f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro rotado

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.94f, 0.515f, -2.19f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		q1 = glm::angleAxis(glm::radians(-25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(-51.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		q3 = glm::angleAxis(glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		q = q3 * q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro rotado

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.635f, 0.525f, -1.67f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		q1 = glm::angleAxis(glm::radians(-25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(-51.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		q3 = glm::angleAxis(glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		q = q3 * q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro rotado

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.642f, 0.98f, -2.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));

		// Rotamos usando quaterniones
		q1 = glm::angleAxis(glm::radians(-25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		q2 = glm::angleAxis(glm::radians(-51.5f), glm::vec3(1.0f, 0.0f, 0.0f));
		q3 = glm::angleAxis(glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		q = q3 * q2 * q1;
		model *= glm::mat4_cast(q);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// CARA VERDE

		// Tetraedro inverso

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.3f, 0.295f, -2.32f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		q1 = glm::angleAxis(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model *= glm::mat4_cast(q1);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro inverso

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.90f, 0.295f, -2.32f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		q1 = glm::angleAxis(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model *= glm::mat4_cast(q1);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);

		// Tetraedro inverso

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.60f, 0.295f, -1.8f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		q1 = glm::angleAxis(glm::radians(-60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model *= glm::mat4_cast(q1);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshColorList[0]->RenderMeshColor();

		glLineWidth(9.0f);
		meshColorList[1]->RenderMeshLines();
		glLineWidth(1.0f);
		
		/*
		//ejercicio: Instanciar primitivas geométricas para recrear el dibujo de la práctica pasada en 3D,
		//se requiere que exista piso y la casa tiene una ventana azul circular justo en medio de la pared trasera y solo 1 puerta frontal.
		model = glm::mat4(1.0f);
		color=glm::vec3(0.0f,1.0f,0.0f);
		//Opcional duplicar esta traslación inicial para posicionar en -Z a los objetos en el mismo punto
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.75f, -2.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		meshList[1]->RenderMeshGeometry();
		*/

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}


