/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODMissingET/versions/MissingETContainer_v1.h"
#include <functional>

using namespace xAOD;

MissingETContainer_v1::MissingETContainer_v1(SG::OwnershipPolicy ownPolicy,SG::IndexTrackingPolicy trackIndices)
  : DataVector<xAOD::MissingET_v1>(ownPolicy,trackIndices)
{ }

MissingETContainer_v1::MissingETContainer_v1(MissingETContainer_v1::iterator first, MissingETContainer_v1::iterator last,
					     SG::OwnershipPolicy ownPolicy,SG::IndexTrackingPolicy trackIndices)
  : DataVector<xAOD::MissingET_v1>(first,last,ownPolicy,trackIndices)
{ }

MissingETContainer_v1::~MissingETContainer_v1()
{ }


const MissingET_v1* MissingETContainer_v1::operator[](const std::string& name) const
{
  const_iterator fObj(this->find(name));
  return fObj != this->end() ? *fObj : (const MissingET_v1*)nullptr;
}

MissingET_v1* MissingETContainer_v1::operator[](const std::string& name)
{
  iterator fObj(this->find(name));
  return fObj != this->end() ? *fObj : (MissingET_v1*)nullptr;
}

// MissingETContainer_v1::const_iterator MissingETContainer_v1::find(const std::string& name) const
// {
//   const_iterator fObj(this->begin());
//   const_iterator lObj(this->end());
//   while ( fObj != lObj && (*fObj)->name() != name ) { ++fObj; }
//   return fObj;
// }

// MissingETContainer_v1::iterator MissingETContainer_v1::find(const std::string& name)
// {
//   iterator fObj(this->begin());
//   iterator lObj(this->end());
//   while ( fObj != lObj && (*fObj)->name() != name ) { ++fObj; }
//   return fObj;
// }

MissingETContainer_v1::const_iterator MissingETContainer_v1::find(const std::string& name) const
{
  const_iterator fObj(this->begin());
  const_iterator lObj(this->end());
  size_t hash_tmp = std::hash<std::string>()(name);
  while ( fObj != lObj && (*fObj)->nameHash() != hash_tmp ) { ++fObj; }
  return fObj;
}

MissingETContainer_v1::iterator MissingETContainer_v1::find(const std::string& name)
{
  iterator fObj(this->begin());
  iterator lObj(this->end());
  size_t hash_tmp = std::hash<std::string>()(name);
  while ( fObj != lObj && (*fObj)->nameHash() != hash_tmp ) { ++fObj; }
  return fObj;
}

MissingETContainer_v1::const_iterator MissingETContainer_v1::find(MissingETBase::Types::bitmask_t src) const
{
  const_iterator fObj(this->begin());
  const_iterator lObj(this->end());
  while ( fObj != lObj && !MissingETBase::Source::hasPattern((*fObj)->source(),src) ) { ++fObj; }
  return fObj;
}

MissingETContainer_v1::iterator MissingETContainer_v1::find(MissingETBase::Types::bitmask_t src)
{
  iterator fObj(this->begin());
  iterator lObj(this->end());
  while ( fObj != lObj && !MissingETBase::Source::hasPattern((*fObj)->source(),src) ) { ++fObj; }
  return fObj;
}

