/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonRecValidatorAOD
// Author:   M.Schott(LMU) - M.Bellomo (INFN)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class describes one Event which all information
//		which are necessary for the Muon-, DiMuon-, Zmumu- and
//		InSitu- Performance
///////////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "MuonValUtils/MSEvent.h"

using namespace std;

MSEvent::MSEvent()
{
	clear();
	m_sDefaultReconstruction="Muonboy";
}

// Copy constructor
MSEvent::MSEvent(const MSEvent& ob)
{
	m_sDefaultReconstruction= ob.m_sDefaultReconstruction;
	m_aktTrackCombination1	= ob.m_aktTrackCombination1;
	m_aktTrackCombination2	= ob.m_aktTrackCombination2;
	m_aktParticle		= ob.m_aktParticle;
	m_aktTrack		= ob.m_aktTrack;
	mapTracks		= ob.mapTracks;
	vecParticles		= ob.vecParticles;
	NumberOfRecoJets 	= ob.NumberOfRecoJets;
	mapMuonTriggerInfo	= ob.mapMuonTriggerInfo;
	m_sDefaultTrigger	= ob.m_sDefaultTrigger;
	m_aktTrigger		= ob.m_aktTrigger;
}

// Overload =
void MSEvent::operator = (const MSEvent& ob)
{
	m_sDefaultReconstruction= ob.m_sDefaultReconstruction;
	m_aktTrackCombination1	= ob.m_aktTrackCombination1;
	m_aktTrackCombination2	= ob.m_aktTrackCombination2;
	m_aktParticle		= ob.m_aktParticle;
	m_aktTrack		= ob.m_aktTrack;
	mapTracks		= ob.mapTracks;
	vecParticles		= ob.vecParticles;

	mapMuonTriggerInfo	= ob.mapMuonTriggerInfo;
	m_sDefaultTrigger	= ob.m_sDefaultTrigger;
	m_aktTrigger		= ob.m_aktTrigger;
}

void MSEvent::clear()
{
	m_sDefaultReconstruction= "Muonboy";

	m_aktTrackCombination1	= 0;
	m_aktTrackCombination2	= 1;
	m_aktParticle		= 0;
	m_aktTrack		= 0;
	mapTracks.clear();
	vecParticles.clear();
	mapMuonTriggerInfo.clear();
	m_sDefaultTrigger.clear();
	m_aktTrigger=0;

	return;
}


bool MSEvent::getNextTrackCombination	(MSTrackObject &Track1, MSTrackObject &Track2)
{
	if ((getNumberOfTracks()<2) || (m_aktTrackCombination1==getNumberOfTracks()-1)) return false;	

	Track1	= mapTracks[m_sDefaultReconstruction][m_aktTrackCombination1];
	Track2	= mapTracks[m_sDefaultReconstruction][m_aktTrackCombination2];

	if (m_aktTrackCombination2==getNumberOfTracks()-1)
	{
		m_aktTrackCombination1++;
		m_aktTrackCombination2=m_aktTrackCombination1;
	}
	m_aktTrackCombination2++;

	return true;
}


bool MSEvent::getNextParticleCombination	(MSTrackObject &Particle1, MSTrackObject &Particle2)
{
	if ((getNumberOfParticles()<2) || (m_aktParticleCombination1==getNumberOfParticles()-1)) return false;	

	Particle1	= vecParticles[m_aktParticleCombination1];
	Particle2	= vecParticles[m_aktParticleCombination2];

	if (m_aktParticleCombination2==getNumberOfParticles()-1)
	{
		m_aktParticleCombination1++;
		m_aktParticleCombination2=m_aktParticleCombination1;
	}
	m_aktParticleCombination2++;

	return true;
}


bool MSEvent::getNextParticle(MSTrackObject &Particle)
{
	Particle.clear();
	if (m_aktParticle>=(int)vecParticles.size()) return false;
	Particle=vecParticles[m_aktParticle];
	m_aktParticle++;
	return true;
}

bool MSEvent::getNextTrack(MSTrackObject &Track)
{
	Track.clear();
	if (m_aktTrack>=(int)mapTracks[m_sDefaultReconstruction].size()) return false;
	Track=mapTracks[m_sDefaultReconstruction][m_aktTrack];
	m_aktTrack++;
	return true;
}

bool	MSEvent::getNextTrigger(MSTrackObject &Trigger)
{
	Trigger.clear();
	if (m_aktTrigger>=(int)mapMuonTriggerInfo[m_sDefaultTrigger].size()) return false;
	Trigger=mapMuonTriggerInfo[m_sDefaultTrigger][m_aktTrigger];
	m_aktTrigger++;
	return true;
}



