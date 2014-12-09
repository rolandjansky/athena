/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARTIMING_H
#define LARELECCALIB_ILARTIMING_H

#include "Identifier/Identifier.h" 
#include "CLIDSvc/CLASS_DEF.h"

#include <vector> 

class ILArTiming {
  /** This class defines the interface for accessing Timing data
   *  e.g. cubic peak time, digital filtering synchronization,
   *  which is required by HEC+EMEC 2002 TB.
   *
   * @stereotype interface
   * @author N. Kanaya
   * @version  0-0-1 , 24/04/2003
   */
  
 public: 
  virtual ~ILArTiming() {};

  virtual float GlobalTime(const Identifier& id, int gain) const=0;
  virtual float Time(const Identifier& id, int gain)       const=0;
  virtual float Error(const Identifier& id, int gain)      const=0;
} ;

CLASS_DEF(ILArTiming ,12383934,1)
#endif  // LARELECCALIB_ILARTIMING_H
