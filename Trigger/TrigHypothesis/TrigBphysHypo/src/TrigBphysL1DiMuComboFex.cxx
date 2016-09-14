/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigBphysL1DiMuComboFex.h"

#include "TrigT1Interfaces/RecMuonRoI.h"

using namespace std;


TrigBphysL1DiMuComboFex::TrigBphysL1DiMuComboFex(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator)
{
   declareMonitoredStdContainer("PT", m_pt);

   declareMonitoredVariable("DEta", m_dEta);
}


     
HLT::ErrorCode
TrigBphysL1DiMuComboFex::hltExecute( HLT::TEConstVec& input, 
                                 HLT::TriggerElement* output )
{
   ATH_MSG_DEBUG("TrigBphysL1DiMuComboFex::hltExecute() with " << input.size() << " input TEs");

   vector<const LVL1::RecMuonRoI*> parentMuonRoIs;

   for( const HLT::TriggerElement * te : input ) {

      const LVL1::RecMuonRoI * muonROI(0);

      HLT::ErrorCode ec = getFeature(te, muonROI);

      if(ec==HLT::OK && muonROI) {
         ATH_MSG_DEBUG("Found RecMuonRoI object in this TE");
         parentMuonRoIs.push_back(muonROI);
         m_pt.push_back(muonROI->getThresholdValue());
      } else {
         ATH_MSG_WARNING("Did not find a RecMuonRoI in the input TE. Could be a configuration error!");
      }
   }
   
   if(parentMuonRoIs.size()==2) {
      const LVL1::RecMuonRoI *muon1(parentMuonRoIs[0]), *muon2(parentMuonRoIs[1]);
      m_dEta = muon1->eta()-muon2->eta();
      ATH_MSG_DEBUG( "Found combo with delta eta " << m_dEta );
   } else {
      ATH_MSG_WARNING( "Expected to find exactly 2 ROI in the input to this Combo alg but found " << parentMuonRoIs.size() );
   }


   // now I can do something with this combination and attach something new to the output



   // and activate the output for good combinations
   output->setActiveState( true );

   return HLT::OK;
}


HLT::ErrorCode
TrigBphysL1DiMuComboFex::hltInitialize() {
   return HLT::OK;
}


HLT::ErrorCode
TrigBphysL1DiMuComboFex::hltFinalize() {
   return HLT::OK;
}
