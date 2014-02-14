/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ================================================================================
   MuonGenericTracksMon.cxx
   ------------------------------
   AUTHORS:     N. Benekos, E. Christidi,  A. Cortes, A. Eppig, I. Nomidis, Tony Liss
   created:     November 2007
   modified June 2008: for Trigger Aware monitoring :  A. Cortes (UIUC)
   description: Implementation code for the MuonGenericTracksMon
   ============================================================================== */

#include "MuonTrackMonitoring/MuonGenericTracksMon.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ResidualPull.h"

#include "TrkMeasurementBase/MeasurementBase.h" 
#include "TrkParameters/TrackParameters.h"
 
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"
 
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonDQAFitFunc.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include <TH1F.h>
#include <TH2F.h> 
#include <TMath.h>

const int s_maxEvents     = 100000;
const int s_maxComponents = 50;
const int s_maxSoS        = 80;


// *********************************************************************
// Public Methods
// *********************************************************************

MuonGenericTracksMon::MuonGenericTracksMon( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ), 
   m_muonHitSummaryTool("Muon::MuonHitSummaryTool/MuonHitSummaryTool"),
   m_log( msgSvc(), name ),  
   m_trigDecTool(""),
   m_trackSumTool("Trk::TrackSummaryTool/MuonTrackSummaryTool"),
   m_muondqafitfunc("Muon::MuonDQAFitFunc/MuonDQAFitFunc"),
   m_trackSummaryTool("Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool")
{
  m_pullCalculator = ToolHandle<Trk::IResidualPullCalculator>("Trk::ResidualPullCalculator/ResidualPullCalculator");
  declareProperty( "WhichTrackCollections", 	m_trackCollectionFlag);
  declareProperty( "MuonTrackCollections",  	m_trackCollectionName);
  declareProperty( "MuonTriggerChainName", 	m_MuonTriggerChainName);
  declareProperty( "NPhiBins",  		m_nphi_bins=360); 
  declareProperty( "TriggerDecisionTool", 	m_trigDecTool);
  declareProperty( "Muon_Trigger_Items", 	m_muon_triggers);
  declareProperty( "UseTriggerVector", 		m_useTrigger); 
  declareProperty( "MuonHitSummaryTool",        m_muonHitSummaryTool);
  declareProperty( "TrkSummaryTool",            m_trackSumTool);         //User Flag to configure TrackSummaryTool in python
  declareProperty( "MuonHistSumTool",           m_muonHitSummaryTool = std::string("Muon::MuonHitSummaryTool/MuonHitSummaryTool"));
}

/** Function to convert Radians into Degrees  */
float MuonGenericTracksMon::RadsToDegrees(float Radians)
{
  float Degrees = Radians * (180.) / M_PI;
  return Degrees;
}

/** Destructor  */ 
//---------------------------------------------------------------------------------------
MuonGenericTracksMon::~MuonGenericTracksMon()
{
  m_log << MSG::INFO << " deleting MuonGenericTracksMon " << endreq;
}


//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode MuonGenericTracksMon:: initialize() 
{ 

  // to fix the https://savannah.cern.ch/bugs/?56680
  StatusCode sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;
 
  // init message stream
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached

  if (m_debuglevel) m_log << MSG::DEBUG << "MuonGenericTracksMon::initialize() to setup tools/services" << endreq;

  sc = StatusCode::SUCCESS;
  
  /** Initialize tools and services */
  if (m_debuglevel) m_log << MSG::DEBUG << "Initialize tools" << endreq;
  sc = setupTools();
   
  return sc;
}

