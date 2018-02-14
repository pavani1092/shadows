#include "OpenGL.h"

#include <string>
#include <vector>

// Includes to create the cube
#include "ISceneNode.h"
#include "ModelManager.h"
#include "DrawableProxy.h"
#include "DrawableCubeSolution.h"
#include "DrawableSphereSolution.h"
#include "DrawableFloorSolution.h"
#include "DrawModeNode.h"
#include "DrawableOBJ.h"
#include "DrawableBrush.h"
#include "DrawableSquare.h"
// Material includes
#include "SolidColorMaterialSolution.h"
#include "ShadedMaterial.h"
#include "MaterialProxy.h"
#include "MaterialManager.h"
#include "ShaderConstantMaterial.h"
#include "Color.h"

#include "BlankTexture2D.h"
#include "BlankDepthTexture2D.h"
#include "TextureBinding.h"
#include "TextureBindManager.h"
#include "TextureBindManagerOpenGL.h"
#include "TextureDataImage.h"
#include "SamplerApplicator.h"
#include "SimpleShaderMaterial.h"
#include "TexParam2DNoMipMap.h"
#include "TexParam2DMipMap.h"

// Includes for the camera
#include "ExaminerCameraNode.h"
#include "PerspectiveTransformSolution.h"
#include "LookAtTransformSolution.h"
#include "ShaderConstantModelView.h"


// Lights
#include "PointLight.h"
#include "DirectionalLight.h"
#include "LightManager.h"
#include "ShaderConstantLights.h"

#include "RenderTargetProxy.h"
#include "RenderTarget.h"
#include "RenderManager.h"
#include "ClearFrameCommand.h"
#include "SwapCommand.h"
#include "SetBackgroundCommand.h"
// Includes for walking the scene graph
#include "DebugRenderVisitor.h"
#include "PrintSceneVisitor.h"
#include "DrawableQuad.h"
#include "CullFaceCommand.h"

// Interaction
std::vector<IMouseHandler*> mouseHandlers;
std::vector<IKeyboardHandler*> keyboardHandlers;


using namespace Crawfis::Graphics;
using namespace Crawfis::Math;
using namespace std;


ISceneNode* rootSceneNode;
ISceneNode* shadowSceneNode;
ISceneNode* shadowVarSceneNode;
ISceneNode* depthSceneNode;
ISceneNode* varSceneNode;

IVisitor* renderVisitor;
IVisitor* shadowRenderVisitor;
IVisitor* shadowVarRenderVisitor;
IVisitor* depthVisitor; 
IVisitor* varVisitor;

ITextureDataObject* shadowMapTexture;
ITextureDataObject* shadowMapTextureVar;


ExaminerCameraNode* examiner;
ExaminerCameraNode* shadowExaminer;
ExaminerCameraNode* shadowVarExaminer;
ExaminerCameraNode* depthExaminer;


int windowGUID;
int windowWidth;
int windowHeight; 
int sideViewSize = 20;

int counter = 0;
boolean shadowCreated;

void CreateGLUTWindow(std::string windowTitle)
{
	windowWidth = 800;
	windowHeight = 600;
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	windowGUID = glutCreateWindow(windowTitle.c_str());
}

void InitializeOpenGLExtensions()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		throw "Error initializing GLEW";
	}

	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void InitializeDevices()
{
	CreateGLUTWindow("Pavani Komati");
	InitializeOpenGLExtensions();
	glDisable(GL_CULL_FACE);
}

