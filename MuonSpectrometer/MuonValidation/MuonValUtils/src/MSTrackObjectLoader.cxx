/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI) - M.Bellomo (INFN) 
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: This class loads all necessary Information from AODs and converts
// them to MSTrackObjects. Moreover the Track-Assoziation is done here. This means
// That  each Track can be assoziated to another Track or Particle...
///////////////////////////////////////////////////////////////////////////
 
// GaudiKernel and StoreGate
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// EventInfo - HepMC - McParticleEvent
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "HepMC/GenEvent.h"//new
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticle.h" 

//JetTagEvent
//#include "JetTagEvent/ParticleJetContainer.h"
//#include "JetTagEvent/ParticleJet.h"
#include "JetTagEvent/JetConstituent.h"
#include "JetEvent/JetCollection.h"
#include "JetTagEvent/TrackConstituents.h"

//AnalysisTools
//#include "AnalysisTools/IAnalysisTools.h"

// Particle and Track
#include "Particle/TrackParticleContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"

//Muon and Electron Container
#include "muonEvent/MuonContainer.h" 
#include "egammaEvent/ElectronContainer.h"

//For Trigger
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"

#include "MuonValUtils/MSTrackObjectLoader.h"
 
#include "TTree.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////

MSTrackObjectLoader::MSTrackObjectLoader()
{
  m_ESD 			= false;
  m_assoProperties	= false;
  m_assoTracksToTracks	= false;
  m_maxHalo		= 0.5;
  m_minHalo		= 0.05;
}


void	MSTrackObjectLoader::setupTrackPropertyAssoziator(bool assoProperties, bool assoTracktoTracks, double coneDistance_min, double coneDistance_max)
{
  m_assoProperties	= assoProperties;
  m_assoTracksToTracks	= assoTracktoTracks;
  m_maxHalo		= coneDistance_max;
  m_minHalo		= coneDistance_min;
}


MSTrackObjectLoader::MSTrackObjectLoader(StoreGateSvc *storeGate, bool doAssoziation, bool MustBeCombinedTrack, bool fromESD, ITruthParticleCnvTool* m_cnvTool)
{
  m_ESD			= fromESD;
  m_assoProperties	= false;
  m_maxHalo		= 0.5;
  m_minHalo		= 0.05;
  setup(storeGate, doAssoziation, MustBeCombinedTrack, fromESD, m_cnvTool);
}


void	MSTrackObjectLoader::setup(StoreGateSvc *storeGate, bool doAssoziation, bool MustBeCombinedTrack, bool fromESD, ITruthParticleCnvTool* m_cnvTool)
{
  cnvTool			= m_cnvTool;
  m_ESD			= fromESD;
  m_MustBeCombinedTrack	= MustBeCombinedTrack;
  m_storeGate		= storeGate;
  m_doAssoziation		= doAssoziation;

  m_AssoziationMuonContainerNames.clear();
  m_AssoziationTrackContainerNames.clear();
	
  msTrackAssociator.setup(storeGate, 0.05, 5000.);
}


MSTrackObjectLoader::~MSTrackObjectLoader()
{
}


