/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RAWDATA_P1_H
#define LUCID_RAWDATA_P1_H

class LUCID_RawData_p1 {
  
 public:
  
  LUCID_RawData_p1(): 
    m_word0 (0),
    m_word1 (0), 
    m_word2 (0), 
    m_word3 (0), 
    m_word0p(0),
    m_word1p(0), 
    m_word2p(0), 
    m_word3p(0), 
    m_word0n(0),
    m_word1n(0), 
    m_word2n(0), 
    m_word3n(0), 
    m_status(0) {}
    
    friend class LUCID_RawDataCnv_p1;
    
 private:
    
    unsigned int m_word0;
    unsigned int m_word1;
    unsigned int m_word2;
    unsigned int m_word3;
    unsigned int m_word0p;
    unsigned int m_word1p;
    unsigned int m_word2p;
    unsigned int m_word3p;
    unsigned int m_word0n;
    unsigned int m_word1n;
    unsigned int m_word2n;
    unsigned int m_word3n;
    unsigned int m_status;
};

#endif
