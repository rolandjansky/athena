/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//   ShifterTool.cxx        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@hep.ucl.ac.uk)    
//
//   $Id: ShifterTool.cxx, v0.0   Wed Jun 11 15:09:03 BST 2008 sutt

#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/StatusCode.h" 
#include "StoreGate/StoreGateSvc.h"

#include "TrigIDSCAN/ShifterTool.h"
#include "TrigIDSCAN/shiftRoI.h"

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>

// SpacePoint creation
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrigSiSpacePointTool/ISpacePointProvider.h"


ShifterTool::ShifterTool( const std::string& type, const std::string& name, const IInterface* parent) : 
  AlgTool( type, name, parent ),
  m_athenaLog( msgSvc(), name ),
  m_translateOnly(false),
  m_xShift(0), m_yShift(0), m_shifterz(0), m_boundary(false)
{
  declareInterface< IShifterTool >( this );
  declareProperty( "PixelSP_ContainerName", m_pixelSpName = "TrigPixelSpacePoints" );
  declareProperty( "SCT_SP_ContainerName",  m_sctSpName = "TrigSCT_SpacePoints" );
  declareProperty( "UsePixels",  m_usePixels = true );
  declareProperty( "UseSCT",     m_useSCT    = true );
  declareProperty( "MaxPIXOccupancy",  m_maxPIXOccupancy = 1000 );
  declareProperty( "MaxSCTOccupancy",  m_maxSCTOccupancy = 1000 );
  declareProperty( "ShiftRoadRadius",  m_shiftRoadRadius = 5. );
}


StatusCode ShifterTool::initialize()
{
  StatusCode sc = AlgTool::initialize();
  // MsgStream athenaLog( msgSvc(), name() );
  
  m_athenaLog << MSG::INFO << " initialise()" << endreq;

  sc = service( "StoreGateSvc", m_StoreGate );
  if (sc.isFailure()) {
    m_athenaLog << MSG::FATAL 
		<< "Unable to retrieve StoreGate service" << endreq;
    return sc;
  }
 
  return sc;
}


StatusCode ShifterTool::finalize()
{
  clear();
  //  MsgStream athenaLog( msgSvc(), name() );
  m_athenaLog << MSG::INFO << " finalize()" << endreq;
  return AlgTool::finalize();
}



