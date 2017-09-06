///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RootDataBucketBranch.cxx 
// Implementation file for class RootDataBucketBranch
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// AthenaRootComps includes
#include "RootDataBucketBranch.h"

// STL
#include <stdexcept>
#include <sstream>

namespace Athena {

DataBucketBranch::~DataBucketBranch()
{
  //std::cerr << "::~DBB[" << m_clid << "," << name() << "]\n";
}

void*
DataBucketBranch::cast(CLID clid, SG::IRegisterTransient* /*itr*/,
                       bool /*isConst*/)
{
  // no conversion needed
  if (allowMismatchCLID || clid == m_clid) {
    return m_ptr;

  } else {
    std::ostringstream err;
    err << "cannot convert from clid [" 
        << this->clID()
        << "] to requested ["
        << clid
        << "]";
    throw std::runtime_error(err.str());
  }
  return 0;
}

void*
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
DataBucketBranch::cast(const std::type_info& tinfo,
#else
DataBucketBranch::cast(const std::type_info& /*tinfo*/,
#endif
                       SG::IRegisterTransient* /*itr*/,
                       bool /*isConst*/)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
  // no conversion needed
  if (tinfo == m_type.TypeInfo()) {
    return m_ptr;
  } else {
    std::ostringstream err;
    err << "cannot convert from type-info [" 
        << System::typeinfoName(m_type.TypeInfo())
        << "] to requested ["
        << System::typeinfoName(tinfo)
        << "]";
    throw std::runtime_error(err.str());
  }
#else
  return m_ptr;
#endif
}

} //> namespace Athena
