/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFEBERRORSUMMARY_H
#define LARFEBERRORSUMMARY_H

#include "Identifier/HWIdentifier.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <stdint.h>
#include <map>


/** 
@class LArFebErrorSummary
@brief Holds information from the FEB Error Summary

@author Hong Ma

*/

class LArFebErrorSummary 
{
 public: 
  
  enum LArFebErrorType{
    Parity,BCID,SampleHeader,EVTID,ScacStatus,ScaOutOfRange,
    GainMismatch,TypeMismatch,NumOfSamples,EmptyDataBlock,DspBlockSize,CheckSum, MissingHeader, BadGain, N_LArFebErrorType
  } ;

  /** @brief Constructor with FEB Id*/
  LArFebErrorSummary() ;

  /** @brief Destructor */
  ~LArFebErrorSummary() ;

  /** @brief Add another FEB error record, return true if added successfully */
  bool  set_feb_error(unsigned int febid,uint16_t e);

  /** @brief get error for feb  */
  uint16_t feb_error(HWIdentifier febid) const ; 

  /** @brief get all febs with error */ 
  const std::map<unsigned int,uint16_t>& get_all_febs() const ; 

  /** @brief interpret the error in string */     
  static std::string error_to_string( uint16_t error) ; 

  /** @brief clear internal map  */     
  void clear( )  ; 

 private:

  /** @brief  error is stored in a map with key = feb id */
  std::map<unsigned int,uint16_t> m_feb_errors; 

  static std::string  m_errorStrings[N_LArFebErrorType] ; 
};

CLASS_DEF(LArFebErrorSummary,223531796,0)

#endif
