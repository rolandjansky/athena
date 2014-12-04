/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_ZEETPCOUNTS
#define NAV_ZEETPCOUNTS

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
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigNavigation/Navigation.h"

#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTrigCalo/TrigEMCluster.h"

#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"

#include <string>
#include <map>

class NavZeeTPCounts : public NavZeeTPBase
{
 public:	
	NavZeeTPCounts(const std::string & name, ISvcLocator * pSvcLocator) :
		NavZeeTPBase(name,pSvcLocator) { std::cout << "Building up" << std::endl;};
	~NavZeeTPCounts() { };
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
	unsigned int m_nProbes[3];
	unsigned int m_nProbesPassed[3];

};

#endif
