#include "Splines.h"
#include "Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"
#include "GLSLProgram.h"
#include "BOX.h"
#include "auxiliar.h"
#include "Scene.h"

#include <windows.h>

#include <gl/glew.h>
#include <gl/gl.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 
#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <FreeImage.h>

using std::vector;

#define numPuntos 10 //Factor para calcular spline
#define tamTex 256 //Tamaño para la textura ruido Perlin

//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Camera
Camera camera;

//Meshes
//Mesh ogre("../img/diffuse.png", "../img/bump.png");
Mesh cube1;
//Mesh cube2;
//Mesh cube3;
//Mesh cube4;

//Variable cambio intensidad
Light light1;
Light light2(DIRECTIONAL_LIGHT);
Light light3(SPOT_LIGHT);

float valorIntensidad = 0.5f;
glm::vec2 lightCoord = glm::vec2(0.0f);
glm::vec3 lightPos = glm::vec3(1.0f);

GLSLProgram programa;
GLSLProgram programa2;
GLSLProgram programa3;

Scene scene1;

//Declaración de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);
void initOGL();
void initObj();
void destroy();


Vector CatmullRom(float t, Vector p1, Vector p2, Vector p3, Vector p4);
vector<vector<float>> GenerateWhiteNoise(int width, int height);
vector<vector<float>> GenerateSmoothNoise(vector<vector<float>> baseNoise, int octave);
float Interpolate(float x0, float x1, float alpha);
vector<vector<float>> GeneratePerlinNoise(vector<vector<float>> baseNoise, int octaveCount);
unsigned char *MapGradient(vector<vector<float>> perlinNoise);

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();

	SplineList mySplines;
	//mySplines.LoadSplines("../Splines/circle.sp");
	
	Vector ejemplo[6];
	ejemplo[0] = Vector(-0.500000, - 0.866025, 0.000000);
	ejemplo[1] = Vector(1.000000, 0.000000, 0.000000);
	ejemplo[2] = Vector(-0.500000, 0.866025, 0.000000);
	ejemplo[3] = Vector(-0.500000, - 0.866025, 0.000000);
	ejemplo[4] = Vector(1.000000, 0.000000, 0.000000);
	ejemplo[5] = Vector(-0.500000, 0.866025, 0.000000);

	/*Vector points[4];
	mySplines.GetCurrent(points);
	printf("Tengo %i puntos, estoy en el %i\nCOORD %f %f %f\n", mySplines.GetSize(), mySplines.GetCurrentPoint(), points[2].x, points[2].y, points[2].z);*/

	Vector calculatedSplines [6 * numPuntos];
	int indice = 0;

	

	int indEj = 0;
	for (int sp = 0; sp < 3; sp++) {
		/*Vector points[4];
		mySplines.GetCurrent(points);*/
		for (float t = 0.0f; t < 1.0f; t += 1.0f / numPuntos){
			//calculatedSplines[indice] = CatmullRom(t, points[0], points[1], points[2], points[3]);
			printf("SP = %i t = %f %Iteración %i: ", sp, t, indice);
			calculatedSplines[indice] = CatmullRom(t, ejemplo[indEj], ejemplo[indEj+1], ejemplo[indEj+2], ejemplo[indEj+3]);
			indice++;
			//printf("Punto calculado %f %f %f\n", calculatedSplines[indice].x, calculatedSplines[indice].y, calculatedSplines[indice].z);
		}
		//mySplines.MoveToNext();
		
		indEj++;
	}
	
	vector<vector<float>> whiteNoise;
	vector<vector<float>> perlinNoise;

	/*whiteNoise.resize(tamTex);
	for (int i = 0; i < tamTex; i++)
		whiteNoise[i].resize(tamTex);*/
	whiteNoise = GenerateWhiteNoise(tamTex, tamTex);
	perlinNoise = GeneratePerlinNoise(whiteNoise, 8);

	/*for (int i = 0; i < tamTex; i++)
		for (int j = 0; j < tamTex; j++)
			printf("Valores obtenidos: %i %i = %f\n", i, j, perlinNoise[i][j]);*/

	unsigned char *PerlinTex = MapGradient(perlinNoise);
	
	Texture perlinTex;
	perlinTex.Load(PerlinTex, tamTex, tamTex);

	cube1 = Mesh(perlinTex);

	programa.InitShader("../shaders_P3/shader.v1.vert", "../shaders_P3/shader.v1.frag"); //v0 sin textura, V1 con textura
	programa2.InitShader("../shaders_P3/shader.v1.vert", "../shaders_P3/shader.v1.frag");
	programa3.InitShader("../shaders_P3/shader.v1.vert", "../shaders_P3/shader.v1.frag");
	initObj();

	glutMainLoop();

	destroy();

	return 0;
}

