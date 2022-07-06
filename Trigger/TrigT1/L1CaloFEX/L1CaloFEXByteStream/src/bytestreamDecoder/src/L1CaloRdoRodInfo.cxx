/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "bytestreamDecoder/L1CaloRdoRodInfo.h"

#include <iostream>

// Class mainly intended to hold information from the ROD fragment header.
// It also contains error codes from the L1Calo ROD fragment status block
// for the legacy Run 1 and Run 2 system.
// Many of these are no longer relevant for Run 3 FEX/Topo ROD fragments.
// But it still seems worth having a common ROD info class and other error
// bits from these might be added?

L1CaloRdoRodInfo::L1CaloRdoRodInfo( int system, int rod, int run, int bcnum, int triggerType, int detType, int version )
: m_system( system )
, m_rod( rod )
, m_run( run )
, m_bcnum( bcnum )
, m_triggerType( triggerType )
, m_detType( detType )
, m_lvl1( 0 )
, m_size( 0 )
, m_status1( 0 )
, m_status2 ( 0 )
, m_version( version )
, m_status( 16, 0 )
{
}

std::string
L1CaloRdoRodInfo::getType( ) const
{
    return std::string( "RodInfo" );
}

void 
L1CaloRdoRodInfo::info( ) const
{
   std::cout << "RDO Object Type: " << getType();
   std::cout << std::hex;
   std::cout << "    System Id: " << getSystemId();
   std::cout << "   ROD Id: " << getRodId() << std::endl << "     ";
   std::cout << "   Run Num: " << getRunNumber();
   std::cout << "   Level-1 Id: " << getLevel1Id();
   std::cout << "   Bc Number: " << getBcNumber();
   std::cout << "   Trig Type: " << getTriggerType();
   std::cout << "   Step Number: " << getStepNumber();
   std::cout << "   Data Size: " << getDataSize();
   std::cout << "   Minor Ver: " << getMinorVersion();
   std::cout << std::endl;
   std::cout << std::dec;
}

int
L1CaloRdoRodInfo::getMinorVersion( ) const
{
   return ( m_version & 0xffff );
}

bool
L1CaloRdoRodInfo::getIsRun1( ) const
{
   return ( (unsigned int)this->getMinorVersion() < s_minRun2Version );
}

bool
L1CaloRdoRodInfo::getIsRun2( ) const
{
   return ( this->getIsRun3() || (unsigned int)this->getMinorVersion() >= s_minRun2Version );
}

bool
L1CaloRdoRodInfo::getIsRun3( ) const
{
   // Run3 fragments have some of bits 12-15 set.
   return ( ( this->getRodId() & 0xf000 ) != 0 );
}

int 
L1CaloRdoRodInfo::getSystemId( ) const
{
   return m_system;
}

int 
L1CaloRdoRodInfo::getRodId( ) const
{
   return m_rod;
}

int
L1CaloRdoRodInfo::getSourceId( ) const
{
   return ( (m_system<<16) | m_rod );
}

int 
L1CaloRdoRodInfo::getRunNumber( ) const
{
   return m_run;
}

int 
L1CaloRdoRodInfo::getBcNumber( ) const
{
   return m_bcnum;
}

int 
L1CaloRdoRodInfo::getTriggerType( ) const
{
   return m_triggerType;
}

int 
L1CaloRdoRodInfo::getLevel1Id( ) const
{
   return m_lvl1;
}

int 
L1CaloRdoRodInfo::getDataSize( ) const
{
   return m_size;
}

int 
L1CaloRdoRodInfo::getStepNumber( ) const
{
   return ( (m_detType>>4) & 0xfff);
}

int 
L1CaloRdoRodInfo::getOrbitCount( ) const
{
   return ( (m_detType>>16) & 0xffff);
}

bool 
L1CaloRdoRodInfo::getBcnumMismatch( ) const
{
   return ( m_status1 & 0x1 );
}

bool 
L1CaloRdoRodInfo::getGlinkTimeout( ) const
{
   return ( m_status1 & 0x4 );
}

bool 
L1CaloRdoRodInfo::getDataTransportError( ) const
{
   return ( m_status1 & 0x8 );
}

bool 
L1CaloRdoRodInfo::getRodOverflow( ) const
{
   return ( m_status1 & 0x10 );
}

bool 
L1CaloRdoRodInfo::getModuleLinkError( ) const
{
   return ( m_status1 & 0x10000 );
}

bool 
L1CaloRdoRodInfo::getCmmParityError( ) const
{
   return ( m_status1 & 0x20000 );
}

bool 
L1CaloRdoRodInfo::getGlinkError( ) const
{
   return ( m_status1 & 0x40000 );
}

bool 
L1CaloRdoRodInfo::getRoiOverflow( ) const
{
   return ( m_status2 & 0x2 );
}

bool 
L1CaloRdoRodInfo::getTriggerTypeTimeout( ) const
{
   return ( m_status2 & 0x10000 );
}

int
L1CaloRdoRodInfo::getModuleStatus( const int module ) const
{
   if ( (module < 0) || (module > 15) )
      return 0;
   return m_status[module];
}

void
L1CaloRdoRodInfo::setLvl1Id( int lvl1 )
{
   m_lvl1 = lvl1;
}

void
L1CaloRdoRodInfo::setSize( int size )
{
   m_size = size;
}

void
L1CaloRdoRodInfo::setStatus1( int status )
{
   m_status1 = status;
}

void
L1CaloRdoRodInfo::setStatus2( int status )
{
   m_status2 = status;
}

void
L1CaloRdoRodInfo::setModuleStatus( int module, int status )
{
   m_status[module] = status;
}

