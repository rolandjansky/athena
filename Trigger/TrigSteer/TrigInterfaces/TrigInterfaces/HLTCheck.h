/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigInterfaces_HLTCheck_h
#define TrigInterfaces_HLTCheck_h

#include "TrigSteeringEvent/Enums.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#define HLT_CHECK( __statement__ )					\
  do{									\
    const HLT::ErrorCode __ec = __statement__;				\
    if ( __ec != HLT::OK ) {						\
      ATH_MSG_WARNING("Failure in invocation \"" << #__statement__ <<"\"");	\
      return __ec;                                                        \
    }	                         					\
  }while(0)


#define HLT_CHECK_SC(  __statement__, __ec_if_failure__ )                        	\
  do{									\
       const StatusCode __sc = __statement__;			                 	\
       if ( __sc.isFailure() ) {					                \
         if ( __ec_if_failure__.action() == HLT::Action::ABORT_JOB )                    \
           ATH_MSG_ERROR("Failure in invocation \"" << #__statement__ <<"\", aborting job");   \
	 else            								\
           ATH_MSG_WARNING("Failure in invocation \"" << #__statement__ <<"\"");               \
         return __ec_if_failure__;					        	\
        }						         			\
     }while(0)

#endif
