///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyComponentMgr.h 
// Header file for class PyAthena::PyComponentMgr
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYCOMPONENTMGR_H 
#define ATHENAPYTHON_PYCOMPONENTMGR_H 

// STL includes
#include <string>
#include <unordered_map>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"
#include "CxxUtils/checker_macros.h"

// AthenaPython includes
#include "AthenaPython/IPyComponentMgr.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;
struct _object;
typedef _object PyObject;
class IPyComponent;

namespace PyAthena {

class ATLAS_NOT_THREAD_SAFE PyComponentMgr : virtual public IPyComponentMgr,
                                             public AthService
{ 
  friend class SvcFactory<PyAthena::PyComponentMgr>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  PyComponentMgr( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~PyComponentMgr(); 

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /**
   * Retrieve a python object from the python world.
   * this returns a NEW reference to that python object.
   */
  PyObject* pyObject( IPyComponent* component );

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  PyComponentMgr();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /** @brief The dictionary of python components' description
   *         It should be of the form:
   *            { 'name' : { 'package' : "MyAnaPkg",
   *                         'class'   : "MyAlg",
   *                         'pyprops' : { 'OutputLevel' : 1,
   *                                       ..., }
   *                       },
   *            }
   */
  PyObject* m_dict;

  typedef std::unordered_map<std::string, PyObject*> PyComponents_t;
  /** @brief A fast look-up hash-map for python components
   *         { 'name' : PyObject* }
   *         PyObject* is NULL if not yet instantiated
   *  Note that we own the PyObjects.
   */
  PyComponents_t m_components;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& PyComponentMgr::interfaceID() 
{ 
   return IPyComponentMgr::interfaceID(); 
}

} //> namespace PyAthena

#endif //> ATHENAPYTHON_PYATHENA_PYCOMPONENTMGR_H
