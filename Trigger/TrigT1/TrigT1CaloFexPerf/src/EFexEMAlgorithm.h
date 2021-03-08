// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * Name    : EFexEMAlgorithm.h
 * PACKAGE : Trigger/TrigT1/TrigT1CaloFexPerf
 * AUTHOR  : Denis Oliveira Damazio
 * PURPOSE : emulate the eFex EM algorithm for phase 1 L1Calo
 */

#ifndef LVL1__EFEXEMALGORITHM
#define LVL1__EFEXEMALGORITHM

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloConstCellContainer.h"

#include "EFexEMEnergyWeightedClusterTool.h"
#include "EFexEMClusterTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODTrigCalo/TrigEMClusterContainer.h"

namespace LVL1 {

   class EFexEMAlgorithm : public AthAlgorithm
   {
   public :
      EFexEMAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~EFexEMAlgorithm();
      StatusCode initialize();
      StatusCode execute();

   private :

      /**
       * member functions
       */
      StatusCode getSuperCellsContainer(CaloConstCellContainer*& scells) const;

      StatusCode getTriggerTowerContainer(const xAOD::TriggerTowerContainer*& TTs) const;

      StatusCode getTileCalCellsContainer(const CaloConstCellContainer*& tileCellCon) const;

      /** retrieve the cell ID helper */
      StatusCode getIDHelper(const CaloCell_SuperCell_ID*& m_idHelper) const;

      /** retrieve the tile ID helper */
      StatusCode getTileIDHelper(const TileID*& m_idHelper) const;

      /**
       * member variables
       */
      ToolHandle<LVL1::EFexEMEnergyWeightedClusterTool> m_eFexTool;

      ToolHandle<LVL1::EFexEMClusterTool> m_eFexAlgTool;

      /**
       * input / output
       */
      SG::ReadHandleKey<CaloCellContainer>            m_inputCellContainerKey;  ///< LAr SuperCell input container
      SG::ReadHandleKey<CaloCellContainer>            m_inputTileCellContainerKey;  ///< Tile cell input container
      SG::ReadHandleKey<xAOD::TriggerTowerContainer>  m_inputTriggerTowerContainerKey;  ///< TriggerTowers (if needed)

      SG::WriteHandleKey<xAOD::TrigEMClusterContainer>  m_outputClusterContainerKey;     ///< Cell signal weights in clusters key

      std::string m_outputClusterName; ///< name of cluster output collection

      /**
       * properties
       */
      bool m_use_tileCells; ///< boolean for using Tile cells instead of Tile TT
      bool m_energyWeightedCluster; ///< clustering method - choose between default and energy weigthed
      bool m_apply_BaseLineCuts;     ///< applying the baseline cuts for default clustering
      float m_deta; ///< deta for the cluster definition
      float m_dphi; ///< dphi for the cluster definition
      bool m_useProvenanceSkim; ///< clear up container from bad BC by making a new container (Denis, old way)
      int m_qualBitMask; ///< Configurable quality bitmask

      float m_timeThr; ///< cut for time measurement
      float m_seedE; ///< cut for seed energy in MeV
      float m_deta_cellFormation; ///< deta for the cluster definition
      float m_dphi_cellFormation; ///< dphi for the cluster definition
      float m_deta_clusterFormation_2; ///< different deta for some of the shower shapes
      float m_dphi_clusterFormation_2; ///< different dphi for some of the shower shapes
      float m_clusterE_EMB2_EMEC2; ///< minimum cluster energy of SCs in EMB2 or EMEC2
      float m_eta_limit; ///< limit for eta of cluster
   };
}
#endif
