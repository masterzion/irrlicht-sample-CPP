
#include <irrlicht.h>
#include "irrBoids.h"


int random( int n) { return rand() % n; }
Esquadrao::~Esquadrao(){}
Inimigo::~Inimigo( ){}

void Inimigo::interpolate( f32 v1 ,  const f32 v2 ,  const f32 d )
{
	if ( v1 <  v2 - d  )
		v1 += d ;
	else if ( v1 < v2 )
		v1 += v2 - v1 ;
	if ( v1 > v2 + d  )
		v1 -= d ;
	else if ( v1 > v2 )
		v1 -= v1 - v2 ;
}


void Inimigo::NovoDestinoAleatorio( )
{
   NovoDestino( core::vector3df(rand() % 2000, rand() % 10 , rand() % 2000));
}

Esquadrao::Esquadrao( core::array<scene::ISceneNode * >  Meshes, IrrlichtDevice * Indevice  )
{
    device =  Indevice;
    u32  tempo  = device->getTimer()->getTime();
    int id = 0;
    // cria lista de inimigos (utilizado para ataque em grupo)
    for( int i = 0; i <= Meshes.size() -1; i++ ) {
//    for( int i = 0; i <= 2 -1; i++ )
        if ( strcmp( Meshes[i]->getName(), "INIMIGO1") == 0  ) {
             id++;
             Inimigos.push_back( Meshes[i]);
             Inimigos.getLast().ID_INIMIGO = id;
             Inimigos.getLast().NovoDestinoAleatorio();
             Inimigos.getLast().smgr = Indevice->getSceneManager();
             Inimigos.getLast().driver = device->getVideoDriver();
             Inimigos.getLast().ultima_rota = tempo;
             Inimigos.getLast().Lider = NULL;
             Inimigos.getLast().AnguloAlvo = 0;
             Inimigos.getLast().AnguloAtual = 0;
             Inimigos.getLast().VelCurva = 0.002f;

             Inimigos.getLast().CriaFumaca( Meshes[i]->getPosition() );


    }
    }
}


/// Fumaça
//   scene::IParticleSystemSceneNode* ps = 0;
void Inimigo::CriaFumaca(core::vector3df b ) {

   particulafumaca = smgr->addParticleSystemSceneNode(false);
   particulafumaca->setPosition(core::vector3df(0,0,0));
   particulafumaca->setScale(core::vector3df(0.005,0.005,0.005));

   particulafumaca->setParticleSize(core::dimension2d<f32>(2.0f, 2.0f));

   emitefumaca = particulafumaca->createBoxEmitter(
      core::aabbox3d<f32>(-3,0,-3,3,1,3),
      core::vector3df(30.0f,0.00f,0.0f),
      15,25,
      video::SColor(0,255,255,255), video::SColor(0,255,255,255),
      400,4100);
   particulafumaca->setMaterialTexture(0, driver->getTexture("media/fumaca.png"));
   particulafumaca->setMaterialFlag(video::EMF_LIGHTING, false);
   particulafumaca->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
   particulafumaca->setEmitter(emitefumaca);
   emitefumaca->drop();
}

Inimigo::Inimigo(scene::ISceneNode * inMeshNode)
{

    // Inicia variaveis
    MeshNode    = inMeshNode;
    ultimo_tiro = 0;
    area_ameaca = 70.0f;
    area_lider  = 25.0f;
    area_minima = 10.0f;
    tipo_acao   = 0;
    velocidade  =  -0.002f;
    velocidade_max  =  -0.007f;
    ultima_rota = 0;
    bemitefumaca = true;
    colisao = false;



#ifdef DEBUG

	scene::ISceneNode* firenode =  sm->addBillboardSceneNode(0, core::dimension2d<f32>(40,40),MeshNode.getPosition() );
	firenode->setMaterialFlag(video::EMF_LIGHTING, false);
	firenode->setMaterialTexture(0, device->getVideoDriver()->getTexture("media/tiro.png"));
	firenode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

#endif
}

