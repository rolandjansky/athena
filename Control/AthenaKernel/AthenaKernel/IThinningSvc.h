///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ITHINNINGSVC_H 
#define ATHENAKERNEL_ITHINNINGSVC_H 

// STL includes
#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <map>         // XXX in fact we'd really want to use a hash_map...
                       // but that's in SGTools !!!

// FrameWork includes
#include "GaudiKernel/IService.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IThinningHdlr.h"
#include "AthenaKernel/ISlimmingHdlr.h"

// Forward declaration
class ThinningOutputTool;

// python forward
struct _object; typedef _object PyObject;
class IThinningSvc;
namespace AthenaInternal {
  PyObject*   thinContainer   (IThinningSvc*,PyObject*,PyObject*,int);
  std::size_t thinIdxContainer(IThinningSvc*,PyObject*,std::size_t  );
}

/**
 * @class IThinningSvc
 * @brief Abstract interface to ThinningSvc. The service allows to thin 
 * (remove elements from) a DataVector-like container while the
 * container is being written out.  See AthExThinning for an example of usage.
 * @author S.Binet<binet@cern.ch>
 */
/////////////////////////////////////////////////////////////////// 
class IThinningSvc : virtual public IService,
		     virtual public IProxyDict
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// @brief the type holding the decision for each element of the container
  typedef std::vector<bool> VecFilter_t;
  /// @brief the type holding the decision for each element of the container
  typedef std::map<std::size_t, bool> SparseFilter_t;
  /// @brief the type holding the decision for each element of the container
  typedef SparseFilter_t Filter_t;

  /////////////////////////////////////////////////////////////////// 
  // Public struct: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  /** @brief Struct to hold the type of filtering one wants to apply.
   *         This is a poor-man way of modeling the && and || combination
   *         of different passes of thinning.
   */
  struct Operator {
    enum Type {
      And = 0,
      Or  = 1
    };
  };

  /////////////////////////////////////////////////////////////////// 
  // Public members: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  // NB: This is an unsigned type, and thus represents the maximum
  // size that the allocator can hold. (modeled after std::string::npos)
  static const std::size_t RemovedIdx = static_cast<std::size_t>(-1);
 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** @brief Destructor: 
   */
  virtual ~IThinningSvc();

  static const InterfaceID& interfaceID();

  /** @brief Tell clients if any thinning occurred during the event processing
   */
  virtual
  bool thinningOccurred() const = 0;

  /** @brief test if a container is thinned
   */
  template<class Container>
  bool 
  thinningOccurred(const Container* container) const
  {
    const void* obj = static_cast<const void*>(container);
    const SG::DataProxy* p = this->proxy(obj);
    return this->is_thinned_impl(p);
  }

  /** @brief Helper method to retrieve the instance of the currently active
   *         thinning service.
   *  @param activeSvc the new active thinning svc instance. 
   *         If null pointer, then returns old/current active thinning svc
   *  @param overrideSvc the current activeSvc with the given one, even if
   *         the given one is NULL.
   *  FIXME: this is a horrible hack and bad things will happen in a 
   *         multithreaded mode. It is well possible the active thinning svc
   *         gets swapped/updated in the middle of a "thinning transaction".
   *         The correct way of implementing this would be to get the pointer
   *         of the active thinning svc through an ActiveThinningSvc (as for
   *         the @c StoreGateSvc and its @c ActiveStoreSvc buddy). However,
   *         as the converters which need that @c IThinningSvc instance to
   *         recompute the correct indices of thinned containers, are "framework
   *         blind", it would just be moving the problem elsewhere...
   *
   *         The pointer is now in thread-local storage, which should alleviate
   *         most of the problem.  Thinning still needs to be rethought for MT.
   */
  static IThinningSvc* instance( IThinningSvc* activeSvc = 0,
				 bool overrideSvc = false );

  // slimming part
  
  /** @brief register a slimming handler with the @c IThinningSvc
   *  Registering a @c Athena::ISlimmingHdlr allows to slim an object and keep
   *  the side effects of slimming it 'guarded' within one output stream.
   *  The original state (before slimming) of the object will be restored after
   *  to-disk serialization took place.
   *  Note: @c IThinningSvc will take ownership of the handler.
   */
  virtual
  StatusCode register_slimmer (Athena::ISlimmingHdlr *handler) =0;

