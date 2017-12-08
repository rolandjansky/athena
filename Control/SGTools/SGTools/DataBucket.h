///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_DATABUCKET_H
#define SGTOOLS_DATABUCKET_H

#include "SGTools/DataBucketBase.h"
#include "AthenaKernel/DataObjectSharedPtr.h"

//FIXME CLID is a tdef and can't be forward declared 
#include "GaudiKernel/ClassID.h"
#include <memory>
#include <type_traits>

/** @class DataBucket
 *  @brief a wrapper inheriting from DataObject (via DataBucketBase)
 *  that takes ownership of a data object to be put in the DataStore
 *  @author ATLAS Collaboration
 *  $Id: DataBucket.h,v 1.10 2008-06-25 22:45:49 calaf Exp $
 **/

namespace SG { 
  class IRegisterTransient;
  class CopyConversionBase;

  template <class T> 
  class DataBucket : public DataBucketBase {
  
  public:
    
    // CONSTRUCTORS:
  
    DataBucket(): m_ptr(0) {}  //needed by the generic converters
    DataBucket(T* data);
    template <class U>
    DataBucket(std::unique_ptr<U> data);
    DataBucket(SG::DataObjectSharedPtr<T> data);
  
    // DESTRUCTOR:
  
    virtual ~DataBucket();
  
    // DATAOBJECT METHODS
    virtual const CLID& clID() const override;
    static const CLID& classID();

    // return the pointer as a void*
    virtual void* object() override
    {
      typedef typename std::remove_const<T>::type T_nc;
      return const_cast<T_nc*>(m_ptr);
    }

    /**
     * @brief Return the @c type_info for the stored object.
     */
    virtual const std::type_info& tinfo() const override
    {
      return typeid(T);
    }

    // Serialize the object for reading
    //  StreamBuffer& serialize(StreamBuffer& s);
    // Serialize the object for writing
    //  StreamBuffer& serialize(StreamBuffer& s)  const;
  
  

    // AUTOMATIC CONVERSION OF TYPES
    operator T*()                 { return ptr(); } //FIXME can we do without?
    operator const T*() const     { return cptr(); }

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
     * @brief Return a new @c DataBucket whose payload has been cloned from the
     *        original one.
     */
    virtual DataBucket* clone() const override;

    /**
     * @brief Give up ownership of the  @c DataBucket contents.
     *        This leaks the contents and it is useful mainly for error handling.
     */
    virtual void relinquish() override{ m_ptr=0;} //LEAKS m_ptr

    /**
     * If the held object derives from @c ILockable, call @c lock() on it.
     */
    virtual void lock() override;


  protected:
    T* ptr() { return m_ptr; }
    const T* cptr() const { return m_ptr; }

  private:
    T* m_ptr;  //we own the thing now!

    /// Objects made by copy conversion.
    typedef std::pair<const CopyConversionBase*, void*> ent_t;
    typedef std::vector<ent_t> vec_t;
    vec_t m_cnvcopies;

    DataBucket (const DataBucket&);
    DataBucket& operator= (const DataBucket&);
  };
} //end namespace SG  
#include "SGTools/DataBucket.icc"

#endif // SGTOOLS_DATABUCKET_H
