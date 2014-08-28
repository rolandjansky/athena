/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGSERGUIDHELPERBASE_H
#define ITRIGSERGUIDHELPERBASE_H

#include "GaudiKernel/IAlgTool.h"
#include <string>

/** @author Jiri.Masik@cern.ch  
    @class ITrigSerGuidHelper
    interface for Guid operations
*/

/** Declaration of the interface ID ( interface id, major version, minor version) */
static const InterfaceID IID_ITrigSerGuidHelper("ITrigSerGuidHelper", 1 , 0);

class ITrigSerGuidHelperBase {
public:

  /** @return StatusCode
   */
  virtual StatusCode ClassNameToInts(const std::string &clname, uint32_t *iarr) = 0;

  /** @return StatusCode
   */
  virtual StatusCode IntsToClassName(const uint32_t *iarr, std::string &clname) = 0;

  /**
   */
  virtual ~ITrigSerGuidHelperBase(){};
};


/**
   @class ITrigSerGuidHelper
   AlgTool interface of a helper for guid operations
*/


class ITrigSerializeGuidHelper : virtual public ITrigSerGuidHelperBase, virtual public IAlgTool

{
 public:

   // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrigSerGuidHelper; }

};

#endif

