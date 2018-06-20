/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinningData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/BinningData.h"

/** distance to boundary value (exact calculation) */
bool Trk::BinningData::distance(const Amg::Vector3D& pos, const Amg::Vector3D& dir, int bb, float& dist, float min, float max) const {

  float bval;

  if (binvalue==Trk::binH || binvalue==Trk::HmodPhi ) {
    if (bb >= int(hbounds.size())) return false;
    bval =  hbounds[bb].second ;
  } else {
    if (type == Trk::equidistant && bb>int(bins) ) return false; 
    else if (bb >= int(boundaries.size()) ) return false; 
    bval = (type == Trk::equidistant) ?  min+bb*step : boundaries[bb] ;
  }
  
  // distance to plane-like
  if (binvalue == Trk::binX) {
    const Amg::Vector3D center(bval,0.,0.);
    const Amg::Vector3D normal(1.,0.,0.);
    Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
    if (ds.numberOfSolutions()) {
      dist = ds.first();
      return true;
    }
    else return false;     
  }
  
  if (binvalue == Trk::binY) {
    const Amg::Vector3D center(0.,bval,0.);
    const Amg::Vector3D normal(0.,1.,0.);
    Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
    if (ds.numberOfSolutions()) {
      dist = ds.first();
      return true;
    }
    else return false;     
  }
  
  if (binvalue == Trk::binZ) {
    const Amg::Vector3D center(0.,0.,bval);
    const Amg::Vector3D normal(0.,0.,1.);
    Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
    if (ds.numberOfSolutions()) {
      dist = ds.first();
      return true;
    }
    else return false;     
  }

  if (binvalue == Trk::modZ) {
    const Amg::Vector3D center(0.,0.,bval);
    const Amg::Vector3D normal(0.,0.,1.);
    const Amg::Vector3D modPos(0.,0.,value(pos));
    Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(modPos,dir,center,normal);
    if (ds.numberOfSolutions()) {
      dist = ds.first();
      return true;
    }
    else return false;     
  }
  
  if (binvalue == Trk::binPhi) {
    const Amg::Vector3D center(0.,0.,0.);
    const Amg::Vector3D normal(sin(bval),-cos(bval),0.);
    Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
    if (ds.numberOfSolutions()) {
      Amg::Vector3D probe = pos+ds.first()*dir;
      if (cos(this->value(probe)-bval)>0.999 ) {
	dist = ds.first();
	return true;
      } else return false;       
    }
    else return false;     
  }
  
  if (binvalue == Trk::modPhi) {
    float phiStep = pos.phi()-this->value(pos);
    const Amg::Vector3D center(0.,0.,0.);
    const Amg::Vector3D normal(sin(bval+phiStep),-cos(bval+phiStep),0.);
    Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
    if (ds.numberOfSolutions()) {
      Amg::Vector3D probe = pos+ds.first()*dir;
      if (cos(this->value(probe)-bval)>0.999 ) {
	dist = ds.first();
	return true;
      } else return false;       
    }
    else return false;     
  }

  if (binvalue == Trk::XmodPhi) {
    float phiStep = pos.phi()-dPhi(pos);
    int bPhi = binPh(pos);
    std::pair<float,float> phiDist = distanceToPhiBounds(pos,dir);
    const Amg::Vector3D center(-bval*sin(phiStep),bval*cos(phiStep),0.);
    const Amg::Vector3D normal(-sin(phiStep),cos(phiStep),0.);
    Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
    if (ds.numberOfSolutions()) {  
      // check change of phi bin : requires redefinition of bin boundary
      // return false and distance to phi boundary 
      Amg::Vector3D probe = pos+ds.first()*dir;
      int bPhiS = binPh(probe); 
      if (bPhi != bPhiS) {
        if ( phiDist.first>0. && phiDist.first<ds.first() ) {
	  dist = phiDist.first;
          return false;
	} else if ( phiDist.second>0. && phiDist.second<ds.first() ) {
	  dist = phiDist.second;
          return false;
	}
      }
      if (acos(cos(probe.phi()-refphi-phiStep))<0.5*phiStep+0.001) {
	dist = ds.first();
        return true;
      }
      else return false;
    }
    else return false;     
  }

  if (binvalue == Trk::HmodPhi) {
    if ( hbounds[bb].first ==0 ) {
      const Amg::Vector3D center(0.,0.,0.);
      const Amg::Vector3D normal(0.,0.,1.);
      Trk::DistanceSolution ds = m_distCalculs.distanceToCylinder(pos,dir,center,normal,bval);
      if (ds.numberOfSolutions()) {
	if (fabs((pos+ds.first()*dir).perp()-bval)<0.01) {
	  dist = ds.first();
	  return true;
	} else return false;
      }
      else return false;     
    } else {     // bevelled cylinder
      float phiStep = pos.phi()-dPhi(pos);
      int bPhi = binPh(pos);
      std::pair<float,float> phiDist = distanceToPhiBounds(pos,dir);
      const Amg::Vector3D center(bval*cos(phiStep),bval*sin(phiStep),0.);
      const Amg::Vector3D normal(cos(phiStep),sin(phiStep),0.);
      Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
      if (ds.numberOfSolutions()) { 
	// check change of phi bin : requires redefinition of boundary surface
	Amg::Vector3D probe = pos+ds.first()*dir;
	int bPhiS = binPh(probe); 
	if (bPhi != bPhiS) {
	  if ( phiDist.first>0. && phiDist.first<ds.first() ) {
	    phiStep -= stepVal;  
            const Amg::Vector3D nc(bval*cos(phiStep),bval*sin(phiStep),0.);
            const Amg::Vector3D nn(cos(phiStep),sin(phiStep),0.);
            ds = m_distCalculs.distanceToPlane(pos,dir,nc,nn);
            if (!ds.numberOfSolutions()) return false;
	    dist = ds.first();
	    return true;
	  } else if ( phiDist.second>0. && phiDist.second<ds.first() ) {
	    phiStep += stepVal;  
            const Amg::Vector3D nc(bval*cos(phiStep),bval*sin(phiStep),0.);
            const Amg::Vector3D nn(cos(phiStep),sin(phiStep),0.);
            ds = m_distCalculs.distanceToPlane(pos,dir,nc,nn);
            if (!ds.numberOfSolutions()) return false;
	    dist = ds.first();
	    return true;
	  }
	}
	dist = ds.first();
	return true;
      }
      else return false;          
    }
  }
    
  // distance to cylinder-like
  if (binvalue == Trk::binR) {
    const Amg::Vector3D center(0.,0.,0.);
    const Amg::Vector3D normal(0.,0.,1.);
    Trk::DistanceSolution ds = m_distCalculs.distanceToCylinder(pos,dir,center,normal,bval);
    if (ds.numberOfSolutions()) {
      if (fabs((pos+ds.first()*dir).perp()-bval)<0.01) {
	dist = ds.first();
	return true;
      } else return false;
    }
    else return false;
  }     

  // distance to cone-like
  if (binvalue == Trk::binEta) {
    const Amg::Vector3D center(0.,0.,0.);
    const Amg::Vector3D normal(0.,0.,1.);
    double theta = 2*atan(exp(-bval));

    Trk::DistanceSolution ds = m_distCalculs.distanceToCone(pos,dir,center,normal,theta);

    if (ds.numberOfSolutions() && ds.first()>-pos.mag()+0.001 && ds.first()>=min && ds.first()<=max ) {
      if (fabs((pos+ds.first()*dir).theta()-theta)<0.001) {
	dist = ds.first();
	return true;
      }
    }
    if (ds.numberOfSolutions()>1 && ds.second()>-pos.mag()+0.001 && ds.second()>=min && ds.second()<=max ) {
      if (fabs((pos+ds.second()*dir).theta()-theta)<0.001) {
	dist = ds.second();
	return true;
      }
    } 
    return false;
  }     
    
  // distance of mixed type
  if (binvalue == Trk::binH) {
    if ( hbounds[bb].first ==0 ) {
      const Amg::Vector3D center(0.,0.,0.);
      const Amg::Vector3D normal(0.,0.,1.);
      Trk::DistanceSolution ds = m_distCalculs.distanceToCylinder(pos,dir,center,normal,bval);
      if (ds.numberOfSolutions()) {
	if (fabs((pos+ds.first()*dir).perp()-bval)<0.01) {
	  dist = ds.first();
	  return true;
	} else return false;
      }
      else return false;     
    } else {     // bevelled cylinder
      const Amg::Vector3D center(bval*sin(refphi),bval*cos(refphi),0.);
      const Amg::Vector3D normal(sin(refphi),cos(refphi),1.);
      Trk::DistanceSolution ds = m_distCalculs.distanceToPlane(pos,dir,center,normal);
      if (ds.numberOfSolutions()) {
	dist = ds.first();
	return true;
      }
      else return false;          
    }
  }

  //std::cout<<"BinningData: distance calculation not defined for BinningValue:"<< binvalue<< std::endl;
  dist=0.;
  return false;
  
}

