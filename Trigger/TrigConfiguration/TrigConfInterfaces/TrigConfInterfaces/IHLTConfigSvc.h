// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFINTERFACES_IHLTCONFIGSVC_H
#define TRIGCONFINTERFACES_IHLTCONFIGSVC_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IService.h"

// Local include(s):
#include "TrigConfInterfaces/IIHLTConfigSvc.h"

#include <string>

namespace TrigConf {

   // Forward declaration(s):
   class HLTChainList;
   class HLTSequenceList;

   /**
    * Athena interface for all services that provide HLT menu configuration
    * information.
    *
    * $Revision: 660352 $
    * $Date: 2015-04-14 01:03:15 +0200 (Tue, 14 Apr 2015) $
    */
   class IHLTConfigSvc : virtual public IService,
                         virtual public IIHLTConfigSvc
                         
   {

   public:
      /**@brief destructor*/
      virtual ~IHLTConfigSvc(){}

      /// Specify the interface declared by this class
      DeclareInterfaceID( IHLTConfigSvc, 0, 1 );

      // This interface is now empty (was used in Run-1&2)

   }; // class IHLTConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_IHLTCONFIGSVC_H
