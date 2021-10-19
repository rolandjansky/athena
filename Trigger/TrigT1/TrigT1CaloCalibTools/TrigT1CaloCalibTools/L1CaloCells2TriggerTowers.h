/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBTOOLS_L1CALOCELLS2TRIGGERTOWERS_H
#define TRIGT1CALOCALIBTOOLS_L1CALOCELLS2TRIGGERTOWERS_H

// Athena Includes
#include "AsgTools/AsgTool.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloMatchCell2Tower.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

// Calo includes
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

// LAr includes
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArDigit.h"
#include "LArCabling/LArOnOffIdMapping.h"

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileDigits.h"
#include "TileConditions/TileCablingService.h"

// PABC interface include
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"

// STL includes
#include <map>
#include <vector>
#include <string>


/**
  * AlgTool that implement several methods
  * used to produced the On/Off and On/Attr
  * trigger tower Id maps.
  *
  *  @author Damien Prieur <damien.prieur@cern.ch>
  *  @author John Morris <john.morris@cern.ch>
  */

namespace LVL1 {

class
L1CaloCells2TriggerTowers final : virtual public IL1CaloCells2TriggerTowers, public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( L1CaloCells2TriggerTowers , LVL1::IL1CaloCells2TriggerTowers )
public:
  /// constructor
  L1CaloCells2TriggerTowers( const std::string& name );
  /// destructor
  virtual ~L1CaloCells2TriggerTowers() {}
  /// delete the big 4
  L1CaloCells2TriggerTowers() = delete;
  L1CaloCells2TriggerTowers(const L1CaloCells2TriggerTowers& rhs) = delete;
  L1CaloCells2TriggerTowers(L1CaloCells2TriggerTowers&& rhs) = delete;
  L1CaloCells2TriggerTowers& operator=(const L1CaloCells2TriggerTowers& rhs) = delete;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // To associate CaloCell to the trigger tower they belong to
  // This method must be called before any other method in order
  // to setup correctly the CaloCell-TT map
  virtual bool initCaloCellsTriggerTowers(const CaloCellContainer& cellContainer) override;
  virtual bool initLArDigitsTriggerTowers(const LArDigitContainer& larDigitContainer) override;
  virtual bool initTileDigitsTriggerTowers(const TileDigitsContainer& tileDigitsContainer) override;

  // John Morris <john.morris@cern.ch> 15/2/2011
  // Moving most useful functions to the top of this file for clarity
  virtual std::vector<const CaloCell*> caloCells(const Identifier& ttId) const override;
  virtual std::vector<std::vector<const CaloCell*> > caloCellsByLayer(const Identifier& ttId) const override;
  virtual std::vector<int> layerNames(const Identifier& ttID) const override;
  virtual int layerNames(const CaloCell* cell) const override;
  virtual float energy(const std::vector<const CaloCell*> &cells) const override;
  virtual float et(const std::vector<const CaloCell*> &cells) const override;
  virtual float energy(const Identifier& ttId) const override;
  virtual float et(const Identifier& ttId) const override;

  // Return a vector digits belonging the to requested trigger tower
  virtual std::vector<double> samples(const Identifier& ttId) const override;
  virtual std::vector<const LArDigit*> larDigits(const Identifier& ttId) const override;
  virtual std::vector<const TileDigits*> tileDigits(const Identifier& ttId) const override;

  virtual void dumpCaloCells() const override;
  virtual void dumpCaloCells(const Identifier& ttId) const override;

  virtual void dumpDigits(const Identifier& ttId) const override;

  virtual void dumpLArDigits(const Identifier& ttId) const override; /* ttId must be a LAr TTId */
  virtual void dumpLArDigits() const override;

  virtual void dumpTileDigits(const Identifier& ttId) const override; /* ttId must be a Tile TTId */
  virtual void dumpTileDigits() const override;

protected:
  virtual float calcEnergyOrEt(const std::vector<const CaloCell*> &cells, const unsigned int mode) const override;

  virtual void reset() override;
  virtual void resetCaloCells() override;
  virtual void resetLArDigits() override;
  virtual void resetTileDigits() override;


  virtual void dump(const std::vector<const CaloCell*>& vCells) const override;
  virtual void dump(const std::vector<const LArDigit*>& vCells) const override;
  virtual void dump(const std::vector<const TileDigits*>& vCells) const override;

private:
  // Id helpers
  const CaloLVL1_ID* m_lvl1Helper;
  const TileID* m_tileID;
  const CaloCell_ID* m_caloCellHelper;

  // Services
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    {this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  ToolHandle<LVL1::IL1CaloMatchCell2Tower> m_cellMatch
    { this, "L1CaloMatchCell2Tower",  "LVL1::L1CaloMatchCell2Tower",  "L1CaloMatchCell2Tower" };
  const TileCablingService * m_tileCablingService;
  CaloTriggerTowerService* m_ttSvc;

  bool m_bInitialized;
  bool m_bLArDigitsInitialized;
  bool m_bTileDigitsInitialized;

  // Offline TT Id - vector of CaloCell map
  std::map<unsigned int, std::vector<const CaloCell*> > m_mTTCaloCells;
  std::map<unsigned int, std::vector<const LArDigit*> > m_mTTLArDigits;
  std::map<unsigned int, std::vector<const TileDigits*> > m_mTTTileDigits;

};
} // end of namespace
#endif
