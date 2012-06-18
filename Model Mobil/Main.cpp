/**
    Keterangan   : Tugas Besar Mata Kuliah Grafika Komputer
    Nama Program : Mobil 3D OpenGL
    Kelompok 2
    Anggota Kelompok : 1. Wanda             10108419
                       2. Giri Supangkat    10108423
                       3. Ringga Anggiat S  10107569
                       4. Muzakki Fadhil    10108474
*/

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <math.h>
#include <time.h>
#include "MilkshapeModel.h"											// Header File For Milkshape File

HDC			hDC=NULL;
HGLRC		hRC=NULL;
HWND		hWnd=NULL;
HINSTANCE	hInstance;

typedef struct {
	int frame;
	float xc;
	float yc;
	float zc;
	float anc;
	int gira;
	int vel;
	float Iox;
	float Ioy;
	float Ioz;
	int cameratype;
} VAR;

VAR var;	// variabel global

bool	keys[256];
bool	active=TRUE;
bool	fullscreen=TRUE;

Model *Auto = NULL;		// Model Auto
Model *Ruota = NULL;	// Model velg

GLuint	texture[3];

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ********************************************************************** variabel
typedef struct
{
	double x;
	double y;
	double z;
} Vector;

typedef struct	// komponen mobil
{
	// mobil
	float x;
	float y;
	float z;
	float any;
	float dany;
	float velocita;
	float vf;
	float dvf;
	// Rute Jalan
	float xruote[4];
	float yruote[4];
	float zruote[4];
	float anruote;
	float gira;
} CAR;

CAR car;

typedef	struct	// Komponen Asap
{
	float x;
	float y;
	float z;
	float dx;
	float dy;
	float dz;
	float dim;
	float ddim;
} PUF;

#define MAXSMOKE 64
PUF smoke[MAXSMOKE];	// Array Asap

#define DIMX 64
#define DIMZ 64
#define SIZE 32
Vector Buf[DIMX][DIMZ];		// Terrain

// ********************************************************************** Fungsi
double VectorLength( Vector v )
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

//-------------------------------------------------------------------
Vector VectorNormalize( Vector v )
{
	Vector vresult;
	double l = VectorLength( v );

	vresult.x = v.x/l;
	vresult.y = v.y/l;
	vresult.z = v.z/l;

	return vresult;
}

//-------------------------------------------------------------------
Vector VectorMultiply( Vector v1, Vector v2 )
{
	Vector vresult;

	vresult.x = v1.y * v2.z - v1.z * v2.y;
	vresult.y = v1.z * v2.x - v1.x * v2.z;
	vresult.z = v1.x * v2.y - v1.y * v2.x;

	return vresult;
}

//-------------------------------------------------------------------
Vector VectorScalarMultiply( Vector v, double s )
{
	Vector vresult;

	vresult.x = v.x * s;
	vresult.y = v.y * s;
	vresult.z = v.z * s;

	return vresult;
}

//-------------------------------------------------------------------
Vector VectorDiff( Vector v1, Vector v2 )
{
	Vector vresult;

	vresult.x = v1.x - v2.x;
	vresult.y = v1.y - v2.y;
	vresult.z = v1.z - v2.z;

	return vresult;
}

