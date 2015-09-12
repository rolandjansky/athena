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
#include "AthenaBaseComps/AthAlgorithm.h"

namespace LVL1 {

   class L1TopoDataMaker;

   class RoiB2TopoInputDataCnv : public ::AthAlgorithm 
   { 
   public: 

      RoiB2TopoInputDataCnv( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~RoiB2TopoInputDataCnv(); 

      // Athena algorithm's Hooks
      virtual StatusCode  initialize();
      virtual StatusCode  execute();
      virtual StatusCode  finalize();

   private: 

      // Default constructor: 
      RoiB2TopoInputDataCnv();

      L1TopoDataMaker * datamaker { nullptr };

      StringProperty m_roibLocation;
      StringProperty m_emTauLocation;
      StringProperty m_jetLocation;
      StringProperty m_energyLocation;

   }; 
} 
#endif
