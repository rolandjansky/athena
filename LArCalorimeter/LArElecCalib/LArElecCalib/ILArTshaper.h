/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARTSHAPER_H
#define LARCONDITIONS_ILARTSHAPER_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"
class Identifier;
class HWIdentifier;

class ILArTshaper {
/** This class defines the interface for accessing Tshaper
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 22/12/2005
   *
   *
   */
  
 public: 

  virtual ~ILArTshaper() {};

  // online ID
  virtual const float& Tshaper(const HWIdentifier& id)  const = 0 ;
  
  // offline ID 
  virtual const float& Tshaper(const Identifier& id)  const=0;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};
} ;

CLASS_DEF( ILArTshaper,134596750,1) 

#endif 
