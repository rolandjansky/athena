/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef LARHVMAPTOOL_H
#define LARHVMAPTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "LArCabling/LArHVCablingTool.h"
#include "CxxUtils/checker_macros.h"

class CaloDetDescrManager;
class Identifier;
class CaloIdManager;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
//class LArHVCablingTool;

static const InterfaceID IID_LArHVMapTool("LArHVMapTool", 1 ,0);

class ATLAS_NOT_THREAD_SAFE LArHVMapTool: public AthAlgTool
{

     public:
          LArHVMapTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

          virtual ~LArHVMapTool(){}

          static const InterfaceID& interfaceID() { return IID_LArHVMapTool; }

          virtual StatusCode initialize();
          virtual StatusCode execute();
          virtual StatusCode finalize(){return StatusCode::SUCCESS;}


          std::vector<int> GetHVLines ATLAS_NOT_THREAD_SAFE(const Identifier& id);

     private:

          const DataHandle<CaloIdManager> m_caloIdMgr;
          const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
          const LArEM_ID* m_larem_id;
          const LArHEC_ID* m_larhec_id;
          const LArFCAL_ID* m_larfcal_id;

          ToolHandle<LArHVCablingTool> m_hvCablingTool;
};

#endif
