/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDITIONS_ILARRAMP_H
#define LARCONDITIONS_ILARRAMP_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArVectorProxy.h"

class ILArRamp {
/** This class defines the interface for accessing Ramp
   * @stereotype Interface
   * @author H. Ma
   * @version  0-0-1 , 22/08/2002 
   *
   * Modifications:
   * W. Lampl, S. Laplace, 10/12/2003: 
   *        - Add method using online ID
   *        - Add 3rd argument isOffline
   * S. Laplace, 16/01/2004: WARNING -> IMPORTANT CHANGE !
   *        - ILArRamp now contains the ADC to DAC conversion factor only
   *        (not ADC to GeV anymore). 
   * S. Laplace, 08/02/2004
   *        - new online ID
   */
  
 public: 

  typedef LArVectorProxy RampRef_t;

  virtual ~ILArRamp() {};

  virtual RampRef_t ADC2DAC(const HWIdentifier& id, int gain )  const =0;
  virtual RampRef_t ADC2DAC(const Identifier& id, int gain )  const =0;
  
} ;

CLASS_DEF( ILArRamp,8123,1) 



//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArRamp> , 39560902 , 1 )

#endif 
