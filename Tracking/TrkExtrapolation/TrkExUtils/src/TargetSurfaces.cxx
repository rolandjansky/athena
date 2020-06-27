/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Trk::TargetSurfaces
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 09/2015 sarka.todorova@cern.ch  
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#include <utility>

#include "GaudiKernel/MsgStream.h"
#include "TrkExUtils/TargetSurfaces.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/TrackingVolume.h"

Trk::ExtrapolationCode  Trk::TargetSurfaces::setOnInput(const Trk::ExCellCharged& eCell, const Trk::Surface* sf, 
							const Trk::BoundaryCheck& bcheck) 
{
  Amg::Vector3D gp  = eCell.leadParameters->position();	
  Amg::Vector3D mom = eCell.leadParameters->momentum().normalized();	
  Trk::PropDirection dir = eCell.propDirection;
  
  return setOnInput( gp, mom*dir, eCell.leadVolume, sf, bcheck);
}

  Trk::ExtrapolationCode  Trk::TargetSurfaces::setOnInput(const Amg::Vector3D& position, const Amg::Vector3D& direction,
							 const Trk::TrackingVolume* fVol, 
               const Trk::Surface* sf, const Trk::BoundaryCheck& bcheck) 
{
  // clear cache
  m_baseSurfaces.clear();
  m_tempSurfaces.clear();
  m_ordered.clear();
  m_orderTrue = false;

  // destination surface(s) first
  if (sf) { 
    Trk::TargetSurface tt(sf,bcheck,Trk::SurfNavigType::Target,0,nullptr,Trk::TVNavigType::Unknown);
    evaluateInputDistance(tt,position,direction,true);  
    // abort if no valid intersection
    if (m_baseSurfaces.empty() || (m_baseSurfaces.back().distanceAlongPath<0 && !sf->isOnSurface(position,bcheck,m_tolerance)))
      return Trk::ExtrapolationCode::FailureDestination;
  }

  if ( initFrameVolume(position,direction,fVol) ) return Trk::ExtrapolationCode::InProgress;
    return Trk::ExtrapolationCode::FailureLoop;   // failure navigation?

}

Trk::TargetSurfaceVector  Trk::TargetSurfaces::orderedIntersections(const Trk::ExCellNeutral& eCell, const Trk::Surface* sf,
							       const Trk::BoundaryCheck& bcheck) 
{
  Amg::Vector3D gp  = eCell.leadParameters->position();	
  Amg::Vector3D mom = eCell.leadParameters->momentum().normalized();	
  Trk::PropDirection dir = eCell.propDirection;
  
  return orderedIntersections( gp, mom*dir, eCell.leadVolume, sf, bcheck);
}

Trk::TargetSurfaceVector  Trk::TargetSurfaces::orderedIntersections(const Amg::Vector3D& position, const Amg::Vector3D& direction,
					  const Trk::TrackingVolume* fVol, const Trk::Surface* sf,const Trk::BoundaryCheck& bcheck  )
{
  Trk::TargetSurfaceVector empty;   

  // clear cache
  m_baseSurfaces.clear();
  m_tempSurfaces.clear();
  m_ordered.clear();
  m_orderTrue = true;

  // destination surface(s) first
  if (sf) { 
    Trk::TargetSurface tt(sf,bcheck,Trk::SurfNavigType::Target,0,nullptr,Trk::TVNavigType::Unknown);
    evaluateInputDistance(tt,position,direction,true);  
    // abort if no valid intersection
    if (m_baseSurfaces.empty() || (m_baseSurfaces.back().distanceAlongPath<0 && !sf->isOnSurface(position,bcheck,m_tolerance)))
      return empty;
  }

  if ( initFrameVolume(position,direction,fVol) ) return orderIntersections();
  return empty;   // failure navigation?

}

