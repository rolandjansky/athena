/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TheLArCollisionsAlg.h
//

#ifndef LARCELLREC_LARCOLLISIONTIMEALG_H
#define LARCELLREC_LARCOLLISIONTIMEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArRecEvent/LArCollisionTime.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloCell_ID;

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
  const CaloCell_ID*       m_calo_id;

  //---------------------------------------------------
  // Properties
  // --------------------------------------------------
  Gaudi::Property<bool>  m_isMC     { this, "isMC", false, "Are we working with simu?" };
  Gaudi::Property<bool>  m_iterCut  { this, "cutIteration", true, "cut on OFC iteration, will not work for Online" };
  Gaudi::Property<float> m_timeCut  { this, "timeDiffCut", 5., "|A-C| time < timeDiffCut to pass the filter" };
  Gaudi::Property<int>   m_minCells { this, "nCells", 2, "min. number of cells per endcap to pass the filter" };


  SG::ReadHandleKey<CaloCellContainer> m_cellsContName;
  SG::WriteHandleKey<LArCollisionTime> m_collTimeName;
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};

};
#endif
