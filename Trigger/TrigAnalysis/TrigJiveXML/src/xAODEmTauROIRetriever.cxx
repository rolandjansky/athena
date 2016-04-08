/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/xAODEmTauROIRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

//#include "AnalysisTriggerEvent/LVL1_ROI.h"

//#include "TrigT1CaloEvent/CPMRoI.h"
//#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "xAODTrigger/EmTauRoIContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  xAODEmTauROIRetriever::xAODEmTauROIRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent), m_typeName("EmTauROI")
  {

    declareInterface<IDataRetriever>(this);
  }

  //--------------------------------------------------------------------------

  StatusCode xAODEmTauROIRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

  DataVect phi;
  DataVect eta; 
  DataVect energy; 
  DataVect energyEM;
  DataVect energyTAU;
  DataVect roiWord; 
  DataVect thrPattern; 

  //// which of those two option is working:

  const xAOD::EmTauRoIContainer* emTauRoIs = 0; 

    //xAOD::EmTauRoIContainer* emTauRoIs = 0;

    // L1JetObject -not- available
    m_sgKey = "LVL1EmTauRoIs"; 
    if ( evtStore()->retrieve(emTauRoIs,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No LVL1EmTauROIs found in SG " << endreq;
      return StatusCode::SUCCESS;
    } 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Found LVL1EmTauROIs in SG ! " << endreq;

    xAOD::EmTauRoIContainer::const_iterator itEM  = emTauRoIs->begin();
    xAOD::EmTauRoIContainer::const_iterator itEMe = emTauRoIs->end();

    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "xAOD EmTauROIs retrieved from StoreGate with size: " << (emTauRoIs->size()) <<endreq;

    int counter = 0;
    for (; itEM != itEMe; ++itEM)
        {
        phi.push_back(DataType( (*itEM)->phi()) );
	eta.push_back(DataType( (*itEM)->eta()) );

       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "xAOD EmTauRoI #" << counter++ 
          << ", eta: " << (*itEM)->eta() << ", phi: " << (*itEM)->phi() << endreq;

// Placeholders ! No direct access to those in Run-2
// Reference:
//    Event/xAOD/xAODTrigger/trunk/Root/EmTauRoI_v2.cxx
// jpt 9Jan15:
        energy.push_back(DataType( 0. ));
        energyEM.push_back(DataType( 0. ));
        energyTAU.push_back(DataType( 0. ));
        roiWord.push_back(DataType( 0. ));
	thrPattern.push_back(DataType( 0. ));
      }

    DataMap myDataMap;
    myDataMap["energy"] = energy;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["energy"] = energy;
    myDataMap["energyEM"] = energyEM;
    myDataMap["energyTAU"] = energyTAU;
    myDataMap["roiWord"] = roiWord;
    myDataMap["thrPattern"] = thrPattern;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size()
					    << " from: " << m_sgKey << endreq;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
