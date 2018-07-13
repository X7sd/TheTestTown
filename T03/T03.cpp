#pragma once
#include <Application.hpp>
#include <DataManager.hpp>
#include <PixelWorld.hpp>
#include <WorldMap.hpp>
#include <PixelObject.hpp>
#include <Input.hpp>
#include <Timer.hpp>
#include <Animation.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <random>
#define random(a,b) (rand()%(b-a+1)+a)

struct MapNPC
{
	int id, x, y;
};

struct MapDoor
{
	int x, y, tm, tx, ty;
};

struct MapInfo
{
	std::vector<MapNPC*> NPCSet;
	std::vector<MapDoor*> DoorSet;
	PixelWorldEngine::WorldMap* map;
};

struct Entity
{
	PixelWorldEngine::PixelObject* entity;
	int fw;
};

//int mapM = 500, mapN = 500;
int vN = 10, vM = 6;
float size = 64;

std::vector<MapInfo*> mapinfo;
std::vector<PixelWorldEngine::PixelObject*> entity;
PixelWorldEngine::Application app = PixelWorldEngine::Application("The Town");
PixelWorldEngine::DataManager data = PixelWorldEngine::DataManager(&app);
PixelWorldEngine::PixelWorld print = PixelWorldEngine::PixelWorld("Print", &app);
//PixelWorldEngine::WorldMap map = PixelWorldEngine::WorldMap("Map", mapM, mapN);
PixelWorldEngine::Camera cam = PixelWorldEngine::Camera(PixelWorldEngine::RectangleF(0, 0, vN*size, vM*size));
PixelWorldEngine::Graphics::Texture2D* tt;
PixelWorldEngine::Animator animator = PixelWorldEngine::Animator("A01");
int nowmap = 0, nowchara = 0;
int isBlank[1000];

auto IntToString(int Int) -> std::string
{
	std::string result = "";

	while (Int > 0) {
		char c = (Int % 10) + '0';
		result = c + result;

		Int /= 10;
	}

	return result;
}

float distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

void OnKeyEvent(void* sender, PixelWorldEngine::Events::KeyClickEvent* eventArg)
{

}

void OnUpdate(void* sender)
{
	PixelWorldEngine::Application* app = (PixelWorldEngine::Application*)sender;

	float deltaTime = app->GetDeltaTime();
	app->SetWindow((std::string)"Test Fps:" + IntToString(app->GetFramePerSecond()), vN * size, vM * size);
	float speed = 100 * deltaTime;
	bool isKeyDown = false;

	glm::vec2 transform(0, 0);

	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Left))
		transform.x -= speed, isKeyDown = true;
	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Right))
		transform.x += speed, isKeyDown = true;
	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Down))
		transform.y += speed, isKeyDown = true;
	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Up))
		transform.y -= speed, isKeyDown = true;

	if (isKeyDown == true)
	{
		entity[0]->Move(transform.x, transform.y);
		cam.SetFocus(entity[0]->GetPositionX(), entity[0]->GetPositionY(), PixelWorldEngine::RectangleF(vN*size/2, vM*size/2, vN*size / 2, vM*size / 2));
		for (int i = 0; i < mapinfo[nowmap]->DoorSet.size(); i++)
		{
			if (distance(entity[0]->GetPositionX(),entity[0]->GetPositionY(),mapinfo[nowmap]->DoorSet[i]->x*size+size/2,mapinfo[nowmap]->DoorSet[i]->y*size+size/2)<size)
			{
				entity[0]->SetPosition(mapinfo[nowmap]->DoorSet[i]->tx*size + size / 2, mapinfo[nowmap]->DoorSet[i]->ty*size + size / 2);
				nowmap = mapinfo[nowmap]->DoorSet[i]->tm;
				print.SetWorldMap(mapinfo[nowmap]->map);
			}
		}
		
	}
}

