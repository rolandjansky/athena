// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchCrossingConfProviderBase.h 748399 2016-05-19 14:55:47Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGCONFPROVIDERBASE_H
#define TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGCONFPROVIDERBASE_H

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingConfProvider.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

namespace Trig {

   /**
    *  @short Common implementation of the IBunchCrossingConfProvider interface
    *
    *         Implementing the IBunchCrossingConfProvider interface is quite
    *         simple actually, and it can be done in a common way. So it doesn't
    *         make sense to do it separately for all the tool implementations.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 748399 $
    * $Date: 2016-05-19 16:55:47 +0200 (Thu, 19 May 2016) $
    */
   class BunchCrossingConfProviderBase :
      public virtual IBunchCrossingConfProvider,
      public virtual IBunchCrossingTool {

   public:
      /// Function providing the BCIDs of the configured colliding bunches
      virtual std::vector< int > configuredBCIDs() const;
      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam1() const;
      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam2() const;

      /// Function providing the BCIDs of the configured unpaired bunches
      virtual std::vector< int > configuredUnpairedBCIDsBeam1() const;
      /// Function providing the BCIDs of the configured unpaired bunches
      virtual std::vector< int > configuredUnpairedBCIDsBeam2() const;
      /// Function providing the intensities of the configured unpaired bunches
      virtual std::vector< float > configuredUnpairedIntensitiesBeam1() const;
      /// Function providing the intensities of the configured unpaired bunches
      virtual std::vector< float > configuredUnpairedIntensitiesBeam2() const;

   protected:
      /// Type name for the incident that such tools should emit
      static const std::string BUNCH_CONFIG_INCIDENT_NAME;

   }; // class BunchCrossingConfProviderBase

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_BUNCHCROSSINGCONFPROVIDERBASE_H
