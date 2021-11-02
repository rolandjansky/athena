/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *


#ifndef TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOOFFLINETRIGGERTOWERTOOLS_H
#define TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOOFFLINETRIGGERTOWERTOOLS_H

#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Database includes
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"

// ID includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadCondHandleKey.h"

// L1 includes
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloCalibTools/L1CaloTTIdTools.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloFcal23Cells2RxMappingTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"

// Calo includes
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"

// LAr includes
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileTTL1CellContainer.h"
#include "TileEvent/TileCell.h"
#include "TileConditions/ITileBadChanTool.h"

// PABC interface include
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"

// STL includes
#include <vector>

namespace LVL1 {

class L1CaloOfflineTriggerTowerTools : virtual public IL1CaloOfflineTriggerTowerTools, public asg::AsgTool
{
      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( L1CaloOfflineTriggerTowerTools , LVL1::IL1CaloOfflineTriggerTowerTools )

public:
      /// constructor
      L1CaloOfflineTriggerTowerTools( const std::string& name );
      /// destructor
      virtual ~L1CaloOfflineTriggerTowerTools() {}
      /// delete the big 4
      L1CaloOfflineTriggerTowerTools() = delete;
      L1CaloOfflineTriggerTowerTools(const L1CaloOfflineTriggerTowerTools& rhs) = delete;
      L1CaloOfflineTriggerTowerTools(L1CaloOfflineTriggerTowerTools&& rhs) = delete;
      L1CaloOfflineTriggerTowerTools& operator=(const L1CaloOfflineTriggerTowerTools& rhs) = delete;

      typedef std::map<Identifier, const TileTTL1Cell*> IdTTL1CellMapType;
      typedef std::vector<const CaloCell*>::const_iterator Itr_vCaloCells;
      typedef std::vector<std::vector<const CaloCell*> >::const_iterator Itr_vvCaloCells;

      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;

      //   Location, ID and Navigation Information
      virtual int                                emPpmCrate(const TriggerTower* tt) const override;
      virtual int                                emPpmModule(const TriggerTower* tt) const override;
      virtual int                                emPpmSubmodule(const TriggerTower* tt) const override;
      virtual int                                emPpmChannel(const TriggerTower* tt) const override;
      virtual unsigned int                       emCoolChannelId(const TriggerTower* tt) const override;
      virtual std::vector<int>                   emLocation(const TriggerTower* tt) const override;
      virtual std::vector<unsigned int>          emRxId(const TriggerTower* tt) const override;

      virtual int                                hadPpmCrate(const TriggerTower* tt) const override;
      virtual int                                hadPpmModule(const TriggerTower* tt) const override;
      virtual int                                hadPpmSubmodule(const TriggerTower* tt) const override;
      virtual int                                hadPpmChannel(const TriggerTower* tt) const override;
      virtual unsigned int                       hadCoolChannelId(const TriggerTower* tt) const override;
      virtual std::vector<int>                   hadLocation(const TriggerTower* tt) const override;
      virtual std::vector<unsigned int>          hadRxId(const TriggerTower* tt) const override;
      virtual int                                hadIsTile(const TriggerTower* tt) const override;

