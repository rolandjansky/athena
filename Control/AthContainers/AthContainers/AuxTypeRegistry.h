// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthContainers/AuxTypeRegistry.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2013
 * @brief Handle mappings between names and auxid_t.
 */


#ifndef ATHCONTAINERS_AUXTYPEREGISTRY_H
#define ATHCONTAINERS_AUXTYPEREGISTRY_H


#include "AthContainersInterfaces/AuxTypes.h"
#include "AthContainersInterfaces/IAuxTypeVector.h"
#include "AthContainersInterfaces/IAuxTypeVectorFactory.h"
#include "AthContainers/tools/AuxTypeVector.h"
#include "AthContainers/tools/AuxTypeVectorFactory.h"
#include "AthContainers/tools/threading.h"
#include "CxxUtils/ConcurrentStrMap.h"
#include "CxxUtils/SimpleUpdater.h"
#include "CxxUtils/bitmask.h"
#ifndef XAOD_STANDALONE
#include "AthenaKernel/IInputRename.h"
#include "AthenaKernel/IStringPool.h"
#endif
#include <cstddef>
#include <typeinfo>
#include <vector>
#include <unordered_map>
#include <functional>


namespace SG {


/**
 * @brief Handle mappings between names and auxid_t.
 *
 * Each auxiliary data item associated with a container has a name.
 * Internally, they are identified by small integers of type @c auxid_t.
 * This class handles the mapping between names and @c auxid_t's.
 * It also keeps track of the type of each aux data item, and provides
 * some generic methods for operating on this data.
 *
 * The @c auxid_t namespace is global, shared among all classes.
 * It's no problem for two classes to use aux data with the same
 * @c auxid_t, as long as the type is the same.  If they want to define
 * them as different types, though, that's a problem.  To help with this,
 * an optional class name may be supplied; this qualifies the aux data
 * name to make it unique across classes.
 *
 * This class is meant to be used as a singleton.  Use the @c instance
 * method to get the singleton instance.
 *
 * Methods are locked internally as needed, so access is thread-safe.
 * Thread-safety is however not supported in standalone builds.
 */
class AuxTypeRegistry
{
public:
  /// Additional flags to quality an auxiliary variable.
  enum Flags {
    /// No special flags set.
    None   = 0x00,

    /// Mark that this variable should only be accessed atomically.
    /// If this is set, then the variable can be accessed only via
    /// @c AtomicDecorator.
    /// Be aware that this is not completely safe; it is still possible
    /// to have non-atomic accesses to the variable via either the implicit
    /// operations provided by this class or by explicit access to the
    /// value vector.  Therefore, this is not recommended for general use.
    /// Contact core software before using this for new code.
    Atomic = 0x01,

    /// Enable bitwise functions on this enum; see bitmask.h.
    IS_ATH_BITMASK
  };

  /**
   * @brief Return the singleton registry instance.
   */
  static AuxTypeRegistry& instance();


  /**
   * @brief Look up a name -> @c auxid_t mapping.
   * @param name The name of the aux data item.
   * @param clsname The name of its associated class.  May be blank.
   * @param flags Optional flags qualifying the type.  See above.
   *
   * The type of the item is given by the template parameter @c T.
   * If an item with the same name was previously requested
   * with a different type, then raise @c SG::ExcAuxTypeMismatch.
   */
  template <class T>
  SG::auxid_t getAuxID (const std::string& name,
                        const std::string& clsname = "",
                        const Flags flags = Flags::None);


  /**
   * @brief Look up a name -> @c auxid_t mapping.
   * @param ti Type of the aux data item.
   * @param name The name of the aux data item.
   * @param clsname The name of its associated class.  May be blank.
   * @param flags Optional flags qualifying the type.  See above.
   *
   * The type of the item is given by @a ti.
   * Return @c null_auxid if we don't know how to make vectors of @a ti.
   * (Use @c addFactory to register additional types.)
   * If an item with the same name was previously requested
   * with a different type, then raise @c SG::ExcAuxTypeMismatch.
   */
  SG::auxid_t getAuxID (const std::type_info& ti,
                        const std::string& name,
                        const std::string& clsname = "",
                        const Flags flags = Flags::None);