bool	MSTrackObjectLoader::loadTruthInfo(MSEvent &msEvent)
{
  StatusCode sc = StatusCode::SUCCESS;
  const TruthParticleContainer* mcpartTES;

  if (m_ESD == true )
    {
      /*convert on the fly TruthEvent from the ESD to TruthParticleContainer*/ 
      if ( cnvTool->execute().isFailure() ) 
	{
	  return false;
	}
      sc=m_storeGate->retrieve( mcpartTES, "SpclMC_ESD");
    }
	
  else
    {
      sc=m_storeGate->retrieve( mcpartTES, "SpclMC");
    } 
	  
  if( sc.isFailure()  ||  !mcpartTES )
    {
      std::cout<< "No MC truth particle container found in TDS"<< std::endl;
      return false;
    }
  else
    {
      MSTrackObject Particle;
      TruthParticleContainer::const_iterator trueItr  = mcpartTES->begin();
      TruthParticleContainer::const_iterator trueItrE = mcpartTES->end();

      for (; trueItr != trueItrE; ++trueItr)
	{
	  //if ((*trueItr)->status()==1)
	  {

	    Particle.clear();
	    if ( 	(((abs((*trueItr)->pdgId())==13) && ((*trueItr)->status()==1))) || ((abs((*trueItr)->pdgId())==23)))
	      {
		double vx, vy, vz;
		//TruthParticle parTruth = (*trueItr);
		get_vertex((*trueItr),vx, vy, vz);
		double d0 = sqrt(pow(vx,2) + pow(vy,2));
		double z0 = vz;
		double phi0 = asin(vx/d0);
		if (vy<0.0) phi0*=(-1);
		Particle.setObject(	(*trueItr)->pt(),
					(*trueItr)->eta(),
					(*trueItr)->phi(),
					d0,
					z0,
					(int)(*trueItr)->charge(),
					(*trueItr)->pdgId());
		Particle.Mother = getMother((*trueItr));
		//if ((*trueItr)->pt()>4000.) std::cout<<"  "<<Particle.Mother<<"  "<<(*trueItr)->pdgId()<<std::endl;
		Particle.AnalysisParameters.dMass   = (*trueItr)->m();
		for (int n=0; n<(int)m_AssoziationMuonContainerNames.size(); n++)
		  msTrackAssociator.assoziateMuonCollectionTrack(Particle, m_AssoziationMuonContainerNames[n]);
		for (int n=0; n<(int)m_AssoziationTrackContainerNames.size(); n++)
		  msTrackAssociator.assoziateTrackContainerTrack(Particle, m_AssoziationTrackContainerNames[n]);
		msTrackAssociator.assoziateTriggerInformation(Particle);
		msEvent.addParticle(Particle);
	      }
	  }
	}
    }

  return true;
}



bool	MSTrackObjectLoader::loadTrackInfo(std::string ContainerName, MSEvent &msEvent)
{
  StatusCode sc = StatusCode::SUCCESS;
  const Rec::TrackParticleContainer* trackTES;
 
  Rec::TrackParticleContainer::const_iterator trackItr;
  Rec::TrackParticleContainer::const_iterator trackItrE;
  const Trk::Perigee* pMeasuredPerigee = (*trackItr)->measuredPerigee();
	
  sc=m_storeGate->retrieve( trackTES, ContainerName);

  if( sc.isFailure()  ||  !trackTES )
    {
      std::cout << "No " << ContainerName << "TrackParticleContainer found in TDS"<< std::endl;
      return false;
    }
  else
    {
      MSTrackObject TrackParticle;
      msEvent.setDefaultTrackReconstruction(ContainerName);
      trackItr  = trackTES->begin();
      trackItrE = trackTES->end();
      for (; trackItr != trackItrE; ++trackItr)
	{
	  if ((*trackItr)->pt()>4000.)
	    {
	      TrackParticle.clear();
	      TrackParticle.setObject(	(*trackItr)->pt(),
					(*trackItr)->eta(),
					(*trackItr)->phi(),
					(double)(pMeasuredPerigee->parameters()[Trk::d0]),
					(double)(pMeasuredPerigee->parameters()[Trk::z0]),
					(int)(*trackItr)->charge(),
					(int)(*trackItr)->charge()*13); //pdgId

	      double errPT = errorPT(	(double)(pMeasuredPerigee->parameters()[Trk::theta]),
					(double)(pMeasuredPerigee->parameters()[Trk::qOverP]),
					(double)(*pMeasuredPerigee->covariance())(Trk::theta,Trk::theta), 
					(double)(*pMeasuredPerigee->covariance())(Trk::qOverP,Trk::qOverP) 
					);
	      double errEta = errorEta(	(double)(pMeasuredPerigee->parameters()[Trk::theta]),
					(double)(*pMeasuredPerigee->covariance())(Trk::theta,Trk::theta));

	      TrackParticle.setObjectErrors(	errPT,
						errEta,
						(double)(*pMeasuredPerigee->covariance())(Trk::phi,Trk::phi)
						);

	      TrackParticle.Chi2		= (double)(*trackItr)->fitQuality()->chiSquared();
	      TrackParticle.Chi2oDOF		= (double)(*trackItr)->fitQuality()->numberDoF();
	      TrackParticle.Hits		= (int)(*trackItr)->trackSummary()->get(Trk::numberOfMdtHits);
	      TrackParticle.EnergyLoss	= 0.0;
	      TrackParticle.Type		= 3;
	      msTrackAssociator.assoziateTruthTrack(TrackParticle);
	      msTrackAssociator.assoziateTriggerInformation(TrackParticle);

	      if (m_assoTracksToTracks)
		{
		  for (int n=0; n<(int)m_AssoziationMuonContainerNames.size(); n++)
		    {
		      if (ContainerName!=m_AssoziationMuonContainerNames[n])
			msTrackAssociator.assoziateMuonCollectionTrack(TrackParticle, m_AssoziationMuonContainerNames[n]);
		    }
		  for (int n=0; n<(int)m_AssoziationTrackContainerNames.size(); n++)
		    {
		      if (ContainerName!=m_AssoziationTrackContainerNames[n])
			{
			  msTrackAssociator.assoziateTrackContainerTrack(TrackParticle, m_AssoziationTrackContainerNames[n]);
			}
		    }
		}

	      if (m_assoProperties)
		{
		  msTrackAssociator.assoziateJetEnergyInCone(TrackParticle, m_maxHalo);
		  msTrackAssociator.assoziateInnerTrackPropertiesInCone(TrackParticle, m_minHalo, m_maxHalo);
		  msTrackAssociator.assoziateElectronInCone(TrackParticle, m_maxHalo);
		}
	      msEvent.addTrack(TrackParticle);
	    }
	}
    }
  return true;
}


