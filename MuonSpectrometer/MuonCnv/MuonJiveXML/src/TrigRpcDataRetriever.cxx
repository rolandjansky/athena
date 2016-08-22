/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/TrigRpcDataRetriever.h"

#include<vector>
#include<list>

#include "MuonJiveXML/MuonFullIDHelper.h"


#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  TrigRpcDataRetriever::TrigRpcDataRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent), 
    m_typeName("RPC"), // same datatype name as RPC ! Must not be run together
    m_rpcDecoder("Muon::RpcRDO_Decoder")
  {

    declareInterface<IDataRetriever>(this);

    declareProperty("StoreGateKey",   m_sgKey = "RPCPAD", "StoreGate key for the RPC RDO container" );
  }

  //--------------------------------------------------------------------------

  StatusCode TrigRpcDataRetriever::initialize(){

    StatusCode sc=detStore()->retrieve(m_rpcIdHelper);
    if (sc.isFailure())
      {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve RpcIdHelper!" << endmsg;
        return StatusCode::FAILURE;
      }  

    // get RPC cablingSvc
    const IRPCcablingServerSvc* RpcCabGet;
    sc = service("RPCcablingServerSvc", RpcCabGet);
    if (sc.isFailure()) {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get RPCcablingServerSvc !" << endmsg;
        return StatusCode::FAILURE;
    }

    sc = RpcCabGet->giveCabling(m_rpcCabling);
    if (sc.isFailure()) {
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get RPCcablingSvc from the Server !" << endmsg;
        return StatusCode::FAILURE;
    } 
    else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << MSG::DEBUG << " Found the RPCcablingSvc. " << endmsg;
    }

    /// retrieve the RPC RDO decoder
    if (m_rpcDecoder.retrieve().isFailure()) {
        msg(MSG::ERROR) << "Failed to retrieve " << m_rpcDecoder << endmsg;
        return StatusCode::FAILURE;
    } else
        msg(MSG::INFO) << "Retrieved Tool " << m_rpcDecoder << endmsg;

    // retrieve the muon detector manager
    sc = detStore()->retrieve(m_muonMgr);
    if (sc.isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Can't retrieve the muon detector manager" << endmsg;
     return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

  StatusCode TrigRpcDataRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieving " << dataTypeName() << endmsg; 

    // retrieve the collection of RDO
    const RpcPadContainer* rdoContainer;
    if ( evtStore()->retrieve( rdoContainer, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Muon::RpcPadContainer '" << m_sgKey << "' was not retrieved." << endmsg;
      return StatusCode::SUCCESS;
    }
    else{
      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG) << "Muon::RpcPadContainer retrieved"  << endmsg;
        msg(MSG::DEBUG) << rdoContainer->size() << " RPC RDO collections." << endmsg;
      }
    }

    const DataHandle<RpcPad> firstRdoColl;
    const DataHandle<RpcPad> lastRdoColl;
    if ( evtStore()->retrieve(firstRdoColl,lastRdoColl).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "RpcPad collections not found" << endmsg;
      return StatusCode::SUCCESS;
    }
    else{
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "RpcPad collections retrieved" << endmsg;
    }

    int ndata=0;
    const DataHandle<RpcPad> rdoColl(firstRdoColl);
    for (; rdoColl!=lastRdoColl; ++rdoColl){
      ndata+= rdoColl->size();
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

    int ipad         = 0                  ;
    double time      = 0.                 ;
    double time1     = 0.                 ;

    //loop on pad
    const DataHandle<RpcPad> itColl(firstRdoColl);
    for (; itColl!=lastRdoColl; ++itColl){
      if ( itColl->size() == 0 ) continue;

      ipad++;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ipad " << ipad << endmsg;
      //Get pad online id and sector id
      uint16_t padId         = itColl->onlineId(); 
      uint16_t sectorId  = itColl->sector()  ;
      
      // For each pad, loop on the coincidence matrices
      RpcPad::const_iterator itCM   = itColl->begin();
      RpcPad::const_iterator itCM_e = itColl->end();
      for (; itCM != itCM_e ; ++itCM) {
        // Get CM online Id
        uint16_t cmaId = (*itCM)->onlineId();
       
        // For each CM, loop on the fired channels
        RpcCoinMatrix::const_iterator itD   = (*itCM)->begin();
        RpcCoinMatrix::const_iterator itD_e = (*itCM)->end();
        int idata = 0;
        for (; itD != itD_e ; ++itD) {
          idata++;
          const RpcFiredChannel * rpcChan = (*itD);
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "RpcFiredChannel: " << 
                                    " sectorId " <<  sectorId           <<
                                    " padId "    <<  padId              <<
                                    " cmId "     <<  cmaId              <<
                                    " bcid "     <<  rpcChan->bcid()    <<
                                    " time "     <<  rpcChan->time()    <<
                                    " ijk  "     <<  rpcChan->ijk()     <<
                                    " ch   "     <<  rpcChan->channel() << endmsg; 
          //look for trigger hits       
          if(rpcChan->ijk()==6){
            //found trigger hit. Look for confirm hits
            RpcCoinMatrix::const_iterator itD1   = (*itCM)->begin();
            RpcCoinMatrix::const_iterator itD_e1 = (*itCM)->end();
            int idata1 = 0;
            for (; itD1 != itD_e1 ; ++itD1) {
            
              const RpcFiredChannel * rpcChan1 = (*itD1);
              if( rpcChan1->ijk() > 1 && rpcChan1->ijk() <6){

                //write trigger hit only once
                if(idata1==0){
                  std::vector<Identifier>* digitVec = m_rpcDecoder->getOfflineData(rpcChan, sectorId, padId, cmaId, time);
                  // Loop on the digits corresponding to the fired channel
                  ///// following still unused
                  //std::vector<Identifier>::iterator itVec   = digitVec->begin();
                  //std::vector<Identifier>::iterator itVec_e = digitVec->end();

                  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of digits: "   << digitVec->size()<< endmsg;

                  // transform the pad sectorId according to the cabling convention
                  uint16_t side                   = (sectorId<32) ? 0 : 1;
                  uint16_t sl                     = sectorId-side*32     ;
                  std::list<Identifier> stripList = m_rpcCabling->give_strip_id(side,sl,padId,cmaId,rpcChan->ijk(),rpcChan->channel());
                  std::list<Identifier>::const_iterator it_list;
                  for (it_list=stripList.begin() ; it_list != stripList.end() ; ++it_list) {
                    Identifier stripOfflineId = *it_list;
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " cablingId " << m_rpcIdHelper->show_to_string(stripOfflineId)<< endmsg;
  
                    const MuonGM::RpcReadoutElement* element = m_muonMgr->getRpcReadoutElement(stripOfflineId);
                    char ChID[100];
                    snprintf(ChID, 100, "SL%d-Pad%d-CM%d-ijk%d-ch%d-time%d",sectorId,padId,cmaId,
                            rpcChan->ijk(),rpcChan->channel(),8*rpcChan->bcid()+rpcChan->time());  
                  
		    //                    HepGeom::Point3D<double> globalPos = element->stripPos(stripOfflineId);
		    Amg::Vector3D globalPos = element->stripPos(stripOfflineId);
                    int measuresPhi = m_rpcIdHelper->measuresPhi(stripOfflineId);
                    double stripLength = element->StripLength(measuresPhi);
                    double stripWidth = element->StripWidth(measuresPhi);

                    x.push_back(DataType(globalPos.x()/CLHEP::cm));
                    y.push_back(DataType(globalPos.y()/CLHEP::cm));
                    z.push_back(DataType(globalPos.z()/CLHEP::cm));
                    lengthVec.push_back(DataType(stripLength/CLHEP::cm));
                    widthVec.push_back(DataType(stripWidth/CLHEP::cm));
                    identifierVec.push_back(DataType(ChID));
		    idVec.push_back(DataType( m_rpcIdHelper->show_to_string(stripOfflineId) )); 
                    barcode.push_back(DataType(0));        
                  }
		  delete digitVec;
                }
                idata1++; 

                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "RpcFiredChannel1:"    << 
                                          " sectorId1 " <<  sectorId            <<
                                          " padId1 "    <<  padId               <<
                                          " cmId1 "     <<  cmaId               <<
                                          " bcid1 "     <<  rpcChan1->bcid()    <<
                                          " time1 "     <<  rpcChan1->time()    <<
                                          " ijk1  "     <<  rpcChan1->ijk()     <<
                                          " ch1   "     <<  rpcChan1->channel() << endmsg;
               
                //write confirm hits
                std::vector<Identifier>* digitVec1 = m_rpcDecoder->getOfflineData(rpcChan1, sectorId, padId, cmaId, time1);
                // Loop on the digits corresponding to the fired channel
                //////// following still unused
                // std::vector<Identifier>::iterator itVec1   = digitVec1->begin();
                // std::vector<Identifier>::iterator itVec_e1 = digitVec1->end();

                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of digits: "   << digitVec1->size()<< endmsg;

                 // transform the pad sectorId according to the cabling convention
                uint16_t side                   = (sectorId<32) ? 0 : 1;
                uint16_t sl                     = sectorId-side*32     ;
                std::list<Identifier> stripList1 = m_rpcCabling->give_strip_id(side,sl,padId,cmaId,rpcChan1->ijk(),rpcChan1->channel());
                std::list<Identifier>::const_iterator it_list1;
                for (it_list1=stripList1.begin() ; it_list1 != stripList1.end() ; ++it_list1) {
                  Identifier stripOfflineId1 = *it_list1;
                  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " cablingId1 " << m_rpcIdHelper->show_to_string(stripOfflineId1)<< endmsg;
  
                  const MuonGM::RpcReadoutElement* element1 = m_muonMgr->getRpcReadoutElement(stripOfflineId1);

                  char ChID1[100];
                  snprintf(ChID1,100, "SL%d-Pad%d-CM%d-ijk%d-ch%d-time%d",sectorId,padId,cmaId,
                          rpcChan1->ijk(),rpcChan1->channel(),8*rpcChan1->bcid()+rpcChan1->time());  

		  //                  HepGeom::Point3D<double> globalPos1 = element1->stripPos(stripOfflineId1);
		  Amg::Vector3D globalPos1 = element1->stripPos(stripOfflineId1);
                  int measuresPhi1      = m_rpcIdHelper->measuresPhi(stripOfflineId1);
                  double stripLength1   = element1->StripLength(measuresPhi1);
                  double stripWidth1    = element1->StripWidth(measuresPhi1);
 
                  x.push_back(DataType(globalPos1.x()/CLHEP::cm));
                  y.push_back(DataType(globalPos1.y()/CLHEP::cm));
                  z.push_back(DataType(globalPos1.z()/CLHEP::cm));
                  lengthVec.push_back(DataType(stripLength1/CLHEP::cm));
                  widthVec.push_back(DataType(stripWidth1/CLHEP::cm));
                  identifierVec.push_back(DataType(ChID1));
		  idVec.push_back(DataType( m_rpcIdHelper->show_to_string(stripOfflineId1) ));
                  barcode.push_back(DataType(0));        
                }
               delete digitVec1;
              }//write hits
            }//look for confirm
          }//found trigger hit                

        }//End channels
      }//End CM        
    }//End if 
   
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
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << ": "<< x.size() << endmsg;

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);  
  }
}
