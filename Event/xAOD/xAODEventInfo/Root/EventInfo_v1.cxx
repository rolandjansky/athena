/*
  Copyright (C) 2002-2017, 2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfo_v1.cxx 729717 2016-03-14 18:52:01Z ssnyder $

// System include(s):
#include <iostream>
#include <iomanip>
#include <stdexcept>

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "AthContainers/tools/AtomicDecorator.h"
#include "CxxUtils/stall.h"

// Local include(s):
#include "xAODEventInfo/versions/EventInfo_v1.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "EventInfoAccessors_v1.h"

namespace xAODEventInfoPrivate {

   // Forward declaration(s):
   template< typename T >
   std::ostream& operator<<( std::ostream& out, const std::set< T >& s );

   /// Convenience operator for printing the contents of vectors
   template< typename T >
   std::ostream& operator<<( std::ostream& out, const std::vector< T >& vec  ) {

      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i != ( vec.size() - 1 ) ) {
            out << ", ";
         }
      }
      out << "]";
      return out;
   }

   /// Convenience operator for printing the contents of sets
   template< typename T >
   std::ostream& operator<<( std::ostream& out, const std::set< T >& s ) {

      return out << std::vector< T >( begin( s ), end( s ) );
   }

} // private namespace

namespace xAOD {

   using xAODEventInfoPrivate::operator<<;

   EventInfo_v1::EventInfo_v1()
      : SG::AuxElement(), m_streamTags(),
        m_subEvents(), m_evtStore( 0 ) {

   }

   EventInfo_v1::EventInfo_v1( const EventInfo_v1& parent )
      : SG::AuxElement(), m_streamTags(),
        m_subEvents(),
        m_evtStore( parent.m_evtStore ) {

      makePrivateStore( parent );
   }

   EventInfo_v1& EventInfo_v1::operator=( const EventInfo_v1& rhs ) {

      if (&rhs != this) {
        // Clear out the caches:
        m_streamTags.store (std::vector< StreamTag >());
        m_streamTags.reset();
        m_subEvents.store (std::vector< SubEvent >());
        m_subEvents.reset();

        // Copy the event store pointer:
        m_evtStore = rhs.m_evtStore;

        // Copy the auxiliary variables:
        SG::AuxElement::operator=( rhs );
      }

      // Return this object:
      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the basic event properties
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         runNumber, setRunNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, unsigned long long,
                                         eventNumber, setEventNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         lumiBlock, setLumiBlock )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         timeStamp, setTimeStamp )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         timeStampNSOffset,
                                         setTimeStampNSOffset )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         bcid, setBCID )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         detectorMask0, setDetectorMask0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         detectorMask1, setDetectorMask1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         detectorMask2, setDetectorMask2 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         detectorMask3, setDetectorMask3 )

   uint64_t EventInfo_v1::detectorMask() const {

      return ( ( static_cast< uint64_t >( detectorMask1() ) << 32 ) |
               detectorMask0() );
   }

   void EventInfo_v1::setDetectorMask( uint32_t mask0, uint32_t mask1 ) {

      setDetectorMask0( mask0 );
      setDetectorMask1( mask1 );
      return;
   }

   void EventInfo_v1::setDetectorMask( uint64_t mask ) {

      setDetectorMask0( static_cast< uint32_t >( mask & 0xffffffff ) );
      setDetectorMask1( static_cast< uint32_t >( ( mask >> 32 ) &
                                                 0xffffffff ) );
      return;
   }

   uint64_t EventInfo_v1::detectorMaskExt() const {

      return ( ( static_cast< uint64_t >( detectorMask3() ) << 32 ) |
               detectorMask2() );
   }

   void EventInfo_v1::setDetectorMaskExt( uint32_t mask2, uint32_t mask3 ) {

      setDetectorMask2( mask2 );
      setDetectorMask3( mask3 );
      return;
   }

   void EventInfo_v1::setDetectorMaskExt( uint64_t mask ) {

      setDetectorMask2( static_cast< uint32_t >( mask & 0xffffffff ) );
      setDetectorMask3( static_cast< uint32_t >( ( mask >> 32 ) &
                                                 0xffffffff ) );
      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the event type properties
   //

   AUXSTORE_OBJECT_SETTER_AND_GETTER( EventInfo_v1,
                                      EventInfo_v1::DetDescrTags_t,
                                      detDescrTags, setDetDescrTags )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         mcChannelNumber, setMCChannelNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, unsigned long long,
                                         mcEventNumber, setMCEventNumber )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( EventInfo_v1, std::vector< float >,
                                      mcEventWeights, setMCEventWeights )

   float EventInfo_v1::mcEventWeight( size_t i ) const {

      // Throw an exception when asked for an impossible index:
      return mcEventWeights().at( i );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         eventTypeBitmask, setEventTypeBitmask )

   bool EventInfo_v1::eventType( EventType type ) const {

      return ( eventTypeBitmask() & static_cast< uint32_t >( type ) );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //            Implementation of the trigger related properties
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         statusElement, setStatusElement )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t,
                                         extendedLevel1ID, setExtendedLevel1ID )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint16_t,
                                         level1TriggerType,
                                         setLevel1TriggerType )

   EventInfo_v1::StreamTag::StreamTag( const std::string& name,
                                       const std::string& type,
                                       bool obeysLumiblock,
                                       const std::set< uint32_t >& robs,
                                       const std::set< uint32_t >& dets )
   : m_name( name ), m_type( type ), m_obeysLumiblock( obeysLumiblock ),
      m_robs( robs ), m_dets( dets ) {

   }

   const std::string& EventInfo_v1::StreamTag::name() const {

      return m_name;
   }

   const std::string& EventInfo_v1::StreamTag::type() const {

      return m_type;
   }

   bool EventInfo_v1::StreamTag::obeysLumiblock() const {

      return m_obeysLumiblock;
   }

   const std::set< uint32_t >& EventInfo_v1::StreamTag::robs() const {

      return m_robs;
   }

   const std::set< uint32_t >& EventInfo_v1::StreamTag::dets() const {

      return m_dets;
   }

   //
   // Accessor objects for the stream properties:
   //
   static const EventInfo_v1::Accessor< std::vector< std::string > >
      names( "streamTagNames" );
   static const EventInfo_v1::Accessor< std::vector< std::string > >
      types( "streamTagTypes" );
   static const EventInfo_v1::Accessor< std::vector< char > >
      olb( "streamTagObeysLumiblock" );
   static const EventInfo_v1::Accessor< std::vector< std::set< uint32_t > > >
      robsets( "streamTagRobs" );
   static const EventInfo_v1::Accessor< std::vector< std::set< uint32_t > > >
      detsets( "streamTagDets" );

   const std::vector< EventInfo_v1::StreamTag >&
   EventInfo_v1::streamTags() const {

      // Cache the new information if necessary:
      if( !m_streamTags.isValid() ) {
         std::vector< StreamTag > tags;

         // A little sanity check:
         if( ( names( *this ).size() != types( *this ).size() ) ||
             ( names( *this ).size() != olb( *this ).size() ) ||
             ( robsets.isAvailable( *this ) &&
               ( names( *this ).size() != robsets( *this ).size() ) ) ||
             ( detsets.isAvailable( *this ) &&
               ( names( *this ).size() != detsets( *this ).size() ) ) ) {
            std::cerr << "<xAOD::EventInfo_v1::streamTags> ERROR "
                      << "Corrupt information found!" << std::endl;
            std::cerr << "  streamTagNames          = " << names( *this )
                      << std::endl;
            std::cerr << "  streamTagTypes          = " << types( *this )
                      << std::endl;
            std::cerr << "  streamTagObeysLumiblock = " << olb( *this )
                      << std::endl;
            std::cerr << "  streamTagRobs           = ";
            if( robsets.isAvailable( *this ) ) {
               std::cerr << "N/A" << std::endl;
            } else {
               std::cerr << robsets( *this ) << std::endl;
            }
            std::cerr << "  streamTagDets           = ";
            if( detsets.isAvailable( *this ) ) {
               std::cerr << "N/A" << std::endl;
            } else {
               std::cerr << detsets( *this ) << std::endl;
            }
         }

         else {
           // Fill the tags.
           for( size_t i = 0; i < names( *this ).size(); ++i ) {
             static const std::set< uint32_t > dummySet;
             tags.emplace_back( names( *this )[ i ],
                                types( *this )[ i ],
                                olb( *this )[ i ],
                                ( robsets.isAvailable( *this ) ?
                                  robsets( *this )[ i ] :
                                  dummySet ),
                                ( detsets.isAvailable( *this ) ?
                                  detsets( *this )[ i ] :
                                  dummySet ) );
           }
         }

         // Set the cache.
         m_streamTags.set (std::move (tags));
      }

      // Return the cached object:
      return *m_streamTags.ptr();
   }

   void EventInfo_v1::setStreamTags( const std::vector< StreamTag >& value ) {

      // Update the cached information:
      m_streamTags.store (value);

      // Clear the persistent information:
      names( *this ).clear(); types( *this ).clear(); olb( *this ).clear();
      robsets( *this ).clear(); detsets( *this ).clear();

      // Fill the persistent information:
      std::vector< StreamTag >::const_iterator itr = value.begin();
      std::vector< StreamTag >::const_iterator end = value.end();
      for( ; itr != end; ++itr ) {
         names( *this ).push_back( itr->name() );
         types( *this ).push_back( itr->type() );
         olb( *this ).push_back( itr->obeysLumiblock() );
         robsets( *this ).push_back( itr->robs() );
         detsets( *this ).push_back( itr->dets() );
      }

      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the pileup information functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, float,
                                         actualInteractionsPerCrossing,
                                         setActualInteractionsPerCrossing )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, float,
                                         averageInteractionsPerCrossing,
                                         setAverageInteractionsPerCrossing )

   EventInfo_v1::SubEvent::
   SubEvent( int16_t time, uint16_t index, PileUpType type,
             const ElementLink< EventInfoContainer_v1 >& link )
      : m_time( time ), m_index( index ), m_type( type ), m_link( link ) {

   }

   int16_t EventInfo_v1::SubEvent::time() const {

      return m_time;
   }

   uint16_t EventInfo_v1::SubEvent::index() const {

      return m_index;
   }

   EventInfo_v1::PileUpType EventInfo_v1::SubEvent::type() const {

      return m_type;
   }

   const std::string& EventInfo_v1::SubEvent::typeName() const {

      // This is a really inefficienct implementation, but didn't
      // feel like writing a short and efficient implementation for C++11,
      // and a longer but still efficient version for C++03. This is a
      // medium-sized, slow implementation that works everywhere... :-P
      if( m_type == Signal ) {
         static const std::string name = "Signal";
         return name;
      } else if( m_type == MinimumBias ) {
         static const std::string name = "MinimumBias";
         return name;
      } else if( m_type == Cavern ) {
         static const std::string name = "Cavern";
         return name;
      } else if( m_type == HaloGas ) {
         static const std::string name = "HaloGas";
         return name;
      } else if( m_type == ZeroBias ) {
         static const std::string name = "ZeroBias";
         return name;
      } else {
         static const std::string name = "Unknown";
         return name;
      }
   }

   const ElementLink< EventInfoContainer_v1 >&
   EventInfo_v1::SubEvent::link() const {

      return m_link;
   }

   const EventInfo_v1* EventInfo_v1::SubEvent::ptr() const {

      if( m_link.isValid() ) {
         return *m_link;
      } else {
         return 0;
      }
   }

   //
   // Accessor objects for the sub-event properties:
   //
   static const SG::AuxElement::Accessor< std::vector< int16_t > >
      timeAcc( "subEventTime" );
   static const SG::AuxElement::Accessor< std::vector< uint16_t > >
      indexAcc( "subEventIndex" );
   static const SG::AuxElement::Accessor< std::vector< ElementLink< EventInfoContainer_v1 > > >
      linkAcc( "subEventLink" );
   static const SG::AuxElement::Accessor< std::vector< uint16_t > >
      typeAcc( "subEventType" );

   std::vector< EventInfo_v1::SubEvent >
   EventInfo_v1::makeSubEvents() const
   {
     std::vector< SubEvent > subEvents;

     // Check if any of the information is available:
     if( ( ! timeAcc.isAvailable( *this ) ) &&
         ( ! indexAcc.isAvailable( *this ) ) &&
         ( ! linkAcc.isAvailable( *this ) ) &&
         ( ! typeAcc.isAvailable( *this ) ) )
     {
       // If not, return right away:
       return subEvents;
     }
     // A little sanity check:
     size_t size = 0;
     if( timeAcc.isAvailable( *this ) ) {
       size = timeAcc( *this ).size();
     } else if( indexAcc.isAvailable( *this ) ) {
       size = indexAcc( *this ).size();
     } else if( linkAcc.isAvailable( *this ) ) {
       size = linkAcc( *this ).size();
     } else if( typeAcc.isAvailable( *this ) ) {
       size = typeAcc( *this ).size();
     } else {
       std::cerr << "xAOD::EventInfo_v1 ERROR Logic error in subEvents()"
                 << std::endl;
       return subEvents;
     }
     if( ( timeAcc.isAvailable( *this ) &&
           ( size != timeAcc( *this ).size() ) ) ||
         ( indexAcc.isAvailable( *this ) &&
           ( size != indexAcc( *this ).size() ) ) ||
         ( linkAcc.isAvailable( *this ) &&
           ( size != linkAcc( *this ).size() ) ) ||
         ( typeAcc.isAvailable( *this ) &&
           ( size != typeAcc( *this ).size() ) ) ) {
       std::cerr << "xAOD::EventInfo_v1 ERROR Data corruption found in "
                 << "the sub-event information" << std::endl;
       std::cerr << "subEventTime  = "
                 << ( timeAcc.isAvailable( *this ) ? timeAcc( *this ) :
                      std::vector< int16_t >() ) << std::endl;
       std::cerr << "subEventIndex  = "
                 << ( indexAcc.isAvailable( *this ) ? indexAcc( *this ) :
                      std::vector< uint16_t >() ) << std::endl;
       std::cerr << "subEventLink = "
                 << ( linkAcc.isAvailable( *this ) ? linkAcc( *this ) :
                      std::vector< ElementLink< EventInfoContainer_v1 > >() )
                 << std::endl;
       std::cerr << "subEventType  = "
                 << ( typeAcc.isAvailable( *this ) ? typeAcc( *this ) :
                      std::vector< uint16_t >() ) << std::endl;
       return subEvents;
     }
     // Fill up the cache:
     for( size_t i = 0; i < size; ++i ) {
       const int16_t time =
         timeAcc.isAvailable( *this ) ? timeAcc( *this )[ i ] : 0;
       const uint16_t index =
         indexAcc.isAvailable( *this ) ? indexAcc( *this )[ i ] : 0;
       const ElementLink< EventInfoContainer_v1 > link =
         linkAcc.isAvailable( *this ) ? linkAcc( *this )[ i ] :
         ElementLink< EventInfoContainer_v1 >();
       const PileUpType type =
         ( typeAcc.isAvailable( *this ) ?
           static_cast< PileUpType >( typeAcc( *this )[ i ] ) :
           Unknown );
       subEvents.emplace_back( time, index, type, link );
     }

     return subEvents;
   }

   const std::vector< EventInfo_v1::SubEvent >&
   EventInfo_v1::subEvents() const {

      // Cache the new information if necessary:
      if( !m_subEvents.isValid() ) {
        m_subEvents.set (makeSubEvents());
      }

      // Return the cached vector:
      return *m_subEvents.ptr();
   }

   void EventInfo_v1::setSubEvents( const std::vector< SubEvent >& value ) {

      // Update the cached information:
      m_subEvents.store (value);

      // Clear the persistent information:
      timeAcc( *this ).clear(); indexAcc( *this ).clear();
      typeAcc( *this ).clear(); linkAcc( *this ).clear();

      // Fill the persistent information:
      std::vector< SubEvent >::const_iterator itr = value.begin();
      std::vector< SubEvent >::const_iterator end = value.end();
      for( ; itr != end; ++itr ) {
         timeAcc( *this ).push_back( itr->time() );
         indexAcc( *this ).push_back( itr->index() );
         typeAcc( *this ).push_back( static_cast< uint16_t >( itr->type() ) );
         linkAcc( *this ).push_back( itr->link() );
      }

      return;
   }

   void EventInfo_v1::addSubEvent( const SubEvent& subEvent ) {

      // First, make sure that the persistent and transient variables are in
      // sync:
      subEvents();

      // Now, add the new sub-event:
      std::vector<SubEvent> subEvents = *m_subEvents.ptr();
      subEvents.push_back( subEvent );
      m_subEvents.store (std::move (subEvents));
      timeAcc( *this ).push_back( subEvent.time() );
      indexAcc( *this ).push_back( subEvent.index() );
      typeAcc( *this ).push_back( static_cast< uint16_t >( subEvent.type() ) );
      linkAcc( *this ).push_back( subEvent.link() );

      return;
   }

   void EventInfo_v1::clearSubEvents() {

      // Clear both the transient and persistent variables:
      m_subEvents.store (std::vector<SubEvent>());
      m_subEvents.reset();
      timeAcc( *this ).clear(); indexAcc( *this ).clear();
      typeAcc( *this ).clear(); linkAcc( *this ).clear();

      return;
   }
   
   const std::string& EventInfo_v1::PileUpType2Name(PileUpType typ)
   {
      static const std::string typNam[PileUp_NTYPES+1] = {
         "Unknown" /*99*/,
         "Signal" /*0*/,
         "MinimumBias" /*1*/,
         "Cavern" /*2*/,
         "HaloGas"/*3*/,
         "ZeroBias"/*4*/ };
      int  t = (typ==Unknown)? 0: (int)typ+1;
      assert( t <= PileUp_NTYPES );
      return typNam[t];
   } 

   EventInfo_v1::PileUpType EventInfo_v1::PileUpInt2Type(unsigned short typ)
   {
      static const PileUpType typEnum[PileUp_NTYPES+1] = {
         Unknown /*99*/, Signal /*0*/, MinimumBias /*1*/, Cavern /*2*/, HaloGas /*3*/, ZeroBias /*4*/
      };
      int t = (typ==99)? 0: (int)typ+1;
      assert( t <= PileUp_NTYPES );
      return typEnum[t];
   } 

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //                    Implementation of the detector flags
   //

   /// Bits that store the "user defined" detector flags
   static const uint32_t EF_BITS        = 0x0FFFFFFF;
   /// Bits that store the error state of the sub-detectors
   static const uint32_t EF_ERROR_BITS  = 0xF0000000;
   /// Shift used to store the sub-detector error states
   static const uint32_t EF_ERROR_SHIFT = 28;

   uint32_t EventInfo_v1::eventFlags( EventFlagSubDet subDet ) const {

      return ( ( *eventFlagsConstAccessorsV1( subDet ) )( *this ) &
               EF_BITS );
   }

   bool EventInfo_v1::isEventFlagBitSet( EventFlagSubDet subDet,
                                         size_t bit ) const {

      // A little sanity check:
      if( bit >= EF_ERROR_SHIFT ) {
         throw std::runtime_error( "EventInfo_v1::isEventFlagBitSet: Asked for "
                                   "non-existent bit" );
      }

      // Get the value of the flags:
      uint32_t value = ( *eventFlagsConstAccessorsV1( subDet ) )( *this );

      // Dig out the bit we are asking for:
      return ( ( value >> bit ) & 0x1 );
   }

   bool EventInfo_v1::setEventFlags( EventFlagSubDet subDet, uint32_t flags ) {

      // Get the accessor:
      const SG::AtomicDecorator< uint32_t >* acc = eventFlagsAccessorsV1( subDet );
      if( ! acc ) {
         return false;
      }

      // Construct the value that we want to store, taking the current error
      // flags into account:
      std::atomic<uint32_t>& a = (*acc)(*this);
      uint32_t orig = a;
      uint32_t value = ( ( orig & EF_ERROR_BITS ) | ( flags & EF_BITS ) );
      while (!a.compare_exchange_strong (orig, value)) {
        CxxUtils::stall();
        value = ( ( orig & EF_ERROR_BITS ) | ( flags & EF_BITS ) );
      }

      // Tell the user that we were successful:
      return true;
   }

   bool EventInfo_v1::setEventFlagBit( EventFlagSubDet subDet, size_t bit)
   {
     return updateEventFlagBit (subDet, bit);
   }

   bool EventInfo_v1::updateEventFlagBit( EventFlagSubDet subDet, size_t bit) const
   {
      // Check if the bit makes sense:
      if( bit >= EF_ERROR_SHIFT ) {
         return false;
      }

      // Don't try to write if the requested flag is already set.
      uint32_t existingFlags = (*eventFlagsConstAccessorsV1( subDet ))( *this );
      if (existingFlags & (1u << bit)) {
        return true;
      }

      // Get the accessor:
      const SG::AtomicDecorator< uint32_t >* acc = eventFlagsAccessorsV1( subDet );
      if( ! acc ) {
         return false;
      }

      // Set the new value:
      ( *acc )( *this ) |= ( 1u << bit );

      // Tell the user that we were successful:
      return true;
   }

   bool EventInfo_v1::resetEventFlagBit( EventFlagSubDet subDet, size_t bit)
   {
      // Get the accessor:
      const SG::AtomicDecorator< uint32_t >* acc = eventFlagsAccessorsV1( subDet );
      if( ! acc ) {
         return false;
      }

      // Check if the bit makes sense:
      if( bit >= EF_ERROR_SHIFT ) {
         return false;
      }

      // Set the new value:
      ( *acc )( *this ) &= ~( 1u << bit );

      // Tell the user that we were successful:
      return true;
   }

   bool EventInfo_v1::updateEventFlags( const EventFlagSubDet subDet,
                                        const uint32_t flags_in) const
   {
      uint32_t flags = flags_in & EF_BITS;

      // Don't try to write if all requested flags are already set.
      uint32_t existingFlags = (*eventFlagsConstAccessorsV1( subDet ))( *this );
      if ((existingFlags | flags) ==  existingFlags) {
        return true;
      }

      // Get the accessor:
      const SG::AtomicDecorator< uint32_t >* acc = eventFlagsAccessorsV1( subDet );
      if( ! acc ) {
         return false;
      }

      // Set the new value:
      ( *acc )( *this ) |= ( flags & EF_BITS );

      // Tell the user that we were successful:
      return true;
   }

   EventInfo_v1::EventFlagErrorState
   EventInfo_v1::errorState( EventFlagSubDet subDet ) const {

      // Get the value in one go:
      return static_cast< EventFlagErrorState >(
         ( ( *eventFlagsConstAccessorsV1( subDet ) )( *this ) >> EF_ERROR_SHIFT ) &
         0xF );
   }

   bool EventInfo_v1::setErrorState( EventFlagSubDet subDet,
                                     EventFlagErrorState state ) {

      // Get the accessor:
      const SG::AtomicDecorator< uint32_t >* acc = eventFlagsAccessorsV1( subDet );
      if( ! acc ) {
         return false;
      }

      // Check its value:
      if( ( state != NotSet ) && ( state != Warning ) && ( state != Error ) ) {
         return false;
      }

      // Construct the new value:
      std::atomic<uint32_t>& a = (*acc)(*this);
      uint32_t orig = a;
      uint32_t value = ( ( orig & EF_BITS ) |
                         ( static_cast< uint32_t >( state ) <<
                           EF_ERROR_SHIFT ) );
      while (!a.compare_exchange_strong (orig, value)) {
        CxxUtils::stall();
        value = ( ( orig & EF_BITS ) |
                  ( static_cast< uint32_t >( state ) <<
                    EF_ERROR_SHIFT ) );
      }

      // Tell the user that we were successful:
      return true;
   }

   bool EventInfo_v1::updateErrorState( const EventFlagSubDet subDet,
                                        const EventFlagErrorState state ) const
   {
      // Check its value:
      if( ( state != NotSet ) && ( state != Warning ) && ( state != Error ) ) {
         return false;
      }

      // Return without trying to write if the desired state is
      // already set.
      if (errorState (subDet) == state) {
        return true;
      }

      // Get the accessor:
      const SG::AtomicDecorator< uint32_t >* acc = eventFlagsAccessorsV1( subDet );
      if( ! acc ) {
         return false;
      }

      // Construct the new value:
      std::atomic<uint32_t>& a = (*acc)(*this);
      uint32_t orig = a;

      // Only allow increasing the error state in an update.
      if (state < ((orig >> EF_ERROR_SHIFT) & 0xf)) {
        return false;
      }

      uint32_t value = ( ( orig & EF_BITS ) |
                         ( static_cast< uint32_t >( state ) <<
                           EF_ERROR_SHIFT ) );
      while (!a.compare_exchange_strong (orig, value)) {
        if (state < ((orig >> EF_ERROR_SHIFT) & 0xf)) {
          return false;
        }

        value = ( ( orig & EF_BITS ) |
                  ( static_cast< uint32_t >( state ) <<
                    EF_ERROR_SHIFT ) );

        CxxUtils::stall();
      }

      // Tell the user that we were successful:
      return true;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //                Implementation of the beam spot functions
   //

   AUXSTORE_PRIMITIVE_GETTER( EventInfo_v1, float, beamPosX )
   AUXSTORE_PRIMITIVE_GETTER( EventInfo_v1, float, beamPosY )
   AUXSTORE_PRIMITIVE_GETTER( EventInfo_v1, float, beamPosZ )

   void EventInfo_v1::setBeamPos( float x, float y, float z ) {

      // The accessor objects:
      static const Accessor< float > accX( "beamPosX" );
      static const Accessor< float > accY( "beamPosY" );
      static const Accessor< float > accZ( "beamPosZ" );

      // Set the variables:
      accX( *this ) = x;
      accY( *this ) = y;
      accZ( *this ) = z;

      return;
   }

   AUXSTORE_PRIMITIVE_GETTER( EventInfo_v1, float, beamPosSigmaX )
   AUXSTORE_PRIMITIVE_GETTER( EventInfo_v1, float, beamPosSigmaY )
   AUXSTORE_PRIMITIVE_GETTER( EventInfo_v1, float, beamPosSigmaZ )

   void EventInfo_v1::setBeamPosSigma( float x, float y, float z ) {

      // The accessor objects:
      static const Accessor< float > accX( "beamPosSigmaX" );
      static const Accessor< float > accY( "beamPosSigmaY" );
      static const Accessor< float > accZ( "beamPosSigmaZ" );

      // Set the variables:
      accX( *this ) = x;
      accY( *this ) = y;
      accZ( *this ) = z;

      return;
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, float, beamPosSigmaXY,
                                         setBeamPosSigmaXY )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, float, beamTiltXZ,
                                         setBeamTiltXZ )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, float, beamTiltYZ,
                                         setBeamTiltYZ )

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( EventInfo_v1, uint32_t, beamStatus,
                                         setBeamStatus )

   //
   /////////////////////////////////////////////////////////////////////////////

   StoreGateSvc* EventInfo_v1::evtStore() const {

      return m_evtStore;
   }

   void EventInfo_v1::setEvtStore( StoreGateSvc* svc ) {

      m_evtStore = svc;
      return;
   }

   void EventInfo_v1::toPersistent() {

      // Check if the ElementLink variable is available, and writable:
      if( linkAcc.isAvailableWritable( *this ) ) {
         // Prepare the ElementLink objects for ROOT I/O:
         std::vector< ElementLink< EventInfoContainer_v1 > >::iterator
            el_itr = linkAcc( *this ).begin();
         std::vector< ElementLink< EventInfoContainer_v1 > >::iterator
            el_end = linkAcc( *this ).end();
         for( ; el_itr != el_end; ++el_itr ) {
            el_itr->toPersistent();
         }
      }

      return;
   }

   /// As it is now fairly complicated to prepare an EventInfo object for
   /// usage after it's been read in from a branch, it seemed to make sense
   /// to put the code that's used to do this, into a separate function. It
   /// tries to do the right thing in all situations...
   ///
   void EventInfo_v1::toTransient() {

      m_streamTags.reset();
      m_subEvents.reset();
      m_evtStore = 0;

      if( usingStandaloneStore() ) {
         setStore( ( SG::IAuxStore* ) 0 );
      }

      return;
   }

   /// This operator is provided to make it convenient to print debug messages
   /// including information about the current event. With something like:
   ///
   /// <code>
   ///   const xAOD::EventInfo* ei = ...;<br/>
   ///   std::cout << "Event = " << *ei << std::endl;
   /// </code>
   ///
   /// or:
   ///
   /// <code>
   ///   const xAOD::EventInfo* ei = ...;<br/>
   ///   ATH_MSG_VERBOSE( "Event = " << *ei );
   /// </code>
   ///
   /// @param out The output stream to write EventInfo information to
   /// @param ei The EventInfo object to print information about
   /// @returns The same output stream that the operator received
   ///
   std::ostream& operator<< ( std::ostream& out,
                              const xAOD::EventInfo_v1& ei ) {

      // Get the current state of the stream:
      const char fillChar = out.fill();
      const std::ios_base::fmtflags flags = out.flags();
      const std::streamsize width = out.width();

      // Do the printout:
      out << "[Run,Evt,Lumi,Time,BunchCross,DetMask] = ["
          << ei.runNumber()
          << "," << ei.eventNumber()
          << "," << ei.lumiBlock()
          << "," << ei.timeStamp()
          << ":" << ei.timeStampNSOffset()
          << "," << ei.bcid()
          << ",0x" << std::hex << std::setw( 16 ) << std::setfill( '0' )
          << ei.detectorMask()
          << "]";

      // Restore the original state of the stream:
      out.fill( fillChar );
      out.flags( flags );
      out.width( width );

      // Return the stream:
      return out;
   }

} // namespace xAOD
