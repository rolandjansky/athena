/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TrigEventInfo.cxx
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*/
#include "TriggerMenuNtuple/TrigEventInfo.h"

TrigEventInfo::TrigEventInfo () :
  run(0),
  lumiblock(0),
  event(0),
  time(0),
  timens(0),
  BCID(0),
  detectormask0(0),
  detectormask1(0), 
  level1TriggerType(0), 
  level1TriggerInfo()
{
}

TrigEventInfo::~TrigEventInfo ()
{
}

void
TrigEventInfo::setTrigEventInfo ( unsigned int s_run, 
                                  unsigned int s_lumiblock, 
                                  unsigned int s_event,
                                  unsigned int s_time,
                                  unsigned int s_BCID )
{

  run       = s_run;
  lumiblock = s_lumiblock;
  event     = s_event;
  time      = s_time;
  BCID      = s_BCID;

}

void
TrigEventInfo::setEventInfo ( unsigned int s_run, 
                                  unsigned int s_lumiblock, 
                                  unsigned int s_event,
                                  unsigned int s_time,
                                  unsigned int s_timens,
                                  unsigned int s_BCID,
                                  unsigned int s_detectormask0,
                                  unsigned int s_detectormask1)
{

  run       = s_run;
  lumiblock = s_lumiblock;
  event     = s_event;
  time      = s_time;
  timens    = s_timens;
  BCID      = s_BCID;
  detectormask0 = s_detectormask0;
  detectormask1 = s_detectormask1;

}

void 
TrigEventInfo::setTrigInfo ( unsigned int s_l1triggertype, 
              std::vector<unsigned int> s_l1triggerinfo ){

  level1TriggerType = s_l1triggertype;
  level1TriggerInfo = s_l1triggerinfo;
}



void
TrigEventInfo::clear () 
{
    
  run       = 0;
  lumiblock = 0;
  event     = 0;
  time      = 0;
  timens    = 0;
  BCID      = 0;
  detectormask0 = 0;
  detectormask1 = 0;

  level1TriggerType = 0;
  level1TriggerInfo.clear();

}
