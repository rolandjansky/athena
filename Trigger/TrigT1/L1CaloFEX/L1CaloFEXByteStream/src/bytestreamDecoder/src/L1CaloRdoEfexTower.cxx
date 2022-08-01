
#include "bytestreamDecoder/L1CaloRdoEfexTower.h"


L1CaloRdoEfexTower::L1CaloRdoEfexTower( int crate, int module, int eta, int phi, 
                                        int layer, const L1CaloDetectorRegion& region )
: L1CaloRdo( crate, module, eta, phi, layer, 1 )
, m_fpgaNumber( 0 )
, m_mgtNumber( 0 )
, m_wordNumber( 0 )
, m_mpodNumber( 0 )
, m_fibreNumber( 0 )
, m_copyExists( false )
, m_overlap( 0 )
{
   // At the moment the decoding needs to use the EfexCellMapping class
   // so we already have a detector region (as well as local eta & phi)
   // and can just store it.
   setRegion( region );
}

std::string
L1CaloRdoEfexTower::getType( ) const
{
    return std::string( "EfexTower" );
}

bool 
L1CaloRdoEfexTower::getCrcError( ) const
{
   // **FIXME** Implement (if appropriate). And/or other error flags.
   return false;
}

const std::vector<uint32_t>& 
L1CaloRdoEfexTower::getSupercells( ) const
{
   return m_supercells;
}

// For the benefit of the mapping tool,
// save and return some hardware info.
uint32_t
L1CaloRdoEfexTower::getFpgaNumber( ) const
{
   return m_fpgaNumber;
}

uint32_t
L1CaloRdoEfexTower::getMgtNumber( ) const
{
   return m_mgtNumber;
}

uint32_t
L1CaloRdoEfexTower::getWordNumber( ) const
{
   return m_wordNumber;
}

uint32_t
L1CaloRdoEfexTower::getMinipodNumber( ) const
{
   return m_mpodNumber;
}

uint32_t
L1CaloRdoEfexTower::getFibreNumber( ) const
{
   return m_fibreNumber;
}

bool
L1CaloRdoEfexTower::sameDatum( const L1CaloRdoEfexTower& rhs ) const
{
   // We can have overlaps between FPGAs in the same module.
   if ( m_fpgaNumber != rhs.m_fpgaNumber )
      return false;
   return L1CaloRdo::sameDatum( rhs );
}

void
L1CaloRdoEfexTower::setSupercells( std::vector<uint32_t>& supercells )
{
   m_supercells = supercells;
}

void
L1CaloRdoEfexTower::setHardwareInfo( uint32_t fpga, uint32_t mgt, uint32_t word,
                                     uint32_t mpod, uint32_t fibre, uint32_t overlap )
{
   m_fpgaNumber  = fpga;
   m_mgtNumber   = mgt;
   m_wordNumber  = word;
   m_mpodNumber  = mpod;
   m_fibreNumber = fibre;
   m_overlap     = overlap;
}

bool 
L1CaloRdoEfexTower::isCoreTower( ) const
{
   return (m_overlap == 0);
}

void 
L1CaloRdoEfexTower::setCopy( std::list<L1CaloRdoEfexTower>::const_iterator& copy, bool exists )
{
   m_copy = copy;
   m_copyExists = exists;
}

const std::list<L1CaloRdoEfexTower>::const_iterator& 
L1CaloRdoEfexTower::getCopy( ) const
{
   return m_copy;
}

bool
L1CaloRdoEfexTower::hasCopy( ) const
{
   return m_copyExists;
}
