/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 type generator that associates a container with an IndexingPolicy. 
 Used by ElementLink
 Please define a (partial) specialization for your container:
 the default value provided here generates a compiler error. 
 The specializations for the STL containers are available in StoreGate/tools
 ----------------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: DefaultIndexingPolicy.h,v 1.1 2003-04-07 23:58:32 calaf Exp $

#ifndef ATHLINKS_TOOLS_DEFAULTINDEXINGPOLICY_H
# define ATHLINKS_TOOLS_DEFAULTINDEXINGPOLICY_H


template <class Foo> class ERROR_PLEASE_ADD_ONE_OF_THE_PREDEFINED_POLICY_INCLUDE_FILES_OR_DEFINE_A_DEFAULT_INDEXING_POLICY_FOR_THIS_TYPE;
template <typename STORABLE>
struct DefaultIndexingPolicy {
  typedef ERROR_PLEASE_ADD_ONE_OF_THE_PREDEFINED_POLICY_INCLUDE_FILES_OR_DEFINE_A_DEFAULT_INDEXING_POLICY_FOR_THIS_TYPE<STORABLE> type;
};

#endif // ATHLINKS_TOOLS_DEFAULTINDEXINGPOLICY_H
