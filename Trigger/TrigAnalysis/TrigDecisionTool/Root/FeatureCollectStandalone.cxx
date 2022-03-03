// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigDecisionTool/FeatureCollectStandalone.h"

#include "TrigDecisionTool/TypelessFeature.h"
#include "TrigNavStructure/TrigNavStructure.h"

#include "TrigSteeringEvent/TrigPassBits.h"



//pretty printing a TypelessFeature object
std::ostream & Trig::operator<<(std::ostream & o, const Trig::TypelessFeature & feat) {
  o << "Feature accessible by : " << feat.accessHelper();
  return o;
}

namespace Trig{
  namespace FeatureAccessImpl {

    HLT::TriggerElement::FeatureAccessHelper getTypelessBits(const HLT::TriggerElement* te, const HLT::TrigNavStructure* navigation){

      //completely disabled for now. should be reenabled once we have more recent TNS tag in Athena
      HLT::TriggerElement::FeatureAccessHelper answer;
      #ifndef XAOD_STANDALONE
      answer = navigation->getFeature(te,ClassID_traits<TrigPassBits>::ID(),"passbits");
      #else
      (void)te;
      (void)navigation;
      #endif


      if(answer.valid()){
#ifdef XAOD_STANDALONE // AnalysisBase 
        // In AnalysisBase we can't access TrigPassBits. So warn the user about
        // this, and pretend that no TrigPassBits is available for this element.
        static bool warningPrinted = false;
        if( ! warningPrinted ) {
                std::cerr << "FeatureAccessImpl::getTypelessBits WARNING "
                          << "Retrieving a container that has TrigPassBits attached "
                          << "to it" << std::endl;
                std::cerr << "FeatureAccessImpl::getTypelessBits WARNING "
                          << "But it's not possible to use TrigPassBits in "
                          << "standalone mode" << std::endl;
                std::cerr << "FeatureAccessImpl::getTypelessBits WARNING "
                          << "So, keep in mind that some of the returned objects "
                          << "didn't actually pass the required chain"
                          << std::endl;
                warningPrinted = true;
        }
        return HLT::TriggerElement::FeatureAccessHelper();
#else
        return answer;
#endif // XAOD_STANDALONE
      }
      return HLT::TriggerElement::FeatureAccessHelper();
    }


    void typelessCollect( const HLT::TriggerElement* te, HLT::class_id_type clid,
        std::vector<Trig::TypelessFeature>& data,
        const std::string& label, unsigned int condition,
        const HLT::TrigNavStructure* navigation ){
      //collect recursively features for this trigger element
      if (condition == TrigDefs::Physics && !te->getActiveState() ) return;   

      const HLT::TriggerElement* source = 0;
      HLT::TriggerElement::FeatureAccessHelper answer = navigation->getFeatureRecursively(te,clid,label,source);
      if(!answer.valid() && source){
        //source was set but answer invalid -> bifurcation
        for(auto& predecessor : navigation->getDirectPredecessors(source)){
          typelessCollect(predecessor,clid,data,label,condition,navigation);
        }
      }
      if(answer.valid()){    
        auto typelessholder = navigation->getHolder(answer);
        if(!typelessholder){
          //this can happen, because features were thinned away (which deletes the Holders, but doesn't touch the TE structure)
          //so we'll let this slip silently
          return;
        }
        data.push_back(Trig::TypelessFeature(answer,source,typelessholder->label()));
      }
    }
  } // EOF namespace FeatureAccessImpl 
} // EOF namespace Trig

