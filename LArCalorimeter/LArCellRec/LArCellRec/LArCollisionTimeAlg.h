/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TheLArCollisionsAlg.h
//

#ifndef _LArCollisionTimeAlg_LArCollisionTimeAlg_H
#define _LArCollisionTimeAlg_LArCollisionTimeAlg_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloIdentifier/CaloIdManager.h"

class LArCollisionTimeAlg : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    LArCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~LArCollisionTimeAlg();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------


  int m_nevt;
  ToolHandle<ICaloNoiseTool>  m_noiseTool;
  bool m_isMC;
  bool m_iterCut;
  float m_timeCut;
  int m_minCells;
  std::string m_cellsContName;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const CaloCell_ID*       m_calo_id;

};
#endif
