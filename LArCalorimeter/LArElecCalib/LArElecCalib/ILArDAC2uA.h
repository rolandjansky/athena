/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARDAC2UA_H
#define LARCONDITIONS_ILARDAC2UA_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"

class ILArDAC2uA {
/** This class defines the interface for accessing DAC2uA
   * @stereotype Interface
   * @author S. Laplace
   * @version  0-0-1 , 16/01/2004
   *
   * The DAC2UA (DAC to microAmp) factor is the product of:
   *          - DAC2VOLT = constant for the whole em calo
   *          - VOLT2UA = function of Rinj, measured for each cell
   * History:
   *  - 08/02/2004, S. Laplace: new online ID
   *  - 29/04/2004, S. Laplace: added ERRORCODE
   */
  
 public: 
  
  virtual ~ILArDAC2uA() {};

  /** access to DAC2UA conversion factor
      index by Identifier, and gain setting
  */ 
  // online ID
  virtual const float& DAC2UA(const HWIdentifier& id )  const = 0 ;
  
  // offline ID 
  virtual const float& DAC2UA(const Identifier& id )  const =0;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArDAC2uA,579584,1) 

//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArDAC2uA> , 24734384 , 1 )

#endif 

