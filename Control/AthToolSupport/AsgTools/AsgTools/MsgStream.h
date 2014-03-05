// MsgStream_H

#ifndef MsgStream_H
#define MsgStream_H

// David Adams
// January 2014
//
// Root replacement for Gaudi MsgStream.
// The stream is constructed from a named object and uses
// that name to prefix messages.

#include "AsgTools/AsgToolsConf.h"
#ifdef ASGTOOL_ATHENA
#include "GaudiKernel/MsgStream.h"
#else

#include "AsgTools/MsgLevel.h"
#include <string>
#include <iostream>
#include "AsgTools/INamed.h"

class MsgStream {

public:

  class EOL { };

  // Ctor.
  MsgStream(const asg::INamed* =0);

  // Insert an object into the stream.
  template <class T>
  MsgStream& insert(const T& rhs);

  // Insertion of message level set the request level accordingly.
  MsgStream& insert(MSG::Level rhs);

  // Insertion of end of line.
  MsgStream& insert(EOL);

  // Set the message level.
  void setLevel(MSG::Level lvl);

  std::string name() const;

  MSG::Level level() const;
  MSG::Level& mutable_level();

private:

  const asg::INamed* m_pnamed;
  MSG::Level m_lvl;
  MSG::Level m_reqlvl;

};

static const MsgStream::EOL endmsg = MsgStream::EOL();
static const MsgStream::EOL endreq = MsgStream::EOL();

#endif

#ifndef MsgStream_ICC
#define MsgStream_ICC

template <class T>
MsgStream& MsgStream::insert(const T& rhs) {
  if ( m_reqlvl >= m_lvl ) {
    std::cout << rhs;
  }
  return *this;
}

// Usual stream insertion syntax.
template <class T>
MsgStream& operator<<(MsgStream& lhs, const T& rhs) {
  lhs.insert(rhs);
  return lhs;
}

#endif
#endif
