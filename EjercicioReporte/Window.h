#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevex2() { return muevex2; } // Getter para mover el helicóptero
	GLfloat getMueveCofre() { return mueveCofre; } // Getter para mover el cofre
	bool getAdelante() { return adelante; } // Getter para saber si el carro avanza
	bool getFogataEncendida() { return fogataEncendida; } // Getter para saber si la fogata está encendida
	bool getEstrellaEncendida() { return estrellaEncendida; } // Getter para saber si la estrella está encendida
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024]; 
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevex2; // Para mover el helicóptero
	GLfloat mueveCofre; // Para mover el cofre
	bool adelante; // Para saber si el carro avanza
	bool mouseFirstMoved;
	bool fogataEncendida; //Bandera para saber si la fogata está encendida
	bool estrellaEncendida; // Bandera para saber si la estrella está encendida
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

