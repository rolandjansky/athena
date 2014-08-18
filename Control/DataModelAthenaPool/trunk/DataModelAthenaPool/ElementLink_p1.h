/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_ELEMENTLINK_P1_H
#define DATAMODELATHENAPOOL_ELEMENTLINK_P1_H

/** @file ElementLink_p1.h
 *  @brief This file contains the class definition for the ElementLink_p1 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include <string>
#include <inttypes.h>

/** @class ElementLink_p1<INDEX>
 *  @brief Persistent representation of ElementLink - constains StoreGate key (name) of the container it is pointing to, and an index to the container element. The C++ type of index can be integer for sequences or a "key" type for sets (like std::map). (std::string is supported, define more types if necessary)
 **/
 
template <class INDEXTYPE>
class ElementLink_p1 {
public:

  ElementLink_p1() : m_elementIndex(), m_SGKeyHash(0) {}

  std::string	m_contName;
  INDEXTYPE	m_elementIndex;
  uint32_t      m_SGKeyHash;     // replaces m_contName
};


// If you define more ElementLink_pN types, remember to add template instantiations to the Dict file

typedef ElementLink_p1<uint32_t>	ElementLinkInt_p1;
typedef ElementLink_p1<std::string>	ElementLinkStr_p1;


// Helper template for conversion from the index type in transient ElementLink
// to the persistent index type

template <typename ContIndxType>
struct GenerateELinkIndexType_p1 {
//  typedef MISSING_DEFINITION_OF_GenerateELinkIndexType_FOR_<ContIndxType>	type;
};

template<>
struct GenerateELinkIndexType_p1<uint32_t> {
   typedef	uint32_t	type;
};

template<>
struct GenerateELinkIndexType_p1<uint64_t> {
    typedef	uint32_t	type;
};

template<>
struct GenerateELinkIndexType_p1<int> {
    typedef	uint32_t	type;
};

#ifdef __APPLE__
template<>
struct GenerateELinkIndexType_p1<size_t> {
    typedef	uint32_t	type;
};
#endif

template<>
struct GenerateELinkIndexType_p1<std::string> {
   typedef	std::string	type;
};


// Helper template to generate type of the persistent ElementLink_pN
// from the type of the transient ElementLink
// Used in the ElementlinkCnv_pN

template <class LINK>
struct GeneratePersELinkType_p1 {
  typedef  ElementLink_p1<typename GenerateELinkIndexType_p1<typename LINK::index_type>::type >	type;
};
				 
#endif // DATAMODELATHENAPOOL_ELEMENTLINK_P1_H



