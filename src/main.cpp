#include <irrlicht.h>
#include <irrString.h>
#pragma comment(lib, "Irrlicht.lib")
using namespace irr;
using namespace gui;

#include <irrKlang.h>
using namespace irrklang;


#include "orion.h"
#include "irrBoids.h"
#include <time.h>


//#define DEBUG
//#define DEBUG_POSITIONS



int main(int argc, char *argv[]) {

    pPath = argv[0];
    getcwd(pPath, _MAX_PATH);
    //Cria mapa de teclado
    MyEventReceiver receiver;
	// Configuracao da saida de video
#ifdef DEBUG
    device = createDevice(video::EDT_OPENGL, // Render com openGL
       core::dimension2d<s32>(800, 600),     // resolucao 800X600
       32,                                   // quantidade de cores
       false,                                // em tela cheia
       false,                                // stencilbuffer
       false,                                // vsync
       &receiver);                           // Mapa de teclado
#else
    device = createDevice(video::EDT_OPENGL, // Render com openGL
       core::dimension2d<s32>(800, 600),     // resolucao 800X600
       32,                                   // quantidade de cores
       true,                                // em tela cheia
       false,                                // stencilbuffer
       false,                                // vsync
       &receiver);                           // Mapa de teclado
#endif
    //Em caso de erro sai do programa
	if (device == 0)
	{
		printf("Erro ao iniciar video\n");
		return 1;
	}


    //Inicia audio
	engine = createIrrKlangDevice();
	if (!engine)
	{
		printf("Erro ao iniciar audio\n");
		return 1;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();


    // Desenha logo
    IGUIEnvironment* guienv = device->getGUIEnvironment();

#ifndef DEBUG
    smgr->drawAll();
    gui::IGUIImage * logo = guienv->addImage(driver->getTexture("media/logo.jpg"), core::position2d<s32>(1,1));
    device->getGUIEnvironment()->drawAll();
    driver->endScene();

    // Aguarda 2 segundos e comeca o jogo
    device->sleep(2000,true);
    guienv->clear();
#endif


    //Desabilita cursor
    device->getCursorControl()->setVisible(false);

	//Carrega cenario
	smgr->loadScene("media/universe.irr");

	//Carrega skybox
	smgr->addSkyBoxSceneNode(
		driver->getTexture("media/default_skybox0.jpg"),
		driver->getTexture("media/default_skyboxdn.jpg"),
		driver->getTexture("media/default_skybox1.jpg"),
		driver->getTexture("media/default_skybox2.jpg"),
		driver->getTexture("media/default_skybox3.jpg"),
		driver->getTexture("media/default_skyboxup.jpg")
		);

   // Key map da camera (ignorado)
   SKeyMap keyMap[2];
   keyMap[0].Action=EKA_MOVE_FORWARD;
   keyMap[0].KeyCode=KEY_UP;
   keyMap[1].Action=EKA_MOVE_BACKWARD;
   keyMap[1].KeyCode=KEY_DOWN;


	// Adiciona camera (com velocidade de deslogamento 10)
	scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(
      smgr->getRootSceneNode(),            // node raiz
      10.0f,                               // velocidade rotacao
      0.01f,                                // velocidade movimento
      -1,                                  // ID
      keyMap,                              // Keymap
      2,                                   // Tamanho Keymap
      false,                               // movimento vertical
      0                                    // velocidade de pulo
    );
    camera->setName("PLAYER");


	//tocar musica A_industrya-infinite_war.mp3
	//"A Industrya" http://www.aindustrya.com/
	engine->setListenerPosition(camera->getAbsolutePosition(), camera->getTarget());

#ifndef DEBUG
	engine->play2D("media/A_industrya-infinite_war.ogg", true);
#endif

    // Carrega Cabine
    gui::IGUIImage * cabine = guienv->addImage(driver->getTexture("media/cabine.png"), core::position2d<s32>(1,1));


    // Escreve dados do monitor
    gui::IGUIFont * gamefont = guienv->getFont("media/font.png");

    wchar_t tmp[255];
    char * svel = "X";
    swprintf(tmp, 255, L" Tiros___________%d \n Energia_________%d  \n Escudo__________%d  \n Velocidade______%S ", nTiros, int(nEnergia) , nEscudo, svel );




    gui::IGUIStaticText* monitor = guienv->addStaticText(tmp, core::rect<s32>(280,430,500,580), false, true, 0, -1, true);
    monitor->setDrawBorder(false);
    monitor->setDrawBackground(false);
    monitor->setOverrideFont(gamefont);
    monitor->setOverrideColor(video::SColor(255,0,255,0) ) ;


    core::array<scene::ISceneNode * >  Meshes;
    smgr->getSceneNodesFromType(  scene::ESNT_MESH,  Meshes, 0);

    Esquadrao * MeuEsquadrao = new Esquadrao( Meshes, device );



     // intervalo do som de impacto
     u32 impacto_meteoro = 0;

    int UltimoFPS = 0;
    WindControlStart ( 1 );
    WindControlSpeed( nWindControl );
	// Enquanto estiver rodando e se a janela esta ativa
	while(device->run() && !quit && !gameover )
        if (device->isWindowActive()) 	{


//            device->sleep(1000,true);
            // Prepara cena
            driver->beginScene(true, true, video::SColor(0,10000,10000,10000));
            smgr->drawAll();



            // Desenha dados no monitor
            nEnergia -= nVelocidade / 2;

            svel = "X";
            if (nVelocidade == 0.02f) svel = "XX";
            if (nVelocidade == 0.03f) svel = "XXX";

            swprintf(tmp, 255, L" Tiros___________%d \n Energia_________%d  \n Escudo__________%d  \n Velocidade______%S", nTiros, int(nEnergia) , nEscudo, svel );
            monitor->setText( tmp);


            core::vector3df  pos = camera->getPosition();
            core::vector3df  dest  = pos + (camera->getTarget() - pos).normalize() * nVelocidade;
            camera->setPosition(dest);


            MeuEsquadrao->MovimentaEsquadrao( dest);


            //Forca permanecer no mapa
            //printf( "Z  %d \n", camera->getPosition().Y ) ;
            //if ( pos.Z > MAPA_MAXIMO_Z ) pos.Z = ( MAPA_MAXIMO_Z -10 ) *-1;
            //if ( pos.Z < MAPA_MAXIMO_Z * -1 ) pos.Z = MAPA_MAXIMO_Z + 10;


            //camera->setPosition(dest);
            engine->setListenerPosition(camera->getAbsolutePosition(), camera->getTarget());

            core::line3d<f32> line;
            line.start = pos;
            line.end = dest;

            //Verifica combustivel
            scene::ISceneNode * Node = smgr->getSceneCollisionManager()->getSceneNodeFromRayBB(line, 10,false);
            if (  nEnergia < 1  )  {
                         printf("Sem Combustivel!\n");
                         gameover=true;
            }

            //Verifica colisao
            if ( Node  )  {
              bool bonusitem = false;
              if ( Node->getType() == scene::ESNT_MESH || Node->getType() == scene::ESNT_ANIMATED_MESH ) {
                    printf( "Objeto:  %s \n", Node->getName() ) ;
                    printf("Colisao\n");
                  //gameover=true;
              }

              if ( Node->getType() == scene::ESNT_BILLBOARD ) {
                  if ( Node->getName() != "" ) {
                    printf( "Objeto:  %s \n", Node->getName() ) ;
                  }

                  // Se for asteroid
                  if ( strcmp( Node->getName(), "ASTEROID") == 0  ) {

                         //diminui 1 no escudo por impacto ;
                         nEscudo -= 1;
                         printf("Escudo em %d \n", nEscudo);

                         // faz som de impacto a cada 100 milesimos
                         if (device->getTimer()->getTime() > impacto_meteoro+100 ) {
	                         ISound* sound = engine->play3D("media/metal.ogg", Node->getPosition(), false, false, true);
		                     if (sound) {
			                    sound->setMinDistance(100);
		                        sound->drop();
                             }
                             impacto_meteoro =  device->getTimer()->getTime();
                         }

                  }


                  // Se for tiro
                  if ( strcmp( Node->getName(), "TIRO1") == 0  ) {


                         nEscudo -= 1;
                         printf("Escudo em %d \n", nEscudo);
                         Node->setVisible(false);

                         //faz barulho do tiro
	                     ISound* sound = engine->play3D("media/metal.ogg", Node->getPosition(), false, false, true);
		                 if (sound) {
			                 sound->setMinDistance(100);
		                     sound->drop();
                         }

                  }

                  // Se for bonus de municao
                  if ( strcmp( Node->getName(), "MUNICAO") == 0  ) {
                         Node->setVisible(false);
                         bonusitem=true;
                         nTiros += 10;
                  }

                  // Se for bonus de energia
                  if ( strcmp( Node->getName(), "ENERGIA") == 0  ) {
                         bonusitem=true;
                         nEnergia += 50;
                  }

                  // Se for bonus de escudo
                  if ( strcmp( Node->getName(), "ESCUDO") == 0  ) {
                         bonusitem=true;
                         nEscudo += 30;
                  }


                  // caso seja um item remove o item e toca o som
                  if ( bonusitem ) {
                	  if (engine) {
	                       ISound* sound = engine->play3D("media/beep.ogg", Node->getPosition(), false, false, true);
		                   if (sound) {
			                 sound->setMinDistance(100);
		                     sound->drop();
                           }
                	  }
                      Node->setVisible(false);
	              }
              }

              if ( nEscudo < 1 ) {
                 nEscudo = 0;
                 gameover=true;
              }


              if ( gameover ) {
                  guienv->clear();
                  if (engine) {
                      ISound* sound = engine->play3D("media/explosion.ogg", Node->getPosition(), false, false, true);
                      if (sound) {
                  	     sound->setMinDistance(100);
                  		 sound->drop();
                       }
                  }
                  guienv->addImage(driver->getTexture("media/gameover.png"), core::position2d<s32>(1,1));
              }
            }



            //Desenha GUI (usado para cabine e game over).
            device->getGUIEnvironment()->drawAll();

            //Desenha Cenario
            driver->endScene();


        // coloca no titulo quantos frames por segundo está rodando
	    int fps = driver->getFPS();
		if (UltimoFPS != fps)
		{

			core::stringw str = L"Orion Battle [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			UltimoFPS = fps;
		}


        }



    // aguarda 2 segundos exibindo a tela game over
#ifndef DEBUG
    if ( gameover ) device->sleep(2000,true);
#endif






    //Limpa audio e video da memoria
    device->drop();
    engine->drop();
    WindControlStop();
//    meuuniverso->drop();

	return 0;
}