bool Trk::TargetSurfaces::initFrameVolume(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const Trk::TrackingVolume* fVol) 
{
  if (!fVol) return true;

  m_currentFrame = fVol;             //  
  m_currentDense = fVol;             // default 

  if (m_debugMode) std::cout <<"DEBUG:input frame volume:"<<fVol->volumeName()<<" at position z:"<< fVol->center().z()<< std::endl;

  m_tolerance = 0.001; 

  // save probe coordinates
  m_probePos = pos;
  m_probeDir = dir; 

  // clear cache : keep target surfaces
  std::vector<Trk::TargetSurface>::iterator is=m_baseSurfaces.begin();
  while ( is!=m_baseSurfaces.end() && (*is).sfType==Trk::SurfNavigType::Target) is++;
  if (is!=m_baseSurfaces.end()) m_baseSurfaces.erase(is,m_baseSurfaces.end());   
  m_tempSurfaces.clear();

  // static frame boundaries
  const std::vector< Trk::SharedObject< const Trk::BoundarySurface< Trk::TrackingVolume> > > &bounds = fVol->boundarySurfaces();
  for (unsigned int ib=0; ib< bounds.size(); ib++ ){
    const Trk::Surface& surf = (bounds[ib].get())->surfaceRepresentation();
    Trk::TargetSurface bb(&surf,true,Trk::SurfNavigType::BoundaryFrame,ib,fVol,Trk::TVNavigType::Frame);
    evaluateInputDistance(bb,pos,dir,true);
    if (m_debugMode) std::cout<<"DEBUG:frame input:id:status:distance:"<<ib<<":"<<bb.status<<":"<<bb.distanceAlongPath<<std::endl;
  }

  // check early exit
  m_nextSf = -1;
  double dExit = 0.;

  // index running over frame boundary only
  int index = 0;

  is=m_baseSurfaces.begin();
  while ( is!=m_baseSurfaces.end() ) {
    if ((*is).sfType==Trk::SurfNavigType::BoundaryFrame ) {
      double dist = (*is).distanceAlongPath;        
      if ( dist < m_tolerance && dist > dExit ) {
        Amg::Vector3D probe = pos+dir*dist;
        if ( (*is).surf->isOnSurface(probe,true,m_tolerance,m_tolerance) ) {
	  const Trk::TrackingVolume* nextVolume = bounds[(*is).index].get()->attachedVolume(probe,dir,Trk::alongMomentum); 
          if (nextVolume!=fVol) {
	    dExit = dist;
	    m_nextSf = index; 
	  }
        }  
      }
      index++;
    }
    is++;
  } 

  //if (m_debugMode) {
  //  findNext();
  //  std::cout <<"resolving SL exit from volume:"<<fVol->volumeName()<<":"<< m_nextSf<< ","<<dExit<<std::endl;
  //}

  if (m_nextSf>-1) {

    m_distanceToNext = dExit;
    if (m_debugMode) std::cout <<"DEBUG:early exit detected at boundary index:"<< m_nextSf<<","<<dExit <<std::endl;
    return false;
  }

  // fill frame volume

  // find closest surface and distance
  findNext();

  if (m_debugMode) std::cout<<"DEBUG:volume exit resolved (SL estimate):"<<m_nextSf<<","<<m_distanceToNext<< std::endl;

  return true;

}

