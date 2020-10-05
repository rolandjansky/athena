/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/RpcPrepDataRetriever.h"

#include "MuonJiveXML/MuonFullIDHelper.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  RpcPrepDataRetriever::RpcPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent):
      AthAlgTool(type, name, parent ),
      m_typeName("RPC")
  {

    declareInterface<IDataRetriever>(this);
   
    declareProperty("StoreGateKey", m_sgKey = "RPC_Measurements", "Storegate key for RPC PredData container");
  }
  
  //--------------------------------------------------------------------------

  StatusCode RpcPrepDataRetriever::initialize(){

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Initializing retriever for " << dataTypeName()); 

    ATH_CHECK( m_idHelperSvc.retrieve() );

    return StatusCode::SUCCESS;
  }        
                
  //--------------------------------------------------------------------------

  StatusCode RpcPrepDataRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Retrieving " << dataTypeName()); 

    const Muon::RpcPrepDataContainer *rpcContainer=nullptr;
    if ( evtStore()->retrieve(rpcContainer, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Muon::RpcPrepDataContainer '" << m_sgKey << "' was not retrieved.");
      return StatusCode::SUCCESS;
    }

    int ndata = 0;
    Muon::RpcPrepDataContainer::const_iterator containerIt;
    for (containerIt=rpcContainer->begin(); containerIt!=rpcContainer->end(); ++containerIt) {
               ndata += (*containerIt)->size();
    }

    //Make the vectors to contain the information and reserve space accordingly
    DataVect x; x.reserve(ndata);
    DataVect y; y.reserve(ndata);
    DataVect z; z.reserve(ndata);
    DataVect lengthVec; lengthVec.reserve(ndata);
    DataVect widthVec; widthVec.reserve(ndata);
    DataVect idVec; idVec.reserve(ndata);
    DataVect identifierVec; identifierVec.reserve(ndata);
    DataVect barcode; barcode.reserve(ndata);

    for (containerIt=rpcContainer->begin(); containerIt!=rpcContainer->end(); ++containerIt) {
      const Muon::RpcPrepDataCollection *rpcCollection = *containerIt;

      Muon::RpcPrepDataCollection::const_iterator collectionIt;
      for (collectionIt=rpcCollection->begin(); collectionIt!=rpcCollection->end(); ++collectionIt) {

        const Muon::RpcPrepData *data = *collectionIt;
        const MuonGM::RpcReadoutElement *element = data->detectorElement();
        Identifier id = data->identify();
  
        if (!element) {
          if (msgLvl(MSG::WARNING)) ATH_MSG_WARNING("No MuonGM::RpcReadoutElement for hit " << id);
          continue;
        }
  
	Amg::Vector3D globalPos = element->stripPos(id);  
        int measuresPhi = m_idHelperSvc->rpcIdHelper().measuresPhi(id);
        double stripLength = element->StripLength(measuresPhi);
        double stripWidth = element->StripWidth(measuresPhi);

        x.push_back(DataType(globalPos.x()/CLHEP::cm));
        y.push_back(DataType(globalPos.y()/CLHEP::cm));
        z.push_back(DataType(globalPos.z()/CLHEP::cm));
        lengthVec.push_back(DataType(stripLength/CLHEP::cm));
        widthVec.push_back(DataType(stripWidth/CLHEP::cm));
        identifierVec.push_back(DataType(MuonFullIDHelper::getFullID(id, m_idHelperSvc->rpcIdHelper())));
        idVec.push_back(DataType( id.get_compact() ));
        barcode.push_back(DataType(0));
      }
    }      

    DataMap myDataMap;
    myDataMap["x"] = x;
    myDataMap["y"] = y;
    myDataMap["z"] = z;
    myDataMap["length"] = lengthVec;
    myDataMap["width"] = widthVec;
    myDataMap["identifier"] = identifierVec;
    myDataMap["id"] = idVec;
    myDataMap["barcode"] = barcode;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(dataTypeName() << ": "<< x.size());

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);
  }
}
