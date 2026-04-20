/*******************************************************************************************
*
*   project_name
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#define pi 3.141592653589793
#include "raylib.h"

#include "rlgl.h"
#include "raymath.h"      // Required for: MatrixPerspective(), MatrixLookAt()
#define sign(x) (((x) > 0) - ((x) < 0))

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#define M_TORAD pi/180
#define M_TODEG 180/pi

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    //#define GLSL_VERSION            100
    #define GLSL_VERSION            300
#endif


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

typedef struct {
    bool isbeebo;
    const char* text;
} Dialog;

const Dialog talks[] = {
    #include "dialogs.txt"
};

typedef struct {
    float x;
    float y;
    float z;
    float s;
    int model;
    int tex;
    Model mdl;
    bool gen;
    bool glow;
    bool plain;
    bool nowall;
    float rx;
    float ry;
    float rz;
    float sx;
    float sy;
    float sz;
    bool nocol;
    bool keepload;
    bool hide;
    Matrix mat;
} Terrain;
typedef struct {
    Terrain items[256];
    int count;
} gm3dlist;
typedef struct {
    RayCollision results[27];
    int8_t lowind;
    Vector3 v3used;
} scuffedrays;

#define BTYPE_CUBE 0
#define BTYPE_BALL 1
#define BTYPE_MESH 2

#define OTYPE_NONE 0
#define OTYPE_TELE 1
    #define V_TELE_TOMAP 0
    #define V_TELE_TOX 1
    #define V_TELE_TOY 2
    #define V_TELE_TOZ 3
#define OTYPE_CAND 2
#define OTYPE_CORK 3
    #define V_CORK_TERRAI 0
#define OTYPE_TALK 4
    #define V_TALK_MODEL 0
    #define V_TALK_SIZ 1
#define OTYPE_ICED 5
#define OTYPE_WATR 6
#define OTYPE_POLE 7
typedef struct {
    Vector3 pos;
    Vector3 size;
    int type; //type id of thing
    Model mdl; //model for rendering
    unsigned short uid;
    bool disabled;
} Entity;
typedef struct {
    Entity items[256];
    int count;
} EntTL;
typedef struct {
    float vals[2048]; //The values (obviously) (only numbers cuz im dumb)
    unsigned short vids[2048]; //ID of variable, will use definitions for variable names
    unsigned short vuid[2048]; //The owner of the variable's unique ID
    int count;
} EntVL;
#define PAR_MAX 64
typedef struct {
    uint8_t type;
    Vector3 speed;
    Vector3 rs;
    Vector3 pos;
    Matrix rot;
    float scale;
} Par; //particle
typedef struct {
    Par items[PAR_MAX];
    int next;
} ParL;

typedef struct { //struct used for when a variable of an entity is found
    float val;
    unsigned short ind; //index of the variable for changing it
} FoundVar;

const bool rg3[]={ //if can be activated by player alternate hitbox (usually spinattacks)
    false,//OTYPE_NONE
    false,//OTYPE_TELE
    true ,//OTYPE_CAND
    true ,//OTYPE_CORK
    false,//OTYPE_TALK
    false,//OTYPE_ICED
    true, //OTYPE_WATR
    false //OTYPE_POLE
};
const bool gr3[]={ //if can be activated by beebo standing on it
    false,//OTYPE_NONE
    false,//OTYPE_TELE
    false,//OTYPE_CAND
    true ,//OTYPE_CORK
    false,//OTYPE_TALK
    false,//OTYPE_ICED
    false,//OTYPE_WATR
    false //OTYPE_POLE
};
//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------

//embedded files

//sfx
const unsigned char sfx_loadto[]={
#embed "sfx/loadmapout.ogg"
};
const unsigned char sfx_loadfr[]={
#embed "sfx/loadmapin.ogg"
};
const unsigned char sfx_jump[]={
#embed "sfx/jump.ogg"
};
const unsigned char sfx_djump[]={
#embed "sfx/djump.ogg"
};
const unsigned char sfx_dive[]={
#embed "sfx/dive.ogg"
};
const unsigned char sfx_jump2[]={
#embed "sfx/misc-jump.ogg"
};
const unsigned char sfx_land[]={
#embed "sfx/land.ogg"
};
const unsigned char sfx_slide[]={
#embed "sfx/slide.ogg"
};
const unsigned char sfx_smack[]={
#embed "sfx/smack.ogg"
};
const unsigned char sfx_spin[]={
#embed "sfx/spin.ogg"
};
const unsigned char sfx_flip[]={
#embed "sfx/flip.ogg"
};
const unsigned char sfx_candy[]={
#embed "sfx/candy.ogg"
};
const unsigned char sfx_break[]={
#embed "sfx/break-box.ogg"
};
const unsigned char sfx_gotice[]={
#embed "sfx/get-ice-cream.ogg"
};
const unsigned char sfx_pole[]={
#embed "sfx/pole.ogg"
};

const unsigned char sfx_sa1[]={ //step A 1
#embed "sfx/sa1.ogg"
};
const unsigned char sfx_sa2[]={ //step A 2
#embed "sfx/sa2.ogg"
};
const unsigned char sfx_sa3[]={ //step A 3
#embed "sfx/sa3.ogg"
};
const unsigned char sfx_sa4[]={ //step A 4
#embed "sfx/sa4.ogg"
};
const unsigned char sfx_sa5[]={ //step A 5
#embed "sfx/sa5.ogg"
};

//music
const unsigned char hub_bgm[]={
#embed "music/hub world.ogg"
};
const unsigned char hub_bgmA[]={
#embed "music/hub world (air).ogg"
};
const unsigned char hub_bgmW[]={
#embed "music/hub-world (water).ogg"
};
const unsigned char hub_bgmP[]={
#embed "music/hub world (paused).ogg"
};
const unsigned char title_bgm[]={
#embed "music/title.ogg"
};
const unsigned char tutorial_bgm[]={
#embed "music/enjoy your stay.ogg"
};
const unsigned char tutorial_bgmP[]={
#embed "music/enjoy your stay (paused).ogg"
};
const unsigned char turtle_bgm[]={
#embed "music/snippy dippy.ogg"
};
const unsigned char turtle_bgmA[]={
#embed "music/snippy dippy (air).ogg"
};
const unsigned char turtle_bgmW[]={
#embed "music/snippy dippy (water).ogg"
};
const unsigned char turtle_bgmP[]={
#embed "music/snippy dippy paused.ogg"
};
//textures
const unsigned char tex_padding[]={
#embed "textures/padding.png"
};
const unsigned char tex_grass[]={ //texture 0
#embed "textures/grass.png"
};
const unsigned char tex_cliff[]={ //texture 1
#embed "textures/cliff.png"
};
const unsigned char tex_logo[]={ //texture 2
#embed "textures/logocolors.png"
};
const unsigned char tex_path[]={ //texture 3
#embed "textures/path.png"
};
const unsigned char tex_sand[]={ //texture 4
#embed "textures/sand.png"
};
const unsigned char img_spinny[]={
#embed "textures/spinny.png"
};
const unsigned char img_trs[]={
#embed "textures/trs.png"
};
const unsigned char img_cur[]={
#embed "textures/cursor.png"
};
const unsigned char img_inv[]={
#embed "textures/invui.png"
};
const unsigned char tex_sky_default[]={
#embed "skybox/default.png"
};
const unsigned char tex_sky_title[]={
#embed "skybox/title.png"
};
const unsigned char tex_skin1[]={
#embed "textures/beebo/1.png"
};
const unsigned char tex_face1[]={
#embed "textures/beebo/face1.png"
};
const unsigned char tex_face2[]={
#embed "textures/beebo/face2.png"
};
const unsigned char tex_face3[]={
#embed "textures/beebo/face3.png"
};
const unsigned char tex_face4[]={
#embed "textures/beebo/face4.png"
};
const unsigned char tex_face5[]={
#embed "textures/beebo/face5.png"
};
const unsigned char tex_candt[]={
#embed "textures/candy.png"
};
//const unsigned char tex_plain[]={
//#embed "textures/plain.png"
//};
const unsigned char tex_sparkle[]={
#embed "textures/sparkle.png"
};
const unsigned char img_startup[]={
#embed "textures/startup.png"
};
const unsigned char tex_hubout[]={ //texture 5
#embed "textures/hubout.png"
};
const unsigned char tex_hubcloudfloor[]={ //texture 6
#embed "textures/hubcloudfloor.png"
};
const unsigned char tex_hubwood[]={ //texture 7
#embed "textures/hubwood.png"
};
const unsigned char tex_hubshop[]={ //texture 8
#embed "textures/hubshop.png"
};
const unsigned char tex_hubrusty[]={ //texture 9
#embed "textures/hubrusty.png"
};
const unsigned char tex_hubokwall[]={ //texture 10
#embed "textures/hubokwall.png"
};
const unsigned char tex_cork[]={
#embed "textures/cork.png"
};
const unsigned char tex_hubgoodfloor[]={ //texture 11
#embed "textures/hubgoodfloor.png"
};
const unsigned char tex_hubstairs[]={ //texture 12
#embed "textures/hubstairs.png"
};
const unsigned char tex_hubcarpet[]={ //texture 13
#embed "textures/hubcarpet.png"
};
const unsigned char tex_hubbookwall[]={ //texture 14
#embed "textures/hubbookwall.png"
};
const unsigned char tex_hubstarryfloor[]={ //texture 15
#embed "textures/hubstarryfloor.png"
};
const unsigned char tex_hubstars[]={ //texture 16
#embed "textures/hubstars.png"
};
const unsigned char img_icedicon[]={
#embed "textures/icedicon.png"
};
const unsigned char img_candicon[]={
#embed "textures/candicon.png"
};
const unsigned char tex_turtlegrass[]={ //texture 17
#embed "textures/turtlegrass.png"
};
const unsigned char tex_wall[]={ //texture 18
#embed "textures/wall.png"
};
const unsigned char tex_turtlebody[]={ //texture 19
#embed "textures/turtlebody.png"
};
const unsigned char tex_icedcream[]={
#embed "textures/icedcream.png"
};
const unsigned char tex_water[]={
#embed "textures/water.png"
};
const unsigned char tex_sun[]={
#embed "textures/sun.png"
};
const unsigned char tex_sky_hub[]={
#embed "skybox/hub.png"
};
const unsigned char img_frame[]={
#embed "textures/frame.png"
};
const unsigned char tex_title[]={ //texture 20
#embed "textures/title.png"
};
const unsigned char tex_jetpack[]={
#embed "textures/beebo/jetpack.png"
};
const unsigned char img_3dUI[]={
#embed "textures/3dUI.png"
};
const unsigned char img_stick2[]={
#embed "textures/stick2.png"
};
const unsigned char tex_sky_turtle[]={
#embed "skybox/turtle.png"
};
#define NUM_TEX 21
typedef struct {
    Texture2D items[NUM_TEX];
    bool loaded[NUM_TEX];
} LvlTexList;
LvlTexList levtexs = {0}; //idk when im gonna use this (probably not cuz json maps are in progress)
const unsigned char *texlist[] = {tex_grass,
                                tex_cliff,
                                tex_logo,
                                tex_path,
                                tex_sand,
                                tex_hubout,
                                tex_hubcloudfloor,
                                tex_hubwood,
                                tex_hubshop,
                                tex_hubrusty,
                                tex_hubokwall,
                                tex_hubgoodfloor,
                                tex_hubstairs,
                                tex_hubcarpet,
                                tex_hubbookwall,
                                tex_hubstarryfloor,
                                tex_hubstars,
                                tex_turtlegrass,
                                tex_wall,
                                tex_turtlebody,
                                tex_title
                                };
const int texsize[] = {sizeof(tex_grass),
                    sizeof(tex_cliff),
                    sizeof(tex_logo),
                    sizeof(tex_path),
                    sizeof(tex_sand),
                    sizeof(tex_hubout),
                    sizeof(tex_hubcloudfloor),
                    sizeof(tex_hubwood),
                    sizeof(tex_hubshop),
                    sizeof(tex_hubrusty),
                    sizeof(tex_hubokwall),
                    sizeof(tex_hubgoodfloor),
                    sizeof(tex_hubstairs),
                    sizeof(tex_hubcarpet),
                    sizeof(tex_hubbookwall),
                    sizeof(tex_hubstarryfloor),
                    sizeof(tex_hubstars),
                    sizeof(tex_turtlegrass),
                    sizeof(tex_wall),
                    sizeof(tex_turtlebody),
                    sizeof(tex_title)
                    };
const unsigned char *skinlist[] = {tex_skin1};
const int skinsize[] = {sizeof(tex_skin1)};
//models (must be glb)
const unsigned char hub_meshgrass[]={ //model 0
#embed "models/hub/grass.glb"
};
const unsigned char hub_meshcliff[]={ //model 1
#embed "models/hub/cliff.glb"
};
const unsigned char title_meshlogo[]={ //model 2
#embed "models/title/logo.glb"
};
const unsigned char hub_meshpath[]={ //model 3
#embed "models/hub/path.glb"
};
const unsigned char hub_meshsand[]={ //model 4
#embed "models/hub/sand.glb"
};
const unsigned char beeb_torso[]={ //model 5 (not terrain)
#embed "models/beebo/torso.glb"
};
const unsigned char beeb_head[]={ //model 6 (not terrain)
#embed "models/beebo/head.glb"
};
const unsigned char beeb_face[]={ //model 7 (not terrain)
#embed "models/beebo/face.glb"
};
const unsigned char beeb_leg[]={ //model 8 (not terrain)
#embed "models/beebo/leg.glb"
};
const unsigned char beeb_tarm[]={ //model 9 (not terrain)
#embed "models/beebo/toparm.glb"
};
const unsigned char beeb_barm[]={ //model 10 (not terrain)
#embed "models/beebo/botarm.glb"
};
const unsigned char beeb_dot[]={ //model 11 (not terrain)
#embed "models/beebo/dot.glb"
};
const unsigned char misc_candy[]={ //model 12 (not terrain)
#embed "models/candy.glb"
};
const unsigned char hub_meshhubout[]={ //model 13
#embed "models/hub/hubout.glb"
};
const unsigned char hub_meshhubbottom[]={ //model 14
#embed "models/hub/hubbottom.glb"
};
const unsigned char hub_meshhubcloudfloor[]={ //model 15
#embed "models/hub/hubcloudfloor.glb"
};
const unsigned char hub_meshhubwood[]={ //model 16
#embed "models/hub/hubwood.glb"
};
const unsigned char hub_meshhubshop[]={ //model 17
#embed "models/hub/hubshop.glb"
};
const unsigned char hub_meshhubrusty[]={ //model 18
#embed "models/hub/hubrusty.glb"
};
const unsigned char hub_meshhubokwall[]={ //model 19
#embed "models/hub/hubokwall.glb"
};
const unsigned char hub_meshhubgoodfloor[]={ //model 20
#embed "models/hub/hubgoodfloor.glb"
};
const unsigned char hub_meshhubstairs[]={ //model 21
#embed "models/hub/hubstairs.glb"
};
const unsigned char hub_meshhubcarpet[]={ //model 22
#embed "models/hub/hubcarpet.glb"
};
const unsigned char hub_meshhubbookwall[]={ //model 23
#embed "models/hub/hubbookwall.glb"
};
const unsigned char hub_meshhubstarryfloor[]={ //model 24
#embed "models/hub/hubstarryfloor.glb"
};
const unsigned char hub_meshhubstars[]={ //model 25
#embed "models/hub/hubstars.glb"
};
const unsigned char turtle_meshgrass[]={ //model 26
#embed "models/turtle/grass.glb"
};
const unsigned char turtle_meshwall[]={ //model 27
#embed "models/turtle/wall.glb"
};
const unsigned char turtle_meshpath[]={ //model 28
#embed "models/turtle/path.glb"
};
const unsigned char turtle_meshsand[]={ //model 29
#embed "models/turtle/sand.glb"
};
const unsigned char turtle_meshturtlebody[]={ //model 30
#embed "models/turtle/turtlebody.glb"
};
const unsigned char misc_cloud[]={ //model 31 (not terrain)
#embed "models/cloud.glb"
};
const unsigned char misc_icedcream[]={ //model 32 (not terrain)
#embed "models/icedcream.glb"
};
const unsigned char beeb_hat1[]={ //model 33 (not terrain)
#embed "models/beebo/hats/hat1.glb"
};
const unsigned char title_meshbeam[]={ //model 34
#embed "models/title/beam.glb"
};
const unsigned char beeb_jetpack[]={ //model 35 (not terrain)
#embed "models/beebo/jetpack.glb"
};
const unsigned char beeb_booster[]={ //model 36 (not terrain)
#embed "models/beebo/booster.glb"
};
const unsigned char beeb_pack[]={ //model 37 (not terrain)
#embed "models/beebo/pack.glb"
};
const unsigned char *glblist[] = {hub_meshgrass,
                                hub_meshcliff,
                                title_meshlogo,
                                hub_meshpath,
                                hub_meshsand,
                                beeb_torso,
                                beeb_head,
                                beeb_face,
                                beeb_leg,
                                beeb_tarm,
                                beeb_barm,
                                beeb_dot,
                                misc_candy,
                                hub_meshhubout,
                                hub_meshhubbottom,
                                hub_meshhubcloudfloor,
                                hub_meshhubwood,
                                hub_meshhubshop,
                                hub_meshhubrusty,
                                hub_meshhubokwall,
                                hub_meshhubgoodfloor,
                                hub_meshhubstairs,
                                hub_meshhubcarpet,
                                hub_meshhubbookwall,
                                hub_meshhubstarryfloor,
                                hub_meshhubstars,
                                turtle_meshgrass,
                                turtle_meshwall,
                                turtle_meshpath,
                                turtle_meshsand,
                                turtle_meshturtlebody,
                                misc_cloud,
                                misc_icedcream,
                                beeb_hat1,
                                title_meshbeam,
                                beeb_jetpack,
                                beeb_booster,
                                beeb_pack
                                };
const int glbsize[] = {sizeof(hub_meshgrass),
                    sizeof(hub_meshcliff),
                    sizeof(title_meshlogo),
                    sizeof(hub_meshpath),
                    sizeof(hub_meshsand),
                    sizeof(beeb_torso),
                    sizeof(beeb_head),
                    sizeof(beeb_face),
                    sizeof(beeb_leg),
                    sizeof(beeb_tarm),
                    sizeof(beeb_barm),
                    sizeof(beeb_dot),
                    sizeof(misc_candy),
                    sizeof(hub_meshhubout),
                    sizeof(hub_meshhubbottom),
                    sizeof(hub_meshhubcloudfloor),
                    sizeof(hub_meshhubwood),
                    sizeof(hub_meshhubshop),
                    sizeof(hub_meshhubrusty),
                    sizeof(hub_meshhubokwall),
                    sizeof(hub_meshhubgoodfloor),
                    sizeof(hub_meshhubstairs),
                    sizeof(hub_meshhubcarpet),
                    sizeof(hub_meshhubbookwall),
                    sizeof(hub_meshhubstarryfloor),
                    sizeof(hub_meshhubstars),
                    sizeof(turtle_meshgrass),
                    sizeof(turtle_meshwall),
                    sizeof(turtle_meshpath),
                    sizeof(turtle_meshsand),
                    sizeof(turtle_meshturtlebody),
                    sizeof(misc_cloud),
                    sizeof(misc_icedcream),
                    sizeof(beeb_hat1),
                    sizeof(title_meshbeam),
                    sizeof(beeb_jetpack),
                    sizeof(beeb_booster),
                    sizeof(beeb_pack)
                    };

const unsigned char *filepointer;
int filepointersize;
unsigned char* readmodel(const char* fileName, int* dataSize){
	*dataSize = filepointersize;
	unsigned char* mem = MemAlloc(filepointersize);
	memcpy(mem, filepointer, *dataSize);
	return mem;
}
void prepmodel(const unsigned char* model, int sz){
	filepointer = model;
	filepointersize = sz;
	SetLoadFileDataCallback(readmodel);
}
//shaders
Shader shader;
int SHldl;
int SHam1l;
int SHam2l;
int SHam3l;
int SHbrl;
//images
Texture2D spinny;
Texture2D trstex;
Texture2D curstx;
Texture2D invtex;
Texture2D ml_candy_tex;
Texture2D t_sparkle;
Texture2D ml_icedcream_tex;
//Texture2D t_plain;
Texture2D t_cork;
Texture2D icedicon;
Texture2D candicon;
Texture2D t_water;
Texture2D t_sun;
Texture2D t_frame;
Texture2D t_3dUI;
Texture2D t_stick2;
//sounds
float soundRolloffGlobal = 0.05f; //rolloff for sound attenuation, higher is quieter

Sound s_load1;
Sound s_load2;
Sound s_jump;
Sound s_djump;
Sound s_dive;
Sound s_jump2;
Sound s_land;
Music s_slide;
Sound s_smack;
Sound s_spin;
Sound s_flip;
Sound s_candy;
Sound s_break;
Sound s_gotice;
Sound s_pole;

Sound stepsA[5];
//models
Model b_torso;
Model b_head;
Model b_face;
Model b_leg;
Model b_tarm;
Model b_barm;
Model b_dot;
Model b_jetpack;
Model b_booster;
Model b_pack;

Model p_break; //for breakables
Model p_sun;

Model curhat;

Model ml_candy;
Model ml_cloud;
Model ml_icedcream;
//textures
Texture2D curskin;
Texture2D bt_faces[5];
Texture2D t_jetpack;

//maphandling
gm3dlist gm3d;
EntTL entlist;
EntVL entlistV;
ParL particles = {0};

Mesh GenMeshCubeT(float width,float height,float length,float studs){ //copied from raylib because idk how todo this, only to modify it
    Mesh mesh = {0};
    float vertices[] = {
        -width/2, -height/2, length/2,
        width/2, -height/2, length/2,
        width/2, height/2, length/2,
        -width/2, height/2, length/2,
        -width/2, -height/2, -length/2,
        -width/2, height/2, -length/2,
        width/2, height/2, -length/2,
        width/2, -height/2, -length/2,
        -width/2, height/2, -length/2,
        -width/2, height/2, length/2,
        width/2, height/2, length/2,
        width/2, height/2, -length/2,
        -width/2, -height/2, -length/2,
        width/2, -height/2, -length/2,
        width/2, -height/2, length/2,
        -width/2, -height/2, length/2,
        width/2, -height/2, -length/2,
        width/2, height/2, -length/2,
        width/2, height/2, length/2,
        width/2, -height/2, length/2,
        -width/2, -height/2, -length/2,
        -width/2, -height/2, length/2,
        -width/2, height/2, length/2,
        -width/2, height/2, -length/2
    };
    float texcoords[] = {
        0.0f, 0.0f,
        width/studs, 0.0f,
        width/studs, height/studs,
        0.0f, height/studs,
        width/studs, 0.0f,
        width/studs, height/studs,
        0.0f, height/studs,
        0.0f, 0.0f,
        0.0f, length/studs,
        0.0f, 0.0f,
        width/studs, 0.0f,
        width/studs, length/studs,
        width/studs, length/studs,
        0.0f, length/studs,
        0.0f, 0.0f,
        width/studs, 0.0f,
        length/studs, 0.0f,
        length/studs, height/studs,
        0.0f, height/studs,
        0.0f, 0.0f,
        0.0f, 0.0f,
        length/studs, 0.0f,
        length/studs, height/studs,
        0.0f, height/studs
    };
    float normals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };

    mesh.vertices = (float *)RL_MALLOC(24*3*sizeof(float));
    memcpy(mesh.vertices, vertices, 24*3*sizeof(float));

    mesh.texcoords = (float *)RL_MALLOC(24*2*sizeof(float));
    memcpy(mesh.texcoords, texcoords, 24*2*sizeof(float));

    mesh.normals = (float *)RL_MALLOC(24*3*sizeof(float));
    memcpy(mesh.normals, normals, 24*3*sizeof(float));

    mesh.indices = (unsigned short *)RL_MALLOC(36*sizeof(unsigned short));

    int k = 0;

    for (int i=0; i<36; i+=6)
    {
        mesh.indices[i] = 4*k;
        mesh.indices[i + 1] = 4*k + 1;
        mesh.indices[i + 2] = 4*k + 2;
        mesh.indices[i + 3] = 4*k;
        mesh.indices[i + 4] = 4*k + 2;
        mesh.indices[i + 5] = 4*k + 3;
        k++;
    }

    mesh.vertexCount = 24;
    mesh.triangleCount = 12;
    UploadMesh(&mesh, false);
    return mesh;
}
Mesh GenMeshPlaneT(float width, float length, int resX, int resZ, float studsx, float studsy){ //copied from raylib because idk how todo this, only to modify it
    Mesh mesh = { 0 };

    resX++;
    resZ++;

    int vertexCount = resX*resZ;

    Vector3 *vertices = (Vector3 *)RL_MALLOC(vertexCount*sizeof(Vector3));
    for (int z = 0; z < resZ; z++)
    {
        float zPos = ((float)z/(resZ - 1) - 0.5f)*length;
        for (int x = 0; x < resX; x++)
        {
            float xPos = ((float)x/(resX - 1) - 0.5f)*width;
            vertices[x + z*resX] = (Vector3){ -xPos, 0.0f, -zPos };
        }
    }

    Vector3 *normals = (Vector3 *)RL_MALLOC(vertexCount*sizeof(Vector3));
    for (int n = 0; n < vertexCount; n++) normals[n] = (Vector3){ 0.0f, 1.0f, 0.0f };

    Vector2 *texcoords = (Vector2 *)RL_MALLOC(vertexCount*sizeof(Vector2));
    
    float crap1 = width/studsx;float crap2 = length/studsy;
    for (int v = 0; v < resZ; v++)
    {
        for (int u = 0; u < resX; u++)
        {
            texcoords[u + v*resX] = (Vector2){ (float)(u*crap1)/(resX - 1), -(float)(v*crap2)/(resZ - 1) };
        }
    }

    int numFaces = (resX - 1)*(resZ - 1);
    int *triangles = (int *)RL_MALLOC(numFaces*6*sizeof(int));
    int t = 0;
    for (int face = 0; face < numFaces; face++)
    {
        int i = face + face/(resX - 1);

        triangles[t++] = i + resX;
        triangles[t++] = i + 1;
        triangles[t++] = i;

        triangles[t++] = i + resX;
        triangles[t++] = i + resX + 1;
        triangles[t++] = i + 1;
    }

    mesh.vertexCount = vertexCount;
    mesh.triangleCount = numFaces*2;
    mesh.vertices = (float *)RL_MALLOC(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)RL_MALLOC(mesh.vertexCount*2*sizeof(float));
    mesh.normals = (float *)RL_MALLOC(mesh.vertexCount*3*sizeof(float));
    mesh.indices = (unsigned short *)RL_MALLOC(mesh.triangleCount*3*sizeof(unsigned short));

    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.vertices[3*i] = vertices[i].x;
        mesh.vertices[3*i + 1] = vertices[i].y;
        mesh.vertices[3*i + 2] = vertices[i].z;
    }

    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.texcoords[2*i] = texcoords[i].x;
        mesh.texcoords[2*i + 1] = texcoords[i].y;
    }

    for (int i = 0; i < mesh.vertexCount; i++)
    {
        mesh.normals[3*i] = normals[i].x;
        mesh.normals[3*i + 1] = normals[i].y;
        mesh.normals[3*i + 2] = normals[i].z;
    }

    for (int i = 0; i < mesh.triangleCount*3; i++) mesh.indices[i] = triangles[i];

    RL_FREE(vertices);
    RL_FREE(normals);
    RL_FREE(texcoords);
    RL_FREE(triangles);

    UploadMesh(&mesh, false);

    return mesh;
}
unsigned short nextuid = 0;
Entity crEnt(unsigned short t,float x,float y,float z,float sx,float sy,float sz){
    Entity e = {0};
    e.pos = (Vector3){x,y,z};
    e.size = (Vector3){sx,sy,sz};
    e.type = t;
    e.uid = nextuid;
    nextuid++;
    return e;
}
void addvar(unsigned short uid,unsigned short nam,float val){
    entlistV.vuid[entlistV.count]=uid;
    entlistV.vids[entlistV.count]=nam;
    entlistV.vals[entlistV.count]=val;
    entlistV.count++;
}
float findvar(unsigned short uid,unsigned short nam){
    if(entlistV.count>0){
        for(int i=0;i<entlistV.count;i++){
            if(entlistV.vuid[i]==uid&&entlistV.vids[i]==nam){
                return entlistV.vals[i];
            }
        }
    }
    return 0;
}
FoundVar findvarF(unsigned short uid,unsigned short nam){
    if(entlistV.count>0){
        for(int i=0;i<entlistV.count;i++){
            if(entlistV.vuid[i]==uid&&entlistV.vids[i]==nam){
                FoundVar r = {0};
                r.val = entlistV.vals[i];
                r.ind = i;
                return r;
            }
        }
    }
    return (FoundVar){0};
}
Matrix MatrixRotateXYZa(Vector3 rot){
    Matrix deeznuts = MatrixRotateXYZ((Vector3){rot.x,0,rot.z});
    deeznuts = MatrixMultiply(deeznuts,MatrixRotateXYZ((Vector3){0,rot.y,0}));
    return deeznuts;
}
Matrix trgetm(uint8_t i){
    Matrix deeznuts = MatrixIdentity();
    deeznuts = MatrixMultiply(deeznuts,MatrixScale(gm3d.items[i].s*gm3d.items[i].sx,
                                                   gm3d.items[i].s*gm3d.items[i].sy,
                                                   gm3d.items[i].s*gm3d.items[i].sz));
    deeznuts = MatrixMultiply(deeznuts,MatrixRotateXYZ((Vector3){gm3d.items[i].rx,0,gm3d.items[i].rz}));
    deeznuts = MatrixMultiply(deeznuts,MatrixRotateXYZ((Vector3){0,gm3d.items[i].ry,0}));
    deeznuts = MatrixMultiply(deeznuts,MatrixTranslate(gm3d.items[i].x,gm3d.items[i].y,gm3d.items[i].z));
    return deeznuts;
}
float mod(float a,float b){
    return a-floor(a/b)*b;
}
void compileassets(){
    printf("compileassets1\n");
    int i;
    for(i=0;i<gm3d.count;i++){
        if(gm3d.items[i].sx==0){gm3d.items[i].sx=1;}
        if(gm3d.items[i].sy==0){gm3d.items[i].sy=1;}
        if(gm3d.items[i].sz==0){gm3d.items[i].sz=1;}
        if(gm3d.items[i].gen){
            if(!gm3d.items[i].glow){
                gm3d.items[i].mdl.materials[0].shader = shader;
            }
            if(!gm3d.items[i].plain){
                SetTextureFilter(gm3d.items[i].mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture,TEXTURE_FILTER_BILINEAR);
            }
        }else{
            Image img = LoadImageFromMemory(".png",texlist[gm3d.items[i].tex],texsize[gm3d.items[i].tex]);
            Texture2D t = LoadTextureFromImage(img);
            SetTextureWrap(t,TEXTURE_WRAP_REPEAT);
            prepmodel(glblist[gm3d.items[i].model],glbsize[gm3d.items[i].model]);
            Model m = LoadModel("tuffness.glb");
            SetLoadFileDataCallback(NULL);
            UnloadImage(img);
            m.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = t;
            if(!gm3d.items[i].glow){
                m.materials[1].shader = shader;
            }
            if(!gm3d.items[i].plain){
                SetTextureFilter(m.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture,TEXTURE_FILTER_BILINEAR);
            }
            gm3d.items[i].mdl = m;
        }
        gm3d.items[i].mat = trgetm(i);
    }
    printf("compileassets2\n");
}
Music bgm;//0
Music bgmA;//1
bool canbgmA=false;
Music bgmW;//2
bool canbgmW=false;
Music bgmP;//7
float bgmvols[9] = {1,0,0,0,0,0,0,0,0};
uint8_t songplay=0;

Model skybox;
bool iswallrad=false;
int wallrad=0;
void unloadassets(){
    printf("unloadassets1\n");
    entlist = (EntTL){0};
    entlistV = (EntVL){0};
    nextuid = 0;
    UnloadTexture(skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);
    StopMusicStream(bgm);
    StopMusicStream(bgmP);
    UnloadMusicStream(bgm);
    if(canbgmA){
        StopMusicStream(bgmA);
        UnloadMusicStream(bgmA);
    }
    if(canbgmW){
        StopMusicStream(bgmW);
        UnloadMusicStream(bgmW);
    }
    UnloadMusicStream(bgmP);
    int i;
    for(i=0;i<9;i++){
        bgmvols[i]=0;
    }
    for(i=0;i<gm3d.count;i++){
        if(!gm3d.items[i].plain){
            UnloadTexture(gm3d.items[i].mdl.materials[gm3d.items[i].gen?0:1].maps[MATERIAL_MAP_DIFFUSE].texture);
        }
        if(!gm3d.items[i].keepload){
            UnloadModel(gm3d.items[i].mdl);
        }
    }
    printf("unloadassets2\n");
}
Shader skyshad;
void loadskybox(){
    // Load skybox model
	Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
	skybox = LoadModelFromMesh(cube);
	skyshad = LoadShader(TextFormat("shaders/glsl%i/skybox.vs", GLSL_VERSION),
                         TextFormat("shaders/glsl%i/skybox.fs", GLSL_VERSION));
    skybox.materials[0].shader = skyshad;
	SetShaderValue(skyshad, GetShaderLocation(skyshad, "environmentMap"), (int[1]){ MATERIAL_MAP_CUBEMAP }, SHADER_UNIFORM_INT);
    SetShaderValue(skyshad, GetShaderLocation(skyshad, "doGamma"), (int[1]) { 0 }, SHADER_UNIFORM_INT);
    SetShaderValue(skyshad, GetShaderLocation(skyshad, "vflipped"), (int[1]){ 1 }, SHADER_UNIFORM_INT);
    // Load cubemap shader and setup required shader locations
    Shader shdrCubemap = LoadShader(TextFormat("shaders/glsl%i/cubemap.vs", GLSL_VERSION),
                                    TextFormat("shaders/glsl%i/cubemap.fs", GLSL_VERSION));
	SetShaderValue(shdrCubemap, GetShaderLocation(shdrCubemap, "equirectangularMap"), (int[1]){ 0 }, SHADER_UNIFORM_INT);
}
Vector3 sunpos = (Vector3){0,1,0};
void setsundir(float x,float y,float z){
    float SHld[3] = {x,y,z};
    sunpos=(Vector3){x,y,z};
    SetShaderValue(shader,SHldl,SHld,SHADER_UNIFORM_VEC3);
}
void setsundir2(float cloc,float geog){
    float lat = (geog-23.5)*M_TORAD;
    float lon = (mod(cloc,24)-6)*(15*M_TORAD);
    float coslat = cos(lat);
    float coslon = cos(lon);
    float sinlat = sin(lat);
    float sinlon = sin(lon);
    Vector3 n = Vector3Normalize((Vector3){coslat*coslon,coslat*sinlon,sinlat});
    float SHld[3] = {n.x,n.y,n.z};
    sunpos=(Vector3){n.x,n.y,n.z};
    SetShaderValue(shader,SHldl,SHld,SHADER_UNIFORM_VEC3);
}
float compbrightness(float n){
    float b = fmax(0,fmin(5,n));
    if(n>=.5){
        b=3.135266+(0.5605744-3.135266)/(pow(1+pow(b/92.74922,3.400255),3365850.0));
    }else{
        b*=1.44611366946;
    }
    return b;
}
void setamb(float r,float g,float b,float i){
    SetShaderValue(shader,SHam1l,(float[1]){((float)r)/255},SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader,SHam2l,(float[1]){((float)g)/255},SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader,SHam3l,(float[1]){((float)b)/255},SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader,SHbrl,(float[1]){compbrightness(i)},SHADER_UNIFORM_FLOAT);
}
//entity presets
Entity spawnBreak(float x,float y,float z){
    Terrain t={0};
    t.x=x;t.y=y;t.z=z;t.s=1;
    t.mdl=p_break;
    t.gen=true;
    t.nowall=true;
    t.plain=true;
    t.keepload=true;
    gm3d.items[gm3d.count]=t;
    Entity e=crEnt(OTYPE_CORK,x,y,z,8,8,8);
    addvar(e.uid,V_CORK_TERRAI,gm3d.count);
    gm3d.count++;
    return e;
}
//maps
#define M_TITLE 0
#define M_HUB 1
#define M_TUTORIAL 2
#define M_TURTLE 3
bool usechar = false;
short map;
uint8_t titleselt;
void map_title(){
    setamb(159,154,135,.3);
    setsundir2(13,40);
    iswallrad=false;
    unloadassets();
    titleselt = 0;
    map = M_TITLE;
    usechar = false;
	Image img = LoadImageFromMemory(".png",tex_sky_title,sizeof(tex_sky_title));
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(img);
    bgm = LoadMusicStreamFromMemory(".ogg",title_bgm,sizeof(title_bgm));
	bgm.looping = true;
	PlayMusicStream(bgm);
    gm3dlist newg;
    int i=0;
    Terrain tmp; //robot 64 logo
    tmp.x = 0;tmp.y = 0;tmp.z = 23;
    tmp.s = .03882770207;tmp.model = 2;tmp.tex = 2;
    newg.items[i] = tmp;i++;
    //beams length in Z: 67.5 studs heh 67 67 67 67 67 67 67
    tmp = (Terrain){0}; //beam1
    tmp.x = 0;tmp.y = 0;tmp.z = 6.749;
    tmp.s = 1;tmp.model = 34;tmp.tex = 20;
    tmp.glow=true;newg.items[i] = tmp;i++;
    newg.count = i;
    gm3d = newg;
    compileassets();
}
void map_hub(){
    setamb(159,154,135,1);
    setsundir2(14,54);
    iswallrad=true;wallrad=512;
    unloadassets();
    map = M_HUB;
	Image img = LoadImageFromMemory(".png",tex_sky_hub,sizeof(tex_sky_hub));
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(img);
    bgm = LoadMusicStreamFromMemory(".ogg",hub_bgm,sizeof(hub_bgm));bgm.looping = true;PlayMusicStream(bgm);
    bgmA = LoadMusicStreamFromMemory(".ogg",hub_bgmA,sizeof(hub_bgmA));bgmA.looping = true;PlayMusicStream(bgmA);canbgmA=true;
    bgmW = LoadMusicStreamFromMemory(".ogg",hub_bgmW,sizeof(hub_bgmW));bgmW.looping = true;PlayMusicStream(bgmW);canbgmW=true;
    bgmP = LoadMusicStreamFromMemory(".ogg",hub_bgmP,sizeof(hub_bgmP));bgmP.looping = true;PlayMusicStream(bgmP);
    gm3dlist newg;
    int i=0;
    Terrain tmp = {0}; //hub grass
    tmp.x = 3.241f;tmp.y = 194.599f;tmp.z = 0.463f;
    tmp.s = 41.92438995f;tmp.model = 0;tmp.tex = 0;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub cliff
    tmp.x = 3.24f;tmp.y = 79.377f;tmp.z = 227.391f;
    tmp.s = 41.92438995f;tmp.model = 1;tmp.tex = 1;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub path
    tmp.x = -2.933f;tmp.y = 11.083f;tmp.z = 144.166f;
    tmp.s = 41.92438995f;tmp.model = 3;tmp.tex = 3;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub sand
    tmp.x = 14.753f;tmp.y = 1.028f;tmp.z = 309.615f;
    tmp.s = 41.92438995f;tmp.model = 4;tmp.tex = 4;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubout
    tmp.x = 58.787;tmp.y = 49.15;tmp.z = 195.429;tmp.s = 4.943275534;
    tmp.ry = 90*M_TORAD;
    tmp.model = 13;tmp.tex = 5;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubbottom
    tmp.x = 58.786;tmp.y = -14.61;tmp.z = 195.428;tmp.s = 4.943275534;
    tmp.sy = 5.118946922;
    tmp.ry = 90*M_TORAD;
    tmp.model = 14;tmp.tex = 5;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub logo
    tmp.x = 9.79;tmp.y = 18.691;tmp.z = 195.165;tmp.s = 0.07183186389;
    tmp.sz = 0.07053696656;
    tmp.rx = 90*M_TORAD;
    tmp.ry = 90*M_TORAD;
    tmp.model = 2;tmp.tex = 2;
    tmp.nocol=true;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubcloudfloor
    tmp.x = 22.208;tmp.y = 18.749;tmp.z = 195.429;tmp.s = 4.943;
    tmp.ry = 90*M_TORAD;
    tmp.model = 15;tmp.tex = 6;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubwood
    tmp.x = 22.207;tmp.y = 33.579;tmp.z = 195.429;tmp.s = 4.943296283;
    tmp.ry = 90*M_TORAD;
    tmp.model = 16;tmp.tex = 7;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubshop
    tmp.x = 54.577;tmp.y = 25.081;tmp.z = 168.77;tmp.s = 0.6387276219;
    tmp.ry = 135.002*M_TORAD;
    tmp.model = 17;tmp.tex = 8;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubrusty
    tmp.x = 51.262;tmp.y = 34.939;tmp.z = 172.351;tmp.s = 0.4689069522;
    tmp.ry = 135.002*M_TORAD;
    tmp.model = 18;tmp.tex = 9;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubokwall
    tmp.x = 114.972;tmp.y = 34.32;tmp.z = 195.429;tmp.s = 4.943241379;
    tmp.ry = 90*M_TORAD;
    tmp.model = 19;tmp.tex = 10;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubgoodfloor
    tmp.x = 109.411;tmp.y = 18.749;tmp.z = 195.428;tmp.s = 4.943;
    tmp.ry = 90*M_TORAD;
    tmp.model = 20;tmp.tex = 11;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubstairs
    tmp.x = 136.944;tmp.y = 34.32;tmp.z = 195.429;tmp.s = 4.9432;
    tmp.ry = 90*M_TORAD;
    tmp.model = 21;tmp.tex = 12;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubcarpet
    tmp.x = 114.972;tmp.y = 49.15;tmp.z = 195.429;tmp.s = 4.943241379;
    tmp.ry = 90*M_TORAD;
    tmp.model = 22;tmp.tex = 13;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubbookwall
    tmp.x = 114.972;tmp.y = 64.721;tmp.z = 195.429;tmp.s = 4.943241379;
    tmp.ry = 90*M_TORAD;
    tmp.model = 23;tmp.tex = 14;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubstarryfloor
    tmp.x = 28.432;tmp.y = 49.892;tmp.z = 195.429;tmp.s = 4.943;
    tmp.ry = 90*M_TORAD;
    tmp.model = 24;tmp.tex = 15;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //hub hubstars
    tmp.x = 28.011;tmp.y = 80.678;tmp.z = 195.429;tmp.s = 4.943241153;
    tmp.ry = 90*M_TORAD;
    tmp.model = 25;tmp.tex = 16;
    newg.items[i] = tmp;i++;
    
    //tutorial pads
    img = LoadImageFromMemory(".png",tex_padding,sizeof(tex_padding));
    Texture2D pad = LoadTextureFromImage(img);
    SetTextureWrap(pad,TEXTURE_WRAP_REPEAT);
    UnloadImage(img);
    tmp = (Terrain){0};tmp.x = -12;tmp.y = 37.45;tmp.z = -52;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    8,16,16,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp;i++;
    tmp = (Terrain){0};tmp.x = 20;tmp.y = 37.45;tmp.z = -52;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    8,16,16,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp;i++;
    tmp = (Terrain){0};tmp.x = 4;tmp.y = 49.45;tmp.z = -52;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    40,8,16,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp;i++;
    
    newg.count = i;
    gm3d = newg;
    
    //entities
    i=0;
    Entity tme = crEnt(OTYPE_TELE,4,37.45,-52,24,16,1);
    entlist.items[i]=tme;i++;
    addvar(tme.uid,V_TELE_TOMAP,M_TUTORIAL);
    addvar(tme.uid,V_TELE_TOX,0);addvar(tme.uid,V_TELE_TOY,44);addvar(tme.uid,V_TELE_TOZ,661.5);
    tme = crEnt(OTYPE_TELE,27.2,21.7,145.49,4,6,1);
    entlist.items[i]=tme;i++;
    addvar(tme.uid,V_TELE_TOMAP,M_TURTLE);
    addvar(tme.uid,V_TELE_TOX,-208.902);addvar(tme.uid,V_TELE_TOY,85.29);addvar(tme.uid,V_TELE_TOZ,-236.065);
    tme = crEnt(OTYPE_ICED,-141.19,38,91.62, 1.545, 3.197, 1.783);
    entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_WATR,.026,-3.98,666.921,1391.189,6,932.26);
    tme.mdl = LoadModelFromMesh(GenMeshPlaneT(tme.size.x+15,tme.size.z+15,1,1,20,20));
    tme.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = t_water;
    tme.mdl.materials[0].shader = shader;
    entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_POLE,317.473,51.376,163.876,  2,20.233,2);
    entlist.items[i]=tme;i++;
    
    tme = crEnt(OTYPE_CAND,-48,15,96  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-52,15,101  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-57,15,108  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-61,15,116  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-64,15,126  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-64,15,136  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-63,15,145  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-60,15,155  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-57,15,166  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-54,15,177  ,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-50,15,187  ,4,4,4);entlist.items[i]=tme;i++;
    
    entlist.count = i;
    compileassets();
}
void map_tutorial(){
    setamb(153,142,165,1);
    setsundir2(13,40);
    iswallrad=false;
    unloadassets();
    map = M_TUTORIAL;
	Image img = LoadImageFromMemory(".png",tex_sky_default,sizeof(tex_sky_default));
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(img);
    bgm = LoadMusicStreamFromMemory(".ogg",tutorial_bgm,sizeof(tutorial_bgm));bgm.looping = true;PlayMusicStream(bgm);
    canbgmA=false;
    canbgmW=false;
    bgmP = LoadMusicStreamFromMemory(".ogg",tutorial_bgmP,sizeof(tutorial_bgmP));bgmP.looping = true;PlayMusicStream(bgmP);
    img = LoadImageFromMemory(".png",tex_padding,sizeof(tex_padding));
    Texture2D pad = LoadTextureFromImage(img);
    SetTextureWrap(pad,TEXTURE_WRAP_REPEAT);
    UnloadImage(img);
    gm3dlist newg;
    int i = 0;                                                  //this is just horrible
    Terrain tmp = {0};tmp.x = 0;tmp.y = -28;tmp.z = 28;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,64,160,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = -28;tmp.z = 164;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,64,80,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = 12;tmp.z = 180;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,16,48,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = 28;tmp.z = 188;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,16,32,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = -12;tmp.z = 276;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,96,80,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = -28;tmp.z = 380;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,64,128,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = -28;tmp.z = 513.5;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,64,69,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = -12;tmp.z = 616;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,96,136,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 28;tmp.y = 20;tmp.z = 340;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    32,32,48,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = -28;tmp.y = 20;tmp.z = 340;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    32,32,48,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    
    tmp.x = 0;tmp.y = 28;tmp.z = -56;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,48,24,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 56;tmp.y = 28;tmp.z = 8;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    24,48,152,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = -56;tmp.y = 28;tmp.z = 8;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    24,48,152,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = 36;tmp.z = 64;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    24,32,40,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 28;tmp.y = 28;tmp.z = 64;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    32,48,40,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = -28;tmp.y = 28;tmp.z = 64;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    32,48,40,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 52;tmp.y = 20;tmp.z = 384;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    16,160,600,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = -56;tmp.y = 20;tmp.z = 384;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    24,160,600,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 64;tmp.y = 80;tmp.z = 384;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    8,40,600,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 64;tmp.y = -4;tmp.z = 384;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    8,112,600,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = 68;tmp.z = 352;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    88,64,24,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 26;tmp.y = 20;tmp.z = 496;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    36,160,152,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = -26;tmp.y = 20;tmp.z = 496;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    36,160,152,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = 76;tmp.z = 672;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    24,48,24,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 28;tmp.y = 20;tmp.z = 672;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    32,160,24,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = -28;tmp.y = 20;tmp.z = 672;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    32,160,24,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    
    tmp.x = 0;tmp.y = 76;tmp.z = -72;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    136,144,8,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = 76;tmp.z = 688;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    136,144,8,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 72;tmp.y = 76;tmp.z = 308;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    8,144,768,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = -72;tmp.y = 76;tmp.z = 308;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    8,144,768,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    tmp.x = 0;tmp.y = 28;tmp.z = 352;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    24,16,24,8));tmp.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = pad;tmp.gen = true;newg.items[i] = tmp; i++;
    
    tmp.x = 0;tmp.y = 148.025;tmp.z = 308;tmp.s = 1;tmp.mdl=LoadModelFromMesh(GenMeshCubeT(
    152,.05,768,8));tmp.plain = true;tmp.gen = true;tmp.glow = true;newg.items[i] = tmp; i++;
    
    newg.count = i;
    gm3d = newg;
    
    //entities
    i=0;
    Entity tme = crEnt(OTYPE_TELE,0,44,668.5,24,16,1);
    entlist.items[i]=tme;i++;
    addvar(tme.uid,V_TELE_TOMAP,M_HUB);
    addvar(tme.uid,V_TELE_TOX,4);addvar(tme.uid,V_TELE_TOY,33.95);addvar(tme.uid,V_TELE_TOZ,-45);
    tme = crEnt(OTYPE_TELE,64,56,92.5,8,8,1);
    entlist.items[i]=tme;i++;
    addvar(tme.uid,V_TELE_TOMAP,M_HUB);
    addvar(tme.uid,V_TELE_TOX,4);addvar(tme.uid,V_TELE_TOY,33.95);addvar(tme.uid,V_TELE_TOZ,-45);
    //breakables
    entlist.items[i]=spawnBreak(0,8,32);i++;
    for(int j=0;j<12;j++){
        entlist.items[i]=spawnBreak(-8+(j%3)*8,(j%6)>2?16:8,j>5?56:48);i++;
    }
    for(int j=0;j<9;j++){
        entlist.items[i]=spawnBreak(-8+(j%3)*8,32,floor(j/3)*8+320);i++;
    }
    //candies
    tme = crEnt(OTYPE_CAND,0,6,8,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,6,16,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,6,24,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,6,64,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,6,72,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,6,152,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,14,152,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,22,152,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,22,160,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,22,168,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,30,168,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,38,168,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,38,176,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,38,184,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,38,192,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,38,200,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,42,208,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,45,216,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,45,224,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,42,232,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,38,240,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,0,38,248,4,4,4);entlist.items[i]=tme;i++;
    
    tme = crEnt(OTYPE_CAND,0,6,424,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-2,6,432,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-4,6,440,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-6,11,446,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,16,453,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,16,463,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,12,473,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,7,482,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,6,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,11,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,16,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-2,21,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,2,22,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,7,25,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,7,30,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,7,34,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,2,37,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-2,38,541,4,4,4);entlist.items[i]=tme;i++;
    tme = crEnt(OTYPE_CAND,-7,41,541,4,4,4);entlist.items[i]=tme;i++;
    entlist.count = i;
    compileassets();
}
void map_turtle(){
    setamb(143,148,159,1);
    setsundir2(11.5,49);
    iswallrad=true;wallrad=516;
    unloadassets();
    map = M_TURTLE;
	Image img = LoadImageFromMemory(".png",tex_sky_turtle,sizeof(tex_sky_turtle));
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);
    UnloadImage(img);
    bgm = LoadMusicStreamFromMemory(".ogg",turtle_bgm,sizeof(turtle_bgm));bgm.looping = true;PlayMusicStream(bgm);
    bgmA = LoadMusicStreamFromMemory(".ogg",turtle_bgmA,sizeof(turtle_bgmA));bgmA.looping = true;PlayMusicStream(bgmA);canbgmA=true;
    bgmW = LoadMusicStreamFromMemory(".ogg",turtle_bgmW,sizeof(turtle_bgmW));bgmW.looping = true;PlayMusicStream(bgmW);canbgmW=true;
    bgmP = LoadMusicStreamFromMemory(".ogg",turtle_bgmP,sizeof(turtle_bgmP));bgmP.looping = true;PlayMusicStream(bgmP);
    gm3dlist newg;
    int i = 0;
    Terrain tmp = {0}; //turtle grass
    tmp.x = -2.267;tmp.y = 129.751;tmp.z = -10.327;
    tmp.s = 7.199987404;tmp.model = 26;tmp.tex = 17;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //turtle wall
    tmp.x = -3.255;tmp.y = 24.725;tmp.z = -13.828;
    tmp.s = 7.200008669;tmp.model = 27;tmp.tex = 18;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //turtle path
    tmp.x = 115.193;tmp.y = 153.723;tmp.z = 24.014;
    tmp.s = 7.200085279;tmp.model = 28;tmp.tex = 3;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //turtle sand
    tmp.x = 104.01;tmp.y = 45.557;tmp.z = -146.02;
    tmp.s = 7.2;tmp.model = 29;tmp.tex = 4;
    newg.items[i] = tmp;i++;
    tmp = (Terrain){0}; //turtle turtlebody
    tmp.x = -4;tmp.y = -47;tmp.z = -63;
    tmp.s = 8.185137841;tmp.model = 30;tmp.tex = 19;
    newg.items[i] = tmp;i++;
    
    newg.count = i;
    gm3d = newg;
    
    //entities
    i=0;
    Entity tme = crEnt(OTYPE_TELE,-208.373,94,-235.624,3.1,5.7,7.6); //fix Y later   -208.373,84.792,-235.624
    entlist.items[i]=tme;i++;
    addvar(tme.uid,V_TELE_TOMAP,M_HUB);
    addvar(tme.uid,V_TELE_TOX,27.621);addvar(tme.uid,V_TELE_TOY,21.601);addvar(tme.uid,V_TELE_TOZ,100.188);
    tme = crEnt(OTYPE_WATR,106.529,42.002,-147.059, 258,61.26,286.5);
    tme.mdl = LoadModelFromMesh(GenMeshPlaneT(tme.size.x+15,tme.size.z+15,1,1,20,20));
    tme.mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = t_water;
    tme.mdl.materials[0].shader = shader;
    entlist.items[i]=tme;i++;
    
    entlist.count = i;
    
    compileassets();
}

//vars
Camera camera = { 0 };
Vector3 lastcampos = {0};
uint32_t frame = 0;
uint32_t bus = 0;

Model skybox;

//player variables
Vector3 plroldpos = {0};
Vector3 plrpos = {0,100,0};
Vector3 plrvel = {0};
Vector3 plrdir = {0};
Vector3 plrpoint = {0,0,1};
Vector3 plrorient = {0,0,1};
Vector3 plrgyro = {0,0,1};
Vector3 plrdot = {0};
Vector3 plrdotvel = {0};
bool plrg=false;
bool plrdjump=false;
bool plrsliding=false;
bool plrcrouch=false;
bool plrattack=false;
bool plrlongjump=false;
bool plrpound=false;
bool plrrolling=false;
bool plrwallrun=false;
Vector3 plrwallpoint={0};
Vector3 plrwallnorm={0};
bool plrledgegrab=false;
Vector3 plrledgepoint={0};
bool plrswimming=false;
bool plrpole=false;
Vector3 plrpolepos={0};
Vector3 plrpolesiz={0};
bool oplrg = false;
bool plrdancing = false;
bool plrgotice = false;
bool plrswip = false;
bool plrhasfly = false;
bool plrflying = false;
float plrflypitch = 0;
float plrflyspeed = 0;
Vector3 plrswippoint = (Vector3){0};
uint8_t plrjumping = 0;
uint8_t plrdjumptimer = 0;
uint8_t plrhurt = 0;
uint8_t plrtimeland = 0;
uint8_t plrskin = 1;
float rotand=0;
float botand=0;
float potand=0;
float flipo=0;
float sliderp=0;
float wallerp=0;
float fallerp=0;
float longerp=0;
float roperp=0;
float holderp=0;
float croucherp=0;
float lederp=0;
float polerp=0;
float swimerp=0;
float skaterp=0;
float flamerp=0;
float rollerp=0;
Vector3 boarderp={0};
float aterp=0;
float dancerp=0;
float swiperp=0;
float fallvr=1;
float speedy=0;
float walklerp=0;
float walktick=0;
float walkfx=0;
//cam variables
float camzoom = 2;
float camzoomlerp = 20;
bool firstperson = false;
float camh = 0;
float camv = .4*M_TODEG;
bool oldrightcursor = false;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);          // Update and draw one frame
static void dotheframecrap(void);

Vector3 matlook(Matrix mat){
    return Vector3Normalize(Vector3RotateByQuaternion((Vector3){0,0,-1},QuaternionFromMatrix(mat)));
}
Vector3 matup(Matrix mat){
    return Vector3Normalize(Vector3RotateByQuaternion((Vector3){0,1,0},QuaternionFromMatrix(mat)));
}
Vector3 matrig(Matrix mat){
    return Vector3Normalize(Vector3RotateByQuaternion((Vector3){1,0,0},QuaternionFromMatrix(mat)));
}
Vector3 qualook(Quaternion mat){
    return Vector3Normalize(Vector3RotateByQuaternion((Vector3){0,0,-1},mat));
}
Vector3 quaup(Quaternion mat){
    return Vector3Normalize(Vector3RotateByQuaternion((Vector3){0,1,0},mat));
}
Vector3 quarig(Quaternion mat){
    return Vector3Normalize(Vector3RotateByQuaternion((Vector3){1,0,0},mat));
}
Matrix materp(Matrix cf1,Matrix cf2,float t){
    Vector3 cf1p = Vector3Transform((Vector3){0},cf1);
    Vector3 cf2p = Vector3Transform((Vector3){0},cf2);
    Quaternion cf1r = QuaternionFromMatrix(cf1);
    Quaternion cf2r = QuaternionFromMatrix(cf2);
    Vector3 pos = Vector3Lerp(cf1p,cf2p,t);
    Quaternion rot = QuaternionSlerp(cf1r,cf2r,t);
    return MatrixMultiply(QuaternionToMatrix(rot),MatrixTranslate(pos.x,pos.y,pos.z));
}
Matrix matrel(Matrix mat,Vector3 trl,Matrix rel){ //relative translation
    Vector3 matx = Vector3Scale(matrig(rel),trl.x);
    Vector3 maty = Vector3Scale(matup(rel),trl.y);
    Vector3 matz = Vector3Scale(matlook(rel),-trl.z);
    Matrix newmat = MatrixMultiply(mat,MatrixTranslate(matx.x,matx.y,matx.z));
    newmat = MatrixMultiply(newmat,MatrixTranslate(maty.x,maty.y,maty.z));
    newmat = MatrixMultiply(newmat,MatrixTranslate(matz.x,matz.y,matz.z));
    return newmat;
}
Vector3 qvecerp(Vector3 v1,Vector3 v2,float t){
    Quaternion v1q = QuaternionFromMatrix(MatrixLookAt((Vector3){0},Vector3Multiply(v1,(Vector3){-1,1,1}),(Vector3){0,1}));
    Quaternion v2q = QuaternionFromMatrix(MatrixLookAt((Vector3){0},Vector3Multiply(v2,(Vector3){-1,1,1}),(Vector3){0,1}));
    Quaternion lerp = QuaternionSlerp(v1q,v2q,t);
    return qualook(lerp);
}

Vector3 rotvec(Vector3 vec,Vector3 rot){//xyz yaw pitch roll
    float cosy = cos(rot.x);
    float siny = sin(rot.x);
    float cosp = cos(rot.y);
    float sinp = sin(rot.y);
    float cosr = cos(rot.z);
    float sinr = sin(rot.z);
    
    float Axx=cosy*cosp;
    float Axy=cosy*sinp*sinr-siny*cosr;
    float Axz=cosy*sinp*cosr+siny*sinr;
    
    float Ayx=siny*cosp;
    float Ayy=siny*sinp*sinr+cosy*cosr;
    float Ayz=siny*sinp*cosr-cosy*sinr;
    
    float Azx=-sinp;
    float Azy=cosp*sinr;
    float Azz=cosp*cosr;
    
    return (Vector3){
        Axx*vec.x+Axy*vec.y+Axz*vec.z,
        Ayx*vec.x+Ayy*vec.y+Ayz*vec.z,
        Azx*vec.x+Azy*vec.y+Azz*vec.z
    };
}

Vector3 projv3(Vector3 target,Vector3 planenormal){
    float speedtonormal = Vector3DotProduct(target,Vector3Normalize(planenormal));
    if(speedtonormal>0){
        return target;
    }else{
        return Vector3Subtract(target,Vector3Scale(planenormal,speedtonormal));
    }
}

scuffedrays scuffedcol(Vector3 pos,float radius,Vector3 only,bool full){
    scuffedrays Skibidigyats = {0};
    uint8_t j=0;
    int i = 0;
    float lowv = 100.0f;
    int8_t lowi = -1;
    for(int8_t x=-1;x<2;x++){
        for(int8_t y=-1;y<2;y++){
            for(int8_t z=-1;z<2;z++){
                Vector3 ndir = (Vector3){x,y,z};
                Vector3 dir = Vector3Normalize(ndir);
                if(!((x==0)&&(y==0)&&(z==0))&&Vector3DotProduct(dir,only)>=0&&(full||Vector3Length(ndir)==1)){
                    Ray r = (Ray){pos,dir};
                    for(i=0;i<gm3d.count;i++){
                        Terrain *v = &gm3d.items[i];
                        if(!v->nocol){
                            RayCollision test = GetRayCollisionMesh(r,v->mdl.meshes[0],v->mat);
                            if(test.hit&&(test.distance<=radius)){
                                float odist = Skibidigyats.results[j].distance;
                                if(odist==0||(odist>=test.distance)){
                                    Skibidigyats.results[j] = test;
                                    if(test.distance<lowv){
                                        lowv=test.distance;
                                        lowi=j;
                                        Skibidigyats.v3used = dir;
                                    }
                                }
                            }
                        }
                    }
                    j++;
                }
            }
        }
    }
    Skibidigyats.lowind = lowi;
    return Skibidigyats;
}

void loadskin(uint8_t id){
    Image img = LoadImageFromMemory(".png",skinlist[id-1],skinsize[id-1]);
    curskin = LoadTextureFromImage(img);
    SetTextureFilter(curskin,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
}

Vector3 v2(Vector3 v){
    return (Vector3){v.x,0,v.z};
}

void tomapid(short id){
    switch(id){
        case M_TITLE:
            map_title();
            break;
        case M_HUB:
            map_hub();
            break;
        case M_TUTORIAL:
            map_tutorial();
            break;
        case M_TURTLE:
            map_turtle();
            break;
    }
}

void particle(uint8_t type,int qty,bool dovel,Vector3 ps,float size){
    float a = rand()/(RAND_MAX/3.14); //its 3.14 in the actual game, not pi
    for(int i=1;i<=qty;i++){
        Par c = {0};
        c.type = type;
        Vector3 angle = {0};
        if(qty==1){
            int r1 = (rand()%61-30)/80;
            int r2 = (rand()%61-30)/80;
            int r3 = (rand()%61-30)/80;
            angle=(Vector3){r1,r2,r3};
        }else{
            float r = (float)i/qty*pi*2+a;
            angle=(Vector3){sin(-r),0,cos(r)};
        }
        c.pos = Vector3Add(ps,angle);
        c.rot = MatrixRotateXYZ((Vector3){rand()/(RAND_MAX/pi),rand()/(RAND_MAX/pi),rand()/(RAND_MAX/pi)});
        c.speed = Vector3Add(Vector3Add(Vector3Scale(angle,4),(Vector3){0,2,0}),dovel?(Vector3){0}:Vector3Scale(plrvel,.5));
        int r1 = (rand()%61-30)/20;
        int r2 = (rand()%61-30)/20;
        int r3 = (rand()%61-30)/20;
        c.rs = (Vector3){r1,r2,r3};
        c.scale = .05*size*(.7+(rand()/(RAND_MAX/.3)));
        particles.items[particles.next]=c;
        particles.next=(particles.next+1)%PAR_MAX;
    }
}

double trstart;
bool trsing;
bool trsing2; //enabled  and disabled when trs sounds plays
bool trsto;
short tomap;
float tomapx = 0;
float tomapy = 100;
float tomapz = 0;
bool loading;
uint8_t loadstate;
void transition(bool to){
    trsto = to;
    trsing = true;
    trsing2=to;
    trstart = GetTime();
    loadstate=0;
    if(to){
        PlaySound(s_load1);
    }else{
        PlaySound(s_load2);
        loading=false;
        //mapstuff, might make a toggle
        plrpos = (Vector3){tomapx,tomapy,tomapz};
        plrvel = (Vector3){0};
    }
}

Vector2 fixrotpos2(float rot){
    float r = (rot+90)*pi;
    return (Vector2){sin(r/180)+cos(r/180),(sin(r/180)-cos(r/180))};
}

Font r64font;
void r64text(const char *text,int x,int y,float size,float ax,float ay,Color clr){
    Vector2 measure = MeasureTextEx(r64font,text,size,0);
    DrawTextPro(r64font,text,(Vector2){x-(measure.x*ax),(y-(measure.y*ay))+(size/16)},(Vector2){0,0},0,size,0,(Color){0,0,0,51});
    DrawTextPro(r64font,text,(Vector2){x-(measure.x*ax),y-(measure.y*ay)},(Vector2){0,0},0,size,0,clr);
}
void tex3d(Texture2D tex,Vector3 pos,float scale,Color c){
    Vector3 forward = Vector3Subtract(camera.target,camera.position);
    Vector3 up = {0,1,0};
    Vector3 right = Vector3CrossProduct(up,forward);
    up = Vector3CrossProduct(forward,right);
    up = Vector3Normalize(up);
    Vector2 size = (Vector2){scale*fabsf((float)tex.width/tex.height),scale};
    
    DrawBillboardPro(camera,tex,(Rectangle){.0f,.0f,(float)tex.width,(float)tex.height},pos,up,size,Vector2Scale(size,.5),0,c);
}

double gtick = 0;
double g06next = 0;
uint8_t faceid = 0;

#define dt 1.0f/60.0f
Matrix vistorso = {0};
Vector3 vistorsopos = {0};
Matrix vistorsorot = {0};
Vector3 visjetpos = {0};
Vector3 shadowpos = {0,-10000,0};
bool paused = false;
uint8_t pausemenu = 0; //id of the menu the pausescreen currently shows
int8_t pauseselt = 0; //id of the button thats selected
float spinyx = .2;
float spinyy = .2;
uint8_t icedrot = 0;
void drawbeeb(){
    Matrix torsopos = MatrixIdentity();
    Matrix headpos = MatrixIdentity();
    Matrix legApos = MatrixIdentity();
    Matrix legBpos = MatrixIdentity();
    Matrix armApos = MatrixIdentity();
    Matrix armBpos = MatrixIdentity();
    Matrix armCpos = MatrixIdentity();
    Matrix armDpos = MatrixIdentity();
    Vector3 torsoonlypos=(Vector3){0};
    Matrix torsorot = MatrixIdentity();
    Matrix mps = MatrixMultiply(MatrixLookAt((Vector3){0},Vector3Multiply(plrorient,(Vector3){-1,1,1}),(Vector3){0,1}),MatrixTranslate(plrpos.x,plrpos.y-1,plrpos.z));
    Matrix mpsrot = mps;
    mpsrot.m12=0;mpsrot.m13=0;mpsrot.m14=0;
    if(plrgotice){
        torsopos = mps;
        torsoonlypos = Vector3Transform((Vector3){0},torsopos);
        torsorot = torsopos;
        torsorot.m12 = 0.0f;
        torsorot.m13 = 0.0f;
        torsorot.m14 = 0.0f;
        headpos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){13.885*M_TORAD,1.324*M_TORAD,2.772*M_TORAD}),torsorot),
        (Vector3){-.001,1.631,0},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        legApos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){44.867*M_TORAD,1.058*M_TORAD,16.932*M_TORAD}),torsorot),
        (Vector3){.584,-.899,-.58},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        legBpos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){-22.601*M_TORAD,6.557*M_TORAD,-10.034*M_TORAD}),torsorot),
        (Vector3){-.447,-1.148,.14},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        armApos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){50.572*M_TORAD,106.882*M_TORAD,-2.542*M_TORAD}),torsorot),
        (Vector3){.903,.782,-.18},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        armBpos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){46.542*M_TORAD,-114.486*M_TORAD,.658*M_TORAD}),torsorot),
        (Vector3){-.887,.707,-.19},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        armCpos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){9.873*M_TORAD,89.274*M_TORAD,175.889*M_TORAD}),torsorot),
        (Vector3){1.292,1.474,-.28},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        armDpos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){19.754*M_TORAD,-113.843*M_TORAD,179.089*M_TORAD}),torsorot),
        (Vector3){-1.293,1.403,-.38},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
    }else{
        //oh boy here we go
        float grn = 1-fabs(fallerp);
        float nrm = (1-holderp)*(1-croucherp)*(1-lederp)*(1-polerp)*(1-aterp)*(1-dancerp);
        float wtk = sin(walktick*14)*walklerp;
        float wallslide = (1-walklerp)*wallerp*(1-lederp)*(1-skaterp);
        float sin1 = fabs(wtk) * (.8*grn)/(roperp+1)*(1-swimerp);
        float sin2 = wtk*-1.2*grn/(roperp+1)/(polerp+1) + fallerp*(1-sliderp)*(1-longerp)*(1-aterp)
            +sin(gtick*6)*dancerp;
        Vector3 ag = {sliderp*2-longerp+croucherp+(-swimerp*1-walklerp*swimerp-(skaterp+rotand*skaterp)*.4)*nrm+polerp+sin2*polerp/2+wallslide
            , sin2*nrm*(1-swimerp)+holderp+sin2*dancerp/2*(1-holderp)
            ,.6+(walklerp*.7*grn*nrm*(1-swimerp)+fabs(fallerp)*nrm)*(1-roperp)+roperp*.7+holderp/2+aterp*(1-holderp)-(fabs(sin2)*dancerp-dancerp)*(1-holderp)};
        Vector3 bg = Vector3Subtract(Vector3Scale((Vector3){sliderp/2+(skaterp+rotand*skaterp)*.4+wallslide, 0,fallerp*.8*(1-longerp)},nrm)
            ,(Vector3){-holderp/2-croucherp*(1-holderp)-polerp*(1-holderp)-(sin(gtick*3)*dancerp/4+dancerp/2)*(1-holderp)});
        float hip = grn*1.4-sliderp*1.4-croucherp-wallerp;
        //                        TORSO
        torsopos = MatrixMultiply(torsopos,MatrixRotateXYZ((Vector3){botand+fabs(rotand)/2*wallerp, potand+sin(walktick*7)*swimerp*.1-skaterp*1.5+sin2*dancerp/8,0}));
        torsopos = MatrixMultiply(torsopos,MatrixRotateXYZ((Vector3){0,0,rotand+wtk*croucherp/5+wtk*polerp/12}));
        torsopos = MatrixMultiply(torsopos,mps);
        torsopos = MatrixMultiply(torsopos,MatrixTranslate(0,-hip+swimerp*1.4+skaterp*.4+fabs(-sin2)*dancerp/9+wallerp-sliderp*1.4,0));
        Vector3 torsotrl = {0,(plrwallrun?sin1/2:sin1)/(polerp*3+1)+hip,croucherp*.5};
        Vector3 torsotrlY = Vector3Scale(matup(torsopos),torsotrl.y);
        Vector3 torsotrlZ = Vector3Scale(matlook(torsopos),-torsotrl.z);
        torsopos = MatrixMultiply(torsopos,MatrixTranslate(torsotrlY.x,torsotrlY.y,torsotrlY.z));
        torsopos = MatrixMultiply(torsopos,MatrixTranslate(torsotrlZ.x,torsotrlZ.y,torsotrlZ.z));
        //    extra torso stuff
        torsoonlypos = Vector3Transform((Vector3){0},torsopos);
        torsorot = torsopos;
        torsorot.m12 = 0.0f;
        torsorot.m13 = 0.0f;
        torsorot.m14 = 0.0f;
        //                        HEAD
        headpos = MatrixMultiply(MatrixRotateXYZ((Vector3){sliderp-longerp+croucherp*.8+polerp/2,skaterp-wallslide,rotand*.2+sin2*dancerp/4}),torsorot);
        Vector3 headtrl = Vector3Scale(matup(torsopos),.8f);
        headpos = MatrixMultiply(headpos,MatrixTranslate(headtrl.x,headtrl.y,headtrl.z));
        headtrl = Vector3Scale(matup(headpos),.8f);
        headpos = MatrixMultiply(headpos,MatrixTranslate(headtrl.x,headtrl.y,headtrl.z));
        headpos = MatrixMultiply(headpos,MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        //                        LEG A
        legApos = MatrixMultiply(MatrixRotateXYZ((Vector3){
            -sin2*(1-dancerp*.8)*(1-swimerp/2)*(1-croucherp/2)*(1-polerp)+polerp/2+sin2*polerp/4+longerp-sliderp/2+lederp-rotand*skaterp/4+wallslide
                ,0,roperp*-.3-polerp*.3+aterp/4 +.1+skaterp/4+fmin(0,sin2*dancerp/2)}),torsorot);
        Vector3 legAtrl = {.35,-.5+wtk*croucherp/3+croucherp/2,-.15-croucherp/3};
        Vector3 legAtrlX = Vector3Scale(matrig(torsopos),legAtrl.x);
        Vector3 legAtrlY = Vector3Scale(matup(torsopos),legAtrl.y);
        Vector3 legAtrlZ = Vector3Scale(matlook(torsopos),-legAtrl.z);
        legApos = MatrixMultiply(legApos,MatrixTranslate(legAtrlX.x,legAtrlX.y,legAtrlX.z));
        legApos = MatrixMultiply(legApos,MatrixTranslate(legAtrlY.x,legAtrlY.y,legAtrlY.z));
        legApos = MatrixMultiply(legApos,MatrixTranslate(legAtrlZ.x,legAtrlZ.y,legAtrlZ.z));
        Vector3 legAtrlY2 = Vector3Scale(matup(legApos),-.7);
        legApos = MatrixMultiply(legApos,MatrixTranslate(legAtrlY2.x,legAtrlY2.y,legAtrlY2.z));
        legApos = MatrixMultiply(legApos,MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        //                        LEG B
        legBpos = MatrixMultiply(MatrixRotateXYZ((Vector3){
            sin2*(1-dancerp*.8)*(1-swimerp/2)*(1-croucherp/2)*(1-polerp)+polerp/2-sin2*polerp/4+longerp-sliderp/2+lederp-rotand*skaterp/2+wallslide
                ,0,roperp*.3+polerp*.3-aterp/4-.1-skaterp/4+fmax(0,sin2*dancerp/2)}),torsorot);
        Vector3 legBtrl = {-.35,-.5-wtk*croucherp/3+croucherp/2,-.15-croucherp/3};
        Vector3 legBtrlX = Vector3Scale(matrig(torsopos),legBtrl.x);
        Vector3 legBtrlY = Vector3Scale(matup(torsopos),legBtrl.y);
        Vector3 legBtrlZ = Vector3Scale(matlook(torsopos),-legBtrl.z);
        legBpos = MatrixMultiply(legBpos,MatrixTranslate(legBtrlX.x,legBtrlX.y,legBtrlX.z));
        legBpos = MatrixMultiply(legBpos,MatrixTranslate(legBtrlY.x,legBtrlY.y,legBtrlY.z));
        legBpos = MatrixMultiply(legBpos,MatrixTranslate(legBtrlZ.x,legBtrlZ.y,legBtrlZ.z));
        Vector3 legBtrlY2 = Vector3Scale(matup(legBpos),-.7);
        legBpos = MatrixMultiply(legBpos,MatrixTranslate(legBtrlY2.x,legBtrlY2.y,legBtrlY2.z));
        legBpos = MatrixMultiply(legBpos,MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        //                        ARM A
        armApos = MatrixMultiply(MatrixRotateXYZ((Vector3){ag.x+lederp,ag.y,ag.z}),torsorot);
        Vector3 armAtrlX = Vector3Scale(matrig(torsopos),.5);
        Vector3 armAtrlY = Vector3Scale(matup(torsopos),.6);
        armApos = MatrixMultiply(armApos,MatrixTranslate(armAtrlX.x,armAtrlX.y,armAtrlX.z));
        armApos = MatrixMultiply(armApos,MatrixTranslate(armAtrlY.x,armAtrlY.y,armAtrlY.z));
        Vector3 armAtrlY2 = Vector3Scale(matup(armApos),-.4);
        armApos = MatrixMultiply(armApos,MatrixTranslate(armAtrlY2.x,armAtrlY2.y,armAtrlY2.z));
        armApos = MatrixMultiply(armApos,MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        //extra armA stuff
        Vector3 armAonlypos = Vector3Transform((Vector3){0},armApos);
        Matrix armArot = armApos;
        armArot.m12 = 0.0f;
        armArot.m13 = 0.0f;
        armArot.m14 = 0.0f;
        //                        ARM B
        armBpos = MatrixMultiply(MatrixRotateXYZ((Vector3){ag.x+lederp,ag.y*(-holderp*2+1),-ag.z}),torsorot);
        Vector3 armBtrlX = Vector3Scale(matrig(torsopos),-.5);
        Vector3 armBtrlY = Vector3Scale(matup(torsopos),.6);
        armBpos = MatrixMultiply(armBpos,MatrixTranslate(armBtrlX.x,armBtrlX.y,armBtrlX.z));
        armBpos = MatrixMultiply(armBpos,MatrixTranslate(armBtrlY.x,armBtrlY.y,armBtrlY.z));
        Vector3 armBtrlY2 = Vector3Scale(matup(armBpos),-.4);
        armBpos = MatrixMultiply(armBpos,MatrixTranslate(armBtrlY2.x,armBtrlY2.y,armBtrlY2.z));
        armBpos = MatrixMultiply(armBpos,MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        //extra armB stuff
        Vector3 armBonlypos = Vector3Transform((Vector3){0},armBpos);
        Matrix armBrot = armBpos;
        armBrot.m12 = 0.0f;
        armBrot.m13 = 0.0f;
        armBrot.m14 = 0.0f;
        //                        ARM C
        armCpos = MatrixMultiply(MatrixRotateXYZ((Vector3){bg.x+fmax(0,sin2)*(1-swimerp)*(1-holderp)+lederp/2*(1-holderp),bg.y+polerp,bg.z}),armArot);
        Vector3 armCtrlY = Vector3Scale(matup(armApos),-.3-fabs(sin2*dancerp/8));
        armCpos = MatrixMultiply(armCpos,MatrixTranslate(armCtrlY.x,armCtrlY.y,armCtrlY.z));
        armCtrlY = Vector3Scale(matup(armCpos),-.4);
        armCpos = MatrixMultiply(armCpos,MatrixTranslate(armCtrlY.x,armCtrlY.y,armCtrlY.z));
        armCpos = MatrixMultiply(armCpos,MatrixTranslate(armAonlypos.x,armAonlypos.y,armAonlypos.z));
        //                        ARM D
        armDpos = MatrixMultiply(MatrixRotateXYZ((Vector3){bg.x+fmax(0,-sin2)*(1-swimerp)*(1-holderp)+lederp/2*(1-holderp),bg.y-polerp,bg.z}),armBrot);
        Vector3 armDtrlY = Vector3Scale(matup(armBpos),-.3-fabs(sin2*dancerp/8));
        armDpos = MatrixMultiply(armDpos,MatrixTranslate(armDtrlY.x,armDtrlY.y,armDtrlY.z));
        armDtrlY = Vector3Scale(matup(armDpos),-.4);
        armDpos = MatrixMultiply(armDpos,MatrixTranslate(armDtrlY.x,armDtrlY.y,armDtrlY.z));
        armDpos = MatrixMultiply(armDpos,MatrixTranslate(armBonlypos.x,armBonlypos.y,armBonlypos.z));
        
        //reanimate to do poses
        if(swiperp>.01){
            float mpsy=plrpos.y-1;
            //                                                                                          rotation
            Matrix newtorso=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){25.423*M_TORAD,-53.578*M_TORAD,-16.02*M_TORAD}),mpsrot),
            (Vector3){.084,-.881,-.177},mpsrot), //position
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            torsopos = materp(torsopos,newtorso,swiperp);
            Matrix newhead=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){2.137*M_TORAD,-26.005*M_TORAD,-.001*M_TORAD}),mpsrot),
            (Vector3){-.022,.707,.244},mpsrot),
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            headpos = materp(headpos,newhead,swiperp);
            Matrix newlegA=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){25.997*M_TORAD,-45.415*M_TORAD,-66.685*M_TORAD}),mpsrot),
            (Vector3){.824,-1.447,-.873},mpsrot),
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            legApos = materp(legApos,newlegA,swiperp);
            Matrix newlegB=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){77.723*M_TORAD,62.107*M_TORAD,75.705*M_TORAD}),mpsrot),
            (Vector3){.058,-1.187,-1.481},mpsrot),
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            legBpos = materp(legBpos,newlegB,swiperp);
            Matrix newarmA=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){-35.808*M_TORAD,42.934*M_TORAD,-2.297*M_TORAD}),mpsrot),
            (Vector3){.553,-.767,.57},mpsrot),
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            armApos = materp(armApos,newarmA,swiperp);
            Matrix newarmB=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){-71.573*M_TORAD,-118.485*M_TORAD,-23.563*M_TORAD}),mpsrot),
            (Vector3){-.704,-.507,-.584},mpsrot),
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            armBpos = materp(armBpos,newarmB,swiperp);
            Matrix newarmC=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){35.811*M_TORAD,-137.064*M_TORAD,-44.387*M_TORAD}),mpsrot),
            (Vector3){1.075,-1.41,.757},mpsrot),
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            armCpos = materp(armCpos,newarmC,swiperp);
            Matrix newarmD=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){.755*M_TORAD,-110.61*M_TORAD,-96.794*M_TORAD}),mpsrot),
            (Vector3){-.798,-.589,-1.359},mpsrot),
            MatrixTranslate(plrpos.x,mpsy,plrpos.z));
            armDpos = materp(armDpos,newarmD,swiperp);
        }else if(rollerp>.01){//                                                                    rotation                                  position
            Matrix newhead=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){-45.228*M_TORAD,0,0}),torsorot),(Vector3){0,1.42,-.84},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            headpos = materp(headpos,newhead,rollerp);
            Matrix newlegA=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){9.239*M_TORAD,0,0}),torsorot),(Vector3){.4,-.48,-.5},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            legApos = materp(legApos,newlegA,rollerp);
            Matrix newlegB=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){6.646*M_TORAD,0,0}),torsorot),(Vector3){-.4,-.57,-.51},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            legBpos = materp(legBpos,newlegB,rollerp);
            Matrix newarmA=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){53.638*M_TORAD,.001*M_TORAD,179.169*M_TORAD}),torsorot),
            (Vector3){.75,.243,-.387},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            armApos = materp(armApos,newarmA,rollerp);
            Matrix newarmB=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){74.7*M_TORAD,.011*M_TORAD,-169.462*M_TORAD}),torsorot),
            (Vector3){-.7,.262,-.445},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            armBpos = materp(armBpos,newarmB,rollerp);
            Matrix newarmC=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){53.641*M_TORAD,0,-68.521*M_TORAD}),torsorot),
            (Vector3){.33,-.117,-.822},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            armCpos = materp(armCpos,newarmC,rollerp);
            Matrix newarmD=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){15.184*M_TORAD,172.898*M_TORAD,-91.869*M_TORAD}),torsorot),
            (Vector3){-.2,.164,-.802},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            armDpos = materp(armDpos,newarmD,rollerp);
        }
    }
    
    vistorso = torsopos;
    vistorsopos = torsoonlypos;
    vistorsorot = torsorot;
    Vector3 headonlypos = Vector3Transform((Vector3){0},headpos);
    if(!paused){
        Vector3 hed = Vector3Add(headonlypos,Vector3Scale(matup(headpos),2));
        Vector3 t = Vector3Subtract(hed,plrdot);
        plrdotvel = Vector3Subtract(Vector3Add(plrdotvel,Vector3Scale(t,dt*8)),Vector3Scale(plrdotvel,dt*5));
        float complexcrap = headonlypos.y+(matup(headpos).y*1.5);
        if(plrdot.y<complexcrap){
            plrdot = Vector3Add(v2(plrdot),(Vector3){0,complexcrap});
        }
        if(Vector3Length(Vector3Subtract(plrdot,hed))>2){
            Vector3 lv = Vector3Scale(Vector3Normalize(Vector3Subtract(plrdot,hed)),1.9f);
            plrdot=Vector3Add(hed,lv);
            plrdotvel=(Vector3){0};
        }else{
            plrdot=Vector3Add(plrdot,Vector3Scale(plrdotvel,dt*10));
        }
        
        RayCollision shadray = {0};
        shadowpos = (Vector3){0,-10000,0};
        float shadowdist = 30;
        for(int i=0;i<gm3d.count;i++){
            if(!gm3d.items[i].nocol){
                shadray = GetRayCollisionMesh((Ray){vistorsopos,(Vector3){0,-1,0}},gm3d.items[i].mdl.meshes[0],gm3d.items[i].mat);
                if((shadray.hit&&shadray.distance<=30)&&(shadray.distance<shadowdist)){
                    shadowdist=shadray.distance;
                    shadowpos = shadray.point;
                }
            }
        }
    }
    
    //where we draw everything
    if(camzoomlerp>.2||plrgotice){
        DrawMesh(b_torso.meshes[0],b_torso.materials[1],MatrixMultiply(MatrixScale(0.9432819383,0.943246311,0.9435414885),torsopos));
        Matrix headposS = MatrixMultiply(MatrixScale(0.25f,0.25f,0.25f),headpos);
        DrawMesh(b_head.meshes[0],b_head.materials[1],headposS);
        DrawMesh(b_face.meshes[0],b_face.materials[1],MatrixMultiply(MatrixScale(1.005f,1.005f,1.005f),headposS));
        DrawMesh(b_leg.meshes[0],b_leg.materials[1],MatrixMultiply(MatrixScale(0.8048076923,0.8043052838,0.8041305796),legApos));
        DrawMesh(b_leg.meshes[0],b_leg.materials[1],MatrixMultiply(MatrixScale(0.8048076923,0.8043052838,0.8041305796),legBpos));
        DrawMesh(b_tarm.meshes[0],b_tarm.materials[1],MatrixMultiply(MatrixScale(0.6875,0.9083402147f,0.6875),armApos));
        DrawMesh(b_tarm.meshes[0],b_tarm.materials[1],MatrixMultiply(MatrixScale(0.6875,0.9083402147f,0.6875),armBpos));
        DrawMesh(b_barm.meshes[0],b_barm.materials[1],MatrixMultiply(MatrixScale(0.5178907721,0.4793028322f,0.5178907721),armCpos));
        DrawMesh(b_barm.meshes[0],b_barm.materials[1],MatrixMultiply(MatrixScale(0.5178907721,0.4793028322f,0.5178907721),armDpos));
        DrawMesh(b_dot.meshes[0],b_dot.materials[1],MatrixMultiply(MatrixScale(0.01200377729f,0.0135f,0.0120024931f),MatrixTranslate(plrdot.x,plrdot.y,plrdot.z)));
        DrawCylinderEx(Vector3Add(headonlypos,Vector3Scale(matup(headpos),.8)),plrdot,.12,.12,3,BLACK);
        
        //the hat                               (replace this with vectror3s of hat ids later)
        
        Matrix headrot = headpos;
        headrot.m12 = 0.0f;
        headrot.m13 = 0.0f;
        headrot.m14 = 0.0f;
        Matrix hatpos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZa((Vector3){0,0,0}),headrot),(Vector3){0,.946,-.2},headrot),
        MatrixTranslate(headonlypos.x,headonlypos.y,headonlypos.z));
        DrawMesh(curhat.meshes[0],curhat.materials[1],MatrixMultiply(MatrixScale(0.0298034168,0.0298,0.02979860879),hatpos)); //originally only x
        
        Matrix jetpos=MatrixMultiply(matrel(torsorot,(Vector3){0,.2,1},torsorot),
        MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
        if(plrhasfly){
            DrawMesh(b_jetpack.meshes[0],b_jetpack.materials[1],MatrixMultiply(MatrixScale(0.02470078114,0.02470538123,0.0247),jetpos));
            float br = 1+rand()/(RAND_MAX*10.0);
            Matrix jetbpos=MatrixMultiply(matrel(torsorot,(Vector3){0,-.6,1},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            DrawMesh(b_booster.meshes[0],b_booster.materials[1],MatrixMultiply(MatrixScale(0.01214169018*br,0.01214515657*br,0.01214*br),jetbpos));
        }else{
            Matrix packpos=MatrixMultiply(matrel(MatrixMultiply(MatrixRotateXYZ((Vector3){1.44,pi,0}),torsorot),(Vector3){0,.12,.48},torsorot),
            MatrixTranslate(torsoonlypos.x,torsoonlypos.y,torsoonlypos.z));
            DrawMesh(b_pack.meshes[0],b_pack.materials[1],MatrixMultiply(MatrixScale(.33,.8403361345,.3229527105),packpos));
        }
        visjetpos = Vector3Transform((Vector3){0},jetpos);
    }
    if(plrgotice){
        Matrix charpos = MatrixTranslate(plrpos.x,plrpos.y,plrpos.z);
        Matrix icedpos = MatrixMultiply(MatrixScale(.0309,.03088978424,.0308824803),MatrixMultiply(MatrixRotateXYZ((Vector3){0,0,(float)icedrot*(pi/2)}),mpsrot));
        DrawMesh(ml_icedcream.meshes[0],ml_icedcream.materials[1],MatrixMultiply(icedpos,MatrixMultiply(charpos,MatrixTranslate(0,4,0))));
    }
}

#define P_TALL 2.868053436279297f
RayCollision beebtryground(Vector3 off, float raydist){
    RayCollision skibidi = {0};
    plrg = false;
    float closest = 67;
    for(int i=0;i<gm3d.count;i++){
        if(!gm3d.items[i].nocol){
            RayCollision test = GetRayCollisionMesh((Ray){Vector3Add(plrpos,off),(Vector3){0,-1,0}},gm3d.items[i].mdl.meshes[0],gm3d.items[i].mat);
            if(test.hit&&test.distance<=raydist&&test.normal.y>=.6&&test.distance<closest){
                plrg = true;
                skibidi = test;
                closest=test.distance;
            }
        }
    }
    if(plrg){
        plrvel = (Vector3){plrvel.x,0,plrvel.z};
        plrpos = Vector3Add(Vector3Subtract(skibidi.point,off),(Vector3){0,P_TALL});
    }
    return skibidi;
}
RayCollision beebtrywall(Vector3 dir, float raydist){
    RayCollision toilets = {0};
    float closest = 67;
    for(int i=0;i<gm3d.count;i++){
        if(!gm3d.items[i].nowall&&!gm3d.items[i].nocol){
            RayCollision test = GetRayCollisionMesh((Ray){Vector3Add(plrpos,(Vector3){0,-1,0}),dir},gm3d.items[i].mdl.meshes[0],gm3d.items[i].mat);
            if(test.hit&&test.distance<=raydist&&test.distance<closest){
                toilets=test;
                plrwallrun = true;
                plrwallpoint = toilets.point;
                plrwallnorm = toilets.normal;
                closest=toilets.distance;
            }
        }
    }
    return toilets;
}

#define MAX_SOUND_INSTANCES 100
#define DOPPLER_SCALE .1f

float CalculateAttenuation(float distance, float maxDistance, float rolloff)
{
    // Legacy attenuation code
    // float attenuation = 1.0f/(1.0f + (distance/maxDist));
    // Inverse-Square Attenuation code
    float attenuation = 1.0f/(1.0f + rolloff * (distance-1.0f));
    attenuation = Clamp(attenuation, 0.0f, 1.0f);

    return attenuation;
}
struct SoundInstance {
    int id; // must be unique
    Sound sound;
    Vector3 position;
    Vector3 lastPosition;
    float lastDist;
    float maxDistance;
    float rolloff;
    float volumeModifier;
    float pitchModifier;
};
struct ActiveSounds {
    int count;
    struct SoundInstance instances[MAX_SOUND_INSTANCES];
};
struct ActiveSounds activeSounds = {0};

// Plays a sound at 3d coordinates. Usage: ConfigureSoundForPosition(camera, sound, (Vector3){x,y,z}, maxDistance);
void ConfigureSoundForPosition(Camera listener, Sound sound, Vector3 position, float maxDist, float rolloff,float volumeModifier)
{
    Vector3 direction = Vector3Subtract(position, listener.position);
    float distance = Vector3Length(direction);

    float attenuation = CalculateAttenuation(distance, maxDist, rolloff);

    Vector3 normalizedDirection = Vector3Normalize(direction);
    Vector3 forward = Vector3Normalize(Vector3Subtract(listener.target, listener.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(listener.up, forward));

    float dotProduct = Vector3DotProduct(forward, normalizedDirection);
    if (dotProduct < 0.0f) attenuation *= (1.0f + dotProduct*0.5f);

    float pan = 0.5f+(0.5f*Vector3DotProduct(normalizedDirection, right));

    SetSoundVolume(sound, attenuation*volumeModifier);
    SetSoundPan(sound, pan);

    // store sound for updating in the main loop
    bool found = false;
    for (int i = 0; i < activeSounds.count; i++) {
        if (activeSounds.instances[i].sound.stream.buffer == sound.stream.buffer) {
            found = true;
            activeSounds.instances[i].lastPosition = activeSounds.instances[i].position;
            activeSounds.instances[i].position = position;
            activeSounds.instances[i].maxDistance = maxDist;
            activeSounds.instances[i].rolloff = rolloff;
            activeSounds.instances[i].volumeModifier = volumeModifier;
            break;
        }
    }
    if (!found && activeSounds.count < MAX_SOUND_INSTANCES) {
        activeSounds.instances[activeSounds.count++] = (struct SoundInstance){ 
            .id = activeSounds.count, 
            .sound = sound, 
            .position = position, 
            .lastPosition = position, 
            .maxDistance = maxDist, 
            .rolloff = rolloff, 
            .volumeModifier = volumeModifier,
            .pitchModifier = 1.0f
        };
    } else {
        TraceLog(LOG_WARNING, "Max sound instances reached, cannot track new sound.");
    }
}

void PlaySoundAtPosition(Sound sound, Vector3 position, float *maxDist,float volumeModifier)
{
    float md = (maxDist != NULL) ? *maxDist : 100.0f; // Default max distance if not provided
    ConfigureSoundForPosition(camera, sound, position, md, soundRolloffGlobal,volumeModifier);
    PlaySound(sound);
}

void PlaySoundAtBeebo(Sound sound,float volumeModifier)
{
    PlaySoundAtPosition(sound, vistorsopos, NULL,volumeModifier);
}

bool debugmode = false;
#define P_DEBUGSPEED 2
#define P_BFORCE .012f
int mx;
int my;
RayCollision skibidi = {0};
RayCollision toilets = {0};
RayCollision oskibidi = {0};
float gravmult = 1;
uint8_t ledgetimer = 0;
uint8_t poundtimer = 0;
float sensitivity = .5;
bool plranchored = false;
uint8_t attacktimer = 0;
bool mouselock = false;
uint8_t plrslidejumptimer = 0;
uint8_t plrbackfliptimer = 0;
uint8_t ledgetimer2 = 0;
uint8_t walltimer = 0;
uint8_t swiptimer = 0;
uint8_t icedtimer = 0;
uint8_t watertimer = 0;
bool plrdebounce=false;
uint8_t plrdebouncetimer = 0;
bool canmove = true;
bool stillcam = true;
bool snapcam = false;
Vector3 snapto = {0};

int candy = 0;
int scandy = 0;
int icedcream = 0;
void stepchar(){
    bool rightcursor = mouselock||IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    float yimh = 0;
    float yimv = 0;
    if(rightcursor){
        Vector2 delta = GetMouseDelta();
        yimh += delta.x*sensitivity;
        yimv += delta.y*sensitivity;
    }
    if(oldrightcursor!=rightcursor){
        if(rightcursor){
            DisableCursor();
        }else{
            EnableCursor();
#if defined(PLATFORM_WEB)
#else
            HideCursor();
#endif
            SetMousePosition(mx,my);
        }
        oldrightcursor = rightcursor;
    }
    {
        bool dUP = IsKeyDown(KEY_UP);
        bool dDN = IsKeyDown(KEY_DOWN);
        bool dRT = IsKeyDown(KEY_RIGHT);
        bool dLF = IsKeyDown(KEY_LEFT);
        if(abs(dUP-dDN)+abs(dRT-dLF)>0){
            Vector2 addtoyim = Vector2Normalize((Vector2){dRT-dLF,dDN-dUP});
            yimv+=addtoyim.y*(dt*2.5*M_TODEG);
            yimh+=addtoyim.x*(dt*2.5*1.3*M_TODEG);
            /*
            if(dUP){
                yimv-=dt*2.5*M_TODEG;
            }
            if(dDN){
                yimv+=dt*2.5*M_TODEG;
            }
            if(dLF){
                yimh-=dt*2.5*1.3*M_TODEG;
            }
            if(dRT){
                yimh+=dt*2.5*1.3*M_TODEG;
            }
            */