void ReadMap()
{
	std::ifstream fin("MapInfo.txt");
	int mapN;
	fin >> mapN;
	for (int i = 0; i < mapN; i++)
	{
		mapinfo.push_back(new MapInfo);
		std::string ts;
		fin >> ts;
		std::ifstream mapin(ts);
		mapin >> ts;
		int maxn, maxm;
		mapin >> maxn >> maxm;
		mapinfo[i]->map=new PixelWorldEngine::WorldMap(ts, maxn, maxm);
		mapinfo[i]->map->SetMapBlockSize(size);
		for (int y = 0; y < maxn; y++)
		{
			for (int x = 0; x < maxm; x++)
			{
				int t;
				mapin >> t;
				auto temp = new PixelWorldEngine::MapData();
				temp->RenderObjectID[0] = t;
				if (isBlank[t]==1)
				{
					temp->MoveEnable = true;
				}
				else
				{
					temp->MoveEnable = false;
				}
				temp->Opacity = 1;
				mapinfo[i]->map->SetMapData(x, y, temp);
			}
		}
		int n1, n2;
		mapin >> n1;
		for (int p = 0; p < n1; p++)
		{
			int id, x, y;
			mapin >> id >> x >> y;
			mapinfo[i]->NPCSet.push_back(new MapNPC);
			mapinfo[i]->NPCSet[p]->id = id;
			mapinfo[i]->NPCSet[p]->x = x;
			mapinfo[i]->NPCSet[p]->y = y;
		}
		mapin >> n2;
		for (int p = 0; p < n2; p++)
		{
			int x, y, tm, tx, ty;
			mapin >> x >> y >> tm >> tx >> ty;
			mapinfo[i]->DoorSet.push_back(new MapDoor);
			mapinfo[i]->DoorSet[p]->x = x;
			mapinfo[i]->DoorSet[p]->y = y;
			mapinfo[i]->DoorSet[p]->tm = tm;
			mapinfo[i]->DoorSet[p]->tx = tx;
			mapinfo[i]->DoorSet[p]->ty = ty;
		}
		mapin.close();
		print.RegisterWorldMap(mapinfo[i]->map);
	}
	fin.close();
}

void ReadTexture()
{
	std::ifstream fin("TextureInfo.txt");
	int textureN;
	fin >> textureN;
	for (int i = 1; i <= textureN; i++)
	{
		std::string ts;
		fin >> ts;
		tt = data.RegisterTexture(ts);
		print.RegisterRenderObjectID(i, tt);
		fin >> isBlank[i];
	}
/**	while (!EOF)
	{

	}*/
	fin.close();
}

void ReadEntity()
{
	std::ifstream fin("EntityInfo.txt");
	int entityN;
	fin >> entityN;
	for (int i = 0; i < entityN; i++)
	{
		std::string ts;
		int rid;
		fin >> ts;
		fin >> rid;
		entity.push_back(new PixelWorldEngine::PixelObject(ts));
		entity[i]->SetRenderObjectID(rid);
		entity[i]->SetSize(size, size);
	}
	fin.close();
}

void ReadGameInfo()
{
	std::ifstream fin("GameInfo.txt");
	fin >> nowchara;
	fin >> nowmap;
}

int main()
{
	ReadTexture();
	ReadEntity();
	ReadMap();
	ReadGameInfo();
//	PixelWorldEngine::Animation anime = PixelWorldEngine::Animation("TA");

	print.RegisterWorldMap(mapinfo[nowmap]->map);
	print.SetWorldMap(mapinfo[0]->map);
	print.SetResolution(vN*size, vM*size);
	print.SetCamera(&cam);

	print.RegisterPixelObject(entity[0]);
	entity[0]->SetPosition(size / 2, size / 2);
	
	app.KeyClick.push_back(OnKeyEvent);
	app.Update.push_back(OnUpdate);

	app.MakeWindow("Test", vN*size, vM*size);
	app.SetWorld(&print);
	app.ShowWindow();

	print.SetWorldMap(mapinfo[nowmap]->map);
	cam.SetFocus(entity[0]->GetPositionX(), entity[0]->GetPositionY(), PixelWorldEngine::RectangleF(vN*size / 2, vM*size / 2, vN*size / 2, vM*size / 2));
	app.RunLoop();
}