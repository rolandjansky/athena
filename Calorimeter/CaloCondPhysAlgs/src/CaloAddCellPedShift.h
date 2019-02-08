/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CaloAddCellPedShift.h
//

#ifndef CALOCONDPHYSALGS_CALOADDCELLPEDSHIFT_H
#define CALOCONDPHYSALGS_CALOADDCELLPEDSHIFT_H

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
#include "LArCabling/LArOnOffIdMapping.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

class CaloCondBlobFlt;
class CondAttrListCollection;


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

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
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
