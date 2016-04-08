/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTOOLS_CALONOISEDBEXAMPLE_H
#define CALOTOOLS_CALONOISEDBEXAMPLE_H

/***************************************************************************
 CaloNoiseDbExample
 -----------------------------------------
 Example algorithm to read back calo noise from COOL DB

 2008-01-14      nils.gollub@cern.ch
 ***************************************************************************/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/DataHandle.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//#include "CaloTools/CaloNoiseToolDB.h"
//#include "CaloInterface/ICaloNoiseTool.h"

#include "CaloIdentifier/CaloGain.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "CaloCondBlobObjs/CaloCondUtils.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "Identifier/Identifier.h"

#include <string>
#include <map>

class StoreGateSvc;
#include "xAODEventInfo/EventInfo.h"
class CaloCondBlobFlt;
class Identifier; 
class CaloIdManager;

class CaloNoiseDbExample: public AthAlgorithm 
{
 public:
  
  CaloNoiseDbExample (const std::string& name, ISvcLocator* pSvcLocator);
  ~CaloNoiseDbExample();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  StatusCode printSomeInfo();
  float getA(    CaloCondUtils::SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const;
  float getB(    CaloCondUtils::SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const;
  float getC(    CaloCondUtils::SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const;
  float getD(    CaloCondUtils::SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const;
  float getE(    CaloCondUtils::SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain) const;

  float getNoise(CaloCondUtils::SYSTEM sysId, unsigned int cellHash, CaloGain::CaloGain caloGain, 
		 float lumi) const;
  
  //=== callback function to update noise map
  virtual StatusCode updateMap(IOVSVC_CALLBACK_ARGS);
  
  //=== non-property members
  const xAOD::EventInfo*    m_evt;
  //
  const DataHandle<CaloIdManager> m_caloIdMgr;
  const CaloCell_ID*       m_calo_id;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  
  //=== blob storage
  const DataHandle<CondAttrListCollection>                m_noiseAttrListColl;
  std::map<CaloCondUtils::SYSTEM, const CaloCondBlobFlt*> m_noiseBlobMap;
  std::string m_foldername;
};

#endif 
