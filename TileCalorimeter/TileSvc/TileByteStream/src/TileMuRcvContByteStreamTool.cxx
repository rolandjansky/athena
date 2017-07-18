/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuRcvContByteStreamTool.cxx
// Author   : Joao Gentil Saraiva (jmendes@cern.ch)
// Created  : February 2015
//
// DESCRIPTION
//    AlgTool used in the TileMuRcv to BS conversion
//
// BUGS:
//
// History:
//
//****************************************************************************

// Gaudi includes
#include "GaudiKernel/ToolFactory.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileByteStream/TileMuRcvContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileROD_Encoder.h"
#include "TileEvent/TileMuonReceiverObj.h"
#include "TileEvent/TileContainer.h"
#include "TileIdentifier/TileHWID.h"

#include "CLIDSvc/CLASS_DEF.h"

#include <map> 
#include <stdint.h>

static const InterfaceID IID_ITileMuRcvContByteStreamTool("TileMuRcvContByteStreamTool", 1, 0);

const InterfaceID& TileMuRcvContByteStreamTool::interfaceID() {
  return IID_ITileMuRcvContByteStreamTool;
}

// default constructor

TileMuRcvContByteStreamTool::TileMuRcvContByteStreamTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_tileHWID(0)
  , m_hid2re(0)
    //, m_verbose(false)
{
  declareInterface<TileMuRcvContByteStreamTool>(this);
}

// destructor

TileMuRcvContByteStreamTool::~TileMuRcvContByteStreamTool() {
}

StatusCode TileMuRcvContByteStreamTool::initialize() {

  ATH_MSG_INFO ("Initializing TileMuRcvContByteStreamTool");

  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  ToolHandle<TileROD_Decoder> dec("TileROD_Decoder");
  CHECK( dec.retrieve() );

  m_hid2re = dec->getHid2reHLT();

  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamTool::finalize() {
  ATH_MSG_INFO ("Finalizing TileMuRcvContByteStreamTool successfuly");
  return StatusCode::SUCCESS;
}

StatusCode TileMuRcvContByteStreamTool::convert(TileMuonReceiverContainer* cont, FullEventAssembler<TileHid2RESrcID> *fea) {

  ATH_MSG_INFO ("Executing TileMuRcvContByteStreamTool::convert method");

  int  n           = 0;
  uint32_t frag_id = 0x0;
  uint32_t reid    = 0x0;
  
  TileMuonReceiverContainer::const_iterator it_cont  = cont->begin();
  TileMuonReceiverContainer::const_iterator end_cont = cont->end();

  // skip thresholds stored at first position of the container
  //
  ++it_cont;

  std::map<uint32_t, TileROD_Encoder> mapEncoder;

  for (; it_cont != end_cont; ++it_cont) 
    {
      n++;
      frag_id = (*it_cont)->identify();
      reid = m_hid2re->getRodTileMuRcvID(frag_id);
      mapEncoder[reid].setTileHWID(m_tileHWID);
      const TileMuonReceiverObj* tileMuRcv = *it_cont;	
      mapEncoder[reid].addTileMuRcvObj(tileMuRcv);
    }                                                            

  ATH_MSG_DEBUG( " Number of TileMuonReceiverObj objects counted " << n << " out of the possible " << cont->size()-1 ); 

  // fill ROD
  //
  std::map<uint32_t, TileROD_Encoder>::iterator map_it  = mapEncoder.begin();
  std::map<uint32_t, TileROD_Encoder>::iterator map_end = mapEncoder.end();

  FullEventAssembler<TileHid2RESrcID>::RODDATA* theROD;

  TileROD_Encoder* theEncoder;

  for (; map_it != map_end; ++map_it) 
    {
      theROD     = fea->getRodData( (*map_it).first );
      theEncoder = &( (*map_it).second );
      theEncoder -> fillRODTileMuRcvObj( *theROD );
      ATH_MSG_DEBUG( " Number of words in ROD " <<MSG::hex<< (*map_it).first <<MSG::dec<< ": " << theROD->size() );// DEBUG
      ATH_MSG_DEBUG( " ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " );// DEBUG
    }

  return StatusCode::SUCCESS;
}
