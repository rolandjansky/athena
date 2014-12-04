/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_ZEETPBASE
#define NAV_ZEETPBASE

#include "TrigEgammaValidation/IValidationAlgo.h"
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

namespace Trig{
	class FeatureContainer;
}

class NavZeeTPBase : public IValidationAlgo
{
 public:	
	NavZeeTPBase(const std::string & name, ISvcLocator * pSvcLocator); 
	~NavZeeTPBase() {};
	/*
	 *Processes "execute" for each chain.
	 *
	 *Returns:
	 * - If the storeGate is not retrieved properly, a warning is gonna be displayed, but SUCCESS will be returned.
	 * - returns from inner functions
	 */
	 StatusCode childInitialize();
	 template<class T> const T* getFeature(const HLT::TriggerElement* te);
	 template<class T> bool ancestorPassed(const HLT::TriggerElement* te);
protected :
	bool EventWiseSelection(); // things like LAr-error, Tile-error, etc, should come here
	bool MinimalTriggerRequirement (); // at least one chain should pass. e28_tight_iloose?
	StatusCode executeTandP(const float ZMassMin, const float ZMassMax,
		const unsigned int isEMoffTag,
		const unsigned int isEMoffProbe,
		const unsigned int offTagTighteness,
		const unsigned int offProbeTighteness,
		const bool oppositeCharge,
		const float tagMinEt, const std::string trigItem);
	bool isTagElectron(const xAOD::Electron *el,
		const unsigned int TagTightness,
        	const unsigned int isEM, const float tagMinEt,
		const std::string trigItem);
	bool isProbeElectron(const xAOD::Electron *el,
		const std::string trigItem,
		const HLT::TriggerElement*& finalFC);
	bool passedTrigger(const HLT::TriggerElement* obj);
	void clearProbeList();
	std::vector<std::pair<const xAOD::Electron*,const HLT::TriggerElement*> > probeElectrons;
        ToolHandle<Trig::TrigDecisionTool> m_trigdec;
 private:
	// ToolHandles
        
        //Offline ++ selectors
	// In python order will matter. Should always be tight, medium, loose
        ToolHandleArray<IAsgElectronIsEMSelector> m_electronPPCutIDTool; 

	// Event Wise offline ElectronContainer Access and end iterator
	const xAOD::ElectronContainer* offElectrons;
	std::vector< std::string > m_minTrig;
        
};

template<class T>
const T*
NavZeeTPBase::getFeature(const HLT::TriggerElement* te){
        if ( te == NULL ) return NULL;
        if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
                return NULL;
        return ( (m_trigdec->ancestor<T>(te)).cptr() );
}

template<class T>
bool
NavZeeTPBase::ancestorPassed(const HLT::TriggerElement* te){
        if ( te == NULL ) return NULL;
        if ( (m_trigdec->ancestor<T>(te)).te() == NULL )
                return false;
        return ( (m_trigdec->ancestor<T>(te)).te()->getActiveState());
}

#endif