void CreateLights()
{
	DirectionalLight* pointLight = new DirectionalLight("light0-pt");
	pointLight->setDirection(Vector3(10,1,1));

	LightManager::Instance()->SetLight(0, pointLight);
	DirectionalLight* dirLight = new DirectionalLight("light1-dir");
	//dirLight->setColor(Colors::IndianRed);
	dirLight->setDirection(Vector3(10, 1, 1));
	LightManager::Instance()->SetLight(1, dirLight);
}
void CreateFloorMaterial() {

	VertexRoutine* vertexRoutine = new VertexRoutine("..\\Media\\Shaders\\VertexLight.glsl");
	FragmentRoutine* fragmentRoutine = new FragmentRoutine("..\\Media\\Shaders\\Textured.frag");
	IShaderProgram* shaderProgram = new ShaderProgramWithMatrices(vertexRoutine, fragmentRoutine);

	Color gold(0.8314f, 0.144f, 0.69f, 1.0f);
	ShadedMaterial* shinyGold = new ShadedMaterial(shaderProgram);
	shinyGold->setAmbientReflection(0.01f*gold);
	shinyGold->setDiffuseReflection(0.7f*gold);
	shinyGold->setSpecularReflection(0.25f*gold);
	shinyGold->setShininess(100.0f);

	ShaderConstantMaterial* materialConstant = new ShaderConstantMaterial("frontMaterial");
	materialConstant->setValue(shinyGold);
	ShaderConstantLights* lightConstant = new ShaderConstantLights();
	ShaderConstantCollection* shaderConstantList = new ShaderConstantCollection();
	shaderConstantList->AddConstant(materialConstant);
	shaderConstantList->AddConstant(lightConstant);
	shaderProgram->setShaderConstant(shaderConstantList);

	MaterialManager::Instance()->RegisterMaterial("FloorMaterial", shinyGold);
}

void CreateTexturedMaterial(Color c, std::string name)
{
	ITextureDataObject* texture = new BlankTexture2D(1024, 1024);
	ITextureDataObject* redTexture = new BlankTexture2D(1024, 1024, Color(1, 0, 0, 1), GL_RGB);
	redTexture->setTextureParams(&TexParam2DNoMipMap::Instance);
	
	SamplerApplicator* uniformBinding1 = new SamplerApplicator("shadowSampler");
	TextureBinding* binding1 = TextureBindManager::Instance()->CreateBinding(shadowMapTexture, uniformBinding1);
	binding1->Enable();
	binding1->Disable();
	
	ITextureDataObject* imageTexture = new TextureDataImage("..\\Media\\Textures\\AmbientOcclusionMap.png", GL_RGB);
	imageTexture->setTextureParams(&TexParam2DNoMipMap::Instance);
	SamplerApplicator* uniformBinding = new SamplerApplicator("OcclusionMap");
	TextureBinding* binding = TextureBindManager::Instance()->CreateBinding(imageTexture, uniformBinding);
	binding->Enable();
	binding->Disable();
	
	ITextureDataObject* imageTexture2 = new TextureDataImage("..\\Media\\Textures\\dino_normals.png", GL_RGB);
	imageTexture2->setTextureParams(&TexParam2DNoMipMap::Instance);
	SamplerApplicator* uniformBinding2 = new SamplerApplicator("normalSampler");
	TextureBinding* binding2 = TextureBindManager::Instance()->CreateBinding(imageTexture2, uniformBinding2);
	binding2->Enable();
	binding2->Disable();



	VertexRoutine* vertexRoutine = new VertexRoutine("..\\Media\\Shaders\\ShadedTextured-vert.glsl");
	FragmentRoutine* fragmentRoutine = new FragmentRoutine("..\\Media\\Shaders\\Textured.frag");

	//UNCOMMENT FOR VARIANCE MAPPING
	/*VertexRoutine* vertexRoutine = new VertexRoutine("..\\Media\\Shaders\\ShadedTexturedVar.glsl");
	FragmentRoutine* fragmentRoutine = new FragmentRoutine("..\\Media\\Shaders\\TexturedVar.frag");*/

	IShaderProgram* shaderProgram = new ShaderProgramWithMatrices(vertexRoutine, fragmentRoutine);
	SimpleShaderMaterial* texturedMaterial = new SimpleShaderMaterial(shaderProgram);
	texturedMaterial->AddTexture(binding1);
	texturedMaterial->AddTexture(binding2);
	texturedMaterial->AddTexture(binding);
	//Color gold(0.8314f, 0.6863f, 0.2169f, 1.0f);
	ShadedMaterial* shinyGold = new ShadedMaterial(shaderProgram);
	shinyGold->setAmbientReflection(0.01f*c);
	shinyGold->setDiffuseReflection(0.7f*c);
	shinyGold->setSpecularReflection(0.25f*c);
	shinyGold->setShininess(100.0f);

	ShaderConstantMaterial* materialConstant = new ShaderConstantMaterial("frontMaterial");
	materialConstant->setValue(shinyGold);
	ShaderConstantLights* lightConstant = new ShaderConstantLights();
	ShaderConstantCollection* shaderConstantList = new ShaderConstantCollection();
	shaderConstantList->AddConstant(materialConstant);
	shaderConstantList->AddConstant(lightConstant);
	shaderConstantList->AddConstant(uniformBinding1);
	shaderConstantList->AddConstant(uniformBinding);
	shaderConstantList->AddConstant(uniformBinding2);
	
	shaderProgram->setShaderConstant(shaderConstantList);

	MaterialManager::Instance()->RegisterMaterial(name, texturedMaterial);
}

