/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARPHYSCALITDIFF_H
#define LARCONDITIONS_ILARPHYSCALITDIFF_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"

class Identifier;
class HWIdentifier;

class ILArPhysCaliTdiff {
/** This class defines the interface for accessing PhysCaliTdiff
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 21/07/2005
   *
   *
   */
  
 public: 

  virtual ~ILArPhysCaliTdiff() {};

  // online ID
  virtual const float& Tdiff(const HWIdentifier& id , int gain )  const = 0 ;
  
  // offline ID 
  virtual const float& Tdiff(const Identifier& id , int gain )  const =0;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};


} ;

CLASS_DEF( ILArPhysCaliTdiff,121911021,1) 


#endif 
