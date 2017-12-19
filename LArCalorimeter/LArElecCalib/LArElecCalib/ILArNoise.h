/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARNOISE_H
#define LARELECCALIB_ILARNOISE_H

#include "CLIDSvc/CLASS_DEF.h" 
#include "LArElecCalib/LArCalibErrorCode.h"
class Identifier;
class HWIdentifier;

class ILArNoise {
/** This class defines the interface for accessing Noise
   * parameters for each channel 
   * @stereotype Interface
   * @author H. Ma
   * @version  0-0-1 , 22/08/2002 
   *
   * Modifications:
   * W. Lampl, S. Laplace, 08/01/2004
   *        - Add method using online ID
   *        - Add 3rd argument isOffline
   * S. Laplace, 08/02/2004
   *        - New online ID
   * S. Laplace: 29/04/2004, 
   *        - added ERRORCODE
   */

public: 
  /** access to noise 
	index by Identifier, and gain setting
  */ 
  // online ID

  virtual ~ILArNoise() {};

  virtual const float& noise( const HWIdentifier& id, int gain) const =0 ;

  virtual const float& noise( const Identifier& id, int gain ) const = 0 ;

  enum {ERRORCODE = LArElecCalib::ERRORCODE};

} ;

CLASS_DEF( ILArNoise, 8125,1) 

//ConditionsContainer clid for athenaMT"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<ILArNoise> , 148605725 , 1 )

#endif 
