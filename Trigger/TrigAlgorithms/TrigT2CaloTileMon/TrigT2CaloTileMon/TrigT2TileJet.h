/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     TrigT2TileJet.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTileMon
 
 AUTHOR:   Jens Damaske
 
 *******************************************************************/

#ifndef TRIGT2CALOTILEMON_TRIGTT2TILEJET_H
#define TRIGT2CALOTILEMON_TRIGTT2TILEJET_H

// #include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/Trig3Momentum.h"
/// #include "Trig3Momentum.h"

#include "TrigT2CaloTileMon/TrigT2Tower.h"
/// #include "TrigT2Tower.h"

#include "GaudiKernel/MsgStream.h"

#include <vector>
#include <iostream>

/** LVL2 Trigger TileJet Class */
class TrigT2TileJet /* : public TrigEMCluster */
{
  
   public:
      // Default ctor/dtor.

      void insertCell(const Trig3Momentum& newCell, MsgStream& log);
      void searchTowerAndInsert( const Trig3Momentum& cell,  MsgStream& log, double etaShift = 0.0 );

      /** find hottest Tower */
      bool findHottestTower(TrigT2Tower &tower) const;

      /** debugging */
      void print(MsgStream& log) const;

   private:
      /** vector of TrigT2Tower elements that compose Jet */
      std::vector<TrigT2Tower> m_towercollection;
};

#endif
