// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RootUtils/Type.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Wrapper for ROOT types.
 */


#ifndef ROOTUTILS_TYPE_H
#define ROOTUTILS_TYPE_H


#include "TClass.h"
#include "TDataType.h"
#include "TMethodCall.h"
#include "boost/thread/tss.hpp"
#include <typeinfo>
#include <atomic>
#include <memory>
#include <mutex>


namespace RootUtils {


/**
 * @brief Wrapper for ROOT types.
 *
 * Using the ROOT interfaces to manipulate objects of generic type
 * is rather awkward, because you have do to completely different things
 * depending on whether the type of object you're dealing with is a class
 * or a fundamental type.  This wrapper allows treating these two cases
 * in a uniform way.  For now, we don't try to handle other kinds of types
 * (pointers, etc).
 */
class Type
{
public:
  /**
   * @brief Default constructor.
   *
   * The only thing you can do with the resulting object is to call
   * @c init() on it.
   */
  Type();


  /**
   * @brief Construct for a class type.
   * @param cls The class of the object on which we're operating.
   */
  Type (TClass* cls);


  /**
   * @brief Construct for a fundamental type.
   * @param type The ROOT type code.
   */
  Type (EDataType type);


  /**
   * @brief Construct from a type name.
   * @param typname The name of the type.
   *
   * @c typname may be either the name of a fundamental type,
   * or the name of a class known to ROOT.  An exception will be
   * thrown if the name is not recognized.
   */
  Type (const std::string& typname);



  /**
   * @brief Copy constructor
   * @param other Object to be copied.
   */
  Type (const RootUtils::Type& other);


  /**
   * @brief Assignment.
   * @param other Object to be copied.
   */
  Type& operator= (const RootUtils::Type& other);


  /**
   * @brief Destructor.
   */
  ~Type();


  /**
   * @brief Initialize from a type name.
   * @param typname The name of the type.
   *
   * The @c Type object must have been just default-constructed.
   *
   * @c typname may be either the name of a fundamental type,
   * or the name of a class known to ROOT.  An exception will be
   * thrown if the name is not recognized.
   */
  void init (const std::string& typname);


  /**
   * @brief Initialize for a class type.
   * @param cls The class of the object on which we're operating.
   *
   * The @c Type object must have been just default-constructed.
   */
  void init (TClass* cls);


  /**
   * @brief Initialize for a fundamental data type..
   * @param type The ROOT type code.
   *
   * The @c Type object must have been just default-constructed.
   */
  void init (EDataType type);


  /**
   * @brief Create an instance of the object.
   *
   * Class objects will be default-constructed; fundamental types
   * will be zero-filled.
   */
  void* create() const;


  /**
   * @brief Destroy an instance of the object (and free memory).
   * @param p Pointer to the object to be destroyed and freed.
   */
  void destroy (void* p) const;


  /**
   * @brief Return the name of this type.
   */
  std::string getTypeName() const;


  /**
   * @brief Return the ROOT class for the described type.
   *
   * Returns 0 if this is for a fundamental type.
   */
  TClass* getClass() const;


  /**
   * @brief Return the ROOT data type code for the described type.
   *
   * Returns @c kNoType_t if this is for a class type.
   */
  EDataType getDataType() const;


  /**
   * @brief Return the @c type_info for the described type.
   */
  const std::type_info* getTypeInfo() const;


  /**
   * @brief Return the size in bytes of an instance of the described type.
   */
  size_t getSize() const;


  /**
   * @brief Copy a range of objects.
   * @param dst Pointer to the start of the first object for the destination.
   * @param src Pointer to the start of the first object for the copy source.
   * @param n Numer of objects to copy.
   *
   * This function properly handles overlapping ranges.
   */
  void copyRange (void* dst, const void* src, size_t n) const;


  /**
   * @brief Clear a range of objects.
   * @param dst Pointer to the start of the first object to clear.
   * @param n Number of objects to clear.
   *
   * Objects of class type are cleared by assigning to them
   * a default-constructed object.  Other objects types are cleared
   * by filling with 0.
   */
  void clearRange (void* dst, size_t n) const;


  /**
   * @brief Clear an object in a vector.
   * @param dst Pointer to the start of the vector's data.
   * @param index Index of the object in the vector.
   */
  void clear (void* dst, size_t index) const;


  /**
   * @brief Clear an object.
   * @param dst Pointer to the object.
   */
  void clear (void* dst) const;


