// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ILVL1ConfigSvc.h 580466 2014-01-28 14:54:00Z krasznaa $
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
    * $Revision: 580466 $
    * $Date: 2014-01-28 15:54:00 +0100 (Tue, 28 Jan 2014) $
    */
   class ILVL1ConfigSvc : public IILVL1ConfigSvc,
                          virtual public IService {

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
