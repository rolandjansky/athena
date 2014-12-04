/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaMonitoring/HLTEgammaNavMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "xAODTrigger/EmTauRoI.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>

#include "TH1F.h"
#include "TMath.h"

/*
template<class T>
std::string rtti_real_name() {
  std::string name(__PRETTY_FUNCTION__);   
  size_t op = name.find('=');
  size_t cl = name.find(']');
  return name.substr(op+1, cl-op-1);
}
*/

HLTEgammaNavMonTool::HLTEgammaNavMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : HLTEgammaFEXBaseTool(type,name,parent){
  m_firstTimeStamp = m_lastTimeStamp = 0;
  declareProperty("signatures",m_signatures);
}

StatusCode HLTEgammaNavMonTool::init() {
  //StatusCode sc;

  if (service("ToolSvc", m_toolSvc).isFailure()){
    (*m_log) << MSG::ERROR << "Unable to get ToolSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  
  // extrapolation -- removed
  m_doExtrapol = false;

  m_fill_condition=TrigDefs::alsoDeactivateTEs;
  m_rate_condition=TrigDefs::Physics;
  (*m_log) << MSG::DEBUG << "Using Fill Condition " << m_fill_condition << endreq;
  (*m_log) << MSG::DEBUG << "Using Rate Condition " << m_rate_condition << endreq;


  return StatusCode::SUCCESS;
}

HLTEgammaNavMonTool::~HLTEgammaNavMonTool() {
}

StatusCode HLTEgammaNavMonTool::book() {


	std::vector<std::string> signs_that_exist;
	const std::vector<std::string> TrigHLTItems = getTDT()->getListOfTriggers(".*");

	for(std::vector<std::string>::const_iterator i = m_signatures.begin();
		i!=m_signatures.end();++i){
           std::string EF_part("HLT_");  EF_part+=(*i);
           bool found = false;
	   for(std::vector<std::string>::const_iterator j = TrigHLTItems.begin();
		j!=TrigHLTItems.end();++j){
		if ( EF_part == (*j) ) {
			signs_that_exist.push_back((*i));
			found = true;
		}
	   }
	   if ( !found )
	   *m_log << MSG::DEBUG << "Did not find this one : " << (*i) << endreq;
	}
	
	m_signatures.clear();
	for(std::vector<std::string>::const_iterator i = signs_that_exist.begin();
		i!=signs_that_exist.end();++i){
		m_signatures.push_back((*i));
	}
	signs_that_exist.clear();
	for(std::vector<std::string>::const_iterator i = m_signatures.begin();
		i!=m_signatures.end();++i){
			*m_log << MSG::INFO << "Found this one " << (*i) << endreq;
			*m_log << MSG::INFO << "Will book histograms for it" << endreq;
			if ( book_per_signature((*i)).isFailure() ) return StatusCode::FAILURE;
			// prepare new table with counters
			for(size_t ii=0;ii<9;++ii)
			  m_counters[(*i)].push_back(0);
	}


  return StatusCode::SUCCESS;

}

StatusCode HLTEgammaNavMonTool::book_per_signature(const std::string signature) {

    *m_log << MSG::DEBUG << "Configuring signature : " << signature << std::endl;
    
    std::string BasicPath("HLT/Egamma/");
    BasicPath+=signature;
    addMonGroup(new MonGroup(this,BasicPath,run));

    addMonGroup(new MonGroup(this,BasicPath+"/L1",run));
    addHistogram(new TH1F("EmClus", "L1 EM Cluster Energy; L1 EM Cluster Energy [GeV]; Probability", 80, 0., 80.));
    addHistogram(new TH1F("EmIsol", "L1 EM Isolation Energy; L1 EM Isolation Energy [GeV]; Probability", 15, 0., 15.));
    addHistogram(new TH1F("HadCore", "L1 Hadronic Core Energy; L1 Had Core Energy [GeV]; Probability", 15, 0., 15.));
    addHistogram(new TH1F("HadIsol", "L1 Hadronic Isolation Energy ; L1 Had Isolation Energy [GeV] ; Probability", 15, 0., 15.));
    addHistogram(new TH1F("Eta", "L1 \\eta ; L1 \\eta ; Probability", 100, -2.5,2.5));
    addHistogram(new TH1F("Phi", "L1 \\phi ; L1 \\phi [rad]; Probability", 128, -3.2,3.2));
  
    addMonGroup(new MonGroup(this,BasicPath+"/L2Calo",run));
    addHistogram(new TH1F("Rcore", "L2 Calo R\\eta ; L2 Calo R\\eta ; Probability", 100, 0., 1.));
    addHistogram(new TH1F("Rstrip", "L2 Calo E_{ratio} ; L2 Calo E_{ratio} ; Probability", 100, 0., 1.));
    addHistogram(new TH1F("Et", "L2 EM Calo E_{T} ; L2 EM Calo E_{T} [GeV] ; Probability", 160, 0., 80.));
    addHistogram(new TH1F("HadEt", "L2 Hadronic Calo E_{T} ; L2 Hadronic Calo E_{T} [GeV]; Probability", 20, 0., 2.));
    addHistogram(new TH1F("eta", "L2 Calo \\eta ; L2 Calo \\eta ; Probability", 100, -2.5, 2.5));
    addHistogram(new TH1F("phi", "L2 Calo \\phi ; L2 Calo \\phi [rad] ; Probability", 128, -2.5, 3.2));

    addMonGroup(new MonGroup(this,BasicPath+"/L2IDScan",run));
    addHistogram(new TH1F("Pt", "L2 IDScan p_{T} ; p_{T} [GeV/c] ; Probability", 80, 0., 80.));
    addHistogram(new TH1F("Dphi", "L2 IDScan \\Delta\\phi (\\phi_{Calo} - \\phi_{track at Calo}); L2 IDScan \\Delta\\phi [rad] ; Probability", 128, 0., m_L2IDDphi*4));
    addHistogram(new TH1F("Deta", "L2 IDScan \\Delta\\eta (\\eta_{Calo} - \\eta_{track at Calo}); \\Delta\\eta ; Probability", 128, 0., m_L2IDDeta*4));
//    addHistogram(new TH1F("A0", "Level 2 ID (ID Scan Match) A_{0} ; A_{0} ; Probability", 128, -3., 3.));
//    addHistogram(new TH1F("Z0", "Level 2 ID (ID Scan Match) Z_{0} ; Z_{0} ; Probability", 128, -100., 100.));
    addHistogram(new TH1F("EP", "L2 IDScan E_{T}/p_{T} ; L2 IDScan E_{T}/p_{T} ; Probability", 100, 0., 6.));
    addHistogram(new TH1F("HTOverAll", "L2 IDScan High Threshold Hits over all Hits ; L2 IDScan High Threshold Hits over all Hits ; Probability", 40, 0., 1.));
    addHistogram(new TH1F("TRTHits", "L2 IDScan TRT Hits ; L2 IDScan TRT Hits ; Probability", 50, 0., 50.));
  
    addMonGroup(new MonGroup(this,BasicPath+"/L2IDSi",run));
    addHistogram(new TH1F("Pt", "L2 SiTrack p_{T} ; L2 SiTrack p_{T} [GeV/c] ; Probability", 80, 0., 80.));
    addHistogram(new TH1F("Dphi", "L2 SiTrack \\Delta\\phi (\\phi_{Calo} - \\phi_{track at Calo}); L2 SiTrack \\Delta\\phi [rad] ; Probability", 128, 0., m_L2IDDphi*4));
    addHistogram(new TH1F("Deta", "L2 SiTrack \\Delta\\eta (\\eta_{Calo} - \\eta_{track at Calo}); \\Delta\\eta ; Probability", 128, 0., m_L2IDDeta*4));
//    addHistogram(new TH1F("A0", "Level 2 ID (SiTrack Match) A_{0} ; A_{0} ; Probability", 128, -3., 3.));
//    addHistogram(new TH1F("Z0", "Level 2 ID (SiTrack Match) Z_{0} ; Z_{0} ; Probability", 128, -100., 100.));
    addHistogram(new TH1F("EP", "L2 SiTrack E_{T}/p_{T} ; L2 SiTrack E_{T}/p_{T} ; Probability", 100, 0., 6.));
    addHistogram(new TH1F("HTOverAll", "L2 SiTrack High Threshold Hits over all Hits ; L2 SiTrack High Threshold Hits over all Hits ; Probability", 40, 0., 1.));
    addHistogram(new TH1F("TRTHits", "L2 SiTrack TRT Hits ; L2 SiTrack TRT Hits ; Probability", 50, 0., 50.));
 
    addMonGroup(new MonGroup(this,BasicPath+"/EFScanCalo",run));
    addHistogram(new TH1F("Et", "EF Calo E_{T} (for  L2 IDScan chains) ; E_{T} [GeV] ; Probability", 80, 0, 80.));
    addHistogram(new TH1F("Dphi", "EF Calo \\Delta\\phi (\\phi_{Calo} - \\phi_{L2 IDScan}) ; \\Delta\\phi [rad] ; Probability", 128, 0., 2*m_EFCaloDphi));
    addHistogram(new TH1F("Deta", "EF Calo \\Delta\\eta (\\eta_{Calo} - \\eta_{L2 IDScan}) ; \\Delta\\eta ; Probability", 128, 0., 2*m_EFCaloDeta));

    addMonGroup(new MonGroup(this,BasicPath+"/EFSiCalo",run));
    addHistogram(new TH1F("Et", "EF Calo E_{T} (for L2 SiTrack chains) ; E_{T} [GeV] ; Probability", 80, 0, 80.));
    addHistogram(new TH1F("Dphi", "EF Calo \\Delta\\phi (\\phi_{Calo} - \\phi_{L2 SiTrack}) ; \\Delta\\phi [rad] ; Probability", 128, 0., 2*m_EFCaloDphi));
    addHistogram(new TH1F("Deta", "EF Calo \\Delta\\eta (\\eta_{Calo} - \\eta_{L2 SiTrack}) ; \\Delta\\eta ; Probability", 128, 0., 2*m_EFCaloDeta));
 
    addMonGroup(new MonGroup(this,BasicPath+"/EFScanID",run));
    addHistogram(new TH1F("Phi", "EF Track \\phi (for L2 IDScan chains) ; EF Track \\phi ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Eta", "EF Track \\eta (for L2 IDScan chains) ; EF Track \\eta ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Pt", "EF Track p_{T} (for L2 IDScan chains) ; EF Track p_{T} [GeV/c] ; Probability", 100, 0, 80.));
//    addHistogram(new TH1F("D0", "d0 for EF Track (after L2 ID Scan Match) ; d0 [mm] ; Probability", 100, -0.5, 0.5));
//    addHistogram(new TH1F("Z0", "z0 for EF Track (after L2 ID Scan Match) ; z0 [mm] ; Probability", 100, -200, 200));
    addHistogram(new TH1F("Qual", "EF Track Quality (for L2 IDScan chains) ; EF Track Quality ; Probability", 60, 0., 60.));
    addHistogram(new TH1F("Pixel", "EF Track Number of Pixel Hits (for L2 IDScan chains) ; EF Track Number of Pixel Hits ; Probability", 8, 0., 8.));
    addHistogram(new TH1F("SCT", "EF Track Number of SCT Hits (for L2 IDScan chains) ; EF Track Number of SCT Hits ; Probability", 14, 0., 14.));
    addHistogram(new TH1F("TRT", "EF Track Number of TRT Hits (for L2 IDScan chains) ; EF Track Number of TRT Hits ; Probability", 50, 0., 50.));
    addHistogram(new TH1F("BLayer", "EF Track Number of B-Layer Hits (for L2 IDScan chains) ; EF Track Number of B-Layer Hits ; Probability", 5, 0., 5.));

    addMonGroup(new MonGroup(this,BasicPath+"/EFSiID",run));
    addHistogram(new TH1F("Phi", "\\phi for EF Track (after L2 ID SiTrack Match) ; \\phi ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Eta", "\\eta for EF Track (after L2 ID SiTrack Match) ; \\eta ; Probability", 100, -3.2, 3.2));
    addHistogram(new TH1F("Pt", "p_{T} for EF Track (after L2 ID SiTrack Match) ; P_T [GeV] ; Probability", 100, 0, 80.));
//    addHistogram(new TH1F("D0", "d0 for EF Track (after L2 ID SiTrack Match) ; d0 [mm] ; Probability", 100, -0.5, 0.5));
//    addHistogram(new TH1F("Z0", "z0 for EF Track (after L2 ID SiTrack Match) ; z0 [mm] ; Probability", 100, -200, 200));
    addHistogram(new TH1F("Qual", "EF Track Quality (for L2 SiTrack chains) ; EF Track Quality ; Probability", 60, 0., 60.));
    addHistogram(new TH1F("Pixel", "EF Track Number of Pixel Hits (for L2 SiTrack chains) ; EF Track Number of Pixel Hits ; Probability", 8, 0., 8.));
    addHistogram(new TH1F("SCT", "EF Track Number of SCT Hits (for L2 SiTrack chains) ; EF Track Number of SCT Hits ; Probability", 14, 0., 14.));
    addHistogram(new TH1F("TRT", "EF Track Number of TRT Hits (for L2 SiTrack chains) ; EF Track Number of TRT Hits ; Probability", 50, 0., 50.));
    addHistogram(new TH1F("BLayer", "EF Track Number of B-Layer Hits (for L2 SiTrack chains) ; EF Track Number of B-Layer Hits ; Probability", 5, 0., 5.));

    addMonGroup(new MonGroup(this,BasicPath+"/Rates",run));
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
  
  return StatusCode::SUCCESS;
}

StatusCode HLTEgammaNavMonTool::fill() {

	//m_nTotal++;
	for(std::vector<std::string>::const_iterator i = m_signatures.begin();
		i!=m_signatures.end();++i){
		*m_log << MSG::DEBUG << "fill signature : " << (*i) << endreq;
		m_counters[(*i)][0]++; // m_nTotal
		if (fill_per_signature((*i)).isFailure() ) return StatusCode::FAILURE;
		if (rate_per_signature((*i)).isFailure() ) return StatusCode::FAILURE;
	}
  return StatusCode::SUCCESS;
}

StatusCode HLTEgammaNavMonTool::fill_per_signature(const std::string signature) {

  // First prepare path
  std::string BasicPath("HLT/Egamma/");
  BasicPath+=signature;

  Trig::FeatureContainer tmp_features = getTDT()->features("HLT_"+signature,m_fill_condition);
  if ( !tmp_features.getCombinations().size() ) return StatusCode::SUCCESS;
  Trig::FeatureContainer::combination_const_iterator cIt = tmp_features.getCombinations().begin();
  for ( ; cIt != tmp_features.getCombinations().end(); ++cIt){ // For each RoI

  std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = cIt->get<TrigRoiDescriptor>("initialRoI",m_fill_condition);
  if ( initRois.size() < 1 ) // Nothing found
	return StatusCode::SUCCESS;

  Trig::Feature<xAOD::EmTauRoI> itEMTau = getTDT()->ancestor<xAOD::EmTauRoI>(initRois[0]);

  if ( !itEMTau.empty() ){
    FillLVL1((*itEMTau.cptr()),BasicPath+"/L1");
  }
  else return StatusCode::SUCCESS;

  std::vector< Trig::Feature<xAOD::TrigEMCluster> > L2Calo = cIt->get<xAOD::TrigEMCluster>("",m_fill_condition);
  if ( L2Calo.size() < 1 )
	return StatusCode::SUCCESS;
  const xAOD::TrigEMCluster* mL2 (0); 
  for(std::vector< Trig::Feature<xAOD::TrigEMCluster> >::const_iterator matchedL2 =
	L2Calo.begin(); matchedL2!= L2Calo.end(); ++ matchedL2 ){
	if ( matchedL2->te()->getActiveState() ) {
	     mL2 = matchedL2->cptr();
  	     FillLVL2Calo(mL2, BasicPath+"/L2Calo");
	}
  }
  if ( !mL2 ) { return StatusCode::SUCCESS; } // End it here



  auto L2El = cIt->get<xAOD::TrigElectronContainer>("",m_fill_condition);
  /*std::cout << rtti_real_name<decltype(L2El)>() << std::endl;; */
  if ( L2El.size() < 1 ) return StatusCode::SUCCESS;
  if ( L2El[0].cptr()->size() < 1 ) return StatusCode::SUCCESS;
  xAOD::TrigElectronContainer::const_iterator L2_thisEL = L2El[0].cptr()->begin();
  const xAOD::TrigElectron* myL2 = (*L2_thisEL);
#ifdef DONTDO
  std::cout << "; et : " <<        myL2->pt();
  std::cout << "; eta : " <<       myL2->eta();
  std::cout << "; phi : " <<       myL2->phi();
  std::cout << "; clus.e : " <<    (*(myL2->emClusterLink()))->et();
  std::cout << "; clus.eta : " <<  (*(myL2->emClusterLink()))->eta();
  std::cout << "; clus.phi : " <<  (*(myL2->emClusterLink()))->phi();
  std::cout << "; track pt : " <<  (*(myL2->trackParticleLink()))->pt();
  std::cout << "; track eta : " << (*(myL2->trackParticleLink()))->eta();
  std::cout << "; track phi : " << (*(myL2->trackParticleLink()))->phi();
  std::cout << std::hex;
  std::cout << "; track recoInfo : " << (*(myL2->trackParticleLink()))->patternRecoInfo();
  std::cout << std::dec;
  std::cout << std::endl;

  //std::vector< Trig::Feature<TrigInDetTrackCollection> > L2ID = cIt->get<TrigInDetTrackCollection>("",m_fill_condition);
  std::vector< Trig::Feature<xAOD::TrackParticleContainer> > L2ID = cIt->get<xAOD::TrackParticleContainer>("",m_fill_condition);
  std::cout << "Hello number of tracks : " << L2ID.size() << std::endl;
  if ( !L2ID.size() ) { return StatusCode::SUCCESS; } // End it here
  if ( !L2ID[0].te()->getActiveState() ) { return StatusCode::SUCCESS; } // End it here
  bool passIDScan = false;
  bool passSiTrk = false;
  for(xAOD::TrackParticleContainer::const_iterator Lvl2t_init = L2ID[0].cptr()->begin();
	Lvl2t_init!= L2ID[0].cptr()->end(); ++ Lvl2t_init ){
	     const xAOD::TrackParticle* trk = *Lvl2t_init;
	     double etac, phic;
/*
	     TrigInDetTrackHelper trackHelper(trk->param());
             trackHelper.extrapolate(1470.0*CLHEP::mm,3800.0*CLHEP::mm, phic, etac);
	     float etacf = (float)etac;
	     float phicf = (float)phic;
	     float pt = (float)trk->param()->pT();
*/
	//     ToolHandle<IExtrapolateToCaloTool> m_trackExtrapolator("IExtrapolateToCaloTool/ExtrapolateToCaloTool");
	     // Removing extrapolator -- requires migration to new tools
	     // See TrigL2ElectronFex
             float offset=0.0;
	     float etacf = 999.0;
	     float phicf = 999.0;
	     float pt = trk->pt();
	     float ll=0; float jj=0;
	     std::cout << "Track pt : " << trk->pt();
	     std::cout << "; pattern : " << trk->patternRecoInfo() << std::endl;
	      if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ) std::cout << "Fast" << std::endl;
	      if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ) std::cout << "strategyA" << std::endl;
	      if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyB] ) std::cout << "strategyB" << std::endl;
	      if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyC] ) std::cout << "strategyC" << std::endl;
	     if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ){
  	        FillLVL2ID(pt,etacf,phicf,ll,jj,mL2,BasicPath+"/L2IDScan");
		passIDScan=true;
		std::cout << "got passIDScan" << std::endl;
	     }
	     if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ){
	     //if ( trk->algorithmId() == TrigInDetTrack::SITRACKID ){
  	        FillLVL2ID(pt,etacf,phicf,ll,jj,mL2,BasicPath+"/L2IDSi");
	        passSiTrk=true;
		std::cout << "got passSiTrack" << std::endl;
	     }
  }
