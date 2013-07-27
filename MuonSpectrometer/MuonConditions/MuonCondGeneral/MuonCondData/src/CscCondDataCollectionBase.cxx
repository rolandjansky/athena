/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
        @class CscCondDataCollectionBase

        @author lampen@physics.arizona.edu

        @brief Base class for CscCondDataCollection.. 

        Exists so that CscCondDataCollection objects of different types can be stored in the same vector, map, etc. 
 **/
#include "MuonCondData/CscCondDataCollectionBase.h"

namespace MuonCalib {
  CscCondDataCollectionBase::CscCondDataCollectionBase() : 
    m_parName(""), 
    m_parFolder(""),   
    m_parSGKey(""), 
    m_parCat (""),   
    m_numCoolChan(0),  
    m_size(0),
    m_numBits(0),
    m_bitOffset(0)
  {}

  CscCondDataCollectionBase::~CscCondDataCollectionBase() {}

  int CscCondDataCollectionBase::setBitInfo(int numBits, int offset)
  {
    m_numBits = numBits;
    m_bitOffset = offset; 
    return 1;
  }
}
