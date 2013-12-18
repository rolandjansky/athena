/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedRawHits.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"

#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"

#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"

#include <sstream>


namespace MuonCalib {

  MuonCalibExtendedRawHits::MuonCalibExtendedRawHits() : m_adcThreshold(50.) {}

  MuonCalibExtendedRawHits::MuonCalibExtendedRawHits( const MuonCalibRawHitCollection& rawHits,
						      const std::vector<const MuonCalibExtendedSegment*>& segments,
						      const std::vector<const MuonCalibExtendedTrack*>& tracks ) :
    m_adcThreshold(70.)  
  {
    MuonFixedIdManipulator idManip;
    // loop over MDT raw hits and 
    MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit mdtIt = rawHits.rawMdtHitCollectionBegin();
    MuonCalibRawHitCollection::MuonCalibRawMdtHitVecCit mdtIt_end = rawHits.rawMdtHitCollectionEnd();
    for( ;mdtIt != mdtIt_end; ++mdtIt ){

      const MuonCalibRawMdtHit* rawHit = *mdtIt;
      MuonFixedId id = rawHit->identify();
      // create ChamberId + StationIndex
      MuonFixedId chId                                  = idManip.chamberIdentifier(id);
      MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);
      
      MdtRegionInfo& chInfo = m_mdtChamberIdMap[chId];
      ExtendedChannelInfo& channelChInfo = chInfo.hits[id];
      MdtRegionInfo& stInfo = m_mdtChamberIndexMap[stationIndex];      
      ExtendedChannelInfo& channelStInfo = stInfo.hits[id];

      channelChInfo.hits.push_back( rawHit );
      channelStInfo.hits.push_back( rawHit );

      if( rawHit->adcCount() > m_adcThreshold ) {
	++chInfo.nhitsAboveAdcThreshold;
	++stInfo.nhitsAboveAdcThreshold;
      }
      // only increase the hit counters once
      bool isFirst = true;
      // loop over tracks and check whether hit is 'on-track'
      std::vector<const MuonCalibExtendedTrack*>::const_iterator trit = tracks.begin();
      std::vector<const MuonCalibExtendedTrack*>::const_iterator trit_end = tracks.end();
      for( ;trit!=trit_end;++trit ){
	
	// look for id in map
	const IdHitMap& idHitMap = (*trit)->idHitMap();
	IdHitMap::const_iterator pos = idHitMap.find(id);
	if( pos == idHitMap.end() ) continue;
	
	// if we get here the hit was found, now check that it is a hit on track
	const MuonCalibHit_E& hit = *pos->second;
	if( hit.type() < 4 ) {
	  if( isFirst ) {
	    ++chInfo.nhitsOnTrack;
	    ++stInfo.nhitsOnTrack;
	    isFirst = false;
	  }
	  (*trit)->rawHitAssociationMap().addMdtRawHit(rawHit);
	  channelChInfo.associatedTracks.push_back( *trit );
	  channelStInfo.associatedTracks.push_back( *trit );
	}
      }

      // only increase the hit counters once
      isFirst = true;
      // loop over segments and check whether hit is 'on-segment'
      std::vector<const MuonCalibExtendedSegment*>::const_iterator sit = segments.begin();
      std::vector<const MuonCalibExtendedSegment*>::const_iterator sit_end = segments.end();
      for( ;sit!=sit_end;++sit ){
	
	// look for id in segment
	if( !(*sit)->idSet().count(id) ) continue;
	
	if( isFirst ) {
	  ++chInfo.nhitsOnSegment;
	  ++stInfo.nhitsOnSegment;
	  isFirst = false;
	}
	(*sit)->rawHitAssociationMap().addMdtRawHit(rawHit);
	channelChInfo.associatedSegments.push_back( *sit );
	channelStInfo.associatedSegments.push_back( *sit );
      }
    }
    
    
  }
  
  std::string MuonCalibExtendedRawHits::dumpMdtHits() const {
    std::ostringstream sout;
    MuonFixedIdPrinter printer;

    sout << " number of MDT chambers with hits: " << m_mdtChamberIdMap.size() << std::endl;
    MdtHitChamberIdMap::const_iterator chIt = m_mdtChamberIdMap.begin();
    MdtHitChamberIdMap::const_iterator chIt_end = m_mdtChamberIdMap.end();
    for( ;chIt!=chIt_end;++chIt ){
      if( chIt->second.nhitsAboveAdcThreshold < 3 ) continue;
      const MuonFixedId& chId = chIt->first;
      sout << "    " << std::setw(20) << printer.chamberIdentifier(chId)
	   << "    nhits " << std::setw(3) << chIt->second.hits.size() 
	   << "    above ADC cut " << std::setw(3) << chIt->second.nhitsAboveAdcThreshold
	   << "    on segment " << std::setw(3) << chIt->second.nhitsOnSegment
	   << "    on track " << std::setw(3) << chIt->second.nhitsOnTrack;

      // look at next iterator to decide whether to add endl or not
      MdtHitChamberIdMap::const_iterator chIt_next = chIt; ++chIt_next;
      if( chIt_next != chIt_end ) sout << std::endl;
    }
    return sout.str();
  }
}
