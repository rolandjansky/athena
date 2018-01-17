/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************************************
//  Filename : TrigTileMuFex.cxx
//  Author   : Heuijin Lim
//  Created  : Sep. 2007
//
//  DESCRIPTION:
//    Muon candidates from TileCal are stored in the Feature, "TileMuFeature"
//    and produced 2nd-ROIs. From these 2nd-ROIs, the tracks from
//    Inner-Detector are extracted and compared with the muons from TileCal.
//    The combined muons with track are stored in the Feature,
//    "TileTrkMuFeature". They will be used in TileMuHypo.cxx  of
//    Trigger/TrigHypothesis/TrigMuonHypo
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/Timeout.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackExtrapolator.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TileEvent/TileMuContainer.h"
#include "TrigTileMuId/TrigTileMuFex.h"
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"

#include "GeneratorObjects/McEventCollection.h"  // for retrieving MC 

 
class ISvcLocator;



TrigTileMuFex::TrigTileMuFex(const std::string& name, ISvcLocator* pSvcLocator)
    : HLT::FexAlgo(name, pSvcLocator), 
      m_algoId(TrigInDetTrack::NULLID),
      m_Typ_IDTrk(0),
      m_trackExtrapolator(NULL),
      m_MagFieldSvc(NULL)
{
  // standard configuration
  declareProperty("IDalgo",           m_ID_algo_to_use="IDSCAN");
  declareProperty("Meth_ExtrapTileR", m_Meth_ExtrapTileR = "DirectPars");
  // DirectPars : based on offline-analysis
  // EMextraWay : using EM-package way
  declareProperty("DelPhi_Cut",       m_DelPhi_Cut = 0.2);
  declareProperty("DelEta_Cut",       m_DelEta_Cut = 0.11);
  declareProperty("Pt_Cut",           m_Pt_Cut = 2000.0);       // MeV 

  // In order to use the following package 
  declareProperty( "RCalBarrelFace",        m_RCAL = 1470.0*CLHEP::mm );
  declareProperty( "ZCalEndcapFace",        m_ZCAL = 3800.0*CLHEP::mm );
  declareProperty( "TrackExtrapolatorName", m_trackExtrapolatorName = "TrigInDetTrackExtrapolator");

  // magnetic field 
  declareProperty( "UseAthenaFieldService", m_useAthenaFieldService = true); 

  declareProperty( "GetTruthMuon",   	m_GetTruthMuon = false);
  declareProperty( "KeyForTruth", 	m_key_for_truth="TruthEvent");

  // Monitoring

  declareMonitoredVariable("NTrack",  m_NTrack = 0);
  declareMonitoredVariable("NTruth",  m_NTruth = 0);

  declareMonitoredStdContainer("DelEtaTruth", m_DelEtaTruth);
  declareMonitoredStdContainer("DelPhiTruth", m_DelPhiTruth);
  declareMonitoredStdContainer("DelPtTruth",  m_DelPtTruth);
  declareMonitoredStdContainer("PtTruth",     m_PtTruth);
  declareMonitoredStdContainer("PtTrack",     m_PtTrack);
  declareMonitoredStdContainer("PtTrackAll",  m_PtTrackAll);
  declareMonitoredStdContainer("EtaTrackAll", m_EtaTrackAll);
  declareMonitoredStdContainer("PhiTrackAll", m_PhiTrackAll);

}



TrigTileMuFex::~TrigTileMuFex()
{
}



