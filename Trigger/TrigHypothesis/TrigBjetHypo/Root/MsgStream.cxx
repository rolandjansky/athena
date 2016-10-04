/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MsgStream.h"

#include <iostream>
#include <iomanip>

using std::setfill;
using std::setw;
using std::left;

NullBuf nullBuffer;
MsgStream nullStream("", true);

std::ostream &endmsg(std::ostream &os) {
  return std::endl(os);
}

MsgStream::MsgStream(const std::string &name, bool isNull)
  : std::ostream(std::cout.rdbuf()),
    m_name(name) {
  if (isNull) {
    rdbuf(&nullBuffer);
  }
}

MsgStream::MsgStream(const MsgStream &m)
  : std::ostream(m.rdbuf()),
    m_name(m.m_name),
    m_lvl(m.m_lvl),
    m_lastLvl(m.m_lastLvl) {
}

MsgStream::~MsgStream() {
}

void MsgStream::setLevel(MSG::Level lvl) {
  m_lvl = lvl;
}

MSG::Level MsgStream::getLevel() const {
  return m_lvl;
}

MsgStream &MsgStream::operator <<(MSG::Level lvl) {
  m_lastLvl = lvl;
  if (m_lastLvl >= m_lvl) {
    std::cout.setf(std::ios::left);
    std::cout << setfill(' ') << setw(20) << m_name;

    if (m_lvl == MSG::FATAL) {
      std::cout << setfill(' ') << setw(20) << "FATAL";
    } else if (m_lvl == MSG::ERROR) {
      std::cout << setfill(' ') << setw(20) << "ERROR";
    } else if (m_lvl == MSG::WARNING) {
      std::cout << setfill(' ') << setw(20) << "WARNING";
    } else if (m_lvl == MSG::INFO) {
      std::cout << setfill(' ') << setw(20) << "INFO";
    } else if (m_lvl == MSG::DEBUG) {
      std::cout << setfill(' ') << setw(20) << "DEBUG";
    } else if (m_lvl == MSG::VERBOSE) {
      std::cout << setfill(' ') << setw(20) << "VERBOSE";
    }
    std::cout << " ";

    return *this;
  }
  return nullStream;
}

NullBuf::NullBuf() {
}

int NullBuf::overflow(int c) {
  return 0;
}

int NullBuf::sync() {
  return 0;
}

StatusCode::StatusCode(StatusCodeEnum e) {
  m_value = e;
}


StatusCode::operator StatusCodeEnum() {
  return m_value;
}

StatusCode &StatusCode::operator=(StatusCodeEnum value) {
  m_value = value;
  return *this;
}

