/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./EMTauInputProvider.h"

#include <math.h>

#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include "TrigT1CaloEvent/CPCMXTopoData.h"

using namespace std;
using namespace LVL1;

EMTauInputProvider::EMTauInputProvider(const std::string& type, const std::string& name, 
                                       const IInterface* parent) :
   base_class(type, name, parent),
   m_emTauLocation( TrigT1CaloDefs::EmTauTopoTobLocation )
{
   declareProperty( "EmTauROILocation", m_emTauLocation, "Storegate key for the EMTAU info from CMX" );
}

EMTauInputProvider::~EMTauInputProvider()
{}

StatusCode
EMTauInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

   /** this will be the new format
       https://indico.cern.ch/conferenceDisplay.py?confId=284687
       Electron ROI:
       | 0 0 1 0 | 2b Crate | 4b CPM Num | 3b CPChip | 3b Local coords | 0 0 0 | 5b electron isolation/veto | 8b electron energy |
       Tau ROI:
       | 0 0 1 1 | 2b Crate | 4b CPM Num | 3b CPChip | 3b Local coords | 0 0 0 | 5b tau isolation/veto      | 8b tau energy      |
   */

   // Retrieve EMTAU RoIs (they are built by EMTAUTrigger)

   DataVector<CPCMXTopoData> * emtau = 0;
   if( ! evtStore()->retrieve(emtau, m_emTauLocation).isSuccess()) {
      ATH_MSG_ERROR("No CPCMXTopoDataCollection found in the event. Configuration issue.");
      return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Filling the input event. Number of emtau topo data objects: " << emtau->size());
   for(const CPCMXTopoData * topoData : * emtau) {

      // fill the vector of TOBs
      std::vector< CPTopoTOB > tobs;
      topoData->tobs(tobs);

      ATH_MSG_DEBUG("Emtau topo data object has # TOBs: " << tobs.size());
      for(const CPTopoTOB & tob : tobs ) {

         ATH_MSG_DEBUG( "EMTAU TOB with cmx = " << tob.cmx() << "[" << (tob.cmx()==0?"EM":"TAU") << "]"
                        << " : e = " << setw(3) << tob.et() << ", isolation " << tob.isolation()
                        << ", eta = " << setw(2) << tob.eta() << ", phi = " << tob.phi()
                        << ", ieta = " << setw(2) << tob.ieta() << ", iphi = " << tob.iphi()
                        << ", word = " << hex << tob.roiWord() << dec
                        );

         TCS::ClusterTOB cl(tob.et(), tob.isolation(), tob.ieta(), tob.iphi(), tob.cmx()==0 ? TCS::CLUSTER : TCS::TAU, tob.roiWord() );
         cl.setEtaDouble( tob.eta() );
         cl.setPhiDouble( tob.phi() );
         
         if(tob.cmx()==0) {
            inputEvent.addCluster( cl );
         } else {
            inputEvent.addTau( cl );            
         }
      }
   }

   return StatusCode::SUCCESS;
}

void 
EMTauInputProvider::CalculateCoordinates(int32_t roiWord, double & eta, double & phi) const {
   static CPRoIDecoder get;
   static double TwoPI = 2 * M_PI;
   CoordinateRange coordRange = get.coordinate( roiWord );
   
   eta = coordRange.eta();
   phi = coordRange.phi();
   if( phi > M_PI ) phi -= TwoPI;
}
