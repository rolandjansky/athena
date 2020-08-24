/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCsmContByteStreamTool.h"

#include "MdtROD_Encoder.h"
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "ByteStreamData/RawEvent.h" 
#include "MuonReadoutGeometry/MuonDetectorManager.h"

// default constructor
Muon::MdtCsmContByteStreamTool::MdtCsmContByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
    :  
    AthAlgTool(type,name,parent),
    m_hid2re(0)
{
  declareInterface< Muon::IMDT_RDOtoByteStreamTool  >( this );
}

StatusCode Muon::MdtCsmContByteStreamTool::initialize() {
  ATH_CHECK( m_idHelperSvc.retrieve() );
  m_hid2re = new MDT_Hid2RESrcID ();
  StatusCode status = m_hid2re->set(&m_idHelperSvc->mdtIdHelper());
  if ( status.isFailure() ){
    ATH_MSG_FATAL("Could not initialize MDT mapping !");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

 
StatusCode Muon::MdtCsmContByteStreamTool::finalize() {
   delete m_hid2re;
   return StatusCode::SUCCESS;
}


StatusCode Muon::MdtCsmContByteStreamTool::convert(CONTAINER* cont, RawEventWrite* re, 
						   MsgStream& log ) {
  
  m_fea.clear();
  StatusCode status = m_fea.idMap().set(&m_idHelperSvc->mdtIdHelper());
  if ( status.isFailure() ){
    ATH_MSG_FATAL("Could not initialize MDT mapping !");
    return StatusCode::FAILURE;
  }
  
  FullEventAssembler<MDT_Hid2RESrcID>::RODDATA*  theROD ;
  
  std::map<uint32_t, MdtROD_Encoder> mapEncoder; 

  const MuonGM::MuonDetectorManager* mdm = nullptr;
  ATH_CHECK( detStore()->retrieve (mdm,"Muon") );
  const MdtIdHelper& mdtIdHelper = *mdm->mdtIdHelper();
  
  ATH_MSG_DEBUG(" number of collections "<< cont->size());
  for (const MdtCsm* csm : *cont) {
    Identifier coll_id = csm->identify(); 
    uint32_t rodId = m_hid2re->getRodID(coll_id); 
    mapEncoder.try_emplace (rodId, mdtIdHelper).first->second.add(csm);
  } 
  
  // MdtCsm_Encoder has collected all the csm, now can fill the
  // ROD block data. 
  
  ATH_MSG_DEBUG(" start to fill Rod ");

  for (auto& p : mapEncoder) {
    theROD  = m_fea.getRodData( p.first ); 
    p.second.fillROD( *theROD ) ; 
  } 
  
  ATH_MSG_DEBUG(" filling the Raw Event ... ");
  
  m_fea.fill(re,log); 
  
  ATH_MSG_DEBUG(" RawEvent size in 32 bit word " << re->size_word());
  //    log <<MSG::DEBUG<<" RawEvent header  " << re->header() << endmsg;
  
  return StatusCode::SUCCESS; 
  
}