#endif

  bool passIDScan = false;
  bool passSiTrk = false;
  float etacf = 999.0;
  float phicf = 999.0;
  // Removing extrapolator -- requires migration to new tools
  // See TrigL2ElectronFex
  // float offset=0.0;
  const xAOD::TrackParticle* trk = ((myL2->trackParticle()));
  float pt = trk->pt();
  float ll=0; float jj=0;
  if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ){
     FillLVL2ID(pt,etacf,phicf,ll,jj,mL2,BasicPath+"/L2IDScan");
     passIDScan=true;
  }
  if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ){
  //if ( trk->algorithmId() == TrigInDetTrack::SITRACKID ){
     FillLVL2ID(pt,etacf,phicf,ll,jj,mL2,BasicPath+"/L2IDSi");
     passSiTrk=true;
  }

  if ( !(passIDScan || passSiTrk) )
	{ return StatusCode::SUCCESS; } // End it here

  std::vector< Trig::Feature<xAOD::CaloClusterContainer> > EFCalo = cIt->get<xAOD::CaloClusterContainer>("",m_fill_condition);
  if ( !EFCalo.size() ) { return StatusCode::SUCCESS; } // End it here
  if ( !EFCalo[0].te()->getActiveState() ) { return StatusCode::SUCCESS; } // End it here
  for(xAOD::CaloClusterContainer::const_iterator CCItr = EFCalo[0].cptr()->begin();
	CCItr != EFCalo[0].cptr()->end(); ++CCItr ){
	double DetaEFCalo = fabsf((*CCItr)->eta() - mL2->eta());
        double DphiEFCalo = delta((*CCItr)->phi() - mL2->phi());
        const xAOD::CaloCluster * matchedEFCalo = (*CCItr);
  	if ( passIDScan ) {
	FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, BasicPath+"/EFScanCalo");
	}
  	if ( passSiTrk )  {
	FillEFCalo(matchedEFCalo, DetaEFCalo, DphiEFCalo, BasicPath+"/EFSiCalo");

	}
  }

  std::vector< Trig::Feature<xAOD::ElectronContainer> > EGCalo = cIt->get<xAOD::ElectronContainer>("",m_fill_condition);
  if ( !EGCalo.size() ) { return StatusCode::SUCCESS; } // End it here
  for(size_t eg=0; eg<EGCalo.size(); ++eg){
    //if ( !EGCalo[eg].te()->getActiveState() && ((EGCalo[eg].label().find("Electron") != std::string::npos) || (EGCalo[eg].label().find("Photon") != std::string::npos))  ) { return StatusCode::SUCCESS; } // End it here
    if ( !EGCalo[eg].te()->getActiveState() ) { return StatusCode::SUCCESS; } // End it here
    // Auxiliary vector for EF FEX variables
    std::map<std::string, std::vector<float> *> vectScan, vectSi;
    vectScan["Phi"] = new std::vector<float>;
    vectScan["Eta"] = new std::vector<float>;
    vectScan["Pt"] = new std::vector<float>;
    vectScan["Qual"] = new std::vector<float>;
    vectScan["Pixel"] = new std::vector<float>;
    vectScan["SCT"] = new std::vector<float>;
    vectScan["TRT"] = new std::vector<float>;
    vectScan["BLayer"] = new std::vector<float>;
    
    vectSi["Phi"] = new std::vector<float>;
    vectSi["Eta"] = new std::vector<float>;
    vectSi["Pt"] = new std::vector<float>;
    vectSi["Qual"] = new std::vector<float>;
    vectSi["Pixel"] = new std::vector<float>;
    vectSi["SCT"] = new std::vector<float>;
    vectSi["TRT"] = new std::vector<float>;
    vectSi["BLayer"] = new std::vector<float>;
    for(xAOD::ElectronContainer::const_iterator eCItr = EGCalo[eg].cptr()->begin();
	eCItr != EGCalo[eg].cptr()->end(); ++eCItr ){
      const xAOD::Electron *matchedEFTrk = *eCItr;
      if ( passIDScan ) {
	FillEFTrack(matchedEFTrk, vectScan);
	FillEFIDHistos(vectScan, BasicPath+"/EFScanID");
      }
      if ( passSiTrk )  {
	FillEFTrack(matchedEFTrk, vectSi);
	FillEFIDHistos(vectSi, BasicPath+"/EFSiID");
      }
    }

    //Delete
    for (std::map<std::string, std::vector<float> *>::iterator i = vectSi.begin(); i != vectSi.end(); i++) {
      if (i->second)
	delete i->second;
    }
    //Delete
    for (std::map<std::string, std::vector<float> *>::iterator i = vectScan.begin(); i != vectScan.end(); i++) {
      if (i->second)
	delete i->second;
    }

  } // end of loop over EGCalo's
  

  } // End of loop over combinations RoIs
  return StatusCode::SUCCESS;

} // End of fill_per_signature


