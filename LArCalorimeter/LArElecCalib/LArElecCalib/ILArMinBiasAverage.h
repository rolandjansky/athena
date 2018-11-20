/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARMINBIASAVERAGE_H
#define LARCONDITIONS_ILARMINBIASAVERAGE_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"
class HWIdentifier;


class ILArMinBiasAverage {
/** This class defines the interface for accessing 
      Average of E in minimum bias events
   * @stereotype Interface
   */
  
 public: 

  virtual ~ILArMinBiasAverage() {};

  /** access to average of E in minimum bias events
      index by Identifier
  */ 

  // online ID
  virtual const float& minBiasAverage(const HWIdentifier& id)  const = 0 ;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};  
} ;
#include "AthenaKernel/CondCont.h"
CLASS_DEF( ILArMinBiasAverage, 112216056 ,1) 
CLASS_DEF( CondCont<ILArMinBiasAverage>,56829592 , 1 )

#endif 
