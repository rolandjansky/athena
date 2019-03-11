/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFBASE_TRIGCONFMESSAGING_H
#define TRIGCONFBASE_TRIGCONFMESSAGING_H 1

/**
 * @file   TrigConfMessaging.h
 * @brief  Messaging base class for TrigConf code shared with Lvl1 ( AthMessaging)
 * @author Frank Winklmeier
 *
 */

#include <string>
#include <boost/thread/tss.hpp>
#include "TrigConfBase/MsgStream.h"
#include "TrigConfBase/MsgStreamMacros.h"

namespace TrigConf {

  /**
   * Class to provide easy access to TrigConf::MsgStream for TrigConf classes
   *
   * This is a copy&paste of AthMessaging and used in exactly the same way but
   * without introducing Gaudi/Athena dependencies.
   */
  class TrigConfMessaging { 
  public: 

    /** @brief Constructor with parameters
     *  @param name Component name used in the messages
     */
    TrigConfMessaging (const std::string& name) :
      m_name(name)
    {}

    /// Destructor
    virtual ~TrigConfMessaging() {}
    
    /** @brief Test the output level
     *  @param lvl The message level to test against
     *  @return boolean Indicting if messages at given level will be printed
     *  @retval true Messages at level "lvl" will be printed
     */
    bool msgLvl (const MSGTC::Level lvl) const;
    
    /** The standard message stream.
     *  Returns a reference to the message stream
     *  May not be invoked before sysInitialize() has been invoked.
     */
    MsgStreamTC& msg() const;
    
    /** The standard message stream.
     *  Returns a reference to the default message stream
     *  May not be invoked before sysInitialize() has been invoked.
     */
    MsgStreamTC& msg (const MSGTC::Level lvl) const;

  private:
    TrigConfMessaging() = delete;
    TrigConfMessaging( const TrigConfMessaging& rhs ) = delete;
    TrigConfMessaging& operator=( const TrigConfMessaging& rhs ) = delete;

    /// MsgStreamTC instance (a std::cout like with print-out levels)
    mutable boost::thread_specific_ptr<MsgStreamTC> m_msg_tls;
    std::string m_name;
  }; 

  inline bool TrigConfMessaging::msgLvl (const MSGTC::Level lvl) const
  {
    if (msg().level() <= lvl) {
      msg() << lvl;
      return true;
    }
    else {
      return false;
    }
  }

  inline MsgStreamTC& TrigConfMessaging::msg() const 
  {
    MsgStreamTC* ms = m_msg_tls.get();
    if (!ms) {
      ms = new MsgStreamTC(m_name);
      m_msg_tls.reset(ms);
    }
    return *ms;
  }
  
  inline MsgStreamTC& TrigConfMessaging::msg (const MSGTC::Level lvl) const 
  {
    return msg() << lvl;
  }
  
} // namespace TrigConf

#endif
