// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IL1TopoConfigSvc.h 580466 2014-01-28 14:54:00Z krasznaa $
#ifndef TRIGCONFINTERFACES_IL1TOPOCONFIGSVC_H
#define TRIGCONFINTERFACES_IL1TOPOCONFIGSVC_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IService.h"

// Forward declaration(s):
namespace TXC {
   class L1TopoMenu;
}

namespace TrigConf {

   /**
    * Athena interface for all services that provide L1Topo menu
    * configuration.
    *
    * $Revision: 580466 $
    * $Date: 2014-01-28 15:54:00 +0100 (Tue, 28 Jan 2014) $
    */
   class IL1TopoConfigSvc : virtual public IService {

   public:
      /// Destructor
      virtual ~IL1TopoConfigSvc() {}

      /// Specify the interface declared by this class
      DeclareInterfaceID( IL1TopoConfigSvc, 1, 0 );

      /// Provides the LVL1 Topo menu
      virtual const TXC::L1TopoMenu* menu() const = 0;

   }; // class IL1TopoConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_IL1TOPOCONFIGSVC_H
