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
	int id, x, y, fw;
};

struct MapDoor
{
	int x, y, tm, sx, sy, tx, ty;
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
	int fw, anifw;
	int fn;
	int rid[9][4];
	PixelWorldEngine::Animation* anime;
	PixelWorldEngine::Animation* chmove;
};

int vN = 10, vM = 6;
float size = 64;

int startM, startX, startY, startFW;
bool inScript, inAnime;
std::vector<MapInfo*> mapinfo;
std::vector<Entity*> object;
PixelWorldEngine::Application app = PixelWorldEngine::Application("The Town");
PixelWorldEngine::DataManager data = PixelWorldEngine::DataManager(&app);
PixelWorldEngine::PixelWorld print = PixelWorldEngine::PixelWorld("Print", &app);
PixelWorldEngine::Camera cam = PixelWorldEngine::Camera(PixelWorldEngine::RectangleF(0, 0, vN*size, vM*size));
PixelWorldEngine::Graphics::Texture2D* tt;
std::vector<PixelWorldEngine::Graphics::Texture2D*> textures;
PixelWorldEngine::Animator animator = PixelWorldEngine::Animator("A01"), animator2 = PixelWorldEngine::Animator("A02");
int nowmap = 0, nowchara = 0;
int isBlank[20000];

auto IntToString(int Int) -> std::string
{
	std::string result = "";

	while (Int > 0)
	{
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

void UnloadMap(int nowmap)
{
	for (int i = 0; i < mapinfo[nowmap]->NPCSet.size(); i++)
	{
	/**	object[mapinfo[nowmap]->NPCSet[i]->id]->entity->SetPosition(mapinfo[nowmap]->NPCSet[i]->y*size - size / 2, mapinfo[nowmap]->NPCSet[i]->x*size - size / 2);
		object[mapinfo[nowmap]->NPCSet[i]->id]->fw = mapinfo[nowmap]->NPCSet[i]->fw;
		object[mapinfo[nowmap]->NPCSet[i]->id]->entity->SetRenderObjectID(object[mapinfo[nowmap]->NPCSet[i]->id]->rid[mapinfo[nowmap]->NPCSet[i]->fw][0]); */
		print.UnRegisterPixelObject(object[mapinfo[nowmap]->NPCSet[i]->id]->entity);
	}
}

void LoadMap(int nowmap)
{
	object[nowchara]->entity->SetDepthLayer(3);
	print.SetWorldMap(mapinfo[nowmap]->map);
	for (int i = 0; i < mapinfo[nowmap]->NPCSet.size(); i++)
	{
		object[mapinfo[nowmap]->NPCSet[i]->id]->entity->SetPosition(mapinfo[nowmap]->NPCSet[i]->y*size - size / 2, mapinfo[nowmap]->NPCSet[i]->x*size - size / 2);
		object[mapinfo[nowmap]->NPCSet[i]->id]->fw = mapinfo[nowmap]->NPCSet[i]->fw;
		object[mapinfo[nowmap]->NPCSet[i]->id]->entity->SetRenderObjectID(object[mapinfo[nowmap]->NPCSet[i]->id]->rid[mapinfo[nowmap]->NPCSet[i]->fw][0]);
		object[mapinfo[nowmap]->NPCSet[i]->id]->entity->SetDepthLayer(2);
		print.RegisterPixelObject(object[mapinfo[nowmap]->NPCSet[i]->id]->entity);
	}
}

void SetRenderObjectID(void* Which, void* Data)
{
	auto which = (PixelWorldEngine::PixelObject*)Which;
	auto data = *(int*)Data;

	which->SetRenderObjectID(data);
}

bool StopWalking(int tch)
{
	if (!animator.IsRun())
	{
		return true;
	}
	animator.Stop();
	app.UnRegisterAnimator(&animator);
	object[tch]->anifw = 0;
	return false;
}

bool Walking(int tch)
{
	if (animator.IsRun())
	{
		if (object[tch]->anifw == object[tch]->fw)
		{
			return true;
		}
		else
		{
			StopWalking(tch);
		}
	}
	object[tch]->anifw = object[tch]->fw;
//	object[tch]->anime = new PixelWorldEngine::Animation("Walk" + object[tch]->entity->GetName());
	object[tch]->anime->SetKeyFrame(object[tch]->rid[object[tch]->fw][1], 0);
	object[tch]->anime->SetKeyFrame(object[tch]->rid[object[tch]->fw][2], 0.2);
	object[tch]->anime->SetKeyFrame(object[tch]->rid[object[tch]->fw][3], 0.4);
	object[tch]->anime->SetKeyFrame(object[tch]->rid[object[tch]->fw][0], 0.6);
	object[tch]->anime->SetKeyFrame(object[tch]->rid[object[tch]->fw][1], 0.8);
	object[tch]->anime->Sort();
	animator.AddAnimation(object[tch]->entity, SetRenderObjectID, object[tch]->anime, 0);
	app.RegisterAnimator(&animator);
	animator.EnableRepeat(true);
	animator.Run();
	std::cout << "Animation Run at " << object[tch]->anifw << " direction.Key Frames registerd sign " << object[tch]->anime->GetEndTime() << "s." << std::endl;
	return false;
}

void ObjectMove(void* Which, void* Data)
{
	auto which = (PixelWorldEngine::PixelObject*)Which;
	auto data = *(glm::vec2*)Data;

	which->SetPosition(data.x, data.y);
}

int Redirection(int x1, int y1, int x2, int y2)
{
	if (x1 == x2)
	{
		if (y1 < y2)
		{
			return 3;
		}
		else
		{
			if (y1 > y2)
			{
				return 1;
			}
		}
	}
	else
	{
		if (y1 == y2)
		{
			if (x1 < x2)
			{
				return 4;
			}
			else
			{
				if (x1 > x2)
				{
					return 2;
				}
			}
		}
		else
		{
			if (x1 < x2)
			{
				if (y1 < y2)
				{
					return 7;
				}
				else
				{
					if (y1 > y2)
					{
						return 8;
					}
				}
			}
			else
			{
				if (x1 > x2)
				{
					if (y1 < y2)
					{
						return 6;
					}
					else
					{
						if (y1 > y2)
						{
							return 5;
						}
					}
				}
			}
		}
	}
}

PixelWorldEngine::KeyFrame InMovingPos(float timePos, PixelWorldEngine::KeyFrame lastFrame, PixelWorldEngine::KeyFrame nextFrame)
{
	glm::vec2 tmove1 = lastFrame.GetData<glm::vec2>();
	glm::vec2 tmove2 = nextFrame.GetData<glm::vec2>();
	glm::vec2 midpos;
	PixelWorldEngine::KeyFrame midFrame =lastFrame;
	midpos.x = tmove1.x + (tmove2.x - tmove1.x) * (timePos - lastFrame.GetTimePos()) / (nextFrame.GetTimePos() - lastFrame.GetTimePos());
	midpos.y = tmove1.y + (tmove2.y - tmove1.y) * (timePos - lastFrame.GetTimePos()) / (nextFrame.GetTimePos() - lastFrame.GetTimePos());
	midFrame.SetData(midpos);
	midFrame.SetTimePos(timePos);
	return midFrame;
}

bool EndEnter(int tch)
{
	if (!inAnime)
	{
		return true;
	}
	StopWalking(tch);
	animator2.Stop();
	app.UnRegisterAnimator(&animator2);
	inAnime = false;
	return false;
}

bool Entering(int tch, int x1, int y1, int x2, int y2)
{
	if (inAnime)
	{
		if (animator2.IsRun())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	inAnime = true;
	object[tch]->fw = Redirection(y1, x1, y2, x2);
	glm::vec2 tmove1, tmove2;
	tmove1.x = y1 * size + size / 2;
	tmove1.y = x1 * size + size / 2;
	tmove2.x = y2 * size + size / 2;
	tmove2.y = x2 * size + size / 2;
	object[tch]->chmove->SetKeyFrame(tmove1, 0);
	object[tch]->chmove->SetKeyFrame(tmove2, 3.2);
	object[tch]->chmove->Sort();
	object[tch]->chmove->SetFrameProcessUnit(InMovingPos);
	
	animator2.AddAnimation(object[tch]->entity, ObjectMove, object[tch]->chmove, 0);
	app.RegisterAnimator(&animator2);
	animator2.EnableRepeat(false);
	animator2.Run();

	Walking(tch);

	return false;
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

	if (inAnime)
	{
		if (!animator2.IsRun())
		{
			EndEnter(nowchara);
		}
		return;
	}

	glm::vec2 transform(0, 0);

	int tfw = 0;
	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Left))
	{
		transform.x -= speed;
		isKeyDown = true;
		tfw = 2;
	}
	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Right))
	{
		transform.x += speed;
		isKeyDown = true;
		tfw = 4;
	}
	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Down))
	{
		transform.y += speed;
		isKeyDown = true;
		if (object[nowchara]->fn>=8)
		{
			if (tfw == 2)
			{
				tfw = 6;
			}
			else
			{
				if (tfw == 4)
				{
					tfw = 7;
				}
				else
				{
					tfw = 3;
				}
			}
		}
		else
		{
			tfw = 3;
		}
	}
	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Up))
	{
		transform.y -= speed;
		isKeyDown = true;
		if (object[nowchara]->fn >= 8)
		{
			if (tfw == 2)
			{
				tfw = 5;
			}
			else
			{
				if (tfw == 4)
				{
					tfw = 8;
				}
				else
				{
					tfw = 1;
				}
			}
		}
		else
		{
			tfw = 1;
		}
	}

	if ((PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::Q)) && (!inScript))
	{
		for (int i = 0; i < mapinfo[nowmap]->NPCSet.size(); i++)
		{
			if (distance(object[nowchara]->entity->GetPositionX(), object[nowchara]->entity->GetPositionY(), (mapinfo[nowmap]->NPCSet[i]->y - 1)*size + size / 2, (mapinfo[nowmap]->NPCSet[i]->x - 1)*size + size / 2) < size / 2)
			{
				std::cout << "NPC #" << mapinfo[nowmap]->NPCSet[i]->id << " script activates." << std::endl;
				inScript = true;
			}
		}
	}
	if ((PixelWorldEngine::Input::GetKeyCodeUp(PixelWorldEngine::KeyCode::Q)) && (inScript))
	{
		inScript = false;
	}

	if (PixelWorldEngine::Input::GetKeyCodeDown(PixelWorldEngine::KeyCode::P))
	{
		std::cout << object[nowchara]->entity->GetPositionX() << "," << object[nowchara]->entity->GetPositionY() << std::endl;
		std::cout << mapinfo[nowmap]->map->GetMapData((object[nowchara]->entity->GetPositionX() - size / 2) / size, (object[nowchara]->entity->GetPositionY() - size / 2) / size)->MoveEnable << std::endl;
		std::cout << mapinfo[nowmap]->map->GetWorldMapData((object[nowchara]->entity->GetPositionX() - size / 2) / size, (object[nowchara]->entity->GetPositionY() - size / 2) / size)->MoveEnable << std::endl;
	}

	if (isKeyDown == true)
	{
		object[nowchara]->fw = tfw;
		object[nowchara]->entity->SetRenderObjectID(object[nowchara]->rid[tfw][0]);
		object[nowchara]->entity->Move(transform.x, transform.y);
		Walking(nowchara);
		cam.SetFocus(object[nowchara]->entity->GetPositionX(), object[nowchara]->entity->GetPositionY(), PixelWorldEngine::RectangleF(vN*size/2, vM*size/2, vN*size / 2, vM*size / 2));
		for (int i = 0; i < mapinfo[nowmap]->DoorSet.size(); i++)
		{
			if (distance(object[nowchara]->entity->GetPositionX(), object[nowchara]->entity->GetPositionY(), mapinfo[nowmap]->DoorSet[i]->y*size + size / 2, mapinfo[nowmap]->DoorSet[i]->x*size + size / 2) < size / 2)
			{
				object[nowchara]->entity->SetPosition(mapinfo[nowmap]->DoorSet[i]->ty*size + size / 2, mapinfo[nowmap]->DoorSet[i]->tx*size + size / 2);
				UnloadMap(nowmap);
				nowmap = mapinfo[nowmap]->DoorSet[i]->tm;
				LoadMap(nowmap);

				Entering(nowchara, mapinfo[nowmap]->DoorSet[i]->sx, mapinfo[nowmap]->DoorSet[i]->sy, mapinfo[nowmap]->DoorSet[i]->tx, mapinfo[nowmap]->DoorSet[i]->ty);
			}
		}
	}
	else
	{
		StopWalking(nowchara);
		object[nowchara]->entity->SetRenderObjectID(object[nowchara]->rid[object[nowchara]->fw][0]);
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
		mapin >> maxm >> maxn;
		mapinfo[i]->map=new PixelWorldEngine::WorldMap(ts, maxn, maxm);
		mapinfo[i]->map->SetMapBlockSize(size);
		for (int y = 0; y < maxm; y++)
		{
			for (int x = 0; x < maxn; x++)
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
			int id, x, y, fw;
			mapin >> id >> x >> y >> fw;
			mapinfo[i]->NPCSet.push_back(new MapNPC);
			mapinfo[i]->NPCSet[p]->id = id;
			mapinfo[i]->NPCSet[p]->x = x;
			mapinfo[i]->NPCSet[p]->y = y;
			mapinfo[i]->NPCSet[p]->fw = fw;
		}
		mapin >> n2;
		for (int p = 0; p < n2; p++)
		{
			int x, y, sx, sy, tm, tx, ty;
			mapin >> x >> y >> tm >> sx >> sy >> tx >> ty;
			mapinfo[i]->DoorSet.push_back(new MapDoor);
			mapinfo[i]->DoorSet[p]->x = x;
			mapinfo[i]->DoorSet[p]->y = y;
			mapinfo[i]->DoorSet[p]->sx = sx;
			mapinfo[i]->DoorSet[p]->sy = sy;
			mapinfo[i]->DoorSet[p]->tm = tm;
			mapinfo[i]->DoorSet[p]->tx = tx;
			mapinfo[i]->DoorSet[p]->ty = ty;
		}
		mapin.close();
		print.RegisterWorldMap(mapinfo[i]->map);

		std::cout << mapinfo[i]->map->GetMapName() << std::endl;
		for (int y = 0; y < maxm; y++)
		{
			for (int x = 0; x < maxn; x++)
			{
				std::cout << *mapinfo[i]->map->GetMapData(x, y)->RenderObjectID << " ";
			}
			std::cout << std::endl;
		}
		for (int y = 0; y < maxm; y++)
		{
			for (int x = 0; x < maxn; x++)
			{
				std::cout << (mapinfo[i]->map->GetMapData(x, y)->MoveEnable == 1);
			}
			std::cout << std::endl;
		}
	}
	fin.close();
}

