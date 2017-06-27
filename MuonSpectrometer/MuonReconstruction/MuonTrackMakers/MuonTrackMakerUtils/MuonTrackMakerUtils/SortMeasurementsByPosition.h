/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SORTMEASUREMENTSBYPOSITION_H
#define SORTMEASUREMENTSBYPOSITION_H

#include "TrkMeasurementBase/MeasurementBase.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include <functional>
#include <iostream>
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "CxxUtils/fpcompare.h"

namespace Muon {

  class DistanceToPars {
  public:
    DistanceToPars(const Trk::TrackParameters* pars) : m_pars(pars) {}
    double operator()(const Amg::Vector3D& pos){
      Amg::Vector3D difPos = pos-m_pars->position();
      double sign = difPos.dot(m_pars->momentum()) < 0 ? -1. : 1.;
      return difPos.mag()*sign;
    }
  private:
    const Trk::TrackParameters* m_pars;
  };

  class SortTSOSsByPos {
  public:
    SortTSOSsByPos(bool isEndcap) : m_isEndcap(isEndcap), m_dummyPosition(0.,0.,0.) {}
    double operator()(const Trk::TrackStateOnSurface* tsos1,const Trk::TrackStateOnSurface* tsos2){
      if( m_isEndcap ) return fabs(position(*tsos1).z()) < fabs(position(*tsos2).z());
      else             return position(*tsos1).perp() < position(*tsos2).perp();
    }
  private:
    Amg::Vector3D position( const Trk::TrackStateOnSurface& tsos ) {
      if( tsos.trackParameters() ) return tsos.trackParameters()->position();
      else if( tsos.measurementOnTrack() ) return tsos.measurementOnTrack()->globalPosition();
      return m_dummyPosition;
    }
    bool m_isEndcap;
    Amg::Vector3D m_dummyPosition;
  };

  class SortTSOSByDistanceToPars {
  public:
    SortTSOSByDistanceToPars(const Trk::TrackParameters* pars) : m_distToPars(pars), m_dummyPosition(0.,0.,0.) {}
    double operator()(const std::pair<bool,const Trk::TrackStateOnSurface*>& tsos1, const std::pair<bool,const Trk::TrackStateOnSurface*>& tsos2){
      return CxxUtils::fpcompare::less (m_distToPars( position(*tsos1.second) ), m_distToPars( position(*tsos2.second) ));
    }
  private:
    Amg::Vector3D position( const Trk::TrackStateOnSurface& tsos ) {
      if( tsos.trackParameters() ) return tsos.trackParameters()->position();
      else if( tsos.measurementOnTrack() ) return tsos.measurementOnTrack()->globalPosition();
      return m_dummyPosition;
    }
    DistanceToPars m_distToPars;
    Amg::Vector3D m_dummyPosition;
  };