void MSEvent::convertTracks(std::string id, MSEventConverterObject &msConOb)
{
	convertToMSPhysicsObject(mapTracks[id], msConOb);
}

void MSEvent::convertParticles(MSEventConverterObject &msConOb)
{
	convertToMSPhysicsObject(vecParticles, msConOb);
}

void MSEvent::convertTriggerInfo(std::string id, MSEventConverterObject &msConOb)
{
	convertToMSPhysicsObject(mapMuonTriggerInfo[id], msConOb);
}


void MSEvent::convertToMSPhysicsObject(vector<MSTrackObject> &vecOb, MSEventConverterObject &msConOb)
{
	clearMSEventConverterObject(msConOb);
	for (int i=0; i<(int)vecOb.size(); i++)
	{
		msConOb.m_pdgID		->push_back(vecOb[i].PdgID);
		msConOb.m_mother	->push_back(vecOb[i].Mother);
		msConOb.m_fPt		->push_back(vecOb[i].Pt);
		msConOb.m_fEta		->push_back(vecOb[i].Eta);
		msConOb.m_fPhi		->push_back(vecOb[i].Phi);
		msConOb.m_fR0		->push_back(vecOb[i].R0);
		msConOb.m_fZ0		->push_back(vecOb[i].Z0);
		msConOb.m_charge	->push_back(vecOb[i].Charge);

		msConOb.m_fDeltaPt	->push_back(vecOb[i].Delta_Pt);
		msConOb.m_fDeltaEta	->push_back(vecOb[i].Delta_Eta);
		msConOb.m_fDeltaPhi	->push_back(vecOb[i].Delta_Phi);

		msConOb.m_type		->push_back(vecOb[i].Type);
		msConOb.m_chi2		->push_back(vecOb[i].Chi2);
		msConOb.m_chi2oDOF	->push_back(vecOb[i].Chi2oDOF);
		msConOb.m_numberOfHits	->push_back(vecOb[i].Hits);
		msConOb.m_energyLoss	->push_back(vecOb[i].EnergyLoss);

		msConOb.m_triggerStatus	->push_back(vecOb[i].AnalysisParameters.nTriggerStatus);
		msConOb.m_isElectron	->push_back(vecOb[i].AnalysisParameters.isElectronCandidate);
		msConOb.m_isZmuon	->push_back(vecOb[i].AnalysisParameters.isZMuonCandidate);
		msConOb.m_trackPt_iso	->push_back(vecOb[i].AnalysisParameters.dTrackPtConeIsolation);
		msConOb.m_ntrack_iso	->push_back(vecOb[i].AnalysisParameters.dNTrackConeIsolation);
		msConOb.m_jet_iso	->push_back(vecOb[i].AnalysisParameters.dJetIsolation);
		msConOb.m_et_iso	->push_back(vecOb[i].AnalysisParameters.dETIsolation);
		msConOb.m_mass		->push_back(vecOb[i].AnalysisParameters.dMass);
	}
}

void MSEvent::setTracks(std::string id, MSEventConverterObject &msConOb)
{
	vector<MSTrackObject> vecOb;
	mapTracks[id]=vecOb;
	convertFromMSPhysicsObject(mapTracks[id], msConOb);
}

void MSEvent::setParticles(MSEventConverterObject &msConOb)
{
	convertFromMSPhysicsObject(vecParticles, msConOb);
}

void MSEvent::setTriggerInfo(std::string id,  MSEventConverterObject &msConOb)
{
	convertFromMSPhysicsObject(mapMuonTriggerInfo[id], msConOb);
}

