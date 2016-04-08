/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/xAODMuonROIRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

//#include "AnalysisTriggerEvent/LVL1_ROI.h"

//#include "TrigT1CaloEvent/CPMRoI.h"
//#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "xAODTrigger/MuonRoIContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  xAODMuonROIRetriever::xAODMuonROIRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent), m_typeName("MuonROI")
  {

    declareInterface<IDataRetriever>(this);
  }

  //--------------------------------------------------------------------------

  StatusCode xAODMuonROIRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

  //// which of those two option is working:

  const xAOD::MuonRoIContainer* muonROIs = 0; 

    //xAOD::MuonROIContainer* muonROIs = 0;

    // L1JetObject -not- available
    m_sgKey = "LVL1MuonRoIs"; 
    if ( evtStore()->retrieve(muonROIs,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No LVL1MuonROIs found in SG " << endreq;
      return StatusCode::SUCCESS;
    } 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Found LVL1MuonROIs in SG ! " << endreq;

    int noRois = muonROIs->size();

    DataVect phi; phi.reserve(noRois);
    DataVect eta; eta.reserve(noRois);
    DataVect roiWord; roiWord.reserve(noRois);
    DataVect thrNumber; thrNumber.reserve(noRois);
    DataVect thrName; thrName.reserve(noRois);
    DataVect thrValue; thrValue.reserve(noRois);
    DataVect energy; energy.reserve(noRois);

    xAOD::MuonRoIContainer::const_iterator itMU  = muonROIs->begin();
    xAOD::MuonRoIContainer::const_iterator itMUe = muonROIs->end();

    //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "xAOD MuonROIs retrieved from StoreGate with size: " << (MuonROIs->size()) <<endreq;

    int counter = 0;
    for (; itMU != itMUe; ++itMU)
        {
        phi.push_back(DataType( (*itMU)->phi()) );
	eta.push_back(DataType( (*itMU)->eta()) );
//	roiWord.push_back(DataType( (*itMU)->getRoIWord()) );
	roiWord.push_back(DataType( (*itMU)->getRoI()) );
        thrNumber.push_back(DataType( (*itMU)->getThrNumber()));

//	std::string m_thrName = (*itMU)->getThrName();
//        if (m_thrName ==""){ m_thrName = "empty"; };
//        thrName.push_back(DataType( m_thrName ));
//
//        thrValue.push_back(DataType( (*itMU)->getThrValue()));
//        energy.push_back(DataType( (*itMU)->getThrValue()/CLHEP::GeV));

	thrName.push_back(DataType( "n_a" )); // placeholders
        thrValue.push_back(DataType( 1. ));
        energy.push_back(DataType( 1. ));

       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "xAOD MuonROI #" << counter++ 
          << ", eta: " << (*itMU)->eta() << ", phi: " << (*itMU)->phi() << endreq;

/* from old MuonRoI object: 'getThrNumber', 'getThrName' and 'getROIWord' have gone !
    for (; itMU != itMUe; ++itMU){
        roiWord.push_back(DataType( itMU->getROIWord()));
        thrNumber.push_back(DataType( itMU->getThrNumber()));

	std::string m_thrName = itMU->getThrName();
        if (m_thrName ==""){ m_thrName = "empty"; };
        thrName.push_back(DataType( m_thrName ));

        thrValue.push_back(DataType( itMU->getThrValue()));
        energy.push_back(DataType( itMU->getThrValue()/CLHEP::GeV));
    }
*/

      }

    DataMap myDataMap;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["energy"] = energy;
    myDataMap["roiWord"] = roiWord;
    myDataMap["thrNumber"] = thrNumber;
    myDataMap["thrName"] = thrName;
    myDataMap["thrValue"] = thrValue;
    myDataMap["energy"] = energy;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size()
					    << " from: " << m_sgKey << endreq;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
