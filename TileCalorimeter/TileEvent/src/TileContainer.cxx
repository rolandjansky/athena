/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileContainer.cxx
//  Author   : Alexandre Solodkov
//  Created  : Mar, 2003
//
//  DESCRIPTION:
//     methods for all container classes
//
//  HISTORY:
//    12Mar03: Created
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "TileEvent/TileContainer.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileCellIDC.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "SGTools/BaseInfo.h"
 
void TileDigitsCollection::clear()
{
  m_FragChipCRCWords.clear();
  m_FragChipCRCWordsHIGH.clear();
  m_FragChipHeaderWords.clear();
  m_FragChipHeaderWordsHIGH.clear();

  m_FragExtraWords.resize(2);
  m_FragExtraWords[0] = m_FragExtraWords[1] = 0;

  m_FragSize = 0;
  m_FragBCID = 0;

  TileDigitsCollection::MyBase::clear();
}

void TileRawChannelCollection::clear()
{
  m_FragGlobalCRC   = 0;
  m_FragBCID        = 0;
  m_FragMemoryPar   = 0;
  m_FragSstrobe     = 0;
  m_FragDstrobe     = 0;
  m_FragHeaderBit   = 0;
  m_FragHeaderPar   = 0;
  m_FragSampleBit   = 0;
  m_FragSamplePar   = 0;
  m_FragFEChipMask  = 0xFFFF;
  m_FragRODChipMask = 0xFFFF;
  m_FragDSPBCID     = 0xDEAD;

  TileRawChannelCollection::MyBase::clear();
}

SG_ADD_BASE (TileContainer<TileCell>, DataVector<TileCell>);
