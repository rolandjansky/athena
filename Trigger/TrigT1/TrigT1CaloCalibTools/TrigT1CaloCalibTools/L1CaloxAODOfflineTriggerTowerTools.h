/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *


#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOXAODOFFLINETRIGERTOWERTOOLS_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOXAODOFFLINETRIGERTOWERTOOLS_H_

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ToolHandle.h"

// STL include(s):
#include <vector>

// xAOD include(s):
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

// Calo include(s):
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

// ID include(s):
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"

// Database includes
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Tool include(s):
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloFcal23Cells2RxMappingTool.h"

// Local include(s):
#include "TrigT1CaloCalibToolInterfaces/IL1CaloxAODOfflineTriggerTowerTools.h"

namespace LVL1{

  class L1CaloxAODOfflineTriggerTowerTools final : public virtual IL1CaloxAODOfflineTriggerTowerTools,
                                                   public asg::AsgTool
  {
    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS( L1CaloxAODOfflineTriggerTowerTools , LVL1::IL1CaloxAODOfflineTriggerTowerTools ) 

    public:
      /// constructor
      L1CaloxAODOfflineTriggerTowerTools( const std::string& name );
      /// destructor
      virtual ~L1CaloxAODOfflineTriggerTowerTools(){}
      /// delete the big 4
      L1CaloxAODOfflineTriggerTowerTools() = delete;
      L1CaloxAODOfflineTriggerTowerTools(const L1CaloxAODOfflineTriggerTowerTools& rhs) = delete;
      L1CaloxAODOfflineTriggerTowerTools(L1CaloxAODOfflineTriggerTowerTools&& rhs) = delete;
      L1CaloxAODOfflineTriggerTowerTools& operator=(const L1CaloxAODOfflineTriggerTowerTools& rhs) = delete;
      
      StatusCode initialize();
      StatusCode finalize();
      
      /// Calo Cells into maps for L1Calo use
      StatusCode                                 initCaloCells(); 

      std::vector<L1CaloRxCoolChannelId>         receivers( const xAOD::TriggerTower& tt ) const;
      std::vector<unsigned int>                  receiversId( const xAOD::TriggerTower& tt ) const;
      unsigned int                               isTile( const xAOD::TriggerTower& tt ) const;
      
      std::vector<const CaloCell*>               getCaloCells( const xAOD::TriggerTower& tt ) const;
      unsigned int                               nCaloCells( const xAOD::TriggerTower& tt ) const;
      std::vector<unsigned int>                  nCaloCellsByLayer( const xAOD::TriggerTower& tt ) const;
//       std::vector<unsigned int>                  nCaloCellsByReceiver( const xAOD::TriggerTower& tt ) const; 
//       std::vector<unsigned int>                  nCaloCellsByReceiverByLayer( const xAOD::TriggerTower& tt ) const; 

      std::vector<std::vector<const CaloCell*>>  caloCellsByReceiver( const xAOD::TriggerTower& tt ) const;
      std::vector<std::vector<std::vector<const CaloCell*>>>            caloCellsByLayerByReceiver( const xAOD::TriggerTower& tt ) const;

      float                                      caloCellsEnergy( const xAOD::TriggerTower& tt ) const;
      float                                      caloCellsET( const xAOD::TriggerTower& tt ) const;
      std::vector<float>                         caloCellsEnergyByLayer( const xAOD::TriggerTower& tt ) const;
      std::vector<float>                         caloCellsETByLayer( const xAOD::TriggerTower& tt ) const;
      std::vector<float>                         caloCellsEnergyByReceiver( const xAOD::TriggerTower& tt ) const;
      std::vector<float>                         caloCellsETByReceiver( const xAOD::TriggerTower& tt ) const;
      std::vector<std::vector<float>>            caloCellsEnergyByLayerByReceiver( const xAOD::TriggerTower& tt ) const;
      std::vector<std::vector<float>>            caloCellsETByLayerByReceiver( const xAOD::TriggerTower& tt ) const;
      
//       unsigned int                               badCaloCells( const xAOD::TriggerTower& tt ) const;
      float                                      caloCellsQuality( const xAOD::TriggerTower& tt ) const;
//       float                                      nCaloCellsNonNominal( const xAOD::TriggerTower& tt ) const;
//       std::vector<float>                         nCaloCellsNonNominalByLayer( const xAOD::TriggerTower& tt ) const;
//       std::vector<std::vector<float>>            nCaloCellsNonNominalByReceiverByLayer( const xAOD::TriggerTower& tt ) const;
//       float                                      nonNominalMeanScale( const xAOD::TriggerTower& tt ) const;
//       std::vector<float>                         nonNominalMeanScaleByLayer( const xAOD::TriggerTower& tt ) const;
//       std::vector<float>                         nonNominalMeanScaleByReceiver( const xAOD::TriggerTower& tt ) const;
//       std::vector<std::vector<float>>            nonNominalMeanScaleByReceiverByLayer( const xAOD::TriggerTower& tt ) const;  
      
      // Database access
      unsigned int                               ModuleId( const xAOD::TriggerTower& tt ) const;
      unsigned int                               ErrorCode( const xAOD::TriggerTower& tt ) const;
      unsigned long long                         PprDacScanResultsTimeStamp( const xAOD::TriggerTower& tt ) const;
      unsigned long long                         PprPedestalRunResultsTimeStamp( const xAOD::TriggerTower& tt ) const;
      unsigned long long                         PprNoiseRunResultsTimeStamp( const xAOD::TriggerTower& tt ) const;
      unsigned long long                         PprTimingResultsTimeStamp( const xAOD::TriggerTower& tt ) const;
      unsigned long long                         PprSatBcidResultsTimeStamp( const xAOD::TriggerTower& tt ) const;
      unsigned long long                         PprFirFilterResultsTimeStamp( const xAOD::TriggerTower& tt ) const;
      unsigned long long                         PprLutValuesResultsTimeStamp( const xAOD::TriggerTower& tt ) const;
      double                                     DacOffset( const xAOD::TriggerTower& tt ) const;
      double                                     DacSlope( const xAOD::TriggerTower& tt ) const;
      double                                     PedMean( const xAOD::TriggerTower& tt ) const;
      unsigned int                               PedValue( const xAOD::TriggerTower& tt ) const;
      unsigned int                               FullDelayData( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         SyncDelayBcid( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         InBcidNegedge( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         ExtBcidThreshold( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         SatBcidThreshLow( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         SatBcidThreshHigh( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         SatBcidLevel( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         BcidEnergyRangeLow( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         BcidEnergyRangeHigh( const xAOD::TriggerTower& tt ) const;
      unsigned short int                         FirStartBit( const xAOD::TriggerTower& tt ) const;
      short                                      FirCoeff1( const xAOD::TriggerTower& tt ) const;
      short                                      FirCoeff2( const xAOD::TriggerTower& tt ) const;
      short                                      FirCoeff3( const xAOD::TriggerTower& tt ) const;
      short                                      FirCoeff4( const xAOD::TriggerTower& tt ) const;
      short                                      FirCoeff5( const xAOD::TriggerTower& tt ) const;
      unsigned short                             LutStrategy( const xAOD::TriggerTower& tt ) const;
      unsigned short                             LutOffset( const xAOD::TriggerTower& tt ) const;
      unsigned short                             LutNoiseCut( const xAOD::TriggerTower& tt ) const;
      unsigned short                             LutSlope( const xAOD::TriggerTower& tt ) const;

      unsigned int                               DeadChannel( const xAOD::TriggerTower& tt ) const;
      unsigned int                               DisabledTower( const xAOD::TriggerTower& tt ) const;

      float                                      RxGain( const xAOD::TriggerTower& tt ) const;
      unsigned int                               RxStatus( const xAOD::TriggerTower& tt ) const;  
      
    private:
      Identifier                                 towerID( const xAOD::TriggerTower& tt ) const;
      int                                        pos_neg_z( const xAOD::TriggerTower& tt ) const;
      int                                        region( const xAOD::TriggerTower& tt ) const;
      int                                        ieta( const xAOD::TriggerTower& tt ) const;
      int                                        iphi( const xAOD::TriggerTower& tt ) const; 

      //  Database Attributes
      const coral::AttributeList*                DbAttributes( const xAOD::TriggerTower& tt , const CondAttrListCollection* dbAttrList ) const;   
      std::vector<const coral::AttributeList*>   DbRxGainsAttributes( const xAOD::TriggerTower& tt , const CondAttrListCollection* dbAttrList ) const;  
      
      // Sort Calo Cells
      std::vector<std::vector<const CaloCell*>>  sortEMCrackCells(const std::vector<const CaloCell*> &cells) const;
      std::vector<std::vector<const CaloCell*>>  sortFCAL23Cells(const std::vector<const CaloCell*> &cells,const std::vector<L1CaloRxCoolChannelId>& rx) const;
      
      // Cell quality
      float                                      LArCaloQuality(const std::vector<const CaloCell*> &cells) const;
      float                                      TileCaloQuality(const std::vector<const CaloCell*> &cells) const;      
      
      /// Tool to do the Calo cell summing
      ToolHandle<LVL1::IL1CaloCells2TriggerTowers>          m_cells2tt;      
      /// Tool to get the Identifier of a TriggerTower
      ToolHandle<LVL1::IL1CaloTTIdTools>                    m_l1CaloTTIdTools;
      // FCAL 23 Mapping
      ToolHandle<LVL1::IL1CaloFcal23Cells2RxMappingTool>    m_rxMapTool;      
      /// Helper class for offline TT identifiers
      const CaloIdManager*                         m_caloMgr;
      const CaloLVL1_ID*                           m_lvl1Helper;
      CaloTriggerTowerService*                     m_ttSvc;
      
      /// StoreGate keys for the Calo Cells 
      SG::ReadHandleKey<CaloCellContainer> m_caloCellContainerKey
      { this, "CaloCellContainerKey", "AllCalo" };
      
      /// Database
      const CondAttrListCollection* m_dbPpmChanCalib;
      
  };
}
#endif

