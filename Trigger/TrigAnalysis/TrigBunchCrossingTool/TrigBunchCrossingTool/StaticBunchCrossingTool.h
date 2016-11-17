// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticBunchCrossingTool.h 749252 2016-05-24 09:30:51Z krasznaa $
#ifndef TRIGBUNCCROSSINGTOOL_STATICBUNCHCROSSINGTOOL_H
#define TRIGBUNCCROSSINGTOOL_STATICBUNCHCROSSINGTOOL_H

// STL include(s):
#include <map>
#include <vector>

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"

namespace Trig {

   /**
    *  @short Stand-alone bunch group tool knowing some static configurations
    *
    *         This tool can be used out of the box to get information about some
    *         of the bunch layouts which were used to take data. It takes its
    *         knowledge from the "StaticConfigs.h" file.
    *
    *         It can be used in a standalone analysis code to analyse ntuples
    *         for instance.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 749252 $
    * $Date: 2016-05-24 11:30:51 +0200 (Tue, 24 May 2016) $
    */
   class StaticBunchCrossingTool : public BunchCrossingToolBase {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( StaticBunchCrossingTool, Trig::IBunchCrossingTool )

   public:
      /// Default constructor
      StaticBunchCrossingTool( const std::string& name =
                               "StaticBunchCrossingTool" );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Load a hard-coded bunch group key
      StatusCode loadConfig( int bgkey );
      /// Load a configuration specified by the user
      StatusCode loadConfig( const std::vector< int >& filledBunches,
                             const std::vector< float >& filledIntensities =
                             std::vector< float >(),
                             const std::vector< int >& unpairedBunches =
                             std::vector< int >(),
                             const std::vector< float >& unpairedIntensities =
                             std::vector< float >() );
      /// Load the configuration specified by the user
      StatusCode loadConfig( const std::vector< float >& bunches );

   private:
      /// Default key to be loaded
      int m_bgkey;

      /// All the hard-coded configs
      std::map< int, std::vector< int > > m_knownBGKeys;

   }; // class StaticBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCCROSSINGTOOL_STATICBUNCHCROSSINGTOOL_H
