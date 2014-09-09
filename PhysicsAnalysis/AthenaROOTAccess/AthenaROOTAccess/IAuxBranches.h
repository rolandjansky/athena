// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file IAuxBranches.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2014
 * @brief xxx
 */


#ifndef ATHENAROOTACCESS_IAUXBRANCHES_H
#define ATHENAROOTACCESS_IAUXBRANCHES_H


#include "AthContainersInterfaces/AuxTypes.h"
#include "GaudiKernel/StatusCode.h"
#include <utility>
#include <string>
#include <vector>
class TBranch;


namespace AthenaROOTAccess {


class IAuxBranches
{
public:
  virtual ~IAuxBranches() {}

  typedef std::pair<std::string, TBranch*> auxpair_t;
  virtual std::vector<auxpair_t> auxBranches() const = 0;
  virtual TBranch* findAuxBranch (SG::auxid_t auxid) const = 0;
  virtual StatusCode readAuxBranch (TBranch& br, void* p, long long entry) = 0;
};


} // namespace AthenaROOTAccess



#endif // not ATHENAROOTACCESS_IAUXBRANCHES_H
