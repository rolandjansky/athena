// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigConfigSvc.h 660352 2015-04-13 23:03:15Z stelzer $
#ifndef TRIGCONFINTERFACES_ITRIGCONFIGSVC_H
#define TRIGCONFINTERFACES_ITRIGCONFIGSVC_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/extend_interfaces.h"

// Local include(s):
#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"

namespace TrigConf {

   /**
    * Athena interface for all service that provide L1Topo, LVL1, and HLT
    * menu configuration information
    *
    * Note that this is used in particular to retrieve config info in an
    * offline analysis.
    *
    * $Revision: 660352 $
    * $Date: 2015-04-14 01:03:15 +0200 (Tue, 14 Apr 2015) $
    */
   class ITrigConfigSvc : public extend_interfaces3< IL1TopoConfigSvc,
                                                     ILVL1ConfigSvc,
                                                     IHLTConfigSvc > {

   public:
      /**@brief destructor*/
      virtual ~ITrigConfigSvc(){}

      /// Specify the interface declared by this class
      DeclareInterfaceIDMultiBase( ITrigConfigSvc, 0, 1 );

   }; // class ITrigConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_ITRIGCONFIGSVC_H