double Inimigo::distancia( core::vector3df b )
{
//   return b.getDistanceFrom(MeshNode->getPosition());


   double v1 = ( b.X - MeshNode->getPosition().X );
   double v2 = ( b.Z - MeshNode->getPosition().Z );
   double total = 0;

   v1 = v1 * v1;
   v2 = v2 * v2;

   total = sqrt( v1 + v2 );

   return total;

}



void Inimigo::NovoDestino(core::vector3df  posicao)
{
    // so permite 1 mudanca de rota a quando não estiver proximo
//    if  ( distancia (posicao) > area_minima  )  {
   if (tempo > ultima_rota+2000  ) {
      ultima_rota = tempo;
      destino_nave =  posicao;


      printf("Inimigo %d indo para rota (%f, %f, %f)  !\n", ID_INIMIGO, destino_nave.X , destino_nave.Y, destino_nave.Z );

    } else {
//      printf("Posicao menor %f !\n", distancia (posicao) );
    }

}

void Inimigo::Move( )
{
//*******************************************************
   core::line3d<f32> linhacolisao(MeshNode->getPosition(), destino_nave);

#ifdef DEBUG_POSITIONS
   MeshNode->setDebugDataVisible(scene::EDS_BBOX);
#endif

   core::vector3df direction = destino_nave - linhacolisao.start;
   core::vector3df curva = direction.getHorizontalAngle()*5;
   core::vector3df vel = core::vector3df( velocidade,0,0);



   // Reduz a velocidade para evitar colisao
   if ( colisao ) {
      if ( velocidade < -0.003f ) {
         velocidade -=  -0.003f;
//         printf( " velocidade %f > %f  !\n", velocidade , area_minima );
      }

   } else  {
      if ( velocidade > velocidade_max ) {
         velocidade +=  -0.003f;
      }
   }




   // Vira a nave
//   if (!distancia ( destino_nave ) > area_minima ) {

/*
    MeshNode->setRotation(curva);
    curva = MeshNode->getRotation();


    core::line2df linhaanguloalvo;
    linhaanguloalvo.start.X = linhacolisao.start.X;
    linhaanguloalvo.start.Y = linhacolisao.start.Z;

    linhaanguloalvo.end.X = linhacolisao.end.X;
    linhaanguloalvo.end.Y = linhacolisao.end.Z;

    AnguloAlvo = (f32)linhaanguloalvo.getVector().getAngle() ;


    core::matrix4 m1;
    m1.setRotationDegrees(MeshNode->getRotation());
    m1.transformVect(vel);
    core::vector3df Novaposicao = MeshNode->getPosition() + vel;

    core::line2df linhaanguloatual;
    linhaanguloatual.start = linhaanguloalvo.start;
    linhaanguloatual.end.X = Novaposicao.X;
    linhaanguloatual.end.Y = Novaposicao.Z;


    f32 novoangulo = linhaanguloatual.getAngleWith(linhaanguloalvo) -10 ;
    while ( novoangulo > 360 ) novoangulo -= 360 ;
    while ( novoangulo < 0 ) novoangulo += 360 ;

    curva.X = -90;
    curva.Y = novoangulo;
    curva.Z = 0;
    MeshNode->setRotation( curva );

   core::matrix4 m;
   m.setRotationDegrees(MeshNode->getRotation());
   m.transformVect(vel);
   MeshNode->setPosition(MeshNode->getPosition() + vel);
   MeshNode->updateAbsolutePosition();

    printf( " atual: %f ==== alvo:  %f \n" , AnguloAtual, AnguloAlvo ) ;
*/



   if (distancia ( destino_nave ) > area_minima ) {
       MeshNode->setRotation(curva);
       curva = MeshNode->getRotation();
       curva.X = -90;
        while ( curva.Y > 360 ) curva.Y -= 360 ;
		while ( curva.Y < 0 ) curva.Y += 360 ;

        while ( curva.Z > 360 ) curva.Z -= 360 ;
		while ( curva.Z < 0 ) curva.Z += 360 ;


       MeshNode->setRotation(curva);
   } else {

   }

   // Faz a nave andar seguir em frente

   core::matrix4 m;
   m.setRotationDegrees(MeshNode->getRotation());
   m.transformVect(vel);
   MeshNode->setPosition(MeshNode->getPosition() + vel);
   MeshNode->updateAbsolutePosition();







/// move fumaca
   particulafumaca->setPosition( MeshNode->getPosition() - (vel * 50.0f) );
   core::vector3df fumaca = MeshNode->getRotation() / 50000.0f;
   fumaca.Y=0;
   emitefumaca->setDirection( fumaca );




   // Alinha Altura
   core::vector3df altura = MeshNode->getPosition();
   if ( altura.Y < destino_nave.Y ) altura.Y += 0.005f;
   if ( altura.Y > destino_nave.Y ) altura.Y -= 0.005f;
   MeshNode->setPosition(altura);


    //Pega posicao final
   linhacolisao.end = MeshNode->getPosition();




         // desenha linha ate alvo
         irr::video::SMaterial mat;
         mat.Lighting=false;
         driver->setMaterial(mat);
         driver->setTransform(video::ETS_WORLD, core::matrix4());
#ifdef DEBUG_POSITIONS
        driver->draw3DLine(linhacolisao.start,  destino_nave , video::SColor(255,255,255,255));
#endif

         // desenha linha ate proximo passo
         linhacolisao.start = linhacolisao.start + (MeshNode->getPosition() - linhacolisao.start).normalize() * 36.0f;
         linhacolisao.end = linhacolisao.start + (MeshNode->getPosition() - linhacolisao.start).normalize() * 30.0f;

          scene::ISceneNode * Node =  smgr->getSceneCollisionManager()->getSceneNodeFromRayBB(linhacolisao ) ;


        if (  Node &&
              ( core::stringw( Node->getName() ).size() > 0  ) &&
              ( strcmp( Node->getName(), "TIRO1") != 0  ) &&
              ( strcmp( Node->getName(), "PLAYER") != 0  ) &&
              ( strcmp( Node->getName(), "") != 0  )
            )  {

            if ( !colisao ) {

                  destino_nave = destino_nave * -1.0f;

                   printf( "Colisao com " );
                   printf( Node->getName()   );
                   printf( " !\n" );
                   DesviaObjeto( linhacolisao.start, linhacolisao.end, Node );

            }
            colisao = true;
            // Ponto de curva

#ifdef DEBUG_POSITIONS
          driver->draw3DLine(linhacolisao.start, linhacolisao.end, video::SColor(255,255,0,0));
#endif


        } else {

            if ( colisao ) {
                   printf( "Desviou! \n" );
            }
          colisao = false;
#ifdef DEBUG
            driver->draw3DLine(linhacolisao.start, linhacolisao.end, video::SColor(255,0,0,255));
#endif

        }


}