HLT::ErrorCode TrigTileMuFex::hltInitialize()
{
  std::string algoId = m_ID_algo_to_use;

  if(algoId=="IDCSAN")         m_algoId = TrigInDetTrack::IDSCANID;
  else if (algoId=="SITRACK")  m_algoId = TrigInDetTrack::SITRACKID;
  else if (algoId=="TRTXK")    m_algoId = TrigInDetTrack::TRTXKID;
  else if (algoId=="TRTSEG")   m_algoId = TrigInDetTrack::TRTLUTID;
  else                         m_algoId = TrigInDetTrack::IDSCANID;

  m_Typ_IDTrk = 0;
  if ( algoId=="IDSCAN" )		  m_Typ_IDTrk = m_Typ_IDTrk + 1;
  else if ( algoId=="SITRACK")		  m_Typ_IDTrk = m_Typ_IDTrk + 2;
  else if ( algoId=="TRTXK") 	          m_Typ_IDTrk = m_Typ_IDTrk + 3;
  else if ( algoId=="TRTSEG")             m_Typ_IDTrk = m_Typ_IDTrk + 4;  
  else					  m_Typ_IDTrk = m_Typ_IDTrk + 1;
 
  if      ( m_Meth_ExtrapTileR=="DirectPars" ) m_Typ_IDTrk = m_Typ_IDTrk+100;
  else if ( m_Meth_ExtrapTileR=="EMextraWay" ) m_Typ_IDTrk = m_Typ_IDTrk+200;
  else	                                       m_Typ_IDTrk = m_Typ_IDTrk+300;

  StatusCode sc = toolSvc()->retrieveTool(m_trackExtrapolatorName,
                                          m_trackExtrapolatorName,
                                          m_trackExtrapolator);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to locate TrackExtrapolator tool " << m_trackExtrapolatorName);
    return HLT::BAD_JOB_SETUP;
  }

  // Here one is supposed to open the file if needed and do the setup ///////
  ATH_MSG_DEBUG("Initialization completed successfully");

  return HLT::OK;  
}



