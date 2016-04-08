/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RedirectMsgStream.h 704615 2015-10-29 18:50:12Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_REDIRECTMSGSTREAM_H
#define RINGERSELECTORTOOLS_TOOLS_REDIRECTMSGSTREAM_H

#include <string>
#include <stdexcept>

// Local includes:
#include "RingerSelectorTools/tools/IRedirectMsgStream.h"
#include "RingerSelectorTools/tools/cxx/mutable.h"
#include "RingerSelectorTools/tools/cxx/RingerUseNewCppFeatures.h"

namespace Ringer {
/**
 * @class RedirectMsgStream
 * @brief Redirect Msg Stream to calling AsgSelector
 *
 *  We do this so that we can display messages as if we were AsgSelector 
 * with our algorithms.
 *
 *  If we are not running using a AsgSelector, use ostream otherwise.
 **/
class RedirectMsgStream : virtual public IRedirectMsgStream {
  public:
    /// Methods:
    /// @{
    /// Check whether stream is available
    bool isStreamAvailable() const {
      return m_msg != nullptr;
    }

    /// Returns MsgSteam with its current level
    void setMsgStream(MsgStream *msg) const {
#if RINGER_USE_NEW_CPP_FEATURES
      m_msg = msg;
#else
      const_cast<RedirectMsgStream*>(this)->m_msg = msg;
#endif
    }
    /// Check if message level is enabled
    bool msgLvl(MSG::Level level) const { 
#if !defined(RINGER_STANDALONE)
      if ( isStreamAvailable() ) {
        return m_msg->level() <= level; 
      }
      return false;
#else
      return MSG::currentLevel <= level;
#endif
    }

    /// Get msg level
    MSG::Level level() const {
#if !defined(RINGER_STANDALONE)
      if ( isStreamAvailable() )
        return m_msg->level();
      else 
        throw std::runtime_error(std::string("Stream not available"));
#else
      return MSG::currentLevel;
#endif
    }

  protected:
    /// Ctors:
    /// @{
    /**
     * @brief Redirects output as if it run by Asg selector. 
     *
     *   When no MsgStream available, build one if on Asg environment,
     * otherwise use std::cout.
     **/ 
    RedirectMsgStream(MsgStream *msg):
      m_msg(msg){;}
    /**
     * @brief Redirects output as if it run by Asg selector. 
     *
     *   When no MsgStream available, build one if on Asg environment,
     * otherwise use std::cout.
     **/ 
    RedirectMsgStream():
      m_msg(nullptr){;}
    /// @}
    ~RedirectMsgStream(){;}

    /// Properties:
    /// @{
    /// The re-directed message stream from AsgSelector
    ATH_RINGER_MUTABLE MsgStream *m_msg;
    /// @}

#if !defined(RINGER_STANDALONE)
    /// Returns MsgSteam with its current level
    MsgStream& msg() const { 
      if ( isStreamAvailable() ) {
        return (*m_msg); 
      } else {
        throw std::runtime_error(std::string(
              "Cannot display message, MsgStream not set."
              " Make sure to use setMsgStream(MsgStream*) method."));
      }
    }

    /// Returns MsgStream and set its level
    MsgStream& msg(MSG::Level level) const { 
      if ( isStreamAvailable() ) {
#if !defined(XAOD_ANALYSIS) && !defined(XAOD_STANDALONE)
        return m_msg->report(level);
#else
        return (*m_msg) << level;
#endif
      } else {
        throw std::runtime_error(std::string(
              "Cannot display message, MsgStream not set."
              " Make sure to use setMsgStream(MsgStream*) method."));
      }
    }

#else
    /// Returns MsgSteam with its current level
    MsgStream& msg() const { 
      if ( isStreamAvailable() ) {
        return (*m_msg); 
      } else {
        return std::cout;
      }
    }

    /// Returns m_msg, otherwise returns
    MsgStream& msg(MSG::Level level) const { 
      if ( isStreamAvailable() ) {
        return (*m_msg) << level; 
      } else {
        return std::cout << level;
      }
    }
#endif
    /// @}

};

} // namespace Ringer

#endif
