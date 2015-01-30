/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Implementation of BaseTool functions
 *
 *  rough sequence of functions in this source file:
 *   - booking histograms
 *   - filling objects
 *   - post processging
 *   - helper functions
 */
#include "TrigEgammaMonitoring/HLTEgammaFEXNavSigTEBaseTool.h"
#include "TrigEgammaMonitoring/HLTEgammaNavMonTool.h"

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
#include "CaloEvent/CaloClusterContainer.h"
#include "egammaEvent/EMShower.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TTree.h"

//c
HLTEgammaFEXNavSigTEBaseTool::HLTEgammaFEXNavSigTEBaseTool(const string & type, const string & name, const IInterface* parent)
  : IHLTMonTool(type,name,parent) {

    // Instantiate TrackToVertex tool
    m_trackToVertexTool = ToolHandle<Reco::ITrackToVertex>("Reco::TrackToVertex");

    // In MeV:
    declareProperty("L1EmClus", m_L1EmClus = 3.*CLHEP::GeV);
    declareProperty("L1EmIsol", m_L1EmIsol = 99.*CLHEP::GeV);
    declareProperty("L1HadCore", m_L1HadCore = 99.*CLHEP::GeV);
    declareProperty("L1HadIsol", m_L1HadIsol = 99.*CLHEP::GeV);
    declareProperty("PassThroughL1", m_passThroughL1 = false);

    declareProperty("L2Rcore", m_L2Rcore_cut = 0.3);
    declareProperty("L2Rstrip", m_L2Rstrip_cut = 0.3);
    // In MeV:
    declareProperty("L2Et", m_L2Et_cut = 3.*CLHEP::GeV);
    declareProperty("L2Ehad", m_L2Ehad_cut = 99.*CLHEP::GeV);

    declareProperty("L2IDPt", m_L2IDPt = 2.*CLHEP::GeV);
    declareProperty("L2IDEP", m_L2IDEP = 5.5);
    declareProperty("L2IDDeta", m_L2IDDeta = 0.2);
    declareProperty("L2IDDphi", m_L2IDDphi = 0.2);

    declareProperty("EFCaloEt", m_EFCaloEt = 5.*CLHEP::GeV);
    declareProperty("EFCaloDeta", m_EFCaloDeta = 0.1);
    declareProperty("EFCaloDphi", m_EFCaloDphi = 0.1);

    //TODO what is this?
    declareProperty("L", m_L = 0.191*pow(10.0, 6.0));

    declareProperty("EFTrackeCalibFactor", m_EFTrackeCalibFactor = 1.0);
    declareProperty("EFTrackDeta", m_EFTrackDeta = 0.0036);
    declareProperty("EFTrackDphi", m_EFTrackDphi = 0.037);
    declareProperty("EFTrackEtaCut", m_EFTrackEtaCut = 1.37);
    declareProperty("EFTrackLowEoPHighEta", m_EFTrackLowEoPHighEta = 0.5);
    declareProperty("EFTrackHighEoPHighEta", m_EFTrackHighEoPHighEta = 3.86);
    declareProperty("EFTrackLowEoPLowEta", m_EFTrackLowEoPLowEta = 0.5);
    declareProperty("EFTrackHighEoPLowEta", m_EFTrackHighEoPLowEta = 2.08);

    declareProperty("lowEtCut", m_lowEtCut = 3.*CLHEP::GeV);
    declareProperty("maxEtaCut", m_maxEtaCut = 2.5);

    declareProperty("doExtrapol", m_doExtrapol = false);

    declareProperty("TrigEMClusterKey", m_EMClusterKey = "HLT_TrigT2CaloEgamma");
    declareProperty("egDetailKey", m_egDetailKey = "egDetailContainer");//for AOD: egDetailAOD

    declareProperty("doActiveChain", m_doActiveChain = true);
    declareProperty("doActiveTe", m_doActiveTe = false);
    declareProperty("doAllTe", m_doAllTe = false);
    declareProperty("skipActiveTe", m_skipActiveTe = true);

    //setting histogram paths (or rather path components)
    // - note: no slashes at beginning or end, please
    declareProperty("BasicPath", m_histoBasePath="HLT/EgammaSigTE");  
    declareProperty("NoCutPath", m_allTePath="AllTEs");  
    declareProperty("ActiveTEsPath", m_activeTePath="PassedTEs");
    declareProperty("ActiveChainPath", m_activeChainPath="PassedChain");
    declareProperty("offlineElectronPath", m_offElePath = "OfflineElectron");
    declareProperty("offlinePhotonPath", m_offPhoPath = "OfflinePhoton");
    declareProperty("matchedOfflineElectronPath", m_matchedOffElePath = "MatchedOffEle");
    declareProperty("matchedOfflinePhotonPath", m_matchedOffPhoPath = "MatchedOffPho");
    declareProperty("efficiencyElectronPath", m_effElePath = "EffEle");
    declareProperty("efficiencyPhotonPath", m_effPhoPath = "EffPho");
    declareProperty("rejectionPath", m_rejectionPath = "Rejection");
    declareProperty("preRejectionPath", m_preRejectionPath = "PreRejection");
    declareProperty("lumiPath", m_lumiPath = "Lumi");


    m_nTotal = 0;
    m_nL1 = 0;
    m_nL2Calo = 0;
    m_nL2ID = 0;
    m_nEFCalo = 0;
    m_nEFeg = 0;
    m_nEFID = 0;


  }

//default destructor
// - TODO: are we sure there is nothing we can clean up here?
HLTEgammaFEXNavSigTEBaseTool::~HLTEgammaFEXNavSigTEBaseTool()
{ }




/************************************************
 *
 *           HISTOGRAM BOOKING
 *
 ***********************************************/


/*  BOOKING HISTOGRAMS PER SIGNATURE
*/
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_per_signature(const string signature)
{
  ATH_MSG_DEBUG("Starting to book histograms for signature "<<signature);

  //book selection variable histograms
  if(book_per_signature_sel(signature).isFailure()){ return(StatusCode::FAILURE); }  

  //book resolution histograms
  if(book_per_signature_res(signature).isFailure()){ return(StatusCode::FAILURE); } 

  //book rejection histograms
  if(book_per_signature_reject(signature).isFailure()){ return(StatusCode::FAILURE); }  

  //book matched offline histograms
  if(book_offline(signature).isFailure()){ return(StatusCode::FAILURE); }

  //book offline efficiency histograms
  if(book_per_signature_eff(signature).isFailure()){ return(StatusCode::FAILURE); }   

  //book luminosity-dependent histograms
  if(m_doLumiCalc) {
    if(book_per_signature_lumi(signature).isFailure()){ return(StatusCode::FAILURE); }
  }

  //OVERLAPTEST
  // - book histograms to store L1 EMClus for events sent to ES by this/other chain
  HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
  addMonGroup(new MonGroup(this, m_histoBasePath+"/"+trigDesc+"/OverlapTest",run));
  addHistogram(new TH1F("l1pt_this","L1 E_{T} (this chain sent to ES);E_{T}[GeV];",100,0,100.));
  addHistogram(new TH1F("l1pt_other","L1 E_{T} (other chain sent to ES);E_{T}[GeV];",100,0,100.));
  //OVERLAPTEST

  //all done
  ATH_MSG_DEBUG("Finished booking histograms for signature "<<signature);
  return(StatusCode::SUCCESS);
}
//END OF PER-SIGNATURE HISTOGRAM BOOKING




/*  BOOK SELECTION VARIABLE HISTOGRAMS
 *  - path is: base + signature + pass-state + stage + variable
 */
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_per_signature_sel(const string signature)
{
  ATH_MSG_DEBUG("Booking selection variable histograms for signature "<<signature);

  //find out whether this signature is a photon signature
  bool isPhoton = isPhotonSignature(signature);

  //collect pass-state subPaths
  vector<string> paths;
  fetchPassStates(paths);

  //prepend base + signature
  HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
  combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), paths); 

  //loop through paths
  vector<string>::iterator pathIt = paths.begin();
  for(; pathIt != paths.end(); ++pathIt){
    ATH_MSG_VERBOSE("Booking selection histograms to path "<<*pathIt<<"/<stage>");

    //---L1
    addMonGroup(new MonGroup(this,*pathIt+"/L1",run));
    addHistogram(new TH1F("EmClus", "L1 EM Cluster Energy; L1 EM Cluster E_{T} [GeV]; Entries", 40, 0., 80.));
    addHistogram(new TH1F("EmIsol", "L1 EM Isolation Energy; L1 EM Isolation Energy [GeV]; Entries", 12, 0., 12.));
    addHistogram(new TH1F("HadCore", "L1 Hadronic Core Energy; L1 Had Core Energy [GeV]; Entries", 12, 0., 12.));
    addHistogram(new TH1F("HadIsol", "L1 Hadronic Isolation Energy ; L1 Had Isolation Energy [GeV] ; Entries", 12, 0., 12.));
    addHistogram(new TH1F("ECore", "L1 EM+Hadronic Core Energy ; L1 EM+Hadronic Core Energy [GeV] ; Entries", 48, 0., 96.));
    addHistogram(new TH1F("Eta", "L1 \\eta ; L1 \\eta ; Entries", 53, -2.65, 2.65));
    addHistogram(new TH1F("Phi", "L1 \\phi ; L1 \\phi [rad]; Entries", 66, -3.30, 3.30));
    addHistogram(new TH2F("EtaPhiMap", "L1 \\eta vs \\phi ; L1 \\eta ; L1 \\phi", 53, -2.65, 2.65, 66, -3.30, 3.30));


    //---L2Calo
    addMonGroup(new MonGroup(this,*pathIt+"/L2Calo",run));
    addHistogram(new TH1F("Rcore", "L2 Calo R\\eta ; L2 Calo R\\eta ; Entries", 50, 0., 1.));
    addHistogram(new TH1F("Rstrip", "L2 Calo E_{ratio} ; L2 Calo E_{ratio} ; Entries", 60, -0.1, 1.4));
    addHistogram(new TH1F("Et", "L2 EM Calo E_{T} ; L2 EM Calo E_{T} [GeV] ; Entries", 80, 0., 80.));
    addHistogram(new TH1F("HadEt", "L2 Hadronic Calo E_{T} ; L2 Hadronic Calo E_{T} [GeV]; Entries", 15, 0., 1.5));
    addHistogram(new TH1F("HadLeak", "L2 Hadronic Leakage; L2 Hadronic Leakage ; Entries", 60, -0.01, 0.05));
    addHistogram(new TH1F("f1", "L2 f1 (E^{S1}/E) ; L2 f1 (E^{S1}/E ; Entries", 24, -0.1, 1.1));
    addHistogram(new TH1F("eta", "L2 Calo \\eta ; L2 Calo \\eta ; Entries", 53, -2.65, 2.65));
    addHistogram(new TH1F("phi", "L2 Calo \\phi ; L2 Calo \\phi [rad] ; Entries", 66, -3.30, 3.30));
    addHistogram(new TH2F("EtaPhiMap", "L2 Calo \\eta vs \\phi ; L2 Calo \\eta; L2 Calo \\phi", 53, -2.65, 2.65, 66, -3.30, 3.30));

    //---L2ID
    if(!isPhoton){
      addMonGroup(new MonGroup(this,*pathIt+"/L2ID",run));
      addHistogram(new TH1F("Eta_AtPeri", "L2 ID \\eta at Perigee; L2 ID \\eta at Perigee; Entries", 53, -2.65, 2.65));
      addHistogram(new TH1F("Phi_AtPeri", "L2 ID \\phi at Perigee; L2 ID \\phi at Perigee; Entries", 66, -3.30, 3.30));
      addHistogram(new TH1F("Pt", "L2 ID p_{T} ; p_{T} [GeV/c] ; Entries", 80, 0., 80.));
      addHistogram(new TH1F("Dphi", "L2 ID \\Delta\\phi (\\phi_{Calo} - \\phi_{track at Calo}); L2 ID \\Delta\\phi [rad] ; Entries", 64, 0., m_L2IDDphi*4));
      addHistogram(new TH1F("Deta", "L2 ID \\Delta\\eta (\\eta_{Calo} - \\eta_{track at Calo}); L2 ID \\Delta\\eta ; Entries", 64, 0., m_L2IDDeta*4));
      addHistogram(new TH1F("AlgoId", "L2 ID Track Algo ; L2 ID Track Algo ; Entries", 10, 0., 10.));    
      addHistogram(new TH1F("EP", "L2 ID E_{T}/p_{T} ; L2 IDScan E_{T}/p_{T} ; Entries", 60, 0., 6.));    
      addHistogram(new TH1F("RTRT", "R_{TRT} ; R_{TRT} ; Entries", 50, 0., 1.));
      addHistogram(new TH2F("EtaPhiAtCaloMap", "L2 ID \\eta vs \\phi at Calo; L2 ID \\eta at Calo; L2 ID \\phi at Calo", 53, -2.65, 2.65, 66, -3.30, 3.30));
    }

    //---EFCalo
    addMonGroup(new MonGroup(this,*pathIt+"/EFCalo",run));
    addHistogram(new TH1F("Et", "EF Calo E_{T} ; EF Calo E_{T} [GeV] ; Entries", 80, 0, 80.));
    addHistogram(new TH1F("Dphi", "EF Calo \\Delta\\phi (\\phi_{Calo} - \\phi_{L2Calo}) ; EF Calo  \\Delta\\phi [rad] ; Entries", 64, 0., 2*m_EFCaloDphi));
    addHistogram(new TH1F("Deta", "EF Calo \\Delta\\eta (\\eta_{Calo} - \\eta_{L2Calo}) ; EF Calo  \\Delta\\eta ; Entries", 64, 0., 2*m_EFCaloDeta));
    addHistogram(new TH1F("Eta", "EF Calo \\eta ; EF Calo \\eta ; Entries", 53, -2.65, 2.65));
    addHistogram(new TH1F("Phi", "EF Calo \\phi ; EF Calo \\phi ; Entries", 66, -3.30, 3.30));
    addHistogram(new TH2F("EtaPhiMap", "EF Calo \\eta vs \\phi; EF Calo \\eta; EF Calo \\phi", 53, -2.65, 2.65, 66, -3.30, 3.30));
    addHistogram(new TH1F("f1", "EF Calo f1 ; EF Calo f1 ; Entries", 60, -0.1, 1.1));
    addHistogram(new TH1F("raphad", "EF Calo Hadronic Leakage ; EF Calo Hadronic Leakage ; Entries", 60, -0.01, 0.05));
    addHistogram(new TH1F("raphad1", "EF Calo Hadronic Leakage (1st sampling) ; EF Calo Hadronic Leakage (1st sampling); Entries", 60, -0.01, 0.05));
    addHistogram(new TH1F("E277", "EF Calo E^{S2} in 7x7 cell window ; EF Calo E^{S2} in 7x7 cell window [GeV]; Entries", 80, 0., 80.));
    addHistogram(new TH1F("wetas1", "EF Calo \\omega_{\\eta} in S1 ; EF Calo \\omega_{\\eta} in S1 ; Entries", 20, 0., 2.));
    addHistogram(new TH1F("wtots1", "EF Calo \\omega_{tot} in S1; EF Calo \\omega_{\\eta} in S1 ; Entries", 75, 0., 15.));
    addHistogram(new TH1F("wetas2", "EF Calo \\omega_{\\eta} in S2 ; EF Calo \\omega_{\\eta} in S2 ; Entries", 20, 0., 2.));
    addHistogram(new TH1F("DES1", "EF Calo \\DeltaE^{S1} ; \\DeltaE^{S1} [GeV]; Entries", 40, 0., 2.));
    addHistogram(new TH1F("DEmax2", "EF Calo \\DeltaE_{max2} ; EF Calo \\DeltaE_{max2} ; Entries", 40, 0., 2));
    addHistogram(new TH1F("fracs1", "EF Calo Core Shower Shape ; EF Calo Core Shower Shape ; Entries", 55, -0.1, 1.6));
    addHistogram(new TH1F("Eratio", "EF Calo E_{ratio} ; EF Calo E_{ratio} ; Entries", 60, -0.1, 1.1));
    addHistogram(new TH1F("CaloIso", "EF Calo Isolation Energy in Cone20 ; EF Calo Isolation Energy in Cone20 ; Entries", 45, -0.1, 0.8));

    //---EFeg
    addMonGroup(new MonGroup(this,*pathIt+"/EFeg",run));
    addHistogram(new TH1F("egEt", "EF egamma E_{T} ; EF egamma E_{T} [GeV] ; Entries", 80, 0, 80.));
    addHistogram(new TH1F("egEta", "EF egamma eta ; EF egamma \\eta  ; Entries", 53, -2.65, 2.65));
    addHistogram(new TH1F("egPhi", "EF egamma phi ; EF egamma \\phi  ; Entries", 66, -3.30, 3.30));
    addHistogram(new TH2F("egEtaPhiMap", "EF egamma \\eta vs \\phi; EF egamma \\eta; EF egamma \\phi", 53, -2.65, 2.65, 66, -3.30, 3.30));

    //---EFID
    if(!isPhoton){
      addMonGroup(new MonGroup(this,*pathIt+"/EFID",run));
      addHistogram(new TH1F("trkPhi", "\\phi for EF Track ; EF Track \\phi ; Entries", 66, -3.30, 3.30));
      addHistogram(new TH1F("trkEta", "\\eta for EF Track ; EF Track \\eta ; Entries", 53, -2.65, 2.65));
      addHistogram(new TH1F("trkPt", "p_{T} for EF Track ; P_T [GeV] ; Entries", 80, 0, 80.));    
      addHistogram(new TH2F("trkEtaPhiMap", "EF Track  \\eta vs \\phi; EF Track \\eta; EF Track \\phi", 53, -2.65, 2.65, 66, -3.30, 3.30));
      addHistogram(new TH1F("Deta", "\\Delta\\eta for EF Track vs Cluster ; \\Delta\\eta for EF Track vs Cluster ; Entries", 40, 0., 0.2));
      addHistogram(new TH1F("Dphi", "\\Delta\\phi for EF Track vs Cluster ; \\Delta\\phi for EF Track vs Cluster [rad] ; Entries", 40, 0., 0.2));
      addHistogram(new TH1F("EoverP", "EF E/p ; EF E/p ; Entries", 50, 0., 10));
      addHistogram(new TH1F("Qual", "EF Track Quality ; EF Track Quality (\\chi^{2}); Entries", 60, 0., 60.));
      addHistogram(new TH1F("Pixel", "EF Track Number of Pixel Hits ; EF Track Number of Pixel Hits ; Entries", 8, 0., 8.));
      addHistogram(new TH1F("SCT", "EF Track Number of SCT Hits ; EF Track Number of SCT Hits ; Entries", 14, 0., 14.));
      addHistogram(new TH1F("TRT", "EF Track Number of TRT Hits ; EF Track Number of TRT Hits ; Entries", 50, 0., 50.));
      addHistogram(new TH1F("TRTtotal", "EF Track Number of total TRT Hits ; EF Track Number of total TRT Hits ; Entries", 50, 0., 50.));
      addHistogram(new TH1F("TRTHT", "EF Track Number of TRT HT Hits ; EF Track Number of TRT HT Hits ; Entries", 20, 0., 20.));
      addHistogram(new TH1F("RtotTRT", "EF R^{total}_{TRT} ; EF R^{total}_{TRT} ; Entries", 40, 0., 0.8));
      addHistogram(new TH1F("BLayer", "EF Track Number of B-Layer Hits ; EF Track Number of B-Layer Hits ; Entries", 5, 0., 5.));
      addHistogram(new TH1F("A0", "EF Impact Parameter; EF Impact Parameter ; Entries", 40, -2., 2.));
    }

  }

  //all done
  ATH_MSG_DEBUG("Finished booking selection variable histograms.");
  return(StatusCode::SUCCESS);
}
//END OF OBSERVABLE HISTOGRAM BOOKING