void Inimigo::DesviaObjeto( core::vector3df Atual, core::vector3df trajeto, scene::ISceneNode * obstaculo )
{
//Atual
//Trajeto

//    NovoDestinoAleatorio();
}


void Inimigo::Atirar()
{

	// Cria o tiro
    if (tempo > ultimo_tiro+1000  ) {

	    core::vector3df inicio = MeshNode->getPosition();

	    scene::ISceneNode* nodetiro =  smgr->addBillboardSceneNode(0, core::dimension2d<f32>(40,40), inicio);

	    nodetiro->setName("TIRO1");

	    nodetiro->setMaterialFlag(video::EMF_LIGHTING, false);
	    nodetiro->setMaterialTexture(0, driver->getTexture("media/tiro_inimigo.png"));
	    nodetiro->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

        // Direcao do tiro
        core::matrix4 m;
        core::vector3df vel = core::vector3df(-90,0,0);
        core::vector3df destino = MeshNode->getPosition();
        m.setRotationDegrees(MeshNode->getRotation());
        m.transformVect(vel);
        destino= destino + vel;
        f32 length = (f32)(destino - inicio).getLength();
	    const f32 speed = 0.4f;

        // cria direcao da particula
	    scene::ISceneNodeAnimator* anim = smgr->createFlyStraightAnimator(inicio, destino, 300);
        nodetiro->addAnimator(anim);
	    anim->drop();


        // Faz tiro desintegrar depois de 0.300 segundo
	    anim = smgr->createDeleteAnimator(300);
	    nodetiro->addAnimator(anim);
	    anim->drop();
	    ultimo_tiro=tempo;
    }


}

