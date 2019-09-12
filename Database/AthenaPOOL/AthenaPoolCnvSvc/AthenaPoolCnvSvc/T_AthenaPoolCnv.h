/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCNV_H

/** @file T_AthenaPoolCnv.h
 *  @brief his file contains the class definition for the templated T_AthenaPoolCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCnvBase.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolViewVectorCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "AthContainers/ViewVector.h"
#include "AthContainers/tools/supportsThinning.h"
#include <type_traits>

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;


/**
 * @brief class to find the proper pool converter type for an EDM object.
 *
 * Generally, we use T_AthenaPoolxAODCnv for xAOD interface objects,
 * T_AthenaPoolAuxContainerCnv for xAOD auxiliary store objects,
 * and T_AthenaPoolCnvBase otherwise.
 * But we fall back to T_AthenaPoolCnvBase for an xAOD type
 * if it defines supportsThinning = false.
 * This class may also be specialized for specific types.
 */
template <class T>
struct T_AthenaPoolFindBase
{
  using type = 
    std::conditional_t<SG::xAODInterfaceSupportingThinning<T>(),
                       T_AthenaPoolxAODCnv<T>,
                       std::conditional_t<SG::xAODStoreSupportingThinning<T>(),
                                          T_AthenaPoolAuxContainerCnv<T>,
                                          T_AthenaPoolCnvBase<T> > >;
};


/// Specialize to use the proper converter for ViewVector classes.
template <class DV>
struct T_AthenaPoolFindBase<ViewVector<DV> >
{
  typedef T_AthenaPoolViewVectorCnv<DV> type;
};


/** @class T_AthenaPoolCnv
 *  @brief This templated class provides the converter to translate an object to/from its persistent POOL representation.
 **/
template <class T>
class T_AthenaPoolCnv
  : public T_AthenaPoolFindBase<T>::type
{
  typedef typename T_AthenaPoolFindBase<T>::type Base;


public:
  using Base::Base;
};


#endif
