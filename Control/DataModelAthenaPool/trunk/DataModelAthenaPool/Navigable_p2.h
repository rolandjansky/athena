/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLE_P2_H
#define DATAMODELATHENAPOOL_NAVIGABLE_P2_H

/** @file Navigable_p2.h
 *  @brief This file contains the class definition for the Navigable_p2 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "VectorElementLink_p1.h"
//#include "Navigable/NavigableDefaults.h"

/** @class Navigable_p2<INDEX_TYPE>
 *  @brief Persistent representation of Navigable - as a vector of ElementLinks with an optional, additional weight parameter
 **/
 
template <typename INDEXTYPE, typename RPAR=void>
class Navigable_p2
{
public:
  Navigable_p2() {}

  typename VectorElementLink_p1_t< INDEXTYPE >::Type	m_links;
  std::vector<RPAR>	        		        m_parameters;
};


template <typename INDEXTYPE>
class Navigable_p2<INDEXTYPE, void>
{
public:
  Navigable_p2() {}

  typename VectorElementLink_p1_t< INDEXTYPE >::Type	m_links;
};



// Helper template to generate type of the persistent Navigable_pN
// from the type of the transient Navigable
// Used in the NavigableCnv_pN 

namespace NavigationDefaults {
   struct DefaultWeight;
}


template <class NAV, typename RPAR = typename NAV::parameter_type >
struct GeneratePersNavigableType_p2
{
  typedef  Navigable_p2<
     typename GenerateELinkIndexType_p3<typename NAV::external_index_type>::type,
     RPAR >  type;
};


template <class NAV >
struct GeneratePersNavigableType_p2<NAV, NavigationDefaults::DefaultWeight >
{
  typedef  Navigable_p2<
     typename GenerateELinkIndexType_p3<typename NAV::external_index_type>::type
     > type;
};



// If you define more Navigable_pN types, remember to add template instantiations to the Dict file

typedef Navigable_p2<uint32_t>		NavigableInt_p2;
typedef Navigable_p2<std::string>	NavigableStr_p2;

				 
#endif



