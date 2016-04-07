/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HAVE_MSGSTREAM_H
#define HAVE_MSGSTREAM_H

#define COLOR_HEADER "\033[95m"
#define COLOR_BOLD "\033[1m"
#define COLOR_OKBLUE "\033[94m"
#define COLOR_OKGREEN "\033[92m"
#define COLOR_WARNING "\033[31m"
#define COLOR_MAJORWARNING "\033[41m"
#define COLOR_ENDC "\033[0m"
#define COLOR_BOLD "\033[1m"
#define COLOR_UNDERLINE "\033[4m"
#define COLOR_FAIL "\033[41m" COLOR_BOLD
#define COLOR_ERROR COLOR_FAIL
#define COLOR_FATAL COLOR_FAIL

#define MY_MSG_VERBOSE( xmsg ) ATH_MSG_VERBOSE( COLOR_OKBLUE << xmsg << COLOR_ENDC) 
#define MY_MSG_DEBUG( xmsg )   ATH_MSG_DEBUG( COLOR_OKBLUE << xmsg << COLOR_ENDC) 
#define MY_MSG_INFO( xmsg )    ATH_MSG_INFO( xmsg )
#define MY_MSG_WARNING( xmsg ) ATH_MSG_WARNING(COLOR_WARNING << xmsg << COLOR_ENDC)
#define MY_MSG_ERROR( xmsg )   ATH_MSG_ERROR(COLOR_ERROR << xmsg << COLOR_ENDC)
#define MY_MSG_FATAL( xmsg )   ATH_MSG_FATAL( COLOR_FATAL << xmsg << COLOR_ENDC) 
#define MY_MSG_ALWAYS( xmsg )  ATH_MSG_ALWAYS( xmsg )

#endif