/*  BOOK RESOLUTION HISTOGRAMS
 *  - path is base + signature + pass-state + stage + "Resolution" + variable
 */
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_per_signature_res(const string signature)
{
  ATH_MSG_DEBUG("Booking resolution histograms for signature "<<signature);

  //find out whether this is a photon signature
  bool isPhoton = isPhotonSignature(signature);

  //paths upto pass-state
  vector<string> paths;
  fetchPassStates(paths);
  HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
  combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), paths); 

  //loop through paths
  vector<string>::iterator pathIt = paths.begin();
  for(; pathIt != paths.end(); ++pathIt){
    ATH_MSG_VERBOSE("Booking resolution histograms to path "<<*pathIt<<"/<stage>/Resolution");

    //---L1
    addMonGroup(new MonGroup(this,*pathIt+"/L1/Resolution",run));
    addHistogram(new TH1F("EmClus", "L1 EM Cluster Energy Resolution; L1 E_{T} / Off CaloCluster E_{T}; Entries", 63, -0.05, 2.05));
    addHistogram(new TH1F("Eta", "L1 \\eta Resolution; L1 \\eta / Off CaloCluster \\eta ; Entries", 63, -0.05, 2.05));
    addHistogram(new TH1F("Phi", "L1 \\phi Resolution; L1 \\phi / Off CaloCluster \\phi; Entries", 63, -0.05, 2.05));

    //---L2Calo
    addMonGroup(new MonGroup(this,*pathIt+"/L2Calo/Resolution",run));
    addHistogram(new TH1F("Rcore", "L2 Calo R\\eta Resolution; L2 Calo R\\eta / Off R\\eta; Entries", 63, -0.05, 2.05));
    addHistogram(new TH1F("Rstrip", "L2 Calo E_{ratio} Resolution; L2 Calo E_{ratio} / Off E_{ratio} ; Entries", 21, -0.05, 2.05));   
    addHistogram(new TH1F("HadEt", "L2 Hadronic Calo E_{T} Resolution; L2 Hadronic Calo E_{T} / Off Hadronic Calo E_{T}; Entries", 51, -0.05, 5.05));
    addHistogram(new TH1F("Et", "L2 EM Calo E_{T} Resolution; L2 EM Calo E_{T} / Off CaloCluster E_{T}; Entries", 21, -0.05, 2.05));
    addHistogram(new TH1F("eta", "L2 Calo \\eta Resolution ; L2 Calo \\eta / Off CaloCluster \\eta ; Entries", 21, -0.05, 2.05));
    addHistogram(new TH1F("phi", "L2 Calo \\phi Resolution; L2 Calo \\phi / Off CaloCluster \\phi ; Entries", 21, -0.05, 2.05));


    //--- L2ID 
    if(!isPhoton){
      addMonGroup(new MonGroup(this,*pathIt+"/L2ID/Resolution",run));//AT: dir to be changed!!!!
      addHistogram(new TH1F("Pt", "L2 p_{T} Resolution; L2 p_{T} / Off p_{T}; Entries", 21, -0.05, 2.05));
      addHistogram(new TH1F("Eta", "L2 eta Resolution (at Perigee); L2 \\eta / Off \\eta ; Entries", 21, -0.05, 2.05));
      addHistogram(new TH1F("Phi", "L2 phi Resolution (at Perigee); L2 \\phi / Off \\phi; Entries", 21, -0.05, 2.05));
    }

    //---EFCalo
    addMonGroup(new MonGroup(this,*pathIt+"/EFCalo/Resolution",run));
    addHistogram(new TH1F("Et", "EF Calo E_{T} Resolution; EF Calo E_{T} / Off CaloCluster E_{T} ; Entries", 21, -0.05, 2.05));
    addHistogram(new TH1F("Eta", "EF Calo eta Resolution; EF Calo \\eta / Off CaloCluster \\eta; Entries", 21, -0.05, 2.05));
    addHistogram(new TH1F("Phi", "EF Calo phi Resolution; EF Calo \\phi / Off CaloCluster \\phi ; Entries", 21, -0.05, 2.05));

    //---EFeg
    addMonGroup(new MonGroup(this,*pathIt+"/EFeg/Resolution",run));
    addHistogram(new TH1F("Et", "EF egamma E_{T} Resolution ; EF egamma E_{T} / Off egamma E_{T} ; Entries", 21, -0.05, 2.05));
    addHistogram(new TH1F("Eta", "EF egamma eta Resolution; EF egamma \\eta / Off egamma \\eta ; Entries", 21, -0.05, 2.05));
    addHistogram(new TH1F("Phi", "EF egamma phi Resolution; EF egamma \\phi / Off egamma \\phi ; Entries", 21, -0.05, 2.05));

    //---EFID
    if(!isPhoton){
      addMonGroup(new MonGroup(this,*pathIt+"/EFID/Resolution",run));
      addHistogram(new TH1F("Phi", "\\phi for EF Track Resolution (at Perigee); EF Track \\phi / Off Track \\phi; Entries", 21, -0.05, 2.05));
      addHistogram(new TH1F("Eta", "\\eta for EF Track Resolution (at Perigee) ; EF Track \\eta / Off Track \\eta; Entries", 21, -0.05, 2.05));
      addHistogram(new TH1F("Pt", "p_{T} for EF Track Resolution; EF Track P_{T} / Off Track P_{T} ; Entries", 21, -0.05, 2.05));
    }
  }

  //all done
  ATH_MSG_DEBUG("Finished booking resolution histograms for signature "<<signature);
  return(StatusCode::SUCCESS);
}
//END OF RESOLUTION HISTOGRAM BOOKING




