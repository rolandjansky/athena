// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_STORABLECONVERSIONS_H
# define SGTOOLS_STORABLECONVERSIONS_H
/** @file StorableConversions.h
 *  @brief convert to and from a SG storable
 * $Id: StorableConversions.h,v 1.13 2008-05-22 22:54:12 calaf Exp $
 * @author ATLAS Collaboration
 **/

#include "SGTools/DataBucket.h"
#include "SGTools/DataBucketTraitFwd.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/tools/type_tools.h"
#include "AthenaKernel/DataObjectSharedPtr.h"
#include "GaudiKernel/DataObject.h"

#ifndef NDEBUG
#  include "AthenaKernel/getMessageSvc.h"
#  include "GaudiKernel/MsgStream.h"
#endif

#include "boost/type_traits/is_same.hpp"
#include "boost/mpl/identity.hpp"
#include "boost/mpl/eval_if.hpp"
#include <memory>
#include <type_traits>

//
//<<<<<< FORWARD DECLARATIONS                                           >>>>>>

//<<<<<< FUNCTION DECLARATIONS                                          >>>>>>
namespace SG {

  // put a dobj pointer in a bucket as appropriate
  template <typename T> 
  DataObject* asStorable(T* pObject);

  // get a dobj pointer from a bucket as appropriate
  template <typename T> 
  bool  fromStorable(DataObject* pDObj, T*& pTrans, bool quiet=false,
                     IRegisterTransient* irt = 0,
                     bool isConst = true);

  template <typename T>
  T* Storable_cast(DataObject* pDObj, bool quiet=true,
                   IRegisterTransient* irt = 0,
                   bool isConst = true) {
    T* result;
    return  fromStorable(pDObj, result, quiet, irt, isConst) ? result : 0;
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
  void* fromStorable(DataObject* pDObj, CLID clid,
                     IRegisterTransient* irt = 0,
                     bool isConst = true);


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
                      IRegisterTransient* irt = 0,
                      bool isConst = true);

}

//////////////////////////////////////////////////////////////////
// implementation details


namespace SG {


  /**
   * @brief Metafunction to find the proper @c DataBucket class for @c T.
   *
   * Here's what we're trying to do.
   *
   * The default @c DataBucket class for @c T is @c SG::DataBucket\<T>.
   *
   * However, if @c T is a @c DataVector or @c DataList, then we want
   * to use instead @c SG::DVLDataBucket\<T>.
   *
   * Further, if @c T derives from @c DataVector or @c DataList (as declared
   * by @c SG_BASE), then we also want to use @c SG::DVLDataBucket\<T>.
   *
   * Further, we don't want this code to depend on @c DataVector
   * or @c SG::DVLDataBucket.  That behavior is enabled only
   * by including the appropriate @c DataVector/@c DataList headers.
   *
   * So, we handle this with the following metafunction.
   * (By saying that this is a `metafunction', it means that the result
   * is given by the nested type @c type.)
   * The @c T argument is the type we're testing, and @c U is the top-level
   * type that was given to the original invocation of @c DataBucketTrait.
   * If @c T has no bases (according to @c SG::Bases), we return
   * @c SG::DataBucket\<U>.  Otherwise, we apply ourselves recursively
   * to the first base class.
   *
   * Other packages can then specialize this to change the behavior.
   *
   * We don't really try to handle multiple bases.  In principle,
   * we could loop over them.  But it seems that that's not really needed,
   * so i don't want to pull in the extra overhead unnecessarily.
   *
   * This class also defines a real static function init().
   * This is called from @c fromStorable.
   * It can be used to force instantiation of reflection information.
   */
  template <class T, class U /* = T*/>
  struct DataBucketTrait
  {
    // The first base of @c T (or @c SG::NoBase).
    typedef typename SG::BaseType<typename SG::Bases<T>::Base1>::type base1;

    // Test to see if it's valid.
    typedef typename boost::is_same<base1, SG::NoBase>::type has_base;

    // This is what we'll return in the default (no base) case.
    typedef boost::mpl::identity<SG::DataBucket<U> > deflt;

    // This is what we use to recursively check the base
    typedef DataBucketTrait<base1, U> recurse;

    // Calculate the output.
    // (Note that it's important to use @c eval_if here; otherwise,
    // applying this to @c SG::NoBase won't work.)
    typedef typename boost::mpl::eval_if<has_base, deflt, recurse>::type type;

    /// Initialization hook.  A no-op by default.
    static void init() {}
  };


  // put a dobj pointer in a bucket as appropriate
  template <typename T>
  DataObject* asStorable(T* pObject) {
    typedef typename DataBucketTrait<T>::type bucket_t;
    return new bucket_t (pObject);
  }  

  template <typename T>
  DataObject* asStorable(std::unique_ptr<T> pObject) {
    typedef typename std::remove_const<T>::type T_nc;
    typedef typename DataBucketTrait<T_nc>::type bucket_t;
    return new bucket_t (std::move(pObject));
  }  

  template <typename T>
  DataObject* asStorable(SG::DataObjectSharedPtr<T> pObject) {
    typedef typename DataBucketTrait<T>::type bucket_t;
    return new bucket_t (std::move(pObject));
  }  

  // get a dobj pointer from a bucket as appropriate
  //the DataObject must be a DataBucket<DATA>*.

  template <typename T>
  bool  fromStorable(DataObject* pDObj, T*& pTrans, 
                     bool
#ifndef NDEBUG
                     quiet
#endif
                     , IRegisterTransient* irt,
                     bool isConst /*= true*/)
  {
    typedef typename std::remove_const<T>::type T_nc;
    typedef typename DataBucketTrait<T_nc>::type bucket_t;
    DataBucketTrait<T_nc>::init();

    //check inputs
    if (0 == pDObj) {
      pTrans=0;
#ifndef NDEBUG
      MsgStream gLog(Athena::getMessageSvc(), "SG::fromStorable");
      gLog << MSG::WARNING << "null input pointer " << endmsg;
#endif
      return false;
    }

    // get T* from DataBucket:
    SG::DataBucket<T_nc>* bucketPtr = dynamic_cast<bucket_t*>(pDObj);
    bool success(0 != bucketPtr);
    if (success)
      pTrans = *bucketPtr;
    else {
      // Try to use BaseInfo information to convert pointers.
      DataBucketBase* b = dynamic_cast<DataBucketBase*>(pDObj);
      if (b) {
        pTrans = b->template cast<T_nc> (irt, isConst);
        if (pTrans)
          success = true;
      }
    }

#ifndef NDEBUG
    if (!quiet && !success) {
      MsgStream gLog(Athena::getMessageSvc(), "SG::fromStorable");
      gLog << MSG::WARNING 
		<< "can't convert stored DataObject " << pDObj 
		<< " to type ("
		<< ClassID_traits<T>::typeName() 
		<< ")\n Unless you are following a symlink,"
	        << " it probably means you have a duplicate "
		<< "CLID = "  << pDObj->clID() 
		<< endmsg;
    }
#endif
    return success;
  }
}

#endif // SGTOOLS_STORABLECONVERSIONS_H
