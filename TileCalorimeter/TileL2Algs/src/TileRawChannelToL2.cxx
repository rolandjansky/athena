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

// Tile includes
#include "TileL2Algs/TileRawChannelToL2.h"
#include "TileEvent/TileL2.h"
#include "TileL2Algs/TileL2Builder.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/WriteHandle.h"


#include <memory>

TileRawChannelToL2::TileRawChannelToL2(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_tileL2Builder("TileL2Builder")
{
}

TileRawChannelToL2::~TileRawChannelToL2() {
}

StatusCode TileRawChannelToL2::initialize() {

  CHECK( m_tileL2Builder.retrieve() );

  ATH_CHECK( m_l2ContainerKey.initialize() );

  ATH_MSG_INFO( "TileRawChannelToL2 initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelToL2::execute() {

  SG::WriteHandle<TileL2Container> l2Container(m_l2ContainerKey);
  ATH_CHECK( l2Container.record(std::make_unique<TileL2Container>()) );

  ATH_MSG_VERBOSE( "TileL2 container registered to the TES with name " << m_l2ContainerKey.key() );

  l2Container->reserve(256);
  for (int i = 0; i < 256; i++) {
    int collId = m_tileL2Builder->indexToId(i);
    std::unique_ptr<TileL2> l2 = std::make_unique<TileL2>(collId);
    l2Container->push_back(l2.release());
  }

  // TileL2Builder is called
  CHECK( m_tileL2Builder->process(0, 0xFFFF, l2Container.ptr()) );

  // Debug
  if (msgLvl(MSG::VERBOSE)) {

    for (const TileL2* l2 : *l2Container) {

      msg(MSG::VERBOSE) << "frag ID = 0x" << MSG::hex << l2->identify() << MSG::dec
                      << "  phi = " << l2->phi(0)
                      << "  Et = " << l2->Et()
                      << "  word = 0x" << MSG::hex << l2->val(0) << MSG::dec << endmsg;

      for (unsigned int i = 0; i < l2->NMuons(); ++i) {
        msg(MSG::VERBOSE) << "Muon found:"
                        << " frag ID = 0x" << MSG::hex << l2->identify() << MSG::dec
                        << " word1 = 0x" << MSG::hex << l2->val(1 + 2 * i) << MSG::dec
                        << " word2 = 0x" << MSG::hex << l2->val(2 + 2 * i) << MSG::dec
                        << " eta = " << l2->eta(i)
                        << " phi = " << l2->phi(i) << endmsg;

        msg(MSG::VERBOSE) << "Muon found:"
                        << " E[0] = " << l2->enemu0(i) << " MeV"
                        << " E[1] = " << l2->enemu1(i) << " MeV"
                        << " E[2] = " << l2->enemu2(i) << " MeV"
                        << " QF = " << l2->qual(i) << endmsg;

      }
    }
  }



  // Execution completed
  ATH_MSG_DEBUG( "TileRawChannelToL2 execution completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelToL2::finalize() {

  ATH_MSG_INFO( "TileRawChannelToL2::finalize() end" );

  return StatusCode::SUCCESS;
}
