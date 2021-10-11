/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_ITRIGEGAMMAFASTELECTRONHYPOTOOL_H
#define TRIGEGAMMAHYPO_ITRIGEGAMMAFASTELECTRONHYPOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"



/**
 * @class Base for tools dooing Egamma Fast Calo Hypo selection
 * @brief 
 **/

class ITrigEgammaFastElectronHypoTool: virtual public ::IAlgTool
{ 

  public: 

    DeclareInterfaceID(ITrigEgammaFastElectronHypoTool, 1, 0);

    virtual ~ITrigEgammaFastElectronHypoTool(){}

    struct ElectronInfo {
      ElectronInfo( TrigCompositeUtils::Decision* d, 
                    const xAOD::TrigElectron* el,
                    const xAOD::TrigEMCluster* cl,
                    const xAOD::TrigRingerRings* r,
                    const TrigCompositeUtils::DecisionIDContainer previousDecisions )
      : decision( d ), 
        electron(el),
        cluster(cl),
        rings(r),
        previousDecisionIDs( previousDecisions ){}

        TrigCompositeUtils::Decision* decision;
        const xAOD::TrigElectron *electron;
        const xAOD::TrigEMCluster *cluster;
        const xAOD::TrigRingerRings* rings;
        const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
        std::map<std::string, float> valueDecorator;
        std::map<std::string, bool> pidDecorator;
    };



   virtual StatusCode decide( std::vector<ElectronInfo>& input )  const = 0;

   /**
    * @brief Makes a decision for a single object
    * The decision needs to be returned
    **/ 
   virtual bool decide( const ElectronInfo& i ) const = 0;

 protected:


}; 


#endif 