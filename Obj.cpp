//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------
#include "Obj.h"

#define SPC " �@\t"



/*!
* "������ ���l"���琔�l�����̕�����݂̂����o��
* @param[in] buf ���̕�����
* @param[in] head ��������
* @param[out] sub ���l�����̕�����
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
* "x y z"�̌`���̕����񂩂�Vec2�^�֕ϊ�
* @param[in] s ������
* @param[out] v �l
* @return �v�f�L�q��
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
* "x y z"�̌`���̕����񂩂�Vec3�^�֕ϊ�
* @param[in] s ������
* @param[out] v �l
* @return �v�f�L�q��
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
* �擪�̋�(�X�y�[�X�C�^�u)���폜
* @param[in] buf ���̕�����
* @return �󔒍폜��̕�����
*/
inline string GetDeleteSpace(const string &buf)
{
	string buf1 = buf;

	size_t pos;
	while ((pos = buf1.find_first_of(" �@\t")) == 0) {
		buf1.erase(buf1.begin());
		if (buf1.empty()) break;
	}

	return buf1;
}

/*!
* �擪�̋�(�X�y�[�X�C�^�u)���폜
* @param[inout] buf ����������
*/
inline void DeleteSpace(string &buf)
{
	size_t pos;
	while ((pos = buf.find_first_of(" �@\t")) == 0) {
		buf.erase(buf.begin());
		if (buf.empty()) break;
	}
}

/*!
* ������ԂɊ܂܂��w�肳�ꂽ������̐���Ԃ�
* @param[in] s ���̕�����
* @param[in] c ����������
* @return �܂܂�鐔
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

	// �Ō�̕����񏜋�
	if (s.rfind(c) == s.size() - n) {
		count--;
	}

	return count;
}

/*!
* �t�@�C��������t�H���_�p�X�݂̂����o��
* @param[in] fn �t�@�C����(�t���p�X or ���΃p�X)
* @return �t�H���_�p�X
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
* �t�@�C��������g���q���폜
* @param[in] fn �t�@�C����(�t���p�X or ���΃p�X)
* @return �t�H���_�p�X
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
// rxVRML�N���X�̎���
//-----------------------------------------------------------------------------
/*!
* �R���X�g���N�^
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
* �f�X�g���N�^
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

		// '#'�ȍ~�̓R�����g�Ƃ��Ė���
		if ((comment_start = buf.find('#')) != string::size_type(-1))
			buf = buf.substr(0, comment_start);

		// �s���̃X�y�[�X�C�^�u���폜
		DeleteSpace(buf);

		// ��s�͖���
		if (buf.empty())
			continue;

		if (buf[0] == 'v') {
			if (buf[1] == 'n') {		// ���_�@��
				Vec3 n;
				if (StringToVec3s(buf, "vn", n)) {
					vnormals.push_back(n);
				}

			}
			else if (buf[1] == 't') {	// �e�N�X�`�����W
				Vec2 tc;
				if (StringToVec2s(buf, "vt", tc)) {
					vtexcoords.push_back(tc);
				}

			}
			else {					// ���_���W
				Vec3 v;
				if (StringToVec3s(buf, "v", v)) {
					vrts.push_back(v);
					vnms.push_back(Vec3(0.0));
				}

			}
		}
		else if (buf[0] == 'f') {		// ��
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

			// ���_�@��
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

		int vidx = 0;	// ���_�C���f�b�N�X
		int tidx = 0;	// �e�N�X�`�����W�C���f�b�N�X
		int nidx = 0;	// �@���C���f�b�N�X
		int offset = 2;
		for (int i = 0; i < num_face; ++i) {
			if (sscanf(&buf[0] + offset, "%d/%d/%d", &vidx, &tidx, &nidx) != 3) {
				if (sscanf(&buf[0] + offset, "%d//%d", &vidx, &nidx) != 2) {
					if (sscanf(&buf[0] + offset, "%d/%d", &vidx, &tidx) != 2) {
						sscanf(&buf[0] + offset, "%d", &vidx);	// ���_���W�̂�
						tidx = 0;
						nidx = 0;
						offset += (int)log10((double)vidx) + 1;
					}
					else {	// ���_���W�C�e�N�X�`�����W
						nidx = 0;
						offset += (int)log10((double)vidx) + (int)log10((double)tidx) + 3;
					}
				}
				else {	// ���_���W�C�@��
					tidx = 0;
					offset += (int)log10((double)vidx) + (int)log10((double)nidx) + 4;
				}
			}
			else { // ���_���W�C�e�N�X�`�����W�C�@��
				offset += (int)log10((double)vidx) + (int)log10((double)tidx) + 1 + (int)log10((double)nidx) + 4;
			}

			offset++;	// �X�y�[�X

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
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//�F�ݒ�̎w��@���ꂪ�d�v
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