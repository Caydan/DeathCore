/*
 * Copyright (C) 2013-2015 DeathCore <http://www.noffearrdeathproject.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "grim_batol.h"
#include "Vehicle.h"

enum Texts
{
    SAY_FREE_DRAGON_0 = 0,
    SAY_FREE_DRAGON_1 = 1,
    SAY_FREE_DRAGON_2 = 2,
    SAY_FREE_DRAGON_3 = 3,
    SAY_FREE_DRAGON_4 = 4,
    SAY_FREE_DRAGON_5 = 5,
    SAY_DRAGON_FREE   = 6,
    SAY_REMOVE_NET    = 7,
};

uint64 sDrakeSays;

enum Spells
{
    SPELL_NET     = 79377,
};

enum Creatures
{
    NPC_NET       = 42570,
    NPC_RED_DRAKE = 39294,
};

static const Position RedDrakeLandInstance[25] =
{
    {-505.096f, -346.693f, 295.148f},
    {-566.881f, -348.535f, 304.804f},
    {-642.781f, -383.608f, 306.45f},
    {-722.426f, -460.139f, 324.362f},
    {-735.085f, -514.648f, 327.044f},
    {-719.083f, -607.351f, 315.022f},
    {-683.708f, -645.474f, 317.541f},
    {-620.409f, -694.036f, 296.418f},
    {-544.672f, -704.941f, 302.507f},
    {-485.056f, -698.166f, 321.069f},
    {-423.972f, -669.439f, 295.727f},
    {-403.6f, -670.734f, 290.807f},
    {-395.438f, -697.2f, 286.55f},
    {-431.827f, -720.217f, 304.647f},
    {-487.154f, -736.53f, 301.165f},
    {-570.273f, -736.688f, 298.638f},
    {-666.459f, -692.118f, 303.875f},
    {-693.671f, -661.456f, 305.498f},
    {-728.92f, -603.343f, 316.247f},
    {-735.455f, -552.554f, 323.006f},
    {-731.263f, -501.56f, 319.229f},
    {-685.958f, -401.127f, 317.204f},
    {-633.514f, -355.87f, 303.427f},
    {-550.539f, -345.475f, 290.205f},
    {-460.16f, -330.359f, 277.596f},
};

class npc_battered_red_drake: public CreatureScript
{
    public:
    npc_battered_red_drake() : CreatureScript("npc_battered_red_drake") { }
    
    struct npc_battered_red_drakeAI : public VehicleAI
    {
        npc_battered_red_drakeAI(Creature *c) : VehicleAI(c)
        {
            sDrakeSays = 0;
        }
        
        uint8 uiPoint;
        bool FlyDisabled;
        bool Fly;
        
        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->CastSpell(me, SPELL_NET, false);
            Fly = false;
            FlyDisabled = false;
        }
        
        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if(apply && me->GetAura(SPELL_NET))
                {
                    Talk(SAY_REMOVE_NET);
                    who->ExitVehicle(me);
                }
                if(apply && !me->GetAura(SPELL_NET))
                {
                    me->SetCanFly(true);
                    me->SetSpeed(MOVE_FLIGHT, 2.0f);
                    GoToTheNextPoint();
                    Talk(SAY_DRAGON_FREE);
                }
                
            }
        }
        
        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
            return;
            
            if (id == uiPoint && !FlyDisabled)
            {
                ++uiPoint;
                Fly = true;
            }
        }
        
        void GoToTheNextPoint()
        {
            if(!FlyDisabled)
            me->GetMotionMaster()->MovePoint(uiPoint, RedDrakeLandInstance[uiPoint]);
        }
        
        void UpdateAI(uint32 const diff) 
        {
            if(Fly && !FlyDisabled)
            {
                GoToTheNextPoint();
                if (uiPoint >= 25)
                {
                    if(Unit *player = me->GetVehicleKit()->GetPassenger(0))
                    player->ExitVehicle(me);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    FlyDisabled = true;
                    me->GetMotionMaster()->MoveIdle();
                    me->StopMoving();
                }
                Fly = false;
            }
        }
        
    };
    
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_battered_red_drakeAI (creature);
    }
};

class npc_net_red_dragon: public CreatureScript
{
    public:
    npc_net_red_dragon() : CreatureScript("npc_net_red_dragon") { }
    
    struct npc_net_red_dragonAI : ScriptedAI
    {
        npc_net_red_dragonAI(Creature *c) : ScriptedAI(c)
        {
           SetCombatMovement(false);
        }
        
        void Reset()
        {
            
        }
        
        void JustDied(Unit* /*killer*/)
        {
            if (Creature* pRedDrake = me->FindNearestCreature(NPC_RED_DRAKE, 2.0f, true))
            {
                if(me->GetPositionX() == pRedDrake->GetPositionX() && me->GetPositionY() == pRedDrake->GetPositionY())
                {
                    pRedDrake->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pRedDrake->RemoveAura(SPELL_NET);
                    switch(sDrakeSays)
                    {
                        case 0:
                            pRedDrake->AI()->Talk(SAY_FREE_DRAGON_0);
                            break;
                        case 1:
                            pRedDrake->AI()->Talk(SAY_FREE_DRAGON_1);
                            break;
                        case 2:
                            pRedDrake->AI()->Talk(SAY_FREE_DRAGON_2);
                            break;
                        case 3:
                            pRedDrake->AI()->Talk(SAY_FREE_DRAGON_3);
                            break;
                        case 4:
                            if(uint32 rand_t = urand(1,2))
                            {
                                if(rand_t == 1)
                                pRedDrake->AI()->Talk(SAY_FREE_DRAGON_4);
                                else
                                pRedDrake->AI()->Talk(SAY_FREE_DRAGON_5);
                            }
                            break;
                    }
                    ++sDrakeSays;
                }
            }
        }
        
        void UpdateAI(uint32 const diff) 
        {
        }
        
    };
    
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_net_red_dragonAI (creature);
    }
};
void AddSC_grimbatol()
{
    new npc_battered_red_drake();
    new npc_net_red_dragon();
}