StatusCode HLTEgammaNavMonTool::rate_per_signature(const std::string signature) {

  const EventInfo *eventInfo = 0;
  if ((m_storeGate->retrieve(eventInfo)).isFailure()) {
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

  // First prepare path
  std::string BasicPathRates("HLT/Egamma/");
  BasicPathRates+=signature;
  BasicPathRates+="/Rates";
  setCurrentMonGroup(BasicPathRates);

  Trig::FeatureContainer tmp_features = getTDT()->features("HLT_"+signature,m_rate_condition);
  if ( !tmp_features.getCombinations().size() ) return StatusCode::SUCCESS;
  Trig::FeatureContainer::combination_const_iterator cIt = tmp_features.getCombinations().begin();
  for ( ; cIt != tmp_features.getCombinations().end(); ++cIt){ // For each RoI

  std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = cIt->get<TrigRoiDescriptor>("initialRoI",m_rate_condition);
  if ( initRois.size() < 1 ) // Nothing found
	return StatusCode::SUCCESS;

  Trig::Feature<xAOD::EmTauRoI> itEMTau = getTDT()->ancestor<xAOD::EmTauRoI>(initRois[0]);

  if ( !itEMTau.empty() ){
    m_counters[signature][1]++; // m_nL1++;
    hist("L1Et")->Fill((itEMTau.cptr())->emClus()/1e3);
    hist("L1Eta")->Fill((itEMTau.cptr())->eta());
    hist("L1Phi")->Fill((itEMTau.cptr())->phi());
  }
  else return StatusCode::SUCCESS;

  std::vector< Trig::Feature<xAOD::TrigEMCluster> > L2Calo = cIt->get<xAOD::TrigEMCluster>("",m_rate_condition);
  if ( L2Calo.size() < 1 )
	return StatusCode::SUCCESS;
  const xAOD::TrigEMCluster* mL2 (0); 
  for(std::vector< Trig::Feature<xAOD::TrigEMCluster> >::const_iterator matchedL2 =
	L2Calo.begin(); matchedL2!= L2Calo.end(); ++ matchedL2 ){
	if ( matchedL2->te()->getActiveState() ) {
	     mL2 = matchedL2->cptr();
	     m_counters[signature][2]++; // m_nL2++;
             hist("L2CaloEt")->Fill((mL2->energy()/cosh(mL2->eta()))/1.e3);
             hist("L2CaloEta")->Fill(mL2->eta());
             hist("L2CaloPhi")->Fill(mL2->phi());
	}
  }
  if ( !mL2 ) { return StatusCode::SUCCESS; } // End it here

  std::vector< Trig::Feature<xAOD::TrackParticleContainer > > L2ID = cIt->get<xAOD::TrackParticleContainer>("",m_rate_condition);
  if ( !L2ID.size() ) { return StatusCode::SUCCESS; } // End it here
  if ( !L2ID[0].te()->getActiveState() ) { return StatusCode::SUCCESS; } // End it here
  bool passIDScan = false;
  bool passSiTrk = false;
  for(xAOD::TrackParticleContainer::const_iterator Lvl2t_init = L2ID[0].cptr()->begin();
	Lvl2t_init!= L2ID[0].cptr()->end(); ++ Lvl2t_init ){
	     const xAOD::TrackParticle* trk = *Lvl2t_init;
/*
	     TrigInDetTrackHelper trackHelper(trk->param());
             trackHelper.extrapolate(1470.0*CLHEP::mm,3800.0*CLHEP::mm, phic, etac);
	     float etacf = (float)etac;
	     float phicf = (float)phic;
	     float pt = (float)trk->param()->pT();
*/
	     float pt = trk->pt(); 

	     if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ){
		hist("L2IDScanEt")->Fill(fabsf(pt)/1e3);
                hist("L2IDScanEta")->Fill(trk->eta());
                hist("L2IDScanPhi")->Fill(trk->phi());
		m_counters[signature][3]++; //m_nL2IDScan++;
		passIDScan=true;
	     }
	     if ( trk->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] ){
                hist("L2IDSiEt")->Fill(fabsf(pt)/1e3);
                hist("L2IDSiEta")->Fill(trk->eta());
                hist("L2IDSiPhi")->Fill(trk->phi());
		m_counters[signature][4]++; //m_nL2IDSi++;
	        passSiTrk=true;
	     }
  }
  if ( !(passIDScan || passSiTrk) )
	{ return StatusCode::SUCCESS; } // End it here

  std::vector< Trig::Feature<xAOD::CaloClusterContainer> > EFCalo = cIt->get<xAOD::CaloClusterContainer>("",m_rate_condition);
  if ( !EFCalo.size() ) { return StatusCode::SUCCESS; } // End it here
  if ( !EFCalo[0].te()->getActiveState() ) { return StatusCode::SUCCESS; } // End it here

  const xAOD::CaloCluster* matchedEFCalo = 0;
  for(xAOD::CaloClusterContainer::const_iterator CCItr = EFCalo[0].cptr()->begin();
	CCItr != EFCalo[0].cptr()->end(); ++CCItr ){
        //double DetaEFCalo = fabsf((*CCItr)->eta() - mL2->eta());
        //double DphiEFCalo = delta((*CCItr)->phi() - mL2->phi());
        matchedEFCalo = (*CCItr);
  	if ( passIDScan ) {
	hist("EFScanCaloEt")->Fill( matchedEFCalo->et()/1e3);
	hist("EFScanCaloEta")->Fill(matchedEFCalo->eta());
	hist("EFScanCaloPhi")->Fill(matchedEFCalo->phi());
	m_counters[signature][5]++; //m_nEFScanCalo++;
	}
  	if ( passSiTrk )  {
	hist("EFSiCaloEt")->Fill(matchedEFCalo->et()/1e3);
	hist("EFSiCaloEta")->Fill(matchedEFCalo->eta());
	hist("EFSiCaloPhi")->Fill(matchedEFCalo->phi());
	m_counters[signature][6]++; //m_nEFSiCalo++;
	}
  }

  std::vector< Trig::Feature<xAOD::ElectronContainer> > EGCalo = cIt->get<xAOD::ElectronContainer>("",m_rate_condition);
  if ( (!EGCalo.size()) || !matchedEFCalo ) { return StatusCode::SUCCESS; } // End it here
  for(size_t eg=0; eg<EGCalo.size(); ++eg){
  if ( !EGCalo[eg].te()->getActiveState() && ((EGCalo[eg].label().find("Electron") != std::string::npos) || (EGCalo[eg].label().find("Photon") != std::string::npos))  ) { return StatusCode::SUCCESS; }
  for(xAOD::ElectronContainer::const_iterator eCItr = EGCalo[eg].cptr()->begin();
	eCItr != EGCalo[eg].cptr()->end(); ++eCItr ){
        const xAOD::Electron *matchedEFTrk = *eCItr;
	if ( matchedEFTrk->trackParticle()  ) {
        double pt = matchedEFTrk->trackParticle()->pt();

        if ( passIDScan ) {
	hist("EFScanIDEt")->Fill(pt);
	hist("EFScanIDEta")->Fill(matchedEFCalo->eta());
	hist("EFScanIDPhi")->Fill(matchedEFCalo->phi());
	m_counters[signature][7]++; //m_nEFScanTrack++;
	}
	if ( passSiTrk )  {
	hist("EFSiIDEt")->Fill(pt);
	hist("EFSiIDEta")->Fill(matchedEFCalo->eta());
	hist("EFSiIDPhi")->Fill(matchedEFCalo->phi());
	m_counters[signature][8]++; //m_nEFSiTrack++;
	}
	} // End of if
  }
  } // End of loop of egamma's
  

  } // End of loop over combinations RoIs
  return StatusCode::SUCCESS;

} // End of rate_per_signature