      //  Energy, Et and Layer Information
      virtual std::vector<const CaloCell*>       emCells(const TriggerTower* tt) const override;
      virtual int                                emNCells(const TriggerTower* tt) const override;
      virtual std::vector<int>                   emNCellsByLayer(const TriggerTower* tt) const override;
      virtual std::vector<int>                   emNCellsByReceiver(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<int> >     emNCellsByReceiverByLayer(const TriggerTower* tt) const override;
      virtual std::vector<int>                   emTTCellsLayerNames(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<int> >     emTTCellsLayerNamesByReceiver(const TriggerTower* tt) const override;
      virtual float                              emTTCellsEnergy(const TriggerTower* tt) const override;
      virtual std::vector<float>                 emTTCellsEnergyByLayer(const TriggerTower* tt) const override;
      virtual std::vector<float>                 emTTCellsEnergyByReceiver(const TriggerTower* tt, const int mode = 0) const override;
      virtual float                              emTTCellsEt(const TriggerTower* tt) const override;
      virtual std::vector<float>                 emTTCellsEtByLayer(const TriggerTower* tt) const override;
      virtual std::vector<float>                 emTTCellsEtByReceiver(const TriggerTower* tt, const int mode = 0) const override;

      virtual std::vector<const CaloCell*>       hadCells(const TriggerTower* tt) const override;
      virtual int                                hadNCells(const TriggerTower* tt) const override;
      virtual std::vector<int>                   hadNCellsByLayer(const TriggerTower* tt) const override;
      virtual std::vector<int>                   hadNCellsByReceiver(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<int> >     hadNCellsByReceiverByLayer(const TriggerTower* tt) const override;
      virtual std::vector<int>                   hadTTCellsLayerNames(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<int> >     hadTTCellsLayerNamesByReceiver(const TriggerTower* tt) const override;
      virtual float                              hadTTCellsEnergy(const TriggerTower* tt) const override;
      virtual std::vector<float>                 hadTTCellsEnergyByLayer(const TriggerTower* tt) const override;
      virtual std::vector<float>                 hadTTCellsEnergyByReceiver(const TriggerTower* tt, const int mode = 0) const override;
      virtual float                              hadTTCellsEt(const TriggerTower* tt) const override;
      virtual std::vector<float>                 hadTTCellsEtByLayer(const TriggerTower* tt) const override;
      virtual std::vector<float>                 hadTTCellsEtByReceiver(const TriggerTower* tt, const int mode = 0) const override;
      virtual float                              tileCellEnergy(const TriggerTower* tt, IdTTL1CellMapType& map) const override;


      //  High Voltage Information
      virtual float                              emCaloQuality(const TriggerTower* tt) const override;
      virtual float                              emNCellsNonNominal(const TriggerTower* tt) const override;
      virtual std::vector<float>                 emNCellsNonNominalByLayer(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<float> >   emNCellsNonNominalByReceiverByLayer(const TriggerTower* tt) const override;
      virtual float                              emNonNominalMeanScale(const TriggerTower* tt) const override;
      virtual std::vector<float>                 emNonNominalMeanScaleByLayer(const TriggerTower* tt) const override;
      virtual std::vector<float>                 emNonNominalMeanScaleByReceiver(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<float> >   emNonNominalMeanScaleByReceiverByLayer(const TriggerTower* tt) const override;

      virtual float                              hadCaloQuality(const TriggerTower* tt) const override;
      virtual float                              hadNCellsNonNominal(const TriggerTower* tt) const override;
      virtual std::vector<float>                 hadNCellsNonNominalByLayer(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<float> >   hadNCellsNonNominalByReceiverByLayer(const TriggerTower* tt) const override;
      virtual float                              hadNonNominalMeanScale(const TriggerTower* tt) const override;
      virtual std::vector<float>                 hadNonNominalMeanScaleByLayer(const TriggerTower* tt) const override;
      virtual std::vector<float>                 hadNonNominalMeanScaleByReceiver(const TriggerTower* tt) const override;
      virtual std::vector<std::vector<float> >   hadNonNominalMeanScaleByReceiverByLayer(const TriggerTower* tt) const override;

      //  Database Attributes
      virtual const coral::AttributeList*        emDbAttributes(const TriggerTower* tt, const CondAttrListCollection* dbAttrList) const override;
      virtual std::vector<const coral::AttributeList*> emDbRxGainsAttributes(const TriggerTower* tt, const CondAttrListCollection* dbAttrList) const override;

      virtual const coral::AttributeList*        hadDbAttributes(const TriggerTower* tt, const CondAttrListCollection* dbAttrList) const override;
      virtual std::vector<const coral::AttributeList*> hadDbRxGainsAttributes(const TriggerTower* tt, const CondAttrListCollection* dbAttrList) const override;

      // Phase 1 SuperCells
      std::vector<int>                           emSuperCellIdentifiers(const TriggerTower* tt) const;
      std::vector<int>                           hadSuperCellIdentifiers(const TriggerTower* tt) const;

      // Database access
      virtual unsigned int                       ModuleId(const coral::AttributeList* attrList) const override;
      virtual unsigned int                       ErrorCode(const coral::AttributeList* attrList) const override;
      virtual unsigned long long                 PprDacScanResultsTimeStamp(const coral::AttributeList* attrList) const override;
      virtual unsigned long long                 PprPedestalRunResultsTimeStamp(const coral::AttributeList* attrList) const override;
      virtual unsigned long long                 PprNoiseRunResultsTimeStamp(const coral::AttributeList* attrList) const override;
      virtual unsigned long long                 PprTimingResultsTimeStamp(const coral::AttributeList* attrList) const override;
      virtual unsigned long long                 PprSatBcidResultsTimeStamp(const coral::AttributeList* attrList) const override;
      virtual unsigned long long                 PprFirFilterResultsTimeStamp(const coral::AttributeList* attrList) const override;
      virtual unsigned long long                 PprLutValuesResultsTimeStamp(const coral::AttributeList* attrList) const override;
      virtual double                             DacOffset(const coral::AttributeList* attrList) const override;
      virtual double                             DacSlope(const coral::AttributeList* attrList) const override;
      virtual double                             PedMean(const coral::AttributeList* attrList) const override;
      virtual unsigned int                       PedValue(const coral::AttributeList* attrList) const override;
      virtual unsigned int                       FullDelayData(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 SyncDelayBcid(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 InBcidNegedge(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 ExtBcidThreshold(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 SatBcidThreshLow(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 SatBcidThreshHigh(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 SatBcidLevel(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 BcidEnergyRangeLow(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 BcidEnergyRangeHigh(const coral::AttributeList* attrList) const override;
      virtual unsigned short int                 FirStartBit(const coral::AttributeList* attrList) const override;
      virtual short                              FirCoeff1(const coral::AttributeList* attrList) const override;
      virtual short                              FirCoeff2(const coral::AttributeList* attrList) const override;
      virtual short                              FirCoeff3(const coral::AttributeList* attrList) const override;
      virtual short                              FirCoeff4(const coral::AttributeList* attrList) const override;
      virtual short                              FirCoeff5(const coral::AttributeList* attrList) const override;
      virtual unsigned short                     LutStrategy(const coral::AttributeList* attrList) const override;
      virtual unsigned short                     LutOffset(const coral::AttributeList* attrList) const override;
      virtual unsigned short                     LutNoiseCut(const coral::AttributeList* attrList) const override;
      virtual unsigned short                     LutSlope(const coral::AttributeList* attrList) const override;

      virtual unsigned int                       DeadChannel(const coral::AttributeList* attrList) const override;
      virtual unsigned int                       DisabledTower(const coral::AttributeList* attrList) const override;

      virtual float                              RxGain(const coral::AttributeList* attrList) const override;
      virtual unsigned int                       RxStatus(const coral::AttributeList* attrList) const override;


      virtual void                               LArHV(const SG::ReadCondHandleKey<ILArHVScaleCorr>& scaleCorrKey,
                                                       const SG::ReadCondHandleKey<LArOnOffIdMapping>& cablingKey) override;
      virtual void                               caloCells(const CaloCellContainer* cells) override;
              Identifier                         ID(const double eta, const double phi, int layer) const;
      virtual Identifier                         emID(const double eta, const double phi) const override;
      virtual Identifier                         hadID(const double eta, const double phi) const override;


protected:
      virtual int                                pos_neg_z(const double eta) const override;
      virtual int                                region(const double eta) const override;
      virtual int                                ieta(const double eta) const override;
      virtual int                                iphi(const double eta, const double phi) const override;
//       virtual Identifier                         emID(const double eta,const double phi) const override;
//       virtual Identifier                         hadID(const double eta,const double phi) const override;

      virtual std::vector<L1CaloRxCoolChannelId> emReceivers(const TriggerTower* tt) const override;
      virtual std::vector<L1CaloRxCoolChannelId> hadReceivers(const TriggerTower* tt) const override;

      virtual std::vector<std::vector<const CaloCell*> > sortFCAL23Cells(const std::vector<const CaloCell*> &cells, const double eta) const override;
      virtual std::vector<std::vector<const CaloCell*> > sortFCAL23Cells(const std::vector<const CaloCell*> &cells, const std::vector<unsigned int>& rxId) const override;

      virtual std::vector<std::vector<const CaloCell*> > sortEMCrackCells(const std::vector<const CaloCell*> &cells) const override;

      virtual float                              LArNonNominalHV(const std::vector<const CaloCell*> &cells) const override;
      virtual float                              LArNonNominalHV(const CaloCell* cell,
                                                                 const ILArHVScaleCorr* scaleCorr,
                                                                 const ILArHVScaleCorr* onlineScaleCorr,
                                                                 const LArOnOffIdMapping* cabling) const override;
      virtual float                              LArHVScale(const  std::vector<const CaloCell*> &cells) const override;
      virtual float                              LArHVScale(const CaloCell* cell,
                                                            const ILArHVScaleCorr* scaleCorr,
                                                            const ILArHVScaleCorr* onlineScaleCorr,
                                                            const LArOnOffIdMapping* cabling) const override;
      virtual float                              TileNonNominal(const  std::vector<const CaloCell*> &cells) const override;
      virtual float                              TileNonNominal(const CaloCell* cell) const override;

      virtual float                              LArCaloQuality(const std::vector<const CaloCell*> &cells) const override;
      virtual float                              TileCaloQuality(const std::vector<const CaloCell*> &cells) const override;

      std::vector<int>                           SuperCellIdentifiers(const std::vector<const CaloCell*> &cells) const;

private:

      ToolHandle<LVL1::IL1CaloTTIdTools>               m_l1CaloTTIdTools;
      ToolHandle<LVL1::IL1CaloCells2TriggerTowers>     m_cells2tt;
      const CaloIdManager*                             m_caloMgr;
      CaloTriggerTowerService*                         m_ttSvc;
      const TTOnlineID*                                m_ttOnlineIdHelper;
      const CaloLVL1_ID*                               m_lvl1Helper;
      L1CaloCondSvc*                                   m_l1CondSvc;
      const LArOnlineID*                               m_larOnlineID;
      ToolHandle<ICaloSuperCellIDTool>                 m_scidtool;

      // LAr HV
      bool                                             m_isLArHVCorrToolSet;
      double                                           m_LArHVNonNomPara;
      SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey;
      SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey;

      // Tile HV
      const TileHWID*                                  m_tileHWID;
      ToolHandle<ITileBadChanTool>                     m_tileBadChanTool;

      // FCAL 23 Mapping
      ToolHandle<LVL1::IL1CaloFcal23Cells2RxMappingTool>    m_rxMapTool;

      SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineScaleCorrKey
      { this, "OnlineLArHVScaleCorr", "LArHVScaleCorr", "" };
};
} // end of namespace
#endif
