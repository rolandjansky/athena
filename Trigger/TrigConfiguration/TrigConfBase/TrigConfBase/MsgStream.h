// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFBASE_MSGSTREAM_H
#define TRIGCONFBASE_MSGSTREAM_H 1
/**
 * @file   TrigConfBase/MsgStream.h
 * @brief  MsgStream for DetCommon
 * @author Frank Winklmeier
 *
 * $Id$
 */

#include <sstream>

class IMessageSvc;

namespace TrigConf {

  /// Messsage levels matching the Gaudi definition
  namespace MSGTC {
    enum Level {
      NIL = 0,
      VERBOSE,
      DEBUG,
      INFO,
      WARNING,
      ERROR,
      FATAL,
      ALWAYS,
      NUM_LEVELS
    };
  }

  /**
   * MsgStreamTC class to be used in DetCommon equivalant to Gaudi's MsgStream
   *
   * The usage and behavior is identical to the default Gaudi message stream.
   * If the code is run within a Gaudi/Athena environment, the MsgStreamTC will
   * try to print its messages via the Gaudi MessageSvc. This is achieved via
   * weak linking. In a non-Gaudi/Athena environemt ERS will be used.
   * The class can be used directly or better via inheriting from
   * TrigConf::TrigConfMessaging, which is the equivalent of AthMessaging.
   *
   * Main difference to Gaudi MsgStream:
   *   - OutputLevel cannot be configued via job options, instead the
   *     gloabal OutputLevel of the MessageSvc is being used
   *   - Only supports endmsg and not the the deprecated endmsg
   */
  class MsgStreamTC : public std::ostringstream {
  public:
    MsgStreamTC(const std::string& name);

    /// Return message level of stream
    MSGTC::Level level() {return m_level;}

    /// Set message level of stream
    void setLevel(MSGTC::Level lvl);

     // set width for printing the name
     void setWidth(unsigned int width) {
        m_width = width;
     }

    /// Output operator for message levels
    MsgStreamTC& operator<< (MSGTC::Level lvl) {
      lvl = (lvl >= MSGTC::NUM_LEVELS) ? MSGTC::ALWAYS : (lvl<MSGTC::NIL) ? MSGTC::NIL : lvl;
      m_active = ((m_msgLevel=lvl) >= m_level);  // active if message level >= stream level
      return *this;
    }

    /// Output operator for default types
    template<typename T>
    MsgStreamTC& operator<< (const T& t) {
      if (m_active) *static_cast<std::ostringstream*>(this) << t;
      return *this;
    }

    /// Output operator for stream modifiers
    MsgStreamTC& operator<< (std::ios& (*f)(std::ios&))    {
      if (m_active) f(*this);
      return *this;
    }

    /// Output operator to support endl, etc.
    MsgStreamTC& operator<< ( std::ostream& (*f)(std::ostream&)) {
      if (m_active) f(*this);
      return *this;
    }

    /// Output operator to support endmsg, etc.
    MsgStreamTC& operator<< ( MsgStreamTC& (*f)(MsgStreamTC&)) {
      if (m_active) f(*this);
      return *this;
    }

    /// Print the current message buffer
    void doOutput();

    /// Is the stream active?
    bool isActive() { return m_active; }

    /// set the name if you want to change the message stream name
    void setName(const std::string & name) { m_name = name; }
    
  private:
    bool m_active;          ///< stream active?
    MSGTC::Level m_level;     ///< stream level
    MSGTC::Level m_msgLevel;  ///< current message level
    std::string  m_name;     ///< stream name
    unsigned int m_width { 30 };     ///< width for printing owner name
  };

  /// End of message
  inline MsgStreamTC& endmsgtc(MsgStreamTC& s) {
    if (s.isActive()) s.doOutput();
    return s;
  }
  
} // namespace TrigConf

#endif
