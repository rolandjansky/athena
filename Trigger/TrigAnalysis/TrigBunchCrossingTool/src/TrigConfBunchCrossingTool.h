// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfBunchCrossingTool.h 511865 2012-07-31 08:44:12Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Trigger include(s):
#include "TrigConfigSvc/ILVL1ConfigSvc.h"

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "BunchCrossingConfProviderBase.h"

namespace Trig {

   /**
    *  @short The trigger config implementation of the IBunchCrossingTool interface
    *
    *         This implementation should be used primarily to get information about the
    *         bunch pattern for data files. It retrieves the bunch pattern configuration
    *         using the trigger configuration service. So in principle it can itself
    *         get the configuration from a few different places. (Currently
    *         TrigConf::LVL1ConfigSvc and TrigConf::DSConfigSvc provide this information.)
    *
    *         It also acts as a bunch crossing configuration provider, implementing the
    *         Trig::IBunchCrossingConfProvider interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 511865 $
    * $Date: 2012-07-31 10:44:12 +0200 (Tue, 31 Jul 2012) $
    */
   class TrigConfBunchCrossingTool : public AthAlgTool,
                                     public virtual IBunchCrossingTool,
                                     public virtual IIncidentListener,
                                     public virtual BunchCrossingToolBase,
                                     public virtual BunchCrossingConfProviderBase {

   public:
      /// Standard AlgTool constructor
      TrigConfBunchCrossingTool( const std::string& type, const std::string& name,
                                 const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();
      /// Regular AlgTool finalization function
      virtual StatusCode finalize();

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

      int m_bgId; ///< DB ID of the BunchGroups settings which was loaded last

      int m_maxBunchSpacing; ///< The maximum bunch spacing that the tool should consider
      std::vector< std::string > m_filledBunchNames; ///< Possible names for filled bunch groups
      int m_frontLength; ///< Length of the "front" of a bunch train
      int m_tailLength; ///< Length of the "tail" of a bunch train

      ServiceHandle< TrigConf::ILVL1ConfigSvc > m_configSvc; ///< The config service handle
      ServiceHandle< IIncidentSvc > m_incidentSvc; ///< The incident service handle

   }; // class TrigConfBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_TRIGCONFBUNCHCROSSINGTOOL_H
