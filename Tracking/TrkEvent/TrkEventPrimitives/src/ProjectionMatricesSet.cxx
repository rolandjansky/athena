/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ProjectionMatricesSet.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkEventPrimitives/ProjectionMatricesSet.h"
#include <cmath>

Trk::ProjectionMatricesSet::ProjectionMatricesSet(int maxdim) :
    m_maxdim(maxdim)
{
  
  // the number of projection matrices is 2**(maxdim),
  // it is the possible number of binary numbers of with maxdim digits
  int numMatrices = int(pow(2,m_maxdim));
  
  for (int imatx=0; imatx<numMatrices; ++imatx){

    std::vector<int>  accessorInt(m_maxdim);
    std::vector<bool> parameterTag(m_maxdim);
    unsigned int cols = 0;
    for (int itag = 0, ipos = 1; itag < m_maxdim; ++itag, ipos *= 2) {
      bool bit = (imatx & ipos);
      parameterTag[itag] = bit;
      if (bit) {
        ++cols;
      }
    }

    //By default set zero 
    Amg::MatrixX reduction;
    reduction.setZero();
    Amg::MatrixX expansion;
    expansion.setZero();
    if (cols){      
      // rows and cols - initialized to zero
      reduction = Amg::MatrixX(m_maxdim, cols);
      reduction.setZero();
      // go through the rows and fill them
      int reduc = 0;
      for (int irow = 0; irow<m_maxdim; irow++)
      {
        int icol = irow - reduc;
        // avoids couting to col 4 for int(0b01111) = 15 matrix 
        icol = (icol < int(cols)) ? icol : cols-1;
        reduction(irow,icol) = parameterTag[irow] ? 1. : 0.;
        if (!parameterTag[irow])
        { 
           accessorInt[irow] = -100;
           ++reduc;
        } else {
          accessorInt[irow] = reduc;
        }
      }
      // the expansion matrix is the transposed reduction matrix
      expansion = Amg::MatrixX(reduction.transpose());
    } 
    // store them
    m_reductions.push_back(reduction);
    m_expansions.push_back(expansion);
    m_accessors.push_back(accessorInt);
  }

}
