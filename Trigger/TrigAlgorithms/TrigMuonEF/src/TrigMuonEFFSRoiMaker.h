/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_TRIGMUONEFFSROIMAKER_H__
#define TRIGMUONEF_TRIGMUONEFFSROIMAKER_H__

// ************************************************
//
// NAME:     TrigMuonEFFSRoiMaker.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigMuonEF
//
// AUTHOR:   Mark Owen
// EMAIL:    markowen AT cern.ch
//
// ************************************************

#include "TrigInterfaces/AllTEAlgo.h"
#include "AthenaKernel/IClassIDSvc.h"

/**
 * Class to make ROIs from the results of an MS full scan.
 * Class takes the EDM objects stored by the full scan and
 * creates an ROI based on the eta-phi of the extrapolated track.
 * The EDM objects are re-packaged and attached to the trigger element
 * so they can be processed in standard ROI mode.
 */
class TrigMuonEFFSRoiMaker : public HLT::AllTEAlgo {

 public:

  /// Constructor
  TrigMuonEFFSRoiMaker(const std::string & name, ISvcLocator* pSvcLocator);

  /// Destructor
  ~TrigMuonEFFSRoiMaker();

  /// Initialize
  HLT::ErrorCode hltInitialize();

  /// Execute for each event
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >&,unsigned int);
  
  /// Finalize
  HLT::ErrorCode hltFinalize();

 protected:
  //Construct RoI in cone back-to-back with muon track direction.
  bool m_doInvertRoI;
  bool m_useFS;
  bool m_createCrackRoI;
  bool m_createFSroi;
  bool m_createRoIfromL1;
  
  /// minimum pT cut on extrapolated tracks - above this create ROIs.
  double m_ptcut;

  //Radius of roi in eta and phi
  double m_roiSizeEta;
  double m_roiSizePhi;

  /// Name of output xAOD::MuonContainer
  std::string m_efContName;

  /// Handle to ClassIDSvc
  ServiceHandle<IClassIDSvc> m_classIDSvc;

  /// monitor number of input tracks
  unsigned int m_nTrkIn;
  /// monitor number of output ROIs
  unsigned int m_nRoIOut;

  /// Label to store RoI with
  std::string m_roiLabel;

};//class TrigMuonEFFSRoiMaker


#endif // TRIGMUONEF_TRIGMUONEFFSROIMAKER_H__
