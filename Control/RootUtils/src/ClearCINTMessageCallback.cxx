/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClearCINTMessageCallback.cxx,v 1.1 2007-10-19 21:03:00 ssnyder Exp $
/**
 * @file RootUtils/src/ClearCINTMessageCallback.cxx
 * @author scott snyder
 * @date Oct 2007
 * @brief Clear Cint's error message callback.
 */


#include "RootUtils/ClearCINTMessageCallback.h"
#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "Api.h"
#endif


namespace RootUtils {


/**
 * @brief Clear Cint's error message callback.
 */
void ClearCINTMessageCallback::initialize()
{
  // Doesn't do anything in root 6 (no cint!).
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  G__set_errmsgcallback (0);
#endif
}


} // namespace RootUtils
