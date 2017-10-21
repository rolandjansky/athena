/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARMINBIAS_H
#define LARCONDITIONS_ILARMINBIAS_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"
class Identifier;
class HWIdentifier;


class ILArMinBias {
/** This class defines the interface for accessing 
      RMS of E in minimum bias events
   * @stereotype Interface
   */
  
 public: 

  virtual ~ILArMinBias() {};

  /** access to RMS of E in minimum bias events
      index by Identifier
  */ 

  // online ID
  virtual const float& minBiasRMS(const HWIdentifier& id)  const = 0 ;
  
  // offline ID 
  virtual const float& minBiasRMS(const Identifier& id)  const =0;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};  
} ;

CLASS_DEF( ILArMinBias, 197482938 ,1) 

#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArMinBias>,88954252 , 1 )



#endif 
