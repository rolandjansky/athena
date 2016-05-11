/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCNV_H

/** @file T_AthenaPoolCnv.h
 *  @brief his file contains the class definition for the templated T_AthenaPoolCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCnvBase.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolViewVectorCnv.h"
#include "AthContainers/ViewVector.h"

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class T_AthenaPoolCnv
 *  @brief This templated class provides the converter to translate an object to/from its persistent POOL representation.
 **/
template <class T>
class T_AthenaPoolCnv : public T_AthenaPoolCnvBase<T> {
   friend class CnvFactory<T_AthenaPoolCnv<T> >;

protected:
   using T_AthenaPoolCnvBase<T>::T_AthenaPoolCnvBase;
};


template <class DV>
class T_AthenaPoolCnv<ViewVector<DV> > : public T_AthenaPoolViewVectorCnv<DV>
{
   friend class CnvFactory<T_AthenaPoolCnv<ViewVector<DV> > >;

protected:
   using T_AthenaPoolViewVectorCnv<DV>::T_AthenaPoolViewVectorCnv;
};


#endif
