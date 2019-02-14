// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfo_v1.h 727083 2016-03-01 15:20:50Z krasznaa $
#ifndef XAODEVENTINFO_VERSIONS_EVENTINFO_V1_H
#define XAODEVENTINFO_VERSIONS_EVENTINFO_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <set>
#include <vector>
#include <string>
#include <iosfwd>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#include "CxxUtils/CachedValue.h"
#include "CxxUtils/checker_macros.h"

// Forward declaration(s):
class StoreGateSvc;

/// Namespace holding all the xAOD classes/functions
namespace xAOD {

   // Some forward declaration(s):
   class EventInfo_v1;
   typedef DataVector< EventInfo_v1 > EventInfoContainer_v1;

   /// Class describing the basic event information
   ///
   /// This is a replacement for the offline EventInfo class, to be
   /// used in xAOD files. It stores the same information as its parent,
   /// but in much simpler format. Practically in the same format in
   /// which we store(d) event information in D3PDs.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 727083 $
   /// $Date: 2016-03-01 16:20:50 +0100 (Tue, 01 Mar 2016) $
   ///
   class EventInfo_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      EventInfo_v1();
      /// Copy constructor
      EventInfo_v1( const EventInfo_v1& parent );

      /// Assignment operator
      EventInfo_v1& operator=( const EventInfo_v1& rhs );

      /// @name Basic event information
      /// @{

      /// The current event's run number
      uint32_t runNumber() const;
      /// Set the current event's run number
      void setRunNumber( uint32_t value );

      /// The current event's event number
      unsigned long long eventNumber() const;
      /// Set the current event's event number
      void setEventNumber( unsigned long long value );

      /// The current event's luminosity block number
      uint32_t lumiBlock() const;
      /// Set the current event's luminosity block number
      void setLumiBlock( uint32_t value );

      /// POSIX time in seconds from 1970. January 1st
      uint32_t timeStamp() const;
      /// Set the POSIX time of the event
      void setTimeStamp( uint32_t value );

      /// Nanosecond time offset wrt. the time stamp
      uint32_t timeStampNSOffset() const;
      /// Set the nanosecond offset wrt. the time stamp
      void setTimeStampNSOffset( uint32_t value );

      /// The bunch crossing ID of the event
      uint32_t bcid() const;
      /// Set the bunch crossing ID of the event
      void setBCID( uint32_t value );

      /// Bit field indicating which TTC zones are present in the event
      uint32_t detectorMask0() const;
      /// Set the  bit field indicating with TTC timezones were present
      void setDetectorMask0( uint32_t value );
      /// Bit field indicating which TTC zones are present in the event
      uint32_t detectorMask1() const;
      /// Set the  bit field indicating with TTC timezones were present
      void setDetectorMask1( uint32_t value );
      /// Bit field indicating which TTC zones are present in the event
      uint32_t detectorMask2() const;
      /// Set the  bit field indicating with TTC timezones were present
      void setDetectorMask2( uint32_t value );
      /// Bit field indicating which TTC zones are present in the event
      uint32_t detectorMask3() const;
      /// Set the  bit field indicating with TTC timezones were present
      void setDetectorMask3( uint32_t value );

      /// Bit field indicating which TTC zones are present in the event
      uint64_t detectorMask() const;
      /// Set the bit fields indicating with TTC timezones were present
      void setDetectorMask( uint32_t mask0, uint32_t mask1 );
      /// Set the bit fields indicating with TTC timezones were present
      void setDetectorMask( uint64_t mask );
      /// Bit field indicating which TTC zones are present in the event
      uint64_t detectorMaskExt() const;
      /// Set the bit fields indicating with TTC timezones were present
      void setDetectorMaskExt( uint32_t mask2, uint32_t mask3 );
      /// Set the bit fields indicating with TTC timezones were present
      void setDetectorMaskExt( uint64_t mask );

      /// @}

      /// @name Event type information
      /// @{

      /// Type of the detector description tags
      typedef std::vector< std::pair< std::string, std::string > >
         DetDescrTags_t;

      /// The detector description tags
      const DetDescrTags_t& detDescrTags() const;
      /// Set the detector description tags
      void setDetDescrTags( const DetDescrTags_t& value );

      /// The MC generator's channel number
      uint32_t mcChannelNumber() const;
      /// Set the MC generator's channel number
      void setMCChannelNumber( uint32_t value );

      /// The MC generator's event number
      unsigned long long mcEventNumber() const;
      /// Set the MC generator's event number
      void setMCEventNumber( unsigned long long value );