  /**
   * @brief Look up a name -> @c auxid_t mapping.
   * @param name The name of the aux data item.
   * @param clsname The name of its associated class.  May be blank.
   *
   * Will only find an existing @c auxid_t; unlike @c getAuxID,
   * this won't make a new one.  If the item isn't found, this
   * returns @c null_auxid.
   */
  SG::auxid_t findAuxID( const std::string& name,
                         const std::string& clsname = "") const;

  
  /**
   * @brief Construct a new vector to hold an aux item.
   * @param auxid The desired aux data item.
   * @param size Initial size of the new vector.
   * @param capacity Initial capacity of the new vector.
   *
   * Returns a newly-allocated object.
   */
  std::unique_ptr<IAuxTypeVector> makeVector (SG::auxid_t auxid,
                                              size_t size,
                                              size_t capacity) const;


  /**
   * @brief Construct an @c IAuxTypeVector object from a vector.
   * @param data The vector object.
   * @param isPacked If true, @c data is a @c PackedContainer.
   * @param ownFlag If true, the newly-created IAuxTypeVector object
   *                will take ownership of @c data.
   *
   * If the element type is T, then @c data should be a pointer
   * to a std::vector<T> object, which was obtained with @c new.
   * But if @c isPacked is @c true, then @c data
   * should instead point at an object of type @c SG::PackedContainer<T>.
   *
   * Returns a newly-allocated object.
   */
  std::unique_ptr<IAuxTypeVector> makeVectorFromData (SG::auxid_t auxid,
                                                      void* data,
                                                      bool isPacked,
                                                      bool ownFlag) const;


  /**
   * @brief Return the name of an aux data item.
   * @param auxid The desired aux data item.
   */
  std::string getName (SG::auxid_t auxid) const;


  /**
   * @brief Return the class name associated with an aux data item
   *        (may be blank). 
   * @param auxid The desired aux data item.
   */
  std::string getClassName (SG::auxid_t auxid) const;


  /**
   * @brief Return the type of an aux data item.
   * @param auxid The desired aux data item.
   */
  const std::type_info* getType (SG::auxid_t auxid) const;


  /**
   * @brief Return the type name of an aux data item.
   * @param auxid The desired aux data item.
   *
   * Returns an empty string if the type is not known.
   */
  std::string getTypeName (SG::auxid_t auxid) const;


  /**
   * @brief Return the type of the STL vector used to hold an aux data item.
   * @param auxid The desired aux data item.
   */
  const std::type_info* getVecType (SG::auxid_t auxid) const;


  /**
   * @brief Return the type of the STL vector used to hold an aux data item.
   * @param auxid The desired aux data item.
   *
   * Returns an empty string if the type is not known.
   */
  std::string getVecTypeName (SG::auxid_t auxid) const;


  /**
   * @brief Return size of an element in the STL vector.
   * @param auxid The desired aux data item.
   */
  size_t getEltSize (SG::auxid_t auxid) const;


  /**
   * @brief Return flags associated with an auxiliary variable.
   * @param auxid The desired aux data item.
   */
  Flags getFlags (SG::auxid_t auxid) const;


  /**
   * @brief Copy an element between vectors.
   * @param auxid The aux data item being operated on.
   * @param dst Pointer to the start of the destination vector's data.
   * @param dst_index Index of destination element in the vector.
   * @param src Pointer to the start of the source vector's data.
   * @param src_index Index of source element in the vector.
   *
   * @c dst and @ src can be either the same or different.
   */
  void copy (SG::auxid_t auxid,
             void* dst,       size_t dst_index,
             const void* src, size_t src_index);


