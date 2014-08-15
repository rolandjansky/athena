/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitoring/HLTEgammaDumpTool.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "CaloEvent/CaloCluster.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"
//#include "egammaEvent/egammaContainer.h"
//#include "egammaEvent/egamma.h"
#include "xAODEgamma/ElectronContainer.h"
#include "egammaEvent/EMTrackMatch.h"

#include "TH1F.h"
#include <string>

HLTEgammaDumpTool::HLTEgammaDumpTool(const std::string & type, const std::string & name, const IInterface* parent) : IHLTMonTool(type,name,parent){
}

HLTEgammaDumpTool::~HLTEgammaDumpTool() {
}

StatusCode HLTEgammaDumpTool::book(){

  addMonGroup( new MonGroup(this,"HLT/EgammaDump",run) );

  //LVL1
  addHistogram(new TH1F("LVL1Et","LVL1 E_T; E_T (GeV); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("LVL1Eta","LVL1 \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("LVL1Phi","LVL1 \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );
  addHistogram(new TH1F("LVL1EmIsol","LVL1 EmIsol; LVL1 EmIsol (GeV); Number of RoIs",40,0.,20.) );
  addHistogram(new TH1F("LVL1HadCore","LVL1 HadCore; LVL1 HadCore (GeV); Number of RoIs",40,0.,20.) );
  addHistogram(new TH1F("LVL1HadIsol","LVL1 HadIsol; LVL1 HadIsol (GeV); Number of RoIs",40,0.,20.) );

  //LVL2 Calo
  addHistogram(new TH1F("TrigEMClusterEt","TrigEMCluster E_T; E_T (GeV); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("TrigEMClusterEta","TrigEMCluster \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("TrigEMClusterPhi","TrigEMCluster \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );
  addHistogram(new TH1F("TrigEMClusterRcore","TrigEMCluster Rcore; Rcore ; Number of RoIs",50,0.,1.) );
  addHistogram(new TH1F("TrigEMClusterEratio","TrigEMCluster Eratio; Eratio ; Number of RoIs",50,0.,1.) );
    
  //LVL2 ID
  addHistogram(new TH1F("IdScanPt","IdScan P_T; P_T (GeV/c); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("IdScanEta","IdScan \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("IdScanPhi","IdScan \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );
  addHistogram(new TH1F("SiTrackPt","SiTrack P_T; P_T (GeV/c); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("SiTrackEta","SiTrack \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("SiTrackPhi","SiTrack \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );
  
  addHistogram(new TH1F("UnknownAlgoPt","Unknown LVL2 Tracking Algo's P_T; P_T (GeV/c); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("UnknownAlgoEta","Unknown LVL2 Tracking Algo's \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("UnknownAlgoPhi","Unknown LVL2 Tracking Algo's \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );
  
  addHistogram(new TH1F("TRTXKIDPt","TRT xKalman LVL2 ID P_T; P_T (GeV/c); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("TRTXKIDEta","TRT xKalman LVL2 ID \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("TRTXKIDPhi","TRT xKalman LVL2 ID \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );

  addHistogram(new TH1F("TRTLUTIDPt","TRT LUT LVL2 ID P_T; P_T (GeV/c); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("TRTLUTIDEta","TRT LUT LVL2 ID \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("TRTLUTIDPhi","TRT LUT LVL2 ID \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );
  
  // EF Calo
  addHistogram(new TH1F("EFCaloEt","EF Calo E_T; E_T (GeV); Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("EFCaloEta","EF Calo \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("EFCaloPhi","EF Calo \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );

  // EF Track
  addHistogram(new TH1F("EFTrackPt","EF Track P_T; PT (GeV/c): Number of RoIs",100,0.,100.) );
  addHistogram(new TH1F("EFTrackEta","EF Track \\eta; \\eta; Number of RoIs",50,-2.5,2.5) );
  addHistogram(new TH1F("EFTrackPhi","EF Track \\phi; \\phi; Number of RoIs",64,-3.2,3.2) );

  *m_log << MSG::DEBUG << "End of book" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode HLTEgammaDumpTool::fill() {


  // LVL1 
  const LVL1_ROI * lvl1ROI;
  if ( (m_storeGate->retrieve(lvl1ROI)).isSuccess() ) {


  LVL1_ROI::emtaus_type::const_iterator itEMTau   =
                                (lvl1ROI->getEmTauROIs()).begin();
  LVL1_ROI::emtaus_type::const_iterator itEMTau_e =
                                (lvl1ROI->getEmTauROIs()).end();

  for( ; itEMTau != itEMTau_e; ++itEMTau) {
      hist("LVL1Et") ->Fill ( (*itEMTau).getEMClus()/1e3  );
      hist("LVL1Eta")->Fill ( (*itEMTau).getEta()  );
      hist("LVL1Phi")->Fill ( (*itEMTau).getPhi()  );
      hist("LVL1EmIsol") ->Fill ( (*itEMTau).getEMIsol()/1e3  );
      hist("LVL1HadCore") ->Fill ( (*itEMTau).getHadCore()/1e3  );
      hist("LVL1HadIsol") ->Fill ( (*itEMTau).getHadIsol()/1e3  );
  } // End of for LVL1
  } else (*m_log) << MSG::DEBUG << "No LVL1 Info Found" << endreq;

  // LVL2 Calo
  const DataHandle<xAOD::TrigEMClusterContainer> l2clus;
  xAOD::TrigEMClusterContainer::const_iterator l2clus_begin,l2clus_end;

  *m_log << MSG::DEBUG << "Check fill" << endreq;
  if (  (m_storeGate->retrieve(l2clus, "HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma") ).isSuccess() ) {
    l2clus_begin = l2clus->begin();
    l2clus_end = l2clus->end();
    for(;l2clus_begin!=l2clus_end; ++l2clus_begin ) {
      hist("TrigEMClusterEt") ->Fill ( (*l2clus_begin)->et()/1e3  );
      hist("TrigEMClusterEta")->Fill ( (*l2clus_begin)->eta() );
      hist("TrigEMClusterPhi")->Fill ( (*l2clus_begin)->phi() );
      if ( (*l2clus_begin)->e277() != 0. )
      hist("TrigEMClusterRcore") ->Fill ( (*l2clus_begin)->e237()/(*l2clus_begin)->e277()  );
      else
      hist("TrigEMClusterRcore") ->Fill ( -1. );
      float Eratio = -1.;
      if ( (*l2clus_begin)->emaxs1() + (*l2clus_begin)->e2tsts1() > 0 ){
        Eratio = ((*l2clus_begin)->emaxs1() - (*l2clus_begin)->e2tsts1() );
        Eratio /= ( (*l2clus_begin)->emaxs1() + (*l2clus_begin)->e2tsts1()) ;
      }
      hist("TrigEMClusterEratio") ->Fill (Eratio);
    }
  } else (*m_log) << MSG::VERBOSE << "No cluster found" << endreq;
  *m_log << MSG::DEBUG << "Check fill2" << endreq;

  // LVL2 ID
  const DataHandle<xAOD::TrackParticleContainer> lvl2t_first, lvl2t_last;

  if ( (m_storeGate->retrieve(lvl2t_first,lvl2t_last)).isSuccess() ) {
    for (; lvl2t_first != lvl2t_last; lvl2t_first++) {
    
        // You need to get the tracks out of the collections
	if ( lvl2t_first.key().find("HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_FTF") == std::string::npos ) continue;
 	xAOD::TrackParticleContainer::const_iterator lvl2t_it = lvl2t_first->begin();
	xAOD::TrackParticleContainer::const_iterator lvl2t_end = lvl2t_first->end();

       for( ; lvl2t_it != lvl2t_end; lvl2t_it++){
		std::string author;
/*
		if ((*lvl2t_it)->algorithmId() == TrigInDetTrack::IDSCANID)
			author = std::string("IdScan");
		else if ((*lvl2t_it)->algorithmId() == TrigInDetTrack::SITRACKID)
			author = std::string("SiTrack");
		else if ((*lvl2t_it)->algorithmId() == TrigInDetTrack::TRTLUTID)
			author = std::string("TRTLUTID");
		else if ((*lvl2t_it)->algorithmId() == TrigInDetTrack::TRTXKID)
			author = std::string("TRTXKID");
		else
			author = std::string("UnknownAlgo");
		hist((author)+"Pt")->Fill( (*lvl2t_it)->pt()/1e3 );
		hist((author)+"Eta")->Fill( (*lvl2t_it)->eta() );
		hist((author)+"Phi")->Fill( (*lvl2t_it)->phi0() );
*/
		
       }
    }
  } else (*m_log) << MSG::VERBOSE << "Could not find TrackParticleContainer" << endreq;

/*
  const DataHandle<xAOD::TrigElectronContainer> trigElectrons;
  if ( (m_storeGate->retrieve(trigElectrons,"HLT_xAOD__TrigElectronContainer_L2ElectronFex")).isSuccess() ) {
	xAOD::TrigElectronContainer::const_iterator trigElectron = trigElectrons->begin();
	xAOD::TrigElectronContainer::const_iterator trigElectronEnd = trigElectrons->end();
        for( ; trigElectron != trigElectronEnd ; ++trigElectron ) {
		std::cout << "TrigElectron::pt() " << (*trigElectron)->pt() << std::endl;
	}
  }
*/

  // EF Calo
  const DataHandle<xAOD::CaloClusterContainer> EFCalo_i, EFCalo_l;
  if ( (m_storeGate->retrieve(EFCalo_i,EFCalo_l)).isSuccess() ) {
     xAOD::CaloClusterContainer::const_iterator CCItr;
     for (;EFCalo_i != EFCalo_l; ++EFCalo_i) {
      if ( (EFCalo_i.key().find("HLTAutoKey_TrigCaloClusterMaker") != std::string::npos) 
	&& (EFCalo_i.key().find("HLTAutoKey_TrigCaloClusterMaker_topo_fullscan") == std::string::npos) ) {
        CCItr = EFCalo_i->begin();
        for (; CCItr != EFCalo_i->end(); CCItr++) {
		hist("EFCaloEt")->Fill ( (*CCItr)->et()/1e3 );
		hist("EFCaloEta")->Fill ( (*CCItr)->eta() );
		hist("EFCaloPhi")->Fill ( (*CCItr)->phi() );
	} // end of for Calo
      } // End of if Calo HLTAutoKey
     } // End of for CaloClusterContainer
  } else (*m_log) << MSG::VERBOSE << "Could not find EFCalo" << endreq;

  // EF Track (must come through egamma)
  const DataHandle<xAOD::ElectronContainer> EFTrk_i, EFTrk_l;
  if ( (m_storeGate->retrieve(EFTrk_i, EFTrk_l)).isSuccess() ) {
      for (; EFTrk_i != EFTrk_l; ++EFTrk_i) {
         //if (EFTrk_i.key().find("HLT_egamma") != std::string::npos) {
         if (EFTrk_i.key().find("HLT_xAOD__ElectronContainer_egamma_Electrons") != std::string::npos) {
           xAOD::ElectronContainer::const_iterator eCItr = EFTrk_i->begin();
           for (; eCItr != EFTrk_i->end(); eCItr++) {
             if ((*eCItr)->trackParticle()) { 
/*
                const Trk::Perigee* aMeasPer
				= (*eCItr)->trackParticle()->perigee();
                float pt2 = pow(aMeasPer->parameters()[Trk::px], 2.)
			+ pow(aMeasPer->parameters()[Trk::py], 2.);
                float eta = -std::log(tan(aMeasPer->parameters()[Trk::theta]/2.));
		hist("EFTrackPt")->Fill ( sqrt(pt2) );
		hist("EFTrackEta")->Fill ( eta );
		hist("EFTrackPhi")->Fill ( aMeasPer->parameters()[Trk::phi] );
*/
	     } // End of if trackParticle
	   } // End of egammaContainer loop
	 } // End of if EFTrk_i check for HLTAutoKey_egamma
      } // end of for over egammaContainers
  } else (*m_log) << MSG::VERBOSE << "Could not retrieve egammaContainer" << endreq;



  return StatusCode::SUCCESS;
}


