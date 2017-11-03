/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARMPHYSOVERMCAL_H
#define LARCONDITIONS_ILARMPHYSOVERMCAL_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"

class ILArMphysOverMcal {
/** This class defines the interface for accessing MphysOverMcal
   * @stereotype Interface
   * @author S. Laplace
   * @version  0-0-1 , 01/07/2005
   *
   *
   */
  
 public: 

  virtual ~ILArMphysOverMcal() {};

  // online ID
  virtual const float& MphysOverMcal(const HWIdentifier& id , int gain)  const = 0 ;
  
  // offline ID 
  virtual const float& MphysOverMcal(const Identifier& id , int gain)  const =0;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};
};

CLASS_DEF( ILArMphysOverMcal,128308807,1) 

//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArMphysOverMcal> , 251728243 , 1 )

#endif 