/*  BOOK REJECTION/PREREJECTION HISTOGRAMS
*/
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_per_signature_reject(const string signature)
{
  ATH_MSG_DEBUG("Booking PreRejection/Rejection histograms for signature "<<signature);

  //find out whether this is a photon signature
  bool isPhoton = isPhotonSignature(signature);

  //fetch stages for later use
  vector<string> stages;
  fetchStages(isPhoton, stages);

  //assemble paths up to pass-state
  vector<string> paths;
  fetchPassStates(paths);
  HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
  combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), paths); 

  //loop through paths (essentially looping through pass-states)
  vector<string>::iterator pathIt = paths.begin();
  for(; pathIt != paths.end(); ++pathIt){

    //add both mongroups
    addMonGroup(new MonGroup(this,*pathIt+"/PreRejection",run));
    // - note: specify DQ merging algorithm
    addMonGroup(new MonGroup(this,*pathIt+"/Rejection",run,ATTRIB_MANAGED,"","effAsPerCent"));

    //loop through stages (stage subPaths)
    vector<string>::iterator stageIt = stages.begin();
    for(; stageIt != (stages.end()-1); ++stageIt){

      ATH_MSG_VERBOSE("Booking (Pre)Rejection histograms for "<<*stageIt<<" to path "<<*pathIt);

      //for L1 only book only PreRejection histograms
      if(stageIt==stages.begin()){
        addHistogram(new TH1F((*stageIt+"Et").c_str(), "Level 1 E_{T} ; L1 E_{T} [GeV] ; Entries",
              80, 0., 80.), *pathIt+"/PreRejection");
        addHistogram(new TH1F((*stageIt+"Eta").c_str(), "Level 1 \\eta ; L1 \\eta ; Entries",
              65, -3.25, 3.25), *pathIt+"/PreRejection");
        addHistogram(new TH1F((*stageIt+"Phi").c_str(), "Level 1 \\phi ; L1 \\phi [rad] ; Entries",
              66, -3.30, 3.30), *pathIt+"/PreRejection");    
      }

      //otherwise add pre- and rejection histograms
      else{
        addHistogram(new TH1F((*stageIt+"Et").c_str(),
              ("Level 1 E_{T} (before "+*stageIt+"); L1 E_{T} [GeV] ; Entries").c_str(),
              80, 0., 80.),*pathIt+"/PreRejection");
        addHistogram(new TH1F((*stageIt+"Eta").c_str(),
              ("Level 1 \\eta (before "+*stageIt+"); L1 \\eta ; Entries").c_str(),
              65, -3.25, 3.25),*pathIt+"/PreRejection");
        addHistogram(new TH1F((*stageIt+"Phi").c_str(),
              ("Level 1 \\phi (before "+*stageIt+"); L1 \\phi [rad] ; Entries").c_str(),
              66, -3.30, 3.30),*pathIt+"/PreRejection");    

        //rejection histograms are efficiencies -> need special merging algorithm
        addHistogram(new TH1F((*stageIt+"Et").c_str(),
              ("Level 1 E_{T} (after "+*stageIt+"); L1 E_{T} [GeV] ; \\varepsilon [%]").c_str(),
              80, 0., 80.),*pathIt+"/Rejection");
        addHistogram(new TH1F((*stageIt+"Eta").c_str(),
              ("Level 1 \\eta (after "+*stageIt+"); L1 \\eta ; \\varepsilon [%]").c_str(),
              65, -3.25, 3.25),*pathIt+"/Rejection");
        addHistogram(new TH1F((*stageIt+"Phi").c_str(),
              ("Level 1 \\phi (after "+*stageIt+"); L1 \\phi [rad] ; \\varepsilon [%]").c_str(),
              66, -3.30, 3.30),*pathIt+"/Rejection");     
      }
    }//done looping through stages
  }//done looping through paths

  //all done
  ATH_MSG_DEBUG("Finished booking (pre)rejection histograms.");
  return(StatusCode::SUCCESS);
}
//END OF REJECTION HISTOGRAM BOOKING




/*  BOOKING EFFICIENCY HISTOGRAMS
 *
 *  - path is: base + signature + pass-state + stage + "EffEle"/"EffPho"
 */
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_per_signature_eff(const string signature)
{
  ATH_MSG_DEBUG("Booking efficiency histograms for signature "<<signature);

  //find out whether we are dealing with a photon signature
  bool isPhoton = isPhotonSignature(signature);

  //fetch pass-states
  vector<string> tmp_paths;
  fetchPassStates(tmp_paths);

  //prepend by basePath and signature and add trailing slash
  HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
  combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), tmp_paths); 
  combineStrings(tmp_paths, string("/"));

  //fetch stages
  vector<string> stages;
  fetchStages(isPhoton, stages);

  //append slashes
  combineStrings(stages, string("/"));

  //append efficiency words
  if(isPhoton){ combineStrings(stages, m_effPhoPath); }
  else{ combineStrings(stages, m_effElePath); }

  //form full paths
  vector<string> paths;
  combineStrings(tmp_paths, stages, paths);

  //loop through paths
  vector<string>::iterator pathIt = paths.begin();
  for(; pathIt != paths.end(); ++pathIt){
    ATH_MSG_VERBOSE("Booking efficiency histograms to path "<<*pathIt);

    addMonGroup(new MonGroup(this,*pathIt,run,ATTRIB_MANAGED,"","effAsPerCent"));
    addHistogram(new TH1F("egEt", " E_{T} for Offline egamma ; Offline egamma E_{T} [GeV] ; \\varepsilon [%]", 80, 0, 80.));
    addHistogram(new TH1F("egEta", "\\eta for Offline egamma; Offline egamma \\eta  ; \\varepsilon [%]", 65, -3.25, 3.25));
    addHistogram(new TH1F("egPhi", "\\phi for Offline egamma ; Offline egamma \\phi  ; \\varepsilon [%]", 66, -3.30, 3.30));

  }//done looping through paths

  //all done
  ATH_MSG_DEBUG("Finished booking efficiency histograms.");
  return(StatusCode::SUCCESS);
}
//END OF EFFICIENCY HISTOGRAM BOOKING




/*  LUMINOSITY-DEPENDENT TRIGGER HISTOGRAM BOOKING
*/
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_per_signature_lumi(const string signature)
{
  ATH_MSG_DEBUG("Booking luminosity histograms for signature "<<signature);

  //find out whether we are dealing with a photon signature
  bool isPhoton = isPhotonSignature(signature);

  //fetch stages for later use
  vector<string> stages;
  fetchStages(isPhoton, stages);

  //fetch pass-states
  vector<string> paths;
  fetchPassStates(paths);

  //prepend with base + signature
  HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
  combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), paths); 
  //append "/Lumi"
  combineStrings(paths, string("/Lumi"));


  //loop through paths
  vector<string>::iterator pathIt = paths.begin();
  for(; pathIt != paths.end(); ++pathIt){
    ATH_MSG_VERBOSE("Booking lumi histograms to path "<<*pathIt);

    addMonGroup(new MonGroup(this,*pathIt,run));

    //build total histogram
    addHistogram(new TH1F("NTot_LB", " NTotal_LB ; Lumiblock ; Total N of Events", m_nLB,m_nLBLo,m_nLBUp));  
    addProfile(new TProfile("NTotOverLumi_LB_TProf", " NTotal_LB ; Lumiblock ; Total N of Events / Lumi", m_nLB,m_nLBLo,m_nLBUp));

    //loop through stages
    vector<string>::iterator stageIt = stages.begin();
    for(; stageIt != stages.end(); ++stageIt){
      addHistogram(new TH1F(("N"+*stageIt+"_LB").c_str(),
            ("N"+*stageIt+"_LB ; Lumiblock ; N of "+*stageIt).c_str(), m_nLB,m_nLBLo,m_nLBUp));  
      addProfile(new TProfile(("N"+*stageIt+"OverLumi_LB_TProf").c_str(),
            ("N"+*stageIt+"_LB ; Lumiblock ; N of "+*stageIt+" / Lumi").c_str(), m_nLB,m_nLBLo,m_nLBUp));  



    }//done looping through stages

  }//done looping through paths

  //all done
  ATH_MSG_DEBUG("Finished booking lumi histograms for signature "<<signature);
  return(StatusCode::SUCCESS);
}
//END OF LUMINOSITY DEPENDENT HISTOGRAM BOOKING




/*  OFFLINE HISTOGRAM BOOKING
*/
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_offline(bool isPhoton, string signature)
{
  ATH_MSG_DEBUG("Booking offline histograms.");

  vector<string> paths;

  //find out whether we need to book for signature or not and assemble path(s)
  if(signature == ""){
    //not booking for triggers so we only book one set of histograms either for electrons or photons
    if(isPhoton){ paths.push_back(m_histoBasePath+"/"+m_offPhoPath); }
    else{ paths.push_back(m_histoBasePath+"/"+m_offElePath); }
  }else{
    //booking for a signature, need to book all stages appropriate to it and all pass-states requested
    //the proper sub path is built up as follows:
    //signature + pass-state + stage + "MatchedOffEle"/"MatchedOffPho"

    //collect pass-state sub paths
    vector<string> tmp_paths;
    fetchPassStates(tmp_paths);

    //prepend them with base and signature
    HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
    combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), tmp_paths); 

    //collect all appropriate stages and add a slash in front
    vector<string> stages;
    fetchStages(isPhoton, stages);  
    combineStrings(string("/"), stages);

    //add them to paths
    combineStrings(tmp_paths, stages, paths);

    //finally add last part of path indicating whether we are matching offline electron or photon
    if(isPhoton){ combineStrings(paths, string("/"+m_matchedOffPhoPath)); }
    else{ combineStrings(paths, string("/"+m_matchedOffElePath)); }

  }//done assembling paths

  //loop through subpaths and book histogram sets to it
  vector<string>::iterator pathIt = paths.begin();
  for(; pathIt!=paths.end(); ++pathIt){
    ATH_MSG_VERBOSE("Booking offline histograms to path "<<*pathIt);

    //book common egamma histograms
    addMonGroup(new MonGroup(this,*pathIt,run));
    addHistogram(new TH1F("egEt", " E_{T} for Offline egamma ; Offline egamma E_{T} [GeV] ; Entries",80, 0, 80.));
    addHistogram(new TH1F("egEta", "\\eta for Offline egamma (w/ P_{T} cut); Offline egamma \\eta  ; Entries",65, -3.25, 3.25));
    addHistogram(new TH1F("egPhi", "\\phi for Offline egamma (w/ P_{T} cut); Offline egamma \\phi  ; Entries", 66, -3.30, 3.30));
    addHistogram(new TH2F("egEtaPhiMap", "\\eta vs \\phi for Offline egamma ; Offline egamma \\eta  ; Offline egamma \\phi", 65, -3.25, 3.25, 66, -3.30, 3.30));

    //book track histograms for electrons
    if(!isPhoton){
      addMonGroup(new MonGroup(this,*pathIt,run));
      addHistogram(new TH1F("trkPhi", "\\phi for Offline Egamma Track ; Offline Egamma Track \\phi ; Entries", 66, -3.30, 3.30));
      addHistogram(new TH1F("trkEta", "\\eta for Offline Egamma Track ; Offline Egamma Track \\eta ; Entries", 65, -3.25, 3.25));
      addHistogram(new TH1F("trkPt", " p_{T} for Offline Egamma Track  ; Offline Egamma Track P_{T} [GeV] ; Entries", 80, 0, 80.));
      addHistogram(new TH2F("trkEtaPhiMap", "\\eta vs \\phi for Offline Egamma Track ; Offline Egamma Track \\eta ; Offline Egamma Track \\phi", 65, -3.25, 3.25, 66, -3.30, 3.30));
      addHistogram(new TH1F("Deta", "\\Delta\\eta for Offline Egamma Track vs Cluster ; \\Delta\\eta for Offline Egamma Track vs Cluster ; Entries", 60, 0., 0.3));
      addHistogram(new TH1F("Dphi", "\\Delta\\phi for Offline Egamma Track vs Cluster ; \\Delta\\phi for Offline Egamma Track vs Cluster [rad] ; Entries", 60, 0., 0.3));
      addHistogram(new TH1F("EoverP", "Offline Egamma Track E/p ; Offline Egamma Track E/p ; Entries", 50, 0., 10));
      addHistogram(new TH1F("Qual", " Track Quality for Offline Egamma Track;  Offline Egamma Track Quality ; Entries", 60, 0., 60.));
      addHistogram(new TH1F("Pixel", "Track Number of Pixel Hits for Offline Egamma Track; Number of Pixel Hits  for Offline Egamma Track ; Entries", 8, 0., 8.));
      addHistogram(new TH1F("SCT", "Track Number of SCT Hits for Offline Egamma Track; Number of SCT Hits  for Offline Egamma Track ; Entries", 14, 0., 14.));
      addHistogram(new TH1F("TRT", "Track Number of TRT Hits for Offline Egamma Track; Number of TRT Hits for Offline Egamma Track  ; Entries", 50, 0., 50.));
      addHistogram(new TH1F("TRTtotal", "Track Number of total TRT Hits for Offline Egamma Track ; Track Number of total TRT Hits for Offline Egamma; Entries", 50, 0., 50.));
      addHistogram(new TH1F("TRTHT", "Offline Egamma Track Number of TRT HT Hits ; Offline Egamma Track Number of TRT HT Hits ; Entries", 20, 0., 20.));
      addHistogram(new TH1F("RtotTRT", "R^{total}_{TRT} for Offline Egamma Track ; R^{total}_{TRT} for Offline Egamma Track ; Entries", 55, 0., 1.1));
      addHistogram(new TH1F("BLayer", "Track Number of B-Layer Hits for Offline Egamma Track ; Number of B-Layer Hits for Offline Egamma Track ; Entries", 5, 0., 5.));
      addHistogram(new TH1F("A0", "Impact Parameter for Offline Egamma Track; Offline Egamma Track Impact Parameter ; Entries", 100, -5., 5.));
    }//done booking electron track histograms

  }//done looping through paths

  //all done
  ATH_MSG_DEBUG("Finished booking offline histograms");
  return(StatusCode::SUCCESS);
}
//END OF OFFLINE HISTOGRAM BOOKING

/*  EFFICIENCY HISTOGRAM BOOKING
*/
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_match(bool isPhoton, string signature)
{
  ATH_MSG_DEBUG("Booking offline match histograms.");

  vector<string> paths;

  //find out whether we need to book for signature or not and assemble path(s)
  if(signature == ""){
    //not booking for triggers so we only book one set of histograms either for electrons or photons
    paths.push_back(m_histoBasePath+"/"+m_offElePath);
  }else{
    //booking for a signature, need to book all stages appropriate to it and all pass-states requested
    //the proper sub path is built up as follows:
    //signature + pass-state + stage + "MatchedOffEle"/"MatchedOffPho"

    //collect pass-state sub paths
    vector<string> tmp_paths;
    fetchPassStates(tmp_paths);

    //prepend them with base and signature
    HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
    combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), tmp_paths); 

    //collect all appropriate stages and add a slash in front
    vector<string> stages;
    fetchStages(isPhoton, stages);  
    combineStrings(string("/"), stages);

    //add them to paths
    combineStrings(tmp_paths, stages, paths);

    //finally add last part of path indicating whether we are matching offline electron or photon
    if(isPhoton){ combineStrings(paths, string("/"+m_matchedOffPhoPath)); }
    else{ combineStrings(paths, string("/"+m_matchedOffElePath)); }

  }//done assembling paths

  //loop through subpaths and book histogram sets to it
  vector<string>::iterator pathIt = paths.begin();
  for(; pathIt!=paths.end(); ++pathIt){
    ATH_MSG_VERBOSE("Booking offline histograms to path "<<*pathIt);

    //book common egamma histograms
    addMonGroup(new MonGroup(this,*pathIt,run));
    addHistogram(new TH1F("egEt", " E_{T} for Offline egamma ; Offline egamma E_{T} [GeV] ; Entries",80, 0, 80.));
    addHistogram(new TH1F("egEta", "\\eta for Offline egamma (w/ P_{T} cut); Offline egamma \\eta  ; Entries",65, -3.25, 3.25));
    addHistogram(new TH1F("egPhi", "\\phi for Offline egamma (w/ P_{T} cut); Offline egamma \\phi  ; Entries", 66, -3.30, 3.30));

  }//done looping through paths

  //all done
  ATH_MSG_DEBUG("Finished booking offline match histograms");
  return(StatusCode::SUCCESS);
}
//END OF EFFICIENCY HISTOGRAM BOOKING

