/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonROIRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"


namespace JiveXML {

  //--------------------------------------------------------------------------

  TrigMuonROIRetriever::TrigMuonROIRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("TrigMuonROI")
  {

    declareInterface<IDataRetriever>(this);

    declareProperty("StoreGateKey", m_sgKey = "LVL1_ROI", "Storegate key for LVL1 Muon RoIs");
  }

  //--------------------------------------------------------------------------

  StatusCode TrigMuonROIRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() << endmsg; 


    const LVL1_ROI * roi;
    
    if ( evtStore()->retrieve(roi,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No MuonROI (LVL1_ROI) found." << endmsg;
      return StatusCode::SUCCESS;
    } 

    int nRoIs = roi->getMuonROIs().size();

    DataVect phi; phi.reserve(nRoIs);
    DataVect eta; eta.reserve(nRoIs);
    DataVect energy; energy.reserve(nRoIs);
    DataVect roiWord; roiWord.reserve(nRoIs);
    DataVect thrNumber; thrNumber.reserve(nRoIs);
    DataVect thrName; thrName.reserve(nRoIs);
    DataVect thrValue; thrValue.reserve(nRoIs);

    // Class: PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent/Muon_ROI.h
    // Access as in: PhysicsAnalysis/AnalysisCommon/AnalysisTest/ReadTrigger
    // Tested with: AnalysisTest/share/AODTriggerRead_topOptions.py

    LVL1_ROI::muons_type::const_iterator itMU  = (roi->getMuonROIs()).begin();
    LVL1_ROI::muons_type::const_iterator itMUe = (roi->getMuonROIs()).end();

    for (; itMU != itMUe; ++itMU){
      phi.push_back(DataType( itMU->getPhi()));
      eta.push_back(DataType( itMU->getEta()));

      roiWord.push_back(DataType( itMU->getROIWord()));
      thrNumber.push_back(DataType( itMU->getThrNumber()));

      // prevent empty threshold name list
      std::string str_thrName = itMU->getThrName();
      if (str_thrName ==""){ str_thrName = "empty"; };
      thrName.push_back(DataType( str_thrName ));
      thrValue.push_back(DataType( itMU->getThrValue()));

      // 'energy' used, as all other ROIs have it
      // Threshold value used here for 'energy'
      energy.push_back(DataType( itMU->getThrValue()/CLHEP::GeV ) );
    }

    DataMap myDataMap;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;
    myDataMap["energy"] = energy;
    myDataMap["roiWord"] = roiWord;
    myDataMap["thrNumber"] = thrNumber;
    myDataMap["thrName"] = thrName;
    myDataMap["thrValue"] = thrValue;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size() << endmsg;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
