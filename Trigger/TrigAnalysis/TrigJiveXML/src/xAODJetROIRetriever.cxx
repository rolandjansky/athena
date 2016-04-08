/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/xAODJetROIRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

//#include "AnalysisTriggerEvent/LVL1_ROI.h"

//#include "TrigT1CaloEvent/CPMRoI.h"
//#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "xAODTrigger/JetRoIContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  xAODJetROIRetriever::xAODJetROIRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent), m_typeName("JetROI")
  {

    declareInterface<IDataRetriever>(this);
  }

  //--------------------------------------------------------------------------

  StatusCode xAODJetROIRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

  DataVect phi;
  DataVect eta; 
  DataVect energy; 
  DataVect roiWord; 
  DataVect thrPattern; 

  //// which of those two option is working:

  const xAOD::JetRoIContainer* jetROIs = 0; 

    //xAOD::JetROIContainer* jetROIs = 0;

    // L1JetObject -not- available
    m_sgKey = "LVL1JetRoIs"; 
    if ( evtStore()->retrieve(jetROIs,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No LVL1JetROIs found in SG " << endreq;
      return StatusCode::SUCCESS;
    } 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Found LVL1JetROIs in SG ! " << endreq;

    xAOD::JetRoIContainer::const_iterator itJET  = jetROIs->begin();
    xAOD::JetRoIContainer::const_iterator itJETe = jetROIs->end();

    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "xAOD JetROIs retrieved from StoreGate with size: " << (JetROIs->size()) <<endreq;

    int counter = 0;
    for (; itJET != itJETe; ++itJET)
        {
        phi.push_back(DataType( (*itJET)->phi()) );
	eta.push_back(DataType( (*itJET)->eta()) );

       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "xAOD JetROI #" << counter++ 
          << ", eta: " << (*itJET)->eta() << ", phi: " << (*itJET)->phi() << endreq;

// Placeholders ! No direct access to those in Run-2
// Reference:
//    Event/xAOD/xAODTrigger/trunk/Root/JetRoI_v2.cxx
// jpt 9Jan15:
        energy.push_back(DataType( 0. ));
        roiWord.push_back(DataType( 0. ));
	thrPattern.push_back(DataType( 0. ));

// Run-1: info from Alan Watson: pT is getET8x8();
//        energy.push_back(DataType(  itJET->pt()/1000. ));
//// other options: getET4x4(), getET6x6()
//      energy.push_back(DataType( (*itJET)->getET4x4()/1000. )); 
//      roiWord.push_back(DataType( (*itJET)->getRoI()));
//	thrPattern.push_back(DataType( (*itJET)->getThrPattern()));

      }

    DataMap myDataMap;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["energy"] = energy;
    myDataMap["roiWord"] = roiWord;
    myDataMap["thrPattern"] = thrPattern;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size()
					    << " from: " << m_sgKey << endreq;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