void Luce(double x, double y, double z,double dimensione,float r,float g,float b,float iox,float ioy,float ioz,int Tex)
{
	Vector Position;
	Vector MyPosition;
	Position.x = x;
	Position.y = y;
	Position.z = z;

	MyPosition.x=iox;
	MyPosition.y=ioy;
	MyPosition.z=ioz;
	Vector sight = VectorDiff(MyPosition, Position);

	Vector cz;
	cz.x = 0;
	cz.y = 0;
	cz.z = 1;

	Vector cross1 = VectorMultiply( sight, cz );
	Vector cross2 = VectorMultiply( sight, cross1 );

	cross1 = VectorNormalize(cross1);
	cross2 = VectorNormalize(cross2);

	cross1 = VectorScalarMultiply(cross1, dimensione);
	cross2 = VectorScalarMultiply(cross2, dimensione);

	glColor3f(r,g,b);
	glEnable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	glBlendFunc( (1,1,1,1), (1,1,1,1));
	glDepthMask (GL_FALSE);

	glBindTexture( GL_TEXTURE_2D, texture[Tex] );
	glBegin(GL_QUADS);
		glTexCoord2d( 0.0, 0.0 );
		glVertex3d( Position.x + cross1.x, Position.y + cross1.y, Position.z + cross1.z);
		glTexCoord2d( 1.0, 0.0 );
		glVertex3d( Position.x - cross2.x, Position.y - cross2.y, Position.z - cross2.z);
		glTexCoord2d( 1.0, 1.0 );
		glVertex3d( Position.x - cross1.x, Position.y - cross1.y, Position.z - cross1.z);
		glTexCoord2d( 0.0, 1.0 );
		glVertex3d( Position.x + cross2.x, Position.y + cross2.y, Position.z + cross2.z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable (GL_BLEND);
	glDepthMask (GL_TRUE);
}

void glPrint(char *ll,float x,float y,float z,float dim,float dir,float r,float g,float b)
{
	int t;
	char k;
	float kx,ky,fx,fy;

	fx=(float)1/16;
	fy=(float)1/16;

	glColor3f(r,g,b);
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, texture[0] );
	glEnable (GL_BLEND);
	glBlendFunc( (1,1,1,1), (1,1,1,1));
	glDepthMask (GL_FALSE);
	for(t=0;t<(int)strlen(ll);t++)
	{
		k=*(ll+t);
		if(k>=32)
		{
			k=k-32;
			kx=(float)(k%16);
			kx=kx*fx;
			ky=15-(float)((int)(k/16));
			ky=ky*fy;

			glBegin(GL_QUADS);
			glTexCoord2d( kx,ky );
			glVertex3f(x,y,z);
			glTexCoord2d( kx+fx, ky );
			glVertex3f(x+dim,y,z);
			glTexCoord2d( kx+fx,ky+fy );
			glVertex3f(x+dim,y+dim,z);
			glTexCoord2d( kx,ky+fy );
			glVertex3f(x,y+dim,z);
			glEnd();
			x=x+dim;
		}
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDisable(GL_TEXTURE_2D);
}

void InitBuf(void)	// Init Terrain
{
	int x,z,t;
	double xi,zi;

	for(z=0;z<DIMZ;z++)
	{
		zi=(double)z-((double)DIMZ/2);
		zi=zi*SIZE;
		for(x=0;x<DIMX;x++)
		{
			xi=(double)x-((double)DIMX/2);
			xi=xi*SIZE;
			Buf[x][z].x=xi;
			if(z==0 || z==DIMZ-2 || x==0 || x==DIMX-2)
				Buf[x][z].y=30;
			else
				Buf[x][z].y=0;
			Buf[x][z].z=zi;
		}
	}
}

void InitSmoke(void)	// Init Asap
{
	int t,k=0;
	float xf,yf,zf;

	for(t=0;t<MAXSMOKE;t++)
	{
		switch(t%4)	// For each velg
		{
		case 0:
			xf=2.5;
			yf=0.3;
			zf=4.5;
			break;
		case 1:
			xf=-2.5;
			yf=0.3;
			zf=4.5;
			break;
		case 2:
			xf=2.5;
			yf=0.3;
			zf=-4.5;
			break;
		case 3:
			xf=-2.5;
			yf=0.3;
			zf=-4.5;
			break;
		}

		smoke[t].x=xf;
		smoke[t].y=yf;
		smoke[t].z=zf;
		smoke[t].dim=0;
		smoke[t].ddim=0.5+(float)(rand()%100)/1000;
		smoke[t].dx=0;
		smoke[t].dy=0.01;
		smoke[t].dz=-car.velocita;
	}
}

void Init(void)
{
	var.frame=0;	// kalkulasi rate frame

	// Init Terrain
	InitBuf();

	// Init the Wheel Smoke
	InitSmoke();

	// kamera
	var.anc=-20;
	var.gira=false;	// True when the car is steering
	var.vel=0;	// >0 if the speed car change
	var.cameratype=0;

	// Posisi velg dan mobil
	car.x=0;
	car.y=0.75;
	car.z=0;
	car.any=0;
	car.dany=0;
	car.velocita=10;	// Inisialisasi kecepatan
	car.vf=10;
	car.dvf=0;

	car.xruote[0]=2.4;
	car.yruote[0]=0.2;
	car.zruote[0]=-5.2;
	car.xruote[1]=-2.4;
	car.yruote[1]=0.2;
	car.zruote[1]=-5.2;
	car.xruote[2]=2.4;
	car.yruote[2]=0.2;
	car.zruote[2]=4.3;
	car.xruote[3]=-2.4;
	car.yruote[3]=0.2;
	car.zruote[3]=4.3;
	car.anruote=0;	// Sudut velg
	car.gira=0;		// Sudut memulai mobil

	// Inisialisasi posisi kamera
	var.Iox=car.x+50*cos((car.any+(rand()%90-rand()%90))*3.141/180);
	var.Ioy=(float)(1+rand()%10);
	var.Ioz=car.z+50*sin((car.any+(rand()%90-rand()%90))*3.141/180);
}

void DrawBuf(void)	// Membuat tanah
{
	register int x,z,ptr;
	register float fk=(float)1/100;

	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, texture[1] );

	for(z=0;z<DIMZ-1;z++)
	{
		for(x=0;x<DIMX-1;x++)
		{
			glBegin(GL_QUADS);
			glTexCoord2d( 0.0, 0.0 );
			glVertex3f(Buf[x][z].x,Buf[x][z].y,Buf[x][z].z);
			glTexCoord2d( 0.0, 1.0 );
			glVertex3f(Buf[x][z+1].x,Buf[x][z+1].y,Buf[x][z+1].z);
			glTexCoord2d( 1.0, 1.0 );
			glVertex3f(Buf[x+1][z+1].x,Buf[x+1][z+1].y,Buf[x+1][z+1].z);
			glTexCoord2d( 1.0, 0.0 );
			glVertex3f(Buf[x+1][z].x,Buf[x+1][z].y,Buf[x+1][z].z);
			glEnd();
		}
	}

	glDisable(GL_TEXTURE_2D);
}

