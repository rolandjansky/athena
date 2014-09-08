/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EIGEN_HELPERS_H
#define EIGEN_HELPERS_H

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <vector>
#include "EventPrimitives/EventPrimitivesHelpers.h"

class EigenHelpers
{
public:

 /// Helper fn to fill the covariance from the raw data (vec<float>)
 /// @param VECTOR the vector<float>
 /// @param COVARIANCE The eigen covariance matrix
 /// @param NAME The name of the convertor     
 template <class VECTOR, class COVARIANCE> 
  inline static void vectorToEigenMatrix(VECTOR& vec, COVARIANCE& cov, const char* ) {      
    Amg::expand(vec.begin(), vec.end(), cov);                                          
  }
    /// Helper fn to get raw data (vec<float>) from the covariance 
    /// @param VECTOR the vector<float>
    /// @param COVARIANCE The eigen covariance matrix
    /// @param NAME The name of the convertor     
    template <class VECTOR, class COVARIANCE> 
  inline static void eigenMatrixToVector(VECTOR& vec, COVARIANCE& cov, const char* ) {                                                    
    Amg::compress(cov, vec);                                          
  }

              /** Convert std :: vector<double> to Amg::Transform3D */
        template <class T>
  static void vectorToEigenTransform3D( const T                     &vec,
    Amg::Transform3D        &trans )
  {
    trans(0, 0) = vec[0];
    trans(0, 1) = vec[1];
    trans(0, 2) = vec[2];
    trans(1, 0) = vec[3];
    trans(1, 1) = vec[4];
    trans(1, 2) = vec[5];
    trans(2, 0) = vec[6];
    trans(2, 1) = vec[7];
    trans(2, 2) = vec[8];
    trans(0, 3) = vec[9];
    trans(1, 3) = vec[10];
    trans(2, 3) = vec[11];
  }

                    /** Convert HepGeom::Transform3D to std :: vector<double>*/
              template <class T> 
  static void eigenTransform3DToVector( const Amg::Transform3D         &trans,
    T                     &vec )
  {
    vec.reserve(12);
    vec.push_back( trans(0, 0) );
    vec.push_back( trans(0, 1) );
    vec.push_back( trans(0, 2) );
    vec.push_back( trans(1, 0) );
    vec.push_back( trans(1, 1) );
    vec.push_back( trans(1, 2) );
    vec.push_back( trans(2, 0) );
    vec.push_back( trans(2, 1) );
    vec.push_back( trans(2, 2) );
    vec.push_back( trans(0, 3) );
    vec.push_back( trans(1, 3) );
    vec.push_back( trans(2, 3) );
  }
};



// template <> 
// static void EigenHelpers::vectorToEigenMatrix<typename std::vector<float>, typename Amg::Transform3D>(VECTOR& vec, TRANSFORM& transform, const char* name) {                                                    
//   size_t size = vec.size();                                                                              
//   assert (size==9);                         
//   transform<vec[0],vec[1],vec[3],
//        vec[1],vec[2],vec[4],
//        vec[3],vec[4],vec[5];
// }

#endif
