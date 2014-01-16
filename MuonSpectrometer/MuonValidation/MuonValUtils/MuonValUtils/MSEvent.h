/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Authors:   M.Schott(LMU) - N. Benekos(MPI)
// MuonValidation-Team
// Feb. 2007
//
// DESCRIPTION:
// Subject: 	This class describes one Event which all information
//		which are necessary for the Muon-, DiMuon-, Zmumu- and
//		InSitu- Performance
///////////////////////////////////////////////////////////////////////////

#ifndef MSEVENT_H
#define MSEVENT_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>

#include "MuonValUtils/MSPhysicsObject.h"
#include "MuonValUtils/MSTrackObject.h"
#include "TrigSteeringEvent/TriggerDecision.h"

using namespace std;

// Ugly method to use TMinuit

#define INFO_RECO		1
#define INFO_TRUE		2


// This structure is only usefull for dumping the Information of one MSTrackObject
// to an AANtuple-tree. It is not important for the analysis itself
struct MSEventConverterObject
{
	vector<long>	*m_pdgID;
	vector<long>	*m_mother;
	vector<double>	*m_fPt;
	vector<double>	*m_fEta;
	vector<double>	*m_fPhi;
	vector<double>	*m_fR0;
	vector<double>	*m_fZ0;
	vector<int>	*m_charge;

	vector<double>	*m_fDeltaPt;
	vector<double>	*m_fDeltaEta;
	vector<double>	*m_fDeltaPhi;


	vector<int>	*m_type;
	vector<int>	*m_numberOfHits;
	vector<double>	*m_chi2;
	vector<double>	*m_chi2oDOF;
	vector<double>	*m_energyLoss;

	vector<int>	*m_triggerStatus;
	vector<int>	*m_isElectron;
	vector<int>	*m_isZmuon;
	vector<double>	*m_trackPt_iso;
	vector<int>	*m_ntrack_iso;
	vector<double>	*m_jet_iso;
	vector<double>	*m_et_iso;
	vector<double>	*m_mass;
};



class MSEvent
{
	public :
		// Default Constructor - Empty
		MSEvent();

		// Copy constructor
		MSEvent(const MSEvent& ob);

		// Overload =
		void operator = (const MSEvent& ob);

		void clear();

		// Which MuonReconstruction should be used as standard?
		void	setDefaultTrackReconstruction(string s)	{	m_sDefaultReconstruction = s;	}
		string	getDefaultTrackReconstruction()		{	return m_sDefaultReconstruction;	}

		// Which Trigger should be used as standard?
		void	setDefaultTrigger(string s)		{	m_sDefaultTrigger = s;}
		string	getDefaultTrigger()			{	return m_sDefaultTrigger;	}

		// Adding a Track to the event. The Track will be stored in the collection of the current default MuonReconstruction
		void	addTrack(MSTrackObject &Track)		{	mapTracks[m_sDefaultReconstruction].push_back(Track);	}
		// Adding TriggerInformation to the event. The Track will be stored in the collection of the current default TriggerName
		void	addTriggerInfo(MSTrackObject &Trigger)	{	mapMuonTriggerInfo[m_sDefaultTrigger].push_back(Trigger);	}
		void	addParticle(MSTrackObject &Particle)	{	vecParticles.push_back(Particle);}

		// Setting up the Track/Particle Combinations. This must be done before calling getNextTrackCombination or getNextParticleCombination
		void	setupTrackCombination()			{	m_aktTrackCombination1=0;
									m_aktTrackCombination2=1;	}
		void	setupParticleCombination()		{	m_aktParticleCombination1=0;
									m_aktParticleCombination2=1;	}

		// Setting up the Track/Particle/Trigger Counters. This must be done before calling getNextParticle,...
		void	setupParticleCounter()			{	m_aktParticle=0;}
		void	setupTriggerCounter()			{	m_aktTrigger=0;	}
		void	setupTrackCounter()			{	m_aktTrack=0;	}

		// The following functions are for retrieving stored Tracks, Particles, ... in any possible combination
		bool	getNextTrackCombination		(MSTrackObject &Track1, MSTrackObject &Track2);
		bool	getNextParticleCombination	(MSTrackObject &Particle1, MSTrackObject &Particle2);
		// The following functions are for retrieving stored Tracks, Particles, ... one after the other
		bool	getNextParticle			(MSTrackObject &Particle);
		bool	getNextTrack			(MSTrackObject &Track);
		bool	getNextTrigger			(MSTrackObject &Trigger);

		int	getNumberOfTracks()			{	return mapTracks[m_sDefaultReconstruction].size();}
		int	getNumberOfParticles()			{	return vecParticles.size();	}
		int	getNumberOfTriggerInfos()		{	return mapMuonTriggerInfo[m_sDefaultTrigger].size();	}

		// The following functions are just for exchanging the Information from the MSEvent-Description to an AAN and via versa
		void	convertTracks(std::string id, MSEventConverterObject &msConOb);
		void	convertTriggerInfo(std::string id, MSEventConverterObject &msConOb);
		void	convertParticles(MSEventConverterObject &msConOb);

		void	setTracks(std::string id, MSEventConverterObject &msConOb);
		void	setTriggerInfo(std::string id, MSEventConverterObject &msConOb);
		void	setParticles(MSEventConverterObject &msConOb);

		void	clearMSEventConverterObject(MSEventConverterObject &msConOb);
		void	setupMSEventConverterObject(MSEventConverterObject &msConOb);

		const   TriggerDecision* getTriggerDecision() {return m_trigDec;}
		void    setTriggerDecision(const TriggerDecision *trigDec) {m_trigDec = trigDec;}

		bool    isTriggered(unsigned int,std::string);
	private:

		void	convertToMSPhysicsObject(vector<MSTrackObject> &vecOb, MSEventConverterObject &msConOb);
		void	convertFromMSPhysicsObject(vector<MSTrackObject> &vecOb, MSEventConverterObject &msConOb);

		string	m_sDefaultReconstruction;
		int	m_aktTrackCombination1;
		int	m_aktTrackCombination2;
		int	m_aktParticleCombination1;
		int	m_aktParticleCombination2;
		int	m_aktTrack;

		string	m_sDefaultTrigger;
		int	m_aktTrigger;

		int	m_aktParticle;

		map<string, vector<MSTrackObject> >	mapTracks;
		map<string, vector<MSTrackObject> >	mapMuonTriggerInfo;
		vector<MSTrackObject>			vecParticles;
		int					NumberOfRecoJets;

		const TriggerDecision* m_trigDec;

};

#endif