void DrawSmoke(void)	// Membuat asap
{
	int t;
	float xf,yf,zf,kf;

	for(t=0;t<MAXSMOKE;t++)
	{
		kf=1-smoke[t].dim;
		glPushMatrix();
		glTranslatef(car.x,0,car.z);
		glRotatef(car.any,0,1,0);
		Luce(smoke[t].x,smoke[t].y,smoke[t].z,smoke[t].dim,kf,kf,kf,var.xc,var.yc,var.zc,2);
		glPopMatrix();

		smoke[t].dim+=smoke[t].ddim;
		if(smoke[t].dim>=1)
		{
			switch(t%4)
			{
			case 0:
				xf=2.5;
				yf=0.3;
				zf=4.5;
				break;
			case 1:
				xf=-2.3;
				yf=0.3;
				zf=4.5;
				break;
			case 2:
				xf=2.5;
				yf=0.3;
				zf=-4.5;
				break;
			case 3:
				xf=-2.3;
				yf=0.3;
				zf=-4.5;
				break;
			}
			smoke[t].x=xf;
			smoke[t].y=yf;
			smoke[t].z=zf;
			smoke[t].dim=(float)(rand()%100)/100;
			smoke[t].ddim=0.1+(float)(rand()%100)/1000;
			smoke[t].dx=0;
			smoke[t].dy=0.01;
			smoke[t].dz=car.velocita/50;
		}
		else
		{
			smoke[t].dz/=1.1;

			smoke[t].x+=smoke[t].dx;
			smoke[t].y+=smoke[t].dy;
			smoke[t].z+=smoke[t].dz;
		}
	}

	glColor3f(1,1,1);
}
// **********************************************************************************

