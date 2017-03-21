/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_TRIGMUONIDTRACKROIMAKER_H__
#define TRIGMUONEF_TRIGMUONIDTRACKROIMAKER_H__

// ************************************************
//
// NAME:     TrigMuonEFIDTrackRoiMaker.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigMuonEF
//
// AUTHOR:   Mark Owen
// EMAIL:    markowen AT cern.ch
//
// ************************************************

#include "TrigInterfaces/FexAlgo.h"
#include <string>

// edm classes
#include "xAODMuon/MuonContainer.h"

// fwd declare TrigRoiDescriptor class
class TrigRoiDescriptor;

/**
 * @brief Algorithm to create RoIs from muon ID tracks.
 * 
 * Class to make ROIs from the ID tracks of xAOD::Muons
 * Class takes the previous RoI  and creates a new one
 * centred on the muon z0, with a configurable width.
 *
 * @author Mark Owen <markowen@cern.ch>
 */
class TrigMuonEFIDTrackRoiMaker : public virtual HLT::FexAlgo
{
 public:
  
  /// Constructor
  TrigMuonEFIDTrackRoiMaker(const std::string & name, ISvcLocator* pSvcLocator);

  /// Destructor
  ~TrigMuonEFIDTrackRoiMaker();

  /// Initialize
  HLT::ErrorCode hltInitialize();

  /// Execute for each RoI
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  
  /// Finalize
  HLT::ErrorCode hltFinalize();

 protected:

  /// Function to create new RoI based on the muons
  TrigRoiDescriptor* createIDtrackRoI(const TrigRoiDescriptor& roi, const xAOD::MuonContainer& muons) ;
  
  /// ROI size in z0
  float m_roiSizeZ0;

  /// input RoI name
  std::string m_inputRoiName;

  /// output RoI name
  std::string m_outputRoiName;

  /// monitor the z position of the RoIs we make
  float m_roiZ;
  
  std::vector<float> m_deltaEta;
  std::vector<float> m_deltaPhi;

}; // class TrigMuonEFIDTrackRoiMaker

#endif //TRIGMUONEF_TRIGMUONIDTRACKROIMAKER_H__