      /// The weights of all the MC events used in the simulation
      const std::vector< float >& mcEventWeights() const;
      /// Set the weights of all the MC events used in the simulation
      void setMCEventWeights( const std::vector< float >& value );
      /// The weight of one specific MC event used in the simulation
      float mcEventWeight( size_t i = 0 ) const;

      /// Event type codes that can be set/checked in the bitmask
      enum EventType {
         /// true: simulation, false: data
         IS_SIMULATION = 1,
         /// true: testbeam, false: full detector
         IS_TESTBEAM = 2,
         /// true: calibration, false: physics
         IS_CALIBRATION = 4
      };

      /// The event type bitmask
      uint32_t eventTypeBitmask() const;
      /// Set the event type bitmask
      void setEventTypeBitmask( uint32_t value );
      /// Check for one particular bitmask value
      bool eventType( EventType type ) const;

      /// @}

      /// @name Trigger related information
      /// @{

      /// Trigger status element
      uint32_t statusElement() const;
      /// Set the trigger status element
      void setStatusElement( uint32_t value );

      /// The extended Level-1 identifier
      uint32_t extendedLevel1ID() const;
      /// Set the extended Level-1 identifier
      void setExtendedLevel1ID( uint32_t value );

      /// The Level-1 trigger type
      uint16_t level1TriggerType() const;
      /// Set the Level-1 trigger type
      void setLevel1TriggerType( uint16_t value );

      /// Class describing a stream tag on the event
      ///
      /// This simple, transient class is used to present the stream tag
      /// information to the users in a nice way.
      ///
      class StreamTag {

      public:
         /// Constructor giving all relevant information to the object
         StreamTag( const std::string& name, const std::string& type,
                    bool obeysLumiblock,
                    const std::set< uint32_t >& robs = std::set< uint32_t >(),
                    const std::set< uint32_t >& dets = std::set< uint32_t >() );

         /// Get the name of the stream
         const std::string& name() const;
         /// Get the type of the stream
         const std::string& type() const;
         /// Get whether the the stream obeys the luminosity block boundaires
         bool obeysLumiblock() const;
         /// Get the (optional) list of RoB IDs for partial event building
         const std::set< uint32_t >& robs() const;
         /// Get the (optional) list of detector IDs for partial event building
         const std::set< uint32_t >& dets() const;

      private:
         std::string m_name; ///< The name of the stream
         std::string m_type; ///< The type of the stream
         /// Whether the stream obeys luminosity block boundaries
         bool m_obeysLumiblock;
         std::set< uint32_t > m_robs; ///< Optional list of RoB IDs for PEB
         std::set< uint32_t > m_dets; ///< Optional list of detector IDs for PEB

      }; // class StreamTag

      /// Get the streams that the event was put in
      const std::vector< StreamTag >& streamTags() const;
      /// Set the streams that the event was put in
      void setStreamTags( const std::vector< StreamTag >& value );

      /// @}

      /// @name Pileup information
      /// @{

      /// Average interactions per crossing for the current BCID - for in-time
      /// pile-up
      float actualInteractionsPerCrossing() const;
      /// Set average interactions per crossing for the current BCID
      void setActualInteractionsPerCrossing( float value );

      /// Average interactions per crossing for all BCIDs - for out-of-time
      /// pile-up
      float averageInteractionsPerCrossing() const;
      /// Set average interactions per crossing for all BCIDs
      void setAverageInteractionsPerCrossing( float value );

      /// Enumerator describing the types of pileup events
      enum PileUpType {
         Unknown     = 99, ///< Type not known/specified
         Signal      =  0, ///< The signal event
         MinimumBias =  1, ///< Minimum bias pileup event
         Cavern      =  2, ///< Cavern background pileup event
         HaloGas     =  3, ///< Halo-gas non-collision background
         ZeroBias    =  4, ///< Zero bias pileup event
         PileUp_NTYPES
      }; // enum PileUpType

      /// Convert PileUpType enum value to string
      static const std::string& PileUpType2Name(PileUpType typ);

      /// Convert int to PileUpType enum value
      static PileUpType PileUpInt2Type(unsigned short typ);

      /// Class describing the properties of one pileup sub-event
      ///
      /// This class mirrors the setup of PileUpEventInfo::SubEvent. But
      /// uses a bit simpler structure on its inside.
      ///
      class SubEvent {

      public:
         /// Constructor giving all relevant information to the object
         SubEvent( int16_t time, uint16_t index, PileUpType type,
                   const ElementLink< EventInfoContainer_v1 >& link );

