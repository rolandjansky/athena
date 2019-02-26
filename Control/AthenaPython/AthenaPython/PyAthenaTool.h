///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaTool.h 
// Header file for class PyAthena::Tool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYATHENATOOL_H 
#define ATHENAPYTHON_PYATHENATOOL_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaPython/IPyComponent.h"
#include "CxxUtils/checker_macros.h"

// Forward declaration
// Python
struct _object;
typedef _object PyObject;

namespace PyAthena {

typedef ::AthAlgTool ToolBase_t;
class ATLAS_NOT_THREAD_SAFE Tool
  : public extends<ToolBase_t,  ::IPyComponent>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  Tool( const std::string& type,
	const std::string& name, 
	const IInterface* parent );

  /// Destructor: 
  virtual ~Tool(); 

  // Athena tool's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  sysInitialize() override;
  virtual StatusCode  finalize() override;


  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  virtual const char* typeName() const override;


  /** @brief return associated python object. BORROWED reference.
   */ 
  virtual PyObject* self() override { return m_self; }


 protected: 

  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj ) override;

 private: 
  /// Default constructor: 
  Tool() = delete;

 protected: 

  /// Pointer to self (from the python world)
  PyObject* m_self;

}; 

} //> end namespace PyAthena

#endif //> ATHENAPYTHON_PYATHENATOOL_H
