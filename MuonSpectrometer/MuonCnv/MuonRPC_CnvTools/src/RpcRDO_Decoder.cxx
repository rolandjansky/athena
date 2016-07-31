/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonDigitContainer/RpcDigit.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonRDO/RpcFiredChannel.h"

#include "RpcRDO_Decoder.h"



Muon::RpcRDO_Decoder::RpcRDO_Decoder
( const std::string& type, const std::string& name,const IInterface* parent )
  :  AthAlgTool(type,name,parent),
  m_rpcIdHelper(0),
  m_cablingSvc(0)
{
  declareInterface<IRPC_RDO_Decoder>( this );
}

StatusCode Muon::RpcRDO_Decoder::initialize()
{
  
  ATH_MSG_DEBUG ( "initialize"); 
  
  // Get the RPC id helper from the detector store
  if (detStore()->retrieve(m_rpcIdHelper, "RPCIDHELPER").isFailure()) {
    ATH_MSG_FATAL ( "Could not get RpcIdHelper !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Found the RpcIdHelper. " );
  }

  // Get MuonDetectorManager
  if (detStore()->retrieve( m_muonMgr ).isFailure())
  {
      msg(MSG::ERROR) << "Can't retrieve MuonGM::MuonDetectorManager" 
		      << endmsg;
  }
  else msg(MSG::DEBUG) << "Found MuonGM::MuonDetectorManager "<<endmsg;
      
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
	m_cablingSvc = 0;
	return StatusCode::FAILURE;
    } 
    else {
	msg (MSG::VERBOSE) << " RPCcablingSvc obtained " << endmsg;
    }

    return sc;
  
}



std::vector<RpcDigit*>* Muon::RpcRDO_Decoder::getDigit(const RpcFiredChannel * fChan, 
							uint16_t& sectorID, uint16_t& padId, 
							uint16_t& cmaId) const
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
  //std::cout << "here" <<std::endl;
  float time = (fChan->bcid()-3)*25 +(fChan->time()+0.5-2)*3.125; 
  
  // skip the trigger hits
  if (ijk==7) {
    return rpcDigitVec;
  } 

  // Get the list of offline channels corresponding to the 
  // online identifier
  std::list<Identifier> idList = m_cablingSvc->give_strip_id(side, slogic, padId, 
							     cmaId, ijk, channel);
 
  std::list<Identifier>::const_iterator it_list;
  
  for (it_list=idList.begin() ; it_list != idList.end() ; ++it_list) {
    
    // and add the digit to the collection
    Identifier stripOfflineId = *it_list;

    //    // RPC digits do not hold anymore time of flight : digit time (and RDO time) is TOF subtracted 
    //    // recalculate the time of flight in case it was not in the RDOs
    //    if (time==0) {
    //      // get the digit position
    //      const MuonGM::RpcReadoutElement* descriptor = 
    //	m_muonMgr->getRpcReadoutElement(stripOfflineId);
    //      
    //      const HepGeom::Point3D<double> stripPos = descriptor->stripPos(stripOfflineId);
    //      // TEMP : set the time of flight from the digit position
    //      // temporary solution
    //      time = static_cast<int> ( stripPos.distance()/(299.7925*CLHEP::mm/CLHEP::ns) );
    //    
    //    }

    RpcDigit* rpcDigit = new RpcDigit(stripOfflineId, time);
    rpcDigitVec->push_back(rpcDigit);

  }


  return rpcDigitVec;
}


std::vector<Identifier>* Muon::RpcRDO_Decoder::getOfflineData(const RpcFiredChannel * fChan, 
							       uint16_t& sectorID, uint16_t& padId, 
							       uint16_t& cmaId, double& time) const
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
  //std::cout << "here" <<std::endl;
  time = (fChan->bcid()-3)*25 +(fChan->time()+0.5-2)*3.125;  
  
  // skip the trigger hits
  if (ijk==7) {
    return rpcIdVec;
  } 

  // Get the list of offline channels corresponding to the 
  // online identifier
  std::list<Identifier> idList = m_cablingSvc->give_strip_id(side, slogic, padId, 
							     cmaId, ijk, channel);
 
  std::list<Identifier>::const_iterator it_list;
  
  for (it_list=idList.begin() ; it_list != idList.end() ; ++it_list) {
    
    // and add the digit to the collection
    Identifier stripOfflineId = *it_list;

// this method is used only in data (real & MC) decoding ... don't shift the time from the RDO
//
//     // recalculate the time of flight in case it was not in the RDOs
//     if (time==0.0) {
//       // get the digit position
//       const MuonGM::RpcReadoutElement* descriptor = 
// 	m_muonMgr->getRpcReadoutElement(stripOfflineId);
//       const HepGeom::Point3D<double> stripPos = descriptor->stripPos(stripOfflineId);
//       // TEMP : set the time of flight from the digit position
//       // temporary solution
//       time = stripPos.distance()/(299.7925*CLHEP::mm/CLHEP::ns);    
//     }

    rpcIdVec->push_back(stripOfflineId);

  }


  return rpcIdVec;
}