void Trk::TargetSurfaces::evaluateInputDistance(Trk::TargetSurface& tt, const Amg::Vector3D& pos, const Amg::Vector3D& mom, bool base) 
{
    Trk::DistanceSolution distSol = tt.surf->straightLineDistanceEstimate(pos,mom);

    double dist = distSol.first();
    Amg::Vector3D posi =  pos + dist*mom;
    // skip trivial solutions
    if (distSol.numberOfSolutions()>1 && dist<m_tolerance && distSol.second()>m_tolerance) {
      dist = distSol.second();
      posi =  pos + dist*mom;
      if (m_orderTrue && !tt.surf->isOnSurface(posi,tt.bcheck,m_tolerance,m_tolerance) ) return;
      double dAbs = distSol.currentDistance(true);
      tt.setDistance(dist,fabs(dAbs),distSol.signedDistance() && dAbs!=0. ?  dAbs/fabs(dAbs)  : 0.);
      tt.setPosition(posi);
      if (fabs(dAbs)<m_tolerance) tt.setStatus(-1);
      else tt.setStatus(1);
      save(tt,base);
      return;
    } 
    // save closest solution 
    if (!m_orderTrue || dist>m_tolerance ) {
      double dAbs = distSol.currentDistance(true);
      tt.setDistance(dist,fabs(dAbs),distSol.signedDistance() && dAbs!=0. ?  dAbs/fabs(dAbs)  : 0.);
      tt.setPosition(posi);
      if (fabs(dAbs) < m_tolerance) tt.setStatus(-1);
      else if (dist > m_tolerance) tt.setStatus(1);
      save(tt,base);
    } 

    // save multiple intersection for neutral transport
    if (distSol.numberOfSolutions()>1 && distSol.second()>m_tolerance && m_orderTrue) {
      dist = distSol.second();
      posi =  pos + dist*mom;
      if ( tt.surf->isOnSurface(posi,tt.bcheck,m_tolerance,m_tolerance) ) {
        double dAbs = distSol.currentDistance(true);
        tt.setDistance(dist,fabs(dAbs),distSol.signedDistance() && dAbs!=0. ?  dAbs/fabs(dAbs)  : 0.);
        tt.setPosition(posi);
        if (fabs(dAbs)<m_tolerance) tt.setStatus(-1);
        else tt.setStatus(1);
        save(tt,base);
      }
    }

    }  

bool Trk::TargetSurfaces::updateDistance(int index, Trk::TargetSurface& tt, const Amg::Vector3D& pos, const Amg::Vector3D& dir) 
{
    double previousDistance = tt.distanceAlongPath;

    Trk::DistanceSolution distSol = tt.surf->straightLineDistanceEstimate(pos,dir);

    double dist = 1.e08;

    // catch step across surface
    if (distSol.numberOfSolutions()>0 ) {
      dist = distSol.first();
      if (distSol.numberOfSolutions()>1 &&
      	  fabs(distSol.first()+m_lastStep-previousDistance) >
      	  fabs(distSol.second()+m_lastStep-previousDistance) && distSol.second() <= 0. ) {
	dist = distSol.second();  
      }
    }

    // verify true intersection and flip direction if necessary 
    if (  previousDistance*dist < 0. &&  fabs(dist)>m_tolerance ) {
      // verify change of sign in signedDistance ( after eliminating situations where this is meaningless )
      if ( !distSol.signedDistance() || fabs(distSol.currentDistance(true))<m_tolerance || tt.distance<m_tolerance
      	   || tt.signAbsDist*distSol.currentDistance(true)<0) {   // true intersection
	//if ( !distSol.signedDistance() || tt.signAbsDist*distSol.currentDistance(true)<0) {   // true intersection
	if (index==m_nextSf) {
          if (m_debugMode) std::cout <<"DEBUG:flipping intersection:signed ? true dist:"<< distSol.signedDistance()<<":"
				     << tt.signAbsDist*distSol.currentDistance(true) <<std::endl; 
	  m_flipDirection = true;
          m_flipDistance = dist;
	}  else if ( tt.distance>m_tolerance && fabs(distSol.currentDistance(true))>m_tolerance ) {
	  // here we need to compare with distance from current closest
	  if ( index>m_nextSf ) {   // easy case, already calculated
	    if ( dist<(m_flipDistance-m_tolerance) && tt.status!=-1)  {
	      m_flipDirection = true;
              m_flipDistance = dist;
	      m_nextSf=index;
	    }
	  } else if (m_distanceToNext>0.) {             // set as nearest (if not iterating already), will be rewritten later
	    if (tt.status!=-1) {
	      m_flipDirection = true;
              m_flipDistance = dist;
	      m_nextSf = index;
	    }
	  }
	}
      }
    }

    // continue iteration if appropriate
    if ( index == m_nextSf && dist<0. && previousDistance<dist ) m_distanceToNext = dist;
	
    // save current distance to surface
    Amg::Vector3D posi =  pos + dist*dir;
    double dAbs = distSol.currentDistance(true);
    tt.setDistance(dist,fabs(dAbs),distSol.signedDistance() && dAbs!=0. ?  dAbs/fabs(dAbs)  : 0.);
    tt.setPosition(posi);
    if (tt.status==-1 && fabs(dAbs)>m_tolerance ) tt.status = dist>0 ? 1 : 0;

    return true;
}  

