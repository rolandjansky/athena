// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackEventTPCnv/TrkParameters/TrackParametersVectorCnv_p1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2015
 * @brief Convert directly to AmgVector.
 */


#ifndef TRACKEVENTTPCNV_TRACKPARAMETERSVECTORCNV_P1_H
#define TRACKEVENTTPCNV_TRACKPARAMETERSVECTORCNV_P1_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventTPCnv/TrkParameters/TrackParameters_p1.h"
#include <stdexcept>


template <typename VEC>
class TrackParametersVectorCnv_p1
  : public T_AthenaPoolTPAbstractPolyCnvBase<VEC, VEC, Trk::TrackParameters_p1>
{
public:
  TrackParametersVectorCnv_p1() {}
  
  void persToTrans( const Trk :: TrackParameters_p1 *persObj,
		    VEC    *transObj,
		    MsgStream &/*log*/ )
  {
    if (persObj->m_parameters.size() != (size_t)transObj->rows()) {
      throw std::runtime_error("TrackParametersVectorCnv_p1::persToTrans bad size!");
    }
    for (int i = 0; i < transObj->rows(); i++)
      (*transObj)[i] = persObj->m_parameters[i];
  }
  
  void transToPers( const VEC               */*transObj*/,
		    Trk::TrackParameters_p1 */*persObj*/,
		    MsgStream &/*log*/ )
  {
    throw std::runtime_error("TrackParametersVectorCnv_p1::transToPers unimplemented!");
  }
};


#endif // not TRACKEVENTTPCNV_TRACKPARAMETERSVECTORCNV_P1_H
