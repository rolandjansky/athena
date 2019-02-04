/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_TRIGMESSAGESVC_H
#define TRIGSERVICES_TRIGMESSAGESVC_H

// Include files
#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrigKernel/ITrigMessageSvc.h"

// Forward declarations
class ISvcLocator;
class TH1I;
class TH2I;

/**@class TrigMessageSvc
 * @brief MessageSvc used by the HLT applications
 *
 * This MessageSvc implementation it used by the HLT applications. It has some additional
 * features compared to the default Gaudi MessageSvc. Most notably the forwarding of messages
 * to the TDAQ ERS message system.
 *
 * The message suppression is configured with the following parameters:
 * @param <level>Limit = 0:       no message suppression for \<level\>
 * @param <level>Limit = N > 0:   suppress messages after N messages (per source)
 * @param <level>Limit = -N < 0:  use logarithmic suppression after N messages (per message)
 *
 * Note, that the logarithmic suppression works on a per-message basis (ignoring any digits
 * in the message).
 *
 * @author    Iain Last, Werner Wiedenmann, Frank Winklmeier
 */

class TrigMessageSvc : public extends<Service,IMessageSvc,ITrigMessageSvc,IIncidentListener> {
public:  
  typedef std::pair< std::string, std::ostream* > NamedStream;
  typedef std::multimap< int, NamedStream > StreamMap;
  typedef std::multimap< StatusCode, Message > MessageMap;
  typedef std::map< std::string, int > ThresholdMap;
  
  // Default constructor.
  TrigMessageSvc( const std::string& name, ISvcLocator* svcloc );
  // Destructor.
  virtual ~TrigMessageSvc();

  // Implementation of IService::reinitialize()
  virtual StatusCode reinitialize() override;
  // Implementation of IService::initialize()
  virtual StatusCode initialize() override;
  // Implementation of IService::finalize()
  virtual StatusCode finalize() override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const Message& message ) override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const Message& message, int outputLevel ) override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const StatusCode& code, const std::string& source = "") override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const char* source, int type, const char* message) override;

  // Implementation of IMessageSvc::reportMessage()
  virtual void reportMessage( const std::string& source, int type, const std::string& message) override;
  
  // Implementation of IMessageSvc::insertMessage()
  virtual void insertMessage( const StatusCode& code, const Message& message ) override;

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage() override;

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage( const StatusCode& code ) override;

  // Implementation of IMessageSvc::eraseMessage()
  virtual void eraseMessage( const StatusCode& code, const Message& message ) override;

  // Implementation of IMessageSvc::insertStream()
  virtual void insertStream( int message_type, const std::string& name, std::ostream* stream ) override;
  
  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream() override;
  
  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( int message_type ) override;
  
  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( int message_type, std::ostream* stream ) override;

  // Implementation of IMessageSvc::eraseStream()
  virtual void eraseStream( std::ostream* stream ) override;

  // Implementation of IMessageSvc::desaultStream()
  virtual std::ostream* defaultStream() const override {
    return m_defaultStream; 
  }

  // Implementation of IMessageSvc::setDefaultStream()
  virtual void setDefaultStream( std::ostream* stream ) override {
    m_defaultStream = stream;
  }

  // Implementation of IMessageSvc::ouputLevel()
  virtual int outputLevel()   const override;

  // Implementation of IMessageSvc::ouputLevel()
  virtual int outputLevel(const std::string& source)   const override;

  // Implementation of IMessageSvc::setOuputLevel()
  virtual void setOutputLevel(int new_level) override;

  // Implementation of IMessageSvc::setOuputLevel()
  virtual void setOutputLevel(const std::string& source, int new_level) override;

  // Implementation of IMessageSvc::useColor()
  virtual bool useColor() const override { return m_color; }

  // Implementation of IMessageSvc::getLogColor()
  virtual std::string getLogColor(int logLevel) const override;

  // Implementation of IMessageSvc::messageCount()
  virtual int messageCount( MSG::Level logLevel ) const override;
 
  /// \name ITrigMessageSvc implementation
  //@{
  /// Reset all individual output levels
  virtual void resetOutputLevels() override;
  //@}

  // Implemenation of IIncidentListener::handle()
  virtual void handle(const Incident& inc) override;

private:
  typedef std::map<std::string, MSG::Color> ColorMap;
  typedef std::map< unsigned int, unsigned int > MsgCountMap;
  
  std::ostream* m_defaultStream;      ///< Pointer to the output stream.
  Message m_defaultMessage;           ///< Default Message
  StreamMap m_streamMap;              ///< Stream map
  MessageMap m_messageMap;            ///< Message map
  ThresholdMap m_thresholdMap;        ///< Output level threshold map
  std::string m_defaultFormat;        ///< Default format for the messages
  std::string m_defaultTimeFormat;    ///< Default time format for the messages
  int m_defaultLimit;                 ///< Original default message limit
  bool m_doSuppress;                  ///< Message suppression currently active?
  bool m_running;                     ///< In RUNNING state
  bool m_canEnter;                    ///< used to avoid infinite recursions
  
  ServiceHandle<StoreGateSvc> m_evtStore;   ///< Event store
  
  /* Properties */
  StringArrayProperty m_thresholdProp[MSG::NUM_LEVELS];
  StringArrayProperty m_logColors[MSG::NUM_LEVELS];
  UnsignedIntegerProperty m_statLevel;
  UnsignedIntegerProperty m_statLevelRun;
  UnsignedIntegerProperty m_publishLevel;
  UnsignedIntegerProperty m_eventIDLevel;
  IntegerProperty m_msgLimit[MSG::NUM_LEVELS];
  BooleanProperty m_color;
  BooleanProperty m_stats;
  BooleanProperty m_publishStats;
  BooleanProperty m_resetStats;
  StringArrayProperty m_useERS[MSG::NUM_LEVELS];
  BooleanProperty m_alwaysUseMsgStream;
  BooleanProperty m_useErsRunningOnly;  
  BooleanProperty m_suppress;
  BooleanProperty m_suppressRunningOnly;
  BooleanProperty m_forceOutputLevel;
  
  /* Message colors */
  std::string m_logColorCodes[MSG::NUM_LEVELS];
  std::string colTrans(std::string, int);
  ColorMap m_colMap;

  /* Message counting */
  struct msgAry { int msg[MSG::NUM_LEVELS]; };
  std::map<std::string, msgAry> m_sourceMap;      ///< Current message counting per source
  std::map<std::string, msgAry> m_sourceMapTotal; ///< Total message counting per source
  int m_msgCount[MSG::NUM_LEVELS];                ///< Message counting per level
  MsgCountMap m_msgCountMap;                      ///< Message counting per message
  TH1I* m_msgCountHist;                           ///< Message counting per level histogram
  TH2I* m_msgCountSrcHist;                        ///< Message counting per message source

  /* Update handlers */
  void initColors(Property& prop);
  void setupColors(Property& prop);
  void setupLimits(Property& prop);
  void setupThreshold(Property& prop);
  void setupSuppression(Property& prop);
  
  unsigned int msgHash(const Message& msg);   ///< Hash of message (for log. suppression)
  void resetMsgStats();                       ///< Reset message statistics
  void reportMsgStats(uint statLevel, bool total=false);        ///< Report message statistics
  void setOutputLevelViaJobOptSvc();          ///< Change OutputLevel properties in JobOptionsSvc
  void bookHistograms();                      ///< Book histograms for message statistics
  bool passErsFilter(const std::string& source, const std::vector<std::string>& filter);  ///< Source passes ERS filter?
};

#endif