bool	MSTrackObjectLoader::loadMuonCollectionInfo(std::string ContainerName, MSEvent &msEvent)
{
  StatusCode sc = StatusCode::SUCCESS;
	
  if (m_ESD == true) 
    {
	
      //const CombinedMuonContainer* muonTES = 0;  //the ESD muon container for TES
      //sc=m_storeGate->retrieve( muonTES, ContainerName);
      const Analysis::MuonContainer* pCombinedMuonContainer;
      sc=m_storeGate->retrieve( pCombinedMuonContainer, ContainerName); 
      if( sc.isFailure()  ||  !pCombinedMuonContainer )
	{
	  std::cout << "No " << ContainerName << "ESD muon container found in TDS"<< std::endl;
	  return false;
	} else
	{
	  MSTrackObject TrackParticle;
	  msEvent.setDefaultTrackReconstruction(ContainerName);
		
	  //CombinedMuonContainer::const_iterator muonItr = muonTES->begin();
	  //CombinedMuonContainer::const_iterator muonItrE = muonTES->end();
		 
	  Analysis::MuonContainer::const_iterator muonItr = pCombinedMuonContainer->begin(); 
	  Analysis::MuonContainer::const_iterator muonItrE = pCombinedMuonContainer->end();
		 
	  for (; muonItr != muonItrE; ++muonItr) 
	    {
	      int muonStatus = -1;
	      int numberOfMdtHits=-1;
	      double d0=-999.9, z0=-999.9;
	      double errPT=-99.9, errEta=-9.9, errPhi=-9.9;
	      // has a combined track but it is not the best matched
	      if ( (*muonItr)->hasCombinedMuonTrackParticle() && !(*muonItr)->bestMatch() ) muonStatus = 0;
	      else if ( (*muonItr)->isCombinedMuon() )           muonStatus = 1; // combined muons
	      else if ( (*muonItr)->isStandAloneMuon() )         muonStatus = 2; // standalone muon
	      else if ( (*muonItr)->isSegmentTaggedMuon() && (*muonItr)->inDetTrackParticle()!=0 ) muonStatus = 3; // lowPt reconsrtructed muon
			 
	      std::pair<double,double> eLoss = (*muonItr)->energyLoss();

	      if ((muonStatus==1) || (muonStatus==2))
		{
		  const Rec::TrackParticle* muonSpectrometerTrack = (*muonItr)->muonSpectrometerTrackParticle();
		  numberOfMdtHits=muonSpectrometerTrack->trackSummary()->get( Trk::numberOfMdtHits ); 
		  const Trk::TrackParameters* mstpar = 0;
		  if (muonSpectrometerTrack) mstpar = dynamic_cast<const Trk::TrackParameters*>(&muonSpectrometerTrack->definingParameters());
		  if (mstpar) {
 
		    d0 = mstpar->parameters()[Trk::d0];
		    z0 = mstpar->parameters()[Trk::z0];
		    // phi0 = muonSpectrometerTrack->measuredPerigee()->parameters()[Trk::phi0];


		    errPT = errorPT(	(double)mstpar->parameters()[Trk::theta],
					(double)mstpar->parameters()[Trk::qOverP],
					(double)(*mstpar->covariance())(Trk::theta,Trk::theta),
					(double)(*mstpar->covariance())(Trk::qOverP,Trk::qOverP));
		    errEta = errorEta(	(double)mstpar->parameters()[Trk::theta],
					(double)(*mstpar->covariance())(Trk::theta,Trk::theta));
		    errPhi = (*mstpar->covariance())(Trk::phi,Trk::phi);
		  }
		}

	      TrackParticle.clear();
	      TrackParticle.setObject(	(*muonItr)->pt(),
					(*muonItr)->eta(),
					(*muonItr)->phi(),
					d0,
					z0,
					(int)(*muonItr)->charge(),
					(int)(*muonItr)->charge()*13);
	      TrackParticle.setObjectErrors(errPT, errEta, errPhi);
	      TrackParticle.Chi2		= (double)(*muonItr)->fitChi2();
	      TrackParticle.Chi2oDOF		= (double)(*muonItr)->fitChi2OverDoF();
	      TrackParticle.Hits		= numberOfMdtHits;
	      TrackParticle.EnergyLoss	= eLoss.first;
	      TrackParticle.Type		= muonStatus;
			
	      //TrackParticle.AnalysisParameters.dETIsolation = (double)(*muonItr)->parameter( CombinedMuonParameters::etcone);
	      //			TrackParticle.AnalysisParameters.dETIsolation = (double)(*muonItr)->parameter( MuonParameters::etcone);
	      //                      number of etcone was reduced to 4 : 0.1, 0.2, 0.3, 0.4 
	      TrackParticle.AnalysisParameters.dETIsolation = (double)(*muonItr)->parameter( MuonParameters::etcone40);
	      msTrackAssociator.assoziateTriggerInformation(TrackParticle);
	      msTrackAssociator.assoziateTruthTrack(TrackParticle);

	      if (m_assoTracksToTracks)
		{
		  for (int n=0; n<(int)m_AssoziationMuonContainerNames.size(); n++)
		    {
		      if (ContainerName!=m_AssoziationMuonContainerNames[n])
			msTrackAssociator.assoziateMuonCollectionTrack(TrackParticle, m_AssoziationMuonContainerNames[n]);
		    }
		  for (int n=0; n<(int)m_AssoziationTrackContainerNames.size(); n++)
		    {
		      if (ContainerName!=m_AssoziationTrackContainerNames[n])
			msTrackAssociator.assoziateTrackContainerTrack(TrackParticle, m_AssoziationTrackContainerNames[n]);
		    }
		}

	      if (m_assoProperties)
		{
		  msTrackAssociator.assoziateJetEnergyInCone(TrackParticle, m_maxHalo);
		  msTrackAssociator.assoziateInnerTrackPropertiesInCone(TrackParticle, m_minHalo, m_maxHalo);
		  msTrackAssociator.assoziateElectronInCone(TrackParticle, m_maxHalo);
		}

	      if ((muonStatus==1) && (m_MustBeCombinedTrack==true))	msEvent.addTrack(TrackParticle);
	      if (m_MustBeCombinedTrack==false)			msEvent.addTrack(TrackParticle);
	    }
	}
    } //if ESD
	
  else 
    {
      const Analysis::MuonContainer* muonTES; //the AOD muon container for TES
      sc=m_storeGate->retrieve( muonTES, ContainerName);

      if( sc.isFailure()  ||  !muonTES )
	{
	  std::cout << "No " << ContainerName << "AOD muon container found in TDS"<< std::endl;
	  return false;
	} else
	{
	  MSTrackObject TrackParticle;
	  msEvent.setDefaultTrackReconstruction(ContainerName);
		
	  Analysis::MuonContainer::const_iterator muonItr = muonTES->begin();
	  Analysis::MuonContainer::const_iterator muonItrE = muonTES->end();
		 
	  for (; muonItr != muonItrE; ++muonItr)
	    {
	      int muonStatus = -1;
	      int numberOfMdtHits=-1;
	      double d0=-999.9, z0=-999.9;//, phi0=-9.9;
	      double errPT=-99.9, errEta=-9.9, errPhi=-9.9;
	      // has a combined track but it is not the best matched
	      if ( (*muonItr)->hasCombinedMuonTrackParticle() && !(*muonItr)->bestMatch() ) muonStatus = 0;
	      else if ( (*muonItr)->isCombinedMuon() )           muonStatus = 1; // combined muons
	      else if ( (*muonItr)->isStandAloneMuon() )         muonStatus = 2; // standalone muon
	      else if ( (*muonItr)->isSegmentTaggedMuon() && (*muonItr)->inDetTrackParticle()!=0 ) muonStatus = 3; // lowPt reconsrtructed muon
 
	      std::pair<double,double> eLoss = (*muonItr)->energyLoss();

	      if ((muonStatus==1) || (muonStatus==2))
		{
		  const	Rec::TrackParticle * muonSpectrometerTrack = (*muonItr)->muonSpectrometerTrackParticle();
		  numberOfMdtHits=muonSpectrometerTrack->trackSummary()->get( Trk::numberOfMdtHits );
		  const Trk::TrackParameters* mstpar = 0;
		  if (muonSpectrometerTrack) mstpar = dynamic_cast<const Trk::TrackParameters*>(&muonSpectrometerTrack->definingParameters());
		  if (mstpar) {




		    d0 = mstpar->parameters()[Trk::d0];
		    z0 = mstpar->parameters()[Trk::z0];
		    // phi0 = muonSpectrometerTrack->measuredPerigee()->parameters()[Trk::phi0];


		    errPT = errorPT(	(double)mstpar->parameters()[Trk::theta],
					(double)mstpar->parameters()[Trk::qOverP],
					(double)(*mstpar->covariance())(Trk::theta,Trk::theta),
					(double)(*mstpar->covariance())(Trk::qOverP,Trk::qOverP));
		    errEta = errorEta(	(double)mstpar->parameters()[Trk::theta],
					(double)(*mstpar->covariance())(Trk::theta,Trk::theta));
		    errPhi = (*mstpar->covariance())(Trk::phi,Trk::phi);
		  }
		}
	      TrackParticle.clear();
	      TrackParticle.setObject(	(*muonItr)->pt(),
					(*muonItr)->eta(),
					(*muonItr)->phi(),
					d0,
					z0,
					(int)(*muonItr)->charge(),
					(int)(*muonItr)->charge()*13);
	      TrackParticle.setObjectErrors(errPT, errEta, errPhi);
	      TrackParticle.Chi2		= (double)(*muonItr)->fitChi2();
	      TrackParticle.Chi2oDOF		= (double)(*muonItr)->fitChi2OverDoF();
	      TrackParticle.Hits		= numberOfMdtHits;
	      TrackParticle.EnergyLoss	= eLoss.first;
	      TrackParticle.Type		= muonStatus;
			
	      //			TrackParticle.AnalysisParameters.dETIsolation = (double)(*muonItr)->parameter( MuonParameters::etcone);
	      //                      number of etcone was reduced to 4 : 0.1, 0.2, 0.3, 0.4 
	      TrackParticle.AnalysisParameters.dETIsolation = (double)(*muonItr)->parameter( MuonParameters::etcone40);
			
	      msTrackAssociator.assoziateTriggerInformation(TrackParticle);
	      msTrackAssociator.assoziateTruthTrack(TrackParticle);

	      if (m_assoTracksToTracks)
		{
		  for (int n=0; n<(int)m_AssoziationMuonContainerNames.size(); n++)
		    {
		      if (ContainerName!=m_AssoziationMuonContainerNames[n])
			msTrackAssociator.assoziateMuonCollectionTrack(TrackParticle, m_AssoziationMuonContainerNames[n]);
		    }
		  for (int n=0; n<(int)m_AssoziationTrackContainerNames.size(); n++)
		    {
		      if (ContainerName!=m_AssoziationTrackContainerNames[n])
			msTrackAssociator.assoziateTrackContainerTrack(TrackParticle, m_AssoziationTrackContainerNames[n]);
		    }
		}

	      if (m_assoProperties)
		{
		  msTrackAssociator.assoziateJetEnergyInCone(TrackParticle, m_maxHalo);
		  msTrackAssociator.assoziateInnerTrackPropertiesInCone(TrackParticle, m_minHalo, m_maxHalo);
		  msTrackAssociator.assoziateElectronInCone(TrackParticle, m_maxHalo);
		}

	      if ((muonStatus==1) && (m_MustBeCombinedTrack==true))	msEvent.addTrack(TrackParticle);
	      if (m_MustBeCombinedTrack==false)			msEvent.addTrack(TrackParticle);
	    }
	}
    } //if AOD
  return true;
}