         /// Get the time wrt. the signal event (which has time() == 0)
         int16_t time() const;
         /// Get the index of the sub-event
         uint16_t index() const;
         /// Get the type of the pileup event
         PileUpType type() const;
         /// The string name of the type
         const std::string& typeName() const;

         /// Get a link to the EventInfo object describing the pileup event
         const ElementLink< EventInfoContainer_v1 >& link() const;
         /// Get a pointer to the EventInfo object describing the pileup event
         const EventInfo_v1* ptr() const;

      private:
         int16_t m_time; ///< The time wrt. the signal event
         uint16_t m_index; ///< The index of the pileup event
         PileUpType m_type; ///< The type of the pileup event
         /// Link to the EventInfo object in question
         ElementLink< EventInfoContainer_v1 > m_link;

      }; // class SubEvent

      /// Get the pileup events that were used in the simulation
      const std::vector< SubEvent >& subEvents() const;
      /// Set the pileup events that were used in the simulation
      void setSubEvents( const std::vector< SubEvent >& value );
      /// Add one sub-event to the existing list
      void addSubEvent( const SubEvent& subEvent );
      /// Clear all the currently held sub-events
      void clearSubEvents();

      /// @}

      /// @name Detector information
      /// @{

      /// Sub-detector types for which we store event-level flags
      enum EventFlagSubDet {
         Pixel      = 0, ///< The pixel tracker
         SCT        = 1, ///< The SCT
         TRT        = 2, ///< The TRT
         LAr        = 3, ///< The LAr calorimeter
         Tile       = 4, ///< The Tile calorimeter
         Muon       = 5, ///< The muon spectrometer
         ForwardDet = 6, ///< The forward detectors
         Core       = 7, ///< Core flags describing the event
         Background = 8, ///< The beam background detectors
         Lumi       = 9, ///< The luminosity detectors
         nDets      = 10 ///< Number of sub-detector type in this enumeration
      };

      /// States that a given sub-detector could be in
      enum EventFlagErrorState {
         NotSet  = 0, ///< The flag was not set to anything
         Warning = 1, ///< The sub-detector issued a warning
         Error   = 2  ///< The sub-detector issued an error
      };

      /// Enum for bits in Background EventFlag word
      enum BackgroundEventFlag {
         MBTSTimeDiffHalo       = 0,
         MBTSTimeDiffCol        = 1,
         LArECTimeDiffHalo      = 2,
         LArECTimeDiffCol       = 3,
         PixMultiplicityHuge    = 4,
         PixSPNonEmpty          = 5,
         SCTMultiplicityHuge    = 6,
         SCTSPNonEmpty          = 7,
         CSCTimeDiffHalo        = 8,
         CSCTimeDiffCol         = 9,
         BCMTimeDiffHalo        = 10,
         BCMTimeDiffCol         = 11,
         MuonTimingCol          = 12,
         MuonTimingCosmic       = 13,
         MBTSBeamVeto           = 14,
         BCMBeamVeto            = 15,
         LUCIDBeamVeto          = 16,
         HaloMuonSegment        = 17,
         HaloClusterShape       = 18,
         HaloMuonOneSided       = 19,
         HaloMuonTwoSided       = 20,
         HaloTileClusterPattern = 21,
         BeamGasPixel           = 22,
         CosmicStandAlone       = 23,
         CosmicStandAloneTight  = 24,
         CosmicCombined         = 25,
         CosmicCombinedTight    = 26,
         BkgdResvBit1           = 27,
         BkgdResvBit2           = 28,
         BkgdResvBit3           = 29,
         BkgdResvBit4           = 30,
         BkgdResvBit5           = 31,
         NBackgroundWords       = 32
      };

      /// Get the event flags for a particular sub-detector
      uint32_t eventFlags( EventFlagSubDet subDet ) const;
      /// Check one particular bit of one particular sub-detector
      bool isEventFlagBitSet( EventFlagSubDet subDet, size_t bit ) const;

      /// Set the event flags for a particular sub-detector
      bool setEventFlags( EventFlagSubDet subDet, uint32_t flags );
      /// Set one particular bit of one particular sub-detector
      bool setEventFlagBit( EventFlagSubDet subDet, size_t bit );
      /// Reset one particular bit of one particular sub-detector
      bool resetEventFlagBit( EventFlagSubDet subDet, size_t bit );

