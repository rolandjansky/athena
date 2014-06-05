/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonInSituResolution.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonPhysicsMonitoring/MuonInsituResolution.h"

//================ Constructor =================================================

//MuonInsituResolution::MuonInsituResolution(const std::string& name, ISvcLocator* pSvcLocator):Algorithm(name,pSvcLocator), m_log(msgSvc(),name),  m_InsituPerformanceTool("InsituPerformanceTool/TestTool"), m_InsituDatabaseTool("InsituDatabaseTool/TestTool")
MuonInsituResolution::MuonInsituResolution(const std::string& type, const std::string& name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_InsituPerformanceTool("InsituPerformanceTool/TestTool") 
					  //   m_InsituDatabaseTool("InsituDatabaseTool/TestTool") 
{
  //declareProperty("MuonTrackContainerName",		m_MSTrackContainerName			= "MuonboyTrackParticles" );
  //declareProperty("CominedMuonContainerName",		m_CombinedMuonTracksContainerName	= "StacoMuonCollection" );
  declareProperty("tracksName",                         m_tracksName);
  declareProperty( "MCTruthCollection",                 m_Truthcurrentcontainer_key = "SpclMC", "Input location of MCTruthCollection" );
  declareProperty("MuonTrackContainerName",		m_MSTrackContainerName);
  declareProperty("MuonTrackContainerFlag",		m_MSTrackContainerFlag);
  declareProperty("CominedMuonContainerName",		m_CombinedMuonTracksContainerName);
  declareProperty("CombinedMuonTracksContainerFlag",	m_CombinedMuonTracksContainerFlag);
  //declareProperty("InsituDatabaseTool",			m_InsituDatabaseTool);
  declareProperty("InsituPerformanceTool",		m_InsituPerformanceTool);
  declareProperty("TrackConeAssociationDistance",       d_assoziationDistance=0.05);
  declareProperty("MCTruthTrackRootFileName",		m_MCTruthTrackRootFileName="../share/ResolutionNtuple.root");
	
}

//================ Destructor =================================================

MuonInsituResolution::~MuonInsituResolution()
{
  ATH_MSG_INFO("Deleting MuonInsituResolution");
}


//================ Initialisation =================================================

StatusCode MuonInsituResolution::initialize() 
{  

  StatusCode sc = StatusCode::SUCCESS;  
  /** Initialize tools and services */ 
  ATH_MSG_DEBUG( "MuonInsituResolution::initialize() to setup tools/services" ); 
  /// Getting InsituPerformanceTools
  sc=m_InsituPerformanceTool.retrieve();
  if (sc.isFailure())
    {
      ATH_MSG_FATAL("My m_InsituPerformanceTool Service not found");
      return sc;
    }
  sc=m_InsituPerformanceTool->initialize();

  /// get StoreGate service
  sc = service("StoreGateSvc",m_storeGate);
  if (sc.isFailure())
    {
      ATH_MSG_FATAL("StoreGate service not found !");
      return sc;
    }

  //   /// Getting InsituPerformanceTools
  //   if (m_InsituDatabaseTool.retrieve().isFailure())
  //     {
  //       m_log << MSG::FATAL << "My m_InsituDatabaseTool Service not found" << endreq;
  //       //return StatusCode::FAILURE;
  //     }

  //m_InsituDatabaseTool->initialize();

  return sc;
}


//================ bookHistograms =================================================

StatusCode MuonInsituResolution::bookHistogramsRecurrent()
{	
  //StatusCode sc;
  StatusCode sc = StatusCode::SUCCESS;

 
  ATH_MSG_DEBUG( "MuonInsituResolution::bookHistograms() to setup tools/services");
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) { 
    
    if(newEventsBlock){}
    if(newLumiBlock){}
    if(newRun)
      {
	//int h_index = -1;
	for(int ii=0; ii<(int)m_MSTrackContainerName.size(); ii++)
	  {
	    if (m_MSTrackContainerFlag[ii] == 0) continue;
	    //h_index++;

	    /// Histograms for Resolution: Binning with be changed later by APMuonResolutionMatrix

	    ATH_MSG_DEBUG( "Booking histograms for: " << m_MSTrackContainerName[ii]);

	    m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_MCTruth_Eta"]   = new TH1F("MuonResolution_MCTruth_Eta","Efficiency",1,0,1);
	    m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_Insitu_Eta"]    = new TH1F("MuonResolution_Insitu_Eta","Efficiency",1,0,1);
	    m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_ZBoson_InData"] = new TH1F("MuonResolution_ZBoson_InData","Efficiency",1,0,1);
	    m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_ZBoson_MC"]     = new TH1F("MuonResolution_ZBoson_MC","Efficiency",1,0,1);

	    ATH_MSG_DEBUG("Done defining the map of histograms for " << m_MSTrackContainerName[ii]);

	    std::string m_generic_path = "Muon/MuonPhysicsMonitoring/MuonResolution/"+m_MSTrackContainerName[ii];

	    MonGroup ms_mctruth_eta ( this, m_generic_path+"/MuonResolution_MCTruth_Eta",  run, ATTRIB_UNMANAGED );
	    MonGroup ms_insitu_eta ( this, m_generic_path+"/MuonResolution_Insitu_Eta",  run, ATTRIB_UNMANAGED );
	    MonGroup ms_zboson_indata ( this, m_generic_path+"/MuonResolution_ZBoson_InData",  run, ATTRIB_UNMANAGED );
	    MonGroup ms_zboson_mc ( this, m_generic_path+"/MuonResolution_ZBoson_MC",  run, ATTRIB_UNMANAGED );

	    sc=ms_mctruth_eta.regHist(m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_MCTruth_Eta"]);
	    if(sc.isFailure())
	      { 
		ATH_MSG_FATAL("m_histograms[" << m_MSTrackContainerName[ii] << "\"MuonResolution_MCTruth_Eta\"] Failed to register histogram ");
		return sc;
	      }
	    sc=ms_insitu_eta.regHist(m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_Insitu_Eta"]);
	    if(sc.isFailure())
	      { 
		ATH_MSG_FATAL("m_histograms[" << m_MSTrackContainerName[ii] << "\"MuonResolution_Insitu_Eta\"] Failed to register histogram ");
		return sc;
	      }
	    sc=ms_zboson_indata.regHist(m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_ZBoson_InData"]);
	    if(sc.isFailure())
	      { 
		ATH_MSG_FATAL("m_histograms[" << m_MSTrackContainerName[ii] << "\"MuonResolution_ZBoson_InData\"] Failed to register histogram ");
		return sc;
	      }
	    sc=ms_zboson_mc.regHist(m_histograms[m_MSTrackContainerName[ii]+"_MuonResolution_ZBoson_MC"]);
	    if(sc.isFailure())
	      { 
		ATH_MSG_FATAL("m_histograms[" << m_MSTrackContainerName[ii] << "\"MuonResolution_ZBoson_MC\"] Failed to register histogram ");
		return sc;
	      }

	    ATH_MSG_DEBUG("Defining Matrices for: " << m_MSTrackContainerName[ii] );

	    /// Initializing the Resolution
	    // m_apMuonResolutionMatrix[m_MSTrackContainerName[ii]+"Muon_Insitu"]	     = new APMuonResolutionMatrix();
	    //m_apMuonResolutionMatrix[m_MSTrackContainerName[ii]+"Muon_MCTruth"]     = new APMuonResolutionMatrix();

	    // m_APZmumuResonanceMatrix[m_MSTrackContainerName[ii]+"Muon_ZBoson_Data"] = new APZmumuResonanceMatrix();
	    //m_APZmumuResonanceMatrix[m_MSTrackContainerName[ii]+"Muon_ZBoson_MC"]   = new APZmumuResonanceMatrix();

	    ATH_MSG_DEBUG("Done with Matrices for: " << m_MSTrackContainerName[ii] );	     

	  } // Loop over Collections

      }// if NewRun
     
  } //environment
  
  return sc;
}

//================ Finalisation =================================================

StatusCode MuonInsituResolution::finalize()
{
  return StatusCode::SUCCESS;
}

//================ procHistograms =================================================
// Code entered here will be executed once at the end of the program run.     

StatusCode MuonInsituResolution::procHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
	
  ATH_MSG_DEBUG("finalize"); 
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

    if(endOfEventsBlock){}
    if(endOfLumiBlock){} 
    if(endOfRun){

      // 	APInsituDatabaseEntry apEntry;
      //        
      //         //int h_index = -1;
      //     		
      //         for (int ii=0; ii<(int)m_MSTrackContainerName.size(); ii++)
      //         {	
      // 	   if (m_debuglevel) m_log << MSG::DEBUG << " Inside loop over TrackCollections " << m_MSTrackContainerName[ii] << endreq;
      //            if (m_MSTrackContainerFlag[ii] == 0) continue;
      // 	   //++h_index;
      // 
      // 	   //readMCTruthResolutionTracks();
      // 	   
      // 	   double scale=1.0;
      // 	   double res=1.00;
      // 	   
      // 	   if (m_debuglevel) m_log << MSG::DEBUG << " Calling findMomentumscale for" << m_MSTrackContainerName[ii] << endreq;
      // 	   scale 	= findMomentumScale(scale-0.1, scale+0.1, res, m_MSTrackContainerName[ii]);
      // 	   
      // 	   if (m_debuglevel) m_log << MSG::DEBUG << " Done with momentumscale, Calling resolutionscale for" << m_MSTrackContainerName[ii] << endreq;
      // 	   res 	= findResolutionScale(res-0.5, res+2.0, scale, m_MSTrackContainerName[ii]);
      // 	   
      // 	   if (m_debuglevel) m_log << MSG::DEBUG << " Done with resolutionscale, Calling filldeterminedresolutionmatrix for" << m_MSTrackContainerName[ii] << endreq;
      // 	   fillDeterminedResolutionMatrix(scale, res, m_MSTrackContainerName[ii]);
      // 
      // 	   //if (m_debuglevel) m_log << MSG::DEBUG << " Done with resolutionmatrix, Filling Muonresolmatrix" << endreq;
      // 	   //m_apMuonResolutionMatrix[m_MSTrackContainerName[ii]+"Muon_MCTruth"]->plotResolutionProjectionToEta(m_histograms[m_MSTrackContainerName[ii]+"MuonResolution_MCTruth_Eta"]);
      // 	   
      // 	   if (m_debuglevel) m_log << MSG::DEBUG << " Done with muonresolmatrix, Filling Muonresolmatrix" << endreq;
      // 	   m_apMuonResolutionMatrix[m_MSTrackContainerName[ii]+"Muon_Insitu"]->plotResolutionProjectionToEta(m_histograms[m_MSTrackContainerName[ii]+"MuonResolution_Insitu_Eta"]);
      // 	   
      // 	   if (m_debuglevel) m_log << MSG::DEBUG << " Done with muonresolutionmatrix, Filling zmumuresolmatrix" << endreq;
      // 	   m_APZmumuResonanceMatrix[m_MSTrackContainerName[ii]+"Muon_ZBoson_Data"]->plotOverallProjection(m_histograms[m_MSTrackContainerName[ii]+"MuonResolution_ZBoson_InData"]);
      // 
      // 	} //Loop over Track Collections

    } //EndOfRun
  
  } //environment
  
  ATH_MSG_DEBUG(" Done.");

  return sc;
}

//================ fillHistograms =================================================
// Code entered here will be executed once per event

StatusCode MuonInsituResolution::fillHistograms()
{	
  StatusCode sc = StatusCode::SUCCESS;
 
  ATH_MSG_DEBUG(" execute"); 
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
 
    for (int ii=0; ii<(int)m_MSTrackContainerName.size(); ii++)
      {	
	if (m_MSTrackContainerFlag[ii] == 0) continue;
 
	/// Loading Muon Track Collection
	trackTES = 0;
	sc=m_storeGate->retrieve( trackTES, m_MSTrackContainerName[ii]);
	if( sc.isFailure()  ||  !trackTES ) 
	  {
	    ATH_MSG_WARNING("No "<< m_MSTrackContainerName[ii] <<" container found in TDS" ); 
	    return sc;
	  }
	else ATH_MSG_DEBUG("Successfully "<< m_MSTrackContainerName[ii] <<" container found in TDS");
	
	//+++ Arely: Is this used?!?
	/*	
       /// Loading CombinedMuon Track Collection
       muonTDS=0;
       sc=m_storeGate->retrieve( muonTDS, m_CombinedMuonTracksContainerName);
       if( sc.isFailure()  ||  !muonTDS ) 
       {
       m_log << MSG::WARNING	<< "No AOD "<<m_CombinedMuonTracksContainerName<<" container of muons found in TDS"<< endreq; 
       return StatusCode::FAILURE;
       }  
       m_log << MSG::DEBUG << "MuonContainer successfully retrieved" << endreq;
	*/

	//determineMuonResolution_MCTruth(m_MSTrackContainerName[ii]);
	sc=determineZBosonMasses(m_MSTrackContainerName[ii]);

      } // Loop over Track Collections
  } //environment
  else {
 
    for (int ii=0; ii<(int)m_MSTrackContainerName.size(); ii++)
      {	
	if (m_MSTrackContainerFlag[ii] == 0) continue;
 
	/// Loading Muon Track Collection
	trackTES = 0;
	sc=m_storeGate->retrieve( trackTES, m_MSTrackContainerName[ii]);
	if( sc.isFailure()  ||  !trackTES ) 
	  {
	    ATH_MSG_WARNING("No "<<m_MSTrackContainerName[ii]<<" container found in TDS"); 
	    return sc;
	  }
	else ATH_MSG_DEBUG("Successfully "<< m_MSTrackContainerName[ii] <<" container found in TDS" ); 	 
	sc=determineMuonResolution_MCTruth(m_MSTrackContainerName[ii]);
	sc=determineZBosonMasses(m_MSTrackContainerName[ii]); 
      } // Loop over Track Collections    
   
  }  
  return sc;

}


