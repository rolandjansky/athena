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
 * @author Mark Owen (markowen@cern.ch)
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

  /// Require that EF muons are combined (should move to pass bits)
  bool m_requireCombined;

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
  std::vector<double> m_idtrkdz;
  std::vector<double> m_idtrkdr;

  //Timers
  bool m_doMyTiming;
  TrigTimer*  m_dataPrepTime;
  TrigTimer*  m_calcTime;
  TrigTimer* m_dataOutputTime;

  void handle(const Incident &);

}; //class TrigMuonEFTrackIsolation

#endif //TRIGMUONEF_TRIGMUONEFTRACKISOLATION_H__
