/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloCellPosition2Ntuple.h
//

#ifndef _CaloCondPhysAlgs_CaloCellPosition2Ntuple_H
#define _CaloCondPhysAlgs_CaloCellPosition2Ntuple_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

class CaloIdManager;
class CaloDetDescrManager;
class CaloCell_ID;
namespace CaloRec {
  class CaloCellPositionShift;
}

class CaloCellPosition2Ntuple : public AthAlgorithm {

  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloCellPosition2Ntuple(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CaloCellPosition2Ntuple();
    
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

  const DataHandle<CaloRec::CaloCellPositionShift> m_cellPos;
  std::string m_key;

  int m_Hash;
  int m_OffId;
  float m_eta;
  float m_phi;
  int m_layer;
  float m_dx;
  float m_dy;
  float m_dz;
  float m_volume;
  TTree* m_tree;

};
#endif