// LUMINOSITY HISTOGRAM BOOKING
StatusCode HLTEgammaFEXNavSigTEBaseTool::book_lumi(void)
{  
  ATH_MSG_DEBUG("Booking luminosity histograms.");  

  string path = m_histoBasePath+"/"+m_lumiPath;
  addMonGroup(new MonGroup(this,path,run));
  addHistogram(new TH1F("Lumi", " Lumi ; log10(Lumi) [cm-2s-1] ; Number of LB's", m_nLumi,m_nLumiLo,m_nLumiUp));  
  addProfile(new TProfile("Lumi_LB_TProf", " Lumi_LB ; Lumiblock ; Lumi [cm-2s-1] ", m_nLB,m_nLBLo,m_nLBUp));

  //all done
  ATH_MSG_DEBUG("Finished booking luminosity histograms.");
  return(StatusCode::SUCCESS);
}
//END OF LUMINOSITY HISTOGRAM BOOKING




/********************************************
 *
 *          HISTOGRAM FILLING
 *
 ********************************************/


/*  FILL LEVEL 1 OBSERVABLE HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillL1(const EmTau_ROI &itEMTau, const std::string &grp)
{
  hist("EmClus", grp)->Fill(itEMTau.getEMClus() / CLHEP::GeV);
  hist("EmIsol", grp)->Fill(itEMTau.getEMIsol() / CLHEP::GeV);
  hist("HadCore", grp)->Fill(itEMTau.getHadCore() / CLHEP::GeV);
  hist("HadIsol", grp)->Fill(itEMTau.getHadIsol() / CLHEP::GeV);
  hist("ECore", grp)->Fill(itEMTau.getCore() / CLHEP::GeV);
  hist("Eta", grp)->Fill(itEMTau.getEta());
  hist("Phi", grp)->Fill(itEMTau.getPhi());    
  hist2("EtaPhiMap", grp)->Fill(itEMTau.getEta(),itEMTau.getPhi(),1.);
}
void HLTEgammaFEXNavSigTEBaseTool::fillL1(const xAOD::EmTauRoI &itEMTau, const std::string &grp)
{
  hist("EmClus", grp)->Fill(itEMTau.emClus() / CLHEP::GeV);
  hist("EmIsol", grp)->Fill(itEMTau.emIsol() / CLHEP::GeV);
  hist("HadCore", grp)->Fill(itEMTau.hadCore() / CLHEP::GeV);
  hist("HadIsol", grp)->Fill(itEMTau.hadIsol() / CLHEP::GeV);
  hist("ECore", grp)->Fill(itEMTau.core() / CLHEP::GeV);
  hist("Eta", grp)->Fill(itEMTau.eta());
  hist("Phi", grp)->Fill(itEMTau.phi());
  hist2("EtaPhiMap", grp)->Fill(itEMTau.eta(),itEMTau.phi(),1.);
}
//END OF LEVEL 1 OBSERVABLE HISTOGRAM FILLING




/*  FILL LEVEL 1 RESOLUTIONS W.R.T. OFFLINE
*/
void HLTEgammaFEXNavSigTEBaseTool::fillL1OffRes(const EmTau_ROI &itEMTau,
    const egamma* matchedEgamma, const std::string &grp) {

  //check a valid offline match was supplied
  if(!matchedEgamma){ return; }

  //fetch associated CaloCluster and make sure it's valid
  const CaloCluster* cluster = matchedEgamma->cluster();
  if(!cluster){
    ATH_MSG_WARNING("Couldn't fetch CaloCluster for filling histograms in group "<<grp);
    return;
  }

  //calculate and fill resolutions
  float egamma_clus_et   = calcRatio(itEMTau.getEMClus(), calcEt(cluster->e(), cluster->eta()));
  float egamma_clus_eta  = calcRatio(itEMTau.getEta(), cluster->eta());
  float egamma_clus_phi  = calcRatio(itEMTau.getPhi(), cluster->phi());
  hist("EmClus", grp)->Fill(egamma_clus_et);
  hist("Eta", grp)->Fill(egamma_clus_eta );
  hist("Phi", grp)->Fill(egamma_clus_phi);

}
void HLTEgammaFEXNavSigTEBaseTool::fillL1OffRes(const xAOD::EmTauRoI &itEMTau,
    const xAOD::Egamma* matchedEgamma, const std::string &grp) {

  //check a valid offline match was supplied
  if(!matchedEgamma){ return; }

  //fetch associated CaloCluster and make sure it's valid
  const xAOD::CaloCluster* cluster = matchedEgamma->caloCluster();
  if(!cluster){
    ATH_MSG_WARNING("Couldn't fetch CaloCluster for filling histograms in group "<<grp);
    return;
  }
  
  //calculate and fill resolutions
  float egamma_clus_et   = calcRatio(itEMTau.emClus(), calcEt(cluster->e(), cluster->eta()));
  float egamma_clus_eta  = calcRatio(itEMTau.eta(), cluster->eta());
  float egamma_clus_phi  = calcRatio(itEMTau.phi(), cluster->phi());
  hist("EmClus", grp)->Fill(egamma_clus_et);
  hist("Eta", grp)->Fill(egamma_clus_eta );
  hist("Phi", grp)->Fill(egamma_clus_phi);
}
//END OF LEVEL 1 RESOLUTION FILLING




/*  FILL LEVEL 2 CALORIMETER OBSERVABLES
*/
void HLTEgammaFEXNavSigTEBaseTool::fillL2Calo(const TrigEMCluster *matchedL2, const string &grp) {
  //check if were given a valid cluster
  if(!matchedL2){ return; }

  //calculate and fill quantities
  float rcore = calcRatio(matchedL2->e237(), matchedL2->e277());
  float rstrip = calcRStrips(matchedL2->emaxs1(), matchedL2->e2tsts1());
  float et = calcEt(matchedL2->energy(), matchedL2->eta());
  float hadEt = calcEt(matchedL2->ehad1(), matchedL2->eta());
  float ES1 = matchedL2->energy(CaloSampling::EMB1) + matchedL2->energy(CaloSampling::EME1);
  float Etotal = matchedL2->energy();

  hist("Rcore", grp)->Fill(rcore);
  hist("Rstrip", grp)->Fill(rstrip);
  hist("eta", grp)->Fill(matchedL2->eta());
  hist("phi", grp)->Fill(matchedL2->phi());
  hist("Et", grp)->Fill(et / CLHEP::GeV); 
  hist("HadEt", grp)->Fill(hadEt / CLHEP::GeV);
  hist("HadLeak", grp)->Fill(hadEt / et);
  hist("f1", grp)->Fill(ES1 / Etotal);
  hist2("EtaPhiMap", grp)->Fill(matchedL2->eta(),matchedL2->phi(),1.);
}

void HLTEgammaFEXNavSigTEBaseTool::fillL2Calo(const xAOD::TrigEMCluster *matchedL2, const string &grp) {
  //check if were given a valid cluster
  if(!matchedL2){ return; }
  
  //calculate and fill quantities
  float rcore = calcRatio(matchedL2->e237(), matchedL2->e277());
  float rstrip = calcRStrips(matchedL2->emaxs1(), matchedL2->e2tsts1());
  float et = calcEt(matchedL2->energy(), matchedL2->eta());
  float hadEt = calcEt(matchedL2->ehad1(), matchedL2->eta());
  float ES1 = matchedL2->energy(CaloSampling::EMB1) + matchedL2->energy(CaloSampling::EME1);
  float Etotal = matchedL2->energy();

  hist("Rcore", grp)->Fill(rcore);
  hist("Rstrip", grp)->Fill(rstrip);
  hist("eta", grp)->Fill(matchedL2->eta());
  hist("phi", grp)->Fill(matchedL2->phi());
  hist("Et", grp)->Fill(et / CLHEP::GeV);
  hist("HadEt", grp)->Fill(hadEt / CLHEP::GeV);
  hist("HadLeak", grp)->Fill(hadEt / et);
  hist("f1", grp)->Fill(ES1 / Etotal);
  hist2("EtaPhiMap", grp)->Fill(matchedL2->eta(),matchedL2->phi(),1.);
}
//END OF LEVEL 2 CALO OBSERVABLE FILLING




