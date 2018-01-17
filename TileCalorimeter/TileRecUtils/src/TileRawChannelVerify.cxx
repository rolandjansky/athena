/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawChannelVerify.cxx
//  Author   : Zhifang
//  Created  : May, 2002
//
//  DESCRIPTION:
//     Implement the TileRawChannelVerify class
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Tile includes
#include "TileRecUtils/TileRawChannelVerify.h"
#include "TileIdentifier/TileHWID.h"
#include "StoreGate/ReadHandle.h"

// Atlas includes
// access all RawChannels inside container
#include "EventContainers/SelectAllObject.h" 
#include "AthenaKernel/errorcheck.h"

// Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// C++ STL includes
#include <vector>
#include <algorithm>

// C includes
#include <cmath>

using namespace std;


/**
@class CompRawChannel
@brief Small class holding a single method to compare two different TileRawChannel  
 
*/
class CompRawChannel: public binary_function<const TileRawChannel*, const TileRawChannel*, bool> {
  public:
    bool operator()(const TileRawChannel* p1, const TileRawChannel* p2) {
      return p1->amplitude() < p2->amplitude();
    }
};

//==========================================================================
// TileRawChannelVerify's implementations
//==========================================================================

// Constructor
TileRawChannelVerify::TileRawChannelVerify(string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(0)
{
  declareProperty("Precision", m_precision = 0);
  declareProperty("DumpRawChannels", m_dumpRawChannels = false);
  declareProperty("SortFlag", m_sortFlag = false);
}

TileRawChannelVerify::~TileRawChannelVerify() {
}

// Alg standard interfacw function
StatusCode TileRawChannelVerify::initialize() {

  // retrieve TileHWID helper from det store
  CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_CHECK( m_rawChannelContainer1Key.initialize() );
  ATH_CHECK( m_rawChannelContainer2Key.initialize() );

  ATH_MSG_INFO( "TileRawChannelVerify initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelVerify::execute() {

  // step1: read two cell containers from TES
  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer1(m_rawChannelContainer1Key);
  SG::ReadHandle<TileRawChannelContainer> rawChannelContainer2(m_rawChannelContainer2Key);

  SelectAllObject<TileRawChannelContainer> selAll1(rawChannelContainer1.cptr());
  SelectAllObject<TileRawChannelContainer>::const_iterator rawItr1 = selAll1.begin();
  SelectAllObject<TileRawChannelContainer>::const_iterator end1 = selAll1.end();

  SelectAllObject<TileRawChannelContainer> selAll2(rawChannelContainer2.cptr());
  SelectAllObject<TileRawChannelContainer>::const_iterator rawItr2 = selAll2.begin();
  SelectAllObject<TileRawChannelContainer>::const_iterator end2 = selAll2.end();

  // step2: first compare the number of cells in the two containers 
  int nSize1 = 0;
  for (; rawItr1 != end1; ++rawItr1) ++nSize1;

  int nSize2 = 0;
  for (; rawItr2 != end2; ++rawItr2) ++nSize2;

  ATH_MSG_INFO( "The number of cells in " << m_rawChannelContainer1Key.key() << " is " << nSize1 );
  ATH_MSG_INFO( "The number of cells in " << m_rawChannelContainer2Key.key() << " is " << nSize2 );

  if (nSize1 != nSize2) {
    ATH_MSG_ERROR( "The number of rawChannels is not equal in the two containers" );
    return (StatusCode::SUCCESS);
  }

  // step3: to sort the cells in the containers by amplitude
  vector<const TileRawChannel*> rawChannels1;
  vector<const TileRawChannel*> rawChannels2;
  const TileRawChannel* rawChannel1;
  const TileRawChannel* rawChannel2;
  if (m_sortFlag) {
    rawItr1 = selAll1.begin();
    end1 = selAll1.end();
    for (; rawItr1 != end1; ++rawItr1)
      rawChannels1.push_back((*rawItr1));

    sort(rawChannels1.begin(), rawChannels1.end(), CompRawChannel());

    rawItr2 = selAll2.begin();
    end2 = selAll2.end();
    for (; rawItr2 != end2; ++rawItr2)
      rawChannels2.push_back((*rawItr2));

    sort(rawChannels2.begin(), rawChannels2.end(), CompRawChannel());
  }

  rawItr1 = selAll1.begin();
  end1 = selAll1.end();

  rawItr2 = selAll2.begin();
  end2 = selAll2.end();

  // step4: then compare every cell-pair in the containers
  bool bErrorFlag = false;
  bool bHeaderFlag = true;
  for (int i = 0; i < nSize1; ++i) {
    if (m_sortFlag) {
      rawChannel1 = rawChannels1[i];
      rawChannel2 = rawChannels2[i];
    } else {
      rawChannel1 = (*rawItr1);
      ++rawItr1;
      rawChannel2 = (*rawItr2);
      ++rawItr2;
    }
    HWIdentifier id1 = rawChannel1->adc_HWID();
    HWIdentifier id2 = rawChannel2->adc_HWID();
    double amp1 = rawChannel1->amplitude();
    double amp2 = rawChannel2->amplitude();
    double diff = fabs(amp1 - amp2);
    if (id1 != id2 || diff > m_precision) bErrorFlag = true;
    if (msgLvl(MSG::VERBOSE) && (m_dumpRawChannels || bErrorFlag)) {
      if (bHeaderFlag) {
        msg(MSG::VERBOSE) << "             ===" << m_rawChannelContainer1Key.key() 
                          << "===      ===" << m_rawChannelContainer2Key.key() << "===" << endmsg;
        msg(MSG::VERBOSE) << "  Index      e1            id1        |        e2           id2" << endmsg;
        msg(MSG::VERBOSE) << "--------------------------------------------------------------------------------" << endmsg;
        bHeaderFlag = false;
      }
      msg(MSG::VERBOSE) << setw(5) << i
                        << "   " << setw(12)  << amp1
                        << "   [" << m_tileHWID->to_string(id1) << "]"
                        << "  |  " << setw(12) << amp2
                        << "   [" << m_tileHWID->to_string(id2) << "]";
      if (diff > m_precision) {
        msg(MSG::VERBOSE) << " A* ";
      }
      if (id1 != id2) {
        msg(MSG::VERBOSE) << " I* ";
      }
      msg(MSG::VERBOSE) << endmsg;
    } else if (bErrorFlag) {
      break;
    }
  }
  if (!bHeaderFlag) {
    msg(MSG::VERBOSE) << "--------------------------------------------------------------------------------" << endmsg;
  }
  if (!bErrorFlag) {
    ATH_MSG_INFO( "The two cellContainers (" << m_rawChannelContainer1Key.key()
                  << " and " << m_rawChannelContainer2Key.key() << ") are the same!!!" );
  } else {
    ATH_MSG_INFO( "The two cellContainers (" << m_rawChannelContainer1Key.key()
                  << " and " << m_rawChannelContainer2Key.key() << ") are not the same!!!" );
  }

  // Execution completed.
  ATH_MSG_INFO( "TileRawChannelVerify execution completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelVerify::finalize() {

  ATH_MSG_INFO( "TileRawChannelVerify finalized successfully" );

  return StatusCode::SUCCESS;
}

