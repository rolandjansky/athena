/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @file HLTTauMonTool.h
 *   Class declaration for HLTTauMonTool
 *   
 *                          
 *    Artur Kalinowski (Artur.Kalinowski@cern.ch)       
 *   
 */

#ifndef HLTTAUMONTOOL_H
#define HLTTAUMONTOOL_H

#include <string>
#include <vector>
#include <map>
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthenaMonitoring/AthenaMonManager.h"
//#include "TrigDecision/TrigDecisionTool.h"
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "tauEvent/TauJet.h"
// Forward declarations
class StatusCode;

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;
class EmTauRoI;
class TauJet;
class TrigRoiDescriptor;

class IInterface;
class StoreGateSvc;
class TH1I;
class TH1F;

class HLTTauMonTool : public IHLTMonTool {

 public:
  HLTTauMonTool(const std::string & type, const std::string & name, const IInterface* parent); 
  virtual ~HLTTauMonTool();
  
  StatusCode init();
#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode book();
#else
  StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun);
#endif
  StatusCode fill();


#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode proc();
#else
  StatusCode proc(bool endOfEventsBlock, bool endOfLumiBlock, bool endOfRun);
#endif

  /// Method fills the histograms for one tau trigger item.
  StatusCode fillHistogramsForItem(const std::string & trigItem);
  
 private:

  /// Method books the histograms for one tau trigger item.
  void bookHistogramsForItem(const std::string & trigItem);
  
  /// Method filling the L1 RoI  histograms
  StatusCode fillL1ForItem(const std::string & trigItem);

  /// Method filling the efficiency histograms
  StatusCode fillEfficiency(const std::string & trigItem);

  /// Method filling the EF tau histograms
  void fillEFTau(const xAOD::TauJet *aEFTau);

  /// Method filling the EV vs Offline tau histograms
  void fillEFTauVsOffline(const xAOD::TauJet *aEFTau);

  /// Method finds  LVL1_ROI object corresponding to  given TrigRoiDescriptor
  const xAOD::EmTauRoI * findLVL1_ROI(const TrigRoiDescriptor * roiDesc);

  /// Method for managing the histogram divisions
  void divide(TH1 *num, TH1 *den, TH1 *quo);

  float deltaPhi(float phi1, float phi2);
  float deltaR(float eta1, float eta2, float phi1, float phi2);
  float deltaR(const xAOD::TauJet* aEFTau, const xAOD::EmTauRoI* l1Tau);

  /* StatusCode OfflineEfficiency(const std::string & trigItem); */
  /* StatusCode OfflineEfficiencyBDTMedium(const std::string & trigItem); */
  bool Match_Offline_L1(const xAOD::TauJet *aOfflineTau, const std::string & trigItem);
  bool Match_Offline_EF(const xAOD::TauJet *aOfflineTau, const std::string & trigItem);

  void FillRelDiffHist(TH1 * hist, float val1, float val2, float checkVal, int checkMode);

  bool m_fakeJetEff;
  bool m_offlineEff;
  bool m_offlineEffBDTMedium;
  bool m_TracksInfo;
  ///Name of the trigger items to be monitored.
  ///Set by job options
  std::vector<std::string> m_trigItems;
  std::vector<std::string> m_primary_tau;
  std::vector<std::string> m_monitoring_tau;
  std::vector<std::string> m_prescaled_tau;

  // offline tau pt threshold for efficiency plots as a function of eta, phi, and nvtx
  double m_effOffTauPtCut;
};

#endif
