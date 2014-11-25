// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBunchCrossingTool.h 620838 2014-10-09 11:38:00Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_XAODBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_XAODBUNCHCROSSINGTOOL_H

// System include(s):
#include <string>
#include <map>

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "TrigBunchCrossingTool/BunchConfig.h"

namespace Trig {

   /**
    * @short Bunch crossing tool to use with xAOD input files
    *
    * xAOD files usually have metadata in them that describes the bunch
    * structure of the events. This tool can be used to make use of this
    * metadata.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 620838 $
    * $Date: 2014-10-09 13:38:00 +0200 (Thu, 09 Oct 2014) $
    */
   class xAODBunchCrossingTool : public BunchCrossingToolBase {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( xAODBunchCrossingTool, Trig::IBunchCrossingTool )

   public:
      /// Default constructor
      xAODBunchCrossingTool( const std::string& name =
                                "xAODBunchCrossingTool" );

      /// Function initialising the tool
      virtual StatusCode initialize();

   protected:
      /// @name Functions inherited from AsgMetadataTool
      /// @{

      /// Function called at the beginning of each input file
      virtual StatusCode beginInputFile();
      /// Function called at the beginning of each event
      virtual StatusCode beginEvent();

      /// @}

   private:
      /// @name Tool properties
      /// @{

      /// Key of the metadata container
      std::string m_metaKey;
      /// Key of the event data object
      std::string m_eventKey;

      /// @}

      /// Type of the internal cache
      typedef std::map< unsigned int, BunchConfig > Cache_t;
      /// Cached configurations
      Cache_t m_cache;

      /// The currently loaded configuration ID
      unsigned int m_currentID;

   }; // class xAODBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_XAODBUNCHCROSSINGTOOL_H
