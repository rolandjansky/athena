// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataBucketVoid.h,v 1.10 2008-06-04 14:52:59 ssnyder Exp $

/**
 * @file  AthenaROOTAccess/DataBucketVoid.h
 * @author scott snyder
 * @date April 2007
 * @brief Concrete @c DataBucket that holds the object via a @c void*
 *        and uses the Root dictionary to do conversions.
 */


#ifndef DATABUCKETVOID_H
#define DATABUCKETVOID_H

#include "SGTools/DataBucketBase.h"
#include "Reflex/Object.h"
#include "Reflex/Member.h"
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
  ~DataBucketVoid();


  /**
   * @brief Return the held object.
   */
  virtual void* object();


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
                      bool isConst =true) const;


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
                      bool isConst = true) const;

  /**
   * @brief Return a new @c DataBucket whose payload has been cloned from the
   *        original one.
   */
  virtual DataBucketVoid* clone() const;


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
  virtual void relinquish();


  /**
   * If the held object derives from @c ILockable, call @lock() on it.
   *
   * This is a no-op for ARA.
   */
  virtual void lock();


private:
  /// The held object.
  ROOT::Reflex::Object m_ptr;

  /// Class of the held object, down-converted to a DataVector.
  ROOT::Reflex::Type m_dvcl;

  /// Pointer to the held object, converted to type @c m_dvcl.
  ROOT::Reflex::Object m_dvptr;

  /// For calling size() on @c m_dvptr.
  ROOT::Reflex::Member m_methsize;

  /// For calling at() on @c m_dvptr.
  ROOT::Reflex::Member m_methat;

  /// Hold information about one DV copying conversion.
  struct dvconv_t {
    /// Destination type for the conversion.
    ROOT::Reflex::Type m_cldest;

    /// Source element type.
    ROOT::Reflex::Type m_eltsource;

    /// Destination element type.
    ROOT::Reflex::Type m_eltdest;

    /// size() method on the destination.
    ROOT::Reflex::Member m_methsize;

    /// clear(SG::OwnershipPolicy) method on the destination.
    ROOT::Reflex::Member m_methclear;

    /// reserve method on the destination.
    ROOT::Reflex::Member m_methreserve;

    /// push_back method on the destination.
    ROOT::Reflex::Member m_methpush_back;

    /// The converted DV object.
    ROOT::Reflex::Object m_obj;
  };

  /// Table of DV copying conversions.
  std::vector<dvconv_t> m_dvconv;


  /**
   * @brief Try to do a copying DV conversion.
   * @param cldest Desired destination class.
   * @return Pointer to converted object, or null.
   */
  void* tryCopyingConversion (const ROOT::Reflex::Type& cldest);


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
  static ROOT::Reflex::Type findDVBase (const ROOT::Reflex::Type& cl);


  /**
   * @brief Helper to look up a member function in @c typ or a base.
   * @param typ The class in which to search.
   * @param name The name of the desired function.
   * @param signature Signature of the desired function.
   *
   * Searches @a typ and its bases for the function member @a name.
   * Warning: ??? The base search order is depth-first, which isn't
   * really right as per C++, but it should suffice for our purposes here.
   */
  static ROOT::Reflex::Member
  getMethod (const ROOT::Reflex::Type& typ,
             const std::string& name,
             const ROOT::Reflex::Type& signature = ROOT::Reflex::Type(0,0));
};


} // namespace AthenaROOTAccess



#endif // not DATABUCKETVOID_H
