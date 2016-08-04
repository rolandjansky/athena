/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFlowUtils/ParticleFlowEventFilter_r207.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODPFlow/PFO.h"

ParticleFlowEventFilter_r207::ParticleFlowEventFilter_r207(const std::string& name, ISvcLocator* pSvcLocator) :  AthAlgorithm(name, pSvcLocator) {
}


StatusCode ParticleFlowEventFilter_r207::initialize(){
  return StatusCode::SUCCESS;
}

StatusCode ParticleFlowEventFilter_r207::execute(){

  this->setFilterPassed(false);
  
  const xAOD::JetContainer* pflowJets = nullptr;
  CHECK( evtStore()->retrieve( pflowJets, "AntiKt4EMPFlowJets") );

  for (const auto* pflowJet: *pflowJets){
    xAOD::JetConstituentVector constituents =  pflowJet->getConstituents();
    for (const auto* aConstituent : constituents){
      float weight = 0.0;
      if (aConstituent){
        const xAOD::IParticle* theRawConstituent = aConstituent->rawConstituent();
        if (theRawConstituent){
          const xAOD::PFO* thePFO = dynamic_cast<const xAOD::PFO*>(theRawConstituent);
          if (thePFO->charge() != 0) {
            if (thePFO->pt() < 30000) weight = 1.0;
            else if (thePFO->pt() >= 30000 && thePFO->pt() < 60000) weight = (1.0 - (thePFO->pt()-30000)/30000);

            if (0.0 != weight){
               xAOD::PFODetails::PFOAttributes myAttribute_isInDenseEnvironment = xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment;
               
               int isInDenseEnvironment = false;
               bool gotVariable = thePFO->attribute(myAttribute_isInDenseEnvironment,isInDenseEnvironment);
               if (false == gotVariable) ATH_MSG_WARNING("This charged PFO did not have eflowRec_isInDenseEnvironment set");
           
               float expectedEnergy = 0.0;
               xAOD::PFODetails::PFOAttributes myAttribute_tracksExpectedEnergyDeposit = xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit;
               gotVariable = thePFO->attribute(myAttribute_tracksExpectedEnergyDeposit,expectedEnergy);

               if (false == gotVariable) {
                  ATH_MSG_WARNING("This charged PFO did not have eflowRec_tracksExpectedEnergyDeposit set");
                  weight = 1.0;
               }
               else{
                 if (true == isInDenseEnvironment) {
                   //nothing
                 }
                 else{
                   if (1.0 == weight) {
                     //nothing;
                   }
                   else{
                     this->setFilterPassed(true);
                   }
                 }
               }               
            }       
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ParticleFlowEventFilter_r207::finalize(){
  return StatusCode::SUCCESS;
}

