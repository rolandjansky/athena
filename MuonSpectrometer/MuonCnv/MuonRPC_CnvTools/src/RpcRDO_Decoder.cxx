/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RpcRDO_Decoder.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "MuonDigitContainer/RpcDigit.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonRDO/RpcFiredChannel.h"

Muon::RpcRDO_Decoder::RpcRDO_Decoder
( const std::string& type, const std::string& name,const IInterface* parent ) :
  AthAlgTool(type,name,parent)
{
  declareInterface<IRPC_RDO_Decoder>( this );
}

StatusCode Muon::RpcRDO_Decoder::initialize()
{
  ATH_CHECK(AthAlgTool::initialize());
  ATH_MSG_DEBUG ("initialize");
        
  // get RPC cablingSvc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  StatusCode sc = service("RPCcablingServerSvc", RpcCabGet);
  if (sc.isFailure()) {
    msg (MSG::FATAL) << "Could not get RPCcablingServerSvc !" << endmsg;
    return StatusCode::FAILURE;
  }
  else msg (MSG::VERBOSE) << " RPCcablingServerSvc retrieved" << endmsg;
  
  sc = RpcCabGet->giveCabling(m_cablingSvc);
  if (sc.isFailure()) {
    msg (MSG::FATAL) << "Could not get RPCcablingSvc from the Server !" << endmsg;
    m_cablingSvc = nullptr;
    return StatusCode::FAILURE;
  } 
  else {
    msg (MSG::VERBOSE) << " RPCcablingSvc obtained " << endmsg;
  }

  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}



std::vector<RpcDigit*>* Muon::RpcRDO_Decoder::getDigit(const RpcFiredChannel* fChan,
							uint16_t& sectorID, uint16_t& padId,
							uint16_t& cmaId, const RpcCablingCondData* /*rpcCab*/) const
{ 
  std::vector<RpcDigit*>* rpcDigitVec = new std::vector<RpcDigit*>;
  
  uint16_t side    = (sectorID < 32) ? 0:1;
  uint16_t slogic  = sectorID - side*32;
  uint16_t ijk     = fChan->ijk();
  uint16_t channel = fChan->channel();

  // Sept 10 2014, M. Corradi changed shift to be consistent with 
  // BCzero=3 and ROOffset=2 in
  // Trigger/TrigT1/TrigT1RPChardware/src/Matrix.cxx
  // need to find a better way than hard-coding
  float time = (fChan->bcid()-3)*25 +(fChan->time()+0.5-2)*3.125; 
  
  // skip the trigger hits
  if (ijk==7) {
    return rpcDigitVec;
  }

  // Get the list of offline channels corresponding to the 
  // online identifier
  std::list<Identifier> idList = m_cablingSvc->give_strip_id(side, slogic, padId, cmaId, ijk, channel);
 
  std::list<Identifier>::const_iterator it_list;
  rpcDigitVec->reserve(idList.size());
  for (it_list=idList.begin() ; it_list != idList.end() ; ++it_list) {
    
    // and add the digit to the collection
    Identifier stripOfflineId = *it_list;

    RpcDigit* rpcDigit = new RpcDigit(stripOfflineId, time);
    rpcDigitVec->push_back(rpcDigit);

  }


  return rpcDigitVec;
}


std::vector<Identifier>* Muon::RpcRDO_Decoder::getOfflineData(const RpcFiredChannel* fChan,
							       uint16_t& sectorID, uint16_t& padId,
							       uint16_t& cmaId, double& time, const RpcCablingCondData* /*rpcCab*/) const
{ 
  std::vector<Identifier>* rpcIdVec = new std::vector<Identifier>;
  
  uint16_t side    = (sectorID < 32) ? 0:1;
  uint16_t slogic  = sectorID - side*32;
  uint16_t ijk     = fChan->ijk();
  uint16_t channel = fChan->channel();

  // Sept 10 2014, M. Corradi changed shift to be consistent with 
  // BCzero=3 and ROOffset=2 in
  // Trigger/TrigT1/TrigT1RPChardware/src/Matrix.cxx
  // need to find a better way than hard-coding
  time = (fChan->bcid()-3)*25 +(fChan->time()+0.5-2)*3.125;  
  
  // skip the trigger hits
  if (ijk==7) {
    return rpcIdVec;
  } 

  // Get the list of offline channels corresponding to the 
  // online identifier
  std::list<Identifier> idList = m_cablingSvc->give_strip_id(side, slogic, padId, cmaId, ijk, channel);
 
  rpcIdVec->assign(idList.begin(), idList.end());


  return rpcIdVec;
}


