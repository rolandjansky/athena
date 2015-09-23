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

// framework includes
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

/** @class AthMessaging AthMessaging.h AthenaBaseComps/AthMessaging.h
 *
 *  Mixin class to provide easy @c MsgStream access and capabilities.
 *  One usually inherits from this class and use it like so:
 *  @code
 *   void some_method (AthMessaging& o) 
 *   { o.msg() << "foo" << endreq; }
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

  /// Constructor, from an explicit existing stream.
  AthMessaging (MsgStream& msg);

  /// Destructor: 
  virtual ~AthMessaging(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** @brief Test the output level
   *  @param lvl The message level to test against
   *  @return boolean Indicting if messages at given level will be printed
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

  /// MsgStream instance (a std::cout like with print-out levels)
  mutable MsgStream m_msg;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
//std::ostream& operator<<( std::ostream& out, const AthMessaging& o );

inline
bool
AthMessaging::msgLvl (const MSG::Level lvl) const
{
 if (m_msg.level() <= lvl) {
   m_msg << lvl;
   return true;
 } else {
   return false;
 }
}

inline
MsgStream&
AthMessaging::msg() const 
{ return m_msg; }

inline
MsgStream&
AthMessaging::msg (const MSG::Level lvl) const 
{ return m_msg << lvl; }


#endif //> !ATHENABASECOMPS_ATHMESSAGING_H
