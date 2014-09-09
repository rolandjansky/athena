// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: branchSeek.h,v 1.2 2008-04-15 18:53:38 ssnyder Exp $

/**
 * @file AthenaROOTAccess/branchSeek.h
 * @author scott snyder
 * @date Nov 2007
 * @brief Seek to a given element of a branch.
 *
 * This is a little helper function to seek to a given element of a branch.
 * If the branch is disabled, then it will be re-enabled.
 */

#ifndef ATHENAROOTACCESS_BRANCHSEEK_H
#define ATHENAROOTACCESS_BRANCHSEEK_H


#include "TBranch.h"


namespace AthenaROOTAccess {


const int NOREAD = -9999;


/**
 * @brief Seek to a given element of a branch.
 * @param br The branch to seek.
 * @param entry The entry to which to seek.
 * @return The status returned from @c GetEntry, or NOREAD if no read was done.
 *
 * Nothing will be done if we're already at the correct entry.
 * If the branch is disabled, it will be automatically enabled.
 */
int branchSeek (TBranch* br, Long64_t entry);


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_BRANCHSEEK_H

