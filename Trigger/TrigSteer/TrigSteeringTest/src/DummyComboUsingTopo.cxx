/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringTest/DummyComboUsingTopo.h"

#include "TrigT1Interfaces/RecJetRoI.h"

using namespace std;


DummyComboUsingTopo::DummyComboUsingTopo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator)
{
   declareMonitoredStdContainer("PT", m_pt);

   declareMonitoredVariable("DEta", m_dEta);
}


     
HLT::ErrorCode
DummyComboUsingTopo::hltExecute( HLT::TEConstVec& input, 
                                 HLT::TriggerElement* output )
{
   ATH_MSG_DEBUG("DummyComboUsingTopo::hltExecute() with " << input.size() << " input TEs");

   vector<const LVL1::RecJetRoI*> parentJetRoIs;

   for( const HLT::TriggerElement * te : input ) {

      //const LVL1::RecJetRoIContainer * recJetRoIs(0);
      const LVL1::RecJetRoI * jetROI(0);

      HLT::ErrorCode ec = getFeature(te, jetROI);

      if(ec==HLT::OK) {
         ATH_MSG_DEBUG("Found recJetRoi object in this TE");
         parentJetRoIs.push_back(jetROI);
         m_pt.push_back(jetROI->etLarge());
      } else {
         ATH_MSG_WARNING("Did not find a RecJetRoI in the input TE. Could be a configuration error!");
      }
   }
   
   if(parentJetRoIs.size()==2) {
      const LVL1::RecJetRoI *jet1(parentJetRoIs[0]), *jet2(parentJetRoIs[1]);
      m_dEta = jet1->eta()-jet2->eta();
      ATH_MSG_DEBUG( "Found combo with delta eta " << m_dEta );
   } else {
      ATH_MSG_WARNING( "Expected to find exactly 2 ROI in the input to this Combo alg but found " << parentJetRoIs.size() );
   }


   // now I can do something with this combination and attach something new to the output



   // and activate the output for good combinations
   output->setActiveState( true );

   return HLT::OK;
}


HLT::ErrorCode
DummyComboUsingTopo::hltInitialize() {
   return HLT::OK;
}


HLT::ErrorCode
DummyComboUsingTopo::hltFinalize() {
   return HLT::OK;
}
