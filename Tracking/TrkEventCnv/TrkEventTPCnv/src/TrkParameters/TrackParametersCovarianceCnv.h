// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackParametersCovarianceCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2015
 * @brief 
 */


#ifndef TRKEVENTTPCNV_TRACKPARAMETERSCOVARIANCECNV_H
#define TRKEVENTTPCNV_TRACKPARAMETERSCOVARIANCECNV_H


#include "TrkParametersBase/ParametersBase.h"

template <typename PARMS>
class TrackParametersCovarianceCnv
{
public:
  static void setCovariance (PARMS* p,
                             const Trk::ErrorMatrix& mat);
};


template <typename PARMS>
void TrackParametersCovarianceCnv<PARMS>::setCovariance (PARMS* p,
                                                         const Trk::ErrorMatrix& mat)
{
   if (!p->m_covariance || p->m_covariance->size() != PARMS::dim) {
     p->m_covariance = std::make_unique<AmgSymMatrix(PARMS::dim)>();
   }
   if (mat.values.size() == PARMS::dim*(PARMS::dim+1)/2)
     EigenHelpers::vectorToEigenMatrix(mat.values, *p->m_covariance, "TrackParametersCovarianceCnv");
   else if (mat.values.size() == 6) {
     auto& cov = *p->m_covariance;
     cov.setZero();
     unsigned int pos = 0;
     for (unsigned int i=0; i < 3; i++)
       for (unsigned int j=0; j <= i; j++)
         cov.fillSymmetric (i, j, mat.values[pos++]);
   }
   else
     std::abort();
}


#endif // not TRKEVENTTPCNV_TRACKPARAMETERSCOVARIANCECNV_H
