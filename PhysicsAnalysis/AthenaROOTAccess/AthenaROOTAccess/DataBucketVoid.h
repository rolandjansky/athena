// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  AthenaROOTAccess/DataBucketVoid.h
 * @author scott snyder
 * @date April 2007
 * @brief Concrete @c DataBucket that holds the object via a @c void*
 *        and uses the Root dictionary to do conversions.
 */


#ifndef DATABUCKETVOID_H
#define DATABUCKETVOID_H

#include "AthenaKernel/DataBucketBase.h"
#include "AthenaKernel/BaseInfo.h"
#include "TMethodCall.h"
#include <typeinfo>
#include <vector>


class TClass;


namespace AthenaROOTAccess {


/**
 * @brief Concrete @c DataBucket that holds the object via a @c void*
 *        and uses the Root dictionary to do conversions.
 *
 * A concrete @c DataBucket instance holds a pointer to an arbitrary
 * object, and is able to further convert it to pointers to other
 * types related by inheritance.  This variant is used for AthenaROOTAccess,
 * where don't have the type available at compile time and thus cannot
 * use templates.  However, we know that we do have the Root dictionary
 * available for the types, so we can use that information for the
 * conversions.
 *
 * This class implements conversions only via @c type_info, not @c CLID.
 */
class DataBucketVoid
  : public DataBucketBase
{
public:
  /**
   * @brief Constructor.
   * @param cl The Root class of the held object.
   * @param ptr The pointer to hold.
   */
  DataBucketVoid (TClass* cl, void* ptr);


  /**
   * @brief Destructor.
   */
  virtual ~DataBucketVoid() override;


  /**
   * @brief Return the held object.
   */
  virtual void* object() override;


  /**
   * @brief Return the @c type_info for the stored object.
   */
  virtual const std::type_info& tinfo() const override;


  using DataBucketBase::cast;

  /**
   * @brief Return the contents of the @c DataBucket,
   *        converted to type given by @a clid.  Note that only
   *        derived->base conversions are allowed here.
   * @param clid The class ID to which to convert.
   * @param irt To be called if we make a new instance.
   * @param isConst True if the object being converted is regarded as const.
   *
   * This is not implemented by this class.
   */
  virtual void* cast (CLID clid,
                      SG::IRegisterTransient* itr = 0,
                      bool isConst =true) override;


  /**
   * @brief Return the contents of the @c DataBucket,
   *        converted to type given by @c std::type_info.  Note that only
   *        derived->base conversions are allowed here.
   * @param tinfo The @c std::type_info of the type to which to convert.
   * @param irt To be called if we make a new instance.
   * @param isConst True if the object being converted is regarded as const.
   */
  virtual void* cast (const std::type_info& tinfo,
                      SG::IRegisterTransient* itr = 0,
                      bool isConst = true) override;

  /**
   * @brief Reset any cached data for a new event.
   */
  void reset();


  /**
   * @brief Give up ownership of the  @c DataBucket contents.
   *        This leaks the contents and it is useful mainly for error handling.
   *
   *        This is a no-op for @c DataBucketVoid (which doesn't own
   *        its target).
   */
  virtual void relinquish() override;


  /**
   * If the held object derives from @c ILockable, call @c lock() on it.
   *
   * This is a no-op for ARA.
   */
  virtual void lock() override;


private:
  /// The class of the held object.
  TClass* m_cl;

  /// The held object.
  void* m_ptr;

  /// For calling baseOffset on the vector.
  mutable TMethodCall m_baseOffsetMeth;

  /// Class of the held object, down-converted to a DataVector.
  TClass* m_dvcl;

  /// Pointer to the held object, converted to type @c m_dvcl.
  void* m_dvptr;

  /// For calling size() on @c m_dvptr.
  TMethodCall m_methsize;

  /// For calling at() on @c m_dvptr.
  TMethodCall m_methat;

  /// Hold information about one DV copying conversion.
  struct dvconv_t {
    /// Destination type for the conversion.
    TClass* m_cldest;

    /// Source element type.
    TClass* m_eltsource;

    /// Function to cast an element pointer.
    SG::BaseInfoBase::castfn_t* m_castfn;

    /// Destination element type.
    TClass* m_eltdest;

    /// size() method on the destination.
    TMethodCall m_methsize;

    /// clear(SG::OwnershipPolicy) method on the destination.
    TMethodCall m_methclear;

    /// reserve method on the destination.
    TMethodCall m_methreserve;

    /// push_back method on the destination.
    TMethodCall m_methpush_back;

    /// The converted DV object.
    void* m_obj;
  };

  /// Table of DV copying conversions.
  std::vector<dvconv_t> m_dvconv;

  /// Table of type/offset pairs for casting.
  typedef std::vector<std::pair<const std::type_info*, int> > offset_table_t;
  mutable offset_table_t m_offsets;


  /**
   * @brief Try to do a copying DV conversion.
   * @param cldest Desired destination class.
   * @return Pointer to converted object, or null.
   */
  void* tryCopyingConversion (TClass* cldest);


  /**
   * @brief Perform a copying DV conversion.
   * @param ent The @c dvconv_t entry describing the conversion.
   * @return Pointer to converted object, or null.
   */
  void* doCopyingConversion (dvconv_t& ent);


  /**
   * @brief Refill the destination container from the source container.
   * @param ent Table entry for the destination container.
   * @param sz_source Size of the source container.
   */
  void refillDest (dvconv_t& ent, size_t sz_source);


  /**
   * @brief Return any DV base class of @a cl.
   * @param cl The class to test.
   * @return @a cl or a base class of it, that derives from DV.
   *
   * If @a cl is a @c DataVector instantiation, return it.
   * Otherwise, walk the base classes.  If we find a base class
   * that's a @c DataVector instantiation, return it.
   */
  static TClass* findDVBase (TClass* cl);
};


} // namespace AthenaROOTAccess



#endif // not DATABUCKETVOID_H
