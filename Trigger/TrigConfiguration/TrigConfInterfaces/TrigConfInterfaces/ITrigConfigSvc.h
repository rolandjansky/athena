// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
    */
   class ITrigConfigSvc : public extend_interfaces< IL1TopoConfigSvc,
                                                    ILVL1ConfigSvc,
                                                    IHLTConfigSvc > {

   public:
      /**@brief destructor*/
      virtual ~ITrigConfigSvc(){}

      /// Specify the interface declared by this class
      DeclareInterfaceID( ITrigConfigSvc, 0, 1 );

      virtual std::string configurationSource() const = 0;

   }; // class ITrigConfigSvc

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_ITRIGCONFIGSVC_H