bool	MSTrackObjectLoader::loadMuonTriggerInfo(MSEvent &msEvent)
{
  StatusCode sc = StatusCode::SUCCESS;

  // load trigger decision
  std::string TriggerDecisionKey="TriggerDecision";
  std::string new_TD = TriggerDecisionKey; 
  std::string lastTD = TriggerDecisionKey; 
  bool old_td  = false;
  do {
    if (m_storeGate->contains<TriggerDecision>( new_TD )) {
      old_td = true;           // flag for do...while loop
      lastTD = new_TD;         // keep last TD key before updating new
      new_TD = new_TD + "+";   // update StoreGate key
    } else {
      old_td = false;           // to stop loop
    }
  } while (old_td);
  // retrieve latest TriggerDecision
  const TriggerDecision* trigDec = NULL;
  sc = m_storeGate->retrieve(trigDec, lastTD);
  if ( sc.isFailure() ) {
    std::cout << "Failed to retrieve TriggerDecision" << std::endl;
  } else {
    msEvent.setTriggerDecision(trigDec);
  }  
	
  const DataHandle<LVL1_ROI> lvl1ROI;

  std::string m_lvl1ROIContainerName="LVL1_ROI";

  if( ( sc = m_storeGate->retrieve( lvl1ROI, m_lvl1ROIContainerName) ).isFailure() )
    {
      std::cout << "LVL1_ROI with key: " <<  m_lvl1ROIContainerName << " not found in StoreGate" << std::endl;
      return false;
    }

  MSTrackObject TriggerObject;

  msEvent.setDefaultTrigger("Trigger_Level_1");
	
  for( LVL1_ROI::muons_type::const_iterator it = lvl1ROI->getMuonROIs().begin(); it != lvl1ROI->getMuonROIs().end(); ++it )
    {
      double Level1_Pt=it->getThrValue();
      double Level1_Phi=it->getPhi();
      double Level1_Eta =it->getEta();

      TriggerObject.clear();
      TriggerObject.setObject(Level1_Pt, Level1_Eta, Level1_Phi, 0.0, 0.0, 0, -1);
      msEvent.addTriggerInfo(TriggerObject);

      std::cout<<"Level 1 Trigger Output "<<Level1_Phi<<"   "<<Level1_Eta<< "  "<<Level1_Pt<<std::endl;
    }

  // Retrieve LVL2 muon spectrometer standalone muons
  const DataHandle<MuonFeature> muonFeature;
  const DataHandle<MuonFeature> muonFeaturesEnd;
  sc = m_storeGate->retrieve(muonFeature,muonFeaturesEnd);
  if (sc != StatusCode::SUCCESS)
    {
      std::cout << "Can't retrieve the MuonFeatures" << std::endl;
      return false;
    }
	
  // Loop on Lvl2 reconstructed muons
  msEvent.setDefaultTrigger("Trigger_Level_2_MS");
  for ( ; muonFeature != muonFeaturesEnd ; ++muonFeature )
    {
      double Level2_Eta	= muonFeature->eta();
      double Level2_Phi	= muonFeature->phi();
      double Level2_Pt	= fabs(muonFeature->pt())*1000;

      int Level2_Charge       = (int) (muonFeature->pt()/fabs(muonFeature->pt()));

      TriggerObject.clear();
      TriggerObject.setObject(Level2_Pt, Level2_Eta, Level2_Phi, 0.0, 0.0, Level2_Charge, -1);
      msEvent.addTriggerInfo(TriggerObject);

      std::cout<<"Level 2 MS Trigger Output "<<Level2_Phi<<"   "<<Level2_Eta<< "  "<<Level2_Pt<<std::endl;
    }

  // Retrieve LVL2 combined muons
  const DataHandle<CombinedMuonFeature> CmuonFeature;
  const DataHandle<CombinedMuonFeature> CmuonFeaturesEnd;
  sc = m_storeGate->retrieve(CmuonFeature,CmuonFeaturesEnd);
  if (sc != StatusCode::SUCCESS)
    {
      std::cout << "Can't retrieve the MuonFeatures" << std::endl;
      return false;
    }
	
  // Loop on Lvl2 reconstructed muons
  msEvent.setDefaultTrigger("Trigger_Level_2_CB");
  for ( ; CmuonFeature != CmuonFeaturesEnd ; ++CmuonFeature )
    {
      double Level2_Eta	= CmuonFeature->eta();
      double Level2_Phi	= CmuonFeature->phi();
      double Level2_Pt	= fabs(CmuonFeature->pt());
	  
      int Level2_Charge       = (int) (CmuonFeature->pt()/fabs(CmuonFeature->pt()));
	  
      TriggerObject.clear();
      TriggerObject.setObject(Level2_Pt, Level2_Eta, Level2_Phi, 0.0, 0.0, Level2_Charge, -1);
      msEvent.addTriggerInfo(TriggerObject);
	  
      std::cout<<"Level 2 CB Trigger Output "<<Level2_Phi<<"   "<<Level2_Eta<< "  "<<Level2_Pt<<std::endl;
    }
	
  // Retrieve EF muons from  
  // MuonTrigEF  container  
	
  const DataHandle<TrigMuonEFContainer> trigEcontIt; 
  const DataHandle<TrigMuonEFContainer> trigEcontEnd;

  sc = m_storeGate->retrieve(trigEcontIt,trigEcontEnd);
  if (sc != StatusCode::SUCCESS) {
    std::cout << "No AOD TrigMuonEFContainer container found in TDS" << std::endl;
    return false;
  }

  std::cout << "TrigMuonEFContainer container found in TDS" << std::endl;

  // Loop on EF reconstructed muons
  for (;trigEcontIt != trigEcontEnd; ++trigEcontIt) {
    //if offline objects of the same type exist you will need to select 
    //the trigger ones for your analysis, a way to do this is to look for 
    // "_HLTAutoKey" in the name of the container: 
    string contName = trigEcontIt.key();
    // find trigger containers + protection against failed find operation
    if( contName.find("HLTAutoKey") != string::npos) { 
	    
      TrigMuonEFContainer::const_iterator trigmuItr = trigEcontIt->begin();
      TrigMuonEFContainer::const_iterator trigmuItrE = trigEcontIt->end();
	    
      for(; trigmuItr != trigmuItrE; ++trigmuItr) {
	      
	std::string ef_string_final = "Trigger_Level_3_";
	     
	// combined ef track
	if((*trigmuItr)->MuonCode() == 121 || (*trigmuItr)->MuonCode() == 111 || (*trigmuItr)->MuonCode() == 120)
	  ef_string_final += "CB";
	      
	// ms only track extrapolated to ip
	else if((*trigmuItr)->MuonCode() == 1) 
	  ef_string_final += "MS";

	// ms only tracks at ms entrance (not used)
	else if((*trigmuItr)->MuonCode() == 0) 
	  ef_string_final += "MSonly";	      
	      
	else 
	  ef_string_final += "unknown";
	      
	msEvent.setDefaultTrigger(ef_string_final);
	TriggerObject.clear();
	TriggerObject.setObject((*trigmuItr)->pt(),(*trigmuItr)->eta(),
				(*trigmuItr)->phi(), 0.0, 0.0, 0, -1);
	msEvent.addTriggerInfo(TriggerObject);
	      
	std::cout<<"Event Filter Trigger Output (" << ef_string_final << ") " << (*trigmuItr)->MuonCode() << " " << 
	  (*trigmuItr)->phi() <<"   "<< (*trigmuItr)->eta() << "  "<< (*trigmuItr)->pt() <<std::endl;
      }
    }
  }
	
  return true;
}


