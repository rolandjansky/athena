/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

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
#include "CxxUtils/checker_macros.h"

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
class LoggedMessageSvc :
  public extends<AthService, ILoggedMessageSvc, IInactiveMessageCounter> {

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
  virtual StatusCode reinitialize() override;
  // Implementation of IService::initialize()
  virtual StatusCode initialize() override;
  // Implementation of IService::finalize()
  virtual StatusCode finalize() override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const Message& message ) override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const Message& msg, int outputLevel ) override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const StatusCode& code, std::string_view source = "") override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( std::string source, int type, std::string message) override;

  // Implementation of IMessageSvc::insertMessage()
  virtual void insertMessage( const StatusCode& code, Message message ) override;

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage() override;

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage( const StatusCode& code ) override;

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage( const StatusCode& code, const Message& message ) override;

  // Implementation of IMessageSvc::insertStream()
  virtual void insertStream( int message_type, std::string name, std::ostream* stream ) override;

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream() override;

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( int message_type ) override;

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( int message_type, std::ostream* stream ) override;

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( std::ostream* stream ) override;

  // Implementation of IMessageSvc::defaultStream()
  virtual std::ostream* defaultStream ATLAS_NOT_CONST_THREAD_SAFE () const override {
    return m_defaultStream;
  }

  // Implementation of IMessageSvc::setDefaultStream()
  virtual void setDefaultStream( std::ostream* stream ) override {
    std::lock_guard<std::mutex> lock(m_reportMutex);
    m_defaultStream = stream;
  }

  // Implementation of IMessageSvc::ouputLevel()
  virtual int outputLevel() const override;

  // Implementation of IMessageSvc::ouputLevel()
  virtual int outputLevel(std::string_view source) const override;

  // Implementation of IMessageSvc::setOuputLevel()
  virtual void setOutputLevel(int new_level) override;

  // Implementation of IMessageSvc::setOuputLevel()
  virtual void setOutputLevel(std::string_view source, int new_level) override;

  // Implementation of IInterface::queryInterface()
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown) override;

  // Implementation of IMessageSvc::useColor()
  virtual bool useColor() const override { return m_color; }

  // Implementation of IMessageSvc::getLogColor()
  virtual std::string getLogColor(int logLevel) const override;

  // Implementation of IMessageSvc::messageCount()
  virtual int messageCount( MSG::Level logLevel ) const override;

  // Implementation of IInactiveMessageCounter::incrInactiveCount()
  virtual void incrInactiveCount( MSG::Level level,
                                  std::string_view src ) override;

  virtual const std::vector< std::pair<std::string, std::string> >& getMessages( MSG::Level level) const override;

  virtual const std::vector< LoggedMessage >& getKeyMessages() const override { return m_msgKeyLog; }

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
  struct MsgAry final {
    /// Internal array of counters.
    std::array<int, MSG::NUM_LEVELS> msg = {{0}};
    /// Default constructor.
    MsgAry() = default;
  };

  std::map<std::string, MsgAry, std::less<>> m_sourceMap, m_inactiveMap;
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
