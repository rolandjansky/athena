///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IPyComponentMgr.h 
// Header file for class IPyComponentMgr
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_IPYCOMPONENTMGR_H 
#define ATHENAPYTHON_IPYCOMPONENTMGR_H 

/** @class IPyComponentMgr
 */

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/IService.h"

// AthenaPython includes

// forward declaration
struct _object;
typedef _object PyObject;
class IPyComponent;

class IPyComponentMgr : virtual public IService
{ 
 public: 
  DeclareInterfaceID (IPyComponentMgr, 1, 0);

  /** Destructor: 
   */
  virtual ~IPyComponentMgr() = default;


  /**
   * Retrieve a python object from the python world
   * this returns a NEW reference to that python object.
   */
  virtual PyObject* pyObject( IPyComponent* comp ) = 0;
}; 


#endif //> ATHENAPYTHON_IPYCOMPONENTMGR_H
