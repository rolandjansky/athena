/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//      Author: Kamal Benslama (University of Regina)
//      Update: E. ortega (Univesity of Regina)
//      Update: Y. MIng (UNiversity of Regina)
//	egamma physics offline monitoring Class
//      Jan 06, 2009 this class was created by A.Kalinowski by
//      splitting previous egammaMonTool class and adding new functionalities
//		University of Regina
//
//	date: Wed Jan  7 08:58:58 CET 2009
//	
//
/////////////////////////////////////////////////////////////
#ifndef physicsMonTool_H
#define physicsMonTool_H

#include <vector>
#include <string>

#include "GaudiKernel/StatusCode.h"
//#include "TrigDecision/TrigDecisionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "egammaPerformance/IEgammaMonTool.h"

class CaloCluster;
class egamma;
class StoreGateSvc;

class physicsMonTool : public IEgammaMonTool {
public:

	physicsMonTool( const std::string & type, const std::string & name,
	                 const IInterface* parent ); 

	virtual ~physicsMonTool();

	virtual StatusCode init(){ return StatusCode::SUCCESS;};

	virtual StatusCode book( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun);

	virtual StatusCode fill();

	virtual StatusCode proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun);
     
private:

	///Method for checking if cluster is within barrel
	bool inBarrel(const CaloCluster * cluster);

	///Method for checking if cluster is within barrel
	bool inCrack(const CaloCluster * cluster);

	///Method for checking if cluster is within barrel
	bool inEndcap(const CaloCluster * cluster);

	///Method for checking if the egamma objest passed identification selections
        bool passesIsEMCut(const unsigned int, const egamma *);

	///Method wrapping division of two histogramms
	void divide(TH1 *num, TH1 *den, TH1 *quo);

	///Method for booking histograms for given trigger item
	StatusCode bookPhysicsHistos(const std::string&);
              
	///Method for filling the histograms for single egamma object
	StatusCode fillHistogramsForItem(const std::string &itemName);

	//Method to estimate the luminosity by the visible cross-section and number of events
	double estLumi();
    
        //egammaContainer names for physicss and physicss
        std::string		m_ElectronCnt;
	
	///Name of the physics process to be monitored (e.g. Zee, JPsiee)
	std::string m_processName;
	
	///Mass shift
	float  m_massShift;

	///Et cut for di-electron cadidates
	float m_ElectronClusterEtCut;

	///Upper cut on the di-electrom mass
	float m_massLowerCut;

	///Lower cut on the di-electrom mass
	float m_massUpperCut;

        ///et distribution range
        float m_EtRange;

	// create and Fill Luminosity histograms
	bool m_doLumi;

	// Trigger is used?
	bool m_UseTrigger;
	// Trigger items
        std::vector<std::string> m_triggers;

	/// List of seletions
	std::vector<std::string> m_selections;

	// Handle for the trig decision tool
	//ToolHandle<TrigDec::TrigDecisionTool> m_trigDec;
        ToolHandle<Trig::TrigDecisionTool> m_trigDec;

	//Visible cross section
	double m_visxs;

	//integrate lumi for multiple blocks
	double m_intEstLumi;

	//vector of luminosities per lumiblock
	std::vector<double> lumivec;

};

#endif
