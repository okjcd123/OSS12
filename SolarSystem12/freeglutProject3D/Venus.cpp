#include "Venus.h"
#include "TextureLoader.h"


Venus::Venus()		//Earth�� ������
	: Sphere(20, 45, 45)	//Sphere(��)�� �߽� ��ǥ ����

{

	init();

}

Venus::~Venus()			//Earth�� �Ҹ���
{

	Venus::~Venus();

}



void Venus::init()
{

	unsigned int width, height;						//�ʺ�� ���� ����
	unsigned char * data = loadBMPRaw("venus.bmp", width, height, false);	//"������ �ҷ��´�

	glGenTextures(1, &textureID);						//OpenGL �ؽ��ĸ� �����
	glBindTexture(GL_TEXTURE_2D, textureID);				//����� �ؽ��� ������Ʈ. ������ �ؽ��Ĵ� �� �ؽ��ķ� �����Ѵ�.


															//OpenGL�� �̹��� ������ �����Ѵ�.	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	//��ǥ ���͸�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		//��巹��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		//��巹��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//Ȯ�� ���͸�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 	//��� ���͸�
	delete[] data;

}


void Venus::draw()	//ȭ�鿡 Earth�� �׸���
{

	glBindTexture(GL_TEXTURE_2D, textureID);	//����� �ؽ��� ������Ʈ
	gluQuadricTexture(quadric, GL_TRUE);		//������ ��� �ؽ��ĵ��� �� �ؽ��ĸ� ����Ѵ�.
	Sphere::draw();					//ȭ�鿡 Sphere�� �׸���
}
