/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI) - M.Bellomo (INFN)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: This class assoziates MSTrackObjects to other
//	    Tracks/Particles/... in AOD-Containers
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
#include "muonEvent/Muon.h"
#include "egammaEvent/ElectronContainer.h"

//For Trigger
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
 
#include "MuonValUtils/MSTrackAssociator.h"
 
#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include "TTree.h"

/////////////////////////////////////////////////////////////////////////////

MSTrackAssociator::MSTrackAssociator()
{
	m_MaxConeDistance	= 0.05;
	m_MinimalMuonPt		= 5000.;
}


void	MSTrackAssociator::setup(StoreGateSvc *storeGate, double MaxConeDistance, double MinimalMuonPt)
{
	m_storeGate		= storeGate;
	m_MaxConeDistance	= MaxConeDistance;
	m_MinimalMuonPt		= MinimalMuonPt;
}

MSTrackAssociator::~MSTrackAssociator()
{
}


bool MSTrackAssociator::assoziateMuonCollectionTrack(MSTrackObject &object, string MuonCollectionName)
{
	int muonStatus(0);
	bool tag=false;
	double d0, z0;
	MSPhysicsObject assoziatedObject;

	StatusCode sc = StatusCode::SUCCESS;

	/// get the AOD muon container for TES
	Analysis::MuonContainer* muonTES = 0;

	sc=m_storeGate->retrieve( muonTES, MuonCollectionName);
	if( sc.isFailure()  ||  !muonTES )
	{
		std::cout << MSG::WARNING << "No AOD muon container found in TDS" << std::endl;
	} else
	{
		for (Analysis::MuonContainer::const_iterator muonItr= muonTES->begin();muonItr!=muonTES->end();++muonItr)
		{
			if ((*muonItr)->pt()>m_MinimalMuonPt)
			{
				if ( (*muonItr)->hasCombinedMuonTrackParticle() && !(*muonItr)->bestMatch() ) muonStatus = 0;
				else if ( (*muonItr)->isCombinedMuon() )           muonStatus = 1; // combined muons
				else if ( (*muonItr)->isStandAloneMuon() )         muonStatus = 2; // standalone muon
				else if ( (*muonItr)->isSegmentTaggedMuon() && (*muonItr)->inDetTrackParticle()!=0 ) muonStatus = 3; // lowPt reconsrtructed muon
 
				d0=0.0;
				z0=0.0;
				if ((muonStatus==1) || (muonStatus==2))
				{
					const	Rec::TrackParticle * muonSpectrometerTrack =
						(*muonItr)->muonSpectrometerTrackParticle();
					d0 = muonSpectrometerTrack->measuredPerigee()->parameters()[Trk::d0];
					z0 = muonSpectrometerTrack->measuredPerigee()->parameters()[Trk::z0];
				}	
				//const	Rec::TrackParticle * muonSpectrometerTrack = (*muonItr)->muonSpectrometerTrackParticle();
				assoziatedObject.setObject(	(*muonItr)->pt(),
								(*muonItr)->eta(),
								(*muonItr)->phi(),
								d0,
								z0,
								(int)(*muonItr)->charge(),
								(int)(*muonItr)->charge()*13);
				double dis = distance(assoziatedObject, object);
				if ( dis < m_MaxConeDistance)
				{
					tag=true;
					assoziatedObject.metricDistance = dis;
					object.setAssoziatedTrackInformation(MuonCollectionName, assoziatedObject);
				}
			}
		}
	}
	return tag;
}


bool MSTrackAssociator::assoziateTrackContainerTrack(MSTrackObject &object, string TrackContainerName)
{
	bool tag=false;
	MSPhysicsObject assoziatedObject;

	StatusCode sc = StatusCode::SUCCESS;
	const Rec::TrackParticleContainer* trackTDS=0;

	sc=m_storeGate->retrieve( trackTDS, TrackContainerName);

	if( sc.isFailure()  ||  !trackTDS )
	{
		std::cout << "No "<<TrackContainerName<<"-Container found in TDS"<< std::endl;
	}
	else
	{
		for (Rec::TrackParticleContainer::const_iterator trackItr= trackTDS->begin(); trackItr != trackTDS->end(); ++trackItr)
		{

			if ((*trackItr)->pt()>m_MinimalMuonPt)
			{
				assoziatedObject.clear();
				assoziatedObject.setObject(	(*trackItr)->pt(),
								(*trackItr)->eta(),
								(*trackItr)->phi(),
								(double)(*trackItr)->measuredPerigee()->parameters()[Trk::d0],
								(double)(*trackItr)->measuredPerigee()->parameters()[Trk::z0],
								(int)(*trackItr)->charge(), (int)(*trackItr)->charge()*13);

				double dis = distance(assoziatedObject, object);
				if (dis < m_MaxConeDistance)
				{
					tag=true;
					assoziatedObject.metricDistance = dis;
					object.setAssoziatedTrackInformation(TrackContainerName, assoziatedObject);
				}
			}
		}
	}

	return tag;
}


