/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARCABLELENGTH_H
#define LARCONDITIONS_ILARCABLELENGTH_H


#include "AthenaKernel/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"
class HWIdentifier;

class ILArCableLength {
/** This class defines the interface for accessing CableLength
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 22/12/2005
   *
   *
   */
  
 public: 

  virtual ~ILArCableLength() {};

  // online ID
  virtual const float& CableLength(const HWIdentifier& id)  const = 0 ;
 
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArCableLength,230982210,1) 

#endif 
