///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RoiB2TopoInputDataCnv.h 
// Header file for class RoiB2TopoInputDataCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef L1TOPOSIMULATION_LVL1_ROIB2TOPOINPUTDATACNV_H
#define L1TOPOSIMULATION_LVL1_ROIB2TOPOINPUTDATACNV_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1CaloEvent/EnergyTopoData.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"
#include "TrigT1CaloEvent/JetCMXTopoDataCollection.h"


namespace LVL1 {

   class L1TopoDataMaker;

   class RoiB2TopoInputDataCnv : public ::AthReentrantAlgorithm 
   { 
   public: 

      RoiB2TopoInputDataCnv( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~RoiB2TopoInputDataCnv(); 

      // Athena algorithm's Hooks
      virtual StatusCode  execute_r (const EventContext& ctx) const override;
      virtual StatusCode  initialize() override;
      //Finalize not overriden because no work is required

   private: 

      // Default constructor: 
      RoiB2TopoInputDataCnv();

      L1TopoDataMaker * m_datamaker { nullptr };

      SG::ReadHandleKey<ROIB::RoIBResult>             m_roibLocation;
      SG::WriteHandleKey<DataVector<CPCMXTopoData>>   m_emTauLocation;
      SG::WriteHandleKey<DataVector<JetCMXTopoData>>  m_jetLocation;
      SG::WriteHandleKey<EnergyTopoData>              m_energyLocation;

   }; 
} 
#endif
