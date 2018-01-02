// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxVectorData.h"
#include "AthContainers/AuxVectorBase.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/ViewVectorBase.h"
#include "AthContainersInterfaces/IAuxElement.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainersInterfaces/IAuxSetOption.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/PackedParameters.h"
#include "AthContainers/PackedContainer.h"
#include "AthContainers/debug.h"
#include "AthLinks/DataLink.h"


struct AthContainersInstan
{
  std::pair<SG::auxid_set_t::iterator, bool> p1;
  std::pair<SG::auxid_set_t::const_iterator, bool> p2;
};


namespace {
  struct DUMMY_INSTANTIATION_ATHCONTAINERS {
    DataLink<SG::IConstAuxStore> dummy1;

    SG::PackedContainer<char>           pchar;
    SG::PackedContainer<unsigned char>  puchar;
    SG::PackedContainer<short>          pshort;
    SG::PackedContainer<unsigned short> pushort;
    SG::PackedContainer<int>            pint;
    SG::PackedContainer<unsigned int>   puint;
    SG::PackedContainer<float>          pfloat;
    SG::PackedContainer<double>         pdouble;

    SG::PackedContainer<std::vector<char> >           pvchar;
    SG::PackedContainer<std::vector<unsigned char> >  pvuchar;
    SG::PackedContainer<std::vector<short> >          pvshort;
    SG::PackedContainer<std::vector<unsigned short> > pvushort;
    SG::PackedContainer<std::vector<int> >            pvint;
    SG::PackedContainer<std::vector<unsigned int> >   pvuint;
    SG::PackedContainer<std::vector<float> >          pvfloat;
    SG::PackedContainer<std::vector<double> >         pvdouble;

    SG::PackedContainer<std::vector<std::vector<char> > >           pvvchar;
    SG::PackedContainer<std::vector<std::vector<unsigned char> > >  pvvuchar;
    SG::PackedContainer<std::vector<std::vector<short> > >          pvvshort;
    SG::PackedContainer<std::vector<std::vector<unsigned short> > > pvvushort;
    SG::PackedContainer<std::vector<std::vector<int> > >            pvvint;
    SG::PackedContainer<std::vector<std::vector<unsigned int> > >   pvvuint;
    SG::PackedContainer<std::vector<std::vector<float> > >          pvvfloat;
    SG::PackedContainer<std::vector<std::vector<double> > >         pvvdouble;
  };
}

// Work around cling error.
template class std::vector<std::pair<unsigned int, unsigned int> >;


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
INSTAN_TYPE(long);
INSTAN_TYPE(unsigned int);
INSTAN_TYPE(unsigned short);
INSTAN_TYPE(unsigned long);
INSTAN_TYPE(unsigned long long);
INSTAN_TYPE(float);
INSTAN_TYPE(double);
INSTAN_TYPE(bool);
INSTAN_TYPE(std::string);

INSTAN_TYPE(std::vector<char>);
INSTAN_TYPE(std::vector<unsigned char>);
INSTAN_TYPE(std::vector<int>);
INSTAN_TYPE(std::vector<short>);
INSTAN_TYPE(std::vector<long>);
INSTAN_TYPE(std::vector<unsigned int>);
INSTAN_TYPE(std::vector<unsigned short>);
INSTAN_TYPE(std::vector<unsigned long>);
INSTAN_TYPE(std::vector<unsigned long long>);
INSTAN_TYPE(std::vector<float>);
INSTAN_TYPE(std::vector<double>);
INSTAN_TYPE(std::vector<bool>);
INSTAN_TYPE(std::vector<std::string>);

#undef ARGS1
#undef ARGS2
#undef INSTAN_TYPE
