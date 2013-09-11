/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <sstream>
#include <iomanip>
#include <cstdlib>

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/CTPTriggerThreshold.h"

using namespace std;

namespace LVL1CTP {

  ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector)
    : m_logger( "ThresholdMap" ) {

    m_logger.send( MSG::VERBOSE, "Created" );

    m_logger.send( MSG::DEBUG, "Creating CTPTriggerThreshold objects for each TrigConf::TriggerThreshold object..." );
    unsigned int pit_start = 0;

    for ( vector< TrigConf::TriggerThreshold* >::const_iterator it = threshold_vector.begin(); it != threshold_vector.end(); ++it ) {

      m_map[ *it ] = new CTPTriggerThreshold( *it );
      unsigned int pit_end = pit_start + abs( ( *it )->cableStart() - ( *it )->cableEnd() );
      m_map[ *it ]->setPitStart( pit_start );
      m_map[ *it ]->setPitEnd( pit_end );
      
      ostringstream message;
      message << "  --> Created object for threshold with name: " << setw( 10 ) << ( *it )->name() << " from PIT = " << setw( 3 ) << pit_start << " to PIT = " << setw( 3 ) << pit_end;
      m_logger.send( MSG::VERBOSE, message.str() );
      
      pit_start = pit_end + 1;
    }
    
    m_logger.send( MSG::DEBUG, "Creating objects done" );

  }

  ThresholdMap::ThresholdMap( const std::vector< TrigConf::TriggerThreshold* >& threshold_vector, 
			      const std::vector< TrigConf::PIT* >& pit_vector)
    : m_logger( "ThresholdMap" ) {

    m_logger.send( MSG::VERBOSE, "Created" );

    m_logger.send( MSG::DEBUG, "Creating CTPTriggerThreshold objects for each TrigConf::TriggerThreshold object..." );
    
    for ( vector< TrigConf::TriggerThreshold* >::const_iterator it = threshold_vector.begin(); it != threshold_vector.end(); ++it ) {

      m_map[ *it ] = new CTPTriggerThreshold( *it );
      
      //default pit values: used if threshold doesn't have a corresponding pit bit
      int pit_start=999;
      int pit_end=-999;
									
      //the below doesn't work for some reason
      //for( vector< TrigConf::PIT* >::const_iterator pit = pit_vector().begin(); pit != pit_vector().end(); ++pit){
	for(unsigned int i=0; i<pit_vector.size(); i++){
	  TrigConf::PIT *pit=pit_vector.at(i);
	  if((pit->thresholdName())==((*it)->name())){	  
	    if(pit->pitNumber()<pit_start)
	      pit_start=pit->pitNumber();
	    if(pit->pitNumber()>pit_end)
	      pit_end=pit->pitNumber();
	  }
	}
	
	m_map[ *it ]->setPitStart( pit_start );
	m_map[ *it ]->setPitEnd( pit_end );
	
	ostringstream message;
	message << "  --> Created object for threshold with name: " << setw( 10 ) << ( *it )->name() << " from PIT = " << setw( 3 ) << pit_start << " to PIT = " << setw( 3 ) << pit_end;
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
