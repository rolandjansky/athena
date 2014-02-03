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
class EmTau_ROI;
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

  /// Method filling the L2 tau histograms
  void fillL2Tau(const TrigTau* aL2Tau);

  /// Method filling the L2 calo tau histograms
  void fillTrigTauCluster(const TrigTauCluster* aCluster);

  /// Method filling the L2 ID tau histograms
  void fillTrigTauTracksInfo(const TrigTauTracksInfo *aTkInfo);

  /// Method filling the EF tau histograms
  void fillEFTau(const Analysis::TauJet *aEFTau);

  /// Method filling the EV vs L2 tau histograms
    //StatusCode fillEFvsL2(const std::string& trigItem);
    void fillEFvsL2(const std::string& trigItem);
  /// Method filling the EV vs Offline tau histograms
  void fillEFTauVsOffline(const Analysis::TauJet *aEFTau);

  /// Method finds  LVL1_ROI object corresponding to  given TrigRoiDescriptor
  const EmTau_ROI * findLVL1_ROI(const TrigRoiDescriptor * roiDesc);

  /// Method for managing the histogram divisions
  void divide(TH1 *num, TH1 *den, TH1 *quo);

  StatusCode fillTagandProbe(const std::string & trigItem);
  float delta_phi(float phi1, float phi2);
  const Analysis::TauJet* OfflineTauMatchedJet(const Jet* jet);
  const Analysis::TauJet* EFTauMatchedJet(const std::string & trigItem, const Jet* jet);
  int NTracks(const Jet* jet);
  bool BackToBackJets(const Jet* jet1, const Jet* jet2);
  void FillTagJet(const Jet* leadJet1, const Jet* leadjet2,const std::string & trigItem);
  void FillProbeJet1p3p(const Jet* leadJet1, const Jet* leadjet2,const std::string & trigItem);
  void FillProbeJettauRec(const Jet* leadJet1, const Jet* leadjet2,const std::string & trigItem);

  void FillTagJetBinned(const Jet* leadJet1, const Jet* leadjet2,const std::string & trigItem);
  void FillProbeJet1p3pBinned(const Jet* leadJet1, const Jet* leadjet2,const std::string & trigItem);
  void FillProbeJettauRecBinned(const Jet* leadJet1, const Jet* leadjet2,const std::string & trigItem);

  StatusCode OfflineEfficiency(const std::string & trigItem);
  StatusCode OfflineEfficiencyBDTMedium(const std::string & trigItem);
  bool Match_Offline_L1(const Analysis::TauJet *aOfflineTau, const std::string & trigItem);
  bool Match_Offline_L2(const Analysis::TauJet *aOfflineTau, const std::string & trigItem);
  bool Match_Offline_EF(const Analysis::TauJet *aOfflineTau, const std::string & trigItem);

  void FillRelDiffHist(TH1 * hist, float val1, float val2, float checkVal, int checkMode);

  // Handle for the trig decision tool
  //  ToolHandle<TrigDec::TrigDecisionTool> m_trigDecTool;

  bool m_fakeJetEff;
  bool m_offlineEff;
  bool m_offlineEffBDTMedium;
  bool m_TracksInfo;
  ///Name of the trigger item to be monitored.
  ///Set by job options
  std::vector<std::string> m_trigItems;

  // offline tau pt threshold for efficiency plots as a function of eta, phi, and nvtx
  double m_effOffTauPtCut;
};

#endif
