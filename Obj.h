#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>

#include <map>
#include <glut.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2\opencv.hpp>

#ifdef _DEBUG
//Debugモードの場合
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_core2412d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_imgproc2412d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_highgui2412d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_objdetect2412d.lib")
/*#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib240d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d240d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_flann240d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu240d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy240d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_ts240d.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc10\\lib\\opencv_video240d.lib")
*/
#else
//Releaseモードの場合
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_core2412.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_imgproc2412.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_highgui2412.lib")
#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_objdetect2412.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_contrib240.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_features2d240.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_flann240.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_gpu240.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_haartraining_engined.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_legacy240.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_ts240.lib")
//#pragma comment(lib,"C:\\opencv\\build\\x86\\vc12\\lib\\opencv_video240.lib")
#endif
#include "rx_vec.h"


//-----------------------------------------------------------------------------
// Name Space
//-----------------------------------------------------------------------------
using namespace std;


#define RX_DEBUG_OUT 0

struct rxMaterialOBJ
{
	string name;

	float diffuse[4];
	float specular[4];
	float ambient[4];

	float color[4];
	float emission[4];

	double shininess;

	int illum;
	string tex_file;
	unsigned int tex_name;
};

struct rxFace
{
	vector<int> vertices;
	string material;
	vector<Vec2> texcoords;
};

typedef map<string, rxMaterialOBJ> rxMTL;


//-----------------------------------------------------------------------------
// rxOBJクラスの宣言 - OBJ形式の読み込み
//-----------------------------------------------------------------------------
class rxOBJ
{
	rxMTL m_mapMaterials;	//!< ラベルとデータのマップ
	string m_strCurrentMat;				//!< 現在のデータを示すラベル
	vector<Vec3> vrts;
	vector<Vec3> vnms;
	vector<rxFace> plys;
	rxMTL mats;
	cv::Mat texture;
	unsigned int texID;
	bool loadObj;
	bool loadTex;
	//vector<rxMaterialOBJ> m_vMaterials;
	//int m_iCurrentMat;

public:
	rxOBJ();
	rxOBJ(string obj_file_name, string texture_file_name);
	rxOBJ(string obj_file_name);
	~rxOBJ();

	bool Read(string obj_file_name, string texture_file_name);

	rxMTL GetMaterials(void) { return m_mapMaterials; }
	void renderModel();

private:
	int loadFace(string &buf, vector<int> &vidxs, vector<int> &nidxs, vector<int> &tidxs);
	bool loadTexture(string texture_file_name);
	int rxOBJ::PolyToTri(vector<rxFace> &plys, const vector<int> &vidxs, const vector<int> &tidxs, const vector<Vec2> &vtc, string mat_name)
	{
		int n = (int)vidxs.size();

		if (n <= 3) return 0;

		rxFace face;
		face.vertices.resize(3);
		face.material = mat_name;
		face.texcoords.resize(3);

		bool tc = !vtc.empty();

		int num_tri = n - 2;
		for (int i = 0; i < num_tri; ++i) {
			face.vertices[0] = vidxs[0];
			face.vertices[1] = vidxs[i + 1];
			face.vertices[2] = vidxs[i + 2];

			if (tc) {
				face.texcoords[0] = vtc[tidxs[0]];
				face.texcoords[1] = vtc[tidxs[i + 1]];
				face.texcoords[2] = vtc[tidxs[i + 2]];
			}
			else {
				face.texcoords[0] = Vec2(0.0);
				face.texcoords[1] = Vec2(0.0);
				face.texcoords[2] = Vec2(0.0);
			}

			plys.push_back(face);
		}

		return 1;
	}

};