void Trk::TargetSurfaces::save(Trk::TargetSurface& tt, bool base) {

  if (base) m_baseSurfaces.push_back(tt);
  else {
    if (m_tempSurfaces.empty() || tt.assocVol!=m_tempSurfaces.back()[0].assocVol) {
      Trk::TargetSurfaceVector local; local.push_back(tt); m_tempSurfaces.push_back(local); 
    } else   m_tempSurfaces.back().push_back(tt);
  }        
}

Trk::TargetSurfaceVector  Trk::TargetSurfaces::orderIntersections() const{

  Trk::TargetSurfaceVector tsOrd ;		

  // base surfaces
  if (m_baseSurfaces.empty()) return tsOrd;
      
  std::vector<unsigned int> sols(m_baseSurfaces.size());
  for (unsigned int i=0;i<m_baseSurfaces.size(); i++) { sols[i]=i; }

  unsigned int itest=1;
  while ( itest<sols.size() ) {
    if ( m_baseSurfaces[sols[itest]].distanceAlongPath < m_baseSurfaces[sols[itest-1]].distanceAlongPath ) {
      unsigned int iex = sols[itest-1];
      sols[itest-1]=sols[itest];
      sols[itest]=iex;
      itest=1;
    } else itest++; 
  }

  for (unsigned int i=0;i<m_baseSurfaces.size(); i++) tsOrd.push_back(m_baseSurfaces[sols[i]]); 
  
  return tsOrd;

}

void Trk::TargetSurfaces::findNext() {

  m_nextSf = -1;
  m_distanceToNext = 1.e06;
  m_numAlongPath = 0;

  // index running over all selected surfaces
  int index = 0;

  std::vector<Trk::TargetSurface>::iterator is=m_baseSurfaces.begin();
  while ( is!=m_baseSurfaces.end() ) {
    if ((*is).status==-1 && (*is).distanceAlongPath>m_tolerance) {
      m_numAlongPath++;
      double dd = (*is).distanceAlongPath;
      if ( dd < m_distanceToNext ) {
	m_nextSf = index;
	m_distanceToNext = dd ;
      }
    }
    if ((*is).status!=-1 && (*is).distanceAlongPath>m_tolerance) {
      m_numAlongPath++;
      double dd = (*is).distanceAlongPath;
      if (dd>m_tolerance && dd<(*is).distance ) dd=(*is).distance;
      if ( dd < m_distanceToNext ) {
	m_nextSf = index;
	m_distanceToNext = dd ; 
      }
    }
    index++; is++;
  } 
  for (unsigned it=0; it<m_tempSurfaces.size(); it++) {
    is=m_tempSurfaces[it].begin();
    while ( is!=m_tempSurfaces[it].end()) {
      if ((*is).status!=-1 && (*is).distanceAlongPath>m_tolerance) {
	m_numAlongPath++; 
	double dd = (*is).distanceAlongPath;
	if (dd>m_tolerance && dd<(*is).distance ) dd=(*is).distance;
	if ( dd < m_distanceToNext ) {
	  m_nextSf = index;
	  m_distanceToNext = dd; 
	}
      }
      index++; is++;
    }
  } 
}


