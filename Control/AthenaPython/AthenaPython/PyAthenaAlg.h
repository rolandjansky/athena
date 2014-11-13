///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaAlg.h 
// Header file for class PyAthena::Alg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYATHENAALG_H 
#define ATHENAPYTHON_PYATHENAALG_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaPython/IPyComponent.h"

// Forward declaration
// Python
struct _object;
typedef _object PyObject;

namespace PyAthena {

typedef ::AthAlgorithm AlgBase_t;

class Alg : virtual public ::IPyComponent,
                    public   AlgBase_t
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor: 
  Alg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~Alg(); 

  // Framework's Hooks
  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode beginRun();
  virtual StatusCode execute();
  virtual StatusCode endRun();
  virtual StatusCode finalize();

  virtual StatusCode sysInitialize();

  virtual void resetExecuted();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  virtual const char* typeName() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** @brief return associated python object. BORROWED reference.
   */ 
  virtual PyObject* self() { return m_self; }

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  Alg();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Pointer to self (from the python world)
  PyObject* m_self;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PyAthena

#endif //> ATHENAPYTHON_PYATHENAALG_H
