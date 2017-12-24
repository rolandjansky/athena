/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelLayer.cxx        
 **
 **   Description:  This is the layer structure at the moment it    
 **                 just contains a vector of the modules in this  
 **                 layer and checks all of them to see if they are 
 **                 in the RoI. Eventually it will check these in    
 **                 a more efficient manner. 
 ** 
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Wed Apr  4 17:40:28 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#include "RegSelLUT/RegSelLayer.h"
#include "RegSelLUT/RegSelTimer.h"


#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>


void RegSelLayer::reset() { 
  m_modules.clear();
  m_disabled.clear();
  std::vector<std::vector<const RegSelModule*> >::iterator itr(m_phimaps.begin()) ; 
  for ( ; itr!=m_phimaps.end() ; itr++ ) (*itr).clear();
  m_set = false;
}


// this calculates how many segments in phi should be used by clustering 
// the modules together 

int RegSelLayer::clusterModules() const { 
  
  // if multiple radial "layers" in this actual layer (as in the TRT 
  // barrel, or the SCT endcaps) just find the inner most layer, as 
  // this will have the largest segmentation in phi
 
  if ( m_modules.size()==0 ) return 0;
  
  double r_in  = m_modules.back()->rMax();

  for ( int i=m_modules.size()-1 ; i-- ; ) {
    double mod_rmax = m_modules[i]->rMax();
    if ( mod_rmax<r_in )  r_in=mod_rmax;
  }

  // now cluster the ends of these innermost modules to calcute how
  // many phi segments to use

  // cluster vectors
  PhiCluster cluster;

  // use the minimum rMax+4mm just in case the detector has "moved"
  double r2_in = 2*(r_in+4);

  for ( int i=m_modules.size() ; i-- ; ) { 
    if ( (m_modules[i]->rMin()+m_modules[i]->rMax())<r2_in )  cluster.addModule(*m_modules[i]);    
  }

  return cluster.size();
}




// add a module to the PhiCluster - used to calculate the 
// phi segmentation automatically

void RegSelLayer::PhiCluster::addModule(const RegSelModule& m) { 

  static const double M_TWOPI = 2*M_PI; 

  double pmin = m.phiMin();
  double pmax = m.phiMax();

  if ( pmin>pmax ) pmax+=M_TWOPI;

  double overlap = 0.2*std::fabs(pmax-pmin);
  double start   = pmin;
  
  bool notoverlaping = true;
  for ( int i=mpositions.size() ; i-- ; ) { 
    
     double tdelphi = mpositions[i]-start;
    
     if ( tdelphi>M_PI  ) start+=M_TWOPI;
     if ( tdelphi<-M_PI ) start-=M_TWOPI;
    
     if ( std::fabs(mpositions[i]-start)<overlap ) {  
        mpositions[i] = mpositions[i]*mN[i]+start;
	mN[i]++;
	mpositions[i] /= mN[i];
	notoverlaping = false;
	break;
      }
  }

  if ( notoverlaping ) {  
    mN.push_back(1);
    mpositions.push_back(start);  
  }

}





// add a module to the vector, update the liimits for 
// this layer as well

void RegSelLayer::addModule(const RegSelModule& module) { 
  m_modules.push_back(&module);

  //  if ( module.enabled() ) m_enabledmodules.push_back(&module);
  //  m_enabledmodules.push_back(&module);

  //  std::cout << module 
  //       << "\t rmin=" << rMin() << "\t module.rMin=" << module.rMin() 
  //       << "\t zmin=" << zMin() << "\t module.zMin=" << module.zMin() << std::endl;

  if ( m_set ) {
    if ( module.rMin()<rMin()) rMin(module.rMin());
    if ( module.rMax()>rMax()) rMax(module.rMax());
    
    if ( module.zMin()<zMin()) zMin(module.zMin());
    if ( module.zMax()>zMax()) zMax(module.zMax());

    if ( module.phiMin()<phiMin()) phiMin(module.phiMin());
    if ( module.phiMax()>phiMax()) phiMax(module.phiMax());
  }
  else { 
    m_set = true;
    rMin(module.rMin());
    rMax(module.rMax());
    zMin(module.zMin());
    zMax(module.zMax());
    phiMin(module.phiMin());
    phiMax(module.phiMax());
  }

}


// set up the internal list of disabled modules for
// this layer

