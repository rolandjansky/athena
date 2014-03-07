/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCafJobs_ILArShapeDumperTool
#define LArCafJobs_ILArShapeDumperTool

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "CaloIdentifier/CaloGain.h"

class Identifier;
class HWIdentifier;
class CaloDetDescrElement;

namespace LArSamples {
  class CellInfo;
  class ShapeInfo;
  class EventData;
}

static const InterfaceID IID_ILArShapeDumperTool("ILArShapeDumperTool", 1, 0 );


class ILArShapeDumperTool : public virtual IAlgTool
{
 public:
 
  ~ILArShapeDumperTool() { };
 
  // retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILArShapeDumperTool; }

  virtual bool doShape() const = 0;

  virtual LArSamples::CellInfo* makeCellInfo(const HWIdentifier& channelID, const Identifier& id, const CaloDetDescrElement* caloDetElement = 0) const = 0;
  virtual LArSamples::ShapeInfo* retrieveShape(const HWIdentifier& channelID, CaloGain::CaloGain gain) const = 0;   
};
#endif
