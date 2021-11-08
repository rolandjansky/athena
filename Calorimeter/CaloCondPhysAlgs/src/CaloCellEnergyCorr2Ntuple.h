/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CaloCellEnergyCorr2Ntuple.h
//

#ifndef CALOCONDPHYSALGS_CALOCELLENERGYCORR2NTUPLE_H
#define CALOCONDPHYSALGS_CALOCELLENERGYCORR2NTUPLE_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "CxxUtils/checker_macros.h"

class AthenaAttributeList;
/* AthenaAttibuteList ctors and dtors are not thread
 * safe leading to certain methods here not being Thread safe
 */

class CaloCellEnergyCorr2Ntuple : public AthAlgorithm {

  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloCellEnergyCorr2Ntuple(const std::string& name, ISvcLocator* pSvcLocator) ATLAS_CTORDTOR_NOT_THREAD_SAFE;
    /** Default Destructor */
    ~CaloCellEnergyCorr2Ntuple() ATLAS_CTORDTOR_NOT_THREAD_SAFE;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode          initialize ATLAS_NOT_THREAD_SAFE() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          finalize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          stop ATLAS_NOT_THREAD_SAFE() override;
    
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  //=== blob storage
  ITHistSvc* m_thistSvc;

  const CaloCell_ID*       m_calo_id;

  std::string m_key;
  std::string  m_FolderName;
  const DataHandle<AthenaAttributeList> m_AttrListColl;
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  int m_Hash;
  int m_OffId;
  float m_eta;
  float m_phi;
  int m_layer;
  int m_detector;
  float m_corr;
  TTree* m_tree;

  int m_runNumber;
  int m_lumiBlock;

};
#endif
