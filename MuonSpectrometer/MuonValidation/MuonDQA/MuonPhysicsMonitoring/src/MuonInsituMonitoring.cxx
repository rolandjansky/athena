/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// MuonInsituMonitoring.cxx, (c) ATLAS Detector software
// **********************************************************************

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "MuonPhysicsMonitoring/MuonInsituMonitoring.h"

using CLHEP::pi;
//================ Constructor =================================================

MuonInsituMonitoring::MuonInsituMonitoring(const std::string& type, const std::string& name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_MSProbeCollectorTool("MSProbeCollectorTool/TestTool"),
   m_IDProbeCollectorTool("IDProbeCollectorTool/TestTool"),
   m_InsituPerformanceTool("InsituPerformanceTool/TestTool")   
{
  declareProperty( "MuonContainerLocation",       m_MuonContainerLocation);
  declareProperty( "MuonAuthor",                  m_MuonAuthor);
  declareProperty( "MSProbeCollectorTool",	  m_MSProbeCollectorTool);
  declareProperty( "IDProbeCollectorTool",	  m_IDProbeCollectorTool);
  declareProperty( "InsituPerformanceTool",	  m_InsituPerformanceTool);
  declareProperty( "doMuonspectrometerAnalysis",  m_doMuonSpectrometer);
  declareProperty( "doInnerDetectorAnalysis",     m_doInnerDetector); 
  declareProperty( "MuonSpectrometerProbeTracks", m_MScurrentcontainer_key = "MuonSpectrometerProbeTracks", "Input location of MuonSpectrometerProbeTracks" );
  declareProperty( "MuonCombinedProbeTracks",     m_MCBcurrentcontainer_key = "MuonSpectrometerProbeTracks", "Input location of MuonCombinedProbeTracks" );
  declareProperty( "InnerDetectorProbeTracks",    m_IDcurrentcontainer_key = "InnerDetectorProbeTracks", "Input location of InnerDetectorProbeTracks" );
  declareProperty( "doCombinedAnalysis",          m_doCombined);
  declareProperty( "AssociationDeltaR",           m_DeltaR = 0.05);
  declareProperty( "Ptcut",			  m_MinPt = 20000.0);
  declareProperty( "Etacut",			  m_MaxEta = 2.5);
}


//================ Destructor =================================================

MuonInsituMonitoring::~MuonInsituMonitoring()
{
  ATH_MSG_INFO(" Deleting MuonInsituMonitoring ");
}

//======================================================================================//
/**  Initialize */
//======================================================================================//
StatusCode MuonInsituMonitoring:: initialize() 
{ 
   
  ATH_MSG_DEBUG("MuonInsituMonitoring::initialize() to setup tools/services");

  StatusCode sc = StatusCode::SUCCESS;
  
  /** Initialize tools and services */
  /// Getting MSProbeCollectorTool
  sc=m_MSProbeCollectorTool.retrieve();
  if (sc.isFailure())
    {
      ATH_MSG_FATAL( "MSProbeCollectorTool Service not found" << m_MSProbeCollectorTool );
      return sc;
    }

  /// Getting IDProbeCollectorTool
   
  sc=m_IDProbeCollectorTool.retrieve();
  if (sc.isFailure())
    {
      ATH_MSG_FATAL( "IDProbeCollectorTool Service not found" << m_IDProbeCollectorTool );
      return sc;
    }  
	
  /// get StoreGate service
  sc = service("StoreGateSvc",m_storeGate);
  if (sc.isFailure())
    {
      ATH_MSG_FATAL( "StoreGate service not found !" );
      return StatusCode::FAILURE;
    } 
   
  return sc;
}


//================ bookHistograms =================================================

StatusCode MuonInsituMonitoring::bookHistogramsRecurrent()
{
 
  ATH_MSG_DEBUG("MuonInsituMonitoring::bookHistograms() to setup tools/services" );

  StatusCode sc = StatusCode::SUCCESS;
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
   
    if(newEventsBlock){}
    if(newLumiBlock){}
    if(newRun)
      { 
	sc = bookMuonSpecHistograms();
	ATH_MSG_DEBUG("Done booking Muon Spectrometer Histograms.");

	sc = bookInDetHistograms();
	ATH_MSG_DEBUG("Done booking Inner Detector Histograms.");

	sc = bookCombinedHistograms();  
	ATH_MSG_DEBUG("Done booking Combined Histograms.");       

      }// if NewRun
  } //environment
  return sc;
}

//================ fillHistograms =================================================
// Code entered here will be executed once per event

