/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBPHYSTRACKROIMAKER_H__
#define TRIGBPHYSTRACKROIMAKER_H__

// ************************************************
//
// NAME:     TrigBphysTrackRoiMaker.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
//
// AUTHOR:  Olya Igonkina, Nikhef
//
// ************************************************

#include "TrigInterfaces/FexAlgo.h"
#include <string>

// edm classes
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticle.h"

// fwd declare TrigRoiDescriptor class
class TrigRoiDescriptor;

/**
 * @brief Algorithm to create RoIs from  ID tracks.
 * 
 */
class TrigBphysTrackRoiMaker : public virtual HLT::FexAlgo
{
 public:
  
  /// Constructor
  TrigBphysTrackRoiMaker(const std::string & name, ISvcLocator* pSvcLocator);

  /// Destructor
  ~TrigBphysTrackRoiMaker();

  /// Initialize
  HLT::ErrorCode hltInitialize();

  /// Execute for each RoI
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  
  /// Finalize
  HLT::ErrorCode hltFinalize();

  TrigRoiDescriptor* createSingleTrigRoiDescriptor( double eta, double phi, double etaWidth, double phiWidth, int roiNum );
 protected:

  /// input track Collection label
  std::string m_inputTrackCollectionKey;


  // Monitoring variables
  //
  int m_nTrksIn;
  int m_nRoiOut;
  double m_nRoiToTrkRatio;
  std::vector<double> m_ptTrkIn;
  std::vector<double> m_etaTrkIn;
  std::vector<double> m_phiTrkIn;
  std::vector<double> m_ptRoiOut;
  std::vector<double> m_etaRoiOut;
  std::vector<double> m_phiRoiOut;
  std::vector<double> m_dEtaTrkSel;
  std::vector<double> m_dPhiTrkSel;
  std::vector<double> m_dRTrkSel;
  std::vector<double> m_logdRTrkSel;



  
}; // class TrigBphysTrackRoiMaker

#endif //TRIGBPHYSTRACKROIMAKER_H__
