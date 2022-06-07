/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "bytestreamDecoder/L1CaloRdoFexTob.h"

#include <iostream>
#include <sstream>

/*!
  \class L1CaloRdoFexTob
  L1CaloRdo subclass for FEX Trigger Objects (TOBs and XTOBs).
  These may come from FEX or phase 1 Topo readout.
 */


L1CaloRdoFexTob::L1CaloRdoFexTob( int crate, int module, int eta, int phi, int numSlices,
                                  L1CaloRdoFexTob::TobType tobType, L1CaloRdoFexTob::TobSource source,
                                  int id, int fibre, int tobSeq )
: L1CaloRdo( crate, module, eta, phi, 0, numSlices )
, m_tobType( tobType )
, m_source( source )
, m_id( id )
, m_fibre( fibre )
, m_tobSeq( tobSeq )
{
}

std::string
L1CaloRdoFexTob::getType( ) const
{
   return getTobTypeString() + std::string( "Tob:" ) + getTobSourceString();
}

bool
L1CaloRdoFexTob::sameDatum( const L1CaloRdoFexTob& rhs ) const
{
   if ( m_tobType != rhs.m_tobType )
      return false;
   if ( m_source != rhs.m_source )
      return false;
   if ( m_id != rhs.m_id )
      return false;
   return L1CaloRdo::sameDatum( rhs );
}

L1CaloRdoFexTob::TobType
L1CaloRdoFexTob::getTobType( ) const
{
   return m_tobType;
}

std::string
L1CaloRdoFexTob::getTobTypeString( ) const
{
   if      ( getIsEM() )       { return "EM"; }
   else if ( getIsTau() )      { return "Tau"; }
   else if ( getIsLargeJet() ) { return "LJet"; }
   else if ( getIsSmallJet() ) { return "SJet"; }
   else if ( getIsEnergy() )   { return "Energy"; }
   else if ( getIsMuon() )     { return "Muon"; }
   else                        { return ""; }
}

int
L1CaloRdoFexTob::getTobID( ) const
{
   return m_id;
}

bool
L1CaloRdoFexTob::getIsEM( ) const
{
   return ( getTobType() == TobType::EM );
}

bool
L1CaloRdoFexTob::getIsTau( ) const
{
   return ( getTobType() == TobType::Tau );
}

bool
L1CaloRdoFexTob::getIsJet( ) const
{
   return ( getTobType() == TobType::LargeJet ||
            getTobType() == TobType::SmallJet );
}

bool
L1CaloRdoFexTob::getIsLargeJet( ) const
{
   return ( getTobType() == TobType::LargeJet );
}

bool
L1CaloRdoFexTob::getIsSmallJet( ) const
{
   return ( getTobType() == TobType::SmallJet );
}

bool
L1CaloRdoFexTob::getIsEnergy( ) const
{
   return ( getTobType() == TobType::Energy );
}

bool
L1CaloRdoFexTob::getIsMuon( ) const
{
   return ( getTobType() == TobType::Muon );
}

L1CaloRdoFexTob::TobSource
L1CaloRdoFexTob::getTobSource( ) const
{
   return m_source;
}

std::string
L1CaloRdoFexTob::getTobSourceString( ) const
{
   if      ( getIsTob() )       { return "Tob"; }
   else if ( getIsXtob() )      { return "Xtob"; }
   else if ( getIsTopo() )      { std::ostringstream s; s << "Topo" << m_id; return s.str(); }
   else                         { return ""; }
}

bool
L1CaloRdoFexTob::getIsTob( ) const
{
   return ( getTobSource() == TobSource::EfexTob ||
            getTobSource() == TobSource::JfexTob ||
            getTobSource() == TobSource::GfexTob ||
            getTobSource() == TobSource::Ph1Topo );
}

bool
L1CaloRdoFexTob::getIsXtob( ) const
{
   return ( getTobSource() == TobSource::EfexXtob ||
            getTobSource() == TobSource::JfexXtob ||
            getTobSource() == TobSource::GfexXtob );
}

bool
L1CaloRdoFexTob::getIsEfex( ) const
{
   return ( getTobSource() == TobSource::EfexTob ||
            getTobSource() == TobSource::EfexXtob );
}

bool
L1CaloRdoFexTob::getIsJfex( ) const
{
   return ( getTobSource() == TobSource::JfexTob ||
            getTobSource() == TobSource::JfexXtob );
}

bool
L1CaloRdoFexTob::getIsGfex( ) const
{
   return ( getTobSource() == TobSource::GfexTob ||
            getTobSource() == TobSource::GfexXtob );
}

bool
L1CaloRdoFexTob::getIsTopo( ) const
{
   return ( getTobSource() == TobSource::Ph1Topo );
}

int
L1CaloRdoFexTob::getFibre( ) const
{
   return m_fibre;
}

int
L1CaloRdoFexTob::getTobSeq( ) const
{
   return m_tobSeq;
}

bool
operator<( const L1CaloRdoFexTob& lhs, const L1CaloRdoFexTob& rhs )
{
   if ( lhs.getCrate() < rhs.getCrate() )
      return true;
   if ( lhs.getCrate() > rhs.getCrate() )
      return false;
   if ( lhs.getModule() < rhs.getModule() )
      return true;
   if ( lhs.getModule() > rhs.getModule() )
      return false;
   if ( lhs.getEta() < rhs.getEta() )
      return true;
   if ( lhs.getEta() > rhs.getEta() )
      return false;
   if ( lhs.getPhi() < rhs.getPhi() )
      return true;
   if ( lhs.getPhi() > rhs.getPhi() )
      return false;
   if ( lhs.getLayer() < rhs.getLayer() )
      return true;
   if ( lhs.getTobType() < rhs.getTobType() )
      return true;
   if ( lhs.getTobType() > rhs.getTobType() )
      return false;
   if ( lhs.getTobSource( ) < rhs.getTobSource( ) )
      return true;
   if ( lhs.getTobSource( ) > rhs.getTobSource( ) )
      return false;
   if ( lhs.getTobID( ) < rhs.getTobID( ) )
      return true;
   return false;
}
