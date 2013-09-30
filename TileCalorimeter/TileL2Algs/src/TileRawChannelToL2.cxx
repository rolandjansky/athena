/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawChannelToL2.cxx
//  Author   : Aranzazu Ruiz
//  Created  : February, 2007
//
//  DESCRIPTION:
//     Created to emulate the algorithms processed at the TileCal ROD DSP level
//     to contribute to the LVL2 trigger: a low Pt muon tagging algorithm based
//     on the typical muon energy deposition in each cell of TileCal following
//     projective patterns and the Et calculation per superdrawer.
//     It takes as input the TileRawChannelContainer. The resulting TileL2 objects 
//     store the information about the number of muons tagged, their coordinates 
//     eta and phi, the energy deposited in Tile, a quality factor and the Et 
//     per superdrawer. There is a TileL2 object per superdrawer, stored in a
//     TileL2Container in StoreGate.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Gaudi includes
#include "GaudiKernel/IToolSvc.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileL2.h"
#include "TileL2Algs/TileRawChannelToL2.h"
#include "TileL2Algs/TileL2Builder.h"

TileRawChannelToL2::TileRawChannelToL2(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_tileL2Builder("TileL2Builder")
  , m_rawChannelContainer("TileRawChannelCnt")
  , m_l2Container("TileL2Cnt")
{
  declareProperty("TileRawChannelContainer", m_rawChannelContainer);  // Raw channels to convert.
  declareProperty("TileL2Container", m_l2Container);                  // Name of output container
}

TileRawChannelToL2::~TileRawChannelToL2() {
}

StatusCode TileRawChannelToL2::initialize() {

  CHECK( m_tileL2Builder.retrieve() );

  ATH_MSG_INFO( "TileRawChannelToL2 initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelToL2::execute() {

  if (evtStore()->contains < TileL2Container > (m_l2Container)) {
    ATH_MSG_DEBUG( m_l2Container << " already exists" );
    return StatusCode::SUCCESS;
  }

  TileL2Container* cont = new TileL2Container;
  cont->reserve(256);
  for (int i = 0; i < 256; i++) {
    int collId = m_tileL2Builder->indexToId(i);
    TileL2* l2 = new TileL2(collId);
    cont->push_back(l2);
  }

  // TileL2Builder is called
  if (m_tileL2Builder->process(0, 0xFFFF, cont).isFailure()) {
    ATH_MSG_ERROR( "Could not call TileL2Builder" );
    cont->clear();
    delete cont;
    return StatusCode::FAILURE;
  }

  // Register the set of TileL2 to the event store
  CHECK( evtStore()->record(cont, m_l2Container, false) );


  // Debug
//
//  if (msgLvl(MSG::DEBUG)) {
//    TileL2Container::const_iterator ind = cont->begin();
//    TileL2Container::const_iterator lastpippo = cont->end();
//
//    for (; ind != lastpippo; ++ind) {
//
//      msg(MSG::DEBUG) << "frag ID = 0x" << MSG::hex << ((*ind)->identify()) << MSG::dec
//                      << "  phi = " << ((*ind)->phi(0))
//                      << "  Et = " << ((*ind)->Et())
//                      << "  word = 0x" << MSG::hex << ((*ind)->val(0)) << MSG::dec << endmsg;
//
//      for (unsigned int i = 0; i < ((*ind)->NMuons()); ++i) {
//        msg(MSG::DEBUG) << "Muon found:"
//                        << " frag ID = 0x" << MSG::hex << ((*ind)->identify()) << MSG::dec
//                        << " word1 = 0x" << MSG::hex << ((*ind)->val(1 + 2 * i)) << MSG::dec
//                        << " word2 = 0x" << MSG::hex << ((*ind)->val(2 + 2 * i)) << MSG::dec
//                        << " eta = " << ((*ind)->eta(i))
//                        << " phi = " << ((*ind)->phi(i)) << endmsg;
//
//        msg(MSG::DEBUG) << "Muon found:"
//                        << " E[0] = " << ((*ind)->enemu0(i)) << " MeV"
//                        << " E[1] = " << ((*ind)->enemu1(i)) << " MeV"
//                        << " E[2] = " << ((*ind)->enemu2(i)) << " MeV"
//                        << " QF = " << ((*ind)->qual(i)) << endmsg;
//
//      }
//    }
//  }

  ATH_MSG_VERBOSE( "TileL2 container registered to the TES with name " << m_l2Container );

  // Execution completed
  ATH_MSG_DEBUG( "TileRawChannelToL2 execution completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelToL2::finalize() {

  ATH_MSG_INFO( "TileRawChannelToL2::finalize() end" );

  return StatusCode::SUCCESS;
}