void ReadTexture()
{
	std::ifstream fin("TextureInfo.txt");
/**	int textureN;
	fin >> textureN;
	for (int i = 1; i <= textureN; i++)
	{
		std::string ts;
		fin >> ts;
		tt = data.RegisterTexture(ts);
		print.RegisterRenderObjectID(i, tt);
		fin >> isBlank[i];
	}*/
	while (!fin.eof())
	{
		std::string ts;
		int id;
		fin >> ts;
		if (ts == "")
		{
			continue;
		}
		fin >> id;
		fin >> isBlank[id];
		if ((ts.find("<")!=std::string::npos) && (ts.find(">")!=std::string::npos))
		{
			std::string dex, path;
			dex.assign(ts, ts.find("<") + 1, ts.find(">") - ts.find("<") - 1);
			path.assign(ts, ts.find(">")+1);
			std::vector<int*> d;
			std::string tnum = "";
			dex += ",";
			for (int i = 0; i < dex.length(); i++)
			{
				if (dex[i] == ',')
				{
					int tn = 0;
					if (tnum[0] == '-')
					{
						for (int j = 1; j < tnum.length(); j++)
						{
							tn = tn * 10 + (tnum[j] - '0');
						}
						tn = -tn;
					}
					else
					{
						for (int j = 0; j < tnum.length(); j++)
						{
							tn = tn * 10 + (tnum[j] - '0');
						}
					}
					d.push_back(new int(tn));
					tnum = "";
				}
				else
				{
					if (dex[i] != ' ')
					{
						tnum += dex[i];
					}
				}
			}
			if (d.size() == 3)
			{
				tt = data.RegisterTexture(path);
				textures.push_back(new PixelWorldEngine::Graphics::Texture2D(tt, PixelWorldEngine::Rectangle(*d[0] * (*d[2] - 1), *d[0] * (*d[1] - 1), *d[0] * *d[2], *d[0] * *d[1])));
				print.RegisterRenderObjectID(id, textures[textures.size() - 1]);
			}
			else
			{
				if (d.size() == 4)
				{
					tt = data.RegisterTexture(path);
					textures.push_back(new PixelWorldEngine::Graphics::Texture2D(tt, PixelWorldEngine::Rectangle(*d[1], *d[0], *d[3], *d[2])));
					print.RegisterRenderObjectID(id, textures[textures.size() - 1]);
				}
				else
				{
					if (d.size() == 5)
					{
						tt = data.RegisterTexture(path);
						textures.push_back(new PixelWorldEngine::Graphics::Texture2D(tt, PixelWorldEngine::Rectangle(*d[0] * (*d[4] - 1) + *d[2], *d[0] * (*d[3] - 1) + *d[1], *d[0] * *d[4] + *d[2], *d[0] * *d[3] + *d[1])));
						print.RegisterRenderObjectID(id, textures[textures.size() - 1]);
					}
				}
			}
		}
		else
		{
			tt = data.RegisterTexture(ts);
			print.RegisterRenderObjectID(id, tt);
		}
	}
	fin.close();
}