bool Trk::TargetSurfaces::checkDistance(const Amg::Vector3D& pos, const Amg::Vector3D& dir, double nextStep) {

  m_lastStep = (pos-m_probePos).mag();

  if (not (m_lastStep>0.)) {
    if (m_debugMode) std::cout <<"DEBUG:check distance with 0 step:"<<"next,dist:"<<m_nextSf<<","<<m_distanceToNext<< std::endl;
    return true;
  } 

  // dont overwrite previous estimates before full loop finished
  // limit the number of reevaluations by using rough estimates

  int nextSfCandidate = -1;
  m_distanceToNext = 1.e08;
  m_numAlongPath = 0;
  m_flipDirection = false;
  m_flipDistance = 0.;

  // index running over all selected surfaces
  int index = 0;

  std::vector<Trk::TargetSurface>::iterator is=m_baseSurfaces.begin();
  while ( is!=m_baseSurfaces.end() ) {
    // reevaluate distance if : status = -1  ( leaving surface with possibility of re-entry,
    //                                         switch to 0 when abs.distance > tolerance )
    //                          abs.distance < 2*nextStep
    // otherwise just subtract distance stepped for rough estimate                                         

    if ( (*is).status==-1 || ( fabs((*is).distance)-m_lastStep ) < 2*fabs(nextStep) || m_absDist) {  
      if (m_lastStep>m_tolerance) (*is).setStatus(0);    
      updateDistance(index,(*is),pos,dir);    
    } else {
      (*is).fastDistanceUpdate(m_lastStep);
    }

    if (m_debugMode) std::cout <<"DEBUG:check distance:"<<index<<":"<<(*is).status<<","<<(*is).distanceAlongPath<<
	      ","<<(*is).distance<<","<<(*is).signAbsDist<<":flip dir,dist:"<<m_flipDirection<<","<<m_flipDistance<< std::endl;
    if ( (*is).status!=-1 || (*is).distanceAlongPath>m_tolerance ) {
      if ( (*is).distanceAlongPath>-m_tolerance) {
	m_numAlongPath++; 
	double dd = (*is).distanceAlongPath;
	if (dd>m_tolerance && dd<(*is).distance ) dd=(*is).distance;
	if ( dd < m_distanceToNext ) {
	  nextSfCandidate = index;
	  m_distanceToNext = dd;
	} 
      }
    }   

    index++; is++;
  } 

  m_absDist=false;

  // distanceAlongPath negative : switch to absolute distance
  if (nextSfCandidate<0 && m_distanceToNext>0.) {
    if (!m_currentFrame->inside(pos,0.001)) {
      std::cout <<"ERROR:frame volume left, aborting:"<<m_currentFrame->volumeName()<<std::endl;
      return false; 
    }
    index = 0;
    is=m_baseSurfaces.begin();
    while ( is!=m_baseSurfaces.end() ) {
      updateDistance(index,(*is),pos,dir);    
      if ( (*is).status!=-1 || (*is).distance>m_tolerance ) {
	if ( (*is).distance>-m_tolerance) {
	  m_numAlongPath++; 
	  double dd = (*is).distance;
	  if ( dd < m_distanceToNext ) {
	    nextSfCandidate = index;
	    m_distanceToNext = dd;
	  } 
	}
      }   
      index++; is++;
    } 
    if (m_debugMode) std::cout <<"DEBUG:closest frame estimate based on absolute distance:"<< nextSfCandidate<<":"<<m_distanceToNext
	      <<", inside frame volume?"<< m_currentFrame->inside(pos,0.)<<std::endl; 
    m_absDist = true;
  }

  for (unsigned it=0; it<m_tempSurfaces.size(); it++) {
    is=m_tempSurfaces[it].begin();

    while ( is!=m_tempSurfaces[it].end()) {

      if ( (*is).status==-1 || ( fabs((*is).distance)-m_lastStep ) < 2*fabs(nextStep) ) {     
	updateDistance(index,(*is),pos,dir);    
      } else {
	(*is).fastDistanceUpdate(m_lastStep);
      }

      if ( (*is).status!=-1  || (*is).distanceAlongPath>m_tolerance ) {
	if ( (*is).distanceAlongPath>-m_tolerance) {
	  m_numAlongPath++; 
	  double dd = (*is).distanceAlongPath;
	  if (dd>m_tolerance && dd<(*is).distance ) dd=(*is).distance;
	  if ( dd<m_distanceToNext ) {
	    nextSfCandidate = index;
	    m_distanceToNext = dd;
	  } 
	}
      }

      index++; is++;
    }
  } 

  // distanceToNext estimate reliable for distances below 2*|nextStep| only 
  if ( !m_flipDirection && nextSfCandidate != m_nextSf  ) m_nextSf = nextSfCandidate;  
  //if ( !m_flipDirection && nextSfCandidate != m_nextSf && m_distanceToNext<2*fabs(nextStep) ) m_nextSf = nextSfCandidate;  

  // flip direction
  if ( m_flipDirection ) m_distanceToNext = m_flipDistance;

  m_probePos = pos;
  m_probeDir = dir;

  if (m_debugMode) std::cout <<"DEBUG:check distance returns:next,dist:"<<m_nextSf<<","<<m_distanceToNext<< std::endl;

  return true;
}