StatusCode MuonInsituMonitoring::fillHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

    sc=m_MSProbeCollectorTool->createProbeCollection();
    sc=m_IDProbeCollectorTool->createProbeCollection(); 


    int h_MuonSpec = -1;
    int h_InnDet   = -1;
    int h_Combined = -1;

    for (int ii=0; ii<(int)m_MuonContainerLocation.size(); ++ii)
      {
	if ( m_doMuonSpectrometer[ii] ) 
	  { 
	    ++h_MuonSpec;
	    sc=determineMuonInsituEfficiency(m_MuonContainerLocation[ii]);
	  }
	if ( m_doInnerDetector[ii] )
	  {
	    ++h_InnDet;
	    sc=determineIDInsituEfficiency(m_MuonContainerLocation[ii]); 
	  }

	if ( m_doCombined[ii] )
	  {
	    ++h_Combined;
	    sc=determineCombinedInsituEfficiency(m_MuonContainerLocation[ii]);
	  }

	const Analysis::MuonContainer* pMuonContainer(0);

	ATH_MSG_DEBUG("Retrieving: " << m_MuonContainerLocation[ii]);
	sc = m_storeGate->retrieve(pMuonContainer, m_MuonContainerLocation[ii] );
	if (sc.isFailure() || 0 == pMuonContainer)
	  {
	    ATH_MSG_WARNING("No Muon Container found in " << m_MuonContainerLocation[ii]); 
	    return sc;
	  }

	Analysis::MuonContainer::const_iterator mc_it     = pMuonContainer->begin();
	Analysis::MuonContainer::const_iterator mc_it_end = pMuonContainer->end();


	for ( ; mc_it != mc_it_end ; ++mc_it) 
	  {
	    // Checking the author of the container
	    //if  ( (*mc_it)->author() != MuonParameters::STACO ) continue;
	    if (m_MuonAuthor[ii] == "STACO") { if  ( (*mc_it)->author() != MuonParameters::STACO ) continue; }
	    if (m_MuonAuthor[ii] == "MuidComb") { if  ( (*mc_it)->author() != MuonParameters::MuidCo ) continue; }

	    if( m_doMuonSpectrometer[ii] && (*mc_it)->muonSpectrometerTrackParticle() )
	      {
		const Rec::TrackParticle* TrackParticle1 = (*mc_it)->muonSpectrometerTrackParticle();

		sc=determineMuonEfficiency(TrackParticle1, h_MuonSpec);
		ATH_MSG_DEBUG("MS Pt "<< TrackParticle1->pt());
		if (TrackParticle1->pt() > 0.) m_TH1F_MuonPt[h_MuonSpec]->Fill(TrackParticle1->pt());

		ATH_MSG_DEBUG("Done using determineMuonEfficiency.");

		//Second Loop to calculate invariant mass
		Analysis::MuonContainer::const_iterator mc2_it     = pMuonContainer->begin();
		Analysis::MuonContainer::const_iterator mc2_it_end = pMuonContainer->end();
		for ( ; mc2_it != mc2_it_end ; ++mc2_it) 
		  {
		    if ( (*mc2_it)->muonSpectrometerTrackParticle() ) {
		      const Rec::TrackParticle* TrackParticle2 = (*mc2_it)->muonSpectrometerTrackParticle();
		      sc=plotInvariantMasses(TrackParticle1, TrackParticle2, "MuonSpec", h_MuonSpec);
		    }
		  }
	      } //MuonSpec

	    if( m_doInnerDetector[ii] && (*mc_it)->hasInDetTrackParticle() )
	      {
		const Rec::TrackParticle* TrackParticle1 = (*mc_it)->inDetTrackParticle();

		sc=determineIDEfficiency(TrackParticle1, h_InnDet);
		ATH_MSG_DEBUG( "ID Pt "<< TrackParticle1->pt() );
		if (TrackParticle1->pt() > 0.) m_TH1F_IDPt[h_InnDet]->Fill(TrackParticle1->pt());

		ATH_MSG_DEBUG( "Done using determineIDEfficiency." );

		//Second Loop to calculate invariant mass
		Analysis::MuonContainer::const_iterator mc2_it     = pMuonContainer->begin();
		Analysis::MuonContainer::const_iterator mc2_it_end = pMuonContainer->end();
		for ( ; mc2_it != mc2_it_end ; ++mc2_it) 
		  {
		    if ( (*mc2_it)->hasInDetTrackParticle() ) {
		      const Rec::TrackParticle* TrackParticle2 = (*mc2_it)->inDetTrackParticle();
		      sc=plotInvariantMasses(TrackParticle1, TrackParticle2, "ID", h_InnDet);
		    }
		  }
	      } //InnerDetector

	    if( m_doCombined[ii] && (*mc_it)->hasCombinedMuonTrackParticle() )
	      { 
		const Rec::TrackParticle* TrackParticle1 = (*mc_it)->combinedMuonTrackParticle();

		sc=determineCombinedEfficiency(TrackParticle1, h_Combined);
		ATH_MSG_DEBUG("Comb Pt "<< TrackParticle1->pt() );
		if (TrackParticle1->pt() > 0.) m_TH1F_CombinedPt[h_Combined]->Fill(TrackParticle1->pt());

		ATH_MSG_DEBUG("Done using determineCombinedEfficiency." );

		//Second Loop to calculate invariant mass
		Analysis::MuonContainer::const_iterator mc2_it     = pMuonContainer->begin();
		Analysis::MuonContainer::const_iterator mc2_it_end = pMuonContainer->end();
		for ( ; mc2_it != mc2_it_end ; ++mc2_it) 
		  {
		    if ( (*mc2_it)->hasCombinedMuonTrackParticle() ) {
		      const Rec::TrackParticle* TrackParticle2 = (*mc2_it)->combinedMuonTrackParticle();
		      sc=plotInvariantMasses(TrackParticle1, TrackParticle2, "COMB", h_Combined);
		    }
		  }
	      }//Combined	   

	  } //MuonContainer::const_iterator

      } //Loop over MuonContainers

    /// Loading MS Probe Collection
    const Rec::TrackParticleContainer *probeTES;
    sc=m_storeGate->retrieve( probeTES, "MuonSpectrometerProbeTracks");
    if( sc.isFailure()  ||  !probeTES ) 
      {
	ATH_MSG_WARNING("No MuonSpectrometerProbeTracks container found in TDS"); 
	return sc;
      } else {
      /// Iterate over Collections
      ATH_MSG_DEBUG("(*probeTES).size(): " << (*probeTES).size() );
      Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
      Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
      for (; probeItr != probeItrE; ++probeItr)
	{
	  m_TH1F_msProbePt->Fill((*probeItr)->pt());  
	  m_TH1F_combProbePt->Fill((*probeItr)->pt());   
	}
    }
    /// Loading ID Probe Collection
    //const Rec::TrackParticleContainer *probeTES;
    sc=m_storeGate->retrieve( probeTES, "InnerDetectorProbeTracks");
    if( sc.isFailure()  ||  !probeTES ) 
      {
	ATH_MSG_WARNING("No InnerDetectorProbeTracks container found in TDS"); 
	return sc;
      } else {
      /// Iterate over Collections
      ATH_MSG_DEBUG("(*probeTES).size(): " << (*probeTES).size() );
      Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
      Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
      for (; probeItr != probeItrE; ++probeItr)
	{
	  m_TH1F_idProbePt->Fill((*probeItr)->pt());     
	}
    }
    /*
   /// Loading COMB Probe Collection
   //const Rec::TrackParticleContainer *probeTES;
   sc=m_storeGate->retrieve( probeTES, "MuonSpectrometerProbeTracks");
   if( sc.isFailure()  ||  !probeTES ) 
   {
   ATH_MSG_WARNING( "No MuonSpectrometerProbeTracks container found in TDS" ); 
   return sc;
   } else {
   /// Iterate over Collections
   if (m_debuglevel) ATH_MSG_DEBUG( "(*probeTES).size(): " << (*probeTES).size()<< endreq;
   Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
   Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
   for (; probeItr != probeItrE; ++probeItr)
   {
   m_TH1F_combProbePt->Fill((*probeItr)->pt());     
   }
   }
    */
  } //environment
      
  return sc; 
   
}

//================ procHistograms =================================================
// Code entered here will be executed once at the end of the program run.     

StatusCode MuonInsituMonitoring::procHistograms()
{

  StatusCode sc = StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG( "Beginning of ProcHistograms." );
      
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
      
    if(endOfEventsBlock){}
    if(endOfLumiBlock){} 
    if(endOfRun){

      sc = procMuonSpecHistograms();

      sc = procInDetHistograms();

      sc = procCombinedHistograms();

    } // EndRun
   
  } //environment 
  return sc;  

}

//============================================================================================
StatusCode MuonInsituMonitoring::determineMuonEfficiency(const Rec::TrackParticle* msTrackParticle, int h_index)
{
  StatusCode sc = StatusCode::SUCCESS;
	
  /// Loading Probe Collection
  const Rec::TrackParticleContainer *probeTES;
  sc=m_storeGate->retrieve( probeTES, "MuonSpectrometerProbeTracks");
  if( sc.isFailure()  ||  !probeTES ) 
    {
      ATH_MSG_WARNING("No MuonSpectrometerProbeTracks container found in TDS"); 
      return sc;
    } 

  /// Iterate over Collections
  Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
  Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
  for (; probeItr != probeItrE; ++probeItr)
    {
      if (((*probeItr)->pt()>m_MinPt) && (fabs((*probeItr)->eta())<m_MaxEta))
	{
	  m_TH1F_TP_MuonEff_D_Eta[h_index]->Fill((*probeItr)->eta());
	  m_TH1F_TP_MuonEff_D_Phi[h_index]->Fill((*probeItr)->phi());
	  m_TH1F_TP_MuonEff_D_Pt[h_index]->Fill((*probeItr)->pt());

	  if ( isAssociated(msTrackParticle, const_cast<Rec::TrackParticle*>(*probeItr)) == true)
	    {
	      m_TH1F_TP_MuonEff_N_Eta[h_index]->Fill((*probeItr)->eta());
	      m_TH1F_TP_MuonEff_N_Phi[h_index]->Fill((*probeItr)->phi());
	      m_TH1F_TP_MuonEff_N_Pt[h_index]->Fill((*probeItr)->pt());
		      
	    }
	}
    } 
     
  return sc;
}

StatusCode MuonInsituMonitoring::finalize()
{
  return StatusCode::SUCCESS;
}
//============================================================================================
StatusCode MuonInsituMonitoring::determineIDEfficiency(const Rec::TrackParticle* idTrackParticle, int h_index)
{
  StatusCode sc = StatusCode::SUCCESS;
 
  /// Loading Probe Collection
  const Rec::TrackParticleContainer *probeTES;
  sc=m_storeGate->retrieve( probeTES, "InnerDetectorProbeTracks");
  if( sc.isFailure()  ||  !probeTES ) 
    {
      ATH_MSG_WARNING("No InnerDetectorProbeTracks container found in TDS"); 
      return sc;
    }
  /// Iterate over Collections
  Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
  Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
  for (; probeItr != probeItrE; ++probeItr)
    {
      if (((*probeItr)->pt()>m_MinPt) && (fabs((*probeItr)->eta())<m_MaxEta))
	{
	  m_TH1F_TP_IDEff_D_Eta[h_index]->Fill((*probeItr)->eta());
	  m_TH1F_TP_IDEff_D_Phi[h_index]->Fill((*probeItr)->phi());
	  m_TH1F_TP_IDEff_D_Pt[h_index]->Fill((*probeItr)->pt());

	  if	(isAssociated(idTrackParticle, const_cast<Rec::TrackParticle*>(*probeItr))==true)
	    {
	      m_TH1F_TP_IDEff_N_Eta[h_index]->Fill((*probeItr)->eta());
	      m_TH1F_TP_IDEff_N_Phi[h_index]->Fill((*probeItr)->phi());
	      m_TH1F_TP_IDEff_N_Pt[h_index]->Fill((*probeItr)->pt());
	   
	    }

	}
    }
	
  return sc;
}