/*  FILL LEVEL 2 CALORIMETER RESOLUTIONS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillL2CaloOffRes(const TrigEMCluster *matchedL2,
    const egamma* matchedEgamma, const string &grp) {

  //skip if we don't have valid pointers to offline or L2 cluster
  if(!matchedL2 || !matchedEgamma){
    ATH_MSG_WARNING("Cannot fill L2Calo resolutions.");
    return;
  }

  const EMShower* shower  = matchedEgamma->detail<EMShower>(m_egDetailKey);  
  float egamma_ethad1 = 0;
  float egamma_rcore  = 0;
  float egamma_rstrip = 0;
  if(shower){
    egamma_ethad1 = shower->ethad1();
    egamma_rcore  = calcRatio(shower->e237(), shower->e277());
    egamma_rstrip = calcRStrips(shower->emaxs1(), shower->e2tsts1());
  } else {
    ATH_MSG_WARNING("Couldn't fetch EMShower for filling histograms in group "<<grp);
  }

  const CaloCluster*  cluster = matchedEgamma->cluster();
  float egamma_clus_et = 0;
  float egamma_clus_eta = 0;  
  float egamma_clus_phi = 0;
  if(cluster) {
    egamma_clus_et = calcEt(cluster->e(), cluster->eta());
    egamma_clus_eta = cluster->eta();  
    egamma_clus_phi = cluster->phi();
  }  else {
    ATH_MSG_WARNING("Couldn't fetch CaloCluster for filling histograms in group "<<grp);
  }

  //
  float matchedL2_rcore = calcRatio(matchedL2->e237(), matchedL2->e277());
  float matchedL2_rstrip = calcRStrips(matchedL2->emaxs1(), matchedL2->e2tsts1());
  float matchedL2_et = calcEt(matchedL2->energy(), matchedL2->eta());
  float matchedL2_ethad1 = calcEt(matchedL2->ehad1(), matchedL2->eta()); 

  float ratio_rcore = calcRatio(matchedL2_rcore, egamma_rcore);
  float ratio_rstrip = calcRatio(matchedL2_rstrip, egamma_rstrip);
  float ratio_eta = calcRatio(matchedL2->eta(), egamma_clus_eta);
  float ratio_phi = calcRatio(matchedL2->phi(), egamma_clus_phi);
  float ratio_et = calcRatio(matchedL2_et, egamma_clus_et);
  float ratio_ethad1 = calcRatio(matchedL2_ethad1, egamma_ethad1);
  hist("Rcore", grp)->Fill(ratio_rcore);
  hist("Rstrip", grp)->Fill(ratio_rstrip);
  hist("eta", grp)->Fill(ratio_eta);// eg-cluster eta
  hist("phi", grp)->Fill(ratio_phi);// eg-cluster phi
  hist("Et", grp)->Fill(ratio_et); //eg-cluster Et
  hist("HadEt", grp)->Fill(ratio_ethad1);

}
void HLTEgammaFEXNavSigTEBaseTool::fillL2CaloOffRes(const xAOD::TrigEMCluster *matchedL2,
    const xAOD::Egamma* matchedEgamma, const string &grp) {

  //skip if we don't have valid pointers to offline or L2 cluster
  if(!matchedL2 || !matchedEgamma){
    ATH_MSG_WARNING("Cannot fill L2Calo resolutions.");
    return;
  }

  float egamma_ethad1 = 0;
  float egamma_rcore  = 0;
  float egamma_rstrip = 0;
  matchedEgamma->showerShapeValue(egamma_ethad1,xAOD::EgammaParameters::ethad1);
  float egamma_e237,egamma_e277;
  matchedEgamma->showerShapeValue(egamma_e237,xAOD::EgammaParameters::e237);
  matchedEgamma->showerShapeValue(egamma_e277,xAOD::EgammaParameters::e277);

  egamma_rcore  = calcRatio(egamma_e237, egamma_e277);
  float egamma_emaxs1,egamma_etsts1;
  matchedEgamma->showerShapeValue(egamma_emaxs1,xAOD::EgammaParameters::emaxs1);
  matchedEgamma->showerShapeValue(egamma_etsts1,xAOD::EgammaParameters::e2tsts1);
  egamma_rstrip = calcRStrips(egamma_emaxs1, egamma_etsts1);

  const xAOD::CaloCluster*  cluster = matchedEgamma->caloCluster();
  float egamma_clus_et = 0;
  float egamma_clus_eta = 0;
  float egamma_clus_phi = 0;
  if(cluster) {
    egamma_clus_et = calcEt(cluster->e(), cluster->eta());
    egamma_clus_eta = cluster->eta();
    egamma_clus_phi = cluster->phi();
  }  else {
    ATH_MSG_WARNING("Couldn't fetch CaloCluster for filling histograms in group "<<grp);
  }
  float matchedL2_rcore = calcRatio(matchedL2->e237(), matchedL2->e277());
  float matchedL2_rstrip = calcRStrips(matchedL2->emaxs1(), matchedL2->e2tsts1());
  float matchedL2_et = calcEt(matchedL2->energy(), matchedL2->eta());
  float matchedL2_ethad1 = calcEt(matchedL2->ehad1(), matchedL2->eta());

  float ratio_rcore = calcRatio(matchedL2_rcore, egamma_rcore);
  float ratio_rstrip = calcRatio(matchedL2_rstrip, egamma_rstrip);
  float ratio_eta = calcRatio(matchedL2->eta(), egamma_clus_eta);
  float ratio_phi = calcRatio(matchedL2->phi(), egamma_clus_phi);
  float ratio_et = calcRatio(matchedL2_et, egamma_clus_et);
  float ratio_ethad1 = calcRatio(matchedL2_ethad1, egamma_ethad1);
  hist("Rcore", grp)->Fill(ratio_rcore);
  hist("Rstrip", grp)->Fill(ratio_rstrip);
  hist("eta", grp)->Fill(ratio_eta);// eg-cluster eta
  hist("phi", grp)->Fill(ratio_phi);// eg-cluster phi
  hist("Et", grp)->Fill(ratio_et); //eg-cluster Et
  hist("HadEt", grp)->Fill(ratio_ethad1);

}

//END OF LEVEL 2 CALORIMETER RESOLUTION FILLING




/*  FILL LEVEL 2 TRACK HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillL2ID(const TrigInDetTrack* trk, const TrigEMCluster* clus,
    const string& grp)
{
  //skip if the object pointers are no good
  if(!clus || !trk){
    ATH_MSG_DEBUG("fillL2ID(...) got bad input. Skipping.");
    return;
  }

  //fetch variables
  // - note: only reading magnitude of pT
  // - note: eta/phi are at perigee, must be extrapolated to calo for dEta/dPhi
  unsigned int algoId = (unsigned int)trk->algorithmId();
  float pt  = fabsf((float)trk->param()->pT());
  float eta = (float)trk->param()->eta();
  float phi = (float)trk->param()->phi0();
  float eOverP = calcRatio(calcEt(clus->energy(), clus->eta()), pt);
  float n_trt = (float)trk->NStrawHits();
  float n_httrt = (float)trk->NTRHits();

  //calc R_trt, prevent division by zero for 0 hits
  float rtrt = 0;
  if(n_trt > 0){rtrt = n_httrt/n_trt;}

  //extrapolate eta/phi to calo
  // - note: if this fails we are filling -999 to histograms
  float etac(-999), phic(-999);
  if(extrapolateTrackToCalo(trk)){
    etac = m_extrapolatedEta;
    phic = m_extrapolatedPhi;
  }

  //fill histograms
  //hist("Eta_AtCalo", grp)->Fill(etac);
  //hist("Phi_AtCalo", grp)->Fill(phic);    
  hist("Eta_AtPeri", grp)->Fill(eta);
  hist("Phi_AtPeri", grp)->Fill(phi);
  hist("Pt", grp)->Fill(pt / CLHEP::GeV);
  hist("Deta", grp)->Fill(calcDeltaEta(etac, clus->eta()));
  hist("Dphi", grp)->Fill(calcDeltaPhi(phic, clus->phi()));
  hist("EP", grp)->Fill(eOverP);    
  hist("RTRT", grp)->Fill(rtrt); //prevent div-by-0
  hist("AlgoId", grp)->Fill(algoId);    
  hist2("EtaPhiAtCaloMap", grp)->Fill(etac, phic, 1.);
}

void HLTEgammaFEXNavSigTEBaseTool::fillL2ID(const xAOD::TrackParticle* trk, const xAOD::TrigEMCluster* clus,
    const string& grp)
{
  //skip if the object pointers are no good
  if(!clus || !trk){
    ATH_MSG_DEBUG("fillL2ID(...) got bad input. Skipping.");
    return;
  }

  //fetch variables
  //  - note: only reading magnitude of pT
  //  - note: eta/phi are at perigee, must be extrapolated to calo for dEta/dPhi
  int algoId = 0; //(*trkIter)->algorithmId();
  if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ) algoId=9;
  if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ) algoId=5;
  if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyB] ) algoId=6;
  if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyC] ) algoId=7;
  //unsigned int algoId = (unsigned int)trk->algorithmId();
  float pt  = fabsf((float)trk->pt());
  float eta = (float)trk->eta();
  float phi = (float)trk->phi0();
  float eOverP = calcRatio(calcEt(clus->energy(), clus->eta()), pt);
//  float n_trt = (float)trk->NStrawHits();
//  float n_httrt = (float)trk->NTRHits();
  uint8_t number=0;
  trk->summaryValue(number,xAOD::numberOfTRTHits);
  float n_trt = (float) number;
  trk->summaryValue(number,xAOD::numberOfTRTHighThresholdHits );
  float n_httrt = (float) number;

  //calc R_trt, prevent division by zero for 0 hits
  float rtrt = 0;
  if(n_trt > 0){rtrt = n_httrt/n_trt;}

  //extrapolate eta/phi to calo
  // - note: if this fails we are filling -999 to histograms
  float etac(-999), phic(-999);
/*
  if(extrapolateTrackToCalo(trk)){
    etac = m_extrapolatedEta;
    phic = m_extrapolatedPhi;
  }
*/

  //fill histograms
  hist("Eta_AtPeri", grp)->Fill(eta);
  hist("Phi_AtPeri", grp)->Fill(phi);
  hist("Pt", grp)->Fill(pt / CLHEP::GeV);
  hist("Deta", grp)->Fill(calcDeltaEta(etac, clus->eta()));
  hist("Dphi", grp)->Fill(calcDeltaPhi(phic, clus->phi()));
  hist("EP", grp)->Fill(eOverP);
  hist("RTRT", grp)->Fill(rtrt); //prevent div-by-0
  hist("AlgoId", grp)->Fill(algoId);
  hist2("EtaPhiAtCaloMap", grp)->Fill(etac, phic, 1.);
}


//END OF LEVEL2 TRACK OBSERVABLE HISTOGRAM FILLING