  /**
   * @brief Copy an element between vectors.
   *        Apply any transformations needed for output.
   * @param auxid The aux data item being operated on.
   * @param dst Pointer to the start of the destination vector's data.
   * @param dst_index Index of destination element in the vector.
   * @param src Pointer to the start of the source vector's data.
   * @param src_index Index of source element in the vector.
   *
   * @c dst and @ src can be either the same or different.
   */
  void copyForOutput (SG::auxid_t auxid,
                      void* dst,       size_t dst_index,
                      const void* src, size_t src_index);


  /**
   * @brief Swap an element between vectors.
   * @param auxid The aux data item being operated on.
   * @param a Pointer to the start of the first vector's data.
   * @param aindex Index of the element in the first vector.
   * @param b Pointer to the start of the second vector's data.
   * @param bindex Index of the element in the second vector.
   *
   * @c a and @ b can be either the same or different.
   */
  void swap (SG::auxid_t auxid,
             void* a, size_t aindex,
             void* b, size_t bindex);


  /**
   * @brief Clear an element within a vector.
   * @param auxid The aux data item being operated on.
   * @param dst Pointer to the start of the vector's data.
   * @param dst_index Index of the element in the vector.
   */
  void clear (SG::auxid_t auxid, void* dst, size_t dst_index);


  /**
   * @brief Return the vector factory for a given vector element type.
   * @param ti The type of the vector element.
   *
   * Returns nullptr if the type is not known.
   * (Use @c addFactory to add new mappings.)
   */
  const IAuxTypeVectorFactory* getFactory (const std::type_info& ti) const;


  /**
   * @brief Add a new type -> factory mapping.
   * @param ti Type of the vector element.
   * @param factory The factory instance.  The registry will take ownership.
   *
   * This records that @c factory can be used to construct vectors with
   * an element type of @c ti.  If a mapping already exists, the new
   * factory is discarded, unless the old one is a dynamic factory and
   * the new one isn't, in which case the new replaces the old one.
   */
  void addFactory (const std::type_info& ti, IAuxTypeVectorFactory* factory);


#ifndef XAOD_STANDALONE
  /**
   * @brief Declare input renaming requests.
   * @param map Map of (hashed) sgkey -> sgkey for renaming requests.
   * @param pool String pool in which the hashed keys are defined.
   *
   * This is called by @c AddressRemappingSvc when there is a request
   * to rename input objects.  It saves any requests involving renaming
   * of auxiliary variables and makes that information available via
   * @c inputRename.
   */
  void setInputRenameMap (const Athena::IInputRename::InputRenameMap_t* map,
                          const IStringPool& pool);
#endif


  /**
   * @brief Check for an input renaming of an auxiliary variable.
   * @brief key The SG key of the object to which the variable is attached.
   * @brief name The name of the variable on the input file.
   *
   * @returns The variable name to use in the transient representation.
   * Will usually be @c name, but may be different if there was a renaming request.
   */
  const std::string& inputRename (const std::string& key,
                                  const std::string& name) const;


private:
  typedef AthContainers_detail::mutex mutex_t;
  typedef AthContainers_detail::lock_guard<mutex_t> lock_t;

  /**
   * @brief Constructor.
   *
   * Populates the type -> factory mappings for standard C++ types.
   */
  AuxTypeRegistry();


  /**
   * @brief Destructor.
   *
   * Delete factory instances.
   */
  ~AuxTypeRegistry();


  /// Disallow copy construction and assignment.
  AuxTypeRegistry (const AuxTypeRegistry&);
  AuxTypeRegistry& operator= (const AuxTypeRegistry&);


  /**
   * @brief Look up a name -> @c auxid_t mapping.
   * @param name The name of the aux data item.
   * @param clsname The name of its associated class.  May be blank.
   * @param flags Optional flags qualifying the type.  See above.
   * @param ti The type of this aux data item.
   * @param makeFactory Function to create a factory for this type, if needed.
   *                    May return 0 if the type is unknown.
   *
   *
   * If the aux data item already exists, check to see if the provided
   * type matches the type that was used before.  If so, then set
   * return the auxid; otherwise, raise @c SG::ExcAuxTypeMismatch.
   *
   * If the aux data item does not already exist, then see if we
   * have a factory registered for this @c type_info.  If not, then
   * call @c makeFactory and use what it returns.  If that returns 0,
   * then fail and return null_auxid.  Otherwise, assign a new auxid
   * and return it.
   */
  SG::auxid_t
  findAuxID (const std::string& name,
             const std::string& clsname,
             const Flags flags,
             const std::type_info& ti,
             IAuxTypeVectorFactory* (AuxTypeRegistry::*makeFactory) () const);


