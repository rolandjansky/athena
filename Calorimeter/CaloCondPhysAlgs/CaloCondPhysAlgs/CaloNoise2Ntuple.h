/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloNoise2Ntuple.h
//

#ifndef _CaloCondPhysAlgs_CaloNoise2Ntuple_H
#define _CaloCondPhysAlgs_CaloNoise2Ntuple_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloInterface/ICaloMBAverageTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

class CaloNoise2Ntuple : public AthAlgorithm {

  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloNoise2Ntuple(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CaloNoise2Ntuple();
    
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
  ITHistSvc* m_thistSvc;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const CaloCell_ID*       m_calo_id;

  ToolHandle<ICaloNoiseTool> m_noiseTool;
  ToolHandle<ICaloMBAverageTool> m_averageTool;

  int m_iCool;
  int m_SubHash;
  int m_Hash;
  int m_OffId;
  float m_eta;
  float m_phi;
  int m_layer;
  int m_Gain;
  float m_noise;
  float m_elecNoise;
  float m_pileupNoise; 
  float m_average;
  TTree* m_tree;

  int m_runNumber;
  int m_lumiBlock;

};
#endif
