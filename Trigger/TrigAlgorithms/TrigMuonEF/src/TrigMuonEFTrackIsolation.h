/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_TRIGMUONEFTRACKISOLATION_H__
#define TRIGMUONEF_TRIGMUONEFTRACKISOLATION_H__

#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"

// fwd declare EF track isolation tool interface
class IMuonEFTrackIsolationTool;

/**
 * @class TrigMuonEFTrackIsolation
 * 
 * @brief Algorithm to extract track isolation for EF muons
 *
 * This class calculates track isolation for EF muons.
 * It uses the IMuonEFTrackIsolationTool tool interface for
 * the actual isolation calculation.
 *
 * As of Dec2016 it can also be used for L2 Muon isolation, using FTK tracks.
 * 
 * @author Mark Owen (markowen@cern.ch)
 * @author Michael Fenton (m.fenton@cern.ch
 */
class TrigMuonEFTrackIsolation : public virtual HLT::FexAlgo,
			  public virtual IIncidentListener
{
 public:

  /// Constructor
  TrigMuonEFTrackIsolation (const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  ~TrigMuonEFTrackIsolation();
  
  /// Initialize the algorithm
  HLT::ErrorCode hltInitialize();
  /// Execute - called per trigger element
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
  /// Finalize the algorithm
  HLT::ErrorCode hltFinalize();

 private:

  /// name of ID Track Particle container
  std::string m_idTrackParticlesName;
  std::string m_FTKTrackParticlesName;
  
  // Which setups to run
  int m_isoType;

  /// Require that EF muons are combined (should move to pass bits)
  bool m_requireCombined;

  /// Use offline isolation variables
  bool m_useVarIso;

  /// Name of output muon container
  std::string m_muonContName;
  
  /// flag to see if debug is enabled
  bool m_debug;

  /// cone sizes to calculate the isolation
  std::vector<double> m_coneSizes;

  /// Tool handle for isolation tool
  ToolHandle<IMuonEFTrackIsolationTool> m_efIsoTool;

  //// Monitoring Histograms
  StringProperty m_histo_path_base;     // set the histo path for Monitoring
  std::vector<double> m_trkptiso_cone2;
  std::vector<double> m_trkptiso_cone3;
  std::vector<double> m_trkdz;
  std::vector<double> m_trkdr;  
  std::vector<double> m_muon_selfpt;
  std::vector<double> m_muon_removedpt;
  std::vector<double> m_muon_combinedpt;
  std::vector<int>    m_n_trks;

  //Timers
  bool m_doMyTiming;
  TrigTimer*  m_dataPrepTime;
  TrigTimer*  m_calcTime;
  TrigTimer* m_dataOutputTime;

  void handle(const Incident &);

}; //class TrigMuonEFTrackIsolation

#endif //TRIGMUONEF_TRIGMUONEFTRACKISOLATION_H__
