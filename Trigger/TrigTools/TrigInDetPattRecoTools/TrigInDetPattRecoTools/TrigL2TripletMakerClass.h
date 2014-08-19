/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_TRIPLET_MAKER_CLASS_H__
#define __TRIG_L2_TRIPLET_MAKER_CLASS_H__

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigL2TrackCandidate.h"
#include "TrigInDetPattRecoTools/TrigL2HoughTransformClass.h"
#include "TrigInDetPattRecoTools/TrigL2HashKeyFunction.h"

#include<iostream>
#include<cmath>

template <class T> 
class TrigL2TripletMakerClass {

 public:
  TrigL2TripletMakerClass(const T& cuts, TrigL2HoughTransformClass* pH, long c = 1000) : 
    m_cuts(cuts), 
    m_pHough(pH), 
    m_pKeyGen(NULL) {
    m_pKeyGen = new TrigL2HashKeyFunction(cuts.invPhi0BinSize,cuts.invdPhidRBinSize,c);
  }
  TrigL2TripletMakerClass(const TrigL2TripletMakerClass& tm) :  m_cuts(tm.m_cuts), 
    m_pHough(tm.m_pHough) {
    m_pKeyGen = new TrigL2HashKeyFunction(*tm.m_pKeyGen);
  }
  TrigL2TripletMakerClass& operator = (const TrigL2TripletMakerClass& tm) {
    m_cuts = tm.m_cuts; 
    m_pHough = tm.m_pHough;
    m_pKeyGen = new TrigL2HashKeyFunction(*tm.m_pKeyGen);
    return *this;
  }

  ~TrigL2TripletMakerClass(){
    if(m_pKeyGen!=NULL) delete m_pKeyGen;
  }
  
 void execute(const TrigL2TrackCandidate* p, double minPhi, bool useBruteForceMethod=false) {
   if(useBruteForceMethod) {
     m_bruteForce(p,minPhi);
   }
   else {
     m_reducedCombinatorics(p,minPhi);
   }
 }
 
 protected:

 T m_cuts;
    TrigL2HoughTransformClass* m_pHough;
    TrigL2HashKeyFunction* m_pKeyGen;

    void m_bruteForce(const TrigL2TrackCandidate* pTrack, double minPhi) {        //former fillMapGC2

      //   Make hit triplets from different layer and if they satisfy certain cuts 
      //   fill the 2D histogram/map in (phi0,dPhidRho).
      //   This is the brute force way of making triplets, which is the fastest for nHits < ~20
      //

      double zVertex = pTrack->m_params.m_z0;

      long nHits = pTrack->spacePoints().size();
      std::vector<const TrigSiSpacePointBase*> hitList = pTrack->spacePoints();
      

      std::vector<const TrigSiSpacePointBase*>::const_iterator Itr0( hitList.begin() );
      std::vector<const TrigSiSpacePointBase*>::const_iterator Itr1, Itr2;
      
      long l[3];
      double rr1, fr1, dzdr21, k21, ddzdr;
      double r[3],z[3],f[3],e[3];
      
      for ( long i=0; i<nHits-2; ++i, ++Itr0 ) {
	
	f[0] = (*Itr0)->phi() - minPhi;
	if(f[0]<0.0) {
	  f[0]+=2*M_PI;
	}
	
	r[0] = (*Itr0)->r();
	z[0] = (*Itr0)->z();
	
	e[0] = (z[0]-zVertex)/r[0];
	e[0] = log(e[0]+sqrt(1.+e[0]*e[0]));
	
	l[0] = (*Itr0)->layer();
	rr1 = r[0]*r[0];
	fr1 = r[0]*f[0];
	
	Itr1 = Itr0; ++Itr1;
	for ( long j=i+1; j<nHits-1; ++j, ++Itr1 ) {
	  l[1] = (*Itr1)->layer();
	  if ( l[1]==l[0] ) continue;
	  
	  e[1] = ((*Itr1)->z()-zVertex)/(*Itr1)->r();
	  e[1] = log(e[1]+sqrt(1.+e[1]*e[1]));
	  
	  if ( fabs(e[1]-e[0])>m_cuts.etaCutInner ) continue;
	  
	  f[1] = (*Itr1)->phi() - minPhi;
	  if(f[1]<0.0) {
	    f[1]+=2*M_PI;
	  }
	  
	  r[1] = (*Itr1)->r();
	  z[1] = (*Itr1)->z();
	  
	  dzdr21 = (z[1]-z[0])/(r[1]-r[0]);
	  k21 = (f[1]*r[1]-f[0]*r[0])/(r[1]-r[0]);

	  Itr2 = Itr1; ++Itr2;
	  for ( long k=j+1; k<nHits; ++k, ++Itr2 ) {
	    l[2] = (*Itr2)->layer();
	    if ( l[2]==l[1] ) continue;
	    
	    e[2] = ((*Itr2)->z()-zVertex)/(*Itr2)->r();
	    e[2] = log(e[2]+sqrt(1.+e[2]*e[2]));
	    
	    if ( fabs(e[2]-e[1]) > m_cuts.etaCutOuter ) continue;
	    
	    r[2] = (*Itr2)->r();
	    z[2] = (*Itr2)->z();
	    
	    ddzdr = dzdr21-(z[2]-z[1])/(r[2]-r[1]);
	    
	    if ( fabs(ddzdr) > m_cuts.deltadZdRCut ) continue;
	    
	    f[2] = (*Itr2)->phi() - minPhi;
	    if(f[2]<0.0) {
	      f[2]+=2*M_PI;
	    }
	    
	    double k32 = (f[2]*r[2]-f[1]*r[1])/(r[2]-r[1]);
	    double dfdr = (k32-k21)/(r[2]-r[0]);
	    
#ifdef IDSCAN_DEBUG
	    std::cout << "makeHashKey 1: 1000*dfdrcut(<)dfdr " << 1000*(m_cuts.dPhidRCut) << " / " << 1000*dfdr << std::endl;
#endif

	    if ( fabs(dfdr)>m_cuts.dPhidRCut ) continue;
	    
	    double f0 = k21 - dfdr*(r[0]+r[1]);
	    
	    double d0 = fr1-dfdr*rr1-f0*r[0];

#ifdef IDSCAN_DEBUG
	    std::cout << "makeHashKey 2: 1000*d0cut/d0 " << 1000*(m_cuts.d0Cut) << " / " << 1000*d0 << std::endl;
#endif

	    if ( fabs(d0)>m_cuts.d0Cut ) continue;
	    
	    f0 += dfdr*m_cuts.phi0Offset;
	    
	    long sp_key = 0;
	    sp_key = (*m_pKeyGen)(dfdr + m_cuts.dPhidRCut, f0);
	    (*m_pHough)[sp_key].AddTriplet( *Itr0, *Itr1, *Itr2 );
	  }
	}
      }
    }