StatusCode MuonInsituResolution::writeToStoreGate()
{
  StatusCode sc = StatusCode::SUCCESS;
  /*	MsgStream log(msgSvc(), name());
    log <<MSG::DEBUG <<"in writeToStoreGate()" <<endreq;

    // locate the conditions store ptr to it.
    sc = service("DetectorStore", m_detStore);
    if (!sc.isSuccess() || 0 == m_detStore)  
    {
    log <<MSG::ERROR <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
    }
	
    // Create APInsituDatabaseEntry
    APInsituDatabaseEntry* apMuonInsituDatabaseEntry =   new APInsituDatabaseEntry;
	
    apMuonInsituDatabaseEntry->setAPEfficiencyMatrix((*m_apMuonEfficiencyMatrix["Muon_MCTruth"]));
    sc = m_detStore->record(apMuonInsituDatabaseEntry, "/IOVDbInsituPerformance/IOVDbMuonPerformance");
    if (sc.isFailure()) 
    {
    log <<MSG::ERROR <<"Could not record APInsituDatabaseEntry" <<endreq;
    return( StatusCode::FAILURE);
    }*/
  return sc;
}


StatusCode MuonInsituResolution::determineMuonResolution_MCTruth(std::string m_Truthcurrentcontainer_key)
{
  StatusCode sc = StatusCode::SUCCESS;

  const TruthParticleContainer*  mcpartTES;
  sc=m_storeGate->retrieve( mcpartTES, m_Truthcurrentcontainer_key);
  if( sc.isFailure()  ||  !mcpartTES ) 
    {
      ATH_MSG_FATAL("No AOD MC truth particle container found in TDS"); 
      return sc;
    }
  else 
    ATH_MSG_DEBUG("AOD MC truth particle container successfully retrieved"); 

  /// Iterate over Collections
  TruthParticleContainer::const_iterator trueItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator trueItrE = mcpartTES->end();

  for (; trueItr != trueItrE; ++trueItr)
    {
      if (((*trueItr)->pt()>20000.) && (fabs((*trueItr)->eta())<2.5) && (abs((*trueItr)->pdgId())==13) && ((*trueItr)->status()==1))
	{
	  double muonPt;
	  if ( canAssociateMuonTrack((*trueItr), muonPt) )
	    {
	      //m_apMuonResolutionMatrix[mstrackcollection+"Muon_MCTruth"]->addRes((*trueItr), 1.0-((*trueItr)->pt())/muonPt);
	    }
	}
    }

  return sc;
}


