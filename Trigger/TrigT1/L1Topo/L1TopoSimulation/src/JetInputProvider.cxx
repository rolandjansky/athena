/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "JetInputProvider.h"
#include "TrigT1CaloEvent/JetROI_ClassDEF.h"
#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"


using namespace std;
using namespace LVL1;

JetInputProvider::JetInputProvider(const std::string& type, const std::string& name, 
                                   const IInterface* parent) :
   base_class(type, name, parent),
   m_jetLocation(TrigT1CaloDefs::JetROILocation)
{
   declareProperty( "JetROILocation", m_jetLocation, "Storegate key for the Jet ROIs" );
}

JetInputProvider::~JetInputProvider()
{}

StatusCode
JetInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
   // https://indico.cern.ch/conferenceDisplay.py?confId=284687
   
   // Electron ROI:
   // | 0 0 1 0 | 2b Crate | 4b CPM Num | 3b CPChip | 3b Local coords | 0 0 0 | 5b isolation | 8b electron energy |


   // Retrieve EMTAU RoIs (they are built by EMTAUTrigger)
   DataVector<JetROI> * jetROIs = new DataVector<JetROI>;
   if( ! evtStore()->retrieve(jetROIs, m_jetLocation).isSuccess()) {
      ATH_MSG_ERROR("No JET ROI container '" << m_jetLocation << "' found in the event. Configuration issue.");
      return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Filling the input event. Number of Jet ROIs: " << jetROIs->size());
   for(const JetROI * roi: *jetROIs) {
      ATH_MSG_DEBUG("Jet ROI: e = " << roi->energy() << ", eta = " << roi->eta() <<", phi = " << roi->phi() << ", w   = " << hex << roi->roiWord() << dec);
      TCS::JetTOB jet( roi->clusterEnergy8(), roi->clusterEnergy4(), round(10*roi->eta()), round(10*roi->phi()) );
      jet.setEtaDouble( roi->eta() );
      jet.setPhiDouble( roi->phi() );
      inputEvent.addJet( jet );
   }

   return StatusCode::SUCCESS;
}
