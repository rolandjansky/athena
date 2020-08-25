/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_STANDALONE

#include "AsgDataHandles/VarHandleKeyArray.h"

// namespace Gaudi { 
//   namespace Parsers {
    
    
// /**
//  * @brief Gaudi function used to initialize a property from a string.
//  * @param v The object to initialize.
//  * @param s The string from which to initialize.
//  *
//  * Used during Gaudi property handling to set object @c v from the string @c s.
//  * Note that @c s is a representation of the property setting; thus, in the
//  * case of setting a property from a string, @c s will contain quote marks.
//  */
//     StatusCode 
//     GAUDI_API
//     parse(SG::VarHandleKeyArray& v, const std::string& s)
//     {
//       std::vector<std::string> vp;
//       StatusCode sc = Gaudi::Parsers::parse(vp, s);
      
//       if (sc.isSuccess())
//         sc = v.assign( vp );
      
//       return sc;
//     }
    
//   } //> ns Parsers

//   namespace Utils {
    
    
// /**
//  * @brief Gaudi function used to convert a property to a string.
//  * @param v The object to convert.
//  * @param o Stream to which to do the conversion.
//  *
//  * Used during Gaudi property handling to get a string representation of @c v.
//  * Note that if the representation is a string, it should be surrounded
//  * by quote marks.
//  */
//     std::ostream& 
//     GAUDI_API
//     toStream(const SG::VarHandleKeyArray& v, std::ostream& o)
//     {
//       o << "[" << v.toString() << "]";      
//       return o;
//     }
    
//   } //> ns Utils
// } //> ns Gaudi




namespace std {
  ostream& operator<<(ostream& s, const SG::VarHandleKeyArray& m) {
    s << "[" <<  m.toString() << "]";
    return s;
  }
}

#endif
