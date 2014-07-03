/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthStrategyManager_H
#define TruthStrategyManager_H

#include <string>
#include <map>
#include <vector>

#include "HepMC/GenEvent.h"
#include "GaudiKernel/MsgStream.h"

class TruthStrategy;
class G4Step;
class G4Track;
class G4StepPoint;
class EventInformation;
class MCTruthSteppingAction;

typedef std::map<std::string,TruthStrategy*, std::less<std::string> > strategyMap;

class TruthStrategyManager {
public:
	static TruthStrategyManager* GetStrategyManager();
	bool IsApplicable() {return !isEmpty;}
	bool AnalyzeVertex(const G4Step*);
	void RegisterStrategy(TruthStrategy *);
	TruthStrategy * GetStrategy(std::string);
	void ListStrategies() const;
	void SetNrOfSecondaries(int i) {nSecondaries=i;}
	int GetNrOfSecondaries() {return nSecondaries;}
	void SetSecondarySavingLevel(int i) {secondarySavingLevel=i;}
	int GetSecondarySavingLevel() const {return secondarySavingLevel;}

	void EnableRecordingEnvelopes();
	void AddRecordingEnvelope(std::string,int,std::string sdName="");
	void ListRecordingEnvelopes();
	
	void SetTruthParameter(const std::string, double);
	double GetTruthParameter(const std::string);
	void PrintParameterList() const;
	
	EventInformation* GetEventInformation() const;
	std::vector<G4Track*> GetSecondaries();

	// here a couple of helper functions
	HepMC::GenVertex* StepPoint2Vertex(G4StepPoint*) const;
	HepMC::GenParticle* Track2Particle(G4Track*) const;
	
	void SaveSecondaryVertex(G4Track*, G4StepPoint*, std::vector<G4Track*>) const;

private:
	TruthStrategyManager(); 
	TruthStrategyManager(const TruthStrategyManager&) : m_log(0) {}
	~TruthStrategyManager() { if (m_log) delete m_log; }
	static TruthStrategyManager *thePointer;

	strategyMap theStrategies;
	bool isEmpty;
	
	int secondarySavingLevel;
	
	// cache the nr. of secondaries produced in an interaction here, on
	// request, to avoid too many tours in the G4 meanders...
	
	int nSecondaries;
	std::vector<G4Track*> theSecondaries;

	// here go the parameters needed by all various strategies

	std::map< std::string, double, std::less<std::string> > truthParams;
	
	MCTruthSteppingAction *theSteppingAction;

    MsgStream * m_log;
    MsgStream log();
	
};

#endif
