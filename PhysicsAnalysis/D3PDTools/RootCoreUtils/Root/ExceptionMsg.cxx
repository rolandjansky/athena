//          Copyright Nils Krumnack 2008 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

//protect
#include <RootCoreUtils/ExceptionMsg.h>

#include <memory>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace RCU
{
  void ExceptionMsg ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    RCU_INVARIANT (m_file != 0);
    RCU_INVARIANT (m_line != 0);
    RCU_INVARIANT (!m_message.empty());
  }



  ExceptionMsg ::
  ExceptionMsg (const char *const val_file, const unsigned val_line,
		const std::string& val_message)
    : m_file ((RCU_REQUIRE (val_file != 0),
	       RCU_REQUIRE (val_line != 0),
	       RCU_REQUIRE (!val_message.empty()),
	       val_file)),
      m_line (val_line), m_message (val_message)
  {
    RCU_NEW_INVARIANT (this);
  }



  ExceptionMsg ::
  ~ExceptionMsg () throw ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  const char *ExceptionMsg ::
  what () const throw ()
  {
    RCU_READ_INVARIANT (this);
    return m_message.c_str();
  }
}
