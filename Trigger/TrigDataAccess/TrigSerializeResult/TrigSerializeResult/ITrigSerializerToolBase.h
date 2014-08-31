/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGSERIALIZERTOOLBASE_H
#define ITRIGSERIALIZERTOOLBASE_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ClassID.h"
#include <vector>
#include <stdint.h>


/** @author Jiri.Masik@cern.ch  
    @class ITrigSerializerToolBase
    Tool interface for serializer
*/

/** Declaration of the interface ID ( interface id, major version, minor version) */
static const InterfaceID IID_ITrigSerializerToolBase("ITrigSerializerToolBase", 1 , 0);


class ITrigSerializer {
public:

  /** serializes an object of a class nameOfClass pointed to by instance
   *  and recursively also other objects which object may point to
   *  @return std::vector<uint32_t>
   */
  virtual void serialize(const std::string &nameOfClass, void* instance, std::vector<uint32_t> &v) = 0;

  /** serializes an object of a class nameOfClass pointed to by instance
   *  and recursively also other objects which object may point to
   *  @return std::vector<uint32_t>
   */
  virtual std::vector<uint32_t> serialize(const std::string &nameOfClass, void* instance) = 0;

  /** deserializes an object of a class nameOfClass (and recursively other objects) found in 
   *  std::vector<uint32_t>& v
   *  @return pointer to a deserialized object
   */
  virtual void* deserialize(const std::string &nameOfClass, std::vector<uint32_t>& v) = 0;

  /** initialize framework to be ready to serialize a given class
   *  method to be called to prevent delayed initializations on the 
   *  first event
   *  @return StatusCode
   */
  virtual StatusCode initClass(const std::string &nameOfClass) const = 0;

  /** 
   * clean internal serializer state. To be used between
   * (de)serialization of unrelated data (for example of a new event)
   */

  virtual void reset()=0;

  /** access clid of the payload
      virtual CLID getCLID() const = 0;
   */

  /** set clid of the persistent class to be stored in data
   */
  virtual void setCLID(const uint32_t *guid) = 0;

  /** access clid of the payload (stored in BS) 
   */
  virtual StatusCode peekCLID(const std::vector<uint32_t>& v, uint32_t *guid) const = 0;

  /**
   *
   */
  virtual ~ITrigSerializer(){;}

};





/**
   @class ITrigSerializerToolBase
   AlgTool interface of a serializer implementation
*/

class ITrigSerializerToolBase : virtual public ITrigSerializer, virtual public IAlgTool

{
 public:

   // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrigSerializerToolBase; }

  //  virtual ~ITrigSerializerToolBase() {}



};

#endif

