#include <irrlicht.h>
#include <strings.h>
using namespace irr;

#include <irrKlang.h>
using namespace irrklang;

#include "windcontrol.h"

#include <io.h>


#ifndef ORION__H
#define ORION__H



#ifdef WIN32
  const char * SEPARATOR = "\\";
#else
  const char * SEPARATOR = "/";
#endif


// Global variables
IrrlichtDevice *device;
ISoundEngine* engine;




bool quit = false;
bool gameover = false;
int nTiros = 999;
float nEnergia = 999;
int nEscudo = 999;
float nVelocidade = 0.01f;
int nScreenShots;
char * pPath;
int nWindControl = 2;


// Recebe comandos do teclado
class MyEventReceiver : public IEventReceiver
{
public:


//********************************************************************
void tiro()
{
	scene::ISceneManager* sm = device->getSceneManager();
	scene::ICameraSceneNode* camera = sm->getActiveCamera();
	if (!camera ) 	        return;
    if ( nTiros == 0 ) 		return;


	nTiros--;

	bool impacto = 0;
    core::vector3df pos;
	core::vector3df outVector;

	// Pega posicao da camera
	core::vector3df start = camera->getPosition();
	core::vector3df end = (camera->getTarget() - start);

	start += end*40.0f;
	end = start * 2 + (end * camera->getFarValue() ) ;
	end = end * 10.0;



	// Cria o tiro
	scene::ISceneNode* firenode =  sm->addBillboardSceneNode(0, core::dimension2d<f32>(40,40), start);
	firenode->setMaterialFlag(video::EMF_LIGHTING, false);
	firenode->setMaterialTexture(0, device->getVideoDriver()->getTexture("media/tiro.png"));
	firenode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

    core::line3d<f32> line(start, end);
    scene::ISceneNode * Node = sm->getSceneCollisionManager()->getSceneNodeFromCameraBB(camera);
    //scene::ISceneNode * Node = sm->getSceneCollisionManager()->getSceneNodeFromRayBB(line, 0, false)

    // Verifica se encontrou um objeto
    if ( Node  &&  ( Node->getType() == scene::ESNT_MESH  ||  Node->getType() == scene::ESNT_ANIMATED_MESH ) ) {


		impacto = 1;
		outVector = Node->getPosition();
		pos = end;
	}

	// cria direcao do tiro
    f32 length = (f32)(end - start).getLength();
	const f32 speed = 0.4f;


    // cria direcao da particula
	scene::ISceneNodeAnimator* anim = sm->createFlyStraightAnimator(start, end, 6000);
    firenode->addAnimator(anim);
	anim->drop();


    // Faz tiro desintegrar depois de 1 segundo
	anim = sm->createDeleteAnimator(3000);
	firenode->addAnimator(anim);
	anim->drop();



	if ( impacto )
	{


        printf("Buum\n");



        // Scene Node da explosao
        scene::IParticleSystemSceneNode* ps;
        ps = sm->addParticleSystemSceneNode(false);
        ps->setPosition(core::vector3df(Node->getPosition() ));
        ps->setScale(core::vector3df(0.5,0.5,0.5));
        ps->setParticleSize(core::dimension2d<f32>(10.0f, 10.0f));


        // Adiciona Fumaça
        scene::IParticleEmitter* em = ps->createBoxEmitter(
        core::aabbox3d<f32>(-6,0,-6,6,1,6),
        core::vector3df(0.05f,0.1f,0.0f), 20,70, video::SColor(0,255,255,255), video::SColor(0,255,255,255),100,500,360);
        ps->setEmitter(em);
        em->drop();

        scene::IParticleAffector* paf = ps->createFadeOutParticleAffector();
        ps->addAffector(paf);
        paf->drop();
        ps->setMaterialFlag(video::EMF_LIGHTING, false);
        ps->setMaterialTexture(0, device->getVideoDriver()->getTexture("media/fumaca.bmp"));
        ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

        // Adiciona Faiscas
        scene::IParticleSystemSceneNode* ps2;
        ps2 = sm->addParticleSystemSceneNode(false);
        ps2->setPosition(core::vector3df(Node->getPosition() ));
        ps2->setScale(core::vector3df(0.5,0.5,0.5));
        ps2->setParticleSize(core::dimension2d<f32>(10.0f, 10.0f));

        scene::IParticleEmitter* em2 = ps->createBoxEmitter(
        core::aabbox3d<f32>(-6,0,-6,6,1,6),
        core::vector3df(0.05f,0.1f,0.0f), 20,100, video::SColor(0,255,255,255), video::SColor(0,255,255,255),100,500,360);
        ps2->setEmitter(em2);
        em2->drop();

        scene::IParticleAffector* paf2 = ps2->createFadeOutParticleAffector();
        ps2->addAffector(paf);
        paf2->drop();
        ps2->setMaterialFlag(video::EMF_LIGHTING, false);
        ps2->setMaterialTexture(0, device->getVideoDriver()->getTexture("media/tiro_inimigo.png"));
        ps2->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);


        //Faz particulas desaparecerem
        scene::ISceneNodeAnimator* anim = sm->createDeleteAnimator(1000);
	    ps->addAnimator(anim);
    	ps2->addAnimator(anim);
	    anim->drop();

		Node->setVisible(false);

	    if (engine) {
	       ISound* sound = engine->play3D("media/explosion.ogg", Node->getPosition(), false, false, true);
		   if (sound) {
			  sound->setMinDistance(100);
		      sound->drop();
           }
	    }







		//impacto = device->getTimer()->getTime() + (time - 100);
	}

