// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/WriteDecorHandle.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2017
 * @brief Handle class for adding a decoration to an object.
 */


#ifndef STOREGATE_WRITEDECORHANDLE_H
#define STOREGATE_WRITEDECORHANDLE_H


#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/tools/DecorKeyHelpers.h"
#include "AthContainers/AuxElement.h"
#include "GaudiKernel/EventContext.h"
#include <type_traits>


namespace SG {


/**
 * @brief Handle class for adding a decoration to an object.
 *
 * This (and the corresponding @c ReadDecorHandle class) allow the scheduler
 * to track dependencies on decorations on objects, in addition to the objects
 * themselves.  For example, one algorithm could produce a container C,
 * a second could read C and add a decoration to it C.d, and a third algorithm
 * could read the decoration C.d.  This allows the scheduler to know that
 * the third algorithm must be executed after the second completes.
 *
 * A @c WriteDecorHandle<T, D> works just like a @c ReadHandle<T> (except that
 * using it as a property directly is not supported).  When the object is first
 * accessed, the decoration is actually created (and a SG alias is made for the
 * object under the C.d name).  The handle also has 
 * functionality for accessing a decoration of type @c D, similarly
 * to the interface provided by @c Decorator objects.  You can either
 * write the decoration from for @c SG::AuxElement object:
 *
 *@code
 *  handle(o) = x;
 @endcode
 *
 * or reference the element by its index in the container:
 *
 *@code
 *  handle(i) = x;
 @endcode
 *
 * Example:
 *
 *@code
 *  class MyAlgorithm : public AthReentrantAlgorithm {
 *    ...
 *    SG::WriteDecorHandleKey<MyCont> m_key;
 *  };
 *  ...
 *  MyAlgorithm::MyAlgorithm (const std::string& name, ISvcLocator* svcloc)
 *    : AthReentrantAlgorithm (name, svcloc), ...
 *  {
 *    declareProperty ("Key", m_key = "container.decor");
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::initialize()
 *  {
 *    ATH_CHECK( m_key.initialize() );
 *    ...
 *  }
 *
 *  StatusCode MyAlgorithm::execute (const EventContext& ctx) const
 *  {
 *    SG::WriteDecorHandle<MyCont, float> h (m_key, ctx);
 *    for (const MyObj& o : *h) {  // Access the container.
 *      h (o) = calculate (o);  // Add the decoration.
 *      ...
 @endcode
 *
 * When the decoration handle is destroyed, the decoration will be locked
 * (i.e., converted to an ordinary variable).
 *
 * Implementation note: we keep track of decorations using aliases.
 * For a container C with decoration d, the @c WriteDecorHandle will make
 * an alias C.d for C.  The @c ReadDecorHandle will then retrieve C.d from
 * StoreGate.  The alias C.d is also what enters into scheduling decisions.
 */
template <class T, class D>
class WriteDecorHandle
  : public ReadHandle<T>
{
public:
  /// Base class.
  typedef ReadHandle<T> Base;

  /// Accessor type for aux data.
  typedef SG::AuxElement::Decorator<D> accessor_t;

  /// Type referencing an aux data item.
  typedef typename accessor_t::reference_type
    reference_type;

  /// Pointer into the vector holding an aux data item.
  typedef typename accessor_t::container_pointer_type
    container_pointer_type;

  /**
   * @brief Constructor from a WriteDecorHandleKey.
   * @param key The key object holding the clid/key/store/attr.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  explicit WriteDecorHandle (const WriteDecorHandleKey<T>& key);


  /**
   * @brief Constructor from a ReadDecorHandleKey and an explicit event context.
   * @param key The key object holding the clid/key.
   * @param ctx The event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  explicit WriteDecorHandle (const WriteDecorHandleKey<T>& key,
                             const EventContext& ctx);

  /**
   * @brief Copy constructor.
   */
  WriteDecorHandle (const WriteDecorHandle& rhs);


  /**
   * @brief Move constructor.
   */
  WriteDecorHandle (WriteDecorHandle&& rhs);


  /**
   * @brief Destructor.  This will lock the decoration.
   */
  ~WriteDecorHandle();


  /**
   * @brief Assignment operator.
   */
  WriteDecorHandle& operator= (const WriteDecorHandle& rhs);


  /**
   * @brief Move operator.
   */
  WriteDecorHandle& operator= (WriteDecorHandle&& rhs);


  /**
   * @brief Is the referenced container present in SG?
   *
   * Note that this tests for the presence of the _container_,
   * not for the decoration.
   *
   * Const method; the handle does not change as a result of this.
   */
  bool isPresent() const;


  /**
   * @brief Explicitly set the event store.
   * @param store The new event store.
   *
   * This implicitly does a reset().
   *
   * We need to override this so that the setting gets made on the container
   * handle as well.
   */
  virtual StatusCode setProxyDict (IProxyDict* store) override;

  
  /**
   * @brief Fetch the variable for one element, as a reference.
   * @param e The element for which to fetch the variable.
   */
  reference_type operator() (const AuxElement& e);


  /**
   * @brief Fetch the variable for one element, as a reference.
   * @param index The index of the desired element.
   *
   * This looks up the variable in the object referenced by this handle.
   * For a standalone object, pass an index of 0.
   */
  reference_type operator() (size_t index);


  /**
   * @brief Get a pointer to the start of the auxiliary data array,
   *        for the referenced object.
   */
  container_pointer_type
  getDecorationArray();


  /**
   * @brief Test to see if this variable exists in the store,
   *        for the referenced object.
   */
  bool isAvailable();


  /**
   * @brief Return the aux id for this variable.
   */
  SG::auxid_t auxid() const;

  
private:
  /**
   * @brief Retrieve an object from StoreGate.
   * @param quiet If true, suppress failure messages.
   *
   * Extended for decoration handles: when we first retrieve the object,
   * we make an alias for the decoration and also create the decoration itself.
   */
  virtual void* typeless_dataPointer_impl (bool quiet) override;


  /**
   * @brief Test to see if this variable exists in the store,
   *        for the referenced object.
   *        Specialization for the case of a standalone object
   *        (@c T derives from @c SG::AuxElement).
   */
  bool isAvailable (std::true_type);


  /**
   * @brief Test to see if this variable exists in the store,
   *        for the referenced object.
   *        Specialization for the case of a container
   *        (@c T does not derive from @c SG::AuxElement).
   */
  bool isAvailable (std::false_type);


  /** 
   * @brief Return the referenced object as a @c SG::AuxVectorData.
   *        Specialization for the case of a standalone object
   *        (@c T derives from @c SG::AuxElement).
   */
  const SG::AuxVectorData* vectorData (std::true_type);


  /** 
   * @brief Return the referenced object as a @c SG::AuxVectorData.
   *        Specialization for the case of a container
   *        (@c T does not derive from @c SG::AuxElement).
   */
  const SG::AuxVectorData* vectorData (std::false_type);


  /** 
   * @brief Return the referenced object as a @c SG::AuxVectorData.
   *
   * If @c T is a container object, then this should be the object itself.
   * But if it is a standalone object, deriving from @c SG::AuxElement,
   * then we need to call container() on the object.
   */
  const SG::AuxVectorData* vectorData();


  /// Handle for reading the referenced object using its original name
  /// (not the alias).
  SG::ReadHandle<T> m_contHandle;


  /// Accessor for the aux data item.
  accessor_t m_acc;

  bool m_madeAlias;
};


/**
 * @brief Return a @c WriteDecorHandle referencing @c key.
 * @param key The key object holding the clid/key/store.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 *
 * The type of the decoration must be included as an explicit template parameter:
 *
 *@code
 *   auto handle = SG::makeHandle<float> (key);
 @endcode
 *
 * Note that @c D comes first in the argument list.  It's given explicitly,
 * while @c T is inferred from @c key.
 */
template <class D, class T>
WriteDecorHandle<T, D> makeHandle (const WriteDecorHandleKey<T>& key);


/**
 * @brief Return a @c WriteDecorHandle referencing @c key for an explicit context.
 * @param key The key object holding the clid/key/store.
 * @param ctx The event context.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 *
 * If the default event store has been requested, then the thread-specific
 * store from the event context will be used.
 *
 * The type of the decoration must be included as an explicit template parameter:
 *
 *@code
 *   auto handle = SG::makeHandle<float> (key, ctx);
 @endcode
 *
 * Note that @c D comes first in the argument list.  It's given explicitly,
 * while @c T is inferred from @c key.
 */
template <class D, class T>
WriteDecorHandle<T, D> makeHandle (const WriteDecorHandleKey<T>& key,
                                   const EventContext& ctx);


/**
 * @brief These two signatures are to catch cases where the explicit
 *        template argument is omitted from the @c makeHandle call
 *        and give an error tailored to that.  Otherwise, the @c makeHandle
 *        call for @c ReadHandle would match, potentially giving a much
 *        more confusing error.
 */
template <class T>
void makeHandle (const WriteDecorHandleKey<T>& key);
template <class T>
void makeHandle (const WriteDecorHandleKey<T>& key,
                 const EventContext& ctx);


} // namespace SG


#include "StoreGate/WriteDecorHandle.icc"


#endif // not STOREGATE_WRITEDECORHANDLE_WRITEDECORHANDLE_H
