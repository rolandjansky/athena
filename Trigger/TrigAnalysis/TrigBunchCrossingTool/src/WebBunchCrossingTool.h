// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: WebBunchCrossingTool.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

// Local include(s):
#include "TrigBunchCrossingTool/WebBunchCrossingToolSA.h"

// Forward declaration(s):
class EventID;

namespace Trig {

   /**
    *  @short A "backup tool" reading the appropriate configuration from the web
    *
    *         This tool was developed as a backup for the case when neither
    *         TrigConfBunchCrossingTool or LHCBunchCrossingTool can be used for instance
    *         because of some COOL/Frontier problems. In this case this tool
    *         should be able to replace these preferred implementations.
    *
    *         The tool inherits from the stand-alone WebBunchCrossingToolSA,
    *         the only functionality which is added to this Athena version of the
    *         tool is that it automatically loads the correct configuration for all
    *         the events.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $ 
    */
   class WebBunchCrossingTool : public AthAlgTool,
                                public virtual IBunchCrossingTool,
                                public virtual IIncidentListener,
                                public virtual WebBunchCrossingToolSA {

   public:
      /// Standard AlgTool constructor
      WebBunchCrossingTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();
      /// Regular AlgTool finalization function
      virtual StatusCode finalize();

      /// Function called when a registered incident happens
      virtual void handle( const Incident& inc );

   private:
      /// Function setting the maximum bunch spacing parameter
      virtual void setMaxBunchSpacing( int spacing );
      /// Get the maximum bunch spacing parameter
      virtual int maxBunchSpacing() const;

      /// Function setting the length of the front of the bunch train
      virtual void setFrontLength( int length );
      /// Get the length of the front of the bunch train
      virtual int frontLength() const;
      /// Function setting the length of the tail of the bunch train
      virtual void setTailLength( int length );
      /// Get the length of the tail of the bunch train
      virtual int tailLength() const;

      /// Make sure that the latest/correct configuration is loaded
      StatusCode load( const EventID* event );

      int m_maxBunchSpacing; ///< The maximum bunch spacing that the tool should consider
      int m_frontLength; ///< Length of the "front" of a bunch train
      int m_tailLength; ///< Length of the "tail" of a bunch train

      unsigned int m_lastRun; ///< Last run for which the configuration was loaded
      unsigned int m_lastLB; ///< Last lumiblock for which the connfiguration was loaded

      ServiceHandle< IIncidentSvc > m_incidentSvc; ///< The incident service handle

   }; // class WebBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_WEBBUNCHCROSSINGTOOL_H