//////////////////////////////////////////
// Funciones auxiliares 
void initContext(int argc, char** argv){
	//Crea un contexto básico
	glutInit(&argc, argv);
	//Genera un contexto de opengl 3.3
	glutInitContextVersion(3, 3);
	//Que no tenga recompatibilidad hacia atrás
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//ELEMENTOS DEL FRAMEBUFFER
	//Crea el framebuffer con profundidad
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Práctica 5");

	//Se pone esto porque glew es muy antigua, para poner funcionalidades experimentales
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}
	// Preguntamos con que versión de opengl estamos trabajando
	const GLubyte *oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
}

void initOGL(){
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	//Activar Culling, pintar cara front y back rellenas.
	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	camera.InitCamera();
}

void destroy(){
	programa.Destroy();
	programa2.Destroy();
	programa3.Destroy();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//ogre.Destroy(programa);
	cube1.Destroy(programa2);
	/*cube2.Destroy(programa2);
	cube3.Destroy(programa3);
	cube4.Destroy(programa3);*/
}

void initObj()
{
	light2.SetIntensity(glm::vec3(1.0, 0.0, 0.0));
	light3.SetIntensity(glm::vec3(0.0, 1.0, 0.0));

	programa.AddLight(light1);
	programa.AddLight(light2);
	programa.AddAmbientLight(scene1.getAmbientLight());

	programa2.AddLight(light2);
	programa2.AddAmbientLight(scene1.getAmbientLight());
	
	programa3.AddLight(light3);
	programa3.AddLight(light2);
	programa3.AddAmbientLight(scene1.getAmbientLight());

	//ogre.AddShader(programa);
	cube1.AddShader(programa2);
	/*cube2.AddShader(programa2);
	cube3.AddShader(programa3);
	cube4.AddShader(programa3);*/
	
	//ogre.InitMesh("../Mallas/ogre.ply");
	cube1.InitDefaultMesh();
	/*cube2.InitDefaultMesh();
	cube3.InitDefaultMesh();
	cube4.InitDefaultMesh();*/

	//scene1.AddObject(ogre);
	scene1.AddObject(cube1);
	/*scene1.AddObject(cube2);
	scene1.AddObject(cube3);
	scene1.AddObject(cube4);*/

	scene1.AddLight(light1);
	scene1.AddLight(light2);
	scene1.AddLight(light3);
	
	scene1.AddCamera(camera);

}

void renderFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene1.Render();

	glutSwapBuffers();
}

void resizeFunc(int width, int height)
{
	camera.ResizeAspectRatio(width, height);
	glViewport(0, 0, width, height);
}

void idleFunc()
{
	scene1.Animation();

	glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y){
	camera.MoveCamera(key);
	light1.LightController(key, camera);
}

void mouseFunc(int button, int state, int x, int y){}

Vector CatmullRom(float t, Vector p1, Vector p2, Vector p3, Vector p4)
{

	float t2 = t*t;
	float t3 = t*t*t;
	Vector v; // Interpolated point

	/* Catmull Rom spline Calculation */

	v.x = ((-t3 + 2 * t2 - t)*(p1.x) + (3 * t3 - 5 * t2 + 2)*(p2.x) + (-3 * t3 + 4 * t2 + t)* (p3.x) + (t3 - t2)*(p4.x)) / 2;
	v.y = ((-t3 + 2 * t2 - t)*(p1.y) + (3 * t3 - 5 * t2 + 2)*(p2.y) + (-3 * t3 + 4 * t2 + t)* (p3.y) + (t3 - t2)*(p4.y)) / 2;
	v.z = p1.z;
	
	printf("Values of v.x = %f and v.y = %f\n", v.x, v.y);

	return v;
}

