/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCsmContByteStreamTool.h"
#include "MdtROD_Encoder.h"

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGate.h"
//#include "StoreGate/tools/ClassID_traits.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "ByteStreamData/RawEvent.h" 

#include <map> 

// default constructor
Muon::MdtCsmContByteStreamTool::MdtCsmContByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
    :  
    AthAlgTool(type,name,parent),
    m_cabling(0),
    m_hid2re(0),
    m_mdtIdHelper(0)
{
  declareInterface< Muon::IMDT_RDOtoByteStreamTool  >( this );
}

// destructor 
 
//Muon::MdtCsmContByteStreamTool::~MdtCsmContByteStreamTool() {
//delete m_hid2re;
//}
  
StatusCode Muon::MdtCsmContByteStreamTool::initialize() {
 
  if (StatusCode::SUCCESS != service("MuonMDT_CablingSvc", m_cabling)) {
    ATH_MSG_ERROR(" Can't get MuonMDT_CablingSvc ");
    return StatusCode::FAILURE; 
  }  
  
  StoreGateSvc * detStore;
  StatusCode status = service("DetectorStore", detStore);
  if (status.isFailure()) {
    ATH_MSG_FATAL("DetectorStore service not found !");
    return StatusCode::FAILURE;
  } else {}
  
  // Get the MDT id helper from the detector store
  const MdtIdHelper* mdt_id;
  status = detStore->retrieve(mdt_id, "MDTIDHELPER");
  if (status.isFailure()) {
    ATH_MSG_FATAL("Could not get MdtIdHelper !");
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG(" Found the MdtIdHelper. ");
  }

  m_hid2re = new MDT_Hid2RESrcID ();
  m_hid2re->set(m_cabling, mdt_id);

  m_mdtIdHelper = mdt_id;

  return StatusCode::SUCCESS;
}

 
StatusCode Muon::MdtCsmContByteStreamTool::finalize() {
   delete m_hid2re;
   return StatusCode::SUCCESS;
}


StatusCode Muon::MdtCsmContByteStreamTool::convert(CONTAINER* cont, RawEventWrite* re, 
						   MsgStream& log ) {
  
  m_fea.clear();
  m_fea.idMap().set(m_cabling, m_mdtIdHelper);
  
  FullEventAssembler<MDT_Hid2RESrcID>::RODDATA*  theROD ;
  
  MdtCsmContainer::const_iterator it_coll = cont->begin(); 
  MdtCsmContainer::const_iterator it_coll_end = cont->end(); 
  
  std::map<uint32_t, MdtROD_Encoder> mapEncoder; 
  
  ATH_MSG_DEBUG(" number of collections "<< cont->size());
  for( ; it_coll!=it_coll_end;++it_coll) {
    Identifier coll_id = (*it_coll)->identify(); 
    uint32_t rodId = m_hid2re->getRodID(coll_id); 
    mapEncoder[rodId].add(*it_coll);
  } 
  
  std::map<uint32_t,MdtROD_Encoder>::iterator it     = mapEncoder.begin(); 
  std::map<uint32_t,MdtROD_Encoder>::iterator it_end = mapEncoder.end();
  
  // MdtCsm_Encoder has collected all the csm, now can fill the
  // ROD block data. 
  
  ATH_MSG_DEBUG(" start to fill Rod ");
  
  for (; it!=it_end;++it) { 
    theROD  = m_fea.getRodData( (*it).first ); 
    //((*it).second).set( m_hid2re, m_mdtIdHelper ) ; 
    ((*it).second).fillROD( *theROD ) ; 
  } 
  
  ATH_MSG_DEBUG(" filling the Raw Event ... ");
  
  m_fea.fill(re,log); 
  
  ATH_MSG_DEBUG(" RawEvent size in 32 bit word " << re->size_word());
  //    log <<MSG::DEBUG<<" RawEvent header  " << re->header() << endmsg;
  
  return StatusCode::SUCCESS; 
  
}






