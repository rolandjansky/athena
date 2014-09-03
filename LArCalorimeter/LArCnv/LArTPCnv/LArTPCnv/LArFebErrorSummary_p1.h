/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARFEBERRORSUMMARY_P1_H
#define LARTPCNV_LARFEBERRORSUMMARY_P1_H

/** @class LArFebErrorSummary_p1
@brief persistent class for LArFebErrorSummary, version p2.
*/

#include  <vector> 

class LArFebErrorSummary_p1 {
  
 public:
  
  LArFebErrorSummary_p1() { };
  
  friend class LArFebErrorSummaryCnv_p1;
    
 private:
  

  std::vector<unsigned int> m_feb_ids;
  std::vector<uint16_t>     m_feb_errors;

};

#endif
