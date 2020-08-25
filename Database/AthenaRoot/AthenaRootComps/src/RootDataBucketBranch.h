///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// RootDataBucketBranch.h 
// Header file for class Athena::RootDataBucketBranch
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAROOTCOMPS_ROOTDATABUCKETBRANCH_H
#define ATHENAROOTCOMPS_ROOTDATABUCKETBRANCH_H 1

// STL includes
#include <string>
#include <stdexcept>

#include "AthenaKernel/DataBucketBase.h"

#include "DataModelRoot/RootType.h"

namespace Athena {

class DataBucketBranch :
  public ::DataBucketBase
{
public:
  DataBucketBranch(CLID clid, 
                   const std::string& type_name,
                   void *ptr = NULL) :
    m_clid(clid),
    m_type(RootType(type_name)),
    m_ptr(ptr)
  {}

  DataBucketBranch(CLID clid, 
                   const std::type_info& ti,
                   void *ptr = NULL) :
    m_clid(clid),
    m_type(RootType(ti)),
    m_ptr(ptr)
  {}

  DataBucketBranch(CLID clid, RootType type, void* ptr) :
    m_clid(clid),
    m_type(type),
    m_ptr(ptr)
  {}

  virtual ~DataBucketBranch() override;

  virtual
  void* object() override;

  using DataBucketBase::cast;

  virtual
  void* cast(CLID clid, SG::IRegisterTransient* itr,
             bool isConst = true) override;

  virtual
  void* cast(const std::type_info& tinfo,
             SG::IRegisterTransient* itr,
             bool isConst = true) override;

  virtual
  void relinquish() override;

  virtual void lock() override;

  virtual const CLID& clID() const override { return m_clid; }

  virtual const std::type_info& tinfo() const override
  {
    return m_type.TypeInfo();
  }

  //private:
  CLID m_clid;
  RootType m_type;
  void* m_ptr;

  bool allowMismatchCLID=false;

};

inline
void*
DataBucketBranch::object()
{
  return m_ptr;
}

inline
void
DataBucketBranch::relinquish()
{}

inline
void
DataBucketBranch::lock()
{}

} //> end namespace Athena

#endif /* !ATHENAROOTCOMPS_ROOTDATABUCKETBRANCH_H */

