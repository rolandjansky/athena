/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_DATABUCKETBASE_H
#define SGTOOLS_DATABUCKETBASE_H

/** @class DataBucketBase 
 * @brief A non-templated base class for DataBucket, allows to access the
 * transient object address as a void*
 *  @author ATLAS Collaboration
 *  $Id: DataBucketBase.h,v 1.7 2008-05-30 22:58:46 calaf Exp $
 **/


#include "GaudiKernel/DataObject.h"
#include <typeinfo>

namespace SG {
  class IRegisterTransient;
}

class DataBucketBase : public DataObject
{

 public:

  DataBucketBase() { };
  virtual ~DataBucketBase() { };
  virtual void* object() = 0;

  /**
   * @brief Return the @c type_info for the stored object.
   */
  virtual const std::type_info& tinfo() const = 0;

  /**
   * @brief Return the contents of the @c DataBucket,
   *        converted to type @a T.  Note that only
   *        derived->base conversions are allowed here.
   *        @a T must have a valid Class ID for this to work.
   * @param irt To be called if we make a new instance.
   * @param isConst True if the object being converted is regarded as const.
   */
  template <class T> T* cast (SG::IRegisterTransient* irt = 0,
                              bool isConst = true);

  /**
   * @brief Return the contents of the @c DataBucket,
   *        converted to type given by @a clid.  Note that only
   *        derived->base conversions are allowed here.
   * @param clid The class ID to which to convert.
   * @param irt To be called if we make a new instance.
   * @param isConst True if the object being converted is regarded as const.
   */
  virtual void* cast (CLID clid,
                      SG::IRegisterTransient* irt = 0,
                      bool isConst = true) = 0;

  /**
   * @brief Return the contents of the @c DataBucket,
   *        converted to type given by @a std::type_info.  Note that only
   *        derived->base conversions are allowed here.
   * @param clid The @a std::type_info of the type to which to convert.
   * @param irt To be called if we make a new instance.
   * @param isConst True if the object being converted is regarded as const.
   */
  virtual void* cast (const std::type_info& tinfo,
                      SG::IRegisterTransient* irt = 0,
                      bool isConst = true) = 0;

  /**
   * @brief Return a new @c DataBucket whose payload has been cloned from the
   *        original one.
   */
  virtual DataBucketBase* clone() const = 0;

  /**
   * @brief Give up ownership of the  @c DataBucket contents.
   *        This leaks the contents and it is useful mainly for error handling.
   */
  virtual void relinquish() = 0;

  /**
   * If the held object derives from @c ILockable, call @c lock() on it.
   */
  virtual void lock() = 0;
};


#include "SGTools/DataBucketBase.icc"


#endif

