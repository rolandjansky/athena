/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERTIMETOOL_H
#define CALOCLUSTERTIMETOOL_H
/********************************************************************

NAME:     CaloClusterTimeTool.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  D. Hu      
CREATED:  Jan 2012   

PURPOSE:  correct time for EM clusters
          
********************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloClusterTimeTool.h"

#include "CaloCondBlobObjs/CaloCondBlobFlt.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

class CaloClusterTimeTool : public AthAlgTool, virtual public ICaloClusterTimeTool 
{
public:

  enum CaloClusterTimeFlags{

        RUN_NO_CORRECTION_FAILED=0,
        RUN_NO_CORRECTION_ZERO=1,
        RUN_NO_CORRECTION_LOW_STATS=2,

        DB_ALL_MISSING=3,
        DB_RUN_MISSING=4,

        FEB_CORRECTION_FAILED=5,

        CHANNEL_CORRECTION_FAILED=10,

        ENERGY_CORRECTION_FAILED=15,
        ENERGY_OUT_OF_RANGE=16,

        PV_CORRECTION_FAILED=20,
        PV_INFO_MISSING=21,

        CLUS_INFO_MISSING = 23,
        CLUS_GOODMIDCELL_MISSING = 24,
        CELL_INFO_MISSING = 25,
        CELL_NO_EM = 26,
        CELL_FTONID_INVALID=27,

        HIGH_GAIN_SET = 29,
        MEDIUM_GAIN_SET =30,
        LOW_GAIN_SET = 31

  };

  // constructor 
  CaloClusterTimeTool (const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~CaloClusterTimeTool();

  virtual StatusCode initialize();

  virtual void makeClusterTimeCorrection(float pv_z, const xAOD::CaloCluster* cluster,float& time_corr, float& error, unsigned int& flag  );
  virtual void makeCellTimeCorrection(float pv_z, const CaloCell* cell, float& time_corr, float& error, unsigned int& flag); 




#define IOVSVC_CALLBACK_ARGS_K(K) int&,std::list<std::string>& K



  //StatusCode updateFebOffsetMap(IOVSVC_CALLBACK_ARGS);
  //StatusCode updateRunOffsetMap(IOVSVC_CALLBACK_ARGS);
  // Ewan edit - added these two
  StatusCode updateFebOffsetMap(IOVSVC_CALLBACK_ARGS_K(keys));
  StatusCode updateRunOffsetMap(IOVSVC_CALLBACK_ARGS_K(keys));




  float energyCorr(float eneOff[], int num, float energy);

  float pvCorr(float cellX, float cellY, float cellZ, float pvZ);

  float errorCompute(float errOff[], int num, float energy);

private:

  const LArOnlineID* m_LArOnlineIDHelper;
  const CaloCell_ID* m_calo_id;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  const DataHandle<CondAttrListCollection>  m_febOffsetColl;
  const DataHandle<CondAttrListCollection>  m_runOffsetColl;
  std::map<unsigned int, const CaloCondBlobFlt*> m_febOffsetBlobMap;
  const CaloCondBlobFlt* m_runOffsetBlob; 
  std::map<unsigned int, const CaloCondBlobFlt*>::const_iterator m_lastItFeb;
  std::string m_keyFebOffset; 
  std::string m_keyRunOffset; 

};

#endif

