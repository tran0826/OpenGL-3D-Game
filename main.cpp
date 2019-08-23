#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//コンソール非表示
#include"Header.h"
using namespace std;
Player* player;
DisplayManage* displaymanage;
ProcessManage* processmanage;
bool GLEW_INIT()
{
	GLenum err;
	err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << '\n';
		return false;
	}
	return true;
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//ウィンドウを塗りつぶす　引数で塗りつぶすバッファを指定
	displaymanage->display();
	glutSwapBuffers();
}

void init(void) {
	glClearColor(0.0/255.0, 91.0/255.0, 225.0/255.0, 0.0);//塗りつぶす際の色指定
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	displaymanage->init();
}

void resize(int w, int h) {
	player->resize(w, h);
}

void Idle(int t){
	processmanage->process();
	player->idle();
	glutPostRedisplay();
	glutTimerFunc(16, Idle, 1);
	//LoadGraphScreen(0, 0, "data/キーボード.png", TRUE);
}

void Keyboard(unsigned char key, int x, int y) {
	player->Keyboard(key,x,y);
}

void Keyboardup(unsigned char key, int x, int y) {
	player->Keyboardup(key,x,y);
}

void motion(int x, int y)
{		
	player->motion(x, y);
}

void mouse(int button, int state, int x, int y)
{
	player->mouse(button, state, x, y);
}

void wheel(int wheel_number, int direction, int x, int y) {
	player->wheel(wheel_number, direction, x, y);
}

int main(int argc, char*argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);



	SetNotWinFlag(true);
	SetAlwaysRunFlag(TRUE);//非アクティブ時も処理を続行
	SetOutApplicationLogValidFlag(FALSE);
	ChangeWindowMode(TRUE);
	SetMainWindowText("操作説明");
	SetWindowInitPosition(1280,  0);
	SetGraphMode(600, 720, 32);


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("POTENTIAL CREATER");
	GLEW_INIT(); //glewの初期化

	FILE* save;
	int level;

	player = new Player;
	displaymanage = new DisplayManage;
	processmanage = new ProcessManage(displaymanage,player);
	
	if (fopen_s(&save, "data/save.sav", "r") == 0) {
		fscanf(save, "%d", &level);
		printf("%d", level);
		player->Changelevel(level);
		fclose(save);
	}

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(Keyboard);//キーボード入力時に呼び出される関数を指定する（関数名：Keyboard）
	glutKeyboardUpFunc(Keyboardup);
//	glutIgnoreKeyRepeat(GL_TRUE);	// int ignore
	glutMouseFunc(mouse);
//	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutMouseWheelFunc(wheel);
	glutTimerFunc(16, Idle, 1);

	glutSetCursor(GLUT_CURSOR_NONE);

	Sound bgm("data/thermometer.wav");
	bgm.ChangeVolume(150);
	bgm.Soundplay(DX_PLAYTYPE_LOOP);

	init();
	
	glutMainLoop();

	DxLib_End();
	return 0;
}