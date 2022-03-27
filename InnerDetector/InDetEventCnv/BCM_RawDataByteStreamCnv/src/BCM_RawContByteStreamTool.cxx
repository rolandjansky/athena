/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  BCM_RawContByteStreamTool.cxx
//   Implementation file for class BCM_RawContByteStreamTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-02 05/03/2007 Daniel Dobos for pixels
//  Version 00-00-11 05/02/2009 Daniel Dobos for BCM
///////////////////////////////////////////////////////////////////

#include "BCM_RawDataByteStreamCnv/BCM_RawContByteStreamTool.h"

#include "InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h"

#include "BCM_RawDataByteStreamCnv/BCM_RodEncoder.h"

#include <cstdint>
#include <map>

////////////////////////
// constructor
////////////////////////
BCM_RawContByteStreamTool::BCM_RawContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent):AthAlgTool(type,name,parent) {
  declareInterface<BCM_RawContByteStreamTool>(this);
  declareProperty("RodBlockVersion",m_RodBlockVersion=0); 
  declareProperty("BCs_per_LVL1ID",m_BCs_per_LVL1ID=1);
}

////////////////////////
// destructor
////////////////////////
BCM_RawContByteStreamTool::~BCM_RawContByteStreamTool() {
}

////////////////////////
// initialize
////////////////////////
StatusCode BCM_RawContByteStreamTool::initialize() {
  ATH_CHECK( AthAlgTool::initialize() );
  return StatusCode::SUCCESS;
}

////////////////////////
// finalize
////////////////////////
StatusCode BCM_RawContByteStreamTool::finalize() {
  ATH_CHECK( AthAlgTool::finalize() );
  return StatusCode::SUCCESS;
}

////////////////////////
// convert
////////////////////////
StatusCode BCM_RawContByteStreamTool::convert(BCM_RDO_Container* cont) const
{
  FullEventAssembler<SrcIdMap>* fea = nullptr;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler (fea,
                                                        "BCMRawCont") );
  FullEventAssembler<SrcIdMap>::RODDATA* theROD;

  // set ROD Minor version
  fea->setRodMinorVersion(m_RodBlockVersion);
  ATH_MSG_DEBUG( "Setting ROD Minor Version Number to: " << m_RodBlockVersion );

  // a map for ROD ID onto Encoder
  std::map<uint32_t, BCM_RodEncoder> mapEncoder; 

  // loop over the BCM RDO collections
  BCM_RDO_Container::const_iterator it_coll = cont->begin(); 
  BCM_RDO_Container::const_iterator it_coll_end = cont->end();
  ATH_MSG_DEBUG( "Found " << cont->size() << " BCM RDO collections" );

  for( ; it_coll!=it_coll_end; ++it_coll) {
    const BCM_RDO_Collection* coll = (*it_coll);

    // get OfflineId (channel number) and RODID
    if (coll != nullptr) {
      unsigned int offlineId = coll->getChannel();
      uint32_t rodId = getSourceID(offlineId);
      if ( offlineId >= 16 ) {
	ATH_MSG_ERROR( "Didn't find RODID for OfflineID: " << offlineId );
      }

      mapEncoder[rodId].setRodMinorVersion(m_RodBlockVersion);

      // loop over the RDOs
      BCM_RDO_Collection::const_iterator it_b = coll->begin();
      BCM_RDO_Collection::const_iterator it_e = coll->end();

      for(; it_b!=it_e; ++it_b){
	const BCM_RawData* d = *it_b; 
	mapEncoder[rodId].addRdo(d);
      }
    }
    else
      ATH_MSG_WARNING( "Container contains NULLpointer to collection, skipping collection" );
  }

  // Fill the ROD Data into Full Event loop over Encoder map and fill all ROD Data Blocks
  std::map<uint32_t,BCM_RodEncoder>::iterator it_map     = mapEncoder.begin();
  std::map<uint32_t,BCM_RodEncoder>::iterator it_map_end = mapEncoder.end();
  for (; it_map != it_map_end; ++it_map) { 
    theROD = fea->getRodData((*it_map).first);
    ((*it_map).second).fillROD(*theROD, m_BCs_per_LVL1ID);
  }
  return StatusCode::SUCCESS; 
}

static const InterfaceID IID_IBCM_RawContByteStreamTool("BCM_RawContByteStreamTool", 1, 0);
const InterfaceID& BCM_RawContByteStreamTool::interfaceID() { 
  return IID_IBCM_RawContByteStreamTool; 
}

////////////////////////
// getSourceID - convert channelID into ROD source ID
////////////////////////
unsigned int BCM_RawContByteStreamTool::getSourceID(int ChannelID) const
{
  // CHANGED TO NEW SOURCE ID 0x0081000A -> 0x0081004A and 0x0081000C ->  0x0081004C 
  switch(ChannelID) {
  case 0:
    return 0x0081004A;
  case 1:
    return 0x0081004C;
  case 2:
    return 0x0081004A;
  case 3:
    return 0x0081004C;
  case 4:
    return 0x0081004A;
  case 5:
    return 0x0081004C;
  case 6:
    return 0x0081004A;
  case 7:
    return 0x0081004C;
  case 8:
    return 0x0081004C;
  case 9:
    return 0x0081004A;
  case 10:
    return 0x0081004C;
  case 11:
    return 0x0081004A;
  case 12:
    return 0x0081004C;
  case 13:
    return 0x0081004A;
  case 14:
    return 0x0081004C;
  case 15:
    return 0x0081004A;
  default:
    return 0xffffffff;
  }
}