int MSTrackObjectLoader::getMother(const TruthParticle *truthParticle)
{
  vector<long> m_ParticlePDG_List;

  if ((truthParticle->hasMother((PDG::pidType)23)==true) || (truthParticle->hasMother((PDG::pidType)-23))==true)		return 23;	// Z-Boson
  if ((truthParticle->hasMother((PDG::pidType)24)==true) || (truthParticle->hasMother((PDG::pidType)-24))==true)		return 24;	// W-Boson
  if ((truthParticle->hasMother((PDG::pidType)6)==true) || (truthParticle->hasMother((PDG::pidType)-6))==true)		return 6;	// Top

  m_ParticlePDG_List.clear();
  for (long i=511; i<=599; i++) m_ParticlePDG_List.push_back(i);
  for (int i=0; i<(int)m_ParticlePDG_List.size(); i++)
    {
      if (	(truthParticle->hasMother((PDG::pidType)m_ParticlePDG_List[i])==true) ||
		(truthParticle->hasMother((PDG::pidType)(-m_ParticlePDG_List[i])))==true) 				return 511;	// Bottom Mesons
    }

  m_ParticlePDG_List.clear();
  for (long i=5122; i<=5554; i++) m_ParticlePDG_List.push_back(i);
  for (int i=0; i<(int)m_ParticlePDG_List.size(); i++)
    {
      if (	(truthParticle->hasMother((PDG::pidType)m_ParticlePDG_List[i])==true) ||
		(truthParticle->hasMother((PDG::pidType)(-m_ParticlePDG_List[i])))==true) 				return 5122;	// Bottom Baryons
    }


  m_ParticlePDG_List.clear();
  for (long i=411; i<=499; i++) m_ParticlePDG_List.push_back(i);
  for (int i=0; i<(int)m_ParticlePDG_List.size(); i++)
    {
      if (	(truthParticle->hasMother((PDG::pidType)m_ParticlePDG_List[i])==true) ||
		(truthParticle->hasMother((PDG::pidType)(-m_ParticlePDG_List[i])))==true) 				return 411;	// Charmed Mesons
    }

  m_ParticlePDG_List.clear();
  for (long i=4122; i<=4444; i++) m_ParticlePDG_List.push_back(i);
  for (int i=0; i<(int)m_ParticlePDG_List.size(); i++)
    {
      if (	(truthParticle->hasMother((PDG::pidType)m_ParticlePDG_List[i])==true) ||
		(truthParticle->hasMother((PDG::pidType)(-m_ParticlePDG_List[i])))==true) 				return 4122;	// Charmed Baryons
    }

  if ((truthParticle->hasMother((PDG::pidType)15)==true) || (truthParticle->hasMother((PDG::pidType)-15))==true)		return 15;	// Tau

  if ((truthParticle->hasMother((PDG::pidType)211)==true) || (truthParticle->hasMother((PDG::pidType)-211))==true)	return 211;	// Pion

  if ((truthParticle->hasMother((PDG::pidType)321)==true) || (truthParticle->hasMother((PDG::pidType)-321))==true)	return 321;	// Kaon

  return 0;
}


