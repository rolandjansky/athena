/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArShapeDumper
 * @author Nicolas.Berger@cern.ch
 *   */

#ifndef LARCAFJOBS_LARSIMPLESHAPEDUMPER_H
#define LARCAFJOBS_LARSIMPLESHAPEDUMPER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArIdentifier/LArOnlineID.h"

#include <string>

class MsgStream;
class StoreGateSvc;
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
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
 private:
   
  const LArOnlineID* m_onlineHelper;
  const DataHandle<ILArShape> m_shape;
};

#endif
