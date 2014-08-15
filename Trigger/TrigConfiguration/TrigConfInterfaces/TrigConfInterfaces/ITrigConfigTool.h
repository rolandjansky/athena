// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigConfigTool.h 612008 2014-08-15 13:35:58Z krasznaa $
#ifndef TRIGCONFINTERFACES_ITRIGCONFIGTOOL_H
#define TRIGCONFINTERFACES_ITRIGCONFIGTOOL_H

// Infrastructure include(s):
#include "AsgTools/IAsgTool.h"

// Local include(s):
#include "TrigConfInterfaces/IILVL1ConfigSvc.h"
#include "TrigConfInterfaces/IIHLTConfigSvc.h"

namespace TrigConf {

   /**
    * Interface for dual-use tools that provide trigger configuration
    * information.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 612008 $
    * $Date: 2014-08-15 15:35:58 +0200 (Fri, 15 Aug 2014) $
    */
   class ITrigConfigTool : public virtual asg::IAsgTool,
                           public virtual IILVL1ConfigSvc,
                           public virtual IIHLTConfigSvc {

      /// Declare the interface that this class provides
      ASG_TOOL_INTERFACE( TrigConf::ITrigConfigTool )

   }; // ITrigConfigTool

} // namespace TrigConf

#endif // TRIGCONFINTERFACES_ITRIGCONFIGTOOL_H
