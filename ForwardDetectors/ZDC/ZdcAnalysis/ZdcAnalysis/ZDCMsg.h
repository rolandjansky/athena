#ifndef _ZDCMsg_h
#define _ZDCMsg_h

namespace ZDCMsg {

    typedef bool (*MessageFunctionPtr)(unsigned int, std::string);

    enum MSGLevels {

        Verbose=1,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };
}

#endif
