// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigConfigSvc.h 581397 2014-02-04 13:59:56Z stelzer $
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
    * $Revision: 581397 $
    * $Date: 2014-02-04 14:59:56 +0100 (Tue, 04 Feb 2014) $
    */
   class ITrigConfigSvc :
      virtual public extend_interfaces3< IL1TopoConfigSvc,
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