#if defined(PLATFORM_WEB)
#else
        }else{
            Vector2 st2 = {GetGamepadAxisMovement(0,GAMEPAD_AXIS_RIGHT_X),
            GetGamepadAxisMovement(0,GAMEPAD_AXIS_RIGHT_Y)};
            if(Vector2Length(st2)>=.2){
                if(Vector2Length(st2)>1){
                    st2=Vector2Normalize(st2);
                }
                yimv+=st2.y*(dt*2.5*M_TODEG);
                yimh+=st2.x*(dt*2.5*1.3*M_TODEG);
            }
#endif            
        }
    }
    
    if(IsKeyPressed(KEY_G)){
        debugmode = !debugmode;
    }
    
    Vector3 camlook = Vector3Multiply(matlook(GetCameraMatrix(camera)),(Vector3){1,1,-1});
    Vector3 camrightvector = Vector3Normalize(Vector3CrossProduct(camlook,(Vector3){0,1,0}));
    if(debugmode){
        plrvel = (Vector3){0};
        plrg=false;
        skibidi = (RayCollision){0};
        Vector3 move = {0};
        Vector3 forwardmover = (Vector3){sin((camh*pi)/-180)*P_DEBUGSPEED,0,cos((camh*pi)/180)*P_DEBUGSPEED};
        if(IsKeyDown(KEY_W)){
            move = forwardmover;
        }
        if(IsKeyDown(KEY_S)){
            move = Vector3Subtract(move,forwardmover);
        }
        Vector3 sidemover = (Vector3){forwardmover.z,0,-forwardmover.x};
        if(IsKeyDown(KEY_A)){
            move = Vector3Add(move,sidemover);
        }
        if(IsKeyDown(KEY_D)){
            move = Vector3Subtract(move,sidemover);
        }
        move = Vector3Add(move,(Vector3){0,(IsKeyDown(KEY_SPACE)-IsKeyDown(KEY_LEFT_SHIFT))*P_DEBUGSPEED,0});
        plrpos = Vector3Add(plrpos,move);
    }else{
        int i;
        Vector3 forwardmover = (Vector3){sin((camh*pi)/-180),0,cos((camh*pi)/180)};
        Vector3 sidemover = (Vector3){-forwardmover.z,0,forwardmover.x};
        
        Vector3 orientrightvector = Vector3Normalize((Vector3){-plrorient.z,0,plrorient.x});
        
        Vector3 mpspar = Vector3Subtract(plrpos,(Vector3){0,2.5,0});
        if((!oplrg)&&plrg){
            particle(0,plrpound?8:3,true,mpspar,1);
            plrtimeland=plrlongjump?30:18;
            plrdjump=false;
            plrlongjump=false;
            plrattack=false;
            plrwallrun=false;
            plrrolling=false;
            plrflying=false;
            PlaySoundAtBeebo(s_land,1);
            walklerp=0;
            plrdjumptimer=0;
        }else if((!plrg)&&oplrg){
            plrswip=false;
            if(plrsliding&&!plrrolling){
                plrdjump=true;
                if(plrjumping==0&&oskibidi.normal.y<=.995){
                    plrvel=v2(plrvel);
                    plrvel=Vector3Add(plrvel,(Vector3){0,fmax(0,-Vector3DotProduct(plrpoint,Vector3Normalize(v2(oskibidi.normal)))*(Vector3Length(plrvel)/1.5)),0});
                }
            }
        }
        if(oplrg){
            plrpound=false;
        }
        oplrg = plrg;
        plrdir = (Vector3){0};
        if(canmove){
            bool kw=IsKeyDown(KEY_W);
            bool ks=IsKeyDown(KEY_S);
            bool ka=IsKeyDown(KEY_A);
            bool kd=IsKeyDown(KEY_D);
            if(abs(ka-kd)+abs(kw-ks)>0){
                if(kw){
                    plrdir = forwardmover;
                }
                if(ks){
                    plrdir = Vector3Subtract(plrdir,forwardmover);
                }
                if(ka){
                    plrdir = Vector3Subtract(plrdir,sidemover);
                }
                if(kd){
                    plrdir = Vector3Add(plrdir,sidemover);
                }
                plrdir = Vector3Normalize(plrdir);
#if defined(PLATFORM_WEB)
#else
            }else{
                Vector2 st1 = {GetGamepadAxisMovement(0,GAMEPAD_AXIS_LEFT_X),
                -GetGamepadAxisMovement(0,GAMEPAD_AXIS_LEFT_Y)};
                if(Vector2Length(st1)>.3){
                    if(Vector2Length(st1)>1){
                        st1=Vector2Normalize(st1);
                    }
                    plrdir=Vector3Scale(forwardmover,st1.y);
                    plrdir=Vector3Add(plrdir,Vector3Scale(sidemover,st1.x));
                }
#endif
            }
        }
        
        if(plrswip){
            plrpoint=plrswippoint;
            plrdir=(Vector3){0};
        }else if(plrg&&!plrswimming&&Vector3Length(plrdir)>.3&&Vector3DotProduct(plrdir,plrorient)<=-.5&&Vector3Length(v2(plrvel))>10&&walklerp>.7&&!plrdebounce){
            plrswip=true;
            plrswippoint=plrorient;
            swiptimer=18;
        }
        
        Vector3 pointrightvector = Vector3Normalize((Vector3){-plrpoint.z,0,plrpoint.x});
        bool plroldwall = plrwallrun;
        plrwallrun=false;
        if(!plrg&&!plrpole&&!plrswimming&&walltimer==0&&!plrdebounce){             //wallrun detect
            Vector3 side = plrpoint;
            float sidelen = 3;
            toilets = beebtrywall(side,sidelen);
            if(!plrwallrun){
                side = pointrightvector;
                sidelen = 2;
                toilets = beebtrywall(side,sidelen);
                if(!plrwallrun){
                    side = Vector3Scale(pointrightvector,-1);
                    toilets = beebtrywall(side,sidelen);
                }
            }
            if(plrwallrun&&fabs(plrwallnorm.y)<.2&&Vector3DotProduct(plrpoint,plrwallnorm)<.3){
                if(!plroldwall){
                    if(Vector3DotProduct(v2(plrvel),plrpoint)>0&&Vector3Length(plrdir)>.5){
                        Vector3 wallnormright = Vector3Normalize(Vector3CrossProduct(plrwallnorm,(Vector3){0,1,0}));
                        Vector3 tforce = Vector3Add(Vector3Scale(wallnormright,-Vector3DotProduct(plrwallnorm,pointrightvector)),
                        (Vector3){0,-Vector3DotProduct(plrwallnorm,plrpoint),0});
                        plrvel = Vector3Add(v2(plrvel),(Vector3){0,(10+(Vector3Normalize(tforce).y*30)),0});
                    }
                }
                plrlongjump=false;
                plrpound=false;
                plrattack=false;
                plrrolling=false;
                plrsliding=false;
                plrflying=false;
                if(ledgetimer2==0&&!plrledgegrab){
                    RayCollision init = {0};
                    RayCollision bees = {0};
                    bool inithit = false;
                    bool beeshit = false;
                    for(int i=0;i<gm3d.count;i++){
                        if(!gm3d.items[i].nowall&&!gm3d.items[i].nocol){
                            if(!inithit){
                                init = GetRayCollisionMesh((Ray){Vector3Add(plrpos,(Vector3){0,1,0}),side},gm3d.items[i].mdl.meshes[0],gm3d.items[i].mat);
                                if(init.hit&&init.distance<=sidelen){
                                    inithit=true;
                                }
                            }
                            if(!beeshit){
                                bees = GetRayCollisionMesh((Ray){Vector3Add(plrpos,Vector3Add((Vector3){0,1,0},Vector3Scale(side,sidelen))),(Vector3){0,-1,0}}
                                ,gm3d.items[i].mdl.meshes[0],gm3d.items[i].mat);
                                if(bees.hit&&bees.distance<=2){
                                    beeshit=true;
                                }
                            }
                            if(inithit&&beeshit){
                                break;
                            }
                        }
                    }
                    if((!inithit)&&beeshit&&bees.normal.y>.8&&(plrpos.y-shadowpos.y>5)){
                        plrvel = (Vector3){0};
                        plrledgegrab=true;
                        plrledgepoint=Vector3Add((Vector3){plrwallpoint.x,bees.point.y-0.4,plrwallpoint.z},plrwallnorm);
                        plrpoint=Vector3Normalize(Vector3Scale(v2(plrwallnorm),-1));
                    }
                }
            }else{
                plrwallrun=false;
            }
        }
        Vector3 cvu = Vector3Length(plrvel)>.2f?Vector3Normalize(plrvel):(Vector3){0};
        if(plrsliding){
            if(Vector3Length(v2(plrvel))>.2){
                if(Vector3DotProduct(plrpoint,Vector3Normalize(plrvel))<0){
                    plrpoint = qvecerp(plrpoint,Vector3Normalize(Vector3Subtract(Vector3Scale(plrdir,2),v2(plrvel))),dt*4);
                }else{
                    plrpoint = qvecerp(plrpoint,Vector3Normalize(Vector3Add(Vector3Scale(plrdir,2),v2(plrvel))),dt*4);
                }
            }
        }else if(plrswimming){
            if(Vector3Length(plrdir)>.2f){
                plrpoint = qvecerp(plrpoint,plrdir,dt*3);
            }
            plrattack=false;
        }else if(plrpole){
        }else if(Vector3Length(plrdir)>.2f&&(plrg&&plrtimeland<18)){
            if(plrcrouch){
                plrpoint = qvecerp(plrpoint,plrdir,dt*5);
            }else{
                plrpoint = plrdir;
                plrdancing=false;
            }
        }
        pointrightvector = Vector3Normalize((Vector3){-plrpoint.z,0,plrpoint.x});
        plrgyro = Vector3RotateByAxisAngle(plrpoint,(Vector3){0,1},Vector3DotProduct(plrpoint,plrorient)<-.96f?fallvr:0); //plrorient is the char
        //rotand botand potand (lerpmaxxing)
        rotand+=((
            plrwallrun ? fmax(-1.5f,fmin(1.5f,Vector3DotProduct(plrwallnorm,pointrightvector)*-6))
                : plrg ? fmax(-.8f,fmin(.8f,Vector3DotProduct(plrorient,pointrightvector)*Vector3Length(plrdir)*1.5f))-Vector3DotProduct(skibidi.normal,pointrightvector)
                : 0
            )-rotand) * dt*6;
        botand+=((
            plrledgegrab ? 0
                : (plrattack) ? 0 //add pole to the () when its implemented, put a OR between it
                //put skate here later
                : plrswimming ? -1.4+fmax(-.8,fmin(.8,plrvel.y/20))
                : plrflying ? plrflypitch-1.4
                : plrwallrun ? (1.5f-fabs(rotand))*fmax(0,Vector3DotProduct(cvu,(Vector3){0,1}))
                : plrsliding ? -1.3f+fmax(-.8f,fmin(.8f,plrg?-Vector3DotProduct(plrpoint,skibidi.normal):plrvel.y/40))
                : plrg ? (-Vector3Length(plrdir))/5-Vector3DotProduct(skibidi.normal,plrpoint)-(plrcrouch?1.2f:0)
                : plrrolling ? 0
                : fmax(-1,fmin(1,plrvel.y/40))
            )-botand) * dt*((plrlongjump&&!plrpound)?3:6);
        float complexcrap = floor(botand/(pi*2)+.5f);
        if(complexcrap!=flipo){
            flipo = complexcrap;
            PlaySoundAtBeebo(s_flip,.5);
        }
        if(plrattack&&!plrgotice){
            potand-=dt*(20+plrcrouch*10);
            if(potand<0){
                potand+=pi*2;
                PlaySoundAtBeebo(s_spin,.5);
            }
        }else{
            potand+=((
                plrflying ? fmax(-1,fmin(1,-Vector3DotProduct(plrorient,pointrightvector)*(Vector3Length(plrdir)*8)))
                : 0
                )-potand)*((plrflying)?(dt*3):(dt*8)); //replace 8 with ((skate or flying) and 3 or 8) when both of these are put
        }
        sliderp+=((plrsliding||plrswimming)-sliderp)*(dt*10); //add swimming with sliding using an OR when u do it
        wallerp+=(plrwallrun-wallerp)*(dt*10);
        fallerp+=(((plrg||plrwallrun||plrsliding||plrpole||plrswimming)?0:fallvr)-fallerp)*(dt*10); //add skate after swimming in here
        longerp+=(plrlongjump-longerp)*(dt*10);
        //roperp
        //holderp
        croucherp+=((plrcrouch&&plrg&&!plrsliding)-croucherp)*(dt*10);
        lederp+=(plrledgegrab-lederp)*(dt*10);
        polerp+=(plrpole-polerp)*(dt*10);
        swimerp+=(plrswimming-swimerp)*(dt*10);
        //skaterp
        //flamerp
        rollerp+=(plrrolling-rollerp)*(dt*10);
        boarderp=Vector3Add(boarderp,Vector3Scale(Vector3Subtract((Vector3){0,0,-Vector3DotProduct(skibidi.normal,pointrightvector)},boarderp),dt*6));
        aterp+=(plrattack-aterp)*(dt*10);
        dancerp+=(plrdancing-dancerp)*(dt*10);
        swiperp+=(plrswip-swiperp)*(dt*10);
        
        //ok were back now
        float force = 600;
        Vector3 damp = {.75f,1,.75f};
        speedy=1;
        if(plrsliding){
            float mg = plrg?((skibidi.normal.y>.995f)?0:1):1; //the 0 should be a 1 if on a forceslide terrain
            force=300*mg*fmax(.3f,1-fabs(Vector3DotProduct(plrdir,plrpoint)))*(Vector3Length(plrvel)/100);
            plrvel=Vector3Add(plrvel,Vector3Scale(v2(skibidi.normal),150*P_BFORCE));//300*P_BFORCE but its 100 instead cuz it feels too fast
            damp=Vector3Length(v2(plrvel))<50?((mg==0)?(Vector3){.94f,1,.94f}:(Vector3){.994f,1,.994f}):(Vector3){.994f,1,.994f};
            speedy=0;
            if((Vector3Length(plrvel)<10)&&(mg<1)&&plrg){plrsliding=false;}
        }else if(plrpole){
            speedy=1;
        }else if(plrattack){
            speedy=0;
            damp=(Vector3){.75f,plrjumping?1:.9f,.75f};
            if(plrcrouch&&plrjumping==0){
                plrvel=Vector3Subtract(plrvel,(Vector3){0,300*P_BFORCE});//600*P_BFORCE but its 300 instead cuz it feels too fast
            }
        }else if(plrswimming){
        }else if(plrlongjump){
            force=150;
            damp=(Vector3){.98f,1,.98f};
        }else if(plrwallrun){ //and not skate, add that when u dput skated
            speedy=fmax(0,fmin(1,Vector3DotProduct(matlook(vistorso),plrvel)/10));
        }else if(plrcrouch){
            force=300;
            speedy=.5f;
            plrdancing=false;
        }else if(plrdancing){
        }else if(!plrg){
            force=200;
            damp=(Vector3){.9f,1,.9f};
        }
        bool tojump = IsKeyDown(KEY_SPACE)
#if defined(PLATFORM_WEB)
#else
        ||IsGamepadButtonDown(0,GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
#endif
        ;
        bool todive = IsKeyDown(KEY_LEFT_SHIFT)
#if defined(PLATFORM_WEB)
#else
        ||IsGamepadButtonDown(0,GAMEPAD_BUTTON_RIGHT_TRIGGER_2)
#endif
        ;
        speedy*=plrwallrun?1:plrswimming?fmin(1,Vector3Length(plrdir)+abs(tojump-todive))*1.4:Vector3Length(plrdir);
        walklerp+=(speedy-walklerp)*dt*4;
        walktick+=dt*speedy*(1-fabs(fallerp));
        
        //shadow code (not realy right here, placed in end of drawbeeb)
        
        //sliding point step
        if(plrsliding&&plrg){
            Vector3 direction = Vector3Subtract(vistorsopos, camera.position);
            float distance = Vector3Length(direction);

            float attenuation = CalculateAttenuation(distance, 100, soundRolloffGlobal);
            SetMusicVolume(s_slide,attenuation);
        }else{
            SetMusicVolume(s_slide,.0f);
        }
        //SetMusicVolume(s_slide,sliderp*(1-swimerp)*fmax(0,fmin(1,shadowpos.y-plrpos.y+3.6)));  //originally vistorsopos.y, not plrpos.y, and it was 2.6
        
        complexcrap=sign(sin(walktick*14));
        if(complexcrap!=walkfx&&((!plrdjump&&!plrswimming)||plrwallrun||plrpole)){
            walkfx=complexcrap;
            if(plrpole){
                PlaySoundAtBeebo(s_pole,.5);
            }else{
                PlaySoundAtBeebo(stepsA[rand()%5],1);
            }
        }
        //wallrad step (if the level has one)
        if(iswallrad&&Vector3Length(v2(plrpos))>wallrad){
            plrpos = Vector3Add(Vector3Scale(Vector3Normalize(v2(plrpos)),wallrad),(Vector3){0,plrpos.y,0});
        }
        float imp = 0;
        float vy = 0;
        if(plrledgegrab||plrswimming){
            gravmult=0;
        }else if(plrflying){
            gravmult=0;
            float dip = (plrflyspeed<1)?(1-plrflyspeed):0;
            plrflypitch=fmin(1,fmax(-1,plrflypitch-(plrflypitch*(dt/2))-Vector3DotProduct(plrdir,camlook)/(1+dip*3)*(dt*1.3)-(plrflyspeed<1?dt:0)));
            plrflyspeed=fmax(0,plrflyspeed-(plrflypitch)*dt);
            plrpoint=rotvec(plrpoint,(Vector3){0,Vector3DotProduct(plrdir,camrightvector)*(dt*3.5),0});
        }else if(plrpole){
            gravmult=0;
            plrrolling=false;
            float crap = -Vector3DotProduct(plrdir,camlook);
            vy = (plrpos.y>plrpolepos.y+plrpolesiz.y/2)?fmin(0,crap)
                :(plrpos.y<plrpolepos.y-plrpolesiz.y/2+2)?fmax(0,crap)
                :crap;
            if(fabsf(vy)<.4){
                vy=0;
            }
            if(Vector3Length(plrdir)>.3){
                plrpoint=rotvec(plrpoint,(Vector3){0,vy==0?-(Vector3DotProduct(plrdir,camrightvector)*(dt*4)/pow(plrpolesiz.x,.5)):0,0});
            }else if(plrpolesiz.x<8){
                int sector = (int)floor(4*atan2f(-plrpoint.x,-plrpoint.z)/pi+.5)%8;
                plrpoint=matlook(MatrixRotateXYZ((Vector3){0,sector*(pi/4),0}));
            }
        }else if(plrwallrun){
            imp = (fabs(Vector3DotProduct(plrdir,plrpoint))>.9)?0:1;
            plrpoint = qvecerp(plrpoint,Vector3Normalize(Vector3Add(Vector3Scale(plrwallnorm,-2),Vector3Scale(plrdir,imp))),dt*3);
            gravmult=.5;
        }else{
            gravmult=1;
        }
        if(frame%2==0){
            if(iswallrad&&Vector3Length(v2(plrpos))>wallrad-20){
                Vector3 wallforce = Vector3Scale(Vector3Normalize(v2(plrpos)),(Vector3Length(v2(plrpos))-wallrad+20)*(.5)); //10 in place of .5 was OG
                plrvel=Vector3Subtract(plrvel,wallforce);
            }
            if(plrswimming){
                Vector3 bforce = Vector3Scale(plrdir,360*P_BFORCE);//120*P_BFORCE
                bforce = Vector3Add(bforce,(Vector3){0,(tojump-todive)*(360*P_BFORCE),0});//120*P_BFORCE
                plrvel=Vector3Add(plrvel,bforce);
                plrvel=Vector3Multiply(plrvel,(Vector3){.9,.9,.9}); //bdamp
            }else if(plrledgegrab){
                if(!plrdebounce){
                    Vector3 bforce = Vector3Scale(Vector3Subtract(plrledgepoint,plrpos),4800*P_BFORCE);//400
                    plrvel=Vector3Add(plrvel,bforce);
                    plrvel=Vector3Multiply(plrvel,(Vector3){.1,.1,.1}); //bdamp
                }
            }else if(plrflying){
                Vector3 tpot = matlook(MatrixRotateXYZ((Vector3){plrflypitch,0,0}));
                tpot = rotvec(tpot,(Vector3){0,atan2f(plrpoint.x,plrpoint.z)+pi,0});
                Vector3 bforce=Vector3Scale(tpot,(150*P_BFORCE)*(plrflyspeed>3?(plrflyspeed/3+2):plrflyspeed));//50*P_BFORCE
                plrvel=Vector3Add(plrvel,bforce);
                plrvel=Vector3Multiply(plrvel,(Vector3){.95,.95,.95}); //bdamp
            }else if(plrpole){
                Vector3 bforce = v2(Vector3Subtract(Vector3Subtract(plrpolepos,plrpos),Vector3Scale(plrpoint,plrpolesiz.x/2+1)));
                bforce = Vector3Add(Vector3Scale(bforce,100),(Vector3){0,vy*100,0});//500,600
                plrvel=Vector3Add(plrvel,bforce);
                plrvel=Vector3Multiply(plrvel,(Vector3){.1,.1,.1}); //bdamp
            }else if(plrwallrun){
                Vector3 bforce = Vector3Add(Vector3Scale(plrpoint,imp*400*P_BFORCE /*150*/),Vector3Scale(plrwallnorm,-100*P_BFORCE));
                plrvel=Vector3Add(plrvel,bforce);
                plrvel=Vector3Multiply(plrvel,(Vector3){.94,.96,.94}); //bdamp
            }else{
                Vector3 bforce = Vector3Scale(plrdir,force*P_BFORCE);//.02f//12//180
                if(plrg){
                    bforce = Vector3Add(bforce,(Vector3){0,((skibidi.point.y+(plrcrouch?(!plrsliding?1:2):2))-(plrpos.y-1))*(300*P_BFORCE),0});//300*P_BFORCE
                }else{
                    //when u add pound add this line
                    bforce = Vector3Add(bforce,(Vector3){0,plrpound?-30*P_BFORCE:0,0});
                }
                plrvel=Vector3Add(plrvel,bforce);
                plrvel=Vector3Multiply(plrvel,(Vector3){damp.x,plrg?.2f:damp.y,damp.z}); //bdamp
            }
        }
        //actions,like jumping
        if(canmove&&!plrswimming){
            if(IsKeyPressed(KEY_SPACE)
#if defined(PLATFORM_WEB)
#else
                ||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
#endif
            ){
                plrdancing=false;
                if(plrg){
                    particle(0,1,false,mpspar,1);
                    plrjumping=18;
                    if(plrswip){
                        plrjumping=30;
                        plrrolling=true;
                        plrlongjump=true;
                        plrpoint=Vector3Scale(plrswippoint,-1);
                        plrorient=plrpoint;
                        potand=pi;
                        botand=pi*4;
                        plrdjump=true;
                        plrvel=Vector3Add(Vector3Scale(plrpoint,10),(Vector3){0,60});
                        plrbackfliptimer=30;
                        plrswip=false;
                    }else if(plrsliding){ //slide jump
                        plrdjump=true;
                        plrslidejumptimer=18;
                        plrjumping=12;
                        plrrolling=true;
                        plrg=false;
                        plrvel=Vector3Add(v2(plrvel),(Vector3){0,20});
                        PlaySoundAtBeebo(s_jump2,1);
                        botand = pi*2;
                    }else if(plrcrouch){
                        plrlongjump = true;
                        plrdjump = true;
                        if(Vector3Length(plrdir)>.8f){ //longjump
                            PlaySoundAtBeebo(s_jump,1);
                            plrvel = Vector3Add(Vector3Scale(plrpoint,30),(Vector3){0,40});
                            botand = pi*4;
                        }else{ //backflip
                            plrjumping=30;
                            plrbackfliptimer=30;
                            plrrolling=true;
                            PlaySoundAtBeebo(s_jump2,1);
                            plrvel = Vector3Add(Vector3Scale(plrpoint,-10),(Vector3){0,60});
                            botand = -pi*4;
                        }
                    }else{ //normal jump
                        plrg=false;
                        plrvel=Vector3Add(v2(plrvel),(Vector3){0,40});
                        PlaySoundAtBeebo(s_jump,1);
                        fallvr*=-1;
                    }
                }else if(plrpole){
                    PlaySoundAtBeebo(s_jump2,1);
                    plrpoint=Vector3Scale(plrpoint,-1);
                    plrvel = Vector3Add(Vector3Scale(plrpoint,30),(Vector3){0,30});
                    plrpole=false;
                    plrdjump=false;
                    plrdebounce=true;
                    plrdebouncetimer=12;
                }else if(plrledgegrab){
                    PlaySoundAtBeebo(s_jump2,1);
                    plrwallrun=false;
                    plrledgegrab=false;
                    ledgetimer2=12;
                    walltimer=6;
                    if(Vector3DotProduct(plrdir,plrpoint)<-.5){
                        plrvel=Vector3Add(Vector3Add(v2(plrvel),(Vector3){0,20}),Vector3Scale(plrpoint,-50));
                        botand=pi*-2;
                        plrjumping=12;
                        plrdebounce=true;
                        plrdebouncetimer=12;
                    }else{
                        plrvel = Vector3Add((Vector3){0,30,0},Vector3Scale(pointrightvector,10));
                        rotand++;
                        ledgetimer=24;
                        plrjumping=24;
                        plrdebounce=true;
                        plrdebouncetimer=24;
                    }
                }else if(plrwallrun){
                    PlaySoundAtBeebo(s_jump2,1);
                    plrvel=Vector3Add(Vector3Add(v2(plrvel),(Vector3){0,30}),Vector3Scale(plrwallnorm,30));
                    plrpoint=Vector3Normalize(v2(Vector3Add(plrpoint,Vector3Scale(plrwallnorm,-2*Vector3DotProduct(plrpoint,plrwallnorm)))));
                    plrdjump=true;
                    plrwallrun=false;
                    plrjumping=12;
                    walltimer=6;
                }else if(!plrdjump){ //double jump
                    particle(0,1,true,mpspar,1);
                    botand-=2;
                    plrjumping=18;
                    plrdjump=true;
                    plrvel=Vector3Add(v2(plrvel),(Vector3){0,40});
                    PlaySoundAtBeebo(s_djump,1);
                    fallvr*=-1;
                    plrdjumptimer=27;
                    if(Vector3Length(plrdir)>.2f){
                        plrpoint = plrdir;
                    }
                }
            }
            if(todive&&(plrg||plrattack)){
                plrsliding=false;
                plrcrouch=true;
            }else{
                plrcrouch=false;
            }
            if(IsKeyPressed(KEY_LEFT_SHIFT)
#if defined(PLATFORM_WEB)
#else
                ||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_TRIGGER_2)
#endif
            ){
                if(plrledgegrab){
                    PlaySoundAtBeebo(s_jump2,1);
                    plrwallrun=false;
                    plrledgegrab=false;
                    plrvel=Vector3Add(Vector3Add(v2(plrvel),(Vector3){0,20}),Vector3Scale(plrpoint,-50));
                    botand=pi*-2;
                    plrjumping=12;
                    ledgetimer2=12;
                    walltimer=6;
                    plrdebounce=true;
                    plrdebouncetimer=12;
                }else if(!plrg&&!plrwallrun&&!plrattack&&!plrpound&&!plrswimming&&!plrpole){   //pound
                    plrpound=true;
                    plrrolling=true;
                    botand=pi*2;
                    plrdjump=true;
                    plrsliding=false;
                    plranchored=true;
                    plrlongjump=true;
                    plrattack=false;
                    poundtimer=24;
                    plrflying=false;
                }
            }
            if((IsKeyPressed(KEY_E)
#if defined(PLATFORM_WEB)
#else
                ||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_FACE_LEFT)
#endif
            )&&!plrattack&&!plrpound&&!plrpole){
                plrdancing=false;
                if(plrsliding){
                    if(plrg){ //slide jump
                        plrdjump=true;
                        plrslidejumptimer=18;
                        plrjumping=12;
                        plrrolling=true;
                        plrg=false;
                        plrvel=Vector3Add(v2(plrvel),(Vector3){0,20});
                        PlaySoundAtBeebo(s_jump2,1);
                        botand = pi*2;
                    }else if(plrflying&&!plrdebounce){
                        plrdebounce=true;
                        plrdebouncetimer=30;
                        potand+=(pi*2)*fallvr;
                    }
                }else if(plrledgegrab){
                    PlaySoundAtBeebo(s_jump2,1);
                    plrwallrun=false;
                    plrledgegrab=false;
                    ledgetimer2=12;
                    walltimer=6;
                    if(Vector3DotProduct(plrdir,plrpoint)<-.5){
                        plrvel=Vector3Add(Vector3Add(v2(plrvel),(Vector3){0,20}),Vector3Scale(plrpoint,-50));
                        botand=pi*-2;
                        plrjumping=12;
                        plrdebounce=true;
                        plrdebouncetimer=12;
                    }else{
                        plrvel = Vector3Add((Vector3){0,30,0},Vector3Scale(pointrightvector,10));
                        rotand++;
                        ledgetimer=24;
                        plrjumping=24;
                        plrdebounce=true;
                        plrdebouncetimer=24;
                    }
                }else if(plrwallrun){
                    PlaySoundAtBeebo(s_jump2,1);
                    plrvel=Vector3Add(Vector3Add(v2(plrvel),(Vector3){0,30}),Vector3Scale(plrwallnorm,30));
                    plrpoint=Vector3Normalize(v2(Vector3Add(plrpoint,Vector3Scale(plrwallnorm,-2*Vector3DotProduct(plrpoint,plrwallnorm)))));
                    plrdjump=true;
                    plrwallrun=false;
                    plrjumping=12;
                    walltimer=6;
                }else{ //dive
                    plrdjumptimer=0;
                    plrjumping=18;
                    plrlongjump=false;
                    plrsliding=true;
                    plrrolling=false;
                    if(plrhasfly&&!plrdjump){
                        plrflying = true;
                        plrflypitch=.5;
                        plrflyspeed=2;
                        plrvel = Vector3Add(Vector3Scale(plrpoint,50),(Vector3){0,plrg?40:20});
                        potand=(pi*2)*fallvr;
                    }else{
                        plrvel = Vector3Add(Vector3Scale(plrpoint,40),(Vector3){0,20}); //idk how to replicate cuz sometimes its 30 sometimes its not (in the og game)
                    }
                    plrdjump=true;
                    plrg=false;
                    PlaySoundAtBeebo(s_dive,1);
                    plranchored=false;
                }
            }
            if((IsKeyPressed(KEY_LEFT_CONTROL)
#if defined(PLATFORM_WEB)
#else
                ||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_LEFT_FACE_LEFT)
#endif
            )
                &&plrg&&Vector3Length(plrdir)<.3&&!plrpole&&!plrsliding&&!plrcrouch&&!plrattack){
                plrdancing=true;
            }
        }
        if(canmove){
            if(!plrwallrun&&(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)||
    #if defined(PLATFORM_WEB)
            IsKeyPressed(KEY_SLASH)||IsKeyPressed(KEY_ENTER)
    #else
            IsKeyPressed(KEY_RIGHT_SHIFT)||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)
    #endif
            )&&!plrattack&&!plrpound&&(plrjumping==0)&&!plrpole){
                plrdancing=false;
                attacktimer=13;
                plrjumping=12;
                plrattack=true;
                PlaySoundAtBeebo(s_smack,1.5);
                plrdjump=true;
                plrsliding=false;
                plrlongjump=false;
                plrrolling=false;
                plrflying=false;
                potand+=pi*2;
                if(plrswimming){
                    plrvel=Vector3Scale(plrvel,.5);
                }else{
                    plrvel=Vector3Add(Vector3Scale(v2(plrvel),.5f),(Vector3){0,20});
                }
            }
        }
        if(plrjumping>0){
            plrjumping--;
        }
        if(ledgetimer>0){
            ledgetimer--;
            if(ledgetimer==12){
                plrvel = Vector3Add(Vector3Add((Vector3){0,plrvel.y,0},Vector3Scale(plrwallnorm,-10)),Vector3Scale(pointrightvector,-7));
            }else if(ledgetimer==0){
                plrdjump=false;
            }
        }
        if(ledgetimer2>0){
            ledgetimer2--;
        }
        if(poundtimer>0){
            poundtimer--;
            if(poundtimer==0){
                plranchored=false;
                plrvel=(Vector3){0,-40,0};
            }
        }
        if(plrdjumptimer>0){
            plrdjumptimer--;
            if(plrdjumptimer%9==0){
                fallvr*=-1;
                particle(0,1,true,mpspar,1);
            }
        }
        if(attacktimer>0){
            attacktimer--;
        }
        if(plrhurt>0){
            plrhurt--;
        }
        if(plrslidejumptimer>0){
            plrslidejumptimer--;
            if(plrslidejumptimer==6){
                plrsliding=false;
                plrdjump=false;
            }else if(plrslidejumptimer==0){
                plrrolling=false;
            }
        }
        if(plrbackfliptimer>0){
            plrbackfliptimer--;
            if(plrbackfliptimer==0){
                plrrolling=false;
            }
        }
        if(plrtimeland>0){
            plrtimeland--;
        }
        if(walltimer>0){
            walltimer--;
        }
        if(swiptimer>0){
            swiptimer--;
            if(swiptimer==0){
                plrswip=false;
            }
        }
        if(icedtimer>0){
            icedtimer--;
            if(icedtimer==60){
                icedcream++; //replace this later
            }else if(icedtimer==0){
                plrgotice=false;
                plranchored=false;
                plrvel=(Vector3){0};
                canmove=true;
            }
        }
        if(watertimer>0){
            watertimer--;
        }
        if(plrdebouncetimer>0){
            plrdebouncetimer--;
            if(plrdebouncetimer==0){
                plrdebounce=false;
            }
        }
        //entity collision
        bool inwatr=false;
        if(entlist.count>0&&!trsing2){
            Vector3 mps = Vector3Subtract(plrpos,(Vector3){0,1,0});
            float bs = attacktimer>0?2.5:1;
            Vector3 bsvec = Vector3Scale((Vector3){1.6,2,1.6},bs);
            BoundingBox bsbox = (BoundingBox){
                Vector3Subtract(mps,bsvec),
                Vector3Add(mps,bsvec)
            };
            BoundingBox grbox = (BoundingBox){
                Vector3Add(plrpos,(Vector3){-.5,-P_TALL-.05,-.5}),
                Vector3Add(plrpos,(Vector3){.5,-P_TALL+.05,.5})
            };
            for(i=0;i<entlist.count;i++){
                Entity *v = &entlist.items[i];
                BoundingBox otherbox = (BoundingBox){
                    Vector3Subtract(v->pos,Vector3Scale(v->size,.5f)),
                    Vector3Add(v->pos,Vector3Scale(v->size,.5f))
                };
                bool ischbox = CheckCollisionBoxSphere(otherbox,plrpos,1.5f);
                bool isbsbox = rg3[v->type]?CheckCollisionBoxes(bsbox,otherbox):false;
                bool isgrbox = gr3[v->type]?CheckCollisionBoxes(grbox,otherbox):false;
                bool above = mps.y<v->pos.y+v->size.y/2;
                if((!v->disabled)&&(ischbox||isbsbox||isgrbox)){
                    switch(v->type){
                        case OTYPE_TELE:
                            if(!plrdebounce){
                                plrdebounce=true;
                                tomap = findvar(v->uid,V_TELE_TOMAP);
                                tomapx = findvar(v->uid,V_TELE_TOX);
                                tomapy = findvar(v->uid,V_TELE_TOY);
                                tomapz = findvar(v->uid,V_TELE_TOZ);
                                transition(true);
                            }
                            break;
                        case OTYPE_CAND:
                            addvar(v->uid,0,0.2);
                            v->disabled=true;
                            candy++;
                            break;
                        case OTYPE_CORK:
                            if((attacktimer>0&&isbsbox&&above)||(plrpound&&plrg&&isgrbox)){
                                PlaySound(s_break);
                                int z = findvar(v->uid,V_CORK_TERRAI);
                                gm3d.items[z].nocol=true;
                                gm3d.items[z].hide=true;
                                v->disabled=true;
                            }
                            break;
                        case OTYPE_ICED:
                            icedtimer=180;
                            PlaySound(s_gotice);
                            bgmvols[0]=0;bgmvols[2]=0;bgmvols[1]=0;bgmvols[3]=0;
                            v->disabled=true;
                            plrgotice=true;
                            plranchored=true;
                            plrpoint=Vector3Scale(forwardmover,-1);
                            plrorient=plrpoint;
                            icedrot=rand()%4;
                            canmove=false;
                            break;
                        case OTYPE_WATR:
                            if(isbsbox&&above){
                                inwatr=true;
                            }
                            break;
                        case OTYPE_POLE:
                            if(!plrpole&&!plrswimming&&!plrwallrun&&!plrdebounce){
                                plrpole=true;
                                plrpolepos=v->pos;
                                plrpolesiz=v->size;
                                plrsliding=false;
                                plrlongjump=false;
                                plrpound=false;
                                plrattack=false;
                                plrpoint=Vector3Normalize(
                                Vector3Multiply(
                                matlook(MatrixLookAt(v2(plrpos),v2(plrpolepos),(Vector3){0,1,0})),
                                (Vector3){-1,0,1}
                                )
                                );
                            }
                            break;
                    }
                }
            }
        }
        if(!plrswimming&&inwatr){
            plrswimming=true;
            watertimer=180;
            plrsliding=false;
            plrpound=false;
            plrlongjump=false;
            plrwallrun=false;
            plrledgegrab=false;
            plrrolling=false;
            plrdjump=true;
            plrflying=false;
        }else if(plrswimming&&!inwatr){
            plrswimming=false;
            watertimer=180;
            plrsliding=true;
            if(plrvel.y>0){
                plrvel.y*=1.5;
            }
        }
        if(!plranchored){
            //roblox collision step not really
            plrvel = Vector3Add(plrvel,(Vector3){0,-1.31f*gravmult,0});
            if(Vector3Length(plrvel)>300){
                plrvel = Vector3Scale(Vector3Normalize(plrvel),300);
            }
            for(i=0;i<4;i++){
                Vector3 slowplrvel = Vector3Scale(plrvel,.25f/60);
                plrpos = Vector3Add(plrpos,slowplrvel);
                scuffedrays colres = scuffedcol(plrpos,1.5f,Vector3Normalize(plrvel),i==3);
                if(colres.lowind!=-1){
                    plrvel = projv3(plrvel,colres.results[colres.lowind].normal);
                    plrpos = Vector3Subtract(colres.results[colres.lowind].point,Vector3Scale(colres.v3used,1.5f));
                }
            }
            //this is bodygyro movement
            plrorient = Vector3Normalize(Vector3Lerp(plrorient,plrgyro,dt*10));
        }
        //ground ray
        float raydist = 3.5;
        if(plrg){
            raydist += Vector3Length(v2(plrvel))/30;
        }
        oskibidi=skibidi;
        if(plrjumping==0&&!plrswimming&&!plrpole){
            skibidi = beebtryground((Vector3){0},raydist);
            if(!plrg){
                skibidi = beebtryground(Vector3Scale(plrorient,.9f),raydist);
                if(!plrg){
                    skibidi = beebtryground(Vector3Scale(plrorient,-.9f),raydist);
                    if(!plrg){
                        skibidi = beebtryground(Vector3Scale(orientrightvector,-.9f),raydist);
                        if(!plrg){
                            skibidi = beebtryground(Vector3Scale(orientrightvector,.9f),raydist);
                        }
                    }
                }
            }
        }else{
            plrg=false;
        }
        if(plrg){
            plrpos = Vector3Add(plrpos,(Vector3){0,-croucherp,0});
        }else{
            skibidi = (RayCollision){0};
        }
    }
    if(plrgotice){
        camera.target = Vector3Add(plrpos,(Vector3){0,2,0});
        camera.position = Vector3Add(camera.target,Vector3Scale(plrorient,20));
    }else{
        if(IsKeyPressed(KEY_Q)
#if defined(PLATFORM_WEB)
#else
            ||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_LEFT_TRIGGER_1)||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_TRIGGER_1)
