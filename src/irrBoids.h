#include <irrlicht.h>
using namespace irr;

//#define DEBUG
//#define DEBUG_POSITIONS



#ifndef IRRBOID__H
#define IRRBOID__H



const int NAVE_CRUZEIRO = 0; // existir alvo
                             //     se inimigo estiver na area de ameaça, alterna para ataque
                             //     se aliado estiver na area de ameaça e Alvo aliado diferente da nave, segue aliado
                             // se nao
                             //   se alcancou o ponto de cruzeiro, random para outro ponto

const int NAVE_FUGA = 1;    // fara um random(100) = 1 para alternar para ataque

const int NAVE_ATAQUE = 2;  // ataca enquanto estiver na area de ameaca

const int NAVE_SEGUIR_LIDER = 3; // se lider destruido alterna para fuga
                                 //     se aliado atacando, ataca





//********************************************************************
struct Point
{
	int x,y,cost,heuristic,parent_x,parent_y;
};




class Inimigo
{
public:

/// Usado pelo jogo
    scene::ISceneNode    * MeshNode;          // Mesh da nave
    scene::ISceneNode    * nodetiro;
    scene::ISceneManager * smgr;
    video::IVideoDriver  * driver ;

/// usado na fumaça
    scene::IParticleSystemSceneNode* particulafumaca;
    scene::IParticleEmitter* emitefumaca;
    bool bemitefumaca;

    bool    colisao;

    int     ID_INIMIGO;
    int     tipo_acao;
    float   velocidade;
    float   velocidade_max;

    double area_ameaca;
    double area_lider;
    double area_minima;

    u32    ultimo_tiro;
    u32    ultima_rota;
    u32    tempo;


   // utilizado para traçar acao de destino
    core::vector3df destino_nave;
    f32 AnguloAlvo;
    f32 AnguloAtual;
    f32 VelCurva;


    Inimigo * Lider; // para o lider
    float distancia_minima; // distancia segura para evitar colisao
	Inimigo(scene::ISceneNode * inMeshNode );
	~Inimigo();
	void Move();
	void Atirar();
    void NovoDestinoAleatorio();
    void NovoDestino(core::vector3df  posicao);
    void CriaFumaca(core::vector3df b );

    void interpolate( f32 v1 ,  const f32 v2 ,  const f32 d );

    double distancia( core::vector3df b  );
    //DesviaObjeto( linhacolisao.start, linhacolisao.end, Node );
    void DesviaObjeto( core::vector3df Atual, core::vector3df trajeto, scene::ISceneNode * obstaculo );





};




//********************************************************************
class Esquadrao
{
public:
    Esquadrao::Esquadrao( core::array<scene::ISceneNode * >  Meshes, IrrlichtDevice *device  );

    int NUM_BOIDS;
    int MAPA_MAXIMO_X;
    int MAPA_MAXIMO_Y;
    int MAPA_MAXIMO_Z;
    IrrlichtDevice * device;


	~Esquadrao();

    void MovimentaEsquadrao(core::vector3df jogador );
	int CalculateInverseTan( double delta_x, double delta_z );

    core::array<Inimigo>  Inimigos; // lista de inimigos ***** mudar para array
};

#endif

