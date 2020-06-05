/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRedirectMsgStream.h 693573 2015-09-07 19:15:49Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_IREDIRECTMSGSTREAM_H
#define RINGERSELECTORTOOLS_TOOLS_IREDIRECTMSGSTREAM_H

// We make these declarations here because we can't forward declare enums
// without C++11 enum class.
#if !defined(RINGER_STANDALONE)
// Asg includes:
# include "AsgTools/MsgStream.h"
# include "AsgTools/MsgStreamMacros.h"
//#pragma message "ASGTOOL or XAOD_STANDALONE"
#else
# include <iostream>
typedef std::ostream MsgStream;
namespace MSG {

  // Keep it simple
  // FIXME This needs to be tested
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
  }

  /**
   * This is just a workaround for simulating the MsgStream when on standalone
   * environment.
   **/
  static Level currentLevel = INFO;

  /**
   * Display message level:
   **/
  std::ostream& operator<<(std::ostream &stream, const Level lvl){
    switch (lvl){
      case VERBOSE:
        return stream << "VERBOSE:  ";
        break;
      case DEBUG:
        return stream << "DEBUG:    ";
        break;
      case INFO:
        return stream << "INFO:     ";
        break;
      case WARNING:
        return stream << "WARNING:  ";
        break;
      case ERROR:
        return stream << "ERROR:    ";
        break;
      case FATAL:
        return stream << "FATAL:    ";
        break;
      otherwise:
        return stream;
    }
  }
}

# define ATH_MSG_LVL( lvl, xmsg)                                               \
  do {                                                                         \
    if ( msgLvl(lvl) ) {                                                       \
      msg(lvl) << xmsg << std::endl;                                           \
    }                                                                          \
  } while( 0 )

# define ATH_MSG_VERBOSE(xmsg)                                                 \
  ATH_MSG_LVL( MSG::VERBOSE, xmsg )

# define ATH_MSG_DEBUG(xmsg)                                                   \
  ATH_MSG_LVL( MSG::DEBUG, xmsg )

# define ATH_MSG_INFO(xmsg)                                                    \
  ATH_MSG_LVL( MSG::INFO, xmsg )

# define ATH_MSG_WARNING(xmsg)                                                 \
  ATH_MSG_LVL( MSG::WARNING, xmsg )

# define ATH_MSG_ERROR(xmsg)                                                   \
  ATH_MSG_LVL( MSG::ERROR, xmsg )

# define ATH_MSG_FATAL(xmsg)                                                   \
  ATH_MSG_LVL( MSG::FATAL, xmsg )

#define ATH_MSG_LVL_DISP_VAR( lvl, var)                                        \
  do {                                                                         \
    if ( msgLvl(lvl) ) {                                                       \
      msg << #var << ":" << var << endl;                                       \
    }                                                                          \
  } while (0)

// =============================================================================
/// Helper operator for printing the contents of vectors (ostream)
template <typename T>
std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec )
{
  // A little prefix:
  out << "[";
  // Print the contents:
  for( size_t i = 0; i < vec.size(); ++i ) {
     out << vec[ i ];
     if( i < vec.size() - 1 ) {
        out << ", ";
     }
  }
  // A little postfix:
  out << "]";
  // Return the stream:
  return out;
}

// =============================================================================
/// Helper operator for printing the contents of vector pointers (ostream)
template <typename T>
std::ostream& operator<< ( std::ostream& out, const std::vector< T >* vec)
{
  return out.operator<<(*vec);
}
#endif // Environment selection

namespace Ringer {
/**
 * @class IRedirectMsgStream
 * @brief Interface for RedirectMsgStream
 **/
class IRedirectMsgStream {
  public:
    /// Methods:
    /// @{
    /// Check whether stream is available
    virtual bool isStreamAvailable() const = 0;

    /// Returns MsgSteam with its current level
    virtual MsgStream& msg() const = 0;

    /// Returns MsgSteam with its current level
    /// It is const member function because m_msg member will be mutable
    virtual void setMsgStream(MsgStream *msg) const = 0;

    /// Set msg level
    virtual bool msgLvl(MSG::Level level) const = 0;
    /// Returns m_msg, otherwise returns
    virtual MsgStream& msg(MSG::Level level) const = 0;

    virtual ~IRedirectMsgStream(){;}
    /// @}
};

} // namespace Ringer


#endif // RINGERSELECTORTOOLS_TOOLS_IREDIRECTMSGSTREAM_H
