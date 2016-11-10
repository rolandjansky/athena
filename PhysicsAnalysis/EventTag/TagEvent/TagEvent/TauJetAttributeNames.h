/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUJETATTRIBUTENAMES_H
#define TAUJETATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the TauJet attributes and their types */

namespace TauID { 
  static const int NtauJetAttr=5;   // This is the number of Tau TAG attributes
  static const int NumTauJet = 4;   // This is the max number of TauJets in the TAG
  enum { NTau=0, Pt=1, Eta=2, Phi=3, PID=4 }; 
}

/** This is the list of the TauJet TAG attributes */
static const std::string TauJetAttributeNames[TauID::NtauJetAttr] = {
                                                    "NTau",
                                                    "TauJetPt",     
                                                    "TauJetEta",
                                                    "TauJetPhi",
                                                    "TauJetPID"

                                                  };

/** This is the list of the TauJet TAG attributes unit names*/
static const std::string TauJetAttributeUnitNames[TauID::NtauJetAttr] = {
                                               UnitNames[Unt::Number],
                                               UnitNames[Unt::Mevc],
                                               UnitNames[Unt::Rap],
                                               UnitNames[Unt::Rad],
                                               UnitNames[Unt::Bitmask]
                                                  };

/** This is the list of the TauJet TAG attributes group names*/
static const std::string TauJetAttributeGroupNames[TauID::NtauJetAttr] = {
                                               GroupNames[Grp::PhysicsTau],
                                               GroupNames[Grp::PhysicsTau],
                                               GroupNames[Grp::PhysicsTau],
                                               GroupNames[Grp::PhysicsTau],
                                               GroupNames[Grp::PhysicsTau]
                                                  };

#endif