vector<vector<float>> GenerateWhiteNoise(int width, int height){
	vector<vector<float>> noise;

	noise.resize(height);

	for (int i = 0; i < height; i++)
	{
		noise[i].resize(width);
		for (int j = 0; j < width; j++)
		{
			noise[i][j] = ((double)rand() / (RAND_MAX));
		}
	}

	return noise;
}

vector<vector<float>> GenerateSmoothNoise(vector<vector<float>> baseNoise, int octave){
	int height = baseNoise.size();
	int width = baseNoise[0].size();

	vector<vector<float>> smoothNoise;

	smoothNoise.resize(height);

	int samplePeriod = pow(2, octave); // calculates 2 ^ k
	float sampleFrequency = 1.0f / samplePeriod;

	for (int i = 0; i < height; i++)
	{
		smoothNoise[i].resize(width);
		//calculate the horizontal sampling indices
		int sample_i0 = (i / samplePeriod) * samplePeriod;
		int sample_i1 = (sample_i0 + samplePeriod) % height; //wrap around
		float horizontal_blend = (i - sample_i0) * sampleFrequency;

		for (int j = 0; j < width; j++)
		{
			//calculate the vertical sampling indices
			int sample_j0 = (j / samplePeriod) * samplePeriod;
			int sample_j1 = (sample_j0 + samplePeriod) % width; //wrap around
			float vertical_blend = (j - sample_j0) * sampleFrequency;

			//blend the top two corners
			float top = Interpolate(baseNoise[sample_i0][sample_j0],
				baseNoise[sample_i1][sample_j0], horizontal_blend);

			//blend the bottom two corners
			float bottom = Interpolate(baseNoise[sample_i0][sample_j1],
				baseNoise[sample_i1][sample_j1], horizontal_blend);

			//final blend
			smoothNoise[i][j] = Interpolate(top, bottom, vertical_blend);
		}
	}

	return smoothNoise;
}

float Interpolate(float x0, float x1, float alpha){
	return x0 * (1 - alpha) + alpha * x1;
}

vector<vector<float>> GeneratePerlinNoise(vector<vector<float>> baseNoise, int octaveCount){
	int width = baseNoise.size();
	int height = baseNoise[0].size();

	vector<vector<vector<float>>> smoothNoise; //an array of 2D arrays containing
	smoothNoise.resize(octaveCount);

	float persistance = 0.5f;

	//generate smooth noise
	for (int i = 0; i < octaveCount; i++)
	{
		smoothNoise[i] = GenerateSmoothNoise(baseNoise, i);
	}

	vector<vector<float>> perlinNoise;
	perlinNoise.resize(height);

	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;

	//blend noise together
	for (int octave = octaveCount - 1; octave >= 0; octave--)
	{
		amplitude *= persistance;
		totalAmplitude += amplitude;

		for (int i = 0; i < height; i++)
		{
			perlinNoise[i].resize(width);
			for (int j = 0; j < width; j++)
			{
				perlinNoise[i][j] += smoothNoise[octave][i][j] * amplitude;
			}
		}
	}

	//normalization
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			perlinNoise[i][j] /= totalAmplitude;
		}
	}

	return perlinNoise;
}
//
//Color GetColor(Color gradientStart, Color gradientEnd, float t)
//{
//	float u = 1 - t;
//
//	Color color = Color.FromArgb(
//		255,
//		(int)(gradientStart.R * u + gradientEnd.R * t),
//		(int)(gradientStart.G * u + gradientEnd.G * t),
//		(int)(gradientStart.B * u + gradientEnd.B * t));
//
//	return color;
//}

unsigned char *MapGradient(vector<vector<float>> perlinNoise)
{
	int width = perlinNoise.size();
	int height = perlinNoise[0].size();

	unsigned char *image = new unsigned char[height * width];

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			image[i * height + j] = (unsigned char) (perlinNoise[i][j] * 255);
		}
	}

	return image;
}