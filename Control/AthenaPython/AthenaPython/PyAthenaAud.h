///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  Aud( const std::string& name, ISvcLocator* svcLocator );

  /// Destructor: 
  virtual ~Aud(); 

  /// Gaudi Aud Implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode sysInitialize();
  virtual StatusCode finalize();
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  const char* typeName() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

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
  virtual PyObject* self() { return m_self; }

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj );

  /// @c Auditor interface

  virtual void before(StandardEventType, INamedInterface*);
  virtual void before(StandardEventType, const std::string&);

  virtual void before(CustomEventTypeRef, INamedInterface*);
  virtual void before(CustomEventTypeRef, const std::string&);

  virtual void after(StandardEventType, INamedInterface*, const StatusCode&);
  virtual void after(StandardEventType, const std::string&, const StatusCode&);

  virtual void after(CustomEventTypeRef, INamedInterface*, const StatusCode&);
  virtual void after(CustomEventTypeRef, const std::string&, const StatusCode&);

  // ---> Obsolete methods
  virtual void beforeInitialize(INamedInterface* ) ;
  virtual void afterInitialize(INamedInterface* ) ;
  virtual void beforeReinitialize(INamedInterface* ) ;
  virtual void afterReinitialize(INamedInterface* ) ;
  virtual void beforeExecute(INamedInterface* );
  virtual void afterExecute(INamedInterface*, const StatusCode& );
  virtual void beforeFinalize(INamedInterface* ) ;
  virtual void afterFinalize(INamedInterface* ) ;
  virtual void beforeBeginRun(INamedInterface* );
  virtual void afterBeginRun(INamedInterface* );
  virtual void beforeEndRun(INamedInterface* );
  virtual void afterEndRun(INamedInterface* );
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
