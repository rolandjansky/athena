/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_ELEMENTLINK_P3_H
#define DATAMODELATHENAPOOL_ELEMENTLINK_P3_H

/** @file ElementLink_p3.h
 *  @brief This file contains the class definition for the ElementLink_p3 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include <string>
#include <inttypes.h>

/** @class ElementLink_p3<INDEX>
 *  @brief Persistent representation of ElementLink - constains a hash of the StoreGate key (name) of the container it is pointing to, and an index to the container element. The C++ type of index can be integer for sequences or a "key" type for sets (like std::map). (std::string is supported, define more types if necessary)
 **/
 
template <class INDEXTYPE>
class ElementLink_p3 {
public:

  ElementLink_p3() : m_SGKeyHash(0) { }
  
  INDEXTYPE	m_elementIndex;
  uint32_t      m_SGKeyHash; 
};


// Specialize the constructor so <int> index type is initialized with 0
template<> inline ElementLink_p3<uint32_t>::ElementLink_p3()
      : m_elementIndex(0), m_SGKeyHash(0)
{ }


// If you define more ElementLink_pN types, remember to add template instantiations to the Dict file

typedef ElementLink_p3<uint32_t>	ElementLinkInt_p3;
typedef ElementLink_p3<std::string>	ElementLinkStr_p3;


// Helper template for conversion from the index type in transient ElementLink
// to the persistent index type

template <typename ContIndxType>
struct GenerateELinkIndexType_p3 {
//  typedef MISSING_DEFINITION_OF_GenerateELinkIndexType_FOR_<ContIndxType>	type;
};

template<>
struct GenerateELinkIndexType_p3<uint32_t> {
   typedef	uint32_t	type;
};

template<>
struct GenerateELinkIndexType_p3<uint64_t> {
    typedef	uint32_t	type;
};

template<>
struct GenerateELinkIndexType_p3<int> {
    typedef	uint32_t	type;
};

#ifdef __APPLE__
template<>
struct GenerateELinkIndexType_p3<size_t> {
    typedef	uint32_t	type;
};
#endif

template<>
struct GenerateELinkIndexType_p3<std::string> {
   typedef	std::string	type;
};


// Helper template to generate type of the persistent ElementLink_pN
// from the type of the transient ElementLink
// Used in the ElementlinkCnv_pN

template <class LINK>
struct GeneratePersELinkType_p3 {
  typedef  ElementLink_p3<typename GenerateELinkIndexType_p3<typename LINK::index_type>::type >	type;
};
				 
#endif // DATAMODELATHENAPOOL_ELEMENTLINK_P3_H



