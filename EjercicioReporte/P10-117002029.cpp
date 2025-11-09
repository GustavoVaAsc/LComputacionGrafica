/* 
Animación por keyframes
La textura del skybox fue conseguida desde la página https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edité en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
	*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <fstream>
#include <iostream>

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

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
float ciclo3, ciclo4, ciclo5, ciclo6, ciclo7, ciclo8 = 0;
float ciclo9, ciclo10, ciclo11, ciclo12, ciclo13, ciclo14 = 0;
float ciclo15, ciclo16, ciclo17, ciclo18 = 0;
float ciclo19, ciclo20, ciclo21, ciclo22 = 0;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;


Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;



Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Pez1_M;

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

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
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


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

// Variables para el pez
float posXpez = -5.0, posYpez = 2.0, posZpez = 0.0;
float movPez_x = 0.0f, movPez_y = 0.0f, movPez_z = 0.0f;
float giroPez = 0;

#define MAX_FRAMES 100 //Número de cuadros máximos
int i_max_steps = 100; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;		//Variable para GiroAvion
	float giroAvionInc;		//Variable para IncrementoGiroAvion
	
	// Variables para el pez
	float movPez_x;
	float movPez_y;
	float movPez_z;
	float movPez_xInc;
	float movPez_yInc;
	float movPez_zInc;
	float giroPez;
	float giroPezInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;			//El número de cuadros guardados actualmente desde 0 para no sobreescribir
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	
	// Guardar datos del pez
	KeyFrame[FrameIndex].movPez_x = movPez_x;
	KeyFrame[FrameIndex].movPez_y = movPez_y;
	KeyFrame[FrameIndex].movPez_z = movPez_z;
	KeyFrame[FrameIndex].giroPez = giroPez;
	
	//Se agregan nuevas líneas para guardar más variables si es necesario
	
	//no volatil,se requiere agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void saveKeyFramesToFile(const char* filename)
{
	std::ofstream file(filename);
	if (!file.is_open())
	{
		std::cout << "Error: No se pudo crear el archivo " << filename << std::endl;
		return;
	}

	file << FrameIndex << std::endl;

	for (int i = 0; i < FrameIndex; i++)
	{
		file << KeyFrame[i].movPez_x << " "
			 << KeyFrame[i].movPez_y << " "
			 << KeyFrame[i].movPez_z << " "
			  <<  KeyFrame[i].giroPez <<std::endl;
	}

	file.close();
	std::cout << "Keyframes guardados exitosamente en " << filename << std::endl;
	std::cout << "Total de frames guardados: " << FrameIndex << std::endl;
}

bool loadKeyFramesFromFile(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cout << "Error: No se pudo abrir el archivo " << filename << std::endl;
		return false;
	}

	int totalFrames = 0;
	file >> totalFrames;
	
	if (file.fail())
	{
		std::cout << "Error: Formato de archivo incorrecto" << std::endl;
		file.close();
		return false;
	}

	if (totalFrames > MAX_FRAMES)
	{
		std::cout << "Advertencia: El archivo contiene " << totalFrames 
				  << " frames, pero el máximo es " << MAX_FRAMES << std::endl;
		totalFrames = MAX_FRAMES;
	}

	for (int i = 0; i < totalFrames; i++)
	{
		file >> KeyFrame[i].movPez_x 
			 >> KeyFrame[i].movPez_y 
			 >> KeyFrame[i].movPez_z
			 >> KeyFrame[i].giroPez;
		
		if (file.fail())
		{
			std::cout << "Error: No se pudo leer el frame " << i << std::endl;
			file.close();
			return false;
		}
		
		KeyFrame[i].movAvion_xInc = 0.0f;
		KeyFrame[i].movAvion_yInc = 0.0f;
		KeyFrame[i].giroAvionInc = 0.0f;
	}

	FrameIndex = totalFrames;
	file.close();
	
	std::cout << "Keyframes cargados exitosamente desde " << filename << std::endl;
	std::cout << "Total de frames cargados: " << FrameIndex << std::endl;
	return true;
}

void resetElements(void) //Tecla 0
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
	
	// Resetear elementos del pez
	movPez_x = KeyFrame[0].movPez_x;
	movPez_y = KeyFrame[0].movPez_y;
	movPez_z = KeyFrame[0].movPez_z;
	giroPez = KeyFrame[0].giroPez;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

	// Interpolación del pez
	KeyFrame[playIndex].movPez_xInc = (KeyFrame[playIndex + 1].movPez_x - KeyFrame[playIndex].movPez_x) / i_max_steps;
	KeyFrame[playIndex].movPez_yInc = (KeyFrame[playIndex + 1].movPez_y - KeyFrame[playIndex].movPez_y) / i_max_steps;
	KeyFrame[playIndex].movPez_zInc = (KeyFrame[playIndex + 1].movPez_z - KeyFrame[playIndex].movPez_z) / i_max_steps;
	KeyFrame[playIndex].giroPezInc = (KeyFrame[playIndex + 1].giroPez - KeyFrame[playIndex].giroPez) / i_max_steps;
}


void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolación del próximo cuadro
			{
				
				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//Dibujar Animación
			movAvion_x += KeyFrame[playIndex].movAvion_xInc ;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc ;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			
			// Animar pez
			movPez_x += KeyFrame[playIndex].movPez_xInc;
			movPez_y += KeyFrame[playIndex].movPez_yInc;
			movPez_z += KeyFrame[playIndex].movPez_zInc;
			giroPez += KeyFrame[playIndex].giroPezInc;
			
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Pez1_M = Model();
	Pez1_M.LoadModel("Models/pez1.obj");
	


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sp2_rt.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_lf.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_dn.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_up.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_bk.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_ft.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
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

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);
	
	//---------PARA TENER KEYFRAMES GUARDADOS NO VOLATILES QUE SIEMPRE SE UTILIZARAN SE DECLARAN AQUÍ

	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].giroAvion = 0;
	KeyFrame[0].movPez_x = 0.0f;
	KeyFrame[0].movPez_y = 0.0f;
	KeyFrame[0].movPez_z = 0.0f;
	KeyFrame[0].giroPez = 0;


	KeyFrame[1].movAvion_x = -2.0f;
	KeyFrame[1].movAvion_y = 4.0f;
	KeyFrame[1].giroAvion = 0;
	KeyFrame[1].movPez_x = 2.0f;
	KeyFrame[1].movPez_y = 1.0f;
	KeyFrame[1].movPez_z = 2.0f;
	KeyFrame[1].giroPez = 45;


	KeyFrame[2].movAvion_x = -4.0f;
	KeyFrame[2].movAvion_y = 0.0f;
	KeyFrame[2].giroAvion = 0;
	KeyFrame[2].movPez_x = 4.0f;
	KeyFrame[2].movPez_y = -1.0f;
	KeyFrame[2].movPez_z = -2.0f;
	KeyFrame[2].giroPez = 90;


	KeyFrame[3].movAvion_x = -6.0f;
	KeyFrame[3].movAvion_y = -4.0f;
	KeyFrame[3].giroAvion = 0;
	KeyFrame[3].movPez_x = 2.0f;
	KeyFrame[3].movPez_y = 2.0f;
	KeyFrame[3].movPez_z = 3.0f;
	KeyFrame[3].giroPez = 180;


	KeyFrame[4].movAvion_x = -8.0f;
	KeyFrame[4].movAvion_y = 0.0f;
	KeyFrame[4].giroAvion = 0.0f;
	KeyFrame[4].movPez_x = 0.0f;
	KeyFrame[4].movPez_y = -2.0f;
	KeyFrame[4].movPez_z = -3.0f;
	KeyFrame[4].giroPez = 270;

	KeyFrame[5].movAvion_x = -10.0f;
	KeyFrame[5].movAvion_y = 4.0f;
	KeyFrame[5].giroAvion = 0.0f;
	KeyFrame[5].movPez_x = -2.0f;
	KeyFrame[5].movPez_y = 0.0f;
	KeyFrame[5].movPez_z = 0.0f;
	KeyFrame[5].giroPez = 360;
	
	//Se agregan nuevos frames 


		printf("\nTeclas para uso de Keyframes:\n");
		printf("K - Reproducir animacion\n");
		printf("0 - Habilitar reproduccion de nuevo\n");
		printf("L - Guardar frame actual\n");
		printf("P - Habilitar guardar nuevo frame\n");
		printf("\n--- Control Helicoptero ---\n");
		printf("1 - Mover helicoptero en X positiva\n");
		printf("2 - Habilitar mover en X\n");
		printf("3 - Mover helicoptero en Y negativa (abajo)\n");
		printf("4 - Habilitar mover en Y negativa\n");
		printf("5 - Mover helicoptero en Y positiva (arriba)\n");
		printf("6 - Habilitar mover en Y positiva\n");
		printf("7 - Rotar helicoptero 10 grados en sentido horario (eje Y)\n");
		printf("8 - Habilitar rotacion\n");
		printf("\n--- Control Pez ---\n");
		printf("Q - Mover pez en X positiva\n");
		printf("Z - Habilitar mover pez en X\n");
		printf("E - Mover pez en Y negativa (abajo)\n");
		printf("X - Habilitar mover pez en Y negativa\n");
		printf("T - Mover pez en Y positiva (arriba)\n");
		printf("F - Habilitar mover pez en Y positiva\n");
		printf("U - Rotar pez 10 grados en sentido horario (eje Y)\n");
		printf("V - Habilitar rotacion pez horario\n");
		printf("J - Rotar pez 10 grados en sentido antihorario (eje Y)\n");
		printf("M - Habilitar rotacion pez antihorario\n");
		printf("O - Mover pez en Z positiva (adelante)\n");
		printf("B - Habilitar mover pez en Z positiva\n");
		printf("H - Mover pez en Z negativa (atras)\n");
		printf("N - Habilitar mover pez en Z negativa\n");


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glm::vec3 lowerLight = glm::vec3(0.0f,0.0f,0.0f);
		glm::vec3 pospez = glm::vec3(0.0f, 0.0f, 0.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		model=glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();


		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche-50.0f, 0.5f, -2.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();


		model = glm::mat4(1.0);
		posblackhawk=glm::vec3(posXavion + movAvion_x, posYavion + movAvion_y, posZavion);
		model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		
		// Renderizado del pez
		model = glm::mat4(1.0);
		pospez = glm::vec3(posXpez + movPez_x, posYpez + movPez_y, posZpez + movPez_z);
		model = glm::translate(model, pospez);
		model = glm::rotate(model, giroPez * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(1.0f, 0.5f, 0.0f); // Color naranja para el pez
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pez1_M.RenderModel();
		
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool* keys)
{
	// Tecla K para reproducir animación
	static bool kKeyPressed = false;
	if (keys[GLFW_KEY_K])
	{
		if (!kKeyPressed && reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;
				kKeyPressed = true;
			}
			else
			{
				play = false;
			}
		}
	}
	else
	{
		kKeyPressed = false;
	}

	// Tecla 0 para habilitar reproducción de nuevo
	static bool key0Pressed = false;
	if (keys[GLFW_KEY_0])
	{
		if (!key0Pressed && habilitaranimacion < 1 && reproduciranimacion > 0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla K'\n");
			reproduciranimacion = 0;
			habilitaranimacion++;
			key0Pressed = true;
		}
	}
	else
	{
		key0Pressed = false;
	}

	// Tecla L para guardar frame
	static bool lKeyPressed = false;
	if (keys[GLFW_KEY_L])
	{
		if (!lKeyPressed && guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
			lKeyPressed = true;
		}
	}
	else
	{
		lKeyPressed = false;
	}
	
	// Tecla P para habilitar guardar nuevo frame
	static bool pKeyPressed = false;
	if (keys[GLFW_KEY_P])
	{
		if (!pKeyPressed && reinicioFrame < 1)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
			reinicioFrame++;
			pKeyPressed = true;
		}
	}
	else
	{
		pKeyPressed = false;
	}

	// Tecla G para guardar keyframes al archivo
	static bool gKeyPressed = false;
	if (keys[GLFW_KEY_G])
	{
		if (!gKeyPressed)
		{
			saveKeyFramesToFile("keyframes.txt");
			gKeyPressed = true;
		}
	}
	else
	{
		gKeyPressed = false;
	}

	// Tecla C para cargar keyframes desde archivo
	static bool cKeyPressed = false;
	if (keys[GLFW_KEY_C])
	{
		if (!cKeyPressed)
		{
			if (loadKeyFramesFromFile("keyframes.txt"))
			{
				resetElements();
			}
			cKeyPressed = true;
		}
	}
	else
	{
		cKeyPressed = false;
	}

	// Tecla 1 para mover en X positiva
	static bool key1Pressed = false;
	if (keys[GLFW_KEY_1])
	{
		if (!key1Pressed && ciclo < 1)
		{
			movAvion_x += 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
			key1Pressed = true;
		}
	}
	else
	{
		key1Pressed = false;
	}

	// Tecla 2 para habilitar modificar variable
	static bool key2Pressed = false;
	if (keys[GLFW_KEY_2])
	{
		if (!key2Pressed && ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
			key2Pressed = true;
		}
	}
	else
	{
		key2Pressed = false;
	}

	// Tecla 3 para mover helicóptero en Y negativa
	static bool key3Pressed = false;
	if (keys[GLFW_KEY_3])
	{
		if (!key3Pressed && ciclo3 < 1)
		{
			movAvion_y -= 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo3++;
			ciclo4 = 0;
			printf("\n Presiona la tecla 4 para poder habilitar la variable\n");
			key3Pressed = true;
		}
	}
	else
	{
		key3Pressed = false;
	}

	// Tecla 4 para habilitar mover en Y negativa
	static bool key4Pressed = false;
	if (keys[GLFW_KEY_4])
	{
		if (!key4Pressed && ciclo4 < 1)
		{
			ciclo3 = 0;
			ciclo4++;
			printf("\n Ya puedes mover en Y negativa presionando la tecla 3\n");
			key4Pressed = true;
		}
	}
	else
	{
		key4Pressed = false;
	}

	// Tecla 5 para mover helicóptero en Y positiva
	static bool key5Pressed = false;
	if (keys[GLFW_KEY_5])
	{
		if (!key5Pressed && ciclo5 < 1)
		{
			movAvion_y += 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo5++;
			ciclo6 = 0;
			printf("\n Presiona la tecla 6 para poder habilitar la variable\n");
			key5Pressed = true;
		}
	}
	else
	{
		key5Pressed = false;
	}

	// Tecla 6 para habilitar mover en Y positiva
	static bool key6Pressed = false;
	if (keys[GLFW_KEY_6])
	{
		if (!key6Pressed && ciclo6 < 1)
		{
			ciclo5 = 0;
			ciclo6++;
			printf("\n Ya puedes mover en Y positiva presionando la tecla 5\n");
			key6Pressed = true;
		}
	}
	else
	{
	key6Pressed = false;
	}

	// Tecla 7 para rotar helicóptero 10 grados en sentido horario sobre eje Y
	static bool key7Pressed = false;
	if (keys[GLFW_KEY_7])
	{
		if (!key7Pressed && ciclo7 < 1)
		{
			giroAvion += 10.0f;
			printf("\n giroAvion es: %f grados\n", giroAvion);
			ciclo7++;
			ciclo8 = 0;
			printf("\n Presiona la tecla 8 para poder habilitar la variable\n");
			key7Pressed = true;
		}
	}
	else
	{
		key7Pressed = false;
	}

	// Tecla 8 para habilitar rotar
	static bool key8Pressed = false;
	if (keys[GLFW_KEY_8])
	{
		if (!key8Pressed && ciclo8 < 1)
		{
			ciclo7 = 0;
			ciclo8++;
			printf("\n Ya puedes rotar el helicoptero presionando la tecla 7\n");
			key8Pressed = true;
		}
	}
	else
	{
		key8Pressed = false;
	}

	// ========== CONTROLES PARA EL PEZ ==========
	
	// Tecla Q para mover pez en X positiva
	static bool keyQPressed = false;
	if (keys[GLFW_KEY_Q])
	{
		if (!keyQPressed && ciclo9 < 1)
		{
			movPez_x += 1.0f;
			printf("\n movPez_x es: %f\n", movPez_x);
			ciclo9++;
			ciclo10 = 0;
			printf("\n Presiona la tecla Z para poder habilitar la variable\n");
			keyQPressed = true;
		}
	}
	else
	{
		keyQPressed = false;
	}

	// Tecla Z para habilitar modificar variable X del pez
	static bool keyZPressed = false;
	if (keys[GLFW_KEY_Z])
	{
		if (!keyZPressed && ciclo10 < 1)
		{
			ciclo9 = 0;
			ciclo10++;
			printf("\n Ya puedes modificar tu variable presionando la tecla Q\n");
			keyZPressed = true;
		}
	}
	else
	{
		keyZPressed = false;
	}

	// Tecla E para mover pez en Y negativa
	static bool keyEPressed = false;
	if (keys[GLFW_KEY_E])
	{
		if (!keyEPressed && ciclo11 < 1)
		{
			movPez_y -= 1.0f;
			printf("\n movPez_y es: %f\n", movPez_y);
			ciclo11++;
			ciclo12 = 0;
			printf("\n Presiona la tecla X para poder habilitar la variable\n");
			keyEPressed = true;
		}
	}
	else
	{
		keyEPressed = false;
	}

	// Tecla X para habilitar mover pez en Y negativa
	static bool keyXPressed = false;
	if (keys[GLFW_KEY_X])
	{
		if (!keyXPressed && ciclo12 < 1)
		{
			ciclo11 = 0;
			ciclo12++;
			printf("\n Ya puedes mover pez en Y negativa presionando la tecla E\n");
			keyXPressed = true;
		}
	}
	else
	{
		keyXPressed = false;
	}

	// Tecla T para mover pez en Y positiva
	static bool keyTPressed = false;
	if (keys[GLFW_KEY_T])
	{
		if (!keyTPressed && ciclo13 < 1)
		{
			movPez_y += 1.0f;
			printf("\n movPez_y es: %f\n", movPez_y);
			ciclo13++;
			ciclo14 = 0;
			printf("\n Presiona la tecla F para poder habilitar la variable\n");
			keyTPressed = true;
		}
	}
	else
	{
		keyTPressed = false;
	}

	// Tecla F para habilitar mover pez en Y positiva
	static bool keyFPressed = false;
	if (keys[GLFW_KEY_F])
	{
		if (!keyFPressed && ciclo14 < 1)
		{
			ciclo13 = 0;
			ciclo14++;
			printf("\n Ya puedes mover pez en Y positiva presionando la tecla T\n");
			keyFPressed = true;
		}
	}
	else
	{
		keyFPressed = false;
	}

	// Tecla U para rotar pez 10 grados en sentido horario sobre eje Y
	static bool keyUPressed = false;
	if (keys[GLFW_KEY_U])
	{
		if (!keyUPressed && ciclo15 < 1)
		{
			giroPez += 10.0f;
			printf("\n giroPez es: %f grados\n", giroPez);
			ciclo15++;
			ciclo16 = 0;
			printf("\n Presiona la tecla V para poder habilitar la variable\n");
			keyUPressed = true;
		}
	}
	else
	{
		keyUPressed = false;
	}

	// Tecla V para habilitar rotar pez
	static bool keyVPressed = false;
	if (keys[GLFW_KEY_V])
	{
		if (!keyVPressed && ciclo16 < 1)
		{
			ciclo15 = 0;
			ciclo16++;
			printf("\n Ya puedes rotar el pez presionando la tecla U\n");
			keyVPressed = true;
		}
	}
	else
	{
		keyVPressed = false;
	}

	// Tecla J para rotar pez 10 grados en sentido antihorario sobre eje Y
	static bool keyJPressed = false;
	if (keys[GLFW_KEY_J])
	{
		if (!keyJPressed && ciclo21 < 1)
		{
			giroPez -= 10.0f;
			printf("\n giroPez es: %f grados\n", giroPez);
			ciclo21++;
			ciclo22 = 0;
			printf("\n Presiona la tecla M para poder habilitar la variable\n");
			keyJPressed = true;
		}
	}
	else
	{
		keyJPressed = false;
	}

	// Tecla M para habilitar rotar pez antihorario
	static bool keyMPressed = false;
	if (keys[GLFW_KEY_M])
	{
		if (!keyMPressed && ciclo22 < 1)
		{
			ciclo21 = 0;
			ciclo22++;
			printf("\n Ya puedes rotar el pez en sentido antihorario presionando la tecla J\n");
			keyMPressed = true;
		}
	}
	else
	{
		keyMPressed = false;
	}

	// Tecla O para mover pez en Z positiva
	static bool keyOPressed = false;
	if (keys[GLFW_KEY_O])
	{
		if (!keyOPressed && ciclo17 < 1)
		{
			movPez_z += 1.0f;
			printf("\n movPez_z es: %f\n", movPez_z);
			ciclo17++;
			ciclo18 = 0;
			printf("\n Presiona la tecla B para poder habilitar la variable\n");
			keyOPressed = true;
		}
	}
	else
	{
		keyOPressed = false;
	}

	// Tecla B para habilitar mover pez en Z positiva
	static bool keyBPressed = false;
	if (keys[GLFW_KEY_B])
	{
		if (!keyBPressed && ciclo18 < 1)
		{
			ciclo17 = 0;
			ciclo18++;
			printf("\n Ya puedes mover pez en Z positiva presionando la tecla O\n");
			keyBPressed = true;
		}
	}
	else
	{
		keyBPressed = false;
	}

	// Tecla H para mover pez en Z negativa
	static bool keyHPressed = false;
	if (keys[GLFW_KEY_H])
	{
		if (!keyHPressed && ciclo19 < 1)
		{
			movPez_z -= 1.0f;
			printf("\n movPez_z es: %f\n", movPez_z);
			ciclo19++;
			ciclo20 = 0;
			printf("\n Presiona la tecla N para poder habilitar la variable\n");
			keyHPressed = true;
		}
	}
	else
	{
		keyHPressed = false;
	}

	// Tecla N para habilitar mover pez en Z negativa
	static bool keyNPressed = false;
	if (keys[GLFW_KEY_N])
	{
		if (!keyNPressed && ciclo20 < 1)
		{
			ciclo19 = 0;
			ciclo20++;
			printf("\n Ya puedes mover pez en Z negativa presionando la tecla H\n");
			keyNPressed = true;
		}
	}
	else
	{
		keyNPressed = false;
	}
}
