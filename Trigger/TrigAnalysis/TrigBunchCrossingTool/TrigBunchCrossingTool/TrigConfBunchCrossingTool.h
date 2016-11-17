// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfBunchCrossingTool.h 748399 2016-05-19 14:55:47Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#ifndef XAOD_STANDALONE
#   include "GaudiKernel/ServiceHandle.h"
#endif // not XAOD_STANDALONE
#include "AsgTools/ToolHandle.h"

// Trigger include(s):
#ifndef XAOD_STANDALONE
#   include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#endif // not XAOD_STANDALONE
#include "TrigConfInterfaces/ITrigConfigTool.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "TrigBunchCrossingTool/BunchCrossingConfProviderBase.h"

namespace Trig {

   /**
    * @short The trigger config implementation of IBunchCrossingTool
    *
    * This implementation should be used primarily to get information about the
    * bunch pattern for data files. It retrieves the bunch pattern configuration
    * using the trigger configuration service. So in principle it can itself
    * get the configuration from a few different places. (Currently
    * TrigConf::LVL1ConfigSvc and TrigConf::DSConfigSvc provide this
    * information.)
    *
    * It also acts as a bunch crossing configuration provider, implementing the
    * Trig::IBunchCrossingConfProvider interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 748399 $
    * $Date: 2016-05-19 16:55:47 +0200 (Thu, 19 May 2016) $
    */
   class TrigConfBunchCrossingTool : public BunchCrossingToolBase,
                                     public BunchCrossingConfProviderBase {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS2( TrigConfBunchCrossingTool,
                       Trig::IBunchCrossingTool,
                       Trig::IBunchCrossingConfProvider )

   public:
      /// Default constructor
      TrigConfBunchCrossingTool( const std::string& name =
                                 "TrigConfBunchCrossingTool" );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();

      /// Unique identifier for the current configuration
      virtual configid_type configID() const;
      /// Override the default implementation because this tool doesn't provide this info
      virtual std::vector< float > configuredIntensitiesBeam1() const;
      /// Override the default implementation because this tool doesn't provide this info
      virtual std::vector< float > configuredIntensitiesBeam2() const;
      /// Override the default implementation because this tool doesn't provide this info
      virtual std::vector< float > configuredUnpairedIntensitiesBeam1() const;
      /// Override the default implementation because this tool doesn't provide this info
      virtual std::vector< float > configuredUnpairedIntensitiesBeam2() const;

   protected:
      /// Function called when a new event is loaded
      virtual StatusCode beginEvent();

   private:
      /// Make sure that the latest/correct configuration is loaded
      StatusCode loadConfig();
      /// Print the "raw" configuration for debugging
      void printBunchGroups( const TrigConf::IILVL1ConfigSvc* svc ) const;

      /// DB ID of the BunchGroups settings which was loaded last
      unsigned int m_bgId;

#ifndef XAOD_STANDALONE
      /// The config service handle
      ServiceHandle< TrigConf::ILVL1ConfigSvc > m_configSvc;
#endif // not XAOD_STANDALONE

      /// The config tool handle
      ToolHandle< TrigConf::ITrigConfigTool > m_configTool;

   }; // class TrigConfBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H