      /// Get the error state for a particular sub-detector
      EventFlagErrorState errorState( EventFlagSubDet subDet ) const;
      /// Set the error state for a particular sub-detector
      bool setErrorState( EventFlagSubDet subDet,
                          EventFlagErrorState state );

      /// Change detector flags with update semantics.  This means that
      /// we can set bits but not reset them, and that the error state
      /// can move higher (NotSet -> Warning -> Error) but not the other way.
      /// These methods are const, so they can be called on an existing
      /// EventInfo, but will fail with an exception if the detector flag
      /// word has been locked.  This will happen on input if the flag word
      /// has already been set to something; it could also happen if the flag
      /// word is written explicitly with a DecorationHandle.
      ///
      /// Be aware that no dependency tracking is done for these flags,
      /// so something that is reading them should ensure that whatever
      /// writes them runs first.  In most cases, setting event flags
      /// goes along with producing some data item; in those cases,
      /// it should suffice for the reader to have a dependency on this
      /// data item.  In other cases, one might need to use explicit
      /// DecorationHandle's or produce a dummy data item.

      /// Set one particular bit of one particular sub-detector
      bool updateEventFlagBit ( const EventFlagSubDet subDet,
                                const size_t bit ) const;
      /// Turn on a set of event flags for one particular sub-detector.
      /// This does a logical OR of the exising flags and FLAGS.
      bool updateEventFlags( const EventFlagSubDet subDet,
                             const uint32_t flags ) const;
      /// Update the error state for one particular sub-detector.
      /// The error state may only be increased by this method.
      /// If STATE is less than the current error state, then this method
      /// will return false and leave the current state unchanged.
      bool updateErrorState( const EventFlagSubDet subDet,
                             const EventFlagErrorState state ) const;

      /// @}

      /// @name Beam spot information
      /// @{

      /// X coordinate of the beam spot position
      float beamPosX() const;
      /// Y coordinate of the beam spot position
      float beamPosY() const;
      /// Z coordinate of the beam spot position
      float beamPosZ() const;
      /// Set the beam spot position
      void setBeamPos( float x, float y, float z );

      /// The uncertainty on the X position of the beam spot
      float beamPosSigmaX() const;
      /// The uncertainty on the Y position of the beam spot
      float beamPosSigmaY() const;
      /// The uncertainty on the Z position of the beam spot
      float beamPosSigmaZ() const;
      /// Set the uncertainties on the beam spot position
      void setBeamPosSigma( float x, float y, float z );

      /// The beam spot uncertainty's X-Y correlation
      float beamPosSigmaXY() const;
      /// Set the beam spot uncertainty's X-Y correlation
      void setBeamPosSigmaXY( float value );

      /// The beam's tilt in radians in XZ
      float beamTiltXZ() const;
      /// Set the beam's tilt in radians in XZ
      void setBeamTiltXZ( float value );

      /// The beam's tilt in radians in YZ
      float beamTiltYZ() const;
      /// Set the beam's tilt in radians in YZ
      void setBeamTiltYZ( float value );

      /// The beam spot's status word
      uint32_t beamStatus() const;
      /// Set the beam spot's status word
      void setBeamStatus( uint32_t value );

      /// @}

      /// @name Functions used by pile-up digitisation
      /// @{

#if not defined(__GCCXML__) and not defined(__ROOTCLING__)
      /// Get the pointer to the event store associated with this event
      StoreGateSvc* evtStore ATLAS_NOT_CONST_THREAD_SAFE () const;
      /// Set the pointer to the event store associated with this event
      void setEvtStore( StoreGateSvc* svc );
#endif // not genreflex or rootcint/rootcling

      /// @}

      /// Prepare the object for writing
      void toPersistent();
      /// Cleanse the object after being read in
      void toTransient();

   private:
      std::vector< EventInfo_v1::SubEvent > makeSubEvents() const;

      /// Cached stream tag objects
      CxxUtils::CachedValue<std::vector< StreamTag > > m_streamTags;
      /// Cached sub-event objects
      CxxUtils::CachedValue<std::vector< SubEvent>  > m_subEvents;

#ifndef __GCCXML__
      /// Transient pointer to the StoreGateSvc instance associated with the
      /// event (Needed for pile-up digitisation)
      StoreGateSvc* m_evtStore;
#endif // not __GCCXML__

   }; // class EventInfo_v1

   /// A helper operator to be able to print debug messages easily
   std::ostream& operator<< ( std::ostream& out, const xAOD::EventInfo_v1& ei );

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EventInfo_v1, SG::AuxElement );

#endif // XAODEVENTINFO_VERSIONS_EVENTINFO_V1_H
