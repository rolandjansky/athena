/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_ZEETPXAOD_VALIDATION
#define NAV_ZEETPXAOD_VALIDATION

#include "TrigEgammaValidation/NavAuxValidation.h"

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

class NavZeeTPxAODValidation : public NavAuxValidation 
{
 public:	
	NavZeeTPxAODValidation(const std::string & name, ISvcLocator * pSvcLocator);
	~NavZeeTPxAODValidation();
	/*
	 *Processes "execute" for each chain.
	 *
	 *Returns:
	 * - If the storeGate is not retrieved properly, a warning is gonna be displayed, but SUCCESS will be returned.
	 * - returns from inner functions
	 */
	StatusCode chainExecute();
	StatusCode childInitialize ();
	StatusCode childExecute();
	StatusCode childFinalize();

 private:
	// ToolHandles
        
        ToolHandle<Trig::TrigDecisionTool> m_trigdec;
        //Offline ++ selectors
        ToolHandle<IAsgElectronIsEMSelector> m_electronTightPPCutIDTool; 
        ToolHandle<IAsgElectronIsEMSelector> m_electronMediumPPCutIDTool;  
        ToolHandle<IAsgElectronIsEMSelector> m_electronLoosePPCutIDTool;  
        
        // Trigger run1 selectors
        // ToolHandle<IAsgElectronIsEMSelector> m_electronTight1CutIDTool; 
        // ToolHandle<IAsgElectronIsEMSelector> m_electronMedium1CutIDTool;  
        // ToolHandle<IAsgElectronIsEMSelector> m_electronLoose1CutIDTool;  

        // Trigger DC14 tunes
        // ToolHandle<IAsgElectronIsEMSelector> m_electronTightCutIDTool; 
        // ToolHandle<IAsgElectronIsEMSelector> m_electronMediumCutIDTool;  
        // ToolHandle<IAsgElectronIsEMSelector> m_electronLooseCutIDTool;  
        
        // Navigation
        const HLT::NavigationCore *m_nav;
	TrigConf::HLTFrame *fHLTFrame;

        // Feature Containers
        Trig::FeatureContainer m_HLTFeatures; 
        // Services 
        ITHistSvc *m_thistSvc;
        // Properties
	std::vector <std::string> vec_chain;

	std::string m_chain;
	bool m_checkElectrons, m_checkPhotons;
	unsigned int m_isEMloose;	
	unsigned int m_isEMloose1;	
	unsigned int m_isEMloosePP;	
	
        unsigned int m_isEMmedium;	
	unsigned int m_isEMmedium1;	
	unsigned int m_isEMmediumPP;	
	
        unsigned int m_isEMtight;	
	unsigned int m_isEMtight1;	
	unsigned int m_isEMtightPP;	
        //Non const methods

        StatusCode checkElectronEDM(std::string trigItem);

        StatusCode checkPhotonEDM(std::string trigItem);

        StatusCode isTagElectron(const xAOD::Electron *);

        StatusCode isProbeElectron(const xAOD::Electron *);

        StatusCode isMatchEF(const xAOD::Electron *,std::string);
        
        StatusCode isMatchL2(const xAOD::Electron *);
        
        StatusCode isMatchL1(const xAOD::Electron *);
      
        StatusCode AbsoluteResolutions(const xAOD::Electron *, const xAOD::Electron *);

        StatusCode combinedEFExecute(const xAOD::Electron *);

        StatusCode combinedL2Execute(const xAOD::Electron *);

        StatusCode combinedL2CaloExecute(const xAOD::Electron *);
        
        StatusCode fillIsEM(const xAOD::Electron *, const xAOD::Electron *);
        // Const methods -- consider we can dress the object?
        const xAOD::Electron * ClosestEFObject(const xAOD::Electron *);

        std::vector<const xAOD::Electron*> m_Probes;
        std::vector<TLorentzVector> m_ProbesLV;
        xAOD::ElectronContainer *m_matchProbes;
        xAOD::ElectronContainer *m_failedProbes;

        std::vector<unsigned int> m_NProbesIsEM;
        std::vector<unsigned int> m_NFailedIsEM;
        // Counters for average efficiencies
        //std::vector<int> m_nProbes; 
        //std::vector<int> m_nMatches;
};

#endif
