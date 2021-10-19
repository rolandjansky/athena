/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARRINJ_H
#define LARCONDITIONS_ILARRINJ_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"
class HWIdentifier;

class ILArRinj {
/** This class defines the interface for accessing Rinj
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 22/12/2005
   *
   *
   */
  
 public: 

  virtual ~ILArRinj() {};

  // online ID
  virtual const float& Rinj(const HWIdentifier& id)  const = 0 ;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};
} ;

CLASS_DEF( ILArRinj,164401013,1)
//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArRinj>, 65913879, 1 ) 
#endif 
