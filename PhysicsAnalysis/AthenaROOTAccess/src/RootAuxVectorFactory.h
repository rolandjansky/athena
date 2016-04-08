// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file RootAuxVectorFactory.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Dynamic implementation of @c IAuxVectorFactory,
 *        relying on root's vector proxy.
 *
 * This is basically the same as the corresponding class in RootStorageSvc.
 * Duplicated here due to the lack of any suitable common packages
 * with the correct dependencies.
 */


#ifndef ATHENAROOTACCESS_ROOTAUXVECTORFACTORY_H
#define ATHENAROOTACCESS_ROOTAUXVECTORFACTORY_H


#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"
#include "AthContainersInterfaces/IAuxTypeVector.h"
#include "RootUtils/Type.h"


class TClass;
class TVirtualCollectionProxy;


namespace AthenaROOTAccess {


class RootAuxVectorFactory;


/**
 * @brief Dynamic implementation of @c IAuxTypeVector,
 *        relying on root vector proxy.
 *
 * This is used for the case when we need to manipulate an aux data vector
 * present in an input data file but we have neither a proper template
 * instantiation for the factory (because the variable was never explicitly
 * referenced), nor can we find a dictionary entry for the factory.j
 *
 * This implementation works by relying entirely on the root
 * dictionary information.
 */
class RootAuxVector
  : public SG::IAuxTypeVector
{
public:
  /**
   * @brief Constructor.  Makes a new vector.
   * @param factory The factory object for this type.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   */
  RootAuxVector (const RootAuxVectorFactory* factory,
                 size_t size,
                 size_t capacity);


  /**
   * @brief Copy constructor.
   * @param other The vector to copy.
   */
  RootAuxVector (const RootAuxVector& other);


  // Disallow assignment.
  RootAuxVector& operator= (const RootAuxVector&) = delete;


  /**
   * @brief Destructor.
   *
   * This will free the vector data.
   */
  virtual ~RootAuxVector() override;


  /**
   * @brief Make a copy of this vector.
   */
  virtual SG::IAuxTypeVector* clone() const override;


  /**
   * @brief Return a pointer to the start of the vector's data.
   */
  virtual void* toPtr() override;


  /**
   * @brief Return a pointer to the overall object.
   */
  virtual void* toVector() override;



  /**
   * @brief Return the size of the vector.
   */
  virtual size_t size() const override;


  /**
   * @brief Change the size of the vector.
   * @param sz The new vector size.
   */
  virtual void resize (size_t sz) override;


  /**
   * @brief Change the capacity of the vector.
   * @param sz The new vector capacity.
   */
  virtual void reserve (size_t sz) override;


  /**
   * @brief Shift the elements of the vector.
   * @param pos The starting index for the shift.
   * @param offs The (signed) amount of the shift.
   *
   * This operation shifts the elements in the vectors for all
   * aux data items, to implement an insertion or deletion.
   * @c offs may be either positive or negative.
   *
   * If @c offs is positive, then the container is growing.
   * The container size should be increased by @c offs,
   * the element at @c pos moved to @c pos + @c offs,
   * and similarly for following elements.
   * The elements between @c pos and @c pos + @c offs should
   * be default-initialized.
   *
   * If @c offs is negative, then the container is shrinking.
   * The element at @c pos should be moved to @c pos + @c offs,
   * and similarly for following elements.
   * The container should then be shrunk by @c -offs elements
   * (running destructors as appropriate).
   */
  virtual void shift (size_t pos, ptrdiff_t offs) override;


  /**
   * @brief Return the type of the complete object to be saved.
   *
   * For example, if the object is a @c std::vector, then we return
   * the @c type_info of the vector.  But if we're holding
   * a @c PackedContainer, then we return the @c type_info of the
   * @c PackedContainer.
   *
   * Can return null if the operation is not supported.  In that case,
   * I/O will use the type found from the variable registry.
   */
  virtual const std::type_info* objType() const override;


private:
  /// Pointer back to the factory class for this type.
  const RootAuxVectorFactory* m_factory;

  /// The collection proxy for the vector.
  TVirtualCollectionProxy* m_proxy;

  /// Pointer to the overall object itself.
  void* m_obj;

  /// Pointer to the vector object itself.
  void* m_vec;
};


/**
 * @brief Dynamic implementation of @c IAuxVectorFactory,
 *        relying on root's vector proxy.
 *
 * This is used for the case when we need to manipulate an aux data vector
 * present in an input data file but we have neither a proper template
 * instantiation for the factory (because the variable was never explicitly
 * referenced), nor can we find a dictionary entry for the factory.j
 *
 * This implementation works by relying entirely on the root
 * dictionary information.
 *
 * We may either be dealing directly with an STL vector class, or with
 * embedded in another class (as for PackedContainer).  Here, @a vecClass
 * is the class of the STL vector and @a objClass is the overall object
 * class.  In the case of a direct STL vector, these are identical.
 */
class RootAuxVectorFactory
  : public SG::IAuxTypeVectorFactory
{
public:
  /**
   * @brief Constructor.
   * @param vecClass The @c TClass for the vector object.
   */
  RootAuxVectorFactory (TClass* objClass);


  /**
   * @brief Destructor.
   */
  virtual ~RootAuxVectorFactory() override;


  /**
   * @brief Return the ROOT type wrapper.
   */
  const RootUtils::Type& rootType() const { return m_type; }


  /**
   * @brief Return the @c TClass for the overall object.
   */
  TClass* objClass() const { return m_objClass; }


  /**
   * @brief Return the @c TClass for the @c std::vector.
   */
  TClass* vecClass() const { return m_vecClass; }


  /**
   * @brief Return the offset of the vector within the object.
   */
  size_t offset() const { return m_offset; }


  /**
   * @brief Create a vector object of this type.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   */
  virtual SG::IAuxTypeVector* create (size_t size, size_t capacity) const
    override;


  /**
   * @brief Copy an element between vectors.
   * @param dst Pointer to the start of the destination vector's data.
   * @param dst_index Index of destination element in the vector.
   * @param src Pointer to the start of the source vector's data.
   * @param src_index Index of source element in the vector.
   *
   * @c dst and @ src can be either the same or different.
   */
  virtual void copy (void* dst,        size_t dst_index,
                     const void* src,  size_t src_index) const override;


  /**
   * @brief Swap an element between vectors.
   * @param a Pointer to the start of the first vector's data.
   * @param aindex Index of the element in the first vector.
   * @param b Pointer to the start of the second vector's data.
   * @param bindex Index of the element in the second vector.
   *
   * @c a and @ b can be either the same or different.
   */
  virtual void swap (void* a, size_t aindex,
                     void* b, size_t bindex) const override;


  /**
   * @brief Clear an element within a vector (static method).
   * @param dst Pointer to the start of the vector's data.
   * @param dst_index Index of the element in the vector.
   */
  virtual void clear (void* dst, size_t dst_index) const override;


  /**
   * @brief Return the size of an element of this vector type.
   */
  virtual size_t getEltSize() const override;


  /**
   * @brief Return the @c type_info of the overall object.
   */
  virtual const std::type_info* tiVec() const override;


  /**
   * @brief True if the vectors created by this factory work by dynamic
   *        emulation (via @c TVirtualCollectionProxy or similar); false
   *        if the std::vector code is used directly.
   */
  virtual bool isDynamic() const override;


private:
  /// The @c TClass for the overall object.
  TClass* m_objClass;

  /// The @c TClass for the std::vector.
  TClass* m_vecClass;

  /// Offset of the STL vector within the overall object.
  size_t m_offset;

  /// Wrapper for the ROOT type of the element.
  RootUtils::Type m_type;
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_ROOTAUXVECTORFACTORY_H
