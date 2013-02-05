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
   static const int Ntrg = 5;      // This is the number of Trigger TAG attributes 
   static const int NumTrig = 8;   // This is the max number of Trigger words in the TAG per some attributes
   enum { LVL1=0, LVL2=1, EF=2, Stream=3, BunchGroup=4 }; 
}

/** This is the list of the Trigger TAG attributes */
static const std::string TriggerAttributeNames[Trg::Ntrg] = {
                                                       "L1PassedTrigMask",
                                                       "L2PassedTrigMask",
                                                       "EFPassedTrigMask",
                                                       "SFOStreams",
                                                       "BunchGroup"
                                                   };

/** This is the list of the Trigger TAG attributes unit names*/
static const std::string TriggerAttributeUnitNames[Trg::Ntrg] = {
                                                        UnitNames[Unt::Bitmask],
                                                        UnitNames[Unt::Bitmask],
                                                        UnitNames[Unt::Bitmask],
                                                        UnitNames[Unt::Bitmask],
                                                        UnitNames[Unt::Number]
                                                   };

/** This is the list of the Trigger TAG attributes group names*/
static const std::string TriggerAttributeGroupNames[Trg::Ntrg] = {
                                                       GroupNames[Grp::TriggerL1],
                                                       GroupNames[Grp::TriggerL2],
                                                       GroupNames[Grp::TriggerEF],
                                                       GroupNames[Grp::TriggerStream],
                                                       GroupNames[Grp::TriggerStream]
                                                   };
#endif
