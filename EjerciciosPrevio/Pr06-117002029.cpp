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
Texture TexturaMadera;
Texture TexturaVidrio;
Texture TexturaLetrero;

Model Kitt_M;
Model Llanta_1;
Model Llanta_2;
Model Blackhawk_M;


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
PointLight pointLights2[MAX_POINT_LIGHTS];
PointLight* currentPointLights = pointLights; // Por defecto elegimos el primero
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//funci�n para seleccionar el arreglo de luces puntuales a usar
void selectLightArray(int selection){
	if(selection == 1){
		currentPointLights = pointLights;
	}
	else{
		currentPointLights = pointLights2;
	}
}

//funci�n de calculo de normales por promedio de v�rtices 
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
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};


	const int segmentos = 20; 
	const float anchoPuerta = 2.0f;
	const float alturaRectangular = 3.0f;
	const float profundidadPuerta = 0.2f;
	const float radioSemicirculo = anchoPuerta / 2.0f;
	
	std::vector<GLfloat> verticesPuerta;
	std::vector<unsigned int> indicesPuerta;
	unsigned int indicVertice = 0;

	// Cara frontal del rectángulo (z = profundidadPuerta/2)
	// Inferior-izquierda
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, 0.0f, profundidadPuerta/2, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f});
	// Inferior-derecha
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, 0.0f, profundidadPuerta/2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f});
	// Superior-derecha
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, alturaRectangular, profundidadPuerta/2, 1.0f, 0.75f, 0.0f, 0.0f, 1.0f});
	// Superior-izquierda
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, alturaRectangular, profundidadPuerta/2, 0.0f, 0.75f, 0.0f, 0.0f, 1.0f});
	indicesPuerta.insert(indicesPuerta.end(), {0, 1, 2, 0, 2, 3});
	indicVertice += 4;

	// Cara trasera del rectángulo (z = -profundidadPuerta/2)
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, 0.0f, -profundidadPuerta/2, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, 0.0f, -profundidadPuerta/2, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, alturaRectangular, -profundidadPuerta/2, 0.0f, 0.75f, 0.0f, 0.0f, -1.0f});
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, alturaRectangular, -profundidadPuerta/2, 1.0f, 0.75f, 0.0f, 0.0f, -1.0f});
	indicesPuerta.insert(indicesPuerta.end(), {4, 6, 5, 4, 7, 6});
	indicVertice += 4;

	// Cara izquierda del rectángulo
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, 0.0f, -profundidadPuerta/2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, 0.0f, profundidadPuerta/2, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, alturaRectangular, profundidadPuerta/2, 1.0f, 0.75f, -1.0f, 0.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, alturaRectangular, -profundidadPuerta/2, 0.0f, 0.75f, -1.0f, 0.0f, 0.0f});
	indicesPuerta.insert(indicesPuerta.end(), {8, 9, 10, 8, 10, 11});
	indicVertice += 4;

	// Cara derecha del rectángulo
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, 0.0f, -profundidadPuerta/2, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, 0.0f, profundidadPuerta/2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, alturaRectangular, profundidadPuerta/2, 0.0f, 0.75f, 1.0f, 0.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, alturaRectangular, -profundidadPuerta/2, 1.0f, 0.75f, 1.0f, 0.0f, 0.0f});
	indicesPuerta.insert(indicesPuerta.end(), {12, 14, 13, 12, 15, 14});
	indicVertice += 4;

	// Cara inferior del rectángulo
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, 0.0f, -profundidadPuerta/2, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, 0.0f, -profundidadPuerta/2, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, 0.0f, profundidadPuerta/2, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, 0.0f, profundidadPuerta/2, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f});
	indicesPuerta.insert(indicesPuerta.end(), {16, 18, 17, 16, 19, 18});
	indicVertice += 4;

	unsigned int indiceCentroFrontal = indicVertice;
	verticesPuerta.insert(verticesPuerta.end(), {0.0f, alturaRectangular, profundidadPuerta/2, 0.5f, 0.7f, 0.0f, 0.0f, 1.0f});
	indicVertice++;
	
	for (int i = 0; i <= segmentos; i++) {
		float angulo = 3.14159265f * (float)i / (float)segmentos; 
		float x = radioSemicirculo * cos(angulo);
		float y = alturaRectangular + radioSemicirculo * sin(angulo);
		float u = 1.0f - (float)i / (float)segmentos;
		float v = 0.75f + 0.35f * sin(angulo);
		verticesPuerta.insert(verticesPuerta.end(), {x, y, profundidadPuerta/2, u, v, 0.0f, 0.0f, 1.0f});
		
		if (i < segmentos) {
			indicesPuerta.insert(indicesPuerta.end(), {indiceCentroFrontal, indicVertice + 1, indicVertice});
		}
		indicVertice++;
	}

	// Cara semicircular trasera (z = -profundidadPuerta/2)
	unsigned int indiceCentroTrasero = indicVertice;
	verticesPuerta.insert(verticesPuerta.end(), {0.0f, alturaRectangular, -profundidadPuerta/2, 0.5f, 0.7f, 0.0f, 0.0f, -1.0f});
	indicVertice++;
	
	for (int i = 0; i <= segmentos; i++) {
		float angulo = 3.14159265f * (float)i / (float)segmentos;
		float x = radioSemicirculo * cos(angulo);
		float y = alturaRectangular + radioSemicirculo * sin(angulo);
		float u = (float)i / (float)segmentos;
		float v = 0.75f + 0.35f * sin(angulo);
		verticesPuerta.insert(verticesPuerta.end(), {x, y, -profundidadPuerta/2, u, v, 0.0f, 0.0f, -1.0f});
		
		if (i < segmentos) {
			indicesPuerta.insert(indicesPuerta.end(), {indiceCentroTrasero, indicVertice, indicVertice + 1});
		}
		indicVertice++;
	}

	// Superficie curva exterior del semicírculo
	unsigned int indiceInicioCurva = indicVertice;
	for (int i = 0; i <= segmentos; i++) {
		float angulo = 3.14159265f * (float)i / (float)segmentos;
		float x = radioSemicirculo * cos(angulo);
		float y = alturaRectangular + radioSemicirculo * sin(angulo);
		float nx = cos(angulo);
		float ny = sin(angulo);
		float u = (float)i / (float)segmentos;
		
		verticesPuerta.insert(verticesPuerta.end(), {x, y, profundidadPuerta/2, u, 1.0f, nx, ny, 0.0f});
		verticesPuerta.insert(verticesPuerta.end(), {x, y, -profundidadPuerta/2, u, 0.0f, nx, ny, 0.0f});
		
		if (i < segmentos) {
			unsigned int idx = indiceInicioCurva + i * 2;
			indicesPuerta.insert(indicesPuerta.end(), {idx, idx + 1, idx + 3});
			indicesPuerta.insert(indicesPuerta.end(), {idx, idx + 3, idx + 2});
		}
	}
	indicVertice += (segmentos + 1) * 2;

	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, alturaRectangular, profundidadPuerta/2, 0.0f, 0.75f, 0.0f, -1.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, alturaRectangular, profundidadPuerta/2, 1.0f, 0.75f, 0.0f, -1.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {anchoPuerta/2, alturaRectangular, -profundidadPuerta/2, 1.0f, 0.75f, 0.0f, -1.0f, 0.0f});
	verticesPuerta.insert(verticesPuerta.end(), {-anchoPuerta/2, alturaRectangular, -profundidadPuerta/2, 0.0f, 0.75f, 0.0f, -1.0f, 0.0f});
	unsigned int indiceBasePlana = indicVertice;
	indicesPuerta.insert(indicesPuerta.end(), {indiceBasePlana, indiceBasePlana + 2, indiceBasePlana + 1});
	indicesPuerta.insert(indicesPuerta.end(), {indiceBasePlana, indiceBasePlana + 3, indiceBasePlana + 2});

	// ===== PRISMA RECTANGULAR INCRUSTADO (OBJETO SEPARADO) =====
	const float anchoPrisma = 0.8f;
	const float alturaPrisma = 0.6f;
	const float profundidadPrisma = 0.3f;
	const float yPosPrisma = alturaRectangular + radioSemicirculo * 0.5f;
	
	std::vector<GLfloat> verticesPrisma;
	std::vector<unsigned int> indicesPrisma;
	unsigned int indicePrisma = 0;
	
	// Cara frontal del prisma (z = profundidadPuerta/2)
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f});
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f});
	indicesPrisma.insert(indicesPrisma.end(), {0, 1, 2, 0, 2, 3});
	indicePrisma += 4;
	
	// Cara trasera del prisma (z = profundidadPuerta/2 + profundidadPrisma)
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f});
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f});
	indicesPrisma.insert(indicesPrisma.end(), {4, 6, 5, 4, 7, 6});
	indicePrisma += 4;
	
	// Cara izquierda del prisma
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f});
	indicesPrisma.insert(indicesPrisma.end(), {8, 9, 10, 8, 10, 11});
	indicePrisma += 4;
	
	// Cara derecha del prisma
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f});
	indicesPrisma.insert(indicesPrisma.end(), {12, 14, 13, 12, 15, 14});
	indicePrisma += 4;
	
	// Cara superior del prisma
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma + alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f});
	indicesPrisma.insert(indicesPrisma.end(), {16, 17, 18, 16, 18, 19});
	indicePrisma += 4;
	
	// Cara inferior del prisma
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f});
	verticesPrisma.insert(verticesPrisma.end(), {-anchoPrisma/2, yPosPrisma - alturaPrisma/2, profundidadPuerta/2 + profundidadPrisma, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f});
	indicesPrisma.insert(indicesPrisma.end(), {20, 22, 21, 20, 23, 22});

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

	Mesh* mallaPuerta = new Mesh();
	mallaPuerta->CreateMesh(verticesPuerta.data(), indicesPuerta.data(), verticesPuerta.size(), indicesPuerta.size());
	meshList.push_back(mallaPuerta);

	Mesh* mallaPrisma = new Mesh();
	mallaPrisma->CreateMesh(verticesPrisma.data(), indicesPrisma.data(), verticesPrisma.size(), indicesPrisma.size());
	meshList.push_back(mallaPrisma);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

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
	TexturaMadera = Texture("Textures/puerta_textura.png");
	TexturaMadera.LoadTextureA();
	TexturaLetrero = Texture("Textures/blanco.png");
	TexturaLetrero.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/delorean_texturizado.obj");
	Llanta_1 = Model();
	Llanta_1.LoadModel("Models/llanta_1.obj");
	Llanta_2 = Model();
	Llanta_2.LoadModel("Models/llanta_2.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");


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
	unsigned int pointLightCount2 = 0;

	pointLights[0] = PointLight(0.0f, 1.0f, 0.0f,
		0.5f, 1.0f,
		-12.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount++;

	pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,
		0.5f, 1.0f,
		-2.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount++;

	pointLights[2] = PointLight(1.0f, 0.0f, 0.0f,
		0.5f, 1.0f,
		8.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount++;
	
	pointLights[3] = PointLight(1.0f, 1.0f, 1.0f,
		0.5f, 1.0f,
		18.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount++;
	
	// PointLights2 - White, Green, Blue, Red
	pointLights2[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.5f, 1.0f,
		-12.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount2++;
	pointLights2[1] = PointLight(0.0f, 1.0f, 0.0f, // Green
		0.5f, 1.0f,
		-2.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount2++;
	pointLights2[2] = PointLight(0.0f, 0.0f, 1.0f, // Blue
		0.5f, 1.0f,
		8.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount2++;
	
	pointLights2[3] = PointLight(1.0f, 0.0f, 0.0f, // Red
		0.5f, 1.0f,
		18.0f, 3.0f, 0.0f,
		1.0f, 0.1f, 0.005f);
	pointLightCount2++;
	
	unsigned int spotLightCount = 0;
	//spotlight de cámara (color blanco)
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.5f, 1.0f,
		2.0f, 3.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.1f, 0.0f, // con, lin, exp
		15.0f);
	spotLightCount++;
	/*
	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
		0.6f, 1.2f,
		-4.0f, 3.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, // con, lin, exp
		5.0f);
	spotLightCount++;
	*/
	bool isOn = true;

	//se crean mas luces puntuales y spotlight 

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
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;

		if (mainWindow.getsKeys()[GLFW_KEY_F])
		{
			isOn = !isOn;
			mainWindow.getsKeys()[GLFW_KEY_F] = false;
		}

		if (isOn)
			spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		else
			spotLights[0].SetFlash(glm::vec3(0.0f), glm::vec3(0.0f));

		if (mainWindow.getsKeys()[GLFW_KEY_1]){
			selectLightArray(1);
		}

		if (mainWindow.getsKeys()[GLFW_KEY_2]){
			selectLightArray(2);
		}

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		// Ahora usamos la referencia a al arreglo de luces puntuales seleccionado
		shaderList[0].SetPointLights(currentPointLights, pointLightCount); 
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

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

		// Renderizar puerta con semicírculo en la parte superior
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		TexturaMadera.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[4]->RenderMesh(); 

		// Renderizar prisma rectangular incrustado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(1.3f, 0.875f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		TexturaLetrero.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[5]->RenderMesh();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
