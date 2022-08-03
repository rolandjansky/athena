
#ifndef L1CALO_RDO_EFEX_TOWER_H
#define L1CALO_RDO_EFEX_TOWER_H

#include "bytestreamDecoder/L1CaloRdo.h"
#include <list>
#include <vector>

class L1CaloRdoEfexTower : public L1CaloRdo
{
public:
   L1CaloRdoEfexTower( int crate, int module, int eta, int phi, 
                       int layer, const L1CaloDetectorRegion& region ); 
   virtual std::string getType( ) const;

   bool getCrcError( ) const;
   
   const std::vector<uint32_t>& getSupercells( ) const;
   uint32_t getFpgaNumber( ) const;
   uint32_t getMgtNumber( ) const;
   uint32_t getWordNumber( ) const;
   uint32_t getMinipodNumber( ) const;
   uint32_t getFibreNumber( ) const;
   
   bool sameDatum( const L1CaloRdoEfexTower& rhs ) const;
   
   void setSupercells( std::vector<uint32_t>& supercells );
   void setHardwareInfo( uint32_t fpga, uint32_t mgt, uint32_t word,
                         uint32_t mpod, uint32_t fibre, uint32_t overlap );
   
   bool isCoreTower( ) const;
   void setCopy( std::list<L1CaloRdoEfexTower>::const_iterator& copy, bool exists );
   const std::list<L1CaloRdoEfexTower>::const_iterator& getCopy( ) const;
   bool hasCopy( ) const;
   
private:
   std::vector<uint32_t> m_supercells;
   uint32_t m_fpgaNumber;
   uint32_t m_mgtNumber;
   uint32_t m_wordNumber;
   uint32_t m_mpodNumber;
   uint32_t m_fibreNumber;
   std::list<L1CaloRdoEfexTower>::const_iterator m_copy;
   bool m_copyExists;
   bool m_overlap;
};

#endif