//============================================================================================
StatusCode MuonInsituMonitoring::determineCombinedEfficiency(const Rec::TrackParticle* combTrackParticle, int h_index)
{
  StatusCode sc = StatusCode::SUCCESS;
    
  /// Loading Probe Collection
  const Rec::TrackParticleContainer *probeTES;
  sc=m_storeGate->retrieve( probeTES, "MuonSpectrometerProbeTracks");
  if( sc.isFailure()  ||  !probeTES ) 
    {
      ATH_MSG_WARNING("No MuonSpectrometerProbeTracks container found in TDS"); 
      return sc;
    }

  /// Iterate over Collections
  Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
  Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
  for (; probeItr != probeItrE; ++probeItr)
    {
      if (((*probeItr)->pt()>m_MinPt) && (fabs((*probeItr)->eta())<m_MaxEta))
	{
	  m_TH1F_TP_CombinedEff_D_Eta[h_index]->Fill((*probeItr)->eta());
	  m_TH1F_TP_CombinedEff_D_Phi[h_index]->Fill((*probeItr)->phi());
	  m_TH1F_TP_CombinedEff_D_Pt[h_index]->Fill((*probeItr)->pt());

	  //if ((isAssociated(msTrackParticle, const_cast<Rec::TrackParticle*>(*probeItr))==true) && (*muonItr)->isCombinedMuon())
	  if ( isAssociated(combTrackParticle, const_cast<Rec::TrackParticle*>(*probeItr)) == true )
	    {
	      m_TH1F_TP_CombinedEff_N_Eta[h_index]->Fill((*probeItr)->eta());
	      m_TH1F_TP_CombinedEff_N_Phi[h_index]->Fill((*probeItr)->phi());
	      m_TH1F_TP_CombinedEff_N_Pt[h_index]->Fill((*probeItr)->pt());

	    }
	}
    }

  return sc;
}


//============================================================================================
StatusCode MuonInsituMonitoring::determineMuonInsituEfficiency(const std::string m_MScurrentcontainer_key)
{
  StatusCode sc = StatusCode::SUCCESS;

  /// Loading the Probe Track Collection
  const Rec::TrackParticleContainer *probeTES;
 
  sc=m_storeGate->retrieve( probeTES, m_MScurrentcontainer_key);
  if( sc.isFailure()  ||  !probeTES ) 
    {
      ATH_MSG_WARNING("No MuonSpectrometerProbeTracks container found in TDS"); 
      return sc;
    }

  /// Iterate over Collections
  Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
  Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
  for (; probeItr != probeItrE; ++probeItr)
    {
      if (((*probeItr)->pt()>m_MinPt) && (fabs((*probeItr)->eta())<m_MaxEta))
	{
	  // m_apMuonEfficiencyMatrix[currentcontainer+"Muon_Insitu"]->addTrial((*probeItr), MuonTrackAssociation(*probeItr, currentcontainer));
	}
    }

  return sc;
}

//============================================================================================
StatusCode MuonInsituMonitoring::determineCombinedInsituEfficiency(const std::string m_MCBcurrentcontainer_key)
{
  StatusCode sc = StatusCode::SUCCESS;

  /// Loading the Probe Track Collection
  const Rec::TrackParticleContainer *probeTES;
  sc=m_storeGate->retrieve( probeTES, m_MCBcurrentcontainer_key);
  if( sc.isFailure()  ||  !probeTES ) 
    {
      ATH_MSG_WARNING("No MuonSpectrometerProbeTracks container found in TDS"); 
      return sc;
    }

  /// Iterate over Collections
  Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
  Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
  for (; probeItr != probeItrE; ++probeItr)
    {
      if (((*probeItr)->pt()>m_MinPt) && (fabs((*probeItr)->eta())<m_MaxEta))
	{
	  /// Test if the closest electron is close enough to the truth particle
	  /// and can be counted as reconstructed
	  //		bool outcome = ((*probeItr)->get(UserData_InsituExample::Distance)<0.1);
	  //	  m_apMuonEfficiencyMatrix[currentcontainer+"CombinedMuon_Insitu"]->addTrial((*probeItr)->eta(), (*probeItr)->pt(), outcome));
 	
	}
    }

  return sc;
}
//============================================================================================
StatusCode MuonInsituMonitoring::determineIDInsituEfficiency(const std::string m_IDcurrentcontainer_key)
{
  StatusCode sc = StatusCode::SUCCESS;

  /// Loading the Probe Track Collection
  const Rec::TrackParticleContainer *probeTES; 
  
  sc=m_storeGate->retrieve( probeTES, m_IDcurrentcontainer_key);
  if( sc.isFailure()  ||  !probeTES ) 
    {
      ATH_MSG_WARNING("No InnerDetectorProbeTracks container found in TDS"); 
      return sc;
    }

  /// Iterate over Collections
  Rec::TrackParticleContainer::const_iterator probeItr  = probeTES->begin();
  Rec::TrackParticleContainer::const_iterator probeItrE = probeTES->end();
  for (; probeItr != probeItrE; ++probeItr)
    {
      if (((*probeItr)->pt()>m_MinPt) && (fabs((*probeItr)->eta())<m_MaxEta))
	{
	  // m_apMuonEfficiencyMatrix[currentcontainer+"ID_Insitu"]->addTrial((*probeItr), InnerDetectorTrackAssociation(*probeItr, currentcontainer));
	}
    }

  return sc;
}

//============================================================================================
bool	MuonInsituMonitoring::MuonTrackAssociation(INavigable4Momentum *truth, std::string currentcontainer)
{
  StatusCode sc=StatusCode::SUCCESS;
  double dist 		= 0.0;
  double min_dist  	= 10.;

  // 	/// Iterate over Collections
  // 	Rec::TrackParticleContainer::const_iterator trackItr  = trackTES->begin();
  // 	Rec::TrackParticleContainer::const_iterator trackItrE = trackTES->end();
  // 
  // 	for (; trackItr != trackItrE; ++trackItr) 
  // 	{
  // 		dist = m_InsituPerformanceTool->getDistance((*trackItr), truth);
  // 		if (dist<min_dist)
  // 		{
  // 			min_dist=dist;
  // 		}
  // 	}
  // 	if (min_dist<d_associationDistance)return true;
  // 	return false;

  const Analysis::MuonContainer* pMuonContainer(0);

  ATH_MSG_DEBUG( "Retrieving: " << currentcontainer );
  sc = m_storeGate->retrieve(pMuonContainer, currentcontainer );
  if (sc.isFailure() || 0 == pMuonContainer)
    {
      ATH_MSG_WARNING("No Muon Container found in " << currentcontainer ); 
      return sc;
    }	
	
  Analysis::MuonContainer::const_iterator muonItr  = pMuonContainer->begin();
  Analysis::MuonContainer::const_iterator muonItrE = pMuonContainer->end();
	
  for (; muonItr != muonItrE; ++muonItr) 
    {
      if ((*muonItr)->muonSpectrometerTrackParticle() )
	{ 
	  //const Rec::TrackParticle* TrkParticle = (*muonItr)->muonSpectrometerTrackParticle();
	  dist = m_InsituPerformanceTool->getDistance((*muonItr), truth);
	  if (dist<min_dist) min_dist=dist;
	}
    }
  if (min_dist<d_associationDistance) return true;
  return false;	
  return sc;
}
//============================================================================================
bool	MuonInsituMonitoring::CombinedMuonTrackAssociation(INavigable4Momentum *truth, std::string currentcontainer)
{
  StatusCode sc=StatusCode::SUCCESS;
  double dist = 0;
  double min_dist = 10.;

  const Analysis::MuonContainer* pMuonContainer(0);

  ATH_MSG_DEBUG( "Retrieving: " << currentcontainer );
  sc = m_storeGate->retrieve(pMuonContainer, currentcontainer );
  if (sc.isFailure() || 0 == pMuonContainer)
    {
      ATH_MSG_WARNING( "No Muon Container found in " << currentcontainer ); 
      return sc;
    }

  Analysis::MuonContainer::const_iterator muonItr  = pMuonContainer->begin();
  Analysis::MuonContainer::const_iterator muonItrE = pMuonContainer->end();
	
  for (; muonItr != muonItrE; ++muonItr) 
    {
      if ((*muonItr)->hasCombinedMuonTrackParticle() )
	{ 
	  //const Rec::TrackParticle* TrkParticle = (*muonItr)->combinedMuonTrackParticle();
	  dist = m_InsituPerformanceTool->getDistance((*muonItr), truth);
	  if (dist<min_dist) min_dist=dist;
	}
    }
  if (min_dist<d_associationDistance) return true;
  return false;

  return sc;

}
//============================================================================================
bool	MuonInsituMonitoring::InnerDetectorTrackAssociation(INavigable4Momentum *truth, std::string currentcontainer)
{
  StatusCode sc=StatusCode::SUCCESS;

  double dist = 0;
  double min_dist = 10.;

  const Analysis::MuonContainer* pMuonContainer(0);

  ATH_MSG_DEBUG("Retrieving: " << currentcontainer);
  sc = m_storeGate->retrieve(pMuonContainer, currentcontainer );
  if (sc.isFailure() || 0 == pMuonContainer)
    {
      ATH_MSG_WARNING("No Muon Container found in " << currentcontainer); 
      return sc;
    }

  Analysis::MuonContainer::const_iterator muonItr  = pMuonContainer->begin();
  Analysis::MuonContainer::const_iterator muonItrE = pMuonContainer->end();
	
  for (; muonItr != muonItrE; ++muonItr) 
    {
      if ((*muonItr)->hasInDetTrackParticle() )
	{ 
	  //const Rec::TrackParticle* TrkParticle = (*muonItr)->inDetTrackParticle();
	  dist = m_InsituPerformanceTool->getDistance((*muonItr), truth);
	  if (dist<min_dist) min_dist=dist;
	}
    }
  if (min_dist<d_associationDistance) return true;
  return false;

  return sc;
}
//============================================================================================
StatusCode	MuonInsituMonitoring::plotInvariantMasses(const Rec::TrackParticle*  track1, const Rec::TrackParticle* track2, std::string type, int h_index)
{
  StatusCode sc = StatusCode::SUCCESS;
	
  if (type == "MuonSpec")
    {
      if ( (isAssociated(track1, track2) == false) && (  track1->charge()!= track2->charge() ) && (track1->pt()>m_MinPt) && (track2->pt()>m_MinPt) )
	m_TH1F_TP_MuonInvariantMass[h_index]->Fill(getInvariantMass(track1, track2));	 
    }
  if (type == "ID")
    {
      if ( (isAssociated(track1, track2) == false) && (  track1->charge()!= track2->charge() ) && (track1->pt()>m_MinPt) && (track2->pt()>m_MinPt) )
	m_TH1F_TP_IDInvariantMass[h_index]->Fill(getInvariantMass(track1, track2));	
    }
  if (type == "COMB")
    {
      if ( (isAssociated(track1, track2) == false) && (  track1->charge()!= track2->charge() ) && (track1->pt()>m_MinPt) && (track2->pt()>m_MinPt) )
	m_TH1F_TP_CombinedInvariantMass[h_index]->Fill(getInvariantMass(track1, track2));	
    }
	
  return sc;
}