bool MSTrackAssociator::assoziateTriggerInformation(MSTrackObject &object)
{
	MSPhysicsObject assoziatedObject;
	bool tag=false;
	StatusCode sc = StatusCode::SUCCESS;

	const DataHandle<LVL1_ROI> lvl1ROI;
	std::string m_lvl1ROIContainerName="LVL1_ROI";

	if( ( sc = m_storeGate->retrieve( lvl1ROI, m_lvl1ROIContainerName) ).isFailure() )
	{
		std::cout << "LVL1_ROI with key: " <<  m_lvl1ROIContainerName << " not found in StoreGate" << std::endl;
	} else
	{
          LVL1_ROI::muons_type::const_iterator itMU  = (lvl1ROI->getMuonROIs()).begin();
          LVL1_ROI::muons_type::const_iterator itMUe = (lvl1ROI->getMuonROIs()).end();	
	
		//for( LVL1_ROI::muons_type::const_iterator it = lvl1ROI->getMuonROIs().begin(); it != lvl1ROI->getMuonROIs().end(); ++it )
		for( ; itMU != itMUe; ++itMU )
		
		{

		        double Level1_Pt=itMU->getThrValue();
			double Level1_Phi=itMU->getPhi();
			double Level1_Eta = itMU->getEta();

			assoziatedObject.clear();
			assoziatedObject.setObject(Level1_Pt, Level1_Eta, Level1_Phi, 0.0, 0.0, 0, -1);

			double dis = distance(assoziatedObject, object);
			if (dis < 0.3)
			{
				tag=true;
				assoziatedObject.metricDistance = dis;
				object.setAssoziatedTriggerInformation("Trigger_Level_1", assoziatedObject);
			}
		}
	}

	// Retrieve LVL2 ms standalone muons
	const DataHandle<MuonFeature> muonFeature;
	const DataHandle<MuonFeature> muonFeaturesEnd;

	sc = m_storeGate->retrieve(muonFeature,muonFeaturesEnd);
	if (sc != StatusCode::SUCCESS)
	{
		//std::cout << "Can't retrieve the MuonFeatures" << std::endl;
	} else
	{
		// Loop on Lvl2 reconstructed muons
		for ( ; muonFeature != muonFeaturesEnd ; ++muonFeature )
		{
		  double Level2_Eta	= muonFeature->eta();
		  double Level2_Phi	= muonFeature->phi();
		  double Level2_Pt	= fabs(muonFeature->pt())*1000;
		  
		  int Level2_Charge       = (int) muonFeature->pt()/fabs(muonFeature->pt());

			assoziatedObject.clear();
			assoziatedObject.setObject(Level2_Pt, Level2_Eta, Level2_Phi, 0.0, 0.0, Level2_Charge, -1);

			double dis = distance(assoziatedObject, object);
			if (dis < 0.3)
			{
				tag=true;
				assoziatedObject.metricDistance = dis;
				object.setAssoziatedTriggerInformation("Trigger_Level_2_MS", assoziatedObject);
			}
		}
	}

	// Retrieve LVL2 combined muons
	const DataHandle<CombinedMuonFeature> CmuonFeature;
	const DataHandle<CombinedMuonFeature> CmuonFeaturesEnd;

	sc = m_storeGate->retrieve(CmuonFeature,CmuonFeaturesEnd);
	if (sc != StatusCode::SUCCESS)
	{
		//std::cout << "Can't retrieve the MuonFeatures" << std::endl;
	} else
	{
		// Loop on Lvl2 reconstructed muons
		for ( ; CmuonFeature != CmuonFeaturesEnd ; ++CmuonFeature )
		{
		  double Level2_Eta	= CmuonFeature->eta();
		  double Level2_Phi	= CmuonFeature->phi();
		  double Level2_Pt	= fabs(CmuonFeature->pt());
		  
		  int Level2_Charge       = (int) CmuonFeature->pt()/fabs(CmuonFeature->pt());

			assoziatedObject.clear();
			assoziatedObject.setObject(Level2_Pt, Level2_Eta, Level2_Phi, 0.0, 0.0, Level2_Charge, -1);

			double dis = distance(assoziatedObject, object);
			if (dis < 0.1)
			{
				tag=true;
				assoziatedObject.metricDistance = dis;
				object.setAssoziatedTriggerInformation("Trigger_Level_2_CB", assoziatedObject);
			}
		}
	}
	
	// Retrieve EF Muons 
	const DataHandle<TrigMuonEFContainer> trigEcontIt; 
	const DataHandle<TrigMuonEFContainer> trigEcontEnd;

	sc = m_storeGate->retrieve(trigEcontIt,trigEcontEnd);
	if (sc != StatusCode::SUCCESS) {
	  std::cout << "Can't retrieve the TrigMuonEFContainer" << std::endl;
	}else{
	  for (;trigEcontIt != trigEcontEnd; ++trigEcontIt) {
	    string contName = trigEcontIt.key();
	    if( contName.find("HLTAutoKey") != string::npos) { 
	      TrigMuonEFContainer::const_iterator trigmuItr = trigEcontIt->begin();
	      TrigMuonEFContainer::const_iterator trigmuItrE = trigEcontIt->end();
	      
	      for(; trigmuItr != trigmuItrE; ++trigmuItr) {
		
		assoziatedObject.clear();
		assoziatedObject.setObject((*trigmuItr)->pt(), (*trigmuItr)->eta(), (*trigmuItr)->phi(), 0.0, 0.0, (*trigmuItr)->charge(), -1);
		
		double dis = distance(assoziatedObject, object);
		
		std::string ef_string_final = "Trigger_Level_3_";

		if((*trigmuItr)->MuonCode() == 121 || (*trigmuItr)->MuonCode() == 111 || (*trigmuItr)->MuonCode() == 120)
		  ef_string_final += "CB";
		
		else if((*trigmuItr)->MuonCode() == 1) 
		  ef_string_final += "MS";
		
		else if((*trigmuItr)->MuonCode() == 0) 
		  ef_string_final += "MSonly";	      
		
		else 
		  ef_string_final += "unknown";
		
		if ( (dis < 0.3 && strstr(ef_string_final.c_str(),"MS")) ||
		     (dis < 0.05 && strstr(ef_string_final.c_str(),"CB")) ) {
		  tag=true;
		  assoziatedObject.metricDistance = dis;
		  object.setAssoziatedTriggerInformation(ef_string_final, assoziatedObject);
		}
	      }
	    }
	  }
	}
	return tag;
}