void Trk::TargetSurfaces::fillSolutions(int hitSf, Amg::Vector3D gp, TargetSurfaceVector& solutions)
{
  // index running over all selected surfaces
  int index = 0;

  if (m_debugMode) std::cout <<"fill solutions at R,z,phi:"<<gp.perp()<<","<<gp.z()<<","<<gp.phi()<< std::endl;

  std::vector<Trk::TargetSurface>::iterator is=m_baseSurfaces.begin();
  while ( is!=m_baseSurfaces.end() ) {

    if ( (*is).status!=-1 ) {

      if ( index==hitSf || fabs((*is).distanceAlongPath)<0.01 ) {   // onSurface && boundary check
        if (m_debugMode) std::cout <<"DEBUG: onSurface, without bcheck, twice tolerance:"<<
			   (*is).surf->isOnSurface(gp,(*is).bcheck ,m_tolerance,m_tolerance)<<","<<
			   (*is).surf->isOnSurface(gp,false,m_tolerance,m_tolerance)<<","<<
			   (*is).surf->isOnSurface(gp,(*is).bcheck,2*m_tolerance,2*m_tolerance)<<std::endl;
	if ( (*is).surf->isOnSurface(gp,(*is).bcheck ,m_tolerance,m_tolerance) ) solutions.push_back((*is));
        else if ( (*is).surf->isOnSurface(gp,false,m_tolerance,m_tolerance) ) {   // probably numerical precision problem 
	   solutions.push_back((*is));                                            // in boundary check, 
           if (m_debugMode && !m_currentFrame->inside(gp,m_tolerance) )           // possible navigation break
	     std::cout<<"DEBUG: frame boundary crossed outside volume "<< m_currentFrame->volumeName()<< std::endl;  
	} else if ( index==hitSf) (*is).status = -1;
      }
    }     

    index++; is++;
  } 

  for (unsigned it=0; it<m_tempSurfaces.size(); it++) {
    is=m_tempSurfaces[it].begin();

    while ( is!=m_tempSurfaces[it].end()) {

      if ( (*is).status!=-1 ) {

	if ( index==hitSf || (*is).distanceAlongPath<0.01 ) {  // onSurface && boundary check
	  if ( (*is).surf->isOnSurface(gp,(*is).bcheck ,m_tolerance,m_tolerance) )  solutions.push_back((*is));
	  else if (index==hitSf) (*is).status=-1.;
	}
      }      

      index++; is++;
    }
  } 

  }
