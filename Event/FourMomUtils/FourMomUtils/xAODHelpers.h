///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODHelpers.h
// Header file for xAOD helper functions
// Author: karsten.koeneke@cern.ch
///////////////////////////////////////////////////////////////////
#ifndef FOURMOMUTILS_XAODHELPERS_H
#define FOURMOMUTILS_XAODHELPERS_H

/**
   Helpers provides static helper functions for easy access to auxiliary data
   for xAOD classed. This is useful for python, as well as TFormula.
   This specialization to the above method is needed since ROOT 6.02 doesn't
   yet support C++11 in the TFormula stuff with JIT compilation, see:
   https://sft.its.cern.ch/jira/browse/ROOT-5083

   TODO: Note that this may not work as TTree->Draw("bla") seems to have issues
         with calling a function that takes a string as an argument...
         Thus, this file may be removed, if no permanent solution is found!

   @author Karsten Koeneke karsten.koeneke@cern.ch
 */


// EDM includes
#include "AthContainers/AuxElement.h"


namespace xAOD
{
  namespace Helpers
  {

    class _importHelper { /* dummy */  };
    
// Define a pre-processor macro for the implementations of all these methods
#define GET_VAL( FUNCNAME, VALTYPE, PERSVALTYPE )                                      \
    inline bool FUNCNAME( const SG::AuxElement& auxEle, const std::string& varName ) { \
      SG::AuxElement::ConstAccessor<PERSVALTYPE> acc(varName);                         \
      return static_cast<VALTYPE>( acc(auxEle) );                                      \
    }

    /// Access to get a variable of the different types
    GET_VAL( getBool,   bool,         char )
    GET_VAL( getInt,    int,          int )
    GET_VAL( getUInt,   unsigned int, unsigned int )
    GET_VAL( getFloat,  float,        float )
    GET_VAL( getDouble, double,       double )



// Define a pre-processor macro for the implementations of all these methods
#define SET_VAL( FUNCNAME, VALTYPE, PERSVALTYPE )                                           \
  inline bool FUNCNAME( SG::AuxElement& auxEle, const std::string& varName, VALTYPE val ) { \
    SG::AuxElement::Accessor<PERSVALTYPE> acc(varName);                                     \
    if ( acc.isAvailable(auxEle) && !(acc.isAvailableWritable(auxEle)) ) {                  \
      return false;                                                                         \
    }                                                                                       \
    acc(auxEle) = static_cast<PERSVALTYPE>(val);                                            \
    return true;                                                                            \
  }

    /// Set a variable of the different types
    SET_VAL(setBool,   bool,         char )
    SET_VAL(setInt,    int,          int )
    SET_VAL(setUInt,   unsigned int, unsigned int )
    SET_VAL(setFloat,  float,        float)
    SET_VAL(setDouble, double,       double)



// Define a pre-processor macro for the implementations of all these methods
#define DECO_VAL( FUNCNAME, VALTYPE, PERSVALTYPE )                                                \
  inline bool FUNCNAME( const SG::AuxElement& auxEle, const std::string& varName, VALTYPE val ) { \
    SG::AuxElement::Decorator<PERSVALTYPE> deco(varName);                                         \
    if ( deco.isAvailable(auxEle) && !(deco.isAvailableWritable(auxEle)) ) {                      \
      return false;                                                                               \
    }                                                                                             \
    deco(auxEle) = static_cast<PERSVALTYPE>(val);                                                 \
    return true;                                                                                  \
  }

    /// Decorate an object with a variable of the different types
    SET_VAL(decoBool,   bool,         char )
    SET_VAL(decoInt,    int,          int )
    SET_VAL(decoUInt,   unsigned int, unsigned int )
    SET_VAL(decoFloat,  float,        float)
    SET_VAL(decoDouble, double,       double)

  } //> namespace Helpers

} // End: namespace xAOD


#endif // FOURMOMUTILS_XAODHELPERS_H
