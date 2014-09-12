// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthContainers/UserDataStore.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxVectorData.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxElement.h"
#include "AthContainersInterfaces/IAuxElement.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthLinks/DataLink.h"


struct AthContainersInstan
{
  std::pair<SG::auxid_set_t::iterator, bool> p1;
  std::pair<SG::auxid_set_t::const_iterator, bool> p2;
};


inline bool
operator!=(const CxxUtils_Internal::hashtable_iterator<unsigned long, true, false>& x,
           const CxxUtils_Internal::hashtable_iterator<unsigned long, true, false>& y)
{
  typedef CxxUtils_Internal::hashtable_iterator_base<unsigned long, false> base;
  return static_cast<const base&>(x) != static_cast<const base&>(y);
}

inline bool
operator!=(const CxxUtils_Internal::hashtable_const_iterator<unsigned long, true, false>& x,
           const CxxUtils_Internal::hashtable_const_iterator<unsigned long, true, false>& y)
{
  typedef CxxUtils_Internal::hashtable_iterator_base<unsigned long, false> base;
  return static_cast<const base&>(x) != static_cast<const base&>(y);
}


inline bool
operator==(const CxxUtils_Internal::hashtable_iterator<unsigned long, true, false>& x,
           const CxxUtils_Internal::hashtable_iterator<unsigned long, true, false>& y)
{
  typedef CxxUtils_Internal::hashtable_iterator_base<unsigned long, false> base;
  return static_cast<const base&>(x) == static_cast<const base&>(y);
}

inline bool
operator==(const CxxUtils_Internal::hashtable_const_iterator<unsigned long, true, false>& x,
           const CxxUtils_Internal::hashtable_const_iterator<unsigned long, true, false>& y)
{
  typedef CxxUtils_Internal::hashtable_iterator_base<unsigned long, false> base;
  return static_cast<const base&>(x) == static_cast<const base&>(y);
}


namespace {
  struct GCCXML_DUMMY_INSTANTIATION_ATHCONTAINERS {
    DataLink<SG::IConstAuxStore> dummy1;
   };
}

#define ARGS1 (const std::string&)
#define ARGS2 (const std::string&, const std::string&)
#define INSTAN_TYPE(TYP) \
  template class SG::AuxElement::ConstAccessor<TYP>; \
  template class SG::AuxElement::Accessor<TYP>; \
  template class SG::AuxElement::Decorator<TYP>; \
  template TYP& SG::AuxElement::auxdata<TYP> ARGS1; \
  template TYP& SG::AuxElement::auxdata<TYP> ARGS2; \
  template const TYP& SG::AuxElement::auxdata<TYP> ARGS1 const; \
  template const TYP& SG::AuxElement::auxdata<TYP> ARGS2 const; \
  template const TYP& SG::AuxElement::auxdataConst<TYP> ARGS1 const; \
  template const TYP& SG::AuxElement::auxdataConst<TYP> ARGS2 const; \
  template TYP& SG::AuxElement::auxdecor<TYP> ARGS1 const; \
  template TYP& SG::AuxElement::auxdecor<TYP> ARGS2 const

INSTAN_TYPE(char);
INSTAN_TYPE(unsigned char);
INSTAN_TYPE(int);
INSTAN_TYPE(short);
INSTAN_TYPE(unsigned int);
INSTAN_TYPE(unsigned short);
INSTAN_TYPE(unsigned long);
INSTAN_TYPE(unsigned long long);
INSTAN_TYPE(float);
INSTAN_TYPE(double);
INSTAN_TYPE(bool);
INSTAN_TYPE(std::string);
INSTAN_TYPE(std::vector<float>);

#undef ARGS1
#undef ARGS2
#undef INSTAN_TYPE
