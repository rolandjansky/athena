/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGSERHELPERBASE_H
#define ITRIGSERHELPERBASE_H

#include "GaudiKernel/IAlgTool.h"
#include <string>

/** @author Jiri.Masik@cern.ch  
    @class ITrigSerConvHelper
    Tool interface for serializer
*/

/** Declaration of the interface ID ( interface id, major version, minor version) */
static const InterfaceID IID_ITrigSerConvHelper("ITrigSerConvHelper", 1 , 0);

class IOpaqueAddress;

class ITrigSerConvHelperBase {
public:

  /** @return StatusCode
   */
  virtual StatusCode createObj(const std::string &clname, IOpaqueAddress* iAddr, void *&ptr, bool) = 0;

  /** @return StatusCode
   */
  virtual StatusCode createRep(const std::string &clname, void *ptr, std::vector<uint32_t> &out, bool) = 0;

  /**
   */
  virtual ~ITrigSerConvHelperBase(){};
};





/**
   @class ITrigSerializeConvHelper
   AlgTool interface of a helper for converter classes
*/

class ITrigSerializeConvHelper : virtual public ITrigSerConvHelperBase, virtual public IAlgTool

{
 public:

   // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrigSerConvHelper; }

};

#endif

