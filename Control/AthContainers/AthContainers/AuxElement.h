// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxElement.h 741471 2016-04-19 20:58:27Z ssnyder $
/**
 * @file AthContainers/AuxElement.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Base class for elements of a container that can have aux data.
 */


#ifndef ATHCONTAINERS_AUXELEMENTBASE_H
#define ATHCONTAINERS_AUXELEMENTBASE_H


#include "AthContainersInterfaces/IAuxElement.h"
#include "AthContainersInterfaces/IConstAuxStore.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthLinks/DataLink.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxVectorData.h"
#include "AthContainers/tools/AuxDataTraits.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/likely.h"
#include <cstddef>



namespace SG {


class AuxElementData;
class AuxElementStandaloneData;
class AuxVectorData_test;
class AuxVectorBase;
class AuxVectorBase_test;


/**
 * @brief Base class for elements of a container that can have aux data.
 *
 * Classes that want to have associated auxiliary data should derive
 * from this class.   (It is also possible to use this class directly,
 * if you want a container that _only_ stores auxiliary data.)
 *
 * The first thing that happens when you derive from @c AuxElement
 * is that when an object is inserted into a @c DataVector, the
 * vector will maintain information in the object telling were
 * it is within the vector.  For example:
 *
 *@code
 *  DataVector<SG::AuxElement> v (2);
 *  v[1] = new SG::AuxElement;
 *  assert (v[1]->index() == 1);
 *  assert (v[1]=>container() == &v);
 @endcode
 *
 * As long as you don't use @c DataVector::stdcont or use unsafe
 * casts, @c DataVector will correctly maintain this information.
 *
 * When an object deriving from @c AuxElement is in a @c DataVector
 * it may have auxiliary data associated with it; that is,
 * named data objects of arbitrary type.  The recommended way
 * of accessing auxiliary data is through the @c Accessor
 * and @c ConstAccessor classes, which cache the lookup between
 * the aux data item name and its internal representation.
 * The difference between these two is that @c ConstAccessor allows
 * only const access to the element, while @c Accessor, which derives
 * from it, allows non-const access as well.  A given name must always
 * have the same type, no matter where it is used (even across
 * different classes); otherwise, an exception will be thrown.
 * To help prevent conflicts between different classes, aux data item
 * names may be optionally qualified with a class name.
 * Here's an example of using @c ConstAccessor:
 *
 *@code
 *   // Only need to do this once.
 *   Myclass::ConstAccessor<int> vint1 ("myInt");
 *   ...
 *   const Myclass* m = ...;
 *   int x = vint1 (*m);
 @endcode
 *
 * The @c auxdata methods can be used as a shortcut for this,
 * but that's not recommended for anything for which performance
 * is an issue.
 *
 * You can also define getters/setters in your class:
 *
 *@code
 *  class Myclass {
 *    ...
 *    int get_x() const
 *    { const static ConstAccessor<int> acc ("x", "Myclass");
 *      return acc (*this); }
 *    int& get_x()
 *    { const static Accessor<int> acc ("x", "Myclass");
 *      return acc (*this); }
 @endcode
 *
 * In addition, one sometimes wants to add additional auxiliary data to
 * an existing const container; for example, after a container has been
 * retrieved from StoreGate.  This is called `decoration', and is handled
 * by the @c Decorator object, which is much like @c Accessor and
 * @c ConstAccessor.  The difference
 * is that @c Decorator can take a const container and return a non-const,
 * modifiable reference.  If the container has been locked by calling
 * @c StoreGateSvc::setConst, then this is allowed only if this is a new
 * auxiliary item, in which case it is marked as a decoration, or if it
 * is already marked as a decoration.  This prevents changing existing
 * variables in a locked container.  An @c auxdecor method is also
 * available, analogous to @c auxdata.
 *
 * In addition to the above, the class @c TypelessConstAccessor is a
 * non-templated class that allows access to auxiliary data items
 * directly as a `void *`.  This is useful for code which operates
 * on auxiliary data generically; it shouldn't really be used
 * in other contexts.
 *
 * Normally, an object can have auxiliary data only when it is part
 * of a container.  But sometimes it is useful to be able to associate
 * aux data with an object before it has been added to a container.
 * You can enable this by creating a `private store' for the
 * object with @c makePrivateStore.  This can optionally take
 * an argument from which aux data should be copied.
 * (Using a private store adds overhead, which is why it is not enabled
 * by default.)  Example:
 *
 *@code
 *  class Myclass : public SG::AuxElement { ... };
 *  ...
 *  Myclass::Accessor<int> myint ("myint");
 *  const Myclass* m = new Myclass;
 *  m->makePrivateStore();
 *  myint(*m) = 10;
 *  DataVector<Myclass> v;
 *  v.push_back(m);
 *  assert (myint(v[0]) == 10);
 @endcode
 *
 * When an object with a private store is added to a container,
 * the aux data is copied to the container and the private store
 * is released.  However, the fact that we had a private store
 * is remembered; if the object is later removed from the container,
 * the private store will be remade, and the aux data will be copied
 * back from the container to the private store.  To explicitly
 * release the private store (so that it won't come back automatically),
 * call @c releasePrivateStore.
 *
 * If you add @c makePrivateStore calls to the constructors of your class,
 * then you should be able to treat aux data as if it were part
 * of the object itself; the required copying will be handled automatically.
 *
 *@code
 *  class Myclass : public SG::AuxElement
 *  {
 *  public:
 *    Myclass() { makePrivateStore(); }
 *    Myclass(const Myclass* other) { makePrivateStore(other); }
 @endcode
 *
 * The @c SG::AuxElementComplete template class may be helpful
 * in setting this up.
 *
 * It is also possible to associate one of these objects with an external
 * aux data store.  This is the `standalone' mode.  To do this, use
 * the @c setStore methods, exactly as you would for a container
 * that has aux data.  @c setStore will throw an exception if the
 * object is a member of a container or has a private store.
 *
 * This class should not have any virtual methods (to avoid forcing
 * derived classes to have a vtable).
 */
class AuxElement
  : public SG::IAuxElement
{
public:
  /// Default constructor.
  AuxElement();


  /**
   * @brief Copy Constructor.
   * @param other Object being copied.
   *
   * We do not copy the container/index --- the new object is not yet
   * in a container!
   *
   * In the case of constructing an object with a private store,
   * @c makePrivateStore will take care of copying the aux data.
   */
  AuxElement (const AuxElement& other);


  /**
   * @brief Assignment.
   * @param other The object from which we're assigning.
   *
   * We don't copy container/index, as assignment doesn't change where
   * this object is.  However, if we have aux data, then we copy aux data
   * if we're copying from an object that also has it; otherwise,
   * if we're copying from an object with no aux data, then we clear ours.
   */
  AuxElement& operator= (const AuxElement& other);


  /**
   * @brief Destructor.
   *
   * Any private store is deleted.
   */
  ~AuxElement();


  /**
   * @brief Return the container holding this element.
   */
  SG::AuxVectorData* container();


  /**
   * @brief Return the container holding this element.
   */
  const SG::AuxVectorData* container() const;


  /**
   * @brief Return the index of this element within its container.
   */
  size_t index() const;


  /**
   * @brief Helper class to provide const generic access to aux data.
   *
   * This is written as a separate class in order to be able
   * to cache the name -> auxid lookup.
   *
   * This should generally only be used by code which treats
   * auxiliary data generically (that is, where the type is not
   * known at compile-time).  Most of the time, you'd want to use
   * the type-safe versions @c ConstAccessor and @c Accessor.
   */
  class TypelessConstAccessor
  {
  public:
    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     *
     * The name -> auxid lookup is done here.
     */
    TypelessConstAccessor (const std::string& name);


    /**
     * @brief Constructor.
     * @param ti The type for this aux data item.
     * @param name Name of this aux variable.
     *
     * The name -> auxid lookup is done here.
     */
    TypelessConstAccessor (const std::type_info& ti,
                           const std::string& name);


    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     * @param clsname The name of its associated class.  May be blank.
     *
     * The name -> auxid lookup is done here.
     */
    TypelessConstAccessor (const std::string& name,
                           const std::string& clsname);


    /**
     * @brief Constructor.
     * @param ti The type for this aux data item.
     * @param name Name of this aux variable.
     * @param clsname The name of its associated class.  May be blank.
     *
     * The name -> auxid lookup is done here.
     */
    TypelessConstAccessor (const std::type_info& ti,
                           const std::string& name,
                           const std::string& clsname);


    /**
     * @brief Fetch the variable for one element, as a const pointer.
     * @param e The element for which to fetch the variable.
     */
    const void* operator() (const AuxElement& e) const;
    

    /**
     * @brief Fetch the variable for one element, as a const pointer.
     * @param container The container from which to fetch the variable.
     * @param index The index of the desired element.
     *
     * This allows retrieving aux data by container / index.
     * Looping over the index via this method will be faster then
     * looping over the elements of the container.
     */
    const void* operator() (const AuxVectorData& container, size_t index) const;

    
    /**
     * @brief Get a pointer to the start of the auxiliary data array.
     * @param container The container from which to fetch the variable.
     */
    const void* getDataArray (const AuxVectorData& container) const;
    

    /**
     * @brief Test to see if this variable exists in the store.
     * @param e An element of the container which to test the variable.
     */
    bool isAvailable (const AuxElement& e) const;


    /**
     * @brief Return the aux id for this variable.
     */
    SG::auxid_t auxid() const;


  protected:
    /// The cached @c auxid.
    SG::auxid_t m_auxid;

    /// Cached element size.
    size_t m_eltSize;
  };


  /**
   * @brief Helper class to provide constant type-safe access to aux data.
   *
   * This is written as a separate class in order to be able
   * to cache the name -> auxid lookup.
   *
   * You might use this something like this:
   *
   *@code
   *   // Only need to do this once.
   *   Myclass::ConstAccessor<int> vint1 ("myInt");
   *   ...
   *   const Myclass* m = ...;
   *   int x = vint1 (*m);
   @endcode
   *
   * This class can be used only for reading data.
   * To modify data, see the class @c Accessor.
   */
  template <class T>
  class ConstAccessor
  {
  public:
    /// Type referencing an item.
    typedef typename AuxDataTraits<T>::const_reference_type
      const_reference_type;

    /// Pointer into the container holding this item.
    typedef typename AuxDataTraits<T>::const_container_pointer_type
      const_container_pointer_type;

    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     *
     * The name -> auxid lookup is done here.
     */
    ConstAccessor (const std::string& name);


    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     * @param clsname The name of its associated class.  May be blank.
     *
     * The name -> auxid lookup is done here.
     */
    ConstAccessor (const std::string& name, const std::string& clsname);


    /**
     * @brief Fetch the variable for one element, as a const reference.
     * @param e The element for which to fetch the variable.
     */
    const_reference_type operator() (const AuxElement& e) const;
    

    /**
     * @brief Fetch the variable for one element, as a const reference.
     * @param container The container from which to fetch the variable.
     * @param index The index of the desired element.
     *
     * This allows retrieving aux data by container / index.
     * Looping over the index via this method will be faster then
     * looping over the elements of the container.
     */
    const_reference_type
    operator() (const AuxVectorData& container, size_t index) const;


    /**
     * @brief Get a pointer to the start of the auxiliary data array.
     * @param container The container from which to fetch the variable.
     */
    const_container_pointer_type
    getDataArray (const AuxVectorData& container) const;
    

    /**
     * @brief Test to see if this variable exists in the store.
     * @param e An element of the container which to test the variable.
     */
    bool isAvailable (const AuxElement& e) const;


    /**
     * @brief Return the aux id for this variable.
     */
    SG::auxid_t auxid() const;


  protected:
    /// The cached @c auxid.
    SG::auxid_t m_auxid;
  };


  /**
   * @brief Helper class to provide type-safe access to aux data.
   *
   * This is written as a separate class in order to be able
   * to cache the name -> auxid lookup.
   *
   * You might use this something like this:
   *
   *@code
   *   // Only need to do this once.
   *   Myclass::Accessor<int> vint1 ("myInt");
   *   ...
   *   const Myclass* m = ...;
   *   int x = vint1 (*m);
   @endcode
   *
   * You can also use this to define getters/setters in your class:
   *
   *@code
   *  class Myclass {
   *    ...
   *    int get_x() const
   *    { const static Accessor<int> acc ("x", "Myclass");
   *      return acc (*this); }
   *    int& get_x()
   *    { const static Accessor<int> acc ("x", "Myclass");
   *      return acc (*this); }
   @endcode
   */
  template <class T>
  class Accessor
    : public ConstAccessor<T>
  {
  public:
    /// Type referencing an item.
    typedef typename AuxDataTraits<T>::reference_type
     reference_type;

    /// Type the user sees.
    typedef typename AuxDataTraits<T>::element_type
     element_type;

    /// Pointer into the container holding this item.
    typedef typename AuxDataTraits<T>::container_pointer_type
      container_pointer_type;

    using ConstAccessor<T>::operator();
    using ConstAccessor<T>::getDataArray;


    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     *
     * The name -> auxid lookup is done here.
     */
    Accessor (const std::string& name);


    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     * @param clsname The name of its associated class.  May be blank.
     *
     * The name -> auxid lookup is done here.
     */
    Accessor (const std::string& name, const std::string& clsname);


    /**
     * @brief Fetch the variable for one element, as a non-const reference.
     * @param e The element for which to fetch the variable.
     */
    reference_type operator() (AuxElement& e) const;


    /**
     * @brief Fetch the variable for one element, as a non-const reference.
     * @param container The container from which to fetch the variable.
     * @param index The index of the desired element.
     *
     * This allows retrieving aux data by container / index.
     * Looping over the index via this method will be faster then
     * looping over the elements of the container.
     */
    reference_type operator() (AuxVectorData& container, size_t index) const;


    /**
     * @brief Set the variable for one element.
     * @param e The element for which to fetch the variable.
     * @param x The variable value to set.
     */
    void set (AuxElement& e, const element_type& x) const;


    /**
     * @brief Get a pointer to the start of the auxiliary data array.
     * @param container The container from which to fetch the variable.
     */
    container_pointer_type getDataArray (AuxVectorData& container) const;


    /**
     * @brief Test to see if this variable exists in the store and is writable.
     * @param e An element of the container which to test the variable.
     */
    bool isAvailableWritable (AuxElement& e) const;
  };


  /**
   * @brief Helper class to provide type-safe access to aux data.
   *
   * This is like @c Accessor, except that it only `decorates' the container.
   * What this means is that this object can operate on a const container
   * and return a non-const reference.  However, if the container is locked,
   * this will only work if either this is a reference to a new variable,
   * in which case it is marked as a decoration, or it is a reference
   * to a variable already marked as a decoration.
   * 
   * This is written as a separate class in order to be able
   * to cache the name -> auxid lookup.
   *
   * You might use this something like this:
   *
   *@code
   *   // Only need to do this once.
   *   Myclass::Decorator<int> vint1 ("myInt");
   *   ...
   *   const Myclass* m = ...;
   *   vint1 (*m) = 123;
   @endcode
   */
  template <class T>
  class Decorator
  {
  public:
    /// Type referencing an item.
    typedef typename AuxDataTraits<T>::reference_type
     reference_type;

    /// Type the user sees.
    typedef typename AuxDataTraits<T>::element_type
     element_type;

    /// Pointer into the container holding this item.
    typedef typename AuxDataTraits<T>::container_pointer_type
      container_pointer_type;
    typedef typename AuxDataTraits<T>::const_container_pointer_type
      const_container_pointer_type;


    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     *
     * The name -> auxid lookup is done here.
     */
    Decorator (const std::string& name);


    /**
     * @brief Constructor.
     * @param name Name of this aux variable.
     * @param clsname The name of its associated class.  May be blank.
     *
     * The name -> auxid lookup is done here.
     */
    Decorator (const std::string& name, const std::string& clsname);


    /**
     * @brief Fetch the variable for one element, as a non-const reference.
     * @param e The element for which to fetch the variable.
     *
     * If the container is locked, this will allow fetching only variables
     * that do not yet exist (in which case they will be marked as decorations)
     * or variables already marked as decorations.
     */
    reference_type operator() (const AuxElement& e) const;


    /**
     * @brief Fetch the variable for one element, as a non-const reference.
     * @param container The container from which to fetch the variable.
     * @param index The index of the desired element.
     *
     * This allows retrieving aux data by container / index.
     * Looping over the index via this method will be faster then
     * looping over the elements of the container.
     *
     * If the container is locked, this will allow fetching only variables
     * that do not yet exist (in which case they will be marked as decorations)
     * or variables already marked as decorations.
     */
    reference_type
    operator() (const AuxVectorData& container, size_t index) const;


    /**
     * @brief Set the variable for one element.
     * @param e The element for which to fetch the variable.
     * @param x The variable value to set.
     */
    void set (const AuxElement& e, const element_type& x) const;


    /**
     * @brief Get a pointer to the start of the auxiliary data array.
     * @param container The container from which to fetch the variable.
     */
    const_container_pointer_type getDataArray (const AuxVectorData& container) const;
    

    /**
     * @brief Get a pointer to the start of the auxiliary data array.
     * @param container The container from which to fetch the variable.
     *
     * If the container is locked, this will allow fetching only variables
     * that do not yet exist (in which case they will be marked as decorations)
     * or variables already marked as decorations.
     */
    container_pointer_type getDecorationArray (const AuxVectorData& container) const;
    

    /**
     * @brief Test to see if this variable exists in the store.
     * @param e An element of the container which to test the variable.
     */
    bool isAvailable (const AuxElement& e) const;


    /**
     * @brief Test to see if this variable exists in the store and is writable.
     * @param e An element of the container which to test the variable.
     */
    bool isAvailableWritable (const AuxElement& e) const;


    /**
     * @brief Return the aux id for this variable.
     */
    SG::auxid_t auxid() const;


  private:
    /// The cached @c auxid.
    SG::auxid_t m_auxid;
  };


  /**
   * @brief Fetch an aux data variable, as a non-const reference.
   * @param name Name of the aux variable.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * class above.
   */
  template <class T>
  typename AuxDataTraits<T>::reference_type
  auxdata (const std::string& name);


  /**
   * @brief Fetch an aux data variable, as a non-const reference.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * class above.
   */
  template <class T>
  typename AuxDataTraits<T>::reference_type
  auxdata (const std::string& name,
           const std::string& clsname);


  /**
   * @brief Fetch an aux data variable, as a const reference.
   * @param name Name of the aux variable.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * or @c ConstAccessor classes above.
   */
  template <class T>
  typename AuxDataTraits<T>::const_reference_type
  auxdata (const std::string& name) const;


  /**
   * @brief Fetch an aux data variable, as a const reference.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * or @c ConstAccessor classes above.
   */
  template <class T>
  typename AuxDataTraits<T>::const_reference_type
  auxdata (const std::string& name,
           const std::string& clsname) const;


  /**
   * @brief Fetch an aux data variable, as a const reference.
   * @param name Name of the aux variable.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c ConstAccessor
   * class above.
   */
  template <class T>
  typename AuxDataTraits<T>::const_reference_type
  auxdataConst (const std::string& name) const;


  /**
   * @brief Fetch an aux data variable, as a const reference.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c ConstAccessor
   * class above.
   */
  template <class T>
  typename AuxDataTraits<T>::const_reference_type
  auxdataConst (const std::string& name,
                const std::string& clsname) const;


  /**
   * @brief Check if an aux variable is available for reading
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * class above.
   */
  template <class T>
  bool isAvailable (const std::string& name,
                    const std::string& clsname = "") const;


  /**
   * @brief Check if an aux variable is available for writing
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * class above.
   */
  template <class T>
  bool isAvailableWritable (const std::string& name,
                            const std::string& clsname = "");


  /**
   * @brief Check if an aux variable is available for writing as a decoration.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * class above.
   */
  template <class T>
  bool isAvailableWritableAsDecoration (const std::string& name,
                                        const std::string& clsname = "") const;


  /**
   * @brief Fetch an aux decoration, as a non-const reference.
   * @param name Name of the aux variable.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * class above.
   *
   * If the container is locked, this will allow fetching only variables
   * that do not yet exist (in which case they will be marked as decorations)
   * or variables already marked as decorations.
   */
  template <class T>
  typename AuxDataTraits<T>::reference_type
  auxdecor (const std::string& name) const;


  /**
   * @brief Fetch an aux decoration, as a non-const reference.
   * @param name Name of the aux variable.
   * @param clsname The name of the associated class.  May be blank.
   *
   * This method has to translate from the aux data name to the internal
   * representation each time it is called.  Using this method
   * inside of loops is discouraged; instead use the @c Accessor
   * class above.
   *
   * If the container is locked, this will allow fetching only variables
   * that do not yet exist (in which case they will be marked as decorations)
   * or variables already marked as decorations.
   */
  template <class T>
  typename AuxDataTraits<T>::reference_type
  auxdecor (const std::string& name,
            const std::string& clsname) const;


  /**
   * @brief Create a new (empty) private store for this object.
   *
   * @c ExcBadPrivateStore will be thrown if this object is already
   * associated with a store.
   */
  void makePrivateStore();


  /**
   * @brief Create a new private store for this object and copy aux data.
   * @param other The object from which aux data should be copied.
   *
   * @c ExcBadPrivateStore will be thrown if this object is already
   * associated with a store.
   *
   * If @c other is an object that has aux data, then those data will
   * be copied; otherwise, nothing will be done.
   */
  template <class U1>
  void makePrivateStore (const U1& other);


  /**
   * @brief Create a new private store for this object and copy aux data.
   * @param other The object from which aux data should be copied.
   *
   * @c ExcBadPrivateStore will be thrown if this object is already
   * associated with a store.
   *
   * If @c other is an object that has aux data, then those data will
   * be copied; otherwise, nothing will be done.
   */
  template <class U1>
  void makePrivateStore (const U1* other);


  /**
   * @brief Release and free any private store associated with this object.
   *
   * @c ExcBadPrivateStore will be thrown if this object does not
   * have a private store.
   */
  void releasePrivateStore();


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * If store is nonzero, this adds a standalone store to the object.
   * The object must not be in a container and must not have a private store.
   * If store is zero, this removes a standalone store.
   */
  void setStore (const SG::IConstAuxStore* store);


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * If store is nonzero, this adds a standalone store to the object.
   * The object must not be in a container and must not have a private store.
   * If store is zero, this removes a standalone store.
   */
  void setStore (SG::IAuxStore* store);


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * If store is nonzero, this adds a standalone store to the object.
   * The object must not be in a container and must not have a private store.
   * If store is zero, this removes a standalone store.
   */
  void setStore (const DataLink< SG::IConstAuxStore >& store);


  /**
   * @brief Synonym for @c setStore with @c IConstAuxStore.
   * @param store The new store.
   */
  void setConstStore (const SG::IConstAuxStore* store);


  /**
   * @brief Synonym for @c setStore with @c IAuxStore.
   * @param store The new store.
   */
  void setNonConstStore (SG::IAuxStore* store);


  /**
   * @brief Test to see if this object is currently using a private store.
   */
  bool usingPrivateStore() const;


  /**
   * @brief Test to see if this object is currently using a standalone store.
   */
  bool usingStandaloneStore() const;


  /**
   * @brief Return the current store, as a const interface.
   *
   * This will be non-zero if either a const or non-const store
   * is associated with this object.
   * This will fetch either a private or standalone store.
   */
  const SG::IConstAuxStore* getConstStore() const;


  /**
   * @brief Return the current store, as a non-const interface.
   *
   * This will be non-zero if a non-const store is associated with this object.
   * This will fetch either a private or standalone store.
   */
  SG::IAuxStore* getStore() const;


  /**
   * @brief Clear the cached aux data pointers.
   *
   * You should call this any time something changes in the aux store
   * that could invalidate the vector pointers.
   */
  void clearCache();


  /**
   * @brief Return a set of identifiers for existing data items
   *        for this object.
   *
   *        If this object has a private or standalone store, then information
   *        from that will be returned.  Otherwise, if this element
   *        is part of a container, then information for the container
   *        will be returned.  Otherwise, return an empty set.
   */
  const SG::auxid_set_t& getAuxIDs() const;


  /**
   * @brief Return true if this object has an associated store.
   *
   * This will be true for either a private or standalone store.
   */
  bool hasStore() const;


  /**
   * @brief Return true if this object has an associated non-const store.
   *
   * This will be true for either a private or standalone store.
   */
  bool hasNonConstStore() const;


  /**
   * @brief Clear all decorations.
   *
   * Erase all decorations from an associated store, restoring the state to when
   * @c lock was called.
   *
   * Returns true if there were any decorations that were cleared,
   * false if the store did not contain any decorations.
   */
  bool clearDecorations() const;


private:
  friend class SG::AuxVectorBase;
  friend class SG::AuxVectorBase_test;


  /**
   * @brief Constructor with explicit container / index.
   * @param container Container of which this element will be a part.
   * @param index Index of this element within the container.
   *
   * This does not make any changes to aux data.
   */
  AuxElement (SG::AuxVectorData* container,  size_t index);



  /**
   * @brief Out-of-line portion of destructor.
   *
   * Delete a private store if we have one.
   */
  void releasePrivateStoreForDtor();


  /**
   * @brief Set the index/container for this element.
   * @param index The index of this object within the container.
   * @param container The container holding this object.
   *                  May be null if this object is being removed
   *                  from a container.
   *
   * Usually this simply sets the index and container members
   * of this object.  However, in the case where this object has
   * an associated private store, then we need to deal with
   * releasing the store if the object is being added to a container,
   * or making a new store if the object is being removed from a container.
   */
  void setIndex (size_t index, SG::AuxVectorData* container);


  /**
   * @brief Set the index/container for this element.
   * @param index The index of this object within the container.
   * @param container The container holding this object.
   *                  May be null if this object is being removed
   *                  from a container.
   *
   * This is called from @c setIndex when we have a private store to deal with.
   */
  bool setIndexPrivate (size_t index, SG::AuxVectorData* container);


  /**
   * @brief Create a new private store for this object and copy aux data.
   * @param other The object from which aux data should be copied.
   *
   * @c ExcBadPrivateStore will be thrown if this object is already
   * associated with a store.
   *
   * This overload handles the case where @c other does not have aux data.
   */
  void makePrivateStore1 (const void*);


  /**
   * @brief Create a new private store for this object and copy aux data.
   * @param other The object from which aux data should be copied.
   *
   * @c ExcBadPrivateStore will be thrown if this object is already
   * associated with a store.
   *
   * This overload handles the case where @c other does have aux data.
   */
  void makePrivateStore1 (const AuxElement* other);


  /**
   * @brief Set the store associated with this object.
   * @param store The new store.
   *
   * Helper for @c setStore.  Creates the @c AuxElementStandaloneData
   * object if needed and returns it.
   */
  AuxElementStandaloneData* setStore1 (const SG::IConstAuxStore* store);


  /**
   * @brief Clear all aux data associated with this element.
   *
   * If this object has no associated store, this does nothing.
   * If the associated aux data is const, this throws @c ExcConstAuxData.
   */
  void clearAux();


  /**
   * @brief Copy aux data from another object.
   * @param other The object from which to copy.
   *
   * If this object has no associated store, this does nothing.
   * If the associated aux data is const, this throws @c ExcConstAuxData.
   *
   * All aux data items from @c other are copied to this object.
   * Any aux data items associated with this object that are not present
   * in @c other are cleared.  (If @c other has no aux data, then all
   * aux data items for this object are cleared.)
   */
  void copyAux (const AuxElement& other);


  /**
   * @brief Helper to test if m_privateData is valid.
   */
  bool privateDataValid() const;


  /// The index of this element within its container.
  /// Should be 0 if this object is not within a container.
  size_t m_index;

  /// The container of which this object is an element.
  /// Should be null if this object is not within a container,
  /// except for the case where this object as a private store.
  /// In that case, @c m_container should be the same as @c m_privateData.
  SG::AuxVectorData* m_container;

  /// If this object has a current private store, then this points
  /// at the container object holding the store.  In that case,
  /// @c m_container should have the same value.  If this object
  /// had a private store but it was released because the object
  /// was added to a container, then this should be set to
  /// @c s_privatePlaceholder.  That way, we know to reconstruct
  /// the private store in the event this object is removed from
  /// the container.  Otherwise, if we no private store association at all,
  /// then this should be null.
  SG::AuxElementData* m_privateData;

  /// Special value used to mark that an object had a private store,
  /// but it was released because it was added to a container.
  /// (And therefore we should recreate the private store if the
  /// object is later removed.)
  static SG::AuxElementData* const s_privatePlaceholder;
};


} // namespace SG


#include "AthContainers/AuxElement.icc"


#ifndef XAOD_STANDALONE
CLASS_DEF (SG::AuxElement, 225182422, 1)
#endif // not XAOD_STANDALONE


#endif // not ATHCONTAINERS_AUXELEMENTBASE_H
