///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaSvc.h 
// Header file for class PyAthena::Svc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYATHENASVC_H 
#define ATHENAPYTHON_PYATHENASVC_H 

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaPython/IPyComponent.h"
#include "CxxUtils/checker_macros.h"

// Forward declaration
// Python
struct _object;
typedef _object PyObject;

namespace PyAthena {

typedef ::AthService SvcBase_t;
class ATLAS_NOT_THREAD_SAFE Svc
  : public extends<SvcBase_t, ::IPyComponent, ::IIncidentListener>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  Svc( const std::string& name, ISvcLocator* svcLocator );

  /// Destructor: 
  virtual ~Svc(); 

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode reinitialize() override;
  virtual StatusCode beginRun();
  virtual StatusCode endRun();
  virtual StatusCode finalize() override;
  //@}

  virtual StatusCode sysInitialize() override;


  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  const char* typeName() const override;

  /** @brief return associated python object. BORROWED reference.
   */ 
  virtual PyObject* self() override { return m_self; }

  /** callback method for the @c IIncidentSvc
   */
  virtual void handle( const Incident& incident ) override;


 protected: 

  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj ) override;


 private: 
  /// Default constructor: 
  Svc() = delete;

 protected: 

  /// Pointer to self (from the python world)
  PyObject* m_self;

}; 

} //> end namespace PyAthena

#endif //> ATHENAPYTHON_PYATHENASVC_H
