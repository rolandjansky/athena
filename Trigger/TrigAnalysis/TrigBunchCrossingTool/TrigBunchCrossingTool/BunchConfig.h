// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfig.h 620838 2014-10-09 11:38:00Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_BUNCHCONFIG_H
#define TRIGBUNCHCROSSINGTOOL_BUNCHCONFIG_H

// System include(s):
#include <vector>

namespace Trig {

   /**
    * @short Structure describing a bunch configuration
    *
    * This custom structure is used just to avoid using either
    * many simple, or just a few cryptic variables for caching
    * the bunch configurations.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 620838 $
    * $Date: 2014-10-09 13:38:00 +0200 (Thu, 09 Oct 2014) $
    */
   struct BunchConfig {

      /// BCIDs of the colliding bunches
      std::vector< int > filledBCIDs;
      /// The beam 1 intensities in the colliding bunches
      std::vector< float > filledIntBeam1;
      /// The beam 2 intensities in the colliding bunches
      std::vector< float > filledIntBeam2;
      /// BCIDs of the unpaired bunches of beam 1
      std::vector< int > unpairedBCIDsBeam1;
      /// BCIDs of the unpaired bunches of beam 2
      std::vector< int > unpairedBCIDsBeam2;
      /// Intensities of the unpaired bunches in beam 1
      std::vector< float > unpairedIntBeam1;
      /// Intensities of the unpaired bunches in beam 2
      std::vector< float > unpairedIntBeam2;

   }; // struct BunchConfig

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_BUNCHCONFIG_H
