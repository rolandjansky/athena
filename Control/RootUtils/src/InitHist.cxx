/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: InitHist.cxx,v 1.3 2007-10-30 16:49:30 ssnyder Exp $
/**
 * @file  RootUtils/src/InitHist.cxx
 * @author scott snyder
 * @date Oct 2007
 * @brief Initialize Root's getline history.
 */

#include "RootUtils/InitHist.h"
#include "Rtypes.h"
#include "Getline.h"


namespace RootUtils {


/**
 * @brief Initialize Root's getline history.
 */
void InitHist::initialize()
{
  Gl_histinit(const_cast<char*> ("-"));
}


} // namespace RootUtils