void createShadowmodel(std::string vertexPath, std::string fragmentPath, std::string materialName) {

	VertexRoutine* vertexRoutine = new VertexRoutine(vertexPath);
	FragmentRoutine* fragmentRoutine = new FragmentRoutine(fragmentPath);
	IShaderProgram* shaderProgram = new ShaderProgramWithMatrices(vertexRoutine, fragmentRoutine);
	SimpleShaderMaterial* shadowMaterial = new SimpleShaderMaterial(shaderProgram);

	MaterialManager::Instance()->RegisterMaterial(materialName, shadowMaterial);

}

ISceneNode* CreateShadowGraph(std::string shadowName) {
	

	DrawableOBJ* obj = new DrawableOBJ("..\\Media\\Obj\\dino.obj");
	ModelManager::Instance()->RegisterModel("Object", obj);

	DrawableFloor* floor = new DrawableFloor(10, 10);
	ModelManager::Instance()->RegisterModel("Floor", floor);



	DrawableProxy* objNode = new DrawableProxy("Object1", "Object");
	MaterialProxy* matNode = new MaterialProxy("Object Material", shadowName, objNode);
	TransformMatrixNodeSolution* objTransform1 = new TransformMatrixNodeSolution("Shadow Transform1", matNode);
	objTransform1->Translate(-0.2, -0.15, -0.3);
	//objTransform1->Rotate(45.0f, Vector3(0, 1, 0));
	objTransform1->Scale(6,5,5);


	DrawableProxy* objNode2 = new DrawableProxy("Object2", "Object");
	MaterialProxy* matNode2 = new MaterialProxy("Object Material", shadowName, objNode2);
	TransformMatrixNodeSolution* objTransform2 = new TransformMatrixNodeSolution("Shadow Transform2", matNode2);
	objTransform2->Translate(0.2, -0.15, 0.3);
	//objTransform2->Rotate(45.0f, Vector3(0, 1, 0));
	objTransform2->Scale(6,5,5);



	DrawableProxy* floorNode = new DrawableProxy("Floor", "Floor");
	MaterialProxy* floormat = new MaterialProxy("Floor Material", shadowName, floorNode);
	TransformMatrixNodeSolution* floorTransform = new TransformMatrixNodeSolution("Shadow Transform", floormat);
	floorTransform->Translate(0.0, -2.5, 0.0);
	floorTransform->Scale(10, 1, 10);


	GroupNode* group = new GroupNode("DepthMap");
	group->AddChild(objTransform1);
	group->AddChild(objTransform2);
	group->AddChild(floorTransform);
	if (shadowName == "ShadowVar") {
		shadowVarExaminer = new ExaminerCameraNode("ShadowVar Examiner", group, Vector3(5, 2, 3));
		shadowVarExaminer->setWidth(windowWidth);
		shadowVarExaminer->setHeight(windowHeight);

		return shadowVarExaminer;
	}

	shadowExaminer = new ExaminerCameraNode("Shadow Examiner", group, Vector3(5, 2, 3));
	shadowExaminer->setWidth(windowWidth);
	shadowExaminer->setHeight(windowHeight);

	return shadowExaminer;

}

void CreateSimpleMaterial(ITextureDataObject* texture, string name)
{
	SamplerApplicator* uniformBinding = new SamplerApplicator("texture");
	TextureBinding* binding = TextureBindManager::Instance()->CreateBinding(texture, uniformBinding);
	binding->Enable();
	binding->Disable();


	VertexRoutine* vertexRoutine = new VertexRoutine("..\\Media\\Shaders\\SimpleVertex.vert");
	FragmentRoutine* fragmentRoutine = new FragmentRoutine("..\\Media\\Shaders\\SimpleFragment.frag");
	IShaderProgram* shaderProgram = new ShaderProgramWithMatrices(vertexRoutine, fragmentRoutine);

	SimpleShaderMaterial* simpleMaterial = new SimpleShaderMaterial(shaderProgram);
	shaderProgram->setShaderConstant(uniformBinding);
	simpleMaterial->AddTexture(binding);

	MaterialManager::Instance()->RegisterMaterial(name, simpleMaterial);

}

