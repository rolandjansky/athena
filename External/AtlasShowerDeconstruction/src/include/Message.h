#ifndef MESSAGE_H
#define MESSAGE_H

#ifndef DEBUGCODE
#define LOG(x) Deconstruction::MsgInterface::nullStream
#else
#define LOG(x) Deconstruction::MsgInterface::msg.m(x)
#endif

#define LOGLEVEL(x) Deconstruction::MsgInterface::msg.setLevel(x)

#include <iostream>
#include <ostream>
#include <iosfwd>

namespace Deconstruction {

    enum MsgLevel {
      DEBUG = 0,
      INFO,
      ERROR,
      FORCE
    };

  class nullbuf : public std::streambuf {
    public:
      nullbuf();
      nullbuf(const nullbuf &c);
  };

  class nullstream : public std::ostream {
      public:
      nullstream();
      nullstream(const nullstream &c);
      std::ostream &m(const MsgLevel x);
  };

  class Message : public std::ostream {
    public:

      Message();
      Message(const Message &c);
      virtual ~Message();
      void setLevel(const MsgLevel level);
      const MsgLevel &level() const;

      std::ostream &m(const MsgLevel level);

    private:
      MsgLevel m_level;
  };

  class MsgInterface {
    public:
    static nullbuf _nullbuf;
    static nullstream nullStream;
    static Message msg;
  };

}

#endif