bool MSTrackAssociator::assoziateTruthTrack(MSTrackObject &object)
{
	// bool	tag=false;
	MSPhysicsObject assoziatedObject;
	StatusCode sc = StatusCode::SUCCESS;

	// Setting True Particle Information
	const TruthParticleContainer*  mcpartTES = 0;
	sc=m_storeGate->retrieve( mcpartTES, "SpclMC");
	if( sc.isFailure()  ||  !mcpartTES )
	{
		std::cout << "No AOD MC truth particle container found in TDS"<< std::endl;
	}
	else
	{
		MSTrackObject Particle;

		for (TruthParticleContainer::const_iterator trueItr  = mcpartTES->begin(); trueItr != mcpartTES->end(); ++trueItr)
		{
			if ((*trueItr)->status()==1)
			{
				Particle.clear();
				if ((abs((*trueItr)->pdgId())==13) && ((*trueItr)->pt()>m_MinimalMuonPt))
				{
					assoziatedObject.clear();
					assoziatedObject.setObject(	(*trueItr)->pt(),
									(*trueItr)->eta(),
									(*trueItr)->phi(),
									0.0, 0.0,
									(int)(*trueItr)->charge(), (*trueItr)->pdgId());

					double dis = distance(assoziatedObject, object);
					if (dis < m_MaxConeDistance)
					{
						 // tag=true;
						assoziatedObject.metricDistance = dis;
						object.setAssoziatedTruthInformation(assoziatedObject);
					}
				}
			}
		}
	}

	return true;
}