/*  FILL LEVEL 2 TRACK RESOLUTION HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillL2IDOffRes(const TrigInDetTrack* track, const egamma* matchedEgamma,
    const string &grp)
{
  //skip if we got bad pointers
  if(!track || !matchedEgamma){ 
    ATH_MSG_WARNING("fillL2IDOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //try to fetch offline egamma track particle and perigee
  // - note: the protections might not be necessary, since the matching should have done this
  if(!matchedEgamma->trackParticle()){
    ATH_MSG_WARNING("fillL2IDOffRes(...) cannot fetch track associated with egamma. Skipping.");
    return;
  }

  //try to fetch perigee
  const Trk::Perigee* matchedPerigee = matchedEgamma->trackParticle()->perigee();
  if(!matchedPerigee){
    ATH_MSG_WARNING("fillL2IDOffRes(...) cannot fetch track associated with egamma. Skipping.");
    return;
  }

  //fetch offline values
  float pt_matchedEgamma  = matchedEgamma->trackParticle()->pt();
  float phi_matchedEgamma = matchedPerigee->parameters()[Trk::phi];
  float eta_matchedEgamma = calcEta(matchedPerigee->parameters()[Trk::theta]) ;

  //calculate ratios
  float ratio_pt = calcRatio(fabsf(track->param()->pT()), pt_matchedEgamma);
  float ratio_eta = calcRatio(track->param()->eta(), eta_matchedEgamma);
  float ratio_phi = calcRatio(track->param()->phi0(), phi_matchedEgamma);

  //fill histograms
  hist("Pt", grp)->Fill(ratio_pt);
  hist("Eta", grp)->Fill(ratio_eta);
  hist("Phi", grp)->Fill(ratio_phi);
}

void HLTEgammaFEXNavSigTEBaseTool::fillL2IDOffRes(const xAOD::TrackParticle* track, const xAOD::Electron* matchedEgamma,
    const string &grp)
{
  //skip if we got bad pointers
  if(!track || !matchedEgamma){
    ATH_MSG_WARNING("fillL2IDOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //try to fetch offline egamma track particle and perigee
  // - note: the protections might not be necessary, since the matching should have done this
  if(!matchedEgamma->trackParticle()){
    ATH_MSG_WARNING("fillL2IDOffRes(...) cannot fetch track associated with egamma. Skipping.");
    return;
  }

  //try to fetch perigee
  const xAOD::TrackParticle* matchedTrack = matchedEgamma->trackParticle();
  if(!matchedTrack){
    ATH_MSG_WARNING("fillL2IDOffRes(...) cannot fetch track associated with egamma. Skipping.");
    return;
  }

  //fetch offline values
  float pt_matchedEgamma  = matchedTrack->pt();
  float phi_matchedEgamma = matchedTrack->phi();
  float eta_matchedEgamma = matchedTrack->eta() ;

  //calculate ratios
  float ratio_pt = calcRatio(fabsf(track->pt()), pt_matchedEgamma);
  float ratio_eta = calcRatio(track->eta(), eta_matchedEgamma);
  float ratio_phi = calcRatio(track->phi(), phi_matchedEgamma);

  //fill histograms
  hist("Pt", grp)->Fill(ratio_pt);
  hist("Eta", grp)->Fill(ratio_eta);
  hist("Phi", grp)->Fill(ratio_phi);
}

//END OF L2 TRACK RESOLUTION HISTOGRAM FILLING




/*  FILL EVENT FILTER CALORIMETER OBSERVABLE HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillEFCalo(const CaloCluster* cluster, const TrigEMCluster *l2Cluster,
  const string &grp) {

  //check pointers
  if(!cluster || !l2Cluster){
    ATH_MSG_DEBUG("fillEFCalo(...) got bad pointers. Skipping.");
    return;
  }

  //calculate L2<->EF resolutions
  float dEta = calcDeltaEta(cluster->eta(), l2Cluster->eta());
  float dPhi = calcDeltaPhi(cluster->phi(), l2Cluster->phi());

  //fill histograms
  hist("Et", grp)->Fill(cluster->et() / CLHEP::GeV);
  hist("Eta", grp)->Fill(cluster->eta());
  hist("Phi", grp)->Fill(cluster->phi()); 
  hist("Dphi", grp)->Fill(dPhi);
  hist("Deta", grp)->Fill(dEta);
  hist2("EtaPhiMap", grp)->Fill(cluster->eta(), cluster->phi(), 1.);
}

void HLTEgammaFEXNavSigTEBaseTool::fillEFCalo(const xAOD::CaloCluster* cluster, const xAOD::TrigEMCluster *l2Cluster,
  const string &grp) {

  //check pointers
  if(!cluster || !l2Cluster){
    ATH_MSG_DEBUG("fillEFCalo(...) got bad pointers. Skipping.");
    return;
  }

  //calculate L2<->EF resolutions
  float dEta = calcDeltaEta(cluster->eta(), l2Cluster->eta());
  float dPhi = calcDeltaPhi(cluster->phi(), l2Cluster->phi());

  //fill histograms
  hist("Et", grp)->Fill(cluster->et() / CLHEP::GeV);
  hist("Eta", grp)->Fill(cluster->eta());
  hist("Phi", grp)->Fill(cluster->phi());
  hist("Dphi", grp)->Fill(dPhi);
  hist("Deta", grp)->Fill(dEta);
  hist2("EtaPhiMap", grp)->Fill(cluster->eta(), cluster->phi(), 1.);
}

//END OF EVENT FILTER CALORIMETER OBSERVABLE HISTOGRAM FILLING

/*  FILL EVENT FILTER CALORIMETER SHOWER OBSERVABLE HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillEFCaloShower(const egamma* EFeg, const string &grp)
{  

  //check pointer
  if(!EFeg){
    ATH_MSG_WARNING("fillEFCaloShower(...) got bad pointer. Skipping.");
    return;
  }


  //check whether we get the shower & cluster, otherwise get out
  if(!(EFeg->detail<EMShower>()) || !(EFeg->cluster())){
    ATH_MSG_DEBUG("Cannot fill EFCalo shower histograms: EMShower (or cluster) not found!");
    return;
  }


  //try and load shower
  const EMShower* shower = EFeg->detail<EMShower>();


  //get transverse energy from cluster
  float et = 0;
  et = EFeg->cluster()->et();

  //calculate eratio
  float emax1plus2 = shower->emaxs1() + shower->e2tsts1();
  float emax1minus2 = shower->emaxs1() - shower->e2tsts1();
  float eratio = 0;
  if(emax1plus2 > 0){
    eratio = emax1minus2 / emax1plus2;
  }

  //calculate et-based ratios, catch E_T of 0
  float caloiso20 = 0;
  float hadleak = 0;
  float hadleak1 = 0;
  float demax2 = 0;
  if(et > 0){
    hadleak = shower->ethad() / et;
    hadleak1 = shower->ethad1() / et;
    demax2 = shower->e2tsts1() / (1000 + 0.009 * et);
    caloiso20 = shower->etcone20()/et;
  }
  else{
    demax2 = shower->e2tsts1() / 1000;
  }

  //fill histograms
  setCurrentMonGroup(grp);
  hist("f1", grp)->Fill(shower->f1());
  hist("raphad", grp)->Fill(hadleak);
  hist("raphad1", grp)->Fill(hadleak1);
  hist("E277", grp)->Fill(shower->e277() / CLHEP::GeV);
  hist("wetas1", grp)->Fill(shower->weta1());
  hist("wtots1", grp)->Fill(shower->wtots1());
  hist("wetas2", grp)->Fill(shower->weta2());
  hist("DES1", grp)->Fill((shower->e2tsts1() - shower->emins1()) / CLHEP::GeV);
  hist("DEmax2", grp)->Fill(demax2);
  hist("fracs1", grp)->Fill(shower->fracs1());
  hist("Eratio", grp)->Fill(eratio);
  hist("CaloIso", grp)->Fill(caloiso20);

}

void HLTEgammaFEXNavSigTEBaseTool::fillEFCaloShower(const xAOD::Egamma* EFeg, const string &grp)
{

  //check pointer
  if(!EFeg){
    ATH_MSG_WARNING("fillEFCaloShower(...) got bad pointer. Skipping.");
    return;
  }


  //check whether we get the shower & cluster, otherwise get out
  if(!(EFeg->caloCluster())){
    ATH_MSG_DEBUG("Cannot fill EFCalo shower histograms: EMShower (or cluster) not found!");
    return;
  }


  //try and load shower


  //get transverse energy from cluster
  float et = 0;
  et = EFeg->caloCluster()->et();

  //calculate eratio
  float shower_emaxs1,shower_e2tsts1;
  EFeg->showerShapeValue(shower_emaxs1,xAOD::EgammaParameters::emaxs1);
  EFeg->showerShapeValue(shower_e2tsts1,xAOD::EgammaParameters::e2tsts1);
  float emax1plus2 = shower_emaxs1 + shower_e2tsts1;
  float emax1minus2 = shower_emaxs1 - shower_e2tsts1;
  float eratio = 0;
  if(emax1plus2 > 0){
    eratio = emax1minus2 / emax1plus2;
  }

  //calculate et-based ratios, catch E_T of 0
  float caloiso20 = 0;
  float hadleak = 0;
  float hadleak1 = 0;
  float demax2 = 0;
  float shower_ethad,shower_ethad1,shower_etcone20;
  if(et > 0){
    EFeg->showerShapeValue(shower_ethad,xAOD::EgammaParameters::ethad);
    EFeg->showerShapeValue(shower_ethad1,xAOD::EgammaParameters::ethad1);
    EFeg->isolationValue(shower_etcone20,xAOD::Iso::etcone20);
    hadleak = shower_ethad / et;
    hadleak1 = shower_ethad1 / et;
    demax2 = shower_e2tsts1 / (1000 + 0.009 * et);
    caloiso20 = shower_etcone20/et;
  }
  else{
    demax2 = shower_e2tsts1 / 1000;
  }

  //fill histograms
  setCurrentMonGroup(grp);
  float shower_e277,shower_weta1,shower_wtots1,shower_weta2,shower_emins1,
        shower_f1,shower_fracs1;
  EFeg->showerShapeValue(shower_e277,xAOD::EgammaParameters::e277);
  EFeg->showerShapeValue(shower_weta1,xAOD::EgammaParameters::weta1);
  EFeg->showerShapeValue(shower_wtots1,xAOD::EgammaParameters::wtots1);
  EFeg->showerShapeValue(shower_weta2,xAOD::EgammaParameters::weta2);
  EFeg->showerShapeValue(shower_emins1,xAOD::EgammaParameters::emins1);
  EFeg->showerShapeValue(shower_f1,xAOD::EgammaParameters::f1);
  EFeg->showerShapeValue(shower_fracs1,xAOD::EgammaParameters::fracs1);
  hist("f1", grp)->Fill(shower_f1);
  hist("raphad", grp)->Fill(hadleak);
  hist("raphad1", grp)->Fill(hadleak1);
  hist("E277", grp)->Fill(shower_e277 / CLHEP::GeV);
  hist("wetas1", grp)->Fill(shower_weta1);
  hist("wtots1", grp)->Fill(shower_wtots1);
  hist("wetas2", grp)->Fill(shower_weta2);
  hist("DES1", grp)->Fill((shower_e2tsts1 - shower_emins1) / CLHEP::GeV);
  hist("DEmax2", grp)->Fill(demax2);
  hist("fracs1", grp)->Fill(shower_fracs1);
  hist("Eratio", grp)->Fill(eratio);
  hist("CaloIso", grp)->Fill(caloiso20);

}



//END OF EVENT FILTER EGAMMA CALORIMETER SHOWER OBSERVABLE FILLING


/*  FILL EVENT FILTER CALORIMETER RESOLUTION HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillEFCaloOffRes(const CaloCluster* cluster, const egamma* offline,
    const std::string &grp)
{

  //check pointers
  if(!cluster || !offline){
    ATH_MSG_DEBUG("fillEFCaloOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //fetch offline cluster
  const CaloCluster* offCluster = offline->cluster();
  if(!offCluster){
    ATH_MSG_DEBUG("fillEFCaloOffRes(...) couldn't retrieve offline cluster. Skipping.");
    return;
  }

  //fetch offline variables
  float offEt = calcEt(offCluster->e(), offCluster->eta());
  float offEta = offCluster->eta();  
  float offPhi = offCluster->phi();

  //calculate ratios
  float ratio_et = calcRatio(cluster->et(), offEt);
  float ratio_eta = calcRatio(cluster->eta(), offEta);
  float ratio_phi = calcRatio(cluster->phi(), offPhi);

  //fill histograms
  hist("Et", grp)->Fill(ratio_et);//eg-cluster info
  hist("Eta", grp)->Fill(ratio_eta);//eg-cluster info
  hist("Phi", grp)->Fill(ratio_phi);//eg-cluster info

}

void HLTEgammaFEXNavSigTEBaseTool::fillEFCaloOffRes(const xAOD::CaloCluster* cluster, const xAOD::Egamma* offline,
    const std::string &grp)
{

  //check pointers
  if(!cluster || !offline){
    ATH_MSG_DEBUG("fillEFCaloOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //fetch offline cluster
  const xAOD::CaloCluster* offCluster = offline->caloCluster();
  if(!offCluster){
    ATH_MSG_DEBUG("fillEFCaloOffRes(...) couldn't retrieve offline cluster. Skipping.");
    return;
  }

  //fetch offline variables
  float offEt = calcEt(offCluster->e(), offCluster->eta());
  float offEta = offCluster->eta();
  float offPhi = offCluster->phi();

  //calculate ratios
  float ratio_et = calcRatio(cluster->et(), offEt);
  float ratio_eta = calcRatio(cluster->eta(), offEta);
  float ratio_phi = calcRatio(cluster->phi(), offPhi);

  //fill histograms
  hist("Et", grp)->Fill(ratio_et);//eg-cluster info
  hist("Eta", grp)->Fill(ratio_eta);//eg-cluster info
  hist("Phi", grp)->Fill(ratio_phi);//eg-cluster info

}

//END OF EVENT FILTER CALORIMETER RESOLUTION HISTOGRAM FILLING




/*  FILL EVENT FILTER/OFFLINE TRACKING HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillEFID(const egamma *eg, const string &grp)
{
  //check we have access to track and bail out if not
  if(!eg || !(eg->trackParticle())){
    ATH_MSG_DEBUG("Cannot fill EF/Offline track histograms: TrackParticle not found.");
    return;
  }

  //load track and perigee, check perigee, load match
  const Rec::TrackParticle* track = eg->trackParticle();
  const Trk::Perigee* perigee = track->perigee();
  const EMTrackMatch* match = eg->detail<EMTrackMatch>();
  if(!perigee){
    ATH_MSG_DEBUG("Cannot fill EF/Offline track histograms: Perigee not found.");
    return;
  }

  //fetch track directions
  float phi = perigee->parameters()[Trk::phi];
  float eta = calcEta(perigee->parameters()[Trk::theta]);

  //fetch impact parameter
  float a0 = -999;
  if(m_doExtrapol){
    const Trk::Perigee* bsPrg = m_trackToVertexTool->perigeeAtBeamspot(*track);
    if(bsPrg){
      a0 = bsPrg->parameters()[Trk::d0];
      delete bsPrg;
    }
  }


  //intermediate TRT hit observables
  float rtottrt = 0;
  float nTRTht = (float)track->trackSummary()->get(Trk::numberOfTRTHighThresholdHits)
    + (float)track->trackSummary()->get(Trk::numberOfTRTHighThresholdOutliers);
  float nTRTtotal = (float)track->trackSummary()->get(Trk::numberOfTRTOutliers)
    + (float)track->trackSummary()->get(Trk::numberOfTRTHits);

  //calc R_tot_trt, prevent division by zero for 0 hits
  if(nTRTtotal > 0){rtottrt = nTRTht / nTRTtotal;}

  //fill histograms
  setCurrentMonGroup(grp);
  hist("trkPhi")->Fill(phi);
  hist("trkEta")->Fill(eta);
  hist("trkPt")->Fill(track->pt() / CLHEP::GeV);
  hist2("trkEtaPhiMap")->Fill(eta, phi, 1.);
  hist("Deta")->Fill(match->deltaEta(1));
  hist("Dphi")->Fill(match->deltaPhi(2));
  hist("EoverP")->Fill( fabsf( eg->e() / track->p() ) );
  hist("Qual")->Fill(track->fitQuality()->chiSquared());
  hist("Pixel")->Fill(track->trackSummary()->get(Trk::numberOfPixelHits));
  hist("SCT")->Fill(track->trackSummary()->get(Trk::numberOfSCTHits));
  hist("TRT")->Fill(track->trackSummary()->get(Trk::numberOfTRTHits));
  hist("TRTtotal")->Fill(nTRTtotal);
  hist("TRTHT")->Fill(nTRTht);
  hist("BLayer")->Fill(track->trackSummary()->get(Trk::numberOfBLayerHits));
  hist("A0")->Fill(a0);
  hist("RtotTRT")->Fill(rtottrt); //prevent div-by-0

  return;
}

void HLTEgammaFEXNavSigTEBaseTool::fillEFID(const xAOD::Electron *eg, const string &grp)
{
  //check we have access to track and bail out if not
  if(!eg || !(eg->trackParticle())){
    ATH_MSG_DEBUG("Cannot fill EF/Offline track histograms: TrackParticle not found.");
    return;
  }

  //load track and perigee, check perigee, load match
  const xAOD::TrackParticle* track = eg->trackParticle();

  //fetch track directions
  float phi = track->phi();
  float eta = track->eta();

  //fetch impact parameter
  float a0 = -999;
/*
  if(m_doExtrapol){
    const Trk::Perigee* bsPrg = m_trackToVertexTool->perigeeAtBeamspot(*track);
    if(bsPrg){
      a0 = bsPrg->parameters()[Trk::d0];
      delete bsPrg;
    }
  }
*/


  //intermediate TRT hit observables
  float rtottrt = 0;
  float nTRTht=0;
  float nTRTtotal= 0;
/*
  float nTRTht = (float)track->trackSummary()->get(Trk::numberOfTRTHighThresholdHits)
    + (float)track->trackSummary()->get(Trk::numberOfTRTHighThresholdOutliers);
  float nTRTtotal = (float)track->trackSummary()->get(Trk::numberOfTRTOutliers)
    + (float)track->trackSummary()->get(Trk::numberOfTRTHits);
*/

  //calc R_tot_trt, prevent division by zero for 0 hits
  if(nTRTtotal > 0){rtottrt = nTRTht / nTRTtotal;}

  //fill histograms
  setCurrentMonGroup(grp);
  hist("trkPhi")->Fill(phi);
  hist("trkEta")->Fill(eta);
  hist("trkPt")->Fill(track->pt() / CLHEP::GeV);
  hist2("trkEtaPhiMap")->Fill(eta, phi, 1.);
/*
  hist("Deta")->Fill(match->deltaEta(1));
  hist("Dphi")->Fill(match->deltaPhi(2));
*/
  hist("EoverP")->Fill( fabsf( eg->caloCluster()->et() / track->pt() ) );
  hist("Qual")->Fill(track->chiSquared());
/*
  hist("Pixel")->Fill(track->trackSummary()->get(Trk::numberOfPixelHits));
  hist("SCT")->Fill(track->trackSummary()->get(Trk::numberOfSCTHits));
  hist("TRT")->Fill(track->trackSummary()->get(Trk::numberOfTRTHits));
*/
  hist("TRTtotal")->Fill(nTRTtotal);
  hist("TRTHT")->Fill(nTRTht);
  //hist("BLayer")->Fill(track->trackSummary()->get(Trk::numberOfBLayerHits));
  hist("A0")->Fill(a0);
  hist("RtotTRT")->Fill(rtottrt); //prevent div-by-0

  return;
}


//END OF EVENT FILTER/OFFLINE TRACK HISTOGRAM FILLING




/*  FILL EVENT FILTER TRACK RESOLUTION HISTOGRAMS
*/

