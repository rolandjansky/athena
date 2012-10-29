// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StaticBunchCrossingToolSA.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCCROSSINGTOOL_STATICBUNCHCROSSINGTOOLSA_H
#define TRIGBUNCCROSSINGTOOL_STATICBUNCHCROSSINGTOOLSA_H

// STL include(s):
#include <map>
#include <vector>

// Local include(s):
#include "BunchCrossingToolBase.h"

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
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   class StaticBunchCrossingToolSA : public virtual BunchCrossingToolBase {

   public:
      /// Default constructor
      StaticBunchCrossingToolSA();

      /// Load a hard-coded bunch group key
      bool loadConfig( int bgkey );
      /// Load a configuration specified by the user
      bool loadConfig( const std::vector< int >& filledBunches,
                       const std::vector< float >& filledIntensities =
                       std::vector< float >(),
                       const std::vector< int >& unpairedBunches =
                       std::vector< int >(),
                       const std::vector< float >& unpairedIntensities =
                       std::vector< float >() );

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

   private:
      std::map< int, std::vector< int > > m_knownBGKeys; ///< All the hard-coded configs

   }; // class StaticBunchCrossingToolSA

} // namespace Trig

#endif // TRIGBUNCCROSSINGTOOL_STATICBUNCHCROSSINGTOOLSA_H