	// play sound
	if (engine) engine->play2D("media/Laser1.ogg");
}




	virtual bool OnEvent(const SEvent &event)
	{
    if ( !device ) return 0;
	scene::ISceneManager* sm = device->getSceneManager();
	scene::ICameraSceneNode* camera = sm->getActiveCamera();


	if (camera != 0 && event.EventType == irr::EET_KEY_INPUT_EVENT  && !event.KeyInput.PressedDown) {
			switch(event.KeyInput.Key)
			{
            // Atira
			case KEY_SPACE:
			               printf("Fire!\n");
			               tiro();
			               break;
            // Sai do jogo
			case KEY_ESCAPE:
			               quit = true;
			               break;
            // Grava Screenshot
			case KEY_F9 : case KEY_F12:
				{
                           printf("Screenshot!\n");
                           video::IImage* image = device->getVideoDriver()->createScreenShot();
                           if (image) {
                             device->getVideoDriver()->writeImageToFile(image, getshotname() );
                           }
                           break;
                }

			case KEY_SUBTRACT: case KEY_KEY_Z:
				{
                        nVelocidade -= 0.01f;
                        nWindControl--;
				        if ( nVelocidade < 0.01f ) {
				           nVelocidade = 0.01f;
				           nWindControl=2;
				        }
                        break;
                }

			case KEY_ADD: case KEY_KEY_X:
				{
                        nVelocidade += 0.01f;
                        nWindControl++;
				        if ( nVelocidade > 0.03f ) {
				           nVelocidade = 0.03f;
				           nWindControl=4;
				        }
                        break;
                }


			}
       WindControlSpeed( nWindControl );
       return true;

	}

	return false;
	}
	private:
	  char * getshotname() {
        struct _finddata_t c_file;
	    int n=0;
        char search_file [255];
        long hFile;

           printf(pPath);
        for (;;) {
//          sprintf (search_file, "%s%s%d.jpg", pPath, SEPARATOR,n);
          n +=1;
          sprintf (search_file, "%s%sscreenshot%d.jpg",pPath,SEPARATOR,n);
           printf("%s\n",search_file);
          if ( (hFile = _findfirst(search_file, &c_file)) == -1L ) break;
        }
        return search_file;

  }



};


#endif



