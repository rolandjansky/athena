// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MCBunchCrossingTool.h 511865 2012-07-31 08:44:12Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_MCBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_MCBUNCHCROSSINGTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Interface include(s):
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "BunchCrossingConfProviderBase.h"

namespace Trig {

   /**
    *  @short The MC implementation of Trig::IBunchCrossingTool
    *
    *         This implementation of the IBunchCrossingTool interface can read the
    *         metadata from pileup MC samples, and present the information to the
    *         analyst in the same way as the other tools represent the information
    *         for real data.
    *
    *         It also acts as a bunch crossing configuration provider, implementing the
    *         Trig::IBunchCrossingConfProvider interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 511865 $
    * $Date: 2012-07-31 10:44:12 +0200 (Tue, 31 Jul 2012) $
    */
   class MCBunchCrossingTool : public AthAlgTool,
                               public virtual IBunchCrossingTool,
                               public virtual BunchCrossingToolBase,
                               public virtual BunchCrossingConfProviderBase {

   public:
      /// Standard AlgTool constructor
      MCBunchCrossingTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();
      /// Regular AlgTool finalization function
      virtual StatusCode finalize();

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

      int m_maxBunchSpacing; ///< The maximum bunch spacing that the tool should consider
      int m_frontLength; ///< Length of the "front" of a bunch train
      int m_tailLength; ///< Length of the "tail" of a bunch train
      float m_minBunchIntensity; ///< Minimum intensity for a bunch crossing to be filled

      bool m_isConfigured; ///< Flag specifying if the configuration could be read
      configid_type m_id; ///< ID of the current configuration

      ServiceHandle< IIncidentSvc > m_incidentSvc; ///< The incident service handle

   }; // class MCBunchCrossingTool

}

#endif // TRIGBUNCHCROSSINGTOOL_MCBUNCHCROSSINGTOOL_H
