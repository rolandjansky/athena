// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CLASS_AUXSTORE.h 568070 2013-10-31 11:07:18Z krasznaa $
/**
 * @file AthContainersInterfaces/CLASS_AUXSTORE_.h
 * @author Paolo Calafiura
 * @date Sep, 2013
 * @brief Allow overriding defaults for associated aux store classes.
 */

#ifndef ATHCONTAINERSINTERFACES_CLASS_AUXSTORE
#define ATHCONTAINERSINTERFACES_CLASS_AUXSTORE 1


/**
 * @brief define which AUXSTORE and CONSTAUXSTORE to use for a DOBJ type.
 * CLASS_AUXSTORE specifies the default IAuxStore and IConstAuxStore interfaces
 * CLASS_AUXSTORE3 allows to specify any AuxStore type.
 */ 
#include "AthContainersInterfaces/AuxStore_traits.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IConstAuxStore.h"


#define CLASS_AUXSTORE(DOBJ)                  \
  namespace SG  {                                       \
  template <> struct AuxStore_traits< DOBJ > {      \
    typedef SG::IAuxStore type;				\
    typedef SG::IConstAuxStore const_type;		\
    static const std::string& typeName() {		\
      static const std::string s_name = "SG::IAuxStore";\
      return s_name;					\
    }							\
    static const std::string& const_typeName() {	\
      static const std::string s_name = "SG::IConstAuxStore";	\
       return s_name;					\
    }							\
  }; }
#define CLASS_AUXSTORE3(DOBJ, AUXSTORE, CONSTAUXSTORE)	\
  namespace SG  {                                       \
  template <> struct AuxStore_traits< DOBJ > {      \
    typedef AUXSTORE type;			        \
    typedef CONSTAUXSTORE const_type;			\
    static const std::string& typeName() {		\
      static const std::string s_name = #AUXSTORE;	\
      return s_name;					\
    }							\
    static const std::string& const_typeName() {	\
      static const std::string s_name = #CONSTAUXSTORE;	\
      return s_name;					\
    }							\
  }; }


#endif // not ATHCONTAINERSINTERFACES_CLASS_AUXSTORE