#endif
        ||IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)){
            snapcam=true;
            snapto=Vector3Normalize((Vector3){-plrpoint.z,0,plrpoint.x});
            if(Vector3DotProduct(plrpoint,camlook)>=-.9){
                snapto=rotvec(snapto,(Vector3){0,.1,0});
            }
        }
        if(snapcam){
            yimh=Vector3DotProduct(camlook,snapto)*(.2*M_TODEG);
            if(fabs(Vector3DotProduct(camlook,snapto))<.01){snapcam=false;}
        }
        if(Vector2Length((Vector2){yimh,yimv})>.0001||stillcam||camzoom==.5){
            camh+=yimh;
            camv+=yimv;
            camh=mod(camh,360.0f);
            if(camv>1.5*M_TODEG){
                camv=1.5*M_TODEG;
            }else if(camv<-1.5*M_TODEG){
                camv=-1.5*M_TODEG;
            }
        }else{
            Vector3 test = Vector3Subtract(v2(plrpos),v2(camera.position));
            float thing = atan2f(test.x,test.z);
            camh = -thing*M_TODEG;
        }
        if(!(Vector2Length((Vector2){yimh,yimv})>.2||stillcam||camzoom==.5)){
            if(plrflying){
                camv+=(-(plrflypitch-.3)-(camv*M_TORAD))*(dt*M_TODEG);
            }else if(plrswimming){
                camv+=(-(botand+1)-(camv*M_TORAD))*(dt*M_TODEG);
            }else if(watertimer>0){
                camv+=(.4-(camv*M_TORAD))*(dt*M_TODEG);
            }
        }
        camera.target = plrpos;
        camera.position = Vector3Add(plrpos,(Vector3){0,0,-1.0f});
        UpdateCameraPro(&camera,(Vector3){0},(Vector3){camh,camv},1);
        camera.position = Vector3Subtract(camera.position,(Vector3){0,-1,-1});
        camera.target = Vector3Subtract(camera.target,(Vector3){0,-1,-1});
        if(camzoom==.5){
            camzoomlerp=0;
        }else{
            float camdist = camzoom*10;
            Vector3 camway = Vector3Subtract(camera.position,camera.target);
            bool hit = false;
            for(int i=0;i<gm3d.count;i++){
                if(!gm3d.items[i].nocol&&!gm3d.items[i].hide){
                    RayCollision test = GetRayCollisionMesh((Ray){camera.target,camway},gm3d.items[i].mdl.meshes[0],gm3d.items[i].mat);
                    test.distance=test.distance*2-3;
                    if((test.hit)&&(test.distance<camdist)){
                        camdist=test.distance;
                        hit = true;
                    }
                }
            }
            if(hit){
                camzoomlerp=camdist;
            }else{
                camzoomlerp+=fmin(dt*40,camdist-camzoomlerp);
            }
        }
        UpdateCameraPro(&camera,(Vector3){0},(Vector3){0},camzoomlerp);
    }
    camera.fovy += ((plrflying?(70+plrflyspeed*3):70)-camera.fovy)*dt;
    if(IsKeyPressed(KEY_H)){
        plrhasfly=!plrhasfly;
        plrflying=false;
        particle(0,8,true,vistorsopos,1);
    }
}

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int sw;
int sh;

