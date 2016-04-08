// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IBunchCrossingConfProvider.h 618301 2014-09-24 08:53:05Z krasznaa $
#ifndef TRIGANALYSISINTERFACES_IBUNCHCROSSINGCONFPROVIDER_H
#define TRIGANALYSISINTERFACES_IBUNCHCROSSINGCONFPROVIDER_H

// STL include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "AsgTools/IAsgTool.h"

namespace Trig {

   /**
    * @short Inteface for providing the bunch structure information
    *
    * To make it simpler for Athena components to save the bunch
    * structure information in a uniform way, they should use this
    * interface. The bunch structure configuration can come from many
    * different places. Since the package already provides code for
    * interpreting all these sources, it makes sense that analysts
    * wouldn't have to re-do all this themselves.
    *
    * The idea is that the information that this interface provides has
    * to be enough for fully configuring any version (especially the
    * standalone one(s)) of the tool. The main use case at the moment
    * being, that if someone saves this configuration into a D3PD, it
    * will become possible to use a standalone version of the bunch
    * crossing tool to analyse the D3PD in the same way as the analysis
    * is done in Athena.
    *
    * This interface is not split into an Athena-dependent and
    * Athena-independent class, as I don't think this functionality
    * would be useful outside Athena. But who knows...
    *
    * All Athena tools that implement this interface have to fire an
    * incident of type "BunchConfig" when their configuration is
    * updated. Unfortunately I can't create a static member with this
    * type name, because then the interface would stop being pure
    * virtual... :-( I could define this incident type name as a
    * pre-processor macro, but I don't like that solution. :-P
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 618301 $
    * $Date: 2014-09-24 10:53:05 +0200 (Wed, 24 Sep 2014) $
    */
   class IBunchCrossingConfProvider : public virtual asg::IAsgTool {

      /// Declare the interface that this class provides
      ASG_TOOL_INTERFACE( Trig::IBunchCrossingConfProvider )

   public:
      /// Type for the unique identifier
      typedef unsigned int configid_type;

      /// Unique identifier for the current configuration
      /**
       * This is a tricky part of the interface. In order to be able to save
       * the bunch pattern configuration for many different setups in a
       * meaningful way, we have to uniquely identify each differing
       * configuration.
       *
       * For some backends this is easy. When reading the configuration from
       * the trigger database, the unique identifier can be the bunch group key
       * (BGK). But when reading the configuration of a MC file, the situation
       * is a bit more tricky.
       *
       * In the end the only requirement with this function is that it should
       * return the same ID for two identical configurations, and that it should
       * return a differiod ID with a "high enough" period for different
       * configurations.
       *
       * @returns A unique identifier for the currently loaded configuration
       */
      virtual configid_type configID() const = 0;

      /// Function providing the BCIDs of the configured colliding bunches
      /**
       * This function returns the BCIDs of the colliding bunches in the current
       * configuration.
       *
       * @returns The BCIDs of the colliding bunches
       */
      virtual std::vector< int > configuredBCIDs() const = 0;

      /// Function providing the intensities of the configured colliding bunches
      /**
       * This function returns the "intensities" of the colliding bunches. It
       * always gives the "intensities" in the same order as the
       * configuredBCIDs() function returns the BCIDs in. The returned
       * values may be context specific.
       *
       * @returns The "intensities" of the colliding bunches (from beam 1)
       */
      virtual std::vector< float > configuredIntensitiesBeam1() const = 0;

      /// Function providing the intensities of the configured colliding bunches
      /**
       * This function returns the "intensities" of the colliding bunches. It
       * always gives the "intensities" in the same order as the
       * configuredBCIDs() function returns the BCIDs in. The returned
       * values may be context specific.
       *
       * @returns The "intensities" of the colliding bunches (from beam 2)
       */
      virtual std::vector< float > configuredIntensitiesBeam2() const = 0;

      /// Function providing the BCIDs of the configured unpaired bunches
      /**
       * This function returns the BCIDs of the unpaired bunches in the current
       * configuration.
       *
       * @returns The BCIDs of the unpaired bunches from beam 1
       */
      virtual std::vector< int > configuredUnpairedBCIDsBeam1() const = 0;

      /// Function providing the BCIDs of the configured unpaired bunches
      /**
       * This function returns the BCIDs of the unpaired bunches in the current
       * configuration.
       *
       * @returns The BCIDs of the unpaired bunches from beam 2
       */
      virtual std::vector< int > configuredUnpairedBCIDsBeam2() const = 0;

      /// Function providing the intensities of the configured unpaired bunches
      /**
       * This function returns the "intensities" of the unpaired bunches. It always
       * gives the "intensities" in the same order as the configuredUnpairedBCIDs()
       * function returns the BCIDs in.
       *
       * @returns The "intensities" of the unpaired bunches from beam 1
       */
      virtual std::vector< float > configuredUnpairedIntensitiesBeam1() const = 0;

      /// Function providing the intensities of the configured unpaired bunches
      /**
       * This function returns the "intensities" of the unpaired bunches. It always
       * gives the "intensities" in the same order as the configuredUnpairedBCIDs()
       * function returns the BCIDs in.
       *
       * @returns The "intensities" of the unpaired bunches from beam 2
       */
      virtual std::vector< float > configuredUnpairedIntensitiesBeam2() const = 0;

   }; // class IBunchCrossingConfProvider

} // namespace Trig

#endif // TRIGANALYSISINTERFACES_IBUNCHCROSSINGCONFPROVIDER_H
