/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/CscPrepDataRetriever.h"

#include "MuonJiveXML/MuonFullIDHelper.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

   CscPrepDataRetriever::CscPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent):
     AthAlgTool(type,name,parent),
     m_typeName("CSCD")
   {

     declareInterface<IDataRetriever>(this);
     
     declareProperty("StoreGateKey", m_sgKey = "CSC_Clusters", "name of the CscPrepDataContainer");
   }

  //--------------------------------------------------------------------------

  StatusCode CscPrepDataRetriever::initialize(){
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initializing retriever for " << dataTypeName() << endmsg; 
    
    StatusCode sc=detStore()->retrieve(m_cscIdHelper);
    if (sc.isFailure())
      {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve CscIdHelper!" << endmsg;
        return StatusCode::FAILURE;
      }

    return StatusCode::SUCCESS;
  }        
                
  //--------------------------------------------------------------------------

  StatusCode CscPrepDataRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() << endmsg; 

    const Muon::CscPrepDataContainer *cscContainer;
    if ( evtStore()->retrieve(cscContainer, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Muon::CscPrepDataContainer '" << m_sgKey << "' was not retrieved." << endmsg;
      return StatusCode::SUCCESS;
    }

    int ndata = 0;
    Muon::CscPrepDataContainer::const_iterator containerIt;
    for (containerIt=cscContainer->begin(); containerIt!=cscContainer->end(); ++containerIt) {
               ndata += (*containerIt)->size();
    }

    //Make the vectors to contain the information and reserve space accordingly
    DataVect x; x.reserve(ndata);
    DataVect y; y.reserve(ndata);
    DataVect z; z.reserve(ndata);
    DataVect lengthVec; lengthVec.reserve(ndata);
    DataVect chargeVec; chargeVec.reserve(ndata);
    DataVect idVec; idVec.reserve(ndata);
    DataVect identifierVec; identifierVec.reserve(ndata);
    DataVect barcode; barcode.reserve(ndata);

    for (containerIt=cscContainer->begin(); containerIt!=cscContainer->end(); ++containerIt) {
      const Muon::CscPrepDataCollection *cscCollection = *containerIt;

      Muon::CscPrepDataCollection::const_iterator collectionIt;
      for (collectionIt=cscCollection->begin(); collectionIt!=cscCollection->end(); ++collectionIt) {

        const Muon::CscPrepData *data = *collectionIt;
        const MuonGM::CscReadoutElement *element = data->detectorElement();
        Identifier id = data->identify();
          
        if (!element) {
           if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No MuonGM::CscReadoutElement for hit " << id << endmsg;
          continue;
        }

	Amg::Vector3D globalPos = element->stripPos(id);
        double length = element->stripLength(id);
        int charge = data->charge();

        x.push_back(DataType(globalPos.x()/CLHEP::cm));
        y.push_back(DataType(globalPos.y()/CLHEP::cm));
        z.push_back(DataType(globalPos.z()/CLHEP::cm));
        lengthVec.push_back(DataType(length/CLHEP::cm));
        chargeVec.push_back(DataType(charge));
        identifierVec.push_back(DataType(MuonFullIDHelper::getFullID(id, m_cscIdHelper)));
        idVec.push_back(DataType( id.get_compact() ));
        barcode.push_back(DataType(0));
      }
    }

    DataMap myDataMap;
    myDataMap["x"] = x;
    myDataMap["y"] = y;
    myDataMap["z"] = z;
    myDataMap["length"] = lengthVec;
    myDataMap["charge"] = chargeVec;
    myDataMap["identifier"] = identifierVec;
    myDataMap["id"] = idVec;
    myDataMap["barcode"] = barcode;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< x.size() << endmsg;

    //forward data to formating tool
    //return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
    //// Atlantis can't deal with SGkey in xml output in CSCD (freezes)
    //// So not output SGKey for now. jpt 20Aug09
    std::string emptyStr="";
    return FormatTool->AddToEvent(dataTypeName(), emptyStr, &myDataMap);
  }

  //--------------------------------------------------------------------------
}
