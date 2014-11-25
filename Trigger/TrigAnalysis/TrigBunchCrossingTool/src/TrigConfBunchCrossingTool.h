// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfBunchCrossingTool.h 618331 2014-09-24 11:55:26Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// Trigger include(s):
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "BunchCrossingConfProviderBase.h"

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
    * $Revision: 618331 $
    * $Date: 2014-09-24 13:55:26 +0200 (Wed, 24 Sep 2014) $
    */
   class TrigConfBunchCrossingTool : public virtual IIncidentListener,
                                     public BunchCrossingToolBase,
                                     public BunchCrossingConfProviderBase {

   public:
      /// Standard AlgTool constructor
      TrigConfBunchCrossingTool( const std::string& type, const std::string& name,
                                 const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();

      /// Function called when a registered incident happens
      virtual void handle( const Incident& inc );

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

   private:
      /// Make sure that the latest/correct configuration is loaded
      StatusCode loadConfig();
      /// Print the "raw" configuration for debugging
      void printBunchGroups() const;

      unsigned int m_bgId; ///< DB ID of the BunchGroups settings which was loaded last

      /// Possible names for filled bunch groups
      std::vector< std::string > m_filledBunchNames;

      ServiceHandle< TrigConf::ILVL1ConfigSvc > m_configSvc; ///< The config service handle
      ServiceHandle< IIncidentSvc > m_incidentSvc; ///< The incident service handle

   }; // class TrigConfBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H
