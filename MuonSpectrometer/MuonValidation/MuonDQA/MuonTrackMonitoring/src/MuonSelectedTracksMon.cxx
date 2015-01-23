/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// MuonSelectedTracksMon
// N. Benekos(Illinois), E. Christidi, A. Eppig, Tony Liss
// November 2007
// modified June 2008: for Trigger Aware monitoring :  A. Cortes (Illinlois)
// ***************************************************************************

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h" 
#include "MuonRecToolInterfaces/IMuonTofTool.h"
#include "MdtCalibSvc/MdtCalibrationSvcInput.h"
 
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"

#include "TrigConfL1Data/TriggerItem.h"
 
#include "TrkTrack/TrackCollection.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h" 
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
 
#include "TrkParameters/TrackParameters.h"
 
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"

#include "TrkMaterialOnTrack/MaterialEffectsBase.h" 
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h" 
  
#include "AthenaMonitoring/AthenaMonManager.h"
#include "MuonTrackMonitoring/MuonSelectedTracksMon.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include <TH1F.h>
#include <TH2F.h> 
#include <TMath.h>
#include "TProfile.h"
#include "TMath.h"
#include <sstream>
#include <math.h>

#include <vector>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <set>

// *********************************************************************
// Public Methods
// *********************************************************************

MuonSelectedTracksMon::MuonSelectedTracksMon( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ), 
   //  commented out - Compilation warnings
  
   m_storeGate(0),
   good(0), 
   good_chi2oDoF(0), 
   good_pT(0), 
   good_eta(0), 
   good_phi(0), 
   good_nStation(0),
   m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
   m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
   m_muonHitSummaryTool("Muon::MuonHitSummaryTool/MuonHitSummaryTool"),
   m_trackSummaryTool("Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool"),
   m_detMgr(0), 
   m_mdtIdHelper(NULL),
   //m_MuonTriggerChainName("NoMuonTriggerSelection"),   
   m_trigDecTool(""),
   m_trackSumTool("Trk::TrackSummaryTool/MuonTrackSummaryTool"),
   trks(NULL), 
   m_TrackCol("Tracks"),
   m_log( msgSvc(), name ),
   m_debuglevel(false), 
   muon_numEvents_passedTrigger(0)
{
  declareProperty("TrackCol" ,             m_TrackCol);
  declareProperty("CheckRate",             m_checkrate=1000);
  declareProperty("NPhiBins",              m_nphi_bins=360);
  declareProperty("MaxEta",                v_maxeta=2.7);
  declareProperty("MinEta",                v_mineta=0.1); 
  declareProperty("AppliedCut",            v_applied_cut=true);
    
  declareProperty("GoodTrk_minPt",         m_gtrk_minPt         = 10.0/*GeV*/);
  declareProperty("GoodTrk_maxChi2oDoF",   m_gtrk_maxChi2oDoF   = 15.0);
  declareProperty("GoodTrk_minPhiHits",    m_gtrk_minPhiHits    = 2);
  declareProperty("GoodTrk_minEtaHits",    m_gtrk_minEtaHits    = 3);
  declareProperty("GoodTrk_minMdtCscHits", m_gtrk_minMdtCscHits = 0); 
  declareProperty("GoodTrk_minStations",   m_gtrk_minStations   = 3);  
  
  declareProperty("MuonTofTool",           m_tofTool);
  declareProperty("DoTofCosmic",           m_DoToFCosmic=false);
  declareProperty("MuonHitSummaryTool",    m_muonHitSummaryTool);
  declareProperty("WhichTrackCollections", m_trackCollectionFlag);
  declareProperty("MuonTrackCollections",  m_trackCollectionName);
  declareProperty("MuonTriggerChainName",  m_MuonTriggerChainName); // Trigger to be studied individually
  declareProperty("TriggerDecisionTool",   m_trigDecTool);          // Decision Tool for TriggerChain
  declareProperty("Muon_Trigger_Items",	   m_muon_triggers);        // Triggers to be studied collectively
  declareProperty("UseTriggerVector",      m_useTrigger);           // User Flag to use TriggerChain
  declareProperty("TrkSummaryTool",        m_trackSumTool);         //User Flag to configure TrackSummaryTool in python
  declareProperty("MuonTrackSummaryHelperTool", m_trackSummaryTool);         //User Flag to configure MuonTrackSummaryTool in python  
}

//======================================================================================//
/** Function to convert Radians into Degrees  */
//======================================================================================//
float MuonSelectedTracksMon::RadsToDegrees(float Radians)
{
  float Degrees = Radians * (180.)/M_PI;
  return Degrees;
}

//======================================================================================//
/**  Destructor */
//======================================================================================//
MuonSelectedTracksMon::~MuonSelectedTracksMon() 
{ 
  m_log << MSG::INFO << " deleting MuonSelectedTracksMon " << endreq;
}

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode MuonSelectedTracksMon:: initialize() 
{ 

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  // init message stream
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached

  if (m_debuglevel) m_log << MSG::DEBUG << "MuonSelectedTracksMon::initialize() to setup tools/services" << endreq;
 
  //  StatusCode sc = StatusCode::SUCCESS;
  sc = StatusCode::SUCCESS;
 
  /** Initialize tools and services */
  if (m_debuglevel) m_log << MSG::DEBUG << "Initialize tools" << endreq;
  sc = setupTools();
   
  return sc;
}


//======================================================================================//
/**  bookHistograms */
//======================================================================================//
StatusCode MuonSelectedTracksMon::bookHistograms()
{
 
  if (m_debuglevel) m_log << MSG::DEBUG << "MuonSelectedTracksMon::bookHistograms() to setup tools/services" << endreq;

  StatusCode sc = StatusCode::SUCCESS;
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {

    /** Creating the histogram to keep track of events that passed each trigger */

    std::string m_generic_path_muonmonselected = "Muon/MuonTrackMonitoring";

    if(newEventsBlock){}
    if(newLumiBlock){}
    if(newRun)
      { 
	if (m_debuglevel) m_log << MSG::DEBUG << " Before TriggerInfo Histogram, UseTrigger? " << m_useTrigger << endreq;
	if (m_debuglevel) m_log << MSG::DEBUG << " Before TriggerInfo Histogram, MuonTriggers size: " << m_muon_triggers.size() << endreq;
	
	/** Creating the passedTrigger histogram only if we're using triggers*/
	if (m_useTrigger && (m_muon_triggers.size() > 1) ) {
	  MonGroup muon_summary_trigInfo ( this, m_generic_path_muonmonselected + "/TriggerAware", run, ATTRIB_MANAGED );
	  std:: string muon_numEvents_passedTrigger_title = "NumberOfEventsThatPassedTriggers";
	  const char* muon_numEvents_passedTrigger_title_char = muon_numEvents_passedTrigger_title.c_str();  

	  /** Number of events passing the given Triggers*/
	  muon_numEvents_passedTrigger = new TH2F(muon_numEvents_passedTrigger_title_char,muon_numEvents_passedTrigger_title_char,(int)m_muon_triggers.size(),0,(int)m_muon_triggers.size(),(int)m_muon_triggers.size(),0,(int)m_muon_triggers.size());
	  muon_numEvents_passedTrigger->SetBit(TH1::kCanRebin);

	  sc=muon_summary_trigInfo.regHist(muon_numEvents_passedTrigger);
	  if(sc.isFailure())
	    { 
	      m_log << MSG::FATAL << "muon_numEvents_passedTrigger Failed to register histogram " << endreq;
	      return sc;
	    } else { if (m_debuglevel) m_log << MSG::DEBUG << "muon_numEvents_passedTrigger has been successfully registered." << endreq; }
	}

	/** Calling booking methods. */
	sc = bookSummaryHistograms();
	sc = bookTrkParametersHistograms();
	sc = bookTrkHolesHistograms();
	sc = bookHitsPerTrkHistograms();
      }
       
  } //environment

  return sc;
}

//======================================================================================//
/**  fill Histograms */
//======================================================================================//
StatusCode MuonSelectedTracksMon::fillHistograms()
{
    
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {

    /** Checking if event passes muon triggers */

    bool useMuonTrigger = ( ( m_muon_triggers.size() != 0 ) && ( ! m_trigDecTool.empty() ) );
    bool passesMuonTrigger = false;

    //  const char* bin_triggerX;
    //  const char* bin_triggerY;

    if (m_debuglevel) m_log << MSG::DEBUG << "Before the loop over triggers, Using trigger? " << m_useTrigger << endreq;

    if( (useMuonTrigger) && (m_useTrigger) )
      {
	// Loop over the triggers.
	for (unsigned int ii = 0; ii < m_muon_triggers.size(); ii++ )
	  {
	    const char* bin_triggerX;
	    const char* bin_triggerY;
	    if (m_debuglevel) m_log << MSG::DEBUG << "Checking trigger " << m_muon_triggers[ii] << endreq;
	    if( m_trigDecTool->isPassed(m_muon_triggers[ii]))
	      {
		if (m_debuglevel) m_log << MSG::DEBUG << "Fired trigger "<< m_muon_triggers[ii]<<endreq;
		if ( m_muon_triggers.size() > 1 ){ 
		  bin_triggerX = m_muon_triggers[ii].c_str();
		  bin_triggerY = m_muon_triggers[ii].c_str();
		  muon_numEvents_passedTrigger->Fill(bin_triggerX,bin_triggerY,1);
		  if (m_debuglevel) m_log << MSG::DEBUG << "Filled diagonal of passedTrigger histogram " << endreq;
		  passesMuonTrigger = true;
		  // Checking if it passes other trigger before breaking.
		  for (unsigned int jj = (ii + 1); jj < m_muon_triggers.size(); jj++) 
		    {
		      if (m_debuglevel) m_log << MSG::DEBUG << "One Trigger fired. Also checking trigger " << m_muon_triggers[jj] << endreq;
		      if ( m_trigDecTool->isPassed(m_muon_triggers[jj]) )
			{
			  if (m_debuglevel) m_log << MSG::DEBUG << "Also fired trigger "<< m_muon_triggers[jj]<<endreq;
			  bin_triggerX = m_muon_triggers[jj].c_str();
			  muon_numEvents_passedTrigger->Fill(bin_triggerX,bin_triggerY,1);
			  if (m_debuglevel) m_log << MSG::DEBUG << "Filled off-diagonal into passedTrigger histogram " << endreq;
			}
		    }
		}
		break; 
	      }
	  }
	if (m_debuglevel) m_log << MSG::DEBUG << "MuonTriggers' size: " << m_muon_triggers.size() << endreq;
	if(!passesMuonTrigger){ if (m_debuglevel) m_log << MSG::DEBUG << "No Trigger Fired!" << endreq; }
      } 

    if(!m_useTrigger) { passesMuonTrigger = true; }

    /** Enters if it passed at least one trigger or if no trigger chain was used. */  
    if( passesMuonTrigger )
      {  
	if (m_debuglevel) m_log << MSG::DEBUG << "Inside **passesMuonTrigger**. " << endreq; 

	/** ================================= Tracks  ================================= */
	//Get tracks from SG
	//bool got_coll = false;
	//bool got_coll;

	/** Index for TrackCollections in histograms*/
	int h_index = -1;

	for (int j=0;j<(int)m_trackCollectionName.size();j++) {

	  if ( m_trackCollectionFlag[j] == 0 ) continue;
	  h_index++;

	  // Fill Histograms that use DetailedTrackSummary
	  StatusCode sc = FillDetailedTrkSummaryHistograms(m_trackCollectionName[j], h_index);
	  if ( sc.isFailure()){
	    ATH_MSG_WARNING( "Failed to FillDetailedTrkSummaryHistograms " ); 
	    return StatusCode::SUCCESS; 
	  }

	  // Fill Histograms that use TrackSummary
	  sc =  FillTrkSummaryHistograms(m_trackCollectionName[j], h_index);

	  if ( sc.isFailure()){
	    ATH_MSG_WARNING( "Failed to FillTrkSummaryHistograms " ); 
	    return StatusCode::SUCCESS; 
	  }

	} // loop over track collection

      } // Once  it passed Muontrigger or not using muon trigger

  } //environment
  ATH_MSG_DEBUG( "MuonSelectedTracksMon::Finalisation of " << name() << " was successful" );
  
  return StatusCode::SUCCESS;
}

