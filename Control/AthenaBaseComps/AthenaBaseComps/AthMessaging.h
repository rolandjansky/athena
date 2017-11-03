///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthMessaging.h 
// Header file for class AthMessaging
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENABASECOMPS_ATHMESSAGING_H
#define ATHENABASECOMPS_ATHMESSAGING_H 1

// STL includes
#include <iosfwd>
#include <string>
#include <atomic>

// framework includes
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <boost/thread/tss.hpp>

/** @class AthMessaging AthMessaging.h AthenaBaseComps/AthMessaging.h
 *
 *  Mixin class to provide easy @c MsgStream access and capabilities.
 *  One usually inherits from this class and use it like so:
 *  @code
 *   void some_method (AthMessaging& o) 
 *   { o.msg() << "foo" << endmsg; }
 *  @endcode
 */

class AthMessaging
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  AthMessaging (IMessageSvc* msgSvc, const std::string& name);

  /// Destructor: 
  virtual ~AthMessaging(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief Test the output level
   *  @param lvl The message level to test against
   *  @return boolean Indicating if messages at given level will be printed
   *  @retval true Messages at level "lvl" will be printed
   */
  bool 
  msgLvl (const MSG::Level lvl) const;

  /** The standard message stream.
   *  Returns a reference to the default message stream
   *  May not be invoked before sysInitialize() has been invoked.
   */
  MsgStream& msg() const;

  /** The standard message stream.
   *  Returns a reference to the default message stream
   *  May not be invoked before sysInitialize() has been invoked.
   */
  MsgStream& 
  msg (const MSG::Level lvl) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Change the current logging level.
   *  Use this rather than msg().setLevel() for proper operation with MT.
   */
  void setLevel (MSG::Level lvl);

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  AthMessaging(); //> not implemented
  AthMessaging( const AthMessaging& rhs ); //> not implemented
  AthMessaging& operator=( const AthMessaging& rhs ); //> not implemented

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Current logging level.
  std::atomic<MSG::Level> m_lvl;

  /// MsgStream instance (a std::cout like with print-out levels)
  mutable boost::thread_specific_ptr<MsgStream> m_msg_tls;

  IMessageSvc* m_imsg { nullptr };
  std::string m_nm;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const AthMessaging& o );

inline
bool
AthMessaging::msgLvl (const MSG::Level lvl) const
{
  if (m_lvl <= lvl) {
    msg() << lvl;
    return true;
  } else {
    return false;
  }
}

inline
MsgStream&
AthMessaging::msg() const 
{
  MsgStream* ms = m_msg_tls.get();
  if (!ms) {
    ms = new MsgStream(m_imsg,m_nm);
    m_msg_tls.reset( ms );
  }

  ms->setLevel (m_lvl);
  return *ms;
}

inline
MsgStream&
AthMessaging::msg (const MSG::Level lvl) const 
{ return msg() << lvl; }


#endif //> !ATHENABASECOMPS_ATHMESSAGING_H
