// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Convert aux container version by generic copy.
 */


#ifndef ATHENAPOOLCNVSVC_T_AUXCONTAINERCOPYTPCNV_H
#define ATHENAPOOLCNVSVC_T_AUXCONTAINERCOPYTPCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthContainers/tools/copyAuxStoreThinned.h"
#include "AthenaKernel/ClassName.h"
#include "GaudiKernel/MsgStream.h"
#include <stdexcept>


/**
 * @brief Convert aux container version by generic copy.
 *
 * When we read old data files, we may need to convert an older aux container
 * version to the current version.  In many cases, this can be done by simply
 * doing a generic copy of the aux container, using SG::copyAuxStoreThinned.
 * This templated class implements that.
 */
template <class NEWTYPE, class OLDTYPE>
class T_AuxContainerCopyTPCnv
  : public T_AthenaPoolTPCnvConstBase< NEWTYPE, OLDTYPE >
{
public:
  using T_AthenaPoolTPCnvConstBase< NEWTYPE, OLDTYPE >::transToPers;
  using T_AthenaPoolTPCnvConstBase< NEWTYPE, OLDTYPE >::persToTrans;


  /// Function converting from the old type to the current one
  virtual void persToTrans( const OLDTYPE* oldObj,
                            NEWTYPE* newObj,
                            MsgStream& log ) const override;
  /// Dummy function inherited from the base class
  virtual void transToPers( const NEWTYPE*,
                            OLDTYPE*,
                            MsgStream& log ) const override;
};


#include "AthenaPoolCnvSvc/T_AuxContainerCopyTPCnv.icc"


#endif // not ATHENAPOOLCNVSVC_T_AUXCONTAINERCOPYTPCNV_H