StatusCode ShifterTool::execute(ToolHandle<ISpacePointProvider>  spacePointProvider,
			    double /*etaMinus*/, double /*etaPlus*/,
			    double phiMin,   double phiMax, double& ret)
{
  m_athenaLog << MSG::DEBUG << " execute()" << endreq;

  ret=0.0;

  //  make sure phi is within the appropriate range
  if ( phiMin < -M_PI ) phiMin += 2*M_PI;
  if ( phiMax >  M_PI ) phiMax -= 2*M_PI;
  if ( phiMin < phiMax )  m_boundary = false;
  else                    m_boundary = true;

  // clear everything if it hasn't been cleared already
  clear();

  //   Get the copy of the SpacePoints
  std::vector< TrigSiSpacePoint* > _spVec;
  std::vector< TrigSiSpacePoint* > spVecTmp;

  std::vector<int> listOfPixIds;
  std::vector<int> listOfSctIds;

  int nCrowdedPIX = 0;
  int nCrowdedSCT = 0;

  StatusCode sc=StatusCode::SUCCESS;

  // If the phi range is (-pi,pi) and eta range is beyond 2.5, this is full-scan.
  sc = spacePointProvider->fillCollections(m_usePixels,m_useSCT,listOfPixIds,listOfSctIds);
  
  if(sc.isRecoverable()||sc.isSuccess())
    {
      const TrigSiSpacePointContainer* pixCont;
      const TrigSiSpacePointContainer* sctCont;

      if(m_usePixels)
	{
	  StatusCode scIDC=m_StoreGate->retrieve(pixCont,m_pixelSpName);
	  if(scIDC.isFailure())
	    {
	      m_athenaLog << MSG::WARNING << "Trig SP Pixel container " <<m_pixelSpName
			  <<" not found"<<endreq;
	      return scIDC;
	    }
	  for(std::vector<int>::iterator idIt=listOfPixIds.begin();idIt!=listOfPixIds.end();++idIt)
	    {
	      TrigSiSpacePointContainer::const_iterator spCollIt=pixCont->indexFind((*idIt));
	      if(spCollIt==pixCont->end()) continue;
	      if((*spCollIt)==NULL) continue;
	      if( (*spCollIt)->size() > (unsigned int)m_maxPIXOccupancy ) { nCrowdedPIX++; continue; }
	      for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();
		  spIt!=(*spCollIt)->end();++spIt)
		{
		  spVecTmp.push_back((*spIt));
		}
	    }
	}
      if(m_useSCT)
	{
	  StatusCode scIDC=m_StoreGate->retrieve(sctCont,m_sctSpName);
	  if(scIDC.isFailure())
	    {
	      m_athenaLog << MSG::WARNING << "Trig SP SCT container " <<m_sctSpName
			  <<" not found"<<endreq;
	      return scIDC;
	    }
	  for(std::vector<int>::iterator idIt=listOfSctIds.begin();idIt!=listOfSctIds.end();++idIt)
	    {
	      TrigSiSpacePointContainer::const_iterator spCollIt=sctCont->indexFind((*idIt));
	      if(spCollIt==sctCont->end()) continue;
	      if((*spCollIt)==NULL) continue;
	      if( (*spCollIt)->size() > (unsigned int)m_maxSCTOccupancy ) { nCrowdedSCT++; continue; }
	      for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();
		  spIt!=(*spCollIt)->end();++spIt)
		{
		  spVecTmp.push_back((*spIt));
		}
	    }
	}
    }

  if ( nCrowdedPIX > 0 )
    m_athenaLog << MSG::DEBUG << "Dropped SPs from " << nCrowdedPIX << " Pixel modules" << endreq;
  if ( nCrowdedSCT > 0 )
    m_athenaLog << MSG::DEBUG << "Dropped SPs from " << nCrowdedSCT << " SCT modules" << endreq;

  if ( spVecTmp.size()>unsigned(m_maxPIXOccupancy+m_maxSCTOccupancy)  ) return  sc;

  /*
  std::vector< TrigSiSpacePoint* > spVecTmp=spacePointProvider->getSpacePoints(etaMinus, etaPlus, 
									       -1*M_PI, M_PI, true, true); 
  */
  std::map<TrigSiSpacePoint*, TrigSiSpacePoint* > spMap;

  int nSPtmp = spVecTmp.size();
    
  for ( long i=0; i < nSPtmp ; ++i ) {
    _spVec.push_back( new TrigSiSpacePoint( *(spVecTmp[i]) ) ); 
  }
  

  // Now do the shifts in x and y and copy the shifted spacepoints to a new 
  // IdScanSpPoint vector for use in the rest of IdScan

  if ( !m_translateOnly ) {  

    m_yShift = 0;
    m_xShift = getShift(_spVec);    
    
    m_athenaLog << MSG::DEBUG << " Calculated shift for spacepoints x=" << m_xShift << endreq;
    //    m_athenaLog << MSG::INFO << " _spVec.size() " << _spVec.size() << endreq;
  }
  else {
    m_athenaLog << MSG::DEBUG << " Translating spacepoints by (x,y)=" << m_xShift << ",\t" << m_yShift << endreq;    
  }

  translate(_spVec);

  double phi;
  int nSP = _spVec.size();

  // add spacepoints to the vector   
  if ( !m_boundary )  {
    for ( long i=0; i < nSP; ++i ) {
      phi = (_spVec[i])->phi();
      if ( phi > phiMin && phi < phiMax )  { 
	m_spVec.push_back( _spVec[i] );
	if ( m_spMap.find(_spVec[i])==m_spMap.end() ) m_spMap[_spVec[i]] = spVecTmp[i]; 
      }
      else delete _spVec[i];
    }
  }
  else {
    for ( long i=0; i < nSP; ++i ) {
      phi = (_spVec[i])->phi();      
      if ( phi > phiMin || phi < phiMax )  { 
	m_spVec.push_back( _spVec[i] ); 
	if ( m_spMap.find(_spVec[i])==m_spMap.end() ) m_spMap[_spVec[i]] = spVecTmp[i]; 
      }
      else delete _spVec[i];
    }
  }
  
  
