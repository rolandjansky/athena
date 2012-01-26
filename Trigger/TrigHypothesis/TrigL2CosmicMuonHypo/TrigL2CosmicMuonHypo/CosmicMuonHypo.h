/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CosmicMuonHypo_h__
#define __CosmicMuonHypo_h__
/*
  CosmicMuonHypo.h
*/
#include "TrigInterfaces/HypoAlgo.h"
#include "TH1.h"
#include "TrigCosmicEvent/CosmicMuon.h"
#include "TrigCosmicEvent/CosmicMuonCollection.h"
#include <vector>

class CosmicMuonHypo : public HLT::HypoAlgo {
public:
  /** Constructor */
  CosmicMuonHypo(const std::string& name, ISvcLocator* svcloc);
  /** Destructor */
  ~CosmicMuonHypo();

  /** HLT steering hook */
  HLT::ErrorCode hltInitialize();
  /** HLT steering hook */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
  /** HLT steering hook */
  HLT::ErrorCode hltFinalize();

  /** HLT steering hook */
  HLT::ErrorCode hltBeginRun();
  /** HLT steering hook */
  HLT::ErrorCode hltEndRun();

  /** the actual selection criteria to accept the CosmicMuon or not */
  bool acceptThisMuon(const CosmicMuon& muon) const;

  /** clear internal data */
  void clearEvent();

private:
  //------------------------------------------------------------
  // Configurable properties
  //------------------------------------------------------------
  /** Minimum number of muons required to accept */
  int mNMuons;

  /** Lower boundary of eta to accept a muon */
  double mMuonEtaMin;
  /** Upper boundary of eta to accept a muon */
  double mMuonEtaMax;
  /**
     Cuts on the distance of closet approach with respect to (0,0,Z0)
     The Z-coordinate of the reference point for DCA (mm).
   */
  double mMuonDCA_Z0;
  /**
     Cuts on the distance of closet approach (xy-plane)
     Cut on the DCA in XY plane (mm)
  */
  double mMuonDCA_XY;
  /** Cut on the Z coordinate (distance from Z0) (mm) */
  double mMuonDCA_Z;
  /** Cuts on the distance of closet approach (3D) (mm) */
  double mMuonDCA_3D;

  /** Minimum number of RPC hits */
  int mNRpcHits;
  /** Minimum number of MDT stations on the track */
  int mNMdtStations;

  // for multi-muon selection
  /** Maximum allowed value for delta(eta) between top and bottom muons */
  double mUpDownDeltaEta;
  /** Maximum allowed value for delta(phi) between top and bottom muons */
  double mUpDownDeltaPhi;
  /**
     Maximum allowed value for timing difference between top and bottom muons
  */
  double mUpDownTiming;

  //------------------------------------------------------------
  /** A vector of eta of accepted muons for monitoring */
  std::vector<float> mAcceptedMuonEta;
  /** A vector of phi of accepted muons for monitoring */
  std::vector<float> mAcceptedMuonPhi;
  /** A vector of DCA in X-Y plane of accepted muons for monitoring */
  std::vector<float> mAcceptedMuonDCA_XY;
  /** A vector of DCA Z-coordinate of accepted muons for monitoring */
  std::vector<float> mAcceptedMuonDCA_Z;
  /** A vector of DCA in 3D of accepted muons for monitoring */
  std::vector<float> mAcceptedMuonDCA_3D;

};

#endif // __CosmicMuonHypo_h__
