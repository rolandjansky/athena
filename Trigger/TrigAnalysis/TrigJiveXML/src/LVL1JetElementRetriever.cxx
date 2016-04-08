/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJiveXML/LVL1JetElementRetriever.h"

#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigT1CaloEvent/JetElement.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  LVL1JetElementRetriever::LVL1JetElementRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_typeName("LVL1JetElement")
  {

    declareInterface<IDataRetriever>(this);

    declareProperty("StoreGateKey", m_sgKey = "JetElements", 
            "Storegate key for Level-1 Jet Elements");
  }

  //--------------------------------------------------------------------------

  StatusCode LVL1JetElementRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    // taken from TrigT1Calo/Tester.cxx
    
    typedef DataVector<LVL1::JetElement> t_JECollection ;
    const t_JECollection* JEVector;
    
    if ( evtStore()->retrieve(JEVector,m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No Level-1 Jet Elements found in SG at "
          << m_sgKey << endreq;
      return StatusCode::SUCCESS;
    } 
    int noJE = JEVector->size();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Retrieving Level-1 Jet Elements with size " << noJE << endreq;

    DataVect phi; phi.reserve(noJE);
    DataVect eta; eta.reserve(noJE);
    DataVect energy; energy.reserve(noJE);

    t_JECollection::const_iterator it ;

    for( it  = JEVector->begin(); it < JEVector->end(); ++it ){
        //  	 log << MSG::DEBUG <<" Lvl1 JE coords ("<<(*it)->phi()<<", "<<(*it)->eta()
	//	     << " and energies : "<<(*it)->energy()<<endreq;
        eta.push_back(DataType((*it)->eta()));
        phi.push_back(DataType((*it)->phi()));
        energy.push_back(DataType((*it)->energy()));
    }//end JE iterator JE loop

    DataMap myDataMap;
    myDataMap["energy"] = energy;
    myDataMap["phi"] = phi;
    myDataMap["eta"] = eta;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< phi.size() << endreq;

    ////forward data to formating tool
    //return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
    ////temporary patch until Atlantis copes with sgKeys in
    ////add datatypes: no sgKey written
    std::string emptyStr = "";
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &myDataMap);
  }
}