void RegSelLayer::setupDisabledModuleList() { 
  //  std::cout << "RegSelLayer::setupDisabledModuleList() clear() m_disabled.size()=" << m_disabled.size() << std::endl;
  m_disabled.clear();
  //  std::cout << "RegSelLayer::setupDisabledModuleList() going into loop m_modules.size()=" << m_modules.size() << std::endl;
  std::vector<const RegSelModule*>::const_iterator mptr(m_modules.begin());
  for ( ; mptr!=m_modules.end() ; mptr++ ) { 
    if ( !(*mptr)->enabled() ) m_disabled.push_back(*mptr);
  }
  //  std::cout << "RegSelLayer::setupDisabledModuleList() disabled " << m_disabled.size() << " modules" << std::endl;
}


// add the modules in this layer to a vector of modules
/** Fixme: This is the completely correct solution, where 
 **        each module is allowed to be at a completely 
 **        different radius. If we want to approximate 
 **        that all the modules are at the same radius 
 **        within a layer, then we only have to calculate
 **        the RoI z end points in this layer once
 **        and then just do the comparisons.
 **        it will save 2 multiplications and two 
 **        additions per module, but only for those
 **        in the RoI in the phi region, since those 
 **        outside the RoI in phi aren't even considered
 **        at the moment. Tried this, hardly quicker.
 **
 ** NB.    Using segmentation in phi and (or) z should 
 **        speed things up. This implements segmentation
 **        in phi only as the z didn't appear much faster. 
 **/

void RegSelLayer::getModules(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules) const { 

  //  std::cout << "RegSelLayer::getModules() " << *this << std::endl;

#if 0

  // look through all the modules in this layer and 
  // don't use the phi segmentation speed up

  std::vector<const RegSelModule*>::const_iterator mptr(m_modules.begin());

  for ( ; mptr!=m_modules.end() ; mptr++ ) { 
    // is the module inside the RoI ???
    if ( (*mptr)->enabled() && (*mptr)->inRoI(roi) ) {
      // then add them to the list 
      modules.push_back(*mptr);
    }
  }

#else
    
  //  std::cout << "\tRegSelLayer::getModules() " << roi << std::endl;

  // get the list of all modules in the phi segments 
  // corresponding to this RoI
    
  int roi_phiMin = (int) ((roi.getphiMin()-phiMin())*m_ideltaphi-1); // look at adjacent segments
  int roi_phiMax = (int) ((roi.getphiMax()-phiMin())*m_ideltaphi+1); // also, hence the +-1

  int roi_phiWidth = roi_phiMax-roi_phiMin;

  // have to see if the RoI spans the phi=pi boundary, 
  // and also whether we need to check segments that span the booundary
  bool noswap = true;

 
  // there MUST be a simpler way to do it than all these add hoc conditions
  // there just has to be, this is *awful* code
  if ( roi_phiWidth>=m_Nphi ) {  
    if ( roi_phiMin>roi_phiMax ) noswap = false;
    roi_phiMin = 0;
    roi_phiMax = m_Nphi-1;
  }
  else { 
    
    if ( roi.getphiWidth()*m_ideltaphi>=m_Nphi-2 ) roi_phiMax = roi_phiMin+m_Nphi-1;

    if ( roi_phiMax>=m_Nphi )   { roi_phiMax -= m_Nphi; noswap=false; }
    if ( roi_phiMin<0 )         { roi_phiMin += m_Nphi; noswap=false; }
    if ( roi_phiMin==0 && roi_phiMax<m_Nphi-1 && roi_phiMax!=0 )   { roi_phiMin = m_Nphi-1; noswap=false; }
    //    if ( roi_phiMin==0 && roi_phiMax==0 )   { roi_phiMin = m_Nphi-1; noswap=false; }

  }

  //    std::cout << "roi  phimin=" << roi.getphiMin() << "\tphimax=" << roi.getphiMax() << std::endl;
  //    std::cout << "roi iphimin=" << roi_phiMin << "\tiphimax=" << roi_phiMax << std::endl;

  //  roi_phiWidth = roi_phiMax-roi_phiMin;

  // if roi not in phi boundary
  if ( roi.getphiMin()<=roi.getphiMax() && noswap ) { 
    for ( int i=roi_phiMin ; i<=roi_phiMax ; i++ ) {
      std::vector<const RegSelModule*>::const_iterator mptr(m_phimaps[i].begin());  
      std::vector<const RegSelModule*>::const_iterator eptr(m_phimaps[i].end());  
      for ( ; mptr!=eptr ; mptr++ ) {
	if ( (*mptr)->enabled() && (*mptr)->inRoI(roi) ) modules.push_back(*mptr); 
      }
    }
  }
  else { 
    // roi spans phi=pi boundary
    // do phi<0 part of roi
    for ( int i=0 ; i<=roi_phiMax ; i++ ) { 
      std::vector<const RegSelModule*>::const_iterator mptr(m_phimaps[i].begin());  
      std::vector<const RegSelModule*>::const_iterator eptr(m_phimaps[i].end());  
      for ( ; mptr!=eptr ; mptr++ ) {
	if ( (*mptr)->enabled() && (*mptr)->inRoI(roi) )  modules.push_back(*mptr); 
      }
    }
    // do phi>0 part of roi
    // this is needed in case the max and min phi slice are the same
    // so the modules aren't added twice
    int firstbin = (roi_phiMin<roi_phiMax?roi_phiMax+1:roi_phiMin);
    for ( int i=firstbin ; i<m_Nphi ; i++ ) { 
      std::vector<const RegSelModule*>::const_iterator mptr(m_phimaps[i].begin());  
      std::vector<const RegSelModule*>::const_iterator eptr(m_phimaps[i].end());  
      for ( ; mptr!=eptr ; mptr++ ) {
	if ( (*mptr)->enabled() && (*mptr)->inRoI(roi) )  modules.push_back(*mptr); 
      }
    }
  }
  
#endif

}



