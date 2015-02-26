/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGTYPEINFO_EDM_TYPEINFORMATION_H
#define TRIGTYPEINFO_EDM_TYPEINFORMATION_H

#include "DataModel/ElementLink.h"
#include "DataModel/DataLink.h"
#include "TrigStorageDefinitions/TypeInformation.h"

#define HLT_BEGIN_TYPE_REGISTRATION typedef HLT::TypeInformation::newlist::

//clever macro overloading from http://stackoverflow.com/questions/16683146/can-macros-be-overloaded-by-number-of-arguments?rq=1

#define CAT( A, B ) A ## B
#define SELECT( NAME, NUM ) CAT( NAME ## _, NUM )

#define GET_COUNT( _1, _2, _3, _4, _5, _6, COUNT, ... ) COUNT
#define VA_SIZE( ... ) GET_COUNT( __VA_ARGS__, 6, 5, 4, 3, 2, 1 )

#define VA_SELECT( NAME, ... ) SELECT( NAME, VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)


#define HLT_REGISTER_IMPL_3(OBJECT,FEATURE,CONTAINER)                \
  addWithChecking<HLT::TypeInformation::type_info<OBJECT ,HLT::TypeInformation::list<FEATURE ,HLT::\
    TypeInformation::nil>,CONTAINER > > ::go::

#define HLT_REGISTER_IMPL_4(OBJECT,FEATURE,CONTAINER,AUX)                \
  addWithChecking<HLT::TypeInformation::type_info<OBJECT ,HLT::TypeInformation::list<FEATURE ,HLT::\
    TypeInformation::nil>,CONTAINER,AUX> > ::go::

#define HLT_REGISTER_TYPE(...) VA_SELECT(HLT_REGISTER_IMPL,__VA_ARGS__)

#define HLT_END_TYPE_REGISTRATION(name) done TypeInfo_##name;

#endif
