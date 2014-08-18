/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLEVECTOR_P2_H
#define DATAMODELATHENAPOOL_NAVIGABLEVECTOR_P2_H

/** @file NavigableVector_p2.h
 *  @brief This file contains the class definition for the NavigableVector_p2 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "VectorElementLink_p1.h"

/** @class NavigableVector_p2<INDEX_TYPE>
 *  @brief Persistent representation of set of Navigables - which is in fact an VectorElementLink with additional weight parameter
 **/
 
template <typename INDEXTYPE, typename RPAR=void>
class NavigableVector_p2
{
public:
  NavigableVector_p2() {}
  size_t	size() const { return m_navigableSizes.size(); }

  typename VectorElementLink_p1_t< INDEXTYPE >::Type	m_links;
  std::vector<RPAR>			m_parameters;
  std::vector<unsigned short>		m_navigableSizes;
};


// specialization of the above class for non-weighted Navigables
template <typename INDEXTYPE>
class NavigableVector_p2<INDEXTYPE, void>
{
public:
  NavigableVector_p2() {}
  size_t	size() const { return m_navigableSizes.size(); }

  typename VectorElementLink_p1_t< INDEXTYPE >::Type	m_links;
  std::vector<unsigned short>		m_navigableSizes;
};



// Helper template to generate type of the persistent NavigableVector_pN
// from the type of the transient Navigable
// Used in the NavigableVectorCnv_pN 

namespace NavigationDefaults {
   struct DefaultWeight;
}


template <class NAV, typename RPAR = typename NAV::parameter_type >
struct GeneratePersNavVectorType_p2
{
  typedef  NavigableVector_p2<
     typename GenerateELinkIndexType_p3<typename NAV::external_index_type>::type,
     RPAR >  type;
};


template <class NAV >
struct GeneratePersNavVectorType_p2<NAV, NavigationDefaults::DefaultWeight >
{
  typedef  NavigableVector_p2<
     typename GenerateELinkIndexType_p3<typename NAV::external_index_type>::type
     > type;
};



// If you define more Navigable_pN types, remember to add template instantiations to the Dict file

typedef NavigableVector_p2<uint32_t>	NavigableVectorInt_p2;
typedef NavigableVector_p2<std::string>	NavigableVectorStr_p2;

				 
#endif



