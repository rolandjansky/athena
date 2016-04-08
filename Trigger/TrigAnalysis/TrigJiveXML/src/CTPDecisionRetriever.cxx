/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/CTPDecisionRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "AnalysisTriggerEvent/CTP_Decision.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  CTPDecisionRetriever::CTPDecisionRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("LVL1Result")
  {
    declareInterface<IDataRetriever>(this);
  }

  //--------------------------------------------------------------------------

  StatusCode CTPDecisionRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    DataVect itemListL1Vec;
    DataVect prescaleListL1Vec;
    DataVect itemListL2Vec;
    DataVect prescaleListL2Vec;
    DataVect itemListEFVec;
    DataVect prescaleListEFVec;
    DataVect passedTrigger;
    DataVect passedL1;
    DataVect passedL2;
    DataVect passedEF;

// placeholders only ! For backwards compatibility.
// Trigger energies moved to TriggerInfoRetriever
    DataVect energySumEt;
    DataVect energyEx;
    DataVect energyEy;
    DataVect energyEtMiss;

    energySumEt.push_back(DataType( -1. ) );
    energyEx.push_back(DataType( -1. ) );
    energyEy.push_back(DataType( -1. ) );
    energyEtMiss.push_back(DataType( -1. ) );

// CTP_Decision has no 'passed' info
    passedTrigger.push_back(DataType( -1. ) );
    passedL1.push_back(DataType( -1. ) );
    passedL2.push_back(DataType( -1. ) );
    passedEF.push_back(DataType( -1. ) );

// end of placeholders

    std::string itemListL1="";
    std::string prescaleListL1="";
    std::string itemListL2="n_a_CTPDecOnly";
    std::string prescaleListL2="0";
    std::string itemListEF="n_a_CTPDecOnly";
    std::string prescaleListEF="0";

//// according to Twiki page LevelOneCentralTriggerData (David Berge)
//// comments taken from there

     const CTP_Decision * ctpDecision;

     // L1JetObject -not- available
     if ( evtStore()->retrieve(ctpDecision,"CTP_Decision").isFailure() ) {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "CTP_Decision retrieval from Storegate failed" << endreq;
       return StatusCode::SUCCESS;
     } 
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found CTP_Decision in StoreGate !" << endreq;

     CTP_Decision::items_type::const_iterator itCTP  = (ctpDecision->getItems()).begin();
     CTP_Decision::items_type::const_iterator itCTPe = (ctpDecision->getItems()).end();

     for (; itCTP != itCTPe; ++itCTP){
//  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " CTP item : " << *itCTP << endreq;
       itemListL1 += "-"+*itCTP;
       prescaleListL1 += "-0";
     }

    itemListL1Vec.push_back( DataType( itemListL1 )); 
    itemListL2Vec.push_back( DataType( itemListL2 ));
    itemListEFVec.push_back( DataType( itemListEF ));
    prescaleListL1Vec.push_back( DataType( prescaleListL1 ));
    prescaleListL2Vec.push_back( DataType( prescaleListL2 ));
    prescaleListEFVec.push_back( DataType( prescaleListEF ));

    DataMap myDataMap;
    myDataMap["ctpItemList"] = itemListL1Vec;
    myDataMap["prescaleListL1"] = prescaleListL1Vec;
    myDataMap["itemListL2"] = itemListL2Vec;
    myDataMap["prescaleListL2"] = prescaleListL2Vec;
    myDataMap["itemListEF"] = itemListEFVec;
    myDataMap["prescaleListEF"] = prescaleListEFVec;
    myDataMap["passedTrigger"] = passedTrigger;
    myDataMap["passedL1"] = passedL1;
    myDataMap["passedL2"] = passedL2;
    myDataMap["passedEF"] = passedEF;
    myDataMap["energySumEt"] = energySumEt;
    myDataMap["energyEx"] = energyEx;
    myDataMap["energyEy"] = energyEy;
    myDataMap["energyEtMiss"] = energyEtMiss;
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< itemListL1Vec.size() << endreq;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), "CTP_Decision", &myDataMap);
  }
}
