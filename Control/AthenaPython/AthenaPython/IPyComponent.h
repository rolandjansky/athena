///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

 public: 
  DeclareInterfaceID (IPyComponent, 1, 0);

  /** Destructor: 
   */
  virtual ~IPyComponent() = default;


  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  virtual const char* typeName() const = 0;


  /** @brief return associated python object. BORROWED reference.
   */ 
  virtual PyObject* self() =0;

 protected:
  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj ) = 0;

}; 


#endif //> ATHENAPYTHON_IPYCOMPONENT_H
