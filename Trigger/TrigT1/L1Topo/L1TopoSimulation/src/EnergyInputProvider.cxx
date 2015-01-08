/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EnergyInputProvider.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloEvent/EnergyTopoData.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"


using namespace std;
using namespace LVL1;

EnergyInputProvider::EnergyInputProvider(const std::string& type, const std::string& name, 
                                         const IInterface* parent) :
   base_class(type, name, parent),
   m_energyLocation(TrigT1CaloDefs::EnergyTopoDataLocation)
{
   declareProperty( "EnergyROILocation", m_energyLocation, "Storegate key for reading the Topo Energy ROI" );
}

EnergyInputProvider::~EnergyInputProvider()
{}

StatusCode
EnergyInputProvider::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

   /** https://indico.cern.ch/conferenceDisplay.py?confId=284687
       Energy ROI:
       | 0 1 0 0 | T | 0  0 0 | 8b Missing ET Sig hits (or 8*0) | XO | 15b Signed Ex       |
       | 0 1 0 1 | T | 0  0 0 | 8b Missing ET hits              | YO | 15b Signed Ey       |
       | 0 1 1 0 | T | 0  0 0 | 8b Sum ET hits                  | TO | 15b Unsigned Sum ET |
       T = sum type: 0-normal, 1-masked
       O = overflow Ex, Ey, Et
    */

//    EnergyRoI * energyROI = 0;
//    if( ! evtStore()->retrieve(energyROI, m_energyLocation).isSuccess()) {
//       ATH_MSG_ERROR("No EnergyROI with key '" << m_energyLocation << "' found in the event. Configuration issue.");
//       return StatusCode::FAILURE;
//    }

//    ATH_MSG_DEBUG("Not filling the input event from EnergyROI. There is not enough information.");
//    ATH_MSG_DEBUG("EnergyROI: word0 = " << hex << energyROI->roiWord0()
//                  << ", word1 = " << hex << energyROI->roiWord1()
//                  << ", word2 = " << hex << energyROI->roiWord2());


   const EnergyTopoData* topoData = 0;
   CHECK( evtStore()->retrieve(topoData, m_energyLocation) );

   ATH_MSG_DEBUG( "EnergyTopoData" << dec
                  << ": Ex = " << topoData->Ex()
                  << ", Ey = " << topoData->Ey()
                  << ", Et = " << topoData->Et()
                  << ", ExTC = " << topoData->ExTC()
                  << ", EyTC = " << topoData->EyTC()
                  );

   
   TCS::MetTOB met( topoData->Ex(), topoData->Ey(), topoData->Et() );
   inputEvent.setMET( met );

   return StatusCode::SUCCESS;
}