bool LiderValido( Inimigo inimigo , Inimigo  nave ) {
    if ( inimigo.Lider == NULL ) {
        return true;
    } else {
        if ( inimigo.Lider->ID_INIMIGO == nave.ID_INIMIGO ) {
           return false;
        } else {
          return  LiderValido( * inimigo.Lider, nave  ) ;
        }

    }


}

void Esquadrao::MovimentaEsquadrao(core::vector3df jogador )
{

  for( int i = 0; i < Inimigos.size(); i++ ) {
//      if (Inimigos[i].MeshNode->isVisible() ) Inimigos[i].Move();

//      if ( Inimigos[i].tipo_acao == NAVE_CRUZEIRO )
//          printf("******** inimigo %d  \n ", Inimigos[i].ID_INIMIGO );
      if (Inimigos[i].MeshNode->isVisible() ) {
        Inimigos[i].tempo = device->getTimer()->getTime();


        switch(Inimigos[i].tipo_acao) {
			case NAVE_FUGA:
			               // se manter em fuga
			               if (random(2000) == 1) {
			                  // tenta despistar mudando a rota periodicamente
			                  Inimigos[i].NovoDestinoAleatorio();
                              break;
			               } else {
			                   // caso contrario tenta fazer uma investida
			                  Inimigos[i].tipo_acao = NAVE_ATAQUE;
                              Inimigos[i].NovoDestino( jogador);
			               }

			case NAVE_ATAQUE:
			               //printf("ataque!\n");
			               // ataca enquanto estiver na area de ameaca
                           if ( Inimigos[i].distancia( jogador  )  <= Inimigos[i].area_ameaca  ) {
			                    Inimigos[i].NovoDestino(jogador);
  			                    Inimigos[i].Atirar();
                              break;
			               } else {
			                  // se jogador fugir, faz cruzeiro
   			                  Inimigos[i].tipo_acao = NAVE_CRUZEIRO;
   			                  Inimigos[i].NovoDestinoAleatorio();
   			                  break;
			               }

			case NAVE_CRUZEIRO:
//			               printf("cruzeiro!\n");

//                           printf("INI%d (%f, %f, %f) -> (%f, %f, %f)!\n",
//                               Inimigos[i].ID_INIMIGO,
//                               Inimigos[i].MeshNode->getPosition().X,
//                               Inimigos[i].MeshNode->getPosition().Y,
//                               Inimigos[i].MeshNode->getPosition().Z,
//                               Inimigos[i].destino_nave.X ,
//                               Inimigos[i].destino_nave.Y,
//                               Inimigos[i].destino_nave.Z );

			               //     se inimigo estiver na area de ameaça, alterna para ataque
			               if ( Inimigos[i].distancia( jogador  )  <= Inimigos[i].area_ameaca  ) {
			                   printf("ataque!\n");
			                  Inimigos[i].tipo_acao = NAVE_ATAQUE;
			                  Inimigos[i].NovoDestino( jogador );
			                  Inimigos[i].ultimo_tiro = Inimigos[i].tempo;
			                  break;
			               }   else  {
			                   //     se aliado estiver na area de ameaça

			                   for( int n = 0; n < Inimigos.size(); n++  ) {
			                       //se nao for ele mesmo ele nao for o lider


			                        if (
                                       ( Inimigos[i].Lider == NULL ) &&
                                       ( Inimigos[i].tipo_acao != NAVE_SEGUIR_LIDER ) &&
			                           ( Inimigos[i].ID_INIMIGO != Inimigos[n].ID_INIMIGO ) &&
//			                           ( Inimigos[i].Lider->ID_INIMIGO != Inimigos[n].ID_INIMIGO ) //&&
			                           ( Inimigos[i].distancia( Inimigos[n].MeshNode->getPosition()  )  <= Inimigos[i].area_lider ) &&
			                           ( Inimigos[n].MeshNode->isVisible()  ) &&
			                           ( LiderValido(  Inimigos[n]  ,  Inimigos[i] ) )
			                           ) {
			                           Inimigos[i].tipo_acao = NAVE_SEGUIR_LIDER;
			                           Inimigos[i].Lider =  &Inimigos[n];
			                           Inimigos[i].NovoDestino ( Inimigos[i].Lider->MeshNode->getPosition() );

			                           printf("%d seguindo %d \n", Inimigos[i].ID_INIMIGO, Inimigos[n].ID_INIMIGO );
			                           break;
			                       }


			                   }


			                   // continua cruzeiro! se alcancou destino, vai para outro ponto
			                   if ( Inimigos[i].distancia( Inimigos[i].destino_nave  )  <= Inimigos[i].area_minima )   {
			                       printf("area minima!\n");
			                       Inimigos[i].NovoDestinoAleatorio();
			                   } else {
			                        // se nao alcancou, sorteia a possibilidade de mudar de rota
			                        if (random(20000) == 1) Inimigos[i].NovoDestinoAleatorio();
			                   }
			               }
			               break;

			case NAVE_SEGUIR_LIDER:

                            if ( Inimigos[i].Lider->MeshNode->isVisible() ) {
                               // se estiver longe
                               if ( Inimigos[i].distancia( Inimigos[i].MeshNode->getPosition()  )  > Inimigos[i].area_ameaca +20  ) {

#ifdef DEBUG_POSITIONS
                                  printf("%d abandonando %d \n", Inimigos[i].ID_INIMIGO, Inimigos[i].Lider->ID_INIMIGO );
#endif

                                  Inimigos[i].tipo_acao = NAVE_CRUZEIRO;
                                  Inimigos[i].NovoDestinoAleatorio();
                               } else {

                                   if ( Inimigos[i].Lider->tipo_acao == NAVE_ATAQUE )  {
                                         Inimigos[i].Atirar();
                                         Inimigos[i].NovoDestino(jogador);
                                   } else {
                                         Inimigos[i].NovoDestino(Inimigos[i].Lider->MeshNode->getPosition());
                                   }
                               }
                            }  else {
                                // se lider destruido, inicia fuga
                                Inimigos[i].tipo_acao = NAVE_FUGA;
                                Inimigos[i].Lider = NULL;

#ifdef DEBUG_POSITIONS
			                    printf("fuga!\n");
#endif


                                Inimigos[i].NovoDestinoAleatorio();
                            }
			               if ( Inimigos[i].distancia( jogador  )  <= Inimigos[i].area_ameaca  ) {
			                   printf("ataque!\n");
			                  Inimigos[i].tipo_acao = NAVE_ATAQUE;
			                  Inimigos[i].NovoDestino( jogador );
			                  Inimigos[i].ultimo_tiro = Inimigos[i].tempo;
			               }
                            break;

        }
        Inimigos[i].Move( );
      } else {
        //Faz da fumaça desaparecerem
        if ( Inimigos[i].bemitefumaca ) {
          scene::ISceneNodeAnimator* anim = Inimigos[i].smgr->createDeleteAnimator(500);
	      Inimigos[i].particulafumaca->addAnimator(anim);
	      anim->drop();
	      printf("removeu !\n");
	      Inimigos[i].bemitefumaca = false;
        }

      }
   }

}