std::pair<float,float> Trk::BinningData::distanceToPhiBounds(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const {

    int bPhi = binPh(pos);
    float philow = refphi+(bPhi-0.5)*stepVal  ;
    float phiup  = refphi+(bPhi+0.5)*stepVal  ;

    const Amg::Vector3D center(0.,0.,0.);
    const Amg::Vector3D nlow(sin(philow),-cos(philow),0.);
    Trk::DistanceSolution dlow = m_distCalculs.distanceToPlane(pos,dir,center,nlow);
    float low = -1.e05;
    if (dlow.numberOfSolutions()) {
      Amg::Vector3D probe = pos+dlow.first()*dir;
      if (cos(this->value(probe)-philow)>0.999 ) low = dlow.first();
    }
    const Amg::Vector3D  nup(sin(phiup),-cos(phiup),0.);
    Trk::DistanceSolution dup = m_distCalculs.distanceToPlane(pos,dir,center,nup);
    float up = 1.e05;
    if (dup.numberOfSolutions()) {
      Amg::Vector3D probe = pos+dup.first()*dir;
      if (cos(this->value(probe)-phiup)>0.999 ) up = dup.first();
    }

    return std::pair<float,float>(low,up);
}

// low level bin retrieval, presumably to be merged 
Trk::BinPath Trk::BinningData::binPath(const Amg::Vector3D& pos, const Amg::Vector3D& dir, float dMin, float dMax) const {

  // current bin (boundary)
  int currBin = searchGlobal(pos);          
  
  // upper bin (boundary)
  unsigned int cBound = currBin+1;
  if (cBound>bins && option == Trk::closed) cBound = 0;
  
  // distance to bin boundaries
  float dLow,dUp;
  bool dL = distance(pos,dir,currBin,dLow,dMin,dMax);
  bool dU = distance(pos,dir,cBound,dUp,dMin,dMax);

  // entry AND/OR exit not resolved
  if (!dL && !dU) {
    Trk::BinPath path(Trk::BinIntersection(dMin,currBin));
    path.add(Trk::BinIntersection(dMax,currBin)); 
    return path;
  }

  int bstep = 0;    // not defined yet
  float shift = 0.;     float period = 0.; float dNext=0.;

  int binEntered = currBin;
  Trk::BinPath path;

  // change of phi bin in special cases
  if ( binvalue==Trk::XmodPhi && (!dL || !dU) ) {
    if (!dL) {
      path = Trk::BinPath(Trk::BinIntersection(fmax(dUp,dMin),currBin));
      if (dLow<dMax) {
	shift = dLow;
	currBin = searchGlobal(pos+(shift+0.01)*dir );
	path.add(Trk::BinIntersection(shift,currBin));
	bool nL = distance(pos+shift*dir,dir,currBin,dLow,dMin,dMax);
	bstep = -1; binEntered = currBin+bstep;
	if (nL) path.add(Trk::BinIntersection(shift+dLow,binEntered));
      } else {
	bstep = -1; binEntered = currBin+bstep;
        shift = dMax;
	path.add(Trk::BinIntersection(shift,-1));
        return path;
      }
    } else if (!dU) {
      path = Trk::BinPath(Trk::BinIntersection(fmax(dLow,dMin),currBin));
      if (dUp<dMax) {
	shift = dUp;
	currBin = searchGlobal(pos+(shift+0.01)*dir );
	path.add(Trk::BinIntersection(shift,currBin));
	bool nU = distance(pos+shift*dir,dir,currBin,dUp,dMin,dMax);
	bstep = 1; binEntered = currBin+bstep;
	if (nU) path.add(Trk::BinIntersection(shift+dUp,binEntered));
      } else {
	bstep = 1; binEntered = currBin+bstep;
        shift = dMax;
	path.add(Trk::BinIntersection(shift,-1));
        return path;
      }
    }

  } else {
  
    if (!dL) dLow = dUp>0.  ? dMin : dMax;
    if (!dU) dUp  = dLow>0. ? dMin : dMax; 
  
    // resolve direction (bin entry/exit)
    bstep = dLow<dUp ? 1 : -1;
    float dEntry = fmin(dLow,dUp);
    dNext  = fmax(dLow,dUp);
  
    if (binvalue==Trk::modZ) {
      float dz0; float dzl; 
      bool s0 = distance(pos,dir,0,dz0,dMin,dMax);
      bool sl = distance(pos,dir,bins,dzl,dMin,dMax);
      if (s0 && sl) period = fabs(dzl-dz0);
    }
    
    // build the path for first bin     
    path = Trk::BinPath(Trk::BinIntersection(fmax(dEntry,dMin),currBin));

    if (dNext>=dMax) {
      path.add(Trk::BinIntersection(dMax,binEntered));      
      return path; 
    }

    binEntered = currBin + bstep; 
    if (binEntered<0 )   {
      binEntered =  option == Trk::open ? -1 : bins;
    }
    if (binEntered>=int(bins) ) {
      binEntered =  option == Trk::open ? -1 : 0;
    }
    
    path.add(Trk::BinIntersection(dNext,binEntered));  
    
    if (binEntered<0) return path;  
    
    if (bstep>0) {
      currBin +=bstep;    // move current bin boundary to bin exit
      if (currBin>int(bins) && option== Trk::closed ) {
	currBin = 0;
	if (binvalue==Trk::modZ) shift += period;
      }  
    }
  }

  // continue along path
  bool inProgress = (shift<dMax) ;
        
  while (inProgress ) {
    currBin +=bstep;      // step to new boundary calculation
    if (currBin<0 && option == Trk::closed) {
      currBin = bins-1;
      if ( binvalue == Trk::modZ ) shift += period;
    }  
    if (currBin>=int(bins) && option== Trk::closed ) {
      currBin = 0;
      binEntered = 0;
      if (binvalue==Trk::modZ) shift += period;
    }  
    if (!distance(pos+shift*dir,dir,currBin,dNext,dMin,dMax)) return path;
    if (dNext+shift>=dMax) {
      path.add(Trk::BinIntersection(dMax,binEntered));     
      return path;
    }
    // save entry
    binEntered = currBin;
    if (currBin>=int(bins)) binEntered = -1;
    if (bstep<0) {
      binEntered = currBin+bstep;
      if (binEntered<0 && option==Trk::closed) binEntered=bins-1;
    }   
    path.add(Trk::BinIntersection(dNext+shift,binEntered)); 
    if (binEntered<0) inProgress=false;
  }  
  
  return path;
  
}
