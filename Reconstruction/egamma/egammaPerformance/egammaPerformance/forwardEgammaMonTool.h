/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef forwardEgammaMonTool_H
#define forwardEgammaMonTool_H

#include <vector>
#include <string>

#include "GaudiKernel/StatusCode.h"
//#include "TrigDecision/TrigDecisionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "egammaPerformance/IEgammaMonTool.h"

class egamma;
class CaloCluster;

class forwardEgammaMonTool : public IEgammaMonTool {
public:

	forwardEgammaMonTool( const std::string & type, const std::string & name,
	                 const IInterface* parent );

	virtual ~forwardEgammaMonTool();

	virtual StatusCode book( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );

	virtual StatusCode fill();

	virtual StatusCode proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );

	virtual StatusCode init(){ return StatusCode::SUCCESS;};

private:

	//are we in EMEC inner Wheel
	bool inEMEC(const CaloCluster * cluster);

	//are we in FCal
	bool inFCal(const CaloCluster * cluster);

	///Method for checking if the egamma objest passed identification selections
	bool passesIsEMCut(const unsigned int, const egamma *);

	//booking histograms for given trigger item
	StatusCode bookFwdEgHistos(const std::string&);

	// filling egamma hist.
	void fillFwdEgHistos(const egamma *eg);

	//
	std::string   m_electronContainerName;

        // Are we using trigger?
        bool m_UseTrigger;

	// Trigger items
        std::vector<std::string> m_e_triggers;

	/// List of seletions
	std::vector<std::string> m_e_selections;

	// Handle for the trig decision tool
	//ToolHandle<TrigDec::TrigDecisionTool> m_trigDecTool;
        ToolHandle<Trig::TrigDecisionTool> m_trigDec;

};

#endif
