// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/MetaContDataBucket.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2018
 * @brief Allow converting MetaCont<T> to T.
 */


#ifndef ATHENAKERNEL_METACONTDATABUCKET_H
#define ATHENAKERNEL_METACONTDATABUCKET_H


#include "AthenaKernel/DataBucket.h"
#include "AthenaKernel/BaseInfo.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/SourceID.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"


namespace SG {


/**
 * @brief Allow converting MetaCont<T> to T.
 *
 * Metadata objects of type T are stored in MetaDataStore as MetaCont<T>.
 * However, to allow using xAOD objects, we'd like to be able to reference
 * a DataLink<T> pointing at this store.  That means that the cast operation
 * of the DataBucket for MetaCont<T> needs to be able to convert to a T
 * as well as MetaCont<T>.  We can arrange this using this custom DataBucket
 * class, which overrides the cast() methods.  The use of this class is enabled
 * by specializing DataBucketTrait in MetaCont.h.
 */
template <class T>
class MetaContDataBucket
  : public DataBucket<T>
{
public:
  // Constructor inherited from base class.
  using DataBucket<T>::DataBucket;


  /**
   * @brief Return the contents of the @c DataBucket,
   *        converted to type given by @a clid.  Note that only
   *        derived->base conversions are allowed here.
   * @param clid The class ID to which to convert.
   * @param irt To be called if we make a new instance.
   * @param isConst True if the object being converted is regarded as const.
   */
  virtual void* cast (CLID clid,
                      IRegisterTransient* irt = 0,
                      bool isConst = true) override;

  
  /**
   * @brief Return the contents of the @c DataBucket,
   *        converted to type given by @a std::type_info.  Note that only
   *        derived->base conversions are allowed here.
   * @param clid The @a std::type_info of the type to which to convert.
   * @param irt To be called if we make a new instance.
   * @param isConst True if the object being converted is regarded as const.
   */
  virtual void* cast (const std::type_info& tinfo,
                      IRegisterTransient* irt = 0,
                      bool isConst = true) override;


    /**
     * @brief Return the contents of the @c DataBucket,
     *        converted to type given by @a clid.  Note that only
     *        derived->base conversions are allowed here.
     * @param clid The class ID to which to convert.
     * @param tinfo The @a std::type_info of the type to which to convert.
     * @param irt To be called if we make a new instance.
     * @param isConst True if the object being converted is regarded as const.
     *
     * This allows the callee to choose whether to use clid or tinfo.
     * Here we use clid.
     */
    virtual void* cast (CLID clid,
                        const std::type_info& tinfo,
                        SG::IRegisterTransient* irt = 0,
                        bool isConst = true) override;


private:
  /**
   * @brief Return the metadata source id for the current event store.
   *        Returns an empty string if there's no source ID set.
   */
  const SourceID getSID() const;
};


} // namespace SG


#include "AthenaKernel/MetaContDataBucket.icc"


#endif // not ATHENAKERNEL_METACONTDATABUCKET_H
