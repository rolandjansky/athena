/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 * TriggerAnalysisHelper.h
 * 
 *
 
 */
#ifndef TRIGGERANALYSISHELPER_H
#define TRIGGERANALYSISHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "xAODTrigger/TrigPassBits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"

//TrigConf include(s)
#include "TrigConfHLTData/HLTTriggerElement.h"
static const InterfaceID IID_TriggerAnalysisHelper("Trig::TriggerAnalysisHelper", 1, 0);
namespace Trig{
    class TriggerAnalysisHelper : public AthAlgTool {

        public:
            TriggerAnalysisHelper( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

            static const InterfaceID& interfaceID() {
                return IID_TriggerAnalysisHelper;
            }
            
            virtual StatusCode initialize();

        public:

            template<class T>
                bool ancestorPassed(const HLT::TriggerElement* te,const std::string key="");
            template<class T>
                const T*
                getFeature(const HLT::TriggerElement* te,const std::string key="");
            template<class T>
                void printFeatureAndTEInfo(const Trig::Feature<T> feature);
            template<class T>
                void getFeatureAndTEInfo(const std::string chainName,unsigned int condition = TrigDefs::Physics,const std::string key="");
            template<class T1, class T2>
                void getCombinationAndTEInfo(const std::string chainName, 
                        unsigned int condition = TrigDefs::Physics, 
                        const std::string key1="", 
                        const std::string key2="");
            template<class T>
                void getSelectedObjects(const std::string chainName,unsigned int condition=TrigDefs::Physics,const std::string key="");
        private:
            ToolHandle< Trig::TrigDecisionTool > m_trigDecisionTool;
            ToolHandle<Trig::IMatchingTool> m_matchTool;


    }; // Class TriggerAnalysisHelper
    // ****************************************************************************************************
    // 
    // The following methods rely on having a particular Trigger Element TE.
    // To retrieve a TE, first need to get the FeatureContainer.
    // With the FeatureContainer get a vector of Features.
    // Each Feature is attached to a TE, the TE corresponds to a Chain.
    // ****************************************************************************************************
    //
    // ancestorPassed
    // Return the state, i.e. did this hypothesis algorithm pass or fail for a particular trigger chain
    //

    template<class T>
        bool TriggerAnalysisHelper::ancestorPassed(const HLT::TriggerElement* te,const std::string key){
            // Sanity check
            if ( te == nullptr ) return false;
            // Possible to get back a nullptr from the navigation
            // in which case, should not be active
            if ( (m_trigDecisionTool->ancestor<T>(te,key)).te() == nullptr )
                return false;
            // Did the hypo pass with given container T tested in hypothesis
            return ( (m_trigDecisionTool->ancestor<T>(te,key)).te()->getActiveState());
        }
    // getFeature
    // Return a container of objects from a particular trigger chain
    template<class T>
        const T*
        TriggerAnalysisHelper::getFeature(const HLT::TriggerElement* te,const std::string key){
            if ( te == nullptr ) return nullptr;
            if ( (m_trigDecisionTool->ancestor<T>(te,key)).te() == nullptr )
                return nullptr;
            return ( (m_trigDecisionTool->ancestor<T>(te,key)).cptr() );
        }


    template<class T>
    void TriggerAnalysisHelper::printFeatureAndTEInfo(const Trig::Feature<T> feature){

        std::string label="";
        const T *container=feature.cptr();
        TrigConf::HLTTriggerElement::getLabel (feature.te()->getId(), label );
        
            ATH_MSG_INFO("T1 feature label: " << feature.label() 
                << " TE Id: "<< feature.te()->getId() 
                << " TE label: " << label 
                << " has " << container->size() << " objects ");
            for(const auto *obj:*container){
                //ATH_MSG_INFO("Object kinematics e: "<< obj->e() << " eta: " << obj->eta << " phi: " << obj->phi());
                ATH_MSG_INFO("Object type: "<< obj->type() << " kinematics, eta: " << obj->eta() << " phi, " << obj->phi()); 
            }
        
    }
    // dumpFeatureAndTEInfo
    // Following templated example retrieves FeatureContainer for a given trigger.
    // Retrieves a vector of features of class type T 
    // Loops over each feature, i.e., the TriggerElements
    // dumps the feature label and the TriggerElement label
    // gets a pointer to the container of type T and dumps the size of the container
    template<class T>
        void TriggerAnalysisHelper::getFeatureAndTEInfo(const std::string chainName,unsigned int condition,const std::string key){
            FeatureContainer features = m_trigDecisionTool->features( chainName, condition);
            std::vector<Feature<T> > vectorOfFeatures = features.get<T>(key);
            ATH_MSG_INFO(chainName << " has " << features.get<T>().size() << " T features ");
            // Loop over the features of type T
            for( const Trig::Feature<T> feature : vectorOfFeatures ) {
                printFeatureAndTEInfo<T>(feature);
            } // End loop over the features
        }

    template<class T1, class T2>
        void TriggerAnalysisHelper::getCombinationAndTEInfo(const std::string chainName,unsigned int condition,const std::string key1,const std::string key2){
            FeatureContainer features = m_trigDecisionTool->features( chainName,condition );

            ATH_MSG_INFO(chainName << " has " << features.getCombinations().size() << " combinations, " 
                    << features.get<T1>().size() << " T1 features, "
                    << features.get<T2>().size() << " T2 features");



            // Loop over Combination container
            std::string label;
            for( const Combination& comb : features.getCombinations()){
                ATH_MSG_INFO( "COMB Combination was " << ( comb.active() ? "" : "not " ) << "active." );
                std::vector< Trig::Feature <T1> > feature1 = comb.get<T1>(key1);
                ATH_MSG_INFO("COMB Combination has " << feature1.size() << " T1 features "); 

                for(const Trig::Feature<T1> feat1:feature1){
                    printFeatureAndTEInfo<T1>(feat1);
                }

                std::vector< Trig::Feature <T2> > feature2 = comb.get<T2>(key2);
                ATH_MSG_INFO("COMB Combination has " << feature2.size() << " T2 features ");
                for(const Trig::Feature<T2> feat2:feature2){
                    printFeatureAndTEInfo<T2>(feat2);
                }
            }
        }

    template<class T>
        void TriggerAnalysisHelper::getSelectedObjects(const std::string chainName,unsigned int condition,const std::string key){
           
            
            FeatureContainer features = m_trigDecisionTool->features( chainName, condition);
            std::vector<Feature<T> > vectorOfFeatures = features.get<T>(key);
            // Loop over the features of type T
            for( const Trig::Feature<T> feature : vectorOfFeatures ) {
                const T *cont=feature.cptr(); //Get pointer to container of type T
                const xAOD::TrigPassBits *bits=(m_trigDecisionTool->ancestor<xAOD::TrigPassBits>(feature.te())).cptr();
                if(!bits) 
                    ATH_MSG_WARNING("xAOD::TrigPassBits null");

                for(const auto *obj : *cont){
                    if(!obj) {
                        ATH_MSG_WARNING("Object is null");
                        continue;
                    }
                    // Check whether the electron was selected by the hypothesis
                    if(bits)
                        ATH_MSG_INFO( "Object selected " << feature.label() << " = " << ( bits->isPassing(obj,cont) ? "PASS" : "FAIL" ) );
                }
            } // End loop over the features
        }
} //Namespace Trig

#endif /* !TRIGGERANALYSISHELPER_H */
