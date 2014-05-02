//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARRAWCONDITIONS_LARCONDITIONSVSCALIBBASE_H
#define LARRAWCONDITIONS_LARCONDITIONSVSCALIBBASE_H

#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 

// class LArCablingService ;

/** Base class for LArConditionsVsCalib<> template
 *
 * @author M. Fanti
 * @version  0-0-1 ,  7/02/06
 *
 * History:
 *  - 
 */

class LArConditionsVsCalibBase
{
  
 public:

  LArConditionsVsCalibBase() : m_init(false), m_detStore(NULL), m_cablingSvc(NULL), m_onlineHelper(NULL) {} ;
  virtual ~LArConditionsVsCalibBase() {} ;

  virtual StatusCode initialize() ;
  // dummy method, only for compatibility with LArConditionsContainerBase
  void setGroupingType(int) {} ;

 protected:
  bool m_init ;
  StoreGateSvc* m_detStore ;
  LArCablingService* m_cablingSvc ;
  const LArOnlineID* m_onlineHelper ;

  HWIdentifier getCalibLine(HWIdentifier ChID) const ;
  //  HWIdentifier getSlotID   (HWIdentifier ChID) const ;
} ;


#endif