  /**
   * @brief Return the vector factory for a given auxid.
   * @param auxid The desired aux data item.
   *
   * Returns nullptr if the type is not known.
   * (Use @c addFactory to add new mappings.)
   */
  const IAuxTypeVectorFactory* getFactory (SG::auxid_t auxid) const;


  /**
   * @brief Add a new type -> factory mapping.  (external locking)
   * @param lock The registry lock.
   * @param ti Type of the vector element.
   * @param factory The factory instance.  The registry will take ownership.
   *
   * This records that @c factory can be used to construct vectors with
   * an element type of @c ti.  If a mapping already exists, the new
   * factory is discarded, unless the old one is a dynamic factory and
   * the new one isn't, in which case the new replaces the old one.
   */
  void addFactory (lock_t& lock,
                   const std::type_info& ti,
                   IAuxTypeVectorFactory* factory);


  /**
   * @brief Create an @c AuxTypeVectorFactory instance.
   *
   * This is passed to @c findAuxID when we're looking up an item
   * for which we know the type at compile-time.
   */
  template <class T>
  IAuxTypeVectorFactory* makeFactory() const;


  /**
   * @brief @c makeFactory implementation that always returns 0.
   *
   * This is passed to @c findAuxID when we're looking up an item
   * for which we do not know the type at compile-time.
   */
  IAuxTypeVectorFactory* makeFactoryNull() const;


  /**
   * @brief Return the key used to look up an entry in m_auxids.
   * @param name The name of the aux data item.
   * @param clsname The name of its associated class.  May be blank.
   */
  static std::string makeKey (const std::string& name,
                              const std::string& clsname);


  /// Hold information about one aux data item.
  struct typeinfo_t
  {
    /// Factory object.
    AthContainers_detail::atomic<const IAuxTypeVectorFactory*> m_factory;

    /// Type of the aux data item.
    const std::type_info* m_ti;

    /// Aux data name.
    std::string m_name;

    /// Class name associated with this aux data item.  May be blank.
    std::string m_clsname;

    /// Additional type flags.
    Flags m_flags;
  };


  /// Table of aux data items, indexed by @c auxid.
  // A concurrent vector, so we don't need to take a lock to read it.
  AthContainers_detail::concurrent_vector<typeinfo_t> m_types;


  /// Map from name -> auxid.
  using id_map_t = CxxUtils::ConcurrentStrMap<SG::auxid_t, CxxUtils::SimpleUpdater>;
  id_map_t m_auxids;

  /// Map from type_info name -> IAuxTypeVectorFactory.
  using ti_map_t = CxxUtils::ConcurrentStrMap<const IAuxTypeVectorFactory*, CxxUtils::SimpleUpdater>;
  ti_map_t m_factories;

  /// Hold additional factory instances we need to delete.
  std::vector<const IAuxTypeVectorFactory*> m_oldFactories;

  /// Save the information provided by @c setInputRenameMap.
  /// Each entry is of the form   KEY.DECOR -> DECOR_RENAMED
  typedef std::unordered_map<std::string, std::string> renameMap_t;
  renameMap_t m_renameMap;

  /// Mutex controlling access to the registry.
  // We originally used an upgrading mutex here.
  // But that's relatively slow, and most of the locked sections are short,
  // so it's not really a win.
  // This guards write access to all members, and read access to all members
  // except for m_types.
  mutable mutex_t m_mutex;
};


} // namespace SG


#include "AthContainers/AuxTypeRegistry.icc"


#endif // not ATHCONTAINERS_AUXTYPEREGISTRY_H
