/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//    See also .h file.
//    
//  Updated May 25, 2001 HMA
//  LAr Sampling bug fix.
//
//  FCAL Sampling bug fix  (sam = 1-3, not 0-2) 
//  Feb 20, 2002, HMA 
//  fix for EM endcap inner wheel (SR)
//  Oct 2003: use caloDDE (DR)
//*****************************************************************************

#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloID.h"

const unsigned int CaloSamplingHelper::m_EMMask     = 0x000000ff;
const unsigned int CaloSamplingHelper::m_HADMask    = 0x00ffff00;
const unsigned int CaloSamplingHelper::m_BarrelMask = 0x001ff00f;
const unsigned int CaloSamplingHelper::m_EndCapMask = 0x00e00ff0;
const unsigned int CaloSamplingHelper::m_LArMask    = 0x00e00fff;
const unsigned int CaloSamplingHelper::m_TileMask   = 0x001ff000;

CaloSamplingHelper::CaloSamplingHelper() {
}

CaloSamplingHelper::CaloSample
CaloSamplingHelper::getSampling( const CaloCell& cell ) {

  const CaloDetDescrElement * theCaloDDE= cell.caloDDE() ;
  if (theCaloDDE!=nullptr) {
    return (CaloSamplingHelper::CaloSample) theCaloDDE->getSampling();
      }
  else{
    return Unknown;
    
  }
}
  
unsigned int CaloSamplingHelper::getSamplingBit(const CaloCell& rCell)
{
  return getSamplingBit(getSampling(rCell));
}

unsigned int CaloSamplingHelper::getSamplingBit(const CaloSample& rSample)
{
  unsigned int bitPattern(0);
  switch ( rSample )
    {
    case PreSamplerB:  { bitPattern = 0x00000001; break; }
    case EMB1:         { bitPattern = 0x00000002; break; }
    case EMB2:         { bitPattern = 0x00000004; break; }
    case EMB3:         { bitPattern = 0x00000008; break; }
    case PreSamplerE:  { bitPattern = 0x00000010; break; }
    case EME1:         { bitPattern = 0x00000020; break; }
    case EME2:         { bitPattern = 0x00000040; break; }
    case EME3:         { bitPattern = 0x00000080; break; }
    case HEC0:         { bitPattern = 0x00000100; break; }
    case HEC1:         { bitPattern = 0x00000200; break; }
    case HEC2:         { bitPattern = 0x00000400; break; }
    case HEC3:         { bitPattern = 0x00000800; break; }
    case TileBar0:     { bitPattern = 0x00001000; break; }
    case TileBar1:     { bitPattern = 0x00002000; break; }
    case TileBar2:     { bitPattern = 0x00004000; break; }
    case TileGap1:     { bitPattern = 0x00008000; break; }
    case TileGap2:     { bitPattern = 0x00010000; break; }
    case TileGap3:     { bitPattern = 0x00020000; break; }
    case TileExt0:     { bitPattern = 0x00040000; break; }
    case TileExt1:     { bitPattern = 0x00080000; break; }
    case TileExt2:     { bitPattern = 0x00100000; break; }
    case FCAL0:        { bitPattern = 0x00200000; break; }
    case FCAL1:        { bitPattern = 0x00400000; break; }
    case FCAL2:        { bitPattern = 0x00800000; break; }
    case MINIFCAL0:    { bitPattern = 0x01000000; break; } 
    case MINIFCAL1:    { bitPattern = 0x02000000; break; } 
    case MINIFCAL2:    { bitPattern = 0x04000000; break; } 
    case MINIFCAL3:    { bitPattern = 0x08000000; break; } 
    default:           { break; }
    };
  return bitPattern;
}

bool
CaloSamplingHelper::getSamplingList(std::vector<CaloSamplingHelper::CaloSample>& vSamp)
{
  // clear vector
  vSamp.clear();
  // prepare vector
  vSamp.resize(getNumberOfSamplings(),Unknown);
  // fill vector for EMB
  vSamp[ 0] = PreSamplerB;
  vSamp[ 1] = EMB1;
  vSamp[ 2] = EMB2;
  vSamp[ 3] = EMB3;
  // fill vector for EME
  vSamp[ 4] = PreSamplerE;
  vSamp[ 5] = EME1;
  vSamp[ 6] = EME2;
  vSamp[ 7] = EME3;
  // fill vector for HEC
  vSamp[ 8] = HEC0;
  vSamp[ 9] = HEC1;
  vSamp[10] = HEC2;
  vSamp[11] = HEC3;
  // fill vector for tile barrel
  vSamp[12] = TileBar0;
  vSamp[13] = TileBar1;
  vSamp[14] = TileBar2;
  // fill vector for tile gap
  vSamp[15] = TileGap1;
  vSamp[16] = TileGap2;
  vSamp[17] = TileGap3;
  // fill vector for extended tile
  vSamp[18] = TileExt0;
  vSamp[19] = TileExt1;
  vSamp[20] = TileExt2;
  // fill vector for FCal
  vSamp[21] = FCAL0;
  vSamp[22] = FCAL1;
  vSamp[23] = FCAL2;

  // fill vector for MiniFCal
  vSamp[24] = MINIFCAL0;
  vSamp[25] = MINIFCAL1;
  vSamp[25] = MINIFCAL2;


  return true;
}

bool CaloSamplingHelper::isEMSampling(const CaloSample& rSample)
{
  return matchPattern(rSample,m_EMMask);
}

bool CaloSamplingHelper::isHADSampling(const CaloSample& rSample)
{
  return matchPattern(rSample,m_HADMask);
}

bool CaloSamplingHelper::isBarrelSampling(const CaloSample& rSample)
{
  return matchPattern(rSample,m_BarrelMask);
}

bool CaloSamplingHelper::isEndCapSampling(const CaloSample& rSample)
{
  return matchPattern(rSample,m_EndCapMask);
}

bool CaloSamplingHelper::isLArSampling(const CaloSample& rSample)
{
  return matchPattern(rSample,m_LArMask);
}

bool CaloSamplingHelper::isTileSampling(const CaloSample& rSample)
{
  return matchPattern(rSample,m_TileMask);
}

bool CaloSamplingHelper::matchPattern(const CaloSample& rSample,
				const unsigned int& mask)
{
  unsigned int bitPattern = getSamplingBit(rSample);
  return ( bitPattern & mask ) == bitPattern;
}

