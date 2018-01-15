/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef __ELECTRONEFFICIENCYCORRECTIONMESSAGING__
#define __ELECTRONEFFICIENCYCORRECTIONMESSAGING__
#include "AsgTools/AsgMessaging.h"

namespace MSGHELPERS{
    const asg::AsgMessaging& getMsgStream();
}

inline const asg::AsgMessaging& MSGHELPERS::getMsgStream(){
    static const asg::AsgMessaging dummymsg("");
    return dummymsg;
}

#define DUMMY_MSG(lvl, ARG ) {						\
    const asg::AsgMessaging& dummymsg=MSGHELPERS::getMsgStream();	\
    dummymsg.msg(lvl)<<ARG<<endmsg ;					\
}
#define MSG_DEBUG( ARG ) {DUMMY_MSG(MSG::DEBUG, ARG);}
#define MSG_INFO( ARG ) {DUMMY_MSG(MSG::INFO, ARG);}
#define MSG_WARNING( ARG ) { DUMMY_MSG(MSG::WARNING, ARG);}
#define MSG_ERROR( ARG ) { DUMMY_MSG(MSG::ERROR, ARG);}
#define MSG_FATAL( ARG ) { DUMMY_MSG(MSG::FATAL, ARG);}
#define MSG_ABORT( ARG ) { DUMMY_MSG(MSG::FATAL, ARG); std::abort();} 

#define CHECK( ARG )						\
    do {								\
        const bool result = ARG;					\
        if( ! result ) {						\
            MSG_ERROR("FAILED  to execute: " <<#ARG);	\
            return EXIT_FAILURE;					\
        }								\
    } while( false )

#endif
