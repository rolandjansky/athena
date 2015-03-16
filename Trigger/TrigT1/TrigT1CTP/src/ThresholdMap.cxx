/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <sstream>
#include <iomanip>
#include <cstdlib>

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/TIP.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/CTPTriggerThreshold.h"
#include "CTPfragment/CTPdataformatVersion.h"

using namespace std;

namespace LVL1CTP {
	
  ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector)
	: m_logger( "ThresholdMap" ) {
		
    m_logger.send( MSG::VERBOSE, "Created" );
		
    m_logger.send( MSG::DEBUG, "Creating CTPTriggerThreshold objects for each TrigConf::TriggerThreshold object..." );
    unsigned int start = 0;
		
    for ( vector< TrigConf::TriggerThreshold* >::const_iterator it = threshold_vector.begin(); it != threshold_vector.end(); ++it ) {
			
      m_map[ *it ] = new CTPTriggerThreshold( *it );
      unsigned int end = start + abs( ( *it )->cableStart() - ( *it )->cableEnd() );
      m_map[ *it ]->setStartBit( start );
      m_map[ *it ]->setEndBit( end );
      
      ostringstream message;
      message << "  --> Created object for threshold with name: " << setw( 10 ) << ( *it )->name() << " from TIP = " << setw( 3 ) << start << " to TIP = " << setw( 3 ) << end;
      m_logger.send( MSG::VERBOSE, message.str() );
      
      start = end + 1;
    }
    
    m_logger.send( MSG::DEBUG, "Creating objects done" );
		
  }
	
  ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, 
														 const std::vector< TrigConf::PIT* >& pit_vector)
	: m_logger( "ThresholdMap" ) {
		
    m_logger.send( MSG::VERBOSE, "Created" );
		

	ostringstream testmessage;
	testmessage << "Length of PIT vector: " << pit_vector.size();
	m_logger.send(MSG::DEBUG, testmessage.str());

    m_logger.send( MSG::DEBUG, "Creating CTPTriggerThreshold objects for each TrigConf::TriggerThreshold object..." );
    
		
		for ( vector< TrigConf::TriggerThreshold* >::const_iterator it = threshold_vector.begin(); it != threshold_vector.end(); ++it ) {
			
			m_map[ *it ] = new CTPTriggerThreshold( *it );
			
			//default pit values: used if threshold doesn't have a corresponding pit bit
			int pit_start=999;
			int pit_end=-999;
			

			for( vector< TrigConf::PIT* >::const_iterator pit = pit_vector.begin(); pit != pit_vector.end(); ++pit){
				if(((*pit)->thresholdName())==((*it)->name())){	  
					if((*pit)->pitNumber()<pit_start)
						pit_start=(*pit)->pitNumber();
					if((*pit)->pitNumber()>pit_end)
						pit_end=(*pit)->pitNumber();
				}
			}
			
			m_map[ *it ]->setStartBit( pit_start ); 
			m_map[ *it ]->setEndBit( pit_end );
			
			ostringstream message;
			message << "  --> Created object for threshold with name: " << setw( 10 ) << ( *it )->name() << " from TIP = " << setw( 3 ) << pit_start << " to TIP = " << setw( 3 ) << pit_end;
			m_logger.send( MSG::VERBOSE, message.str() );
			
			
		}
		
    m_logger.send( MSG::DEBUG, "Creating objects done" );
		
  }
	
	
	
	ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, 
														 const std::vector< TrigConf::TIP* >& tip_vector)
	: m_logger( "ThresholdMap" ) {
		
		m_logger.send( MSG::VERBOSE, "Created" );
		
		m_logger.send( MSG::DEBUG, "Creating CTPTriggerThreshold objects for each TrigConf::TriggerThreshold object..." );
		
		
		for ( vector< TrigConf::TriggerThreshold* >::const_iterator it = threshold_vector.begin(); it != threshold_vector.end(); ++it ) {
			
			m_map[ *it ] = new CTPTriggerThreshold( *it );
			
			//default pit values: used if threshold doesn't have a corresponding pit bit
			int tip_start=999;
			int tip_end=-999;
			
			for( vector< TrigConf::TIP* >::const_iterator tip = tip_vector.begin(); tip != tip_vector.end(); ++tip){
				if(((*tip)->thresholdName())==((*it)->name())){	  
					if((*tip)->tipNumber()<tip_start)
						tip_start=(*tip)->tipNumber();
					if((*tip)->tipNumber()>tip_end)
						tip_end=(*tip)->tipNumber();
				}
			}
			
			m_map[ *it ]->setStartBit( tip_start ); 
			m_map[ *it ]->setEndBit( tip_end );
			
			ostringstream message;
			message << "  --> Created object for threshold with name: " << setw( 10 ) << ( *it )->name() << " from TIP = " << setw( 3 ) << tip_start << " to TIP = " << setw( 3 ) << tip_end;
			m_logger.send( MSG::VERBOSE, message.str() );
			
			
		}

	m_logger.send( MSG::DEBUG, "Creating objects done" );
	
}



ThresholdMap::~ThresholdMap() {
	
	for ( map< const TrigConf::TriggerThreshold*, CTPTriggerThreshold* >::const_iterator it = m_map.begin();
			 it != m_map.end(); ++it ) {
		
		delete it->second;
		
	}
	
	m_logger.send( MSG::VERBOSE, "Deleted" );
	
}

CTPTriggerThreshold* ThresholdMap::decision( const TrigConf::TriggerThreshold* threshold ) const {
	
	return m_map.find( threshold )->second;
	
}

void ThresholdMap::clear() {
	
	m_logger.send( MSG::VERBOSE, "Clearing multiplicities in all CTPTriggerThreshold objects" );
	
	for ( map< const TrigConf::TriggerThreshold*, CTPTriggerThreshold* >::const_iterator it = m_map.begin();
			 it != m_map.end(); ++it ) {
		
		it->second->setValue( 0 );
		
	}
	
}

} // namespace LVL1CTP
