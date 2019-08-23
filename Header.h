#pragma once
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include<glew.h>
#include<glut.h>
#include<freeglut.h>
#include<stdio.h>
#include<math.h>
#include <iostream>
#include<vector>
#include<mmsystem.h>
#include<stdlib.h>
#include<time.h>
#include<DxLib.h>

#include"Obj.h";
using namespace std;
#define MAPCHANGE 25 //マップ切り替えする幅
#define MAPWID 150 //表示するマップの大きさ
#define MAPLENGTH 150
#define MENUHIGH 120 //メニューの縦の長さ
#define MINIMAP 100 //minimapの長さ
#define ENN 50.0 //円の分割数
#define SPACE 10.0
#define VERCOUNT (90/SPACE)*(360/SPACE)*4
#define SKYWID 200.0
#define SKYHIGH 20.0
#define CLOUD 0
#define SUNNY 1
#define TIMEUNIT 0.001
#define CHARGESPEED 0.4//電荷溜める速さ


typedef struct
{
	double X;
	double Y;
	double Z;
	double U;
	double V;
}VERTICES;

class Sound {
private:
	MCI_OPEN_PARMS mop;
	int SHandle;
	double timer = 0;
public:
	int flag = 0;
	Sound(const char* path) {
		SHandle = LoadSoundMem(path);
	}
	~Sound() {
		DeleteSoundMem(SHandle);
	}
	void Soundplay(int PlayType) {
		
		if (flag == 0&&PlayType==DX_PLAYTYPE_LOOP) {
			PlaySoundMem(SHandle, DX_PLAYTYPE_LOOP, TRUE);
			flag = 1;
		}
		else if (flag == 0 && PlayType == DX_PLAYTYPE_BACK) {
			PlaySoundMem(SHandle, DX_PLAYTYPE_BACK, TRUE);
			flag = 1;
		}
	}
	void Soundstop() {
		if (flag == 1) {
			StopSoundMem(SHandle);
			flag = 0;
		}
	}
	void ChangeVolume(int vol) {
		ChangeVolumeSoundMem(vol, SHandle);
	}
};

class Display {
public:
	virtual void render(void)=0;
	virtual void init(){}
};

class picture {//RAW画像読み込み
private:
	char *pic;
	int wid, high;
	int channel;
public:
	picture(const char *path, int w, int h, int nrElement) {
		FILE *fp;
		if (fopen_s(&fp, path, "rb") == 0) {
			cout << "成功";
		}

		// 画像サイズ
		int sz = w * h * nrElement;

		// バッファ確保
		pic = new char[sz];

		// ヘッダ(余計な情報)がないため fread() のみで読める
		fread(pic, sz, 1, fp);

		fclose(fp);
		wid = w; high = h; channel = nrElement;
	}
	void render_windowpos(double x, double y,double z) {
//		glDisable(GL_DEPTH_TEST);
		glWindowPos3f(x, y, z);
		if (channel == 3) {
			glDrawPixels(wid, high, GL_RGB, GL_UNSIGNED_BYTE, pic);
		}
		else if (channel == 4) {
			glDrawPixels(wid, high, GL_RGBA, GL_UNSIGNED_BYTE, pic);
		}
//		glEnable(GL_DEPTH_TEST);
	}
	void render_rasterpos(double x, double y, double z) {
		glRasterPos3d(x, y, z);
		if (channel == 3) {
			glDrawPixels(wid, high, GL_RGB, GL_UNSIGNED_BYTE, pic);
		}
		else if (channel == 4) {
			glDrawPixels(wid, high, GL_RGBA, GL_UNSIGNED_BYTE, pic);
		}
	}
	char* Getpic() { return pic; }
	int Getchannel() { return channel; }
	int Getw() { return wid; }
	int Geth() { return high; }
};

class texture2d :public picture {
private:
	GLuint texname;
public:
	texture2d(const char *path, int w, int h, int nrElement) :picture(path, w, h, nrElement) {	}
	void texinit() {
		glGenTextures(1, &texname);
		glBindTexture(GL_TEXTURE_2D, texname);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		if (Getchannel() == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Getw(), Geth(), 0, GL_RGBA, GL_UNSIGNED_BYTE, Getpic());

		}
		if (Getchannel() == 3)glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Getw(), Geth(), 0, GL_RGB, GL_UNSIGNED_BYTE, Getpic());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	void Selecttex() {
		glBindTexture(GL_TEXTURE_2D, texname);
	}
	void rendertex(const double* locate) {

		//		/* テクスチャマッピング開始 
		glEnable(GL_TEXTURE_2D);
		Selecttex();
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 1.0);
		glVertex3d(locate[0], locate[1], locate[2]);
		glTexCoord2d(1.0, 1.0);
		glVertex3d(locate[3], locate[4], locate[5]);
		glTexCoord2d(1.0, 0.0);
		glVertex3d(locate[6], locate[7], locate[8]);
		glTexCoord2d(0.0, 0.0);
		glVertex3d(locate[9], locate[10], locate[11]);
		glEnd();

		//* テクスチャマッピング終了 
		glDisable(GL_TEXTURE_2D);
	}
	
};

