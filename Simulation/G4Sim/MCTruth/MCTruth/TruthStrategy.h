/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthStrategy_H
#define TruthStrategy_H

#include <string>
#include <map>
#include "MCTruth/TruthStrategyManager.h"
#include "GaudiKernel/MsgStream.h" // So that all who inherit have full access
class G4Step;
class TruthStrategy {
public:
	TruthStrategy(const std::string);
	virtual ~TruthStrategy() { if (m_log) delete m_log; }
	std::string StrategyName() {return strategyName;}
	//std::string VolumeName() {return volumeName;}
	//int VolumeLevel() {return volumeLevel;}
	virtual bool IsApplicable(const G4Step*);
	virtual bool AnalyzeVertex(const G4Step*) = 0;
	bool Activate(const std::string, int); 
	void Disactivate() {MCActiveArea.clear();activated=false;}
	bool IsActivated() {return activated;}
protected:
	TruthStrategyManager *theTruthManager;
    MsgStream * m_log;
    MsgStream log();
private:
	std::string strategyName;
	std::map<std::string,int> MCActiveArea;//key=volumeName, val=volumeLevel
	typedef std::map<std::string,int>::value_type valType;
	bool activated;
};

#endif
