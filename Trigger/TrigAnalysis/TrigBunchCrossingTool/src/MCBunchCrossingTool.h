// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MCBunchCrossingTool.h 762070 2016-07-15 08:39:37Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_MCBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_MCBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "TrigBunchCrossingTool/BunchCrossingConfProviderBase.h"

namespace Trig {

   /**
    * @short The MC implementation of Trig::IBunchCrossingTool
    *
    * This implementation of the IBunchCrossingTool interface can read the
    * metadata from pileup MC samples, and present the information to the
    * analyst in the same way as the other tools represent the information
    * for real data.
    *
    * It also acts as a bunch crossing configuration provider, implementing the
    * Trig::IBunchCrossingConfProvider interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 762070 $
    * $Date: 2016-07-15 10:39:37 +0200 (Fri, 15 Jul 2016) $
    */
   class MCBunchCrossingTool : public BunchCrossingToolBase,
                               public BunchCrossingConfProviderBase {

   public:
      /// Standard AlgTool constructor
      MCBunchCrossingTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();

      /// The simplest query: Is the bunch crossing filled or not?
      virtual bool isFilled( bcid_type bcid ) const;
      /// Function deciding if there was a bunch from "beam 1" in this bunch crossing
      virtual bool isBeam1( bcid_type bcid = 0 ) const;
      /// Function deciding if there was a bunch from "beam 2" in this bunch crossing
      virtual bool isBeam2( bcid_type bcid = 0 ) const;
      /// Function returning the "intensity" of a given bunch crossing
      virtual float bcIntensity( bcid_type bcid,
                                 BeamType type = Crossing ) const;
      /// Get the type of the specific bunch crossing
      virtual BunchCrossingType bcType( bcid_type bcid ) const;
      /// Gap before a particular bunch
      virtual int gapBeforeBunch( bcid_type bcid,
                                  BunchDistanceType dtype = NanoSec,
                                  BunchFillType ftype = CollidingBunch ) const;
      /// Gap after a particular bunch
      virtual int gapAfterBunch( bcid_type bcid,
                                 BunchDistanceType dtype = NanoSec,
                                 BunchFillType ftype = CollidingBunch ) const;
      /// Function returning whether the previous bunches were filled, and how
      virtual std::vector< bool > bunchesInFront( bcid_type bcid,
                                                  int bunches = 10 ) const;
      /// Function returning whether the following bunches were filled, and how
      virtual std::vector< bool > bunchesAfter( bcid_type bcid,
                                                int bunches = 10 ) const;
      /// Function returning the intensities of the bunch crossings before the reference
      virtual std::vector< float > bunchIntInFront( bcid_type bcid,
                                                    int bunches = 10,
                                                    BeamType type = Crossing ) const;
      /// Function returning the intensities of the bunch crossings after the reference
      virtual std::vector< float > bunchIntAfter( bcid_type bcid,
                                                  int bunches = 10,
                                                  BeamType type = Crossing ) const;

      /// Get the number of filled bunches in the current configuration
      virtual unsigned int numberOfFilledBunches() const;

      /// Unique identifier for the current configuration
      virtual configid_type configID() const;
      /// Function providing the BCIDs of the configured colliding bunches
      virtual std::vector< int > configuredBCIDs() const;
      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam1() const;
      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam2() const;

   private:
      /// Function called when the MC configuration changes
      StatusCode update( IOVSVC_CALLBACK_ARGS );
      /// Convert an std::string into a vector of floats
      static std::vector< float > tokenize( const std::string& pattern );

      /// Minimum intensity for a bunch crossing to be considered filled
      float m_minBunchIntensity;

      /// Flag specifying if the configuration could be read
      bool m_isConfigured;
      /// ID of the current configuration
      configid_type m_id;

      /// The incident service handle
      ServiceHandle< IIncidentSvc > m_incidentSvc;

   }; // class MCBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_MCBUNCHCROSSINGTOOL_H
