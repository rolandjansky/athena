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
      /** Constructor */
      TrigT2TileJet();
      /** copy constructor */
      TrigT2TileJet(TrigT2TileJet&p);
      /** Destructor */
      ~TrigT2TileJet();

      /** TrigT2Tower entities making part of Jet */
      std::vector<TrigT2Tower>* towercollection()  const {  return m_towercollection; }

      /** Clear TrigT2Tower m_towercollection */
      void clearTowerCollection();

      void insertCell(Trig3Momentum newCell, MsgStream& log);
      void searchTowerAndInsert( Trig3Momentum cell,  MsgStream& log, double etaShift = 0.0 );

      /** find hottest Tower */
      bool findHottestTower(TrigT2Tower &tower);

      /** debugging */
      void print(MsgStream& log);

   private:
      /** vector of TrigT2Tower elements that compose Jet */
      std::vector<TrigT2Tower>* m_towercollection;

};

/// conversion to formatted string: multi-line!
std::string str (const TrigT2TileJet& a);

#endif