ISceneNode* CreateShowShadowSceneGraph()
{



	GroupNode* mainGroup = new GroupNode("ShadowScene");


	DrawableQuad* TexObj = new DrawableQuad();
	ModelManager::Instance()->RegisterModel("ShadowTexObj", TexObj);

	DrawableProxy* texObjNode = new DrawableProxy("ShadowTexObj", "ShadowTexObj");

	CreateSimpleMaterial(shadowMapTexture, "shadowTexture");


	MaterialProxy* texObjMaterial = new MaterialProxy("Shadow Tex Material", "shadowTexture", texObjNode);
	TransformMatrixNodeSolution* texObjTransform = new TransformMatrixNodeSolution("ShadowTexSpace", texObjMaterial);

	mainGroup->AddChild(texObjTransform);
	return mainGroup;
}

ISceneNode* CreateShowVarShadowSceneGraph()
{



	GroupNode* mainGroup = new GroupNode("Scene");


	DrawableQuad* TexObj = new DrawableQuad();
	ModelManager::Instance()->RegisterModel("TexObj", TexObj);

	DrawableProxy* texObjNode = new DrawableProxy("TexObj", "TexObj");

	CreateSimpleMaterial(shadowMapTextureVar, "varShadow");


	MaterialProxy* texObjMaterial = new MaterialProxy("Tex Material", "varShadow", texObjNode);
	TransformMatrixNodeSolution* texObjTransform = new TransformMatrixNodeSolution("TexSpace", texObjMaterial);

	mainGroup->AddChild(texObjTransform);
	return mainGroup;
}



ISceneNode* CreateSceneGraph()
{
	Color gold(0.8314f, 0.6863f, 0.2169f, 1.0f);
	Color c1(0.1f, 0.6863f, 0.2169f, 1.0f);
	CreateTexturedMaterial(gold, "goldMat");
	CreateTexturedMaterial(c1, "c1Mat");
	CreateLights();
	DrawableProxy* objNode = new DrawableProxy("Object1", "Object");
	MaterialProxy* matNode = new MaterialProxy("Object Material", "c1Mat", objNode);
	TransformMatrixNodeSolution* objTransform1 = new TransformMatrixNodeSolution("Object Transform1", matNode);
	objTransform1->Translate(-0.2, -0.15, -0.3);
	//objTransform1->Rotate(45.0f, Vector3(0, 1, 0));
	objTransform1->Scale(6, 5, 5);


	DrawableProxy* objNode2 = new DrawableProxy("Object2", "Object");
	MaterialProxy* matNode2 = new MaterialProxy("Object Material", "goldMat", objNode2);
	TransformMatrixNodeSolution* objTransform2 = new TransformMatrixNodeSolution("Object Transform2", matNode2);
	objTransform2->Translate(0.2, -0.15, 0.3);
	//objTransform2->Rotate(45.0f, Vector3(0, 1, 0));
	objTransform2->Scale(6, 5, 5);

	CreateFloorMaterial();
	DrawableProxy* floorNode = new DrawableProxy("Floor", "Floor");
	MaterialProxy* floormat = new MaterialProxy("Floor Material", "FloorMaterial", floorNode);
	TransformMatrixNodeSolution* floorTransform = new TransformMatrixNodeSolution("Floor Transform", floormat);
	floorTransform->Translate(0.0, -2.5, 0.0);
	floorTransform->Scale(8, 1, 8);

	
	GroupNode* group = new GroupNode("Shadowed");
	group->AddChild(objTransform1);
	group->AddChild(objTransform2);
	group->AddChild(floorTransform);
	
	examiner = new ExaminerCameraNode("Examiner", group,Vector3 (5,2,3));
	examiner->setWidth(windowWidth);
	examiner->setHeight(windowHeight);
	
	return examiner;
}