AUX_RGBImageRec *LoadBMP(const char *Filename)
{
	FILE *File=NULL;

	if (!Filename)
	{
		return NULL;
	}

	File=fopen(Filename,"r");

	if (File)
	{
		fclose(File);
		return auxDIBImageLoad(Filename);
	}

	return NULL;
}

GLuint LoadGLTexture( const char *filename )						// Load Bitmaps dan Convert ke Textures
{
	AUX_RGBImageRec *pImage;										// membuat alokasi memori untuk Texture
	GLuint texture = 0;												// Texture ID

	// Load gambar Bitmap, Cek Errors, jika gambar Bitmap tidak ditemukan, keluar
	pImage = LoadBMP( filename );									// Loads spesifikasi gambar Bitmap dari filename
	if ( pImage != NULL && pImage->data != NULL )					// jika Texture  gambar ada
	{
		glGenTextures(1, &texture);									// buat tekstur

		// Generasi Tekstur Khas Menggunakan Data Dari Bitmap ini
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);

		free(pImage->data);											// kosongkan alokasi memori
		free(pImage);												// kosongkan komponen gambar
	}

	return texture;													// kembali ke status
}

int LoadGLTextures()
{
	int Status=FALSE;

	AUX_RGBImageRec *TextureImage[1];

	memset(TextureImage,0,sizeof(void *)*1);

	if (TextureImage[0]=LoadBMP("Data/Font.bmp"))
	{
		Status=TRUE;

		glGenTextures(1, &texture[0]);

		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[0]=LoadBMP("Data/Floor.bmp"))
	{
		Status=TRUE;

		glGenTextures(1, &texture[1]);

		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[0]=LoadBMP("Data/Smoke.bmp"))
	{
		Status=TRUE;

		glGenTextures(1, &texture[2]);

		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[0])
	{
		if (TextureImage[0]->data)
		{
			free(TextureImage[0]->data);
		}

		free(TextureImage[0]);
	}

	return Status;
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height==0)
	{
		height=1;
	}

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,2000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int InitGL(GLvoid)
{
	GLfloat LightAmbientA[]=	{ 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuseA[]=		{ 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightSpecularA[]=		{ 0.7f, 0.7f, 0.5f, 1.0f };
	GLfloat LightPositionA[]=	{ 0.0f, 15.0f, 0.0f, 1.0f };
	GLfloat fgcolor[]=		{ 0.0f,0.0f,0.0f };

	if (!LoadGLTextures())
	{
		return FALSE;
	}

	Init();

	Auto->reloadTextures();										// Load tekstur model
	Ruota->reloadTextures();										// Loads tekstur model

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbientA);		// Setup cahaya ambient
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuseA);		// Setup cahaya Diffuse
	glLightfv(GL_LIGHT0, GL_POSITION,LightPositionA);	// Posisi cahaya
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecularA);		// Setup cahaya Ambient
	glEnable(GL_LIGHT0);

	glFogfv(GL_FOG_COLOR,fgcolor);
	glFogf(GL_FOG_START,120.0f);
	glFogf(GL_FOG_END,200.0f);
	glFogf(GL_FOG_MODE,GL_LINEAR);

//	glEnable(GL_FOG);

	return TRUE;
}

