/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IOVSVCDEFS_H
#define ATHENAKERNEL_IOVSVCDEFS_H
/**
 * @file IOVSvcDefs.h
 * @author Charles Leggett
 *  $Id: IOVSvcDefs.h,v 1.3 2007-06-14 01:57:23 calaf Exp $ 
 * @brief defines and typedefs for IOVSvc 
 */

#include <list>
#include <string>

/**
 * @brief short hand for IOVSvc call back argument list, to be used
 * when no access to formal arguments is needed, e.g. in method declaration (.h)
 * @code
 * StatusCode callBack( IOVSVC_CALLBACK_ARGS );
 * @endcode
 */
#define IOVSVC_CALLBACK_ARGS int&,std::list<std::string>&

/**
 * @brief short hand for IOVSvc call back argument list, to be used
 * when when only the keys argument is needed.
 * @code
 * StatusCode CoolHistExample::callBack( IOVSVC_CALLBACK_ARGS_K(keys) ) {
 * @endcode
 */
#define IOVSVC_CALLBACK_ARGS_K(K) int&,std::list<std::string>& K

/**
 * @brief short hand for IOVSvc call back argument list, to be used 
 * when access to formal arguments is needed, e.g. in method definition (.cxx)
 * @code
 * StatusCode CoolHistExample::callBack( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {
 * @endcode
 */
#define IOVSVC_CALLBACK_ARGS_P(I,K) int& I,std::list<std::string>& K


#include "boost/function.hpp"


#ifndef KERNEL_STATUSCODES_H
 #include "GaudiKernel/StatusCode.h"
#endif

/* This can also be done as:
typedef boost::function2< StatusCode, IOVSVC_CALLBACK_ARGS > IOVSvcCallBackFcn;
*/

/// the type of an IOVSvc call back: it wraps both the method and the object
/// the method is called on
typedef boost::function< StatusCode (IOVSVC_CALLBACK_ARGS) > IOVSvcCallBackFcn;

#endif
