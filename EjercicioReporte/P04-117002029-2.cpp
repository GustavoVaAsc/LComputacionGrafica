/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
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
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
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

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
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

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(5, 1.0f);//índice 2 en MeshList
	CrearCono(25, 2.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 2.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.5f, 0.5f);


	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0); //Matriz de modelo principal
	glm::mat4 modelaux(1.0); //Matriz auxiliar para guardar estados de la jerarquía

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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		// Se envían las matrices de Proyección y Vista (una sola vez si no cambian)
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));



		// 1. TORSO (Raíz)

		glm::mat4 modelTorso = glm::mat4(1.0);
		modelTorso = glm::translate(modelTorso, glm::vec3(0.0f, 3.0f, 0.0f));

		model = glm::scale(modelTorso, glm::vec3(2.0f, 2.0f, 4.5f));

		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 1.5 CABEZA (prisma rectangular)
		glm::mat4 modelCabeza = glm::translate(modelTorso, glm::vec3(0.0f, 1.75f, 2.75f));
		model = glm::scale(modelCabeza, glm::vec3(1.8f, 1.75f, 2.0f)); // ancho, alto, largo
		color = glm::vec3(0.7f, 0.7f, 0.7f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 1.6 HOCICO 
		glm::mat4 modelHocico = glm::translate(modelCabeza, glm::vec3(0.0f, -0.25f, 1.5f));
		model = glm::scale(modelHocico, glm::vec3(1.2f, 1.0f, 1.5f));
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 1.6.1 NARIZ 
		glm::mat4 modelNariz = glm::translate(modelHocico, glm::vec3(0.0f, 0.25f, 0.75f));
		model = glm::scale(modelNariz, glm::vec3(0.3f, 0.2f, 0.3f)); // Esfera aplanada
		color = glm::vec3(0.1f, 0.1f, 0.1f); // Negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// 1.7 OREJA IZQUIERDA 
		glm::mat4 modelOrejaIzq = glm::translate(modelCabeza, glm::vec3(-0.7f, 0.75f, 0.0f));
		modelOrejaIzq = glm::rotate(modelOrejaIzq, glm::radians(25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(modelOrejaIzq, glm::vec3(0.5f, 2.5f, 1.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMeshGeometry();

		// 1.8 OREJA DERECHA
		glm::mat4 modelOrejaDer = glm::translate(modelCabeza, glm::vec3(0.7f, 0.75f, 0.0f));
		modelOrejaDer = glm::rotate(modelOrejaDer, glm::radians(-25.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(modelOrejaDer, glm::vec3(0.5f, 2.5f, 1.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMeshGeometry();


		// 1.9 PANTALLA
		glm::mat4 modelPantalla = glm::translate(modelCabeza, glm::vec3(0.0f, 0.4f, 1.01f)); 
		model = glm::scale(modelPantalla, glm::vec3(1.6f, 0.8f, 0.05f));
		color = glm::vec3(0.05f, 0.05f, 0.05f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 1.9.1 OJO IZQUIERDO 
		glm::mat4 modelOjoIzq = glm::translate(modelPantalla, glm::vec3(-0.4f, 0.15f, 0.03f));
		model = glm::scale(modelOjoIzq, glm::vec3(0.2f, 0.2f, 0.2f));
		color = glm::vec3(0.5f, 0.8f, 1.0f); 
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// 1.9.2 OJO DERECHO 
		glm::mat4 modelOjoDer = glm::translate(modelPantalla, glm::vec3(0.4f, 0.15f, 0.03f));
		model = glm::scale(modelOjoDer, glm::vec3(0.2f, 0.2f, 0.2f));
		// color ya está seteado a azul claro
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// --- INICIO DE PATAS Y COLA ---

		// 2. PATA DELANTERA IZQUIERDA
		// 2.1 Articulación Hombro
		// Partimos de la matriz de POSICIÓN del torso.
		glm::mat4 articSuperior1 = glm::translate(modelTorso, glm::vec3(-1.0f, -1.0f, 2.0f));
		articSuperior1 = glm::rotate(articSuperior1, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		// Aplicamos escalado solo para renderizar.
		model = glm::scale(articSuperior1, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// 2.2 Muslo
		glm::mat4 modelMuslo1 = glm::translate(articSuperior1, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelMuslo1, glm::vec3(0.4f, 1.5f, 0.4f));
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 2.3 Articulación Rodilla
		glm::mat4 articRodilla1 = glm::translate(modelMuslo1, glm::vec3(0.0f, -0.75f, 0.0f));
		articRodilla1 = glm::rotate(articRodilla1, glm::radians(mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articRodilla1, glm::vec3(0.3f, 0.3f, 0.3f));
		color = glm::vec3(1.0f, 0.8f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// 2.4 Canilla
		glm::mat4 modelCanilla1 = glm::translate(articRodilla1, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelCanilla1, glm::vec3(0.35f, 1.5f, 0.35f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 2.5 Pie
		glm::mat4 modelPie1 = glm::translate(modelCanilla1, glm::vec3(0.0f, -0.75f, 0.2f));
		model = glm::scale(modelPie1, glm::vec3(0.4f, 0.2f, 0.6f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		// 3. PATA DELANTERA DERECHA
		glm::mat4 articSuperior2 = glm::translate(modelTorso, glm::vec3(1.0f, -1.0f, 2.0f));
		articSuperior2 = glm::rotate(articSuperior2, glm::radians(mainWindow.getarticulacion2()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articSuperior2, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		glm::mat4 modelMuslo2 = glm::translate(articSuperior2, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelMuslo2, glm::vec3(0.4f, 1.5f, 0.4f));
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glm::mat4 articRodilla2 = glm::translate(modelMuslo2, glm::vec3(0.0f, -0.75f, 0.0f));
		articRodilla2 = glm::rotate(articRodilla2, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articRodilla2, glm::vec3(0.3f, 0.3f, 0.3f));
		color = glm::vec3(1.0f, 0.8f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		glm::mat4 modelCanilla2 = glm::translate(articRodilla2, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelCanilla2, glm::vec3(0.35f, 1.5f, 0.35f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glm::mat4 modelPie2 = glm::translate(modelCanilla2, glm::vec3(0.0f, -0.75f, 0.2f));
		model = glm::scale(modelPie2, glm::vec3(0.4f, 0.2f, 0.6f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 4. PATA TRASERA IZQUIERDA
		glm::mat4 articInferior1 = glm::translate(modelTorso, glm::vec3(-1.0f, -1.0f, -2.0f));
		articInferior1 = glm::rotate(articInferior1, glm::radians(mainWindow.getarticulacion3()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articInferior1, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		glm::mat4 modelMuslo3 = glm::translate(articInferior1, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelMuslo3, glm::vec3(0.4f, 1.5f, 0.4f));
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glm::mat4 articRodilla3 = glm::translate(modelMuslo3, glm::vec3(0.0f, -0.75f, 0.0f));
		articRodilla3 = glm::rotate(articRodilla3, glm::radians(mainWindow.getarticulacion7()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articRodilla3, glm::vec3(0.3f, 0.3f, 0.3f));
		color = glm::vec3(1.0f, 0.8f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		glm::mat4 modelCanilla3 = glm::translate(articRodilla3, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelCanilla3, glm::vec3(0.35f, 1.5f, 0.35f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glm::mat4 modelPie3 = glm::translate(modelCanilla3, glm::vec3(0.0f, -0.75f, 0.2f));
		model = glm::scale(modelPie3, glm::vec3(0.4f, 0.2f, 0.6f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// 5. PATA TRASERA DERECHA
		glm::mat4 articInferior2 = glm::translate(modelTorso, glm::vec3(1.0f, -1.0f, -2.0f));
		articInferior2 = glm::rotate(articInferior2, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articInferior2, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		glm::mat4 modelMuslo4 = glm::translate(articInferior2, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelMuslo4, glm::vec3(0.4f, 1.5f, 0.4f));
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glm::mat4 articRodilla4 = glm::translate(modelMuslo4, glm::vec3(0.0f, -0.75f, 0.0f));
		articRodilla4 = glm::rotate(articRodilla4, glm::radians(mainWindow.getarticulacion8()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articRodilla4, glm::vec3(0.3f, 0.3f, 0.3f));
		color = glm::vec3(1.0f, 0.8f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		glm::mat4 modelCanilla4 = glm::translate(articRodilla4, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(modelCanilla4, glm::vec3(0.35f, 1.5f, 0.35f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glm::mat4 modelPie4 = glm::translate(modelCanilla4, glm::vec3(0.0f, -0.75f, 0.2f));
		model = glm::scale(modelPie4, glm::vec3(0.4f, 0.2f, 0.6f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();


		// 6. COLA
		glm::mat4 articulacionCola = glm::translate(modelTorso, glm::vec3(0.0f, 0.5f, -2.25f));
		articulacionCola = glm::rotate(articulacionCola, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 1.0f, 0.0f));
		articulacionCola = glm::rotate(articulacionCola, glm::radians(mainWindow.getarticulacion10()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(articulacionCola, glm::vec3(0.3f, 0.3f, 0.3f));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		glm::mat4 modelCola = glm::translate(articulacionCola, glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale(modelCola, glm::vec3(0.3f, 0.3f, 3.5f));
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	
	}
	return 0;
}