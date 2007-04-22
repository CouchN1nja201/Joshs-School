#include "world.h"


CWorld::CWorld()
{
// Begin - Phase 12
	terrain = new CTerrain(32, 0.5);
// End - Phase 12
}

CWorld::~CWorld()
{
	// Begin - Phase 19
	audioSystem->Shutdown();
	// End - Phase 19
	// Begin - Phase 18
	delete gui;
	// End - Phase 18
	// Begin - Phase 12
	delete terrain;
	terrain = NULL;
    // End - Phase 12
	// Begin - Phase 18
	gui = NULL;
	// End - Phase 18
	// Begin - Phase 19
	delete audioSystem;
	delete worldSound;
	audioSystem = NULL;
	worldSound = NULL;
}

CWorld::CWorld(CCamera *c)
{
	camera = c;
	// Begin - Phase 12
	terrain = new CTerrain(32, 1.0f);
	// End - Phase 12
	// Begin - Phase 14
	player = new CPlayer;
	// End - Phase 14
	// Begin - Phase 19
	audioSystem = new CAudioSystem;
	// End - Phase 19
	// Begin - Phase 18
	gui = new CGUI;
	// End - Phase 18
	// Begin - Phase 16
	gameDone = false;
	// End - Phase 16
	// Begin - Phase 19
	audioSystem->InitDirectXAudio(NULL); 
	// End - Phase 19
	// Begin - Phase 14
	player->AttachTo(terrain);
	player->SetCamera(camera);
	player->SetTerrain(terrain);
	// End - Phase 14
	// Begin - Phase 19
	worldSound = audioSystem->Create("\\Quake.wav", false);
	audioSystem->Play(worldSound, DMUS_SEG_REPEAT_INFINITE, true);

	player->SetAudioSystem(audioSystem);
	// End - Phase 19
	LoadWorld();
	timeStart = 300.0;			// 300 seconds = 5 minutes
	timeElapsed = 0.0;
	// Begin - Phase 18
	gui->SetCurrentTime(timeStart);
	gui->SetEnemiesLeft(numOgros + numSods + numCows + numMechs);
	// End - Phase 18
}

void CWorld::Animate(float deltaTime)
{
	//Todd Brown's Code Modifications
	TB_PlayerPosition.x = camera->position.x;
	TB_PlayerPosition.y = camera->position.z;
	TB_PlayerYaw = camera->yaw;
	//End Todd Brown's Code Modifications
	// Phase 12 - Begin
	// Phase 13 - Add hardwired 7.0f for player size
	// Phase 14 - Add player-> size back to camera position
	// set camera height based on player position on terrain
	camera->position.y = terrain->GetHeight(camera->position.x, camera->position.z) + player->size;	
	
	if (camera->position.x <= terrain->GetScanDepth())
		camera->position.x = terrain->GetScanDepth();
	if (camera->position.x >= terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth())
		camera->position.x = terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth();
	if (camera->position.z <= terrain->GetScanDepth())
		camera->position.z = terrain->GetScanDepth();
	if (camera->position.z >= terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth())
		camera->position.z = terrain->GetWidth()*terrain->GetMul() - terrain->GetScanDepth();

	terrain->Animate(deltaTime);   // animate terrain
	// Phase 12 - End
	// Phase 15 - Begin
	const type_info &ogro = typeid(COgroEnemy);  // get ogro typeid
	const type_info &sod = typeid(CSodEnemy);    // get sod typeid
	//new cow enemy
	const type_info &cow = typeid(CCowEnemy);
	//new mech enemy
	const type_info &mech = typeid(CMechEnemy);
	const type_info &droid = typeid(CDroidEnemy);

	numOgros = CountObjectTypes(ogro);           // count ogros
	TB_NumOgros = numOgros;
	numSods = CountObjectTypes(sod);             // count sods
	TB_NumSods = numSods;
	numCows = CountObjectTypes(cow);			// count sods
	TB_NumCows = numCows;
	numMechs = CountObjectTypes(mech);			// count mech
	TB_NumMechs = numMechs;
	numDroids = CountObjectTypes(droid);	
	TB_NumDroids = numDroids;
	// Phase 15 - End

	// Begin - Phase 18
	gui->SetEnemiesLeft(numOgros + numSods + numCows + numMechs + numDroids);
	gui->SetCurrentTime(timeStart - timeElapsed);
	// End - Phase 18

	// Phase 16 - Begin
	if (!gameDone)
		timeElapsed += deltaTime;
	else
		timeElapsed = timeStart;
    // Phase 16 - End


}

