// MsgStream.cxx

#include "AsgTools/MsgStream.h"
#include <string>
#include <iostream>
#include <iomanip>

using std::string;
using std::cout;
using std::setw;

//**********************************************************************

MsgStream::MsgStream(const asg::INamed* pnamed)
: m_pnamed(pnamed), m_lvl(MSG::NIL), m_reqlvl(MSG::NIL) {
}

//**********************************************************************

void MsgStream::setLevel(MSG::Level lvl) {
  m_lvl = lvl;
}

//**********************************************************************

string MsgStream::name() const {
  string sname;
  if ( m_pnamed != 0 ) sname = m_pnamed->name();
  return sname;
}

//**********************************************************************

MSG::Level MsgStream::level() const {
  return m_lvl;
}

//**********************************************************************

MSG::Level& MsgStream::mutable_level() {
  return m_lvl;
}

//**********************************************************************

// Insert request level.
MsgStream& MsgStream::insert(MSG::Level rhs) {
  m_reqlvl = rhs;
  if ( m_reqlvl >= m_lvl ) {
    string sname = name();
    string sreqlvl = MSG::name(m_reqlvl);
    int npad = 20 - sname.size();
    cout << sname;
    for ( int ipad=0; ipad<npad; ++ipad ) cout << " ";
    cout << " " << setw(10) << sreqlvl << " ";
  }
  return *this;
}

//**********************************************************************

// Insert end of line.
MsgStream& MsgStream::insert(EOL) {
  if ( m_reqlvl >= m_lvl ) {
    std::cout << std::endl;
  }
  return *this;
}


//**********************************************************************