class Gravity:public Display {
public:
	double x, y,z;
	double m;
	double time;
	Gravity(double _x, double _z, double _m) {
		x = _x; z = _z; m = _m; time = 0; y = 0;
	}
	void render() {
		glColor3d(m, 0, -m);

		glPushMatrix();
		glTranslated(x, y, z);
		glPushMatrix();
		glRotated(0.3*time, 1, 1, 0);
		glBegin(GL_LINE_STRIP);
		int extend = GetRand(100);
		for (int i = 0; i <= ENN; i++) {
			if (i ==extend) {
				double biribiri = GetRand(2)+1;
				glVertex3d(biribiri*0.2*m*sin(2 * M_PI*(double)i / ENN), 0, biribiri*0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
			else {
				glVertex3d(0.2*m*sin(2 * M_PI*(double)i / ENN), 0, 0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
		}
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glRotated(0.51*time, 0, 1, 1);
		glBegin(GL_LINE_STRIP);
		extend = GetRand(100);
		for (int i = 0; i <= ENN; i++) {
			if (i == extend) {
				double biribiri = GetRand(2) + 1;
				glVertex3d(biribiri*0.2*m*sin(2 * M_PI*(double)i / ENN), 0, biribiri*0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
			else {
				glVertex3d(0.2*m*sin(2 * M_PI*(double)i / ENN), 0, 0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
		}
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glRotated(0.64*time, 1, 0, 1);
		glBegin(GL_LINE_STRIP);
		extend = GetRand(100);
		for (int i = 0; i <= ENN; i++) {
			if (i == extend) {
				double biribiri = GetRand(2) + 1;
				glVertex3d(0.2*m*sin(biribiri*2 * M_PI*(double)i / ENN), 0, biribiri*0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
			else {
				glVertex3d(0.2*m*sin(2 * M_PI*(double)i / ENN), 0, 0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
		}
		glEnd();
		glPopMatrix();

		glPopMatrix();
		time += 10;
	}
	void Changecoordinate(double _x, double _y,double _z) {
		x = _x; z = _z; y = _y;
	}
};

class ShotGravity :public Gravity {
public:
	double shottime;
	double wxz;
	ShotGravity(double _x, double _y, double _z, double _m,double _wxz) : Gravity(_x, _z, _m) {
		y = _y; wxz = _wxz; shottime = 0;
	}
	void render() {
		glColor3d(m, 0, -m);

		glPushMatrix();
		glTranslated(x, y, z);
		glPushMatrix();
		glRotated(0.3*time, 1, 1, 0);
		glBegin(GL_LINE_STRIP);
		int extend = GetRand(100);
		for (int i = 0; i <= ENN; i++) {
			if (i == extend) {
				double biribiri = GetRand(2) + 1;
				glVertex3d(biribiri*0.2*m*sin(2 * M_PI*(double)i / ENN), 0, biribiri*0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
			else {
				glVertex3d(0.2*m*sin(2 * M_PI*(double)i / ENN), 0, 0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
		}
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glRotated(0.51*time, 0, 1, 1);
		glBegin(GL_LINE_STRIP);
		extend = GetRand(100);
		for (int i = 0; i <= ENN; i++) {
			if (i == extend) {
				double biribiri = GetRand(2) + 1;
				glVertex3d(biribiri*0.2*m*sin(2 * M_PI*(double)i / ENN), 0, biribiri*0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
			else {
				glVertex3d(0.2*m*sin(2 * M_PI*(double)i / ENN), 0, 0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
		}
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glRotated(0.64*time, 1, 0, 1);
		glBegin(GL_LINE_STRIP);
		extend = GetRand(100);
		for (int i = 0; i <= ENN; i++) {
			if (i == extend) {
				double biribiri = GetRand(2) + 1;
				glVertex3d(0.2*m*sin(biribiri * 2 * M_PI*(double)i / ENN), 0, biribiri*0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
			else {
				glVertex3d(0.2*m*sin(2 * M_PI*(double)i / ENN), 0, 0.2*m*cos(2 * M_PI* (double)i / ENN));
			}
		}
		glEnd();
		glPopMatrix();

		glPopMatrix();

		z += 5*cos(wxz) / 2.0;
		x += 5*sin(wxz) / 2.0;
		time += 10;
		shottime++;
	}
	void reset(double _x, double _y, double _z, double _m, double _wxz) {
		x = _x; y = _y; z = _z; wxz = _wxz; m = _m; shottime = 0;
	}
};

class Sphere :public Display {//星空＆昼夜
private:
	texture2d* texture;
	double x, y, z, r;
	VERTICES* Vertex;
	double time = 0;
public:
	Sphere(const char* path,int w,int h,int nrElement,double _r) {
		texture = new texture2d(path, w, h, nrElement);
		x = y = z = 0;
		r = _r;
		int n = 0;
		double a, b;
		Vertex = new VERTICES[(int)VERCOUNT];
		for (b = 0; b <= 90 - SPACE; b += SPACE) {
			for (a = 0; a <= 360 - SPACE; a += SPACE) {
				Vertex[n].X = r * sin((a) / 180.0 * M_PI) * sin((b) / 180.0 * M_PI);
				Vertex[n].Y = r * cos((a) / 180.0 * M_PI) * sin((b) / 180.0 * M_PI);
				Vertex[n].Z = r * cos((b) / 180.0 * M_PI);
				Vertex[n].V = (2.0 * b) / 360.0;
				Vertex[n].U = (a) / 360.0;
				n++;
				Vertex[n].X = r * sin((a) / 180.0 * M_PI) * sin((b + SPACE) / 180.0 * M_PI);
				Vertex[n].Y = r * cos((a) / 180.0 * M_PI) * sin((b + SPACE) / 180.0 * M_PI);
				Vertex[n].Z = r * cos((b + SPACE) / 180.0 * M_PI);
				Vertex[n].V = (2.0 * (b + SPACE)) / 360.0;
				Vertex[n].U = (a) / 360.0;
				n++;
				Vertex[n].X = r * sin((a + SPACE) / 180.0 * M_PI) * sin((b) / 180.0 * M_PI);
				Vertex[n].Y = r * cos((a + SPACE) / 180.0 * M_PI) * sin((b) / 180.0 * M_PI);
				Vertex[n].Z = r * cos((b) / 180.0 * M_PI);
				Vertex[n].V = (2.0 * b) / 360.0;
				Vertex[n].U = (a + SPACE) / 360.0;
				n++;
				Vertex[n].X = r * sin((a + SPACE) / 180.0 * M_PI) * sin((b + SPACE) / 180.0 * M_PI);
				Vertex[n].Y = r * cos((a + SPACE) / 180.0 * M_PI) * sin((b + SPACE) / 180.0 * M_PI);
				Vertex[n].Z = r * cos((b + SPACE) / 180.0 * M_PI);
				Vertex[n].V = (2.0 * (b + SPACE)) / 360.0;
				Vertex[n].U = (a + SPACE) / 360.0;
				n++;
			}
		}
	}
	void init() {
		texture->texinit();
	}
	void render() {
		glEnable(GL_TEXTURE_2D);
		texture->Selecttex();
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glPushMatrix();
		glRotated(90, 1, 0, 0);
		glBegin(GL_TRIANGLE_STRIP);
		glColor4d(1, 1, 1, (cos(time-M_PI)+1.0)/2.0);
		for (int b = 0; b <= VERCOUNT-1; b++) {
			glTexCoord2f(Vertex[b].U, Vertex[b].V);
			glVertex3f(Vertex[b].X, Vertex[b].Y, -Vertex[b].Z);
		}
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		for (int b = 0; b <= VERCOUNT-1; b++) {
			glTexCoord2f(Vertex[b].U, -Vertex[b].V);
			glVertex3f(Vertex[b].X, Vertex[b].Y, Vertex[b].Z);
		}
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		time += TIMEUNIT;
		if (time > 2 * M_PI) {
			time = 0;
		}
	}
};//

class Sky :public Display{
private:
	texture2d* texture;
	double timer;
	double rotate;
	int weather = CLOUD;
public:
	Sky(const char* path,int w,int h ,int nrElement) {
		texture = new texture2d(path,w,h,nrElement);
		timer = 0;
		rotate = 0;
	}
	void init() {
		texture->texinit();
	}
	void render() {
		glEnable(GL_TEXTURE_2D);
		texture->Selecttex();
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (cos(timer) < -0.99) {
			srand((unsigned)time(NULL));
			if (rand() % 10 > 2) {
				weather = CLOUD;
			}
			else {
				weather = SUNNY;
			}
		}
		if(weather==CLOUD)glColor4d(1, 1, 1, (cos(timer)+1.0)/2.0);
		if (weather == SUNNY)glColor4d(1, 1, 1, 0);
		glRotated(2*rotate, 0, 1.0, 0);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 1.0);
		glVertex3d(SKYWID, SKYHIGH, SKYWID);
		glTexCoord2d(1.0, 1.0);
		glVertex3d(SKYWID, SKYHIGH, -SKYWID);
		glTexCoord2d(1.0, 0.0);
		glVertex3d(-SKYWID, SKYHIGH, -SKYWID);
		glTexCoord2d(0.0, 0.0);
		glVertex3d(-SKYWID, SKYHIGH, SKYWID);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		timer += TIMEUNIT;
		rotate += 0.01;
	}
	int GetWeather() { return weather; }
};//雲

class Battery {
private:
	rxOBJ* _battery;
	vector<VERTICES> drop;
	double time = 0;
public:
	Battery() {
		_battery = new rxOBJ("data/battery.obj", "data/battery.png");
		drop.resize(100);
		for (int i = 0; i < drop.size(); i++) {
			drop[i].X = GetRand(2000) - 1000;
			drop[i].Z = GetRand(2000) - 1000;
			drop[i].Y = GetRand(50) - 25;
			drop[i].U = (double)(GetRand(9) + 1) / 10.0;
		}
	}
	void renderbattery(double x, double z) {
		glColor4d(1, 1, 1, 1);
		for (int i = 0; i < drop.size(); i++) {
			if (abs(drop[i].X - x)<150 && abs(drop[i].Z - z)<150) {
				glPushMatrix();
				glTranslated(drop[i].X, drop[i].Y, drop[i].Z);
				glRotated(time, 0, 1, 0);

				glScaled(drop[i].U, drop[i].U, drop[i].U);
				_battery->renderModel();
				glPopMatrix();
			}
		}
		time += 1;
	}
	int hitcheck(double x, double y,double z) {//当たったバッテリーの番号を返す 当たってなければサイズを返す
		int num;
		for (int i = 0; i < drop.size(); i++) {
			if (abs(drop[i].X - x) < 2 && abs(drop[i].Z - z) < 2&& abs(drop[i].Y - y)<2) {
				num = i;
				break;
			}
			num = drop.size();
		}
		return num;
	}
	int Getdropsize() { return drop.size(); }
	double reload(int i,int level) {//指定したバッテリーを排除、その大きさを返し、新しいバッテリーを配置　レベルにより深度増
		double tempx = drop[i].X;
		double tempu = drop[i].U;
		drop.erase(drop.begin() + i);
		VERTICES temp;
		while (1) {
			temp.X = GetRand(2000) - 1000;
			temp.Z = GetRand(2000) - 1000;
			if (abs(temp.X - tempx) > 150) {
				break;
			}
		}
		temp.Y = GetRand(100+level*2) - 50-level;
		temp.U = (double)(GetRand(15+(int)(level/2))) / 10.0;
		drop.push_back(temp);
		return tempu;
	}
	~Battery() {
		delete _battery;
	}
};

class Player {
private:

	

	double x, y, z;//coordinate
	double wxz, wy;//direction
	double jump;
	double zoom;
	double v;//移動速度
	GLint savepoint[2] = { 0,0 };
	
	vector<Gravity> gravity;
	ShotGravity* shotgravity;
	Battery* battery;
	Sound* footsteps;
	Sound* walk;
	Sound* charge;
	Sound* magic;
	Sound* reload;
	Sound* shot;
	Sound* hit;
	bool cleep = false;//creepな、正しくは
	int flag = 0;
	int jumpflag = 0;
	int nowjump = 0;
	double Level = 1;
	double speed = 0;
	bool canstore = true;
	double usepower = 0;//ポテンシャル計算するフラグ管理用　変数名は気にしない
	double storepower = 0;//現在溜めている量
	double nowpower = 55;//残りMP的なノリ
	double maxpower = 55;
	int Wid, High;
	int keyw, keya, keys, keyd, keyq, keye;
	int damagecount;
public:
	Player() {
		damagecount = 0;
		x = y = z = 0;
		jump = 0;
		wy = 0; wxz = -M_PI;
		zoom = 30;
		v = 0.4;
		Wid = 1280; High = 720;
		keyw = keya = keys = keyd = keyq = keye = 0;
		footsteps = new Sound("data/dash.wav");
		walk = new Sound("data/walk.wav");
		charge = new Sound("data/charge.wav");
		charge->ChangeVolume(150);
		magic = new Sound("data/magic.wav");
		magic->ChangeVolume(150);
		reload = new Sound("data/reload.wav");
		shot = new Sound("data/shot.wav");
		hit = new Sound("data/hit.wav");
		shot->ChangeVolume(150);
		shotgravity = new ShotGravity(0, 0, 0, 0, 0);
	}
	void resize(int w, int h) {
		/* ウィンドウ全体をビューポートにする←してない */
		glViewport(0,MENUHIGH, w, h);

		glMatrixMode(GL_PROJECTION);
		/* 変換行列の初期化 */
		glLoadIdentity();

		gluPerspective(zoom, (double)w / (double)(h - 100), 1.0, 1000.0);//画角,アスペクト比,奥行き方向の表示範囲
		gluLookAt(x, y, z, x + sin(wxz), y + sin(wy), z + cos(wxz), 0.0, 1.0, 0.0);//視点の位置,視界の方向,上方向のベクトル
		glMatrixMode(GL_MODELVIEW);
		Wid = w; High = h;
//		cout <<"座標"<< x << ',' << y << ',' << z << endl;
	}
	void idle() {
		if (damagecount > 0) {
			damagecount--;
		}
		glMatrixMode(GL_PROJECTION);
		/* 変換行列の初期化 */
		glLoadIdentity();

		gluPerspective(zoom, (double)Wid / (double)(High - 100), 1.0, 1000.0);//画角,アスペクト比,奥行き方向の表示範囲
		gluLookAt(x, y+jump-cleep, z, x + sin(wxz)*cos(wy), y+jump -cleep+ sin(wy), z + cos(wxz)*cos(wy), 0.0, 1.0, 0.0);//視点の位置,視界の方向,上方向のベクトル
		glMatrixMode(GL_MODELVIEW);

//		cout <<"座標"<< x << ',' << y << ',' << z << endl;

		if (gravity.size() > 15) {
			gravity.erase(gravity.begin());
		}

		maxpower = 50.0 + Level * 5.0;//
		int hitnum = battery->hitcheck(x, y + jump, z);
		if ( hitnum != battery->Getdropsize()) {
			Level+=1.5*(battery->reload(hitnum,Level));
			
			nowpower += maxpower/(2.0+Level/100.0);
			reload->Soundplay(DX_PLAYTYPE_BACK);
			reload->flag = 0;
		}

//		cout << "LEVEL" << Level<<",";
	//	cout << "max" << maxpower << ",";
		nowpower += maxpower*0.0003;//毎フレーム少しずつMP回復
		if (nowpower > maxpower)nowpower = maxpower;

		if (keyw == 1) {
			z += v*cos(wxz)/2.0;
			x += v*sin(wxz)/2.0;
			flag = 1;
			if (!cleep) {
				z += v*cos(wxz) / 2.0;
				x += v*sin(wxz) / 2.0;
			}
		}
		if (keya == 1) {
			x += v*cos(-wxz)/2.0;
			z += v*sin(-wxz)/2.0;
			flag = 1;
			if (!cleep) {
				x += v*cos(-wxz) / 2.0;
				z += v*sin(-wxz) / 2.0;
			}
		}
		if (keys == 1) {
			z -= v*cos(wxz)/2.0;
			x -= v*sin(wxz)/2.0;
			flag = 1;
			if (!cleep) {
				z -= v*cos(wxz) / 2.0;
				x -= v*sin(wxz) / 2.0;
			}
		}
		if (keyd == 1) {
			x -= v*cos(-wxz)/2.0;
			z -= v*sin(-wxz)/2.0;
			flag = 1;
			if (!cleep) {
				x -= v*cos(-wxz) / 2.0;
				z -= v*sin(-wxz) / 2.0;
			}
		}
		if (flag == 1&&nowjump==0) {//足音再生
			footsteps->Soundplay(DX_PLAYTYPE_LOOP);
			flag = 0;
			if (cleep) {
				footsteps->Soundstop();
				walk->Soundplay(DX_PLAYTYPE_LOOP);
			}
			else {
				if (walk->flag == 1) {
					walk->Soundstop();
				}
			}
		}
		else  {
			walk->Soundstop();
			footsteps->Soundstop();
		}
		flag = 0;
		if (keyq == 1&&canstore==true) {
			storepower+=CHARGESPEED;
			if (storepower > nowpower)storepower = nowpower;
			flag = 1;
		}
		if (keye == 1&&canstore==true) {
			storepower-=CHARGESPEED;
			if (abs(storepower) > nowpower)storepower = -nowpower;
			flag = 1;
		}
		if (flag == 1) {
			charge->Soundplay(DX_PLAYTYPE_LOOP);
			flag = 0;
		}
		else if (flag == 0) {
			charge->Soundstop();
		}

		if (jumpflag == 1&&nowjump==0) {
			nowjump = 1;
			speed = 10;
		}
		if (nowjump == 1) {
			jump += speed*0.02;
			speed -= 0.6;
			if (jump < 0) {
				jump = 0;
				nowjump = 0;
			}
		}
	}
	void wheel(int wheel_number, int direction, int x, int y) {
		zoom -= (double)direction;
		if (zoom < 10)zoom = 10;
		if (zoom > 60)zoom = 60;
	}
	void Keyboard(unsigned char key, int _x, int _y) {

		switch (key) {
		case 'A':
		case'a':
			keya = 1;
			break;
		case'D':
		case'd':
			keyd = 1;
			break;
		case'W':
		case'w':
			keyw = 1;
			break;
		case'S':
		case's':
			keys = 1;
			break;
		case'Q':
		case'q':
			keyq = 1;
			break;
		case'E':
		case'e':
			keye = 1;
			break;
		case' ':
			jumpflag = 1;
			break;
		case'\033':
			FILE* save;
			if (fopen_s(&save, "data/save.sav", "w") == 0) {
				fprintf(save, "%d", (int)Level);
				fclose(save);
			}
			exit(0);
			break;
		default:
			break;
		}
	}
	void Keyboardup(unsigned char key, int _x, int _y) {
		switch (key) {
		case 'A':
		case'a':
			keya = 0;
			break;
		case'D':
		case'd':
			keyd = 0;
			break;
		case'W':
		case'w':
			keyw = 0;
			break;
		case'S':
		case's':
			keys = 0;
			break;
		case'Q':
		case'q':
			keyq = 0;
			if (storepower != 0) {
				for (int i = 0; i < gravity.size(); i++) {
					if (gravity[i].x == x + 5 * sin(wxz) && gravity[i].z == z + 5 * cos(wxz)) {
						gravity[i].m += storepower;
						flag = 1;
					}
				}
				if (flag == 0)gravity.push_back(Gravity(x + 5 * sin(wxz), z + 5 * cos(wxz), storepower));
			}
			flag = 0;
			usepower += storepower;
			nowpower -= abs(storepower);
			storepower = 0;
			if (canstore == true) {
				magic->Soundplay(DX_PLAYTYPE_BACK);
				magic->flag = 0;
			}
			canstore = true;
			break;
		case'E':
		case'e':
			keye = 0;
			if (storepower != 0) {
				for (int i = 0; i < gravity.size(); i++) {
					if (gravity[i].x == x + 5 * sin(wxz) && gravity[i].z == z + 5 * cos(wxz)) {
						gravity[i].m += storepower;
						flag = 1;
					}
				}
				if (flag == 0)gravity.push_back(Gravity(x + 5 * sin(wxz), z + 5 * cos(wxz), storepower));
			}
			flag = 0;
			usepower += storepower;
			nowpower -= abs(storepower);
			storepower = 0;
			if (canstore == true) {
				magic->Soundplay(DX_PLAYTYPE_BACK);
				magic->flag = 0;
			}
			canstore = true;
			break;
		case'c':
		case'C':
			if (cleep) {
				cleep = false;
			}
			else if (!cleep) {
				cleep = true;
			}
			break;
		case' ':
			jumpflag = 0;
			break;
		default:
			break;
		}
	}
	void motion(int u, int v)
	{
			int ww = glutGet(GLUT_WINDOW_WIDTH);
			int wh = glutGet(GLUT_WINDOW_HEIGHT);

			double dx = u - ww / 2;
			double dy = v - wh / 2;

			// Do something with dx and dy here
			wxz += -dx / 200.0;
			if (wxz > M_PI / 2.0) {
				wxz -= 2 * M_PI;
			}
			else if (wxz<-3 * M_PI / 2.0) {
				wxz += 2 * M_PI;
			}
			wy += -dy / 200.0;
			if (wy > 3.14 / 2.0) { wy = 3.14 / 2.0; }
			if (wy < -3.14 / 2.0) { wy = -3.14 / 2.0; }

			glutWarpPointer(ww / 2, wh / 2);
/*
		if (savepoint[0] != 0 || savepoint[1] != 0) {
			wxz += ((double)(savepoint[0] - u)) / 100.0;
			if (wxz > M_PI/2.0) {
				wxz -= 2 * M_PI;
			}
			else if (wxz<-3*M_PI/2.0) {
				wxz += 2 * M_PI;
			}
			wy += ((double)(savepoint[1] - v)) / 100.0;
			if (wy > 3.14 / 2.0) { wy = 3.14 / 2.0; }
			if (wy < -3.14 / 2.0) { wy = -3.14 / 2.0; }
		}
		savepoint[0] = u;
		savepoint[1] = v;
		*/
	}
	void mouse(int button, int state, int u, int v)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN) {
				if (storepower != 0) {
					canstore = false;
					shotgravity->reset(x,y,z,storepower,wxz);
					nowpower -= abs(storepower);
					storepower = 0;
					shot->Soundplay(DX_PLAYTYPE_BACK);
					shot->flag = 0;
				}
				
			}
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		default:
			break;
		}
	}
	void damage() {
		if (damagecount == 0) {
			nowpower -= maxpower*0.3;
			if (nowpower < 0) {
				nowpower = 0;
			}
			hit->Soundplay(DX_PLAYTYPE_BACK);
			hit->flag = 0;
			damagecount = 60;
		}
	}
	double GetX() { return x; }
	double GetZ() { return z; }
	double GetY() { return y; }
	double GetJ() { return jump; }
	double GetWxz() { return wxz; }
	double Getzoom() { return zoom; }
	void ChangeY(double _y) { y = _y; }
	void Changelevel(double _level) { Level = _level; }
	void PushBattery(Battery* _battery) { battery = _battery; }
	vector<Gravity>* Getgravity() {
		return &gravity;
	}
	ShotGravity* GetShot() { return shotgravity; }
	double Getusepower() { return usepower; }
	double* Getstorepower() { return &storepower; }
	double* Getnowpower() { return &nowpower; }
	double* Getmaxpower() { return &maxpower; }
};

class Enemy {
private:
	double hp;
	double maxhp;
	double x, y, z;
	double tempy;
	double fowardy;
	double tempfowardy;
	double wxz;
	double tempwxz;
	double wy;
	double v;
	rxOBJ *robottop;
	rxOBJ *robotbottom;
	rxOBJ *bullet[3];
	Sound *foot;
	Sound *bomb;
	Sound *death;
	Sound *missile;
	bool step;
	int count;
	bool firstflag;
	double time;
	double bulletw;
	double bullettime;
	double bulletx, bullety, bulletz;
	bool shotflag;
	bool shotnow;
public:
	Enemy() {
		count = 0;
		firstflag = FALSE;
		hp = -1;
		maxhp = 0;
		x = z = 200;
		y = 0;
		fowardy = 0;
		v = 0.2;
		step = true;
		
		robottop = new rxOBJ("data/robottop.obj", "data/robot.png");
		robotbottom = new rxOBJ("data/robotbottom.obj", "data/robot.png");
		bullet[0] = new rxOBJ("data/bullet.obj", "data/bullet.png");
		bullet[1] = new rxOBJ("data/bullet2.obj", "data/bullet.png");
		bullet[2] = new rxOBJ("data/bullet3.obj", "data/bullet.png");
		bomb = new Sound("data/bomb.wav");
		bomb->ChangeVolume(180);
		foot = new Sound("data/robotfoot.wav");
		death = new Sound("data/death.wav");
		missile = new Sound("data/missile.wav");
		missile->ChangeVolume(200);
		time = 0;
		shotflag = false;
		bullettime = 0;
		shotnow = false;
	}
	void shotrender() {
		if (shotflag == true) {
			if (shotnow==false) {
				if (GetRand(50) < 1) {
					missile->Soundplay(DX_PLAYTYPE_BACK);
					missile->flag = 0;
					shotnow = true;
					bullettime = 0;
					bulletx = x;
					bullety = y + 1;
					bulletz = z;
					bulletw = wxz + 10 * sin(time / 2.0)*M_PI/180.0;
				}
			}
		}
		if (shotnow == true) {
			bullettime += 0.01;
			glPushMatrix();
			glTranslated(bulletx + 80*bullettime*cos(bulletw), bullety, bulletz + 80*bullettime*sin(bulletw));
			glRotated(-bulletw * 180 / M_PI + 90, 0, 1, 0);
			glScaled(0.2, 0.2, 0.2);
			bullet[0]->renderModel();
			glPopMatrix();

			glPushMatrix();
			glTranslated(bulletx + 80 * bullettime*cos(bulletw - M_PI / 9.0), bullety, bulletz + 80 * bullettime*sin(bulletw - M_PI / 9.0));
			glRotated(-bulletw * 180 / M_PI + 90, 0, 1, 0);
			glScaled(0.2, 0.2, 0.2);
			bullet[1]->renderModel();
			glPopMatrix();

			glPushMatrix();
			glTranslated(bulletx + 80 * bullettime*cos(bulletw+M_PI/9.0), bullety, bulletz + 80 * bullettime*sin(bulletw+M_PI/9.0));
			glRotated(-bulletw * 180 / M_PI + 90, 0, 1, 0);
			glScaled(0.2, 0.2, 0.2);
			bullet[2]->renderModel();
			glPopMatrix();
			if (bullettime > 2.0) {
				shotnow = false;
			}
		}

	}
	bool GetShotnow() { return shotnow; }
	double GetshotX() { return bulletx + 80 * bullettime*cos(bulletw); }
	double GetshotZ() { return bulletz + 80 * bullettime*sin(bulletw); }
	double GetshotY() { return bullety; }
	void Changeyxy(double _y,double _fowardy) {
		tempy = _y; tempfowardy = _fowardy;
	}
	void idle(double px,double pz) {
		y += (tempy - y) / 50.0;
		fowardy += (tempfowardy - fowardy) / 50.0;

		tempwxz = atan2(pz - z, px - x);
		wxz += (tempwxz - wxz);
		
		if (hp > 0) {
			if (((pz - z)*(pz - z) + (px - x)*(px - x)) < 20000) {
				shotflag = true;
			}
			else {
				shotflag = false;
			}
			if (((pz - z)*(pz - z) + (px - x)*(px - x)) > 2000) {
				x += v*cos(wxz);
				z += v*sin(wxz);
			}
			if (((pz - z)*(pz - z) + (px - x)*(px - x)) < 15000 && ((pz - z)*(pz - z) + (px - x)*(px - x)) > 2000) {
				foot->ChangeVolume(55 + 150 * 1500 / ((pz - z)*(pz - z) + (px - x)*(px - x)));
				foot->Soundplay(DX_PLAYTYPE_LOOP);
			}
			else if (((pz - z)*(pz - z) + (px - x)*(px - x)) < 500) {
				x -= v*cos(wxz);
				z -= v*sin(wxz);
			}
			else if (((pz - z)*(pz - z) + (px - x)*(px - x)) <= 2000 && step == true) {
				x += v*sin(wxz);
				z -= v*cos(wxz);
				if (GetRand(200) < 1) {
					step = false;
				}
			}
			else if (((pz - z)*(pz - z) + (px - x)*(px - x)) <= 2000 && step == false) {
				x -= v*sin(wxz);
				z += v*cos(wxz);
				if (GetRand(200) < 1) {
					step = true;
				}
			}
			else {
				foot->Soundstop();
			}
		}
		else {
			shotflag = false;
			if (((pz - z)*(pz - z) + (px - x)*(px - x)) < 15000 && ((pz - z)*(pz - z) + (px - x)*(px - x)) > 10) {
				foot->ChangeVolume(55 + 150 * 10 / ((pz - z)*(pz - z) + (px - x)*(px - x)));
				foot->Soundplay(DX_PLAYTYPE_LOOP);
			}
			else {
				foot->Soundstop();
			}
			x -= v*cos(wxz);
			z -= v*sin(wxz);
			if (count == 0) {
				if (firstflag) {
					death->Soundplay(DX_PLAYTYPE_BACK);
					death->flag = 0;
				}
			}
			count++;
			if (count > 2700) {
				if (!firstflag) {
					x = px + 200;
					z = pz + 200;
				}
				count = 0;
				maxhp += 50;
				hp = maxhp;
				firstflag = TRUE;
			}
		}
	}
	void Damagehp(double damage) {
		hp -= damage;
		bomb->Soundplay(DX_PLAYTYPE_BACK);
		bomb->flag = 0;
	}
	void render() {
		static GLfloat lightPosition[4] = { 0,0 , 10, 0 }; //光源の位置
		static GLfloat lightDiffuse[4] = { 0.7,   0.7, 0.7 ,1 }; //拡散光
		static GLfloat lightAmbient[4] = { 0.25, 0.25, 0.25,1 }; //環境光
		static GLfloat lightSpecular[4] = { 1.0,   1.0, 1.0,1 }; //鏡面光			

		static GLfloat lightDiffusemat[4] = { 0.7,   0.7, 0.7 ,1 }; //拡散光
		static GLfloat lightAmbientmat[4] = { 0.3, 0.7, 0.7,1 }; //環境光
		static GLfloat lightSpecularmat[4] = { 0.4,   1.0, 1.0,1 }; //鏡面光			
		glEnable(GL_LIGHTING);//「照明パラメータ」の有効化													   		
		glEnable(GL_LIGHT0);//「光源０」を「ON」にする。
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
		glMaterialfv(GL_FRONT, GL_SPECULAR, lightSpecularmat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, lightDiffusemat);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lightAmbientmat);

		if (firstflag) {
			glPushMatrix();
			glTranslated(x, y + 1, z);
			glRotated(-wxz * 180 / M_PI + 90, 0, 1, 0);
			glRotated(atan2((fowardy - y), 2) * 180 / M_PI, -1, 0, 0);
			glScaled(0.2, 0.2, 0.2);
			robotbottom->renderModel();
			glTranslated(0, 2 + 2 * sin(time), 0);
			glRotated(10 * sin(time / 2.0), 0, 1, 0);
			robottop->renderModel();
			bullet[0]->renderModel();
			bullet[1]->renderModel();
			bullet[2]->renderModel();
			glPopMatrix();

			shotrender();
		}
		glDisable(GL_LIGHTING);
		time += 0.05;
	}
	double GetX() { return x; }
	double GetZ() { return z; }
	double GetfX() { return x + 2*cos(wxz); }
	double GetfZ() { return z + 2 * sin(wxz); }
	double Gethp() { return hp; }
};

class DisplayManage {
private:
	picture* menu;
	vector<Gravity> *gravity;
	ShotGravity* shotgravity;
	Sphere* sphere;//夜空
	Sky* sky;//雲
	picture* integer[10];
	picture* ten;// :
	picture* tenki[3];
	picture* juuden;
	picture* level;
	picture* setumei;
	picture* goal[3];
	Battery* battery;

	Enemy *enemy;
	rxOBJ* diamond;
	rxOBJ* Pobj;
	double time = M_PI;
	double px, py, pz;//プレイヤー座標
	double pj;//jump
	double pw;
	double* pstorepower;//プレイヤーの溜めている電荷
	double* pnowpower;
	double* pmaxpower;
	double mapcenterx, mapcenterz;//描画するマップの中心位置
	double zoom;
	double map[MAPWID][MAPLENGTH];
	double maptemp[MAPWID][MAPLENGTH];
	unsigned char shadow[40000] = {};
	unsigned char storepowerR[3072000] = {};
	unsigned char storepowerB[3072000] = {};
	unsigned char zannryou[25200] = {};
	unsigned char radar[40000] = { };
	unsigned char radarrot[40000] = {};
	unsigned char* mappic;
	void ObjDis() {
		glColor4d(1.0, 1.0, 1.0,0.8);
		static GLfloat lightPosition[4] = { 0,0 , 10, 0 }; //光源の位置
		static GLfloat lightDiffuse[4] = { 0.7,   0.7, 0.7 ,1}; //拡散光
		static GLfloat lightAmbient[4] = { 0.25, 0.25, 0.25,1 }; //環境光
		static GLfloat lightSpecular[4] = { 1.0,   1.0, 1.0,1 }; //鏡面光			
		
		static GLfloat lightDiffusemat[4] = { 0.7,   0.7, 0.7 ,1 }; //拡散光
		static GLfloat lightAmbientmat[4] = { 0.3, 0.7, 0.7,1 }; //環境光
		static GLfloat lightSpecularmat[4] = { 0.4,   1.0, 1.0,1 }; //鏡面光			
		glEnable(GL_LIGHTING);//「照明パラメータ」の有効化													   		
		glEnable(GL_LIGHT0);//「光源０」を「ON」にする。
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
		glMaterialfv(GL_FRONT, GL_SPECULAR, lightSpecularmat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, lightDiffusemat);
		glMaterialfv(GL_FRONT, GL_AMBIENT, lightAmbientmat);


		glPushMatrix();
		glTranslated(0, 500, 0);
		glRotated(time*180.0/M_PI, 0, 1, 0);
		diamond->renderModel();
	
	
		
		glPopMatrix();

		glDisable(GL_LIGHTING);

		glColor4d(1.0, 1.0, 1.0, 1);

	}
	void PlayerDis() {
		glPushMatrix();
		glTranslated(px, py+pj-2, pz);
		glRotated(pw*180.0/M_PI, 0, 1, 0);
		glTranslated(0, 0, -0.22);
		double scale = 0.07;
		glScaled(scale, scale, scale);
		Pobj->renderModel();
		glPopMatrix();
	}
	void render_string(float x, float y, const char* string,int R,int G,int B)
	{
		float z = 0.0f;
		glColor3f(R, G, B);
		glWindowPos3f(x, y, z);
		char* p = (char*)string;
		while (*p != '\0') glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
	}
	void MapDis() {

		for (int i = 0; i < MAPWID;i++) {
			for (int j = 0; j < MAPLENGTH;j++) {
				map[i][j] += (maptemp[i][j] - map[i][j]) / 50.0;
			}
		}

//#define POLYGONMAP
#ifdef POLYGONMAP


		static GLfloat lightPosition[4] = { 0,0 , 0, 1.0 }; //光源の位置
		static GLfloat lightDiffuse[3] = { 1.0,   1.0, 1.0 }; //拡散光
		static GLfloat lightAmbient[3] = { 0.25, 0.25, 0.25 }; //環境光
		static GLfloat lightSpecular[3] = { 1.0,   1.0, 1.0 }; //鏡面光
//		glEnable(GL_LIGHTING);//「照明パラメータ」の有効化
//		glEnable(GL_LIGHT0);//「光源０」を「ON」にする。
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

		for (int i = 0; i < MAPWID-1; i++) {
			for (int j = 0; j < MAPLENGTH-1; j++) {
				glBegin(GL_POLYGON);
				glColor4d(1.0 + (map[i][j]) / 10.0, 1.0 - abs(map[i][j]), 1.0 - (map[i][j]) / 10.0,0.5);
				glVertex3d((double)(i - MAPWID/2.0)+mapcenterx, -2 + map[i][j], (double)(j - MAPLENGTH/2.0)+mapcenterz);
				j++;
				glColor4d(1.0 + (map[i][j]) / 10.0, 1.0 - abs(map[i][j]), 1.0 - (map[i][j]) / 10.0, 0.5);
				glVertex3d((double)(i - MAPWID / 2.0) + mapcenterx, -2 + map[i][j], (double)(j - MAPLENGTH / 2.0) + mapcenterz);
				i++;
				glColor4d(1.0 + (map[i][j]) / 10.0, 1.0 - abs(map[i][j]), 1.0 - (map[i][j]) / 10.0, 0.5);
				glVertex3d((double)(i - MAPWID / 2.0) + mapcenterx, -2 + map[i][j], (double)(j - MAPLENGTH / 2.0) + mapcenterz);
				j--;
				glColor4d(1.0 + (map[i][j]) / 10.0, 1.0 - abs(map[i][j]), 1.0 - (map[i][j]) / 10.0, 0.5);
				glVertex3d((double)(i - MAPWID / 2.0) + mapcenterx, -2 + map[i][j], (double)(j - MAPLENGTH / 2.0) + mapcenterz);
				i--;
				glEnd();
			}
			
		}
		glDisable(GL_LIGHTING);


#else

		for (int i = 0; i < MAPWID; i++) {
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < MAPLENGTH; j++) {
				glColor4d(1.0 + (map[i][j]) / 10.0, 1.0 - abs(map[i][j]), 1.0 - (map[i][j]) / 10.0, 0.3);
				glVertex3d((double)(i - MAPWID / 2.0) + mapcenterx, -2 + map[i][j], (double)(j - MAPLENGTH / 2.0) + mapcenterz);

			}
			glEnd();
		}

		for (int j = 0; j < MAPLENGTH; j++) {
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < MAPWID; i++) {
				glColor4d(1.0 + (map[i][j]) / 10.0, 1.0 - abs(map[i][j]), 1.0 - (map[i][j]) / 10.0, 0.3);

				glVertex3d((double)(i - MAPWID / 2.0) + mapcenterx, -2 + map[i][j], (double)(j - MAPLENGTH / 2.0) + mapcenterz);

			}
			glEnd();
		}

#endif

		int tempx, tempz;
//		cout <<"向き"<< pw;
		for (int j = 0; j < MINIMAP; j++) {
			for (int i = 0; i < MINIMAP; i++) {

				tempx = j + (MAPLENGTH - MINIMAP) / 2 + px - mapcenterx;
				tempz = MINIMAP - 1 - i + (MAPLENGTH - MINIMAP) / 2 + pz - mapcenterz;
				int color = 0;
				if (255 > 100.0 * (1.0 + (map[tempx][tempz]) / 10.0) && 0 < 100.0 * (1.0 + (map[tempx][tempz]) / 10.0)) {
					mappic[color + j * 4 + i * 400] = 100.0 * (1.0 + (map[tempx][tempz]) / 10.0);//R
				}
				else if (255.0 <= 100.0 * (1.0 + (map[tempx][tempz]) / 10.0)) {
					mappic[color + j * 4 + i * 400] = 255;
				}
				else {
					mappic[color + j * 4 + i * 400] = 0;
				}
				color++;


				if (200 > 100.0*(1.0 - abs(map[tempx][tempz])) && 0 < 100.0*(1.0 - abs(map[tempx][tempz]))) {
					mappic[color + j * 4 + i * 400] = 100.0*(1.0 - abs(map[tempx][tempz]));//G
				}
				else if (200.0 <= 100.0*(1.0 - abs(map[tempx][tempz]))) {
					mappic[color + j * 4 + i * 400] = 200;
				}
				else {
					mappic[color + j * 4 + i * 400] = 0;
				}

				double tempw1 = (pw + zoom*M_PI / 180.0 + M_PI / 2.0), tempw2 = (pw - zoom*M_PI / 180.0 + M_PI / 2.0);

				if (pw + zoom*M_PI / 180.0 > M_PI / 2.0) {//視界
					tempw1 -= 2 * M_PI;
					if ((i - 50)*(i - 50) + (j - 50)*(j - 50) < 2000 &&
						(atan2((50 - i), (50 - j)) < tempw1 || atan2((50 - i), (50 - j)) > tempw2)) {
						mappic[color + j * 4 + i * 400] += 50;
					}
				}
				else if (pw - zoom*M_PI / 180.0 < -3 * M_PI / 2.0) {
					tempw2 += 2 * M_PI;
					if ((i - 50)*(i - 50) + (j - 50)*(j - 50) < 2000 &&
						(atan2((50 - i), (50 - j)) < tempw1 || atan2((50 - i), (50 - j)) > tempw2)) {
						mappic[color + j * 4 + i * 400] += 50;
					}
				}
				else if ((i - 50)*(i - 50) + (j - 50)*(j - 50) < 2000 &&
					(atan2((50 - i), (50 - j)) < tempw1&& atan2((50 - i), (50 - j)) > tempw2)) {
					mappic[color + j * 4 + i * 400] += 50;
				}

				color++;
				if (255 > 100.0*(1.0 - (map[tempx][tempz]) / 10.0) && 0 < 100.0*(1.0 - (map[tempx][tempz]) / 10.0)) {
					mappic[color + j * 4 + i * 400] = 100.0*(1.0 - (map[tempx][tempz]) / 10.0);//B
				}
				else if (255.0 <= 100.0*(1.0 - (map[tempx][tempz]) / 10.0)) {
					mappic[color + j * 4 + i * 400] = 255;
				}
				else {
					mappic[color + j * 4 + i * 400] = 0;
				}


				int tempx, tempz;//格子状の線
				if (px > 0) { tempx = 19 - (int)abs(px) % 20; }
				else { tempx = (int)abs(px) % 20; }
				if (pz < 0) { tempz = 19 - (int)abs(pz) % 20; }
				else { tempz = (int)abs(pz) % 20; }
				color++;
				if (i % 20 == tempz || j % 20 == tempx) {
					color -= 3;
					for (int n = 0; n < 3; n++) {
						if (mappic[color + j * 4 + i * 400] + 20 > 255) {
							mappic[color + j * 4 + i * 400] = 255;
						}
						else {
							mappic[color + j * 4 + i * 400] += 20;
						}
						color++;
					}
				}
	
				mappic[color + j * 4 + i * 400] = 255;//アルファ値
			}
		}
		static unsigned char view[27] = { };
		

		glWindowPos3f(1170, 10, 0.1);
		glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, mappic);
		glWindowPos3f(1170, 10, 0.05);
		glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, shadow);
		glWindowPos3f(1219, 59, 0.0);
		glDrawPixels(3, 3, GL_RGB, GL_UNSIGNED_BYTE, view);
		

	}
	void GravDis(){
		for (int i = 0; i < gravity->size(); i++) {
			(*gravity)[i].render();
		}
		shotgravity->render();
	}
	void MenuDis() {
		setumei->render_windowpos(20, 200, -0.05);
		if (enemy->Gethp() > 0) {
			goal[1]->render_windowpos(140, 600, 0.05);
		}else if (*pmaxpower > 100) {
			goal[2]->render_windowpos(140, 600, 0.05);
		}
		else {
			goal[0]->render_windowpos(140, 600, 0.05);
		}

		int temp = (time / (2 * M_PI)) * 24;//時間
		if (temp < 10) {
			integer[0]->render_windowpos(10, 10, 0.1);
			integer[temp]->render_windowpos(60, 10, 0.1);
		}
		else if (temp >= 20) {
			integer[2]->render_windowpos(10, 10, 0.1);
			integer[temp - 20]->render_windowpos(60, 10, 0.1);
		}
		else {
			integer[1]->render_windowpos(10, 10, 0.1);
			integer[temp - 10]->render_windowpos(60, 10, 0.1);
		}

		if (temp > 18 || temp < 6) {//天気
			tenki[2]->render_windowpos(290, 10, 0.1);
		}
		else if (sky->GetWeather() == CLOUD) {
			tenki[1]->render_windowpos(290, 10, 0.1);
		}
		else {
			tenki[0]->render_windowpos(290, 10, 0.1);
		}



		static int flash = 0;
		flash++;
		if (flash % 100 > 50) {//:
			ten->render_windowpos(110, 0, 0.1);
		}
		temp = (int)(time / (2 * M_PI / 96.0)) % 4;//分
		if (temp == 0) {
			integer[0]->render_windowpos(160, 10,0.1);
			integer[0]->render_windowpos(210, 10, 0.1);
		}
		else if (temp == 1) {
			integer[1]->render_windowpos(160, 10, 0.1);
			integer[5]->render_windowpos(210, 10, 0.1);
		}
		else if (temp == 2) {
			integer[3]->render_windowpos(160, 10, 0.1);
			integer[0]->render_windowpos(210, 10, 0.1);
		}
		else if (temp == 3) {
			integer[4]->render_windowpos(160, 10, 0.1);
			integer[5]->render_windowpos(210, 10, 0.1);
		}


		level->render_windowpos(450, 10, 0.1);
		int lev = (*pmaxpower - 50.0) / 5.0;
		if (lev > 99) {
			integer[9]->render_windowpos(650, 10, 0.1);
			integer[9]->render_windowpos(700, 10, 0.1);
		}
		else {
			integer[lev / 10]->render_windowpos(650, 10, 0.1);
			integer[lev % 10]->render_windowpos(700, 10, 0.1);
		}

		juuden->render_windowpos(750, 10, 0.1);
		
		glWindowPos3f(1010, 10, 0.1);
		glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, radar);
		static double radartime = 0;
		radartime += 0.05;
		if (radartime > 2 * M_PI)radartime -= 2 * M_PI;
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
				double temp = radartime - M_PI - atan2(50 - i, 50 - j);
				if ((i - 50)*(i - 50) + (j - 50)*(j - 50) < 2304) {
					if (radartime < M_PI / 6.0) {
						if (atan2(50 - i, 50 - j) < radartime - M_PI || atan2(50 - i, 50 - j) > radartime - M_PI - M_PI / 6.0 + 2 * M_PI) {
							if (temp < 0) {
								temp += 2 * M_PI;
							}
							radarrot[i * 400 + j * 4 + 1] = 255;
							radarrot[i * 400 + j * 4 + 3] = 255 - temp*255.0*6.0 / M_PI;
						}
						else {
							radarrot[i * 400 + j * 4 + 3] = 0;
						}
					}
					else if (atan2(50 - i, 50 - j) < radartime - M_PI&&atan2(50 - i, 50 - j) > radartime - M_PI - M_PI / 6.0) {
						radarrot[i * 400 + j * 4 + 3] = 255 - temp*255.0*6.0 / M_PI;
						radarrot[i * 400 + j * 4 + 1] = 255;
					}
					else {
						radarrot[i * 400 + j * 4 + 3] = 0;
					}

					if (atan2(px, pz) > -M_PI / 2.0 + M_PI / 12.0) {
						if (atan2(px, pz) + M_PI / 12 - M_PI / 2.0 > atan2(50 - i, 50 - j) && atan2(50 - i, 50 - j) > atan2(px, pz) - M_PI / 12 - M_PI / 2.0) {
							radarrot[i * 400 + j * 4 + 2] = 255 - (abs(atan2(px, pz) - M_PI / 2.0 - atan2(50 - i, 50 - j))) * 255 * 12 / M_PI;
							radarrot[i * 400 + j * 4 + 1] = (abs(atan2(px, pz) - M_PI / 2.0 - atan2(50 - i, 50 - j))) * 255 * 12 / M_PI;
						}
						else {
							radarrot[i * 400 + j * 4 + 2] = 0;
						}
					}
					else if(atan2(px, pz) > -M_PI / 2.0 + M_PI / 12.0-M_PI/6.0){
						if (atan2(px, pz) + M_PI / 12 - M_PI / 2.0 > atan2(50 - i, 50 - j) || atan2(50 - i, 50 - j) > atan2(px, pz) - M_PI / 12 - M_PI / 2.0+2*M_PI) {
							radarrot[i * 400 + j * 4 + 2] = 255;
							radarrot[i * 400 + j * 4 + 1] = 0;
						}
						else {
							radarrot[i * 400 + j * 4 + 2] = 0;
						}
					}
					else {
						if (atan2(px, pz) + M_PI / 12 - M_PI / 2.0+2*M_PI > atan2(50 - i, 50 - j) && atan2(50 - i, 50 - j) > atan2(px, pz) - M_PI / 12 - M_PI / 2.0 + 2 * M_PI) {
												
							radarrot[i * 400 + j * 4 + 2] = 255;
							radarrot[i * 400 + j * 4 + 1] = 0;
						}
						else {
							radarrot[i * 400 + j * 4 + 2] = 0;
						}
					}

				}
			}
		}
		glWindowPos3f(1010, 10, 0.07);
		glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, radarrot);
		glWindowPos3f(1010, 10, 0.05);
		glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, shadow);

		for (int i = 0; i < 60; i++) {
			for (int j = 0; j < 105; j++) {
				if (j >= (*pnowpower / *pmaxpower) * 105) {
					zannryou[i * 420 + j * 4 + 3] = 0;
				}
				else if(j>=(1.0-(abs(*pstorepower)/ *pnowpower))*((*pnowpower / *pmaxpower) * 105) ){
					zannryou[i * 420 + j * 4 + 3] = 100;
					if (*pstorepower > 0) { 
						zannryou[i * 420 + j * 4 + 1] = 0; 
						zannryou[i * 420 + j * 4 + 2] = 0;
						zannryou[i * 420 + j * 4 + 0] = 255;
					}
					else {
						zannryou[i * 420 + j * 4 + 2] = 255;
						zannryou[i * 420 + j * 4 + 1] = 0;
						zannryou[i * 420 + j * 4 + 0] = 0;
					}
				}
				else {
					zannryou[i * 420 + j * 4 + 3] = 255;
					zannryou[i * 420 + j * 4 + 2] = 0;
					zannryou[i * 420 + j * 4 + 1] = 255;
					zannryou[i * 420 + j * 4 + 0] = 255;
				}
			}
		}
	
		glWindowPos3f(790, 30, 0.05);
		glDrawPixels(105, 60, GL_RGBA, GL_UNSIGNED_BYTE, zannryou);
	}

	void StorepowerDis() {//画面に溜めているエフェクトを表示
		static double flush = 0;
		flush += 0.05;
		if (*pstorepower > 0) {
			for (int i = 0; i < 600; i++) {
				for (int j = 0; j < 1280; j++) {
					double distance;
					if (j <640 ) {	
						distance = j;
					}else { 
						distance = 1279 - j;
					}
					if (i < 300) {
						if (distance > i) {
							distance = i;
						}
					}
					else {
						if (distance > 599 -i) {
							distance = 599 - i;
						}
					}
					if (distance > *pstorepower) {
						storepowerR[j * 4 + i * 5120 + 3] = 0;
					}
					else {
						storepowerR[j * 4 + i * 5120 + 3] = (255 - 255.0 * distance / *pstorepower)*((sin(flush)+3.0)/4.0);
					}
					
				}
			}
			glWindowPos3f(0, 120, 0.1);
			glDrawPixels(1280, 600, GL_RGBA, GL_UNSIGNED_BYTE, storepowerR);
		}
		else if (*pstorepower < 0) {
			for (int i = 0; i < 600; i++) {
				for (int j = 0; j < 1280; j++) {
					double distance;
					if (j <640) {
						distance = j;
					}
					else {
						distance = 1279 - j;
					}
					if (i < 300) {
						if (distance > i) {
							distance = i;
						}
					}
					else {
						if (distance > 599 - i) {
							distance = 599 - i;
						}
					}
					if (distance > abs(*pstorepower)) {
						storepowerB[j * 4 + i * 5120 + 3] = 0;
					}
					else {
						storepowerB[j * 4 + i * 5120 + 3] = (255 - 255.0 * distance / abs(*pstorepower))*((sin(flush) + 3.0) / 4.0);
					}

				}
			}
			glWindowPos3f(0, 120, 0.1);
			glDrawPixels(1280, 600, GL_RGBA, GL_UNSIGNED_BYTE, storepowerB);
		}
	}

public:
	DisplayManage();
	~DisplayManage() {
		delete menu;
	}
	void init() {
		sphere->init();
		sky->init();
		
	}
	void display() {
		PlayerDis();
		menu->render_windowpos(0, 0,0.2);

		battery->renderbattery(px, pz);
		MapDis();
		enemy->render();
		GravDis();

		glPushMatrix();
		glTranslated(px, 0, pz);
		glPushMatrix();
		glTranslated(0, py, 0);
		sphere->render();
		glPopMatrix();
		sky->render();
		glPopMatrix();
		ObjDis();
		

		StorepowerDis();
		MenuDis();
	//	DrawFormatString(0, 0, GetColor(255, 255, 255), "%f", time);
		time += TIMEUNIT;
		if (time > 2 * M_PI)time = 0;
	}
	void mapchange(int x,int z,double high) {
		map[x][z] = high;
	}
	void maptempchange(int x, int z, double high) {
		maptemp[x][z] = high;
	}
	void pLocateChange(double x, double y, double z,double w,double _zoom,double jump) {
		px = x; py = y; pz = z; pw = w; zoom = _zoom; pj = jump;
	}
	double GetCenterX() {	return mapcenterx;	}
	double GetCenterZ() { return mapcenterz; }
	void ChangeCenterX(double x) { mapcenterx += x;}
	void ChangeCenterZ(double z) { mapcenterz += z; }
	void ChangepStorepower(double* power) { pstorepower = power; }
	void Changenowpower(double* now) { pnowpower = now; }
	void Pushmaxpower(double* max) { pmaxpower = max; }
	void Putgravity(vector<Gravity>* _gravity) { gravity = _gravity; }
	void PutShot(ShotGravity* _shot) { shotgravity = _shot; }
	void Putenemy(Enemy* _enemy) { enemy = _enemy; }
	double GetMapY(int i, int j) { return map[i][j]; }
	double GetMaptempY(int i, int j) { return maptemp[i][j]; }
	Battery* GetBattery() { return battery; }
};
DisplayManage::DisplayManage() {
	menu = new picture("data/menu.data", 1280, 120, 4);
	integer[0] = new picture("data/0.data", 50, 100, 4); integer[1] = new picture("data/1.data", 50, 100, 4); integer[2] = new picture("data/2.data", 50, 100, 4);
	integer[3] = new picture("data/3.data", 50, 100, 4); integer[4] = new picture("data/4.data", 50, 100, 4); integer[5] = new picture("data/5.data", 50, 100, 4);
	integer[6] = new picture("data/6.data", 50, 100, 4); integer[7] = new picture("data/7.data", 50, 100, 4); integer[8] = new picture("data/8.data", 50, 100, 4); 
	integer[9] = new picture("data/9.data", 50, 100, 4); ten = new picture("data/ten.data", 50, 100, 4);
	tenki[0] = new picture("data/sun.data", 100, 100, 4); tenki[1] = new picture("data/kumori.data", 100, 100, 4); tenki[2] = new picture("data/night.data", 100, 100, 4);
	juuden = new picture("data/battery.data", 200, 100, 4);
	level = new picture("data/level.data", 200, 100, 4);
	setumei = new picture("data/setumei.data", 480, 300, 4);
	goal[0] = new picture("data/goal0.data", 1000, 100, 4); goal[1] = new picture("data/goal1.data", 1000, 100, 4); goal[2] = new picture("data/goal2.data", 1000, 100, 4);
	battery = new Battery;
	diamond = new rxOBJ("data/diamond.obj");
	Pobj = new rxOBJ("data/kaito.obj","data/tex.png");

	for (int i = 0; i < MAPWID; i++) {
		for (int j = 0; j < MAPLENGTH; j++) {
			map[i][j] = 0;
			maptemp[i][j] = 0;
		}
	}
	px = py = pz = 0;
	pw = 0; zoom = 30;
	pstorepower = 0;
	mapcenterx = mapcenterz = 0;
	sphere = new Sphere("data/star.data", 3000, 3000, 4,900.0);
	sky = new Sky("data/cloud.data", 3000, 3000, 4);
	mappic = new unsigned char[MINIMAP*MINIMAP*4];
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			for (int color = 0; color < 3; color++) {
				mappic[color + j * 3 + i * 300] = 0;
			}
		}
	}
	for (int i = 0; i < 100; i++) {//マップの影
		for (int j = 0; j < 100; j++) {
			double distance;
			if (j <50) {
				distance = j;
			}
			else {
				distance = 99 - j;
			}
			if (i < 50) {
				if (distance > i) {
					distance = i;
				}
			}
			else {
				if (distance > 99 - i) {
					distance = 99 - i;
				}
			}
			if (distance < 10) {
				shadow[i * 400 + j * 4 + 3] = 150 - 150.0*distance / 10.0;
			}
			if (i == 0 || i == 99 || j == 0 || j == 99) {//境界線
				shadow[i * 400 + j * 4 + 0] = shadow[i * 400 + j * 4 + 1] = shadow[i * 400 + j * 4 + 2] = 20;
			}
		}
	}
	for (int i = 0; i < 1280; i++) {
		for (int j = 0; j < 600; j++) {
			storepowerR[j * 4 + i * 2400 + 0] = 255;
			storepowerB[j * 4 + i * 2400 + 2] = 255;
		}
	}
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < 105; j++) {
			zannryou[i * 420 + j * 4 + 0] = 255;//R
			zannryou[i * 420 + j * 4 + 1] = 255;//G
			zannryou[i * 420 + j * 4 + 3] = 255;
		}
	}

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			radar[i * 400 + j * 4 + 1] = 100;
			radar[i * 400 + j * 4 + 3] = 255;
			if (i == 50 || j == 50) {
				radar[i * 400 + j * 4 + 1] = 170;
				radar[i * 400 + j * 4 + 2] = 170;
				radar[i * 400 + j * 4 + 0] = 170;
				radar[i * 400 + j * 4 + 3] = 200;
			}
			else if (i == 25 || j == 25 || i == 75 || j == 75) {
				radar[i * 400 + j * 4 + 1] = 150;
				radar[i * 400 + j * 4 + 2] = 150;
				radar[i * 400 + j * 4 + 0] = 150;
				radar[i * 400 + j * 4 + 3] = 200;
			}
			if ((i - 50)*(i - 50) + (j - 50)*(j - 50) < 2304) {
				radar[i * 400 + j * 4 + 1] +=80 ;
				radar[i * 400 + j * 4 + 3] = 255;
				if ((((((i - 50)*(i - 50) + (j - 50)*(j - 50)) < 2304) && (((i - 50)*(i - 50) + (j - 50)*(j - 50)) > 2209))) ||
					(((i - 50)*(i - 50) + (j - 50)*(j - 50) < 1296 && (i - 50)*(i - 50) + (j - 50)*(j - 50) > 1225)) ||
					(((i - 50)*(i - 50) + (j - 50)*(j - 50) < 576 && (i - 50)*(i - 50) + (j - 50)*(j - 50) > 529)) ||
					(((i - 50)*(i - 50) + (j - 50)*(j - 50) < 144 && (i - 50)*(i - 50) + (j - 50)*(j - 50) > 121))) {
					radar[i * 400 + j * 4 + 1] = 230;
					radar[i * 400 + j * 4 + 2] = 150;
					radar[i * 400 + j * 4 + 0] = 150;
					radar[i * 400 + j * 4 + 3] = 255;
				}
			}
		}
	}
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			radarrot[i * 400 + j * 4 + 1] = 255;
		}
	}
}

