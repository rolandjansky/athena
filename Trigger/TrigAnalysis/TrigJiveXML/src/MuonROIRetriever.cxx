/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/MuonROIRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  MuonROIRetriever::MuonROIRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("MuonROI")
  {

    declareInterface<IDataRetriever>(this);

    declareProperty("StoreGateKey", m_sgKey = "LVL1_ROI", 
            "Storegate key for ROIs incl MuonROIs");
  }

  //--------------------------------------------------------------------------

  StatusCode MuonROIRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    const LVL1_ROI * roi;
    
    if ( evtStore()->retrieve(roi,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No MuonROI found in SG at "
          << m_sgKey << endreq;
      return StatusCode::SUCCESS;
    } 
    int noRois = roi->getMuonROIs().size();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieving MuonROIs with size " << noRois << endreq;
 
    DataVect phi; phi.reserve(noRois);
    DataVect eta; eta.reserve(noRois);
    DataVect roiWord; roiWord.reserve(noRois);
    DataVect thrNumber; thrNumber.reserve(noRois);
    DataVect thrName; thrName.reserve(noRois);
    DataVect thrValue; thrValue.reserve(noRois);
    DataVect energy; energy.reserve(noRois);

    LVL1_ROI::muons_type::const_iterator itMU  = (roi->getMuonROIs()).begin();
    LVL1_ROI::muons_type::const_iterator itMUe = (roi->getMuonROIs()).end();

    for (; itMU != itMUe; ++itMU){
        phi.push_back(DataType( itMU->phi()));
        eta.push_back(DataType( itMU->eta()));
        roiWord.push_back(DataType( itMU->getROIWord()));
        thrNumber.push_back(DataType( itMU->getThrNumber()));

     // prevent empty threshold name list
	std::string m_thrName = itMU->getThrName();
        if (m_thrName ==""){ m_thrName = "empty"; };
        thrName.push_back(DataType( m_thrName ));

        thrValue.push_back(DataType( itMU->getThrValue()));
        energy.push_back(DataType( itMU->getThrValue()/CLHEP::GeV));
    }

    DataMap myDataMap;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["roiWord"] = roiWord;
    myDataMap["thrNumber"] = thrNumber;
    myDataMap["thrName"] = thrName;
    myDataMap["thrValue"] = thrValue;
    myDataMap["energy"] = energy;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size() << endreq;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
