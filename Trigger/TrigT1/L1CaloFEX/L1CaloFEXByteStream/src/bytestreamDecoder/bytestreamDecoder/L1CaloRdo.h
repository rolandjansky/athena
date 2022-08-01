/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1CALO_RDO_H
#define L1CALO_RDO_H

#include <vector>
#include <list>
#include <string>


#include "channelMappings/L1CaloDetectorRegion.h"


class L1CaloRdoRodInfo;

class L1CaloRdo 
{
public:
   virtual ~L1CaloRdo();

   int getCrate( ) const;
   int getModule( ) const;
   int getEta( ) const;
   int getPhi( ) const;
   int getLayer( ) const;
   int getValue( size_t slice ) const;
   int getValue( ) const;
   int getFlag( size_t slice ) const;
   int getFlag( ) const;
   int getL1aPos( ) const;

   L1CaloDetectorRegion getRegion( ) const;
#ifdef OFFLINE_DECODER
    void setWord0( uint32_t val, size_t slice ){ if(slice < m_word0s.size()) m_word0s[slice] = val; };
    void setWord1( uint32_t val, size_t slice ){ if(slice < m_word1s.size()) m_word1s[slice] = val; };
    uint32_t getWord0( size_t slice ) const { return (slice < m_word0s.size()) ? m_word0s[slice] : 0; };
    uint32_t getWord1( size_t slice ) const { return (slice < m_word1s.size()) ? m_word1s[slice] : 0; };
#endif
   const std::list<L1CaloRdoRodInfo>::const_iterator& getRodInfo( ) const;
   int getModuleStatus( ) const;

   bool getModuleErrorGlinkParity( ) const;
   bool getModuleErrorGlinkProtocol( ) const;
   bool getModuleErrorBcnMismatch( ) const;
   bool getModuleErrorFifoOverflow( ) const;
   bool getModuleErrorSpecific( ) const;
   bool getModuleErrorUnused( ) const;
   bool getModuleErrorGlinkTimeout( ) const;
   bool getModuleErrorGlinkDown( ) const;
   
   size_t numSlices( ) const;
   bool sameDatum( const L1CaloRdo& rhs ) const;
   
   void setValue( int val, size_t slice );
   void setValue( int val );
   void setFlag( int flag, size_t slice );
   void setFlag( int flag );
   void setRodInfo( std::list<L1CaloRdoRodInfo>::const_iterator& rodInfo );
   
   void info( ) const;
   virtual void infoSpecific( ) const;   
   virtual std::string getType( ) const = 0;

protected:
   L1CaloRdo( int crate, int module, int eta, int phi, 
              int layer, int numSlices );

   void setRegion( const L1CaloDetectorRegion& region );

private:
   L1CaloRdo( );
   
   int m_crate;
   int m_module;
   int m_eta;
   int m_phi;
   int m_layer;
   std::vector<int> m_vals;
   std::vector<int> m_flags;
   int m_l1aPos;

   L1CaloDetectorRegion m_region;
#ifdef OFFLINE_DECODER
   std::vector<uint32_t> m_word0s,m_word1s;
#endif
   std::list<L1CaloRdoRodInfo>::const_iterator m_rodInfo;
};

bool operator<(const L1CaloRdo& lhs, const L1CaloRdo& rhs);

#endif
