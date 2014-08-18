/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_NAVIGABLEVECTOR_P1_H
#define DATAMODELATHENAPOOL_NAVIGABLEVECTOR_P1_H

/** @file NavigableVector_p1.h
 *  @brief This file contains the class definition for the NavigableVector_p1 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "ElementLinkVector_p1.h"

/** @class NavigableVector_p1<INDEX_TYPE>
 *  @brief Persistent representation of set of Navigables - which is in fact an ElementLinkVector with additional weight parameter
 **/
 
template <typename INDEXTYPE, typename RPAR=void>
class NavigableVector_p1
{
public:
  NavigableVector_p1() {}
  size_t	size() const { return m_navigableSizes.size(); }

  ElementLinkVector_p1< INDEXTYPE >	m_links;
  std::vector<RPAR>			m_parameters;
  std::vector<unsigned short>		m_navigableSizes;
};


// specialization of the above class for non-weighted Navigables
template <typename INDEXTYPE>
class NavigableVector_p1<INDEXTYPE, void>
{
public:
  NavigableVector_p1() {}
  size_t	size() const { return m_navigableSizes.size(); }

  ElementLinkVector_p1< INDEXTYPE >	m_links;
  std::vector<unsigned short>		m_navigableSizes;
};



// Helper template to generate type of the persistent NavigableVector_pN
// from the type of the transient Navigable
// Used in the NavigableVectorCnv_pN 

namespace NavigationDefaults {
   struct DefaultWeight;
}


template <class NAV, typename RPAR = typename NAV::parameter_type >
struct GeneratePersNavVectorType
{
  typedef  NavigableVector_p1<
     typename GenerateELinkIndexType_p1<typename NAV::external_index_type>::type,
     RPAR >  type;
};


template <class NAV >
struct GeneratePersNavVectorType<NAV, NavigationDefaults::DefaultWeight >
{
  typedef  NavigableVector_p1<
     typename GenerateELinkIndexType_p1<typename NAV::external_index_type>::type
     > type;
};



// If you define more Navigable_pN types, remember to add template instantiations to the Dict file

typedef NavigableVector_p1<uint32_t>	NavigableVectorInt_p1;
typedef NavigableVector_p1<std::string>	NavigableVectorStr_p1;

				 
#endif



