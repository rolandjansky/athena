/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonJiveXML/TrigRpcDataRetriever.h"

#include <vector>
#include <list>

#include "MuonJiveXML/MuonFullIDHelper.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"

namespace JiveXML {

  //--------------------------------------------------------------------------

  TrigRpcDataRetriever::TrigRpcDataRetriever(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent), 
    m_typeName("RPC") // same datatype name as RPC ! Must not be run together
  {
    declareInterface<IDataRetriever>(this);
    declareProperty("StoreGateKey",   m_sgKey = "RPCPAD", "StoreGate key for the RPC RDO container" );
  }

  //--------------------------------------------------------------------------

  StatusCode TrigRpcDataRetriever::initialize(){

    const IRPCcablingServerSvc* RpcCabGet;
    ATH_CHECK(service("RPCcablingServerSvc", RpcCabGet));
    ATH_CHECK(RpcCabGet->giveCabling(m_rpcCabling));

    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_readKey.initialize());
    ATH_CHECK(m_rpcDecoder.retrieve());
    ATH_CHECK(m_DetectorManagerKey.initialize());
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

  StatusCode TrigRpcDataRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    //be verbose
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Retrieving " << dataTypeName());

    // retrieve the collection of RDO
    const RpcPadContainer* rdoContainer;
    if ( evtStore()->retrieve( rdoContainer, m_sgKey).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Muon::RpcPadContainer '" << m_sgKey << "' was not retrieved.");
      return StatusCode::SUCCESS;
    }
    else{
      if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG("Muon::RpcPadContainer retrieved");
        ATH_MSG_DEBUG(rdoContainer->size() << " RPC RDO collections.");
      }
    }

    const DataHandle<RpcPad> firstRdoColl;
    const DataHandle<RpcPad> lastRdoColl;
    if ( evtStore()->retrieve(firstRdoColl,lastRdoColl).isFailure() ) {
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("RpcPad collections not found");
      return StatusCode::SUCCESS;
    }
    else{
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("RpcPad collections retrieved");
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

    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
    if(!MuonDetMgr){
      ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
      return StatusCode::FAILURE; 
    } 

 
    //loop on pad
    SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey, Gaudi::Hive::currentContext()};
    const RpcCablingCondData* rpcCabling{*readHandle};

    const DataHandle<RpcPad> itColl(firstRdoColl);
    for (; itColl!=lastRdoColl; ++itColl){
      if ( itColl->size() == 0 ) continue;

      ipad++;
      if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Ipad " << ipad);
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
          if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("RpcFiredChannel: " <<
                                    " sectorId " <<  sectorId           <<
                                    " padId "    <<  padId              <<
                                    " cmId "     <<  cmaId              <<
                                    " bcid "     <<  rpcChan->bcid()    <<
                                    " time "     <<  rpcChan->time()    <<
                                    " ijk  "     <<  rpcChan->ijk()     <<
                                    " ch   "     <<  rpcChan->channel());
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
                  std::vector<Identifier>* digitVec = m_rpcDecoder->getOfflineData(rpcChan, sectorId, padId, cmaId, time, rpcCabling);
                  // Loop on the digits corresponding to the fired channel
                  ///// following still unused
                  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Number of digits: "   << digitVec->size());

                  // transform the pad sectorId according to the cabling convention
                  uint16_t side                   = (sectorId<32) ? 0 : 1;
                  uint16_t sl                     = sectorId-side*32     ;
                  std::list<Identifier> stripList = m_rpcCabling->give_strip_id(side,sl,padId,cmaId,rpcChan->ijk(),rpcChan->channel());
                  std::list<Identifier>::const_iterator it_list;
                  for (it_list=stripList.begin() ; it_list != stripList.end() ; ++it_list) {
                    Identifier stripOfflineId = *it_list;
                    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(" cablingId " << m_idHelperSvc->rpcIdHelper().show_to_string(stripOfflineId));
  
                    const MuonGM::RpcReadoutElement* element = MuonDetMgr->getRpcReadoutElement(stripOfflineId);
                    char ChID[100];
                    snprintf(ChID, 100, "SL%d-Pad%d-CM%d-ijk%d-ch%d-time%d",sectorId,padId,cmaId,
                            rpcChan->ijk(),rpcChan->channel(),8*rpcChan->bcid()+rpcChan->time());  
                  
		    Amg::Vector3D globalPos = element->stripPos(stripOfflineId);
                    int measuresPhi = m_idHelperSvc->rpcIdHelper().measuresPhi(stripOfflineId);
                    double stripLength = element->StripLength(measuresPhi);
                    double stripWidth = element->StripWidth(measuresPhi);

                    x.push_back(DataType(globalPos.x()/CLHEP::cm));
                    y.push_back(DataType(globalPos.y()/CLHEP::cm));
                    z.push_back(DataType(globalPos.z()/CLHEP::cm));
                    lengthVec.push_back(DataType(stripLength/CLHEP::cm));
                    widthVec.push_back(DataType(stripWidth/CLHEP::cm));
                    identifierVec.push_back(DataType(ChID));
		    idVec.push_back(DataType( m_idHelperSvc->rpcIdHelper().show_to_string(stripOfflineId) )); 
                    barcode.push_back(DataType(0));        
                  }
		  delete digitVec;
                }
                idata1++; 

                if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("RpcFiredChannel1:"    <<
                                          " sectorId1 " <<  sectorId            <<
                                          " padId1 "    <<  padId               <<
                                          " cmId1 "     <<  cmaId               <<
                                          " bcid1 "     <<  rpcChan1->bcid()    <<
                                          " time1 "     <<  rpcChan1->time()    <<
                                          " ijk1  "     <<  rpcChan1->ijk()     <<
                                          " ch1   "     <<  rpcChan1->channel());
               
                //write confirm hits
                std::vector<Identifier>* digitVec1 = m_rpcDecoder->getOfflineData(rpcChan1, sectorId, padId, cmaId, time1, rpcCabling);
                // Loop on the digits corresponding to the fired channel

                if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Number of digits: "   << digitVec1->size());

                 // transform the pad sectorId according to the cabling convention
                uint16_t side                   = (sectorId<32) ? 0 : 1;
                uint16_t sl                     = sectorId-side*32     ;
                std::list<Identifier> stripList1 = m_rpcCabling->give_strip_id(side,sl,padId,cmaId,rpcChan1->ijk(),rpcChan1->channel());
                std::list<Identifier>::const_iterator it_list1;
                for (it_list1=stripList1.begin() ; it_list1 != stripList1.end() ; ++it_list1) {
                  Identifier stripOfflineId1 = *it_list1;
                  if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(" cablingId1 " << m_idHelperSvc->rpcIdHelper().show_to_string(stripOfflineId1));
  
                  const MuonGM::RpcReadoutElement* element1 = MuonDetMgr->getRpcReadoutElement(stripOfflineId1);

                  char ChID1[100];
                  snprintf(ChID1,100, "SL%d-Pad%d-CM%d-ijk%d-ch%d-time%d",sectorId,padId,cmaId,
                          rpcChan1->ijk(),rpcChan1->channel(),8*rpcChan1->bcid()+rpcChan1->time());  

		  Amg::Vector3D globalPos1 = element1->stripPos(stripOfflineId1);
                  int measuresPhi1      = m_idHelperSvc->rpcIdHelper().measuresPhi(stripOfflineId1);
                  double stripLength1   = element1->StripLength(measuresPhi1);
                  double stripWidth1    = element1->StripWidth(measuresPhi1);
 
                  x.push_back(DataType(globalPos1.x()/CLHEP::cm));
                  y.push_back(DataType(globalPos1.y()/CLHEP::cm));
                  z.push_back(DataType(globalPos1.z()/CLHEP::cm));
                  lengthVec.push_back(DataType(stripLength1/CLHEP::cm));
                  widthVec.push_back(DataType(stripWidth1/CLHEP::cm));
                  identifierVec.push_back(DataType(ChID1));
		  idVec.push_back(DataType( m_idHelperSvc->rpcIdHelper().show_to_string(stripOfflineId1) ));
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
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG(dataTypeName() << ": "<< x.size());

    //forward data to formating tool
    return FormatTool->AddToEvent(dataTypeName(), m_sgKey, &myDataMap);  
  }
}
