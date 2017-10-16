/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARUA2MEV_H
#define LARCONDITIONS_ILARUA2MEV_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"

class ILAruA2MeV {
/** This class defines the interface for accessing uA2MeV
   * @stereotype Interface
   * @author S. Laplace
   * @version  0-0-1 , 16/01/2004
   *
   * History:
   *  - 08/02/2004, S. Laplace: new online ID
   *  - 29/04/2004, S. Laplace: added ERRORCODE
   *
   */
  
 public: 
  /** access to UA2MEV conversion factor
      index by Identifier, and gain setting
  */ 
  // online ID

  virtual ~ILAruA2MeV() {};

  virtual const float& UA2MEV(const HWIdentifier& id )  const = 0 ;
  
  // offline ID 
  virtual const float& UA2MEV(const Identifier& id )  const =0;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILAruA2MeV,154639332,1) 


//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILAruA2MeV> , 263271148 , 1 )

#endif 
