/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/ToolFactory.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileByteStream/TileDigitsContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"
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
  : AthAlgTool(type, name, parent)
  , m_tileHWID(0)
  , m_hid2re(0)
  , m_verbose(false)
{
  declareInterface< TileDigitsContByteStreamTool >( this );
  declareProperty("DoFragType1", m_doFragType1 = false);
  declareProperty("DoFragType5", m_doFragType5 = false);
}

// destructor 

TileDigitsContByteStreamTool::~TileDigitsContByteStreamTool() {}

StatusCode TileDigitsContByteStreamTool::initialize() {

  ATH_MSG_INFO ("Initializing TileDigitsContByteStreamTool");

  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  ToolHandle<TileROD_Decoder> dec("TileROD_Decoder");
  CHECK( dec.retrieve() );

  m_hid2re = dec->getHid2reHLT();

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsContByteStreamTool::finalize() {
  ATH_MSG_INFO ("Finalizing TileDigitsContByteStreamTool successfuly");
  return StatusCode::SUCCESS;
}

StatusCode TileDigitsContByteStreamTool::convert(DIGITS* digitsContainer, FullEventAssembler<TileHid2RESrcID> *fea) {

  FullEventAssembler<TileHid2RESrcID>::RODDATA* theROD;

  std::map<uint32_t, TileROD_Encoder> mapEncoder;

  ATH_MSG_DEBUG( " number of digits collections " << digitsContainer->size() << " " << evtStore()->proxy(digitsContainer)->name() );

  int m         = 0;
  int n         = 0;
  uint32_t reid = 0x0;

  bool isTMDB = evtStore()->proxy(digitsContainer)->name() == "MuRcvDigitsCnt";

  for (const TileDigitsCollection* digitsCollection : *digitsContainer) {

    TileDigitsCollection::ID frag_id = digitsCollection->identify(); 

    if (isTMDB){  
       reid = m_hid2re->getRodTileMuRcvID(frag_id);
       mapEncoder[reid].setTileHWID(m_tileHWID);
    } else {
       reid = m_hid2re->getRodID(frag_id);
       mapEncoder[reid].setTileHWID(m_tileHWID, m_verbose, 1);
    }

    for (const TileDigits* digits : *digitsCollection) {
      mapEncoder[reid].addDigi(digits);
      ++n;
    }
    ++m;

    ATH_MSG_DEBUG( " Collection " << m << ": " << MSG::hex << "0x" << frag_id
                  << " ROD " << "0x" << reid
                  << " number of channels " << MSG::dec << n );
  }

  TileROD_Encoder* theEncoder;

  // TileROD_Encoder has collected all the channels, now can fill the ROD block data.

  for (std::pair<uint32_t, TileROD_Encoder> reidAndEncoder: mapEncoder) {

    theROD = fea->getRodData(reidAndEncoder.first);
    theEncoder = &(reidAndEncoder.second);

    // RODId is already defined so use it for the exception

    if ((reidAndEncoder.first & 0xf00)) {
      theEncoder->fillRODTileMuRcvDigi(*theROD);
    } else {
      if (m_doFragType1) theEncoder->fillROD1(*theROD);
      if (m_doFragType5) theEncoder->fillROD5D(*theROD);
    }
    
    ATH_MSG_DEBUG( " Number words in ROD " << MSG::hex <<" 0x"<< reidAndEncoder.first << MSG::dec << " : " << theROD->size() );
  }

  return StatusCode::SUCCESS;
}
