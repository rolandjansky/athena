/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//      Author: Kamal Benslama (University of Regina)
//      Updated by
//        E. Ortega (University of Regina)
//        O. Arnaez (CNRS/IN2P3)
//
//      electron offline monitorin tool
//      Jan 06, 2009 - Class created by Artur Kalinowski (University of Regina):
//      this class was created from splitting previous egammaMonTool class
//
//              University of Regina
//
//      date: Tue Jan  6 15:37:37 CET 2009
//
//
/////////////////////////////////////////////////////////////
#ifndef softEMonTool_H
#define softEMonTool_H

#include <vector>
#include <string>

#include "GaudiKernel/StatusCode.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "egammaPerformance/IEgammaMonTool.h"

class egamma;
class CaloCluster;


class softEMonTool : public IEgammaMonTool {
public:

	softEMonTool( const std::string & type, const std::string & name,
	                 const IInterface* parent ); 

	virtual ~softEMonTool();

	virtual StatusCode book( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );

	virtual StatusCode fill();

	virtual StatusCode proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun );

	virtual StatusCode init(){ return StatusCode::SUCCESS;};
     
private:

	///Method for checking if cluster is within barrel
	bool inBarrel(const CaloCluster * cluster);

	///Method for checking if cluster is within barrel
	bool inCrack(const CaloCluster * cluster);

	///Method for checking if cluster is within barrel
	bool inEndcap(const CaloCluster * cluster);

	///Method for checking if the egamma objest passed identification selections
        bool passesIsEMCut(const unsigned int, const egamma *);

	///Method for booking histograms for given trigger item
        StatusCode bookElectronHistosDec(const std::string& groupName, const std::string & decName);
                  
	///Method for filling the histograms for single egamma object
        void fillElectronHistos(const egamma *eg, const std::string&);
         std::vector<std::string> m_e_decdivs;

        //egammaContainer names for electrons and electrons
        std::string		m_ElectronCnt;

	//egDetail container names
	std::string		m_EMTrackMatchCnt;
	std::string		m_EMShowerCnt;
	std::string		m_EMConvertCnt;

	// Are we using trigger?
	bool m_UseTrigger;

	// Trigger items
        std::vector<std::string> m_e_triggers;

	/// List of seletions
	std::vector<std::string> m_e_selections;

	//Base for the name of the group of monitoring plots ("egamma/" or "egamma/softe/")
	std::string             m_basegroupName;

	// Handle for the trig decision tool
	ToolHandle<Trig::TrigDecisionTool> m_trigDec;


};

#endif
