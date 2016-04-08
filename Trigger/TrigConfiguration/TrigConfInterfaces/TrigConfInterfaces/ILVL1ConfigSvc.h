// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ILVL1ConfigSvc.h 660352 2015-04-13 23:03:15Z stelzer $
#ifndef TRIGCONFINTERFACES_ILVL1CONFIGSVC_H
#define TRIGCONFINTERFACES_ILVL1CONFIGSVC_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IService.h"

// Local include(s):
#include "TrigConfInterfaces/IILVL1ConfigSvc.h"

namespace TrigConf {

   // Forward declaration(s):
   class Muctpi;

   /**
    * Athena interface for all services that provide LVL1 menu configuration
    * information.
    *
    * $Revision: 660352 $
    * $Date: 2015-04-14 01:03:15 +0200 (Tue, 14 Apr 2015) $
    */
   class ILVL1ConfigSvc : virtual public IService, 
                          virtual public IILVL1ConfigSvc
   {
   public:
      /**@brief destructor*/
      virtual ~ILVL1ConfigSvc(){}

      /// Specify the interface declared by this class
      DeclareInterfaceID( ILVL1ConfigSvc, 0, 1 );

      /**@brief Provides the MuCTPI configuration*/
      virtual const Muctpi* muctpiConfig() const = 0;

   }; // class ILVL1ConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_ILVL1CONFIGSVC_H
