/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETATTRIBUTENAMES_H
#define MISSINGETATTRIBUTENAMES_H

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL MissingEt attributes and their types */
namespace EtMiss { 
  static const int NetAttr=19;              // This number of MissingEt TAG Attributes
  static const int NumMissingEt=1;         // This the max number of MissingEt objects in the TAG 
  enum { MET_RefFinalX=0, MET_RefFinalY=1, SumET=2, MET_SoftClusX=3, MET_SoftClusY=4, MET_RefJetX=5, MET_RefJetY=6, MET_RefTauX=7, MET_RefTauY=8, MET_MuonsX=9, MET_MuonsY=10, MET_RefEleX=11, MET_RefEleY=12, MET_RefGammaX=13, MET_RefGammaY=14, MET_PVSoftTrkX=15, MET_PVSoftTrkY=16, MET_FinalTrkX=17,  MET_FinalTrkY=18
}; 
}

/** This is the list of the Missing ET TAG attributes */
static const std::string MissingEtAttributeNames[EtMiss::NetAttr] = {
                                                       "MET_RefFinalX",
                                                       "MET_RefFinalY",
                                                       "SumET",
                                                       "MET_SoftClusX",
                                                       "MET_SoftClusY",
						       "MET_RefJetX",
						       "MET_RefJetY",
						       "MET_RefTauX",
						       "MET_RefTauY",
						       "MET_MuonsX",
						       "MET_MuonsY",
						       "MET_RefEleX",
						       "MET_RefEleY",
						       "MET_RefGammaX",
						       "MET_RefGammaY",
						       "MET_PVSoftTrkX",
						       "MET_PVSoftTrkY",
						       "MET_FinalTrkX",
						       "MET_FinalTrkY"
                                                     };

/** This is the list of the Missing ET TAG attributes unit names*/
static const std::string MissingEtAttributeUnitNames[EtMiss::NetAttr] = {
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
						       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
						       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev],
                                                       UnitNames[Unt::Mev]
                                                     };


/** This is the list of the Missing ET TAG attributes group names*/
static const std::string MissingEtAttributeGroupNames[EtMiss::NetAttr] = {
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
						      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
						      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet],
                                                      GroupNames[Grp::PhysicsJetmet]
                                                     };

#endif
