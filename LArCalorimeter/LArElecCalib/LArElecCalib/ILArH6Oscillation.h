/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARH6OSCILLATION_H
#define LARELECCALIB_ILARH6OSCILLATION_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "LArElecCalib/LArCalibErrorCode.h"


class ILArH6Oscillation {

/** This class defines the interface for accessing channel phase and channel amplitues values
   * for each channel, needed to correct for the HEC oscillating noise in 2004 H6 combined testbeam 
   * @stereotype Interface
   * @author T. Barillari
   * @version  \$Id: ILArH6Oscillation.h,v 1.2 2008-09-27 10:23:37 wlampl Exp $*/

public: 
  /** access to channel phase  
  *   index by Identifier
  */ 

  virtual ~ILArH6Oscillation() {};

  // online ID
  virtual  const double& channelPhase(const HWIdentifier& id) const =0;
  

  /** access to channel amplitude
  *   index by Identifier
  */ 
  // online ID
  virtual const double& channelAmplitude(const HWIdentifier& id) const =0;

} ;

CLASS_DEF( ILArH6Oscillation, 55993376,1) 

#endif 
