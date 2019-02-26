///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaAud.h 
// Header file for class PyAthena::Aud
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYATHENAAUD_H 
#define ATHENAPYTHON_PYATHENAAUD_H 

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "AthenaPython/IPyComponent.h"
#include "CxxUtils/checker_macros.h"

// Forward declaration
class INamedInterface;
// Python
struct _object;
typedef _object PyObject;

namespace PyAthena {

class ATLAS_NOT_THREAD_SAFE Aud : virtual public ::IPyComponent,
                                  public ::Auditor
{ 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  Aud( const std::string& name, ISvcLocator* svcLocator );

  /// Destructor: 
  virtual ~Aud(); 

  /// Gaudi Aud Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode sysInitialize() override;
  virtual StatusCode finalize() override;
  //@}


  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  virtual const char* typeName() const override;


  /// Audit the start of a standard "event".
  virtual void py_before(IAuditor::StandardEventType, const std::string&);

  /// Audit the start of a custom "event".
  virtual void py_before(IAuditor::CustomEventTypeRef, const std::string&);

  /// Audit the end of a standard "event".
  virtual void py_after(IAuditor::StandardEventType,
			const std::string&,
			const StatusCode&);

  /// Audit the end of a custom "event".
  virtual void py_after(IAuditor::CustomEventTypeRef,
			const std::string&,
			const StatusCode&);

  /** @brief return associated python object. BORROWED reference.
   */ 
  virtual PyObject* self() override { return m_self; }

 protected: 

  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj ) override;

  /// @c Auditor interface

  virtual void before(StandardEventType, INamedInterface*) override;
  virtual void before(StandardEventType, const std::string&) override;

  virtual void before(CustomEventTypeRef, INamedInterface*) override;
  virtual void before(CustomEventTypeRef, const std::string&) override;

  virtual void after(StandardEventType, INamedInterface*, const StatusCode&) override;
  virtual void after(StandardEventType, const std::string&, const StatusCode&) override;

  virtual void after(CustomEventTypeRef, INamedInterface*, const StatusCode&) override;
  virtual void after(CustomEventTypeRef, const std::string&, const StatusCode&) override;

  // ---> Obsolete methods
  virtual void beforeInitialize(INamedInterface* ) override;
  virtual void afterInitialize(INamedInterface* ) override;
  virtual void beforeReinitialize(INamedInterface* ) override;
  virtual void afterReinitialize(INamedInterface* ) override;
  virtual void beforeExecute(INamedInterface* ) override;
  virtual void afterExecute(INamedInterface*, const StatusCode& ) override;
  virtual void beforeFinalize(INamedInterface* ) override;
  virtual void afterFinalize(INamedInterface* ) override;
  virtual void beforeBeginRun(INamedInterface* ) override;
  virtual void afterBeginRun(INamedInterface* ) override;
  virtual void beforeEndRun(INamedInterface* ) override;
  virtual void afterEndRun(INamedInterface* ) override;
  // <--- Obsolete methods

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  Aud();

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

#endif //> ATHENAPYTHON_PYATHENAAUD_H
