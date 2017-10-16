/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARPEDESTAL_H
#define LARELECCALIB_ILARPEDESTAL_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"

class ILArPedestal {

/** This class defines the interface for accessing Pedestal 
   * for each channel 
   * @stereotype Interface
   * @author H. Ma
   * @version  0-0-1 , 22/08/2002 
   *
   * Modifications:
   * W. Lampl, S. Laplace, 15/12/2003: 
   *        - Add method using online ID
   *        - Add 3rd argument isOffline
   * S. Laplace, 08/02/2004:
   *        - new online ID
   * W. Lampl Apr 2009
   * move from vector to single float
   */


public: 
  /** access to Pedestal 
      index by Identifier, and gain setting
  */ 

  virtual ~ILArPedestal() {};

  // online ID
  virtual float pedestal(const HWIdentifier& id, int gain ) const =0 ;
  
  // offline ID
  virtual float pedestal(const Identifier& id, int gain) const = 0 ;
  

  /** access to RMS of Pedestal 
      index by Identifier, and gain setting
  */ 
  // online ID
  virtual float pedestalRMS(const HWIdentifier& id, int gain ) const =0;

  // offline ID
  virtual float pedestalRMS(const Identifier& id, int gain) const = 0 ;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArPedestal, 8122,1) 

#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArPedestal>,161156680 , 1 )



#endif 
