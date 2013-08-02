/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARDETCELLPARAMS_H
#define LARCONDITIONS_ILARDETCELLPARAMS_H

#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CLIDSvc/CLASS_DEF.h" 


class ILArDetCellParams {
/** This class defines the interface for accessing DetCellParams
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 21/07/2005
   *
   *
   */
  
 public: 

  virtual ~ILArDetCellParams() {};

  // online ID
  virtual const float& Omega0(const HWIdentifier& id , int gain )  const = 0 ;
  virtual const float& Taur(const HWIdentifier& id , int gain )  const = 0 ;
  
  // offline ID 
  virtual const float& Omega0(const Identifier& id , int gain )  const =0;
  virtual const float& Taur(const Identifier& id , int gain )  const =0;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};
  
} ;

CLASS_DEF( ILArDetCellParams,155926678,1) 

#endif 