#if 0
  // print out spacepoints if needed
  for ( long i=0; i<m_spVec.size() ; ++i )  {  
    m_athenaLog << MSG::VERBOSE << " SP " << i << " phi " << m_spVec[i]->phi() 
		<< " phimin/max " << phiMin << " / " << phiMax << endreq;
  }
#endif  
  
  m_athenaLog << MSG::DEBUG << "REGTEST / Finally: " << m_spVec.size() 
	      << " space points inside RoI" << endreq;  

  ret= m_xShift;
  return sc;
}





void ShifterTool::clear() { 

  if ( ! m_spVec.empty() ) {

    std::vector<TrigSiSpacePoint*>::const_iterator  spItr( m_spVec.begin() );
    std::vector<TrigSiSpacePoint*>::const_iterator  spEnd( m_spVec.end() );
  
    while (  spItr!=spEnd  )  delete *(spItr++);
  
    // clear map of shifted and unshifted points
    m_spVec.clear();
    m_spMap.clear();
  }

}



TrigSiSpacePoint* ShifterTool::mapPoint(TrigSiSpacePoint* sp) const { 
  
  std::map<TrigSiSpacePoint*,TrigSiSpacePoint*>::const_iterator spitr(m_spMap.find(sp));
  
  if ( spitr!=m_spMap.end() ) return spitr->second;
  else { 
    m_athenaLog << MSG::WARNING << " no spacepoint in map " << sp << endreq;    
    return NULL;
  }  
  
}




void ShifterTool::translate(std::vector<TrigSiSpacePoint*>& sp) 
{  
  // shift the points
  
  std::vector<TrigSiSpacePoint*>::iterator spItr(sp.begin());
  std::vector<TrigSiSpacePoint*>::iterator spEnd(sp.end());
  if ( m_xShift!=0 || m_yShift!=0 ) { 
    spItr = sp.begin();
    for ( long i=0 ; spItr!=spEnd ; ++i, ++spItr ) {
      point p( (*spItr)->r(), (*spItr)->phi(), (*spItr)->z(), (*spItr)->layer() );
      p.shiftxy( m_xShift, m_yShift );
      // put shifted values back into original vector 
      (*spItr)->r( p.r );
      // make sure the phi is from 0 to 2pi
      //      (*spItr)->phi(   ( p.phi<0 ? p.phi+2*M_PI : p.phi )   );
      (*spItr)->phi( p.phi );
    }
  }    
}





// straight line to all the points within the road which maximises
// the number of hits in the road assuming a straight track.
// NB: Can be very slow, since many combinations are used, and there 
// is lots of redundant copying.
// If execution time is an issue, the spacepoint class and the track 
// road finder should be reimplemented to avoid any uneccessary copying.
 

double  ShifterTool::getShift(std::vector<TrigSiSpacePoint*>& sp) {
  
  std::vector<TrigSiSpacePoint*>::iterator spItr(sp.begin());
  std::vector<TrigSiSpacePoint*>::iterator spEnd(sp.end());
  
  std::vector<point> p;
  
  // get all the points 
  
  for ( int i=0 ; spItr!=spEnd ; ++i, ++spItr ) {
    p.push_back( point( (*spItr)->r(), (*spItr)->phi(), (*spItr)->z(), (*spItr)->layer() ) );
  }

  // calculate all the possible track combinations, and pick the best one 

  TrigIdScan::TrackShift  trackshift(p);

  double xvtx =  trackshift.doShift(m_shiftRoadRadius);

  m_shifterz = trackshift.road().zvtx();

  return xvtx;
}
