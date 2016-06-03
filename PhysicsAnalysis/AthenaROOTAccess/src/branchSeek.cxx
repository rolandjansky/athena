/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: branchSeek.cxx,v 1.2 2008-04-15 18:53:35 ssnyder Exp $
/**
 * @file AthenaROOTAccess/src/branchSeek.cxx
 * @author scott snyder
 * @date Nov 2007
 * @brief Seek to a given element of a branch.
 */


#include "AthenaROOTAccess/branchSeek.h"
#include "RootUtils/TBranchElementClang.h"
#include "TBranch.h"
#include "TObjArray.h"
#include <cstring>


namespace AthenaROOTAccess {


/**
 * @brief Enable a @c TBranch and any descendants it might have.
 * @param branch The branch to enable.
 */
static void enableBranch (TBranch* branch)
{
  if (TBranchElement* br = dynamic_cast<TBranchElement*> (branch)) {
    TObjArray* branches = br->GetListOfBranches();
    int nbranches = branches->GetEntriesFast();
    for (int i = 0; i < nbranches; i++) {
      TBranch* b = dynamic_cast<TBranch*> (branches->At(i));
      enableBranch (b);
    }
  }

  if (branch) {
    if (std::strcmp (branch->GetName(), "TObject") != 0 &&
        std::strcmp (branch->GetName(), "TMatrixTBase<float>") != 0)
    {
      branch->ResetBit (kDoNotProcess);

      // Saved read entry may not be correct now --- invalidate it.
      branch->ResetReadEntry();
    }
  }
}


/**
 * @brief Seek to a given element of a branch.
 * @param br The branch to seek.
 * @param entry The entry to which to seek.
 * @return The status returned from @c GetEntry, or NOREAD if no read was done.
 *
 * Nothing will be done if we're already at the correct entry.
 * If the branch is disabled, it will be automatically enabled.
 */
int branchSeek (TBranch* br, Long64_t entry)
{
  int ret = NOREAD;
  if (br->TestBit (kDoNotProcess))
    enableBranch (br);

  if (br->GetReadEntry() != entry)
    ret = br->GetEntry (entry);

  return ret;
}


}
