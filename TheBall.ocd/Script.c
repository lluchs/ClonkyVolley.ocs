
local last_hit;

public func Initialize()
{
 AddEffect("CheckClonks",this,1,1,this);
 last_hit = CreateArray(2);
 last_hit[0] = -1;
 last_hit[1] = -1;
}

func GetLastHit(int team)
{
	return last_hit[team-1];
}

func SetLastHit(int plr)
{
	if(plr == -1)
		return;
	last_hit[GetPlayerTeam(plr) - 1] = plr;
}

func FxCheckClonksTimer(pTarget,iEffectNumber)
{
 var radius=30;
 for(var obj in pTarget->FindObjects(pTarget->Find_Distance(radius), Find_OCF(OCF_CrewMember)))
 {
  var eff;
  if(eff=GetEffect("IntShield",obj))
   	EffectCall(obj, eff, "CheckBall", pTarget);
 }
}


protected func Hit()
{
  //ExplosionEffect(18, 0, 0, 0, false, 18);
  DoExplosion(0, 0, 18, nil, GetOwner(), nil, false, 18);
  RemoveObject();
}

func Init()
{
	SetAction("Fly");
	AddEffect("Init", this, 1, 1, this);
}

func FxInitTimer(target, fx, timer)
{
	if(timer > 80)
		return -1;
}

func FxInitStop()
{
	SetAction("Idle");
	SetYDir(-55);
}

local ActMap = {

	Fly = {
		Prototype = Action,
		Name = "Travel",
		Procedure = DFA_FLOAT,
		NextAction = "Travel",
		Length = 1,
		Delay = 1,
		FacetBase = 1,
		StartCall = "Traveling",
		Speed=1000
	},
};