/** bookHistograms */
//---------------------------------------------------------------------------------------
StatusCode MuonGenericTracksMon::bookHistograms()
{
  
  if (m_debuglevel) {
    m_log << MSG::DEBUG << "MuonGenericTracksMon::bookHistograms() to setup tools/services" << endreq;
  }
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {
 

    if (sc.isFailure()) {
      m_log << MSG::FATAL << "MuonGenericTracksMon::Failed to initialize tools/services!" << endreq;
      return sc;
    } 
    else if (m_debuglevel) {m_log << MSG::DEBUG << "MuonGenericTracksMon::Successfully initialized tools/services" << endreq; }

    /*   if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
    }

    if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
    } */

    /** Loop over Track Collections */
    if (m_debuglevel) m_log << MSG::DEBUG << "TrackCollectionSize " << (int)m_trackCollectionName.size() << endreq;

    for (int j=0;j<(int)m_trackCollectionName.size();j++) {
      if ( m_trackCollectionFlag[j] == 0 ) continue;

      std::string muontracks_generic_path = "Muon/MuonTrackMonitoring";
 
      std::string muontracks_path = "unknown";
      std::string muontracks_residuals_barrel_path = "unknown";
      std::string muontracks_residuals_eca_path    = "unknown";
      std::string muontracks_residuals_ecc_path    = "unknown";
      std::string muontracks_chargeratio_path      = "unknown";
      std::string muontracks_MDTresiduals_path     = "unknown"; 


      //if (m_debuglevel) m_log << MSG::DEBUG << "UseTrigger? before creating MonGroup's for " << m_trackCollectionName[j] << ", " << m_useTrigger << endreq;

      if (m_MuonTriggerChainName == "NoMuonTriggerSelection")
	{ 
	  muontracks_path                  = muontracks_generic_path+"/NoTrigger/MuonGenericTracksMon/"+m_trackCollectionName[j];
	  muontracks_residuals_barrel_path = muontracks_generic_path+"/NoTrigger/MuonGenericTracksMon/"+m_trackCollectionName[j] + "/ResidualsBarrel";
	  muontracks_residuals_eca_path    = muontracks_generic_path+"/NoTrigger/MuonGenericTracksMon/"+m_trackCollectionName[j] + "/ResidualsEndCapA";
	  muontracks_residuals_ecc_path    = muontracks_generic_path+"/NoTrigger/MuonGenericTracksMon/"+m_trackCollectionName[j] + "/ResidualsEndCapC";
	  muontracks_chargeratio_path      = muontracks_generic_path+"/NoTrigger/MuonGenericTracksMon/"+m_trackCollectionName[j] + "/ChargeRatio";
	  muontracks_MDTresiduals_path     = muontracks_generic_path+"/NoTrigger/MuonGenericTracksMon/"+m_trackCollectionName[j] + "/MDT_Pull_Residuals";
	}   else
	{ 
	  muontracks_path                  = muontracks_generic_path+"/TriggerAware/MuonGenericTracksMon/"+m_trackCollectionName[j]+ m_MuonTriggerChainName;
	  muontracks_residuals_barrel_path = muontracks_generic_path+"/TriggerAware/MuonGenericTracksMon/"+m_trackCollectionName[j]+ m_MuonTriggerChainName+ "/ResidualsBarrel";
	  muontracks_residuals_eca_path    = muontracks_generic_path+"/TriggerAware/MuonGenericTracksMon/"+m_trackCollectionName[j]+ m_MuonTriggerChainName+ "/ResidualsEndCapA";
	  muontracks_residuals_ecc_path    = muontracks_generic_path+"/TriggerAware/MuonGenericTracksMon/"+m_trackCollectionName[j]+ m_MuonTriggerChainName + "/ResidualsEndCapC";    
	  muontracks_chargeratio_path      = muontracks_generic_path+"/TriggerAware/MuonGenericTracksMon/"+m_trackCollectionName[j]+ m_MuonTriggerChainName + "/ChargeRatio";
	  muontracks_MDTresiduals_path     = muontracks_generic_path+"/TriggerAware/MuonGenericTracksMon/"+m_trackCollectionName[j]+ m_MuonTriggerChainName + "/MDT_Pull_Residuals";
	} 

      MgmtAttr_t attr = ATTRIB_MANAGED;
      MonGroup muontracks_shift                 (this, muontracks_path,  run, attr );
      MonGroup muontracks_shift_Norm            (this, muontracks_path,  run, attr , "", "weightedEff" );
      MonGroup muontracks_debug                 (this, muontracks_path,  run, attr );    
      MonGroup muontracks_residuals_barrel      (this, muontracks_residuals_barrel_path, run, attr );
      MonGroup muontracks_residuals_eca         (this, muontracks_residuals_eca_path, run, attr );
      MonGroup muontracks_residuals_ecc         (this, muontracks_residuals_ecc_path, run, attr );
      MonGroup muontracks_residuals_barrel_Norm (this, muontracks_residuals_barrel_path, run, attr );
      MonGroup muontracks_residuals_eca_Norm    (this, muontracks_residuals_eca_path, run, attr );
      MonGroup muontracks_residuals_ecc_Norm    (this, muontracks_residuals_ecc_path, run, attr );
      MonGroup muontracks_chargeratio           (this, muontracks_chargeratio_path,  run, attr );
      MonGroup muontracks_chargeratio_Norm      (this, muontracks_chargeratio_path,  run, attr, "", "weightedEff" );
      MonGroup muontracks_MDTresidual_debug     (this, muontracks_MDTresiduals_path, run, attr );
      MonGroup muontracks_MDTresidual_Norm      (this, muontracks_MDTresiduals_path, run, attr, "", "weightedEff" );
      MonGroup muontracks_MDTresidual_shift     (this, muontracks_MDTresiduals_path, run, attr );

      if(newEventsBlock){}
      if(newLumiBlock){}
      if(newRun)
	{      
	  if (m_debuglevel) m_log << MSG::DEBUG << "MuonGenericTracksMon : isNewRun" << endreq;

	  // Shift Monitoring Plots
	  /** P_T distribution for reconstructed Muons.  */
	  muon_rec_pT.push_back(new TH1F("muon_rec_pT",  "Reconstructed Muon P_{T}", 100,0.,100.));
	  sc=muontracks_shift.regHist(muon_rec_pT.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pT Failed to register histogram " << endreq;       
	      return sc;
	    }  
	  /** P_T distribution for reconstructed Muons with Positive charge.  */  
	  muon_rec_pos_pT.push_back(new TH1F("muon_rec_pos_pT",  "Reconstructed Muon P_{T}, POSITIVE charge", 100,0.,100.));
	  sc=muontracks_debug.regHist(muon_rec_pos_pT.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge.  */
	  muon_rec_neg_pT.push_back(new TH1F("muon_rec_neg_pT",  "Reconstructed Muon P_{T}, NEGATIVE charge",100,0.,100.));
	  sc=muontracks_debug.regHist(muon_rec_neg_pT.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Carge Ratio distribution for reconstructed Muons vrs pT.  */
	  muon_rec_ChargeRatio_pT.push_back(new TH1F("muon_rec_ChargeRatio_pT",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}",100,0.,100.));
	  sc=muontracks_shift_Norm.regHist(muon_rec_ChargeRatio_pT.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT Failed to register histogram " << endreq;       
	      return sc;
	    } 	    
	  /** High P_T distribution for reconstructed Muons.  */
	  muon_rec_HighpT.push_back(new TH1F("muon_rec_HighpT",  "Reconstructed Muon (High) P_{T}", 90,100.,1000.));
	  sc=muontracks_shift.regHist(muon_rec_HighpT.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_HighpT Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  /** High P_T distribution for reconstructed Muons with Positive charge.  */  
	  muon_rec_pos_HighpT.push_back(new TH1F("muon_rec_pos_HighpT",  "Reconstructed Muon (High) P_{T}, POSITIVE charge",90, 100., 1000.));
	  sc=muontracks_debug.regHist(muon_rec_pos_HighpT.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_HighpT Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** High P_T distribution for reconstructed Muons with negative charge.  */
	  muon_rec_neg_HighpT.push_back(new TH1F("muon_rec_neg_HighpT",  "Reconstructed Muon (High) P_{T}, NEGATIVE charge",90, 100., 1000.));
	  sc=muontracks_debug.regHist(muon_rec_neg_HighpT.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_HighpT Failed to register histogram " << endreq;       
	      return sc;
	    } 
	    
	  /* pT histograms to understand charge asymmetry */
	  
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector01.push_back(new TH1F("muon_rec_pos_pT_Sector01",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector01", 75,0.,150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector01.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector01 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector01.push_back(new TH1F("muon_rec_neg_pT_Sector01",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector 01",75,0.,150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector01.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector01 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector01.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector01",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 01",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector01.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector01 Failed to register histogram " << endreq;       
	      return sc;
	    }   
  
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector02.push_back(new TH1F("muon_rec_pos_pT_Sector02",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector02", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector02.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector02 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector02.push_back(new TH1F("muon_rec_neg_pT_Sector02",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector02",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector02.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector02 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector02.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector02",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 02",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector02.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector02 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector03.push_back(new TH1F("muon_rec_pos_pT_Sector03",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector03", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector03.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector03 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector03.push_back(new TH1F("muon_rec_neg_pT_Sector03",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector03",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector03.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector03 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector03.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector03",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 03",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector03.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector03 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector04.push_back(new TH1F("muon_rec_pos_pT_Sector04",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector04", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector04.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector04 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector04.push_back(new TH1F("muon_rec_neg_pT_Sector04",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector04",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector04.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector04 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector04.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector04",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 04",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector04.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector04 Failed to register histogram " << endreq;       
	      return sc;
	    } 	    
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector05.push_back(new TH1F("muon_rec_pos_pT_Sector05",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector05", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector05.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector05 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector05.push_back(new TH1F("muon_rec_neg_pT_Sector05",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector05",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector05.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector05 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector05.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector05",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 05",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector05.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector05 Failed to register histogram " << endreq;       
	      return sc;
	    } 	    
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector06.push_back(new TH1F("muon_rec_pos_pT_Sector06",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector06", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector06.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector06 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector06.push_back(new TH1F("muon_rec_neg_pT_Sector06",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector06",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector06.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector06 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector06.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector06",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 06",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector06.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector06 Failed to register histogram " << endreq;       
	      return sc;
	    } 	    
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector07.push_back(new TH1F("muon_rec_pos_pT_Sector07",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector07", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector07.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector07 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector07.push_back(new TH1F("muon_rec_neg_pT_Sector07",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector07",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector07.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector07 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector07.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector07",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 07",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector07.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector07 Failed to register histogram " << endreq;       
	      return sc;
	    } 	    
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector08.push_back(new TH1F("muon_rec_pos_pT_Sector08",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector08", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector08.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector08 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector08.push_back(new TH1F("muon_rec_neg_pT_Sector08",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector08",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector08.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector08 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector08.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector08",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 08",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector08.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector08 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector09.push_back(new TH1F("muon_rec_pos_pT_Sector09",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector09", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector09.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector09 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector09.push_back(new TH1F("muon_rec_neg_pT_Sector09",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector09",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector09.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector09 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector09.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector09",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 09",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector09.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector09 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector10.push_back(new TH1F("muon_rec_pos_pT_Sector10",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector10", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector10.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector10 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector10.push_back(new TH1F("muon_rec_neg_pT_Sector10",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector10",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector10.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector10 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector10.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector10",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 10",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector10.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector10 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector11.push_back(new TH1F("muon_rec_pos_pT_Sector11",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector11", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector11.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector11 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector11.push_back(new TH1F("muon_rec_neg_pT_Sector11",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector11",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector11.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector11 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector11.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector11",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 11",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector11.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector11 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector12.push_back(new TH1F("muon_rec_pos_pT_Sector12",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector12", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector12.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector12 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector12.push_back(new TH1F("muon_rec_neg_pT_Sector12",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector12",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector12.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector12 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector12.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector12",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 12",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector12.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector12 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector13.push_back(new TH1F("muon_rec_pos_pT_Sector13",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector13", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector13.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector13 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector13.push_back(new TH1F("muon_rec_neg_pT_Sector13",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector13",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector13.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector13 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector13.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector13",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 13",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector13.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector13 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector14.push_back(new TH1F("muon_rec_pos_pT_Sector14",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector14", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector14.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector14 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector14.push_back(new TH1F("muon_rec_neg_pT_Sector14",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector14",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector14.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector14 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector14.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector14",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 14",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector14.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector14 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector15.push_back(new TH1F("muon_rec_pos_pT_Sector15",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector15", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector15.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector15 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector15.push_back(new TH1F("muon_rec_neg_pT_Sector15",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector15",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector15.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector15 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector15.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector15",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 15",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector15.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector15 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Sector16.push_back(new TH1F("muon_rec_pos_pT_Sector16",  "Reconstructed Muon P_{T}, POSITIVE charge, Sector16", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Sector16.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Sector16 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Sector16.push_back(new TH1F("muon_rec_neg_pT_Sector16",  "Reconstructed Muon P_{T}, NEGATIVE charge, Sector16",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Sector16.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Sector16 Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_Sector16.push_back(new TH1F("muon_rec_ChargeRatio_pT_Sector16",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Sector 16",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_Sector16.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_Sector16 Failed to register histogram " << endreq;       
	      return sc;
	    } 	    
 	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Up.push_back(new TH1F("muon_rec_pos_pT_Up",  "Reconstructed Muon P_{T}, POSITIVE charge, upper sectors", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Up.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Up Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Up.push_back(new TH1F("muon_rec_neg_pT_Up",  "Reconstructed Muon P_{T}, NEGATIVE charge, upper sector",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Up.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Up Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_up.push_back(new TH1F("muon_rec_ChargeRatio_pT_up",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, upper sectors",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_up.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_up Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_Down.push_back(new TH1F("muon_rec_pos_pT_Down",  "Reconstructed Muon P_{T}, POSITIVE charge, lower sectors", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_Down.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_Down Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_Down.push_back(new TH1F("muon_rec_neg_pT_Down",  "Reconstructed Muon P_{T}, NEGATIVE charge, lower sectors",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_Down.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_Down Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_dw.push_back(new TH1F("muon_rec_ChargeRatio_pT_dw",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, lower sectors",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_dw.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_dw Failed to register histogram " << endreq;       
	      return sc;
	    }  
	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_A.push_back(new TH1F("muon_rec_pos_pT_A",  "Reconstructed Muon P_{T}, POSITIVE charge, A", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_A.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_A Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_A.push_back(new TH1F("muon_rec_neg_pT_A",  "Reconstructed Muon P_{T}, NEGATIVE charge, A",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_A.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_A Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_A.push_back(new TH1F("muon_rec_ChargeRatio_pT_A",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Side A",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_A.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_A Failed to register histogram " << endreq;       
	      return sc;
	    }    	    	    	    	    	      
 	  /** P_T distribution for reconstructed Muons with Positive charge per Sector*/  
	  muon_rec_pos_pT_C.push_back(new TH1F("muon_rec_pos_pT_C",  "Reconstructed Muon P_{T}, POSITIVE charge, C", 75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_C.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_C Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** P_T distribution for reconstructed Muons with negative charge per Sector */
	  muon_rec_neg_pT_C.push_back(new TH1F("muon_rec_neg_pT_C",  "Reconstructed Muon P_{T}, NEGATIVE charge, C",75, 0., 150.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_C.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_C Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  /** Charg Ratio vs pT for reconstructed Muons per Sector */
	  muon_rec_ChargeRatio_pT_C.push_back(new TH1F("muon_rec_ChargeRatio_pT_C",  "Charge Ratio (+/-) vs. Reconstructed Muon P_{T}, Side C",75, 0., 150.));
	  sc=muontracks_chargeratio_Norm.regHist(muon_rec_ChargeRatio_pT_C.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_ChargeRatio_pT_C Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  ///++++ pT histograms to understand charge ratio

	  /** Curvature (1 / P_T) distribution for reconstructed Muons.  */
	  muon_rec_1overpT.push_back(new TH1F("muon_rec_1overpT",  "Reconstructed Muon 1/Pt (in 1/GeV)", 20,0.,10.));
	  sc=muontracks_debug.regHist(muon_rec_1overpT.back());
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_1overpT Failed to register histogram " << endreq;       
	      return sc;
	    } 
	    
	  muon_rec_nStations.push_back(new TH1F("muon_rec_nStations",  "Number of stations in the Reconstructed Muon Track", 11,-0.5, 10.5));
	  sc=muontracks_debug.regHist(muon_rec_nStations.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_nStations Failed to register histogram " << endreq;       
	      return sc;
	    } 
	    
	  /** Charge distribution for reconstructed Muons.  */
	  muon_rec_charge.push_back(new TH1F("muon_rec_charge",  "Charge of Reconstructed Muon Track", 3,-1.5,1.5));
	  sc=muontracks_debug.regHist(muon_rec_charge.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_charge Failed to register histogram " << endreq;       
	      return sc;
	    } 

 	  /** Diff of P_T distribution for reconstructed Muons with Positive charge between Up and Down sectors*/  
	  muon_rec_pos_pT_up_dw_diff.push_back(new TH1F("muon_rec_pos_pT_up_dw_diff",  "Reconstructed Muon P_{T} difference between Up-Down sectors, POSITIVE charge", 100, -400., 400.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_up_dw_diff.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_up_dw_diff Failed to register histogram " << endreq;       
	      return sc;
	    } 

 	  /** Diff of P_T distribution for reconstructed Muons with Negative Charge between Up and Down sectors*/  
	  muon_rec_neg_pT_up_dw_diff.push_back(new TH1F("muon_rec_neg_pT_up_dw_diff",  "Reconstructed Muon P_{T} difference between Up-Down sectors, NEGATIVE charge", 100, -400., 400.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_up_dw_diff.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_up_dw_diff Failed to register histogram " << endreq;       
	      return sc;
	    } 

 	  /** Diff of P_T distribution for reconstructed Muons with Positive Charge between SideA and SideC sectors*/  
	  muon_rec_pos_pT_A_C_diff.push_back(new TH1F("muon_rec_pos_pT_A_C_diff",  "Reconstructed Muon P_{T} difference between SideA and SideC sectors, POSITIVE charge", 100, -400., 400.));
	  sc=muontracks_chargeratio.regHist(muon_rec_pos_pT_A_C_diff.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pos_pT_A_C_diff Failed to register histogram " << endreq;       
	      return sc;
	    } 

 	  /** Diff of P_T distribution for reconstructed Muons with Negative Charge between SideA and SideC sectors*/  
	  muon_rec_neg_pT_A_C_diff.push_back(new TH1F("muon_rec_neg_pT_A_C_diff",  "Reconstructed Muon P_{T} difference between SideA and SideC sectors, NEGATIVE charge", 100, -400., 400.));
	  sc=muontracks_chargeratio.regHist(muon_rec_neg_pT_A_C_diff.back());                                                       
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_neg_pT_A_C_diff Failed to register histogram " << endreq;       
	      return sc;
	    }  
 
	  /** Reconstructed ETA vs. PHI*/ 
	  muon_rec_eta_phi.push_back(new TH2F("muon_rec_eta_phi",  "Eta vrs Phi of reconstructed muon",m_nphi_bins,-180.,180., 40,-4.,4.));
	  sc=muontracks_shift.regHist(muon_rec_eta_phi.back());                  
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_eta_phi Failed to register histogram " << endreq;       
	      return sc;
	    } 
	
	  muon_rec_eta_phi_3moreStations.push_back(new TH2F("muon_rec_eta_phi_3moreStations",  "Eta vrs Phi of reconstructed muon (with 3 or more stations)",m_nphi_bins,-180.,180., 40,-4.,4.));
	  sc=muontracks_shift.regHist(muon_rec_eta_phi_3moreStations.back());                  
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_eta_phi_3moreStations Failed to register histogram " << endreq;       
	      return sc;
	    } 
	    
	  muon_rec_eta_phi_2lessStations.push_back(new TH2F("muon_rec_eta_phi_2lessStations",  "Eta vrs Phi of reconstructed muon (with 2 or less stations)",m_nphi_bins,-180.,180., 40,-4.,4.));
	  sc=muontracks_shift.regHist(muon_rec_eta_phi_2lessStations.back());                  
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_eta_phi_2lessStations Failed to register histogram " << endreq;       
	      return sc;
	    }        
	    
	  /** Reconstructed P_T vs. ETA*/
	  muon_rec_pT_vs_eta.push_back(new TH2F("muon_rec_pT_vs_eta","Reconstructed muon P_{T} vrs Eta", 40,-4.,4.,10,0.,100.));
	  sc=muontracks_debug.regHist(muon_rec_pT_vs_eta.back());
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pT_vs_eta Failed to register histogram " << endreq;
	      return sc;
	    }
	  /** Reconstructed P_T vs. PHI*/ 
	  muon_rec_pT_vs_phi.push_back(new TH2F("muon_rec_pT_vs_phi","Reconstructed muon P_{T} vrs Phi",m_nphi_bins,-180.,180.,10,0.,100.));
	  sc=muontracks_debug.regHist(muon_rec_pT_vs_phi.back());
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_pT_vs_phi Failed to register histogram " << endreq;
	      return sc;
	    }
           
	  //  eta and phi histograms to understand charge asymmetry
	  /** Reconstructed charge vs. ETA*/
	  muon_rec_charge_vs_eta.push_back(new TH2F("muon_rec_charge_vs_eta","Reconstructed muon charge vrs Eta", 40,-4.0,4.0,3,-1.5,1.5));
	  sc=muontracks_debug.regHist(muon_rec_charge_vs_eta.back());
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_charge_vs_eta Failed to register histogram " << endreq;
	      return sc;
	    }
	  /** Reconstructed charge vs. PHI*/ 
	  muon_rec_charge_vs_phi.push_back(new TH2F("muon_rec_charge_vs_phi","Reconstructed muon charge vrs Phi",m_nphi_bins,-180.,180.,3,-1.5,1.5));
	  sc=muontracks_debug.regHist(muon_rec_charge_vs_phi.back());
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_charge_vs_phi Failed to register histogram " << endreq;
	      return sc;
	    }   
	   
	  ///Track Parameters and its error at perigee 
	  muon_rec_DegreesOfFreedom.push_back( new TH1F("muon_rec_DegreesOfFreedom",  "Number degrees of freedom of muon reconstructed track", 50,-0.5,49.5));
	  sc=muontracks_debug.regHist(muon_rec_DegreesOfFreedom.back());                   
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_DegreesOfFreedom Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  muon_rec_chi2.push_back(new TH1F("muon_rec_chi2",  "chi2 of muon reconstructed track/numberDoF", 100,0.,30.));
	  sc=muontracks_debug.regHist(muon_rec_chi2.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_rec_chi2 Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_qOverP.push_back(new TH1F("muon_perigee_qOverP",  "Reconstructed inverse momentum of muon track", 20,-1.,1.));
	  sc=muontracks_debug.regHist(muon_perigee_qOverP.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_perigee_qOverP Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_P.push_back(new TH1F("muon_perigee_P",  "Reconstructed momentum of muon track", 25,0.,250.));
	  sc=muontracks_debug.regHist(muon_perigee_P.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_perigee_P Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  float m_d0_ranges[2] = {-500.0, 500.0}; 
	  float m_z0_ranges[2] = {-500.0, 500.0}; 
	  
	  if (m_trackCollectionName[j] == "MooreTracks") { 
	    m_d0_ranges[0] = -1.0;  m_d0_ranges[1] = 1.0; 
	    m_z0_ranges[0] = -5.0; m_z0_ranges[1] = 5.0; 
	  }
	  
	  muon_perigee_d0.push_back(new TH1F("muon_perigee_d0",  "Reconstructed d_{0} (impact parameter) of muon track", 100,m_d0_ranges[0],m_d0_ranges[1]));
	  sc=muontracks_shift.regHist(muon_perigee_d0.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_perigee_d0 Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_z0.push_back(new TH1F("muon_perigee_z0",  "Reconstructed z_{0} of muon track", 100,m_z0_ranges[0],m_z0_ranges[1]));
	  sc=muontracks_shift.regHist(muon_perigee_z0.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_perigee_z0 Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_phi0.push_back(new TH1F("muon_perigee_phi0",  "Reconstructed Phi_{0} of muon track",m_nphi_bins,-180.,180.));
	  sc=muontracks_debug.regHist(muon_perigee_phi0.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_perigee_phi0 Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_theta.push_back(new TH1F("muon_perigee_theta",  "Reconstructed Theta of muon track", 100,0.,4.));
	  sc=muontracks_debug.regHist(muon_perigee_theta.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_perigee_theta Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_eta.push_back(new TH1F("muon_perigee_eta",  "Reconstructed Eta of muon track", 100,-4.0,4.0));
	  sc=muontracks_debug.regHist(muon_perigee_eta.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_perigee_eta Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_error_d0.push_back(new TH1F("muon_perigee_error_d0",  "error in d_{0} on muon track", 40,0.,400.));
	  sc=muontracks_debug.regHist(muon_perigee_error_d0.back());                                                      //add each histogram to this group
	  if(sc.isFailure()) {
	    m_log << MSG::FATAL << "m_muon_error_d0 Failed to register histogram " << endreq;       
	    return sc;
	  } 

	  muon_perigee_error_z0.push_back(new TH1F("muon_perigee_error_z0",  "error in z_{0} on muon track", 100,0.,1000.));
	  sc=muontracks_debug.regHist(muon_perigee_error_z0.back());                                                      //add each histogram to this group
	  if(sc.isFailure()) {
	    m_log << MSG::FATAL << "m_muon_error_z0 Failed to register histogram " << endreq;       
	    return sc;
	  } 

	  muon_perigee_error_phi0.push_back(new TH1F("muon_perigee_error_phi0",  "error in phi_{0} on muon track", 30,0.,30.));
	  sc=muontracks_debug.regHist(muon_perigee_error_phi0.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "m_muon_error_phi0 Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_error_theta0.push_back(new TH1F("muon_perigee_error_theta0",  "error in theta_{0} on muon track", 10,0.,1.));
	  sc=muontracks_debug.regHist(muon_perigee_error_theta0.back());                                                      //add each histogram to this group
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "m_muon_error_theta0 Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_perigee_error_qOverP.push_back(new TH1F("muon_perigee_error_qOverP",  "error in qOverP on muon track", 50,-1.,1.));
	  sc=muontracks_debug.regHist(muon_perigee_error_qOverP.back());                                              
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "m_muon_error_qoverp Failed to register histogram " << endreq;       
	      return sc;
	    } 


	  muon_rec_locX.push_back(new TH1F("muon_rec_locX","locX of Reconstructed Track",100,-50.,50.));
	  sc=muontracks_debug.regHist(muon_rec_locX.back());
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_locX Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_error_locX.push_back(new TH1F("muon_rec_error_locX","sigma(locX) of Reconstructed Track",100,0.,1.));
	  sc=muontracks_debug.regHist(muon_rec_error_locX.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_error_locX Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_locY.push_back(new TH1F("muon_rec_locY","locY of Reconstructed Track",100,-50.,50.));
	  sc=muontracks_debug.regHist(muon_rec_locY.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_locY Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_error_locY.push_back(new TH1F("muon_rec_error_locY","sigma(locY) of Reconstructed Track",100,0.,1.));
	  sc=muontracks_debug.regHist(muon_rec_error_locY.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_error_locY Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_locR.push_back(new TH1F("muon_rec_locR","locR of Reconstructed Track",100, -50.,50.));
	  sc=muontracks_debug.regHist(muon_rec_locR.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_locR Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_error_locR.push_back(new TH1F("muon_rec_error_locR","sigma(locR) of Reconstructed Track",100,0.,1.));
	  sc=muontracks_debug.regHist(muon_rec_error_locR.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_error_locR Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_phi.push_back(new TH1F("muon_rec_phi","Phi of Reconstructed Track",40,-4.,4.));
	  sc=muontracks_debug.regHist(muon_rec_phi.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_phi Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_error_phi.push_back(new TH1F("muon_rec_error_phi","sigma(Phi) of Reconstructed Track",20,0.,100.));
	  sc=muontracks_debug.regHist(muon_rec_error_phi.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_phi Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_theta.push_back(new TH1F("muon_rec_theta","Theta of Reconstructed Track",40,-4.,4.));
	  sc=muontracks_debug.regHist(muon_rec_theta.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_theta Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_error_theta.push_back(new TH1F("muon_rec_error_theta","sigma(Theta) of Reconstructed Track",10,-1.,1.));
	  sc=muontracks_debug.regHist(muon_rec_error_theta.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_error_theta Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_qOverP.push_back(new TH1F("muon_rec_qOverP", "qOverP of Reconstructed Track",500,-10.,10.));
	  sc=muontracks_debug.regHist(muon_rec_qOverP.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_qOverP Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_error_qOverP.push_back(new TH1F("muon_rec_error_qOverP", "sigma(qOverP) of Reconstructed Track",500,-10.,10.));
	  sc=muontracks_debug.regHist(muon_rec_error_qOverP.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_error_qOverP Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_posX.push_back(new TH1F("muon_rec_posX","posX of Reconstructed Track",120,0.,12000.));
	  sc=muontracks_debug.regHist(muon_rec_posX.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_posX Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_posY.push_back(new TH1F("muon_rec_posY","posY of Reconstructed Track",120,0.,12000.));
	  sc=muontracks_debug.regHist(muon_rec_posY.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_posY Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_posZ.push_back(new TH1F("muon_rec_posZ","posZ of Reconstructed Track",240,-12000.,12000.));
	  sc=muontracks_debug.regHist(muon_rec_posZ.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_posZ Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_rec_posR.push_back(new TH1F("muon_rec_posR","posR of Reconstructed Track",120,0.,12000.));
	  sc=muontracks_debug.regHist(muon_rec_posR.back()); 
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "trk_rec_posR Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  /** Residual Histograms, per detector technology*/

	  muon_glbl_residual.push_back(new TH1F("muon_glbl_residual","All_Residuals; Hit to track distance",100,-5.,5.));
	  sc=muontracks_debug.regHist(muon_glbl_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "All_Residuals Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_MDT_residual.push_back(new TH1F("muon_MDT_residual","MDT_Residuals;Hit to track distance",200,-5.,5.));
	  sc=muontracks_debug.regHist(muon_MDT_residual.back());              
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT_Residuals Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_RPC_residual.push_back(new TH1F("muon_RPC_residual","RPC_Residuals;Hit to track distance",100,-20.,20.));
	  sc=muontracks_debug.regHist(muon_RPC_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RPC_Residuals Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_TGC_residual.push_back(new TH1F("muon_TGC_residual","Residuals;Hit to track distance",150,-100.,100.));
	  sc=muontracks_debug.regHist(muon_TGC_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "TGC_Residuals Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_CSC_residual.push_back(new TH1F("muon_CSC_residual","Residuals;Hit to track distance",100,-20.,20.));
	  sc=muontracks_debug.regHist(muon_CSC_residual.back());              
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "CSC_Residuals Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  muon_RPC_eta_residual.push_back(new TH1F("muon_RPC_eta_residual","RPC #eta residuals;Hit to track distance",100,-20.,20.));
	  sc=muontracks_debug.regHist(muon_RPC_eta_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RPC_Eta_Res Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_RPC_phi_residual.push_back(new TH1F("muon_RPC_phi_residual","RPC #phi residuals;Hit to track distance",100,-20.,20.));
	  sc=muontracks_debug.regHist(muon_RPC_phi_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RPC_Phi_Res Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_TGC_eta_residual.push_back(new TH1F("muon_TGC_eta_residual","TGC #eta residuals;Hit to track distance",100,-100.,100.));
	  sc=muontracks_debug.regHist(muon_TGC_eta_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "TGC_Eta_Res Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_TGC_phi_residual.push_back(new TH1F("muon_TGC_phi_residual","TGC #phi residuals;Hit to track distance",150,-100.,100.));
	  sc=muontracks_debug.regHist(muon_TGC_phi_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "TGC_Phi_Res Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_CSC_phi_residual.push_back(new TH1F("muon_CSC_phi_residual","CSC #phi residuals;Hit to track distance",100,-20.,20.));
	  sc=muontracks_debug.regHist(muon_CSC_phi_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "CSC_Phi_Res Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_CSC_eta_residual.push_back(new TH1F("muon_CSC_eta_residual","CSC #eta residuals;Hit to track distance",100,-20.,20.));
	  sc=muontracks_debug.regHist(muon_CSC_eta_residual.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "CSC_Eta_Res Failed to register histogram " << endreq;       
	      return sc;
	    }

	  /** Pull Histograms, per detector technology*/

	  muon_glbl_pull.push_back(new TH1F("muon_glbl_pull","All_Pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_glbl_pull.back());           
	  if(sc.isFailure()){
	    m_log << MSG::FATAL << "All_Pull Failed to register histogram " << endreq;       
	    return sc;
	  } 

	  muon_MDT_pull.push_back(new TH1F("muon_MDT_pull","MDT_Pull",200,-10.,10.));
	  sc=muontracks_debug.regHist(muon_MDT_pull.back());               
	  if(sc.isFailure()){
	    m_log << MSG::FATAL << "MDT_Pull Failed to register histogram " << endreq;       
	    return sc;
	  } 

	  muon_RPC_pull.push_back(new TH1F("muon_RPC_pull","RPC_Pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_RPC_pull.back());             
	  if(sc.isFailure()){
	    m_log << MSG::FATAL << "RPC_Pull Failed to register histogram " << endreq;       
	    return sc;
	  }

	  muon_TGC_pull.push_back(new TH1F("muon_TGC_pull","TGC_Pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_TGC_pull.back());               
	  if(sc.isFailure()){
	    m_log << MSG::FATAL << "TGC_Pull Failed to register histogram " << endreq;       
	    return sc;
	  } 

	  muon_CSC_pull.push_back(new TH1F("muon_CSC_pull","CSC_Pull ",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_CSC_pull.back());               
	  if(sc.isFailure()){
	    m_log << MSG::FATAL << "CSC_Pull Failed to register histogram " << endreq;       
	    return sc;
	  }

	  muon_RPC_pull_eta.push_back(new TH1F("muon_RPC_pull_eta","RPC #eta pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_RPC_pull_eta.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RPC_Pull_Eta Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_RPC_pull_phi.push_back(new TH1F("muon_RPC_pull_phi","RPC #phi pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_RPC_pull_phi.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RPC_Pull_Phi Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_TGC_pull_eta.push_back(new TH1F("muon_TGC_pull_eta","TGC #eta pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_TGC_pull_eta.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "TGC_Pull_Eta Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_TGC_pull_phi.push_back(new TH1F("muon_TGC_pull_phi","TGC #phi pull",150,-10.,10.));
	  sc=muontracks_debug.regHist(muon_TGC_pull_phi.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "TGC_Pull_Phi Failed to register histogram " << endreq;       
	      return sc;
	    }

	  muon_CSC_pull_eta.push_back(new TH1F("muon_CSC_pull_eta","CSC #eta pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_CSC_pull_eta.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "CSC_Pull_Eta Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_CSC_pull_phi.push_back(new TH1F("muon_CSC_pull_phi","CSC #phi pull",100,-10.,10.));
	  sc=muontracks_debug.regHist(muon_CSC_pull_phi.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "CSC_Pull_Phi Failed to register histogram " << endreq;       
	      return sc;
	    }

	  /** Muon Residuals vrs. pT Histograms */
	  
	  muon_MDT_residual_pT.push_back(new TH2F("muon_MDT_residual_pT","MDT residual vs. p_{T}", 100, 0.0, 100.0, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_pT.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_pT Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_pT_RMS.push_back(new TProfile("muon_MDT_residual_pT_RMS","MDT rsidual vs. p_{T}",100, 0.0, 100.0,-10,10));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_pT_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_pT_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	    
	  
	  /** TH2F: MDT Pull per phi sector, per side for tracks with 3 or more stations*/
	  
	  muon_MDT_residual_BA.push_back(new TH2F("muon_MDT_residual_BA","MDT residual per sector, per layer in Barrel A (track w/3 or more stations)",49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_BA.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BA Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  muon_MDT_residual_BC.push_back(new TH2F("muon_MDT_residual_BC","MDT residual per sector, per layer in Barrel C (track w/3 or more stations)", 49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_BC.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BC Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_EA.push_back(new TH2F("muon_MDT_residual_EA","MDT residual per sector, per layer in EndCap A (track w/3 or more stations)",49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_EA.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EA Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  muon_MDT_residual_EC.push_back(new TH2F("muon_MDT_residual_EC","MDT residual per sector, per layer in EndCap C (track w/3 or more stations)", 49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_EC.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EC Failed to register histogram " << endreq;       
	      return sc;
	    }
	  
	  
	  /** TH2F: MDT Pull per phi sector, per side for tracks with 2 or less stations*/
	  
	  muon_MDT_residual_BA_2stations.push_back(new TH2F("muon_MDT_residual_BA_2stations","MDT residual per sector, per layer in Barrel A (track w/2 or less stations)",49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_BA_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BA_2stations Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  muon_MDT_residual_BC_2stations.push_back(new TH2F("muon_MDT_residual_BC_2stations","MDT residual per sector, per layer in Barrel C (track w/2 or less stations)", 49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_BC_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BC_2stations Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_EA_2stations.push_back(new TH2F("muon_MDT_residual_EA_2stations","MDT residual per sector, per layer in EndCap A (track w/2 or less stations)",49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_EA_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EA_2stations Failed to register histogram " << endreq;       
	      return sc;
	    } 
	  muon_MDT_residual_EC_2stations.push_back(new TH2F("muon_MDT_residual_EC_2stations","MDT residual per sector, per layer in EndCap C (track w/2 or less stations)", 49, 0.5, 49.5, 100, -10.0, 10.0));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_residual_EC_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EC_2stations Failed to register histogram " << endreq;       
	      return sc;
	    }
	     
	  /** TProfile: MDT Pull per phi sector, per side*/
	    
	  muon_MDT_residual_BA_fit.push_back(new TH1F("muon_MDT_residual_BA_fit","MDT residual mean, #sigma per sector, per layer in Barrel A", 49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_Norm.regHist(muon_MDT_residual_BA_fit.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BA_fit Failed to register histogram " << endreq;       
	      return sc;
	    }  
	  muon_MDT_residual_BC_fit.push_back(new TH1F("muon_MDT_residual_BC_fit","MDT residual mean, #sigma per sector, per layer in Barrel C", 49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_Norm.regHist(muon_MDT_residual_BC_fit.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BC_fit Failed to register histogram " << endreq;       
	      return sc;
	    }  
	  muon_MDT_residual_EA_fit.push_back(new TH1F("muon_MDT_residual_EA_fit","MDT residual mean, #sigma per sector, per layer in EndCap A", 49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_Norm.regHist(muon_MDT_residual_EA_fit.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EA_fit Failed to register histogram " << endreq;       
	      return sc;
	    }  
	  muon_MDT_residual_EC_fit.push_back(new TH1F("muon_MDT_residual_EC_fit","MDT residual mean, #sigma per sector, per layer in EndCap C", 49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_Norm.regHist(muon_MDT_residual_EC_fit.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EC_fit Failed to register histogram " << endreq;       
	      return sc;
	    }  
	    
	    
	  muon_MDT_pull_sideA_RMS.push_back(new TProfile("muon_MDT_pull_sideA_RMS","(mean, RMS) MDT pull, per sector, per layer in Side A",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_pull_sideA_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_pull_sideA_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_pull_sideC_RMS.push_back(new TProfile("muon_MDT_pull_sideC_RMS","(mean, RMS) MDT pull, per sector, per layer in Side C",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_debug.regHist(muon_MDT_pull_sideC_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_pull_sideC_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }

	  /* MDT residuals per sector */

	  muon_MDT_residual_sideA_RMS.push_back(new TProfile("muon_MDT_residual_sideA_RMS","(mean, RMS) MDT residual, per sector, per layer in Side A",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_sideA_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_sideA_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_sideC_RMS.push_back(new TProfile("muon_MDT_residual_sideC_RMS","(mean, RMS) MDT residual, per sector, per layer in Side C",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_sideC_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_sideC_RMS Failed to register histogram " << endreq;       
	      return sc;
	    } 


	  /* mdt residuals plots for tracks with 2 or less stations */
	  
	  muon_MDT_residual_BA_RMS_2stations.push_back(new TProfile("muon_MDT_residual_BA_RMS_2stations","(mean, RMS) MDT residual, per sector, per layer in Barrel-A",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_BA_RMS_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BA_RMS_2stations Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_BC_RMS_2stations.push_back(new TProfile("muon_MDT_residual_BC_RMS_2stations","(mean, RMS) MDT residual, per sector, per layer in Barrel-C",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_BC_RMS_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BC_RMS_2stations Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_EA_RMS_2stations.push_back(new TProfile("muon_MDT_residual_EA_RMS_2stations","(mean, RMS) MDT residual, per sector, per layer in EndCap-A",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_EA_RMS_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EA_RMS_2stations Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_EC_RMS_2stations.push_back(new TProfile("muon_MDT_residual_EC_RMS_2stations","(mean, RMS) MDT residual, per sector, per layer in EndCap-C",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_EC_RMS_2stations.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EC_RMS_2stations Failed to register histogram " << endreq;       
	      return sc;
	    }
          
	  /* monitoring MDT residuals plots*/

	  muon_MDT_residual_BA_RMS.push_back(new TProfile("muon_MDT_residual_BA_RMS","(mean, RMS) MDT residual, per sector, per layer in Barrel-A",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_BA_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BA_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_BC_RMS.push_back(new TProfile("muon_MDT_residual_BC_RMS","(mean, RMS) MDT residual, per sector, per layer in Barrel-C",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_BC_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_BC_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_EA_RMS.push_back(new TProfile("muon_MDT_residual_EA_RMS","(mean, RMS) MDT residual, per sector, per layer in EndCap-A",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_EA_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EA_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_EC_RMS.push_back(new TProfile("muon_MDT_residual_EC_RMS","(mean, RMS) MDT residual, per sector, per layer in EndCap-C",49, 0.5, 49.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_MDT_residual_EC_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_MDT_residual_EC_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }

	  /* RPC residuals per sector */

	  muon_RPC_eta_residual_RMS.push_back(new TProfile("muon_RPC_eta_residual_RMS","(mean, RMS) RPC-eta residual, per sector", 33, -16.5, 16.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_RPC_eta_residual_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_RPC_eta_residual_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_RPC_phi_residual_RMS.push_back(new TProfile("muon_RPC_phi_residual_RMS","(mean, RMS) RPC-phi residual, per sector", 33, -16.5, 16.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_RPC_phi_residual_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_RPC_phi_residual_RMS Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  /* TGC residuals per sector */

	  muon_TGC_residual_wire_RMS.push_back(new TProfile("muon_TGC_residual_wire_RMS","(mean, RMS) TGC-wire residual, per wheel", 17, -8.5, 8.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_TGC_residual_wire_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_TGC_residual_wire_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_TGC_residual_strip_RMS.push_back(new TProfile("muon_TGC_residual_strip_RMS","(mean, RMS) TGC-strip residual, per wheel", 17, -8.5, 8.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_TGC_residual_strip_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_TGC_residual_strip_RMS Failed to register histogram " << endreq;       
	      return sc;
	    } 

	  /* CSC residuals per sector */

	  muon_CSC_eta_residual_RMS.push_back(new TProfile("muon_CSC_eta_residual_RMS","(mean, RMS) CSC-eta residual, per sector", 33, -16.5, 16.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_CSC_eta_residual_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_CSC_eta_residual_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_CSC_phi_residual_RMS.push_back(new TProfile("muon_CSC_phi_residual_RMS","(mean, RMS) CSC-phi residual, per sector", 33, -16.5, 16.5));
	  sc=muontracks_MDTresidual_shift.regHist(muon_CSC_phi_residual_RMS.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_CSC_phi_residual_RMS Failed to register histogram " << endreq;       
	      return sc;
	    }   
	    
	  /** TProfile: TGC Residuals vs phi sector*/
	  
	  muon_TGC_residual_EndCapWire_phi.push_back(new TProfile("muon_TGC_residual_EndCapWire_phi","(mean, RMS) TGC-wire residual vs. phi station (EndCap) ", 97, -48.5, 48.5));
	  sc=muontracks_MDTresidual_debug.regHist(muon_TGC_residual_EndCapWire_phi.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_TGC_residual_EndCapWire_phi Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_TGC_residual_ForwardWire_phi.push_back(new TProfile("muon_TGC_residual_ForwardWire_phi","(mean, RMS) TGC-wire residual vs. phi station (Forward)", 49, -24.5, 24.5));
	  sc=muontracks_MDTresidual_debug.regHist(muon_TGC_residual_ForwardWire_phi.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_TGC_residual_ForwardWire_phi Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_TGC_residual_EndCapStrip_phi.push_back(new TProfile("muon_TGC_residual_EndCapStrip_phi","(mean, RMS) TGC-strip residual vs. phi station (EndCap)", 97, -48.5, 48.5));
	  sc=muontracks_MDTresidual_debug.regHist(muon_TGC_residual_EndCapStrip_phi.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_TGC_residual_EndCapStrip_phi Failed to register histogram " << endreq;       
	      return sc;
	    }  
	  muon_TGC_residual_ForwardStrip_phi.push_back(new TProfile("muon_TGC_residual_ForwardStrip_phi","(mean, RMS) TGC-strip residual vs. phi station (Forward)", 49, -24.5, 24.5));
	  sc=muontracks_MDTresidual_debug.regHist(muon_TGC_residual_ForwardStrip_phi.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "muon_TGC_residual_ForwardStrip_phi Failed to register histogram " << endreq;       
	      return sc;
	    }    	   
	       	  
	  /** TProfile: MDT Residuals per phi sector*/

	  muon_MDT_residual_barrel_SectorPhi1.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi1","MDT residuals Barrel, SectorPhi1 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi1.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi1 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_barrel_SectorPhi2.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi2","MDT residuals Barrel, SectorPhi2 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi2.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi2 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_barrel_SectorPhi3.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi3","MDT residuals Barrel, SectorPhi3 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi3.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi3 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_barrel_SectorPhi4.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi4","MDT residuals Barrel, SectorPhi4 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi4.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi4 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_barrel_SectorPhi5.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi5","MDT residuals Barrel, SectorPhi5 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi5.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi5 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_barrel_SectorPhi6.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi6","MDT residuals Barrel, SectorPhi6 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi6.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi6 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_barrel_SectorPhi7.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi7","MDT residuals Barrel, SectorPhi7 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi7.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi7 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_barrel_SectorPhi8.push_back(new TProfile("muon_MDT_residual_barrel_SectorPhi8","MDT residuals Barrel, SectorPhi8 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_barrel.regHist(muon_MDT_residual_barrel_SectorPhi8.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Barrel, SectorPhi8 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi1.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi1","MDT residuals Eca, SectorPhi1 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi1.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi1 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi2.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi2","MDT residuals Eca, SectorPhi2 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi2.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi2 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi3.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi3","MDT residuals Eca, SectorPhi3 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi3.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi3 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi4.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi4","MDT residuals Eca, SectorPhi4 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi4.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi4 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi5.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi5","MDT residuals Eca, SectorPhi5 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi5.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi5 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi6.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi6","MDT residuals Eca, SectorPhi6 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi6.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi6 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi7.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi7","MDT residuals Eca, SectorPhi7 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi7.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi7 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_eca_SectorPhi8.push_back(new TProfile("muon_MDT_residual_eca_SectorPhi8","MDT residuals Eca, SectorPhi8 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_eca.regHist(muon_MDT_residual_eca_SectorPhi8.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Eca, SectorPhi8 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi1.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi1","MDT residuals Ecc, SectorPhi1 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi1.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi1 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi2.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi2","MDT residuals Ecc, SectorPhi2 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi2.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi2 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi3.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi3","MDT residuals Ecc, SectorPhi3 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi3.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi3 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi4.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi4","MDT residuals Ecc, SectorPhi4 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi4.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi4 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi5.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi5","MDT residuals Ecc, SectorPhi5 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi5.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi5 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi6.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi6","MDT residuals Ecc, SectorPhi6 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi6.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi6 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi7.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi7","MDT residuals Ecc, SectorPhi7 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi7.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi7 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residual_ecc_SectorPhi8.push_back(new TProfile("muon_MDT_residual_ecc_SectorPhi8","MDT residuals Ecc, SectorPhi8 (S & L)",1, 0, 1));
	  sc=muontracks_residuals_ecc.regHist(muon_MDT_residual_ecc_SectorPhi8.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "MDT residuals Ecc, SectorPhi8 Failed to register histogram " << endreq;       
	      return sc;
	    }

	  /** TProfile MDT RMS from Residuals TProfiles per phi sector*/

	  muon_MDT_residualRMS_barrel_SectorPhi1.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi1","RMS of MDT residuals Barrel, SectorPhi1",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi1.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi1 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_barrel_SectorPhi2.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi2","RMS of MDT residuals Barrel, SectorPhi2",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi2.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi2 Failed to register histogram " << endreq;       
	      return sc;
	    }	  
	  muon_MDT_residualRMS_barrel_SectorPhi3.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi3","RMS of MDT residuals Barrel, SectorPhi3",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi3.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi3 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_barrel_SectorPhi4.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi4","RMS of MDT residuals Barrel, SectorPhi4",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi4.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi4 Failed to register histogram " << endreq;       
	      return sc;
	    }	
	  muon_MDT_residualRMS_barrel_SectorPhi5.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi5","RMS of MDT residuals Barrel, SectorPhi5",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi5.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi5 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_barrel_SectorPhi6.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi6","RMS of MDT residuals Barrel, SectorPhi6",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi6.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi6 Failed to register histogram " << endreq;       
	      return sc;
	    }	  
	  muon_MDT_residualRMS_barrel_SectorPhi7.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi7","RMS of MDT residuals Barrel, SectorPhi7",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi7.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi7 Failed to register histogram " << endreq;       
	      return sc;
	    }	  
	  muon_MDT_residualRMS_barrel_SectorPhi8.push_back(new TH1F("muon_MDT_residualRMS_barrel_SectorPhi8","RMS of MDT residuals Barrel, SectorPhi8",10, 0, 10));
	  sc=muontracks_residuals_barrel_Norm.regHist(muon_MDT_residualRMS_barrel_SectorPhi8.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Barrel, SectorPhi8 Failed to register histogram " << endreq;       
	      return sc;
	    }	  	  

	  muon_MDT_residualRMS_eca_SectorPhi1.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi1","RMS of MDT residuals Eca, SectorPhi1",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi1.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi1 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_eca_SectorPhi2.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi2","RMS of MDT residuals Eca, SectorPhi2",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi2.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi2 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_eca_SectorPhi3.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi3","RMS of MDT residuals Eca, SectorPhi3",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi3.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi3 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_eca_SectorPhi4.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi4","RMS of MDT residuals Eca, SectorPhi4",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi4.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi4 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_eca_SectorPhi5.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi5","RMS of MDT residuals Eca, SectorPhi5",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi5.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi5 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_eca_SectorPhi6.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi6","RMS of MDT residuals Eca, SectorPhi6",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi6.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi6 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_eca_SectorPhi7.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi7","RMS of MDT residuals Eca, SectorPhi7",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi7.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi7 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_eca_SectorPhi8.push_back(new TH1F("muon_MDT_residualRMS_eca_SectorPhi8","RMS of MDT residuals Eca, SectorPhi8",10, 0, 10));
	  sc=muontracks_residuals_eca_Norm.regHist(muon_MDT_residualRMS_eca_SectorPhi8.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Eca, SectorPhi8 Failed to register histogram " << endreq;       
	      return sc;
	    }	

	  muon_MDT_residualRMS_ecc_SectorPhi1.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi1","RMS of MDT residuals Ecc, SectorPhi1",10, 0, 10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi1.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi1 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_ecc_SectorPhi2.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi2","RMS of MDT residuals Ecc, SectorPhi2",10, 0, 10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi2.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi2 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_ecc_SectorPhi3.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi3","RMS of MDT residuals Ecc, SectorPhi3",10, 0, 10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi3.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi3 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_ecc_SectorPhi4.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi4","RMS of MDT residuals Ecc, SectorPhi4",10, 0, 10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi4.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi4 Failed to register histogram " << endreq;       
	      return sc;
	    }
	  muon_MDT_residualRMS_ecc_SectorPhi5.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi5","RMS of MDT residuals Ecc, SectorPhi5",10, 0, 10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi5.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi5 Failed to register histogram " << endreq;       
	      return sc;
	    }	
	  muon_MDT_residualRMS_ecc_SectorPhi6.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi6","RMS of MDT residuals Ecc, SectorPhi6",10, 0, 10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi6.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi6 Failed to register histogram " << endreq;       
	      return sc;
	    }	  
	  muon_MDT_residualRMS_ecc_SectorPhi7.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi7","RMS of MDT residuals Ecc, SectorPhi7",10, 0, 10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi7.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi7 Failed to register histogram " << endreq;       
	      return sc;
	    }	  
	  muon_MDT_residualRMS_ecc_SectorPhi8.push_back(new TH1F("muon_MDT_residualRMS_ecc_SectorPhi8","RMS of MDT residuals Ecc, SectorPhi8",10, 0,10));
	  sc=muontracks_residuals_ecc_Norm.regHist(muon_MDT_residualRMS_ecc_SectorPhi8.back());               
	  if(sc.isFailure())
	    { m_log << MSG::FATAL << "RMS MDT residuals Ecc, SectorPhi8 Failed to register histogram " << endreq;       
	      return sc;
	    }

	} //  if NewRun

    } // loop over Muon Track collections
    //  } // loop over Muon Triggers
  
  } //Environment
  
  return sc;
  
}

/** fillHistograms */
//---------------------------------------------------------------------------------------
StatusCode MuonGenericTracksMon::fillHistograms()
{
     
  m_log << MSG::DEBUG << "******************************************************************************" << endreq;
  m_log << MSG::DEBUG << "                          INSIDE fillHistograms()                             " << endreq;
  m_log << MSG::DEBUG << "******************************************************************************" << endreq;
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {

    // Set the event counter
    // m_eventNumber = m_eventCounter;

    /** Checking if event passes muon triggers */

    bool useMuonTrigger = ( ( m_muon_triggers.size() != 0 ) && ( ! m_trigDecTool.empty() ) );
    bool passesMuonTrigger = false;
    if( useMuonTrigger && m_useTrigger )

      {
	// Loop over the triggers.
	for (unsigned int ii = 0; ii < m_muon_triggers.size(); ii++ )
	  {
	    if (m_debuglevel) m_log << MSG::DEBUG << "Checking trigger " << m_muon_triggers[ii] << endreq;
	    if( m_trigDecTool->isPassed(m_muon_triggers[ii]) )
	      {
		if (m_debuglevel) m_log << MSG::DEBUG << "Fired trigger "<< m_muon_triggers[ii]<<endreq;
		passesMuonTrigger = true;
		break; 
	      }
	  }
	if(!passesMuonTrigger){ if (m_debuglevel) m_log << MSG::DEBUG << "No Trigger Fired!" << endreq; }
      } 
    if(!m_useTrigger) passesMuonTrigger = true;

    if(passesMuonTrigger)
      { //Once it has passed Muontrigger or not using muon trigger

	////////////////////////Tracks//////////////////////////////////
	//Get tracks from SG
	//  bool got_coll = false;
	bool got_coll = true;

	int h_index = -1;
	for (int j=0;j<(int)m_trackCollectionName.size();++j) {

	  if (m_debuglevel) 
	    m_log << MSG::DEBUG << "Track collection FLAG is " << m_trackCollectionFlag[j] << endreq;

	  if ( m_trackCollectionFlag[j] == 0 ) 
	    continue;

	  ++h_index;

	  StatusCode sc = m_storeGate->retrieve(m_tracks, m_trackCollectionName[j]);

	  if (m_debuglevel)  m_log << MSG::DEBUG << "Track collection Name is " << m_trackCollectionName[j] << endreq;

	  if ( sc.isFailure() ){
	    if ( (j == (int)m_trackCollectionName.size()-1) && !got_coll)
	      {
		ATH_MSG_WARNING("Unable to retrieve reconstructed tracks from any collection... Exiting!" );  
		return StatusCode::SUCCESS; 
	      } else {
	      ATH_MSG_WARNING("Unable to retrieve reconstructed tracks from collection... Trying next...");
	      continue;
	    }
	    got_coll = false;
	  } else{
	    if (m_debuglevel){m_log <<MSG::DEBUG <<"Tracks in StoreGate found" <<endreq;
	      m_log << MSG::DEBUG << "Track collection Name is " << m_trackCollectionName[j] << endreq;}
	    got_coll = true;
	  }

	  if ( m_tracks->empty() ){
	    if (m_debuglevel){
	      m_log << MSG::DEBUG <<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<< endreq;
	      m_log << MSG::DEBUG<<"      Track Collection is Empty.  Trying next...    "<<endreq;
	      m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
	    }
	    continue;
	  }

	  if(!got_coll) continue; // protection

	  if (m_debuglevel){
	    m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
	    m_log << MSG::DEBUG<<"      Track Collection size         " << endreq;
	    m_log << MSG::DEBUG << "Number of tracks found: " << m_tracks->size() << endreq;
	    m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;}

	  if ( m_tracks->size() > 0 && m_log.level() <= MSG::VERBOSE ){
	    m_log << MSG::DEBUG << "********************************* WARNING ************************************" << endreq;
	    m_log << MSG::DEBUG << "Number of tracks in event is: " << m_tracks->size()                      << endreq;
	    m_log << MSG::DEBUG << "This algorithm is designed to operate for single track / event only"            << endreq;
	    m_log << MSG::DEBUG << "Processing only the first track in the track collection"                        << endreq;
	    m_log << MSG::DEBUG << "******************************************************************************" << endreq;
	  }


	  /** Loop over tracks */
	  for (int i=0; i<(int)m_tracks->size(); i++){
	    // Get track
	    const Trk::Track *track=(*m_tracks)[i];
	    if (track == 0) {
	      m_log << MSG::ERROR << "no pointer to track!!!" << endreq;
	      break;
	    }

	    /** Fill track parameters at perigee */

	    const  Trk::Perigee *measPerigee = dynamic_cast< const Trk::Perigee *>(track->perigeeParameters());
	    if(!measPerigee){continue;}
	    Amg::Vector3D pos = measPerigee->position();
	    Amg::Vector3D direction = measPerigee->momentum();

	    /** Dealing with SL tracks.  */
	    
	    float measPerigee_p_T = 0.0;
	    float measPerigee_p = 0.0;
	    float measPerigee_q_over_p = 0.0;
	    
	    if(m_helperTool->isSLTrack(*track)){
	      if (m_debuglevel) m_log << MSG::DEBUG << "***  We have a SL Track (GenericTrkMon)***" << endreq;
	      measPerigee_p_T = -10.0;
	      measPerigee_p = -10.0;
	      measPerigee_q_over_p = measPerigee->charge();
	      //continue; 
	    }  else{ 
	      measPerigee_p_T = measPerigee->pT()/1000.;
	      measPerigee_q_over_p = measPerigee->parameters()[Trk::qOverP]*1000.;
	      if ((measPerigee->parameters()[Trk::qOverP]) != 0) measPerigee_p = fabs(1. / (measPerigee->parameters()[Trk::qOverP] * 1000.));
	    }
	    
	    muon_rec_pT[h_index]->Fill(measPerigee_p_T);
	    if (measPerigee->charge() > 0) muon_rec_pos_pT[h_index]->Fill(measPerigee_p_T);
	    if (measPerigee->charge() < 0) muon_rec_neg_pT[h_index]->Fill(measPerigee_p_T);  
	    
	    if (measPerigee_p_T > 100.0) {
	      // "High pT" muons
	      muon_rec_HighpT[h_index]->Fill(measPerigee_p_T);
	      if (measPerigee->charge() > 0) muon_rec_pos_HighpT[h_index]->Fill(measPerigee_p_T);
	      if (measPerigee->charge() < 0) muon_rec_neg_HighpT[h_index]->Fill(measPerigee_p_T); 	  
	    }
	    
	    muon_rec_1overpT[h_index]->Fill(1./measPerigee_p_T);             
	    muon_rec_charge[h_index]->Fill(measPerigee->charge());        
	    muon_perigee_qOverP[h_index]->Fill(measPerigee_q_over_p);
	    
	    if ((measPerigee->parameters()[Trk::qOverP]) != 0) muon_perigee_P[h_index]->Fill(measPerigee_p);
	    muon_perigee_d0[h_index]->Fill(measPerigee->parameters()[Trk::d0]);
	    muon_perigee_z0[h_index]->Fill(measPerigee->parameters()[Trk::z0]);
	    muon_perigee_phi0[h_index]->Fill(RadsToDegrees(measPerigee->parameters()[Trk::phi0]));
	    muon_perigee_theta[h_index]->Fill(measPerigee->parameters()[Trk::theta]);
	    muon_perigee_eta[h_index]->Fill(measPerigee->eta());
	    muon_rec_eta_phi[h_index]->Fill(RadsToDegrees(measPerigee->parameters()[Trk::phi0]), measPerigee->eta());

	    muon_rec_pT_vs_eta[h_index]->Fill( measPerigee->eta() ,measPerigee_p_T );
	    muon_rec_pT_vs_phi[h_index]->Fill( RadsToDegrees(measPerigee->parameters()[Trk::phi0]) , measPerigee_p_T );
	    
	    muon_rec_charge_vs_eta[h_index]->Fill( measPerigee->eta(), measPerigee->charge());
	    muon_rec_charge_vs_phi[h_index]->Fill( RadsToDegrees(measPerigee->parameters()[Trk::phi0]), measPerigee->charge());
	    
	    ///++++ pT histograms to understand charge asymmetry 
	    
	    float phiStation = RadsToDegrees(measPerigee->parameters()[Trk::phi0]);
	    
	    if (m_debuglevel) m_log << MSG::DEBUG << "Trk Perigee, phi: " << RadsToDegrees(measPerigee->parameters()[Trk::phi0]) << endreq;

	    if ((phiStation > 348.75) && (phiStation < 11.25) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 01"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector01[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector01[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 11.25) && (phiStation < 33.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 02"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector02[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector02[h_index]->Fill(measPerigee_p_T );
            }
	    else if (((phiStation > 33.75) && (phiStation < 56.25) ) || ((phiStation > 0.0) && (phiStation < 22.5) ) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 03"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector03[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector03[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 56.25) && (phiStation < 78.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 04"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector04[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector04[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 78.75) && (phiStation < 101.25) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 05"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector05[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector05[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 101.25) && (phiStation < 123.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 06"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector06[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector06[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 123.75) && (phiStation < 146.25) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 07"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector07[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector07[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 146.21) && (phiStation < 168.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 08"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector08[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector08[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 168.75) && (phiStation < 191.25) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 09"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector09[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector09[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 191.25) && (phiStation < 213.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 10"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector10[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector10[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation >213.75) && (phiStation < 236.25) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 11"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector11[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector11[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 236.25) && (phiStation < 258.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 12"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector12[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector12[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 258.75) && (phiStation < 281.25) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 13"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector13[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector13[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 281.25) && (phiStation < 303.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 14"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector14[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector14[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 303.75) && (phiStation < 326.25) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 15"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector15[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector15[h_index]->Fill(measPerigee_p_T );
            }
	    else if ((phiStation > 326.25) && (phiStation < 348.75) ) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Sector 16"<< endreq;
	      if (measPerigee->charge() > 0)  muon_rec_pos_pT_Sector16[h_index]->Fill(measPerigee_p_T );
	      else if (measPerigee->charge() < 0)  muon_rec_neg_pT_Sector16[h_index]->Fill(measPerigee_p_T );
            }
	    
	    double m_recomuonpT_pos_up;
	    double m_recomuonpT_pos_low;
	    double m_recomuonpT_neg_up;
	    double m_recomuonpT_neg_low;
	    m_recomuonpT_pos_up=0;
	    m_recomuonpT_pos_low=0.;
	    m_recomuonpT_neg_up=0.;
	    m_recomuonpT_neg_low=0.;
	    
	    if (phiStation < 180.0) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Up"<< endreq;
	      if (measPerigee->charge() > 0) { 
		muon_rec_pos_pT_Up[h_index]->Fill(measPerigee_p_T ); 
		m_recomuonpT_pos_up=measPerigee_p_T; 
	      }
	      else if (measPerigee->charge() < 0) 
		{   
		  muon_rec_neg_pT_Up[h_index]->Fill(measPerigee_p_T );	    
		  m_recomuonpT_neg_up=measPerigee_p_T;
		}
	    } else if(measPerigee->eta() < 0.) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Down"<< endreq;
	      if (measPerigee->charge() > 0){  
		muon_rec_pos_pT_Down[h_index]->Fill(measPerigee_p_T );
		m_recomuonpT_pos_low=measPerigee_p_T;
	      }
	      else if (measPerigee->charge() < 0)
		{
		  muon_rec_neg_pT_Down[h_index]->Fill(measPerigee_p_T );
		  m_recomuonpT_neg_low=measPerigee_p_T;	    
		}
	    }

	    double m_recomuonpT_pos_sideA;
	    double m_recomuonpT_pos_sideC;
	    double m_recomuonpT_neg_sideA;
	    double m_recomuonpT_neg_sideC;
	    m_recomuonpT_pos_sideA=0.;
	    m_recomuonpT_pos_sideC=0.;
	    m_recomuonpT_neg_sideA=0.;
	    m_recomuonpT_neg_sideC=0.;
	    	    
	    if (measPerigee->eta() > 0.) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Side A"<< endreq;
	      if (measPerigee->charge() > 0)  
		{
		  muon_rec_pos_pT_A[h_index]->Fill(measPerigee_p_T );
		  m_recomuonpT_pos_sideA=measPerigee_p_T;
		}
	      else if (measPerigee->charge() < 0) 
		{
		  muon_rec_neg_pT_A[h_index]->Fill(measPerigee_p_T );	    
		  m_recomuonpT_neg_sideA=measPerigee_p_T;
		}
	    } else if(measPerigee->eta() < 0.) {
	      if (m_debuglevel) m_log << MSG::DEBUG << "where's the trk?: Side C"<< endreq;
	      if (measPerigee->charge() > 0) {
		muon_rec_pos_pT_C[h_index]->Fill(measPerigee_p_T );
		m_recomuonpT_pos_sideC=measPerigee_p_T;
	      }
	      else if (measPerigee->charge() < 0) {
		muon_rec_neg_pT_C[h_index]->Fill(measPerigee_p_T );	    
		m_recomuonpT_neg_sideC=measPerigee_p_T;
	      }
	    }	    

	    if (m_debuglevel) m_log << MSG::DEBUG << "diff POS muon trk?: Up-Down"<< endreq;
	    muon_rec_pos_pT_up_dw_diff[h_index]->Fill(m_recomuonpT_pos_up-m_recomuonpT_pos_low);
	    muon_rec_neg_pT_up_dw_diff[h_index]->Fill(m_recomuonpT_neg_up-m_recomuonpT_neg_low);
	    muon_rec_pos_pT_A_C_diff[h_index]->Fill(m_recomuonpT_pos_sideA-m_recomuonpT_pos_sideC);
	    muon_rec_neg_pT_A_C_diff[h_index]->Fill(m_recomuonpT_neg_sideA-m_recomuonpT_neg_sideC);

	    //++++ pT histograms to understand charge asymmetry
 
	    float _chi2oDoF = -1.0;
	    if( track->fitQuality() ){       
	      muon_rec_chi2[h_index]->Fill(track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF());
	      muon_rec_DegreesOfFreedom[h_index]->Fill(track->fitQuality()->numberDoF());
	      
	      _chi2oDoF =  track->fitQuality()->chiSquared()/track->fitQuality()->numberDoF();
	      
	    }
	    float errP = -999.;

	    if( measPerigee ) errP = std::sqrt( (*measPerigee->covariance())(Trk::d0,Trk::d0)+ (*measPerigee->covariance())(Trk::z0,Trk::z0) ); 
	    if (m_debuglevel){ m_log << MSG::DEBUG << " Muon Track position " << pos << " d0 " << measPerigee->parameters()[Trk::d0] << " z0 " << measPerigee->parameters()[Trk::z0] 
				     << " theta Per " << measPerigee->parameters()[Trk::theta] << " theta " << direction.theta() 
				     <<   " phi Per " << measPerigee->parameters()[Trk::phi0] << " phi " << direction.phi() << endreq;			       
	      m_log << MSG::DEBUG << " Muon Track charge * momentum perigee " << 1/(measPerigee->parameters()[Trk::qOverP]) << " charge mom " << (measPerigee->charge())*direction.mag() << endreq; 
	    }
	    
	    //const Trk::ErrorMatrix ErrorMat=measPerigee->localErrorMatrix();

	    muon_perigee_error_d0[h_index]->Fill( (*measPerigee->covariance())(Trk::d0,Trk::d0) );
	    muon_perigee_error_z0[h_index]->Fill( (*measPerigee->covariance())(Trk::z0,Trk::z0) );
	    muon_perigee_error_phi0[h_index]->Fill( (*measPerigee->covariance())(Trk::phi0,Trk::phi0) );
	    muon_perigee_error_theta0[h_index]->Fill( (*measPerigee->covariance())(Trk::theta,Trk::theta) );
	    muon_perigee_error_qOverP[h_index]->Fill( (*measPerigee->covariance())(Trk::qOverP,Trk::qOverP) );
	    
	    //------------------------------------------------------------------------//
	    // Good Track
	    
	    bool isGoodTrack = true;
	    
	    int _nhmdt = 0;
	    int _nhcsc = 0;
	    
	    /** Access to TrackSummary Information */

	    const Trk::TrackSummary* summary = NULL;       
	    summary = m_trackSumTool->createSummary(*track);
	    if(summary==0) m_log << MSG::WARNING << "Could not create TrackSummary" << endreq;    
	    else{
	      _nhmdt = summary->get(Trk::numberOfMdtHits);	 
	      _nhcsc = summary->get(Trk::numberOfCscPhiHits)+summary->get(Trk::numberOfCscEtaHits);
	    }
	          
	    Trk::MuonTrackSummary muonSummary;
	    if( summary ){
	      if( summary->muonTrackSummary() ) muonSummary = *summary->muonTrackSummary();
	      else{
		Trk::TrackSummary* tmpSum = const_cast<Trk::TrackSummary*>(summary);
		if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(*track,*tmpSum);
		if( tmpSum->muonTrackSummary() ) muonSummary = *tmpSum->muonTrackSummary();
	      }
	    }else{
	      Trk::TrackSummary tmpSummary;
	      m_trackSummaryTool->addDetailedTrackSummary(*track,tmpSummary);
	      if( tmpSummary.muonTrackSummary() ) muonSummary = *tmpSummary.muonTrackSummary();
	    }

	    int _etaHits = muonSummary.netaHits(); 
	    int _phiHits = muonSummary.nphiHits();

	    Muon::IMuonHitSummaryTool::CompactSummary mHitSummary;
	    int trkStations = 0; 
	    if (m_muonHitSummaryTool) {
	      mHitSummary = m_muonHitSummaryTool->summary(*track);   
	
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
	  
	  
	    delete summary;
	   
	    muon_rec_nStations[h_index]->Fill(trkStations);
	    if (trkStations >= 3)
	      muon_rec_eta_phi_3moreStations[h_index]->Fill(RadsToDegrees(measPerigee->parameters()[Trk::phi0]), measPerigee->eta());
	    else muon_rec_eta_phi_2lessStations[h_index]->Fill(RadsToDegrees(measPerigee->parameters()[Trk::phi0]), measPerigee->eta());
	    
	     
	    // NB This cuts have been implemented by hand
	    if (measPerigee_p_T < 10.0) isGoodTrack = false;
	    if (_chi2oDoF > 15.0 )      isGoodTrack = false;
	    if ((_nhmdt+_nhcsc) < 0)    isGoodTrack = false;
	    if (_etaHits < 3)           isGoodTrack = false;
	    if (_phiHits < 2)           isGoodTrack = false; 
	    //if ( trkStations < 3 )      isGoodTrack = false; //not done here!
	    
	    //------------------------------------------------------------------------// 

	    // Get pointer to track state on surfaces -   // Retrieve the states on surfaces from the track
	    const DataVector< const Trk::TrackStateOnSurface>* trackSoS=track->trackStateOnSurfaces();

	    if (trackSoS == 0)
	      {
		m_log << MSG::ERROR << "for current track is TrackStateOnSurfaces == Null, no data will be written for this track" << endreq;
	      } else { 

	      /** Loop over all the state on surface objects in the track */
	      DataVector<const Trk::TrackStateOnSurface>::const_iterator stateOnSurface = trackSoS->begin();

	      if (m_debuglevel) m_log << MSG::DEBUG << "Begin hitLoop over " << trackSoS << " TrkSurfaces" << endreq;

	      unsigned int surfaceIndex = 0;

	      for ( ; stateOnSurface != trackSoS->end(); ++stateOnSurface, ++surfaceIndex ){

		if ((*stateOnSurface) == 0) { m_log << MSG::WARNING << "TrackStateOnSurface == Null, is the tracking OK?" << endreq;}

		// Reset the component number every time a new state on surface occurs
		m_rec_nComponents = 0;
		if (m_debuglevel){m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
		  m_log << MSG::DEBUG<<"           Inside loop over SoS objects            "<<endreq;
		  m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;}

		const Trk::TrackParameters* trackParameters = (*stateOnSurface)->trackParameters();
		if ( !trackParameters ){
		  if (m_debuglevel){m_log << MSG::DEBUG << "Error retrieving track parameters from TrackStateOnSurface object... ignoring TSOS!" << endreq;
		    m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
		    m_log << MSG::DEBUG<<"           Can't find trackParameters              "<<endreq;
		    m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;}
		  continue;
		}

		// check whether state is a measurement
		const Trk::MeasurementBase* meas = (*stateOnSurface)->measurementOnTrack();
		if( !meas ){
		  if (m_debuglevel){m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
		    m_log << MSG::DEBUG<<"           This SoS is not a measurement            "<<endreq;
		    m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;}
		  continue;
		}

		Amg::Vector3D pos = meas->globalPosition();
		float driftRadius(0.);
		if( meas->localParameters().contains(Trk::locX) ){
		  driftRadius = meas->localParameters()[Trk::locX];
		}else if( meas->localParameters().contains(Trk::locY) ){
		  driftRadius = meas->localParameters()[Trk::locY];
		} 
		
		Identifier id;

		// pointer to resPull
		const Trk::ResidualPull* resPull = m_pullCalculator->residualPull(meas, trackParameters, Trk::ResidualPull::Unbiased);
		if( resPull ){
		  const Amg::Vector2D* locPos = meas->associatedSurface().globalToLocal(trackParameters->position());
		  if( !locPos ){
		    if (m_debuglevel){m_log << MSG::DEBUG << " localToGlobal failed !!!!! " << endreq;
		      m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
		      m_log << MSG::DEBUG<<"           localToGlobal failed                    "<<endreq;
		      m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;}
		    continue;
		  } else if (locPos)
		    {

		      //Identifier id = m_helperTool->getIdentifier(*meas);
		      id = m_helperTool->getIdentifier(*meas);
		      std::string idString   = id != Identifier() ? m_idHelperTool->toString( id ) : " Pseudomeasurement "; 
		      std::string boundCheck = meas->associatedSurface().insideBounds(*locPos) ? "inBounds" : "outBounds"; 
		      if (m_debuglevel) m_log << MSG::DEBUG << m_printer->print( *resPull ) << " "  << idString << " " << boundCheck << endreq;
		    }
		  if (locPos) delete locPos;


		  bool measuresPhi = m_idHelperTool->measuresPhi(id);

		  // Calculate Residual for hit: res = (vec(x_track) - vec(x_hit)) * vec(n_perpendicular)	      
		  float residual = resPull->residual().front();
		  if (residual) {
		    float pull = resPull->pull().front();
		    if (m_debuglevel){ 
		      m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
		      m_log << MSG::DEBUG<<"& Found a residual!! : "<<residual<<"&"<<endreq;
		      m_log << MSG::DEBUG<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endreq;
		    }
		    if(!id.is_valid()) {
		      if (m_debuglevel) m_log << MSG::DEBUG<<"Not a valid id. Possibly a pseudo measurement."<<endreq;
		    } else if ( (*stateOnSurface)->type(Trk::TrackStateOnSurface::Outlier) ){
		      if (m_debuglevel) m_log << MSG::DEBUG<<"Measurement flagged as Outlier. Not including it in the ResPull distributions."<<endreq;

		    } else { //Valid measurement

		      if (m_debuglevel) m_log << MSG::DEBUG<<"Valid id"<<endreq;

		      if ( m_idHelperTool->isMdt(id) ){
		      
			muon_MDT_residual[h_index]->Fill(residual);
			muon_MDT_pull[h_index]->Fill(pull);
			   
			muon_MDT_residual_pT[h_index]->Fill(measPerigee_p_T, residual);
			muon_MDT_residual_pT_RMS[h_index]->Fill(measPerigee_p_T, residual);

			int imdtstationPhi = int(m_mdtIdHelper->stationPhi(id));
			int imdtstationEta = int(m_mdtIdHelper->stationEta(id));
			int imdtstationName = int(m_mdtIdHelper->stationName(id));
			std::string type="MDT";
			std::string hardware_name = convertChamberName(imdtstationName,imdtstationEta,imdtstationPhi,type);

			bool isEndCapA=false,isEndCapC=false,isBarrel=false;
			if ( !(m_idHelperTool->isEndcap(id)) ) isBarrel=true;
			else if (measPerigee->eta()>1) isEndCapA=true;
			else if (measPerigee->eta()<-1) isEndCapC=true;
			   
			int MdtStationLayer = -1;
			if (m_debuglevel) { 
			  m_log << MSG::DEBUG<<"stationName: " << hardware_name << "\t" << imdtstationName <<endreq;
			  m_log << MSG::DEBUG<<"TubeLayer: " << int(m_mdtIdHelper->tubeLayer(id))  <<endreq; 
			}
          	
			// Inner stations: BIL:0, BIS:1, BIR:7, BIM:52, EIC:12, EIL:13, FIL:35, FIS:36, EIS:49
			if ( (imdtstationName == 0) ||
			     (imdtstationName == 1)||
			     (imdtstationName == 7)||
			     (imdtstationName ==52)||
			     (imdtstationName ==12)||
			     (imdtstationName ==13)||
			     (imdtstationName ==35)||
			     (imdtstationName ==36)||
			     (imdtstationName ==49) ){
			
			  if(m_idHelperTool->isSmallChamber(id)) MdtStationLayer = 2*imdtstationPhi;   
			  else MdtStationLayer = 2*imdtstationPhi-1;
			
			  // Middle layers: BML:2, BMS:3, BMF:8, EMC:16, EML:17, EMS:18, FML:37, FMS:38
			} else if ( (imdtstationName == 2) ||
			            (imdtstationName == 3) ||
				    (imdtstationName == 8) ||
				    (imdtstationName ==16) ||
				    (imdtstationName ==17) ||
				    (imdtstationName ==18) ||
				    (imdtstationName ==37) ||
				    (imdtstationName ==38)  ){
			  
			  if(m_idHelperTool->isSmallChamber(id)) MdtStationLayer = 16 + 2*imdtstationPhi;
			  else MdtStationLayer = 16 + 2*imdtstationPhi-1;
			
			  // Outer layers: BOL:4, BOS:5, BOF:9, BOG:10, BOH:11, EOL:20, EOS:21 
			} else if ( (imdtstationName == 4) ||
			            (imdtstationName == 5) ||
				    (imdtstationName == 9) ||
				    (imdtstationName ==10) ||
				    (imdtstationName ==11) ||
				    (imdtstationName ==20) ||
				    (imdtstationName ==21)  ){
			  if(m_idHelperTool->isSmallChamber(id)) MdtStationLayer = 32 + 2*imdtstationPhi;
			  else MdtStationLayer = 32 + 2*imdtstationPhi-1;
			
			  // extension layers: BEE:6, EES:15, EEL:14
			} else if ( (imdtstationName == 6) ||
			            (imdtstationName ==14) ||
				    (imdtstationName ==15) ){
			  MdtStationLayer = 49;
			
			}
			
			if (measPerigee->eta()>0){
			  if (MdtStationLayer != -1) {  
			    muon_MDT_pull_sideA_RMS[h_index]     ->Fill(MdtStationLayer, pull); 
			    muon_MDT_residual_sideA_RMS[h_index] ->Fill(MdtStationLayer, residual); 
			    if (isGoodTrack) {
			      if (isBarrel) { 
				if (trkStations>=3) {
				  muon_MDT_residual_BA_RMS[h_index]->Fill(MdtStationLayer, residual); 
				  muon_MDT_residual_BA[h_index]    ->Fill(MdtStationLayer, residual);
				} else {
				  muon_MDT_residual_BA_RMS_2stations[h_index]->Fill(MdtStationLayer, residual); 
				  muon_MDT_residual_BA_2stations[h_index]    ->Fill(MdtStationLayer, residual); 
				}
			      } else {          
				if (trkStations>=3) {
				  muon_MDT_residual_EA_RMS[h_index]->Fill(MdtStationLayer, residual);
				  muon_MDT_residual_EA[h_index]    ->Fill(MdtStationLayer, residual);
				} else {
				  muon_MDT_residual_EA_RMS_2stations[h_index]->Fill(MdtStationLayer, residual);
				  muon_MDT_residual_EA_2stations[h_index]    ->Fill(MdtStationLayer, residual);
				}
			      }
			    }
			  } 
			
			} else {
			  if (MdtStationLayer != -1) { 
			    muon_MDT_pull_sideC_RMS[h_index]     ->Fill(MdtStationLayer, pull); 
			    muon_MDT_residual_sideC_RMS[h_index] ->Fill(MdtStationLayer, residual); 
			    if (isGoodTrack) {
			      if (isBarrel) {
				if (trkStations>=3) {
				  muon_MDT_residual_BC_RMS[h_index]->Fill(MdtStationLayer, residual); 
				  muon_MDT_residual_BC[h_index]    ->Fill(MdtStationLayer, residual);
				} else {
				  muon_MDT_residual_BC_RMS_2stations[h_index]->Fill(MdtStationLayer, residual); 
				  muon_MDT_residual_BC_2stations[h_index]    ->Fill(MdtStationLayer, residual);
				}
			      } else {
				if (trkStations>=3) {
				  muon_MDT_residual_EC_RMS[h_index]->Fill(MdtStationLayer, residual);
				  muon_MDT_residual_EC[h_index]    ->Fill(MdtStationLayer, residual);
				} else {
				  muon_MDT_residual_EC_RMS_2stations[h_index]->Fill(MdtStationLayer, residual);
				  muon_MDT_residual_EC_2stations[h_index]    ->Fill(MdtStationLayer, residual);
				}
			      }
			    }
			  } 
			
			}

			if (isBarrel) {
			  if (m_mdtIdHelper->stationPhi(id)==1) {
			    muon_MDT_residual_barrel_SectorPhi1[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==2) {
			    muon_MDT_residual_barrel_SectorPhi2[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==3) {
			    muon_MDT_residual_barrel_SectorPhi3[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==4) {
			    muon_MDT_residual_barrel_SectorPhi4[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==5) {
			    muon_MDT_residual_barrel_SectorPhi5[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==6) {
			    muon_MDT_residual_barrel_SectorPhi6[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==7) {
			    muon_MDT_residual_barrel_SectorPhi7[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==8) {
			    muon_MDT_residual_barrel_SectorPhi8[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			}

			else if (isEndCapA) {
			  if (m_mdtIdHelper->stationPhi(id)==1) {
			    muon_MDT_residual_eca_SectorPhi1[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==2) {
			    muon_MDT_residual_eca_SectorPhi2[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==3) {
			    muon_MDT_residual_eca_SectorPhi3[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==4) {
			    muon_MDT_residual_eca_SectorPhi4[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==5) {
			    muon_MDT_residual_eca_SectorPhi5[h_index]->Fill(hardware_name.c_str(),residual);		      
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==6) {
			    muon_MDT_residual_eca_SectorPhi6[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==7) {
			    muon_MDT_residual_eca_SectorPhi7[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==8) {
			    muon_MDT_residual_eca_SectorPhi8[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			}
			else if (isEndCapC) { 
			  if (m_mdtIdHelper->stationPhi(id)==1) {
			    muon_MDT_residual_ecc_SectorPhi1[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==2) {
			    muon_MDT_residual_ecc_SectorPhi2[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==3) {
			    muon_MDT_residual_ecc_SectorPhi3[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==4) {
			    muon_MDT_residual_ecc_SectorPhi4[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==5) {
			    muon_MDT_residual_ecc_SectorPhi5[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==6) {
			    muon_MDT_residual_ecc_SectorPhi6[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==7) {
			    muon_MDT_residual_ecc_SectorPhi7[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			  else if (m_mdtIdHelper->stationPhi(id)==8) {
			    muon_MDT_residual_ecc_SectorPhi8[h_index]->Fill(hardware_name.c_str(),residual);
			  }
			}     

		      } // isMDT
		      else if ( m_idHelperTool->isRpc(id) ){ 
			
			int _rpc_idx = 0;
			if ( m_idHelperTool->isSmallChamber(id) ) _rpc_idx = 2 * m_idHelperTool->stationPhi(id);
			else _rpc_idx = 2 * m_idHelperTool->stationPhi(id) - 1;
			if ( m_idHelperTool->stationEta(id) < 0 ) _rpc_idx = (-1) * _rpc_idx;
			 
			muon_RPC_residual[h_index]->Fill(residual);
			muon_RPC_pull[h_index]->Fill(pull);
			  
			if (m_debuglevel) m_log << MSG::DEBUG<<"RPC --->>> stationPhi:" << m_idHelperTool->stationPhi(id) << ", stationEta:" << m_idHelperTool->stationEta(id) 
						<<", stationName: " << m_rpcIdHelper->stationName(id) << ", stationNameString: "
						<< m_idHelperTool->toString( id ) << endreq;
						  
			if (measuresPhi) {
			  muon_RPC_phi_residual[h_index]->Fill(residual);
			  muon_RPC_pull_phi[h_index]->Fill(pull);
			    
			  muon_RPC_phi_residual_RMS[h_index]->Fill(_rpc_idx, residual); 
			    
			}
			else { //measuresEta
			  muon_RPC_eta_residual[h_index]->Fill(residual);
			  muon_RPC_pull_eta[h_index]->Fill(pull);
			    
			  muon_RPC_eta_residual_RMS[h_index]->Fill(_rpc_idx, residual); 
			    
			}
		      } //is RPC
		      else if ( m_idHelperTool->isTgc(id) ){ 
			muon_TGC_residual[h_index]->Fill(residual);
			muon_TGC_pull[h_index]->Fill(pull);
			  
			int _tgc_idx = m_tgcIdHelper->stationName(id) - 40;
			if ( m_idHelperTool->stationEta(id) < 0 ) _tgc_idx = _tgc_idx*(-1);
			
			int _tgc_phi = 0;
			if ( m_idHelperTool->stationEta(id) < 0 ) _tgc_phi = (-1) * m_idHelperTool->stationPhi(id);
			else                                      _tgc_phi = m_idHelperTool->stationPhi(id);
			  
			if (m_debuglevel) m_log << MSG::DEBUG<<"TGC --->>> stationPhi:" << m_idHelperTool->stationPhi(id) << ", stationEta:" << m_idHelperTool->stationEta(id)  
						<<", stationName: " << m_tgcIdHelper->stationName(id) << ", stationNameString: "
						<< m_idHelperTool->toString( id ) << endreq;
			if (m_tgcIdHelper->isStrip(id)) {//measuresPhi -> Strip
			  //if (measuresPhi) {
			  muon_TGC_phi_residual[h_index]->Fill(residual);
			  muon_TGC_pull_phi[h_index]->Fill(pull);
			    
			  muon_TGC_residual_strip_RMS[h_index]->Fill(_tgc_idx, residual);
			  
			  if (abs(_tgc_idx)%2 == 0)
			    muon_TGC_residual_EndCapStrip_phi[h_index]->Fill(_tgc_phi, residual);
			  else muon_TGC_residual_ForwardStrip_phi[h_index]->Fill(_tgc_phi, residual);
			      
			}
			else { //measuresEta->Wire
			  muon_TGC_eta_residual[h_index]->Fill(residual);
			  muon_TGC_pull_eta[h_index]->Fill(pull);
			    
			  muon_TGC_residual_wire_RMS[h_index]->Fill(_tgc_idx, residual);
			  
			  if (abs(_tgc_idx)%2 == 0)
			    muon_TGC_residual_EndCapWire_phi[h_index]->Fill(_tgc_phi, residual);
			  else muon_TGC_residual_ForwardWire_phi[h_index]->Fill(_tgc_phi, residual);
			      
			}
		      } //is TGC
		      else if ( m_idHelperTool->isCsc(id) ){ 
			
			int _csc_idx = 0;
			if ( m_idHelperTool->isSmallChamber(id) ) _csc_idx = 2 * m_idHelperTool->stationPhi(id);
			else _csc_idx = 2 * m_idHelperTool->stationPhi(id) - 1;
			if ( m_idHelperTool->stationEta(id) < 0 ) _csc_idx = (-1) * _csc_idx;
			
			muon_CSC_residual[h_index]->Fill(residual);
			muon_CSC_pull[h_index]->Fill(pull);
			
			if (measuresPhi) {
			  muon_CSC_phi_residual[h_index]->Fill(residual);
			  muon_CSC_pull_phi[h_index]->Fill(pull);
			  
			  muon_CSC_phi_residual_RMS[h_index]->Fill(_csc_idx, residual); 
			  
			}
			else { //measuresEta
			  muon_CSC_eta_residual[h_index]->Fill(residual);
			  muon_CSC_pull_eta[h_index]->Fill(pull);
			  
			  muon_CSC_eta_residual_RMS[h_index]->Fill(_csc_idx, residual); 
			  
			}
		      } // is CSC
		      muon_glbl_residual[h_index]->Fill(residual);
		      muon_glbl_pull[h_index]->Fill(pull);
		    } // if valid id
		  } //if residual
		} //if respull

		if (resPull) delete resPull;

 
		// Retrieve fit quality on surface
		const Trk::FitQualityOnSurface* fitQualityOnSurface = (*stateOnSurface)->fitQualityOnSurface();

		// Test to see if the state on surface is a multi-component state
		const Trk::MultiComponentStateOnSurface* multiComponentSoS = dynamic_cast<const Trk::MultiComponentStateOnSurface*>(*stateOnSurface);

		if ( !multiComponentSoS ){

		  if (m_debuglevel) m_log << MSG::VERBOSE << "Filling the MuonGenericTracksMon monitoring Histo based on a regular TrackStateOnSurface" << endreq;

		  if ( fitQualityOnSurface ){
		    float chiSquared = fitQualityOnSurface->chiSquared();
		    int numberDoF     = fitQualityOnSurface->numberDoF();

		    float trackX = trackParameters->position().x();

		    if (m_debuglevel) m_log << MSG::VERBOSE << "State on surface chi squared and number DOF: " << trackX << ": " << chiSquared << " / " << numberDoF << endreq;       
		  } 
 

		  muon_rec_locX[h_index]->Fill(trackParameters->parameters()[Trk::locX]);
		  muon_rec_locY[h_index]->Fill(trackParameters->parameters()[Trk::locY]);
		  muon_rec_locR[h_index]->Fill(trackParameters->parameters()[Trk::driftRadius]);
		  muon_rec_phi[h_index]->Fill(trackParameters->parameters()[Trk::phi]);
		  muon_rec_theta[h_index]->Fill(trackParameters->parameters()[Trk::theta]);
		  muon_rec_qOverP[h_index]->Fill(trackParameters->parameters()[Trk::qOverP]);

		  muon_rec_posX[h_index]->Fill(trackParameters->position().x());
		  muon_rec_posY[h_index]->Fill(trackParameters->position().y());
		  muon_rec_posZ[h_index]->Fill(trackParameters->position().z());
		  muon_rec_posR[h_index]->Fill(trackParameters->position().perp());


		  // Test to see if the track parameters have associated error matrix
		  const Trk::TrackParameters* measuredTrackParameters = dynamic_cast<const Trk::TrackParameters*>(trackParameters);

		  if ( measuredTrackParameters ){
		    // const Trk::ErrorMatrix& errorMatrix = measuredTrackParameters->localErrorMatrix();
		    muon_rec_error_locX[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::locX,Trk::locX) ) ;
		    muon_rec_error_locY[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::locY,Trk::locY) );
		    muon_rec_error_locR[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::driftRadius,Trk::driftRadius) );
		    muon_rec_error_phi[h_index]->Fill(RadsToDegrees( (*measuredTrackParameters->covariance())(Trk::phi,Trk::phi) ));
		    muon_rec_error_theta[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::theta,Trk::theta) );
		    muon_rec_error_qOverP[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::qOverP,Trk::qOverP) );		
		  }

		} // end condition of single component state on surface

		else{
		  if (m_debuglevel) m_log << MSG::VERBOSE << "Filling the MuonGenericTracksMon monitoring Histo based on a MultiComponentStateOnSurface" << endreq;

		  // Retrieve the components
		  const Trk::MultiComponentState* multiComponentState = multiComponentSoS->components();

		  if ( !multiComponentState ){
		    if (m_debuglevel) m_log << MSG::WARNING << "MultiComponentState cannot be retrieved for this StateOnSurface... Continuing!" << endreq;
		    continue;
		  }

		  // Loop over all components in the MultiComponentState
		  Trk::MultiComponentState::const_iterator component = multiComponentState->begin();
		  for ( ; component != multiComponentState->end(); ++component ){

		    // Get the track parameters and weight
		    const Trk::TrackParameters* componentParameters = component->first;
		    //float weight = component->second;
		    // Check and see if the TrackParameters object has an associated error matrix
		    const Trk::TrackParameters* measuredTrackParameters = dynamic_cast<const Trk::TrackParameters*>(componentParameters);

		    // Fill ntuple entries
		    //muon_rec_weights[m_rec_nSoS][m_rec_nComponents] = weight;
		    //muon_rec_weights[h_index]->Fill(weight);

		    //muon_rec_locX  [m_rec_nSoS][m_rec_nComponents] = componentParameters->parameters()[Trk::locX];
		    muon_rec_locX[h_index]->Fill(trackParameters->parameters()[Trk::locX]);
		    muon_rec_locY[h_index]->Fill(trackParameters->parameters()[Trk::locY]);
		    muon_rec_locR[h_index]->Fill(trackParameters->parameters()[Trk::driftRadius]);
		    muon_rec_phi[h_index]->Fill(trackParameters->parameters()[Trk::phi]);
		    muon_rec_theta[h_index]->Fill(trackParameters->parameters()[Trk::theta]);
		    muon_rec_qOverP[h_index]->Fill(trackParameters->parameters()[Trk::qOverP]);

		    muon_rec_posX[h_index]->Fill(trackParameters->position().x());
		    muon_rec_posY[h_index]->Fill(trackParameters->position().y());
		    muon_rec_posZ[h_index]->Fill(trackParameters->position().z());
		    muon_rec_posR[h_index]->Fill(trackParameters->position().perp());

		    if ( measuredTrackParameters ){
		      //		      const Trk::ErrorMatrix& errorMatrix = measuredTrackParameters->localErrorMatrix();
		      //  muon_rec_error_locX  [m_rec_nSoS][m_rec_nComponents] = errorMatrix.error(Trk::locX);
		      muon_rec_error_locX[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::locX,Trk::locX) );
		      muon_rec_error_locY[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::locY,Trk::locY) );
		      muon_rec_error_locR[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::driftRadius,Trk::driftRadius) );
		      muon_rec_error_phi[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::phi,Trk::phi) );
		      muon_rec_error_theta[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::theta,Trk::theta) );
		      muon_rec_error_qOverP[h_index]->Fill( (*measuredTrackParameters->covariance())(Trk::qOverP,Trk::qOverP) ); 

		    }

		    // Increment the number of components in the state on surface
		    ++m_rec_nComponents;
		  } // end loop over all components in the state on surface

		} // end condition requiring multiple component states

		  // Increment the number of states on surface in the track
		++m_rec_nSoS;

	      } // end loop over all track state on surface on track 

	    } // else -->  Get pointer to track state on surfaces

	  } // loop over tracks

	} // loop over collections

      } // Once  it passed Muontrigger or not using muon trigger


    ++m_eventCounter;
    if (m_debuglevel){m_log << MSG::DEBUG << "Number of events : "<< m_eventCounter << endreq;
      m_log << MSG::DEBUG << "Number of components in the state on surface : "<< m_rec_nComponents << endreq; 
      m_log << MSG::DEBUG << "Number of states on surface in the track: "<< m_rec_nSoS << endreq; 

    } //environment condition
  
    m_log << MSG::DEBUG << "MuonGenericTracksMon::Finalisation of " << name() << " was successful" << endreq;}
  
  return StatusCode::SUCCESS;

}

/** procHistograms*/
//---------------------------------------------------------------------------------------
 
StatusCode MuonGenericTracksMon::procHistograms()
{
  StatusCode sc=StatusCode::SUCCESS;
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {
  
    if (m_debuglevel) m_log << MSG::DEBUG << "MuonTrackMonitoring finalize()" << endreq;
    if(endOfEventsBlock){}
    if(endOfLumiBlock){}
    if(endOfRun){

      /** Normalize histograms  */

      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi1.size(); ii++) {

	//give hardware names to myChamberIDs
	//  for (ibin=1; ibin<=hwNames.size(); ibin++) 
	//        muon_MDT_residual_barrel_SectorPhi1[ii]->GetXAxis()->SetBinLabel(ibin,hwNames[ibin-1]);

	// sort profile histos bins in accordance to labels
	// Option = "a" sort by alphabetic order
	// Option = "v" draw labels vertical      
	if (muon_MDT_residual_barrel_SectorPhi1[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi1[ii]->LabelsOption("va");
	if (muon_MDT_residual_barrel_SectorPhi2[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi2[ii]->LabelsOption("va");
	if (muon_MDT_residual_barrel_SectorPhi3[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi3[ii]->LabelsOption("va");
	if (muon_MDT_residual_barrel_SectorPhi4[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi4[ii]->LabelsOption("va");
	if (muon_MDT_residual_barrel_SectorPhi5[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi5[ii]->LabelsOption("va");
	if (muon_MDT_residual_barrel_SectorPhi6[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi6[ii]->LabelsOption("va");
	if (muon_MDT_residual_barrel_SectorPhi7[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi7[ii]->LabelsOption("va");
	if (muon_MDT_residual_barrel_SectorPhi8[ii]->GetEntries()>0) muon_MDT_residual_barrel_SectorPhi8[ii]->LabelsOption("va");

	if (muon_MDT_residual_eca_SectorPhi1[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi1[ii]->LabelsOption("va");
	if (muon_MDT_residual_eca_SectorPhi2[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi2[ii]->LabelsOption("va");
	if (muon_MDT_residual_eca_SectorPhi3[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi3[ii]->LabelsOption("va");
	if (muon_MDT_residual_eca_SectorPhi4[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi4[ii]->LabelsOption("va");
	if (muon_MDT_residual_eca_SectorPhi5[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi5[ii]->LabelsOption("va");
	if (muon_MDT_residual_eca_SectorPhi6[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi6[ii]->LabelsOption("va");
	if (muon_MDT_residual_eca_SectorPhi7[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi7[ii]->LabelsOption("va");
	if (muon_MDT_residual_eca_SectorPhi8[ii]->GetEntries()>0) muon_MDT_residual_eca_SectorPhi8[ii]->LabelsOption("va");

	if (muon_MDT_residual_ecc_SectorPhi1[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi1[ii]->LabelsOption("va");
	if (muon_MDT_residual_ecc_SectorPhi2[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi2[ii]->LabelsOption("va");
	if (muon_MDT_residual_ecc_SectorPhi3[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi3[ii]->LabelsOption("va");
	if (muon_MDT_residual_ecc_SectorPhi4[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi4[ii]->LabelsOption("va");
	if (muon_MDT_residual_ecc_SectorPhi5[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi5[ii]->LabelsOption("va");
	if (muon_MDT_residual_ecc_SectorPhi6[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi6[ii]->LabelsOption("va");
	if (muon_MDT_residual_ecc_SectorPhi7[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi7[ii]->LabelsOption("va");
	if (muon_MDT_residual_ecc_SectorPhi8[ii]->GetEntries()>0) muon_MDT_residual_ecc_SectorPhi8[ii]->LabelsOption("va");

      }

      //for(unsigned int ii = 0; ii < muon_rec_weights.size(); ii++) {
      for(unsigned int ii = 0; ii < muon_rec_pT.size(); ii++) {
	muon_rec_pT[ii]->GetXaxis()->SetTitle("P_{T} [GeV]");
	muon_rec_pT[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT.size(); ii++) {
	muon_rec_ChargeRatio_pT[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})"); 
	if (muon_rec_neg_pT[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT[ii]->Divide(muon_rec_pos_pT[ii], muon_rec_neg_pT[ii], 1.0,1.0,""); 
	muon_rec_ChargeRatio_pT[ii]->SetEntries(muon_rec_pos_pT[ii]->GetEntries() + muon_rec_neg_pT[ii]->GetEntries()); }	
      for(unsigned int ii = 0; ii < muon_rec_pos_pT.size(); ii++) {
	muon_rec_pos_pT[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT.size(); ii++) {
	muon_rec_neg_pT[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_HighpT.size(); ii++) {
	muon_rec_HighpT[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_HighpT[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_pos_HighpT.size(); ii++) {
	muon_rec_pos_HighpT[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_HighpT[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_HighpT.size(); ii++) {
	muon_rec_neg_HighpT[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_HighpT[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_1overpT.size(); ii++) {
        muon_rec_1overpT[ii]->GetXaxis()->SetTitle("1/p_{T} [(GeV)^{-1}]");
        muon_rec_1overpT[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_charge.size(); ii++) {
	muon_rec_charge[ii]->GetXaxis()->SetTitle("Charge");
	muon_rec_charge[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_nStations.size(); ii++) {
	muon_rec_nStations[ii]->GetXaxis()->SetTitle("NUmber of stations");
	muon_rec_nStations[ii]->GetYaxis()->SetTitle("Tracks"); }

      
      ///++++ Charge Asymmetry
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector01.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector01[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector01[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector01[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector01[ii]->Divide(muon_rec_pos_pT_Sector01[ii],muon_rec_neg_pT_Sector01[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector01[ii]->SetEntries(muon_rec_pos_pT_Sector01[ii]->GetEntries() + muon_rec_neg_pT_Sector01[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector01.size(); ii++) {
	muon_rec_pos_pT_Sector01[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector01[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector01.size(); ii++) {
	muon_rec_neg_pT_Sector01[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector01[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector02.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector02[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector02[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector02[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector02[ii]->Divide(muon_rec_pos_pT_Sector02[ii],muon_rec_neg_pT_Sector02[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector02[ii]->SetEntries(muon_rec_pos_pT_Sector02[ii]->GetEntries() + muon_rec_neg_pT_Sector02[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector02.size(); ii++) {
	muon_rec_pos_pT_Sector02[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector02[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector02.size(); ii++) {
	muon_rec_neg_pT_Sector02[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector02[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector03.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector03[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector03[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector03[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector03[ii]->Divide(muon_rec_pos_pT_Sector03[ii],muon_rec_neg_pT_Sector03[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector03[ii]->SetEntries(muon_rec_pos_pT_Sector03[ii]->GetEntries() + muon_rec_neg_pT_Sector03[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector03.size(); ii++) {
	muon_rec_pos_pT_Sector03[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector03[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector03.size(); ii++) {
	muon_rec_neg_pT_Sector03[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector03[ii]->GetYaxis()->SetTitle("Tracks");  }
           
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector04.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector04[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector04[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector04[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector04[ii]->Divide(muon_rec_pos_pT_Sector04[ii],muon_rec_neg_pT_Sector04[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector04[ii]->SetEntries(muon_rec_pos_pT_Sector04[ii]->GetEntries() + muon_rec_neg_pT_Sector04[ii]->GetEntries());}   
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector04.size(); ii++) {
	muon_rec_pos_pT_Sector04[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector04[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector04.size(); ii++) {
	muon_rec_neg_pT_Sector04[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector04[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector05.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector05[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector05[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector05[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector05[ii]->Divide(muon_rec_pos_pT_Sector05[ii],muon_rec_neg_pT_Sector05[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector05[ii]->SetEntries(muon_rec_pos_pT_Sector05[ii]->GetEntries() + muon_rec_neg_pT_Sector05[ii]->GetEntries());}               
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector05.size(); ii++) {
	muon_rec_pos_pT_Sector05[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector05[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector05.size(); ii++) {
	muon_rec_neg_pT_Sector05[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector05[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector06.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector06[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector06[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector06[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector06[ii]->Divide(muon_rec_pos_pT_Sector06[ii],muon_rec_neg_pT_Sector06[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector06[ii]->SetEntries(muon_rec_pos_pT_Sector06[ii]->GetEntries() + muon_rec_neg_pT_Sector06[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector06.size(); ii++) {
	muon_rec_pos_pT_Sector06[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector06[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector06.size(); ii++) {
	muon_rec_neg_pT_Sector06[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector06[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector07.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector07[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector07[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector07[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector07[ii]->Divide(muon_rec_pos_pT_Sector07[ii],muon_rec_neg_pT_Sector07[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector07[ii]->SetEntries(muon_rec_pos_pT_Sector07[ii]->GetEntries() + muon_rec_neg_pT_Sector07[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector07.size(); ii++) {
	muon_rec_pos_pT_Sector07[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector07[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector07.size(); ii++) {
	muon_rec_neg_pT_Sector07[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector07[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector08.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector08[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector08[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector08[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector08[ii]->Divide(muon_rec_pos_pT_Sector08[ii],muon_rec_neg_pT_Sector08[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector08[ii]->SetEntries(muon_rec_pos_pT_Sector08[ii]->GetEntries() + muon_rec_neg_pT_Sector08[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector08.size(); ii++) {
	muon_rec_pos_pT_Sector08[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector08[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector08.size(); ii++) {
	muon_rec_neg_pT_Sector08[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector08[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector09.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector09[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector09[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector09[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector09[ii]->Divide(muon_rec_pos_pT_Sector09[ii],muon_rec_neg_pT_Sector09[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector09[ii]->SetEntries(muon_rec_pos_pT_Sector09[ii]->GetEntries() + muon_rec_neg_pT_Sector09[ii]->GetEntries());}         
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector09.size(); ii++) {
	muon_rec_pos_pT_Sector09[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector09[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector09.size(); ii++) {
	muon_rec_neg_pT_Sector09[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector09[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector10.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector10[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector10[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector10[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector10[ii]->Divide(muon_rec_pos_pT_Sector10[ii],muon_rec_neg_pT_Sector10[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector10[ii]->SetEntries(muon_rec_pos_pT_Sector10[ii]->GetEntries() + muon_rec_neg_pT_Sector10[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector10.size(); ii++) {
	muon_rec_pos_pT_Sector10[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector10[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector10.size(); ii++) {
	muon_rec_neg_pT_Sector10[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector10[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector11.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector11[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector11[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector11[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector11[ii]->Divide(muon_rec_pos_pT_Sector11[ii],muon_rec_neg_pT_Sector11[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector11[ii]->SetEntries(muon_rec_pos_pT_Sector11[ii]->GetEntries() + muon_rec_neg_pT_Sector11[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector11.size(); ii++) {
	muon_rec_pos_pT_Sector11[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector11[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector11.size(); ii++) {
	muon_rec_neg_pT_Sector11[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector11[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector12.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector12[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector12[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector12[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector12[ii]->Divide(muon_rec_pos_pT_Sector12[ii],muon_rec_neg_pT_Sector12[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector12[ii]->SetEntries(muon_rec_pos_pT_Sector12[ii]->GetEntries() + muon_rec_neg_pT_Sector12[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector12.size(); ii++) {
	muon_rec_pos_pT_Sector12[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector12[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector12.size(); ii++) {
	muon_rec_neg_pT_Sector12[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector12[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector13.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector13[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector13[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector13[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector13[ii]->Divide(muon_rec_pos_pT_Sector13[ii],muon_rec_neg_pT_Sector13[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector13[ii]->SetEntries(muon_rec_pos_pT_Sector13[ii]->GetEntries() + muon_rec_neg_pT_Sector13[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector13.size(); ii++) {
	muon_rec_pos_pT_Sector13[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector13[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector13.size(); ii++) {
	muon_rec_neg_pT_Sector13[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector13[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector14.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector14[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector14[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector14[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector14[ii]->Divide(muon_rec_pos_pT_Sector14[ii],muon_rec_neg_pT_Sector14[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector14[ii]->SetEntries(muon_rec_pos_pT_Sector14[ii]->GetEntries() + muon_rec_neg_pT_Sector14[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector14.size(); ii++) {
	muon_rec_pos_pT_Sector14[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector14[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector14.size(); ii++) {
	muon_rec_neg_pT_Sector14[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector14[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector15.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector15[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector15[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector15[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector15[ii]->Divide(muon_rec_pos_pT_Sector15[ii],muon_rec_neg_pT_Sector15[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector15[ii]->SetEntries(muon_rec_pos_pT_Sector15[ii]->GetEntries() + muon_rec_neg_pT_Sector15[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector15.size(); ii++) {
	muon_rec_pos_pT_Sector15[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector15[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector15.size(); ii++) {
	muon_rec_neg_pT_Sector15[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector15[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_Sector16.size(); ii++) {
	muon_rec_ChargeRatio_pT_Sector16[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_Sector16[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Sector16[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_Sector16[ii]->Divide(muon_rec_pos_pT_Sector16[ii],muon_rec_neg_pT_Sector16[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_Sector16[ii]->SetEntries(muon_rec_pos_pT_Sector16[ii]->GetEntries() + muon_rec_neg_pT_Sector16[ii]->GetEntries());}						
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Sector16.size(); ii++) {
	muon_rec_pos_pT_Sector16[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Sector16[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Sector16.size(); ii++) {
	muon_rec_neg_pT_Sector16[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Sector16[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_A.size(); ii++) {
	muon_rec_ChargeRatio_pT_A[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_A[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_A[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_A[ii]->Divide(muon_rec_pos_pT_A[ii],muon_rec_neg_pT_A[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_A[ii]->SetEntries(muon_rec_pos_pT_A[ii]->GetEntries() + muon_rec_neg_pT_A[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_A.size(); ii++) {
	muon_rec_pos_pT_A[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_A[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_A.size(); ii++) {
	muon_rec_neg_pT_A[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_A[ii]->GetYaxis()->SetTitle("Tracks"); }
      
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_C.size(); ii++) {
	muon_rec_ChargeRatio_pT_C[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_C[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_C[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_C[ii]->Divide(muon_rec_pos_pT_C[ii],muon_rec_neg_pT_C[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_C[ii]->SetEntries(muon_rec_pos_pT_C[ii]->GetEntries() + muon_rec_neg_pT_C[ii]->GetEntries());}
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_C.size(); ii++) {
	muon_rec_pos_pT_C[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_C[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_C.size(); ii++) {
	muon_rec_neg_pT_C[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_C[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_up.size(); ii++) {
	muon_rec_ChargeRatio_pT_up[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_up[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Up[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_up[ii]->Divide(muon_rec_pos_pT_Up[ii],muon_rec_neg_pT_Up[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_up[ii]->SetEntries(muon_rec_pos_pT_Up[ii]->GetEntries() + muon_rec_neg_pT_Up[ii]->GetEntries());}      
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Up.size(); ii++) {
	muon_rec_pos_pT_Up[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Up[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Up.size(); ii++) {
	muon_rec_neg_pT_Up[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Up[ii]->GetYaxis()->SetTitle("Tracks");  }
      
      for(unsigned int ii = 0; ii < muon_rec_ChargeRatio_pT_dw.size(); ii++) {
	muon_rec_ChargeRatio_pT_dw[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_ChargeRatio_pT_dw[ii]->GetYaxis()->SetTitle("Charge Ratio (#mu^{+}/#mu^{-})");
	if (muon_rec_neg_pT_Down[ii]->GetEntries()  > 0) muon_rec_ChargeRatio_pT_dw[ii]->Divide(muon_rec_pos_pT_Down[ii],muon_rec_neg_pT_Down[ii], 1.0, 1.0, ""); 
	muon_rec_ChargeRatio_pT_dw[ii]->SetEntries(muon_rec_pos_pT_Down[ii]->GetEntries() + muon_rec_neg_pT_Down[ii]->GetEntries());} 
      for(unsigned int ii = 0; ii < muon_rec_pos_pT_Down.size(); ii++) {
	muon_rec_pos_pT_Down[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_pos_pT_Down[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_rec_neg_pT_Down.size(); ii++) {
	muon_rec_neg_pT_Down[ii]->GetXaxis()->SetTitle("P_{T} [GeV/c]");
	muon_rec_neg_pT_Down[ii]->GetYaxis()->SetTitle("Tracks"); }
 
      for(unsigned int ii = 0; ii < muon_rec_eta_phi.size(); ii++) {
        muon_rec_eta_phi[ii]->SetOption("COLZ");
	muon_rec_eta_phi[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_rec_eta_phi[ii]->GetYaxis()->SetTitle("#eta [rad]");}
      for(unsigned int ii = 0; ii < muon_rec_eta_phi_3moreStations.size(); ii++) {
        muon_rec_eta_phi_3moreStations[ii]->SetOption("COLZ");
	muon_rec_eta_phi_3moreStations[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_rec_eta_phi_3moreStations[ii]->GetYaxis()->SetTitle("#eta [rad]");}
      for(unsigned int ii = 0; ii < muon_rec_eta_phi_2lessStations.size(); ii++) {
        muon_rec_eta_phi_2lessStations[ii]->SetOption("COLZ");
	muon_rec_eta_phi_2lessStations[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_rec_eta_phi_2lessStations[ii]->GetYaxis()->SetTitle("#eta [rad]");}
            
      for(unsigned int ii = 0; ii < muon_rec_pT_vs_eta.size(); ii++) {
	muon_rec_pT_vs_eta[ii]->SetOption("COLZ");
	muon_rec_pT_vs_eta[ii]->GetXaxis()->SetTitle("#eta [rad]");
	muon_rec_pT_vs_eta[ii]->GetYaxis()->SetTitle("P_{T} [GeV/c]");}    
      for(unsigned int ii = 0; ii < muon_rec_pT_vs_phi.size(); ii++) {
	muon_rec_pT_vs_phi[ii]->SetOption("COLZ");
	muon_rec_pT_vs_phi[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_rec_pT_vs_phi[ii]->GetYaxis()->SetTitle("P_{T} [GeV/c]");}
      for(unsigned int ii = 0; ii < muon_rec_charge_vs_eta.size(); ii++) {
	muon_rec_charge_vs_eta[ii]->SetOption("COLZ");
	muon_rec_charge_vs_eta[ii]->GetXaxis()->SetTitle("#eta [rad]");
	muon_rec_charge_vs_eta[ii]->GetYaxis()->SetTitle("Charge");}    
      for(unsigned int ii = 0; ii < muon_rec_charge_vs_phi.size(); ii++) {
	muon_rec_charge_vs_phi[ii]->SetOption("COLZ");
	muon_rec_charge_vs_phi[ii]->GetXaxis()->SetTitle("#phi [degrees]");
	muon_rec_charge_vs_phi[ii]->GetYaxis()->SetTitle("Charge");}

      for(unsigned int ii = 0; ii < muon_rec_locX.size(); ii++) {
	muon_rec_locX[ii]->GetXaxis()->SetTitle("Local X [mm]");
	muon_rec_locX[ii]->GetYaxis()->SetTitle("Tracks");}
      for(unsigned int ii = 0; ii < muon_rec_error_locX.size(); ii++) {
	muon_rec_error_locX[ii]->GetXaxis()->SetTitle("Local X Error [mm]");
	muon_rec_error_locX[ii]->GetYaxis()->SetTitle("Tracks");}

      for(unsigned int ii = 0; ii < muon_rec_locY.size(); ii++) {
	muon_rec_locY[ii]->GetXaxis()->SetTitle("Local Y [mm]");
	muon_rec_locY[ii]->GetYaxis()->SetTitle("Tracks");}
      for(unsigned int ii = 0; ii < muon_rec_error_locY.size(); ii++) {
	muon_rec_error_locY[ii]->GetXaxis()->SetTitle("Local Y Error [mm]");
	muon_rec_error_locY[ii]->GetYaxis()->SetTitle("Tracks");}

      for(unsigned int ii = 0; ii < muon_rec_locR.size(); ii++) {
	muon_rec_locR[ii]->GetXaxis()->SetTitle("Local R [mm]");
	muon_rec_locR[ii]->GetYaxis()->SetTitle("Tracks");}
      for(unsigned int ii = 0; ii < muon_rec_error_locR.size(); ii++) {
	muon_rec_error_locR[ii]->GetXaxis()->SetTitle("Local R Error [mm]");
	muon_rec_error_locR[ii]->GetYaxis()->SetTitle("Tracks");}

      for(unsigned int ii = 0; ii < muon_rec_phi.size(); ii++) {
	muon_rec_phi[ii]->GetXaxis()->SetTitle("#phi [rad]");
	muon_rec_phi[ii]->GetYaxis()->SetTitle("Tracks");}
      for(unsigned int ii = 0; ii < muon_rec_error_phi.size(); ii++) {
	muon_rec_error_phi[ii]->GetXaxis()->SetTitle("#phi Error [rad]");
	muon_rec_error_phi[ii]->GetYaxis()->SetTitle("Tracks");}

      for(unsigned int ii = 0; ii < muon_rec_theta.size(); ii++) {
	muon_rec_theta[ii]->GetXaxis()->SetTitle("#theta [rad]");
	muon_rec_theta[ii]->GetYaxis()->SetTitle("Tracks");}
      for(unsigned int ii = 0; ii < muon_rec_error_theta.size(); ii++) {
	muon_rec_error_theta[ii]->GetXaxis()->SetTitle("#theta Error [rad]");
	muon_rec_error_theta[ii]->GetYaxis()->SetTitle("Tracks");}

      for(unsigned int ii = 0; ii < muon_rec_qOverP.size(); ii++) {
	muon_rec_qOverP[ii]->GetXaxis()->SetTitle("q/P [(MeV)^{-1}]");
	muon_rec_qOverP[ii]->GetYaxis()->SetTitle("Tracks");}
      for(unsigned int ii = 0; ii < muon_rec_error_qOverP.size(); ii++) {
	muon_rec_error_qOverP[ii]->GetXaxis()->SetTitle("q/P Error [(MeV)^{-1}]");
	muon_rec_error_qOverP[ii]->GetYaxis()->SetTitle("Tracks");}

      for(unsigned int ii = 0; ii < muon_rec_posX.size(); ii++) {
	muon_rec_posX[ii]->GetXaxis()->SetTitle("X-Position [mm]");
	muon_rec_posX[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_posY.size(); ii++) {
	muon_rec_posY[ii]->GetXaxis()->SetTitle("Y-Position [mm]");
	muon_rec_posY[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_posZ.size(); ii++) {
	muon_rec_posZ[ii]->GetXaxis()->SetTitle("Z-Position [mm]");
	muon_rec_posZ[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_posR.size(); ii++) {
	muon_rec_posR[ii]->GetXaxis()->SetTitle("R-Position [mm]");
	muon_rec_posR[ii]->GetYaxis()->SetTitle("Tracks");  }


      for(unsigned int ii = 0; ii < muon_glbl_residual.size(); ii++) {
	muon_glbl_residual[ii]->GetXaxis()->SetTitle("Global residual [mm]");
	muon_glbl_residual[ii]->GetYaxis()->SetTitle("Hits"); }
      for(unsigned int ii = 0; ii < muon_MDT_residual.size(); ii++) {
	muon_MDT_residual[ii]->GetXaxis()->SetTitle("MDT residual [mm]");
	muon_MDT_residual[ii]->GetYaxis()->SetTitle("MDT Hits"); }
      for(unsigned int ii = 0; ii < muon_RPC_residual.size(); ii++) {
	muon_RPC_residual[ii]->GetXaxis()->SetTitle("RPC residual [mm]");
	muon_RPC_residual[ii]->GetYaxis()->SetTitle("RPC Hits");  }
      for(unsigned int ii = 0; ii < muon_TGC_residual.size(); ii++) {
	muon_TGC_residual[ii]->GetXaxis()->SetTitle("TGC residual [mm]");
	muon_TGC_residual[ii]->GetYaxis()->SetTitle("TGC Hits");  }
      for(unsigned int ii = 0; ii < muon_CSC_residual.size(); ii++) {
	muon_CSC_residual[ii]->GetXaxis()->SetTitle("CSC residual [mm]");
	muon_CSC_residual[ii]->GetYaxis()->SetTitle("CSC Hits");  }

      for(unsigned int ii = 0; ii < muon_glbl_pull.size(); ii++) {
        muon_glbl_pull[ii]->GetXaxis()->SetTitle("Global Pull");
        muon_glbl_pull[ii]->GetYaxis()->SetTitle("Hits");  }
      for(unsigned int ii = 0; ii < muon_MDT_pull.size(); ii++) {
        muon_MDT_pull[ii]->GetXaxis()->SetTitle("MDT pull");
        muon_MDT_pull[ii]->GetYaxis()->SetTitle("MDT Hits");  }
      for(unsigned int ii = 0; ii < muon_RPC_pull.size(); ii++) {
        muon_RPC_pull[ii]->GetXaxis()->SetTitle("RPC pull");
        muon_RPC_pull[ii]->GetYaxis()->SetTitle("RPC Hits"); }
      for(unsigned int ii = 0; ii < muon_TGC_pull.size(); ii++) {
        muon_TGC_pull[ii]->GetXaxis()->SetTitle("TGC pull");
        muon_TGC_pull[ii]->GetYaxis()->SetTitle("TGC Hits");  }
      for(unsigned int ii = 0; ii < muon_CSC_pull.size(); ii++) {
        muon_CSC_pull[ii]->GetXaxis()->SetTitle("CSC pull");
        muon_CSC_pull[ii]->GetYaxis()->SetTitle("CSC Hits"); }

      for(unsigned int ii = 0; ii < muon_RPC_eta_residual.size(); ii++) {
	muon_RPC_eta_residual[ii]->GetXaxis()->SetTitle("RPC #eta residual [mm]");
	muon_RPC_eta_residual[ii]->GetYaxis()->SetTitle("RPC #eta Hits"); }
      for(unsigned int ii = 0; ii < muon_RPC_phi_residual.size(); ii++) {
	muon_RPC_phi_residual[ii]->GetXaxis()->SetTitle("RPC #phi residual [mm]");
	muon_RPC_phi_residual[ii]->GetYaxis()->SetTitle("RPC #phi Hits");  }
      for(unsigned int ii = 0; ii < muon_TGC_eta_residual.size(); ii++) {
	muon_TGC_eta_residual[ii]->GetXaxis()->SetTitle("TGC #eta residual [mm]");
	muon_TGC_eta_residual[ii]->GetYaxis()->SetTitle("TGC #eta Hits");  }
      for(unsigned int ii = 0; ii < muon_TGC_phi_residual.size(); ii++) {
	muon_TGC_phi_residual[ii]->GetXaxis()->SetTitle("TGC #phi residual [mm]");
	muon_TGC_phi_residual[ii]->GetYaxis()->SetTitle("TGC #phi hits"); }
      for(unsigned int ii = 0; ii < muon_CSC_eta_residual.size(); ii++) {
	muon_CSC_eta_residual[ii]->GetXaxis()->SetTitle("CSC #eta residual [mm]");
	muon_CSC_eta_residual[ii]->GetYaxis()->SetTitle("CSC #eta Hits");  }
      for(unsigned int ii = 0; ii < muon_CSC_phi_residual.size(); ii++) {
	muon_CSC_phi_residual[ii]->GetXaxis()->SetTitle("CSC #phi residual [mm]");
	muon_CSC_phi_residual[ii]->GetYaxis()->SetTitle("CSC #phi Hits");  }

      for(unsigned int ii = 0; ii < muon_RPC_pull_eta.size(); ii++) {
        muon_RPC_pull_eta[ii]->GetXaxis()->SetTitle("RPC #eta pull");
	if (muon_RPC_pull_eta[ii]->GetEntries()  > 0) muon_RPC_pull_eta[ii]->Scale( 1. / (muon_RPC_pull_eta[ii]->GetEntries()) ); }
      for(unsigned int ii = 0; ii < muon_RPC_pull_phi.size(); ii++) {
        muon_RPC_pull_phi[ii]->GetXaxis()->SetTitle("RPC #phi pull");
	if (muon_RPC_pull_phi[ii]->GetEntries()  > 0) muon_RPC_pull_phi[ii]->Scale( 1. / (muon_RPC_pull_phi[ii]->GetEntries()) ); }
      for(unsigned int ii = 0; ii < muon_TGC_pull_eta.size(); ii++) {
        muon_TGC_pull_eta[ii]->GetXaxis()->SetTitle("TGC #eta pull");
	if (muon_TGC_pull_eta[ii]->GetEntries()  > 0) muon_TGC_pull_eta[ii]->Scale( 1. / (muon_TGC_pull_eta[ii]->GetEntries()) ); }
      for(unsigned int ii = 0; ii < muon_TGC_pull_phi.size(); ii++) {
        muon_TGC_pull_phi[ii]->GetXaxis()->SetTitle("TGC #phi pull");
	if (muon_TGC_pull_phi[ii]->GetEntries()  > 0) muon_TGC_pull_phi[ii]->Scale( 1. / (muon_TGC_pull_phi[ii]->GetEntries()) ); }
      for(unsigned int ii = 0; ii < muon_CSC_pull_eta.size(); ii++) {
        muon_CSC_pull_eta[ii]->GetXaxis()->SetTitle("CSC #eta pull");
	if (muon_CSC_pull_eta[ii]->GetEntries()  > 0) muon_CSC_pull_eta[ii]->Scale( 1. / (muon_CSC_pull_eta[ii]->GetEntries()) ); }
      for(unsigned int ii = 0; ii < muon_CSC_pull_phi.size(); ii++) {
        muon_CSC_pull_phi[ii]->GetXaxis()->SetTitle("CSC #phi pull");
	if (muon_CSC_pull_phi[ii]->GetEntries()  > 0) muon_CSC_pull_phi[ii]->Scale( 1. / (muon_CSC_pull_phi[ii]->GetEntries()) ); }
	
      for(unsigned int ii = 0; ii < muon_MDT_residual_pT.size(); ii++) {
	muon_MDT_residual_pT[ii]->GetXaxis()->SetTitle("p_{T} [GeV]"); 
	muon_MDT_residual_pT[ii]->GetYaxis()->SetTitle("MDT Residual [mm]");  
	muon_MDT_residual_pT[ii]->SetOption("COLZ");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_pT_RMS.size(); ii++) {
	muon_MDT_residual_pT_RMS[ii]->GetXaxis()->SetTitle("p_{T} [GeV]"); 
	muon_MDT_residual_pT_RMS[ii]->GetYaxis()->SetTitle("MDT Residual [mm]");  }
      
      for(unsigned int ii = 0; ii < muon_MDT_residual_BA_2stations.size(); ii++) {
	muon_MDT_residual_BA_2stations[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BA_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_BA_2stations[ii]->SetOption("COLZ");  }	
      for(unsigned int ii = 0; ii < muon_MDT_residual_BC_2stations.size(); ii++) {
	muon_MDT_residual_BC_2stations[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BC_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_BC_2stations[ii]->SetOption("COLZ"); }
      for(unsigned int ii = 0; ii < muon_MDT_residual_EA_2stations.size(); ii++) {
	muon_MDT_residual_EA_2stations[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EA_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_EA_2stations[ii]->SetOption("COLZ");  }	
      for(unsigned int ii = 0; ii < muon_MDT_residual_EC_2stations.size(); ii++) {
	muon_MDT_residual_EC_2stations[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EC_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_EC_2stations[ii]->SetOption("COLZ"); }
	
      for(unsigned int ii = 0; ii < muon_MDT_residual_BA.size(); ii++) {
	muon_MDT_residual_BA[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BA[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_BA[ii]->SetOption("COLZ");  }	
      for(unsigned int ii = 0; ii < muon_MDT_residual_BC.size(); ii++) {
	muon_MDT_residual_BC[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BC[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_BC[ii]->SetOption("COLZ"); }
      for(unsigned int ii = 0; ii < muon_MDT_residual_EA.size(); ii++) {
	muon_MDT_residual_EA[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EA[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_EA[ii]->SetOption("COLZ");  }	
      for(unsigned int ii = 0; ii < muon_MDT_residual_EC.size(); ii++) {
	muon_MDT_residual_EC[ii]->GetXaxis()->SetTitle("[1-16] Sectors 1-16 Inner, [17-32] Sectors 1-16 Middle, [33-48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EC[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  
	muon_MDT_residual_EC[ii]->SetOption("COLZ"); }
	
		
      for(unsigned int ii = 0; ii < muon_MDT_residual_BA_fit.size(); ii++) {
        muon_MDT_residual_BA_fit[ii]->Sumw2();
	m_muondqafitfunc->FillGaussMeanAndWidth(muon_MDT_residual_BA[ii], muon_MDT_residual_BA_fit[ii], -5.0, 5.0);
	muon_MDT_residual_BA_fit[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BA_fit[ii]->GetYaxis()->SetTitle("MDT residual: mean, #sigma");  }		
      for(unsigned int ii = 0; ii < muon_MDT_residual_BC_fit.size(); ii++) {
        muon_MDT_residual_BC_fit[ii]->Sumw2();
        m_muondqafitfunc->FillGaussMeanAndWidth(muon_MDT_residual_BC[ii], muon_MDT_residual_BC_fit[ii], -5.0, 5.0);
	muon_MDT_residual_BC_fit[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BC_fit[ii]->GetYaxis()->SetTitle("MDT residual: mean, #sigma");  }
	
      for(unsigned int ii = 0; ii < muon_MDT_residual_EA_fit.size(); ii++) {
        muon_MDT_residual_EA_fit[ii]->Sumw2();
	m_muondqafitfunc->FillGaussMeanAndWidth(muon_MDT_residual_EA[ii], muon_MDT_residual_EA_fit[ii], -5.0, 5.0);
	muon_MDT_residual_EA_fit[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EA_fit[ii]->GetYaxis()->SetTitle("MDT residual: mean, #sigma");  }		
      for(unsigned int ii = 0; ii < muon_MDT_residual_EC_fit.size(); ii++) {
        muon_MDT_residual_EC_fit[ii]->Sumw2();
        m_muondqafitfunc->FillGaussMeanAndWidth(muon_MDT_residual_EC[ii], muon_MDT_residual_EC_fit[ii], -5.0, 5.0);
	muon_MDT_residual_EC_fit[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EC_fit[ii]->GetYaxis()->SetTitle("MDT residual: mean, #sigma");  }
	
	
      for(unsigned int ii = 0; ii < muon_MDT_pull_sideA_RMS.size(); ii++) {
	muon_MDT_pull_sideA_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_pull_sideA_RMS[ii]->GetYaxis()->SetTitle("[MDT pull]");  }
      for(unsigned int ii = 0; ii < muon_MDT_pull_sideC_RMS.size(); ii++) {
	muon_MDT_pull_sideC_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_pull_sideC_RMS[ii]->GetYaxis()->SetTitle("[MDT pull]");  }
	
      for(unsigned int ii = 0; ii < muon_MDT_residual_sideA_RMS.size(); ii++) {
	muon_MDT_residual_sideA_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_sideA_RMS[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_sideC_RMS.size(); ii++) {
	muon_MDT_residual_sideC_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_sideC_RMS[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  } 
      
      for(unsigned int ii = 0; ii < muon_MDT_residual_BA_RMS_2stations.size(); ii++) {
	muon_MDT_residual_BA_RMS_2stations[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BA_RMS_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_BC_RMS_2stations.size(); ii++) {
	muon_MDT_residual_BC_RMS_2stations[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BC_RMS_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_EA_RMS_2stations.size(); ii++) {
	muon_MDT_residual_EA_RMS_2stations[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EA_RMS_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_EC_RMS_2stations.size(); ii++) {
	muon_MDT_residual_EC_RMS_2stations[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EC_RMS_2stations[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
            
      for(unsigned int ii = 0; ii < muon_MDT_residual_BA_RMS.size(); ii++) {
	muon_MDT_residual_BA_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BA_RMS[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_BC_RMS.size(); ii++) {
	muon_MDT_residual_BC_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_BC_RMS[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_EA_RMS.size(); ii++) {
	muon_MDT_residual_EA_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EA_RMS[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_MDT_residual_EC_RMS.size(); ii++) {
	muon_MDT_residual_EC_RMS[ii]->GetXaxis()->SetTitle("[1,16] Sectors 1-16 Inner, [17,32] Sectors 1-16 Middle, [33,48] Sectors 1-16 Outer, [49] sectors 1-16 Ext"); 
	muon_MDT_residual_EC_RMS[ii]->GetYaxis()->SetTitle("MDT residual [mm]");  }
      
      for(unsigned int ii = 0; ii < muon_RPC_eta_residual_RMS.size(); ii++) {
	muon_RPC_eta_residual_RMS[ii]->GetXaxis()->SetTitle("Side C   <--- [1,16] Sectors 1-16 --->   Side A"); 
	muon_RPC_eta_residual_RMS[ii]->GetYaxis()->SetTitle("RPC #eta residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_RPC_phi_residual_RMS.size(); ii++) {
	muon_RPC_phi_residual_RMS[ii]->GetXaxis()->SetTitle("Side C   <--- [1,16] Sectors 1-16 --->   Side A"); 
	muon_RPC_phi_residual_RMS[ii]->GetYaxis()->SetTitle("RPC #phi residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_TGC_residual_strip_RMS.size(); ii++) {
	muon_TGC_residual_strip_RMS[ii]->GetXaxis()->SetTitle("Side C <-- 1:T1F,2:T1E,3:T2F,4:T2E,5:T3F,6:T3E,7:T4F,8:T4E  --> Side A"); 
	muon_TGC_residual_strip_RMS[ii]->GetYaxis()->SetTitle("TGC strip residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_TGC_residual_wire_RMS.size(); ii++) {
	muon_TGC_residual_wire_RMS[ii]->GetXaxis()->SetTitle("Side C <-- 1:T1F,2:T1E,3:T2F,4:T2E,5:T3F,6:T3E,7:T4F,8:T4E  --> Side A"); 
	muon_TGC_residual_wire_RMS[ii]->GetYaxis()->SetTitle("TGC wire residual [mm]");  } 	
      for(unsigned int ii = 0; ii < muon_CSC_eta_residual_RMS.size(); ii++) {
	muon_CSC_eta_residual_RMS[ii]->GetXaxis()->SetTitle("Side C   <--- [1,16] Sectors 1-16 --->   Side A"); 
	muon_CSC_eta_residual_RMS[ii]->GetYaxis()->SetTitle("CSC #eta residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_CSC_phi_residual_RMS.size(); ii++) {
	muon_CSC_phi_residual_RMS[ii]->GetXaxis()->SetTitle("Side C   <--- [1,16] Sectors 1-16 --->   Side A"); 
	muon_CSC_phi_residual_RMS[ii]->GetYaxis()->SetTitle("CSC #phi residual [mm]");  }
      
      for(unsigned int ii = 0; ii < muon_TGC_residual_EndCapWire_phi.size(); ii++) {
	muon_TGC_residual_EndCapWire_phi[ii]->GetXaxis()->SetTitle("Side C <-- #phi Station  --> Side A"); 
	muon_TGC_residual_EndCapWire_phi[ii]->GetYaxis()->SetTitle("EndCap TGC wire residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_TGC_residual_EndCapStrip_phi.size(); ii++) {
	muon_TGC_residual_EndCapStrip_phi[ii]->GetXaxis()->SetTitle("Side C <-- #phi Station  --> Side A"); 
	muon_TGC_residual_EndCapStrip_phi[ii]->GetYaxis()->SetTitle("EndCap TGC strip residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_TGC_residual_ForwardWire_phi.size(); ii++) {
	muon_TGC_residual_ForwardWire_phi[ii]->GetXaxis()->SetTitle("Side C <-- #phi Station  --> Side A"); 
	muon_TGC_residual_ForwardWire_phi[ii]->GetYaxis()->SetTitle("Forward TGC wire residual [mm]");  }
      for(unsigned int ii = 0; ii < muon_TGC_residual_ForwardStrip_phi.size(); ii++) {
	muon_TGC_residual_ForwardStrip_phi[ii]->GetXaxis()->SetTitle("Side C <-- #phi Station  --> Side A"); 
	muon_TGC_residual_ForwardStrip_phi[ii]->GetYaxis()->SetTitle("Forward TGC strip residual [mm]");  }
		
      //Track Parameters at Perigee and their error
      for(unsigned int ii = 0; ii < muon_perigee_qOverP.size(); ii++) {
        muon_perigee_qOverP[ii]->GetXaxis()->SetTitle("q/P [(GeV)^{-1}]");
        muon_perigee_qOverP[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_perigee_P.size(); ii++) {
	muon_perigee_P[ii]->GetXaxis()->SetTitle("P [GeV]");
	muon_perigee_P[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_perigee_d0.size(); ii++) {
	muon_perigee_d0[ii]->GetXaxis()->SetTitle("d_{0} [mm]");
	muon_perigee_d0[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_perigee_z0.size(); ii++) {
	muon_perigee_z0[ii]->GetXaxis()->SetTitle("z_{0} [mm]");
	muon_perigee_z0[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_perigee_phi0.size(); ii++) {
        muon_perigee_phi0[ii]->GetXaxis()->SetTitle("#phi_{0} [degrees]");
        muon_perigee_phi0[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_perigee_theta.size(); ii++) {
        muon_perigee_theta[ii]->GetXaxis()->SetTitle("#theta [rad]");
        muon_perigee_theta[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_perigee_eta.size(); ii++) {
        muon_perigee_eta[ii]->GetXaxis()->SetTitle("#eta [rad]");
        muon_perigee_eta[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_chi2.size(); ii++) {
        muon_rec_chi2[ii]->GetXaxis()->SetTitle("#chi^{2}/DoF");
        muon_rec_chi2[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_rec_DegreesOfFreedom.size(); ii++) {
	muon_rec_DegreesOfFreedom[ii]->GetXaxis()->SetTitle("Degrees of Freedom");
	muon_rec_DegreesOfFreedom[ii]->GetYaxis()->SetTitle("Tracks"); }
 
      for(unsigned int ii = 0; ii < muon_perigee_error_d0.size(); ii++) {
	muon_perigee_error_d0[ii]->GetXaxis()->SetTitle("d_{0} Error [mm]");
	muon_perigee_error_d0[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_perigee_error_z0.size(); ii++) {
	muon_perigee_error_z0[ii]->GetXaxis()->SetTitle("z_{0} Error [mm]");
	muon_perigee_error_z0[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_perigee_error_phi0.size(); ii++) {
	muon_perigee_error_phi0[ii]->GetXaxis()->SetTitle("#phi_{0} Error [degrees]");
	muon_perigee_error_phi0[ii]->GetYaxis()->SetTitle("Tracks"); }
      for(unsigned int ii = 0; ii < muon_perigee_error_theta0.size(); ii++) {
        muon_perigee_error_theta0[ii]->GetXaxis()->SetTitle("#theta Error [rad]");
        muon_perigee_error_theta0[ii]->GetYaxis()->SetTitle("Tracks");  }
      for(unsigned int ii = 0; ii < muon_perigee_error_qOverP.size(); ii++) {
        muon_perigee_error_qOverP[ii]->GetXaxis()->SetTitle("q/P Error [(GeV)^{-1}]");
        muon_perigee_error_qOverP[ii]->GetYaxis()->SetTitle("Tracks");  }


      /** Filling MDT RMS from Residuals TH1Fs  using MuonDQAUtils function */

      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi1.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi1[ii],muon_MDT_residualRMS_barrel_SectorPhi1[ii]); }
      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi2.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi2[ii],muon_MDT_residualRMS_barrel_SectorPhi2[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi3.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi3[ii],muon_MDT_residualRMS_barrel_SectorPhi3[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi4.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi4[ii],muon_MDT_residualRMS_barrel_SectorPhi4[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi5.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi5[ii],muon_MDT_residualRMS_barrel_SectorPhi5[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi6.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi6[ii],muon_MDT_residualRMS_barrel_SectorPhi6[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi7.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi7[ii],muon_MDT_residualRMS_barrel_SectorPhi7[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_barrel_SectorPhi8.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_barrel_SectorPhi8[ii],muon_MDT_residualRMS_barrel_SectorPhi8[ii]);}

      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi1.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi1[ii], muon_MDT_residualRMS_eca_SectorPhi1[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi2.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi2[ii], muon_MDT_residualRMS_eca_SectorPhi2[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi3.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi3[ii], muon_MDT_residualRMS_eca_SectorPhi3[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi4.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi4[ii], muon_MDT_residualRMS_eca_SectorPhi4[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi5.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi5[ii], muon_MDT_residualRMS_eca_SectorPhi5[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi6.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi6[ii], muon_MDT_residualRMS_eca_SectorPhi6[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi7.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi7[ii], muon_MDT_residualRMS_eca_SectorPhi7[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi8.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_eca_SectorPhi8[ii], muon_MDT_residualRMS_eca_SectorPhi8[ii]);}

      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi1.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi1[ii], muon_MDT_residualRMS_ecc_SectorPhi1[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi2.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi2[ii], muon_MDT_residualRMS_ecc_SectorPhi2[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi3.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi3[ii], muon_MDT_residualRMS_ecc_SectorPhi3[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi4.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi4[ii], muon_MDT_residualRMS_ecc_SectorPhi4[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi5.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi5[ii], muon_MDT_residualRMS_ecc_SectorPhi5[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi6.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi6[ii], muon_MDT_residualRMS_ecc_SectorPhi6[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi7.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi7[ii], muon_MDT_residualRMS_ecc_SectorPhi7[ii]);}
      for(unsigned int ii = 0; ii < muon_MDT_residual_eca_SectorPhi8.size(); ii++) {
	m_muondqafitfunc->FillRMSFromCharProfile(muon_MDT_residual_ecc_SectorPhi8[ii], muon_MDT_residualRMS_ecc_SectorPhi8[ii]);}
 
      // Clear Muon Monitoring Histograms 

      //muon_rec_weights.clear();
      muon_rec_pT.clear();
      muon_rec_1overpT.clear();
      muon_rec_charge.clear();
      muon_rec_nStations.clear();
      //muon_num_tracks.clear();

      muon_rec_pos_pT.clear();
      muon_rec_neg_pT.clear();
      muon_rec_ChargeRatio_pT.clear();
   
      muon_rec_charge_vs_eta.clear();
      muon_rec_charge_vs_phi.clear();
      
      muon_rec_pos_pT_Sector01.clear();
      muon_rec_neg_pT_Sector01.clear(); 
      muon_rec_ChargeRatio_pT_Sector01.clear();
      muon_rec_pos_pT_Sector02.clear();
      muon_rec_neg_pT_Sector02.clear();
      muon_rec_ChargeRatio_pT_Sector02.clear();
      muon_rec_pos_pT_Sector03.clear();
      muon_rec_neg_pT_Sector03.clear(); 
      muon_rec_ChargeRatio_pT_Sector03.clear();
      muon_rec_pos_pT_Sector04.clear();
      muon_rec_neg_pT_Sector04.clear();
      muon_rec_ChargeRatio_pT_Sector04.clear();
      muon_rec_pos_pT_Sector05.clear();
      muon_rec_neg_pT_Sector05.clear(); 
      muon_rec_ChargeRatio_pT_Sector05.clear();
      muon_rec_pos_pT_Sector06.clear();
      muon_rec_neg_pT_Sector06.clear();
      muon_rec_ChargeRatio_pT_Sector06.clear();
      muon_rec_pos_pT_Sector07.clear();
      muon_rec_neg_pT_Sector07.clear(); 
      muon_rec_ChargeRatio_pT_Sector07.clear();
      muon_rec_pos_pT_Sector08.clear();
      muon_rec_neg_pT_Sector08.clear();
      muon_rec_ChargeRatio_pT_Sector08.clear();
      muon_rec_pos_pT_Sector09.clear();
      muon_rec_neg_pT_Sector09.clear();
      muon_rec_ChargeRatio_pT_Sector09.clear();
      muon_rec_pos_pT_Sector10.clear();
      muon_rec_neg_pT_Sector10.clear();
      muon_rec_ChargeRatio_pT_Sector10.clear();
      muon_rec_pos_pT_Sector11.clear();
      muon_rec_neg_pT_Sector11.clear();
      muon_rec_ChargeRatio_pT_Sector11.clear();
      muon_rec_pos_pT_Sector12.clear();
      muon_rec_neg_pT_Sector12.clear();
      muon_rec_ChargeRatio_pT_Sector12.clear();
      muon_rec_pos_pT_Sector13.clear();
      muon_rec_neg_pT_Sector13.clear();
      muon_rec_ChargeRatio_pT_Sector13.clear();
      muon_rec_pos_pT_Sector14.clear();
      muon_rec_neg_pT_Sector14.clear();
      muon_rec_ChargeRatio_pT_Sector14.clear();
      muon_rec_pos_pT_Sector15.clear();
      muon_rec_neg_pT_Sector15.clear();
      muon_rec_ChargeRatio_pT_Sector15.clear();
      muon_rec_pos_pT_Sector16.clear();
      muon_rec_neg_pT_Sector16.clear();
      muon_rec_ChargeRatio_pT_Sector16.clear();
      
      muon_rec_pos_pT_A.clear();
      muon_rec_neg_pT_A.clear();
      muon_rec_ChargeRatio_pT_A.clear();
      
      muon_rec_pos_pT_C.clear();
      muon_rec_neg_pT_C.clear();
      muon_rec_ChargeRatio_pT_C.clear();
      
      muon_rec_pos_pT_Up.clear();
      muon_rec_neg_pT_Up.clear();
      muon_rec_ChargeRatio_pT_up.clear();
      
      muon_rec_pos_pT_Down.clear();
      muon_rec_neg_pT_Down.clear();
      muon_rec_ChargeRatio_pT_dw.clear();
     
      muon_rec_HighpT.clear();
      muon_rec_pos_HighpT.clear();
      muon_rec_neg_HighpT.clear();

      muon_rec_pos_pT_up_dw_diff.clear();
      muon_rec_neg_pT_up_dw_diff.clear();
      muon_rec_pos_pT_A_C_diff.clear();
      muon_rec_neg_pT_A_C_diff.clear();
       
      muon_rec_eta_phi.clear();
      muon_rec_eta_phi_3moreStations.clear();
      muon_rec_eta_phi_2lessStations.clear();

      muon_rec_locX.clear();
      muon_rec_error_locX.clear();

      muon_rec_locY.clear();
      muon_rec_error_locY.clear();

      muon_rec_locR.clear();
      muon_rec_error_locR.clear();

      muon_rec_phi.clear();
      muon_rec_error_phi.clear();

      muon_rec_theta.clear();
      muon_rec_error_theta.clear();

      muon_rec_qOverP.clear();
      muon_rec_error_qOverP.clear();

      muon_rec_posX.clear();
      muon_rec_posY.clear();
      muon_rec_posZ.clear();
      muon_rec_posR.clear();

      muon_glbl_residual.clear();
      muon_MDT_residual.clear();
      muon_RPC_residual.clear();
      muon_TGC_residual.clear();
      muon_CSC_residual.clear();

      muon_glbl_pull.clear();
      muon_MDT_pull.clear();
      muon_RPC_pull.clear();
      muon_TGC_pull.clear();
      muon_CSC_pull.clear();

      muon_RPC_eta_residual.clear();
      muon_RPC_phi_residual.clear();
      muon_TGC_eta_residual.clear();
      muon_TGC_phi_residual.clear();
      muon_CSC_eta_residual.clear();
      muon_CSC_phi_residual.clear();

      muon_RPC_pull_eta.clear();
      muon_RPC_pull_phi.clear();
      muon_TGC_pull_eta.clear();
      muon_TGC_pull_phi.clear();
      muon_CSC_pull_eta.clear();
      muon_CSC_pull_phi.clear(); 
      
      muon_MDT_residual_pT.clear(); 
      muon_MDT_residual_pT_RMS.clear(); 
      
      muon_MDT_residual_BA_2stations.clear();
      muon_MDT_residual_BC_2stations.clear(); 
      muon_MDT_residual_EA_2stations.clear();
      muon_MDT_residual_EC_2stations.clear(); 
      
      muon_MDT_residual_BA.clear();
      muon_MDT_residual_BC.clear(); 
      muon_MDT_residual_EA.clear();
      muon_MDT_residual_EC.clear(); 
      
      muon_MDT_residual_BA_fit.clear();
      muon_MDT_residual_BC_fit.clear(); 
      muon_MDT_residual_EA_fit.clear();
      muon_MDT_residual_EC_fit.clear(); 
      
      muon_MDT_pull_sideA_RMS.clear();
      muon_MDT_pull_sideC_RMS.clear();
      
      muon_MDT_residual_sideA_RMS.clear();
      muon_MDT_residual_sideC_RMS.clear(); 

      muon_MDT_residual_BA_RMS_2stations.clear();
      muon_MDT_residual_BC_RMS_2stations.clear(); 
      muon_MDT_residual_EA_RMS_2stations.clear();
      muon_MDT_residual_EC_RMS_2stations.clear(); 

      muon_MDT_residual_BA_RMS.clear();
      muon_MDT_residual_BC_RMS.clear(); 
      muon_MDT_residual_EA_RMS.clear();
      muon_MDT_residual_EC_RMS.clear(); 
        
      muon_RPC_eta_residual_RMS.clear();
      muon_RPC_phi_residual_RMS.clear();  
      
      muon_TGC_residual_wire_RMS.clear();
      muon_TGC_residual_strip_RMS.clear();  
        
      muon_CSC_eta_residual_RMS.clear();
      muon_CSC_phi_residual_RMS.clear();
      
      muon_TGC_residual_EndCapWire_phi.clear();
      muon_TGC_residual_EndCapStrip_phi.clear();
      muon_TGC_residual_ForwardWire_phi.clear();
      muon_TGC_residual_ForwardStrip_phi.clear();


      //Track Parameters at Perigee and their error
      muon_perigee_qOverP.clear();    
      muon_perigee_P.clear();    
      muon_perigee_d0.clear();     
      muon_perigee_z0.clear();     
      muon_perigee_phi0.clear();     
      muon_perigee_theta.clear();     
      muon_perigee_eta.clear();     
      
      muon_rec_chi2.clear();     
      muon_rec_DegreesOfFreedom.clear();     

      muon_rec_pT_vs_eta.clear();
      muon_rec_pT_vs_phi.clear();

      muon_perigee_error_d0.clear();     
      muon_perigee_error_z0.clear();     
      muon_perigee_error_phi0.clear();     
      muon_perigee_error_theta0.clear();     
      muon_perigee_error_qOverP.clear();

      muon_MDT_residual_barrel_SectorPhi1.clear();
      muon_MDT_residual_barrel_SectorPhi2.clear();
      muon_MDT_residual_barrel_SectorPhi3.clear();
      muon_MDT_residual_barrel_SectorPhi4.clear();
      muon_MDT_residual_barrel_SectorPhi5.clear();
      muon_MDT_residual_barrel_SectorPhi6.clear();
      muon_MDT_residual_barrel_SectorPhi7.clear();
      muon_MDT_residual_barrel_SectorPhi8.clear();

      muon_MDT_residual_eca_SectorPhi1.clear();
      muon_MDT_residual_eca_SectorPhi2.clear();
      muon_MDT_residual_eca_SectorPhi3.clear();
      muon_MDT_residual_eca_SectorPhi4.clear();
      muon_MDT_residual_eca_SectorPhi5.clear();
      muon_MDT_residual_eca_SectorPhi6.clear();
      muon_MDT_residual_eca_SectorPhi7.clear();
      muon_MDT_residual_eca_SectorPhi8.clear();

      muon_MDT_residual_ecc_SectorPhi1.clear();
      muon_MDT_residual_ecc_SectorPhi2.clear();
      muon_MDT_residual_ecc_SectorPhi3.clear();
      muon_MDT_residual_ecc_SectorPhi4.clear();
      muon_MDT_residual_ecc_SectorPhi5.clear();
      muon_MDT_residual_ecc_SectorPhi6.clear();
      muon_MDT_residual_ecc_SectorPhi7.clear();
      muon_MDT_residual_ecc_SectorPhi8.clear();

      muon_MDT_residualRMS_barrel_SectorPhi1.clear();
      muon_MDT_residualRMS_barrel_SectorPhi2.clear();
      muon_MDT_residualRMS_barrel_SectorPhi3.clear();
      muon_MDT_residualRMS_barrel_SectorPhi4.clear();
      muon_MDT_residualRMS_barrel_SectorPhi5.clear();
      muon_MDT_residualRMS_barrel_SectorPhi6.clear();
      muon_MDT_residualRMS_barrel_SectorPhi7.clear();
      muon_MDT_residualRMS_barrel_SectorPhi8.clear();

      muon_MDT_residualRMS_eca_SectorPhi1.clear();
      muon_MDT_residualRMS_eca_SectorPhi2.clear();
      muon_MDT_residualRMS_eca_SectorPhi3.clear();
      muon_MDT_residualRMS_eca_SectorPhi4.clear();
      muon_MDT_residualRMS_eca_SectorPhi5.clear();
      muon_MDT_residualRMS_eca_SectorPhi6.clear();
      muon_MDT_residualRMS_eca_SectorPhi7.clear();
      muon_MDT_residualRMS_eca_SectorPhi8.clear();

      muon_MDT_residualRMS_ecc_SectorPhi1.clear();
      muon_MDT_residualRMS_ecc_SectorPhi2.clear();
      muon_MDT_residualRMS_ecc_SectorPhi3.clear();
      muon_MDT_residualRMS_ecc_SectorPhi4.clear();
      muon_MDT_residualRMS_ecc_SectorPhi5.clear();
      muon_MDT_residualRMS_ecc_SectorPhi6.clear();
      muon_MDT_residualRMS_ecc_SectorPhi7.clear();
      muon_MDT_residualRMS_ecc_SectorPhi8.clear();

    } // isEndOfRun

  } //environment


  return sc;   
}

//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode MuonGenericTracksMon:: finalize() 
{ 

  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;

  // init message stream
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached

  if (m_debuglevel) m_log << MSG::DEBUG << "MuonGenericTracksMon::finalize() " << endreq;

  //StatusCode sc = StatusCode::SUCCESS;
  sc = StatusCode::SUCCESS;

  // Clear Muon Monitoring Histograms 

  //muon_rec_weights.clear();
  muon_rec_pT.clear();
  muon_rec_1overpT.clear();
  muon_rec_charge.clear();
  muon_rec_nStations.clear();
  //muon_num_tracks.clear();

  muon_rec_pos_pT.clear();
  muon_rec_neg_pT.clear();
  muon_rec_ChargeRatio_pT.clear();
  
  muon_rec_charge_vs_eta.clear();
  muon_rec_charge_vs_phi.clear();
      
  muon_rec_pos_pT_Sector01.clear();
  muon_rec_neg_pT_Sector01.clear(); 
  muon_rec_ChargeRatio_pT_Sector01.clear();
  muon_rec_pos_pT_Sector02.clear();
  muon_rec_neg_pT_Sector02.clear();
  muon_rec_ChargeRatio_pT_Sector02.clear();
  muon_rec_pos_pT_Sector03.clear();
  muon_rec_neg_pT_Sector03.clear(); 
  muon_rec_ChargeRatio_pT_Sector03.clear();
  muon_rec_pos_pT_Sector04.clear();
  muon_rec_neg_pT_Sector04.clear();
  muon_rec_ChargeRatio_pT_Sector04.clear();
  muon_rec_pos_pT_Sector05.clear();
  muon_rec_neg_pT_Sector05.clear(); 
  muon_rec_ChargeRatio_pT_Sector05.clear();
  muon_rec_pos_pT_Sector06.clear();
  muon_rec_neg_pT_Sector06.clear();
  muon_rec_ChargeRatio_pT_Sector06.clear();
  muon_rec_pos_pT_Sector07.clear();
  muon_rec_neg_pT_Sector07.clear(); 
  muon_rec_ChargeRatio_pT_Sector07.clear();
  muon_rec_pos_pT_Sector08.clear();
  muon_rec_neg_pT_Sector08.clear();
  muon_rec_ChargeRatio_pT_Sector08.clear();
  muon_rec_pos_pT_Sector09.clear();
  muon_rec_neg_pT_Sector09.clear();
  muon_rec_ChargeRatio_pT_Sector09.clear();
  muon_rec_pos_pT_Sector10.clear();
  muon_rec_neg_pT_Sector10.clear();
  muon_rec_ChargeRatio_pT_Sector10.clear();
  muon_rec_pos_pT_Sector11.clear();
  muon_rec_neg_pT_Sector11.clear();
  muon_rec_ChargeRatio_pT_Sector11.clear();
  muon_rec_pos_pT_Sector12.clear();
  muon_rec_neg_pT_Sector12.clear();
  muon_rec_ChargeRatio_pT_Sector12.clear();
  muon_rec_pos_pT_Sector13.clear();
  muon_rec_neg_pT_Sector13.clear();
  muon_rec_ChargeRatio_pT_Sector13.clear();
  muon_rec_pos_pT_Sector14.clear();
  muon_rec_neg_pT_Sector14.clear();
  muon_rec_ChargeRatio_pT_Sector14.clear();
  muon_rec_pos_pT_Sector15.clear();
  muon_rec_neg_pT_Sector15.clear();
  muon_rec_ChargeRatio_pT_Sector15.clear();
  muon_rec_pos_pT_Sector16.clear();
  muon_rec_neg_pT_Sector16.clear();
  muon_rec_ChargeRatio_pT_Sector16.clear();
      
  muon_rec_pos_pT_A.clear();
  muon_rec_neg_pT_A.clear();
  muon_rec_ChargeRatio_pT_A.clear();
      
  muon_rec_pos_pT_C.clear();
  muon_rec_neg_pT_C.clear();
  muon_rec_ChargeRatio_pT_C.clear();
      
  muon_rec_pos_pT_Up.clear();
  muon_rec_neg_pT_Up.clear();
  muon_rec_ChargeRatio_pT_up.clear();
      
  muon_rec_pos_pT_Down.clear();
  muon_rec_neg_pT_Down.clear();
  muon_rec_ChargeRatio_pT_dw.clear();
     
  muon_rec_HighpT.clear();
  muon_rec_pos_HighpT.clear();
  muon_rec_neg_HighpT.clear();

  muon_rec_pos_pT_up_dw_diff.clear();
  muon_rec_neg_pT_up_dw_diff.clear();
  muon_rec_pos_pT_A_C_diff.clear();
  muon_rec_neg_pT_A_C_diff.clear();
       
  muon_rec_eta_phi.clear();
  muon_rec_eta_phi_3moreStations.clear();
  muon_rec_eta_phi_2lessStations.clear();

  muon_rec_locX.clear();
  muon_rec_error_locX.clear();

  muon_rec_locY.clear();
  muon_rec_error_locY.clear();

  muon_rec_locR.clear();
  muon_rec_error_locR.clear();

  muon_rec_phi.clear();
  muon_rec_error_phi.clear();

  muon_rec_theta.clear();
  muon_rec_error_theta.clear();

  muon_rec_qOverP.clear();
  muon_rec_error_qOverP.clear();

  muon_rec_posX.clear();
  muon_rec_posY.clear();
  muon_rec_posZ.clear();
  muon_rec_posR.clear();

  muon_glbl_residual.clear();
  muon_MDT_residual.clear();
  muon_RPC_residual.clear();
  muon_TGC_residual.clear();
  muon_CSC_residual.clear();

  muon_glbl_pull.clear();
  muon_MDT_pull.clear();
  muon_RPC_pull.clear();
  muon_TGC_pull.clear();
  muon_CSC_pull.clear();

  muon_RPC_eta_residual.clear();
  muon_RPC_phi_residual.clear();
  muon_TGC_eta_residual.clear();
  muon_TGC_phi_residual.clear();
  muon_CSC_eta_residual.clear();
  muon_CSC_phi_residual.clear();

  muon_RPC_pull_eta.clear();
  muon_RPC_pull_phi.clear();
  muon_TGC_pull_eta.clear();
  muon_TGC_pull_phi.clear();
  muon_CSC_pull_eta.clear();
  muon_CSC_pull_phi.clear(); 
      
  muon_MDT_residual_pT.clear(); 
  muon_MDT_residual_pT_RMS.clear(); 
      
  muon_MDT_residual_BA_2stations.clear();
  muon_MDT_residual_BC_2stations.clear(); 
  muon_MDT_residual_EA_2stations.clear();
  muon_MDT_residual_EC_2stations.clear(); 
      
  muon_MDT_residual_BA.clear();
  muon_MDT_residual_BC.clear(); 
  muon_MDT_residual_EA.clear();
  muon_MDT_residual_EC.clear(); 
  
  muon_MDT_residual_BA_fit.clear();
  muon_MDT_residual_BC_fit.clear();
  muon_MDT_residual_EA_fit.clear();
  muon_MDT_residual_EC_fit.clear();
  
  muon_MDT_pull_sideA_RMS.clear();
  muon_MDT_pull_sideC_RMS.clear();
   
  muon_MDT_residual_sideA_RMS.clear();
  muon_MDT_residual_sideC_RMS.clear(); 
  
  muon_MDT_residual_BA_RMS_2stations.clear();
  muon_MDT_residual_BC_RMS_2stations.clear(); 
  muon_MDT_residual_EA_RMS_2stations.clear();
  muon_MDT_residual_EC_RMS_2stations.clear(); 
        
  muon_MDT_residual_BA_RMS.clear();
  muon_MDT_residual_BC_RMS.clear(); 
  muon_MDT_residual_EA_RMS.clear();
  muon_MDT_residual_EC_RMS.clear(); 

  muon_RPC_eta_residual_RMS.clear();
  muon_RPC_phi_residual_RMS.clear(); 
   
  muon_TGC_residual_wire_RMS.clear();
  muon_TGC_residual_strip_RMS.clear();  
        
  muon_CSC_eta_residual_RMS.clear();
  muon_CSC_phi_residual_RMS.clear();
  
  muon_TGC_residual_EndCapWire_phi.clear();
  muon_TGC_residual_EndCapStrip_phi.clear();
  muon_TGC_residual_ForwardWire_phi.clear();
  muon_TGC_residual_ForwardStrip_phi.clear();

  //Track Parameters at Perigee and their error
  muon_perigee_qOverP.clear();    
  muon_perigee_P.clear();    
  muon_perigee_d0.clear();     
  muon_perigee_z0.clear();     
  muon_perigee_phi0.clear();     
  muon_perigee_theta.clear();     
  muon_perigee_eta.clear();     
  
  muon_rec_chi2.clear();     
  muon_rec_DegreesOfFreedom.clear();     

  muon_rec_pT_vs_eta.clear();
  muon_rec_pT_vs_phi.clear();

  muon_perigee_error_d0.clear();     
  muon_perigee_error_z0.clear();     
  muon_perigee_error_phi0.clear();     
  muon_perigee_error_theta0.clear();     
  muon_perigee_error_qOverP.clear();

  muon_MDT_residual_barrel_SectorPhi1.clear();
  muon_MDT_residual_barrel_SectorPhi2.clear();
  muon_MDT_residual_barrel_SectorPhi3.clear();
  muon_MDT_residual_barrel_SectorPhi4.clear();
  muon_MDT_residual_barrel_SectorPhi5.clear();
  muon_MDT_residual_barrel_SectorPhi6.clear();
  muon_MDT_residual_barrel_SectorPhi7.clear();
  muon_MDT_residual_barrel_SectorPhi8.clear();

  muon_MDT_residual_eca_SectorPhi1.clear();
  muon_MDT_residual_eca_SectorPhi2.clear();
  muon_MDT_residual_eca_SectorPhi3.clear();
  muon_MDT_residual_eca_SectorPhi4.clear();
  muon_MDT_residual_eca_SectorPhi5.clear();
  muon_MDT_residual_eca_SectorPhi6.clear();
  muon_MDT_residual_eca_SectorPhi7.clear();
  muon_MDT_residual_eca_SectorPhi8.clear();

  muon_MDT_residual_ecc_SectorPhi1.clear();
  muon_MDT_residual_ecc_SectorPhi2.clear();
  muon_MDT_residual_ecc_SectorPhi3.clear();
  muon_MDT_residual_ecc_SectorPhi4.clear();
  muon_MDT_residual_ecc_SectorPhi5.clear();
  muon_MDT_residual_ecc_SectorPhi6.clear();
  muon_MDT_residual_ecc_SectorPhi7.clear();
  muon_MDT_residual_ecc_SectorPhi8.clear();

  muon_MDT_residualRMS_barrel_SectorPhi1.clear();
  muon_MDT_residualRMS_barrel_SectorPhi2.clear();
  muon_MDT_residualRMS_barrel_SectorPhi3.clear();
  muon_MDT_residualRMS_barrel_SectorPhi4.clear();
  muon_MDT_residualRMS_barrel_SectorPhi5.clear();
  muon_MDT_residualRMS_barrel_SectorPhi6.clear();
  muon_MDT_residualRMS_barrel_SectorPhi7.clear();
  muon_MDT_residualRMS_barrel_SectorPhi8.clear();

  muon_MDT_residualRMS_eca_SectorPhi1.clear();
  muon_MDT_residualRMS_eca_SectorPhi2.clear();
  muon_MDT_residualRMS_eca_SectorPhi3.clear();
  muon_MDT_residualRMS_eca_SectorPhi4.clear();
  muon_MDT_residualRMS_eca_SectorPhi5.clear();
  muon_MDT_residualRMS_eca_SectorPhi6.clear();
  muon_MDT_residualRMS_eca_SectorPhi7.clear();
  muon_MDT_residualRMS_eca_SectorPhi8.clear();

  muon_MDT_residualRMS_ecc_SectorPhi1.clear();
  muon_MDT_residualRMS_ecc_SectorPhi2.clear();
  muon_MDT_residualRMS_ecc_SectorPhi3.clear();
  muon_MDT_residualRMS_ecc_SectorPhi4.clear();
  muon_MDT_residualRMS_ecc_SectorPhi5.clear();
  muon_MDT_residualRMS_ecc_SectorPhi6.clear();
  muon_MDT_residualRMS_ecc_SectorPhi7.clear();
  muon_MDT_residualRMS_ecc_SectorPhi8.clear();
  
  
  return sc;
}


//--------------------------------------------------------------------------------------- 
StatusCode MuonGenericTracksMon::setupTools()
//--------------------------------------------------------------------------------------- 
{
  //initializing tools
  // Retrieve the StoreGate service
  StatusCode sc = StatusCode::SUCCESS;
  sc = service( "StoreGateSvc", m_storeGate );

  if ( sc.isFailure() ){
    m_log << MSG::FATAL << "Unable to retrieve the StoreGate service... Exiting!" << endreq;
    return sc;
  }
  if (m_debuglevel) m_log << MSG::DEBUG << "Defined detector service" << endreq;
 
  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    m_log << MSG::ERROR << " Cannot get ActiveStoreSvc " << endreq;
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return sc;
  }   
  
  // Retrieve the MuonDetectorManager  
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "Cannot get MuonDetectorManager from detector store" << endreq;
    return sc;
  }  
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the MuonDetectorManager from detector store. " << endreq;
  }
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "Cannot get MdtIdHelper" << endreq;
    return sc;
  }  
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the MdtIdHelper " << endreq;
  }  
 
  sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
    {
      m_log << MSG::ERROR << "Can't retrieve RpcIdHelper" << endreq;
      return sc;
    }	 
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the RpcIdHelper " << endreq;
  }  
  sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure())
    {
      m_log << MSG::ERROR << "Can't retrieve TgcIdHelper" << endreq;
      return sc;
    }	   
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the TgcIdHelper " << endreq;
  }
  
  sc = m_muonHitSummaryTool.retrieve();
  if (sc.isFailure()){
    m_log << MSG::ERROR << "Can't retrieve " << m_muonHitSummaryTool << endreq;
    return sc;
  }	   
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the muonHitSummaryTool " << endreq;
  }
  
  sc = m_pullCalculator.retrieve();
  if (sc.isFailure())
    {
      m_log << MSG::ERROR << "Can't retrieve " << m_pullCalculator << endreq;
      return sc;
    }	   
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the " << m_pullCalculator << endreq;
  }   

  sc = m_helperTool.retrieve();
  if (sc.isSuccess()){
    if (m_debuglevel) m_log<<MSG::DEBUG << "Retrieved " << m_helperTool << endreq;
  }else{
    m_log<<MSG::FATAL<<"Could not get " << m_helperTool <<endreq; 
    return sc;
  }
  sc = m_printer.retrieve();
  if (sc.isSuccess()){
    m_log<<MSG::DEBUG << "Retrieved " << m_printer << endreq;
  }else{
    m_log<<MSG::FATAL<<"Could not get " << m_printer <<endreq; 
    return sc;
  }
  sc = m_idHelperTool.retrieve();
  if (sc.isSuccess()){
    if (m_debuglevel) m_log<<MSG::DEBUG << "Retrieved " << m_idHelperTool << endreq;
  }else{
    m_log<<MSG::FATAL<<"Could not get " << m_idHelperTool <<endreq; 
    return sc;
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

  sc = m_muonHitSummaryTool.retrieve();
  if (sc.isFailure()){
    m_log << MSG::ERROR << "Can't retrieve " << m_muonHitSummaryTool << endreq;
    return sc;
  }     
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the muonHitSummaryTool " << endreq;
  }

  if ( sc = m_trackSummaryTool.retrieve().isFailure() ) {
    m_log << MSG::ERROR << "Unable to retrieve" << m_trackSummaryTool << endreq;
    return sc;
  }

  sc = m_muondqafitfunc.retrieve();
  if (sc.isSuccess()){
    if (m_debuglevel) m_log<<MSG::DEBUG << "Retrieved " << m_muondqafitfunc << endreq;
  }else{
    m_log<<MSG::FATAL<<"Could not get " << m_muondqafitfunc <<endreq; 
    return sc;
  }
  return sc;
}  
  
 

