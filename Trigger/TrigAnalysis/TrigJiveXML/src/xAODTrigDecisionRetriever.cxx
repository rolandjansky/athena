/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/xAODTrigDecisionRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

// references:
//   Event/xAOD/xAODTrigger/trunk/Root/TrigDecision_v1.cxx
//   Event/xAOD/xAODTrigger/trunk/Root/TrigDecisionAuxInfo_v1.cxx
#include "xAODTrigger/TrigDecision.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  xAODTrigDecisionRetriever::xAODTrigDecisionRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent), m_typeName("TrigDecision_xAOD")
  {

    declareInterface<IDataRetriever>(this);
  }

  //--------------------------------------------------------------------------

  StatusCode xAODTrigDecisionRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

  DataVect test;

  const xAOD::TrigDecision* trigDec = 0; 

    m_sgKey = "xTrigDecision"; //sgKey from 20.0.0.2 AOD
    if ( evtStore()->retrieve(trigDec,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No xAOD::xTrigDecision found in SG " << endreq;
      return StatusCode::SUCCESS;
    } 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Found xAOD::xTrigDecision in SG ! " << endreq;

    DataVect smk;
    DataVect bgCode;
    DataVect tav;
    DataVect tap;
    DataVect tbp;

    smk.push_back(DataType( trigDec->smk() ));
/// all those are vectors, ok for DEBUG output, but 
/// need iterators and decoding for valid output !
/// 9Jan15 jpt
/*
    bgCode.push_back(DataType( trigDec->bgCode() ));
    tav.push_back(DataType( trigDec->tav() ));
    tap.push_back(DataType( trigDec->tap() ));
    tbp.push_back(DataType( trigDec->tbp() ));
*/
    DataMap myDataMap;
/*
    myDataMap["smk"] = smk;
    myDataMap["bgCode"] = bgCode;
    myDataMap["tav"] = tav;
    myDataMap["tap"] = tap;
    myDataMap["tbp"] = tbp;
*/
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "
          << " SMK: " << trigDec->smk()
          << " BGCODE: " << trigDec->bgCode()
          << " TAV: " << trigDec->tav()
          << " TAP: " << trigDec->tap()
          << " TBP: " << trigDec->tbp()
          << " from: " << m_sgKey << endreq;

    /// Test only ! No output to file yet.
    //return 0;
    ////forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