StatusCode MuonInsituResolution::determineZBosonMasses(std::string /*mstrackcollection*/)
{
 // double dMass=0.;
  StatusCode sc = StatusCode::SUCCESS;  
  Rec::TrackParticleContainer::const_iterator muon1Itr  = trackTES->begin();
  Rec::TrackParticleContainer::const_iterator muon1ItrE = trackTES->end();
  for (; muon1Itr != muon1ItrE; ++muon1Itr) 
    {
      Rec::TrackParticleContainer::const_iterator muon2Itr  = trackTES->begin();
      Rec::TrackParticleContainer::const_iterator muon2ItrE = trackTES->end();
      for (; muon2Itr != muon2ItrE; ++muon2Itr)
	{
	  if (((*muon1Itr)->pt()>20000.) && ((*muon2Itr)->pt()>20000.) && ((*muon1Itr)->charge()!=(*muon2Itr)->charge()))
	    {
	      if (m_InsituPerformanceTool->isZBosonCandidate((*muon1Itr), (*muon2Itr))==true)
		{
		 // dMass = m_InsituPerformanceTool->getInvariantMass((*muon1Itr), (*muon2Itr));
		  //m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_Data"]->addZBoson((*muon1Itr)->eta(), (*muon2Itr)->eta(), dMass);
		}
	    }
	}
    }
  return sc;
}

