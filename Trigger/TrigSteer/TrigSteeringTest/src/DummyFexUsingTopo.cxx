/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringTest/DummyFexUsingTopo.h"

using namespace std;


DummyFexUsingTopo::DummyFexUsingTopo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
{
   //declareProperty("prescale", m_prescale=1, "fraction of accept input returns true");
}


     
HLT::ErrorCode
DummyFexUsingTopo::hltExecute( const HLT::TriggerElement* input,
                               HLT::TriggerElement* output )
{
   ATH_MSG_INFO("DummyFexUsingTopo::hltExecute()");

   const xAOD::TrigCompositeContainer * compCont(0);

   HLT::ErrorCode ec = getFeature(input, compCont);
   if(ec==HLT::OK) {
      ATH_MSG_INFO("Found " << compCont->size() << " TrigComposite objects");
      for(const xAOD::TrigComposite * l1topo : *compCont) {
         ATH_MSG_INFO("TrigComposite ptr = " << (void*)l1topo);
         for(const string & collName : l1topo->linkColNames()) {
            ATH_MSG_INFO("    link to collection " << collName);
         }         
         ATH_MSG_INFO("Found link named EM0 : " << (l1topo->hasObjectLink( "EM0" ) ? "yes":"no"));
         ATH_MSG_INFO("Found link named EM1 : " << (l1topo->hasObjectLink( "EM1" ) ? "yes":"no"));
         ATH_MSG_INFO("Found link named EM2 : " << (l1topo->hasObjectLink( "EM2" ) ? "yes":"no"));
         if(l1topo->hasObjectLink("EM0")) {
            const xAOD::EmTauRoI* emRoi = l1topo->object<xAOD::EmTauRoI>( "EM0" );
            ATH_MSG_INFO("EM0 link pointer : ET = " << emRoi->emClus() << ", eta=" << emRoi->eta() << ", phi=" << emRoi->phi());
         }
      }
   }
   output->setActiveState( true );

   return HLT::OK;
}


HLT::ErrorCode
DummyFexUsingTopo::hltInitialize() {

   return HLT::OK;
}


HLT::ErrorCode
DummyFexUsingTopo::hltFinalize() {

   return HLT::OK;
}
