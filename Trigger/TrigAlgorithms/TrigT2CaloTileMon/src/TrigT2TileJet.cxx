/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloTileMon/TrigT2TileJet.h"
/// #include "TrigT2TileJet.h"
#include "TrigCaloEvent/Trig3Momentum.h"
/// #include "Trig3Momentum.h"

TrigT2TileJet::TrigT2TileJet()
{
   if ( !m_towercollection ) m_towercollection = new std::vector<TrigT2Tower>;
   else m_towercollection->clear();
}

TrigT2TileJet::TrigT2TileJet(TrigT2TileJet& p)
{
   std::vector<TrigT2Tower>* vec = p.towercollection();
   if ( !m_towercollection ) m_towercollection = new std::vector<TrigT2Tower>;
   m_towercollection->clear();
   m_towercollection->insert(m_towercollection->end(),vec->begin(),vec->end() );
}

void TrigT2TileJet::clearTowerCollection()
{
  std::vector<TrigT2Tower>::iterator i_begin;
  std::vector<TrigT2Tower>::iterator i_end;
  std::vector<TrigT2Tower>::iterator i_it;
  i_begin = m_towercollection->begin();
  i_end = m_towercollection->end();
  m_towercollection->erase(i_begin,i_end);
  m_towercollection->clear();
  delete m_towercollection;
  m_towercollection = 0;
}

TrigT2TileJet::~TrigT2TileJet()
{
  if ( m_towercollection ) clearTowerCollection();
}


// std::string str (const TrigT2TileJet& a){
// 
//   std::string s="";
//   char buff[1024];
// 
//   std::sprintf(buff,"L2 TileJet EHad  = %10.2f MeV\n", a.e());     s += buff;
//   std::sprintf(buff,"L2 TileJet Eta   = %10.2f \n",    a.eta());   s += buff;
//   std::sprintf(buff,"L2 TileJet Phi   = %10.2f \n",    a.phi());   s += buff;
// 
//   if(a.towercollection())
//     std::sprintf(buff,"Tower Members = %4d\n", a.towercollection()->size());  s += buff;
// 
//   return s;
// 
// }

void TrigT2TileJet::insertCell(Trig3Momentum newCell, MsgStream& log)
{
   if ( log.level() <= MSG::DEBUG ){
   log << MSG::DEBUG << " REGTEST:     CELL: initial cell info:   eta:" << newCell.eta() << " phi:" << newCell.phi()
             << " e:" << newCell.e() << " layer:" << newCell.caloSample() << endreq;
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

void TrigT2TileJet::searchTowerAndInsert( Trig3Momentum cell, MsgStream& log, double etaShift )
{
   bool flag = 0;
   
   std::vector<TrigT2Tower>::iterator itt, itt_end;
   itt_end = m_towercollection->end();

   // walk through tower collection and look for appropriate tower exists
   for (itt = m_towercollection->begin(); itt!=itt_end; itt++)
   {
      if( (*itt).isMember(cell, log, etaShift) )
      {
         if( (*itt).insertCell(cell, log) )
         {
            // insert new Cell and end loop immediately
            flag = 1;
            break; // stop loop
         }
         else
         {
            log << MSG::WARNING << " REGTEST:     CELL: tried to insert Cell twice in a tower!" << endreq;
            flag=1; // do not insert in new tower
            break;
         }
      }
   }
   // tower not in collection yet, so create one
   if ( !flag )
   {
     if ( log.level() <= MSG::DEBUG )
     log << MSG::DEBUG << " REGTEST:     CELL:          insert in NEW tower" << endreq;
      m_towercollection->push_back( TrigT2Tower(cell, log, etaShift) );

   }

}

void TrigT2TileJet::print(MsgStream& log)
{
   std::vector<TrigT2Tower>::iterator itt, itt_end;
   itt_end = m_towercollection->end();
   log << MSG::DEBUG << " REGTEST: JET:  print jet:" << endreq;
   for (itt = m_towercollection->begin(); itt!=itt_end; itt++)
   {
      (*itt).print(log,MSG::DEBUG);
   }
}

bool TrigT2TileJet::findHottestTower(TrigT2Tower &tower)
{
   tower.setE(0.);
   std::vector<TrigT2Tower>::iterator itt, itt_end;
   itt_end = m_towercollection->end();
   for (itt = m_towercollection->begin(); itt!=itt_end; itt++)
   {
      if ( (*itt).e() > tower.e() ) tower = (*itt);
   }
   if ( tower.e() > 0.0) return 1;
   else return 0;
}
