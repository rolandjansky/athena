/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArShapeDumper
 * @author Nicolas.Berger@cern.ch
 *   */

#ifndef LArSimpleShapeDumper_H
#define LArSimpleShapeDumper_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "LArTools/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"

#include <string>

class MsgStream;
class StoreGateSvc;
class CaloDetDescrManager;
class ILArShape;
class HWIdentifier;
class Identifier;
class CaloDetDescrElement;

class LArSimpleShapeDumper : public AthAlgorithm
{
 public:
  LArSimpleShapeDumper(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArSimpleShapeDumper();

  //standart algorithm methods
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
 private:
   
  const LArOnlineID* m_onlineHelper;
  const DataHandle<ILArShape> m_shape;
};

#endif
