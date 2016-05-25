/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArTesting_H
#define LArTesting_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/AlgTool.h"

#include "StoreGate/StoreGateSvc.h"

class CaloDetDescrManager;
class CaloIdManager;

class LArOnlineID;
class LArElectrodeID;
class HWIdentifier;
class Identifier;
class LArOnlineIDStrHelper;
class LArCablingService;
class LArHVCablingTool;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;

class LArHVMapTool;

class LArHVLineID;

class LArTesting : public AthAlgorithm  {

 public:

   LArTesting(const std::string& name, ISvcLocator* pSvcLocator);
   ~LArTesting();

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode execute();

 private:

   //functions
   std::vector<int>* GetHVLines(const Identifier& id);
     

 private:

   /*Tools*/
   ToolHandle<LArCablingService> m_LArCablingService;
   ToolHandle<LArHVCablingTool> m_LArHVCablingTool;
   ToolHandle<LArHVMapTool> m_LArHVMapTool;

   /*services*/
   const LArOnlineID* m_LArOnlineIDHelper;
   const LArHVLineID* m_LArHVLineIDHelper;
   const LArElectrodeID* m_LArElectrodeIDHelper;
   const LArEM_ID* m_LArEM_IDHelper;
   const LArFCAL_ID* m_LArFCAL_IDHelper;
   const LArHEC_ID*  m_LArHEC_IDHelper;
   const CaloIdManager*  m_caloIdMgr;
   const CaloDetDescrManager* m_calodetdescrmgr;
};


#endif // LArTesting_H

