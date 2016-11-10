/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONATTRIBUTENAMES_H
#define PHOTONATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Photon attributes  */
namespace PhotonID { 
  static const int NphotonAttr=7;                                // This is the number of Photon Tag Attributes
  static const int NumPhoton = 4;                                // This max number of Photons on the TAG
  enum { NPhoton=0, NConverted=1, Pt=2, Eta=3, Phi=4, Tight=5, Isol=6 }; 
}

/** This is the list of the Photon TAG attributes */
static const std::string PhotonAttributeNames[PhotonID::NphotonAttr] = {
                                                      "NLoosePhoton",
                                                      "NConvertedLoosePhoton",
                                                      "LoosePhotonPt",
                                                      "LoosePhotonEta",
                                                      "LoosePhotonPhi",
                                                      "LoosePhotonTightness",
                                                      "LoosePhotonIsolation"
                                                   };

/** This is the list of the Photon TAG attributes unit names*/
static const std::string PhotonAttributeUnitNames[PhotonID::NphotonAttr] = {
                                                      UnitNames[Unt::Number],
                                                      UnitNames[Unt::Number],
                                                      UnitNames[Unt::Mevc],
                                                      UnitNames[Unt::Rap],
                                                      UnitNames[Unt::Rad],
                                                      UnitNames[Unt::Bitmask],
                                                      UnitNames[Unt::Bitmask]
                                                   };

/** This is the list of the Photon TAG attributes group names*/
static const std::string PhotonAttributeGroupNames[PhotonID::NphotonAttr] = {
                                                       GroupNames[Grp::PhysicsPhoton],
                                                       GroupNames[Grp::PhysicsPhoton],
                                                       GroupNames[Grp::PhysicsPhoton],
                                                       GroupNames[Grp::PhysicsPhoton],
                                                       GroupNames[Grp::PhysicsPhoton],
                                                       GroupNames[Grp::PhysicsPhoton],
                                                       GroupNames[Grp::PhysicsPhoton]
                                                   };

#endif
