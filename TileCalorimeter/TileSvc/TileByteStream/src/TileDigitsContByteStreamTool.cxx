/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/ToolFactory.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileByteStream/TileDigitsContByteStreamTool.h"
#include "TileByteStream/TileROD_Encoder.h"
#include "TileEvent/TileDigitsCollection.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "TileIdentifier/TileHWID.h"

#include "CLIDSvc/CLASS_DEF.h"

#include <map> 
#include <stdint.h>


static const InterfaceID IID_ITileDigitsContByteStreamTool
            ("TileDigitsContByteStreamTool", 1, 0);

const InterfaceID& TileDigitsContByteStreamTool::interfaceID( )
{ return IID_ITileDigitsContByteStreamTool; }

// default constructor
 
TileDigitsContByteStreamTool::TileDigitsContByteStreamTool( const std::string& type
    , const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent)
  , m_verbose(false)
{
  declareInterface< TileDigitsContByteStreamTool >( this );
  declareProperty("DoFragType1", m_doFragType1 = false);
  declareProperty("DoFragType5", m_doFragType5 = false);
}

// destructor 
 

TileDigitsContByteStreamTool::~TileDigitsContByteStreamTool() {
}
 

 

StatusCode TileDigitsContByteStreamTool::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  m_hid2re.setTileHWID(m_tileHWID);
  m_fea.idMap().setTileHWID(m_tileHWID);
  /*
   IProperty* propertyServer(0);
   sc = serviceLocator()->service("TileROD_Decoder", propertyServer);
   if (sc.isSuccess() ) {

   bool boolProp;
   BooleanProperty boolProperty("VerboseOutput",boolProp);

   sc = propertyServer->getProperty(&boolProperty);
   if (sc.isSuccess()) {
   m_verbose = boolProperty.value();
   }
   }
   */
  return StatusCode::SUCCESS;
}

StatusCode TileDigitsContByteStreamTool::finalize() {

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsContByteStreamTool::convert(DIGITS* cont,
    FullEventAssembler<TileHid2RESrcID> *fea) {

  //fea->clear();
  // m_fea.idMap().setTileHWID(m_tileHWID);

  FullEventAssembler<TileHid2RESrcID>::RODDATA* theROD;
  TileDigitsContainer::const_iterator it_coll = cont->begin();
  TileDigitsContainer::const_iterator it_coll_end = cont->end();

  std::map<uint32_t, TileROD_Encoder> mapEncoder;

  ATH_MSG_DEBUG( " number of collections " << cont->size() );
  for (; it_coll != it_coll_end; ++it_coll) {
    const TileDigitsCollection* coll = (*it_coll);

    // the same ROD id for all channels in collection
    TileDigitsCollection::ID frag_id = coll->identify();
    uint32_t reid = m_hid2re.getRodID(frag_id);
    mapEncoder[reid].setTileHWID(m_tileHWID, m_verbose, 1);
    TileDigitsCollection::const_iterator it_b = coll->begin();
    TileDigitsCollection::const_iterator it_e = coll->end();

    int n = 0;
    for (; it_b != it_e; ++it_b) {
      const TileDigits* digi = *it_b;
      mapEncoder[reid].addDigi(digi);
      ++n;
    }

    ATH_MSG_DEBUG( " collection " << MSG::hex << "0x" << frag_id
                  << " ROD " << "0x" << reid
                  << " number of channels " << MSG::dec << n );
  }

  std::map<uint32_t, TileROD_Encoder>::iterator it = mapEncoder.begin();
  std::map<uint32_t, TileROD_Encoder>::iterator it_end = mapEncoder.end();

  TileROD_Encoder* theEncoder;

  // TileROD_Encoder has collected all the channels, now can fill the
  // ROD block data.

  for (; it != it_end; ++it) {
    theROD = fea->getRodData((*it).first);
    theEncoder = &((*it).second);
    if (m_doFragType1)
      theEncoder->fillROD1(*theROD);
    if (m_doFragType5)
      theEncoder->fillROD5D(*theROD);
    ATH_MSG_DEBUG( "  Number of words in ROD " << theROD->size() );
  }

  return StatusCode::SUCCESS;
}
