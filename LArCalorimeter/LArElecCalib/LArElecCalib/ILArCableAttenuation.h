/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARCABLEATTENUATION_H
#define LARCONDITIONS_ILARCABLEATTENUATION_H

#include "Identifier/HWIdentifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "AthenaKernel/CLASS_DEF.h" 

#include <vector> 

class ILArCableAttenuation {
/** This class defines the interface for accessing CableAttenuation
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 22/12/2005
   *
   *
   */
  
 public: 

  virtual ~ILArCableAttenuation() {}; 
  
  // online ID
  virtual const float& CableAttenuation(const HWIdentifier& id)  const = 0 ;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};  
} ;

CLASS_DEF( ILArCableAttenuation,66767159,1) 

#endif 