StatusCode MuonInsituResolution::readMCTruthResolutionTracks()
{
  StatusCode sc = StatusCode::SUCCESS;
  TChain *m_tree = new TChain("MuonCTNTuple");
  m_tree->Add(m_MCTruthTrackRootFileName.c_str());

  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_pt_truth_1", &m_track1.pt_truth);
  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_pt_reco_1", &m_track1.pt_rec);
  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_eta_1", &m_track1.eta);
  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_phi_1", &m_track1.phi);
  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_pt_truth_2", &m_track2.pt_truth);
  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_pt_reco_2", &m_track2.pt_rec);
  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_eta_2", &m_track2.eta);
  m_tree->SetBranchAddress("dMuonSpectrometerStandalone_phi_2", &m_track2.phi);
		
  int nNumberOfEvents = static_cast<int>(m_tree->GetEntries());
			
  for(int i = 0; i<nNumberOfEvents; i++)
    {
      m_tree->GetEvent(i);
      m_vecMuon1.push_back(m_track1);
      m_vecMuon2.push_back(m_track2);
    }
  delete m_tree;
  return sc;
}

StatusCode MuonInsituResolution::fillMCTruthZBosonPlots(double scale, double res, std::string /*CurrentTrackCollection*/ )
{
 // double dMass=0.;
  StatusCode sc = StatusCode::SUCCESS;
  double pt1;
  double pt2;

  //m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_MC"]->clearEntries();
  for (int i=0; i<(int)m_vecMuon1.size(); i++)
    {
      pt1=(m_vecMuon1[i].pt_rec-m_vecMuon1[i].pt_truth)*res+m_vecMuon1[i].pt_truth;
      pt2=(m_vecMuon2[i].pt_rec-m_vecMuon2[i].pt_truth)*res+m_vecMuon2[i].pt_truth;
      TLorentzVector muon1;
      TLorentzVector muon2;
      muon1.SetPtEtaPhiM(pt1*scale, m_vecMuon1[i].eta, m_vecMuon1[i].phi, 105.7);
      muon2.SetPtEtaPhiM(pt2*scale, m_vecMuon2[i].eta, m_vecMuon2[i].phi, 105.7);
     // dMass = (muon1+muon2).M();
      //m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_MC"]->addZBoson(m_vecMuon1[i].eta, m_vecMuon2[i].eta, dMass);
    }
  return sc;
}


