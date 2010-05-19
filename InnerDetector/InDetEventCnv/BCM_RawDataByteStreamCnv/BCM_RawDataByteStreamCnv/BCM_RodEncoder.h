/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BCM_RodEncoder.h
//   Header file for class BCM_RodEncoder
///////////////////////////////////////////////////////////////////
// (c) ATLAS BCM Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//  Version 00-00-01 12/05/2008 Daniel Dobos
//  Version 00-00-11 05/02/2009 Daniel Dobos
///////////////////////////////////////////////////////////////////

#ifndef BCM_RAWDATABYTESTREAMCNV_BCM_RODENCODER_H
#define BCM_RAWDATABYTESTREAMCNV_BCM_RODENCODER_H

#include <inttypes.h>

#include "InDetBCM_RawData/InDetBCM_RawDataCLASS_DEF.h"

class BCM_RodEncoder 
{
public: 
 
  typedef BCM_RawData RDO;
  typedef BCM_RDO_Container BCM_RawContainer; 
  typedef std::vector<const RDO*> VRDO ;

  // constructor
  BCM_RodEncoder();

  // destructor
  ~BCM_RodEncoder();

  // addRdo
  void addRdo(const RDO * rdo) {
    m_RDOs.push_back(rdo);
  }

  // setRodMinorVersion 
  void setRodMinorVersion (unsigned short RodBlockVersion) {
    m_RodBlockVersion = RodBlockVersion;
  }

  // fill the Rod  
  void fillROD(std::vector<uint32_t>& v, int BCs_per_LVL1ID); 

private:

  unsigned int encode_data_block(std::vector<uint32_t>& data_block, unsigned int BCID, unsigned int Pulse1Position[8], unsigned int Pulse1Width[8], unsigned int Pulse2Position[8], unsigned int Pulse2Width[8], unsigned int Error);

  unsigned int getDataword_position(int ChannelID);

  unsigned short m_RodBlockVersion;
  VRDO m_RDOs;

}; 

#endif
