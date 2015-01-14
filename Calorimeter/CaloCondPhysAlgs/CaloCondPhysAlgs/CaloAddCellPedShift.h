/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloAddCellPedShift.h
//

#ifndef _CaloCondPhysAlgs_CaloAddCellPedShift_H
#define _CaloCondPhysAlgs_CaloAddCellPedShift_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/DataHandle.h"  
#include "AthenaKernel/IOVSvcDefs.h"
#include "CaloCondBlobObjs/ICaloCoolIdTool.h"
#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

class CaloCondBlobFlt;
class CondAttrListCollection;
class LArCablingService ;


class CaloAddCellPedShift : public AthAlgorithm {

  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloAddCellPedShift(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CaloAddCellPedShift();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    /** standard Athena-Algorithm method */
    StatusCode          stop();
    
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  std::string m_fname;
  std::string m_folderName;

  ITHistSvc* m_thistSvc;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  ToolHandle<LArCablingService> m_cablingService;
  const CaloCell_ID*       m_calo_id;
  const LArOnlineID*      m_onlineID;

  virtual StatusCode updateMap(IOVSVC_CALLBACK_ARGS);
  //=== blob storage
  const DataHandle<CondAttrListCollection> m_noiseAttrListColl;
  std::map<unsigned int, const CaloCondBlobFlt*> m_noiseBlobMap;

  ToolHandle<ICaloCoolIdTool> m_caloCoolIdTool;

  int m_iCool;
  int m_SubHash;
  int m_Hash;
  int m_OffId;
  float m_eta;
  float m_phi;
  int m_layer;
  int m_Gain;
  int   m_bec;
  int   m_posneg;
  int   m_FT;
  int   m_slot;
  int   m_channel;
  float m_ped1;
  float m_ped1corr;
  float m_ped2;

  TTree* m_tree;
  

};
#endif