void DisplayFrame()
{
	glEnable(GL_SCISSOR_TEST);
	
	
	int viewportSize = 180;
	
	glViewport(0, 0, 600, 600);
	glScissor(0, 0, 600, 600);
	shadowSceneNode->Accept(shadowRenderVisitor);

	glViewport(0, 0, 600, 600);
	glScissor(0, 0, 600, 600);
	shadowVarSceneNode->Accept(shadowVarRenderVisitor);
	
	glViewport(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 1 * viewportSize - 1 * sideViewSize, viewportSize, viewportSize);
	glScissor(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 1 * viewportSize - 1 * sideViewSize, viewportSize, viewportSize);
	varSceneNode->Accept(varVisitor);
	
	glViewport(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 2 * viewportSize - 2 * sideViewSize, viewportSize, viewportSize);
	glScissor(windowWidth - viewportSize - sideViewSize + 10, windowHeight - 2 * viewportSize - 2 * sideViewSize, viewportSize, viewportSize);
	depthSceneNode->Accept(depthVisitor);
	
	glViewport(0, 0, 600, 600);
	glScissor(0, 0, 600, 600);
	rootSceneNode->Accept(renderVisitor);


}

void ReshapeWindow(int newWidth, int newHeight)
{
	windowWidth = newWidth;
	windowHeight = newHeight;
	examiner->setWidth(windowWidth);
	examiner->setHeight(windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glutPostRedisplay();
}

ISceneNode* CreateFrameBuffer(Crawfis::Graphics::ISceneNode * scene)
{
	IRenderTarget* screen = new RenderTarget();
	RenderManager::Instance()->RegisterRenderTarget("Screen", screen);
	screen->setEnableCommand(new ClearFrameCommand(Colors::Blue));
	screen->setEnableCommand(new SetBackgroundCommand());
	screen->setDisableCommand(new SwapCommand(true));
	RenderTargetProxy* frameBuffer = new RenderTargetProxy("Screen Display", "Screen", scene);
	return frameBuffer;
}

ISceneNode* CreateFrameBuffer(Crawfis::Graphics::ISceneNode * scene, string name, bool isSwap, bool hasBg, Crawfis::Graphics::Color color)
{

	IRenderTarget* screen = new RenderTarget();
	RenderManager::Instance()->RegisterRenderTarget(name, screen);
	if (hasBg) {
		screen->setEnableCommand(new SetBackgroundCommand(color));//
	}
	else {
		ClearFrameCommand* c = new ClearFrameCommand(color);
		c->setName(name);
		screen->setEnableCommand(c);

	}

	screen->setDisableCommand(new SwapCommand(isSwap));

	RenderTargetProxy* frameBuffer = new RenderTargetProxy("Screen Display", name, scene);
	return frameBuffer;
}

void KeyboardController(unsigned char key, int x, int y)
{
	printf("Key Pressed: %c\n", key);
	std::vector<IKeyboardHandler*>::iterator handlerIterator;
	for (handlerIterator = keyboardHandlers.begin(); handlerIterator != keyboardHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->KeyPress(key, x, y);
	}
	glutPostRedisplay();
}

void NumPadController(int key, int x, int y)
{
	std::vector<IKeyboardHandler*>::iterator handlerIterator;
	for (handlerIterator = keyboardHandlers.begin(); handlerIterator != keyboardHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->NumPadPress(key, x, y);
	}
	glutPostRedisplay();
}

void MousePressController(int button, int state, int ix, int iy)
{
	std::vector<IMouseHandler*>::iterator handlerIterator;
	for (handlerIterator = mouseHandlers.begin(); handlerIterator != mouseHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->MouseEvent(button, state, ix, iy);
	}
	glutPostRedisplay();
}

void MouseMotionController(int ix, int iy)
{
	std::vector<IMouseHandler*>::iterator handlerIterator;
	for (handlerIterator = mouseHandlers.begin(); handlerIterator != mouseHandlers.end(); handlerIterator++)
	{
		(*handlerIterator)->MouseMoved(ix, iy);
	}
	glutPostRedisplay();
}

void IdleCallback()
{
}
void InitializeDevIL()
{
	::ilInit();
	::iluInit();
	::ilutInit();
	
}

void initMainScene() {
	ISceneNode* scene = CreateSceneGraph();
	rootSceneNode = CreateFrameBuffer(scene, "mainScene", true, true, Colors::Grey);
	renderVisitor = new DebugRenderVisitor();
	PrintSceneVisitor* printScene = new PrintSceneVisitor();
	rootSceneNode->Accept(printScene);
	keyboardHandlers.push_back(examiner);
	mouseHandlers.push_back(examiner);
}
void initShadowScene() {
	createShadowmodel("..\\Media\\Shaders\\Shadow.vert", "..\\Media\\Shaders\\Shadow.frag","Shadow");
	ISceneNode* scene = CreateShadowGraph("Shadow");
	//////////////////////////
	shadowMapTexture = new BlankDepthTexture2D(600, 600);
	shadowMapTexture->setName("shadowMap");
	shadowMapTexture->setTextureParams(&TexParam2DNoMipMap::Instance);
	shadowMapTexture->Enable();

	IRenderTarget* target = new RenderTarget(shadowMapTexture);
	RenderManager::Instance()->RegisterRenderTarget("shadowMap", target);
	ClearFrameCommand* c = new ClearFrameCommand(Colors::Brown);
	c->setName("shadowMap");
	target->setEnableCommand(c);
	target->setEnableCommand(new CullFaceCommand(GL_FRONT));
	target->setDisableCommand(new SwapCommand(true));
	target->setDisableCommand(new CullFaceCommand(GL_BACK));
	shadowSceneNode = new RenderTargetProxy("Display", "shadowMap", scene);

	///////////////////////////
	shadowRenderVisitor = new DebugRenderVisitor();
	PrintSceneVisitor* printScene = new PrintSceneVisitor();
	shadowSceneNode->Accept(printScene);
	keyboardHandlers.push_back(shadowExaminer);
	mouseHandlers.push_back(shadowExaminer);
}

void initVarianceShadowScene() {
	createShadowmodel("..\\Media\\Shaders\\ShadowVar.vert", "..\\Media\\Shaders\\ShadowVar.frag", "ShadowVar");
	ISceneNode* scene = CreateShadowGraph("ShadowVar");
	//////////////////////////
	shadowMapTextureVar = new BlankTexture2D(600, 600);
	shadowMapTextureVar->setName("shadowMapVar");
	shadowMapTextureVar->setTextureParams(&TexParam2DNoMipMap::Instance);
	shadowMapTextureVar->Enable();

	IRenderTarget* target = new RenderTarget(shadowMapTextureVar);
	RenderManager::Instance()->RegisterRenderTarget("shadowMapVar", target);
	ClearFrameCommand* c = new ClearFrameCommand(Colors::Brown);
	c->setName("shadowMapVar");
	target->setEnableCommand(c);
	target->setDisableCommand(new SwapCommand(true));
	shadowVarSceneNode = new RenderTargetProxy("Display", "shadowMapVar", scene);

	///////////////////////////
	shadowVarRenderVisitor = new DebugRenderVisitor();
	PrintSceneVisitor* printScene = new PrintSceneVisitor();
	shadowVarSceneNode->Accept(printScene);
	keyboardHandlers.push_back(shadowVarExaminer);
	mouseHandlers.push_back(shadowVarExaminer);
}

void initShowShadowScene() {
	ISceneNode* scene = CreateShowShadowSceneGraph();
	depthSceneNode = CreateFrameBuffer(scene, "ShadowScene", false, false, Colors::Blue);
	depthVisitor = new DebugRenderVisitor();
	PrintSceneVisitor* printScene = new PrintSceneVisitor();
	depthSceneNode->Accept(printScene);
	//keyboardHandlers.push_back(wfExaminer);
	//mouseHandlers.push_back(wfExaminer);
}

void initShowVarShadowScene() {
	ISceneNode* scene = CreateShowVarShadowSceneGraph();
	varSceneNode = CreateFrameBuffer(scene, "ShadowSceneVar", false, false, Colors::Blue);
	varVisitor = new DebugRenderVisitor();
	PrintSceneVisitor* printScene = new PrintSceneVisitor();
	varSceneNode->Accept(printScene);
	//keyboardHandlers.push_back(wfExaminer);
	//mouseHandlers.push_back(wfExaminer);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	InitializeDevices();
	InitializeDevIL();
	TextureBindManagerOpenGL::Init();

	MatrixStack::Init();
	initShadowScene();
	initVarianceShadowScene();

	initMainScene();
	initShowShadowScene();
	initShowVarShadowScene();
	
	glutReshapeWindow(800, 600);

	glutDisplayFunc(DisplayFrame);
	glutReshapeFunc(ReshapeWindow);
	glutKeyboardFunc(KeyboardController);
	glutSpecialFunc(NumPadController);
	glutMouseFunc(MousePressController);
	glutMotionFunc(MouseMotionController);

	glutMainLoop();

	return 0;
}