void CWorld::FadeScreen()
{
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
	glBegin(GL_QUADS);
	glVertex3f(-5, -5, -3.0);
	glVertex3f(5, -5, -3.0);
	glVertex3f(5, 5, -3.0);
	glVertex3f(-5, 5, -3.0);
	glEnd();
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void CWorld::SetScreen(int width, int height)
{
	screenWidth = width;
	screenHeight = height;
}

void CWorld::Draw(CCamera *camera)
{
	// Begin - Phase 12
	terrain->Draw(camera);
	// End - Phase 12
	// Begin - Phase 18
	gui->Draw();

	if (gameDone)
	{
		FadeScreen();
		if (numOgros + numSods + numCows + numMechs + numDroids <=0)
			gui->DrawWinner();
		else
			gui->DrawLoser();
	}
// End - Phase 18

}

void CWorld::OnPrepare()
{
	// Begin - Phase 12
	glClearColor(terrain->fogColor[0], terrain->fogColor[1], terrain->fogColor[2], terrain->fogColor[3]);
	terrain->Prepare();
	// End - Phase 12
	// Phase 15 - Begin
	if ((numOgros + numSods + numCows + numMechs +numDroids <= 0) || (timeElapsed >= timeStart))
		gameDone = true;
	// Phase 15 - End
	
}


// LoadWorld()
// desc: initializes the world
void CWorld::LoadWorld()
{
	
	// Phase 15 - Begin
	int enemyIdx = 0;
	int rndInt = 0;

	numOgros = 0;
	numSods = 0;
	numCows = 0;
	numMechs = 0;

	srand((unsigned int)time(NULL));
	
	rndInt = (rand() % (MAX_ENEMIES-4)) + 4;	// random # from 4 to MAX
	numOgros = numSods = numCows = numMechs = numDroids = rndInt;

	//generate cows
	for (enemyIdx = 0; enemyIdx < numMechs; enemyIdx++)
	{
		cowEnemy = new CCowEnemy;
		cowEnemy->AttachTo(terrain);
		cowEnemy->SetPlayer(player);
		cowEnemy->SetAudioSystem(audioSystem);
		cowEnemy->LoadAudio(audioSystem, "\\models\\Cow\\cow.wav", false);
		cowEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		cowEnemy->position.y = 0.0f;
		cowEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}
	
	//generate mechs
	for (enemyIdx = 0; enemyIdx < numMechs; enemyIdx++)
	{
		mechEnemy = new CMechEnemy;
		mechEnemy->AttachTo(terrain);
		mechEnemy->SetPlayer(player);
		mechEnemy->SetAudioSystem(audioSystem);
		mechEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		mechEnemy->position.y = 0.0f;
		mechEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}

	// generate ogros
	for (enemyIdx = 0; enemyIdx < numOgros; enemyIdx++)
	{
		ogroEnemy = new COgroEnemy;
		ogroEnemy->AttachTo(terrain);
		ogroEnemy->SetPlayer(player);
		// Phase 19 - Uncomment
		ogroEnemy->SetAudioSystem(audioSystem);
		ogroEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		ogroEnemy->position.y = 0.0f;
		ogroEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}

	// generate sods
	for (enemyIdx = 0; enemyIdx < numSods; enemyIdx++)
	{
		sodEnemy = new CSodEnemy;
		sodEnemy->AttachTo(terrain);
		sodEnemy->SetPlayer(player);
		// Phase 19 - Uncomment
		sodEnemy->SetAudioSystem(audioSystem);
		sodEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		sodEnemy->position.y = 0.0f;
		sodEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}

	for (enemyIdx = 0; enemyIdx < numDroids; enemyIdx++)
	{
		droidEnemy = new CDroidEnemy;
		droidEnemy->AttachTo(terrain);
		droidEnemy->SetPlayer(player);
		// Phase 19 - Uncomment
		droidEnemy->SetAudioSystem(audioSystem);
		droidEnemy->LoadAudio(audioSystem, "\\models\\Droid\\death1.wav", false);
		droidEnemy->position.x = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
		droidEnemy->position.y = 0.0f;
		droidEnemy->position.z = (float)(rand() % (int)(terrain->GetWidth() * terrain->GetMul()));
	}

		
  // Phase 15 - End
}
// Phase 15 - Begin
// CountObjectTypes
// desc: counts the number of objects passed as classID in the hierarchy
int CWorld::CountObjectTypes(const type_info &classID)
{
	int count = 0;
	CObject *c1;
	CObject *c2;

	c1 = (CObject*)terrain->childNode;

	//Todd Brown's Code Modifications
	const type_info &ogro = typeid(COgroEnemy);  // get ogro typeid
	const type_info &sod = typeid(CSodEnemy);    // get sod typeid
	const type_info &cow = typeid(CCowEnemy);
	const type_info &mech = typeid(CMechEnemy);
	const type_info &droid = typeid(CDroidEnemy);
	int index = 0;
	if (classID == ogro)
	{
		for (int x=0; x<MAX_ENEMIES; x++)
			TB_OgroPtr[x]=NULL;
	}
	if (classID == sod)
	{
		for (int x=0; x<MAX_ENEMIES; x++)
			TB_SodPtr[x]=NULL;
	}
	if (classID == cow)
	{
		for (int x=0; x<MAX_ENEMIES; x++)
			TB_CowPtr[x]=NULL;
	}
	if (classID == mech)
	{
		for (int x=0; x<MAX_ENEMIES; x++)
			TB_MechPtr[x]=NULL;
	}
	if (classID == droid)
	{
		for (int x=0; x<MAX_ENEMIES; x++)
			TB_DroidPtr[x]=NULL;
	}
	//End Todd Brown's Code Modifications

	while (c1 != NULL)
	{
		c2 = c1;
		do
		{
			if (typeid(*c2) == classID)
			{
				if (classID == ogro)
				{
					TB_OgroPtr[index] = static_cast<COgroEnemy *>(c2);
					index ++;
				}
				if (classID == sod)
				{
					TB_SodPtr[index] = static_cast<CSodEnemy *>(c2);
					index ++;
				}
				if (classID == cow)
				{
					TB_CowPtr[index] = static_cast<CCowEnemy *>(c2);
					index ++;
				}
				if (classID == mech)
				{
					TB_MechPtr[index] = static_cast<CMechEnemy *>(c2);
					index ++;
				}
				if (classID == droid)
				{
					TB_DroidPtr[index] = static_cast<CDroidEnemy *>(c2);
					index ++;
				}
				count++;
			}
			c2 = (CObject*)c2->nextNode;
		} while (!c2->IsFirstChild());
//      Phase 19 - Remove the following
//		count--;

		c1 = (CObject*)c1->childNode;
	}

	return count;
}
// Phase 15 - End
