// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrigT1CaloCalibToolInterfaces/IL1CaloMatchCell2Tower.h
 * @author scott snyder <snyder@bnl.gov>, from existing code
 * @date Oct, 2021
 * @brief Map from calo cells to trigger towers.
 *
 * Factored out from IL1CaloCell2TriggerTowers:
 *   Author: John Morris (john.morris@cern.ch)                             *
 *           Queen Mary University of London                               *
 */


#ifndef TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOMATCHCELL2TOWER_H
#define TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOMATCHCELL2TOWER_H


#include "GaudiKernel/IAlgTool.h"
class LArOnOffIdMapping;
class CaloCell;
class Identifier;


namespace LVL1 {


class IL1CaloMatchCell2Tower
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (IL1CaloMatchCell2Tower, 1, 0);

  virtual void matchCell2Tower (const CaloCell* caloCell,
                                Identifier& ttId1,
                                Identifier& ttId2) const = 0;
  virtual void matchCell2Tower (const LArOnOffIdMapping& cabling,
                                const CaloCell* caloCell,
                                Identifier& ttId1,
                                Identifier& ttId2) const = 0;
};


} // namespace LVL1


#endif // not TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOMATCHCELL2TOWER_H
