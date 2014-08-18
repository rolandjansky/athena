/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLE_P1_H
#define DATAMODELATHENAPOOL_NAVIGABLE_P1_H

/** @file Navigable_p1.h
 *  @brief This file contains the class definition for the Navigable_p1 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "ElementLinkVector_p1.h"
//#include "Navigable/NavigableDefaults.h"

/** @class Navigable_p1<INDEX_TYPE>
 *  @brief Persistent representation of Navigable - which is in fact an ElementLinkVector with additional weight parameter
 **/
 
template <typename INDEXTYPE, typename RPAR=void>
class Navigable_p1
{
public:
  Navigable_p1() {}

  ElementLinkVector_p1< INDEXTYPE >	m_links;
  std::vector<RPAR>			m_parameters;
};


template <typename INDEXTYPE>
class Navigable_p1<INDEXTYPE, void>
{
public:
  Navigable_p1() {}

  ElementLinkVector_p1< INDEXTYPE >	m_links;
};



// Helper template to generate type of the persistent Navigable_pN
// from the type of the transient Navigable
// Used in the NavigableCnv_pN 

namespace NavigationDefaults {
   struct DefaultWeight;
}


template <class NAV, typename RPAR = typename NAV::parameter_type >
struct GeneratePersNavigableType
{
  typedef  Navigable_p1<
     typename GenerateELinkIndexType_p1<typename NAV::external_index_type>::type,
     RPAR >  type;
};


template <class NAV >
struct GeneratePersNavigableType<NAV, NavigationDefaults::DefaultWeight >
{
  typedef  Navigable_p1<
     typename GenerateELinkIndexType_p1<typename NAV::external_index_type>::type
     > type;
};



// If you define more Navigable_pN types, remember to add template instantiations to the Dict file

typedef Navigable_p1<uint32_t>		NavigableInt_p1;
typedef Navigable_p1<std::string>	NavigableStr_p1;

				 
#endif



