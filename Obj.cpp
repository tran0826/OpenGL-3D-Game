//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------
#include "Obj.h"

#define SPC " 　\t"



/*!
* "文字列 数値"から数値部分の文字列のみを取り出す
* @param[in] buf 元の文字列
* @param[in] head 文字部分
* @param[out] sub 数値部分の文字列
*/
inline bool StringToString(const string &buf, const string &head, string &sub)
{
	size_t pos = 0;
	if ((pos = buf.find(head)) == string::npos) return false;
	pos += head.size();

	if ((pos = buf.find_first_not_of(SPC, pos)) == string::npos) return false;

	sub = buf.substr(pos);
	return true;
}

inline bool StringToDouble(const string &buf, const string &head, double &val)
{
	string sub;
	StringToString(buf, head, sub);

	sscanf(&sub[0], "%lf", &val);
	return true;
}


/*!
* "x y z"の形式の文字列からVec2型へ変換
* @param[in] s 文字列
* @param[out] v 値
* @return 要素記述数
*/
inline int StringToVec2s(const string &buf, const string &head, Vec2 &v)
{
	string sub;
	StringToString(buf, head, sub);

	Vec2 tmp;

	if (sscanf(&sub[0], "%lf %lf", &tmp[0], &tmp[1]) != 2) {
		return 0;
	}
	v = tmp;

	return 2;
}

/*!
* "x y z"の形式の文字列からVec3型へ変換
* @param[in] s 文字列
* @param[out] v 値
* @return 要素記述数
*/
inline int StringToVec3s(const string &buf, const string &head, Vec3 &v)
{
	string sub;
	StringToString(buf, head, sub);

	Vec3 tmp;
	double tmp_w;

	if (sscanf(&sub[0], "%lf %lf %lf %lf", &tmp[0], &tmp[1], &tmp[2], &tmp_w) != 4) {
		if (sscanf(&sub[0], "%lf %lf %lf", &tmp[0], &tmp[1], &tmp[2]) != 3) {
			return 0;
		}
	}
	v = tmp;

	return 3;
}

template<class T>
inline string ArrayToString(const T *v)
{
	stringstream ss;
	ss << v[0] << " " << v[1] << " " << v[2];
	return ss.str();
	//string s;
	//sscanf(&s[0], "%f %f %f", v[0], v[1], v[2]);
	//return s;
}

inline string Vec3ToString(const Vec3 v)
{
	stringstream ss;
	ss << v[0] << " " << v[1] << " " << v[2];
	return ss.str();
	//string s;
	//sscanf(&s[0], "%f %f %f", v[0], v[1], v[2]);
	//return s;
}


/*!
* 先頭の空白(スペース，タブ)を削除
* @param[in] buf 元の文字列
* @return 空白削除後の文字列
*/
inline string GetDeleteSpace(const string &buf)
{
	string buf1 = buf;

	size_t pos;
	while ((pos = buf1.find_first_of(" 　\t")) == 0) {
		buf1.erase(buf1.begin());
		if (buf1.empty()) break;
	}

	return buf1;
}

/*!
* 先頭の空白(スペース，タブ)を削除
* @param[inout] buf 処理文字列
*/
inline void DeleteSpace(string &buf)
{
	size_t pos;
	while ((pos = buf.find_first_of(" 　\t")) == 0) {
		buf.erase(buf.begin());
		if (buf.empty()) break;
	}
}

/*!
* 文字列間に含まれる指定された文字列の数を返す
* @param[in] s 元の文字列
* @param[in] c 検索文字列
* @return 含まれる数
*/
inline int CountString(string &s, int offset, string c)
{
	int count = 0;
	size_t pos0 = offset, pos = 0;
	int n = (int)c.size();

	while ((pos = s.find(c, pos0)) != string::npos) {
		if (pos != pos0) {
			count++;
		}
		else {
			s.erase(s.begin() + pos);
		}
		pos0 = pos + n;
	}

	// 最後の文字列除去
	if (s.rfind(c) == s.size() - n) {
		count--;
	}

	return count;
}

/*!
* ファイル名からフォルダパスのみを取り出す
* @param[in] fn ファイル名(フルパス or 相対パス)
* @return フォルダパス
*/
inline string ExtractDirPath(const string &fn)
{
	string::size_type pos;
	if ((pos = fn.find_last_of("/")) == string::npos) {
		if ((pos = fn.find_last_of("\\")) == string::npos) {
			return "";
		}
	}

	return fn.substr(0, pos);
}

/*!
* ファイル名から拡張子を削除
* @param[in] fn ファイル名(フルパス or 相対パス)
* @return フォルダパス
*/
inline string ExtractPathWithoutExt(const string &fn)
{
	string::size_type pos;
	if ((pos = fn.find_last_of(".")) == string::npos) {
		return fn;
	}

	return fn.substr(0, pos);
}



//-----------------------------------------------------------------------------
// rxVRMLクラスの実装
//-----------------------------------------------------------------------------
/*!
* コンストラクタ
*/
rxOBJ::rxOBJ(void)
{
	m_strCurrentMat = "";
	texID = 0;
	loadObj = false;
	loadTex = false;
}

rxOBJ::rxOBJ(string obj_file_name, string texture_file_name) {
	loadObj = false;
	loadTex = false;
	Read(obj_file_name, texture_file_name);
}

rxOBJ::rxOBJ(string obj_file_name) {
	loadObj = false;
	loadTex = false;
	Read(obj_file_name, "");
}



/*!
* デストラクタ
*/
rxOBJ::~rxOBJ()
{
}