void ReadEntity()
{
	std::ifstream fin("EntityInfo.txt");
	int entityN;
	fin >> entityN;
	for (int i = 0; i < entityN; i++)
	{
		object.push_back(new Entity);
		std::string ts;
		fin >> ts;
		int tn;
		fin >> tn;
		object[i]->fn = tn;
		if (tn = 8)
		{
			for (int f = 1; f <= 8; f++)
			{
				for (int stp = 0; stp < 4; stp++)
				{
					int t;
					fin >> t;
					object[i]->rid[f][stp] = t;
				}
			}
		}
		else
		{
			if (tn = 9)
			{
				for (int f = 0; f <= 8; f++)
				{
					for (int stp = 0; stp < 4; stp++)
					{
						int t;
						fin >> t;
						object[i]->rid[f][stp] = t;
					}
				}
			}
			else
			{
				if (tn = 5)
				{
					for (int f = 0; f <= 4; f++)
					{
						for (int stp = 0; stp < 4; stp++)
						{
							int t;
							fin >> t;
							object[i]->rid[f][stp] = t;
						}
					}
				}
				else
				{
					for (int f = 1; f <= 4; f++)
					{
						for (int stp = 0; stp < 4; stp++)
						{
							int t;
							fin >> t;
							object[i]->rid[f][stp] = t;
						}
					}
				}
			}
		}
		object[i]->entity = new PixelWorldEngine::PixelObject(ts);
		object[i]->entity->SetSize(size - 1, size - 1);
		object[i]->anime = new PixelWorldEngine::Animation("Walk" + object[i]->entity->GetName());
		object[i]->chmove = new PixelWorldEngine::Animation("Move" + object[i]->entity->GetName());
		
	//	animator.AddAnimation(object[i]->entity, SetRenderObjectID, object[i]->anime, 0);
	}
	
	fin.close();
}

