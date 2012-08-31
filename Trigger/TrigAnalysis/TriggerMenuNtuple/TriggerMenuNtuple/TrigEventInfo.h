/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**  @file TrigEventInfo.h
*
*    authors: Akimasa Ishikawa (akimasa@phys.sci.kobe-u.ac.jp)             
*/
#ifndef __Trig_Event_Info_H__
#define __Trig_Event_Info_H__

#include <vector>

class TrigEventInfo
{

public:
  TrigEventInfo () ;
  virtual ~TrigEventInfo ();

  //set functions
  void setTrigEventInfo ( unsigned int s_run, unsigned int s_lumiblock, unsigned int s_event, unsigned int s_time, unsigned int s_BCID );
  void setEventInfo ( unsigned int s_run, unsigned int s_lumiblock, unsigned int s_event, unsigned int s_time, unsigned int s_timens, unsigned int s_BCID, unsigned int detectormask0, unsigned int detectormask1 );
  void setTrigInfo ( unsigned int s_l1triggertype, std::vector<unsigned int> s_l1triggerinfo );
  
  //clear function
  void clear ();

public:
  unsigned int run;
  unsigned int lumiblock;
  unsigned int event;
  unsigned int time;
  unsigned int timens;
  unsigned int BCID;
  unsigned int detectormask0;
  unsigned int detectormask1;

  unsigned int level1TriggerType;
  std::vector<unsigned int> level1TriggerInfo;

};

#endif