int fullposx;
int fullposy;
int fullsizx;
int fullsizy;
bool fullscreen = false;
int main(){
    // Initialization
    //--------------------------------------------------------------------------------------
    //SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(960, 540, "Robot 64");
    rlEnableDepthTest();
    BeginDrawing();
    ClearBackground(BLACK);
    Image img = LoadImageFromMemory(".png",img_startup,sizeof(img_startup));
    Texture2D startuptex = LoadTextureFromImage(img);
    DrawTexture(startuptex,339,210,WHITE);
    EndDrawing();
    UnloadImage(img);
	SetWindowState(FLAG_WINDOW_RESIZABLE);
#if defined(PLATFORM_WEB)
    MaximizeWindow();
#endif
	InitAudioDevice();
    
    srand(time(NULL));
    
    //load fonts
    r64font = LoadFontEx("font/r64.ttf",128,NULL,0);
    SetTextureFilter(r64font.texture,TEXTURE_FILTER_BILINEAR);
    
    //load textures
	img = LoadImageFromMemory(".png",img_spinny,sizeof(img_spinny));
    spinny = LoadTextureFromImage(img);SetTextureFilter(spinny,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_trs,sizeof(img_trs));
    trstex = LoadTextureFromImage(img);SetTextureFilter(trstex,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_cur,sizeof(img_cur));
    curstx = LoadTextureFromImage(img);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_inv,sizeof(img_inv));
    invtex = LoadTextureFromImage(img);SetTextureFilter(invtex,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_candt,sizeof(tex_candt));
    ml_candy_tex = LoadTextureFromImage(img);SetTextureFilter(ml_candy_tex,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    //img = LoadImageFromMemory(".png",tex_plain,sizeof(tex_plain));
    //t_plain = LoadTextureFromImage(img);
    //UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_sparkle,sizeof(tex_sparkle));
    t_sparkle = LoadTextureFromImage(img);SetTextureFilter(t_sparkle,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_cork,sizeof(tex_cork));
    t_cork = LoadTextureFromImage(img);SetTextureFilter(t_cork,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_icedicon,sizeof(img_icedicon));
    icedicon = LoadTextureFromImage(img);SetTextureFilter(icedicon,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_candicon,sizeof(img_candicon));
    candicon = LoadTextureFromImage(img);SetTextureFilter(candicon,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_icedcream,sizeof(tex_icedcream));
    ml_icedcream_tex = LoadTextureFromImage(img);SetTextureFilter(ml_icedcream_tex,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_water,sizeof(tex_water));
    t_water = LoadTextureFromImage(img);
    SetTextureWrap(t_water,TEXTURE_WRAP_REPEAT);SetTextureFilter(t_water,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_sun,sizeof(tex_sun));
    t_sun = LoadTextureFromImage(img);SetTextureFilter(t_sun,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_frame,sizeof(img_frame));
    t_frame = LoadTextureFromImage(img);SetTextureFilter(t_frame,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_3dUI,sizeof(img_3dUI));
    t_3dUI = LoadTextureFromImage(img);SetTextureFilter(t_3dUI,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",img_stick2,sizeof(img_stick2));
    t_stick2 = LoadTextureFromImage(img);SetTextureFilter(t_stick2,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    
    loadskin(plrskin);
    img = LoadImageFromMemory(".png",tex_jetpack,sizeof(tex_jetpack));
    t_jetpack = LoadTextureFromImage(img);SetTextureFilter(t_jetpack,TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    
    img = LoadImageFromMemory(".png",tex_face1,sizeof(tex_face1));
    bt_faces[0] = LoadTextureFromImage(img);SetTextureFilter(bt_faces[0],TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_face2,sizeof(tex_face2));
    bt_faces[1] = LoadTextureFromImage(img);SetTextureFilter(bt_faces[1],TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_face3,sizeof(tex_face3));
    bt_faces[2] = LoadTextureFromImage(img);SetTextureFilter(bt_faces[2],TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_face4,sizeof(tex_face4));
    bt_faces[3] = LoadTextureFromImage(img);SetTextureFilter(bt_faces[3],TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    img = LoadImageFromMemory(".png",tex_face5,sizeof(tex_face5));
    bt_faces[4] = LoadTextureFromImage(img);SetTextureFilter(bt_faces[4],TEXTURE_FILTER_BILINEAR);
    UnloadImage(img);
    
    //load sounds
    Wave wav = LoadWaveFromMemory(".ogg",sfx_loadto,sizeof(sfx_loadto));
    s_load1 = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_loadfr,sizeof(sfx_loadfr));
    s_load2 = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_jump,sizeof(sfx_jump));
    s_jump = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_djump,sizeof(sfx_djump));
    s_djump = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_dive,sizeof(sfx_dive));
    s_dive = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_jump2,sizeof(sfx_jump2));
    s_jump2 = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_land,sizeof(sfx_land));
    s_land = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_smack,sizeof(sfx_smack));
    s_smack = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_spin,sizeof(sfx_spin));
    s_spin = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_flip,sizeof(sfx_flip));
    s_flip = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_candy,sizeof(sfx_candy));
    s_candy = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_break,sizeof(sfx_break));
    s_break = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_gotice,sizeof(sfx_gotice));
    s_gotice = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_pole,sizeof(sfx_pole));
    s_pole = LoadSoundFromWave(wav);
    UnloadWave(wav);
    
    wav = LoadWaveFromMemory(".ogg",sfx_sa1,sizeof(sfx_sa1));
    stepsA[0] = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_sa2,sizeof(sfx_sa2));
    stepsA[1] = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_sa3,sizeof(sfx_sa3));
    stepsA[2] = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_sa4,sizeof(sfx_sa4));
    stepsA[3] = LoadSoundFromWave(wav);
    UnloadWave(wav);
    wav = LoadWaveFromMemory(".ogg",sfx_sa5,sizeof(sfx_sa5));
    stepsA[4] = LoadSoundFromWave(wav);
    UnloadWave(wav);
    //this is a music stream because its looped and always playing
    s_slide = LoadMusicStreamFromMemory(".ogg",sfx_slide,sizeof(sfx_slide));
	s_slide.looping = true;
    PlayMusicStream(s_slide);
    SetMusicVolume(s_slide,.0f);
    
    //sounds that have different volumes
    SetSoundVolume(s_smack,1.5f);
    SetSoundVolume(s_spin,.5f);
    SetSoundVolume(s_flip,.5f);
    SetSoundVolume(s_break,1.5f);
    
    //load shaders
    shader = LoadShader(TextFormat("shaders/glsl%i/angle.vs", GLSL_VERSION),
                        TextFormat("shaders/glsl%i/angle.fs", GLSL_VERSION));
    SHldl = GetShaderLocation(shader,"lightDir");
    SHam1l = GetShaderLocation(shader,"ambientR");
    SHam2l = GetShaderLocation(shader,"ambientG");
    SHam3l = GetShaderLocation(shader,"ambientB");
    SHbrl = GetShaderLocation(shader,"bright");
    {
        float SHld[3] = {1.0f,1.0f,.5f};
        SetShaderValue(shader,SHldl,SHld,SHADER_UNIFORM_VEC3);
        SetShaderValue(shader,SHam1l,(float[1]){((float)159)/255},SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader,SHam2l,(float[1]){((float)154)/255},SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader,SHam3l,(float[1]){((float)135)/255},SHADER_UNIFORM_FLOAT);
        SetShaderValue(shader,SHbrl,(float[1]){compbrightness(1)},SHADER_UNIFORM_FLOAT);
    }
    
    //load models
    prepmodel(glblist[5],glbsize[5]);
    b_torso = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_torso.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = curskin;
    b_torso.materials[1].shader = shader;
    prepmodel(glblist[6],glbsize[6]);
    b_head = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_head.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = curskin;
    b_head.materials[1].shader = shader;
    prepmodel(glblist[7],glbsize[7]);
    b_face = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_face.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = bt_faces[0];
    b_face.materials[1].shader = shader;
    prepmodel(glblist[8],glbsize[8]);
    b_leg = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_leg.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = curskin;
    b_leg.materials[1].shader = shader;
    prepmodel(glblist[9],glbsize[9]);
    b_tarm = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_tarm.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = curskin;
    b_tarm.materials[1].shader = shader;
    prepmodel(glblist[10],glbsize[10]);
    b_barm = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_barm.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = curskin;
    b_barm.materials[1].shader = shader;
    prepmodel(glblist[11],glbsize[11]);
    b_dot = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    //b_dot.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = t_plain;
    b_dot.materials[1].maps[MATERIAL_MAP_DIFFUSE].color = (Color){124,255,129,255};
    b_dot.materials[1].shader = shader;
    prepmodel(glblist[35],glbsize[35]);
    b_jetpack = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_jetpack.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = t_jetpack;
    b_jetpack.materials[1].shader = shader;
    prepmodel(glblist[36],glbsize[36]);
    b_booster = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_booster.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = t_jetpack;
    b_booster.materials[1].shader = shader;
    prepmodel(glblist[37],glbsize[37]);
    b_pack = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    b_pack.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = curskin;
    b_pack.materials[1].shader = shader;
    
    
    prepmodel(glblist[33],glbsize[33]);
    curhat = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    curhat.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = curskin;
    curhat.materials[1].shader = shader;
    
    
    prepmodel(glblist[12],glbsize[12]);
    ml_candy = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    ml_candy.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = ml_candy_tex;
    ml_candy.materials[1].shader = shader;
    prepmodel(glblist[31],glbsize[31]);
    ml_cloud = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    ml_cloud.materials[1].shader = shader;
    prepmodel(glblist[32],glbsize[32]);
    ml_icedcream = LoadModel("tuffness.glb");
    SetLoadFileDataCallback(NULL);
    ml_icedcream.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = ml_icedcream_tex;
    ml_icedcream.materials[1].shader = shader;
    
    p_break = LoadModelFromMesh(GenMeshCube(8,8,8));
    p_break.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = t_cork;
    p_sun = LoadModelFromMesh(GenMeshPlaneT(1,1,1,1,1,-1));
    p_sun.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = t_sun;
	
    camera.position = (Vector3){0,0,-10};
    camera.target = (Vector3){0};
    camera.up = (Vector3){0,1,0};
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    loadskybox();
    map_title();
    SetExitKey(KEY_NULL);
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(dotheframecrap, 60, 1);
#else
    HideCursor();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        dotheframecrap();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadShader(shader);
    CloseWindow();                  // Close window and OpenGL context