  /**
   * @brief Copy an object within vectors.
   * @param dst Pointer to the start of the destination vector's data.
   * @param dst_index Index of destination object in the vector.
   * @param src Pointer to the start of the source vector's data.
   * @param src_index Index of source object in the vector.
   *
   * @c dst and @ src can be either the same or different.
   */
  void assign (void* dst,       size_t dst_index,
               const void* src, size_t src_index) const;


  /**
   * @brief Copy an object.
   * @param dst Destination for the copy.
   * @param src Source for the copy.
   *
   * The copy will be done using either @c m_tsAssign or @c memcpy,
   * depending on whether or not the object has class type.
   * If the payload does not have class type and @c src is null,
   * then the destination element will be filled with 0's.
   */
  void assign (void* dst, const void* src) const;


  /**
   * @brief Swap an object between vectors.
   * @param a Pointer to the start of the first vector's data.
   * @param aindex Index of the object in the first vector.
   * @param b Pointer to the start of the second vector's data.
   * @param bindex Index of the object in the second vector.
   *
   * @c a and @ b can be either the same or different.
   */
  void swap (void* a, size_t a_index,
             void* b, size_t b_index) const;


  /**
   * @brief Swap two objects.
   * @param a Pointer to the first object.
   * @param b Pointer to the second object.
   */
  void swap (void* a, void* b) const;


  /**
   * @brief Initialize an object from a string.
   * @param p Pointer to the object to initialize.
   * @param s String from which to initialize.
   *
   * Only works for basic types and `std::string`.
   */
  void fromString (void* p, const std::string& s) const;


  /**
   * @brief Deleter object, compatible with `std::unique_ptr`.
   */
  class Deleter
  {
  public:
    /**
     * @brief Default constructor.
     */
    Deleter();


    /**
     * @brief Constructor.
     * @param t The type describing this generic pointer.
     */
    Deleter (const Type& t);


    /**
     * @brief Delete an object.
     * @param p Pointer to the object to delete.
     */
    void operator() (void* p);


  private:
    /// The type describing this pointer.
    const Type* m_t;
  };


  /**
   * @brief A `std::unique_ptr` type referencing a generic pointer.
   *
   * You need to pass in the type when constructing it, like this:
   *
   *@code
   *  RootUtils::Type typ (...);
   *  RootUtils::Type::unique_ptr up (typ.create(), typ);
   @endcode
   */
  typedef std::unique_ptr<void, Deleter> unique_ptr;


private:
  /// See if @c m_assign is initialized.  If not, try to initialize it now,
  /// and copy to the thread-specific variable.
  /// Returns true on success.
  bool checkAssign() const;

  /// The class of the derived type, or 0 if it's not a class type.
  TClass* m_cls;

  /// The ROOT type code of the derived type, or @c kNoType_t for a class type.
  EDataType m_type;

  /// The @c type_info for the described type.
  const std::type_info* m_ti;

  /// The size in bytes of an instance of the described type.
  size_t m_size;

  /// TMethodCall "safe deleter"
  struct TMCDeleter
  {
    /// Static function to use with @c boost::thread_specific_ptr
    static void destroy (TMethodCall* ptr);
    /// Operator to use with @c std::unique_ptr
    void operator() (void* ptr);
  };

  /// Object to call the assignment operator on the payload type.
  /// This will be left invalid if the payload is not a class type.
  /// This instance is not actually used for calls --- since we have
  /// to change the object state to make a call (registering the
  /// arguments), this is not thread-safe.  Instead, we use the
  /// thread-specific instances accessed through @c m_tsAssign.
  /// Mutable, to allow calls to GetMethod().
  mutable std::unique_ptr<TMethodCall, TMCDeleter> m_assign;

  /// Flag whether or not m_assign has been initialized.
  /// We don't want to do that before we actually need it,
  /// to prevent needless parses with root6.
  mutable std::atomic<bool> m_assignInitialized;

  /// Control access to m_assign for initialization.
  mutable std::mutex m_assignMutex;

  /// Objects used to call assignment on the payload object.
  /// Left invalid if the payload does not have class type.
  /// Copied from @c m_assign the first time it's used in
  /// a particular thread.
  mutable boost::thread_specific_ptr<TMethodCall> m_tsAssign;

  /// Pointer to a default-constructed instance of the payload object.
  /// Null if the payload does not have class type.
  mutable void* m_defElt;
};


} // namespace RootUtils


#include "RootUtils/Type.icc"


#endif // not ROOTUTILS_TYPE_H
