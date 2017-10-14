/**
	BlobbyVolley

	@author 
*/

static pBall;
static TeamScore;

func Initialize()
{
	TeamScore = CreateArray(2);
	
	SetSkyAdjust(RGB(150, 150, 150));
	InitScoreboard();
	
	ScheduleCall(nil, "AdjustMap", 1, 0);
}

global func AdjustMap()
{
	var map = 0;
	if(GetPlayerCount() > 2)
		map = 4;
	if(GetPlayerCount() > 4)
		map = 6;
	if(GetPlayerCount() > 6)
		map = 8;
			
	if(map)
	{
		for(var i=0; i < GetPlayerCount(); i++)
		{
		 	GetCrew(GetPlayerByIndex(i))->SetObjectStatus(2);
		}

		LoadScenarioSection(Format("%dPlayers", map));

		for(var i=0; i < GetPlayerCount(); i++)
 		{
 			GetCrew(GetPlayerByIndex(i))->SetObjectStatus(1);
 			GameCall("InitializePlayer", GetPlayerByIndex(i));
 			SetViewOffset(GetPlayerByIndex(i), 0, -50);
 		}
	}
	
	InitGame();
}

global func InitGame()
{
	ScheduleCall(nil, "InitBall", 40, 0, RandomX(1,2));
	CreateEnvironment();
	AddEffect("Check", nil, 1, 1);
	
	for(var i=0; i < GetPlayerCount(); i++)
	{
		SetViewOffset(GetPlayerByIndex(i), 0, -50);
	}
}

protected func InitializePlayer(iPlr)
{
	 if(GetPlayerTeam(iPlr)==1)
	 	GetCrew(iPlr)->SetPosition(LandscapeWidth()/4, LandscapeHeight()/2 - 50); 
	 	
	 if(GetPlayerTeam(iPlr)==2)
	 	GetCrew(iPlr)->SetPosition(LandscapeWidth()*3/4, LandscapeHeight()/2 - 50); 
	 	
	 SetFoW(0,iPlr);
	 SetPlayerZoomByViewRange(iPlr, 1000, 1000, PLRZOOM_LimitMax);
	 SetPlayerZoomByViewRange(iPlr, 600, 600, PLRZOOM_Direct);
}

global func InitScoreboard()
{
	Scoreboard->Init(
	[
	 	{key = "team", title = "", sorted = true, priority = 200, default = ""},
	 	{key = "score", title = "Score", priority = 100, default = ""}]
	);
	
	for(var i = 1; i <= 2; i++)
	{
		var team_id = i;
		Scoreboard->NewEntry(team_id, GetTaggedTeamName(i));
		Scoreboard->SetData(team_id, "team", "", team_id);
		Scoreboard->SetData(team_id, "score", 0);
	}
}

global func CreateEnvironment()
{
	CreateObject(Net, LandscapeWidth()/2, LandscapeHeight()/2-20);
	Grass->Place(100*(GetPlayerCount()/2), nil);
	Tree_Coniferous->Place(10);
	
	for(var o in FindObjects(Find_Func("IsTree")))
	{
		o->SetObjectLayer(o);
		o.Plane = 300;
	}
}

global func InitBall(int team)
{
	var side = team - 1;
	
	var ball = CreateBall(TheBall, LandscapeWidth()/4 + LandscapeWidth()/2 * side, LandscapeHeight()/2 - 100, 0, 0, -1);
	ball->Init();
}

global func FxCheckTimer(pTarget)
{
	for(var obj in FindObjects(Find_ID(Clonk)))
	{
		if(GetPlayerTeam(obj->GetOwner())==1)if(obj->GetX() > LandscapeWidth()/2-3) { obj->SetPosition(obj->GetX(),obj->GetY()-1); obj->Fling(-5,-2); }
	 	if(GetPlayerTeam(obj->GetOwner())==2)if(obj->GetX() < LandscapeWidth()/2+3) { obj->SetPosition(obj->GetX(),obj->GetY()-1); obj->Fling(5,-2); }
	}
}

global func CreateBall(ID, x, y, xdir, ydir, Owner) {
    
    // Create
    pBall = CreateObject(ID, x, y, Owner);
    pBall->SetXDir(xdir); pBall->SetYDir(ydir);
    
	AddEffect("Ball",pBall,1, 1, nil);
	
	return pBall;
}

