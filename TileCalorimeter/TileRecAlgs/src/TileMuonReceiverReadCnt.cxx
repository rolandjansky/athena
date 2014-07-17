/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuonReceiverReadCnt.cxx 
//  Author   : Joao Gentil Saraiva <jmendes@cern.ch> 
//  Created  : April 2014
//
//  DESCRIPTION:
//    To read a container of the TileContainer type
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloIdentifier/TileID.h"

// Tile includes
#include "TileEvent/TileContainer.h"
//#include "TileEvent/TileMuonReceiverContainer.h"
#include "TileRecAlgs/TileMuonReceiverReadCnt.h"

//C++ STL includes
#include <vector>
#include <algorithm>

using namespace std;

//==========================================================================
// TileMuonReceiverReadCnt's implementations
//==========================================================================

// Constructor
TileMuonReceiverReadCnt::TileMuonReceiverReadCnt(string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_TileMuRcvContainer("TileMuRcvCnt")
  , m_tileID(0)
{
  declareProperty("TileMuRcvContainer", m_TileMuRcvContainer);
}

TileMuonReceiverReadCnt::~TileMuonReceiverReadCnt() {
}

// Alg standard interfacw function
StatusCode TileMuonReceiverReadCnt::initialize() {

  // retrieve TileID helper from det store
  CHECK( detStore()->retrieve(m_tileID) );

  ATH_MSG_INFO( "TileMuonReceiverReadCnt initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverReadCnt::execute() {

  // step1: read the tile muon receiver container from TES
  //
  const TileMuonReceiverContainer *container;
  CHECK(evtStore()->retrieve(container, m_TileMuRcvContainer));
 
  // step2: check the number of entries that should be 128 per event
  //
  int nentries = container->size();
  ATH_MSG_INFO( "The number of container elements in " <<  m_TileMuRcvContainer << " is " << nentries );

  // step3: check container contents
  //
  TileMuonReceiverContainer::const_iterator it = container->begin();
  TileMuonReceiverContainer::const_iterator itLast = container->end();

  for (; it != itLast; ++it) {

    const TileMuonReceiverObj * obj = (*it);

    int id = obj->GetID();
    ATH_MSG_INFO( "Identifier of module " << id );

    // -- read thresholds
    const std::vector<float> & thresh = obj->GetThresholds();
    if (thresh.size()>=4)
      ATH_MSG_INFO( " thresholds' vector size : " <<   thresh.size() << " value: " <<   thresh[0] << " " << thresh[1] << " " << thresh[2] << " " << thresh[3] );
    else
      ATH_MSG_INFO( " thresholds' vector size : " <<   thresh.size() );

    // -- read energy
    const std::vector<float> & ene = obj->GetEne(); 
    if (ene.size()==2)
      ATH_MSG_INFO( " energy vector size : "             <<      ene.size() << " value: " <<      ene[0] << " " << ene[1] );
    else
      ATH_MSG_INFO( " energy vector size : "             <<      ene.size() ); 

    // -- read time
    const std::vector<float> & time = obj->GetTime(); 
    if (time.size()>=2)
      ATH_MSG_INFO( " time vector size : "              <<     time.size() << " value: " <<     time[0] << " " << time[1] );
    else
      ATH_MSG_INFO( " time vector size : "              <<     time.size() );

    // -- read decision
    const std::vector<bool> & decision = obj->GetDecision(); 
    if (decision.size()>=4)
      ATH_MSG_INFO( " decision vector size : "    << decision.size() << " value: " << decision[0] << " " << decision[1] << " " << decision[2] << " " << decision[3] ); 
    else
      ATH_MSG_INFO( " decision vector size : "    << decision.size() ); 

  }

  // Execution completed.
  ATH_MSG_INFO( "TileMuonReceiverReadCnt execution completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuonReceiverReadCnt::finalize() {

  ATH_MSG_INFO( "TileMuonReceiverReadCnt::finalize()" );

  return StatusCode::SUCCESS;
}