bool rxOBJ::Read(string obj_file_name, string texture_file_name)
{
	ifstream file;

	file.open(obj_file_name.c_str());
	if (!file || !file.is_open() || file.bad() || file.fail()) {
		cout << "rxOBJ::Read : Invalid file specified" << endl;
		return false;
	}

	vector<Vec3> vnormals;
	vector<Vec2> vtexcoords;

	string buf;
	string::size_type comment_start = 0;
	while (!file.eof()) {
		getline(file, buf);

		// '#'以降はコメントとして無視
		if ((comment_start = buf.find('#')) != string::size_type(-1))
			buf = buf.substr(0, comment_start);

		// 行頭のスペース，タブを削除
		DeleteSpace(buf);

		// 空行は無視
		if (buf.empty())
			continue;

		if (buf[0] == 'v') {
			if (buf[1] == 'n') {		// 頂点法線
				Vec3 n;
				if (StringToVec3s(buf, "vn", n)) {
					vnormals.push_back(n);
				}

			}
			else if (buf[1] == 't') {	// テクスチャ座標
				Vec2 tc;
				if (StringToVec2s(buf, "vt", tc)) {
					vtexcoords.push_back(tc);
				}

			}
			else {					// 頂点座標
				Vec3 v;
				if (StringToVec3s(buf, "v", v)) {
					vrts.push_back(v);
					vnms.push_back(Vec3(0.0));
				}

			}
		}
		else if (buf[0] == 'f') {		// 面
			int num_face = 0;
			vector<int> vidxs, nidxs, tidxs;
			if (!(num_face = loadFace(buf, vidxs, nidxs, tidxs))) continue;



			if (num_face >= 4) {
				PolyToTri(plys, vidxs, tidxs, vtexcoords, m_strCurrentMat);
			}
			else {
				rxFace face;
				face.vertices.resize(num_face);;
				face.material = m_strCurrentMat;
				face.texcoords.resize(num_face);
				bool tc = !vtexcoords.empty();
				for (int i = 0; i < num_face; ++i) {
					face.vertices[i] = vidxs[i];
					if (tc) {
						face.texcoords[i] = vtexcoords[tidxs[i]];
					}
					else {
						face.texcoords[i] = Vec2(0.0);
					}
				}
				plys.push_back(face);
			}

			// 頂点法線
			if (!vnormals.empty()) {
				for (int i = 0; i < num_face; ++i) {
					vnms[vidxs[i]] += vnormals[nidxs[i]];
				}
			}
		}
	}

	if (vnormals.empty()) {
		vnms.clear();
	}

	if (!vnms.empty()) {
		for (int i = 0; i < (int)vnms.size(); ++i) {
			normalize(vnms[i]);
		}
	}

	mats = m_mapMaterials;

	file.close();
	loadObj = true;
	if (texture_file_name != "") {
		loadTexture(texture_file_name);
	}
	return true;
}

bool rxOBJ::loadTexture(string texture_file_name) {
	texture = cv::imread(texture_file_name);
	loadTex = true;
	return true;
}




int rxOBJ::loadFace(string &buf, vector<int> &vidxs, vector<int> &nidxs, vector<int> &tidxs)
{
	int num_face = CountString(buf, 2, " ") + 1;

	if (num_face >= 3) {
		vidxs.resize(num_face);
		nidxs.resize(num_face);
		tidxs.resize(num_face);

		int vidx = 0;	// 頂点インデックス
		int tidx = 0;	// テクスチャ座標インデックス
		int nidx = 0;	// 法線インデックス
		int offset = 2;
		for (int i = 0; i < num_face; ++i) {
			if (sscanf(&buf[0] + offset, "%d/%d/%d", &vidx, &tidx, &nidx) != 3) {
				if (sscanf(&buf[0] + offset, "%d//%d", &vidx, &nidx) != 2) {
					if (sscanf(&buf[0] + offset, "%d/%d", &vidx, &tidx) != 2) {
						sscanf(&buf[0] + offset, "%d", &vidx);	// 頂点座標のみ
						tidx = 0;
						nidx = 0;
						offset += (int)log10((double)vidx) + 1;
					}
					else {	// 頂点座標，テクスチャ座標
						nidx = 0;
						offset += (int)log10((double)vidx) + (int)log10((double)tidx) + 3;
					}
				}
				else {	// 頂点座標，法線
					tidx = 0;
					offset += (int)log10((double)vidx) + (int)log10((double)nidx) + 4;
				}
			}
			else { // 頂点座標，テクスチャ座標，法線
				offset += (int)log10((double)vidx) + (int)log10((double)tidx) + 1 + (int)log10((double)nidx) + 4;
			}

			offset++;	// スペース

			if (vidx >= 1) {
				vidxs[i] = vidx - 1;
				nidxs[i] = nidx - 1;
				tidxs[i] = tidx - 1;
			}
			else {
				return 0;
			}
		}
	}

	return num_face;
}

void rxOBJ::renderModel() {
	if (!loadObj) { return; }


	if (loadTex) {
		glEnable(GL_TEXTURE_2D);
		if (texID == 0) {
			glGenTextures(1, &texID);
		}

		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//色設定の指定　これが重要
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.cols, texture.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, texture.data);
	}

	for (int n = 0; n<(int)plys.size(); n++) {
		glBegin(GL_POLYGON);
		for (int m = 0; m<plys[n].vertices.size(); m++) {
			if (!vnms.empty()) {
				glNormal3dv(vnms[plys[n].vertices[m]].data);
			}
			if (loadTex) { glTexCoord2dv(plys[n].texcoords[m].data); }
			glVertex3dv(vrts[plys[n].vertices[m]].data);
		}
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}