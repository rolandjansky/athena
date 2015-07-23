/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_GLOBALTRIGGERATTRIBUTENAMES_H
#define EVENTTAGALGS_GLOBALTRIGGERATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Trigger status attributes */
namespace Trg { 
   static const int Ntrg = 4;      // This is the number of Trigger TAG attributes 
   static const int NumTrig = 16;   // This is the max number of Trigger words in the TAG per some attributes
   enum { LVL1=0, HLT=1, Stream=2, BunchGroup=3 }; 
}

/** This is the list of the Trigger TAG attributes */
static const std::string TriggerAttributeNames[Trg::Ntrg] = {
                                                       "L1PassedTrigMask",
                                                       "HLTPassedTrigMask",
                                                       "SFOStreams",
                                                       "BunchGroup"
                                                   };

/** This is the list of the Trigger TAG attributes unit names*/
static const std::string TriggerAttributeUnitNames[Trg::Ntrg] = {
                                                        UnitNames[Unt::Bitmask],
                                                        UnitNames[Unt::Bitmask],
                                                        UnitNames[Unt::Bitmask],
                                                        UnitNames[Unt::Number]
                                                   };

/** This is the list of the Trigger TAG attributes group names*/
static const std::string TriggerAttributeGroupNames[Trg::Ntrg] = {
                                                       GroupNames[Grp::TriggerL1],
                                                       GroupNames[Grp::TriggerHLT],
                                                       GroupNames[Grp::TriggerStream],
                                                       GroupNames[Grp::TriggerStream]
                                                   };
#endif