int DrawGLScene(GLvoid)
{
	int t;
	float dist;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Posisi kamera
	if(var.cameratype==0)
	{
		dist=sqrt( (car.x-var.Iox)*(car.x-var.Iox)+(car.z-var.Ioz)*(car.z-var.Ioz) );
		if(dist>150)
		{
			srand(rand());
			t=(rand()%45-rand()%45);
			var.Iox=car.x+50*cos((car.any+90+t)*3.141/180);
			var.Ioy=(float)(1+rand()%10);
			var.Ioz=car.z-50*sin((car.any+90+t)*3.141/180);
		}
		gluLookAt(var.Iox,var.Ioy,var.Ioz,car.x,1.0f,car.z,0.0f,1.0f,0.0f);
	}
	if(var.cameratype==1)
	{
		var.xc=car.x+30*cos(var.anc*3.141/180);
		var.yc=8;
		var.zc=car.z+30*sin(var.anc*3.141/180);
		gluLookAt(var.xc,var.yc,var.zc,car.x,0.0f,car.z,0.0f,1.0f,0.0f);
		var.anc+=0.5;
	}

	// Random untuk mobil balik
	if(var.gira==false)
	{
		if(rand()%100==1)
		{
			car.anruote=(float)(rand()%30-rand()%30);
			var.gira=true;
		}
	}
	// Random perubahan kecepatan
	if(var.vel==0)
	{
		if(rand()%100==1)
		{
			car.vf=10+(float)(rand()%30);
			car.dvf=(car.vf-car.velocita)/100;
			var.vel=100;
		}
	}
	else
	{
		car.velocita+=car.dvf;
		var.vel--;
	}

	DrawBuf();	// membuat terrain

	//Sudut mobil dan velg
	car.any+=car.dany;
	car.dany=car.anruote/30;
	car.anruote/=1.01;
	if(fabs(car.anruote)<2)
	{
		var.gira=false;
		car.anruote=0;
	}

	// mengubah arah mobil
	car.x+=car.velocita*cos((car.any+90)*3.141/180)/30;
	car.z+=-car.velocita*sin((car.any+90)*3.141/180)/30;

	// Clipping
	if( car.x<-400 )
		car.x=400;
	if( car.x>400 )
		car.x=-400;
	if( car.z<-400 )
		car.z=400;
	if( car.z>400 )
		car.z=-400;

	// membuat bayangan
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_DST_COLOR);
	glPushMatrix();
		glTranslatef(car.x+sin(car.any*3.141/180),0,car.z+sin(car.any*3.141/180));
		glRotatef(car.any,0,1,0);
		glColor3f(0.1,0.1,0.1);
		glBegin(GL_QUADS);
			glVertex3f(-2.5,0.01,-6);
			glVertex3f(2.5,0.01,-6);
			glVertex3f(2.5,0.01,6);
			glVertex3f(-2.5,0.01,6);
		glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);

	// membuat mobil
	glColor3f(1,1,1);
	glEnable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(car.x,car.y,car.z);
	glRotatef(car.any,0,1,0);
	Auto->draw();
	// membuat velg
	for(t=0;t<4;t++)
	{
		glPushMatrix();
		glTranslatef(car.xruote[t],car.yruote[t]+(float)(rand()%10)/100,car.zruote[t]);
		if(t==0 || t==1)
			glRotatef(car.anruote,0,1,0);
		if(t==1 || t==3)
		{
			glRotatef(180,0,1,0);
			glRotatef(-car.gira,1,0,0);
		}
		if(t==0 || t==2)
			glRotatef(car.gira,1,0,0);

		Ruota->draw();
		glPopMatrix();
		car.gira-=car.velocita/3;
	}
	glPopMatrix();
	glDisable(GL_LIGHTING);

	DrawSmoke();	// membuat asap velg

	var.frame++;

	return TRUE;
}