func Score(int team)
{
	TeamScore[team-1]++;
	Scoreboard->SetData(team, "score", TeamScore[team - 1]);
	
	DoScoreboardShow(1);
	Schedule(nil, Format("DoScoreboardShow(-1)"), 35 * Goal_DeathMatch.ShowBoardTime);
	
	var dir = (team - 1) * 2 - 1;
	
	var rgb = SplitRGBaValue(GetTeamColor(team));
	
	var props = {
		R = rgb.R,
		G = rgb.G,
		B = rgb.B,
		Size = PV_Linear(15, 0),
		Alpha = PV_Linear(255, 0),
		BlitMode = GFX_BLIT_Additive,
		Rotation = PV_Random(0, 360),
	};
	
	for(var i = 0; i < 50 + (GetPlayerCount()/2 * 50); i++)
	{
		CreateParticle("StarSpark", LandscapeWidth()/2 + Random(LandscapeWidth()/2)*dir, Random(LandscapeHeight()), 12 * dir, 0, 50, props, 2);
	}
	
	if(CheckWin(team))
		return;
		
	if (TeamScore[0] >= 19 || TeamScore[1] >= 19)
		CustomMessage("Match ball!");
	
	InitBall(!(team-1) + 1);
}

func CheckWin(team)
{
	if (TeamScore[team - 1] >= GameCall("ScoreToWin") && Abs(TeamScore[0] - TeamScore[1]) > 1)
		{
			for(var i = 0; i < GetPlayerCount(); i++)
			{
				if(GetPlayerTeam(GetPlayerByIndex(i)) != team)
				{
					EliminatePlayer(GetPlayerByIndex(i));
				}
			}
			
			AddEffect("Fireworks", nil, 1, 2, nil);
			
			ScheduleCall(nil, "GameOver", 200);
			Sound("UI::Trumpet", true, 100);
			return 1;
		}
		
	return 0;
}

global func FxFireworksTimer(target, fx, time)
{
	if(time%40 == 0 || !Random(10))
	{
		Fireworks(HSL(Random(255), 255, 127) , Random(LandscapeWidth()), Random(LandscapeHeight()/3));
		Sound("Firework*", true, 50);
	}
}

func ScoreToWin() { return 20; }

global func FxBallStart(target, fx)
{
	fx.props =
	{
		Prototype = Particles_FireTrail(),
		Size = PV_Linear(10,0),
		BlitMode = GFX_BLIT_Additive,
	};
	
	fx.x = target->GetX();
	fx.y = target->GetY();
}

global func FxBallTimer(target, fx)
{
	target->DrawParticleLine("Fire", 0, 0, fx.x - target->GetX(), fx.y - target->GetY(), 1, PV_Random(-3,3), PV_Random(-3,3), 15, fx.props);
	
	fx.x = target->GetX();
	fx.y = target->GetY();
}

global func FxBallStop(pTarget, fx, reason, temp)
{
	if(temp)
		return;
	
	  // Out?
    if((pTarget->GetX() < 0 || pTarget->GetX() > LandscapeWidth()) && pTarget->GetY() < LandscapeHeight()/2 - 3) 
    {
        var ball = GameCall("CreateBall", pTarget->GetID(), BoundBy(pTarget->GetX(), 0, LandscapeWidth()-1), pTarget->GetY(), -pTarget->GetXDir()*2/3, pTarget->GetYDir(), pTarget->GetOwner());
        ball->SetLastHit(pTarget->GetLastHit(1));
        ball->SetLastHit(pTarget->GetLastHit(2));
        return;
    }
    
    if(pTarget->GetX() > LandscapeWidth()/2) 
    {
    	GameCall("Score", 1);
		GameCall("ScoreMsg", 1, pTarget->GetLastHit(1), pTarget->GetLastHit(2));
     	Sound("UI::Ding", true, 50);
    }
    else if(pTarget->GetX() < LandscapeWidth()/2)  
    {
     	GameCall("Score", 2);
		GameCall("ScoreMsg", 2, pTarget->GetLastHit(2), pTarget->GetLastHit(1));
     	Sound("UI::Ding", true, 50);
    }
    else
    	InitBall(Random(2) + 1);
    
}

func ScoreMsg(team, plr, plr2)
{
     	if(plr != -1)
     	{
     		var r = Random(9) + 1;
     		Log(Translate(Format("ScoreMsg%d", r)), GetTaggedTeamName(team), GetTaggedPlayerName(plr));
     	}
     	else
     		Log("Team %s scored!", GetTaggedTeamName(team));
}
