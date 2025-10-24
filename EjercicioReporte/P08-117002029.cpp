/*
Práctica 7: Iluminación 1
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
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoOchoTexture;

Model Kitt_M;
Model Llanta_1;
Model Llanta_2;
Model Blackhawk_M;
Model Fogata;
Model Cofre;
Model Cuphead;
Model Estrella;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
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

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f

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

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	unsigned int octaedroIndices[] = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,

		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23
	};

	GLfloat octaedroVertices[] = {
		0.0f, 1.0f, 0.0f,		0.5f, 0.25f,		-0.577f, -0.577f, -0.577f,
		0.0f, 0.0f, 1.0f,		0.67f, 0.5f,		-0.577f, -0.577f, -0.577f,
		1.0f, 0.0f, 0.0f,		0.34f, 0.5f,		-0.577f, -0.577f, -0.577f,

		0.0f, 1.0f, 0.0f,		0.5f, 0.25f,		-0.577f, -0.577f, 0.577f,
		1.0f, 0.0f, 0.0f,		0.25f, 0.5f,		-0.577f, -0.577f, 0.577f,
		0.0f, 0.0f, -1.0f,		0.0f, 0.25f,		-0.577f, -0.577f, 0.577f,

		0.0f, 1.0f, 0.0f,		0.5f, 0.25f,		0.577f, -0.577f, 0.577f,
		0.0f, 0.0f, -1.0f,		1.0f, 0.25f,		0.577f, -0.577f, 0.577f,
		-1.0f, 0.0f, 0.0f,		0.75f, 0.0f,		0.577f, -0.577f, 0.577f,

		0.0f, 1.0f, 0.0f,		0.5f, 0.25f,		0.577f, -0.577f, -0.577f,
		-1.0f, 0.0f, 0.0f,		1.0f, 0.25f,		0.577f, -0.577f, -0.577f,
		0.0f, 0.0f, 1.0f,		0.75f, 0.5f,		0.577f, -0.577f, -0.577f,

		0.0f, -1.0f, 0.0f,		0.5f, 0.75f,		-0.577f, 0.577f, -0.577f,
		1.0f, 0.0f, 0.0f,		0.25f, 0.5f,		-0.577f, 0.577f, -0.577f,
		0.0f, 0.0f, 1.0f,		0.75f, 0.5f,		-0.577f, 0.577f, -0.577f,

		0.0f, -1.0f, 0.0f,		0.5f, 0.75f,		-0.577f, 0.577f, 0.577f,
		0.0f, 0.0f, -1.0f,		0.0f, 0.75f,		-0.577f, 0.577f, 0.577f,
		1.0f, 0.0f, 0.0f,		0.25f, 0.5f,		-0.577f, 0.577f, 0.577f,

		0.0f, -1.0f, 0.0f,		0.5f, 0.75f,		0.577f, 0.577f, 0.577f,
		-1.0f, 0.0f, 0.0f,		1.0f, 0.75f,		0.577f, 0.577f, 0.577f,
		0.0f, 0.0f, -1.0f,		0.75f, 1.0f,		0.577f, 0.577f, 0.577f,

		0.0f, -1.0f, 0.0f,		0.5f, 0.75f,		0.577f, 0.577f, -0.577f,
		0.0f, 0.0f, 1.0f,		0.75f, 0.5f,		0.577f, 0.577f, -0.577f,
		-1.0f, 0.0f, 0.0f,		1.0f, 0.75f,		0.577f, 0.577f, -0.577f
	};

	//calcAverageNormals(octaedroIndices, 24, octaedroVertices, 192, 8, 5);

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(octaedroVertices, octaedroIndices, 192, 24);
	meshList.push_back(octaedro);

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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	dadoOchoTexture = Texture("Textures/dado_8.png");
	dadoOchoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/delorean_texturizado.dae");
	Llanta_1 = Model();
	Llanta_1.LoadModel("Models/llanta_1.dae");
	Llanta_2 = Model();
	Llanta_2.LoadModel("Models/llanta_2.dae");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Fogata = Model();
	Fogata.LoadModel("Models/fogata.obj");
	Cofre = Model();
	Cofre.LoadModel("Models/cofre_texturizado.dae");
	Cuphead = Model();
	Cuphead.LoadModel("Models/cuphead.dae");
	Estrella = Model();
	Estrella.LoadModel("Models/estrella.dae");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;


	pointLights[1] = PointLight(
		1.0f, 1.0f, 1.0f,
		4.0f, 5.5f,
		7.0f, 0.5f, 10.0f,
		1.0f, 0.05f, 0.022f
	);
	pointLightCount++;

	// PointLight para la estrella (amarilla)
	pointLights[2] = PointLight(
		1.0f, 1.0f, 0.0f,  
		2.0f, 3.0f,
		-20.0f, 3.0f, -15.0f,
		1.0f, 0.09f, 0.032f
	);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	// Ahora la spotlight 1 es la del cofre (El helicóptero ya no tiene spotlight)
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		-5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.005f,
		25.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		-5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// Luz trasera del carro
	spotLights[3] = spotLights[2];
	spotLightCount++;

	SpotLight temp = SpotLight();
	PointLight tempPL = PointLight();

	//se crean mas luces puntuales y spotlight 

	glm::vec3 fogataPosition(7.0f, -0.85f, 10.0f);
	glm::vec3 fogataLightPosition;

	glm::vec3 carPosition;
	glm::vec3 carFrontLight;
	glm::vec3 carDirection(1.0f, 0.0f, 0.0f);
	glm::vec3 carBackLight;
	glm::vec3 carDirectionForward;
	glm::vec3 carDirectionBackward;

	glm::vec3 cofrePosition;
	glm::vec3 cofreFrontLight;
	glm::vec3 cofreDirection(1.0f, 0.0f, 0.0f);
	glm::vec3 cofreOffset; // Posición del frente del cofre relativa al carro
	glm::vec3 lightOffsetLocal; // Offset desde el pivote del cofre hasta el frente
	glm::vec3 lightOffsetRotated; // Offset rotado según el ángulo del cofre
	glm::vec3 cofreDirectionRotated; // Dirección de la luz rotada según el ángulo del cofre
	
	glm::vec3 estrellaPosition(-20.0f, 3.0f, -15.0f);
	glm::vec3 estrellaLightPosition;
	
	float contador = 0;
	float diff = 0.0006;
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
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

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// luz ligada al frente del carro
		carPosition = glm::vec3(0.0f + mainWindow.getmuevex(), -1.2f, -3.0f);
		
		// Posición de la luz delantera 
		carFrontLight = carPosition;
		carFrontLight.x += 8.55f; // Posición delantera
		carFrontLight.y += 0.5f;
		
		// Posición de la luz trasera 
		carBackLight = carPosition;
		carBackLight.x += -6.15f; // Posición trasera
		carBackLight.y += 0.5f;
		
		carDirectionForward = glm::vec3(1.0f, 0.0f, 0.0f);
		carDirectionBackward = glm::vec3(-1.0f, 0.0f, 0.0f);
		
		// Siempre actualizar ambas luces
		spotLights[2].SetFlash(carFrontLight, carDirectionForward);
		spotLights[3].SetFlash(carBackLight, carDirectionBackward);

		// luz ligada al frente del cofre
		cofrePosition = carPosition; // El cofre es hijo del carro
		
		// Calcular la posición del frente del cofre con la rotación aplicada
		float cofreAngle = mainWindow.getMueveCofre() * toRadians;
		
		cofreOffset = glm::vec3(6.2f, 1.75f, 0.0f);
		
		lightOffsetLocal = glm::vec3(2.0f, 0.0f, 0.0f);
		
		// Rotar el offset de la luz según el ángulo de apertura del cofre
		lightOffsetRotated.x = lightOffsetLocal.x * cos(cofreAngle) - lightOffsetLocal.y * sin(cofreAngle);
		lightOffsetRotated.y = lightOffsetLocal.x * sin(cofreAngle) + lightOffsetLocal.y * cos(cofreAngle);
		lightOffsetRotated.z = 0.0f;
		
		// Posición final de la luz del cofre
		cofreFrontLight = cofrePosition + cofreOffset + lightOffsetRotated;
		
		cofreDirectionRotated.x = cofreDirection.x * cos(cofreAngle) - cofreDirection.y * sin(cofreAngle);
		cofreDirectionRotated.y = cofreDirection.x * sin(cofreAngle) + cofreDirection.y * cos(cofreAngle);
		cofreDirectionRotated.z = 0.0f;
		
		// Actualizar la luz del cofre
		spotLights[1].SetFlash(cofreFrontLight, cofreDirectionRotated);

		// Variación de intensidad del fuego (fogata)

		if (contador >= 3.0f) // intensidad máxima
		{
			diff = -0.0006f;
		}
		else {
			if (contador <= 0.2f) { // intensidad mínima
				diff = 0.0006f;
			}
		}

		contador += diff;
		//printf("Contador: %f \n", contador);

		pointLights[1].variateIntensity(diff);

		// Actualizar posición de la luz de la fogata
		fogataLightPosition = fogataPosition;
		fogataLightPosition.y += 0.5f;
		pointLights[1].SetPos(fogataLightPosition);

		// Actualizar posición de la luz de la estrella
		estrellaLightPosition = estrellaPosition;
		pointLights[2].SetPos(estrellaLightPosition);

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		if (mainWindow.getFogataEncendida() and mainWindow.getEstrellaEncendida()){
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}else if (mainWindow.getFogataEncendida() and !(mainWindow.getEstrellaEncendida())){
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}else if (!(mainWindow.getFogataEncendida()) and mainWindow.getEstrellaEncendida()){
			// Hacemos swap entre pointLights[1] y pointLights[2]
			tempPL = pointLights[1];
			pointLights[1] = pointLights[2];
			pointLights[2] = tempPL;
			
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
			
			// Restauramos el orden original
			tempPL = pointLights[1];
			pointLights[1] = pointLights[2];
			pointLights[2] = tempPL;
		}
		else {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 2);
		}
			
		if (mainWindow.getAdelante()) {
			shaderList[0].SetSpotLights(spotLights, 3);
		} else {
			// Hacemos swap entre spotLights[2] y spotLights[3]
			temp = spotLights[2];
			spotLights[2] = spotLights[3];
			spotLights[3] = temp;
			
			shaderList[0].SetSpotLights(spotLights, 3);
			
			// Restauramoso el orden original
			temp = spotLights[2];
			spotLights[2] = spotLights[3];
			spotLights[3] = temp;
		}

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		// Dado 8 caras
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-28.0f, 3.0f, -10.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		dadoOchoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 1.3f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		// Cofre del carro
		model = modelaux;
		model = glm::translate(model, glm::vec3(6.2f, 1.75f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, toRadians * mainWindow.getMueveCofre(), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre.RenderModel();


		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(8.55f, -0.85f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.55f, 1.55f, 1.55f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_2.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-6.15f, -0.65f, 5.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.85f, 1.85f, 1.85f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_1.RenderModel();

		//Llanta delantera derecha 
		model = modelaux;
		model = glm::translate(model, glm::vec3(8.55f, -0.85f, -5.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.55f, 1.55f, 1.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_2.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-6.15f, -0.65f, -5.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.85f, 1.85f, 1.85f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex2(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(7.0f, -0.85f, 10.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Fogata.RenderModel();

		// Cuphead 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(22.0f, 5.0f, -3.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cuphead.RenderModel();

		// Estrella
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, 3.0f, -15.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); 
		model = glm::rotate(model, now * 50.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); 
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Estrella.RenderModel();


		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
