/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigGenericAlgs/DummyCopyAllTEAlgo.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
/*
 * create TE for each input TE except last
 * copy all previouse features from last TE to each outputTE
 */
using namespace PESA;
DummyCopyAllTEAlgo::DummyCopyAllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::AllTEAlgo(name, pSvcLocator) {
  
}

HLT::ErrorCode DummyCopyAllTEAlgo::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
						  unsigned int output) {
    unsigned counter(0);

    if ( input.size() != 2 ) {
        ATH_MSG_WARNING("Expect 2 inputs, got " << input.size());
        return HLT::OK;
    }

    if ( input.back().size() != 1 ) {
        ATH_MSG_WARNING("Expect 1 TE, got " << input.back().size());
        return HLT::OK;
    }

    const HLT::TriggerElement *cacheTE=input.back().back();
    ATH_MSG_DEBUG("cacheTE features " << cacheTE->getPreviousFeatures().size());
    
    for(const auto& vec : input){
        for(auto *te : vec){
            if(te==cacheTE) continue;
            HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(te, output);
            outputTE->setActiveState(true);
            for ( auto f: cacheTE->getPreviousFeatures() ){
                outputTE->addFeature(f);
            }
            ++counter;
        }       
    }
    ATH_MSG_DEBUG("Created: " << counter << " TEs seeded by every input passed");    
    return HLT::OK;
}
    
