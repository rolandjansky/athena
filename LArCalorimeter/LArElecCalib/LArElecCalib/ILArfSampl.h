/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARFSAMPL_H
#define LARCONDITIONS_ILARFSAMPL_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"


class ILArfSampl {
/** This class defines the interface for accessing fSampling
   * @stereotype Interface
   * @author S. Laplace
   * @version  0-0-1 , 21/01/2004
   *
   * History:
   *  - 08/02/2004, S. Laplace: new online ID
   *  - 29/04/2004, S. Laplace: added ERRORCODE
   *
   */
  
 public: 
  /** access to FSAMPL conversion factor
      index by Identifier, and gain setting
  */ 

  virtual ~ILArfSampl() {};

  // online ID
  virtual const float& FSAMPL(const HWIdentifier& id )  const = 0 ;
  
  // offline ID 
  virtual const float& FSAMPL(const Identifier& id )  const=0;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};
  
} ;

CLASS_DEF( ILArfSampl,128126607,1) 

//ConditionsContainer clid for athenaMT
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArfSampl>, 155688745, 1 )



#endif 
