/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_VECTELEMENTLINK_P1_H
#define DATAMODELATHENAPOOL_VECTELEMENTLINK_P1_H

/** @file VectorElementLink_p1.h
 *  @brief This file contains the class definition for theVector ElementLink_p1 class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "ElementLink_p3.h"

/** @class VectorElementLink_p1<INDEX_TYPE>
 *  @brief Persistent representation of a vector of ElementLinks 
 The C++ type of an index in a container can be integer for sequences or a "key" type for sets (like std::map). (std::string is supported, define more types if necessary)
 **/

#include <vector>

// Simulate template typedef - usa it as in GeneratePersVectorELinkType_p1
template <typename INDEXTYPE>
struct VectorElementLink_p1_t {
  typedef   std::vector< ElementLink_p3< INDEXTYPE > >   Type;
};


// Helper template to generate type of the persistent VectorElementLink_pN<INDEXTYPE>
// from the type of the transient ElementLink
// Used in VectorElementlinkCnv_pN 

template <class TRANS_ELINK_VECT>
struct GeneratePersVectorELinkType_p1 {
  typedef  typename VectorElementLink_p1_t<typename GenerateELinkIndexType_p3<typename TRANS_ELINK_VECT::value_type::index_type>::type >::Type	type;
};



// If you define more VectorElementLink_pN types, remember to add template instantiations to the Dict file

typedef VectorElementLink_p1_t<uint32_t>::Type		VectorElementLinkInt_p1;
typedef VectorElementLink_p1_t<std::string>::Type 	VectorElementLinkStr_p1;

				 
#endif