//======================================================================================//
/**  procHistograms */
//======================================================================================//
StatusCode MuonSelectedTracksMon::procHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
 
  if (m_debuglevel) m_log << MSG::DEBUG << "MuonSelectedTracksMon finalize()" << endreq;
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {

    if(endOfEventsBlock){}
    if(endOfLumiBlock){} 
    if(endOfRun){ 

      int h_index = -1;
      for (int j=0;j<(int)m_trackCollectionName.size();j++) {
	if ( m_trackCollectionFlag[j] == 0 ) continue;
	h_index++;

	// Barrel
	for (int i=0;i<muon_trk_phi0_neg_barrel[h_index]->GetNbinsX();i++){
	  float nneg=muon_trk_phi0_neg_barrel[h_index]->GetBinContent(i);
	  float npos=muon_trk_phi0_pos_barrel[h_index]->GetBinContent(i);
	  float asym=0;
	  if (nneg+npos>0) asym=(nneg-npos)/(nneg+npos);
	  muon_trk_phi0_asym_barrel[h_index]->SetBinContent(i,asym);
	  nneg=muon_trk_phi0_neg_barrel[h_index]->GetBinContent(i);
	}
	// Endcap A
	for (int i=0;i<muon_trk_phi0_neg_eca[h_index]->GetNbinsX();i++){
	  float nneg=muon_trk_phi0_neg_eca[h_index]->GetBinContent(i);
	  float npos=muon_trk_phi0_pos_eca[h_index]->GetBinContent(i);
	  float asym=0;
	  if (nneg+npos>0) asym=(nneg-npos)/(nneg+npos);
	  muon_trk_phi0_asym_eca[h_index]->SetBinContent(i,asym);
	}
	// Endcap C
	for (int i=0;i<muon_trk_phi0_neg_ecc[h_index]->GetNbinsX();i++){
	  float nneg=muon_trk_phi0_neg_ecc[h_index]->GetBinContent(i);
	  float npos=muon_trk_phi0_pos_ecc[h_index]->GetBinContent(i);
	  float asym=0;
	  if (nneg+npos>0) asym=(nneg-npos)/(nneg+npos);
	  muon_trk_phi0_asym_ecc[h_index]->SetBinContent(i,asym);
	}
      } //  loop over Muon Track collections

      if ( m_useTrigger ) 
	{
	  //muon_numEvents_passedTrigger->LabelsDeflate("X");
	  //muon_numEvents_passedTrigger->LabelsDeflate("Y");
	  if (muon_numEvents_passedTrigger->GetEntries()>0) {
	    muon_numEvents_passedTrigger->LabelsOption("ha", "Y"); 
	    muon_numEvents_passedTrigger->LabelsOption("ha", "X");  }
	  muon_numEvents_passedTrigger->SetOption("text");
	}

      //Setting drawing options fro 2D histos
      for(unsigned int ii = 0; ii < muon_MDThits_per_Trk_vs_Eta.size(); ii++) {
        muon_MDThits_per_Trk_vs_Eta[ii]->GetXaxis()->SetTitle("[Eta]");
	muon_MDThits_per_Trk_vs_Eta[ii]->GetYaxis()->SetTitle("[MDT hits]");
	muon_MDThits_per_Trk_vs_Eta[ii]->SetOption("COLZ"); 
      }
      for(unsigned int ii = 0; ii < muon_MDThits_per_Trk_vs_Phi.size(); ii++) {
        muon_MDThits_per_Trk_vs_Phi[ii]->GetXaxis()->SetTitle("[Phi]");
	muon_MDThits_per_Trk_vs_Phi[ii]->GetYaxis()->SetTitle("[MDT hits]");
	muon_MDThits_per_Trk_vs_Phi[ii]->SetOption("COLZ"); 
      }

      for (unsigned int ii=0; ii < muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel.size(); ii++) 
	{
	  /** Setting up the Label options */

	  muon_nmdthits_per_track_vs_phi_per_chamber[ii]->SetOption("text");
	  // sort profile histos bins in accordance to labels
	  // Option = "a" sort by alphabetic order
	  // Option = "v" draw labels vertical      
	  // Barrel
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel[ii]->LabelsOption("va");

	  if (muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel[ii]->LabelsOption("va");

	  // End Cap A
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_2_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_2_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_4_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_4_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_6_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_6_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_8_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_8_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_10_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_10_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_12_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_12_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_14_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_14_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_16_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_16_eca[ii]->LabelsOption("va");

	  if (muon_nmdthits_per_track_vs_eta_PhiSector_1_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_1_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_3_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_3_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_5_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_5_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_7_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_7_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_9_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_9_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_11_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_11_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_13_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_13_eca[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_15_eca[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_15_eca[ii]->LabelsOption("va");  

	  // End Cap C
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc[ii]->LabelsOption("va");

	  if (muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc[ii]->LabelsOption("va");
	  if (muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc[ii]->GetEntries()>0) muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc[ii]->LabelsOption("va");   

	} 
      /** Normalization of histograms */
      /** Monitoring Summary Histograms*/

      for(unsigned int ii = 0; ii < muon_numHits_per_Trk.size(); ii++) {
	muon_numHits_per_Trk[ii]->GetXaxis()->SetTitle("Hits");
	muon_numHits_per_Trk[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_numHits_per_Event.size(); ii++) {
 	muon_numHits_per_Event[ii]->GetXaxis()->SetTitle("Hits");
 	muon_numHits_per_Event[ii]->GetYaxis()->SetTitle("Events"); }
      for(unsigned int ii = 0; ii < muon_numEtaHits_per_Trk.size(); ii++) {
	muon_numEtaHits_per_Trk[ii]->GetXaxis()->SetTitle("#eta Hits");
	muon_numEtaHits_per_Trk[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_numPhiHits_per_Trk.size(); ii++) {
	muon_numPhiHits_per_Trk[ii]->GetXaxis()->SetTitle("#phi Hits");
	muon_numPhiHits_per_Trk[ii]->GetYaxis()->SetTitle("Tracks"); }
      //for(unsigned int ii = 0; ii < muon_numTrigHits_per_Trk.size(); ii++) {
      //muon_numTrigHits_per_Trk[ii]->GetXaxis()->SetTitle("Trigger Hits");
      //muon_numTrigHits_per_Trk[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_numScatterers_per_Trk.size(); ii++) {
        muon_numScatterers_per_Trk[ii]->GetXaxis()->SetTitle("Scatterers");
        muon_numScatterers_per_Trk[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_numHoles_per_Trk.size(); ii++) {
        muon_numHoles_per_Trk[ii]->GetXaxis()->SetTitle("Holes");
        muon_numHoles_per_Trk[ii]->GetYaxis()->SetTitle("Tracks"); }
 
      /** Muon Monitoring Histograms */

      // TProfile

      for(unsigned int ii = 0; ii < muon_trk_chi2oDoF.size(); ii++) {
 	muon_trk_chi2oDoF[ii]->GetXaxis()->SetTitle("#eta [rad]");
	muon_trk_chi2oDoF[ii]->GetYaxis()->SetTitle("#chi^{2}/DoF");}
	
      for(unsigned int ii = 0; ii < muon_trk_chi2oDoF_pT.size(); ii++) {
	muon_trk_chi2oDoF_pT[ii]->GetXaxis()->SetTitle("p_{T} [GeV/c]");
	muon_trk_chi2oDoF_pT[ii]->GetYaxis()->SetTitle("#chi^{2}/DoF");}
      
      for(unsigned int ii = 0; ii < muon_trk_chi2oDoF_MDThits.size(); ii++) {
	muon_trk_chi2oDoF_MDThits[ii]->GetXaxis()->SetTitle("MDT Hits");
	muon_trk_chi2oDoF_MDThits[ii]->GetYaxis()->SetTitle("#chi^{2}/DoF");}      

      for(unsigned int ii = 0; ii < muon_trk_chi2Prob.size(); ii++) {
	muon_trk_chi2Prob[ii]->GetXaxis()->SetTitle("#eta [rad]");
	muon_trk_chi2Prob[ii]->GetYaxis()->SetTitle("#chi^{2} Probability"); }

      for(unsigned int ii = 0; ii < muon_trk_pT_MDThits.size(); ii++) {
	muon_trk_pT_MDThits[ii]->GetXaxis()->SetTitle("p_{T} [GeV/c]");
	muon_trk_pT_MDThits[ii]->GetYaxis()->SetTitle("MDT Hits");
        muon_trk_pT_MDThits[ii]->SetOption("COLZ");} 

      for(unsigned int ii = 0; ii < muon_HitOnTrk_X0.size(); ii++) {
	muon_HitOnTrk_X0[ii]->GetXaxis()->SetTitle("X_{0} [mm]");
	muon_HitOnTrk_X0[ii]->GetYaxis()->SetTitle("Hits"); } 

      for(unsigned int ii = 0; ii < muon_HitOnTrk_ToF.size(); ii++) {
	muon_HitOnTrk_ToF[ii]->GetXaxis()->SetTitle("Time of Flight");
	muon_HitOnTrk_ToF[ii]->GetYaxis()->SetTitle("Tracks"); } 


      // MuonSpectrometer barrel 

      for(unsigned int ii = 0; ii < muon_trk_d0_barrel.size(); ii++) {
	muon_trk_d0_barrel[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_trk_d0_barrel[ii]->GetYaxis()->SetTitle("Tracks"); } 
      for(unsigned int ii = 0; ii < muon_trk_d0c_barrel.size(); ii++) {
	muon_trk_d0c_barrel[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_trk_d0c_barrel[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_z0_barrel.size(); ii++) {
	muon_trk_z0_barrel[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_trk_z0_barrel[ii]->GetYaxis()->SetTitle("Tracks"); }
	
      // TProfiles
      for(unsigned int ii = 0; ii < muon_trk_d0_vs_phi_barrel.size(); ii++) {
	muon_trk_d0_vs_phi_barrel[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_d0_vs_phi_barrel[ii]->GetYaxis()->SetTitle("d_{0} [mm]");}
      for(unsigned int ii = 0; ii < muon_trk_d0_vs_z0_barrel.size(); ii++) {
	muon_trk_d0_vs_z0_barrel[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_trk_d0_vs_z0_barrel[ii]->GetYaxis()->SetTitle("d_{0} [mm]"); }

      for(unsigned int ii = 0; ii < muon_trk_phi0_neg_barrel.size(); ii++) {
	muon_trk_phi0_neg_barrel[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_neg_barrel[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_phi0_pos_barrel.size(); ii++) {
	muon_trk_phi0_pos_barrel[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_pos_barrel[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_phi0_asym_barrel.size(); ii++) {
	muon_trk_phi0_neg_barrel[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_neg_barrel[ii]->GetYaxis()->SetTitle("Charge Asymmetry"); }

      // MuonSpectrometer endcap A
      for(unsigned int ii = 0; ii < muon_trk_d0_eca.size(); ii++) {
	muon_trk_d0_eca[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_trk_d0_eca[ii]->GetYaxis()->SetTitle("Tracks"); } 
      for(unsigned int ii = 0; ii < muon_trk_d0c_eca.size(); ii++) {
	muon_trk_d0c_eca[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_trk_d0c_eca[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_z0_eca.size(); ii++) {
	muon_trk_z0_eca[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_trk_z0_eca[ii]->GetYaxis()->SetTitle("Tracks"); }
	
      // TProfiles 
      for(unsigned int ii = 0; ii < muon_trk_d0_vs_phi_eca.size(); ii++) {
	muon_trk_d0_vs_phi_eca[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_d0_vs_phi_eca[ii]->GetYaxis()->SetTitle("d_{0} [mm]");}
      for(unsigned int ii = 0; ii < muon_trk_d0_vs_z0_eca.size(); ii++) {
	muon_trk_d0_vs_z0_eca[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_trk_d0_vs_z0_eca[ii]->GetYaxis()->SetTitle("d_{0} [mm]"); }
	
      for(unsigned int ii = 0; ii < muon_trk_phi0_neg_eca.size(); ii++) {
	muon_trk_phi0_neg_eca[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_neg_eca[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_phi0_pos_eca.size(); ii++) {
	muon_trk_phi0_pos_eca[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_pos_eca[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_phi0_asym_eca.size(); ii++) {
	muon_trk_phi0_asym_eca[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_asym_eca[ii]->GetYaxis()->SetTitle("Charge Asymmetry"); }

      // MuonSpectrometer endcap C
      for(unsigned int ii = 0; ii < muon_trk_d0_ecc.size(); ii++) {
	muon_trk_d0_ecc[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_trk_d0_ecc[ii]->GetYaxis()->SetTitle("Tracks"); } 
      for(unsigned int ii = 0; ii < muon_trk_d0c_ecc.size(); ii++) {
	muon_trk_d0c_ecc[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_trk_d0c_ecc[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_z0_ecc.size(); ii++) {
	muon_trk_z0_ecc[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_trk_z0_ecc[ii]->GetYaxis()->SetTitle("Tracks"); }
	
      // TProfiles
      for(unsigned int ii = 0; ii < muon_trk_d0_vs_phi_ecc.size(); ii++) {
	muon_trk_d0_vs_phi_ecc[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_d0_vs_phi_ecc[ii]->GetYaxis()->SetTitle("d_{0} [mm]");}
      for(unsigned int ii = 0; ii < muon_trk_d0_vs_z0_ecc.size(); ii++) {
	muon_trk_d0_vs_z0_ecc[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_trk_d0_vs_z0_ecc[ii]->GetYaxis()->SetTitle("d_{0} [mm]"); }
	
      for(unsigned int ii = 0; ii < muon_trk_phi0_neg_ecc.size(); ii++) {
	muon_trk_phi0_neg_ecc[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_neg_ecc[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_phi0_pos_ecc.size(); ii++) {
	muon_trk_phi0_pos_ecc[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_pos_ecc[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_trk_phi0_asym_ecc.size(); ii++) {
	muon_trk_phi0_asym_ecc[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_trk_phi0_asym_ecc[ii]->GetYaxis()->SetTitle("Charge Asymmetry"); }
 

      /* Summary */
      for(unsigned int ii = 0; ii < muon_nhits_per_track.size(); ii++) {
	muon_nhits_per_track[ii]->GetXaxis()->SetTitle("Hits");
	muon_nhits_per_track[ii]->GetYaxis()->SetTitle("Tracks"); }

      for(unsigned int ii = 0; ii < muon_ntracks.size(); ii++) {
	muon_ntracks[ii]->GetXaxis()->SetTitle("Tracks");
	muon_ntracks[ii]->GetYaxis()->SetTitle("Events");  }
      for(unsigned int ii = 0; ii < muon_ngtracks.size(); ii++) {
	muon_ngtracks[ii]->GetXaxis()->SetTitle("Good Tracks (>11 MDThits, >1#phi hits, >2#eta hits, p_{T}>2GeV, #chi^{2}/DoF<15.0)"); 
	muon_ngtracks[ii]->GetXaxis()->SetTitle("Events ");  } 
      for(unsigned int ii = 0; ii < muon_gtrk_cutflow.size(); ii++) {
	if (muon_gtrk_cutflow[ii]->GetEntries() > 0) muon_gtrk_cutflow[ii]->LabelsOption("a");
	muon_gtrk_cutflow[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_acceptance.size(); ii++) {
	if (muon_gtrk_acceptance[ii]->GetEntries() > 0) muon_gtrk_acceptance[ii]->LabelsOption("a");
	muon_gtrk_acceptance[ii]->GetYaxis()->SetTitle("Tracks"); }  
      for(unsigned int ii = 0; ii < muon_gtrk_eta_phi.size(); ii++) {
	muon_gtrk_eta_phi[ii]->GetYaxis()->SetTitle("#eta [rad]");
	muon_gtrk_eta_phi[ii]->GetXaxis()->SetTitle("#phi [degrees]"); } 
      for(unsigned int ii = 0; ii < muon_gtrk_eta.size(); ii++) {
	muon_gtrk_eta[ii]->GetXaxis()->SetTitle("#eta [rad]"); 
	muon_gtrk_eta[ii]->GetYaxis()->SetTitle("Good Tracks"); }
      for(unsigned int ii = 0; ii < muon_gtrk_phi.size(); ii++) {
	muon_gtrk_phi[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_gtrk_phi[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_d0.size(); ii++) {
	muon_gtrk_d0[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_gtrk_d0[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_z0.size(); ii++) {
	muon_gtrk_z0[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_gtrk_z0[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_numEtaHits.size(); ii++) {
	muon_gtrk_numEtaHits[ii]->GetXaxis()->SetTitle("#eta Hits");
	muon_gtrk_numEtaHits[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_numPhiHits.size(); ii++) {
	muon_gtrk_numPhiHits[ii]->GetXaxis()->SetTitle("#phi Hits");
	muon_gtrk_numPhiHits[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_pT.size(); ii++) {
	muon_gtrk_pT[ii]->GetXaxis()->SetTitle("p_{T}[GeV/c]");
	muon_gtrk_pT[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_pT_pos.size(); ii++) {
	muon_gtrk_pT_pos[ii]->GetXaxis()->SetTitle("p_{T}[GeV/c]");
	muon_gtrk_pT_pos[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_pT_neg.size(); ii++) {
	muon_gtrk_pT_neg[ii]->GetXaxis()->SetTitle("p_{T}[GeV/c]");
	muon_gtrk_pT_neg[ii]->GetYaxis()->SetTitle("Good Tracks");  }
      for(unsigned int ii = 0; ii < muon_gtrk_ChargeRatio_pT.size(); ii++) {
	muon_gtrk_ChargeRatio_pT[ii]->GetXaxis()->SetTitle("p_{T}[GeV/c]");
	muon_gtrk_ChargeRatio_pT[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");  }
      for(unsigned int ii = 0; ii < muon_gtrk_chi2oDoF.size(); ii++) {
	muon_gtrk_chi2oDoF[ii]->GetYaxis()->SetTitle("#chi^{2}/DoF");
	muon_gtrk_chi2oDoF[ii]->GetXaxis()->SetTitle("#eta [rad]"); }
      for(unsigned int ii = 0; ii < muon_nmdthits_per_track.size(); ii++) {
	muon_nmdthits_per_track[ii]->GetXaxis()->SetTitle("MDT Hits");
	muon_nmdthits_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_nrpchits_per_track.size(); ii++) {
	muon_nrpchits_per_track[ii]->GetXaxis()->SetTitle("RPC Hits");
	muon_nrpchits_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_mdt_vs_rpc_hits_per_track.size(); ii++) {
        muon_mdt_vs_rpc_hits_per_track[ii]->SetOption("COLZ");
	muon_mdt_vs_rpc_hits_per_track[ii]->GetXaxis()->SetTitle("RPC Hits");
	muon_mdt_vs_rpc_hits_per_track[ii]->GetYaxis()->SetTitle("MDT Hits"); }
      for(unsigned int ii = 0; ii < muon_ntgchits_per_track.size(); ii++) {
	muon_ntgchits_per_track[ii]->GetXaxis()->SetTitle("TGC Hits");
	muon_ntgchits_per_track[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_mdt_vs_tgc_hits_per_track.size(); ii++) {
        muon_mdt_vs_tgc_hits_per_track[ii]->SetOption("COLZ");
	muon_mdt_vs_tgc_hits_per_track[ii]->GetXaxis()->SetTitle("TGC Hits");
	muon_mdt_vs_tgc_hits_per_track[ii]->GetYaxis()->SetTitle("MDT Hits"); }
      for(unsigned int ii = 0; ii < muon_ncschits_per_track.size(); ii++) {
	muon_ncschits_per_track[ii]->GetXaxis()->SetTitle("CSC Hits");
	muon_ncschits_per_track[ii]->GetYaxis()->SetTitle("Tracks"); }

      for(unsigned int ii = 0; ii < muon_nrpcetahits_per_track.size(); ii++) {
	muon_nrpcetahits_per_track[ii]->GetXaxis()->SetTitle("RPC #eta Hits");
	muon_nrpcetahits_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_nrpcphihits_per_track.size(); ii++) {
	muon_nrpcphihits_per_track[ii]->GetXaxis()->SetTitle("RPC #phi Hits");
	muon_nrpcphihits_per_track[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_ntgcetahits_per_track.size(); ii++) {
	muon_ntgcetahits_per_track[ii]->GetXaxis()->SetTitle("TGC #eta Hits");
	muon_ntgcetahits_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_ntgcphihits_per_track.size(); ii++) {
	muon_ntgcphihits_per_track[ii]->GetXaxis()->SetTitle("TGC #phi Hits");
	muon_ntgcphihits_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_ncscetahits_per_track.size(); ii++) {
	muon_ncscetahits_per_track[ii]->GetXaxis()->SetTitle("CSC #eta Hits");
	muon_ncscetahits_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_ncscphihits_per_track.size(); ii++) {
	muon_ncscphihits_per_track[ii]->GetXaxis()->SetTitle("CSC #phi Hits");
	muon_ncscphihits_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }

      for(unsigned int ii = 0; ii < muon_numTrks_flaggedSL.size(); ii++) {
	muon_numTrks_flaggedSL[ii]->GetXaxis()->SetTitle("Tracks flagged as Straigh-Line");
	muon_numTrks_flaggedSL[ii]->GetYaxis()->SetTitle("Events"); }

      for(unsigned int ii = 0; ii < muon_noutliers_per_track.size(); ii++) {
	muon_noutliers_per_track[ii]->GetXaxis()->SetTitle("Outliers");
	muon_noutliers_per_track[ii]->GetYaxis()->SetTitle("Tracks"); }

      for(unsigned int ii = 0; ii < muon_nmdtholes_per_track.size(); ii++) {
	muon_nmdtholes_per_track[ii]->GetXaxis()->SetTitle("MDT Holes");
	muon_nmdtholes_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_nrpcholes_per_track.size(); ii++) {
	muon_nrpcholes_per_track[ii]->GetXaxis()->SetTitle("RPC Holes");
	muon_nrpcholes_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_ntgcholes_per_track.size(); ii++) {
	muon_ntgcholes_per_track[ii]->GetXaxis()->SetTitle("TGC Holes");
	muon_ntgcholes_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_ncscholes_per_track.size(); ii++) {
	muon_ncscholes_per_track[ii]->GetXaxis()->SetTitle("CSC Holes");
	muon_ncscholes_per_track[ii]->GetYaxis()->SetTitle("Tracks");  }


      /** Clear monitoring vectors - for a new run */

      /** Monitoring Summary Histograms*/
      muon_numHits_per_Trk.clear();
      muon_numHits_per_Event.clear();
      muon_numEtaHits_per_Trk.clear();
      muon_numPhiHits_per_Trk.clear();
      //muon_numTrigHits_per_Trk.clear();
      muon_numScatterers_per_Trk.clear();
      muon_numTrks_flaggedSL.clear();
      //muon_numPseudos_per_Trk.clear();
      muon_numHoles_per_Trk.clear();      
      muon_MDThits_per_Trk_vs_Eta.clear();
      muon_MDThits_per_Trk_vs_Phi.clear();

      /** Muon Monitoring Histograms */

      muon_trk_chi2oDoF.clear();
      muon_trk_chi2oDoF_pT.clear();
      muon_trk_chi2oDoF_MDThits.clear();
      muon_trk_pT_MDThits.clear();
      muon_trk_chi2Prob.clear();
      
      /** Muon X0 and ToF Histograms */
      muon_HitOnTrk_X0.clear();
      muon_HitOnTrk_ToF.clear();

      // MuonSpectrometer barrel 
      muon_trk_d0_barrel.clear();
      muon_trk_d0c_barrel.clear();
      muon_trk_z0_barrel.clear();
      muon_trk_d0_vs_phi_barrel.clear();
      muon_trk_d0_vs_z0_barrel.clear();
      muon_trk_phi0_neg_barrel.clear();
      muon_trk_phi0_pos_barrel.clear();
      muon_trk_phi0_asym_barrel.clear();

      // MuonSpectrometer endcap A
      muon_trk_d0_eca.clear(); 
      muon_trk_d0c_eca.clear();
      muon_trk_z0_eca.clear();
      muon_trk_d0_vs_phi_eca.clear();
      muon_trk_d0_vs_z0_eca.clear();
      muon_trk_phi0_neg_eca.clear();
      muon_trk_phi0_pos_eca.clear();
      muon_trk_phi0_asym_eca.clear();

      // MuonSpectrometer endcap C
      muon_trk_d0_ecc.clear();
      muon_trk_d0c_ecc.clear();
      muon_trk_z0_ecc.clear();
      muon_trk_d0_vs_phi_ecc.clear();
      muon_trk_d0_vs_z0_ecc.clear();
      muon_trk_phi0_neg_ecc.clear();
      muon_trk_phi0_pos_ecc.clear();
      muon_trk_phi0_asym_ecc.clear();

      //muon_nhits_per_event.clear();
      muon_nhits_per_track.clear();
      muon_ntracks.clear();
      muon_ngtracks.clear();
      muon_gtrk_cutflow.clear();
      muon_gtrk_acceptance.clear();
      muon_gtrk_eta_phi.clear();
      muon_gtrk_eta.clear();
      muon_gtrk_phi.clear();
      muon_gtrk_d0.clear();
      muon_gtrk_z0.clear();
      muon_gtrk_numEtaHits.clear();
      muon_gtrk_numPhiHits.clear();
      muon_gtrk_pT.clear();
      muon_gtrk_pT_pos.clear();
      muon_gtrk_pT_neg.clear();
      muon_gtrk_ChargeRatio_pT.clear();
      muon_gtrk_chi2oDoF.clear();
      muon_nmdthits_per_track.clear();
      muon_nrpchits_per_track.clear();
      muon_mdt_vs_rpc_hits_per_track.clear();
      muon_ntgchits_per_track.clear();
      muon_mdt_vs_rpc_hits_per_track.clear();
      muon_ncschits_per_track.clear();

      muon_nrpcetahits_per_track.clear();
      muon_nrpcphihits_per_track.clear();
      muon_ntgcetahits_per_track.clear();
      muon_ntgcphihits_per_track.clear();
      muon_ncscetahits_per_track.clear();
      muon_ncscphihits_per_track.clear();

      muon_noutliers_per_track.clear();

      muon_nmdtholes_per_track.clear();  
      muon_nrpcholes_per_track.clear();  
      muon_ntgcholes_per_track.clear();  
      muon_ncscholes_per_track.clear();  

      /** MDT Hits per track histograms */

      // Barrel
      muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel.clear();

      muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel.clear();

      // End Cap A
      muon_nmdthits_per_track_vs_eta_PhiSector_2_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_4_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_6_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_8_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_10_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_12_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_14_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_16_eca.clear();

      muon_nmdthits_per_track_vs_eta_PhiSector_1_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_3_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_5_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_7_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_9_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_11_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_13_eca.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_15_eca.clear();  

      // End Cap C
      muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc.clear();

      muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc.clear();
      muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc.clear(); 

      muon_nmdthits_per_track_vs_phi_per_chamber.clear();      

    } //  if NewRun
      
  } //environment  

  return sc;   
}

//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode MuonSelectedTracksMon::finalize() 
{ 

  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;

  // init message stream
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached

  if (m_debuglevel) m_log << MSG::DEBUG << "MuonSelectedTracksMon::finalize()" << endreq;

  //StatusCode sc = StatusCode::SUCCESS;
  sc = StatusCode::SUCCESS;

  /** Clear monitoring vectors - for a new run */

  /** Monitoring Summary Histograms*/
  muon_numHits_per_Trk.clear();
  muon_numHits_per_Event.clear();
  muon_numEtaHits_per_Trk.clear();
  muon_numPhiHits_per_Trk.clear();
  //muon_numTrigHits_per_Trk.clear();
  muon_numScatterers_per_Trk.clear();
  muon_numTrks_flaggedSL.clear();
  //muon_numPseudos_per_Trk.clear();
  muon_numHoles_per_Trk.clear();      
  muon_MDThits_per_Trk_vs_Eta.clear();
  muon_MDThits_per_Trk_vs_Phi.clear();

  /** Muon Monitoring Histograms */

  muon_trk_chi2oDoF.clear();
  muon_trk_chi2oDoF_pT.clear();
  muon_trk_chi2oDoF_MDThits.clear();
  muon_trk_pT_MDThits.clear();
  muon_trk_chi2Prob.clear();
      
  /** Muon X0 and ToF Histograms */
  muon_HitOnTrk_X0.clear();
  muon_HitOnTrk_ToF.clear();

  // MuonSpectrometer barrel 
  muon_trk_d0_barrel.clear();
  muon_trk_d0c_barrel.clear();
  muon_trk_z0_barrel.clear();
  muon_trk_d0_vs_phi_barrel.clear();
  muon_trk_d0_vs_z0_barrel.clear();
  muon_trk_phi0_neg_barrel.clear();
  muon_trk_phi0_pos_barrel.clear();
  muon_trk_phi0_asym_barrel.clear();

  // MuonSpectrometer endcap A
  muon_trk_d0_eca.clear(); 
  muon_trk_d0c_eca.clear();
  muon_trk_z0_eca.clear();
  muon_trk_d0_vs_phi_eca.clear();
  muon_trk_d0_vs_z0_eca.clear();
  muon_trk_phi0_neg_eca.clear();
  muon_trk_phi0_pos_eca.clear();
  muon_trk_phi0_asym_eca.clear();

  // MuonSpectrometer endcap C
  muon_trk_d0_ecc.clear();
  muon_trk_d0c_ecc.clear();
  muon_trk_z0_ecc.clear();
  muon_trk_d0_vs_phi_ecc.clear();
  muon_trk_d0_vs_z0_ecc.clear();
  muon_trk_phi0_neg_ecc.clear();
  muon_trk_phi0_pos_ecc.clear();
  muon_trk_phi0_asym_ecc.clear();

  //muon_nhits_per_event.clear();
  muon_nhits_per_track.clear();
  muon_ntracks.clear();
  muon_ngtracks.clear();
  muon_gtrk_cutflow.clear();
  muon_gtrk_acceptance.clear();
  muon_gtrk_eta_phi.clear();
  muon_gtrk_eta.clear();
  muon_gtrk_phi.clear();
  muon_gtrk_d0.clear();
  muon_gtrk_z0.clear();
  muon_gtrk_numEtaHits.clear();
  muon_gtrk_numPhiHits.clear();
  muon_gtrk_pT.clear();
  muon_gtrk_pT_pos.clear();
  muon_gtrk_pT_neg.clear();
  muon_gtrk_ChargeRatio_pT.clear();
  muon_gtrk_chi2oDoF.clear();
  muon_nmdthits_per_track.clear();
  muon_nrpchits_per_track.clear();
  muon_mdt_vs_rpc_hits_per_track.clear();
  muon_ntgchits_per_track.clear();
  muon_mdt_vs_rpc_hits_per_track.clear();
  muon_ncschits_per_track.clear();

  muon_nrpcetahits_per_track.clear();
  muon_nrpcphihits_per_track.clear();
  muon_ntgcetahits_per_track.clear();
  muon_ntgcphihits_per_track.clear();
  muon_ncscetahits_per_track.clear();
  muon_ncscphihits_per_track.clear();

  muon_noutliers_per_track.clear();

  muon_nmdtholes_per_track.clear();  
  muon_nrpcholes_per_track.clear();  
  muon_ntgcholes_per_track.clear();  
  muon_ncscholes_per_track.clear();  

  /** MDT Hits per track histograms */

  // Barrel
  muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel.clear();

  muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel.clear();

  // End Cap A
  muon_nmdthits_per_track_vs_eta_PhiSector_2_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_4_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_6_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_8_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_10_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_12_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_14_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_16_eca.clear();

  muon_nmdthits_per_track_vs_eta_PhiSector_1_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_3_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_5_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_7_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_9_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_11_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_13_eca.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_15_eca.clear();  

  // End Cap C
  muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc.clear();

  muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc.clear();
  muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc.clear(); 

  muon_nmdthits_per_track_vs_phi_per_chamber.clear();      
  
  return sc;
}
 
//======================================================================================//
/**  FillDetailedTrkSummaryHistograms */
//======================================================================================// 

StatusCode MuonSelectedTracksMon::FillDetailedTrkSummaryHistograms(std::string CurrentTrackCollection, int h_ind)
{
   
  ATH_MSG_DEBUG( "Start FillDetailedTrkSummaryHistograms" );
  
  const TrackCollection* trks_summary(0);
 
  StatusCode sc = m_storeGate->retrieve(trks_summary, CurrentTrackCollection);    

  if ( sc.isFailure() || trks_summary == 0 ){
    ATH_MSG_WARNING( "Unable to retrieve track collection  " << CurrentTrackCollection << "  from StoreGate" );
    
    ATH_MSG_DEBUG("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    ATH_MSG_DEBUG("      Track Collection is Empty.  Trying next...   ");
    ATH_MSG_DEBUG("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");    
    
    muon_numTrks_flaggedSL[h_ind]->Fill(0);
    return StatusCode::SUCCESS; 
  }
 
  ATH_MSG_DEBUG("Tracks in StoreGate found");
  ATH_MSG_DEBUG("Track collection Name is " << CurrentTrackCollection); 
       
  int nHits_event = 0;
  int nHits_trk = 0;

  TrackCollection::const_iterator trksumItr = trks_summary->begin();
  TrackCollection::const_iterator trksumItrE = trks_summary->end();

  //counter for Straight-line tracks (SL)
  int SL_trks = 0;
	
  for (; trksumItr != trksumItrE; ++trksumItr) {
    if( !*trksumItr ) continue;

    /** Monitoring Summary Histograms */

    if(m_helperTool->isSLTrack(**trksumItr)){
      if (m_debuglevel) m_log << MSG::DEBUG << "*********  We have a SL Track (SelectedTrkMon:DetailedTrkSummary)*******" << endreq;
      SL_trks++; // Counting SL tracks
      //continue;	
    } 
           	    
    const Trk::Track* track = *trksumItr;
    if( !track->trackSummary() || !track->trackSummary()->muonTrackSummary() ) continue;
    const Trk::MuonTrackSummary& muonSummary = *track->trackSummary()->muonTrackSummary();
      
    if (m_debuglevel) 
      {
	m_log << MSG::DEBUG << "Hits per track, neta+nphi+netaTrig = " << muonSummary.netaHits() + muonSummary.nphiHits() /* + muonSummary.nEtaTrig()*/ << endreq;
	m_log << MSG::DEBUG << "Number pseudo = " << muonSummary.npseudoMeasurements() << endreq;
	m_log << MSG::DEBUG << "Number of scatterers = " <<muonSummary.nscatterers() << endreq;
	m_log << MSG::DEBUG << "Number of holes = " << muonSummary.nholes() << endreq;
      }
	 
    nHits_trk = muonSummary.netaHits() + muonSummary.nphiHits() /*+ muonSummary.nEtaTrig()*/ ;
    nHits_event += nHits_trk;
    muon_numHits_per_Trk[h_ind]->Fill(nHits_trk); 
    if (m_debuglevel) m_log << MSG::DEBUG << "Done filling muon_numHits_per_Trk." << endreq;	  
    muon_numEtaHits_per_Trk[h_ind]->Fill( muonSummary.netaHits() );
    if (m_debuglevel) m_log << MSG::DEBUG << "Done filling muon_numEtaHits_per_Trk." << endreq;
    muon_numPhiHits_per_Trk[h_ind]->Fill( muonSummary.nphiHits() );
    if (m_debuglevel) m_log << MSG::DEBUG << "Done filling muon_numPhiHits_per_Trk." << endreq;
    
    // muon_numTrigHits_per_Trk[h_ind]->Fill( muonSummary.nEtaTrig() );
    // if (m_debuglevel) m_log << MSG::DEBUG << "Done filling muon_numTrigHits_per_Trk." << endreq;
    
    muon_numScatterers_per_Trk[h_ind]->Fill( muonSummary.nscatterers() );
    if (m_debuglevel) m_log << MSG::DEBUG << "Done filling muon_numScatterers_per_Trk." << endreq;
    // muon_numPseudos_per_Trk[h_ind]->Fill(detailedSummary.npseudo);
       
    if ( muonSummary.nholes() > 0) muon_numHoles_per_Trk[h_ind]->Fill( muonSummary.nholes() ); 
    if (m_debuglevel)  m_log << MSG::DEBUG << "Done using DetailedTrackSummary for Sumary Histograms." << endreq; 

    /** MDT Hits per track, per chamber Histograms */
    // detailedSummary is my DetailedTrackSummary!!!

    // Detailed Track Summary is a structure, which contains:struct chamberHitSummary,  

    std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit = muonSummary.chamberHitSummary().begin();
    std::vector<Trk::MuonTrackSummary::ChamberHitSummary>::const_iterator chit_end = muonSummary.chamberHitSummary().end();
    /** Loop over chambers used by this Track */
    for( ;chit!=chit_end;++chit )
      {
        const Identifier& id_chamber = chit->chamberId();
	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: Identifier " << m_idHelperTool->toStringChamber(id_chamber) << endreq;
	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: nhitsETA " << chit->netaHits() << endreq;
	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: nhitsPHI " << chit->nphiHits() << endreq; 
	  	  
	int imdtstationName = -999;
	int imdtstationPhi  = -999;
	int imdtstationEta  = -999;

	std::string type="NoType";
	std::string stationName = "NoStation";
	const char* stationName_char = stationName.c_str();
	std::string Id_string = "NULL";

	if (m_idHelperTool->isMdt(id_chamber)) 
	  {
	    if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: it's MDT " << endreq;
	    imdtstationName = int(m_mdtIdHelper->stationName(id_chamber));
	    imdtstationPhi  = int(m_mdtIdHelper->stationPhi(id_chamber));
	    imdtstationEta  = int(m_mdtIdHelper->stationEta(id_chamber));
	    type="MDT";
	    Id_string = convertChamberName(imdtstationName,imdtstationEta,imdtstationPhi,type);
	    stationName = "Eta"+ imdtstationEta + m_mdtIdHelper->stationNameString( m_mdtIdHelper->stationName(id_chamber) );
	    if (m_debuglevel) m_log << MSG::DEBUG << "StationName " << stationName << endreq;
	    stationName_char = stationName.c_str();
	  } 
	  
	const char* Id_char = Id_string.c_str();

	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: IdentifierCHAR " << Id_char << endreq;
	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: Is MDT  " << m_idHelperTool->isMdt(id_chamber) << endreq;
	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: Is EndCap  " << m_idHelperTool->isEndcap(id_chamber) << endreq;
	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: Is SmallChamber  " << m_idHelperTool->isSmallChamber(id_chamber) << endreq;

	bool Etaneg=false;

	if ((m_mdtIdHelper->stationEta(id_chamber) ) < 0 ) { Etaneg = true; }
	if (m_debuglevel) m_log << MSG::DEBUG << "DetailedTrackSummary:: Is EtaNeg  " << Etaneg << endreq;


	/** Using kCanRebin on these histograms*/

	// Barrel
	muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel[h_ind]->SetBit(TH1::kCanRebin);

	muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel[h_ind]->SetBit(TH1::kCanRebin);

	// End Cap A
	muon_nmdthits_per_track_vs_eta_PhiSector_2_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_4_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_6_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_8_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_10_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_12_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_14_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_16_eca[h_ind]->SetBit(TH1::kCanRebin);

	muon_nmdthits_per_track_vs_eta_PhiSector_1_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_3_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_5_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_7_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_9_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_11_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_13_eca[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_15_eca[h_ind]->SetBit(TH1::kCanRebin);  

	// End Cap C
	muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc[h_ind]->SetBit(TH1::kCanRebin);

	muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc[h_ind]->SetBit(TH1::kCanRebin);
	muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc[h_ind]->SetBit(TH1::kCanRebin); 

	/** Number of MDT hits per chamber */
	muon_nmdthits_per_track_vs_phi_per_chamber[h_ind]->SetBit(TH1::kCanRebin);

	/** Filling these MDT hits per track histograms*/

	// Only MDT hits
	if (m_idHelperTool->isMdt(id_chamber) ) 
	  {
	    //EndCap
	    if( m_idHelperTool->isEndcap(id_chamber) )
	      {
		// Eta Neg -> EndCap C
		if ( (m_mdtIdHelper->stationEta(id_chamber)) < 0){

		  //Small Chambers (have Phi Station even number)
		  if(m_idHelperTool->isSmallChamber(id_chamber))
		    {
		      if ( imdtstationPhi == 1) muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 2) muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 3) muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 4) muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 5) muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 6) muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 7) muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 8) muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc[h_ind]->Fill(Id_char, chit->netaHits());

		      // Large Chambers (have Phi Station odd number)
		    } else {

		    if ( imdtstationPhi == 1) muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 2) muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 3) muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 4) muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 5) muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 6) muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 7) muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 8) muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc[h_ind]->Fill(Id_char, chit->netaHits());
		  }
		  // Eta Pos -> EndCap A
		} else {

		  //Small Chambers (have Phi Station even number)
		  if(m_idHelperTool->isSmallChamber(id_chamber))
		    {
		      if ( imdtstationPhi == 1) muon_nmdthits_per_track_vs_eta_PhiSector_2_eca[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 2) muon_nmdthits_per_track_vs_eta_PhiSector_4_eca[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 3) muon_nmdthits_per_track_vs_eta_PhiSector_6_eca[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 4) muon_nmdthits_per_track_vs_eta_PhiSector_8_eca[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 5) muon_nmdthits_per_track_vs_eta_PhiSector_10_eca[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 6) muon_nmdthits_per_track_vs_eta_PhiSector_12_eca[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 7) muon_nmdthits_per_track_vs_eta_PhiSector_14_eca[h_ind]->Fill(Id_char, chit->netaHits());
		      if ( imdtstationPhi == 8) muon_nmdthits_per_track_vs_eta_PhiSector_16_eca[h_ind]->Fill(Id_char, chit->netaHits());

		      // Large Chambers (have Phi Station odd number)
		    } else {

		    if ( imdtstationPhi == 1) muon_nmdthits_per_track_vs_eta_PhiSector_1_eca[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 2) muon_nmdthits_per_track_vs_eta_PhiSector_3_eca[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 3) muon_nmdthits_per_track_vs_eta_PhiSector_5_eca[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 4) muon_nmdthits_per_track_vs_eta_PhiSector_7_eca[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 5) muon_nmdthits_per_track_vs_eta_PhiSector_9_eca[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 6) muon_nmdthits_per_track_vs_eta_PhiSector_11_eca[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 7) muon_nmdthits_per_track_vs_eta_PhiSector_13_eca[h_ind]->Fill(Id_char, chit->netaHits());
		    if ( imdtstationPhi == 8) muon_nmdthits_per_track_vs_eta_PhiSector_15_eca[h_ind]->Fill(Id_char, chit->netaHits());
		  }      
		}

		//Barrel
	      }  else {

	      //Small Chamber (have Phi Station even number)
	      if(m_idHelperTool->isSmallChamber(id_chamber))
		{
		  if ( imdtstationPhi == 1) muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		  if ( imdtstationPhi == 2) muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		  if ( imdtstationPhi == 3) muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		  if ( imdtstationPhi == 4) muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		  if ( imdtstationPhi == 5) muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		  if ( imdtstationPhi == 6) muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		  if ( imdtstationPhi == 7) muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		  if ( imdtstationPhi == 8) muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel[h_ind]->Fill(Id_char, chit->netaHits());

		  // Large Chamber  (have Phi Station odd number)
		} else {

		if ( imdtstationPhi == 1) muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		if ( imdtstationPhi == 2) muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		if ( imdtstationPhi == 3) muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		if ( imdtstationPhi == 4) muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		if ( imdtstationPhi == 5) muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		if ( imdtstationPhi == 6) muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		if ( imdtstationPhi == 7) muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel[h_ind]->Fill(Id_char, chit->netaHits());
		if ( imdtstationPhi == 8) muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel[h_ind]->Fill(Id_char, chit->netaHits());
	      }
	    }

	    muon_nmdthits_per_track_vs_phi_per_chamber[h_ind]->Fill(imdtstationPhi, stationName_char, chit->netaHits());

	  } //is MDT?->Done filling the histos

      }//loop over chambers        

  } // Loop over tracksumItr
    
  muon_numTrks_flaggedSL[h_ind]->Fill(SL_trks);

  muon_numHits_per_Event[h_ind]->Fill(nHits_event);    
  
  ATH_MSG_DEBUG( "Done with FillDetailedTrackSummaryHistograms" );
  return  StatusCode::SUCCESS;
}
   
//======================================================================================//
/**  FillTrkSummaryHistograms */
//======================================================================================//  
StatusCode MuonSelectedTracksMon::FillTrkSummaryHistograms(std::string CurrentTrackCollection, int h_ind)
{
     
  ATH_MSG_DEBUG( "Start FillTrkSummaryHistograms" ); 
   
  muon_gtrk_cutflow[h_ind]   ->SetBit(TH1::kCanRebin);
  muon_gtrk_acceptance[h_ind]->SetBit(TH1::kCanRebin);
  
  StatusCode sc = m_storeGate->retrieve(trks, CurrentTrackCollection);    
  if ( sc.isFailure() ){
    ATH_MSG_WARNING( "Unable to retrieve track collection  " << CurrentTrackCollection << " from StoreGate" );
    muon_ntracks[h_ind]->Fill(0);
    muon_ngtracks[h_ind]->Fill(0); 
    return StatusCode::SUCCESS;
  }
 
  ATH_MSG_DEBUG("Tracks in StoreGate found" );
  ATH_MSG_DEBUG( "Track collection Name is " << CurrentTrackCollection );
 
  if ( trks->empty() ){
    ATH_MSG_DEBUG("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    ATH_MSG_DEBUG("      Track Collection is Empty.  Trying next...    ");
    ATH_MSG_DEBUG("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    muon_ntracks[h_ind]->Fill(0);
    muon_ngtracks[h_ind]->Fill(0); 
  } else {

    //if(!got_coll) continue; // protection 

    float xv=-999;
    float yv=-999;
    float zv=-999;

    if (xv==-999 || yv==-999 || zv==-999) {
      if (m_debuglevel) m_log << MSG::DEBUG << "No vertex found => setting it to 0"<<endreq;
      xv=0;yv=0;zv=0;
    }

    //int nHits=0;
    //     int nHits_event = 0;
    //     int nHits_trk = 0;

    int nTracks=0;
    int ngTracks=0;

    DataVector<Trk::Track>::const_iterator trksItr  = trks->begin();
    DataVector<Trk::Track>::const_iterator trksItrE = trks->end();
    for (; trksItr != trksItrE; ++trksItr) {
      if( !*trksItr ) continue; 
      bool isStraightLine = false;
      
      nTracks++;  


      /** Selected Regions Histograms */

      float chi2Prob=0.;
      float chisquared     = 0.;
      int   DoF            = 0;
      int   Num_MDThits    = 0;
      int   Num_MDTCSChits = 0;
      //int   Num_RPCphiHits = 0;
      int   Num_EtaHits    = 0;
      int   Num_PhiHits    = 0;
      
      float chi2oDoF       = -999;
      float trkd0          = -999;
      float trkz0          = -999;
      float trkphi         = -999;
      //float trktheta       = -999;
      float trketa         = -999;
      float qOverP         = -999;
      float trkpt          = -999;
      float charge         = 0;
      float trkd0c         = -999;

      // get fit quality and chi2 probability of track
      // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
      const Trk::FitQuality* fitQual = (*trksItr)->fitQuality();
      const Trk::Perigee* startPerigee = (*trksItr)->perigeeParameters();
      const Trk::Perigee* measPer = dynamic_cast<const Trk::Perigee*>( startPerigee );
  
      if (measPer==0) {
	m_log << MSG::WARNING << "No measured perigee parameters assigned to the track" << endreq; 
      }
      else{ 
       
        Amg::VectorX perigeeParams = measPer->parameters();
	
	//Dealing with SL tracks
        if(m_helperTool->isSLTrack(**trksItr)){
          if (m_debuglevel) m_log << MSG::DEBUG << "***  We have a SL Track (SelectedTrkMon:TrkSummary)***" << endreq;
	  isStraightLine = true;
	  //continue;
	  if (perigeeParams[Trk::qOverP] > 0)  qOverP = 1.0;
	  else qOverP = -1.0;
	  trkpt    =-10.0;	
        } else {
	  isStraightLine = false;
	  qOverP   = perigeeParams[Trk::qOverP]*1000.;  
	  trkpt    = measPer->pT()/1000.;
        }
           
	trkd0    = perigeeParams[Trk::d0];  
	trkz0    = perigeeParams[Trk::z0];    
	trkphi   = perigeeParams[Trk::phi0];  
	//trktheta = perigeeParams[Trk::theta];
	trketa   = measPer->eta(); 
	//qOverP   = perigeeParams[Trk::qOverP]*1000.;  
	//trkpt    = measPer->pT()/1000.;  
        if (qOverP<0) charge=-1;
	else charge=+1; 

	// correct the track d0 for the vertex position
	// would rather corrected for the beamline but could not find beamline
	trkd0c=trkd0-(yv*cos(trkphi)-xv*sin(trkphi));
      }

      if (fitQual==0) {
	m_log << MSG::WARNING << "No fit quality assigned to the track" << endreq; 
	chi2Prob = -1e12; // return big value
      }
      else {
	if (fitQual->chiSquared() > 0. && fitQual->numberDoF() > 0) {
	  Genfun::CumulativeChiSquare probabilityFunction( fitQual->numberDoF() );
	  chi2Prob = 1 - probabilityFunction( fitQual->chiSquared() );
	}
      }            

      chisquared = (fitQual) ? fitQual->chiSquared() : -1.;
      DoF = (fitQual) ? fitQual->numberDoF() : -1;
      if(DoF>0) chi2oDoF = chisquared/(float)DoF;

      /** Access to TrackSummary Information */
      const Trk::TrackSummary* summary = NULL;       
      summary = m_trackSumTool->createSummary(**trksItr);
      
      if(summary==0) m_log << MSG::WARNING << "Could not create TrackSummary" << endreq;    
      else{
	int nhits=0;
	int nhmdt=0, nhrpc=0, nhrpceta=0, nhrpcphi=0, nhtgc=0, nhtgceta=0, nhtgcphi=0, nhcsc=0, nhcsceta=0, nhcscphi=0, nhoutliers=0;
	
	nhmdt = summary->get(Trk::numberOfMdtHits);	 
	nhrpc = (summary->get(Trk::numberOfRpcPhiHits))+(summary->get(Trk::numberOfRpcEtaHits));
	nhrpceta = summary->get(Trk::numberOfRpcEtaHits);
	nhrpcphi = summary->get(Trk::numberOfRpcPhiHits);
	nhtgc = summary->get(Trk::numberOfTgcPhiHits)+summary->get(Trk::numberOfTgcEtaHits);
	nhtgceta = summary->get(Trk::numberOfTgcEtaHits);
	nhtgcphi = summary->get(Trk::numberOfTgcPhiHits);
	nhcsc = summary->get(Trk::numberOfCscPhiHits)+summary->get(Trk::numberOfCscEtaHits);
	nhcsceta = summary->get(Trk::numberOfCscEtaHits);
	nhcscphi = summary->get(Trk::numberOfCscPhiHits);
	nhoutliers = summary->get(Trk::numberOfOutliersOnTrack);
	
	if(nhmdt==-1)      nhmdt=0;      
	if(nhoutliers==-1) nhoutliers=0;      
	if(nhrpc==-1)      nhrpc=0;     
	if(nhrpceta==-1)   nhrpceta=0;     
	if(nhrpcphi==-1)   nhrpcphi=0;     
	if(nhtgc==-1)      nhtgc=0;
	if(nhtgceta==-1)   nhtgceta=0;
	if(nhtgcphi==-1)   nhtgcphi=0;
	if(nhcsc==-1)      nhcsc=0;
	if(nhcsceta==-1)   nhcsceta=0;
	if(nhcscphi==-1)   nhcscphi=0; 

	if (m_debuglevel) {m_log << MSG::DEBUG<<" * Number of MDT hits : "<<summary->get(Trk::numberOfMdtHits)<<endreq;
	  m_log << MSG::DEBUG<<" * Number of TGC hits : phi "<< nhtgcphi <<" eta " << nhtgceta <<endreq;
	  m_log << MSG::DEBUG<<" * Number of RPC hits : phi "<< nhrpcphi <<" eta " << nhrpceta <<endreq;
	  m_log << MSG::DEBUG<<" * Number of CSC hits : phi "<< nhcsceta <<" eta " << nhcscphi <<endreq;
	  m_log << MSG::DEBUG<<" * Number of Outliers :     "<< nhoutliers<<endreq;
	}
	
	nhits =  nhmdt + nhrpc + nhtgc + nhcsc; 
	Num_MDTCSChits = nhmdt + nhcsc;
	Num_MDThits    = nhmdt;
	//Num_RPCphiHits = nhrpcphi;
	 	
 	          
	Trk::MuonTrackSummary muonSummary;
	if( summary ){
	  if( summary->muonTrackSummary() ) muonSummary = *summary->muonTrackSummary();
	  else{
	    Trk::TrackSummary* tmpSum = const_cast<Trk::TrackSummary*>(summary);
	    if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(**trksItr,*tmpSum);
	    if( tmpSum->muonTrackSummary() ) muonSummary = *tmpSum->muonTrackSummary();
	  }
	}else{
	  Trk::TrackSummary tmpSummary;
	  m_trackSummaryTool->addDetailedTrackSummary(**trksItr,tmpSummary);
	  if( tmpSummary.muonTrackSummary() ) muonSummary = *tmpSummary.muonTrackSummary();
	}    	
	Num_EtaHits = muonSummary.netaHits();  
	Num_PhiHits = muonSummary.nphiHits();
	
	//nHits += nhits;

	muon_nmdthits_per_track[h_ind]       ->Fill(nhmdt);
	muon_nrpchits_per_track[h_ind]       ->Fill(nhrpc);
	muon_mdt_vs_rpc_hits_per_track[h_ind]->Fill(nhrpc, nhmdt);
	muon_ntgchits_per_track[h_ind]       -> Fill(nhtgc);
	muon_mdt_vs_tgc_hits_per_track[h_ind]->Fill(nhtgc, nhmdt);
	muon_ncschits_per_track[h_ind]       -> Fill(nhcsc);
	muon_nhits_per_track[h_ind]          ->Fill(nhits);

	muon_nrpcetahits_per_track[h_ind]->Fill(nhrpceta);
	muon_nrpcphihits_per_track[h_ind]->Fill(nhrpcphi);
	muon_ntgcetahits_per_track[h_ind]->Fill(nhtgceta);
	muon_ntgcphihits_per_track[h_ind]->Fill(nhtgcphi);
	muon_ncscetahits_per_track[h_ind]->Fill(nhcsceta);
	muon_ncscphihits_per_track[h_ind]->Fill(nhcscphi);

	muon_noutliers_per_track[h_ind]->Fill(nhoutliers);
      }
      if (summary) delete summary;
      
      Muon::IMuonHitSummaryTool::CompactSummary mHitSummary;
      int trkStations = 0; 
      if (m_muonHitSummaryTool) {
	mHitSummary = m_muonHitSummaryTool->summary(**trksItr);   
	//trkStations = mHitSummary.stationLayers.size(); 
	std::map<Muon::MuonStationIndex::StIndex,Muon::IMuonHitSummaryTool::HitSummary>::const_iterator hsit = mHitSummary.stationLayers.begin();
        std::map<Muon::MuonStationIndex::StIndex,Muon::IMuonHitSummaryTool::HitSummary>::const_iterator hsit_end = mHitSummary.stationLayers.end(); 
	for( ;hsit != hsit_end; ++hsit ){
	  const Muon::IMuonHitSummaryTool::HitSummary& hitSummary = hsit->second;
	  if( hitSummary.nprecisionHits > 2 ) ++trkStations;
	}
	if (m_debuglevel) m_log << MSG::DEBUG << " Hit summary, number of stations in track:"          << mHitSummary.stationLayers.size() 
				<< ", number of layers with at least 3 precision hits: " << trkStations << endreq;	 
      } else m_log << MSG::WARNING << "Unable to create the hitsummary for the track..." << endreq;	
	     

      //some good quality requirements (will need to refine them later)
      if(v_applied_cut){

	good          = true;
	good_chi2oDoF = false;
	good_pT       = false;
	good_eta      = false;
	good_phi      = false;
	good_nStation = false;
	
	// cut flow 
	muon_gtrk_cutflow[h_ind] ->Fill("0:No Cuts", 1.0);
	 
	if ( Num_MDTCSChits >= m_gtrk_minMdtCscHits )
	  { 
	    muon_gtrk_cutflow[h_ind] ->Fill("1:MDT hits", 1.0);
	    if ( ( trkpt >= m_gtrk_minPt ) || (isStraightLine) )
	      { 
		muon_gtrk_cutflow[h_ind] ->Fill("2:pT cut", 1.0);
		if ( Num_EtaHits >= m_gtrk_minEtaHits ) 
		  { 
		    muon_gtrk_cutflow[h_ind] ->Fill("3:Eta hits", 1.0);
		    if ( Num_PhiHits >= m_gtrk_minPhiHits )
		      { 
			muon_gtrk_cutflow[h_ind] ->Fill("4:Phi hits", 1.0); 
			if ( chi2oDoF <= m_gtrk_maxChi2oDoF )
			  {
			    muon_gtrk_cutflow[h_ind] ->Fill("5:chi2oDoF cut", 1.0); 
			    if ( trkStations >= m_gtrk_minStations)
			      {
				muon_gtrk_cutflow[h_ind] ->Fill("6:nStations cut", 1.0); 
			      }
			   
			  }
			
		      }
		  }
	      }
	  }
	  
	muon_gtrk_acceptance[h_ind] ->Fill("0:No Cuts", 1.0);
	 
	// MDT cut
	if ( Num_MDTCSChits < m_gtrk_minMdtCscHits ) good = false; 
	else muon_gtrk_acceptance[h_ind] ->Fill("1:MDT hits", 1.0); 

	// pT cut
	if ( ( trkpt < m_gtrk_minPt ) && (!isStraightLine) ) good = false; 
	else  muon_gtrk_acceptance[h_ind] ->Fill("2:pT cut", 1.0);

	// eta hits cut
	if ( Num_EtaHits < m_gtrk_minEtaHits ) good = false; 
	else  muon_gtrk_acceptance[h_ind] ->Fill("3:Eta hits", 1.0); 

	// phi hits cut
	if ( Num_PhiHits < m_gtrk_minPhiHits ) good = false;  
	else  muon_gtrk_acceptance[h_ind] ->Fill("4:Phi hits", 1.0); 

	// chi2oDoF cut
	if ( chi2oDoF > m_gtrk_maxChi2oDoF ) good = false; 
	else  muon_gtrk_acceptance[h_ind] ->Fill("5:chi2oDoF cut", 1.0); 
	
	// n stations
	if ( trkStations < m_gtrk_minStations) good = false; 
	else  muon_gtrk_acceptance[h_ind] ->Fill("6:nStations cut", 1.0); 
	
	 
	good_pT       = ((Num_MDTCSChits>=m_gtrk_minMdtCscHits)&&(Num_EtaHits>=m_gtrk_minEtaHits)&&(Num_PhiHits>=m_gtrk_minPhiHits)&&(chi2oDoF<=m_gtrk_maxChi2oDoF)&&(trkStations>=m_gtrk_minStations));
	good_nStation = ((Num_MDTCSChits>=m_gtrk_minMdtCscHits)&&(Num_EtaHits>=m_gtrk_minEtaHits)&&(Num_PhiHits>=m_gtrk_minPhiHits)&&(chi2oDoF<=m_gtrk_maxChi2oDoF)&&((trkpt>m_gtrk_minPt)||(isStraightLine)));
	good_chi2oDoF = ((Num_MDTCSChits>=m_gtrk_minMdtCscHits)&&(Num_EtaHits>=m_gtrk_minEtaHits)&&(Num_PhiHits>=m_gtrk_minPhiHits)&&((trkpt>m_gtrk_minPt)||(isStraightLine))&&(trkStations>=m_gtrk_minStations));
	good_eta      = (((trkpt<m_gtrk_minPt)||(isStraightLine))&&(Num_PhiHits >= m_gtrk_minPhiHits)&&(chi2oDoF<=m_gtrk_maxChi2oDoF)&&(trkStations>=m_gtrk_minStations));  
	good_phi      = ((Num_MDTCSChits>=m_gtrk_minMdtCscHits)&&(Num_EtaHits >= m_gtrk_minEtaHits)&&((trkpt>m_gtrk_minPt)||(isStraightLine))&&(chi2oDoF<=m_gtrk_maxChi2oDoF)&&(trkStations>=m_gtrk_minStations));

	
      }	else {	
	good = ((fabs(trketa)<v_maxeta && trkpt>0.0) || (isStraightLine && fabs(trketa)<v_maxeta));
	good_chi2oDoF = true;
	good_pT       = true;
	good_eta      = true;
	good_phi      = true;
	good_nStation = true;
      }
 
      if (good_chi2oDoF)
	muon_gtrk_chi2oDoF[h_ind]->Fill(trketa, chi2oDoF);
      if (good_pT) {
	muon_gtrk_pT     [h_ind]->Fill(trkpt);
	if (charge>0)
	  muon_gtrk_pT_pos [h_ind]->Fill(trkpt);
	else if (charge<0)
	  muon_gtrk_pT_neg [h_ind]->Fill(trkpt);
      }
      if (good_eta)
	muon_gtrk_numEtaHits[h_ind]->Fill(Num_EtaHits);
      if (good_phi)
	muon_gtrk_numPhiHits[h_ind]->Fill(Num_PhiHits);
	  
      if (good) {
	ngTracks++;

	if (m_debuglevel) m_log << MSG::DEBUG << nTracks << " is a good track!" << endreq; 
	muon_gtrk_eta     [h_ind]->Fill(trketa);
	muon_gtrk_phi     [h_ind]->Fill(RadsToDegrees(trkphi));
	muon_gtrk_eta_phi [h_ind]->Fill(RadsToDegrees(trkphi), trketa);
	muon_gtrk_d0      [h_ind]->Fill(trkd0);
	muon_gtrk_z0      [h_ind]->Fill(trkz0);
	 
      }

      int numMdtHoles=0, numRpcHoles=0, numTgcHoles=0, numCscHoles=0;

      // loop over track and get chamber Identifiers
      const DataVector<const Trk::TrackStateOnSurface>* states = (*trksItr)->trackStateOnSurfaces();
      if( !states ) continue;

      // loop over TSOSs
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
      for( ; tsit!=tsit_end ; ++tsit ){
	const Trk::TrackParameters* pars = (*tsit)->trackParameters();       
	if( !pars ){
          if (m_debuglevel) m_log << MSG::DEBUG<<" Failed to createTrk::TrackParameters from SoS iterator. "<<endreq;
	  continue;
	}

	//X0 calculation only on Measurements!!!
		
 	const Trk::MaterialEffectsBase *materialEffects = (*tsit)->materialEffectsOnTrack(); 
	const Trk::MeasurementBase*     meas = (*tsit)->measurementOnTrack();

 	if (meas) {	  
	  if ( materialEffects && ( (*tsit)->type(Trk::TrackStateOnSurface::Measurement) ) ) {

	    const Trk::MaterialEffectsOnTrack* matEffectsOnTrk = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);

	    float hitTrk_X0 = matEffectsOnTrk ? matEffectsOnTrk->thicknessInX0() : -999.0;
	    if (m_debuglevel){m_log << MSG::DEBUG<<"FillTrkSummaryHistograms: x0:" << hitTrk_X0 <<endreq;} 
	    //float hitTrk_X0 = materialEffects ? materialEffects->thicknessInX0() : -999.0;

	    if (hitTrk_X0 != -999.0){
	      muon_HitOnTrk_X0[h_ind]->Fill(hitTrk_X0);
	    } else if (m_debuglevel){m_log << MSG::DEBUG<<"Failed to retrieve thicknessInX0 from materialEffectsOnTrk."<<endreq;}	  

	  } else if (m_debuglevel) {
	    if  (!materialEffects){
	      m_log << MSG::DEBUG<<"Failed to retrieve Trk::MaterialEffectsBase from stateOnSurface."<<endreq;
	    }
	    if ( !(*tsit)->type(Trk::TrackStateOnSurface::Measurement) ){
	      m_log << MSG::DEBUG<<"This SoS is not a measurement."<<endreq;
	    }
	  }

	  if (m_DoToFCosmic) {
	    if ( ( (*tsit)->type(Trk::TrackStateOnSurface::Measurement) ) ) {
	      Identifier id_ToF = m_helperTool->getIdentifier(*meas);
	      if (!id_ToF.is_valid()) m_log << MSG::DEBUG<<"DoToF: Not a valid id!!:" << id_ToF <<endreq;
	      if (m_debuglevel){m_log << MSG::DEBUG<<"DoToF: ID_tof:" << id_ToF <<endreq;}
	      const Amg::Vector3D globalPos = pars->position();
	      if (m_debuglevel){m_log << MSG::DEBUG<<"DoToF: global pos:" << globalPos <<endreq;}
	      //Amg::Vector3D pos = meas->globalPosition();
	      //if (m_debuglevel){m_log << MSG::DEBUG<<"DoToF: pos:" << pos <<endreq;}
	      MdtCalibrationSvcInput input;
	      /*float HitOnTrk_ToF*/ input.tof = m_tofTool->timeOfFlight(id_ToF, globalPos);
	      //float HitOnTrk_ToF = m_tofTool->timeOfFlight(id_ToF, pos);
	      if (m_debuglevel){m_log << MSG::DEBUG<<"FillTrkSummaryHistograms: ToF:" << input.tof /*HitOnTrk_ToF*/ <<endreq;}
	      muon_HitOnTrk_ToF[h_ind]->Fill(input.tof /*HitOnTrk_ToF*/);
	    }
	  }
	
	} else if (m_debuglevel){m_log << MSG::DEBUG<<"Failed to retrieve MeasurementBase from SoS."<<endreq;}

	if( (*tsit)->type(Trk::TrackStateOnSurface::Hole) ) {
	  Identifier id = pars->associatedSurface().associatedDetectorElementIdentifier();
	  if( !id.is_valid() || !id.get_compact() ) continue;

	  if ( m_idHelperTool->isMdt(id) ) {
	    numMdtHoles++;
	  } else if (m_idHelperTool->isRpc(id) ) {
	    numRpcHoles++;
	  } else if (m_idHelperTool->isTgc(id) ) {
	    numTgcHoles++;
	  } else if (m_idHelperTool->isCsc(id) ) {
	    numCscHoles++;
	  }
	  continue;
	}
      }

      if (numMdtHoles>0 ) muon_nmdtholes_per_track[h_ind] -> Fill(numMdtHoles);
      if (numRpcHoles>0 ) muon_nrpcholes_per_track[h_ind] -> Fill(numRpcHoles);
      if (numTgcHoles>0 ) muon_ntgcholes_per_track[h_ind] -> Fill(numTgcHoles);
      if (numCscHoles>0 ) muon_ncscholes_per_track[h_ind] -> Fill(numCscHoles);

      muon_trk_chi2oDoF[h_ind]        ->Fill(trketa, chi2oDoF);
      muon_trk_chi2oDoF_pT[h_ind]     ->Fill(trkpt, chi2oDoF);
      muon_trk_chi2oDoF_MDThits[h_ind]->Fill((float)Num_MDThits, chi2oDoF);
      muon_trk_pT_MDThits[h_ind]      ->Fill(trkpt, Num_MDThits);
      muon_trk_chi2Prob[h_ind]        ->Fill(trketa, chi2Prob);
      
      muon_MDThits_per_Trk_vs_Eta[h_ind]->Fill(trketa,Num_MDThits);
      muon_MDThits_per_Trk_vs_Phi[h_ind]->Fill(RadsToDegrees(trkphi), Num_MDThits);

      if (fabs(trketa)<1) {
	muon_trk_d0_barrel[h_ind]->Fill(trkd0);
	muon_trk_d0c_barrel[h_ind]->Fill(trkd0c);
	muon_trk_z0_barrel[h_ind]->Fill(trkz0);
	muon_trk_d0_vs_phi_barrel[h_ind]->Fill(RadsToDegrees(trkphi), trkd0);
	muon_trk_d0_vs_z0_barrel[h_ind]->Fill(trkz0, trkd0);
	if (charge<0) muon_trk_phi0_neg_barrel[h_ind]->Fill(RadsToDegrees(trkphi));
	else muon_trk_phi0_pos_barrel[h_ind]->Fill(RadsToDegrees(trkphi));
      } else if (trketa>1) {
	muon_trk_d0_eca[h_ind]->Fill(trkd0);
	muon_trk_d0c_eca[h_ind]->Fill(trkd0c);
	muon_trk_z0_eca[h_ind]->Fill(trkz0);
	muon_trk_d0_vs_phi_eca[h_ind]->Fill(RadsToDegrees(trkphi),trkd0);
	muon_trk_d0_vs_z0_eca[h_ind]->Fill(trkz0,trkd0);
	if (charge<0) muon_trk_phi0_neg_eca[h_ind]->Fill(RadsToDegrees(trkphi));
	else muon_trk_phi0_pos_eca[h_ind]->Fill(RadsToDegrees(trkphi));
      } else if (trketa<-1) {
	muon_trk_d0_ecc[h_ind]->Fill(trkd0);
	muon_trk_d0c_ecc[h_ind]->Fill(trkd0c);
	muon_trk_z0_ecc[h_ind]->Fill(trkz0);
	muon_trk_d0_vs_phi_ecc[h_ind]->Fill(RadsToDegrees(trkphi),trkd0);
	muon_trk_d0_vs_z0_ecc[h_ind]->Fill(trkz0,trkd0);
	if (charge<0) muon_trk_phi0_neg_ecc[h_ind]->Fill(RadsToDegrees(trkphi));
	else muon_trk_phi0_pos_ecc[h_ind]->Fill(RadsToDegrees(trkphi));
      } 

    } // Loop over Tracks


    ATH_MSG_DEBUG( "Number of good tracks from TrackCollection: "<< ngTracks );
     
    // muon_nhits_per_event[h_ind]->Fill(nHits);
    // muon_numHits_per_Event[h_ind]->Fill(nHits_event);
    muon_ntracks[h_ind]->Fill(nTracks);
    muon_ngtracks[h_ind]->Fill(ngTracks); 

  } // !Trks->empty()

  ATH_MSG_DEBUG("Done with FillTrkSummaryHistograms");
 
  return StatusCode::SUCCESS;
}

//======================================================================================//
/**  setupTools */
//======================================================================================// 

StatusCode MuonSelectedTracksMon::setupTools()
{ 
  //initializing tools
  // Retrieve the StoreGate service
  StatusCode sc = StatusCode::SUCCESS;

  //   if ( sc.isFailure() ){
  //     m_log << MSG::FATAL << "Unable to retrieve the StoreGate service... Exiting!" << endreq;
  //     return sc;
  //   }
  //   if (m_debuglevel) m_log << MSG::DEBUG << "Defined StatusCode service" << endreq;
 

  //sc = service( "StoreGateSvc", m_storeGate );
  
  //   /** Cosmics */  
  //   if (AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
  //     m_log<< MSG::DEBUG << "Running in cosmic mode "<<std::endl;
  //   } else {
  //     if (m_debuglevel) m_log<< MSG::DEBUG << "Running in collision mode "<< endreq;
  //   }
  
  /** Get StoreGate */
  sc = service("StoreGateSvc", m_storeGate);
  
  //if ( service("StoreGateSvc",m_storeGate).isFailure() ) {
  if ( sc.isFailure() ) {
    m_log << MSG::FATAL << "Unable to get pointer to StoreGateSvc" << endreq;
    return sc;
  } else {
    if (m_debuglevel) m_log << MSG::DEBUG << "Defined StoreGateSvc service" << endreq; 
  }
  
  /** Get TrackSummaryTool */
  
  sc = m_trackSumTool.retrieve();
  //if ( m_trackSumTool.retrieve().isFailure() ) {
  if ( sc.isFailure() )  {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_trackSumTool << endreq;
    return sc;
  } else {
    if (m_debuglevel) m_log << MSG::DEBUG << "Retrieved tool " << m_trackSumTool << endreq;
  }
   
  if ( sc = m_trackSummaryTool.retrieve().isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve" << m_trackSummaryTool << endreq;
    return sc;
  } else {
    if (m_debuglevel) m_log << MSG::DEBUG << "Retrieved tool " << m_trackSummaryTool << endreq;
  }
     
  /** Get idHelperTool */
  
  sc = m_idHelperTool.retrieve();
  //if ( m_idHelperTool.retrieve().isFailure() ) {
  if( sc.isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_idHelperTool << endreq;
    return sc;
  } else {
    if (m_debuglevel) m_log << MSG::DEBUG << "Retrieved tool " << m_idHelperTool << endreq;
  }
        
  /** Get EDMhelperTool */
  if ( m_helperTool.retrieve().isFailure() ) {
    m_log << MSG::FATAL << "Failed to retrieve tool " << m_helperTool << endreq;
    return sc;
  } else { 
    if (m_debuglevel) m_log << MSG::DEBUG << "Retrieved tool " << m_helperTool << endreq;
  }

  /** Get hit summary tool */
  sc = m_muonHitSummaryTool.retrieve();
  if (sc.isFailure()){
    m_log << MSG::ERROR << "Can't retrieve " << m_muonHitSummaryTool << endreq;
    return sc;
  }	   
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the muonHitSummaryTool " << endreq;
  }
  
  /** Get timeOfFlight tool */
  if (m_DoToFCosmic) {
    if( m_tofTool.empty() ) {
      m_log << MSG::ERROR << "The time of flight tool is not configured. Please check your configuration"  << endreq;
      return StatusCode::FAILURE;
    }
    if( m_tofTool.retrieve().isSuccess() ){
      if (m_debuglevel) m_log << MSG::DEBUG << "Retrieved " << m_tofTool << endreq;
    }else{
      m_log << MSG::ERROR <<"Could not get " << m_tofTool <<endreq; 
      return StatusCode::FAILURE;
    }
  }

  StoreGateSvc* detStore=0;
  sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "DetectorStore service not found." << endreq;
    return sc;
  }else{
    if (m_debuglevel) m_log << MSG::DEBUG << "Retrieved tool " << detStore << endreq;
  
    sc = detStore->retrieve( m_detMgr );
     
    if ( sc.isFailure() ) {
      m_log << MSG::ERROR << " Cannot retrieve MuonDetDescrMgr from detector store" << endreq;
      return sc;
    }else{
      if (m_debuglevel) m_log << MSG::DEBUG << "Retrieved MuonDetDescrMgr  " << m_detMgr << endreq;
      //m_mdtIdHelper = m_detMgr->mdtIdHelper();
      sc = detStore->retrieve(m_mdtIdHelper, "MDTIDHELPER");
      if (sc.isFailure()){
	m_log << MSG::ERROR << "Cannot get MdtIdHelper" << endreq;
	return sc;
      } else {
	m_log << MSG::DEBUG << "mdtIdHelper successfully retrieved. " << endreq;
      }
    }
  }


  //  if ( sc.isSuccess() ) {
  //     sc = detStore->retrieve( m_detMgr );
  //     if ( sc.isFailure() ) {
  //       m_log << MSG::ERROR << " Cannot retrieve MuonDetDescrMgr " << endreq;
  //       return sc;
  //     } else {
  //       m_log << MSG::INFO << " MuonDetDescrMgr found in DetectorStore " << endreq;
  //       m_mdtIdHelper = m_detMgr->mdtIdHelper();
  //       m_log << MSG::INFO << "mdtIdHelper successfully retrieved. " << endreq;
  //     }
  //   } else {
  // 
  //   }

  if (m_debuglevel) m_log << MSG::DEBUG << "Before retrieving trigDecTool, UseTrigger? " << m_useTrigger << endreq;
  if( !m_trigDecTool.empty() && m_useTrigger )
    //if (!m_trigDecTool.epty())
    {
      sc = m_trigDecTool.retrieve();
      if( !sc.isSuccess() ) {
	m_log << MSG::ERROR << "!! Unable to retrieve the TrigDecisionTool !!" << endreq;
	return sc;
      }
      if (m_debuglevel) m_log << MSG::DEBUG << "  --> Found AlgTool \"TrigDecisionTool\"" << endreq;    
      if (m_debuglevel) m_log << MSG::DEBUG << "Size of Muon Trigger vector = "<< m_muon_triggers.size() << endreq;
    } 

  if (m_debuglevel) m_log << MSG::DEBUG << "Done setting up tools." << endreq;
  return sc;
}  

//======================================================================================//
/**  bookSummaryHistograms */
//======================================================================================// 
StatusCode MuonSelectedTracksMon::bookSummaryHistograms()

{
  StatusCode sc = StatusCode::SUCCESS;
  if (m_debuglevel) m_log << MSG::DEBUG << "Inside bookSummaryHistograms()" << endreq;
  
    
  std::string m_generic_path_muonmonselected = "Muon/MuonTrackMonitoring";
  std::string muon_summary_path = "unknown";

  for (int j=0;j<(int)m_trackCollectionName.size();j++) {
    if ( m_trackCollectionFlag[j] == 0 ) continue;
    
 
    if ( m_MuonTriggerChainName == "NoMuonTriggerSelection")
      muon_summary_path = m_generic_path_muonmonselected+"/NoTrigger/MuonMonTrkSummary/"+m_trackCollectionName[j];
    else
      muon_summary_path = m_generic_path_muonmonselected+"/TriggerAware/MuonMonTrkSummary/"+m_trackCollectionName[j]+ m_MuonTriggerChainName;


    //declare a group of histograms
    MonGroup muon_summary_shift ( this, muon_summary_path, run, ATTRIB_MANAGED ); 
    MonGroup muon_summary       ( this, muon_summary_path, run, ATTRIB_MANAGED );
    MonGroup muon_summary_Norm  ( this, muon_summary_path, run, ATTRIB_MANAGED, "", "weightedEff" ); 
     
    /** Monitoring Summary Histograms*/
	        
    /** Number of Hits per track. It does NOT include Outliers, and it does NOT combine RPC hits. */
    muon_numHits_per_Trk.push_back(new TH1F("muon_numHits_per_Trk","Number of Hits per Muon Track",61,-0.5,60.5));
    sc=muon_summary.regHist(muon_numHits_per_Trk.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_numHits_per_Trk Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of hits per event. It does NOT include Outliers, and it does NOT combine RPC hits.  */
    muon_numHits_per_Event.push_back(new TH1F("muon_numHits_per_Event", "Number of Hits on Track per Event",101, -0.5, 100.5));	
    sc=muon_summary.regHist(muon_numHits_per_Event.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_numHits_per_Event Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of hits in ETA. It does NOT include Outliers, and it does NOT combine RPC hits. */
    muon_numEtaHits_per_Trk.push_back(new TH1F("muon_numEtaHits_per_Trk","Number of Eta Hits per Muon Track",51,-0.5,50.5));
    sc=muon_summary_shift.regHist(muon_numEtaHits_per_Trk.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_numEtaHits_per_Trk Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of hits in PHI. It does NOT include Outliers, and it does NOT combine RPC hits. */
    muon_numPhiHits_per_Trk.push_back(new TH1F("muon_numPhiHits_per_Trk","Number of Phi Hits per Muon Track",51,-0.5,50.5));
    sc=muon_summary_shift.regHist(muon_numPhiHits_per_Trk.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_numPhiHits_per_Trk Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of TRIGGER hits (eta.) It does NOT include Outliers, and it does NOT combine RPC hits. */
    // muon_numTrigHits_per_Trk.push_back(new TH1F("muon_numTrigHits_per_Trk","Number of Trigger Hits per Muon Track",51,-0.5,50.5));
    // sc=muon_summary_shift.regHist(muon_numTrigHits_per_Trk.back()) ;
    //  if(sc.isFailure())
    //    { m_log << MSG::FATAL << "muon_numTrigHits_per_Trk Failed to register histogram " << endreq;
    //	return sc;
    //  }
    /** Number of scatterers per track. (DetailedTrackSummary) */
    muon_numScatterers_per_Trk.push_back(new TH1F("muon_numScatterers_per_Trk","Number of Scatterers per Muon Track",51,-0.5,50.5));
    sc=muon_summary.regHist(muon_numScatterers_per_Trk.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_numScatterers_per_Trk Failed to register histogram " << endreq;
	return sc;
      } 
    /** Number of holes per track. (DetailedTrackSummary) */  
    muon_numHoles_per_Trk.push_back(new TH1F("muon_numHoles_per_Trk","Number of Holes per Muon Track",51,-0.5,50.5));
    sc=muon_summary.regHist(muon_numHoles_per_Trk.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_numHoles_per_Trk Failed to register histogram " << endreq;
	return sc;
      } 
    /** Number of muon Tracks flagged as Straight line trk per event. */  
    muon_numTrks_flaggedSL.push_back(new TH1F("muon_numTrks_flaggedSL","Number of Muon Tracks Flagged as Straight Line Trks",26,-0.5,25.5));
    sc=muon_summary.regHist(muon_numTrks_flaggedSL.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_numTrks_flaggedSL Failed to register histogram " << endreq;
	return sc;
      }      
    /** Number of tracks per event. */
    muon_ntracks.push_back(new TH1F("muon_ntracks","Number of Tracks per Event",11,-0.5,10.5));
    sc=muon_summary_shift.regHist(muon_ntracks.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ntracks Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of good tracks (after applying cut on MDThits, P_T, eta and phi Hits, chi2oDoF) */
    muon_ngtracks.push_back(new TH1F("muon_ngtracks","Number of Good Muon Tracks after Cut",11,-0.5,10.5));
    sc=muon_summary_shift.regHist(muon_ngtracks.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ngtracks Failed to register histogram " << endreq;
	return sc;
      }
    /**  Acceptance of good tracks per cut */
    muon_gtrk_acceptance.push_back(new TH1F("muon_gtrk_acceptance","Muon Tracks passing each cut.", 7, 0.0, 7.0));
    sc=muon_summary.regHist(muon_gtrk_acceptance.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_acceptance Failed to register histogram " << endreq;
	return sc;
      } 
    /** Cut Flow of good tracks */
    muon_gtrk_cutflow.push_back(new TH1F("muon_gtrk_cutflow","Cut Flow of Good Tracks selection", 7, 0.0, 7.0));
    sc=muon_summary.regHist(muon_gtrk_cutflow.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_cutflow Failed to register histogram " << endreq;
	return sc;
      } 
    /** Eta vs Phi distribution for good tracks */
    muon_gtrk_eta_phi.push_back(new TH2F("muon_gtrk_eta_phi","Eta vs. Phi Distribution of Good Tracks",m_nphi_bins,-180.,180., 100,-4.0,4.0));
    sc=muon_summary_shift.regHist(muon_gtrk_eta_phi.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_eta_phi Failed to register histogram " << endreq;
	return sc;
      }
    /** Eta distribution for good tracks */
    muon_gtrk_eta.push_back(new TH1F("muon_gtrk_eta","Eta Distribution of Good Tracks",100,-4.0,4.0));
    sc=muon_summary.regHist(muon_gtrk_eta.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_eta Failed to register histogram " << endreq;
	return sc;
      }
    /** Phi distribution of good tracks */
    muon_gtrk_phi.push_back(new TH1F("muon_gtrk_phi","Phi Distribution of Good Tracks",m_nphi_bins,-180.,180.));
    sc=muon_summary.regHist(muon_gtrk_phi.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_phi Failed to register histogram " << endreq;
	return sc;
      }
    /** d0 distribution of good tracks */
    muon_gtrk_d0.push_back(new TH1F("muon_gtrk_d0","Transverse Impact Parameter of Good Tracks",100, -500.0, 500.0));
    sc=muon_summary_shift.regHist(muon_gtrk_d0.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_d0 Failed to register histogram " << endreq;
	return sc;
      }
    /** z0 distribution of good tracks */
    muon_gtrk_z0.push_back(new TH1F("muon_gtrk_z0","Longitudinal Impact Parameter of Good Tracks",100, -500.0, 500.0));
    sc=muon_summary_shift.regHist(muon_gtrk_z0.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_z0 Failed to register histogram " << endreq;
	return sc;
      }
    /** Num Eta distribution of good tracks */
    muon_gtrk_numEtaHits.push_back(new TH1F("muon_gtrk_numEtaHits","Eta Hits Distribution of Good Tracks",51, -0.5, 50.5));
    sc=muon_summary_shift.regHist(muon_gtrk_numEtaHits.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_numEtaHits Failed to register histogram " << endreq;
	return sc;
      }
    /** Num Phi distribution of good tracks */
    muon_gtrk_numPhiHits.push_back(new TH1F("muon_gtrk_numPhiHits","Phi Hits Distribution of Good Tracks",51, -0.5, 50.5));
    sc=muon_summary_shift.regHist(muon_gtrk_numPhiHits.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_numPhiHits Failed to register histogram " << endreq;
	return sc;
      }
    /** pT distribution of good tracks */
    muon_gtrk_pT.push_back(new TH1F("muon_gtrk_pT","Transverse Momentum of Good tracks",100,0.0,100.0));
    sc=muon_summary_shift.regHist(muon_gtrk_pT.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_pT Failed to register histogram " << endreq;
	return sc;
      }
    muon_gtrk_pT_pos.push_back(new TH1F("muon_gtrk_pT_pos","Transverse Momentum of Good tracks (positive)",100,0.0,100.0));
    sc=muon_summary.regHist(muon_gtrk_pT_pos.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_pT_pos Failed to register histogram " << endreq;
	return sc;
      }
    muon_gtrk_pT_neg.push_back(new TH1F("muon_gtrk_pT_neg","Transverse Momentum of Good tracks (negative)",100,0.0,100.0));
    sc=muon_summary.regHist(muon_gtrk_pT_neg.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_pT_neg Failed to register histogram " << endreq;
	return sc;
      }
    muon_gtrk_ChargeRatio_pT.push_back(new TH1F("muon_gtrk_ChargeRatio_pT","Charge Ratio vs. Transverse Momentum of Good tracks",100,0.0,100.0));
    sc=muon_summary_Norm.regHist(muon_gtrk_ChargeRatio_pT.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_ChargeRatio_pT Failed to register histogram " << endreq;
	return sc;
      }
    /** chi2 per Degree of freedom for good tracks */
    muon_gtrk_chi2oDoF.push_back(new TProfile("muon_gtrk_chi2oDoF","#chi^{2} per degree of freedom of good tracks",120,-4.0,4.0,-25.0,25.0));
    sc=muon_summary_shift.regHist(muon_gtrk_chi2oDoF.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_gtrk_chi2oDoF Failed to register histogram " << endreq;
	return sc;
      }
      
    /** Number of hits per track. It includes outliers and (in Moore reco) combines RPC hits. */  
    muon_nhits_per_track.push_back(new TH1F("muon_nhits_per_track","Number of (mdt+rpc+csc+tgc) hits per muontrack",60,-0.5,59.5));  
    sc=muon_summary.regHist(muon_nhits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nhits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of OUTLIERS per track. */
    muon_noutliers_per_track.push_back(new TH1F("muon_noutliers_per_track","Number of measurements flagged as outliers on track",30,-0.5,29.5));  
    sc=muon_summary.regHist(muon_noutliers_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_noutliers_per_track Failed to register histogram " << endreq;
	return sc;
      } 
    /** Number of hits vs. Eta*/
    muon_MDThits_per_Trk_vs_Eta.push_back(new TH2F("muon_MDThits_per_Trk_vs_Eta","Number of MDT hits per track vs. Eta",40,-4.,4., 50, -0.5, 49.5));  
    sc=muon_summary.regHist(muon_MDThits_per_Trk_vs_Eta.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_MDThits_per_Trk_vs_Eta Failed to register histogram " << endreq;
	return sc;
      }	 
    /** Number of hits vs. Phi*/
    muon_MDThits_per_Trk_vs_Phi.push_back(new TH2F("muon_MDThits_per_Trk_vs_Phi","Number of MDT hits per track vs. Phi",m_nphi_bins,-180.,180.,50, -0.5, 49.5));  
    sc=muon_summary.regHist(muon_MDThits_per_Trk_vs_Phi.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_MDThits_per_Trk_vs_Phi Failed to register histogram " << endreq;
	return sc;
      }	   
     
  }
   
  return sc;   
}

//======================================================================================//
/**  bookTrkParametersHistograms */
//======================================================================================// 
StatusCode MuonSelectedTracksMon::bookTrkParametersHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
  if (m_debuglevel) m_log << MSG::DEBUG << "Inside bookTrkParametersHistograms()" << endreq;
    
  std::string m_generic_path_muonmonselected = "Muon/MuonTrackMonitoring";
  std::string muon_trkparameters_path = "unknown";
 

  for (int j=0;j<(int)m_trackCollectionName.size();j++) {
    if ( m_trackCollectionFlag[j] == 0 ) continue;


    if ( m_MuonTriggerChainName == "NoMuonTriggerSelection")
      muon_trkparameters_path = m_generic_path_muonmonselected+"/NoTrigger/MuonTrkParameters/"+m_trackCollectionName[j];
    else
      muon_trkparameters_path = m_generic_path_muonmonselected+"/TriggerAware/MuonTrkParameters/"+m_trackCollectionName[j]+ m_MuonTriggerChainName;
    
    //declare a group of histograms 
    MonGroup muon_trkparameters ( this, muon_trkparameters_path,  run, ATTRIB_MANAGED ); 
    MonGroup muon_trkparameters_shift ( this, muon_trkparameters_path,  run, ATTRIB_MANAGED ); 
    MonGroup muon_trkparameters_barrel ( this, muon_trkparameters_path+"/Barrel", run, ATTRIB_MANAGED );
    MonGroup muon_trkparameters_endcap ( this, muon_trkparameters_path+"/EndCap", run, ATTRIB_MANAGED ); 
    MonGroup muon_trkparameters_barrel_asym ( this, muon_trkparameters_path+"/Barrel", run, ATTRIB_MANAGED , "", "weightedEff" );
    MonGroup muon_trkparameters_endcap_asym ( this, muon_trkparameters_path+"/EndCap", run, ATTRIB_MANAGED, "", "weightedEff" ); 
     
    /** Increase d0 and z0 range for cosmics */
    /** Barrel, EndCap A , EndCap C Histograms*/
    if (AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
      muon_trk_d0_barrel.push_back(new TH1F("muon_trk_d0_barrel","Impact parameter: all cosmics muon tracks (Barrel)",1000,-500,500)); 
      muon_trk_d0c_barrel.push_back(new TH1F("muon_trk_d0c_barrel","Impact parameter (corrected for vertex): all tracks (Barrel)",1000,-500,500));  
      muon_trk_z0_barrel.push_back(new TH1F("muon_trk_z0_barrel","Track z0: all tracks (Barrel)",1000,-5000,5000));  
      muon_trk_d0_eca.push_back(new TH1F("muon_trk_d0_eca","Impact parameter: all tracks (Endcap A)",1000,-500,500));  
      muon_trk_d0c_eca.push_back(new TH1F("muon_trk_d0c_eca","Impact parameter (corrected for vertex): all tracks  (Endcap A)",1000,-500,500));  
      muon_trk_z0_eca.push_back(new TH1F("muon_trk_z0_eca","Track z0: all tracks (Endcap A)",1000,-5000,5000));  
      muon_trk_d0_ecc.push_back(new TH1F("muon_trk_d0_ecc","Impact parameter: all tracks (Endcap C)",1000,-5,5));  
      muon_trk_d0c_ecc.push_back(new TH1F("muon_trk_d0c_ecc","Impact parameter (corrected for vertex): all tracks  (Endcap C)",1000,-5,5));  
      muon_trk_z0_ecc.push_back(new TH1F("muon_trk_z0_ecc","Track z0: all tracks (Endcap C)",1000,-5000,5000));  
    } else {
      muon_trk_d0_barrel.push_back(new TH1F("muon_trk_d0_barrel","Impact parameter: all muon tracks (Barrel)",50,-10,10));  
      muon_trk_d0c_barrel.push_back(new TH1F("muon_trk_d0c_barrel","Impact parameter (corrected for vertex): all tracks (Barrel)",50,-10,10));  
      muon_trk_z0_barrel.push_back(new TH1F("muon_trk_z0_barrel","Track z0: all tracks (Barrel)",1000,-5000,5000));  
      muon_trk_d0_eca.push_back(new TH1F("muon_trk_d0_eca","Impact parameter: all tracks (Endcap A)",50,-10,10));  
      muon_trk_d0c_eca.push_back(new TH1F("muon_trk_d0c_eca","Impact parameter (corrected for vertex): all tracks  (Endcap A)",50,-10,10));  
      muon_trk_z0_eca.push_back(new TH1F("muon_trk_z0_eca","Track z0: all tracks (Endcap A)",1000,-5000,5000));  
      muon_trk_d0_ecc.push_back(new TH1F("muon_trk_d0_ecc","Impact parameter: all tracks (Endcap C)",1000,-5,5));  
      muon_trk_d0c_ecc.push_back(new TH1F("muon_trk_d0c_ecc","Impact parameter (corrected for vertex): all tracks  (Endcap C)",1000,-5,5));  
      muon_trk_z0_ecc.push_back(new TH1F("muon_trk_z0_ecc","Track z0: all tracks (Endcap C)",1000,-5000,5000));  
    }
    
    sc=muon_trkparameters_barrel.regHist(muon_trk_d0_barrel.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_barrel Failed to register histogram " << endreq;
	return sc;
      }

    sc=muon_trkparameters_barrel.regHist(muon_trk_d0c_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0c_barrel Failed to register histogram " << endreq;
	return sc;
      }
    sc=muon_trkparameters_barrel.regHist(muon_trk_z0_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_z0_barrel Failed to register histogram " << endreq;
	return sc;
      }

    sc=muon_trkparameters_endcap.regHist(muon_trk_d0_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_eca Failed to register histogram " << endreq;
	return sc;
      }
    sc=muon_trkparameters_endcap.regHist(muon_trk_d0c_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0c_eca Failed to register histogram " << endreq;
	return sc;
      }
    sc=muon_trkparameters_endcap.regHist(muon_trk_z0_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_z0_eca Failed to register histogram " << endreq;
	return sc;
      }

    sc=muon_trkparameters_endcap.regHist(muon_trk_d0_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_ecc Failed to register histogram " << endreq;
	return sc;
      }
    sc=muon_trkparameters_endcap.regHist(muon_trk_d0c_ecc.back()) ; 
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0c_ecc Failed to register histogram " << endreq;
	return sc;
      }
    sc=muon_trkparameters_endcap.regHist(muon_trk_z0_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_z0_ecc Failed to register histogram " << endreq;
	return sc;
      }

    /** Barrel, EndCap A , EndCap C Histograms*/
    muon_trk_d0_vs_phi_barrel.push_back(new TProfile("muon_trk_d0_vs_phi_barrel","Impact parameter versus phi0 (Barrel)",m_nphi_bins,-180.,180.,-5,5));  
    sc=muon_trkparameters_barrel.regHist(muon_trk_d0_vs_phi_barrel.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_vs_phi_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_d0_vs_z0_barrel.push_back(new TProfile("muon_trk_d0_vs_z0_barrel","Impact parameter versus z0 (Barrel)",100,-200,200,-5,5));  
    sc=muon_trkparameters_barrel.regHist(muon_trk_d0_vs_z0_barrel.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_vs_z0_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_neg_barrel.push_back(new TH1F("muon_trk_phi0_neg_barrel","Phi distribution for negative tracks (Barrel)",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_barrel.regHist(muon_trk_phi0_neg_barrel.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_neg_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_pos_barrel.push_back(new TH1F("muon_trk_phi0_pos_barrel","Phi distribution for positive tracks (Barrel)",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_barrel.regHist(muon_trk_phi0_pos_barrel.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_pos_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_asym_barrel.push_back(new TH1F("muon_trk_phi0_asym_barrel","Track Charge Asymmetry versus phi (Barrel) ",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_barrel_asym.regHist(muon_trk_phi0_asym_barrel.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_asym_barrel Failed to register histogram " << endreq;
	return sc;
      }
	

    muon_trk_d0_vs_phi_eca.push_back(new TProfile("muon_trk_d0_vs_phi_eca","Impact parameter versus phi0 (Endcap A)",m_nphi_bins,-180.,180.,-5,5));  
    sc=muon_trkparameters_endcap.regHist(muon_trk_d0_vs_phi_eca.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_vs_phi_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_d0_vs_z0_eca.push_back(new TProfile("muon_trk_d0_vs_z0_eca","Impact parameter versus z0 (Endcap A)",100,-200,200,-5,5));  
    sc=muon_trkparameters_endcap.regHist(muon_trk_d0_vs_z0_eca.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_vs_z0_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_neg_eca.push_back(new TH1F("muon_trk_phi0_neg_eca","Phi distribution for negative tracks (Endcap A)",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_endcap.regHist(muon_trk_phi0_neg_eca.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_neg_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_pos_eca.push_back(new TH1F("muon_trk_phi0_pos_eca","Phi distribution for positive tracks (Endcap A)",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_endcap.regHist(muon_trk_phi0_pos_eca.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_pos_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_asym_eca.push_back(new TH1F("muon_trk_phi0_asym_eca","Track Charge Asymmetry versus phi (Endcap A) ",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_endcap_asym.regHist(muon_trk_phi0_asym_eca.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_asym_eca Failed to register histogram " << endreq;
	return sc;
      }

    muon_trk_d0_vs_phi_ecc.push_back(new TProfile("muon_trk_d0_vs_phi_ecc","Impact parameter versus phi0 (Endcap C)",m_nphi_bins,-180.,180.,-5,5));  
    sc=muon_trkparameters_endcap.regHist(muon_trk_d0_vs_phi_ecc.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_vs_phi_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_d0_vs_z0_ecc.push_back(new TProfile("muon_trk_d0_vs_z0_ecc","Impact parameter versus z0 (Endcap C)",100,-200,200,-5,5));  
    sc=muon_trkparameters_endcap.regHist(muon_trk_d0_vs_z0_ecc.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_d0_vs_z0_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_neg_ecc.push_back(new TH1F("muon_trk_phi0_neg_ecc","Phi distribution for negative tracks (Endcap C)",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_endcap.regHist(muon_trk_phi0_neg_ecc.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_neg_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_pos_ecc.push_back(new TH1F("muon_trk_phi0_pos_ecc","Phi distribution for positive tracks (Endcap C)",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_endcap.regHist(muon_trk_phi0_pos_ecc.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_pos_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_trk_phi0_asym_ecc.push_back(new TH1F("muon_trk_phi0_asym_ecc","Track Charge Asymmetry versus phi (Endcap C) ",m_nphi_bins,-180.,180.));
    sc=muon_trkparameters_endcap_asym.regHist(muon_trk_phi0_asym_ecc.back());
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_phi0_asym_ecc Failed to register histogram " << endreq;
	return sc;
      }

    muon_trk_chi2oDoF.push_back(new TProfile("muon_trk_chi2oDoF","#chi^{2} over Degrees of Freedom vrs Eta",120,-4.0,4.0,-25.0,25.0));  
    //muon_trk_chi2oDoF.push_back(new TProfile("muon_trk_chi2oDoF","#chi^{2} over Degrees of Freedom vrs Eta",100,-2.5,2.5,-5,5));  
    sc=muon_trkparameters_shift.regHist(muon_trk_chi2oDoF.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_chi2oDoF Failed to register histogram " << endreq;
	return sc;
      } 
    
    muon_trk_chi2oDoF_pT.push_back(new TProfile("muon_trk_chi2oDoF_pT","#chi^{2} over Degrees of Freedom vrs p_{T}",100,0.0,100.0,-25.0,25.0));  
    sc=muon_trkparameters_shift.regHist(muon_trk_chi2oDoF_pT.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_chi2oDoF_pT Failed to register histogram " << endreq;
	return sc;
      }      

    muon_trk_chi2oDoF_MDThits.push_back(new TProfile("muon_trk_chi2oDoF_MDThits","#chi^{2} over Degrees of Freedom vrs Number of MDT hits",50,-0.5,49.5,-25.0,25.0));  
    sc=muon_trkparameters_shift.regHist(muon_trk_chi2oDoF_MDThits.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_chi2oDoF_MDThits Failed to register histogram " << endreq;
	return sc;
      } 

    muon_trk_pT_MDThits.push_back(new TH2F("muon_trk_pT_MDThits","Number of MDT hits vrs. p_{T}",50,0.0,100.0,50,-0.5,49.5));  
    sc=muon_trkparameters.regHist(muon_trk_pT_MDThits.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_pT_MDThits Failed to register histogram " << endreq;
	return sc;
      } 
    
    muon_trk_chi2Prob.push_back(new TProfile("muon_trk_chi2Prob","chi2 Probability vrs Eta",100,-2.5,2.5));  
    sc=muon_trkparameters.regHist(muon_trk_chi2Prob.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_trk_chi2Prob Failed to register histogram " << endreq;
	return sc;
      } 

    muon_HitOnTrk_X0.push_back(new TH1F("muon_HitOnTrk_X0","X_{0} for hits (measurements) on track",100,-2.5,2.5));  
    sc=muon_trkparameters.regHist(muon_HitOnTrk_X0.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_HitOnTrk_X0 Failed to register histogram " << endreq;
	return sc;
      } 
      
    muon_HitOnTrk_ToF.push_back(new TH1F("muon_HitOnTrk_ToF","ToF for hits (measurements) on track",100,-2.5,2.5));  
    sc=muon_trkparameters.regHist(muon_HitOnTrk_ToF.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_HitOnTrk_ToF Failed to register histogram " << endreq;
	return sc;
      }   
         
     
  }
   
  return sc;  
}

//======================================================================================//
/**  bookTrkHolesHistograms */
//======================================================================================// 
StatusCode MuonSelectedTracksMon::bookTrkHolesHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
  if (m_debuglevel) m_log << MSG::DEBUG << "Inside bookTrkHolesHistograms()" << endreq;
    
  std::string m_generic_path_muonmonselected = "Muon/MuonTrackMonitoring";
  std::string muon_trkholes_path = "unknown";

  for (int j=0;j<(int)m_trackCollectionName.size();j++) {
    if ( m_trackCollectionFlag[j] == 0 ) continue;


 
    if ( m_MuonTriggerChainName == "NoMuonTriggerSelection")
      muon_trkholes_path = m_generic_path_muonmonselected+"/NoTrigger/MuonTrkHoles/"+m_trackCollectionName[j];
    else
      muon_trkholes_path = m_generic_path_muonmonselected+"/TriggerAware/MuonTrkHoles/"+m_trackCollectionName[j]+ m_MuonTriggerChainName;
    
    //declare a group of histograms
    MonGroup muon_trkholes ( this, muon_trkholes_path, run, ATTRIB_MANAGED );  
 
 
    muon_nmdtholes_per_track.push_back(new TH1F("muon_nmdtholes_per_track","Number of MDT holes per muontrack",15,-0.5,14.5));
    sc=muon_trkholes.regHist(muon_nmdtholes_per_track.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdtholes_per_track Failed to register histogram " << endreq;
	return sc;
      }
    muon_nrpcholes_per_track.push_back(new TH1F("muon_nrpcholes_per_track","Number of RPC holes per muontrack",15,-0.5,14.5));
    sc=muon_trkholes.regHist(muon_nrpcholes_per_track.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nrpcholes_per_track Failed to register histogram " << endreq;
	return sc;
      }
    muon_ntgcholes_per_track.push_back(new TH1F("muon_ntgcholes_per_track","Number of TGC holes per muontrack",15,-0.5,14.5));
    sc=muon_trkholes.regHist(muon_ntgcholes_per_track.back()) ; 
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ntgcholes_per_track Failed to register histogram " << endreq;
	return sc;
      }
    muon_ncscholes_per_track.push_back(new TH1F("muon_ncscholes_per_track","Number of CSC holes per muontrack",15,-0.5,14.5));
    sc=muon_trkholes.regHist(muon_ncscholes_per_track.back()) ;
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ncscholes_per_track Failed to register histogram " << endreq;
	return sc;
      }       
  }
   
  return sc;  
}

//======================================================================================//
/**  bookHitsPerTrkHistograms */
//======================================================================================// 
StatusCode MuonSelectedTracksMon::bookHitsPerTrkHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
  if (m_debuglevel) m_log << MSG::DEBUG << "Inside bookHitsPerTrkHistograms()" << endreq;
    
  std::string m_generic_path_muonmonselected = "Muon/MuonTrackMonitoring";
  std::string muon_hitspertrk_path = "unknown";
 

  for (int j=0;j<(int)m_trackCollectionName.size();j++) {
    if ( m_trackCollectionFlag[j] == 0 ) continue;


    if ( m_MuonTriggerChainName == "NoMuonTriggerSelection")
      muon_hitspertrk_path = m_generic_path_muonmonselected+"/NoTrigger/HitsPerTrk/"+m_trackCollectionName[j];
    else
      muon_hitspertrk_path = m_generic_path_muonmonselected+"/TriggerAware/HitsPerTrk/"+m_trackCollectionName[j]+ m_MuonTriggerChainName;
    
    //declare a group of histograms 
    MonGroup muon_hitspertrk ( this, muon_hitspertrk_path, run, ATTRIB_MANAGED ); 
    MonGroup muon_hitspertrk_MDT ( this, muon_hitspertrk_path+"/MDTHitsPerChamber", run, ATTRIB_MANAGED );
   
    /** Number of MDT hits per track. */ 
    muon_nmdthits_per_track.push_back(new TH1F("muon_nmdthits_per_track","Number of MDT hits per muontrack",50,-0.5,49.5));  
    sc=muon_hitspertrk.regHist(muon_nmdthits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of RPC hits per track. For Moore, it combines the combine the two adjacent RPC gas gaps into a single 
	measurement */
    muon_nrpchits_per_track.push_back(new TH1F("muon_nrpchits_per_track","Number of RPC hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_nrpchits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nrpchits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of MDT vs RPC hits per track. */ 
    muon_mdt_vs_rpc_hits_per_track.push_back(new TH2F("muon_mdt_vs_rpc_hits_per_track","MDThits vs. RPChits per muontrack",30,-0.5,29.5, 50,-0.5,49.5));
    sc=muon_hitspertrk.regHist(muon_mdt_vs_rpc_hits_per_track.back());                  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_mdt_vs_rpc_hits_per_track Failed to register histogram " << endreq;       
	return sc;
      }	            
    /** Number of TGC hits per track. */
    muon_ntgchits_per_track.push_back(new TH1F("muon_ntgchits_per_track","Number of TGC hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_ntgchits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ntgchits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of MDT vs TGC hits per track. */ 
    muon_mdt_vs_tgc_hits_per_track.push_back(new TH2F("muon_mdt_vs_tgc_hits_per_track","MDThits vs. TGChits per muontrack",20,-0.5,19.5, 50,-0.5,49.5));
    sc=muon_hitspertrk.regHist(muon_mdt_vs_tgc_hits_per_track.back());                  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_mdt_vs_tgc_hits_per_track Failed to register histogram " << endreq;       
	return sc;
      }      
    /** Number of CSC hits per track. */
    muon_ncschits_per_track.push_back(new TH1F("muon_ncschits_per_track","Number of CSC hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_ncschits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ncschits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of RPC ETA hits per track. For Moore, it combines the combine the two adjacent RPC gas gaps into a single 
	measurement */        
    muon_nrpcetahits_per_track.push_back(new TH1F("muon_nrpcetahits_per_track","Number of RPC Eta hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_nrpcetahits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nrpcetahits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of RPC PHI hits per track. For Moore, it combines the combine the two adjacent RPC gas gaps into a single
	measurement */
    muon_nrpcphihits_per_track.push_back(new TH1F("muon_nrpcphihits_per_track","Number of RPC Phi hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_nrpcphihits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nrpcphihits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of TGC ETA hits per track. */
    muon_ntgcetahits_per_track.push_back(new TH1F("muon_ntgcetahits_per_track","Number of TGC Eta hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_ntgcetahits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ntgcetahits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of TGC PHI hits per track. */
    muon_ntgcphihits_per_track.push_back(new TH1F("muon_ntgcphihits_per_track","Number of TGC Phi hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_ntgcphihits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ntgcphihits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of CSC ETA hits per track. */
    muon_ncscetahits_per_track.push_back(new TH1F("muon_ncscetahits_per_track","Number of CSC Eta hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_ncscetahits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ncscetahits_per_track Failed to register histogram " << endreq;
	return sc;
      }
    /** Number of CSC PHI hits per track. */
    muon_ncscphihits_per_track.push_back(new TH1F("muon_ncscphihits_per_track","Number of CSC Phi hits per muontrack",30,-0.5,29.5));  
    sc=muon_hitspertrk.regHist(muon_ncscphihits_per_track.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_ncscphihits_per_track Failed to register histogram " << endreq;
	return sc;
      }

	 
    /** Number of MDT hits per track.. per PHI sector in BARREL */ 
	  
    muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel.push_back(new
								TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel","Number of MDT hits, in Barrel, Phi Sector 2",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_2_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel.push_back(new
								TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel","Number of MDT hits, in Barrel, Phi Sector 4",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_4_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel.push_back(new
								TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel","Number of MDT hits, in Barrel, Phi Sector 6",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_6_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel.push_back(new
								TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel","Number of MDT hits, in Barrel, Phi Sector 8",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_8_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel","Number of MDT hits, in Barrel, Phi Sector 10",48,0.,48., 0, 
									      50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_10_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel","Number of MDT hits, in Barrel, Phi Sector 12",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_12_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel","Number of MDT hits, in Barrel, Phi Sector 14",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_14_barrel Failed to register histogram " << endreq;
	return sc;
      }

    muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel","Number of MDT hits, in Barrel, Phi Sector 0",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_16_barrel Failed to register histogram " << endreq;
	return sc;
      }	  

    muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel","Number of MDT hits, in Barrel, Phi Sector 1",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_1_barrel Failed to register histogram " << endreq;
	return sc;
      }	  
    muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel","Number of MDT hits, in Barrel, Phi Sector 3",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_3_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel","Number of MDT hits, in Barrel, Phi Sector 5",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_5_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel","Number of MDT hits, in Barrel, Phi Sector 7",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_7_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel","Number of MDT hits, in Barrel, Phi Sector 9",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_9_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel","Number of MDT hits, in Barrel, Phi Sector 11",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_11_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel","Number of MDT hits, in Barrel, Phi Sector 13",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_13_barrel Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel","Number of MDT hits, in Barrel, Phi Sector 15",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_15_barrel Failed to register histogram " << endreq;
	return sc;
      }
	  
    /** Number of MDT hits per track.. per PHI sector in ENDCAP A */ 
		
    muon_nmdthits_per_track_vs_eta_PhiSector_2_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_2_eca","Number of MDT hits, in End Cap A, Phi Sector 2",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_2_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_2_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_4_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_4_eca","Number of MDT hits, in End Cap A, Phi Sector 4",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_4_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_4_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_6_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_6_eca","Number of MDT hits, in End Cap A, Phi Sector 6",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_6_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_6_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_8_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_8_eca","Number of MDT hits, in End Cap A, Phi Sector 8",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_8_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_8_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_10_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_10_eca","Number of MDT hits, in End Cap A, Phi Sector 10",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_10_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_10_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_12_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_12_eca","Number of MDT hits, in End Cap A, Phi Sector 12",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_12_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_12_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_14_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_14_eca","Number of MDT hits, in End Cap A, Phi Sector 14",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_14_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_14_eca Failed to register histogram " << endreq;
	return sc;
      }

    muon_nmdthits_per_track_vs_eta_PhiSector_16_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_16_eca","Number of MDT hits, in End Cap A, Phi Sector 0",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_16_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_16_eca Failed to register histogram " << endreq;
	return sc;
      }	  

    muon_nmdthits_per_track_vs_eta_PhiSector_1_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_1_eca","Number of MDT hits, in End Cap A, Phi Sector 1",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_1_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_1_eca Failed to register histogram " << endreq;
	return sc;
      }	  
    muon_nmdthits_per_track_vs_eta_PhiSector_3_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_3_eca","Number of MDT hits, in End Cap A, Phi Sector 3",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_3_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_3_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_5_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_5_eca","Number of MDT hits, in End Cap A, Phi Sector 5",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_5_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_5_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_7_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_7_eca","Number of MDT hits, in End Cap A, Phi Sector 7",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_7_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_7_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_9_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_9_eca","Number of MDT hits, in End Cap A, Phi Sector 9",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_9_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_9_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_11_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_11_eca","Number of MDT hits, in End Cap A, Phi Sector 11",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_11_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_11_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_13_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_13_eca","Number of MDT hits, in End Cap A, Phi Sector 13",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_13_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_13_eca Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_15_eca.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_15_eca","Number of MDT hits, in End Cap A, Phi Sector 15",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_15_eca.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_15_eca Failed to register histogram " << endreq;
	return sc;
      }	 
	
    /** Number of MDT hits per track.. per PHI sector in ENDCAP C */ 
	
    muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc","Number of MDT hits, in End Cap C, Phi Sector 2",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_2_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc.push_back(new
							     TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc","Number of MDT hits, in End Cap C, Phi Sector 4",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_4_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc","Number of MDT hits, in End Cap C, Phi Sector 6",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_6_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc","Number of MDT hits, in End Cap C, Phi Sector 8",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_8_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc","Number of MDT hits, in End Cap C, Phi Sector 10",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_10_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc","Number of MDT hits, in End Cap C, Phi Sector 12",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_12_ecc Failed to register histogram " << endreq;
	return sc;
      }

    muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc","Number of MDT hits, in End Cap C,P hi Sector 14",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_14_ecc Failed to register histogram " << endreq;
	return sc;
      }

    muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc","Number of MDT hits, in End Cap C, Phi Sector 0",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_16_ecc Failed to register histogram " << endreq;
	return sc;
      }	  


    muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc","Number of MDT hits, in End Cap C, Phi Sector 1",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_1_ecc Failed to register histogram " << endreq;
	return sc;
      }	  
    muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc","Number of MDT hits, in End Cap C, Phi Sector 3",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_3_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc","Number of MDT hits, in End Cap C, Phi Sector 5",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_5_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc","Number of MDT hits, in End Cap C, Phi Sector 7",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_7_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc","Number of MDT hits, in End Cap C, Phi Sector 9",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_9_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc","Number of MDT hits, in End Cap C, Phi Sector 11",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_11_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc","Number of MDT hits, in End Cap C, Phi Sector 13",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_13_ecc Failed to register histogram " << endreq;
	return sc;
      }
    muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc.push_back(new TProfile("muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc","Number of MDT hits, in End Cap C, Phi Sector 15",1,0,1, 0, 50));  
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc.back()) ;  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_eta_PhiSector_15_ecc Failed to register histogram " << endreq;
	return sc;
      }	
	
    // MDT hits vs. phi, per chamber
    muon_nmdthits_per_track_vs_phi_per_chamber.push_back(new TH2F("muon_nmdthits_per_track_vs_phi_per_chamber","MDThits per chamber vs. Phi Sector", 8,0,8, 50,-0.5,49.5));
    sc=muon_hitspertrk_MDT.regHist(muon_nmdthits_per_track_vs_phi_per_chamber.back());                  
    if(sc.isFailure())
      { m_log << MSG::FATAL << "muon_nmdthits_per_track_vs_phi_per_chamber Failed to register histogram " << endreq;       
	return sc;
      }	  
  }
   
  return sc;  
}


