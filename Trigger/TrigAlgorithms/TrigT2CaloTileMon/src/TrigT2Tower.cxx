/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     TrigT2Tower.cxx
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTileMon

 AUTHOR:   Jens Damaske
 PURPOSE:  Combines 3 Trig3Momentum Objects to a Tower

 *******************************************************************/

#include "TrigT2CaloTileMon/TrigT2Tower.h"
/// #include "TrigT2Tower.h"


TrigT2Tower::TrigT2Tower(): m_eta(0.0), m_phi(0.0)
{
   // flush tower
   setComplete( 0 );
   setE( 0. );

   // flush all 3 cells
   for( int i=0; i<3; i++ )
   {
      m_cells[i] = Trig3Momentum( 0., 0., 0., CaloSampling::Unknown);
      m_cells[i].setInCone(0);
   }
}

/** insert first Cell everytime new tower is build */
TrigT2Tower::TrigT2Tower(const Trig3Momentum& newCell, MsgStream& log, double etaShift): m_eta(0.0), m_phi(0.0)
{
   // flush tower
   setComplete( 0 );
   setE( 0. );
   
   // flush all 3 cells
   for( int i=0; i<3; i++ )
   {
      m_cells[i] = Trig3Momentum( 0., 0., 0., CaloSampling::Unknown);
      m_cells[i].setInCone(0);
   }

   // set tower position to first cell position
   setEta( newCell.eta() + etaShift );
   setPhi( newCell.phi() );

   insertCell( newCell, log);
}

bool TrigT2Tower::insertCell(const Trig3Momentum& cell, MsgStream& log)
{
   if ( cell.caloSample() == CaloSampling::TileBar0 || cell.caloSample() == CaloSampling::TileExt0 )
   {
      if ( !m_cells[0].inCone() )
      {
         m_cells[0] = cell;
         setE( e() + cell.e() );
         m_cells[0].setInCone(1);
      }
      else return 0;
   }
   else if ( cell.caloSample() == CaloSampling::TileBar1 || cell.caloSample() == CaloSampling::TileExt1 )
   {
      if ( !m_cells[1].inCone() )
      {
         m_cells[1] = cell;
         setE( e() + cell.e() );
         m_cells[1].setInCone(1);
      }
      else return 0;
   }
   else if ( cell.caloSample() == CaloSampling::TileBar2 || cell.caloSample() == CaloSampling::TileExt2 )
   {
      if ( !m_cells[2].inCone() )
      {
         m_cells[2] = cell;
         setE( e() + cell.e()/2. );
         m_cells[2].setInCone(1);
      }
      else return 0;
   }
   else
   {
      log << MSG::WARNING << " REGTEST:     CELL: is not in TILE. caloSample ID is: " << cell.caloSample() << endmsg;
      return 0;
   }

   // is tower full now ??
   int sum = 0;
   for( int i=0; i<3; i++ )
   {
      if ( m_cells[i].inCone() ) sum++;
   }
   if ( sum == 3 ) setComplete(1);
   
   return 1;
}

// phi and eta is known up to 3 decimal
bool TrigT2Tower::isMember(const Trig3Momentum& cell, MsgStream& log, double etaShift) const
{
   double delta = 0.005;
   if ( fabs(eta()-(cell.eta()+etaShift))<delta && fabs(phi()-cell.phi())<delta )
   {
     if ( log.level()<MSG::DEBUG )
      log << MSG::DEBUG << " REGTEST:     CELL:          insert in EXISTING tower:   eta" << eta() << "   phi:" << phi() << endmsg;
     return true;
   }

   return false;
}

void TrigT2Tower::print(MsgStream& log, MSG::Level level) const
{
   log << level << " REGTEST:   TOWER:  eta:" << eta() << " phi:" << phi() << " e:" << e() << endmsg;
   
   for( int i=0; i<3; i++ )
   {
      if ( m_cells[i].inCone() )
      {
         log << level << " REGTEST:     CELL " << i << " :  eta:" <<  m_cells[i].eta() << " phi:" << m_cells[i].phi()
                  << " e:" << m_cells[i].e() << endmsg;
      }
      else log << level << " REGTEST:     CELL " << i << " :  is NOT SET" << endmsg;
   }

}
