/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_ZEETPINEFF
#define NAV_ZEETPINEFF

#include "TrigEgammaValidation/NavZeeTPBase.h"

// Framework includes
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TrigNavigation/Navigation.h"

#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"

#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"

#include <string>
#include <map>

class NavZeeTPIneff : public NavZeeTPBase
{
 public:	
	NavZeeTPIneff(const std::string & name, ISvcLocator * pSvcLocator) :
		NavZeeTPBase(name,pSvcLocator) { 
		declareProperty("ElectronOnlPPSelector", m_electronOnlPPCutIDTool);
		declareProperty("IneffLabels",m_labels);
		std::cout << "Building up" << std::endl;
	};
	~NavZeeTPIneff() { };
	/*
	 *Processes "execute" for each chain.
	 *
	 *Returns:
	 * - If the storeGate is not retrieved properly, a warning is gonna be displayed, but SUCCESS will be returned.
	 * - returns from inner functions
	 */
	StatusCode chainExecute() { return StatusCode::SUCCESS; };
	StatusCode childInitialize ();
	StatusCode childExecute();
	StatusCode childFinalize();

 private:
	unsigned int m_eventCounter;
	unsigned int m_probesTried;
	// Tools needed for isEM rebuilding
	ToolHandleArray<IAsgElectronIsEMSelector> m_electronOnlPPCutIDTool;
	std::vector< std::string > m_labels;

};

#endif