void HLTEgammaFEXNavSigTEBaseTool::fillEFIDOffRes(const egamma* egEF, const egamma* egOff, const string &grp)
{
  //check input pointers
  if(!egEF || !egOff){
    ATH_MSG_WARNING("fillEFIDOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //try and fetch track particles
  if ( !(egEF->trackParticle()) || !(egOff->trackParticle())){
    ATH_MSG_DEBUG("fillEFIDOffRes(...) couldn't retrieve TrackParticle(s). Skipping."); 
    return;
  }

  //fetch perigees
  const Trk::Perigee* efPerigee = egEF->trackParticle()->perigee(); 
  const Trk::Perigee* offPerigee = egOff->trackParticle()->perigee(); 
  if ( !efPerigee  || !offPerigee){
    ATH_MSG_DEBUG("fillEFIDOffRes(...) couldn't retrieve Perigee(s). Skipping."); 
    return;
  }

  //fetch variables
  float phi_egEF  = efPerigee->parameters()[Trk::phi];
  float phi_egOff = offPerigee->parameters()[Trk::phi];
  float eta_egEF  = calcEta(efPerigee->parameters()[Trk::theta]);
  float eta_egOff = calcEta(offPerigee->parameters()[Trk::theta]);
  float pt_egEF  = egEF->trackParticle()->pt();
  float pt_egOff = egOff->trackParticle()->pt();

  //calculate ratios
  float ratio_phi = calcRatio(phi_egEF, phi_egOff);
  float ratio_eta = calcRatio(eta_egEF, eta_egOff);
  float ratio_pt = calcRatio(pt_egEF, pt_egOff);

  //fill histograms
  hist("Phi", grp)->Fill(ratio_phi);
  hist("Eta", grp)->Fill(ratio_eta );
  hist("Pt", grp)->Fill(ratio_pt);

}
void HLTEgammaFEXNavSigTEBaseTool::fillEFIDOffRes(const xAOD::Electron* egEF, const xAOD::Electron* egOff, const string &grp)
{
  //check input pointers
  if(!egEF || !egOff){
    ATH_MSG_WARNING("fillEFIDOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //try and fetch track particles
  if ( !(egEF->trackParticle()) || !(egOff->trackParticle())){
    ATH_MSG_DEBUG("fillEFIDOffRes(...) couldn't retrieve TrackParticle(s). Skipping."); 
    return;
  }
  const xAOD::TrackParticle *ef, *off;
  ef = egEF->trackParticle();
  off = egOff->trackParticle();


  //fetch variables
  float phi_egEF  = ef->phi();
  float phi_egOff = off->phi();
  float eta_egEF  = ef->eta();
  float eta_egOff = off->eta();
  float pt_egEF  = ef->pt();
  float pt_egOff = off->pt();

  //calculate ratios
  float ratio_phi = calcRatio(phi_egEF, phi_egOff);
  float ratio_eta = calcRatio(eta_egEF, eta_egOff);
  float ratio_pt = calcRatio(pt_egEF, pt_egOff);

  //fill histograms
  hist("Phi", grp)->Fill(ratio_phi);
  hist("Eta", grp)->Fill(ratio_eta );
  hist("Pt", grp)->Fill(ratio_pt);

}
//END OF EVENT FILTER TRACK RESOLUTION HISTOGRAM FILLING



/*  FILL EVENT FILTER/OFFLINE EGAMMA HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillEFeg(const egamma* eg, const string &grp, const double pt_cut)
{  
  //check pointer
  if(!eg){
    ATH_MSG_WARNING("fillEFeg(...) got bad pointer. Skipping.");
    return;
  }

  //fill histograms
  setCurrentMonGroup(grp);
  hist("egEt")->Fill(eg->et() / CLHEP::GeV);
  if(eg->pt()/CLHEP::GeV > pt_cut){
    hist("egEta")->Fill(eg->eta());
    hist("egPhi")->Fill(eg->phi());     
  }
  hist2("egEtaPhiMap", grp)->Fill(eg->eta(), eg->phi(), 1.);

}

void HLTEgammaFEXNavSigTEBaseTool::fillEFeg(const xAOD::Egamma* eg, const string &grp, const double pt_cut)
{
  //check pointer
  if(!eg){
    ATH_MSG_WARNING("fillEFeg(...) got bad pointer. Skipping.");
    return;
  }

  //fill histograms
  setCurrentMonGroup(grp);
  hist("egEt")->Fill(eg->caloCluster()->et() / CLHEP::GeV);
  if(eg->pt()/CLHEP::GeV > pt_cut){
    hist("egEta")->Fill(eg->eta());
    hist("egPhi")->Fill(eg->phi());
  }
  hist2("egEtaPhiMap", grp)->Fill(eg->eta(), eg->phi(), 1.);

}
//END OF EVENT FILTER/OFFLINE EGAMMA HISTOGRAM FILLING


/*   FILL OFFLINE EFFICIENCY HISTOGRAMS (FOR BOOTSTRAP)
*/
void HLTEgammaFEXNavSigTEBaseTool::fillEFMatch(const egamma* eg, const string &grp, const double pt_cut)
{
  //check pointer
  if(!eg){
    ATH_MSG_WARNING("fillEFMatch(...) got bad pointer. Skipping.");
    return;
  }

  //fill histograms
  setCurrentMonGroup(grp);
  hist("egEt")->Fill(eg->et() / CLHEP::GeV);
  if(eg->pt()/CLHEP::GeV > pt_cut){
    hist("egEta")->Fill(eg->eta());
    hist("egPhi")->Fill(eg->phi());
  }

}
void HLTEgammaFEXNavSigTEBaseTool::fillEFMatch(const xAOD::Egamma* eg, const string &grp, const double pt_cut)
{
  //check pointer
  if(!eg){
    ATH_MSG_WARNING("fillEFMatch(...) got bad pointer. Skipping.");
    return;
  }

  //fill histograms
  setCurrentMonGroup(grp);
  hist("egEt")->Fill(eg->caloCluster()->et() / CLHEP::GeV);
  if(eg->pt()/CLHEP::GeV > pt_cut){
    hist("egEta")->Fill(eg->eta());
    hist("egPhi")->Fill(eg->phi());
  }

}
//END OF OFFLINE EFFICIENCY HISTOGRAMS


/*  FILL EVENT FILTER EGAMMA RESOLUTION HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::fillEFegOffRes(const egamma* egEF, const egamma* egOff, const string &grp)
{
  //check pointers  
  if(!egEF && !egOff){
    ATH_MSG_WARNING("fillEFefOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //calculate ratios
  float ratio_et = calcRatio(egEF->et(), egOff->pt());
  float ratio_eta = calcRatio(egEF->eta(), egOff->eta());
  float ratio_phi = calcRatio(egEF->phi(), egOff->phi());

  //fill histos
  hist("Et", grp)->Fill(ratio_et);
  hist("Eta", grp)->Fill(ratio_eta);
  hist("Phi", grp)->Fill(ratio_phi); 

}
void HLTEgammaFEXNavSigTEBaseTool::fillEFegOffRes(const xAOD::Egamma* egEF, const xAOD::Egamma* egOff, const string &grp)
{
  //check pointers 
  if(!egEF && !egOff){
    ATH_MSG_WARNING("fillEFefOffRes(...) got bad pointers. Skipping.");
    return;
  }

  //calculate ratios
  float ratio_et = calcRatio(egEF->caloCluster()->et(), egOff->pt());
  float ratio_eta = calcRatio(egEF->eta(), egOff->eta());
  float ratio_phi = calcRatio(egEF->phi(), egOff->phi());

  //fill histos
  hist("Et", grp)->Fill(ratio_et);
  hist("Eta", grp)->Fill(ratio_eta);
  hist("Phi", grp)->Fill(ratio_phi);

}

//END OF EVENT FILTER EGAMMA RESOLUTION HISTOGRAM FILLING




/*  FILL OFFLINE EGAMMA HISTOGRAMS
 *  - uses EFeg/EFID filling functions
 *  - if no path is supplied, it is assumed we are dealing with a regular
 *    offline object and fill to the appropriate path.
 *  - if a path is supplied, the function will add the appropriate foldername
 *    for matched objects
 *  - second function is specialised version for the offline efficiencies (less histograms)
 */
void HLTEgammaFEXNavSigTEBaseTool::fillOfflineEgamma(const egamma* eg, bool isPhoton, string path="", const double elec_cut=0., const double phot_cut=0.)
{
  //set path if none was supplied
  if(path==""){path = m_histoBasePath + "/" + (isPhoton ? m_offPhoPath : m_offElePath); }
  //otherwise append match foldername
  else{ path += "/" + (isPhoton ? m_matchedOffPhoPath : m_matchedOffElePath); }
  ATH_MSG_DEBUG("Filling offline "<<(isPhoton ? "photon" : "electron")<<" histograms to path "<<path);

  //call event filter filling functions
  if(!isPhoton) fillEFeg(eg, path, elec_cut);
  else fillEFeg(eg, path, phot_cut);
  if(!isPhoton){ fillEFID(eg, path); }
}
void HLTEgammaFEXNavSigTEBaseTool::fillOfflineEgamma(const xAOD::Electron* eg, bool isPhoton, string path="", const double elec_cut=0., const double phot_cut=0.)
{
  //set path if none was supplied
  if(path==""){path = m_histoBasePath + "/" + (isPhoton ? m_offPhoPath : m_offElePath); }
  //otherwise append match foldername
  else{ path += "/" + (isPhoton ? m_matchedOffPhoPath : m_matchedOffElePath); }
  ATH_MSG_DEBUG("Filling offline "<<(isPhoton ? "photon" : "electron")<<" histograms to path "<<path);

  //call event filter filling functions
  if(!isPhoton) fillEFeg(eg, path, elec_cut);
  else fillEFeg(eg, path, phot_cut);
  if(!isPhoton){ fillEFID(eg, path); }
}


void HLTEgammaFEXNavSigTEBaseTool::fillOfflineMatches(const egamma* eg, bool isPhoton, string path="", const double elec_cut=0., const double phot_cut=0.)
{
  //set path if none was supplied
  if(path==""){path = m_histoBasePath + "/" + (isPhoton ? m_offPhoPath : m_offElePath); }
  //otherwise append match foldername
  else{ path += "/" + (isPhoton ? m_matchedOffPhoPath : m_matchedOffElePath); }
  ATH_MSG_DEBUG("Filling offline "<<(isPhoton ? "photon" : "electron")<<" histograms to path "<<path);

  //call event filter filling functions
  if(!isPhoton) fillEFMatch(eg, path, elec_cut);
  else fillEFMatch(eg, path, phot_cut);
}

void HLTEgammaFEXNavSigTEBaseTool::fillOfflineMatches(const xAOD::Egamma* eg, bool isPhoton, string path="", const double elec_cut=0., const double phot_cut=0.)
{
  //set path if none was supplied
  if(path==""){path = m_histoBasePath + "/" + (isPhoton ? m_offPhoPath : m_offElePath); }
  //otherwise append match foldername
  else{ path += "/" + (isPhoton ? m_matchedOffPhoPath : m_matchedOffElePath); }
  ATH_MSG_DEBUG("Filling offline "<<(isPhoton ? "photon" : "electron")<<" histograms to path "<<path);

  //call event filter filling functions
  if(!isPhoton) fillEFMatch(eg, path, elec_cut);
  else fillEFMatch(eg, path, phot_cut);
}

//END OF OFFLINE EGAMMA HISTOGRAM FILLING



//FILL RATES
void HLTEgammaFEXNavSigTEBaseTool::FillRates(float Pt, float Eta, float Phi, const std::string &level, const std::string &grp) {
  hist(level+"Et", grp)->Fill(Pt / CLHEP::GeV);
  hist(level+"Eta", grp)->Fill(Eta);
  hist(level+"Phi", grp)->Fill(Phi);
}





/****************************************
 *
 *          POST PROCESSING
 *
 ****************************************/


/*  POST PROCESSING PER SIGNATURE
 *
 */
StatusCode HLTEgammaFEXNavSigTEBaseTool::proc_per_signature(const string signature)
{
  ATH_MSG_DEBUG("Starting post-processing for signature "<<signature);

  //find out whether we are dealing with a photon signature
  bool isPhoton = isPhotonSignature(signature);

  //calculate efficiencies w.r.t. offline
  // - common path: base + signature + pass-state
  // - numerator paths: common + matched offline path
  // - denominator paths: common + offline path
  // - output paths: common + efficiency path

  //fetch pass-states to build subPaths
  vector<string> subPaths;
  fetchPassStates(subPaths);

  //prepend with base + signature and append slash
  HLTEgammaNavMonTool::trigger_description(signature,trigDesc);
  combineStrings(string(m_histoBasePath + "/" + trigDesc + "/"), subPaths); 
  combineStrings(subPaths, string("/"));

  //fetch stages
  vector<string> stages;
  fetchStages(isPhoton, stages);

  //build paths for efficiency calculation
  vector<string> paths;
  combineStrings(subPaths, stages, paths);

  //loop through paths
  vector<string>::const_iterator pathIt = paths.begin();
  for(; pathIt != paths.end(); ++pathIt){

    //build individual histogram subPaths (numerator, denominator, efficiency)
    string numPath = *pathIt + "/" + (isPhoton ? m_matchedOffPhoPath : m_matchedOffElePath);
    string denPath = m_histoBasePath + "/" + (isPhoton ? m_offPhoPath : m_offElePath);
    string effPath = *pathIt + "/" + (isPhoton ? m_effPhoPath : m_effElePath);

    ATH_MSG_VERBOSE("Building ofline efficiencies from/to:");
    ATH_MSG_VERBOSE("   numerator path "<<numPath);
    ATH_MSG_VERBOSE(" denominator path "<<denPath);
    ATH_MSG_VERBOSE("  efficiency path "<<effPath);

    //divide matched offline by offline into efficiency (with scaling to [%] enabled)
    histDivide(hist("egEt",numPath), hist("egEt",denPath), hist("egEt",effPath), true);
    histDivide(hist("egEta",numPath), hist("egEta",denPath), hist("egEta",effPath), true);
    histDivide(hist("egPhi",numPath), hist("egPhi",denPath), hist("egPhi",effPath), true);

  }//done looping through paths for efficiency calculation

  //calcualte rejection efficiency
  // -> rejection = stage_i / stage_{i-1}
  // - note: recycling vector of stages fetched previously
  // - note: rely on correct order of stages

  //loop through subPaths (pass-states)
  // - note: reusing vector<string> iterator from above
  pathIt = subPaths.begin();
  for(; pathIt != subPaths.end(); ++pathIt){
    ATH_MSG_VERBOSE("Building rejection efficiencies at path "<<*pathIt);

    //build individual histogram subpaths
    string numPath = *pathIt + m_preRejectionPath;
    string denPath = *pathIt + m_preRejectionPath;
    string rejPath = *pathIt + m_rejectionPath;

    //loop through stages
    // - note: loop skips first one (there's no stage prior to L1 and we do (this stage)/(previous stage))
    // - note: loop skips last one (no rejection histogram for EFeg)
    vector<string>::const_iterator stageIt = stages.begin() + 1;
    for(; stageIt != stages.end() - 1; ++stageIt){

      //divide histograms (with scaling to [%] enabled)
      histDivide(hist(*stageIt+"Et", numPath), hist(*(stageIt-1)+"Et", denPath),
          hist(*stageIt+"Et", rejPath),true);
      histDivide(hist(*stageIt+"Eta", numPath), hist(*(stageIt-1)+"Eta", denPath),
          hist(*stageIt+"Eta", rejPath),true);
      histDivide(hist(*stageIt+"Phi", numPath), hist(*(stageIt-1)+"Phi", denPath),
          hist(*stageIt+"Phi", rejPath),true);

    }//done looping through stages to calculate rejection efficiencies

  }//done looping through paths (pass-states)

  //all done
  ATH_MSG_DEBUG("Finished post-processing for signature "<<signature);
  return(StatusCode::SUCCESS);
}




/*  VARIOUS LUMI FILLERS
 *  TODO tidy
 */
void HLTEgammaFEXNavSigTEBaseTool::FillLumi(const double lumi, const string &grp) {
  hist("Lumi", grp)->Fill(lumi);
}

void HLTEgammaFEXNavSigTEBaseTool::FillLumiHist2(const long unsigned int lb, const double lumi, const string &grp) {
  hist2("Lumi_LB", grp)->Fill(lb,lumi,1.); 
}

void HLTEgammaFEXNavSigTEBaseTool::FillLumiProfile(const long unsigned int lb, const double lumi, const string &grp) {
  profile("Lumi_LB_TProf", grp)->Fill(lb,lumi,1.);
}


void HLTEgammaFEXNavSigTEBaseTool::FillRateLumi(const long unsigned int lb, const long unsigned int rate, const string &hname, const string &grp) {
  for(unsigned int i=1; i<=rate;i++){
    hist(hname+"_LB", grp)->Fill(lb);
  }
}

void HLTEgammaFEXNavSigTEBaseTool::FillRateLumiHist2(const long unsigned int lb, const double lumi, const long unsigned int rate, const string &hname, const string &grp) {
  hist2(hname+"OverLumi_LB", grp)->Fill(lb,rate/lumi,1.);
}

void HLTEgammaFEXNavSigTEBaseTool::FillRateLumiProfile(const long unsigned int lb, const double lumi, const long unsigned int rate, const string &hname, const string &grp) {  
  if(msg().level() <= MSG::VERBOSE) msg() << MSG::VERBOSE 
    << "FillRateLumiProfile: lb, rate, lumi = " 
      << lb << ", " << rate << ", " << lumi << endreq;
  if(lumi>0) { profile(hname+"OverLumi_LB_TProf", grp)->Fill(lb,rate/lumi,1.); }
  else {
    if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "FillRateLumiProfile: lumi = " << lumi << endreq;
  }
}
//END OF LUMI FILLERS




/*  CALCULATE DELTA PHI
 *  - checks whether absolute difference is larger than pi and corrects
 */
float HLTEgammaFEXNavSigTEBaseTool::calcDeltaPhi(float phi1, float phi2)
{
  float dphi = fabs(phi1 - phi2);
  if(dphi > TMath::Pi()){ dphi -= 2*TMath::Pi(); }
  return(dphi);
}


//  PRINT COUNTS AT EACH STAGE
void HLTEgammaFEXNavSigTEBaseTool::PrintTable() {
  msg() << MSG::INFO << "BEGIN N EVENTS" << endreq;
  msg() << MSG::INFO << "Total                 " << m_nTotal << endreq;
  msg() << MSG::INFO << "L1                    " << m_nL1 << endreq;
  msg() << MSG::INFO << "L2Calo                " << m_nL2Calo << endreq;
  msg() << MSG::INFO << "L2ID                  " << m_nL2ID << endreq;
  msg() << MSG::INFO << "EFCalo                " << m_nEFCalo << endreq;
  msg() << MSG::INFO << "EFeg                  " << m_nEFeg << endreq;
  msg() << MSG::INFO << "EFTrack               " << m_nEFID << endreq;
}




/*****************************************
 *
 *          HELPER FUNCTIONS
 *
 *****************************************/


/*  SCALE HISTOGRAM ERRORS
*/
void HLTEgammaFEXNavSigTEBaseTool::scaleError(float factor, TH1 *h) {
  if (h) {
    h->Scale(factor);
    for (int i = 0; i <= h->GetNbinsX(); i++) {
      if (h->GetBinContent(i) != 0)
        h->SetBinError(i, factor*sqrt(h->GetBinContent(i)));
      else
        h->SetBinError(i, 0.);
    }
  }
}
//END OF HISTOGRAM ERROR SCALING




/*  DIVIDE HISTOGRAMS
*/
void HLTEgammaFEXNavSigTEBaseTool::histDivide(TH1* num, TH1* den, TH1* quo, bool scale)
{
  num->Sumw2();
  den->Sumw2();
  quo->Divide(num, den, 1., 1., "b");
  if(scale){ quo->Scale(100.); }
}
//END OF HISTOGRAM DIVISION




/*  IDENTIFY PHOTON CHAINS
 *  - not sure this is robust against all inclusions of 'g' in the signature name
 *  - some regex checking might be better (but might need BOOST or some other regex parser)
 *    maybe the TDT provides this functionality???
 */
bool HLTEgammaFEXNavSigTEBaseTool::isPhotonSignature(string signature)
{
  ATH_MSG_DEBUG("Checking whether "<<signature<<" is a photon chain.");  

  bool isPhoton = false;

  //loop through characters in string
  for(string::size_type pos = 0; pos<signature.size() ; ++pos) {

    string::size_type loc = signature.find( "g", pos );

    if( loc != string::npos ) {  
      string str_exclude = "tight";  
      string::size_type loc_exclude = signature.find(str_exclude, pos ); 
      pos=loc;

      if(loc_exclude != loc-2) {
        isPhoton = true;
      }

    }
    else{break;}    
  }//for loop

  //all done
  ATH_MSG_DEBUG("Done checking: It is"<<(isPhoton ? "." : " not."));
  return(isPhoton);
}
//END OF PHOTON CHAIN IDENTIGICATION


/* FETCH PASS-STATES
*/
void HLTEgammaFEXNavSigTEBaseTool::fetchPassStates(vector<string>& output)
{
  if(m_doActiveChain){ output.push_back(m_activeChainPath); }
  if(m_doActiveTe && !m_skipActiveTe){ output.push_back(m_activeTePath); }
  if(m_doAllTe){ output.push_back(m_allTePath); }
}




/* FETCH STAGES
 * - note: the order of these is important for fetching the right histograms
 *         for rejection efficiency calculation. handle with care.
 */
void HLTEgammaFEXNavSigTEBaseTool::fetchStages(bool isPhoton, vector<string>& output)
{
  output.push_back("L1");
  output.push_back("L2Calo");
  if(!isPhoton){ output.push_back("L2ID"); }
  output.push_back("EFCalo");
  if(!isPhoton){ output.push_back("EFID"); }
  output.push_back("EFeg");
}




/* HELPER FUNCTION FOR COMBINING VECTORS OF STRINGS WITH OTHER (VECTORS OF) STRING(S)
*/
void HLTEgammaFEXNavSigTEBaseTool::combineStrings(
    vector<string>& parts1, vector<string>& parts2, vector<string>& output)
{
  //check whether either of the inputs is the output (this will create problems)
  if(&parts1 == &output || &parts2 == &output){
    ATH_MSG_WARNING("combineStrings() cannot output to one of the input vectors. An error may occur.");
  }

  //loop through first parts
  vector<string>::const_iterator p1It = parts1.begin();
  for(; p1It != parts1.end(); ++p1It){
    //loop through second parts
    vector<string>::const_iterator p2It = parts2.begin();
    for(; p2It != parts2.end(); ++p2It){
      //combine parts and push into return vector
      output.push_back(*p1It + *p2It);
    }//done looping through second parts
  }//done looping through first parts
}
//END OF




//  PREPEND LIST OF STRINGS WITH THE SAME STRING
void HLTEgammaFEXNavSigTEBaseTool::combineStrings(string part1, vector<string>& parts2)
{
  //loop through second part
  vector<string>::iterator it = parts2.begin();
  for(; it!=parts2.end(); ++it){
    //prepend part1
    *it = part1 + *it;
  }
}

//  APPEND SAME STRING TO LIST OF STRINGS
void HLTEgammaFEXNavSigTEBaseTool::combineStrings(vector<string>& parts1, string part2)
{
  //loop through first parts
  vector<string>::iterator it = parts1.begin();
  for(; it!=parts1.end(); ++it){
    //prepend part2
    *it = *it + part2;
  }
}




/*  EXTRAPOLATION OF TRACK DIRECTIONS TO CALORIMETER
*/
bool HLTEgammaFEXNavSigTEBaseTool::extrapolateTrackToCalo(const TrigInDetTrack* trk)
{
  //check input
  if(!trk){
    ATH_MSG_WARNING("Bad track pointer given to extrapolator.");  
    return(false);
  }

  ATH_MSG_VERBOSE("Extrapolating track at eta="<<trk->param()->eta()<<", phi="<<trk->param()->phi0());

  //initialise temporary extrapolation result variables
  // - note: apparently these cannot be cast to float automatically
  double eta(-999), phi(-999);

  //set up extrapolation helper for track and fetch extrapolated numbers
  //TODO should we move these constants to job options?
  //TODO check how we can catch an error thrown by the extrapolator and report it back
  //TODO should we not divide by "mm"? it shouldn't make a difference right now, because
  //     that's our standard unit, so mm:=1
  TrigInDetTrackHelper trackHelper(trk->param());
  trackHelper.extrapolate(1470.0*CLHEP::mm,3800.0*CLHEP::mm, phi, eta);

  ATH_MSG_VERBOSE("Extrapolated track to eta="<<eta<<", phi="<<phi);

  //publish extrapolated values to member variables and report back a success
  m_extrapolatedEta = (float)eta;
  m_extrapolatedPhi = (float)phi;
  return(true);
}
//END OF TRACK DIRECTION EXTRAPOLATION




/* the following is not called in current implementation 
   note sure what it does
   TODO find out whether this can be cleaned away or replaced with something

   StatusCode HLTEgammaFEXNavSigTEBaseTool::book_per_signature_rate(const string signature, string SubPath) {

   string BasicPath = m_BasicPath+signature+SubPath;

   ATH_MSG_VERBOSE("Booking rate histograms to path "<<BasicPath<<"/stage/Rates");

//---L1
addMonGroup(new MonGroup(this,BasicPath+"/L1/Rates",run));
addHistogram(new TH1F("L1Et", "Level 1 E_{T} ; E_{T} [GeV] ; Rate [Hz]", 80, 0., 80.));
addHistogram(new TH1F("L1Eta", "Level 1 \\eta ; \\eta ; Rate [Hz]", 65, -3.25, 3.25));
addHistogram(new TH1F("L1Phi", "Level 1 \\phi ; \\phi [rad] ; Rate [Hz]", 66, -3.30, 3.30));

//---L2Calo    
addMonGroup(new MonGroup(this,BasicPath+"/L2Calo/Rates",run));
addHistogram(new TH1F("L2CaloEt", "Level 2 Calo E_{T} ; E_{T} [GeV] ; Rate [Hz]", 80, 0., 80.));
addHistogram(new TH1F("L2CaloEta", "Level 2 Calo \\eta ; \\eta ; Rate [Hz]", 65, -3.25, 3.25));
addHistogram(new TH1F("L2CaloPhi", "Level 2 Calo \\phi ; \\phi [rad] ; Rate [Hz]", 66, -3.30, 3.30));

//---L2ID    
addMonGroup(new MonGroup(this,BasicPath+"/L2ID/Rates",run));
addHistogram(new TH1F("L2IDEt", "p_{T} after Level 2 ID  ; p_{T} [GeV] ; Rate [Hz]", 80, 0., 80.));
addHistogram(new TH1F("L2IDPhi", "\\phi after Level 2 ID ; \\phi [rad] ; Rate [Hz]", 66, -3.30, 3.30));
addHistogram(new TH1F("L2IDEta", "\\eta after Level 2 ID ; \\eta ; Rate [Hz]", 65, -3.25, 3.25));

//---EFCalo   
addMonGroup(new MonGroup(this,BasicPath+"/EFCalo/Rates",run));
addHistogram(new TH1F("EFCaloEt", "E_{T} at EF Calo ; E_{T} [GeV] ; Rate [Hz]", 80, 0, 80.));
addHistogram(new TH1F("EFCaloEta", "\\eta at EF Calo ; \\eta ; Rate [Hz]", 65, -3.25, 3.25));
addHistogram(new TH1F("EFCaloPhi", "\\phi at EF Calo ; \\phi [rad] ; Rate [Hz]", 66, -3.30, 3.30));


//---EFID   
addMonGroup(new MonGroup(this,BasicPath+"/EFID/Rates",run));
addHistogram(new TH1F("EFIDEt", "E_{T} after EF Track; E_{T} [GeV] ; Rate [Hz]", 80, 0, 80.));
addHistogram(new TH1F("EFIDPhi", "\\phi after EF Track ; \\phi [rad] ; Rate [Hz]", 66, -3.30, 3.30));
addHistogram(new TH1F("EFIDEta", "\\eta after EF Track ; \\eta ; Rate [Hz]", 65, -3.25, 3.25));


return StatusCode::SUCCESS;
}
*/




