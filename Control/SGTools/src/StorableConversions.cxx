/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file StorableConversions.cxx
 *  @brief convert to and from a SG storable
 * $Id: StorableConversions.cxx,v 1.2 2008-04-08 16:05:32 ssnyder Exp $
 * @author ATLAS Collaboration
 **/

#include "SGTools/StorableConversions.h"


namespace SG {


/**
 * @brief Try to get the pointer back from a @a DataObject,
 *        converted to be of type @a clid.
 * @param pDObj The @a DataObject.
 * @param clid The ID of the class to which to convert.
 * @param irt To be called if we make a new instance.
 * @param isConst True if the object being converted is regarded as const.
 *
 * Only works if the @a DataObject is a @a DataBucket.
 * Returns 0 on failure,
 */
void* fromStorable(DataObject* pDObj, CLID clid,
                   IRegisterTransient*irt/*= 0*/,
                   bool isConst /*= true*/)
{
  // Try to use BaseInfo information to convert pointers.
  DataBucketBase* b = dynamic_cast<DataBucketBase*>(pDObj);
  if (b)
    return b->cast (clid, irt, isConst);
  return 0;
}


/**
 * @brief Try to get the pointer back from a @a DataObject,
 *        converted to be of type @a clid.
 * @param pDObj The @a DataObject.
 * @param clid The ID of the class to which to convert.
 * @param irt To be called if we make a new instance.
 * @param isConst True if the object being converted is regarded as const.
 *
 * Only works if the @a DataObject is a @a DataBucket.
 * Returns 0 on failure,
 */
void* Storable_cast(DataObject* pDObj, CLID clid,
                    IRegisterTransient* irt /*= 0*/,
                    bool isConst /*= true*/)
{
  return fromStorable (pDObj, clid, irt, isConst);
}


} // namespace SG