StatusCode HLTEgammaNavMonTool::proc() {
  if (endOfRun) {
        for(std::vector<std::string>::const_iterator i = m_signatures.begin();
                i!=m_signatures.end();++i){
                *m_log << MSG::INFO << "table for signature : " << (*i) << std::endl;
                if (proc_per_signature((*i)).isFailure() ) return StatusCode::FAILURE;
        }

  }
  *m_log << MSG::DEBUG << "BEGIN TimeStamps" << endreq;
  *m_log << MSG::DEBUG << "First      " << m_firstTimeStamp << endreq;
  *m_log << MSG::DEBUG << "Last       " << m_lastTimeStamp << endreq;
  *m_log << MSG::DEBUG << "END TimeStamps" << endreq;

  return StatusCode::SUCCESS;
}
StatusCode HLTEgammaNavMonTool::proc_per_signature(const std::string signature) {

    if (m_lastTimeStamp != m_firstTimeStamp) {
      float factor = 1.0/fabsf((float) (m_lastTimeStamp - m_firstTimeStamp)); // Get rates! (no idea of what happens to SIMULATED data! Should work well on real data)

      // First prepare path
      std::string BasicPathRates("HLT/Egamma/");
      BasicPathRates+=signature;
      BasicPathRates+="/Rates";
      setCurrentMonGroup(BasicPathRates);

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
 
/*
      scaleError(factor, hist("EFScanIDEt"));
      scaleError(factor, hist("EFScanIDEta"));
      scaleError(factor, hist("EFScanIDPhi"));

      scaleError(factor, hist("EFSiIDEt"));
      scaleError(factor, hist("EFSiIDEta"));
      scaleError(factor, hist("EFSiIDPhi"));
*/
    }
    
    m_nTotal = m_counters[signature][0];
    m_nL1 = m_counters[signature][1];
    m_nL2 = m_counters[signature][2];
    m_nL2IDScan = m_counters[signature][3];
    m_nL2IDSi = m_counters[signature][4];
    m_nEFScanCalo = m_counters[signature][5];
    m_nEFSiCalo = m_counters[signature][6];
    m_nEFScanTrack = m_counters[signature][7];
    m_nEFSiTrack = m_counters[signature][8];
    
    PrintTable(false);
  return StatusCode::SUCCESS;
}

