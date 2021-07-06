/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// TheLArCollisionsAlg.h
//

#ifndef LARCELLREC_LARCOLLISIONTIMEALG_H
#define LARCELLREC_LARCOLLISIONTIMEALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArRecEvent/LArCollisionTime.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloCell_ID;

class LArCollisionTimeAlg : public AthReentrantAlgorithm {
 public:
    using AthReentrantAlgorithm::AthReentrantAlgorithm;
  
    virtual ~LArCollisionTimeAlg() = default;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode          initialize() override final;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute(const EventContext& ctx) const override final;
    /** standard Athena-Algorithm method */
    virtual StatusCode          finalize() override final;

 private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  const CaloCell_ID*       m_calo_id=nullptr;

  //---------------------------------------------------
  // Properties
  // --------------------------------------------------
  Gaudi::Property<bool>  m_isMC     { this, "isMC", false, "Are we working with simu?" };
  Gaudi::Property<bool>  m_iterCut  { this, "cutIteration", false, "cut on OFC iteration, will not work for Online" };
  Gaudi::Property<float> m_timeCut  { this, "timeDiffCut", 5., "|A-C| time < timeDiffCut to pass the filter" };
  Gaudi::Property<int>   m_minCells { this, "nCells", 2, "min. number of cells per endcap to pass the filter" };


  SG::ReadHandleKey<CaloCellContainer> m_cellsContName{this,"cellContainerName","AllCalo"};
  SG::WriteHandleKey<LArCollisionTime> m_collTimeName{this,"collisionTime","LArCollisionTime"};
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};

};
#endif
