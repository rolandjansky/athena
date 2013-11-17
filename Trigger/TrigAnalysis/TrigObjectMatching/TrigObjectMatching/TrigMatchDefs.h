/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_MATCH_DEFS_H
#define TRIG_MATCH_DEFS_H

class Muon_ROI;
class EmTau_ROI;
class Jet_ROI;
class MuonFeature;
class TrigMuonEF;
class TrigElectron;
class egamma;
class TrigT2Jet;
class Jet;
class TrigTau;

namespace Analysis {
    class TauJet;
}

namespace TrigMatch {

  // For ease of use, these typedefs contain the objects you
  // can match to in order to determine if an offline object
  // passes a chain or not for a given trigger level

  // muons
  typedef Muon_ROI              TrigMuonL1;
  typedef MuonFeature           TrigMuonL2;
  typedef ::TrigMuonEF          TrigMuonEF;

  // electrons
  typedef EmTau_ROI             TrigElectronL1;
  typedef TrigElectron          TrigElectronL2;
  typedef egamma                TrigElectronEF;

  // photons
  typedef EmTau_ROI             TrigPhotonL1;
  typedef TrigPhoton            TrigPhotonL2;
  typedef egamma                TrigPhotonEF;

  // jets
  typedef Jet_ROI               TrigJetL1;
  typedef TrigT2Jet             TrigJetL2;
  typedef Jet                   TrigJetEF;

  // taus
  typedef EmTau_ROI             TrigTauL1;
  typedef TrigTau               TrigTauL2;
  typedef Analysis::TauJet      TrigTauEF;

}

#endif
