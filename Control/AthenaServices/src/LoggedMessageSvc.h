/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiSvc/src/AthMessageSvc/AthMessageSvc.h,v 1.15 2008/10/21 16:25:55 marcocle Exp $
#ifndef ATHENASERVICES_LOGGEDMESSAGESVC_H
#define ATHENASERVICES_LOGGEDMESSAGESVC_H 1

// Include files
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iosfwd>
#include <mutex>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Message.h"
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/ILoggedMessageSvc.h"

#include <boost/array.hpp>

// Forward declarations
class ISvcLocator;

//
// ClassName:   LoggedMessageSvc
//
// Description: The LoggedMessageSvc service implements the IMessageSvc 
//              interface, extending the MessageSvc to include logging of
//              all messages above a user defined threshold.
//
// Author:      Charles Leggett
//
class LoggedMessageSvc : public extends2<AthService, ILoggedMessageSvc, IInactiveMessageCounter> {
public:
  typedef std::pair< std::string, std::ostream* > NamedStream;
  typedef std::multimap< int, NamedStream > StreamMap;
  typedef std::multimap< StatusCode, Message > MessageMap;
  typedef std::map< std::string, int, std::less<> > ThresholdMap;

  // Default constructor.
  LoggedMessageSvc( const std::string& name, ISvcLocator* svcloc );
  // Destructor.
  virtual ~LoggedMessageSvc();

  // Implementation of IService::reinitialize()
  virtual StatusCode reinitialize();
  // Implementation of IService::initialize()
  virtual StatusCode initialize();
  // Implementation of IService::finalize()
  virtual StatusCode finalize();

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const Message& message );

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const Message& msg, int outputLevel );

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const StatusCode& code, const std::string& source = "");

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const char* source, int type, const char* message);

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const std::string& source, int type, const std::string& message);

  // Implementation of IMessageSvc::insertMessage()
  virtual void insertMessage( const StatusCode& code, const Message& message );

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage();

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage( const StatusCode& code ) ;

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage( const StatusCode& code, const Message& message );

  // Implementation of IMessageSvc::insertStream()
  virtual void insertStream( int message_type, std::string name, std::ostream* stream );

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream();

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( int message_type );

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( int message_type, std::ostream* stream );

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( std::ostream* stream );

  // Implementation of IMessageSvc::desaultStream()
  virtual std::ostream* defaultStream() const {
    return m_defaultStream;
  }

  // Implementation of IMessageSvc::setDefaultStream()
  virtual void setDefaultStream( std::ostream* stream ) {
    std::lock_guard<std::mutex> lock(m_reportMutex);
    m_defaultStream = stream;
  }

  // Implementation of IMessageSvc::ouputLevel()
  virtual int outputLevel()   const;

  // Implementation of IMessageSvc::ouputLevel()
  virtual int outputLevel(std::string_view source)   const;

  // Implementation of IMessageSvc::setOuputLevel()
  virtual void setOutputLevel(int new_level);

  // Implementation of IMessageSvc::setOuputLevel()
  virtual void setOutputLevel(std::string_view source, int new_level);

  // Implementation of IInterface::queryInterface()
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  // Implementation of IMessageSvc::useColor()
  virtual bool useColor() const { return m_color; }

  // Implementation of IMessageSvc::getLogColor()
  virtual std::string getLogColor(int logLevel) const;

  // Implementation of IMessageSvc::messageCount()
  virtual int messageCount( MSG::Level logLevel ) const;

  // Implementation of IInactiveMessageCounter::incrInactiveCount()
  virtual void incrInactiveCount( MSG::Level level,
				  const std::string& src );

  virtual const std::vector< std::pair<std::string, std::string> >& getMessages( MSG::Level level) const;

  virtual const std::vector< LoggedMessage >& getKeyMessages() const { return m_msgKeyLog; }

private:
  std::ostream* m_defaultStream;      ///< Pointer to the output stream.
  Message m_defaultMessage;           ///< Default Message
  StreamMap m_streamMap;              ///< Stream map
  MessageMap m_messageMap;            ///< Message map
  ThresholdMap m_thresholdMap;        ///< Output level threshold map
  std::string m_defaultFormat;        ///< Default format for the messages
  std::string m_defaultTimeFormat;    ///< Default format for timestamps in the messages
  StringArrayProperty m_thresholdProp[MSG::NUM_LEVELS]; ///< Properties controling
  BooleanProperty m_color;
  BooleanProperty m_stats;
  UnsignedIntegerProperty m_statLevel;
  StringArrayProperty m_logColors[MSG::NUM_LEVELS];
  IntegerProperty m_msgLimit[MSG::NUM_LEVELS];
  IntegerProperty m_logLevel;
  StringArrayProperty m_keywords;

  std::string m_logColorCodes[MSG::NUM_LEVELS];

  /// Private helper class to keep the count of messages of a type (MSG::LEVEL).
  struct MsgAry {
    /// Simple typedef for readability.
    typedef boost::array<int,MSG::NUM_LEVELS> ArrayType;
    /// Internal array of counters.
    ArrayType msg;
    /// Default constructor.
    MsgAry() {
      // This is a special hack to have a fast initialization of the array
      // because we cannot use initializer lists in the constructor (should be
      // possible in C++0X).
      static const ArrayType zero = {{0}};
      msg = zero;
    }
  };

  std::map<std::string,MsgAry> m_sourceMap, m_inactiveMap;
  BooleanProperty m_suppress, m_inactCount;

  std::string colTrans(std::string, int);
  typedef std::map<std::string, MSG::Color> ColorMap;
  ColorMap m_colMap;

  int m_msgCount[MSG::NUM_LEVELS];

  std::map<std::string, std::string> m_loggedStreamsName;
  typedef std::map<std::string, std::ostream*> LoggedStreamsMap_t;
  LoggedStreamsMap_t m_loggedStreams;

  void initColors(Gaudi::Details::PropertyBase& prop);
  void setupColors(Gaudi::Details::PropertyBase& prop);
  void setupLimits(Gaudi::Details::PropertyBase& prop);
  void setupThreshold(Gaudi::Details::PropertyBase& prop);
  void setupInactCount(Gaudi::Details::PropertyBase& prop);

  void setupLogStreams();

  void tee( const std::string& sourceName, const std::string& logFileName,
	    const std::set<std::string>& declaredOutFileNames );

  /// Mutex to synchronize multiple threads printing.
  mutable std::mutex m_reportMutex;

  /// Mutex to synchronize multiple access to m_messageMap.
  mutable std::recursive_mutex m_messageMapMutex;

  /// Mutex to synchronize multiple access to m_thresholdMap
  /// (@see MsgStream::doOutput).
  mutable std::mutex m_thresholdMapMutex;

  std::vector< std::pair<std::string, std::string> > m_msgLog[ MSG::NUM_LEVELS ];
  std::vector< LoggedMessage > m_msgKeyLog;
  bool m_keysUsed;

};

#endif