//   /** @brief retrieve an iterator over the @c ISlimmingHdlrs
//    */
//   virtual
//   range<Athena::ISlimmingHdlr> begin_slimmers() =0;

//   /** @brief retrieve an iterator over the @c ISlimmingHdlrs
//    */
//   virtual
//   range<Athena::ISlimmingHdlr> end_slimmers() =0;
  
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:
  friend class TrigNavigationThinningSvc;

  /** @brief Retrieve the handler (if any) to thin a @c DataProxy
   */
  virtual Athena::IThinningHdlr* handler( SG::DataProxy* proxy ) = 0;

  /** @brief Build the 'db' of elements to be kept for each container
   */
  virtual StatusCode
  filter_impl( Athena::IThinningHdlr* handler,
	       SG::DataProxy* proxy,
	       const Filter_t& filter,
	       const IThinningSvc::Operator::Type op = Operator::And ) = 0;

  /// @brief make the @c ThinningOutputTool a friend so it can access the 
  ///       internal @c IThinningSvc::commit and 
  ///       @c IThinningSvc::rollback methods
  friend class ThinningOutputTool;

  /** @brief Commit the pending slimming and thinning actions
   *     - first apply slimming via the registered ISlimmingHdlrs
   *     - then apply thinning via the registered IThinningHdlrs
   *          - update the association of before/after-thinning indices
   *          - actually delete the requested to-be-thinned elements
   */
  virtual 
  StatusCode commit() = 0;

  /** @brief Rollback the slimming and thinning actions:
   *     - first rollback thinning
   *          - restore the requested to-be-thinned elements
   *     - then rollback slimming
   */
  virtual 
  StatusCode rollback() = 0;

  /** @brief Get the index after thinning of a given container, providing
   *         the old index.
   *  @returns IThinningSvc::RemovedIdx if the element asked-for has been
   *           removed during thinning.
   */
  virtual 
  std::size_t index_impl( const SG::DataProxy* objProxy, 
                          std::size_t idx ) const = 0;

  /** @brief test if a container is thinned
   */
  virtual
  bool is_thinned_impl(const SG::DataProxy* p) const = 0;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  
  /////////////////////////////////////////////////////////////////// 
  // friends
  /////////////////////////////////////////////////////////////////// 
  ///access filter_impl
  friend 
  PyObject* 
  AthenaInternal::thinContainer(IThinningSvc*,PyObject*,PyObject*,int);
  ///access index
  friend 
  std::size_t 
  AthenaInternal::thinIdxContainer(IThinningSvc*,PyObject*,std::size_t);
}; 


/////////////////////////////////////////////////////////////////// 
// Thinning hook function.
//


/**
 * @brief Propagate thinning.  Called after applying thinning to @c in.
 * @param in The object that was thinned.
 * @param svc The thinning service (for convenience).
 * @param filter Lists the elements to be kept.
 * @param op How to merge results: AND or OR.
 *
 * Sometimes containers depend on one another, such that when one
 * is thinned, the other must also be thinned in the same way.
 * An example is the DataVector auxiliary stores.
 * This function provides a way of automatically propagating
 * thinning for certain types.  Once thinning has been applied to an object,
 * @c thinningHook will be called with a pointer to the object as the
 * first argument.  This default version is a no-op and matches
 * any pointer type.  However, additional overloads can be provided
 * for specific container types.
 */
inline
StatusCode thinningHook (const void* /*in*/,
                         IThinningSvc* /*svc*/,
                         const IThinningSvc::Filter_t& /*filter*/,
                         const IThinningSvc::Operator::Type /*op*/ )
{
  return StatusCode::SUCCESS;
}
                         
/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IThinningSvc::interfaceID() 
{ 
  static const InterfaceID IID_IThinningSvc("IThinningSvc", 1, 0);
  return IID_IThinningSvc; 
}

template<>
inline
bool 
IThinningSvc::thinningOccurred<SG::DataProxy>(const SG::DataProxy* dp) const
{
  return this->is_thinned_impl(dp);
}


#endif //> ATHENAKERNEL_ITHINNINGSVC_H