class ProcessManage {
private:
	double time = 0;
	DisplayManage *displaymanage;
	Player *player;
	vector<Gravity> *gravity;
	ShotGravity* shotgravity;
	Enemy *enemy;
	double pUsepower;
	double dist(double x1,double  z1,double  x2,double z2) {
		return sqrt((x1 - x2)*(x1 - x2)/10000 + (z1 - z2)*(z1 - z2)/10000);//二点間の距離を計算
	}
	double mapcalc(double x, double z) {//x,z座標の高さを計算
		double temp = 0;
		for (int i = 0; i < gravity->size(); i++) {
			temp += (*gravity)[i].m / (10*(dist(x, z, (*gravity)[i].x, (*gravity)[i].z)+0.1)) ;
		}
		if (shotgravity->shottime < 119) {
			temp += (*shotgravity).m / (10 * (dist(x, z, (*shotgravity).x, (*shotgravity).z) + 0.1));
		}
		return temp;
	}
public:
	ProcessManage(DisplayManage* _displaymanage,Player* _player) {
		displaymanage =  _displaymanage;
		player =  _player;
		enemy = new Enemy;
		gravity = player->Getgravity();
		shotgravity = player->GetShot();
		displaymanage->Putgravity(gravity);
		displaymanage->PutShot(shotgravity);
		displaymanage->Putenemy(enemy);
		pUsepower=0;
		displaymanage->ChangepStorepower(player->Getstorepower());
		displaymanage->Changenowpower(player->Getnowpower());
		displaymanage->Pushmaxpower(player->Getmaxpower());
		player->PushBattery(displaymanage->GetBattery());
	}
	void process() {
		
//		cout <<"設置数"<< gravity->size() << ',';

//		displaymanage->ChangepStorepower(player->Getstorepower());
		//map関係
		if (shotgravity->shottime<120||pUsepower != player->Getusepower() || abs(displaymanage->GetCenterX() - player->GetX()) > MAPCHANGE || abs(displaymanage->GetCenterZ() - player->GetZ()) > MAPCHANGE) {
			int mapflag = 0;
			if (displaymanage->GetCenterX() - player->GetX() > MAPCHANGE) {
				displaymanage->ChangeCenterX(-MAPCHANGE);
				mapflag = 1;
			}
			else if (displaymanage->GetCenterX() - player->GetX() < -MAPCHANGE) {
				displaymanage->ChangeCenterX(+MAPCHANGE);
				mapflag = 2;
			}
			else if (displaymanage->GetCenterZ() - player->GetZ() > MAPCHANGE) {
				displaymanage->ChangeCenterZ(-MAPCHANGE);
				mapflag = 3;
			}
			else if (displaymanage->GetCenterZ() - player->GetZ() < -MAPCHANGE) {
				displaymanage->ChangeCenterZ(+MAPCHANGE);
				mapflag = 4;
			}
			pUsepower = player->Getusepower();
//			cout << gravity->size() << ',';
			double map[MAPWID][MAPLENGTH];
			double maptemp[MAPWID][MAPLENGTH];
			if (mapflag != 0) {
				for (int i = 0; i < MAPWID; i++) {
					for (int j = 0; j < MAPLENGTH; j++) {
						map[i][j] = displaymanage->GetMapY(i, j);
						maptemp[i][j] = displaymanage->GetMaptempY(i, j);

					}
				}
			}
			for (int i = 0; i <MAPWID; i++) {//MAP値計算
				for (int j = 0; j < MAPLENGTH; j++) {
					if (mapflag == 1) {
						double temp = mapcalc(i - MAPWID / 2.0 + displaymanage->GetCenterX(), j - MAPLENGTH / 2.0 + displaymanage->GetCenterZ());
						if (i >= MAPCHANGE) {
							displaymanage->mapchange(i, j, map[i-MAPCHANGE][j]);
							displaymanage->maptempchange(i, j, maptemp[i-MAPCHANGE][j]);
						}
						else {
							displaymanage->mapchange(i, j, temp);
							displaymanage->maptempchange(i, j, temp);
						}
					}
					else if (mapflag == 2) {
						double temp = mapcalc(i - MAPWID / 2.0 + displaymanage->GetCenterX(), j - MAPLENGTH / 2.0 + displaymanage->GetCenterZ());
						if (i <MAPWID- MAPCHANGE) {
							displaymanage->mapchange(i, j, map[i + MAPCHANGE][j]);
							displaymanage->maptempchange(i, j, maptemp[i + MAPCHANGE][j]);
						}
						else {
							displaymanage->mapchange(i, j, temp);
							displaymanage->maptempchange(i, j, temp);
						}
					}
					else if (mapflag == 3) {
						double temp = mapcalc(i - MAPWID / 2.0 + displaymanage->GetCenterX(), j - MAPLENGTH / 2.0 + displaymanage->GetCenterZ());
						if (j >= MAPCHANGE) {
							displaymanage->mapchange(i, j, map[i][j - MAPCHANGE]);
							displaymanage->maptempchange(i, j, maptemp[i][j - MAPCHANGE]);
						}
						else {
							displaymanage->mapchange(i, j, temp);
							displaymanage->maptempchange(i, j, temp);
						}
					}
					else if (mapflag == 4) {
						double temp = mapcalc(i - MAPWID / 2.0 + displaymanage->GetCenterX(), j - MAPLENGTH / 2.0 + displaymanage->GetCenterZ());
						if (j <MAPWID - MAPCHANGE) {
							displaymanage->mapchange(i, j, map[i][j + MAPCHANGE]);
							displaymanage->maptempchange(i, j, maptemp[i][j + MAPCHANGE]);
						}
						else {
							displaymanage->mapchange(i, j, temp);
							displaymanage->maptempchange(i, j, temp);
						}
					}
					else {
						displaymanage->maptempchange(i, j, mapcalc(i - MAPWID / 2.0 + displaymanage->GetCenterX(), j - MAPLENGTH / 2.0 + displaymanage->GetCenterZ()));
					}
				}
			}
			for (int i = 0; i < gravity->size();i++) {
				(*gravity)[i].y = mapcalc((*gravity)[i].x, (*gravity)[i].z);
			}
		}

//		cout <<"マップセンター座標"<< displaymanage->GetCenterX()<<','<< displaymanage->GetCenterZ()<<',';
		displaymanage->pLocateChange(player->GetX(), player->GetY(), player->GetZ(),player->GetWxz(),player->Getzoom(),player->GetJ());//情報を伝える
		double temp =
			(1.0-(player->GetZ() - (int)player->GetZ()))*((1.0-(player->GetX() - (int)player->GetX()))*displaymanage->GetMapY(MAPWID / 2.0 - displaymanage->GetCenterX() + player->GetX(), MAPLENGTH / 2.0 - displaymanage->GetCenterZ() + player->GetZ())
				+ (player->GetX() - (int)player->GetX())*displaymanage->GetMapY(MAPWID / 2.0 - displaymanage->GetCenterX() + player->GetX() + 1.0, MAPLENGTH / 2.0 - displaymanage->GetCenterZ() + player->GetZ()))
			+  (player->GetZ() - (int)player->GetZ())*((1.0-(player->GetX() - (int)player->GetX()))*displaymanage->GetMapY(MAPWID / 2.0 - displaymanage->GetCenterX() + player->GetX(), MAPLENGTH / 2.0 - displaymanage->GetCenterZ() + player->GetZ()+1.0)
				+ (player->GetX() - (int)player->GetX())*displaymanage->GetMapY(MAPWID / 2.0 - displaymanage->GetCenterX() + player->GetX() + 1.0, MAPLENGTH / 2.0 - displaymanage->GetCenterZ() + player->GetZ()+1.0));
			
		player->ChangeY(temp);
		
		enemy->idle(player->GetX(),player->GetZ());
		enemy->Changeyxy(mapcalc(enemy->GetX(), enemy->GetZ()), mapcalc(enemy->GetfX(), enemy->GetfZ()));
		if (((enemy->GetfX() - shotgravity->x)*(enemy->GetfX() - shotgravity->x) + (enemy->GetfZ() - shotgravity->z)*(enemy->GetfZ() - shotgravity->z)) < 4 + 0.2*shotgravity->m) {//敵への当たり判定
			enemy->Damagehp(abs(shotgravity->m));
		}
		if (abs(enemy->GetshotX() - player->GetX()) < 1 && abs(enemy->GetshotY() - player->GetY()) < 3 && abs(enemy->GetshotZ() - player->GetZ())<1&&enemy->GetShotnow()==true) {
			player->damage();
		}

	}
};