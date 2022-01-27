/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_ELEMENTLINKVECT_P1_H
#define DATAMODELATHENAPOOL_ELEMENTLINKVECT_P1_H

/** @file ElementLinkVector_p1.h
 *  @brief This file contains the class definition for the ElementLinkVector_p1 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "ElementLink_p1.h"
#include "DataLink_p1.h"

/** @class ElementLinkVector_p1<INDEX_TYPE>
 *  @brief Persistent representation of ElementLinkVector - constains a list persistent ElementLinks in a compacted form (exactly in the same way as the transient ElementLinkVector)
 Different StoreGate keys (container names) all stored only once, and the list of pairs<contaner name, index> is replaced by a list of pairs<container ID, index> where the ID is an index to the list of container names
 The C++ type of an index in a container can be integer for sequences or a "key" type for sets (like std::map). (std::string is supported, define more types if necessary)
 **/
 
template <class INDEXTYPE>
class ElementLinkVector_p1 {
public:

  typedef INDEXTYPE Index_t;

  ElementLinkVector_p1() {}

  class ElementRef {
   public:
    ElementRef() : m_elementIndex(), m_nameIndex(0) {}
    ElementRef(const INDEXTYPE &elink_idx, uint32_t name_idx)
	  : m_elementIndex(elink_idx), m_nameIndex(name_idx)
    { }

    Index_t 	m_elementIndex;
    uint32_t 	m_nameIndex;
  };

  typedef std::vector<ElementRef>	ElementRefVector;

  ElementRefVector	m_elementRefs;
  DataLinkVector_p1	m_links;
};



// Helper template to generate type of the persistent ElementLinkVector_pN
// from the type of the transient ElementLinkVector
// Used in the ElementlinkVectorCnv_pN 

template <class LINK>
struct GeneratePersELinkVectorType_p1 {
  typedef  ElementLinkVector_p1<typename GenerateELinkIndexType_p1<typename LINK::index_type>::type >	type;
};



// If you define more ElementLinkVector_pN types, remember to add template instantiations to the Dict file

typedef ElementLinkVector_p1<uint32_t>		ElementLinkIntVector_p1;
typedef ElementLinkVector_p1<std::string>	ElementLinkStrVector_p1;

				 
#endif



