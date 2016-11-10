/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONATTRIBUTENAMES_H
#define MUONATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Muon attributes  */
namespace MuonTAG { 
  static const int NmuonAttr=10;        // This the number of Muon TAG attributes
  static const int NumMuon = 6;        // This the max number of Muons in the TAG
  enum { NMuon=0, Pt=1, Eta=2, Phi=3, Tight=4, Isol=5, NPrecLay=6, NPrecHoleLay=7, NPhiLay=8, MomBalSign=9 }; 
}

/** This is the list of the Muon TAG attributes */
static const std::string MuonAttributeNames[MuonTAG::NmuonAttr] = {
                                                  "NLooseMuon",
                                                  "LooseMuonPt",
                                                  "LooseMuonEta",
                                                  "LooseMuonPhi",
                                                  "LooseMuonTightness",
                                                  "LooseMuonIsolation",
						  "LooseMuonPrecLay",
						  "LooseMuonPrecHoleLay",
						  "LooseMuonPhiLay",
						  "LooseMuonMomBalSig",
                                               };

static const std::string MuonAttributeUnitNames[MuonTAG::NmuonAttr] = {
                                                  UnitNames[Unt::Number],
                                                  UnitNames[Unt::Mevc],
                                                  UnitNames[Unt::Rap],
                                                  UnitNames[Unt::Rad],
                                                  UnitNames[Unt::Bitmask],
                                                  UnitNames[Unt::Bitmask],
                                                  UnitNames[Unt::Number],
                                                  UnitNames[Unt::Number],
                                                  UnitNames[Unt::Number],
                                                  UnitNames[Unt::Number]
                                               };

static const std::string MuonAttributeGroupNames[MuonTAG::NmuonAttr] = {
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon],
                                                 GroupNames[Grp::PhysicsMuon]
                                               };


#endif
