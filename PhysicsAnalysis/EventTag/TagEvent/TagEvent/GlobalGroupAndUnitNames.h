/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAGEVENT_GLOBALGROUPANDUNITNAMES_H
#define TAGEVENT_GLOBALGROUPANDUNITNAMES_H

/****************************************
 Author - Tulay Cuhadar Donszelmann
 Nov 2008
****************************************/
#include <vector>
#include <string>
#include <iostream>

namespace Grp {
    //static const int NGroupNames = 16;
    enum { Nav=0, Time=1, Vertex=2, Quality=3, PhysicsElectron=4, PhysicsPhoton=5, PhysicsMuon=6, PhysicsTau=7,   PhysicsJetmet=8, TriggerL1=9, TriggerHLT=10, TriggerStream=11, QualityDetector=12, QualityPhysics=13,  RawSummary=14, Test=15, NGroupNames=16 };
    
}    
    
    /** List of all the  Group names */
static const std::string GroupNames[Grp::NGroupNames] = {  "NAV",
                                                           "TIME",
                                                           "VERTEX",
                                                           "QUALITY",
                                                           "PHYSICS_ELECTRON",
                                                           "PHYSICS_PHOTON",
                                                           "PHYSICS_MUON",
                                                           "PHYSICS_TAU",
                                                           "PHYSICS_JETMET",
                                                           "TRIGGER_L1",
                                                           "TRIGGER_HLT",
                                                           "TRIGGER_STREAM", 
                                                           "QUALITY_DETECTOR", 
                                                           "QUALITY_PHYSICS",
                                                           "RAWSUMMARY",
                                                           "TEST"
                                                        };

/** List of all the  unit names */
namespace Unt {
    static const int NUnitNames = 12;
    enum { Bitmask=0, milimeter=1, Mev=2, Mevc=3, Rad=4, Number=5, invpicobarn=6, invfemtobarn=7, Rap=8, StatusFlag=9, invMev=10, nanosec=11 };
}    
static const std::string UnitNames[Unt::NUnitNames] = {  "BITMASK",
                                                         "MM",
                                                         "MEV",
                                                         "MEV/c",
                                                         "RAD",
                                                         "NUMBER",
                                                         "INV_PB", 
                                                         "INV_FB",
                                                         "PSEUDO_RAP",
                                                         "RYG_FLAG",
                                                         "1/MEV",
                                                         "NS" 
                                                      };
	                                                                               


#endif
