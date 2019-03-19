/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTTAGALGS_GLOBALEVENTATTRIBUTENAMES_H
#define EVENTTAGALGS_GLOBALEVENTATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Event attributes */
namespace Evt { 

  //static const int NevtAttr = 24;    // This is the number of the Event TAG attributes 
  enum { Run=0, Event=1, Time=2, TimeNS=3, NLumiBlock=4, Weight=5, BunchId=6, ConditionsRun=7, 
         Pixel=8, SCT=9, TRT=10, LAr=11, Tile=12, Muon=13, ForwardDet=14, Core=15, 
         Simu=16, Calib=17, Test=18, Random=19, ActualInt=20, AvgInt=21,
         NCaloClus=22, NTrk=23, NVx=24, VtxX=25, VtxY=26, VtxZ=27, VtxChiSq=28, VtxNdof=29,//, NVxTight=30, 
         RecoTimeRAW2ESD=30, RecoTimeESD2AOD=31,  RecoTimeRAWtoALL=32, RecoTimeEVNT2HITS=33, RecoTimeHITS2RDO=34,
         NevtAttr = 35
       }; 
}

static const AthenaAttributeSpecification EventAttributeSpecs[Evt::NevtAttr] = 
{
   AthenaAttributeSpecification("RunNumber","unsigned int",        GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("EventNumber","unsigned long long",      GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("EventTime","unsigned int",        GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("EventTimeNanoSec","unsigned int", GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("LumiBlockN","unsigned int",       GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("EventWeight","float",             GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("BunchId","unsigned int",          GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("ConditionsRun","unsigned int",        GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("PixelFlags","unsigned int",       GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("SCTFlags","unsigned int",         GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("TRTFlags","unsigned int",         GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("LArFlags","unsigned int",         GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("TileFlags","unsigned int",        GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("MuonFlags","unsigned int",        GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("ForwardDetFlags","unsigned int",  GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("CoreFlags","unsigned int",        GroupNames[Grp::QualityDetector],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("IsSimulation","bool",             GroupNames[Grp::Quality],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("IsCalibration","bool",            GroupNames[Grp::Quality],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("IsTestBeam","bool",               GroupNames[Grp::Quality],UnitNames[Unt::Bitmask]),
   AthenaAttributeSpecification("RandomNumber","float",            GroupNames[Grp::Quality],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("ActualIntPerXing","float",        GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("AvgIntPerXing","float",           GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("NCaloClus","unsigned int",        GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("NTrk","unsigned int",             GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("NVtx","unsigned int",             GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("VtxX","float",                    GroupNames[Grp::Vertex],UnitNames[Unt::milimeter]),
   AthenaAttributeSpecification("VtxY","float",                    GroupNames[Grp::Vertex],UnitNames[Unt::milimeter]),
   AthenaAttributeSpecification("VtxZ","float",                    GroupNames[Grp::Vertex],UnitNames[Unt::milimeter]),
   AthenaAttributeSpecification("VtxChiSq","float",                GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("VtxNDOF","unsigned int",          GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   //   AthenaAttributeSpecification("NVtxTight","int",                 GroupNames[Grp::Vertex],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("RecoTimeRAWtoESD","float",        GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("RecoTimeESDtoAOD","float",        GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("RecoTimeRAWtoALL","float",        GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("RecoTimeEVNTtoHITS","float",      GroupNames[Grp::Time],UnitNames[Unt::Number]),
   AthenaAttributeSpecification("RecoTimeHITStoRDO","float",       GroupNames[Grp::Time],UnitNames[Unt::Number])

};


#endif
