/*********************************************************************************************
파 일 명 : Naptune.h
목    적 : Naptune 클래스의 선언
사용방식 : Header Files 내부에 위치
제한사항 : Naptune 클래스가 Sphere 클래스를 상속받기 때문에 클래스 내에서 draw 함수 재정의 필요
**********************************************************************************************/

#ifndef NaptuneH
#define NaptuneH

#include "Sphere.h"	

/* Sphere 클래스를 상속받는 Naptune 클래스 선언 */
class Naptune : public Sphere				
{
	/* Naptune 클래스의 private 멤버 */
	private: 				
		GLuint textureID;		//텍스처 번호가 저장되는 텍스쳐ID
		void init();			//천왕성을 그릴 때 필요한 함수들이 정의된 초기화 함수

	/* Naptune 클래스의 private 멤버 */
	public:				
		Naptune();				//Naptune 객체 생성자
		~Naptune();				//Naptune 객체 소멸자
		void draw();			//천왕성을 화면에 그려주는 함수
};


#endif

