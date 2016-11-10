/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEJETATTRIBUTENAMES_H
#define PARTICLEJETATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Jet attribute  */
namespace PJet { 
  static const int NjetAttr=9;   // This is the number of Jet TAG attributes
  static const int NumJet = 10;   // This is the max number of ParticleJets in the TAG
  enum { NJet=0, Pt=1, Eta=2, Phi=3, JetSumET=4, E=5, JetPID=6, NpTJet=7, NpTBJet=8 };
}

/** This is the list of Jet TAG attributes */
static const std::string JetAttributeNames[PJet::NjetAttr] = {
                                                  "NJet",
                                                  "JetPt",
                                                  "JetEta",
                                                  "JetPhi",
                                                  "JetSumET",
						  "JetE",
						  "JetPID",
						  "NpTJet",
						  "NpTBJet",
                                               };

/** This is the list of Jet TAG attributes unit names*/
static const std::string JetAttributeUnitNames[PJet::NjetAttr] = {
                                               UnitNames[Unt::Number],
                                               UnitNames[Unt::Mevc],
                                               UnitNames[Unt::Rap],
                                               UnitNames[Unt::Rad],
                                               UnitNames[Unt::Mev],
                                               UnitNames[Unt::Mev],
                                               UnitNames[Unt::Bitmask],
                                               UnitNames[Unt::Number],
                                               UnitNames[Unt::Number],
};

/** This is the list of Jet TAG attributes group names */
static const std::string JetAttributeGroupNames[PJet::NjetAttr] = {
                                                GroupNames[Grp::PhysicsJetmet],
                                                GroupNames[Grp::PhysicsJetmet],
                                                GroupNames[Grp::PhysicsJetmet],
                                                GroupNames[Grp::PhysicsJetmet],
                                                GroupNames[Grp::PhysicsJetmet],
						GroupNames[Grp::PhysicsJetmet],
                                                GroupNames[Grp::PhysicsJetmet],
                                                GroupNames[Grp::PhysicsJetmet],
                                                GroupNames[Grp::PhysicsJetmet],
                                               };


#endif