void	MSEvent::convertFromMSPhysicsObject(vector<MSTrackObject> &vecOb, MSEventConverterObject &msConOb)
{
	vecOb.clear();
	MSTrackObject	tmp;
	for (int i=0; i<(int)msConOb.m_fPt->size(); i++)
	{
		tmp.clear();

		tmp.PdgID	= ((*msConOb.m_pdgID)[i]);
		tmp.Mother	= ((*msConOb.m_mother)[i]);
		tmp.Pt		= ((*msConOb.m_fPt)[i]);
		tmp.Eta		= ((*msConOb.m_fEta)[i]);
		tmp.Phi		= ((*msConOb.m_fPhi)[i]);
		tmp.R0		= ((*msConOb.m_fR0)[i]);
		tmp.Z0		= ((*msConOb.m_fZ0)[i]);
		tmp.Charge	= ((*msConOb.m_charge)[i]);

		tmp.Delta_Pt	= ((*msConOb.m_fDeltaPt)[i]);
		tmp.Delta_Eta	= ((*msConOb.m_fDeltaEta)[i]);
		tmp.Delta_Phi	= ((*msConOb.m_fDeltaPhi)[i]);

		tmp.Type	= ((*msConOb.m_type)[i]);
		tmp.Chi2	= ((*msConOb.m_chi2)[i]);
		tmp.Chi2oDOF	= ((*msConOb.m_chi2oDOF)[i]);
		tmp.Hits	= ((*msConOb.m_numberOfHits)[i]);
		tmp.EnergyLoss	= ((*msConOb.m_energyLoss)[i]);

		tmp.AnalysisParameters.nTriggerStatus		= ((*msConOb.m_triggerStatus)[i]);
		tmp.AnalysisParameters.isElectronCandidate	= ((*msConOb.m_isElectron)[i]);
		tmp.AnalysisParameters.isZMuonCandidate		= ((*msConOb.m_isZmuon)[i]);
		tmp.AnalysisParameters.dTrackPtConeIsolation	= ((*msConOb.m_trackPt_iso)[i]);
		tmp.AnalysisParameters.dNTrackConeIsolation	= ((*msConOb.m_ntrack_iso)[i]);
		tmp.AnalysisParameters.dJetIsolation		= ((*msConOb.m_jet_iso)[i]);
		tmp.AnalysisParameters.dETIsolation		= ((*msConOb.m_et_iso)[i]);
		tmp.AnalysisParameters.dMass			= ((*msConOb.m_mass)[i]);

		vecOb.push_back(tmp);
	}
}


void MSEvent::setupMSEventConverterObject(MSEventConverterObject &msConOb)
{
	msConOb.m_pdgID		= new vector<long>;
	msConOb.m_mother	= new vector<long>;
	msConOb.m_fPt		= new vector<double>;
	msConOb.m_fEta		= new vector<double>;
	msConOb.m_fPhi		= new vector<double>;
	msConOb.m_fR0		= new vector<double>;
	msConOb.m_fZ0		= new vector<double>;
	msConOb.m_charge	= new vector<int>;

	msConOb.m_fDeltaPt	= new vector<double>;
	msConOb.m_fDeltaEta	= new vector<double>;
	msConOb.m_fDeltaPhi	= new vector<double>;

	msConOb.m_type		= new vector<int>;
	msConOb.m_numberOfHits	= new vector<int>;
	msConOb.m_chi2		= new vector<double>;
	msConOb.m_chi2oDOF	= new vector<double>;
	msConOb.m_energyLoss	= new vector<double>;

	msConOb.m_triggerStatus	= new vector<int>;
	msConOb.m_isElectron	= new vector<int>;
	msConOb.m_isZmuon	= new vector<int>;
	msConOb.m_trackPt_iso	= new vector<double>;
	msConOb.m_ntrack_iso	= new vector<int>;
	msConOb.m_jet_iso	= new vector<double>;
	msConOb.m_et_iso	= new vector<double>;
	msConOb.m_mass		= new vector<double>;

}


void MSEvent::clearMSEventConverterObject(MSEventConverterObject &msConOb)
{
	msConOb.m_pdgID->clear();
	msConOb.m_mother->clear();
	msConOb.m_fPt->clear();
	msConOb.m_fEta->clear();
	msConOb.m_fPhi->clear();
	msConOb.m_fR0->clear();
	msConOb.m_fZ0->clear();
	msConOb.m_charge->clear();

	msConOb.m_fDeltaPt->clear();
	msConOb.m_fDeltaEta->clear();
	msConOb.m_fDeltaPhi->clear();

	msConOb.m_type->clear();
	msConOb.m_numberOfHits->clear();
	msConOb.m_chi2->clear();
	msConOb.m_chi2oDOF->clear();
	msConOb.m_energyLoss->clear();

	msConOb.m_triggerStatus->clear();
	msConOb.m_isElectron->clear();
	msConOb.m_isZmuon->clear();
	msConOb.m_trackPt_iso->clear();
	msConOb.m_ntrack_iso->clear();
	msConOb.m_jet_iso->clear();
	msConOb.m_et_iso->clear();
	msConOb.m_mass->clear();
}

bool MSEvent::isTriggered(unsigned int level, std::string item)
{
  if(!m_trigDec) {
    std::cout << "FATAL - Trigger Decision not found in event" << std::endl;
    return false;
  }
  if( m_trigDec->isDefined(item,level) ) {
    if( m_trigDec->isTriggered(item,level) ) {   
      return true;
    }
  }
  return false;
}
