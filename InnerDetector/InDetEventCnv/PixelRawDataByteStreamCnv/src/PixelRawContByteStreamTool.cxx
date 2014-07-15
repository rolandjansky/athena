/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  PixelRawContByteStreamTool.cxx
//   Implementation file for class PixelRawContByteStreamTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Pixel Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-39 05/03/2007 Daniel Dobos
///////////////////////////////////////////////////////////////////

#include "PixelRawContByteStreamTool.h"

#include "PixelCabling/IPixelCablingSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 
#include "PixelRodEncoder.h"

#include <map>


////////////////////////
// constructor
////////////////////////
PixelRawContByteStreamTool::PixelRawContByteStreamTool(const std::string& type,const std::string& name,const IInterface* parent) : 
  AthAlgTool(type,name,parent), 
  m_pixelCabling("PixelCablingSvc", name),
  m_log(msgSvc(), "PixelRawContByteStreamTool")
{
  declareInterface<PixelRawContByteStreamTool>(this);
  declareProperty("RodBlockVersion",m_RodBlockVersion=0); 
  declareProperty("BCs_per_LVL1ID",m_BCs_per_LVL1ID=1);
}

////////////////////////
// destructor
////////////////////////
PixelRawContByteStreamTool::~PixelRawContByteStreamTool() {
}

////////////////////////
// initialize
////////////////////////
StatusCode PixelRawContByteStreamTool::initialize() {

  StatusCode sc = AthAlgTool::initialize(); 

  IToolSvc* toolSvc;
  if(StatusCode::SUCCESS != service("ToolSvc",toolSvc)) {
    m_log << MSG::ERROR << "Can't get ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }
  
  std::string toolType ; 

  if (m_pixelCabling.retrieve().isFailure()) {
    m_log << MSG::ERROR << "Can't get PixelCablingSvc" << endreq;
    return StatusCode::FAILURE;
  }
   
  // Get PixelID
  if (detStore()->retrieve(m_PixelID, "PixelID").isFailure()) {
   msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
   return StatusCode::FAILURE;
  }

  if (detStore()->retrieve(m_pixelManager, "Pixel").isFailure()) {
    msg(MSG::FATAL) << "Failed to get Pixel Manager" << endreq;
    return StatusCode::FAILURE;
  }
   
  return sc;
}

////////////////////////
// finalize
////////////////////////
StatusCode PixelRawContByteStreamTool::finalize() {
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

////////////////////////
// convert - 
////////////////////////
StatusCode PixelRawContByteStreamTool::convert(PixelRDO_Container* cont,RawEventWrite* re) {
  m_fea.clear(); 
  FullEventAssembler<SrcIdMap>::RODDATA* theROD;

  // set ROD Minor version
  m_fea.setRodMinorVersion(m_RodBlockVersion);
#ifdef _DEBUG
  m_log << MSG::DEBUG << "Setting ROD Minor Version Number to: " << m_RodBlockVersion << endreq;
#endif

  // a map for ROD ID onto Encoder
  std::map<uint32_t, PixelRodEncoder> mapEncoder; 
  
  //loop over the Pixel modules
  PixelRDO_Container::const_iterator it_coll = cont->begin(); 
  PixelRDO_Container::const_iterator it_coll_end = cont->end();
#ifdef _DEBUG
  m_log << MSG::DEBUG << "Found " << cont->size() << " Pixel modules" << endreq ;
#endif


  for( ; it_coll!=it_coll_end;++it_coll) {
    const InDetRawDataCollection<PixelRDORawData>* coll = (*it_coll) ;

    // get OfflineId and RODID
    if (coll != NULL){
      Identifier offlineId = coll->identify();
      uint32_t rodId = m_pixelCabling->getRobId(offlineId);  //FIXME
      if (  rodId < 1) {
	m_log << MSG::ERROR << "Didn't found ROBID for OfflineID: 0x" << std::hex << offlineId << std::dec << endreq;
      } 

      //=======================================================================================================================//
      //                                    Set the properties in the encoder                                                  //
      //=======================================================================================================================//
      mapEncoder[rodId].set_pixelCabling(m_pixelCabling);
      mapEncoder[rodId].set_PixelID(m_PixelID);
      mapEncoder[rodId].setRodMinorVersion(m_RodBlockVersion);
      mapEncoder[rodId].setPixelDetectorManager(m_pixelManager);

      // loop over the HITS 
      InDetRawDataCollection<PixelRDORawData>::const_iterator it_b = coll->begin(); 
      InDetRawDataCollection<PixelRDORawData>::const_iterator it_e = coll->end(); 
    
      for(; it_b!=it_e; ++it_b){
	const PixelRDORawData* d = *it_b; 
	mapEncoder[rodId].addRdo(d);
      }
    }
    else
      m_log << MSG::WARNING << "IDC contains NULLpointer to collection, skipping collection" << endreq;
  }

  //=======================================================================================================================//
  //      Fill the ROD Data into  Full Event loop over Encoder map and fill all ROD Data Blocks                           //
  //=======================================================================================================================//
  std::map<uint32_t,PixelRodEncoder>::iterator it_map     = mapEncoder.begin() ;
  std::map<uint32_t,PixelRodEncoder>::iterator it_map_end = mapEncoder.end();
  for (; it_map != it_map_end; ++it_map) { 
    theROD = m_fea.getRodData((*it_map).first); 
    ((*it_map).second).fillROD( *theROD, m_log, m_BCs_per_LVL1ID);  
  } 
  m_fea.fill(re,m_log); 
  return StatusCode::SUCCESS; 
}

static const InterfaceID IID_IPixelRawContByteStreamTool("PixelRawContByteStreamTool", 1, 0);

const InterfaceID& PixelRawContByteStreamTool::interfaceID() { 
  return IID_IPixelRawContByteStreamTool; 
}
