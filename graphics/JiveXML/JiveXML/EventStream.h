/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_EVENTSTREAM_H
#define JIVEXML_EVENTSTREAM_H

#include <string>
#include <map>

namespace JiveXML {

  /**
   * For the client-server communication, each event is uniquely identified by
   * the run number, the event number and a string determining which stream it
   * belongs to. Operators are defined only for the stream name, in order to
   * check for identity of two events use isSameEvent
   */
  class EventStreamID {

    public:

      //Constructor with all parameters
      EventStreamID( unsigned long EventNumber, unsigned int RunNumber, std::string StreamName ) :
        m_event(EventNumber), m_run(RunNumber), m_stream(StreamName){
      };

      //Constructor with all paramters and char *
      EventStreamID( unsigned long EventNumber, unsigned int RunNumber, const char* StreamName ) :
        m_event(EventNumber), m_run(RunNumber){
          //Check for NULL pointer
          if (StreamName) m_stream=StreamName;
      };

      //Constructor with just a stream name - usefull for searching in the map
      EventStreamID( std::string StreamName ) :
        m_event(0), m_run(0), m_stream(StreamName){
      };

      //Get the different values
      unsigned long EventNumber() const { return m_event; };
      unsigned int RunNumber() const { return m_run; };
      std::string StreamName() const { return m_stream; };

      //Check wether two EventStreamIDs are refering to the same event
      bool isSameEvent ( const EventStreamID& id ) const {
        return (( id.StreamName() == m_stream ) &&
                ( id.RunNumber() == m_run ) &&
                ( id.EventNumber() == m_event ));
      }

      //Check wether the ID is valid, i.e. stream name, run and event number
      //greater / greater-equal zero
      bool isValid () const {
        return (( !m_stream.empty() ) && ( m_run != 0 ) && ( m_event != 0 ));
      }

      //Define comparision operators for the map,
      //i.e. only based on stream name, which is the key
      inline bool operator == ( const EventStreamID& id ) const {
        return ( id.StreamName() == m_stream );
      }

      inline bool operator < ( const EventStreamID& id ) const {
        return ( id.StreamName() < m_stream );
      }
 
    private:

      //the event number
      unsigned int m_event;
      //the run number
      unsigned int m_run;
      //the stream name
      std::string m_stream;
   };

   /**
    * A map that stores events according to their EventStreamID
    * Due to the way EventStreamID is build, events will be sorted by
    * their stream name. std::map::find(key) and operator[] will return the
    * event that belongs to this stream. In order to compare for identity of
    * events use EventStreamID::isSameEvent()
    */
   typedef std::pair< const EventStreamID, const std::string> EventStreamPair;
   typedef std::map< const EventStreamID, const std::string> EventStreamMap;

} //namespace

#endif

