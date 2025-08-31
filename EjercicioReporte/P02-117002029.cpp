//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
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
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";
//shaders nuevos se crearían acá

float angulo = 0.0f;

//color café en RGB : 0.478, 0.255, 0.067

// Función para agregar un cuadrado RGB
void agregarCuadradoRGB(GLfloat*& vertices, int& numVertices, float x, float y, float r, float g, float b, float tam){
	int nuevosVertices = 6;
	int totalNuevos = nuevosVertices * 6;

	GLfloat* temp = new GLfloat[numVertices + totalNuevos];

	for (int i = 0; i < numVertices; i++)
		temp[i] = vertices[i];

	float mitad = tam / 2.0f;
	int idx = numVertices;

	float x0 = x - mitad, x1 = x + mitad;
	float y0 = y - mitad, y1 = y + mitad;
	float z = 0.0f;

	temp[idx++] = x0; temp[idx++] = y0; temp[idx++] = z; temp[idx++] = r; temp[idx++] = g; temp[idx++] = b;
	temp[idx++] = x1; temp[idx++] = y0; temp[idx++] = z; temp[idx++] = r; temp[idx++] = g; temp[idx++] = b;
	temp[idx++] = x1; temp[idx++] = y1; temp[idx++] = z; temp[idx++] = r; temp[idx++] = g; temp[idx++] = b;

	temp[idx++] = x0; temp[idx++] = y0; temp[idx++] = z; temp[idx++] = r; temp[idx++] = g; temp[idx++] = b;
	temp[idx++] = x1; temp[idx++] = y1; temp[idx++] = z; temp[idx++] = r; temp[idx++] = g; temp[idx++] = b;
	temp[idx++] = x0; temp[idx++] = y1; temp[idx++] = z; temp[idx++] = r; temp[idx++] = g; temp[idx++] = b;

	delete[] vertices;

	vertices = temp;
	numVertices += totalNuevos;
}




//Pirámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3

	};
	GLfloat vertices[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

//Vértices de un cubo
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

void CrearLetrasyFiguras()
{
	GLfloat *vertices_letras = nullptr;

	int numVertices = 0;
	float tam = 0.05f;
	float r = 0.8f, g = 0.0f, b = 0.0f;

	// Rectángulo horizontal superior
	agregarCuadradoRGB(vertices_letras, numVertices, -0.8f, 0.5f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.75f, 0.5f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.70f, 0.5f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.65f, 0.5f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.60f, 0.5f, r, g, b, tam);

	// Rectángulo vertical izquierdo
	agregarCuadradoRGB(vertices_letras, numVertices, -0.8f, 0.45f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.8f, 0.40f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.8f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.8f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.8f, 0.25f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.8f, 0.20f, r, g, b, tam);

	// Rectángulo horizontal inferior
	agregarCuadradoRGB(vertices_letras, numVertices, -0.60f, 0.2f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.65f, 0.2f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.70f, 0.2f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.75f, 0.2f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.80f, 0.2f, r, g, b, tam);

	// Rectángulo vertical derecho
	agregarCuadradoRGB(vertices_letras, numVertices, -0.6f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.6f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.6f, 0.25f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.6f, 0.20f, r, g, b, tam);

	// Rectángulo horizontal medio
	agregarCuadradoRGB(vertices_letras, numVertices, -0.60f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.65f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.70f, 0.35f, r, g, b, tam);

	// V 

	r = 0.0f, g = 1.0f, b = 0.5f;

	// Rectángulo vertical izquierdo
	agregarCuadradoRGB(vertices_letras, numVertices, -0.20f, 0.5f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.20f, 0.45f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.20f, 0.40f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.20f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.20f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.15f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.15f, 0.25f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.10f, 0.20f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, -0.05f, 0.20f, r, g, b, tam);

	// Rectángulo derecho de la V
	agregarCuadradoRGB(vertices_letras, numVertices, 0.00f, 0.25f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.00f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.05f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.05f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.05f, 0.40f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.05f, 0.45f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.05f, 0.50f, r, g, b, tam);

	// A

	r = 0.5f, g = 0.0f, b = 0.5f;

	// Rectángulo izquierdo
	agregarCuadradoRGB(vertices_letras, numVertices, 0.50f, 0.50f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.50f, 0.45f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.50f, 0.40f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.50f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.50f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.50f, 0.25f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.50f, 0.20f, r, g, b, tam);

	// Rectángulo derecho
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.50f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.45f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.40f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.30f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.25f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.20f, r, g, b, tam);

	// Línea horizontal superior
	agregarCuadradoRGB(vertices_letras, numVertices, 0.55f, 0.50f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.60f, 0.50f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.65f, 0.50f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.50f, r, g, b, tam);

	// Línea media horizontal 
	agregarCuadradoRGB(vertices_letras, numVertices, 0.55f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.60f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.65f, 0.35f, r, g, b, tam);
	agregarCuadradoRGB(vertices_letras, numVertices, 0.70f, 0.35f, r, g, b, tam);

	MeshColor* letras = new MeshColor();
	letras->CreateMeshColor(vertices_letras, numVertices);
	meshColorList.push_back(letras);
}

void CreateShaders()
{

	Shader* shader1 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();//shader para usar color como parte del VAO: letras 
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);
}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	CreaPiramide(); //índice 0 en MeshList
	CrearCubo();//índice 1 en MeshList
	CrearLetrasyFiguras(); //usa MeshColor, índices en MeshColorList
	CreateShaders();
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformColor = 0;
	//Projection: Matriz de Dimensión 4x4 para indicar si vemos en 2D( orthogonal) o en 3D) perspectiva
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 100.0f);
	//glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0); //fuera del while se usa para inicializar la matriz con una identidad

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// Dibujar letras
		shaderList[1].useShader();
		GLuint uniformModel = shaderList[1].getModelLocation();
		GLuint uniformProjection = shaderList[1].getProjectLocation();
		glm::mat4 modelLetras = glm::mat4(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelLetras));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshColorList[0]->RenderMeshColor(); 
		
		/*
		// Dibujar casa con cubos y pirámides
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformColor = shaderList[0].getUniformLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// Cubo rojo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 1.0f, 0.0f, 0.0f);
		meshList[1]->RenderMesh();

		// Pirámide azul
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.85f, -3.0f));
		model = glm::scale(model, glm::vec3(0.87f, 0.87f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.0f, 0.0f, 1.0f);
		meshList[0]->RenderMesh();

		// Puerta 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.235f, -2.9f));
		model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.0f, 1.0f, 0.0f);
		meshList[1]->RenderMesh();

		// Ventana izquierda 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.20f, 0.2f, -2.9f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.0f, 1.0f, 0.0f);
		meshList[1]->RenderMesh();

		// Ventana derecha 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.20f, 0.2f, -2.9f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.0f, 1.0f, 0.0f);
		meshList[1]->RenderMesh();

		// Tronco izquierdo
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.85f, -0.235f, -2.9f));
		model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.478f, 0.255f, 0.067f);
		meshList[1]->RenderMesh();

		// Tronco derecho
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.85f, -0.235f, -2.9f));
		model = glm::scale(model, glm::vec3(0.2f, 0.3f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.478f, 0.255f, 0.067f);
		meshList[1]->RenderMesh();
		
		// Copa izquierda
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.88f, 0.21f, -3.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.0f, 0.5f, 0.0f);
		meshList[0]->RenderMesh();
		// Copa derecha

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.88f, 0.21f, -3.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(uniformColor, 0.0f, 0.5f, 0.0f);
		meshList[0]->RenderMesh();
		*/
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/