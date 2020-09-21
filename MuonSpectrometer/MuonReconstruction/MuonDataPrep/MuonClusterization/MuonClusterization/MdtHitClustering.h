/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTHITCLUSTERING_H
#define MDTHITCLUSTERING_H

#include <vector>
#include <algorithm>
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepData.h"

#include "Identifier/Identifier.h"

class MdtIdHelper;
namespace MuonGM {
  class MuonDetectorManager;
  class MdtReadoutElement;
}

namespace Muon {
  
  class MdtPrepData;
  
  struct MdtCluster {
    
    typedef std::vector< const MdtPrepData* > HitList;
    typedef HitList::iterator                 HitIt;
    typedef HitList::const_iterator           HitCit;
    MdtCluster() {}
    
    void add( const MdtPrepData* prd ) { hitList.push_back(prd); }
    void addSecond( const MdtPrepData* prd ) { 
      HitIt it = hitList.begin();
      HitIt it_end = hitList.end();
      for( ;it!=it_end;++it ){
	if( (*it)->identify() != prd->identify() ) continue;

	// selected on of the two
	if( (*it)->status() == MdtStatusDriftTime ) {
	  if( prd->status() == MdtStatusDriftTime ) {
	    //std::cout << " found two hits inside the drift time spectrum!! " << prd->tdc() << " " << (*it)->tdc() << std::endl;
	    if( prd->tdc() < (*it)->tdc() ) *it = prd;
	  }
	}else if( prd->status() == MdtStatusDriftTime ){
	  // replace PRD
	  *it = prd;
	}
 	return;
      }
      std::cout << "could not find duplicate prd " << std::endl;
      add(prd);
    }
    void merge( MdtCluster& cluster ) {
      hitList.insert(hitList.end(),cluster.hitList.begin(),cluster.hitList.end());
      cluster.hitList.clear();
    }
    
    bool active() const { return !hitList.empty(); }

    void isolation( int& all, int& inTime, int& inTimeAndBefore ) const {
      all = hitList.size();
      inTime = 0;
      inTimeAndBefore = 0;
      HitCit it = hitList.begin();
      HitCit it_end = hitList.end();
      for( ;it!=it_end;++it ){
	if( (*it)->adc() > 50 ){
	  if( (*it)->status() == Muon::MdtStatusDriftTime ) {
	    ++inTime;
	    ++inTimeAndBefore;
	  }else if( (*it)->status() == Muon::MdtStatusBeforeSpectrum ) {
	    ++inTimeAndBefore;	    
	  }
	}
      }
    }
    
    HitList hitList;
  };
  
  struct MdtHitClustering {

    struct Id {
      Id( int l, int t ) : lay(l),tube(t) {}
      int lay;
      int tube;
    };

    typedef std::vector< std::vector<int> > HitPattern;

    MdtHitClustering( const MdtIdHelper& mdtIdHelper, const MuonGM::MuonDetectorManager& detMgr ) : 
      m_mdtIdHelper(&mdtIdHelper), m_detMgr(&detMgr), detEl1(0), detEl2(0), nlay(0), ntube1(0), ntube2(0), nml(0), debug(false)
    {

    }

    bool init( const Identifier& chid );

    void neighbouringTubes( const Id& id, std::vector<Id>& neighbours ) const;

    bool cluster( const MdtPrepDataCollection& col );

    void dump() const;

    void layers( const MdtCluster& cl, int& all, int& inTime, int& inTimeAndBefore ) const;

    const MdtIdHelper* m_mdtIdHelper;
    const MuonGM::MuonDetectorManager* m_detMgr; // cannot use ReadCondHandleKey since no athena component
    const MuonGM::MdtReadoutElement* detEl1;
    const MuonGM::MdtReadoutElement* detEl2;
    unsigned int nlay;
    unsigned int ntube1;
    unsigned int ntube2;
    unsigned int nml;
    HitPattern hitPattern;

    std::vector< MdtCluster > clusters;
    std::vector<int> staggeringCorrection;    
    bool debug;
  };

}



#endif
