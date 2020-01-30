#ifndef _ZDCMsg_h
#define _ZDCMsg_h

namespace ZDCMsg {

// typedef bool (*MessageFunctionPtr)(unsigned int, std::string);

    typedef std::function<bool(int, std::string)> MessageFunction;

    typedef std::shared_ptr<MessageFunction> MessageFunctionPtr;

    enum MSGLevels {

        Verbose = 1,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };
}

#endif
