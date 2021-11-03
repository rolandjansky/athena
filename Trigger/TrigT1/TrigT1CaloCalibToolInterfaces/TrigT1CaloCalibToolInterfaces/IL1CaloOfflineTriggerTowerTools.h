/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

//
// Pure abstract base class interface to L1CaloOfflineTriggerTowerTools
//

#ifndef TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOOFFLINETRIGGERTOWERTOOLS_H_
#define TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOOFFLINETRIGGERTOWERTOOLS_H_

// Framework include(s):
#include "AsgTools/IAsgTool.h"

#include <vector>
#include <map>

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"

class CaloCell;
class CaloCellContainer;
class LArDigitContainer;
class TileDigitsContainer;
class Identifier;
class TileTTL1Cell;
class CondAttrListCollection;
class L1CaloRxCoolChannelId;

namespace coral{
  class AttributeList;
}

namespace LVL1{
  
  class IL1CaloOfflineTriggerTowerTools : virtual public asg::IAsgTool{
        
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( LVL1::IL1CaloOfflineTriggerTowerTools )  
    
    public:
      typedef std::map<Identifier, const TileTTL1Cell*> IdTTL1CellMapType;

      //   Location, ID and Navigation Information
      virtual int                                        emPpmCrate(const TriggerTower* tt) const = 0;
      virtual int                                        emPpmModule(const TriggerTower* tt) const = 0;
      virtual int                                        emPpmSubmodule(const TriggerTower* tt) const = 0;
      virtual int                                        emPpmChannel(const TriggerTower* tt) const = 0;
      virtual unsigned int                               emCoolChannelId(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           emLocation(const TriggerTower* tt) const = 0;
      virtual std::vector<unsigned int>                  emRxId(const TriggerTower* tt) const = 0;

      virtual int                                        hadPpmCrate(const TriggerTower* tt) const = 0;
      virtual int                                        hadPpmModule(const TriggerTower* tt) const = 0;
      virtual int                                        hadPpmSubmodule(const TriggerTower* tt) const = 0;
      virtual int                                        hadPpmChannel(const TriggerTower* tt) const = 0;
      virtual unsigned int                               hadCoolChannelId(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           hadLocation(const TriggerTower* tt) const = 0;
      virtual std::vector<unsigned int>                  hadRxId(const TriggerTower* tt) const = 0;
      virtual int                                        hadIsTile(const TriggerTower* tt) const = 0;

      //  Energy, Et and Layer Information
      virtual std::vector<const CaloCell*>               emCells(const TriggerTower* tt) const = 0;
      virtual int                                        emNCells(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           emNCellsByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           emNCellsByReceiver(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<int> >             emNCellsByReceiverByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           emTTCellsLayerNames(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<int> >             emTTCellsLayerNamesByReceiver(const TriggerTower* tt) const = 0;
      virtual float                                      emTTCellsEnergy(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         emTTCellsEnergyByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         emTTCellsEnergyByReceiver(const TriggerTower* tt,const int mode=0) const = 0;    
      virtual float                                      emTTCellsEt(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         emTTCellsEtByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         emTTCellsEtByReceiver(const TriggerTower* tt,const int mode=0) const = 0;

      virtual std::vector<const CaloCell*>               hadCells(const TriggerTower* tt) const = 0;
      virtual int                                        hadNCells(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           hadNCellsByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           hadNCellsByReceiver(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<int> >             hadNCellsByReceiverByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<int>                           hadTTCellsLayerNames(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<int> >             hadTTCellsLayerNamesByReceiver(const TriggerTower* tt) const = 0;
      virtual float                                      hadTTCellsEnergy(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         hadTTCellsEnergyByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         hadTTCellsEnergyByReceiver(const TriggerTower* tt,const int mode=0) const = 0;
      virtual float                                      hadTTCellsEt(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         hadTTCellsEtByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         hadTTCellsEtByReceiver(const TriggerTower* tt,const int mode=0) const = 0;
      virtual float                                      tileCellEnergy(const TriggerTower* tt,IdTTL1CellMapType& map) const = 0;

      //  High Voltage Information
      virtual float                                      emCaloQuality(const TriggerTower* tt) const = 0;
      virtual float                                      emNCellsNonNominal(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         emNCellsNonNominalByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<float> >           emNCellsNonNominalByReceiverByLayer(const TriggerTower* tt) const = 0;
      virtual float                                      emNonNominalMeanScale(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         emNonNominalMeanScaleByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         emNonNominalMeanScaleByReceiver(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<float> >           emNonNominalMeanScaleByReceiverByLayer(const TriggerTower* tt) const = 0;

      virtual float                                      hadCaloQuality(const TriggerTower* tt) const = 0;
      virtual float                                      hadNCellsNonNominal(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         hadNCellsNonNominalByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<float> >           hadNCellsNonNominalByReceiverByLayer(const TriggerTower* tt) const = 0;
      virtual float                                      hadNonNominalMeanScale(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         hadNonNominalMeanScaleByLayer(const TriggerTower* tt) const = 0;
      virtual std::vector<float>                         hadNonNominalMeanScaleByReceiver(const TriggerTower* tt) const = 0;
      virtual std::vector<std::vector<float> >           hadNonNominalMeanScaleByReceiverByLayer(const TriggerTower* tt) const = 0;

      //  Database Attributes
      virtual const coral::AttributeList*                emDbAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const = 0;
      virtual std::vector<const coral::AttributeList*>   emDbRxGainsAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const = 0;

      virtual const coral::AttributeList*                hadDbAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const = 0;
      virtual std::vector<const coral::AttributeList*>   hadDbRxGainsAttributes(const TriggerTower* tt,const CondAttrListCollection* dbAttrList) const = 0;


      // Database access
      virtual unsigned int                               ModuleId(const coral::AttributeList* attrList) const = 0;
      virtual unsigned int                               ErrorCode(const coral::AttributeList* attrList) const = 0;
      virtual unsigned long long                         PprDacScanResultsTimeStamp(const coral::AttributeList* attrList) const = 0;
      virtual unsigned long long                         PprPedestalRunResultsTimeStamp(const coral::AttributeList* attrList) const = 0;
      virtual unsigned long long                         PprNoiseRunResultsTimeStamp(const coral::AttributeList* attrList) const = 0;
      virtual unsigned long long                         PprTimingResultsTimeStamp(const coral::AttributeList* attrList) const = 0;
      virtual unsigned long long                         PprSatBcidResultsTimeStamp(const coral::AttributeList* attrList) const = 0;
      virtual unsigned long long                         PprFirFilterResultsTimeStamp(const coral::AttributeList* attrList) const = 0;
      virtual unsigned long long                         PprLutValuesResultsTimeStamp(const coral::AttributeList* attrList) const = 0;
      virtual double                                     DacOffset(const coral::AttributeList* attrList) const = 0;
      virtual double                                     DacSlope(const coral::AttributeList* attrList) const = 0;
      virtual double                                     PedMean(const coral::AttributeList* attrList) const = 0;
      virtual unsigned int                               PedValue(const coral::AttributeList* attrList) const = 0;
      virtual unsigned int                               FullDelayData(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         SyncDelayBcid(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         InBcidNegedge(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         ExtBcidThreshold(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         SatBcidThreshLow(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         SatBcidThreshHigh(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         SatBcidLevel(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         BcidEnergyRangeLow(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         BcidEnergyRangeHigh(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short int                         FirStartBit(const coral::AttributeList* attrList) const = 0;
      virtual short                                      FirCoeff1(const coral::AttributeList* attrList) const = 0;
      virtual short                                      FirCoeff2(const coral::AttributeList* attrList) const = 0;
      virtual short                                      FirCoeff3(const coral::AttributeList* attrList) const = 0;
      virtual short                                      FirCoeff4(const coral::AttributeList* attrList) const = 0;
      virtual short                                      FirCoeff5(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short                             LutStrategy(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short                             LutOffset(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short                             LutNoiseCut(const coral::AttributeList* attrList) const = 0;
      virtual unsigned short                             LutSlope(const coral::AttributeList* attrList) const = 0;

      virtual unsigned int                               DeadChannel(const coral::AttributeList* attrList) const = 0;
      virtual unsigned int                               DisabledTower(const coral::AttributeList* attrList) const = 0;

      virtual float                                      RxGain(const coral::AttributeList* attrList) const = 0;
      virtual unsigned int                               RxStatus(const coral::AttributeList* attrList) const = 0;


      virtual void                                       LArHV(const SG::ReadCondHandleKey<ILArHVScaleCorr>& scaleCorrKey,
                                                               const SG::ReadCondHandleKey<LArOnOffIdMapping>& cablingKey) = 0;
      virtual void                                       caloCells(const CaloCellContainer* cells) = 0;
      virtual Identifier                                 emID(const double eta,const double phi) const = 0;
      virtual Identifier                                 hadID(const double eta,const double phi) const = 0;


    protected:

      virtual int                                        pos_neg_z(const double eta) const = 0;
      virtual int                                        region(const double eta) const = 0;
      virtual int                                        ieta(const double eta) const = 0;
      virtual int                                        iphi(const double eta,const double phi) const = 0;
//       virtual Identifier                                 emID(const double eta,const double phi) const = 0;
//       virtual Identifier                                 hadID(const double eta,const double phi) const = 0;

      virtual std::vector<L1CaloRxCoolChannelId>         emReceivers(const TriggerTower* tt) const = 0;
      virtual std::vector<L1CaloRxCoolChannelId>         hadReceivers(const TriggerTower* tt) const = 0;

      virtual std::vector<std::vector<const CaloCell*> > sortFCAL23Cells(const std::vector<const CaloCell*> &cells,const double eta) const = 0;
      virtual std::vector<std::vector<const CaloCell*> > sortFCAL23Cells(const std::vector<const CaloCell*> &cells,const std::vector<unsigned int>& rxId) const = 0;

      virtual std::vector<std::vector<const CaloCell*> > sortEMCrackCells(const std::vector<const CaloCell*> &cells) const = 0;

      virtual float                                      LArNonNominalHV(const std::vector<const CaloCell*> &cells) const = 0;
      virtual float                                      LArNonNominalHV(const CaloCell* cell,
                                                                         const ILArHVScaleCorr* scaleCorr,
                                                                         const ILArHVScaleCorr* onlineScaleCorr,
                                                                         const LArOnOffIdMapping* cabling) const = 0;
      virtual float                                      LArHVScale(const  std::vector<const CaloCell*> &cells) const = 0;
      virtual float                                      LArHVScale(const CaloCell* cell,
                                                                    const ILArHVScaleCorr* scaleCorr,
                                                                    const ILArHVScaleCorr* onlineScaleCorr,
                                                                    const LArOnOffIdMapping* cabling) const = 0;
      virtual float                                      TileNonNominal(const  std::vector<const CaloCell*> &cells) const = 0;
      virtual float                                      TileNonNominal(const CaloCell* cell) const = 0;

      virtual float                                      LArCaloQuality(const std::vector<const CaloCell*> &cells) const = 0;
      virtual float                                      TileCaloQuality(const std::vector<const CaloCell*> &cells) const = 0;
  };  
} // end of namespace
#endif
