#appendto Clonk

local MaxEnergy = 100000;
local MaxMagic = 150000;


func Construction()
{
	AddEffect("ManaRegen", this, 1, 1, this, Clonk);
	AddEffect("IntShield", this, 1, 1, nil, GetID());
	return _inherited();
}

func FxManaRegenTimer()
{
	if(GetY() > LandscapeHeight()/2 - 75)
		DoMagicEnergy(2);
}

func StartTumble()
{
	if(GetEffect("StopTumbling", this))
		RemoveEffect("StopTumbling", this);
		
	AddEffect("StopTumbling", this, 20, 20, this, Clonk);
	return _inherited();
}

func FxStopTumblingTimer()
{
	if (GetAction() == "Tumble")
	{
		SetAction("Jump");
	}
	return -1;
}


public func ObjectControl(int plr, int ctrl, int x, int y, int strength, bool repeat, bool release)
{		
	if (ctrl == CON_Use && !release)
	{
		if(GetAction() == "Tumble" || GetMagicEnergy() < 30)
		{
			Sound("UI::Error", false, 50, GetOwner());
			return;
		}
		
		if(GetEffect("ToadCD", this))
			return;
		
		AddEffect("ToadCD", this, 1, 15);
		
		Sound("ToadJump", false, 50);
		DoMagicEnergy(-30);
		
		var angle = Angle(0, 0, x, y);
		SetAction("Jump");
		SetVelocity(angle, 60);
		
		return true;
	}
	 
	return _inherited(plr, ctrl, x, y, strength, repeat, release);
}

func FxIntShieldStart(target, effect)
{
	var clr = GetPlayerColor(target->GetOwner());
	var rgba = SplitRGBaValue(clr);

	effect.props = {
		R = rgba[0],
		G = rgba[1],
		B = rgba[2],
		Size = PV_Linear(3, 0),
		Attach = ATTACH_Front|ATTACH_MoveRelative,
		Alpha = PV_Linear(255, 0)
	};
}

func FxIntShieldDamage()
{
	return 0;
}

func FxIntShieldTimer(object pTarget, proplist effect, int iEffecttime)
{
    effect.angle += 4;
    var radius = 30;
    
    for(var i = 0; i<3; i+=1) 
        pTarget->CreateParticle("Flash", Sin(effect.angle + 120 * i, radius), Cos(effect.angle + 120 * i, radius), 0, 0, 35, effect.props, 2);
 }
 
func FxIntShieldCheckBall(object pTarget, proplist effect, pBall)
 {
    //for(var object in pTarget->FindObjects(pTarget->Find_Distance(radius), Find_Category(C4D_Object)))
    if(pBall)
    {
        var xdir = pBall->GetXDir(), ydir = pBall->GetYDir();
        if(xdir || ydir)
        {
            var entryangle = Angle(0,0,xdir,ydir);
            var objectangle = Angle(pTarget->GetX(), pTarget->GetY(), pBall->GetX(), pBall->GetY());
            
           /* if(Distance(pTarget->GetX(), pTarget->GetY(), pBall->GetX(), pBall->GetY()) < 25)
            {
            	pBall->SetPosition(pTarget->GetX() + Sin(objectangle, 30), pTarget->GetY() + -Cos(objectangle, 30));
            }*/
            
            if(!(Inside(Abs(objectangle - entryangle), 0, 90) || Inside(Abs(objectangle - entryangle), 270, 360)))
            {
                var tangle = 2* ( (objectangle + 90)%360 - entryangle) + entryangle;
                var speed = Sqrt(xdir**2+ydir**2);
                if(speed > 90)speed=90;
                //if(speed < 40)speed=40;
                pBall->SetSpeed(Sin(tangle, speed)+pTarget->GetXDir()/3*2, -Cos(tangle, speed)+pTarget->GetYDir()/3*2);
                pBall->SetOwner(pTarget->GetOwner());
                pBall->SetLastHit(pTarget->GetOwner());
            }
        }
    }
    //if(iEffecttime > 40*37) return(-1);
}