void RegSelLayer::getModules( std::vector<const RegSelModule*>& modules) const { 
  std::vector<const RegSelModule*>::const_iterator mptr(m_modules.begin());  
  std::vector<const RegSelModule*>::const_iterator eptr(m_modules.end());  
  for ( ; mptr!=eptr ; mptr++ ) if ( (*mptr)->enabled() ) modules.push_back(*mptr); 
}









// look through all the disabled modules in this layer and 
// don't use any phi segmentation speed up
// NB this is slow but will be speeded up by storing 
// disabled modules and only looking at them

void RegSelLayer::getDisabledModules(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules) const { 

  //  std::cout << "RegSelLayer::getDisabledModules() " << *this << std::endl;
  //  std::cout << "RegSelLayer::getDisabledModules() m_disabled.size()=" << m_disabled.size() < std::endl;

  std::vector<const RegSelModule*>::const_iterator mptr(m_disabled.begin());
  //  std::vector<const RegSelModule*>::const_iterator mptr(m_modules.begin());

  for ( ; mptr!=m_disabled.end() ; mptr++ ) { 
    // is the module inside the RoI ???
    if ( (*mptr)->enabled()==false && (*mptr)->inRoI(roi) ) {
      // then add them to the list 
      modules.push_back(*mptr);
    }
  }

}



void RegSelLayer::createMaps() { 

  //  std::cout << "RegSelLayer::createMaps() " << *this << std::endl; 
  
  m_Nphi = clusterModules();

  if ( m_Nphi == 0 ) return; /// can this really ever happen?

  //  std::cout << "\t\t\tRegSelLayer::createMaps() layer with " << m_Nphi << " phi segments" << std::endl; 

  m_phimaps.resize(m_Nphi);
  
  m_phiMin = -M_PI+0.001;
  m_phiMax =  M_PI+0.001;
  
  m_ideltaphi = 0.5*m_Nphi/M_PI;
  
  //  std::map<const RegSelModule*, const RegSelModule*> tmap;
  std::set<const RegSelModule*> tmap;
  
  double trmax = 0;

  std::vector<const RegSelModule*>::const_iterator mptr(m_modules.begin());
  for ( ; mptr!=m_modules.end() ; mptr++ ) {
    
    double phicent = 0.5*((*mptr)->phiMax()+(*mptr)->phiMin());
    
    if ( (*mptr)->rMax()>trmax ) trmax = (*mptr)->rMax();

    // if cell spans pi boundary
    if ( (*mptr)->phiMax()<(*mptr)->phiMin() ) phicent += M_PI;
    
    if ( phicent>M_PI  ) phicent -= 2*M_PI;
    if ( phicent<-M_PI ) phicent += 2*M_PI;
    
    int phibin = ((int)((phicent-phiMin())*m_ideltaphi))%m_Nphi;
    
    //   std::cout << "phibin=" << (int)phibin 
    //	           << "\tphicent=" << phicent 
    //	           << "\t  [ " << (*mptr)->phiMin() << " - " << (*mptr)->phiMax() << " ]" << std::endl;
    
    // check module isn't already in another phi segment 
    if ( tmap.find(*mptr)!=tmap.end() ) continue;
    
    //    tmap[*mptr] = *mptr;
    tmap.insert(*mptr);
    m_phimaps[phibin].push_back(*mptr);
    
  }
  
  //  std::cout << "RegSelLayer::createMaps() exiting" << std::endl;   
}





std::ostream& operator<<(std::ostream& s, const RegSelLayer& layer) { 
  return s << "[ layer ID=" << layer.ID() 
	   << ",\tNmodules=" << layer.size() 
	   << ",\tr=" << layer.rMin() << " - " << layer.rMax()  
	   << ",\tz=" << layer.zMin() << " - " << layer.zMax()  
	   << " ]"; 
}