void ReadGameInfo()
{
	std::ifstream fin("GameInfo.txt");
	fin >> nowchara;
	fin >> startM;
	fin >> startX >> startY >> startFW;
}

int main()
{
	ReadTexture();
	ReadEntity();
	ReadMap();
	ReadGameInfo();
	
	nowmap = startM;

	print.RegisterWorldMap(mapinfo[nowmap]->map);
	print.SetWorldMap(mapinfo[0]->map);
	print.SetResolution(vN*size, vM*size);
	print.SetCamera(&cam);

	object[nowchara]->entity->SetPosition(startY*size - size / 2, startX*size - size / 2);
	object[nowchara]->fw = startFW;
	object[nowchara]->entity->SetRenderObjectID(object[nowchara]->rid[startFW][0]);
	print.RegisterPixelObject(object[nowchara]->entity);
	
	app.KeyClick.push_back(OnKeyEvent);
	app.Update.push_back(OnUpdate);

	app.MakeWindow("Test", vN*size, vM*size);
	app.SetWorld(&print);
	app.ShowWindow();

	LoadMap(nowmap);
//	print.SetWorldMap(mapinfo[nowmap]->map);
	cam.SetFocus(object[nowchara]->entity->GetPositionX(), object[nowchara]->entity->GetPositionY(), PixelWorldEngine::RectangleF(vN*size / 2, vM*size / 2, vN*size / 2, vM*size / 2));
	app.RunLoop();
}