  class SortTSOSs {
  public:
    bool operator()(const Trk::TrackStateOnSurface* tsos1, const Trk::TrackStateOnSurface* tsos2) const{
      //std::cout << " comparing " << tsos1 << "  " << tsos2 << std::endl; 
      if( !tsos1->trackParameters() ) {
	std::cout << "Muon::SortTSOSs: state 1 without parameters " << std::endl;
	return false;
      }
      if( !tsos2->trackParameters() ) {
	std::cout << "Muon::SortTSOSs: state 2 without parameters " << std::endl;
	return true;
      }
      //check between muon and non-muon hits first
      const Trk::MeasurementBase* meas1 = tsos1->measurementOnTrack();
      Identifier id1 = meas1 ? m_helperTool->getIdentifier(*meas1) : Identifier();
      
      const Trk::MeasurementBase* meas2 = tsos2->measurementOnTrack();
      Identifier id2 = meas2 ? m_helperTool->getIdentifier(*meas2) : Identifier();	  
      
      bool okId1 = id1.is_valid() && m_idHelperTool->isMuon(id1) ? true : false;
      bool okId2 = id2.is_valid() && m_idHelperTool->isMuon(id2) ? true : false;
      // put invalid hits and non-muon hits after valid muon hits
      if(  okId1 && !okId2 ) return true;
      if( !okId1 &&  okId2 ) return false;
      // get average direction of the 2 TSOSs
      Amg::Vector3D trackDir = tsos1->trackParameters()->momentum().unit();
      trackDir += tsos2->trackParameters()->momentum().unit();
      const Amg::Vector3D& pos1 = tsos1->trackParameters()->position();
      const Amg::Vector3D& pos2 = tsos2->trackParameters()->position();
      double dist = (pos2 - pos1).dot(trackDir);
      //std::cout<<"tsos1 is at "<<pos1<<", tsos2 at "<<pos2<<std::endl;
      
      //std::cout << "Muon::SortTSOSs: comparing " << tsos1 << "  " << tsos2 << " dist " << dist
      //	<</* "\n  mom1 " << tsos1->trackParameters()->momentum() <<*/ " \n dir1 " <<  tsos1->trackParameters()->momentum().unit()
      //	<</* "\n  mom2 " << tsos2->trackParameters()->momentum() <<*/ " \n dir2 " <<  tsos2->trackParameters()->momentum().unit()
      //        << "\n  trackDir " << trackDir << std::endl;
      if( fabs(dist) < 1e-5 ){
	//std::cout << " close hits " << tsos1->measurementOnTrack() << "   " << tsos2->measurementOnTrack()
        //          << "\ndist " << dist << std::endl;
	// both invalid or non-muon: consider them equal
	if ( !okId1 && !okId2 ) return false;
	// now we have 2 valid muon Ids
	bool measPhi1 = m_idHelperTool->measuresPhi(id1);
	bool measPhi2 = m_idHelperTool->measuresPhi(id2);
	//std::cout << " meas1 " << okId1 << "  " << measPhi1 << "   meas2 " << okId2 << "  " << measPhi2 << std::endl;
	// put phi measurement in front of eta measurements
	if(  measPhi1 && !measPhi2 ) return true;
	if( !measPhi1 &&  measPhi2 ) return false;
	// now both are phi or both are eta
	// decide on identifier (increasing away from IP within same technology)
	double inOrOut = (pos1 + pos2).dot(trackDir);
	//std::cout << "sortTSOSs(): inOrOut = " << inOrOut << std::endl;
	if ( inOrOut >= 0.0 ) {
	  return id1 < id2;
	} else {
	  return id1 > id2;
	}
          
	//std::cout << " no decision " << std::endl;
        // if we get here, there was no decision
	return false; //consider them equal
      }
      return dist > 0.;
    }

    SortTSOSs( const MuonEDMHelperTool* h, const MuonIdHelperTool*  idh ) : m_helperTool(h),m_idHelperTool(idh) {}
  
    const MuonEDMHelperTool* m_helperTool;
    const MuonIdHelperTool*  m_idHelperTool;
  };



  class SortMeasurementsByPosition : public std::binary_function<const Trk::MeasurementBase*,const Trk::MeasurementBase*,bool> {
  public:
    
    SortMeasurementsByPosition(bool hasEndcap=true) : 
      m_isEndcap(hasEndcap) {
    }
    
    bool operator()( const Trk::MeasurementBase* meas1, const Trk::MeasurementBase* meas2 ) {
      const Trk::MeasurementBase* m1 = getMeas(meas1);
      const Trk::MeasurementBase* m2 = getMeas(meas2);
      
      double d1 = m_isEndcap ? fabs(m1->globalPosition().z()) : fabs(m1->globalPosition().perp());
      double d2 = m_isEndcap ? fabs(m2->globalPosition().z()) : fabs(m2->globalPosition().perp());
      bool result = d1 < d2;
/*       bool result2 = fabs(m1->globalPosition().perp()) < fabs(m2->globalPosition().perp());  */
/*       if( m_isEndcap ) result2 = fabs(m1->globalPosition().z()) < fabs(m2->globalPosition().z());  */

/*       std::cout << " meas 1 " << meas1 << " d " << d1 << "   meas2 " << meas2 << " d " << d2  */
/* 		<< "   result " << result << " result2 " << result2 << std::endl; */
/*       if( result != result2 ) std::cout << " ERROR these number should be the same!!! " << std::endl; */
      return result;
    }
  private:
    // hack to correctly sort competing rots
    const Trk::MeasurementBase* getMeas( const Trk::MeasurementBase* meas ) const {
      const CompetingMuonClustersOnTrack* cm = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
      if( cm ) {
	if( cm->numberOfContainedROTs() != 0 ) return &cm->rioOnTrack( cm->indexOfMaxAssignProb() );
      }
      return meas;
    }


    bool m_isEndcap;
  };

}

#endif