/*
  StatusCode MuonInsituResolution::fillDeterminedResolutionMatrix(double scale, double res, std::string mstrackcollection)
  {
  StatusCode sc = StatusCode::SUCCESS;
  double pt1;
  double pt2;
  //m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_MC"]->clearEntries();
  for (int i=0; i<(int)m_vecMuon1.size(); i++)
  {
  pt1=(m_vecMuon1[i].pt_rec-m_vecMuon1[i].pt_truth)*res+m_vecMuon1[i].pt_truth;
  pt2=(m_vecMuon2[i].pt_rec-m_vecMuon2[i].pt_truth)*res+m_vecMuon2[i].pt_truth;
  TLorentzVector muon1;
  TLorentzVector muon2;
  muon1.SetPtEtaPhiM(pt1*scale, m_vecMuon1[i].eta, m_vecMuon1[i].phi, 105.7);
  muon2.SetPtEtaPhiM(pt2*scale, m_vecMuon2[i].eta, m_vecMuon2[i].phi, 105.7);
  //m_apMuonResolutionMatrix[mstrackcollection+"Muon_Insitu"]->addRes(muon1, 1.0-m_vecMuon1[i].pt_truth/muon1.Pt());
  //m_apMuonResolutionMatrix[mstrackcollection+"Muon_Insitu"]->addRes(muon2, 1.0-m_vecMuon1[i].pt_truth/muon2.Pt());
  }
  return sc;
  }
*/

