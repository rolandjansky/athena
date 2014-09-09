///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IPyComponent.h 
// Header file for class IPyComponent
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_IPYCOMPONENT_H 
#define ATHENAPYTHON_IPYCOMPONENT_H 

/** @class IPyComponent
 */

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/INamedInterface.h"

// AthenaPython includes

// fwd declares
struct _object;
typedef _object PyObject;
namespace PyAthena { class PyComponentMgr; }

class IPyComponent : virtual public INamedInterface
{ 
  friend class PyAthena::PyComponentMgr; // for access to setPyAttr
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IPyComponent();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  virtual const char* typeName() const = 0;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /** @brief return associated python object. BORROWED reference.
   */ 
  virtual PyObject* self() =0;

 protected:
  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj ) = 0;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& IPyComponent::interfaceID() 
{ 
  static const InterfaceID IID_IPyComponent("IPyComponent", 1, 0);
  return IID_IPyComponent; 
}

#endif //> ATHENAPYTHON_IPYCOMPONENT_H
