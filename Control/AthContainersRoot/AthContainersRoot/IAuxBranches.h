// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/IAuxBranches.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief 
 */


#ifndef ATHCONTAINERSROOT_IAUXBRANCHES_H
#define ATHCONTAINERSROOT_IAUXBRANCHES_H


#include "AthContainersInterfaces/AuxTypes.h"
#include "GaudiKernel/StatusCode.h"
#include <utility>
#include <string>
#include <vector>
#include <typeinfo>
class TBranch;


namespace SG {


class IAuxBranches
{
public:
  virtual ~IAuxBranches() {}

  virtual const std::string& getName() const = 0;
  typedef std::pair<std::string, TBranch*> auxpair_t;
  virtual std::vector<auxpair_t> auxBranches() const = 0;
  virtual TBranch* findAuxBranch (SG::auxid_t auxid) = 0;
  virtual StatusCode readAuxBranch (TBranch& br, void* p, long long entry) = 0;
  virtual StatusCode readAuxBranch (TBranch& br, void* p,
                                    const std::type_info& ti, long long entry) = 0;
};


} // namespace SG


#endif // not ATHCONTAINERSROOT_IAUXBRANCHES_H
