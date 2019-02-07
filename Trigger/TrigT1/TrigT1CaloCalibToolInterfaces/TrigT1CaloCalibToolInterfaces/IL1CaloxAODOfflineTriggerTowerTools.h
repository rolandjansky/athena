/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOXAODOFFLINETRIGGERTOWERTOOLS_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOXAODOFFLINETRIGGERTOWERTOOLS_H_

// Framework include(s):
#include "AsgTools/IAsgTool.h"

#include <vector>
#include <map>

class CaloCell;
class CaloCellContainer;
class LArDigitContainer;
class TileDigitsContainer;
class Identifier;
class TileTTL1Cell;
class CondAttrListCollection;
class ILArHVCorrTool;
class L1CaloRxCoolChannelId;

namespace coral{
  class AttributeList;
}

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

namespace LVL1{

  class IL1CaloxAODOfflineTriggerTowerTools : virtual public asg::IAsgTool{
    
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( LVL1::IL1CaloxAODOfflineTriggerTowerTools ) 
    
    public:
      
      /// Calo Cells into maps for L1Calo use
      virtual StatusCode                                 initCaloCells() = 0;

      virtual std::vector<L1CaloRxCoolChannelId>         receivers( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<unsigned int>                  receiversId( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned int                               isTile( const xAOD::TriggerTower& tt ) const = 0;
      
      virtual std::vector<const CaloCell*>               getCaloCells( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned int                               nCaloCells( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<unsigned int>                  nCaloCellsByLayer( const xAOD::TriggerTower& tt ) const = 0;
//       virtual std::vector<unsigned int>                  nCaloCellsByReceiver( const xAOD::TriggerTower& tt ) const = 0; 
//       virtual std::vector<unsigned int>                  nCaloCellsByReceiverByLayer( const xAOD::TriggerTower& tt ) const = 0; 

      virtual std::vector<std::vector<const CaloCell*>>  caloCellsByReceiver( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<std::vector<std::vector<const CaloCell*>>>            caloCellsByLayerByReceiver( const xAOD::TriggerTower& tt ) const = 0;

      virtual float                                      caloCellsEnergy( const xAOD::TriggerTower& tt ) const = 0;
      virtual float                                      caloCellsET( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<float>                         caloCellsEnergyByLayer( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<float>                         caloCellsETByLayer( const xAOD::TriggerTower& tt ) const = 0;      
      
      virtual std::vector<float>                         caloCellsEnergyByReceiver( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<float>                         caloCellsETByReceiver( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<std::vector<float>>            caloCellsEnergyByLayerByReceiver( const xAOD::TriggerTower& tt ) const = 0;
      virtual std::vector<std::vector<float>>            caloCellsETByLayerByReceiver( const xAOD::TriggerTower& tt ) const = 0;
      
//       virtual unsigned int                               badCaloCells( const xAOD::TriggerTower& tt ) const = 0;
      virtual float                                      caloCellsQuality( const xAOD::TriggerTower& tt ) const = 0;
//       virtual float                                      nCaloCellsNonNominal( const xAOD::TriggerTower& tt ) const = 0;
//       virtual std::vector<float>                         nCaloCellsNonNominalByLayer( const xAOD::TriggerTower& tt ) const = 0;
//       virtual std::vector<std::vector<float>>            nCaloCellsNonNominalByReceiverByLayer( const xAOD::TriggerTower& tt ) const = 0;
//       virtual float                                      nonNominalMeanScale( const xAOD::TriggerTower& tt ) const = 0;
//       virtual std::vector<float>                         nonNominalMeanScaleByLayer( const xAOD::TriggerTower& tt ) const = 0;
//       virtual std::vector<float>                         nonNominalMeanScaleByReceiver( const xAOD::TriggerTower& tt ) const = 0;
//       virtual std::vector<std::vector<float>>            nonNominalMeanScaleByReceiverByLayer( const xAOD::TriggerTower& tt ) const = 0;   
      
      // Database access
      virtual unsigned int                               ModuleId( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned int                               ErrorCode( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned long long                         PprDacScanResultsTimeStamp( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned long long                         PprPedestalRunResultsTimeStamp( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned long long                         PprNoiseRunResultsTimeStamp( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned long long                         PprTimingResultsTimeStamp( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned long long                         PprSatBcidResultsTimeStamp( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned long long                         PprFirFilterResultsTimeStamp( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned long long                         PprLutValuesResultsTimeStamp( const xAOD::TriggerTower& tt ) const = 0;
      virtual double                                     DacOffset( const xAOD::TriggerTower& tt ) const = 0;
      virtual double                                     DacSlope( const xAOD::TriggerTower& tt ) const = 0;
      virtual double                                     PedMean( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned int                               PedValue( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned int                               FullDelayData( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         SyncDelayBcid( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         InBcidNegedge( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         ExtBcidThreshold( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         SatBcidThreshLow( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         SatBcidThreshHigh( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         SatBcidLevel( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         BcidEnergyRangeLow( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         BcidEnergyRangeHigh( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short int                         FirStartBit( const xAOD::TriggerTower& tt ) const = 0;
      virtual short                                      FirCoeff1( const xAOD::TriggerTower& tt ) const = 0;
      virtual short                                      FirCoeff2( const xAOD::TriggerTower& tt ) const = 0;
      virtual short                                      FirCoeff3( const xAOD::TriggerTower& tt ) const = 0;
      virtual short                                      FirCoeff4( const xAOD::TriggerTower& tt ) const = 0;
      virtual short                                      FirCoeff5( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short                             LutStrategy( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short                             LutOffset( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short                             LutNoiseCut( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned short                             LutSlope( const xAOD::TriggerTower& tt ) const = 0;

      virtual unsigned int                               DeadChannel( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned int                               DisabledTower( const xAOD::TriggerTower& tt ) const = 0;

      virtual float                                      RxGain( const xAOD::TriggerTower& tt ) const = 0;
      virtual unsigned int                               RxStatus( const xAOD::TriggerTower& tt ) const = 0;      

  };
}
#endif
