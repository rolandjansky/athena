/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILAREMEC_CPHI_H
#define LARCONDITIONS_ILAREMEC_CPHI_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"


class ILArEMEC_Cphi {
/** This class defines the interface for accessing EMEC_Cphi
   * @stereotype Interface
   * @author M. Fanti
   * @version  0-0-1 , 22/12/2005
   *
   *
   */
  
 public: 

  virtual ~ILArEMEC_Cphi() {};

  // online ID
  virtual const float& EMEC_Cphi(const HWIdentifier& id)  const = 0 ;
  
  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArEMEC_Cphi,64028922,1) 

#endif 
