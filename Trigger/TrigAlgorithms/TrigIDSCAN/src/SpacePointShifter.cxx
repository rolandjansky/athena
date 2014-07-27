/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// IdScan: SpacePointShifter
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

// SpacePoint creation
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"

// IdScan stuff
#include "TrigIDSCAN/RoI_Shape.h"
#include "TrigIDSCAN/SpacePointShifter.h"
#include "TrigIDSCAN/shiftRoI.h"

using namespace std;

namespace TrigIdScan{


SpacePointShifter::SpacePointShifter( MsgStream&           log,
				      int                  outputLevel,
				      ToolHandle<ISpacePointProvider>  spacePointProvider,
				      RoI_Shape*           roiShape,
				      double               /*etaRoI*/,
				      double               phiRoI,
				      double x,  double y, bool translateOnly) :
  m_athenaLog( log )
{
  
  if (outputLevel <= MSG::DEBUG) m_athenaLog << MSG::DEBUG << "SpacePointShifter: in constructor" << endreq;
  
  //   Retrieve the common SpacePoints 
  //
  // phiRoI always comes in from -pi->+pi

  double roiPhiMin = phiRoI - roiShape->phiHalfWidth();
  double roiPhiMax = phiRoI + roiShape->phiHalfWidth();

  // region selector needs both the min and max phi to be in the range -pi to +pi
  // double regSel_roiPhiMin = roiPhiMin;
  // if ( roiPhiMin < -M_PI ) regSel_roiPhiMin += 2*M_PI;
  // double regSel_roiPhiMax = roiPhiMax;
  // if ( roiPhiMax >  M_PI ) regSel_roiPhiMax -= 2*M_PI;

  // onlineSPs are in the range 0 to 2pi
  // also offlineSPs; the conversion is done in the OfflineSpacePointProviderTool
  //  if ( roiPhiMin < 0 ) roiPhiMin += 2*M_PI;
  //  if ( roiPhiMax < 0 ) roiPhiMax += 2*M_PI;
  if ( roiPhiMin < -M_PI ) roiPhiMin += 2*M_PI;
  if ( roiPhiMax >  M_PI ) roiPhiMax -= 2*M_PI;
  if ( roiPhiMin < roiPhiMax )  m_boundary = false;
  else                          m_boundary = true;

  //  double roiEtaMin = etaRoI - roiShape->etaHalfWidth();
  //  double roiEtaMax = etaRoI + roiShape->etaHalfWidth();

  //  cout << "TrigIDSCAN etaMin=" << roiEtaMin << "\tetaMax=" << roiEtaMax << endl;
  //  cout << "TrigIDSCAN phiMin=" << roiPhiMin << "\tphiMax=" << roiPhiMax << endl;

  //
  //   Get the SpacePoints 
  //

  std::vector< TrigSiSpacePoint* > spVec;

  m_xshift = 0;
  m_yshift = 0;


  std::vector< TrigSiSpacePoint* > spVecTmp=spacePointProvider->getSpacePoints(true, true); 
 
  int nSPtmp = spVecTmp.size();
    
  for ( long i=0; i < nSPtmp ; ++i ) {
    spVec.push_back( new TrigSiSpacePoint( *(spVecTmp[i]) ) ); 
    m_spVec.push_back(spVec[i]);

    if ( m_spMap.find(spVec[i])==m_spMap.end() ) { 
      m_spMap[spVec[i]] = spVecTmp[i]; 
      //      cout << "MAP:: " << hex << spVecTmp[i] << " -> " << hex << spVec[i] << dec << endl; 
    }
  }
  
  // Now do the shift in y and copy the shifted spacepoints to a new 
  // IdScanSpPoint vector for use in the rest of IdScan

  //  cout << "SpacePointShifter translateOnly=" << translateOnly << endl;

  if ( translateOnly ) { 
    translate(spVec, x, y);
    m_xshift = x;
    m_yshift = y;
    if (outputLevel <= MSG::DEBUG) m_athenaLog << MSG::DEBUG << " Translating spacepoints by (x,y)=" << x << ",\t" << y << endreq;    
    
    m_athenaLog << MSG::DEBUG << " spVec.size() = " << spVec.size() << endl;
  }
  else {
    m_xshift = shift(spVec);
    if (outputLevel <= MSG::DEBUG) m_athenaLog << MSG::DEBUG << " Shifted spacepoints by x=" << m_xshift << endreq;

    cout << "spVec.size() " << spVec.size() << endl;
  }

  double phi;
  int nSP = spVec.size();

  if ( !this->isInPhiBoundary() )  {
    for ( long i=0; i < nSP; ++i )     {
      phi = (spVec[i])->phi();
      if (outputLevel <= MSG::VERBOSE) m_athenaLog << MSG::VERBOSE << " SP " << i << " phi " << phi 
						   << " roiphimin/max " << roiPhiMin << " / " << roiPhiMax 
						   << endreq; 
      if ( phi > roiPhiMin && phi < roiPhiMax ) {
	if (outputLevel <= MSG::VERBOSE) m_athenaLog << MSG::VERBOSE << " SP " << i << " phi " 
						   << phi << " roiphimin/max " << roiPhiMin 
						   << " / " << roiPhiMax << " in roi" << endreq; 
	m_commonSPs.push_back( spVec[i] );
      }
    }
  }
  else {
    for ( long i=0; i < nSP; ++i ) {
      phi = (spVec[i])->phi();
      if (outputLevel <= MSG::VERBOSE) m_athenaLog << MSG::VERBOSE << " SP " << i << " phi " << phi 
						   << " roiphimin/max " << roiPhiMin << " / " 
						   << roiPhiMax << endreq; 
      if ( phi > roiPhiMin || phi < roiPhiMax )  {
	if (outputLevel <= MSG::VERBOSE) m_athenaLog << MSG::VERBOSE << " SP " << i << " phi " 
						   << phi << " roiphimin/max " << roiPhiMin 
						   << " / " << roiPhiMax << " in roi" << endreq; 
	m_commonSPs.push_back( spVec[i] );
      }
    }
  }
  if (outputLevel <= MSG::DEBUG) m_athenaLog << MSG::DEBUG << "REGTEST / Finally: " << this->HitCount() 
					     << " space points inside RoI" << endreq;
  
  spVec.clear();
}
  
//-------------------------------------------------------------------------



// return the shifted point corresponding to the given unshifted point

TrigSiSpacePoint*  SpacePointShifter::mapPoint(TrigSiSpacePoint* sp) const { 
  
  std::map<TrigSiSpacePoint*,TrigSiSpacePoint*>::const_iterator spitr(m_spMap.find(sp));
  
  if ( spitr!=m_spMap.end() ) return spitr->second;
  else { 
    m_athenaLog << MSG::WARNING << " no spacepoint in map " << sp << endreq;    
    return NULL;
  }  
}
  
  
  


// loop over a vector of IdScanSpPoints and form a simple best fit
// straight line to all the points within the road which maximises
// the number of hits in the road assuming a straight track.
// NB: Can be very slow, since many combinations are used, and there 
// is lots of redundant copying.
// If execution time is an issue, the spacepoint class and the track 
// road finder should be reimplemented to avoid any uneccessary copying.
 

double SpacePointShifter::shift(std::vector<TrigSiSpacePoint*>& sp) {
  
  std::vector<TrigSiSpacePoint*>::iterator spItr(sp.begin());
  std::vector<TrigSiSpacePoint*>::iterator spEnd(sp.end());
  
  std::vector<point> p;
  
  // get all the points 
  
  for ( int i=0 ; spItr!=spEnd ; ++i, ++spItr ) {
    p.push_back( point( (*spItr)->r(), (*spItr)->phi(), (*spItr)->z(), (*spItr)->layer() ) );
  }

  // calculate all the possible track combinations, and pick the best one 

  TrigIdScan::TrackShift  trackshift(p);

  double xvtx =  trackshift.doShift();

  // shift the points
  
  if ( xvtx != 0 ) { 
    spItr = sp.begin();
    for ( long i=0 ; spItr!=spEnd ; ++i, ++spItr ) {
      p[i].shiftxy(xvtx);
      // put shifted values back into original vector 
      (*spItr)->r(p[i].r);
      // make sure the phi is from 0 to 2pi
      //      (*spItr)->phi(   ( p[i].phi<0 ? p[i].phi+2*M_PI : p[i].phi )   );
      (*spItr)->phi( p[i].phi );
    }
  }  
  

  return xvtx;
}


void SpacePointShifter::translate(std::vector<TrigSiSpacePoint*>& sp, double x, double y) 
{  
  // shift the points
  
  std::vector<TrigSiSpacePoint*>::iterator spItr(sp.begin());
  std::vector<TrigSiSpacePoint*>::iterator spEnd(sp.end());
  if ( x!=0 || y!=0 ) { 
    spItr = sp.begin();
    for ( long i=0 ; spItr!=spEnd ; ++i, ++spItr ) {
      point p( (*spItr)->r(), (*spItr)->phi(), (*spItr)->z(), (*spItr)->layer() );
      p.shiftxy( x, y );
      // put shifted values back into original vector 
      (*spItr)->r( p.r );
      // make sure the phi is from 0 to 2pi
      //      (*spItr)->phi(   ( p.phi<0 ? p.phi+2*M_PI : p.phi )   );
      (*spItr)->phi( p.phi );
    }
  }    
}

  
} //namespace TrigIdScan
