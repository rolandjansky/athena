/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// \file ISerializable.h
/// \author Gianluca Comune <gianluca.comune@cern.ch>
/// a base class objects should inherit from to allow 
/// persistency of seed object in L2Result 

#ifndef ISerializable_H
#define ISerializable_H

//#include "TrigData/TriggerElement.h"
#include "GaudiKernel/StatusCode.h"
#include <string>
#include <vector>
#include <stdint.h>

///
/// \class ISerializable
/// \brief Class from which HLT seeds objects derive.
///

class ISerializable 
{
  
public:
  ISerializable();
  virtual ~ISerializable();

  virtual std::vector<uint32_t>& serialize() = 0;
  virtual StatusCode deserialize(std::vector<uint32_t>& v) = 0;

 // TriggerElement* getTE() const;
 // void setTE(TriggerElement* theTE);
  void setClid(uint32_t cl){m_clid=cl;};
  uint32_t getClid() const {return m_clid;};

private:
//  TriggerElement* m_te;
  uint32_t m_clid;
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ISerializable, 9193, 1)

#endif //ISERIALIZABLE_H
