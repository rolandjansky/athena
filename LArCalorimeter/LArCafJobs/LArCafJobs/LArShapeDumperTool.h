/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCAFJOBS_LARSHAPEDUMPERTOOL_H
#define LARCAFJOBS_LARSHAPEDUMPERTOOL_H

#include "LArCafJobs/ILArShapeDumperTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

class ILArShape;
class LArOnlineID;

class LArShapeDumperTool : public AthAlgTool, public virtual ILArShapeDumperTool
{
 public:
 
  LArShapeDumperTool(const std::string& type, const std::string& name,
                     const IInterface* parent);
 
  virtual ~LArShapeDumperTool();

  StatusCode initialize() override;
  StatusCode finalize() override;
  
  virtual LArSamples::CellInfo* makeCellInfo(const HWIdentifier& channelID, const Identifier& id, const CaloDetDescrElement* caloDetElement = 0) const override;
  virtual LArSamples::ShapeInfo* retrieveShape(const HWIdentifier& channelID, CaloGain::CaloGain gain) const override;

  virtual bool doShape() const override { return m_doShape; }
  
 private:
  
  bool m_doShape, m_doAllShapes;
  std::string m_shapeKey;
   
  const LArOnlineID* m_onlineHelper{nullptr};
  const LArEM_ID* m_emId{nullptr};
  const LArHEC_ID* m_hecId{nullptr};
  const LArFCAL_ID* m_fcalId{nullptr};
  const DataHandle<ILArShape> m_shape;
};
#endif
