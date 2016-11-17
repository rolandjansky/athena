// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LHCBunchCrossingTool.h 762070 2016-07-15 08:39:37Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_LHCBUNCHCROSSINGTOOL_H
#define TRIGBUNCHCROSSINGTOOL_LHCBUNCHCROSSINGTOOL_H

// STL include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IOVSvcDefs.h"

// Local include(s):
#include "TrigBunchCrossingTool/BunchCrossingToolBase.h"
#include "TrigBunchCrossingTool/BunchCrossingConfProviderBase.h"

// Forward declaration(s):
namespace coral {
   class AttributeList;
   class Blob;
}

namespace Trig {

   /**
    * @short Implementation reading the information from the LHC conditions DB
    *
    * This implementation of the tool behaves pretty much like
    * TrigConfBunchCrossingTool. But instead of reading the BCIDs of the
    * colliding bunches out of the trigger configuration, it reads them
    * from the LHC conditions, stored in the TDAQ COOL folders.
    *
    * Apparently for some runs this information is more reliable than the
    * trigger configuration.
    *
    * It also acts as a bunch crossing configuration provider,
    * implementing the Trig::IBunchCrossingConfProvider interface.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 762070 $
    * $Date: 2016-07-15 10:39:37 +0200 (Fri, 15 Jul 2016) $
    */
   class LHCBunchCrossingTool : public BunchCrossingToolBase,
                                public BunchCrossingConfProviderBase {

   public:
      /// Standard AlgTool constructor
      LHCBunchCrossingTool( const std::string& type, const std::string& name,
                            const IInterface* parent );

      /// Regular AlgTool initialization function
      virtual StatusCode initialize();

      /// Unique identifier for the current configuration
      virtual configid_type configID() const;

      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam1() const;
      /// Function providing the intensities of the configured colliding bunches
      virtual std::vector< float > configuredIntensitiesBeam2() const;
      /// Function providing the intensities of the configured unpaired bunches
      virtual std::vector< float > configuredUnpairedIntensitiesBeam1() const;
      /// Function providing the intensities of the configured unpaired bunches
      virtual std::vector< float > configuredUnpairedIntensitiesBeam2() const;

   private:
      /// Function called when the LHC configuration changes
      StatusCode update( IOVSVC_CALLBACK_ARGS );
      /// Function decoding the colliding BCID pattern
      std::vector< int > decodeColliding( const coral::AttributeList& attr );
      /// Function decoding the unpaired BCID pattern from beam 1
      std::vector< int > decodeUnpaired1( const coral::AttributeList& attr );
      /// Function decoding the unpaired BCID pattern from beam 2
      std::vector< int > decodeUnpaired2( const coral::AttributeList& attr );
      /// Function decoding the beam 1 intensities for the colliding bunches
      std::vector< float >
      decodeCollIntBeam1( const coral::AttributeList& attr ) const;
      /// Function decoding the beam 2 intensities for the colliding bunches
      std::vector< float >
      decodeCollIntBeam2( const coral::AttributeList& attr ) const;
      /// Function decoding the beam 1 intensities for the unpaired bunches
      std::vector< float >
      decodeUnpairedIntBeam1( const coral::AttributeList& attr ) const;
      /// Function decoding the beam 2 intensities for the unpaired bunches
      std::vector< float >
      decodeUnpairedIntBeam2( const coral::AttributeList& attr ) const;

      /// Function used in decoding the beam intensities
      template< typename TYPE >
      std::vector< double > decodeIntV0( const coral::Blob& ints,
                                         const std::vector< bool >& mask,
                                         bool state ) const;
      /// Function used in decoding the beam intensities
      template< typename TYPE >
      std::vector< double > decodeIntV1( const coral::Blob& ints,
                                         const std::vector< int >& bcids ) const;
      /// Function used in decoding the beam intensities
      template< typename TYPE >
      std::vector< double > decodeIntV2( const coral::Blob& ints,
                                         const std::vector< int >& bcids ) const;

      int m_intChannel; ///< Bunch intensity channel

      configid_type m_id; ///< ID of the current configuration
      bool m_intValid; ///< Is the beam intensity info valid?

      ServiceHandle< IIncidentSvc > m_incidentSvc; ///< Incident service handle
      ServiceHandle< IIOVDbSvc > m_iovSvc; ///< The IOVDbSvc handle

      /// Number of beam 1 bunches about which info is stored in FILLPARAMS
      unsigned int m_beam1Bunches;
      /// Number of beam 2 bunches about which info is stored in FILLPARAMS
      unsigned int m_beam2Bunches;
      /// Number of luminous bunches about which info is stored in FILLPARAMS
      unsigned int m_luminousBunches;

      /// Internal flag for which filled bunches in beam 1 are colliding bunches
      std::vector< bool > m_beam1Colliding;
      /// Internal flag for which filled bunches in beam 2 are colliding bunches
      std::vector< bool > m_beam2Colliding;

      /// The current filled bunch crossings
      std::vector< int > m_filledBunches;
      /// The current unpaired bunch crossings in beam 1
      std::vector< int > m_unpairedBunches1;
      /// The current unpaired bunch crossings in beam 2
      std::vector< int > m_unpairedBunches2;
      /// Beam 1 intensities for the filled bunch crossings
      std::vector< float > m_filledIntBeam1;
      /// Beam 2 intensities for the filled bunch crossings
      std::vector< float > m_filledIntBeam2;
      /// Beam 1 intensities for the unpaired bunches
      std::vector< float > m_unpairedIntBeam1;
      /// Beam 2 intensities for the unpaired bunches
      std::vector< float > m_unpairedIntBeam2;

   }; // class LHCBunchCrossingTool

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_LHCBUNCHCROSSINGTOOL_H
