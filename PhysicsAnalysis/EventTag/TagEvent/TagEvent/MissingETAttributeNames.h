/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETATTRIBUTENAMES_H
#define MISSINGETATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL MissingEt attributes and their types */
namespace EtMiss { 
  static const int NetAttr=9;              // This number of MissingEt TAG Attributes
  static const int NumMissingEt=1;         // This the max number of MissingEt objects in the TAG 
  enum { MET_RefFinalX=0, MET_RefFinalY=1, SumET=2, MET_CellOutX=3, MET_CellOutY=4, MET_SoftJetsX=5, MET_SoftJetsY=6, MET_RefTauX=7, MET_RefTauY=8}; 
}

/** This is the list of the Missing ET TAG attributes */
static const std::string MissingEtAttributeNames[EtMiss::NetAttr] = {
                                                       "MET_RefFinalX",
                                                       "MET_RefFinalY",
                                                       "SumET",
                                                       "MET_CellOutX",
                                                       "MET_CellOutY",
						       "MET_SoftJetsX",
						       "MET_SoftJetsY",
						       "MET_RefTauX",
						       "MET_RefTauY",
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
                                                      GroupNames[Grp::PhysicsJetmet]
                                                     };

#endif
