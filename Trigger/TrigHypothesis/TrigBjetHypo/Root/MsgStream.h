/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSGSTREAM_H
#define MSGSTREAM_H

#include <iostream>

std::ostream &endmsg(std::ostream &);

namespace MSG {
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
  };
}

class NullBuf : public std::streambuf {
  public:
    NullBuf();
  protected:
    int overflow(int c);
    int sync();
};


class MsgStream : public std::ostream {

  public:

    MsgStream(const std::string &name, bool isNull = false);
    MsgStream(const MsgStream &m);
    virtual ~MsgStream();

    void setLevel(MSG::Level lvl);
    MSG::Level getLevel() const;
    MsgStream &operator <<(MSG::Level lvl);

  protected:
    std::string m_name;
    MSG::Level m_lvl;
    MSG::Level m_lastLvl;
};

class StatusCode {

  public:

  enum StatusCodeEnum {
    SUCCESS = 0,
    FAILURE
  };

  StatusCode(StatusCodeEnum e);

  operator StatusCodeEnum();

  StatusCode &operator=(StatusCodeEnum value);

  private:

  StatusCodeEnum m_value;
  
  
};

#endif