    void m_reducedCombinatorics(const TrigL2TrackCandidate* pTrack, double minPhi) {//former fillMapGC1

      //   Make hit triplets from different layer and if they satisfy certain cuts 
      //   fill the 2D histogram/map in (phi0,dPhidRho).
      //   This is the fancy way of making triplets. It is faster than m_bruteForce for nHits > ~20
      //

      double zVertex = pTrack->m_params.m_z0;

      long used[MAX_SILICON_LAYER_NUM]={0};
      std::vector<const TrigSiSpacePointBase*> ll[MAX_SILICON_LAYER_NUM];
      const std::vector<const TrigSiSpacePointBase*>& hitList = pTrack->spacePoints();
      
      for ( std::vector<const TrigSiSpacePointBase*>::const_iterator i = hitList.begin(); 
	    i != hitList.end(); ++i ) {
	long lr = (*i)->layer();
	ll[lr].push_back(*i);
	used[lr]=1;
      }
      long filled[MAX_SILICON_LAYER_NUM];
      long nf = 0;
      for ( long i = 0; i < MAX_SILICON_LAYER_NUM; ++i ) {
	if ( used[i] ) {
	  filled[nf++] = i;
	}
      }

      double rr1,fr1, dzdr21, k21, ddzdr;
      
      double r[3],z[3],f[3],e[3];
      for ( long i = 0; i < nf-2; ++i ) {
	for ( std::vector<const TrigSiSpacePointBase*>::const_iterator iit = ll[filled[i]].begin(); 
	      iit != ll[filled[i]].end(); ++iit ) {
	  
#ifdef IDSCAN_DEBUG
	  std::cout << "layer1: " << filled[i] << std::endl;
#endif
	  
	  f[0] = (*iit)->phi() - minPhi;
	  if(f[0]<0.0) {
	    f[0]+=2*M_PI;
	  }
	  r[0] = (*iit)->r();
	  z[0] = (*iit)->z();
	  
	  e[0] = (z[0]-zVertex)/r[0];
	  e[0] = log(e[0]+sqrt(1.+e[0]*e[0]));
	  
	  rr1 = r[0]*r[0];
	  fr1 = r[0]*f[0];
	  for ( long j = i+1; j < nf-1; ++j ) {
	    for ( std::vector<const TrigSiSpacePointBase*>::const_iterator jit = ll[filled[j]].begin(); 
		  jit != ll[filled[j]].end(); ++jit ) {
	      
#ifdef IDSCAN_DEBUG
	      std::cout << "layer1/2: " << filled[i] << "/" << filled[j] << std::endl;
#endif
	      
	      e[1] = ((*jit)->z()-zVertex)/(*jit)->r();
	      e[1] = log(e[1]+sqrt(1.+e[1]*e[1]));
	      
#ifdef IDSCAN_DEBUG
	      std::cout << " d_etacutInner/21: " << 1000*(m_cuts.etaCutInner) << " / " << 1000*(e[1]-e[0]) << std::endl;
#endif
	      
	      if ( fabs(e[1]-e[0]) > m_cuts.etaCutInner ) continue; //cut is on delta eta actually
	      
	      f[1] = (*jit)->phi() - minPhi;
	      if(f[1]<0.0) {
		f[1]+=2*M_PI;
	      }
	      
	      r[1] = (*jit)->r();
	      z[1] = (*jit)->z();
	      
	      dzdr21 = (z[1]-z[0])/(r[1]-r[0]);
	      k21 = (f[1]*r[1]-f[0]*r[0])/(r[1]-r[0]);
	      
	      for ( long k = j+1; k < nf; ++k ) {
		for ( std::vector<const TrigSiSpacePointBase*>::const_iterator kit = ll[filled[k]].begin(); 
		      kit != ll[filled[k]].end(); ++kit ) {
		  
#ifdef IDSCAN_DEBUG
		  std::cout << "layer1/2/3: " << filled[i] << "/" << filled[j] << "/" << filled[k] << std::endl;
#endif

		  e[2] = ((*kit)->z()-zVertex)/(*kit)->r();
		  e[2] = log(e[2]+sqrt(1.+e[2]*e[2]));
		  
#ifdef IDSCAN_DEBUG
		  std::cout << " d_etacutOuter/321: " << 1000*(m_cuts.etaCutOuter) << " / " << 1000*(e[2]-e[1]) << std::endl;
#endif

		  if ( fabs(e[2]-e[1]) > m_cuts.etaCutOuter ) continue;
		  r[2] = (*kit)->r();
		  z[2] = (*kit)->z();
		  ddzdr = dzdr21-(z[2]-z[1])/(r[2]-r[1]);
		  
#ifdef IDSCAN_DEBUG
		  std::cout << "   1000ddzdr, 1000deltadZdRCut " << 1000*m_cuts.deltadZdRCut << 1000*ddzdr << " / "  << std::endl;
#endif

		  if ( fabs(ddzdr) > m_cuts.deltadZdRCut ) continue;
		  
		  f[2] = (*kit)->phi() - minPhi;
		  if(f[2]<0.0) {
		    f[2]+=2*M_PI;
		  }

		  double k32 = (f[2]*r[2]-f[1]*r[1])/(r[2]-r[1]);
		  double dfdr = (k32-k21)/(r[2]-r[0]);
		  
#ifdef IDSCAN_DEBUG
		  std::cout << "makeHashKey 1: 1000*dfdrcut(<)dfdr " << 1000*(m_cuts.dPhidRCut) << " / " << 1000*dfdr << std::endl;
#endif

		  if ( fabs(dfdr)>m_cuts.dPhidRCut ) continue;

		  double f0 = k21 - dfdr*(r[0]+r[1]);
		  
		  double d0 = fr1-dfdr*rr1-f0*r[0];

#ifdef IDSCAN_DEBUG
		  std::cout << "makeHashKey 2: 1000*d0cut/d0 " << 1000*(m_cuts.d0Cut) << " / " << 1000*d0 << std::endl;
#endif

		  if ( fabs(d0)>m_cuts.d0Cut ) continue;

		  f0 += dfdr*m_cuts.phi0Offset;

		  long sp_key = 0;
		  
		  sp_key = (*m_pKeyGen)(dfdr + m_cuts.dPhidRCut, f0);
		  (*m_pHough)[sp_key].AddTriplet( *iit, *jit, *kit );
		  
#ifdef IDSCAN_DEBUG
		  std::cout << "                Made sp_key: " << sp_key << std::endl;
#endif
		  
		}
	      }
	    }
	  }
	}
      }
      
      for (long i = 0; i < nf; ++i ) {
	ll[filled[i]].clear();
      }
    }

};


#endif
