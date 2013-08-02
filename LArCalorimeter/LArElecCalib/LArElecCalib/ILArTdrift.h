/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARTDRIFT_H
#define LARCONDITIONS_ILARTDRIFT_H

class Identifier;
class HWIdentifier;
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CLIDSvc/CLASS_DEF.h" 

class ILArTdrift {
/** This class defines the interface for accessing Tdrift
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 21/07/2005
   *
   *
   */
  
 public: 

  virtual ~ILArTdrift() {};

  // online ID
  virtual const float& Tdrift(const HWIdentifier& id )  const = 0 ;
  
  // offline ID 
  virtual const float& Tdrift(const Identifier& id )  const =0;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArTdrift,174525073,1) 

#endif