bool MSTrackAssociator::assoziateJetEnergyInCone(MSTrackObject &object, double coneDistance_max)
{
	// Setting Jet Information
	bool tag = false;
	StatusCode sc = StatusCode::SUCCESS;
	MSPhysicsObject assoziatedJet;

	double dis_min = coneDistance_max;
	object.AnalysisParameters.dJetIsolation=0.0;

	//Get jet container
	//const ParticleJetContainer*  mcpartJet = 0; //AOD Key
	const JetCollection* mcpartJet;  //ESD Key
	sc=m_storeGate->retrieve( mcpartJet, "Kt6H1TowerJets");
	if( sc.isFailure()  ||  !mcpartJet )
	{
		std::cout << "No Jet-container found in TDS"<< std::endl;
	}
	else
	{
		//** Iterator on Reco Jets
		//ParticleJetContainer::const_iterator jetItr  = mcpartJet->begin();
		//ParticleJetContainer::const_iterator jetItrE = mcpartJet->end();
		JetCollection::const_iterator jetItr  = mcpartJet->begin();
		JetCollection::const_iterator jetItrE = mcpartJet->end();
		for (; jetItr != jetItrE; ++jetItr)
		{
			assoziatedJet.clear();
			assoziatedJet.setObject(	(*jetItr)->pt(),
							(*jetItr)->eta(),
							(*jetItr)->phi(),
							0.0, 0.0, 0,0);
			double dis_akt = distance(assoziatedJet, object);
			if (dis_akt < dis_min)
			{
				tag=true;
				dis_min=dis_akt;
				object.AnalysisParameters.dJetIsolation=(*jetItr)->pt();
			}
		}

	}
	return tag;
}


bool MSTrackAssociator::assoziateInnerTrackPropertiesInCone(MSTrackObject &object, double coneDistance_min, double coneDistance_max)
{
	bool		tag=false;
	MSPhysicsObject assoziatedInnerTrack;

	StatusCode sc = StatusCode::SUCCESS;
	const Rec::TrackParticleContainer* trackTDS=0;

	sc=m_storeGate->retrieve( trackTDS, "TrackParticleCandidate");

	double	dSumPt	= 0.0;
	int	nTracks	= 0;

	if( sc.isFailure()  ||  !trackTDS )
	{
		std::cout << "No TrackParticleCandidate-Container found in TDS"<< std::endl;
	}
	else
	{
		for (Rec::TrackParticleContainer::const_iterator trackItr= trackTDS->begin(); trackItr != trackTDS->end(); ++trackItr)
		{

				assoziatedInnerTrack.clear();
				assoziatedInnerTrack.setObject(	(*trackItr)->pt(),
								(*trackItr)->eta(),
								(*trackItr)->phi(),
								0.0, 0.0,
								(int)(*trackItr)->charge(), -1);
				double dis = distance(assoziatedInnerTrack, object);
				if (	(coneDistance_min < dis) &&
					(dis < coneDistance_max) &&
					((fabs(assoziatedInnerTrack.Pt-object.Pt)/object.Pt)>0.1)
					)
				{
					nTracks++;
					dSumPt+=(*trackItr)->pt();
					tag=true;
				}
		}
		object.AnalysisParameters.dTrackPtConeIsolation = dSumPt;
		object.AnalysisParameters.dNTrackConeIsolation	= nTracks;
	}

	return tag;
}

bool	MSTrackAssociator::assoziateElectronInCone(MSTrackObject &object, double coneDistance_max)
{
	StatusCode sc = StatusCode::SUCCESS;
	MSPhysicsObject assoziatedElectron;
	/// get the AOD muon container for TES
	const ElectronContainer* electronTES = 0;

	sc=m_storeGate->retrieve( electronTES, "ElectronCollection");
	if( sc.isFailure()  ||  !electronTES )
	{
		std::cout<< "No AOD electron container found in TDS"<< std::endl;
	} else
	{
		for (ElectronContainer::const_iterator electronItr= electronTES->begin();electronItr!=electronTES->end();++electronItr)
		{
			assoziatedElectron.clear();
			assoziatedElectron.setObject(	(*electronItr)->pt(),
							(*electronItr)->eta(),
							(*electronItr)->phi(),
							0.0, 0.0,
							(int)(*electronItr)->charge(), -1);
			if (distance(assoziatedElectron, object) < coneDistance_max)
			{
				object.AnalysisParameters.isElectronCandidate=1;
				return true;
			}
		}
	}
	return false;
}

double	MSTrackAssociator::distance(MSPhysicsObject &ob1, MSPhysicsObject &ob2)
{
	double dis;

	dis = sqrt(pow(ob1.Eta-ob2.Eta,2) + pow(ob1.Phi-ob2.Phi,2));

	return dis;
}