int	MSTrackObjectLoader::get_vertex(const TruthParticle *par, double& vx, double& vy, double& vz)
{
  vx = 0.0;
  vy = 0.0;
  vz = 0.0;
  const HepMC::GenParticle* pgenp = par->genParticle();
  if ( pgenp == 0 ) return 11;
  const HepMC::GenVertex* pgenv = pgenp->production_vertex();
  if ( pgenv == 0 ) return 12;
  vx = pgenv->point3d().x();
  vy = pgenv->point3d().y();
  vz = pgenv->point3d().z();
  //vx = pgenv->position()[0];
  //vy = pgenv->position()[1];
  //vz = pgenv->position()[2];
  return 0;
}

double	MSTrackObjectLoader::cot(double theta)
{
  return cos(theta)/sin(theta);
}

double	MSTrackObjectLoader::errorPT	(double theta, double qOverP, double D_theta, double D_qOverP)
{
  //	double qOverPT = qOverP * sqrt(1.0+cot(theta)*cot(theta));
  double cotqOverPT2 = cot(theta)*cot(theta);
  double term1 = 1.0/(qOverP * (1.0+cotqOverPT2)) * D_qOverP * D_qOverP;
  double term2 = (cotqOverPT2*cotqOverPT2*pow((1.0+cotqOverPT2),2))/(qOverP*qOverP*pow((1.0+cotqOverPT2),3)) * D_theta* D_theta;
  double D_qOverPT = sqrt(term1 + term2);
  return D_qOverPT;
}

double	MSTrackObjectLoader::errorEta(double theta, double D_theta)
{
  return fabs(1.0+cot(theta)*cot(theta))/sqrt(1.0+cot(theta)*cot(theta))*D_theta;
}
