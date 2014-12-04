/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMonitoring/HLTEgammaMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigCaloEvent/TrigEMCluster.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>

#include "TH1F.h"
#include "TMath.h"

HLTEgammaMonTool::HLTEgammaMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : HLTEgammaFEXBaseTool(type,name,parent) {
  m_firstTimeStamp = m_lastTimeStamp = 0;
}

StatusCode HLTEgammaMonTool::init() {
  StatusCode sc;

  if (service("ToolSvc", m_toolSvc).isFailure()){
    (*m_log) << MSG::ERROR << "Unable to get ToolSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // extrapolation -- removed 
  m_doExtrapol = false;

  return StatusCode::SUCCESS;
}

HLTEgammaMonTool::~HLTEgammaMonTool() {
}

StatusCode HLTEgammaMonTool::book() {

    addMonGroup(new MonGroup(this,"HLT/EgammaMon",run));

    addMonGroup(new MonGroup(this,"HLT/EgammaMon/L1",run));
    addHistogram(new TH1F("EmClus", "L1 EM Cluster Energy; L1 EM Cluster Energy [GeV]; Probability", 80, 0., 80.));
    addHistogram(new TH1F("EmIsol", "L1 EM Isolation Energy; L1 EM Isolation Energy [GeV]; Probability", 15, 0., 15.));
    addHistogram(new TH1F("HadCore", "L1 Hadronic Core Energy; L1 Had Core Energy [GeV]; Probability", 15, 0., 15.));
    addHistogram(new TH1F("HadIsol", "L1 Hadronic Isolation Energy ; L1 Had Isolation Energy [GeV] ; Probability", 15, 0., 15.));
    addHistogram(new TH1F("Eta", "L1 \\eta ; L1 \\eta ; Probability", 100, -2.5,2.5));
    addHistogram(new TH1F("Phi", "L1 \\phi ; L1 \\phi [rad]; Probability", 128, -3.2,3.2));
  
    addMonGroup(new MonGroup(this,"HLT/EgammaMon/L2Calo",run));
    addHistogram(new TH1F("Rcore", "L2 Calo R\\eta ; L2 Calo R\\eta ; Probability", 100, 0., 1.));
    addHistogram(new TH1F("Rstrip", "L2 Calo E_{ratio} ; L2 Calo E_{ratio} ; Probability", 100, 0., 1.));
    addHistogram(new TH1F("Et", "L2 EM Calo E_{T} ; L2 EM Calo E_{T} [GeV] ; Probability", 160, 0., 80.));
    addHistogram(new TH1F("HadEt", "L2 Hadronic Calo E_{T} ; L2 Hadronic Calo E_{T} [GeV]; Probability", 20, 0., 2.));
    addHistogram(new TH1F("eta", "L2 Calo \\eta ; L2 Calo \\eta ; Probability", 100, -2.5, 2.5));
    addHistogram(new TH1F("phi", "L2 Calo \\phi ; L2 Calo \\phi [rad] ; Probability", 128, -2.5, 3.2));

    addMonGroup(new MonGroup(this,"HLT/EgammaMon/L2IDScan",run));
    addHistogram(new TH1F("Pt", "L2 IDScan p_{T} ; p_{T} [GeV/c] ; Probability", 80, 0., 80.));
    addHistogram(new TH1F("Dphi", "L2 IDScan \\Delta\\phi (\\phi_{Calo} - \\phi_{track at Calo}); L2 IDScan \\Delta\\phi [rad] ; Probability", 128, 0., m_L2IDDphi*4));
    addHistogram(new TH1F("Deta", "L2 IDScan \\Delta\\eta (\\eta_{Calo} - \\eta_{track at Calo}); \\Delta\\eta ; Probability", 128, 0., m_L2IDDeta*4));
//    addHistogram(new TH1F("A0", "Level 2 ID (ID Scan Match) A_{0} ; A_{0} ; Probability", 128, -3., 3.));
//    addHistogram(new TH1F("Z0", "Level 2 ID (ID Scan Match) Z_{0} ; Z_{0} ; Probability", 128, -100., 100.));
    addHistogram(new TH1F("EP", "L2 IDScan E_{T}/p_{T} ; L2 IDScan E_{T}/p_{T} ; Probability", 100, 0., 6.));
    addHistogram(new TH1F("HTOverAll", "L2 IDScan High Threshold Hits over all Hits ; L2 IDScan High Threshold Hits over all Hits ; Probability", 40, 0., 1.));
    addHistogram(new TH1F("TRTHits", "L2 IDScan TRT Hits ; L2 IDScan TRT Hits ; Probability", 50, 0., 50.));
  
    addMonGroup(new MonGroup(this,"HLT/EgammaMon/L2IDSi",run));
    addHistogram(new TH1F("Pt", "L2 SiTrack p_{T} ; L2 SiTrack p_{T} [GeV/c] ; Probability", 80, 0., 80.));
    addHistogram(new TH1F("Dphi", "L2 SiTrack \\Delta\\phi (\\phi_{Calo} - \\phi_{track at Calo}); L2 SiTrack \\Delta\\phi [rad] ; Probability", 128, 0., m_L2IDDphi*4));
    addHistogram(new TH1F("Deta", "L2 SiTrack \\Delta\\eta (\\eta_{Calo} - \\eta_{track at Calo}); \\Delta\\eta ; Probability", 128, 0., m_L2IDDeta*4));
//    addHistogram(new TH1F("A0", "Level 2 ID (SiTrack Match) A_{0} ; A_{0} ; Probability", 128, -3., 3.));
//    addHistogram(new TH1F("Z0", "Level 2 ID (SiTrack Match) Z_{0} ; Z_{0} ; Probability", 128, -100., 100.));
    addHistogram(new TH1F("EP", "L2 SiTrack E_{T}/p_{T} ; L2 SiTrack E_{T}/p_{T} ; Probability", 100, 0., 6.));
    addHistogram(new TH1F("HTOverAll", "L2 SiTrack High Threshold Hits over all Hits ; L2 SiTrack High Threshold Hits over all Hits ; Probability", 40, 0., 1.));
    addHistogram(new TH1F("TRTHits", "L2 SiTrack TRT Hits ; L2 SiTrack TRT Hits ; Probability", 50, 0., 50.));
 
    addMonGroup(new MonGroup(this,"HLT/EgammaMon/EFScanCalo",run));
    addHistogram(new TH1F("Et", "EF Calo E_{T} (for  L2 IDScan chains) ; E_{T} [GeV] ; Probability", 80, 0, 80.));
    addHistogram(new TH1F("Dphi", "EF Calo \\Delta\\phi (\\phi_{Calo} - \\phi_{L2 IDScan}) ; \\Delta\\phi [rad] ; Probability", 128, 0., 2*m_EFCaloDphi));
    addHistogram(new TH1F("Deta", "EF Calo \\Delta\\eta (\\eta_{Calo} - \\eta_{L2 IDScan}) ; \\Delta\\eta ; Probability", 128, 0., 2*m_EFCaloDeta));

    addMonGroup(new MonGroup(this,"HLT/EgammaMon/EFSiCalo",run));
    addHistogram(new TH1F("Et", "EF Calo E_{T} (for L2 SiTrack chains) ; E_{T} [GeV] ; Probability", 80, 0, 80.));
    addHistogram(new TH1F("Dphi", "EF Calo \\Delta\\phi (\\phi_{Calo} - \\phi_{L2 SiTrack}) ; \\Delta\\phi [rad] ; Probability", 128, 0., 2*m_EFCaloDphi));
    addHistogram(new TH1F("Deta", "EF Calo \\Delta\\eta (\\eta_{Calo} - \\eta_{L2 SiTrack}) ; \\Delta\\eta ; Probability", 128, 0., 2*m_EFCaloDeta));
 
    addMonGroup(new MonGroup(this,"HLT/EgammaMon/EFScanID",run));
    addHistogram(new TH1F("Phi", "EF Track \\phi (for L2 IDScan chains) ; EF Track \\phi ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Eta", "EF Track \\eta (for L2 IDScan chains) ; EF Track \\eta ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Pt", "EF Track p_{T} (for L2 IDScan chains) ; EF Track p_{T} [GeV/c] ; Probability", 100, 0, 80.));
//    addHistogram(new TH1F("D0", "d0 for EF Track (after L2 ID Scan Match) ; d0 [mm] ; Probability", 100, -0.5, 0.5));
//    addHistogram(new TH1F("Z0", "z0 for EF Track (after L2 ID Scan Match) ; z0 [mm] ; Probability", 100, -200, 200));
    addHistogram(new TH1F("Qual", "EF Track Quality (for L2 IDScan chains) ; EF Track Quality ; Probability", 60, 0., 60.));
    addHistogram(new TH1F("Pixel", "EF Track Number of Pixel Hits (for L2 IDScan chains) ; EF Track Number of Pixel Hits ; Probability", 8, 0., 8.));
    addHistogram(new TH1F("SCT", "EF Track Number of SCT Hits (for L2 IDScan chains) ; EF Track Number of SCT Hits ; Probability", 14, 0., 14.));
    addHistogram(new TH1F("TRT", "EF Track Number of TRT Hits (for L2 IDScan chains) ; EF Track Number of TRT Hits ; Probability", 10, 0., 10.));
    addHistogram(new TH1F("BLayer", "EF Track Number of B-Layer Hits (for L2 IDScan chains) ; EF Track Number of B-Layer Hits ; Probability", 5, 0., 5.));

    addMonGroup(new MonGroup(this,"HLT/EgammaMon/EFSiID",run));
    addHistogram(new TH1F("Phi", "\\phi for EF Track (after L2 ID SiTrack Match) ; \\phi ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Eta", "\\eta for EF Track (after L2 ID SiTrack Match) ; \\eta ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Pt", "p_{T} for EF Track (after L2 ID SiTrack Match) ; P_T [GeV] ; Probability", 100, 0, 80.));
//    addHistogram(new TH1F("D0", "d0 for EF Track (after L2 ID SiTrack Match) ; d0 [mm] ; Probability", 100, -0.5, 0.5));
//    addHistogram(new TH1F("Z0", "z0 for EF Track (after L2 ID SiTrack Match) ; z0 [mm] ; Probability", 100, -200, 200));
    addHistogram(new TH1F("Qual", "EF Track Quality (for L2 SiTrack chains) ; EF Track Quality ; Probability", 60, 0., 60.));
    addHistogram(new TH1F("Pixel", "EF Track Number of Pixel Hits (for L2 SiTrack chains) ; EF Track Number of Pixel Hits ; Probability", 8, 0., 8.));
    addHistogram(new TH1F("SCT", "EF Track Number of SCT Hits (for L2 SiTrack chains) ; EF Track Number of SCT Hits ; Probability", 14, 0., 14.));
    addHistogram(new TH1F("TRT", "EF Track Number of TRT Hits (for L2 SiTrack chains) ; EF Track Number of TRT Hits ; Probability", 10, 0., 10.));
    addHistogram(new TH1F("BLayer", "EF Track Number of B-Layer Hits (for L2 SiTrack chains) ; EF Track Number of B-Layer Hits ; Probability", 5, 0., 5.));

    addMonGroup(new MonGroup(this,"HLT/EgammaMon/Rates",run));
    addHistogram(new TH1F("L1Et", "Level 1 E_{T} ; E_{T} [GeV] ; Rate [Hz]", 80, 0., 80.));
    addHistogram(new TH1F("L1Eta", "Level 1 \\eta ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("L1Phi", "Level 1 \\phi ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("L2CaloEt", "Level 2 Calo E_{T} ; E_{T} [GeV] ; Rate [Hz]", 80, 0., 80.));
    addHistogram(new TH1F("L2CaloEta", "Level 2 Calo \\eta ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2CaloPhi", "Level 2 Calo \\phi ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("L2IDScanEt", "p_{T} after Level 2 ID (ID Scan Match) ; p_{T} [GeV] ; Rate [Hz]", 80, 0., 80.));
    addHistogram(new TH1F("L2IDScanPhi", "\\phi after Level 2 ID (ID Scan Match) ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2IDScanEta", "\\eta after Level 2 ID (ID Scan Match) ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("L2IDSiEt", "p_{T} after Level 2 ID (SiTrack Match) ; p_{T} [GeV] ; Rate [Hz]", 80, 0., 80.));
    addHistogram(new TH1F("L2IDSiPhi", "\\phi after Level 2 ID (SiTrack Match) ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2IDSiEta", "\\eta after Level 2 ID (SiTrack Match) ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
  
    // EF Calo
    addHistogram(new TH1F("EFScanCaloEt", "E_{T} at EF Calo (after L2 ID Scan Match) ; E_{T} [GeV] ; Rate [Hz]", 80, 0, 80.));
    addHistogram(new TH1F("EFScanCaloEta", "\\eta at EF Calo (after L2 ID Scan Match) ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFScanCaloPhi", "\\phi at EF Calo (after L2 ID Scan Match) ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("EFSiCaloEt", "E_{T} at EF Calo (after L2 ID SiTrack Match) ; E_{T} [GeV] ; Rate [Hz]", 80, 0, 80.));
    addHistogram(new TH1F("EFSiCaloEta", "\\eta at EF Calo (after L2 ID SiTrack Match) ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFSiCaloPhi", "\\phi at EF Calo (after L2 ID SiTrack Match) ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));

    // EF ID
    addHistogram(new TH1F("EFScanIDEt", "E_{T} after EF Track (and L2 ID Scan Match) ; E_{T} [GeV] ; Rate [Hz]", 80, 0, 80.));
    addHistogram(new TH1F("EFScanIDPhi", "\\phi after EF Track (and L2 ID Scan Match) ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFScanIDEta", "\\eta after EF Track (and L2 ID Scan Match) ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("EFSiIDEt", "E_{T} after EF Track (and L2 ID SiTrack Match) ; E_{T} [GeV] ; Rate [Hz]", 80, 0, 80.));
    addHistogram(new TH1F("EFSiIDPhi", "\\phi after EF Track (and L2 ID SiTrack Match) ; \\phi [rad] ; Rate [Hz]", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFSiIDEta", "\\eta after EF Track (and L2 ID SiTrack Match) ; \\eta ; Rate [Hz]", 128, -3.2, 3.2));
  
    addMonGroup(new MonGroup(this,"HLT/EgammaMon/PreRelativeEfficiency",run));
    addHistogram(new TH1F("L1Et", "Level 1 E_{T} ; E_{T} [GeV] ; Rate", 80, 0., 80.));
    addHistogram(new TH1F("L1Eta", "Level 1 \\eta ; \\eta ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("L1Phi", "Level 1 \\phi ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("L2CaloEt", "Level 2 E_{T} ; E_{T} [GeV] ; Rate", 80, 0., 80.));
    addHistogram(new TH1F("L2CaloEta", "Level 2 \\eta ; \\eta ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2CaloPhi", "Level 2 \\phi ; \\ phi [rad] ; Rate", 128, -3.2, 3.2));
  
    // For ID Efficiencies
    addHistogram(new TH1F("L2IDScanEt", "E_{T} after Level 2 ID (ID Scan Match) ; E_{T} [GeV] ; Rate", 80, 0., 80.));
    addHistogram(new TH1F("L2IDScanPhi", "\\phi after Level 2 ID (ID Scan Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2IDScanEta", "\\eta after Level 2 ID (ID Scan Match) ; \\eta ; Rate", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("L2IDSiEt", "E_{T} after Level 2 ID (SiTrack Match) ; E_{T} [GeV] ; Rate", 80, 0., 80.));
    addHistogram(new TH1F("L2IDSiPhi", "\\phi after Level 2 ID (SiTrack Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2IDSiEta", "\\eta after Level 2 ID (SiTrack Match) ; \\eta ; Rate", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("EFScanCaloEt", "E_{T} at EF Calo (after L2 ID Scan Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFScanCaloEta", "\\eta at EF Calo (after L2 ID Scan Match) ; \\eta ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFScanCaloPhi", "\\phi at EF Calo (after L2 ID Scan Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));

    addHistogram(new TH1F("EFSiCaloEt", "E_{T} at EF Calo (after L2 ID SiTrack Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFSiCaloEta", "\\eta at EF Calo (after L2 ID SiTrack Match) ; \\eta ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFSiCaloPhi", "\\phi at EF Calo (after L2 ID SiTrack Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    
    addHistogram(new TH1F("EFScanIDEt", "E_{T} after EF Track (and L2 ID Scan Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFScanIDPhi", "\\phi after EF Track (and L2 ID Scan Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFScanIDEta", "\\eta after EF Track (and L2 ID Scan Match) ; \\eta ; Rate", 128, -3.2, 3.2));

    addHistogram(new TH1F("EFSiIDEt", "E_{T} after EF Track (and L2 ID SiTrack Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFSiIDPhi", "\\phi after EF Track (and L2 ID SiTrack Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFSiIDEta", "\\eta after EF Track (and L2 ID SiTrack Match) ; \\eta ; Rate", 128, -3.2, 3.2));

    addMonGroup(new MonGroup(this,"HLT/EgammaMon/RelativeEfficiency",run));

    addHistogram(new TH1F("L2CaloEt", "Level 2 E_{T} ; E_{T} [GeV] ; Rate", 80, 0., 80.));
    addHistogram(new TH1F("L2CaloEta", "Level 2 \\eta ; \\eta ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2CaloPhi", "Level 2 \\phi ; \\ phi [rad] ; Rate", 128, -3.2, 3.2));
  
    // For ID Efficiencies
    addHistogram(new TH1F("L2IDScanEt", "E_{T} after Level 2 ID (ID Scan Match) ; E_{T} [GeV] ; Rate", 80, 0., 80.));
    addHistogram(new TH1F("L2IDScanPhi", "\\phi after Level 2 ID (ID Scan Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2IDScanEta", "\\eta after Level 2 ID (ID Scan Match) ; \\eta ; Rate", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("L2IDSiEt", "E_{T} after Level 2 ID (SiTrack Match) ; E_{T} [GeV] ; Rate", 80, 0., 80.));
    addHistogram(new TH1F("L2IDSiPhi", "\\phi after Level 2 ID (SiTrack Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("L2IDSiEta", "\\eta after Level 2 ID (SiTrack Match) ; \\eta ; Rate", 128, -3.2, 3.2));
  
    addHistogram(new TH1F("EFScanCaloEt", "E_{T} at EF Calo (after L2 ID Scan Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFScanCaloEta", "\\eta at EF Calo (after L2 ID Scan Match) ; \\eta ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFScanCaloPhi", "\\phi at EF Calo (after L2 ID Scan Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));

    addHistogram(new TH1F("EFSiCaloEt", "E_{T} at EF Calo (after L2 ID SiTrack Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFSiCaloEta", "\\eta at EF Calo (after L2 ID SiTrack Match) ; \\eta ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFSiCaloPhi", "\\phi at EF Calo (after L2 ID SiTrack Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    
    addHistogram(new TH1F("EFScanIDEt", "E_{T} after EF Track (and L2 ID Scan Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFScanIDPhi", "\\phi after EF Track (and L2 ID Scan Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFScanIDEta", "\\eta after EF Track (and L2 ID Scan Match) ; \\eta ; Rate", 128, -3.2, 3.2));

    addHistogram(new TH1F("EFSiIDEt", "E_{T} after EF Track (and L2 ID SiTrack Match) ; E_{T} [GeV] ; Rate", 80, 0, 80.));
    addHistogram(new TH1F("EFSiIDPhi", "\\phi after EF Track (and L2 ID SiTrack Match) ; \\phi [rad] ; Rate", 128, -3.2, 3.2));
    addHistogram(new TH1F("EFSiIDEta", "\\eta after EF Track (and L2 ID SiTrack Match) ; \\eta ; Rate", 128, -3.2, 3.2));
  
  return StatusCode::SUCCESS;
}

StatusCode HLTEgammaMonTool::fill() {
  StatusCode sc;

  m_nTotal++;

  const EventInfo *eventInfo = 0;
  sc = m_storeGate->retrieve(eventInfo);
  if (sc.isFailure()) {
    *m_log << MSG::ERROR << "Cannot retrieve event information for this event! Something is VERY wrong!" << endreq;
  } else {
    if (!eventInfo) {
      *m_log << MSG::ERROR << "Retrieved EventInfo object, but it is a null pointer!" << endreq;
    } else {
      if (m_firstTimeStamp == 0) {
        m_firstTimeStamp = eventInfo->event_ID()->time_stamp();
      }
      m_lastTimeStamp = eventInfo->event_ID()->time_stamp();
    }
  }

  bool doL2 = true;
  TrigEMClusterContainer::const_iterator EMClusterBegin, EMClusterEnd;
  const DataHandle<TrigEMClusterContainer> EMClusterCont;
  sc = m_storeGate->retrieve(EMClusterCont, m_EMClusterKey);
  if (sc.isFailure()) {
    (*m_log) << MSG::VERBOSE << "It wasn't possible to retrieve the TrigEMCluster Collection." << endreq;
    doL2 = false; // Don't stop proccessing events
  } else {
    EMClusterBegin = EMClusterCont->begin();
    EMClusterEnd = EMClusterCont->end();
  }
  
  bool doL2ID = true;
  const DataHandle<TrigInDetTrackCollection> lvl2t_first;
  const DataHandle<TrigInDetTrackCollection> lvl2t_last;
  sc = m_storeGate->retrieve(lvl2t_first,lvl2t_last);
  if (sc.isFailure()) {
    (*m_log) << MSG::VERBOSE << "Could not find TrigInDetTrack" << endreq;
    doL2ID = false;
  }

  bool doEFCalo = true;
  const DataHandle<CaloClusterContainer> EFCalo_i;
  const DataHandle<CaloClusterContainer> EFCalo_l;
  sc = m_storeGate->retrieve(EFCalo_i, EFCalo_l);
  if (sc.isFailure()) {
    (*m_log) << MSG::VERBOSE << "Could not find CaloClusterContainer" << endreq;
    doEFCalo = false;
  }

  bool EFScanCalo_pass = false;
  std::vector<float> RateEFScanCaloEt;
  std::vector<float> RateEFScanCaloEta;
  std::vector<float> RateEFScanCaloPhi;
  std::vector<float> EffEFScanCaloEt;
  std::vector<float> EffEFScanCaloEta;
  std::vector<float> EffEFScanCaloPhi;

  bool EFSiCalo_pass = false;
  std::vector<float> RateEFSiCaloEt;
  std::vector<float> RateEFSiCaloEta;
  std::vector<float> RateEFSiCaloPhi;
  std::vector<float> EffEFSiCaloEt;
  std::vector<float> EffEFSiCaloEta;
  std::vector<float> EffEFSiCaloPhi;

  bool doEFTrack = true;
  const DataHandle<egammaContainer> EFTrk_i;
  const DataHandle<egammaContainer> EFTrk_l;
  sc = m_storeGate->retrieve(EFTrk_i, EFTrk_l);
  if (sc.isFailure()) {
    (*m_log) << MSG::VERBOSE << "Could not retrieve egammaContainer" << endreq;
    doEFTrack = false;
  }

  if (!m_doExtrapol) {
    doEFTrack = false;
  }

  bool EFScanTrk_pass = false;
  std::vector<float> RateEFScanIDEt;
  std::vector<float> RateEFScanIDEta;
  std::vector<float> RateEFScanIDPhi;
  std::vector<float> EffEFScanIDEt;
  std::vector<float> EffEFScanIDEta;
  std::vector<float> EffEFScanIDPhi;

  bool EFSiTrk_pass = false;
  std::vector<float> RateEFSiIDEt;
  std::vector<float> RateEFSiIDEta;
  std::vector<float> RateEFSiIDPhi;
  std::vector<float> EffEFSiIDEt;
  std::vector<float> EffEFSiIDEta;
  std::vector<float> EffEFSiIDPhi;

  double caloeta, calophi;

  const Trk::Perigee* aMeasPer;
  float pt2;

  const LVL1_ROI * lvl1ROI;
  sc = m_storeGate->retrieve(lvl1ROI);
  if (sc.isFailure()) {
    (*m_log) << MSG::VERBOSE << "Could not find LVL1_ROI in StoreGate. Aborting!!!" << endreq;
    return StatusCode::SUCCESS; // Don't stop proccessing events
  }

  // Auxiliary vector for EF FEX variables
  std::map<std::string, std::vector<float> *> vectScan, vectSi;
  vectScan["Phi"] = new std::vector<float>;
  vectScan["Eta"] = new std::vector<float>;
  vectScan["Pt"] = new std::vector<float>;
//  vectScan["D0"] = new std::vector<float>;
//  vectScan["Z0"] = new std::vector<float>;
  vectScan["Qual"] = new std::vector<float>;
  vectScan["Pixel"] = new std::vector<float>;
  vectScan["SCT"] = new std::vector<float>;
  vectScan["TRT"] = new std::vector<float>;
  vectScan["BLayer"] = new std::vector<float>;


  vectSi["Phi"] = new std::vector<float>;
  vectSi["Eta"] = new std::vector<float>;
  vectSi["Pt"] = new std::vector<float>;
//  vectSi["D0"] = new std::vector<float>;
//  vectSi["Z0"] = new std::vector<float>;
  vectSi["Qual"] = new std::vector<float>;
  vectSi["Pixel"] = new std::vector<float>;
  vectSi["SCT"] = new std::vector<float>;
  vectSi["TRT"] = new std::vector<float>;
  vectSi["BLayer"] = new std::vector<float>;

  // Did it trigger LVL1?
  LVL1_ROI::emtaus_type::const_iterator itEMTau = lvl1ROI->getEmTauROIs().begin();
  LVL1_ROI::emtaus_type::const_iterator itEMTau_e = lvl1ROI->getEmTauROIs().end();
  LVL1_ROI::emtaus_type::const_iterator matchedL1;
  
  (*m_log) << MSG::DEBUG << "There are " << itEMTau_e - itEMTau << " elements to loop." << endreq;

  if (itEMTau != itEMTau_e) {
  
  for( ; itEMTau != itEMTau_e; ++itEMTau) {
    matchedL1 = itEMTau;
    
    (*m_log) << MSG::DEBUG << "Loop on LVL1_ROI::emtaus_type::const_iterator: " << matchedL1->getEMClus() << ", " << matchedL1->getEMIsol() << ", " << matchedL1->getHadCore() << ", " << matchedL1->getHadIsol() << endreq;

    // Fill Histograms for L1 info
    FillLVL1(*matchedL1, "HLT/EgammaMon/L1");

    if ( ( (matchedL1->getEMClus() > m_L1EmClus) &&
           (matchedL1->getEMIsol() <= m_L1EmIsol) &&
           (matchedL1->getHadCore() <= m_L1HadCore) &&
           (matchedL1->getHadIsol() <= m_L1HadIsol)) ||
         (m_passThroughL1) ) {
      // Triggered LVL 1!
      
      (*m_log) << MSG::DEBUG << "Triggered LVL1" << endreq;
      m_nL1++;
      
      setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
      hist("L1Et")->Fill(matchedL1->et()/1e3);
      hist("L1Eta")->Fill(matchedL1->getEta());
      hist("L1Phi")->Fill(matchedL1->getPhi());
      
      setCurrentMonGroup("HLT/EgammaMon/Rates");
      hist("L1Et")->Fill(matchedL1->et()/1e3);
      hist("L1Eta")->Fill(matchedL1->getEta());
      hist("L1Phi")->Fill(matchedL1->getPhi());
      
      if (doL2) {
        // Finds closest (eta, phi)
	TrigEMClusterContainer::const_iterator theEMCluster = EMClusterBegin;
	TrigEMCluster *matchedL2 = 0;
	double DetaL2 = 0.1;
	double DphiL2 = 0.1;
	for (; theEMCluster != EMClusterEnd; theEMCluster++) {
	  if ( (fabsf((*theEMCluster)->eta() - matchedL1->getEta()) < DetaL2) &&
	       (delta((*theEMCluster)->phi() - matchedL1->getPhi()) < DphiL2) ) {
	    DetaL2 = fabsf((*theEMCluster)->eta() - matchedL1->getEta());
	    DphiL2 = delta((*theEMCluster)->phi() - matchedL1->getPhi());
	    matchedL2 = const_cast<TrigEMCluster *>(*theEMCluster);
	  }
	}
	if (matchedL2) {
	  // This is the right one!
	  // This one activated level 2!!!
	  
          (*m_log) << MSG::DEBUG << "Matched LVL2" << endreq;

	  // Fill FEX histograms
	  FillLVL2Calo(matchedL2, "HLT/EgammaMon/L2Calo");
	  
	  // Level 2 cut
	  if ( matchedL2->e277()!=0.0 && ((matchedL2->e237()/matchedL2->e277()) > m_L2Rcore_cut) && (matchedL2->emaxs1()+matchedL2->e2tsts1()) !=0 &&
	       (((matchedL2->emaxs1()-matchedL2->e2tsts1())/(matchedL2->emaxs1()+matchedL2->e2tsts1())) > m_L2Rstrip_cut) &&
	       ((matchedL2->energy()/cosh(matchedL2->eta())) > m_L2Et_cut) &&
	       ((matchedL2->ehad1()/cosh(matchedL2->eta())) < m_L2Ehad_cut) ) {
		 
            (*m_log) << MSG::DEBUG << "Got thru LVL2 with Et = " << (matchedL2->energy()/cosh(matchedL2->eta())) << endreq;
	    // Got thru level 2!
	    m_nL2++;
	    
            setCurrentMonGroup("HLT/EgammaMon/Rates");
	    hist("L2CaloEt")->Fill((matchedL2->energy()/cosh(matchedL2->eta()))/1.e3);
	    hist("L2CaloEta")->Fill(matchedL2->eta());
	    hist("L2CaloPhi")->Fill(matchedL2->phi());
	    
            setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
	    hist("L2CaloEt")->Fill(matchedL1->et()/1e3);
	    hist("L2CaloEta")->Fill(matchedL1->getEta());
	    hist("L2CaloPhi")->Fill(matchedL1->getPhi());
	      
	    if (doL2ID) {
	    
	      float Pt, Eta, Phi, HTOverAll, TRTHits;
	      
	      if (runTrack(lvl2t_first, lvl2t_last, matchedL2->eta(), matchedL2->phi(), Pt, Eta, Phi, HTOverAll, TRTHits, TrigInDetTrack::IDSCANID)) {
		FillLVL2ID(Pt, Eta, Phi, HTOverAll, TRTHits, matchedL2, "HLT/EgammaMon/L2IDScan");
		
		if ( (fabsf(Pt) > m_L2IDPt) &&
		     (fabsf(Eta - matchedL2->eta()) < m_L2IDDeta) &&
		     (delta(Phi - matchedL2->phi()) < m_L2IDDphi) &&
		     (fabsf(matchedL2->energy()/cosh(matchedL2->eta())/Pt) < m_L2IDEP) ) {
		  // Got thru L2ID cut!
		  
                  setCurrentMonGroup("HLT/EgammaMon/Rates");
		  hist("L2IDScanEt")->Fill(fabsf(Pt)/1e3);
		  hist("L2IDScanEta")->Fill(Eta);
		  hist("L2IDScanPhi")->Fill(Phi);
		  
		  m_nL2IDScan++;
		  
                  setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
	          hist("L2IDScanEt")->Fill(matchedL1->et()/1e3);
	          hist("L2IDScanEta")->Fill(matchedL1->getEta());
	          hist("L2IDScanPhi")->Fill(matchedL1->getPhi());

		  // EF Calo
		  if (doEFCalo) {
		    (*m_log) << MSG::DEBUG << "doEFCalo" << endreq;
		    CaloClusterContainer::const_iterator CCItr;
		    const CaloCluster *matchedEFCalo = 0;
		    float DetaEFCalo = 99;
		    float DphiEFCalo = 99;
		    const DataHandle<CaloClusterContainer> EFCalo_f = EFCalo_i;
		    for (;EFCalo_f != EFCalo_l;
			 EFCalo_f++) {
		      if (EFCalo_f.key().find("HLT_TrigCaloClusterMaker") != std::string::npos) {
		        CCItr = EFCalo_f->begin();
		        for (; CCItr != EFCalo_f->end(); CCItr++) {
		  	  if ( (fabsf((*CCItr)->eta() - matchedL2->eta()) < DetaEFCalo) &&
			       (delta((*CCItr)->phi() - matchedL2->phi()) < DphiEFCalo) ) {
			    DetaEFCalo = fabsf((*CCItr)->eta() - matchedL2->eta());
			    DphiEFCalo = delta((*CCItr)->phi() - matchedL2->phi());
			    matchedEFCalo = (*CCItr);
			  }
		        }
	              }
		    }
		    if (matchedEFCalo) {
		      (*m_log) << MSG::DEBUG << "matchedEFCalo (Scan)" << endreq;
		      FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "HLT/EgammaMon/EFScanCalo");

		      if ( (DetaEFCalo < m_EFCaloDeta) &&
			   (DphiEFCalo < m_EFCaloDphi) &&
			   (matchedEFCalo->et() > m_EFCaloEt) ) {
			// WHOLE Event thru EF Calo
			EFScanCalo_pass = true;
		        (*m_log) << MSG::DEBUG << "roi trigger EFCalo (Scan)" << endreq;
		      }
		      RateEFScanCaloEt.push_back(matchedEFCalo->et()/1e3);
		      RateEFScanCaloEta.push_back(matchedEFCalo->eta());
		      RateEFScanCaloPhi.push_back(matchedEFCalo->phi());
		      EffEFScanCaloEt.push_back(matchedL1->et()/1e3);
		      EffEFScanCaloEta.push_back(matchedL1->getEta());
		      EffEFScanCaloPhi.push_back(matchedL1->getPhi());

		      // Do EF Track
		      // Must do it even if EFScanCalo doesn't pass in this iteration,
		      // because the whole event might pass on another one
		      if (doEFTrack) {
			egammaContainer::const_iterator eCItr;
			const DataHandle<egammaContainer> EFTrk_f = EFTrk_i;
			const egamma *matchedEFTrk = 0;
			float DetaEFTrk = 99;
			float DphiEFTrk = 99;
			for (; EFTrk_f != EFTrk_l; EFTrk_f++) {
			  if (EFTrk_f.key().find("HLT_") != std::string::npos) {
			    eCItr = EFTrk_f->begin();
			    for (; eCItr != EFTrk_f->end(); eCItr++) {
			      if ((*eCItr)->trackParticle()) {
				extrapolateToFirstSurface((*eCItr)->trackParticle(), caloeta, calophi);
				if ( (fabsf(caloeta - matchedL2->eta()) < DetaEFTrk) &&
				     (delta(calophi - matchedL2->phi()) < DphiEFTrk) ) {
				  DetaEFTrk = fabsf(caloeta - matchedL2->eta());
				  DphiEFTrk = delta(calophi - matchedL2->phi());
				  matchedEFTrk = (*eCItr);
				}
			      }
			    }
			  }
			}
			if (matchedEFTrk) {
			  extrapolateToFirstSurface(matchedEFTrk->trackParticle(), caloeta, calophi);
                          const EMTrackMatch *emTrackMatch = matchedEFTrk->detail<EMTrackMatch>();
                          const Trk::Perigee *per = dynamic_cast<const Trk::Perigee *>(&(matchedEFTrk->trackParticle()->definingParameters()));
                          const CaloCluster *EFTrackCluster = matchedEFTrk->cluster();
			  float EoverP = 0.;
			  if ( per->momentum().mag()!= 0. )
                          EoverP = EFTrackCluster->energy()/per->momentum().mag();

			  if (emTrackMatch && EFTrackCluster) {
			    if ( (matchedEFTrk->trackParticle()->trackSummary()) &&
				 (fabsf(emTrackMatch->deltaEta(2)) < m_EFTrackDeta) &&
				 (delta(emTrackMatch->deltaPhi(1)) < m_EFTrackDphi) ) {
			      if (fabsf(EFTrackCluster->eta() > m_EFTrackEtaCut) ) {
				// high eta
				if ( (EoverP > m_EFTrackLowEoPHighEta) && (EoverP < m_EFTrackHighEoPHighEta) ) {
				  EFScanTrk_pass = true;
				}
			      } else {
				// low eta
				if ( (EoverP > m_EFTrackLowEoPLowEta) && (EoverP < m_EFTrackHighEoPLowEta) ) {
				  EFScanTrk_pass = true;
				}
			      }
			    }
			  }
			  FillEFTrack(matchedEFTrk, vectScan);
			  
			  aMeasPer = matchedEFTrk->trackParticle()->perigee();
			  pt2 = pow(aMeasPer->parameters()[Trk::px], 2.) + pow(aMeasPer->parameters()[Trk::py], 2.);

			  RateEFScanIDEt.push_back(sqrt(pt2));
			  RateEFScanIDEta.push_back(caloeta);
			  RateEFScanIDPhi.push_back(calophi);
			  EffEFScanIDEt.push_back(matchedL1->et()/1e3);
			  EffEFScanIDEta.push_back(matchedL1->getEta());
			  EffEFScanIDPhi.push_back(matchedL1->getPhi());
			}
		      }
		    }
		  }
		}
	      }
	      
	      if (runTrack(lvl2t_first, lvl2t_last, matchedL2->eta(), matchedL2->phi(), Pt, Eta, Phi, HTOverAll, TRTHits, TrigInDetTrack::SITRACKID)) {
		FillLVL2ID(Pt, Eta, Phi, HTOverAll, TRTHits, matchedL2, "HLT/EgammaMon/L2IDSi");
		
		if ( (fabsf(Pt) > m_L2IDPt) &&
		     (fabsf(Eta - matchedL2->eta()) < m_L2IDDeta) &&
		     (delta(Phi - matchedL2->phi()) < m_L2IDDphi) &&
		     (fabsf(matchedL2->energy()/cosh(matchedL2->eta())/Pt) < m_L2IDEP) ) {
		  // Got thru L2ID cut!
                    setCurrentMonGroup("HLT/EgammaMon/Rates");
		    hist("L2IDSiEt")->Fill(fabsf(Pt)/1e3);
		    hist("L2IDSiEta")->Fill(Eta);
		    hist("L2IDSiPhi")->Fill(Phi);
		  
                    setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
	            hist("L2IDSiEt")->Fill(matchedL1->et()/1e3);
	            hist("L2IDSiEta")->Fill(matchedL1->getEta());
	            hist("L2IDSiPhi")->Fill(matchedL1->getPhi());
		    
		  m_nL2IDSi++;
		  
		  // EF Calo
		  if (doEFCalo) {
		    CaloClusterContainer::const_iterator CCItr;
		    const CaloCluster *matchedEFCalo = 0;
		    float DetaEFCalo = 99;
		    float DphiEFCalo = 99;
		    const DataHandle<CaloClusterContainer> EFCalo_f = EFCalo_i;
		    for (;EFCalo_f != EFCalo_l;
			 EFCalo_f++) {
	              if (EFCalo_f.key().find("HLT_TrigCaloClusterMaker") != std::string::npos) {
		        CCItr = EFCalo_f->begin();
		        for (; CCItr != EFCalo_f->end(); CCItr++) {
		  	  if ( (fabsf((*CCItr)->eta() - matchedL2->eta()) < DetaEFCalo) &&
			       (delta((*CCItr)->phi() - matchedL2->phi()) < DphiEFCalo) ) {
			    DetaEFCalo = fabsf((*CCItr)->eta() - matchedL2->eta());
			    DphiEFCalo = delta((*CCItr)->phi() - matchedL2->phi());
			    matchedEFCalo = (*CCItr);
			  }
		        }
                      }
		    }
		    if (matchedEFCalo) {
		      FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, "HLT/EgammaMon/EFSiCalo");

		      if ( (DetaEFCalo < m_EFCaloDeta) &&
			   (DphiEFCalo < m_EFCaloDphi) &&
			   (matchedEFCalo->et() > m_EFCaloEt) ) {
			// WHOLE Event thru EF Calo
			EFSiCalo_pass = true;
		      }
		      RateEFSiCaloEt.push_back(matchedEFCalo->et()/1e3);
		      RateEFSiCaloEta.push_back(matchedEFCalo->eta());
		      RateEFSiCaloPhi.push_back(matchedEFCalo->phi());
		      EffEFSiCaloEt.push_back(matchedL1->et()/1e3);
		      EffEFSiCaloEta.push_back(matchedL1->getEta());
		      EffEFSiCaloPhi.push_back(matchedL1->getPhi());

		      // Do EF Track
		      // Must do it even if EFSiCalo doesn't pass in this iteration,
		      // because the whole event might pass on another one
		      if (doEFTrack) {
			egammaContainer::const_iterator eCItr;
			const DataHandle<egammaContainer> EFTrk_f = EFTrk_i;
			const egamma *matchedEFTrk = 0;
			float DetaEFTrk = 99;
			float DphiEFTrk = 99;
			for (; EFTrk_f != EFTrk_l; EFTrk_f++) {
			  if (EFTrk_f.key().find("HLT_") != std::string::npos) {
			    eCItr = EFTrk_f->begin();
			    for (; eCItr != EFTrk_f->end(); eCItr++) {
			      if ((*eCItr)->trackParticle()) {
				extrapolateToFirstSurface((*eCItr)->trackParticle(), caloeta, calophi);
				if ( (fabsf(caloeta - matchedL2->eta()) < DetaEFTrk) &&
				     (delta(calophi - matchedL2->phi()) < DphiEFTrk) ) {
				  DetaEFTrk = fabsf(caloeta - matchedL2->eta());
				  DphiEFTrk = delta(calophi - matchedL2->phi());
				  matchedEFTrk = (*eCItr);
				}
			      }
			    }
			  }
			}
			if (matchedEFTrk) {
			  extrapolateToFirstSurface(matchedEFTrk->trackParticle(), caloeta, calophi);
                          const EMTrackMatch *emTrackMatch = matchedEFTrk->detail<EMTrackMatch>();
                          const Trk::Perigee *per = dynamic_cast<const Trk::Perigee *>(&(matchedEFTrk->trackParticle()->definingParameters()));
                          const CaloCluster *EFTrackCluster = matchedEFTrk->cluster();
			  float EoverP = 0.;
			  if ( per->momentum().mag() !=0. )
                            EoverP = EFTrackCluster->energy()/per->momentum().mag();
			  if (emTrackMatch && EFTrackCluster) {
			    if ( (matchedEFTrk->trackParticle()->trackSummary()) &&
				 (fabsf(emTrackMatch->deltaEta(2)) < m_EFTrackDeta) &&
				 (delta(emTrackMatch->deltaPhi(1)) < m_EFTrackDphi) ) {
			      if (fabsf(EFTrackCluster->eta() > m_EFTrackEtaCut) ) {
				// high eta
				if ( (EoverP > m_EFTrackLowEoPHighEta) && (EoverP < m_EFTrackHighEoPHighEta) ) {
				  EFSiTrk_pass = true;
				}
			      } else {
				// low eta
				if ( (EoverP > m_EFTrackLowEoPLowEta) && (EoverP < m_EFTrackHighEoPLowEta) ) {
				  EFSiTrk_pass = true;
				}
			      }
			    }
			  }
                          FillEFTrack(matchedEFTrk, vectSi);
			  
			  aMeasPer = matchedEFTrk->trackParticle()->perigee();
			  pt2 = pow(aMeasPer->parameters()[Trk::px], 2.) + pow(aMeasPer->parameters()[Trk::py], 2.);

			  RateEFSiIDEt.push_back(sqrt(pt2));
			  RateEFSiIDEta.push_back(caloeta);
			  RateEFSiIDPhi.push_back(calophi);
			  EffEFSiIDEt.push_back(matchedL1->et()/1e3);
			  EffEFSiIDEta.push_back(matchedL1->getEta());
			  EffEFSiIDPhi.push_back(matchedL1->getPhi());
			}
		      }
		    }
		  }
		}
	      }
	    } // do L2 ID
	  } // level 2 cut
	} // activated Level 2
      } // do L2
    } // triggered Level 1
  } // activated Level 1

  } // check LVL1 iterator

  // Add EF stuff to histos
  if (doEFCalo) {
    if (EFScanCalo_pass) {
      for (unsigned int counter = 0; counter < RateEFScanCaloEt.size(); counter++) {
          setCurrentMonGroup("HLT/EgammaMon/Rates");
	  hist("EFScanCaloEt")->Fill(RateEFScanCaloEt.at(counter));
	  hist("EFScanCaloEta")->Fill(RateEFScanCaloEta.at(counter));
	  hist("EFScanCaloPhi")->Fill(RateEFScanCaloPhi.at(counter));
	  
          setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
	  hist("EFScanCaloEt")->Fill(EffEFScanCaloEt.at(counter));
	  hist("EFScanCaloEta")->Fill(EffEFScanCaloEta.at(counter));
	  hist("EFScanCaloPhi")->Fill(EffEFScanCaloPhi.at(counter));
      }
      m_nEFScanCalo += RateEFScanCaloEt.size();

      if (doEFTrack) {
	for (unsigned int counter = 0; counter < RateEFScanIDEt.size(); counter++) {
	  if (EFScanTrk_pass) {
              setCurrentMonGroup("HLT/EgammaMon/Rates");
              hist("EFScanIDEt")->Fill(RateEFScanIDEt.at(counter));
              hist("EFScanIDEta")->Fill(RateEFScanIDEta.at(counter));
              hist("EFScanIDPhi")->Fill(RateEFScanIDPhi.at(counter));
              setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
              hist("EFScanIDEt")->Fill(EffEFScanIDEt.at(counter));
              hist("EFScanIDEta")->Fill(EffEFScanIDEta.at(counter));
              hist("EFScanIDPhi")->Fill(EffEFScanIDPhi.at(counter));
	  }
	}
	FillEFIDHistos(vectScan, "HLT/EgammaMon/EFScanID");
	if (EFScanTrk_pass)
	  m_nEFScanTrack += RateEFScanIDEt.size();
      }
    } 
    
    if (EFSiCalo_pass) {
      for (unsigned int counter = 0; counter < RateEFSiCaloEt.size(); counter++) {
          setCurrentMonGroup("HLT/EgammaMon/Rates");
	  hist("EFSiCaloEt")->Fill(RateEFSiCaloEt.at(counter));
	  hist("EFSiCaloEta")->Fill(RateEFSiCaloEta.at(counter));
	  hist("EFSiCaloPhi")->Fill(RateEFSiCaloPhi.at(counter));
          setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
	  hist("EFSiCaloEt")->Fill(EffEFSiCaloEt.at(counter));
	  hist("EFSiCaloEta")->Fill(EffEFSiCaloEta.at(counter));
	  hist("EFSiCaloPhi")->Fill(EffEFSiCaloPhi.at(counter));
      }
      m_nEFSiCalo += RateEFSiCaloEt.size();

      if (doEFTrack) {
	for (unsigned int counter = 0; counter < RateEFSiIDEt.size(); counter++) {
	  if (EFScanTrk_pass) {
              setCurrentMonGroup("HLT/EgammaMon/Rates");
              hist("EFSiIDEt")->Fill(RateEFSiIDEt.at(counter));
              hist("EFSiIDEta")->Fill(RateEFSiIDEta.at(counter));
              hist("EFSiIDPhi")->Fill(RateEFSiIDPhi.at(counter));
              setCurrentMonGroup("HLT/EgammaMon/PreRelativeEfficiency");
              hist("EFSiIDEt")->Fill(EffEFSiIDEt.at(counter));
              hist("EFSiIDEta")->Fill(EffEFSiIDEta.at(counter));
              hist("EFSiIDPhi")->Fill(EffEFSiIDPhi.at(counter));
	  }
	}
	FillEFIDHistos(vectSi, "HLT/EgammaMon/EFSiID");
	if (EFSiTrk_pass)
	  m_nEFSiTrack += RateEFSiIDEt.size();
      }
    }
  }

  for (std::map<std::string, std::vector<float> *>::iterator i = vectScan.begin(); i != vectScan.end(); i++) {
    if (i->second)
      delete i->second;
  }
  for (std::map<std::string, std::vector<float> *>::iterator i = vectSi.begin(); i != vectSi.end(); i++) {
    if (i->second)
      delete i->second;
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTEgammaMonTool::proc() {
  if (endOfRun) {
    if (m_lastTimeStamp != m_firstTimeStamp) {
      float factor = 1.0/fabsf((float) (m_lastTimeStamp - m_firstTimeStamp)); // Get rates! (no idea of what happens to SIMULATED data! Should work well on real data)

      setCurrentMonGroup("HLT/EgammaMon/Rates");
      scaleError(factor, hist("L1Et"));
      scaleError(factor, hist("L1Eta"));
      scaleError(factor, hist("L1Phi"));

      scaleError(factor, hist("L2CaloEt"));
      scaleError(factor, hist("L2CaloEta"));
      scaleError(factor, hist("L2CaloPhi"));

      scaleError(factor, hist("L2IDScanEt"));
      scaleError(factor, hist("L2IDScanEta"));
      scaleError(factor, hist("L2IDScanPhi"));
 
      scaleError(factor, hist("L2IDSiEt"));
      scaleError(factor, hist("L2IDSiEta"));
      scaleError(factor, hist("L2IDSiPhi"));
   
      scaleError(factor, hist("EFScanCaloEt"));
      scaleError(factor, hist("EFScanCaloEta"));
      scaleError(factor, hist("EFScanCaloPhi"));

      scaleError(factor, hist("EFSiCaloEt"));
      scaleError(factor, hist("EFSiCaloEta"));
      scaleError(factor, hist("EFSiCaloPhi"));
 
      scaleError(factor, hist("EFScanIDEt"));
      scaleError(factor, hist("EFScanIDEta"));
      scaleError(factor, hist("EFScanIDPhi"));

      scaleError(factor, hist("EFSiIDEt"));
      scaleError(factor, hist("EFSiIDEta"));
      scaleError(factor, hist("EFSiIDPhi"));
    }
    
    divide(hist("L2CaloEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L1Et", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloEt", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("L2CaloEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L1Eta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloEta", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("L2CaloPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L1Phi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloPhi", "HLT/EgammaMon/RelativeEfficiency") );
    
    divide(hist("L2IDScanEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanEt", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("L2IDScanEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanEta", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("L2IDScanPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanPhi", "HLT/EgammaMon/RelativeEfficiency") );
    
    divide(hist("L2IDSiEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiEt", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("L2IDSiEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiEta", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("L2IDSiPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2CaloPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiPhi", "HLT/EgammaMon/RelativeEfficiency") );
    
    divide(hist("EFScanCaloEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFScanCaloEt", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFScanCaloEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFScanCaloEta", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFScanCaloPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFScanCaloPhi", "HLT/EgammaMon/RelativeEfficiency") );
    
    divide(hist("EFSiCaloEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFSiCaloEt", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFSiCaloEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFSiCaloEta", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFSiCaloPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFSiCaloPhi", "HLT/EgammaMon/RelativeEfficiency") );

    divide(hist("EFScanIDEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFScanIDEt", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFScanIDEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFScanIDEta", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFScanIDPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDScanPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFScanIDPhi", "HLT/EgammaMon/RelativeEfficiency") );
    
    divide(hist("EFSiIDEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiEt", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFSiIDEt", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFSiIDEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiEta", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFSiIDEta", "HLT/EgammaMon/RelativeEfficiency") );
    divide(hist("EFSiIDPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("L2IDSiPhi", "HLT/EgammaMon/PreRelativeEfficiency"), hist("EFSiIDPhi", "HLT/EgammaMon/RelativeEfficiency") );
    
    PrintTable();
    *m_log << MSG::DEBUG << "BEGIN TimeStamps" << endreq;
    *m_log << MSG::DEBUG << "First      " << m_firstTimeStamp << endreq;
    *m_log << MSG::DEBUG << "Last       " << m_lastTimeStamp << endreq;
    *m_log << MSG::DEBUG << "END TimeStamps" << endreq;
  }
  return StatusCode::SUCCESS;
}

