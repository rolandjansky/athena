/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileL2ContByteStreamTool.cxx
// Author   : Aranzazu Ruiz
// Created  : March 2007
//
// DESCRIPTION
//    AlgTool used in the TileL2 to BS conversion
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
#include "TileByteStream/TileL2ContByteStreamTool.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "TileByteStream/TileROD_Encoder.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileL2.h"
#include "TileIdentifier/TileHWID.h"



#include "CLIDSvc/CLASS_DEF.h"

#include <map> 
#include <stdint.h>

static const InterfaceID IID_ITileL2ContByteStreamTool("TileL2ContByteStreamTool", 1, 0);

const InterfaceID& TileL2ContByteStreamTool::interfaceID() {
  return IID_ITileL2ContByteStreamTool;
}

// default contructor

TileL2ContByteStreamTool::TileL2ContByteStreamTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_tileHWID(0)
  , m_hid2re(0)
  , m_verbose(false)
{
  declareInterface<TileL2ContByteStreamTool>(this);
  declareProperty("DoFragTypeMu", m_doFragTypeMu = true);
  declareProperty("DoFragTypeEt", m_doFragTypeEt = true);
}

// destructor

TileL2ContByteStreamTool::~TileL2ContByteStreamTool() {
}

StatusCode TileL2ContByteStreamTool::initialize() {

  ATH_MSG_INFO ("Initializing TileL2ContByteStreamTool");

  CHECK( detStore()->retrieve(m_tileHWID, "TileHWID") );

  ToolHandle<TileROD_Decoder> dec("TileROD_Decoder");
  CHECK( dec.retrieve() );

  m_hid2re = dec->getHid2reHLT();

  return StatusCode::SUCCESS;
}

StatusCode TileL2ContByteStreamTool::finalize() {

  return StatusCode::SUCCESS;
}

StatusCode TileL2ContByteStreamTool::convert(TileL2Container* cont,
    FullEventAssembler<TileHid2RESrcID> *fea) {

  //fea->clear(); 

  FullEventAssembler<TileHid2RESrcID>::RODDATA* theROD;
  TileL2Container::const_iterator it_cont = cont->begin();
  TileL2Container::const_iterator it_cont_end = cont->end();

  std::map<uint32_t, TileROD_Encoder> mapEncoder;

  int n = 0;
  int nm = 0;

  for (; it_cont != it_cont_end; ++it_cont) {

    int frag_id = (*it_cont)->identify();

    uint32_t reid = m_hid2re->getRodID(frag_id);
    mapEncoder[reid].setTileHWID(m_tileHWID, m_verbose, 0x12);

    const TileL2* l2 = *it_cont;
    mapEncoder[reid].addL2(l2);
    ++n;
    nm += l2->NMuons();

    if (msgLvl(MSG::VERBOSE)) {
      unsigned int ndata = l2->Ndata();
      unsigned int j = 0;
      for (unsigned int i = 0; i < (l2->NMuons()); ++i, j += 2) {
        msg(MSG::VERBOSE) << "Muon found:" << MSG::hex
                          << " frag ID = 0x" << (l2->identify())
                          << " word1 = 0x" << ((j < ndata) ? l2->val(j) : 0)
                          << " word2 = 0x" << ((j + 1 < ndata) ? l2->val(j + 1) : 0) << MSG::dec
                          << " eta = " << (l2->eta(i))
                          << " phi = " << (l2->phi(i)) << endmsg;
      }
      for (; j < ndata; ++j) {
        msg(MSG::VERBOSE) << "extra word = 0x" << MSG::hex << l2->val(j) << MSG::dec << endmsg;
      }
    }
  }

  ATH_MSG_DEBUG( " Number of TileL2 objects " << n
                << "  Number of muons " << nm );

  std::map<uint32_t, TileROD_Encoder>::iterator it = mapEncoder.begin();
  std::map<uint32_t, TileROD_Encoder>::iterator it_end = mapEncoder.end();

  TileROD_Encoder* theEncoder;

  // TileROD_Encoder has collected all the TileL2s, now can fill the ROD block data

  for (; it != it_end; ++it) {
    theROD = fea->getRodData((*it).first);
    theEncoder = &((*it).second);
    if (m_doFragTypeMu) theEncoder->fillROD12(*theROD);
    if (m_doFragTypeEt) theEncoder->fillRODL2(*theROD);
  }

  return StatusCode::SUCCESS;
}
