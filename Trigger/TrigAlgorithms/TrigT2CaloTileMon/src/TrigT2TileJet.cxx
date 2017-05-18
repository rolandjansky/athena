/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloTileMon/TrigT2TileJet.h"
/// #include "TrigT2TileJet.h"
#include "TrigCaloEvent/Trig3Momentum.h"
/// #include "Trig3Momentum.h"

void TrigT2TileJet::insertCell(const Trig3Momentum& newCell, MsgStream& log)
{
   if ( log.level() <= MSG::DEBUG ){
   log << MSG::DEBUG << " REGTEST:     CELL: initial cell info:   eta:" << newCell.eta() << " phi:" << newCell.phi()
             << " e:" << newCell.e() << " layer:" << newCell.caloSample() << endmsg;
   }

   // ONLY TILE BARREL AND EXTEND BARREL ALLOWED TO PASS HERE
   if (
         newCell.caloSample() == CaloSampling::TileBar0 ||
         newCell.caloSample() == CaloSampling::TileBar1 ||
         newCell.caloSample() == CaloSampling::TileBar2 ||
         newCell.caloSample() == CaloSampling::TileExt0 ||
         newCell.caloSample() == CaloSampling::TileExt1 ||
         newCell.caloSample() == CaloSampling::TileExt2
      )
   {
      // if cell in layer 2, cell size is 0.2 x 0.1
      // make a copy of this cell and assign both two a tower
      if (
         newCell.caloSample() == CaloSampling::TileBar2 ||
         newCell.caloSample() == CaloSampling::TileExt2
         )
      {
         searchTowerAndInsert( newCell, log, -0.05 );
         searchTowerAndInsert( newCell, log, +0.05 );
      }
      else
      {
         searchTowerAndInsert(newCell, log);
      }

   }
}

void TrigT2TileJet::searchTowerAndInsert( const Trig3Momentum& cell, MsgStream& log, double etaShift )
{
   bool flag = 0;
   
   // walk through tower collection and look for an appropriate tower
   for (TrigT2Tower& tt : m_towercollection)
   {
      if( tt.isMember(cell, log, etaShift) )
      {
         if( tt.insertCell(cell, log) )
         {
            // insert new Cell and end loop immediately
            flag = 1;
            break; // stop loop
         }
         else
         {
            log << MSG::WARNING << " REGTEST:     CELL: tried to insert Cell twice in a tower!" << endmsg;
            flag=1; // do not insert in new tower
            break;
         }
      }
   }
   // tower not in collection yet, so create one
   if ( !flag )
   {
     if ( log.level() <= MSG::DEBUG )
     log << MSG::DEBUG << " REGTEST:     CELL:          insert in NEW tower" << endmsg;
      m_towercollection.emplace_back( cell, log, etaShift );

   }

}

void TrigT2TileJet::print(MsgStream& log) const
{
   log << MSG::DEBUG << " REGTEST: JET:  print jet:" << endmsg;
   for (const TrigT2Tower& tt : m_towercollection)
   {
      tt.print(log,MSG::DEBUG);
   }
}

bool TrigT2TileJet::findHottestTower(TrigT2Tower &tower) const
{
   tower.setE(0.);
   for (const TrigT2Tower& tt : m_towercollection)
   {
      if ( tt.e() > tower.e() ) tower = tt;
   }
   if ( tower.e() > 0.0) return true;
   else return false;
}
