// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticBunchCrossingTool.h 618331 2014-09-24 11:55:26Z krasznaa $
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
    * $Revision: 618331 $
    * $Date: 2014-09-24 13:55:26 +0200 (Wed, 24 Sep 2014) $
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

   private:
      /// Default key to be loaded
      int m_bgkey;

      /// All the hard-coded configs
      std::map< int, std::vector< int > > m_knownBGKeys;

   }; // class StaticBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCCROSSINGTOOL_STATICBUNCHCROSSINGTOOL_H
