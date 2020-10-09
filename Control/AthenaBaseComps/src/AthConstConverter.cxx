/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaBaseComps/AthConstConverter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2020
 * @brief Gaudi converter base class with const interfaces.
 */


#include "AthenaBaseComps/AthConstConverter.h"


/**
 * @brief Create the transient representation of an object.
 *
 * The default implementation is a no-op.
 */
StatusCode AthConstConverter::createObjConst( IOpaqueAddress* /*pAddress*/,
                                              DataObject*& /*refpObject*/ ) const
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Convert the transient object to the requested representation.
 *
 * The default implementation is a no-op.
 */
StatusCode AthConstConverter::createRepConst( DataObject* /*pObject*/,
                                              IOpaqueAddress*& /*refpAddress*/ ) const
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Create the transient representation of an object.
 *
 * Non-const version; just calls the const version.
 */
StatusCode AthConstConverter::createObj( IOpaqueAddress* pAddress,
                                         DataObject*& refpObject )
{
  return createObjConst (pAddress, refpObject);
}


/**
 * @brief Convert the transient object to the requested representation.
 *
 * Non-const version; just calls the const version.
 */
StatusCode AthConstConverter::createRep( DataObject* pObject,
                                         IOpaqueAddress*& refpAddress )
{
  return createRepConst (pObject, refpAddress);
}