//============================================================================================
double	MuonInsituMonitoring::getInvariantMass(const Rec::TrackParticle *track1, const Rec::TrackParticle *track2)
{
  TLorentzVector tvec1;
  TLorentzVector tvec2;
  tvec1.SetPtEtaPhiM(track1->pt(), track1->eta(), track1->phi(), 105.65);
  tvec2.SetPtEtaPhiM(track2->pt(), track2->eta(), track2->phi(), 105.65);
  return (tvec1+tvec2).M();
}
	
//============================================================================================		
bool	MuonInsituMonitoring::isAssociated(const Rec::TrackParticle *track1, const Rec::TrackParticle *track2)
{
  double dphi = fabs(track1->phi()-track2->phi());
  double pi = 3.14159265;
  if (dphi>pi)	dphi = fabs(dphi-2.0*pi);
  if (sqrt(pow(track1->eta()-track2->eta(),2)+pow(dphi,2))< m_DeltaR) return true;
  return false;
}


//============================================================================================
StatusCode MuonInsituMonitoring::bookMuonSpecHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
    
  int h_index = -1;
    
  for (int ii=0; ii<(int)m_MuonContainerLocation.size(); ++ii)
    {
      if (m_doMuonSpectrometer[ii] == 0) continue;
      ++h_index;

      std::string m_generic_path = "Muon/MuonPhysicsMonitoring/MuonInsituMonitoring/"+m_MuonContainerLocation[h_index];     
       
      MonGroup ms_insitumonitoring_eff ( this, m_generic_path+"/MuonSpectrometer/Efficiency",  run, ATTRIB_UNMANAGED, "", "weightedEff" ); 
      MonGroup ms_insitumonitoring_calc ( this, m_generic_path+"/MuonSpectrometer/Calculations",  run, ATTRIB_UNMANAGED );
      MonGroup ms_insitumonitoring_InvMass ( this, m_generic_path+"/MuonSpectrometer/InvariantMass",  run, ATTRIB_UNMANAGED );
      MonGroup ms_insitumonitoring_Pt (this, m_generic_path +"/MuonSpectrometer/PtDistributions",  run, ATTRIB_UNMANAGED );

      ///////////////Nectar///////////////
      // Histograms for Efficiency: Binning with be changed later by APMuonEfficiencyMatrix
       
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Eta"]   = new TH1F("Muon_Insitu_Efficiency_Eta","Efficiency",1,0,1);
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Phi"]   = new TH1F("Muon_Insitu_Efficiency_Phi","Efficiency",1,0,1);
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Pt"]   = new TH1F("Muon_Insitu_Efficiency_Pt","Efficiency",1,0,1);
      //////////////Nectar////////////////

      /// Histograms for Efficiency 
      m_TH1F_TP_MuonEff_Eta.push_back(new TH1F("Muon_Efficiency_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_MuonEff_Phi.push_back(new TH1F("Muon_Efficiency_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_MuonEff_Pt.push_back(new TH1F("Muon_Efficiency_Pt","Efficiency",20,0.0,100000.));

      /// Histograms for internal calculations
      m_TH1F_TP_MuonEff_D_Eta.push_back(new TH1F("Muon_Efficiency_D_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_MuonEff_N_Eta.push_back(new TH1F("Muon_Efficiency_N_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_MuonEff_D_Phi.push_back(new TH1F("Muon_Efficiency_D_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_MuonEff_N_Phi.push_back(new TH1F("Muon_Efficiency_N_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_MuonEff_D_Pt.push_back(new TH1F("Muon_Efficiency_D_Pt","Efficiency",20,0.0,100000.));
      m_TH1F_TP_MuonEff_N_Pt.push_back(new TH1F("Muon_Efficiency_N_Pt","Efficiency",20,0.0,100000.));

      /// Histograms for Invariant Mass - should be exported to a new Algorithm/Tool
      m_TH1F_TP_MuonInvariantMass.push_back(new TH1F("Muon_Invariant Mass","Invariant Mass",120,0.0,120000.));
       
      //Pt histogram
      m_TH1F_MuonPt.push_back(new TH1F("Muon_Pt","P_{T}",100,0.0,200000.));
      m_TH1F_msProbePt = new TH1F("MS_Probe_Pt","P_{T}",100,0.0,200000.);

      /// Registration of Histograms for Efficiency  
       
      //////////////Nectar////////////////
      sc=ms_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Eta"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "Muon_Insitu_Efficiency_Eta Failed to register histogram " );
	  return sc;
	}
      sc=ms_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Phi"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "Muon_Insitu_Efficiency_Phi Failed to register histogram " );
	  return sc;
	}
      sc=ms_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Pt"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "Muon_Insitu_Efficiency_Pt Failed to register histogram " );
	  return sc;
	}       
      //////////////Nectar////////////////


      sc=ms_insitumonitoring_eff.regHist(m_TH1F_TP_MuonEff_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_Eta Failed to register histogram " );
	  return sc;
	}
      sc=ms_insitumonitoring_eff.regHist(m_TH1F_TP_MuonEff_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_Phi Failed to register histogram " );
	  return sc;
	}   
      sc=ms_insitumonitoring_eff.regHist(m_TH1F_TP_MuonEff_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_Pt Failed to register histogram " );
	  return sc;
	} 

      /// Registration of Histograms for internal calculations

      sc=ms_insitumonitoring_calc.regHist(m_TH1F_TP_MuonEff_D_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_D_Eta Failed to register histogram " );
	  return sc;
	}
      sc=ms_insitumonitoring_calc.regHist(m_TH1F_TP_MuonEff_N_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_N_Eta Failed to register histogram " );
	  return sc;
	}	
      sc=ms_insitumonitoring_calc.regHist(m_TH1F_TP_MuonEff_D_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_D_Phi Failed to register histogram " );
	  return sc;
	}	
      sc=ms_insitumonitoring_calc.regHist(m_TH1F_TP_MuonEff_N_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_N_Phi Failed to register histogram " );
	  return sc;
	}
      sc=ms_insitumonitoring_calc.regHist(m_TH1F_TP_MuonEff_D_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_D_Pt Failed to register histogram " );
	  return sc;
	}	
      sc=ms_insitumonitoring_calc.regHist(m_TH1F_TP_MuonEff_N_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonEff_N_Pt Failed to register histogram " );
	  return sc;
	}	

      /// Registration of Histograms for Invariant Mass - should be exported to a new Algorithm/Tool

      sc=ms_insitumonitoring_InvMass.regHist(m_TH1F_TP_MuonInvariantMass.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_MuonInvariantMass Failed to register histogram ");
	  return sc;
	}
      /// Registration of pt Histogram  for Muon Spec
      sc=ms_insitumonitoring_Pt.regHist(m_TH1F_MuonPt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_MuonPt Failed to register histogram " );
	  return sc;
	}     
       
      /// Registration of pt Histogram fro Probe collection
      sc=ms_insitumonitoring_Pt.regHist(m_TH1F_msProbePt);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_msProbePt Failed to register histogram " );
	  return sc;
	} 

      /////////////Nectar///////////////
      /// Initializing the EfficiencyMatricies
      /// Obsolete
      // m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"Muon_Insitu"] = new Insitu::APMuonEfficiencyMatrix(); 
      /////////////Nectar/////////////// 
   
    } // Loop over MuonContainers		
	    
  return sc;
}

//============================================================================================
StatusCode MuonInsituMonitoring::bookInDetHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
    
  int h_index = -1;
        
  for (int ii=0; ii<(int)m_MuonContainerLocation.size(); ++ii)
    {
      if (m_doInnerDetector[ii] == 0) continue;
      ++h_index;

      std::string m_generic_path = "Muon/MuonPhysicsMonitoring/MuonInsituMonitoring/"+m_MuonContainerLocation[h_index];
       
      MonGroup id_insitumonitoring_eff ( this, m_generic_path+"/InnerDetector/Efficiency",   run, ATTRIB_UNMANAGED, "", "weightedEff" ); 
      MonGroup id_insitumonitoring_calc ( this, m_generic_path+"/InnerDetector/Calculations",  run, ATTRIB_UNMANAGED );
      MonGroup id_insitumonitoring_InvMass ( this, m_generic_path+"/InnerDetector/InvariantMass",  run, ATTRIB_UNMANAGED );
      MonGroup id_insitumonitoring_Pt ( this, m_generic_path+"/InnerDetector/PtDistributions",  run, ATTRIB_UNMANAGED );

      ///////////////Nectar///////////////
      // Histograms for Efficiency: Binning with be changed later by APMuonEfficiencyMatrix
       
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Eta"]   = new TH1F("ID_Insitu_Efficiency_Eta","Efficiency",1,0,1);
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Phi"]   = new TH1F("ID_Insitu_Efficiency_Phi","Efficiency",1,0,1);
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Pt"]   = new TH1F("ID_Insitu_Efficiency_Pt","Efficiency",1,0,1);
      //////////////Nectar////////////////

      /// Histograms for Efficiency	
      m_TH1F_TP_IDEff_Eta.push_back(new TH1F("ID_Efficiency_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_IDEff_Phi.push_back(new TH1F("ID_Efficiency_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_IDEff_Pt.push_back(new TH1F("ID_Efficiency_Pt","Efficiency",20,0.0,100000.));     
       
      /// Histograms for internal calculations
      m_TH1F_TP_IDEff_D_Eta.push_back(new TH1F("ID_Efficiency_D_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_IDEff_N_Eta.push_back(new TH1F("ID_Efficiency_N_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_IDEff_D_Phi.push_back(new TH1F("ID_Efficiency_D_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_IDEff_N_Phi.push_back(new TH1F("ID_Efficiency_N_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_IDEff_D_Pt.push_back(new TH1F("ID_Efficiency_D_Pt","Efficiency",20,0.0,100000.));
      m_TH1F_TP_IDEff_N_Pt.push_back(new TH1F("ID_Efficiency_N_Pt","Efficiency",20,0.0,100000.));

      /// Histograms for Invariant Mass - should be exported to a new Algorithm/Tool	
      m_TH1F_TP_IDInvariantMass.push_back(new TH1F("ID_Invariant Mass","Invariant Mass",120,0.0,120000.));
       
      //Pt distribution
      m_TH1F_IDPt.push_back(new TH1F("ID_Pt","P_{T}",100,0.0,200000.));
      m_TH1F_idProbePt = new TH1F("ID_Probe_Pt","P_{T}",100,0.0,200000.);

      /// Registration of Histograms for Efficiency
      //////////////Nectar////////////////
      sc=id_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Eta"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "ID_Insitu_Efficiency_Eta Failed to register histogram " );
	  return sc;
	}
      sc=id_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Phi"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "ID_Insitu_Efficiency_Phi Failed to register histogram " );
	  return sc;
	}
      sc=id_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Pt"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "ID_Insitu_Efficiency_Pt Failed to register histogram " );
	  return sc;
	}       
      //////////////Nectar////////////////

      sc=id_insitumonitoring_eff.regHist(m_TH1F_TP_IDEff_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_Eta Failed to register histogram " );
	  return sc;
	} 
      sc=id_insitumonitoring_eff.regHist(m_TH1F_TP_IDEff_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_Phi Failed to register histogram " );
	  return sc;
	} 
      sc=id_insitumonitoring_eff.regHist(m_TH1F_TP_IDEff_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_Pt Failed to register histogram " );
	  return sc;
	} 

      /// Registration of Histograms for internal calculations

      sc=id_insitumonitoring_calc.regHist(m_TH1F_TP_IDEff_D_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_D_Eta Failed to register histogram " );
	  return sc;
	}	
      sc=id_insitumonitoring_calc.regHist(m_TH1F_TP_IDEff_N_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_N_Eta Failed to register histogram " );
	  return sc;
	}
      sc=id_insitumonitoring_calc.regHist(m_TH1F_TP_IDEff_D_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_D_Phi Failed to register histogram " );
	  return sc;
	}	
      sc=id_insitumonitoring_calc.regHist(m_TH1F_TP_IDEff_N_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_N_Phi Failed to register histogram " );
	  return sc;
	}
      sc=id_insitumonitoring_calc.regHist(m_TH1F_TP_IDEff_D_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_D_Pt Failed to register histogram " );
	  return sc;
	}	
      sc=id_insitumonitoring_calc.regHist(m_TH1F_TP_IDEff_N_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDEff_N_Pt Failed to register histogram " );
	  return sc;
	}

      /// Registration of Histograms for Invariant Mass - should be exported to a new Algorithm/Tool

      sc=id_insitumonitoring_InvMass.regHist(m_TH1F_TP_IDInvariantMass.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_IDInvariantMass Failed to register histogram " );
	  return sc;
	}   
       
      /// Registration of pt Histogram fro InnerDetector
      sc=id_insitumonitoring_Pt.regHist(m_TH1F_IDPt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_IDPt Failed to register histogram " );
	  return sc;
	} 
      sc=id_insitumonitoring_Pt.regHist(m_TH1F_idProbePt);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_idProbePt Failed to register histogram " );
	  return sc;
	}
      /////////////Nectar///////////////
      /// Initializing the EfficiencyMatricies
     /// Obsolete
      // m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"ID_Insitu"] = new Insitu::APMuonEfficiencyMatrix(); 
      /////////////Nectar///////////////        
    } //Loop over MuonContainers
  return sc;
}

//============================================================================================
StatusCode MuonInsituMonitoring::bookCombinedHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
    
  int h_index = -1;
        
  for (int ii=0; ii<(int)m_MuonContainerLocation.size(); ++ii)
    {      
      if (m_doCombined[ii] == 0) continue;
      ++h_index;
     
      std::string m_generic_path = "Muon/MuonPhysicsMonitoring/MuonInsituMonitoring/"+m_MuonContainerLocation[h_index];
           
      MonGroup comb_insitumonitoring_eff ( this, m_generic_path+"/Combined/Efficiency",  run, ATTRIB_UNMANAGED, "", "weightedEff" ); 
      MonGroup comb_insitumonitoring_calc ( this, m_generic_path+"/Combined/Calculations",  run, ATTRIB_UNMANAGED );
      MonGroup comb_insitumonitoring_InvMass ( this, m_generic_path+"/Combined/InvariantMass",  run, ATTRIB_UNMANAGED );
      MonGroup comb_insitumonitoring_Pt ( this, m_generic_path+"/Combined/PtDistributions",  run, ATTRIB_UNMANAGED );


      ///////////////Nectar///////////////
      // Histograms for Efficiency: Binning with be changed later by APMuonEfficiencyMatrix
       
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Eta"]   = new TH1F("Combined_Insitu_Efficiency_Eta","Efficiency",1,0,1);
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Phi"]   = new TH1F("Combined_Insitu_Efficiency_Phi","Efficiency",1,0,1);
      m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Pt"]   = new TH1F("Combined_Insitu_Efficiency_Pt","Efficiency",1,0,1);
      //////////////Nectar////////////////

      /// Histograms for Efficiency		
      m_TH1F_TP_CombinedEff_Eta.push_back(new TH1F("Combined_Efficiency_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_CombinedEff_Phi.push_back(new TH1F("Combined_Efficiency_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_CombinedEff_Pt.push_back(new TH1F("Combined_Efficiency_Pt","Efficiency",20,0.0,100000.));
       
   
      /// Histograms for internal calculations
      m_TH1F_TP_CombinedEff_D_Eta.push_back(new TH1F("Combined_Efficiency_D_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_CombinedEff_N_Eta.push_back(new TH1F("Combined_Efficiency_N_Eta","Efficiency",25,-2.5,2.5));
      m_TH1F_TP_CombinedEff_D_Phi.push_back(new TH1F("Combined_Efficiency_D_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_CombinedEff_N_Phi.push_back(new TH1F("Combined_Efficiency_N_Phi","Efficiency",20,-3.14,3.14));
      m_TH1F_TP_CombinedEff_D_Pt.push_back(new TH1F("Combined_Efficiency_D_Pt","Efficiency",20,0.0,100000.));
      m_TH1F_TP_CombinedEff_N_Pt.push_back(new TH1F("Combined_Efficiency_N_Pt","Efficiency",20,0.0,100000.));

      /// Histograms for Invariant Mass - should be exported to a new Algorithm/Tool 	
      m_TH1F_TP_CombinedInvariantMass.push_back(new TH1F("Combined_Invariant Mass","Invariant Mass",120,0.0,120000.));
       
      //Pt Distribution
      m_TH1F_CombinedPt.push_back(new TH1F("Combined_Pt","P_{T}",100,0.0,200000.));
      m_TH1F_combProbePt = new TH1F("Combined_Probe_Pt","P_{T}",100,0.0,200000.);

      /// Registration of Histograms for Efficiency
      //////////////Nectar////////////////
      sc=comb_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Eta"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "Combined_Insitu_Efficiency_Eta Failed to register histogram " );
	  return sc;
	}
      sc=comb_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Phi"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "Combined_Insitu_Efficiency_Phi Failed to register histogram " );
	  return sc;
	}
      sc=comb_insitumonitoring_eff.regHist(m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Pt"]);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "Combined_Insitu_Efficiency_Pt Failed to register histogram " );
	  return sc;
	}       
      //////////////Nectar////////////////
      sc=comb_insitumonitoring_eff.regHist(m_TH1F_TP_CombinedEff_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_Eta Failed to register histogram " );
	  return sc;
	} 
      sc=comb_insitumonitoring_eff.regHist(m_TH1F_TP_CombinedEff_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_Phi Failed to register histogram " );
	  return sc;
	} 
      sc=comb_insitumonitoring_eff.regHist(m_TH1F_TP_CombinedEff_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_Pt Failed to register histogram " );
	  return sc;
	}       

      /// Registration of Histograms for internal calculations

      sc=comb_insitumonitoring_calc.regHist(m_TH1F_TP_CombinedEff_D_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_D_Eta Failed to register histogram " );
	  return sc;
	}
      sc=comb_insitumonitoring_calc.regHist(m_TH1F_TP_CombinedEff_N_Eta.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_N_Eta Failed to register histogram " );
	  return sc;
	}
      sc=comb_insitumonitoring_calc.regHist(m_TH1F_TP_CombinedEff_D_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_D_Phi Failed to register histogram " );
	  return sc;
	}
      sc=comb_insitumonitoring_calc.regHist(m_TH1F_TP_CombinedEff_N_Phi.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_N_Phi Failed to register histogram " );
	  return sc;
	}
      sc=comb_insitumonitoring_calc.regHist(m_TH1F_TP_CombinedEff_D_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_D_Pt Failed to register histogram " );
	  return sc;
	}
      sc=comb_insitumonitoring_calc.regHist(m_TH1F_TP_CombinedEff_N_Pt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedEff_N_Pt Failed to register histogram " );
	  return sc;
	}

      /// Registration of Histograms for Invariant Mass - should be exported to a new Algorithm/Tool

      sc=comb_insitumonitoring_InvMass.regHist(m_TH1F_TP_CombinedInvariantMass.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_TP_CombinedInvariantMass Failed to register histogram " );
	  return sc;
	} 
       
      /// Registration of pt Histogram for combined
      sc=comb_insitumonitoring_Pt.regHist(m_TH1F_CombinedPt.back());
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_CombinedPt Failed to register histogram " );
	  return sc;
	} 
      sc=comb_insitumonitoring_Pt.regHist(m_TH1F_combProbePt);
      if(sc.isFailure())
	{ 
	  ATH_MSG_FATAL( "m_TH1F_combProbePt Failed to register histogram " );
	  return sc;
	}

      /////////////Nectar///////////////
      /// Initializing the EfficiencyMatricies
      /// Obsolete
      // m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"CombinedMuon_Insitu"] = new Insitu::APMuonEfficiencyMatrix(); 
      /////////////Nectar///////////////                     
    } // Loop over MuonContainers
                
  return sc;
}

//============================================================================================
StatusCode MuonInsituMonitoring::procMuonSpecHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
    
  ATH_MSG_DEBUG( "Begin procMuonSpecHistograms." );
 
  for (int h_index=0; h_index < (int)m_TH1F_TP_MuonEff_D_Eta.size(); h_index++)
    {
      if (m_TH1F_TP_MuonEff_D_Eta[h_index]->GetEntries() > 0) m_TH1F_TP_MuonEff_Eta[h_index]->Divide(m_TH1F_TP_MuonEff_N_Eta[h_index], m_TH1F_TP_MuonEff_D_Eta[h_index], 1.0, 1.0, "");
      m_TH1F_TP_MuonEff_Eta[h_index]->SetEntries(m_TH1F_TP_MuonEff_Eta[h_index]->GetNbinsX());
    }
  for (int h_index=0; h_index < (int)m_TH1F_TP_MuonEff_D_Phi.size(); h_index++)
    {
      if (m_TH1F_TP_MuonEff_D_Phi[h_index]->GetEntries() > 0) m_TH1F_TP_MuonEff_Phi[h_index]->Divide(m_TH1F_TP_MuonEff_N_Phi[h_index], m_TH1F_TP_MuonEff_D_Phi[h_index], 1.0, 1.0, "");
      m_TH1F_TP_MuonEff_Phi[h_index]->SetEntries(m_TH1F_TP_MuonEff_Phi[h_index]->GetNbinsX());
    }
  for (int h_index=0; h_index < (int)m_TH1F_TP_MuonEff_D_Pt.size(); h_index++)
    {
      if (m_TH1F_TP_MuonEff_D_Pt[h_index]->GetEntries() > 0)  m_TH1F_TP_MuonEff_Pt[h_index]->Divide( m_TH1F_TP_MuonEff_N_Pt[h_index],  m_TH1F_TP_MuonEff_D_Pt[h_index],  1.0, 1.0, ""); 
      m_TH1F_TP_MuonEff_Pt[h_index]->SetEntries(m_TH1F_TP_MuonEff_Pt[h_index]->GetNbinsX());    
    }
  
      
  // 
  //   float ratio=0.;
  //      
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_MuonEff_D_Eta.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for MS Eta, " << h_index );
  //        
  //       if( (m_TH1F_TP_MuonEff_N_Eta[h_index]->GetNbinsX() == m_TH1F_TP_MuonEff_D_Eta[h_index]->GetNbinsX()) && (m_TH1F_TP_MuonEff_D_Eta[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_MuonEff_N_Eta[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_MuonEff_D_Eta[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_MuonEff_N_Eta[h_index]->GetBinContent(ibin) / m_TH1F_TP_MuonEff_D_Eta[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_MuonEff_Eta[h_index]->Fill(m_TH1F_TP_MuonEff_N_Eta[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	}
  //     }
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_MuonEff_D_Pt.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for MS Pt, " << h_index );
  //        
  //       if( (m_TH1F_TP_MuonEff_N_Pt[h_index]->GetNbinsX() == m_TH1F_TP_MuonEff_D_Pt[h_index]->GetNbinsX()) && (m_TH1F_TP_MuonEff_D_Pt[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_MuonEff_N_Pt[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_MuonEff_D_Pt[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_MuonEff_N_Pt[h_index]->GetBinContent(ibin) / m_TH1F_TP_MuonEff_D_Pt[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_MuonEff_Pt[h_index]->Fill(m_TH1F_TP_MuonEff_N_Pt[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	}
  //     }
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_MuonEff_D_Phi.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for MS Phi, " << h_index );
  //         
  //       if( (m_TH1F_TP_MuonEff_N_Phi[h_index]->GetNbinsX() == m_TH1F_TP_MuonEff_D_Phi[h_index]->GetNbinsX()) && (m_TH1F_TP_MuonEff_D_Phi[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_MuonEff_N_Phi[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_MuonEff_D_Phi[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_MuonEff_N_Phi[h_index]->GetBinContent(ibin) / m_TH1F_TP_MuonEff_D_Phi[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_MuonEff_Phi[h_index]->Fill(m_TH1F_TP_MuonEff_N_Phi[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	}    
  //     }

  ATH_MSG_DEBUG( "Done dividing." );    

/*    
  int h_index = -1;
  for (int ii=0; ii<(int)m_MuonContainerLocation.size(); ++ii)
    {
      ATH_MSG_DEBUG( "Inside procMuon, container:" << m_MuonContainerLocation[ii] );
      if (m_doMuonSpectrometer[ii] == 0) continue;
      ++h_index;
      /////////Nectar////////////
      /// Obsolete
     // m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"Muon_Insitu"]->plotEfficiencyProjectionToEta((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Eta"]));
     // ATH_MSG_DEBUG( "Done apMuonEfficiencyMatrix Eta." ); 
       
     // m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"Muon_Insitu"]->plotEfficiencyProjectionToPhi((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Phi"]));
     // ATH_MSG_DEBUG( "Done apMuonEfficiencyMatrix Phi." ); 
       
     // m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"Muon_Insitu"]->plotEfficiencyProjectionToPt((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Muon_Insitu_Efficiency_Pt"]));
     // ATH_MSG_DEBUG( "Done apMuonEfficiencyMatrix Pt." ); 
       
      ////////Nectar/////////////
    }
 */
    
  m_TH1F_TP_MuonInvariantMass.clear();
    
  m_TH1F_MuonPt.clear();

  m_TH1F_TP_MuonEff_Eta.clear();
  m_TH1F_TP_MuonEff_Phi.clear();
  m_TH1F_TP_MuonEff_Pt.clear();

  m_TH1F_TP_MuonEff_D_Eta.clear();
  m_TH1F_TP_MuonEff_N_Eta.clear();
  m_TH1F_TP_MuonEff_D_Phi.clear();
  m_TH1F_TP_MuonEff_N_Phi.clear();
  m_TH1F_TP_MuonEff_D_Pt.clear();
  m_TH1F_TP_MuonEff_N_Pt.clear();
    
  ATH_MSG_DEBUG( "Done clearing." );
       
  return sc;
}
//============================================================================================
StatusCode MuonInsituMonitoring::procInDetHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
    
  ATH_MSG_DEBUG( "Begin procInnDetHistograms." );

  for (int h_index=0; h_index < (int)m_TH1F_TP_IDEff_D_Eta.size(); ++h_index)
    {
      if (m_TH1F_TP_IDEff_D_Eta[h_index]->GetEntries() > 0) m_TH1F_TP_IDEff_Eta[h_index]->Divide(m_TH1F_TP_IDEff_N_Eta[h_index], m_TH1F_TP_IDEff_D_Eta[h_index], 1.0, 1.0, "");
      m_TH1F_TP_IDEff_Eta[h_index]->SetEntries(m_TH1F_TP_IDEff_Eta[h_index]->GetNbinsX());
    }  
  for (int h_index=0; h_index < (int)m_TH1F_TP_IDEff_D_Phi.size(); ++h_index)
    {   
      if (m_TH1F_TP_IDEff_D_Phi[h_index]->GetEntries() > 0) m_TH1F_TP_IDEff_Phi[h_index]->Divide(m_TH1F_TP_IDEff_N_Phi[h_index], m_TH1F_TP_IDEff_D_Phi[h_index], 1.0, 1.0, "");
      m_TH1F_TP_IDEff_Phi[h_index]->SetEntries(m_TH1F_TP_IDEff_Phi[h_index]->GetNbinsX());
    }
  for (int h_index=0; h_index < (int)m_TH1F_TP_IDEff_D_Pt.size(); ++h_index)
    {   
      if (m_TH1F_TP_IDEff_D_Pt[h_index]->GetEntries() > 0) m_TH1F_TP_IDEff_Pt[h_index]->Divide( m_TH1F_TP_IDEff_N_Pt[h_index],  m_TH1F_TP_IDEff_D_Pt[h_index],  1.0, 1.0, "");
      m_TH1F_TP_IDEff_Pt[h_index]->SetEntries(m_TH1F_TP_IDEff_Pt[h_index]->GetNbinsX());
    }
 
  //   float ratio=0.;
  //      
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_IDEff_D_Eta.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for ID Eta, " << h_index );
  //        
  //       if( (m_TH1F_TP_IDEff_N_Eta[h_index]->GetNbinsX() == m_TH1F_TP_IDEff_D_Eta[h_index]->GetNbinsX()) && (m_TH1F_TP_IDEff_D_Eta[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_IDEff_N_Eta[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_IDEff_D_Eta[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_IDEff_N_Eta[h_index]->GetBinContent(ibin) / m_TH1F_TP_IDEff_D_Eta[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_IDEff_Eta[h_index]->Fill(m_TH1F_TP_IDEff_N_Eta[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	}
  //     }
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_IDEff_D_Pt.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for ID Pt, " << h_index );
  //        
  //       if( (m_TH1F_TP_IDEff_N_Pt[h_index]->GetNbinsX() == m_TH1F_TP_IDEff_D_Pt[h_index]->GetNbinsX()) && (m_TH1F_TP_IDEff_D_Pt[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_IDEff_N_Pt[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_IDEff_D_Pt[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_IDEff_N_Pt[h_index]->GetBinContent(ibin) / m_TH1F_TP_IDEff_D_Pt[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_IDEff_Pt[h_index]->Fill(m_TH1F_TP_IDEff_N_Pt[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	} 
  //     }
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_IDEff_D_Phi.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for ID Phi, " << h_index );
  //        
  //       if( (m_TH1F_TP_IDEff_N_Phi[h_index]->GetNbinsX() == m_TH1F_TP_IDEff_D_Phi[h_index]->GetNbinsX()) && (m_TH1F_TP_IDEff_D_Phi[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_IDEff_N_Phi[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_IDEff_D_Phi[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_IDEff_N_Phi[h_index]->GetBinContent(ibin) / m_TH1F_TP_IDEff_D_Phi[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_IDEff_Phi[h_index]->Fill(m_TH1F_TP_IDEff_N_Phi[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	}    
  //     }
        
  ATH_MSG_DEBUG( "Done dividing." );

/* Osbsolete
  int h_index = -1;
  for (int ii=0; ii<(int)m_MuonContainerLocation.size(); ++ii)
    {
      if (m_doInnerDetector[ii] == 0) continue;
      ++h_index;
      /////////Nectar////////////
      m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"ID_Insitu"]->plotEfficiencyProjectionToEta((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Eta"]));
      ATH_MSG_DEBUG( "Done apIDEfficiencyMatrix Eta." ); 
      
      m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"ID_Insitu"]->plotEfficiencyProjectionToPhi((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Phi"]));
      ATH_MSG_DEBUG( "Done apIDEfficiencyMatrix Phi." ); 
      
      m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"ID_Insitu"]->plotEfficiencyProjectionToPt((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"ID_Insitu_Efficiency_Pt"]));
      ATH_MSG_DEBUG( "Done apIDEfficiencyMatrix Pt." ); 

      ////////Nectar/////////////
    }
*/
        
  m_TH1F_TP_IDInvariantMass.clear();
    
  m_TH1F_IDPt.clear();

  m_TH1F_TP_IDEff_Eta.clear();
  m_TH1F_TP_IDEff_Phi.clear();
  m_TH1F_TP_IDEff_Pt.clear();
    
  m_TH1F_TP_IDEff_D_Eta.clear();
  m_TH1F_TP_IDEff_N_Eta.clear();
  m_TH1F_TP_IDEff_D_Phi.clear();
  m_TH1F_TP_IDEff_N_Phi.clear();
  m_TH1F_TP_IDEff_D_Pt.clear();
  m_TH1F_TP_IDEff_N_Pt.clear();
    
  ATH_MSG_DEBUG( "Done clearing." );
    
  return sc;
}
//============================================================================================
StatusCode MuonInsituMonitoring::procCombinedHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG( "Begin procCombHistograms." );

  for (int h_index=0; h_index < (int)m_TH1F_TP_CombinedEff_D_Eta.size(); ++h_index)
    { 
      if (m_TH1F_TP_CombinedEff_D_Eta[h_index]->GetEntries() > 0) m_TH1F_TP_CombinedEff_Eta[h_index]->Divide(m_TH1F_TP_CombinedEff_N_Eta[h_index], m_TH1F_TP_CombinedEff_D_Eta[h_index], 1.0, 1.0, "");
      m_TH1F_TP_CombinedEff_Eta[h_index]->SetEntries(m_TH1F_TP_CombinedEff_Eta[h_index]->GetNbinsX());
    }
  for (int h_index=0; h_index < (int)m_TH1F_TP_CombinedEff_D_Phi.size(); ++h_index)
    {
      if (m_TH1F_TP_CombinedEff_D_Phi[h_index]->GetEntries() > 0) m_TH1F_TP_CombinedEff_Phi[h_index]->Divide(m_TH1F_TP_CombinedEff_N_Phi[h_index], m_TH1F_TP_CombinedEff_D_Phi[h_index], 1.0, 1.0, "");
      m_TH1F_TP_CombinedEff_Phi[h_index]->SetEntries(m_TH1F_TP_CombinedEff_Phi[h_index]->GetNbinsX());  
    } 
  for (int h_index=0; h_index < (int)m_TH1F_TP_CombinedEff_D_Pt.size(); ++h_index)
    {
      if (m_TH1F_TP_CombinedEff_D_Pt[h_index]->GetEntries() > 0) m_TH1F_TP_CombinedEff_Pt[h_index]->Divide( m_TH1F_TP_CombinedEff_N_Pt[h_index],  m_TH1F_TP_CombinedEff_D_Pt[h_index],  1.0, 1.0, "");
      m_TH1F_TP_CombinedEff_Pt[h_index]->SetEntries(m_TH1F_TP_CombinedEff_Pt[h_index]->GetNbinsX());  
    }
 
  //   float ratio=0.;
  //      
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_CombinedEff_D_Eta.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for COMB Eta, " << h_index );
  //        
  //       if( (m_TH1F_TP_CombinedEff_N_Eta[h_index]->GetNbinsX() == m_TH1F_TP_CombinedEff_D_Eta[h_index]->GetNbinsX()) && (m_TH1F_TP_CombinedEff_D_Eta[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_CombinedEff_N_Eta[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_CombinedEff_D_Eta[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_CombinedEff_N_Eta[h_index]->GetBinContent(ibin) / m_TH1F_TP_CombinedEff_D_Eta[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_CombinedEff_Eta[h_index]->Fill(m_TH1F_TP_CombinedEff_N_Eta[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	}
  //     }
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_CombinedEff_D_Pt.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for COMB Pt, " << h_index );
  //       if( (m_TH1F_TP_CombinedEff_N_Pt[h_index]->GetNbinsX() == m_TH1F_TP_CombinedEff_D_Pt[h_index]->GetNbinsX()) && (m_TH1F_TP_CombinedEff_D_Pt[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_CombinedEff_N_Pt[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_CombinedEff_D_Pt[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_CombinedEff_N_Pt[h_index]->GetBinContent(ibin) / m_TH1F_TP_CombinedEff_D_Pt[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_CombinedEff_Pt[h_index]->Fill(m_TH1F_TP_CombinedEff_N_Pt[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	} 
  //     }
  //   for (int h_index=0; h_index < (int)m_TH1F_TP_CombinedEff_D_Phi.size(); h_index++)
  //     {
  //       ATH_MSG_DEBUG( "Doing divisions for COMB phi, " << h_index );
  //       if( (m_TH1F_TP_CombinedEff_N_Phi[h_index]->GetNbinsX() == m_TH1F_TP_CombinedEff_D_Phi[h_index]->GetNbinsX()) && (m_TH1F_TP_CombinedEff_D_Phi[h_index])->GetEntries() > 0)
  // 	{
  // 	  for(int ibin = 0; ibin < (int)m_TH1F_TP_CombinedEff_N_Phi[h_index]->GetNbinsX(); ibin++)
  // 	    {
  // 	      if (m_TH1F_TP_CombinedEff_D_Phi[h_index]->GetBinContent(ibin) != 0){
  // 	        ratio = m_TH1F_TP_CombinedEff_N_Phi[h_index]->GetBinContent(ibin) / m_TH1F_TP_CombinedEff_D_Phi[h_index]->GetBinContent(ibin);
  // 	      } else ratio = 0;
  // 	      m_TH1F_TP_CombinedEff_Phi[h_index]->Fill(m_TH1F_TP_CombinedEff_N_Phi[h_index]->GetBinCenter(ibin), ratio); 
  // 	    }
  // 	}    
  //     }
    
  ATH_MSG_DEBUG( "Done dividing." );

/* Obsolete
  int h_index = -1;
  for (int ii=0; ii<(int)m_MuonContainerLocation.size(); ++ii)
    {
      if (m_doCombined[ii] == 0) continue;
      ++h_index;
      /////////Nectar////////////
      m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"CombinedMuon_Insitu"]->plotEfficiencyProjectionToEta((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Eta"]));
      ATH_MSG_DEBUG( "Done apCombEfficiencyMatrix Eta." ); 
       
      m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"CombinedMuon_Insitu"]->plotEfficiencyProjectionToPhi((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Phi"]));
      ATH_MSG_DEBUG( "Done apCombEfficiencyMatrix Phi." ); 
       
      m_apMuonEfficiencyMatrix[m_MuonContainerLocation[h_index]+"CombinedMuon_Insitu"]->plotEfficiencyProjectionToPt((m_EfficiencyHistograms[m_MuonContainerLocation[h_index]+"Combined_Insitu_Efficiency_Pt"]));
      ATH_MSG_DEBUG( "Done apCombEfficiencyMatrix Pt." ); 

      ////////Nectar/////////////
    }
*/
    
  m_TH1F_TP_CombinedInvariantMass.clear();
    
  m_TH1F_CombinedPt.clear();

  m_TH1F_TP_CombinedEff_Eta.clear();
  m_TH1F_TP_CombinedEff_Phi.clear();
  m_TH1F_TP_CombinedEff_Pt.clear();
   
  m_TH1F_TP_CombinedEff_D_Eta.clear();
  m_TH1F_TP_CombinedEff_N_Eta.clear();
  m_TH1F_TP_CombinedEff_D_Phi.clear();
  m_TH1F_TP_CombinedEff_N_Phi.clear();
  m_TH1F_TP_CombinedEff_D_Pt.clear();
  m_TH1F_TP_CombinedEff_N_Pt.clear();
    
  ATH_MSG_DEBUG( "Done clearing." );
    
  return sc;
}


//============================================================================================
