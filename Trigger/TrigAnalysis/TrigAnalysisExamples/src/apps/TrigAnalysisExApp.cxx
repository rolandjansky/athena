/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TrigAnalysisApp.cxx
 * 2016 Ryan Mackenzie White <ryan.white@cern.ch>
 * Simple TEvent application for trigger analysis
 * Original implementation provided by W. Buttinger
 */

#ifndef TRIGANALYSISEXAMPLE_TRIGANALYSISEXAPP_H
#define TRIGANALYSISEXAMPLE_TRIGANALYSISEXAPP_H 1


#include "AthAnalysisBaseComps/AthAnalysisHelper.h" //tool creation and configuration
#include "POOLRootAccess/TEvent.h" //event looping
#include "GaudiKernel/ToolHandle.h" //for better working with tools

#include "TrigDecisionTool/TrigDecisionTool.h"

//ROOT includes
#include "TString.h"
#include "TSystem.h"

#include <iostream>


int main() {

    IAppMgrUI* app = POOL::Init("POOLRootAccess/basic.opts"); //important to do this first, for MessageSvc to exist properly

    // Open the input file:
    TString fileName = "/afs/cern.ch/atlas/project/PAT/xAODs/r8565/valid2.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e4993_s2887_r8565/AOD.09534072._000257.pool.root.1";

    ToolHandle<Trig::TrigDecisionTool> tdt("Trig::TrigDecisionTool/TrigDecisionTool");
#ifndef XAOD_ANALYSIS
    //only need to set this property if using a FULL offline release (not analysis release)
    std::vector<std::string> EDMLibraries = { "TrigSteeringEvent", "TrigMuonEvent",
                      "TrigBphysicsEvent", "TrigCaloEvent",
                      "TrigInDetEvent",
                      "TrigParticle",      "TrigMissingEtEvent",
                      "TrigDecisionEvent", "TrigMonitoringEvent",
              "TrigTopoEvent" , "TrigCombinedEvent",
                  "TrigMonitoringEventTPCnv",   "RecTPCnv",
                      "TrigCaloEventTPCnv",
                      "TrigDecisionEventTPCnv",     "TrigInDetEventTPCnv",
                      "TrigMissingEtEventTPCnv",    "TrigMuonEventTPCnv",
                      "TrigParticleTPCnv",          "TrigSteeringEventTPCnv",
              "TrigTopoEventTPCnv",         "TrigCombinedEventTPCnv",
              "tauEventTPCnvDict",          "RecTPCnvDict",
              "TrigMonitoringEventTPCnvDict",
              "OLD_RecTPCnvDict",
                      "TrigCosmicEvent",
                      "xAODCaloEvent", "xAODEgamma", "xAODTracking", "xAODMuon", "xAODTau", "xAODJet",
                      "xAODTrigBphys", "xAODTrigMissingET", "xAODTrigEgamma", "xAODTrigMuon", "xAODTrigCalo",
                      "xAODTrigMinBias", "xAODBTagging", "xAODTrigRinger",
              "xAODHIEvent",
                      "xAODCaloEventDict", "xAODEgammaDict", "xAODTrackingDict", "xAODMuonDict", "xAODTauDict", "xAODJetDict",
                      "xAODTrigBphysDict", "xAODTrigMissingETDict", "xAODTrigEgammaDict", "xAODTrigMuonDict", "xAODTrigCaloDict",
                      "xAODTrigMinBiasDict", "xAODBTaggingDict",
                                                 "xAODHIEventDict"};
    AAH::setProperty(tdt,"Navigation.Dlls",EDMLibraries).ignore();
    POOL::TEvent::EReadMode mode = POOL::TEvent::kPOOLAccess; //use POOL mode for full offline release
#else
    POOL::TEvent::EReadMode mode = POOL::TEvent::kClassAccess; //use fast xAODRootAccess mode in analysis release
#endif

    //loop over input file with POOL
    POOL::TEvent evt(mode);
    evt.readFrom( fileName ).ignore();
    evt.getEntry(0);

    tdt.retrieve().ignore();

    for(int i=0;i < 10 /*evt.getEntries()*/; i++) {
        if( evt.getEntry(i) < 0) { std::cout << "Failed to read event " << i << std::endl; continue; }

        std::cout << "HLT_.* decision = " << tdt->isPassed("HLT_.*") << std::endl;

        auto cg = tdt->getChainGroup("HLT_.*");

        //this only works in FULL offline release
        auto feat = cg->features();
        std::vector<Trig::Feature<xAOD::TrigMissingET> > metfeats = feat.elementFeature<xAOD::TrigMissingETContainer>();
        std::cout << "found " << metfeats.size() << " met features" << std::endl;

        for(auto& feat : metfeats) {
            std::cout << "feature pointing to " << feat.cptr() << " ex: " << feat.cptr()->ex() << " ey:" << feat.cptr()->ey() << std::endl;
        }

        const std::string eltrigger="HLT_2e17_lhvloose_nod0";
        std::cout << eltrigger << " passed: " << tdt->isPassed( eltrigger ) << std::endl;

        // Features can be retrieved even when trigger fails
        // The following condition returns the features for all TriggerElements, irregardless whether they are deactivated (i.e., the hypo fails)
        auto fcel = tdt->features( eltrigger, TrigDefs::alsoDeactivateTEs );

        //Retrieve the RoI descriptors for electron trigger
        std::vector< Trig::Feature<TrigRoiDescriptor> > rois = fcel.elementFeature<TrigRoiDescriptorCollection>("initialRoI");
        std::cout << "found " << rois.size() << " roi features" << std::endl;
        for(auto& roi : rois) {
            std::cout << "feature pointing to " << roi.cptr() << " eta: " << roi.cptr()->eta() << " phi:" << roi.cptr()->phi() << std::endl;
        }
        
        // Retrieve a vector of Trigger Element (TE) features, i.e. collect all the TEs that contain ElectronContainers
        std::vector<Trig::Feature<xAOD::ElectronContainer> > vec = fcel.containerFeature<xAOD::ElectronContainer>("egamma_Electrons");
        std::cout << "Electron trigger has " << vec.size() << " electron features " << std::endl;
        for( const Trig::Feature<xAOD::ElectronContainer>& feat : vec ) {
            // With a TE, retrieve ancestors and activeState (did the hypo pass/fail?)
            // Which step is active?
            const xAOD::ElectronContainer *cont=feat.cptr();
            for(const xAOD::Electron *e : *cont){
                if(!e) 
                    continue;
                // Check whether the electron was selected by the hypothesis
                std::cout << " eta: " << e->eta() << " phi: " << e->phi() << "et: " << e->e()/cosh(e->eta()) << std::endl;
            }
            std::cout << " L2Calo Active " <<  (tdt->ancestor<xAOD::TrigEMCluster>(feat.te(),"")).te()->getActiveState()  
                    << " L2Electron Active " <<(tdt->ancestor<xAOD::TrigElectronContainer>(feat.te(),"")).te()->getActiveState()  
                    << " EFCalo Active " << (tdt->ancestor<xAOD::CaloClusterContainer>(feat.te(),"")).te()->getActiveState() << std::endl; 
        }

        // Retrieve featues from L2 step
        std::vector<Trig::Feature<xAOD::TrigElectronContainer> > vecl2 = fcel.containerFeature<xAOD::TrigElectronContainer>();
        std::cout << "Electron trigger has " << vecl2.size() << " trig electron features " << std::endl;
        for( const Trig::Feature<xAOD::TrigElectronContainer>& feat : vecl2 ) {
            // With a TE, retrieve ancestors and activeState (did the hypo pass/fail?)
            // Which step is active?
            const xAOD::TrigElectronContainer *cont=feat.cptr();
            for(const xAOD::TrigElectron *e : *cont){
                if(!e) 
                    continue;
                // Check whether the electron was selected by the hypothesis
                std::cout << " eta: " << e->eta() << " phi: " << e->phi() << "e: " << e->e() << std::endl;
            }
            std::cout << " L2Calo Active " <<  (tdt->ancestor<xAOD::TrigEMCluster>(feat.te(),"")).te()->getActiveState() << std::endl;  
        }


        // Can also look at the combinations of the two rois 
        std::cout << eltrigger << " has " << fcel.getCombinations().size() << " combinations, " 
            << fcel.containerFeature<xAOD::ElectronContainer>().size() << " Electron features, " << std::endl;
        for( const Trig::Combination& comb : fcel.getCombinations()){ 
            std::vector< Trig::Feature <xAOD::ElectronContainer> > elfeats = comb.containerFeature<xAOD::ElectronContainer>();
            std::cout << "Combination has " << elfeats.size() << " Electron features " << std::endl;
        }


    }

    app->finalize().ignore(); //optional trigger finalization of all services and tools created by the Gaudi Application
    return 0;
}

#endif //> !TRIGANALYSISEXAMPLE_TRIGANALYSISEXAPP_H))))
