/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_TimingTool.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

namespace Muon {

  RPC_TimingTool::RPC_TimingTool(const std::string& t, const std::string& n, const IInterface* p) :
      AthAlgTool(t,n,p) {
    declareInterface<IMuonHitTimingTool>(this);
  }

  StatusCode RPC_TimingTool::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  IMuonHitTimingTool::TimingResult RPC_TimingTool::calculateTimingResult( const std::vector<const MuonClusterOnTrack*>& hits ) const {
    
    // sanity check
    if( hits.empty() )     return TimingResult();
    
    ATH_MSG_DEBUG(" calculating timing for hits " << hits.size() );

    std::vector<int> histogram(100,0);
    float minTime = -100.;
    float binwidth = 2*std::abs(minTime)/histogram.size();
    float invbinwidth = 1./binwidth;
    
    
    for( auto hit : hits ){
      const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(hit);
      if( !rpc ) continue;
      int bin = invbinwidth*(rpc->time()-minTime);
      ATH_MSG_DEBUG(m_idHelperSvc->toString(hit->identify()) << " time " << rpc->time() << " bin " << bin );
      if( bin < 0 || bin >= (int)histogram.size() ) continue;
      ++histogram[bin];
    }
    int maxbin = -1;
    int maxval = 0;
    for( unsigned int i=0; i<histogram.size();++i ){
      int val = histogram[i];
      if( val > maxval ){
        maxbin = i;
        maxval = val;
      }
    }
    if( maxbin != -1 ){      
      double time = 0.;
      unsigned int nhits = 0;
      double error = getError(*hits.front());
      for( auto hit : hits ){
        const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(hit);
        if( !rpc ) continue;
        int bin = invbinwidth*(rpc->time()-minTime);
        ATH_MSG_DEBUG(m_idHelperSvc->toString(hit->identify()) << " time " << rpc->time() << " bin " << bin );
        // select hits in the max bin or the two neighbouring ones
        if( bin > maxbin+1 || bin < maxbin-1 ) continue;
        time += rpc->time();
        ++nhits;
      }
      if( nhits == 0 ) return TimingResult();
      time /= nhits;
      ATH_MSG_DEBUG( " final time " << time << " error " << error );
      return TimingResult(true,time,error);
    }
    return TimingResult();
  }

  double RPC_TimingTool::getError(const MuonClusterOnTrack& rpcRIO) const { 

    // S. Spagnolo: Aug. 5th 2011 - implementation for rel 17 reprocessing (based on slide 8 and 9 of 
    // https://indico.cern.ch/getFile.py/access?contribId=6&resId=0&materialId=slides&confId=147791
    // ... no calibration offline yet, no correction for signal propagation time)
    Identifier id = rpcRIO.identify();
    int measPhi = m_idHelperSvc->measuresPhi(id);
    MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
    if( stIndex == MuonStationIndex::BO) {
      // outer layer station => high pt 
      if (measPhi==1) return 5.10;
      return 4.84;     
    }else{
      // pivot plane of Middle station dbR=2 or low pT confirm dbR=1 but names like "BMS", "BML", etc
      if (measPhi==1) return 5.04;
      else return 4.18;      
    }
  }

}
