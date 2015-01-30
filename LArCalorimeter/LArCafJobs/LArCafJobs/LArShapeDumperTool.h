/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCafJobs_LArShapeDumperTool
#define LArCafJobs_LArShapeDumperTool

#include "LArCafJobs/ILArShapeDumperTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

class ILArShape;
class LArOnlineID;
class CaloDetDescrManager;

class LArShapeDumperTool : public AthAlgTool, public virtual ILArShapeDumperTool
{
 public:
 
  LArShapeDumperTool(const std::string& type, const std::string& name,
                     const IInterface* parent);
 
  virtual ~LArShapeDumperTool();

  StatusCode initialize();
  StatusCode finalize();
  
  virtual LArSamples::CellInfo* makeCellInfo(const HWIdentifier& channelID, const Identifier& id, const CaloDetDescrElement* caloDetElement = 0) const;
  virtual LArSamples::ShapeInfo* retrieveShape(const HWIdentifier& channelID, CaloGain::CaloGain gain) const;

  bool doShape() const { return m_doShape; }
  
 private:
  
  bool m_doShape, m_doAllShapes;
  std::string m_shapeKey;
   
  const CaloDetDescrManager* m_caloDetDescrMgr;
  const LArOnlineID* m_onlineHelper;
  const LArEM_ID* m_emId;
  const LArHEC_ID* m_hecId;
  const LArFCAL_ID* m_fcalId;
  const DataHandle<ILArShape> m_shape;
};
#endif
