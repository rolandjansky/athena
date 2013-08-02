/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARCALIPULSEPARAMS_H
#define LARCONDITIONS_ILARCALIPULSEPARAMS_H

#include "Identifier/HWIdentifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CLIDSvc/CLASS_DEF.h" 

#include <vector> 

class ILArCaliPulseParams {
/** This class defines the interface for accessing CaliPulseParams
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 21/07/2005
   *
   *
   */
  
 public: 
  virtual ~ILArCaliPulseParams() {};

  // online ID
  virtual const float& Tcal(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& Fstep(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& Offset(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& dTimeCal(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const short& nCB(const HWIdentifier& id , int gain )  const = 0 ;
  
  /*
  // offline ID 
  virtual const float& Tcal(const Identifier& id , int gain )  const=0;
  virtual const float& Fstep(const Identifier& id , int gain )  const=0;
  virtual const float& Offset(const Identifier& id , int gain )  const=0;
  virtual const float& dTimeCal(const Identifier& id , int gain )  const=0;
  virtual const short& nCB(const Identifier& id , int gain )  const=0;
  */

  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArCaliPulseParams,73129415,1) 

#endif 