GLvoid KillGLWindow(GLvoid)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}

	if (hRC)
	{
		if (!wglMakeCurrent(NULL,NULL))
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;
	}

	if (hDC && !ReleaseDC(hWnd,hDC))
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;
	}

	if (hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;
	}

	if (!UnregisterClass("OpenGL",hInstance))
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;
	}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)height;

	fullscreen=fullscreenflag;

	Auto = new MilkshapeModel();									// memori untuk menahan/rem model
	if ( Auto->loadModelData( ".\\data\\auto.ms3d" ) == false )		// Banyak Model Dan Cek Untuk Kesalahan
	{
		MessageBox( NULL, "Couldn't load the model Auto.ms3d", "Error", MB_OK | MB_ICONERROR );
		return 0;													// jika Model gagal Load maka keluar
	}

	Ruota = new MilkshapeModel();									// memori untuk menahan/rem model
	if ( Ruota->loadModelData( ".\\data\\ruota.ms3d" ) == false )		// Banyak Model Dan Cek Untuk Kesalahan
	{
		MessageBox( NULL, "Couldn't load the model Ruota.ms3d", "Error", MB_OK | MB_ICONERROR );
		return 0;													// jika Model gagal Load maka keluar
	}

	hInstance			= GetModuleHandle(NULL);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= bits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;
			}
			else
			{
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	if (fullscreen)
	{
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(hWnd=CreateWindowEx(	dwExStyle,
								"OpenGL",
								title,
								dwStyle |
								WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN,
								0, 0,
								WindowRect.right-WindowRect.left,
								WindowRect.bottom-WindowRect.top,
								NULL,
								NULL,
								hInstance,
								NULL)))
	{
		KillGLWindow();
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	if (!(hDC=GetDC(hWnd)))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC=wglCreateContext(hDC)))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!wglMakeCurrent(hDC,hRC))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ReSizeGLScene(width, height);

	if (!InitGL())
	{
		KillGLWindow();
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(	HWND	hWnd,
							UINT	uMsg,
							WPARAM	wParam,
							LPARAM	lParam)
{
	switch (uMsg)
	{
		case WM_ACTIVATE:
		{
			if (!HIWORD(wParam))
			{
				active=TRUE;
			}
			else
			{
				active=FALSE;
			}

			return 0;
		}

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_KEYDOWN:
		{
			keys[wParam] = TRUE;
			return 0;
		}

		case WM_KEYUP:
		{
			keys[wParam] = FALSE;
			return 0;
		}

		case WM_SIZE:
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
			return 0;
		}
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					LPSTR		lpCmdLine,
					int			nCmdShow)
{
	MSG		msg;
	BOOL	done=FALSE;
	clock_t t1,t2;
	char s[80];

	fullscreen=FALSE;

	if (!CreateGLWindow("CARMODEL by eyes22",800,600,32,fullscreen))
	{
		return 0;
	}

	srand(clock());
	t1=clock();
	var.frame=0;
	while(!done)
	{
		t2=clock();
		if( (t2-t1)>=CLK_TCK )
		{
			if(var.vel==0)
				sprintf(s,"CARMODEL by eyes22 (fps : %f)\0",(float)var.frame/( (float)(clock()-t1)/CLK_TCK) );
			else
				sprintf(s,"CARMODEL by eyes22 (fps : %f) Speed\0",(float)var.frame/( (float)(clock()-t1)/CLK_TCK) );
			t1=clock();
			var.frame=0;
			SetWindowText(hWnd,s);
		}
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				done=TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])
			{
				done=TRUE;
			}
			else
			{
				SwapBuffers(hDC);

				if (keys['1'])
				{
					var.Iox=car.x+50*cos((car.any+(rand()%90-rand()%90))*3.141/180);
					var.Ioy=(float)(1+rand()%10);
					var.Ioz=car.z+50*sin((car.any+(rand()%90-rand()%90))*3.141/180);
					var.cameratype=0;
					keys['1']=0;
				}
				if (keys['2'])
				{
					var.cameratype=1;
					keys['2']=0;
				}
				if (keys['G'])
				{
					car.anruote=(float)(rand()%30-rand()%30);
					var.gira=true;
					keys['G']=0;
				}
				if (keys['V'])
				{
					car.vf=10+(float)(rand()%30);
					car.dvf=(car.vf-car.velocita)/100;
					var.vel=100;
					keys['V']=0;
				}
			}
		}
	}

	KillGLWindow();
	return (msg.wParam);
}
