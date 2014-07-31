/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//      Author: Kamal Benslama (University of Regina)
//      Update: E. ortega (Univesity of Regina)
//      Update: Y. Ming (UNiversity of Regina)
//	egamma physics offline monitoring Class
//      Jan 06, 2009 this class was created by A.Kalinowski by
//      splitting previous egammaMonTool class and adding new functionalities
//		University of Regina
//
//	date: Wed Jan  7 08:58:58 CET 2009
//	
//
/////////////////////////////////////////////////////////////
#ifndef ephysicsMonTool_H
#define ephysicsMonTool_H

#include <vector>
#include <string>

#include "GaudiKernel/StatusCode.h"
//#include "TrigDecision/TrigDecisionTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "egammaPerformance/IEgammaMonTool.h"

class CaloCluster;
class egamma;
class StoreGateSvc;

class ephysicsMonTool : public IEgammaMonTool {
public:

	ephysicsMonTool( const std::string & type, const std::string & name,
	                 const IInterface* parent ); 

	virtual ~ephysicsMonTool();

	virtual StatusCode init(){ return StatusCode::SUCCESS;};

	virtual StatusCode book( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun);

	virtual StatusCode fill();

	virtual StatusCode proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun);
     
private:

        ///Method for checking Wenu,by leading cluster and met. 
        int isWenu(const CaloCluster* , double) const;

        double signedDeltaPhi(double phi1, double phi2) const;

        //Met Container names for physicss and physicss
        std::string             m_metName;


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
	StatusCode bookPhysicsHistos(const std::string &groupName, const std::string &subgroupName);
              
	///Method for filling the histograms for single egamma object
	StatusCode fillHistogramsForItem(const std::string &itemName, const std::string &selectionName);

    
        //egammaContainer names for physics and ephysics
        std::string		m_ElectronCnt;

        //jetContainer for ephysics
        std::string             m_JetContainer;

        //deltaRcut for ephysics when identify jet and real electrons
        double m_deltaRcut;

	///Name of the physics process to be monitored (e.g. Zee, JPsiee)
	std::string m_processName;
	
	///Mass shift
	float  m_massShift;

        //Energy cut for Jets
        float m_JetEnergyCut; 
  
	///Et cut for di-electron cadidates
	float m_ElectronClusterEtCut;

	///Upper cut on the di-electrom mass
	float m_massLowerCut;

	///Lower cut on the di-electrom mass
	float m_massUpperCut;

        ///et distribution range
        float m_EtRange;

	// Use trigger?
	bool m_UseTrigger;

	// Trigger items
        std::vector<std::string> m_triggers;

	/// List of seletions
	std::vector<std::string> m_selections;

        /// List of triggers and selections
        std::vector<std::string> m_triggerXselection;

	// Handle for the trig decision tool
	//ToolHandle<TrigDec::TrigDecisionTool> m_trigDec;
        ToolHandle<Trig::TrigDecisionTool> m_trigDec;

        //Pt cut for leading electron Clusters when reconstructing Wenu
        float m_LeadingElectronClusterPtCut;

	//Et cut for the misssing particles when reconstruction Wenu
        float m_MissingParticleEtCut;

};

#endif