double	MuonInsituResolution::findMomentumScale(double from, double to, double res, std::string CurrentTrackCollection)
{
  double dscale=1.0;
  double min=100;
  double diff=0;

  //  APHistogramEntry 	apZBosonMC;
  //  APHistogramEntry 	apZBosonData;
	
  for (double scale=from; scale<to; scale+=(to-from)/10.0)
    {
      fillMCTruthZBosonPlots(scale, res, CurrentTrackCollection);

      //temporary commented in     

      //m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_MC"]->plotOverallProjection(m_histograms[mstrackcollection+"MuonResolution_ZBoson_MC"]);
      //      apZBosonMC	= m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_MC"]->getOverallHistogram();
      //      apZBosonData	= m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_Data"]->getOverallHistogram();
      //      diff		= apZBosonMC.difference(apZBosonData);
      ATH_MSG_DEBUG(" Scale: "<<scale<<"  "<<diff ); 
      if (diff<min)
	{
	  dscale = scale;
	  min=diff;
	}
    }
  ATH_MSG_INFO(" Determined Scale: "<<dscale<<"  "<<diff); 
  return dscale;
}

/*
  double	MuonInsituResolution::findResolutionScale(double from, double to, double scale, std::string CurrentTrackCollection)
  {
  double dres=1.0;
  double min=100;
  double diff=0;
	
  APHistogramEntry 	apZBosonMC;
  APHistogramEntry 	apZBosonData;
	
  for (double res=from; res<to; res+=(to-from)/10.0)
  {
  fillMCTruthZBosonPlots(scale, res, CurrentTrackCollection);
  m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_MC"]->plotOverallProjection(m_histograms[mstrackcollection+"MuonResolution_ZBoson_MC"]);
  apZBosonMC	= m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_MC"]->getOverallHistogram();
  apZBosonData	= m_APZmumuResonanceMatrix[mstrackcollection+"Muon_ZBoson_Data"]->getOverallHistogram();
  diff		= apZBosonMC.difference(apZBosonData);
  m_log << MSG::INFO << " Resolution: "<<res<<"  "<<diff << endreq; 
  if (diff<min)
  {
  dres = res;
  min=diff;
  }
  }
  m_log << MSG::INFO << " Determined Resolution: "<<dres<<"  "<<diff << endreq; 
  return dres;

  }
*/

bool MuonInsituResolution::canAssociateMuonTrack(const INavigable4Momentum *truth, double &associatedMuonPt)
{
  double dis 		= 0.0;
  double min_dis	= 10.;
  associatedMuonPt	= 0.0;

  Rec::TrackParticleContainer::const_iterator trackItr  = trackTES->begin();
  Rec::TrackParticleContainer::const_iterator trackItrE = trackTES->end();
  for (; trackItr != trackItrE; ++trackItr) 
    {
      dis = m_InsituPerformanceTool->getDistance((*trackItr), truth);
      if (dis<min_dis)
	{
	  min_dis=dis;
	  associatedMuonPt = (*trackItr)->pt();
	}
    }
  if (min_dis<d_assoziationDistance) return true;
  return false;
}

//============================================================================================
