#ifndef _ZDCMsg_h
#define _ZDCMsg_h

namespace ZDCMsg {

  typedef bool (*MessageFunctionPtr)(unsigned int, std::string);

  enum MSGLevels {Error=0, Warn, Info, Verbose, Debug, Fatal=9};
}

#endif
