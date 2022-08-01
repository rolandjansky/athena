/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "bytestreamDecoder/L1CaloRdo.h"

#include "bytestreamDecoder/L1CaloRdoRodInfo.h"

#include <iostream>

/*!
 * \class L1CaloRdo
   Abstract base class for L1Calo "Raw Data Objects" (RDOs).
   Each RDO represents some information associated with an eta,phi,layer
   for the complete set of readout time slices (bunch crossings).
   Different coordinates imply different RDOs, but all timeslices for
   the same coordinate are always kept in the same RDO.
   For each RDO there is an integer value and flag per timeslice.
   What this means is up to the subclasses.
   The value may be an Et of towers or jet elements, threshold
   multiplicities or hit bits indicating which thresholds were passed.
   Sometimes the value is bit coded to pack two values into one word.
   The flags are generally error bits, but in some cases other values
   are bit packed into the flags.
   Where either field is bit packed the subclass should have specific
   methods to decode them.
 */

L1CaloRdo::L1CaloRdo(  int crate, int module, int eta, int phi, 
                       int layer, int numSlices )
: m_crate( crate )
, m_module( module )
, m_eta( eta )
, m_phi( phi )
, m_layer( layer )
, m_vals( numSlices, 0 )
, m_flags( numSlices, 0 )
, m_l1aPos( numSlices/2 )
#ifdef OFFLINE_DECODER
, m_word0s(numSlices,0),m_word1s(numSlices,0)
#endif
{
   // std::cout << "Object Created" << std::endl;
}

L1CaloRdo::~L1CaloRdo( )
{
   // std::cout << "Object Deleted" << std::endl;
}

bool
operator<( const L1CaloRdo& lhs, const L1CaloRdo& rhs )
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
   return false;
}

int 
L1CaloRdo::getCrate( ) const
{
   return m_crate;
}

int 
L1CaloRdo::getModule( ) const
{
   return m_module;
}

int 
L1CaloRdo::getEta( ) const
{
   return m_eta;
}

int 
L1CaloRdo::getPhi( ) const
{
   return m_phi;
}

int 
L1CaloRdo::getLayer( ) const
{
   return m_layer;
}

int 
L1CaloRdo::getValue( size_t slice ) const
{
   if ( slice < m_vals.size() )
      return m_vals[slice];
   return 0;
}

int 
L1CaloRdo::getValue( ) const
{
   return getValue( getL1aPos() );
}

int 
L1CaloRdo::getFlag( size_t slice ) const
{
   if ( slice < m_flags.size() )
      return m_flags[slice];
   return 0;
}

int 
L1CaloRdo::getFlag( ) const
{
   return getFlag( getL1aPos() );
}

int
L1CaloRdo::getL1aPos( ) const
{
   return m_l1aPos;
}

L1CaloDetectorRegion 
L1CaloRdo::getRegion( ) const
{
   return m_region;
}


const std::list<L1CaloRdoRodInfo>::const_iterator&
L1CaloRdo::getRodInfo( ) const
{
   return m_rodInfo;
}

int
L1CaloRdo::getModuleStatus( ) const
{
   return m_rodInfo->getModuleStatus( getModule() );
}

bool
L1CaloRdo::getModuleErrorGlinkParity( ) const
{
   return getModuleStatus( ) & 0x01 ;
}

bool
L1CaloRdo::getModuleErrorGlinkProtocol( ) const
{
   return getModuleStatus( ) & 0x02 ;
}

bool
L1CaloRdo::getModuleErrorBcnMismatch( ) const
{
   return getModuleStatus( ) & 0x04 ;
}

bool
L1CaloRdo::getModuleErrorFifoOverflow( ) const
{
   return getModuleStatus( ) & 0x08 ;
}

bool
L1CaloRdo::getModuleErrorSpecific( ) const
{
   return getModuleStatus( ) & 0x10 ;
}

bool
L1CaloRdo::getModuleErrorUnused( ) const
{
   return getModuleStatus( ) & 0x20 ;
}

bool
L1CaloRdo::getModuleErrorGlinkTimeout( ) const
{
   return getModuleStatus( ) & 0x40 ;
}

bool
L1CaloRdo::getModuleErrorGlinkDown( ) const
{
   return getModuleStatus( ) & 0x80 ;
}

size_t
L1CaloRdo::numSlices( ) const
{
   return m_vals.size();
}

bool
L1CaloRdo::sameDatum( const L1CaloRdo& rhs ) const
{
   if ( m_crate != rhs.m_crate )
      return false;
   if ( m_module != rhs.m_module )
      return false;
   if ( m_eta != rhs.m_eta )
      return false;
   if ( m_phi != rhs.m_phi )
      return false;
   if ( m_layer != rhs.m_layer )
      return false;
   return true;
}

void
L1CaloRdo::setValue( int val, size_t slice ) 
{
   if ( slice < m_vals.size() )
      m_vals[slice] = val;
}

void
L1CaloRdo::setValue( int val ) 
{
   setValue( val, getL1aPos() );
}

void
L1CaloRdo::setFlag( int flag, size_t slice ) 
{
   if ( slice < m_flags.size() )
      m_flags[slice] = flag;
}

void
L1CaloRdo::setFlag( int flag ) 
{
   setFlag( flag, getL1aPos() );
}

void
L1CaloRdo::setRodInfo( std::list<L1CaloRdoRodInfo>::const_iterator& rodInfo )
{
   m_rodInfo = rodInfo;
}

void 
L1CaloRdo::info( ) const
{
   std::cout << "RDO Object Type: " << getType()
             << "  Crate: " << getCrate()
             << "  Module: " << getModule()
	     << "  Eta: " << getEta()
	     << "  Phi: " << getPhi()
	     << "  Layer: " << getLayer() << std::endl;
   std::cout << "      Values:  " << std::hex;
   for ( size_t i = 0 ; i < m_vals.size() ; ++i )
      std::cout << getValue(i) << " ";
   std::cout << "      Flags:  ";
   for ( size_t i = 0 ; i < m_flags.size() ; ++i )
      std::cout << getFlag(i) << " ";
   std::cout << std::endl << std::dec;
   infoSpecific();
}

void 
L1CaloRdo::infoSpecific( ) const
{
}


void
L1CaloRdo::setRegion( const L1CaloDetectorRegion& region )
{
   m_region = region;
}


