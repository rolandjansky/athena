// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFBASE_TRIGCONFMESSAGING_H
#define TRIGCONFBASE_TRIGCONFMESSAGING_H 1

/**
 * @file   TrigConfMessaging.h
 * @brief  Messaging base class for DetCommon (copy of AthMessaging)
 * @author Frank Winklmeier (original author Sebastien Binet)
 *
 * $Id$
 */

#include <string>
#include "TrigConfBase/MsgStream.h"
#include "TrigConfBase/MsgStreamMacros.h"

namespace TrigConf { 

  /**
   * Class to provide easy access to TrigConf::MsgStream within DetCommon
   *
   * This is a copy&paste of AthMessaging and used in exactly the same way.
   */
  class TrigConfMessaging { 
  public: 

    /** @brief Constructor with parameters
     *  @param name Component name used in the messages
     */
    TrigConfMessaging (const std::string& name) :
      m_msg(name),
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
     *  Returns a reference to the default message stream
     *  May not be invoked before sysInitialize() has been invoked.
     */
    MsgStreamTC& msg() const;
    
    /** The standard message stream.
     *  Returns a reference to the default message stream
     *  May not be invoked before sysInitialize() has been invoked.
     */
    MsgStreamTC& msg (const MSGTC::Level lvl) const;

  private: 
    TrigConfMessaging( const TrigConfMessaging& rhs ); //> not implemented
    TrigConfMessaging& operator=( const TrigConfMessaging& rhs ); //> not implemented

  private: 
    
    /// MsgStreamTC instance (a std::cout like with print-out levels)
    mutable MsgStreamTC m_msg;
    std::string m_name;
  }; 

  inline bool TrigConfMessaging::msgLvl (const MSGTC::Level lvl) const
  {
    if (m_msg.level() <= lvl) {
      m_msg << lvl;
      return true;
    }
    else {
      return false;
    }
  }

  inline MsgStreamTC& TrigConfMessaging::msg() const 
  {
    return m_msg;
  }
  
  inline MsgStreamTC& TrigConfMessaging::msg (const MSGTC::Level lvl) const 
  {
    return m_msg << lvl;
  }
  
} // namespace TrigConf

#endif