#endif
    //--------------------------------------------------------------------------------------

    return 0;
}
int8_t blink = 10;
int8_t candoffset = 0;
Rectangle pauserec = {0};
static void dotheframecrap(){
    sw = GetScreenWidth();
    sh = GetScreenHeight();
#if defined(PLATFORM_WEB)
    if(IsWindowResized()){
        SetWindowSize(GetScreenWidth(),GetScreenHeight());
    }
#else
    if(IsKeyPressed(KEY_F11)){
        if(fullscreen){
            SetWindowPosition(fullposx,fullposy);
            SetWindowSize(fullsizx,fullsizy);
            ToggleBorderlessWindowed();
        }else{
            Vector2 wp = GetWindowPosition();
            fullposx = wp.x;
            fullposy = wp.y;
            fullsizx = sw;
            fullsizy = sh;
            ToggleBorderlessWindowed();
            SetWindowFocused();
        }
        sw = GetScreenWidth();
        sh = GetScreenHeight();
    }
#endif
    Vector2 m = GetMousePosition();
    if((IsKeyPressed(KEY_ESCAPE)||IsKeyPressed(KEY_P)||IsKeyPressed(KEY_GRAVE)
    ||(!paused&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&&CheckCollisionPointRec((Vector2){m.x,m.y},pauserec))    
    )&&usechar){
        if(paused){
            ResumeMusicStream(s_slide);
        }else{
            if(oldrightcursor){
                EnableCursor();
#if defined(PLATFORM_WEB)
#else
                HideCursor();
#endif
                SetMousePosition(mx,my);
            }
            oldrightcursor=false;
            PauseMusicStream(s_slide);
        }
        paused = !paused;
    }
    if(!oldrightcursor){
        mx = m.x;
        my = m.y;
    }
    //step frame
    if(paused){
        songplay=7;
    }else{
        gtick+=dt;
        g06next+=dt;
        frame+=1;
        if(usechar&&!loading){
            if(IsKeyPressed(KEY_N)){
                mouselock=!mouselock;
            }
            int wheel = GetMouseWheelMoveV().y;
            if(wheel!=0){
                float ocam=camzoom;
                camzoom=fmax(.5,fmin(5,camzoom+wheel*-.5));
                camzoomlerp+=(camzoom-ocam)*10;
            }
            stepchar();
        }
        if(g06next>=.06){
            if(plrswip||(plrsliding&&plrg)){
                particle(0,1,false,Vector3Subtract(plrpos,(Vector3){0,2.5,0}),1);
            }else if(plrflying){
                Vector3 pos=Vector3Transform((Vector3){0},
                    MatrixMultiply(matrel(vistorsorot,(Vector3){bus%2==0?-.3:.3,-.9,-.2},vistorsorot),
                    MatrixTranslate(visjetpos.x,visjetpos.y,visjetpos.z))
                );
                particle(0,1,false,pos,.4);
            }else if((1-walklerp)*wallerp*(1-lederp)*(1-skaterp)>.6){
                particle(0,1,false,Vector3Transform((Vector3){0},matrel(vistorso,(Vector3){0,0,-1},vistorsorot)),1);
            }
            bus++;
            g06next-=.06;
            blink--;
            if(blink<0){
                blink=20+(rand()%4+1)*10;
            }
            if(plrdancing||plrgotice){
                faceid=1;
            }else if(plrhurt>0){
                faceid=2;
            }else if(blink==2){
                faceid=3;
            }else if(blink==1){
                faceid=4;
            }else if(blink==0){
                faceid=3;
            }else{
                faceid=0;
            }
            b_face.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = bt_faces[faceid];
            if(bus%2==0){
                if(scandy<candy){
                    scandy++;
                    PlaySound(s_candy);
                    candoffset=-1;
                }else if(scandy>candy){
                    scandy--;
                    candoffset=1;
                }else{
                    candoffset=0;
                }
            }else{
                candoffset=0;
            }
        }
        songplay=
        (plrgotice||trsing2)?67
        :plrflying&&canbgmA?1
        :plrswimming&&canbgmW?2
        :0;
    }
    if(map==M_TITLE){
        for(int i=0;i<9;i++){
            bgmvols[i]=i==0?1:0;
        }
    }else{
        for(int i=0;i<9;i++){
            bgmvols[i]+=((songplay==i?(
                (songplay==7)?.8
                :1
            ):0)-bgmvols[i])*(dt*2);
        }
    }
    SetMusicVolume(bgm,bgmvols[0]);UpdateMusicStream(bgm);
    if(canbgmA){
        SetMusicVolume(bgmA,bgmvols[1]);UpdateMusicStream(bgmA);
    }
    if(canbgmW){
        SetMusicVolume(bgmW,bgmvols[2]);UpdateMusicStream(bgmW);
    }
    SetMusicVolume(bgmP,bgmvols[7]);UpdateMusicStream(bgmP);

    // Calculate doppler for all positional sounds
    float ft = GetFrameTime();
    if (ft <= 0.0f) return;
    Vector3 camVel = Vector3Scale(Vector3Subtract(camera.position, lastcampos), 1.0f / ft);
    for (int i = 0; i < activeSounds.count; i++) {
        struct SoundInstance *s = &activeSounds.instances[i];
        Vector3 sourceVel = Vector3Scale(Vector3Subtract(s->position, s->lastPosition), 1.0f / ft);
        Vector3 toSource = Vector3Subtract(s->position, camera.position);
        float dist = Vector3Length(toSource);
        if (dist > 0.1f) {
            Vector3 dir = Vector3Scale(toSource, 1.0f / dist);
            float listenerV = Vector3DotProduct(camVel, dir);
            float sourceV = Vector3DotProduct(sourceVel, Vector3Negate(dir));
            float relativeVelocity = listenerV + sourceV;
            float c = 343.0f / DOPPLER_SCALE;
            float pitch = Clamp((c + relativeVelocity) / c, 0.5f, 2.0f);
            s->pitchModifier = pitch;
            SetSoundPitch(s->sound, pitch);
        }
        s->lastPosition = s->position;
    }

    UpdateMusicStream(s_slide);
    UpdateDrawFrame();
    plroldpos = plrpos;
    lastcampos = camera.position;
}
#define inset 36
static void UpdateDrawFrame(void){
    BeginDrawing();
        
        ClearBackground(BLANK);

        BeginMode3D(camera);
            
            //skybox
            rlDisableBackfaceCulling();
            rlDisableDepthMask();
            DrawMesh(skybox.meshes[0],skybox.materials[0],MatrixIdentity());
            rlEnableBackfaceCulling();
            if(!loading){
                Vector3 gop = Vector3Add(camera.position,sunpos);
                Vector3 v2sunpos = v2(sunpos);
                Matrix sunlook1 = QuaternionToMatrix(QuaternionFromVector3ToVector3((Vector3){0,-1,0},(Vector3){0,sunpos.y,Vector3Length(v2sunpos)}));
                Matrix sunlook2 = QuaternionToMatrix(QuaternionFromVector3ToVector3((Vector3){0,0,1},v2sunpos));
                Matrix sunlook = MatrixMultiply(sunlook1,sunlook2);
                DrawMesh(p_sun.meshes[0],p_sun.materials[0],MatrixMultiply(
                    MatrixMultiply(MatrixScale(.21,.21,.21),sunlook),
                    MatrixTranslate(gop.x,gop.y,gop.z)
                ));
            }
			rlEnableDepthMask();
            
            int i;
            for(i=0;i<gm3d.count;i++){
                Terrain *v = &gm3d.items[i];
                if(!v->hide){
                    DrawMesh(v->mdl.meshes[0],v->mdl.materials[v->gen?0:1],v->mat);
                }
            }
            if(usechar){
                //DrawSphere(plrpos,1.5f,WHITE);
                //draw beebo
                drawbeeb();
                DrawCube(shadowpos,1,.5,1,BLACK);                   //UNFINISHED
                for(i=0;i<PAR_MAX;i++){
                    Par *v = &particles.items[i];
                    if(v->scale>0){
                        DrawMesh(ml_cloud.meshes[0],ml_cloud.materials[1],MatrixMultiply(MatrixScale(v->scale,v->scale,v->scale),
                        MatrixMultiply(v->rot,MatrixTranslate(v->pos.x,v->pos.y,v->pos.z))));
                        v->pos=Vector3Add(v->pos,Vector3Scale(v->speed,dt));
                        v->scale-=dt/10;
                    }
                }
            }
            if(entlist.count>0){
                Matrix candycf = MatrixMultiply(MatrixRotateXYZ((Vector3){sin(gtick*5)/3,gtick*5,cos(gtick*5)/3}),MatrixTranslate(0,sin(gtick*2)/2,0));
                for(i=0;i<entlist.count;i++){
                    Entity *v = &entlist.items[i];
                    if(v->disabled){    // if disabled
                        switch(v->type){
                            case OTYPE_CAND:
                                FoundVar timeleft = findvarF(v->uid,0);
                                if(timeleft.val>0){
                                    Color c = {0};
                                    if(timeleft.val>0.1){
                                        c = (Color){255,255,255,(int)floor(127.5+637.5*timeleft.val)};
                                    }else{
                                        c = (Color){255,255,255,(int)floor(1912.5*timeleft.val)};
                                    }
                                    tex3d(t_sparkle,v->pos,(3.55-11.75*timeleft.val)*2,c);
                                    entlistV.vals[timeleft.ind]-=dt;
                                }
                                break;
                        }
                    }else{                            // if enabled
                        switch(v->type){
                            case OTYPE_TELE:
                                DrawCubeV(v->pos,v->size,BLACK);
                                break;
                            case OTYPE_CAND:
                                DrawMesh(ml_candy.meshes[0],ml_candy.materials[1],
                                MatrixMultiply(
                                    MatrixScale(.03f,.03f,.03f),
                                    MatrixMultiply(
                                        candycf,
                                        MatrixTranslate(v->pos.x,v->pos.y,v->pos.z)
                                    )
                                ));
                                break;
                            case OTYPE_ICED:
                                DrawMesh(ml_icedcream.meshes[0],ml_icedcream.materials[1],
                                MatrixMultiply(
                                    MatrixScale(.0309,.03088978424,.0308824803),
                                    MatrixMultiply(
                                        candycf,
                                        MatrixTranslate(v->pos.x,v->pos.y,v->pos.z)
                                    )
                                ));
                                break;
                            case OTYPE_WATR:
                                rlDisableBackfaceCulling();
                                Matrix mat = MatrixTranslate(
                                    v->pos.x+sin(gtick/4)*10,
                                    v->pos.y+v->size.y/2,
                                    v->pos.z+cos(gtick/4)*10
                                );
                                DrawMesh(v->mdl.meshes[0],v->mdl.materials[0],mat);
                                rlEnableBackfaceCulling();
                                break;
                            case OTYPE_POLE:
                                Vector3 thing=(Vector3){0,v->size.y/2,0};
                                DrawCylinderEx(Vector3Subtract(v->pos,thing),Vector3Add(v->pos,thing),2,2,8,GREEN);
                                break;
                        }
                    }
                }
            }
            /*for(int8_t x=-1;x<2;x++){               //hitbox vizualizer
                for(int8_t y=-1;y<2;y++){
                    for(int8_t z=-1;z<2;z++){
                        if(!((x==0)&&(y==0)&&(z==0))){
                            Vector3 dir = Vector3Normalize((Vector3){x,y,z});
                            DrawLine3D(plrpos,Vector3Add(plrpos,Vector3Scale(dir,1.5)),BLACK);
                        }
                    }
                }
            }*/

        EndMode3D();
        
        float sw2 = sw/2;
        float sh2 = sh/2;
        if(map==M_TITLE){
            Vector2 m = GetMousePosition();
            bool h = false;
            if(m.x>sw*.35f&&m.x<sw*.65f){
                if(m.y>sh*.7f&&m.y<sh*.76f){
                    titleselt=0;
                    h=true;
                }else if(m.y>sh*.8f&&m.y<sh*.86f){
                    titleselt=1;
                    h=true;
                }else if(m.y>sh*.9f&&m.y<sh*.96f){
                    titleselt=2;
                    h=true;
                }
            }
            if(((IsMouseButtonDown(MOUSE_BUTTON_LEFT)&&h)||IsKeyPressed(KEY_SPACE)
#if defined(PLATFORM_WEB)
#else
                ||IsGamepadButtonPressed(0,GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
#endif
            )&&(!trsing)){
                tomap = titleselt>0?M_TUTORIAL:M_HUB;
                if(tomap==M_TUTORIAL){
                    tomapy=6;
                    tomapz=-8;
                }else{
                    tomapx=-12.433;
                    tomapy=15.02;
                    tomapz=56.217;
                }
                transition(true);
            }
            r64text("Continue",sw2,sh*.7f,sh*0.06f,.5f,0,WHITE);
            r64text("New Game",sw2,sh*.8f,sh*0.06f,.5f,0,WHITE);
            r64text("Speedrun",sw2,sh*.9f,sh*0.06f,.5f,0,WHITE);
            float rot = mod(GetTime()*80.0f,360);
            Vector2 off = Vector2Scale(fixrotpos2(rot),sh*-0.03f);
            DrawTextureEx(spinny,Vector2Add((Vector2){(sw*.35f)+(sh*0.03f),(sh*.73f)+(titleselt*(sh*.1f))},off),rot,0.0078125f*(sh*0.06f),WHITE);
        }else{
            int ish = sh-inset;
            float uisize = (ish*.23f)+50;
            float xoffset = (sw-170)-uisize;
            float invx = xoffset+(uisize*.4f);
            float invw = (uisize*.6f);
            float invh = (uisize*.20625);
            float invs = invw/256;
            DrawTextureEx(invtex,(Vector2){invx,inset},0,invs,WHITE);
            DrawTextureEx(invtex,(Vector2){invx,inset+(uisize*.2f)},0,invs,WHITE);
            float numx = invx+(invw*.353);
            float numy = inset+(invh*.06);
            float nums = invh*.8;
            r64text(TextFormat("%i",icedcream),numx,numy,nums,0,0,WHITE);
            r64text(TextFormat("%i",scandy),numx,numy+(uisize*.2f),nums,0,0,WHITE);
            float icox = invx+(invw*0.045);
            float icoy = inset+(invh*.1);
            float icos = nums/256;
            DrawTextureEx(icedicon,(Vector2){icox,icoy},0,icos,WHITE);
            DrawTextureEx(candicon,(Vector2){icox,icoy+(uisize*.2f)+(invh*(candoffset*.1))},0,icos,WHITE);
            
            float pbx = xoffset+(uisize*.1);
            float pby = inset+(uisize*.068);
            float pbsize = uisize*.25;
            float pbgsize = pbsize*1.3;
            float pbgx = pbx+pbsize/2-pbgsize/2;
            float pbgy = pby+pbsize/2-pbgsize/2;
            DrawTextureEx(t_stick2,(Vector2){pbgx,pbgy},0,pbgsize/256,(Color){138,138,138,255});
            DrawTexturePro(t_3dUI,(Rectangle){384,256,128,128},(Rectangle){pbx,pby,pbsize,pbsize},(Vector2){0},0,WHITE);
            pauserec=(Rectangle){pbx,pby,pbsize,pbsize};
        }
        if(trsing){
            float t = GetTime()-trstart;
            if(t<.4f){
                t=t/.4f;
                if(trsto){
                    t=1.0f-t;
                }
                t*=2.0f;
                int touse;
                if(sw>sh){
                    touse=sw;
                }else{
                    touse=sh;
                }
                float s = t*touse;
                float size = s/512.0f;
                Vector2 pos = (Vector2){sw2-s/2,sh2-s/2};
                DrawTextureEx(trstex,pos,0,size,WHITE);
                DrawRectangleV((Vector2){0},(Vector2){pos.x,sh},BLACK);
                DrawRectangleV((Vector2){pos.x+s},(Vector2){sw,sh},BLACK);
                DrawRectangleV((Vector2){0},(Vector2){sw,pos.y},BLACK);
                DrawRectangleV((Vector2){0,pos.y+s},(Vector2){sw,sh},BLACK);
            }else if(trsto){
                DrawRectangleV((Vector2){0},(Vector2){sw,sh},BLACK);
                if(loadstate==0){
                    StopMusicStream(bgm);
                    loadstate=1;
                    usechar=false;
                }else if(loadstate==1){
                    if(t>1){
                        loading=true;
                        loadstate=2;
                    }
                }else if(loadstate==2){
                    usechar=true;
                    tomapid(tomap);
                    transition(false);
                    plrdebounce=false;
                }
            }else{
                trsing = false;
            }
        }
        if(paused){
            DrawRectangleV((Vector2){0},(Vector2){sw,sh},(Color){17,17,17,77});
            int ish = sh-inset;
            float pausesh = ish*1.1;
            float framesize = pausesh*0.76;
            float framex = (sw2)-framesize/2;
            float framey = (pausesh/2)-(framesize/2)-ish*.1+inset;
            DrawTextureEx(t_frame,(Vector2){framex,framey},0,framesize/512,WHITE);
            float sframesize = pausesh*0.8;
            float sframex = (sw2)-sframesize/2;
            float sframey = (pausesh/2)-(sframesize/2)-ish*.1+inset;
            float spinysize = sframesize*0.1;
            
            Vector2 m = GetMousePosition();
            float srot = mod(GetTime()*80.0f,360);
            Vector2 soff = Vector2Scale(fixrotpos2(srot),spinysize*-.5);
            switch(pausemenu){
                case 0:
                    float relmouse = m.y-(framey+(framesize*.24));
                    int mousehover = floor(relmouse/(framesize*.1));
                    bool ishovering = mod(relmouse,framesize*.1)<=framesize*.09
                    &&mousehover>-1&&mousehover<7
                    &&m.x>=framex&&m.x<=framex+framesize;
                    if(ishovering){
                        pauseselt=mousehover;
                    }
                    float hovery = sframey+(sframesize*(.24+pauseselt/10.0))+sframesize*.045;
                    spinyy += (hovery-spinyy)*(dt*20);
                    float hoverx = sframex+sframesize*.1+spinysize*.5;
                    spinyx += (hoverx-spinyx)*(dt*20);
                    DrawTextureEx(spinny,Vector2Add((Vector2){spinyx,spinyy},soff),srot,0.0078125f*spinysize,WHITE);
                    
                    r64text("Paused",sw2,framey+(framesize*.047),framesize*.12,.5,0,WHITE);
                    
                    float btsize = framesize*.09;
                    float btposx = framex+framesize*.8;
                    r64text("Resume",sw2,framey+(framesize*.24),btsize,.5,0,WHITE); //id 0
                    DrawTexturePro(t_3dUI,(Rectangle){128,0,128,128},(Rectangle){btposx,framey+(framesize*.24),btsize,btsize},(Vector2){0},0,WHITE);
                    r64text("Icecreams",sw2,framey+(framesize*.34),btsize,.5,0,WHITE);
                    DrawTexturePro(t_3dUI,(Rectangle){256,256,128,128},(Rectangle){btposx,framey+(framesize*.34),btsize,btsize},(Vector2){0},0,WHITE);
                    r64text("Snapshot Mode",sw2,framey+(framesize*.44),btsize,.5,0,WHITE);
                    DrawTexturePro(t_3dUI,(Rectangle){384,128,128,128},(Rectangle){btposx,framey+(framesize*.44),btsize,btsize},(Vector2){0},0,WHITE);
                    r64text("Clothing",sw2,framey+(framesize*.54),btsize,.5,0,WHITE);
                    DrawTexturePro(t_3dUI,(Rectangle){384,0,128,128},(Rectangle){btposx,framey+(framesize*.54),btsize,btsize},(Vector2){0},0,WHITE);
                    r64text("Follow Camera",sw2,framey+(framesize*.64),btsize,.5,0,WHITE);
                    DrawTexturePro(t_3dUI,(Rectangle){stillcam?128:0,128,128,128},(Rectangle){btposx,framey+(framesize*.64),btsize,btsize},(Vector2){0},0,WHITE);
                    r64text("Back to Hub",sw2,framey+(framesize*.74),btsize,.5,0,WHITE);
                    DrawTexturePro(t_3dUI,(Rectangle){256,0,128,128},(Rectangle){btposx,framey+(framesize*.74),btsize,btsize},(Vector2){0},0,WHITE);
                    r64text("Quit to Title",sw2,framey+(framesize*.84),btsize,.5,0,WHITE);
                    DrawTexturePro(t_3dUI,(Rectangle){256,128,128,128},(Rectangle){btposx,framey+(framesize*.84),btsize,btsize},(Vector2){0},0,WHITE);
                    
                    
                    r64text("Sensitivity:",sw2,sh*0.9,sh*0.04f,.5f,.5f,WHITE);
                    DrawRectangleV((Vector2){sw*.4,sh*.95},(Vector2){sw*.2,sh*.02},WHITE);
                    DrawRectangleV((Vector2){sw*.395+sensitivity*sw*.2,sh*.94},(Vector2){sw*.01,sh*.04},GREEN);
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        if(ishovering){
                            switch(pauseselt){
                                case 0:
                                    ResumeMusicStream(s_slide);
                                    paused = !paused;
                                    break;
                                case 4:
                                    stillcam=!stillcam;
                                    break;
                            }
                        }
                    }
                    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)&&m.y>sh*.93&&m.y<sh*.99){
                        sensitivity = fmax(0,fmin(1,(m.x/sw)*5-(1/.5)));
                        sensitivity = round(sensitivity*8)/8;
                    }
                    break;
            }
        }
        if(plrgotice){
            int ish = sh-inset;
            float ictexty = inset+ish*0.7;
            float ictexts = ish*.08;
            r64text("YOU GOT ICE CREAM!",sw/2,ictexty,ictexts,.5,0,WHITE);
            r64text("Watch for rolling rocks 0x",sw/2,ictexty+ictexts,ictexts*.6,.5,0,WHITE);
        }
        
        
        //DrawRectangleV((Vector2){0},(Vector2){sw,inset},(Color){31,31,31,127}); //roblox topbar replica
        //DONT PUT ANYTHING AFTER THIS YOU LITTLE DIDDYBLUD, this is the cursor draw
        if(!mouselock||paused){
            DrawTextureEx(curstx,(Vector2){mx-1,my-1},0,1,WHITE);
        }
        
        //except for debugging stuff because that oesnt really matter
        // DEBUGGGING TEXTs
        r64text(TextFormat("robot 64 raylib v9\nterrain count: %i\nentity count: %i",gm3d.count,entlist.count),20,20,20,0,0,WHITE);
        r64text(TextFormat("campos: %.2f, %.2f, %.2f",camera.position.x,camera.position.y,camera.position.z),20,100,20,0,0,WHITE);
        //r64text(TextFormat("lastcampos: %.2f, %.2f, %.2f",lastcampos.x,lastcampos.y,lastcampos.z),20,110,20,0,0,WHITE);
        //for (i=0;i<activeSounds.count;i++){
        //    struct SoundInstance *s = &activeSounds.instances[i];
        //    r64text(TextFormat("sound %i: pos(%.2f, %.2f, %.2f) doppler: %.2f",i,s->position.x,s->position.y,s->position.z,1-s->pitchModifier),20,140+(i*30),20,0,0,WHITE);
        //}

    EndDrawing();
}