HLT::ErrorCode TrigTileMuFex::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE)
{
  const double UpperMax = 9999.9;
  const double LowerMax = -9999.9;
  const double User_Pi = 3.141592654;

  m_DelEtaTruth.clear();
  m_DelPhiTruth.clear();
  m_DelPtTruth.clear();
  m_PtTruth.clear();
  m_PtTrack.clear();
  m_EtaTrackAll.clear();
  m_PhiTrackAll.clear();	
  m_PtTrackAll.clear();

  ATH_MSG_DEBUG("outputTE->ID(): " << outputTE->getId());

  //========= Extraction of Muon candidates from TileCal =========
  ElementLink<TileMuFeatureContainer> TileMuFeatureEL;
  if ( HLT::OK != getFeatureLink<TileMuFeatureContainer, TileMuFeature>
				(outputTE, TileMuFeatureEL) )
  {
    ATH_MSG_ERROR(" getFeatureLink fails to get TileMuFeature ");
    return HLT::ERROR;                                                    
  }                                                                     
  if ( !TileMuFeatureEL.isValid() )                                   
  {                                                                  
    ATH_MSG_ERROR(" getFeatureLink finds no TileMuFeature (EL invalid)");
    return HLT::NAV_ERROR;  
  }

  // -------------------------------------------------------------
  /*
  std::vector<const TileMuFeature*> vectorMuon; 
  HLT::ErrorCode status = getFeatures(outputTE, vectorMuon);

  if (status == HLT::OK) {
    ATH_MSG_DEBUG(" Okay to get vectorMuon with size=" << vectorMuon.size());
  } else {
    ATH_MSG_WARNING(" Failed to get vectorMuon ");
    return status;
  }

  if (vectorMuon.size() == 0) {
    ATH_MSG_DEBUG(" N of TileMuFeature == 0");
    return HLT::NAV_ERROR;
  }

  const TileMuFeature* muon = vectorMuon.front();
  if (muon == 0) {
    ATH_MSG_DEBUG(" TileMuFeature pointer null");
    return HLT::NAV_ERROR;
  }
  */
  const TileMuFeature* muon = *TileMuFeatureEL;

  ATH_MSG_DEBUG("REGTEST Eta=" << muon->eta() << " Phi=" << muon->phi() 
	<< " EnergyVec[0] = " << muon->enedep().at(0) 
	<< " EnergyVec[1] = " << muon->enedep().at(1)
	<< " EnergyVec[2] = " << muon->enedep().at(2)
        << " EnergyVec[3] = " << muon->enedep().at(3));

  float eta_tile = muon->eta();
  float phi_tile = muon->phi();
  std::vector<float> E_tile(4);
  E_tile = muon->enedep();
  //float qf_tile = muon->quality();

  bool do_straight = false;

  if (m_useAthenaFieldService) {

    if (!m_MagFieldSvc) service("AtlasFieldSvc", m_MagFieldSvc, /*createIf=*/ false).ignore();

    if (m_MagFieldSvc) {

      // Magnetic field status
      bool solenoidOn = m_MagFieldSvc->solenoidOn();
      bool toroidOn = m_MagFieldSvc->toroidOn();
 
      ATH_MSG_DEBUG("=========== Magnetic Field Status ========== ");
      ATH_MSG_DEBUG(" ---> Solenoid : " << ((solenoidOn)? "ON" : "OFF"));
      ATH_MSG_DEBUG(" ---> Toroid   : " << ((toroidOn)? "ON" : "OFF"));

      if (!solenoidOn) do_straight = true;
    }
  }

  // ---------------------------------------------------------------------

  std::vector<float> TrueMu_Eta;	TrueMu_Eta.clear();
  std::vector<float> TrueMu_Phi;	TrueMu_Phi.clear();
  std::vector<float> TrueMu_Pt;		TrueMu_Pt.clear();
  std::vector<float> TrueMu_PhiTR;	TrueMu_PhiTR.clear();

  if ( m_GetTruthMuon ) {
    
    // retrieving the Montecarlo thru from the Storegate
    const DataHandle<McEventCollection> mcCollptr;
    //std::string       key = "GEN_EVENT";
    std::string   key = m_key_for_truth;
    
    bool cont = evtStore()->contains<McEventCollection>(key);
    StatusCode retr = evtStore()->retrieve(mcCollptr,key);
    if( cont && retr.isSuccess() ) {
      McEventCollection::const_iterator itr;
      for(itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
        HepMC::GenEvent::particle_const_iterator Part;

        for(Part = (*itr)->particles_begin();
            Part!=(*itr)->particles_end(); ++Part ) {

          // store MCthru only for muon particles
          if( ((*Part)->pdg_id() == -13 || (*Part)->pdg_id() == 13)
	      && (*Part)->barcode() < 200000
	      && fabs((*Part)->momentum().perp()) > 500.) {

	    int muCharge   = (*Part)->pdg_id()/(-13);
            float muEnergy = (*Part)->momentum().e();
            float muPhi    = (*Part)->momentum().phi();
            float muPt     = (*Part)->momentum().perp();
            float theta    = atan(muPt/fabs((*Part)->momentum().z()));
            float eta      = -log (tan(theta/2.) );
            float muEta    = ((*Part)->momentum().z() >= 0.)? eta : -eta;

	    float muPhiTR;
	    if (do_straight) {
	      muPhiTR = muPhi;
	    }else{
	      muPhiTR = muPhi - (1.23053e-04*muCharge + 5.06985e-01/(muPt*muCharge/1000.));
	    }

	    ATH_MSG_DEBUG("REGTEST TruthMuon::"
                  << " pdg_id()=" << (*Part)->pdg_id()
                  << " E=" << muEnergy << " Phi=" << muPhi
                  << " Pt=" << muPt*muCharge << " Eta=" << muEta
                  << " Barcode=" << (*Part)->barcode()
                  << " Status=" << (*Part)->status());

            TrueMu_Eta.push_back(muEta);
            TrueMu_Phi.push_back(muPhi); TrueMu_PhiTR.push_back(muPhiTR);
            TrueMu_Pt.push_back(muPt*muCharge); 

          }
        }
      }
    } else {
      ATH_MSG_WARNING("Could not retrieve McEventCollection");
    }
  }
    
  //========= OPEN InDetTrackCollection to get the matching tracks =========

  //std::vector<TrigInDetTrackCollection::const_iterator> mySelectedTracks;
  //mySelectedTracks.clear();

  std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
  HLT::ErrorCode status = getFeatures(outputTE,vectorOfTrackCollections, "");

  TileTrackMuFeature* TileTrkMu = 0;
  bool 	 Get_MatchedTrack = false;

  if(status!=HLT::OK) {
    ATH_MSG_DEBUG(" Failed to get InDetTrackCollections ");
  } else {
    ATH_MSG_DEBUG(" Got " << vectorOfTrackCollections.size()
           << " InDetTrackCollections, now fill InDet Block ");

     if (vectorOfTrackCollections.size() > 0) {

       double DelRMin = UpperMax, DelRNow = UpperMax;
       double Match_pt_idtr = LowerMax;
       //double Match_phi_id = LowerMax;
       double Match_phiTR_id = LowerMax;
       //double Match_eta_id = LowerMax;
       double Match_etaTR_id = LowerMax;
       //double Match_zPos_id = LowerMax;
       const TrigInDetTrack* selTrack=0;
       const TrigInDetTrackCollection* SeltheTrackColl=0;
       int    SelTrackIndex = -1;

       int 	NTrack = 0, NTruth = 0;
 
       std::vector<const TrigInDetTrackCollection*>::iterator
	 theTrackColl = vectorOfTrackCollections.begin(),
	 endTrackColl = vectorOfTrackCollections.end();

       for ( ; theTrackColl != endTrackColl;  theTrackColl++) {

         TrigInDetTrackCollection::const_iterator
	   track     = (*theTrackColl)->begin(),
	   lasttrack = (*theTrackColl)->end();

         for (int trackIndex=0; track !=lasttrack; track++, trackIndex++ ) {

	   // Check if event timeout was reached
	   if (Athena::Timeout::instance().reached()) {
	     ATH_MSG_DEBUG("Timeout reached. Trk loop, Aborting sequence.");
	     return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
	   }

           if( (*track)->algorithmId() == m_algoId ) {

             if (!((*track)->param())) continue;
             
             NTrack++;
             double pt_idtr  = (*track)->param()->pT(); // with sign of charge
             double phi_id   = (*track)->param()->phi0();
             double eta_id   = (*track)->param()->eta();
             double zPos_id  = (*track)->param()->z0();
	     double phiTR_id = (*track)->param()->phi0();
	     double etaTR_id = (*track)->param()->eta();

             m_PtTrackAll.push_back( pt_idtr/1000.); 	// MeV -> GeV       
	     m_EtaTrackAll.push_back( eta_id );
             m_PhiTrackAll.push_back( phi_id );

	     if (m_Meth_ExtrapTileR == "DirectPars") {
	       if (do_straight) {
		 phiTR_id = phi_id;
	       }else{
		 phiTR_id = phi_id - ( 1.23053e-04*(pt_idtr/fabs(pt_idtr)) + 5.06985e-01/(pt_idtr/1000.)); // Unit(pt_idtr) need to be changed into GeV
	       }
	     } else if (m_Meth_ExtrapTileR == "EMextraWay") {
	       StatusCode sc = m_trackExtrapolator->extrapolateToCalo((*track), m_RCAL, m_ZCAL, phiTR_id, etaTR_id); 
             }

	     double DelPhi = phi_tile - phiTR_id;
	     if (DelPhi >      User_Pi) DelPhi -= 2*User_Pi;
             if (DelPhi < (-1)*User_Pi) DelPhi += 2*User_Pi;

	     if ( fabs(pt_idtr) > m_Pt_Cut &&
	          fabs(eta_tile - eta_id) < m_DelEta_Cut &&
                  fabs(DelPhi) < m_DelPhi_Cut ) {

	       DelRNow = sqrt( pow( (eta_tile - eta_id), 2) + pow( DelPhi, 2));
	     
	       if (DelRNow < DelRMin ) {
	         DelRMin = DelRNow;
	         Match_pt_idtr	= pt_idtr;
                 //Match_phi_id	= phi_id;
	         Match_phiTR_id	= phiTR_id;
	         //Match_eta_id	= eta_id; 
                 Match_etaTR_id = etaTR_id;
	         //Match_zPos_id	= zPos_id;

	         selTrack = *track;

                 SeltheTrackColl = *theTrackColl;
	         SelTrackIndex = trackIndex;

	       }
	     }
	   
	     ATH_MSG_DEBUG(
		      "REGTEST For all possible Tracks" << m_ID_algo_to_use
		   << " track with pt=" << pt_idtr
		   << ", eta=" << eta_id
                   << ", etaTR=" << etaTR_id
                   << ", etaTile=" << eta_tile  
                   << ", phi=" << phi_id
		   << ", phiTR=" << phiTR_id
	           << ", phiTile=" << phi_tile 
		   << ", DelphiTR=" << DelRNow
		   << " using " << m_Meth_ExtrapTileR
                   << ", Zid=" << zPos_id);
           }  
         }
       }      

       ATH_MSG_DEBUG("REGTEST NTrack=" << NTrack);
  
       if ( DelRMin < UpperMax ) {
         m_NTrack = NTrack;
         Get_MatchedTrack = true;
         if ( SeltheTrackColl ) {
         ElementLink<TrigInDetTrackCollection> pTrack(*SeltheTrackColl,SelTrackIndex );
	 TileTrkMu = new TileTrackMuFeature(
              Match_pt_idtr,
              Match_etaTR_id, Match_phiTR_id, m_Typ_IDTrk, 
              TileMuFeatureEL, pTrack);
         }

	 ATH_MSG_DEBUG(
		  "REGTEST for final matched Track:" << m_ID_algo_to_use
	       << " track with pT=" << selTrack->param()->pT()
	       << " eta=" << selTrack->param()->eta()
               << " phi=" << selTrack->param()->phi0());

         if ( m_GetTruthMuon ) {
           float DelRMin_TT = UpperMax, DelRNow_TT = UpperMax;
           int iMin = -1;
           for(unsigned int i=0; i<TrueMu_Eta.size(); i++) {
          
	     float SignChk =  selTrack->param()->pT() * TrueMu_Pt.at(i); 
             float DelPhi = selTrack->param()->phi0() - TrueMu_Phi.at(i);
             if (DelPhi >      User_Pi) DelPhi -= 2*User_Pi;
             if (DelPhi < (-1)*User_Pi) DelPhi += 2*User_Pi;
            
             float DelEta = selTrack->param()->eta() - TrueMu_Eta.at(i);  
             if ( SignChk > 0 &&  
		  (fabs(DelPhi) < m_DelPhi_Cut) &&
                  (fabs(DelEta) < m_DelEta_Cut) ) {
	       NTruth++;	

               DelRNow_TT = sqrt( pow(DelPhi,2) + pow(DelEta,2));
               if (DelRNow_TT < DelRMin_TT ) {
                 iMin = i;
                 DelRMin_TT = DelRNow_TT;
               } 
             }          
           }   
 
           if (iMin != -1) {

             m_PtTruth.push_back(TrueMu_Pt.at(iMin)/1000.); // MeV --> GeV
             m_PtTrack.push_back(selTrack->param()->pT()/1000.); // MeV --> GeV
             m_DelEtaTruth.push_back(selTrack->param()->eta() - TrueMu_Eta.at(iMin));
            
             float DelPhi = selTrack->param()->phi0() - TrueMu_Phi.at(iMin);
             if (DelPhi >      User_Pi) DelPhi -= 2*User_Pi;
             if (DelPhi < (-1)*User_Pi) DelPhi += 2*User_Pi;    
             m_DelPhiTruth.push_back(DelPhi);

             float DelPt = (selTrack->param()->pT()-TrueMu_Pt.at(iMin))/1000.;
             m_DelPtTruth.push_back(DelPt);
 
	     ATH_MSG_DEBUG(
		      "REGTEST Matching::"
                   << " [Truth] Pt=" << TrueMu_Pt.at(iMin)
                   << " Eta=" << TrueMu_Eta.at(iMin)
                   << " Phi=" << TrueMu_Phi.at(iMin)
		   << " [Track] Pt=" << selTrack->param()->pT()
                   << " Eta=" << selTrack->param()->eta()
                   << " Phi=" << selTrack->param()->phi0()
		   << " [Comparison] DelEta=" << selTrack->param()->eta() - TrueMu_Eta.at(iMin)
                   << " DelPhi=" << DelPhi
		   << " DelPt=" << DelPt); 
           }
         }  
       }

       if ( m_GetTruthMuon ) {
         m_NTruth = NTruth; 
       } 
     }
  }

  if ( !Get_MatchedTrack ){
    TileTrkMu = new TileTrackMuFeature( -9999.9, -9999.9,  -9999.9, -900,
                                        TileMuFeatureEL,
                                        ElementLink<TrigInDetTrackCollection>());
    ATH_MSG_DEBUG("REGTEST No matched track!");
  }

  /*HLT::ErrorCode HLTStatus =*/ attachFeature(outputTE, TileTrkMu); 
  outputTE->setActiveState(true);

  return HLT::OK;
}



HLT::ErrorCode TrigTileMuFex::hltFinalize()
{
  return HLT::OK;
}
