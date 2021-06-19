/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EventPrimitivesHelpers.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef EVENTPRIMITIVES_EVENTPRIMITIVESHELPERS_H
#define EVENTPRIMITIVES_EVENTPRIMITIVESHELPERS_H

#include "EventPrimitives/EventPrimitives.h"
#include "cmath"
#include <iostream>
#include <vector>

/** Event primitives helper functions
 @author  Niels van Eldik
 @author  Robert Johannes Langenberg <robert.langenberg@cern.ch>
 @author  Andreas Salzburger <Andreas.Salzburger@cern.ch>

 */

namespace Amg {
/**  Sometimes the extrapolation to the next surface succeeds but has termendoulsy large errors leading to uncertainties larger 
    than the radius of the Geneva metropole. These ones themself are clearly unphysical, but if extrapolation continues 
    to the next surface the numerical values blow up giving rise to floating point exception. The covariance_cutoff defines a maximum value
    for the diagonal elements of the covariance matrix to consider them as sanish
*/

inline bool saneCovarianceElement(double ele) {
    constexpr double upper_covariance_cutoff = 1.e34;
    return !(std::isnan(ele) || std::isinf(ele) || std::abs(ele) >= upper_covariance_cutoff );
} 
/** return diagonal error of the matrix
 caller should ensure the matrix is symmetric and the index is in range
 */
inline double error(const Amg::MatrixX& mat, int index) {
    return std::sqrt(mat(index, index));
}
/// Returns true if all diagonal elements of the covariance matrix
/// are finite, greater than zero and also below the covariance cutoff scale.
/// This check avoids floating point exceptions raised during the uncertainty calculation on the perigee parameters 
/// Add a double epislon parameter to avoid numerical precision issues. The one chosen here has been
/// taken from https://stackoverflow.com/questions/1566198/how-to-portably-get-dbl-epsilon-in-c-and-c
template <int N> inline bool valid_cov(const AmgSymMatrix(N)& mat) {
   static const double MIN_COV_EPSILON = 2.2204460492503131e-16;
   const int dim = mat.cols();
   for (int i = 0; i < dim ; ++i){
        if ( mat(i,i) <= MIN_COV_EPSILON || !saneCovarianceElement(mat(i,i)))  return false;
   }
   return true;
}
template<int N>
inline double error(const AmgSymMatrix(N)& mat, int index ) {
    assert(index<N);
    return std::sqrt(mat(index,index));
}

// expression template to evaluate the required size of the compressed matrix at compile time
template<int N>
struct CalculateCompressedSize {
    static const int value = N + CalculateCompressedSize<N - 1>::value;
};

template<>
struct CalculateCompressedSize<1> {
    static const int value = 1;
};

template<int N>
inline void compress(const AmgSymMatrix(N)& covMatrix, std::vector<float>& vec ) {
    vec.reserve(CalculateCompressedSize<N>::value);
    for (unsigned int i = 0; i < N ; ++i)
      for (unsigned int j = 0; j <= i; ++j)
          vec.emplace_back(covMatrix(i,j));
  }
inline void compress(const MatrixX& covMatrix, std::vector<float>& vec) {
    int rows = covMatrix.rows();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j <= i; ++j) {
            vec.emplace_back(covMatrix(i, j));
        }
    }
}

template<int N>
inline void expand(std::vector<float>::const_iterator it,
        std::vector<float>::const_iterator, AmgSymMatrix(N)& covMatrix ) {
            for (unsigned int i = 0; i < N; ++i) {
                for (unsigned int j = 0; j <= i; ++j) {
                    covMatrix.fillSymmetric(i,j, *it);
                    ++it;
                }
            }
        }

inline void expand(std::vector<float>::const_iterator it,
        std::vector<float>::const_iterator it_end, MatrixX& covMatrix) {
    unsigned int dist = std::distance(it, it_end);
    unsigned int n;
    for (n = 1; dist > n; ++n) {
        dist = dist - n;
    }
    covMatrix = MatrixX(n, n);
    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j <= i; ++j) {
            covMatrix.fillSymmetric(i, j, *it);
            ++it;
        }
    }
}

/** get largest difference between two matrices
 Users can decide whether the difference should be evaluate relative to the values or absolutely
 */
template<int N>
double largestDifference(const AmgSymMatrix(N)& m1, const AmgSymMatrix(N)& m2,bool relative = false ) {
    if( N < 1 ) return 0;
    double max = relative ? 0.5*std::abs(m1(0,0)-m2(0,0))/(m1(0,0)+m2(0,0)) : std::abs(m1(0,0)-m2(0,0));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double val = std::abs(m1(i,j)-m2(i,j));
            if( relative ) {
                val = 0.5*val/(m1(i,j)+m2(i,j));
            }
            if( val > max ) max = val;
        }
    }
    return max;
}

/** get largest difference betweem two vectors
 Users can decide whether the difference should be evaluate relative to the values or absolutely
 */
template<int N>
int largestDifference(const AmgVector(N)& m1, const AmgVector(N)& m2, bool relative = false ) {
    if( N < 1 ) return 0;
    double max = relative ? 0.5*std::abs(m1(0)-m2(0))/(m1(0)+m2(0)) : std::abs(m1(0)-m2(0));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double val = std::abs(m1(i)-m2(i));
            if( relative ) {
                val = 0.5*val/(m1(i)+m2(i));
            }
            if( val > max ) max = val;
        }
    }
    return max;
}

/** compare two matrices, returns the indices of the first element that fails the condition, returns <-1,-1> if all is ok
 Users can provide the required precision and whether the difference should be evaluate relative to the values or absolutely
 */
template<int N>
std::pair<int, int> compare(const AmgSymMatrix(N)& m1, const AmgSymMatrix(N)& m2, double precision = 1e-9, bool relative = false ) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if( relative ) {
                if( 0.5*std::abs(m1(i,j)-m2(i,j))/(m1(i,j)+m2(i,j)) > precision ) return std::make_pair(i,j);
            } else {
                if( std::abs(m1(i,j)-m2(i,j)) > precision ) return std::make_pair(i,j);
            }
        }
    }
    return std::make_pair(-1,-1);
}

/** compare two vectors, returns the indices of the first element that fails the condition, returns <-1,-1> if all is ok
 Users can provide the required precision and whether the difference should be evaluate relative to the values or absolutely
 */
template<int N>
int compare(const AmgVector(N)& m1, const AmgVector(N)& m2, double precision = 1e-9, bool relative = false ) {
    for (int i = 0; i < N; ++i) {
        if( relative ) {
            if( 0.5*std::abs(m1(i)-m2(i))/(m1(i)+m2(i)) > precision ) return i;
        } else {
            if( std::abs(m1(i)-m2(i)) > precision ) return i;
        }
    }
    return -1;
}

template<int N>
bool isSymMatrix(const AmgSymMatrix(N)& m ) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= i; ++j) {
            // check that the diagonal is filled with positive values
        if( i==j ) {
            if( m(i,j) < 0. ) return false;
        } else {
            // check that the off-diagonal elements are symmetric
            if( std::abs(m(i,j)-m(j,i)) > 1e-9 ) return false;
        }
    }
}
return true;
}

}

#endif
