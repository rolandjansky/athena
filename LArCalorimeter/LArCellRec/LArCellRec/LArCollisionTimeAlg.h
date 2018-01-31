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
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArRecEvent/LArCollisionTime.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class LArCollisionTimeAlg : public AthAlgorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    LArCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    virtual ~LArCollisionTimeAlg();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode          initialize() override final;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute() override final;
    /** standard Athena-Algorithm method */
    virtual StatusCode          finalize() override final;

  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------

  int m_nevt;
  const CaloCell_ID*       m_calo_id;
  const DataHandle<CaloIdManager> m_caloIdMgr;

  //---------------------------------------------------
  // Properties
  // --------------------------------------------------
  Gaudi::Property<bool>  m_isMC     { this, "isMC", false, "Are we working with simu?" };
  Gaudi::Property<bool>  m_iterCut  { this, "cutIteration", true, "cut on OFC iteration, will not work for Online" };
  Gaudi::Property<float> m_timeCut  { this, "timeDiffCut", 5., "|A-C| time < timeDiffCut to pass the filter" };
  Gaudi::Property<int>   m_minCells { this, "nCells", 2, "min. number of cells per endcap to pass the filter" };

  ToolHandle<ICaloNoiseTool>  m_noiseTool;

  SG::ReadHandleKey<CaloCellContainer> m_cellsContName;
  SG::WriteHandleKey<LArCollisionTime> m_collTimeName;

};
#endif
