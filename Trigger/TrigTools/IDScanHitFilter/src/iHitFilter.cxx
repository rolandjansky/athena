/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// iHitFilter
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

// IdScan stuff
#include "GC_Constants.h"
#include "IdScanSpPoint.h"
#include "Group.h"
#include "FilterBin.h"
#include "iHitFilter.h"

#include <iostream>
#include <algorithm>

#include <cmath>
#include <cstring>

//#define IDSCAN_DEBUG 1

using std::log;
using std::exp;
using std::atan;
using std::fabs;
using std::cerr;
using std::endl;
using std::memset;

int iHitFilter::initialise(long maxlayers, long maxBarrelLayer, double maxeta) 
{

  m_IdScan_MaxNumLayers = maxlayers;
  m_maxBarrelLayer      = maxBarrelLayer;
  m_IdScan_MaxEta       = maxeta;

  if ( m_fullScan ) m_useROIphiHalfWidth = M_PI;
  else              m_useROIphiHalfWidth = m_ROIphiHalfWidth;

  //in some cases I got 360/3.6 = 99, so divide with something a bit less than m_phiBinSize
  m_maxPhiBins = long(360./(m_phiBinSize-1e-6));

  // avoid obliging the user to use phi bin size that is an integer multiple of 360.
  double tmp_phiBinSize = 360./double(m_maxPhiBins);

  m_invPhiBinSize = 180./(M_PI*tmp_phiBinSize);
  m_invEtaBinSize = 1./m_etaBinSize;

  m_binMap = new FilterMap;

  m_tight = new GCP_tight(m_tight_deltadzdrCut, 
			  m_tight_etaCutInner, 
			  m_tight_etaCutOuter,
			  m_tight_d0Cut,
			  m_tight_dPhidRBinSize,
			  m_tight_phi0Bins,
			  m_dPhidRCut
			  );

#ifdef IDSCAN_DEBUG
  std::cout << "tight params: ddzdr,etainner,etaouter,d0,dphidrbinsize,dpdrcut: " << m_tight_deltadzdrCut
	    << " / " <<   m_tight_etaCutInner 
	    << " / " <<	  m_tight_etaCutOuter
	    << " / " <<	  m_tight_d0Cut
	    << " / " <<	  m_tight_dPhidRBinSize
	    << " / " <<	  m_tight_phi0Bins
	    << " / " <<    m_dPhidRCut << std::endl;
#endif


  m_loose = new GCP_loose(m_loose_deltadzdrCut,
			  m_loose_etaCutInner,
			  m_loose_etaCutOuter,
			  m_loose_d0Cut,
			  m_loose_dPhidRBinSize,
			  m_loose_phi0Bins,
			  m_dPhidRCut
			  );

#ifdef IDSCAN_DEBUG
  std::cout << "loose params: ddzdr,etainner,etaouter,d0,dphidrbinsize,dpdrcut: " << m_loose_deltadzdrCut
	    << " / " <<   m_loose_etaCutInner 
	    << " / " <<	  m_loose_etaCutOuter
	    << " / " <<	  m_loose_d0Cut
	    << " / " <<	  m_loose_dPhidRBinSize
	    << " / " <<	  m_loose_phi0Bins
	    << " / " <<    m_dPhidRCut << std::endl;
#endif

  return 0;
}

iHitFilter::~iHitFilter()
 { 
   if (m_binMap) delete m_binMap; 
   if (m_loose)  delete m_loose;
   if (m_tight)  delete m_tight;
 }


void iHitFilter::makeIDScanSPs( std::vector<const ExternalSpacePoint* > spVec, 
				double meanEta, double deltaEta,
				double meanPhi, double deltaPhi,
				double zVertex) 
{

  int Counter=1;
  
  m_zVertex = zVertex;

  double roiPhiMin, roiPhiMax;

  if ( m_fullScan ) { 
    meanPhi=0;
    meanEta=0;
    roiPhiMin=-M_PI;
    roiPhiMax= M_PI;
  }
  else { 

    // If we trust that all the SPs are properly input, we determine the RoI phi width
    //  using the SPs themselves.
    //  If the RoI phi range is wider than pi, ie. if the RoI is large enough, we keep everything as usual.
    //   if ( 2 * m_useROIphiHalfWidth < M_PI )
    if ( 2 * deltaPhi < M_PI )
      {
	double roiPhiPosMin( 9.9), roiPhiPosMax(0);
	double roiPhiNegMin(-9.9), roiPhiNegMax(0);  // least negative and most negative
	
	std::vector<const ExternalSpacePoint* >::const_iterator SpItr( spVec.begin() );
	for(unsigned int i=0; i<spVec.size(); ++i, ++SpItr)
	  {
	    double spphi = (*SpItr)->phi();
	    if ( spphi>0  &&  spphi>roiPhiPosMax )  roiPhiPosMax = spphi;
	    if ( spphi>0  &&  spphi<roiPhiPosMin )  roiPhiPosMin = spphi;
	    
	    if ( spphi<0  &&  spphi<roiPhiNegMax )  roiPhiNegMax = spphi;
	    if ( spphi<0  &&  spphi>roiPhiNegMin )  roiPhiNegMin = spphi;
	  }
	
	// if all SPs are in (0, pi):
	if ( roiPhiNegMax > roiPhiNegMin )  { roiPhiMin = roiPhiPosMin; roiPhiMax = roiPhiPosMax; }
	// if all SPs are in (-pi, 0):
	else if ( roiPhiPosMax < roiPhiPosMin )  { roiPhiMin = roiPhiNegMax; roiPhiMax = roiPhiNegMin; }
	// if we have an RoI that covers phi=0 axis
	else if ( roiPhiPosMin - roiPhiNegMin < M_PI )  { roiPhiMin = roiPhiNegMax; roiPhiMax = roiPhiPosMax; }
	// if we have an RoI that covers phi=pi axis
	else  { roiPhiMin = roiPhiPosMin; roiPhiMax = roiPhiNegMin; }
	
	roiPhiMin -= 1e-10;
	roiPhiMax += 1e-10;
	//double ROIphiWidth = roiPhiMax - roiPhiMin;
	//if ( ROIphiWidth < 0 )  ROIphiWidth += 2*M_PI;
	//m_useROIphiHalfWidth = ROIphiWidth / 2.;
      }
    else
      {
	//	roiPhiMin = meanPhi - m_useROIphiHalfWidth;
	//	roiPhiMax = meanPhi + m_useROIphiHalfWidth;
	roiPhiMin = meanPhi - deltaPhi;
	roiPhiMax = meanPhi + deltaPhi;
      }
    
    if ( roiPhiMin < -M_PI ) roiPhiMin += 2.0*M_PI;
    if ( roiPhiMax >  M_PI ) roiPhiMax -= 2.0*M_PI;

    //    std::cout << "TrigIDSCAN IDScanHitFilter roiPhiMin " << roiPhiMin << std::endl;
    //    std::cout << "TrigIDSCAN IDScanHitFilter roiPhiMax " << roiPhiMax << std::endl;

  }

  bool boundary = (roiPhiMin>roiPhiMax);
  
  if ( m_internalSPs!=NULL ) cleanInternalSPs();
  m_internalSPs = new hitVec;
  m_internalSPs->reserve( spVec.size() );
  
  //   Now that we know the z of the pp vertex, we can move from a trapezium shape RoI 
  //   to a triangle shape and get rid of some extra hits that are of no interest
  //


  //  std::cout << "TrigIDSCAN IDScanHitFilter  m_ROIetaHalfWidth " << m_ROIetaHalfWidth << std::endl;
  //  std::cout << "TrigIDSCAN IDScanHitFilter  roi->etaHalfWidth " << deltaEta << std::endl;

  // NB: should set a default full scan range when m_fullScan==true but it is 
  // etaHalfWidth=3 for the fullscan slice and etaHalfWidth=5 for the cosmic, 
  // so which to chose? left as it is for the moment
  //  double dEta = m_ROIetaHalfWidth;
  double dEta = deltaEta;

  // The 'core' resolution from muFast is ~0.015
  dEta += 0.045;

  // SP etas can change significantly when shifted to account for an offset beamspot.
  // Offset BS affects most the hits on the B-layer ==> 50mm.
  dEta += (  std::fabs(meanEta)  +
	     log(tan(atan(50./((50-m_maxBeamSpotShift)/tan(2.*atan(exp(-std::fabs(meanEta))))))/2.))  );

  // An extra eta range to account for the fact that the RoI eta is computed with for z=0
  // Assume that the eta of the RoI is computed at the radius of the calorimeter and compute
  //   the eta of an RoI which is said to be at eta=0.
  double extraEta(0);
  if ( zVertex!=0 )  extraEta -= log(tan(atan(2800./std::fabs(zVertex))/2));

  //  std::cout << "TrigIDSCAN IDScanHitFilter  dEta "     << dEta     << std::endl;
  //  std::cout << "TrigIDSCAN IDScanHitFilter  extraEta " << extraEta << std::endl;
  
  std::vector<const ExternalSpacePoint* >::const_iterator siSpItr( spVec.begin() );
  for ( ; siSpItr!=spVec.end(); ++siSpItr ) {
    double eta = (*siSpItr)->eta(zVertex);

    if ( std::fabs(eta)>m_IdScan_MaxEta ) continue;
    if ( zVertex<=0 && ( eta-meanEta>dEta+extraEta || meanEta-eta>dEta ) ) continue;
    if ( zVertex> 0 && ( meanEta-eta>dEta+extraEta || eta-meanEta>dEta ) ) continue;

    IdScanSpPoint* pSP=new IdScanSpPoint( *siSpItr, eta );
    pSP->index(Counter++);
    m_internalSPs->push_back( pSP );
  }

  long nSPs = m_internalSPs->size();
  std::vector<double> rotatedPhi(nSPs);

  hitVec::iterator SpItr = m_internalSPs->begin();
  for ( long i=0; i<nSPs; ++i, ++SpItr ) {
    rotatedPhi[i] = (*SpItr)->phi() - roiPhiMin;
  }

  if ( boundary ){ 
    for ( long i=0; i<nSPs; ++i ) {
      if(rotatedPhi[i]<0.0) rotatedPhi[i]+=2.0*M_PI;
    }
  }

  SpItr = m_internalSPs->begin();
  for ( long i=0; i<nSPs; ++i, ++SpItr ) {
    (*SpItr)->setRotatedPhi( rotatedPhi[i] );
  }

}


void iHitFilter::cleanInternalSPs() { 
  for ( hitVec::iterator spItr( m_internalSPs->begin() ); spItr != m_internalSPs->end(); ++spItr ) {
    delete (*spItr);
  }
  m_internalSPs->clear();
  delete m_internalSPs;
  m_internalSPs = NULL;
}    


GroupList iHitFilter::execute()
{

  if ( m_internalSPs==NULL ) { 
    cerr << "iHitFilter::execute() m_internalSPs=NULL" << endl;
  }

  m_HF = true; // we are in hitFilter; becomes false when we go to groupCleaner

//   1. Loop over the hits to make bins and fill the binMap
//
  long sp_key;
  hitVec::iterator hitItr( m_internalSPs->begin() ); 
  hitVec::iterator hitEnd( m_internalSPs->end() );

  for ( ; hitItr != hitEnd;  ++hitItr ) {
    sp_key = makeHashKey( (*hitItr)->rotatedPhi(), (*hitItr)->eta() );
    (*m_binMap)[sp_key].AddHit( *hitItr, m_IdScan_MaxNumLayers, m_maxBarrelLayer );
#ifdef IDSCAN_DEBUG
    std::cout << "IDSCAN_DEBUG hitfilter: layer/phi/z/eta/sp_key: " 
	      << (*hitItr)->layer() << " / " << (*hitItr)->phi() << " / " << (*hitItr)->z() << " / " << sp_key << std::endl;
#endif
  }

//   Special treatment for hits at pixel layer 0. Optionally enter them two more times
//   in the binMap, at +/- 2 bins in eta, as their eta is affected most by mis-measurements
//   in the ZFinder. Important mostly for low p_T tracks (B physics, b-tagging).
//

  if ( m_enhanceLayer0 ) {
    long offset = 2*HashKeyCoeff;
    for ( hitItr = m_internalSPs->begin(); hitItr != hitEnd;  ++hitItr ) {
      if ( (*hitItr)->layer() == 0 ) {
 	sp_key = this->makeHashKey( (*hitItr)->rotatedPhi(), (*hitItr)->eta() );
	(*m_binMap)[sp_key+offset].AddHit( *hitItr, m_IdScan_MaxNumLayers, m_maxBarrelLayer );
	(*m_binMap)[sp_key-offset].AddHit( *hitItr, m_IdScan_MaxNumLayers, m_maxBarrelLayer );
      }
    }
  }

//   2. Loop over bins to form groups
//
  GroupList gList, cleanGroups;
  this->clusterBins( gList );
  m_HF = false;

//   3. Loop over groups to delete "small" groups and run second level filter (group cleaner)
//
  GroupList::iterator gItr( gList.begin() ), gEnd( gList.end() );
  while( gItr != gEnd ) {
    if ( m_enhanceLayer0 ) {    // if 0 layer hits entered 3 times, remove duplicates first
      gItr->removeDuplicates();
    }
    if ( gItr->NumberOfHits()>m_layerThresholdTmp ) {

#ifdef IDSCAN_DEBUG
      std::cout << "IDSCAN_DEBUG: " << std::endl;
      std::cout << "IDSCAN_DEBUG: Group entering the groupCleaner - number of sps: " 
		<< gItr->NumberOfHits() << std::endl;
      std::list<IdScanSpPoint *> hitList = gItr->groupHits();
      for ( std::list<IdScanSpPoint *>::const_iterator i = hitList.begin(); i != hitList.end(); ++i ) {
	std::cout << "IDSCAN_DEBUG: layer = " << (*i)->layer()
		  << " phi= " << (*i)->phi()
		  << " rho= " << (*i)->rho()
		  << " zed= " << (*i)->z()
		  << std::endl;
      }
#endif

      this->groupCleaner( gItr, cleanGroups );
      gList.erase( gItr++ );
    }
    else {
      gList.erase( gItr++ );
    }
  }

//   4. Loop over cleaned groups to delete "small" groups
//
  GroupList::iterator gItr2( cleanGroups.begin() ), gEnd2( cleanGroups.end() );
  for ( ; gItr2 != gEnd2; ) { 
    if(gItr2->NumberOfHits()!=0)
      {
	if ( gItr2->NumberOfHits() > m_layerThresholdTmp ) {
	  ++gItr2;
	}
	else {
	  cleanGroups.erase(gItr2++);
	}
      }
    else cleanGroups.erase(gItr2++);
  }

  return cleanGroups;
}

void iHitFilter::groupCleaner( GroupList::iterator& inputItr, GroupList& cleanGroups )
{

  groupCleanerParams* params;
  if( inputItr->NumberOfHits() > m_looseTight ) {
    params = m_tight;

#ifdef IDSCAN_DEBUG
    std::cout << "IDSCAN_DEBUG: tight set of grouCleaner parameters" << std::endl;
#endif

  }
  else {
    params = m_loose;

#ifdef IDSCAN_DEBUG
    std::cout << "IDSCAN_DEBUG: loose set of grouCleaner parameters" << std::endl;
#endif

  }
 

  // deal with phi wrap-around when in full scan mode
  if ( m_fullScan) {
    double posPhi = 0;
    double negPhi = 0;
    std::list<IdScanSpPoint *> hitList = inputItr->groupHits();
    for ( std::list<IdScanSpPoint *>::const_iterator i = hitList.begin(); 
	  i != hitList.end(); ++i ) {
      if ((*i)->phi()<-3.0) negPhi++;
      if ((*i)->phi()> 3.0) posPhi++;
    }

    if (negPhi>0 && posPhi>0) {   // this group is in the phi wrap-around 

#ifdef IDSCAN_DEBUG
      std::cout << "IDSCAN_DEBUG: had to deal with phi wrap-around: " << posPhi<< "/" << negPhi << std::endl;
#endif

      for ( std::list<IdScanSpPoint *>::const_iterator i = hitList.begin(); 
	    i != hitList.end(); ++i ) {
	if ((*i)->phi()<0) ((*i)->setRotatedPhi(((*i)->rotatedPhi()+2.0*M_PI)));
      }
    }
  }

  this->fillMapGC1( inputItr, params );
  //  this->fillMapGC2(inputItr, params );

// Same clustering algo as before: Loop over bins to form groups
//
  unsigned long groupsBefore = cleanGroups.size();
  this->clusterBins( cleanGroups );
  unsigned long newGroups = cleanGroups.size()-groupsBefore;

#ifdef IDSCAN_DEBUG
  std::cout << "IDSCAN_DEBUG: grouCleaner found " << newGroups << " groups in the original group." << std::endl;
#endif

  if (newGroups!=0)
    {
      double eta = (inputItr->bin1()+0.5)/m_invEtaBinSize-m_IdScan_MaxEta;
      GroupList::reverse_iterator gItr2=cleanGroups.rbegin();
      for ( unsigned long i=0; i<newGroups;++i,++gItr2)
	{
	  double dfdrho = ((gItr2->bin2()+0.5)/params->invdPhidRBinSize)-params->dPhidRCut;
	  double ptinv =  -dfdrho/params->dPhidRCut/1000.;
	  double phi0 = (gItr2->bin1()+0.5)/params->invPhi0BinSize - dfdrho*params->phi0Offset;
	  gItr2->setPhi0( phi0 );
	  gItr2->setPtInv( ptinv );
	  gItr2->setEta( eta );
          gItr2->setz0( m_zVertex );
	  gItr2->removeDuplicates();

#ifdef IDSCAN_DEBUG
	  std::cout << "IDSCAN_DEBUG: New grouCleaner group with number of hits = " << gItr2->NumberOfHits() 
		    << " 1/pT= " << ptinv 
		    << " phi0= " << phi0
		    << " eta= " << eta 
		    << " z0= " << m_zVertex
		    << std::endl;
#endif

          for (IdScanSpPoint* pSP : gItr2->groupHits()) {
	    pSP->setUsed(1);
#ifdef IDSCAN_DEBUG
	    std::cout << "IDSCAN_DEBUG: new group - layer = " << pSP->layer()
		      << " phi= " << pSP->phi()
		      << " rho= " << pSP->rho()
		      << " zed= " << pSP->z()
		      << std::endl;
#endif
	  }
	  
	  int badHit=0;
	  //setting used=0 to spacepoints in the same layer
	  for(std::list<IdScanSpPoint *>::iterator phIt1=(*gItr2).groupHits().begin();
	      phIt1!=(*gItr2).groupHits().end();++phIt1) 
	    {
	      //if((*phIt1)->used()==0) continue;
	      int lay1=(*phIt1)->layer();
	      int type1=((*phIt1)->dz()<(*phIt1)->dr())?1:0;
	      std::list<IdScanSpPoint *>::iterator phIt2=phIt1;
	      ++phIt2;
	      for(;phIt2!=(*gItr2).groupHits().end();++phIt2)
		{
		  //if((*phIt2)->used()==0) continue;
		  int lay2=(*phIt2)->layer();
		  int type2=((*phIt2)->dz()<(*phIt2)->dr())?1:0;
		  if((lay2==lay1)&&(type2==type1))
		    {
 		      if(((type1==0)&&(std::fabs((*phIt2)->rho()-(*phIt1)->rho())<5.0))
 			 ||((type1==1)&&(std::fabs((*phIt2)->z()-(*phIt1)->z())<2.0)))
 			{
			  if((*phIt2)->used()!=0) 
			    {
			      badHit++;
			      (*phIt2)->setUsed(0);
			    }
			  if((*phIt1)->used()!=0) 
			    {
			      badHit++;
			      (*phIt1)->setUsed(0);
			    }
 			}
		    }
		}
	    }
	  //printf("%d duplicated hits detected...\n",badHit);
	  fitGroup((*gItr2));
	  //fitGroupWithoutHitRemoval((*gItr2));

	  //deleting spacepoints that have used==0
	  if(badHit!=0)
	    {
	      std::list<IdScanSpPoint *>::iterator hitItr=(*gItr2).groupHits().begin();
	      while(hitItr!=(*gItr2).groupHits().end())
		{
		  IdScanSpPoint* pSP=(*hitItr);
		  if(pSP->used()==0)
		    {
		      (*gItr2).groupHits().erase(hitItr++);pSP->setUsed(1);
		    }
		  else 
		    {
		      ++hitItr;pSP->setUsed(1);
		    }
		}
	    }

	  if (badHit!=0)
            {
	      gItr2->setChi2(1e9);
            }
          else
            {
	      gItr2->setChi2(0);
            }

#ifdef IDSCAN_DEBUG
	  std::cout << "IDSCAN_DEBUG: after ambig. resol. - number of hits = " << (*gItr2).NumberOfHits() << std::endl;
	  for( hitItr=(*gItr2).groupHits().begin(); hitItr!=(*gItr2).groupHits().end(); hitItr++) {
	    IdScanSpPoint* pSP=(*hitItr);
	    std::cout << "IDSCAN_DEBUG: after ambig. resol. - layer = " << pSP->layer()
		      << " phi= " << pSP->phi()
		      << " rho= " << pSP->rho()
		      << " zed= " << pSP->z()
		      << std::endl;
	  }
#endif
	}


      if((newGroups>1)&&m_cloneRemoval)
	{
	  TrackFilterClass* pTF = new TrackFilterClass(m_usePtSign);
	  gItr2=cleanGroups.rbegin();
	  for ( unsigned long i=0; i<newGroups;++i,++gItr2)
	    {
	      pTF->addNewTrackCandidate(&(*gItr2));
	    }
	  pTF->removeClones(m_cloneRemovalCut);
	  delete pTF;
	}
#ifdef IDSCAN_DEBUG
	  std::cout << "IDSCAN_DEBUG: after clone removal remaining groups = " << cleanGroups.size()-groupsBefore << std::endl;

	  gItr2=cleanGroups.rbegin();
	  for ( unsigned long i=0; i<cleanGroups.size()-groupsBefore; ++i,++gItr2) {
	    std::cout << "IDSCAN_DEBUG: Final group after clone rem. with number of hits = " << gItr2->NumberOfHits() 
		      << " 1/pT= " << gItr2->getPtInv()
		      << " phi0= " << gItr2->getPhi0()
		      << " eta= " << gItr2->getEta()
		      << " z0= " << gItr2->getz0()
		      << std::endl;
	  }

#endif

    }
}


void iHitFilter::fillMapGC1( GroupList::iterator& inputItr, groupCleanerParams* params ) const
{
//   Make hit triplets from different layer and if they satisfy certain cuts 
//   fill the 2D histogram/map in (phi0,dPhidRho).
//   This is the fancy way of making triplets. It is faster than fillMapGC1 for nHits > ~20
//
  std::vector<long> used(m_IdScan_MaxNumLayers,0);
  std::vector<std::list<IdScanSpPoint*> > ll(m_IdScan_MaxNumLayers);
  std::list<IdScanSpPoint*> hitList = inputItr->groupHits();
  for ( std::list<IdScanSpPoint*>::const_iterator i = hitList.begin(); 
       i != hitList.end(); ++i ) {
    long lr = (*i)->layer();
    ll[lr].push_back(*i);
    used[lr]=1;
  }
  std::vector<long> filled(m_IdScan_MaxNumLayers);
  long nf = 0;
  for ( long i = 0; i < m_IdScan_MaxNumLayers; ++i ) {
    if ( used[i] ) {
      filled[nf++] = i;
    }
  }

  double rr1,fr1, dzdr21, k21, ddzdr;
  long sp_key;
  double r[3],z[3],f[3],e[3];
  for ( long i = 0; i < nf-2; ++i ) {
    for ( std::list<IdScanSpPoint *>::const_iterator iit = ll[filled[i]].begin(); 
	 iit != ll[filled[i]].end(); ++iit ) {

#ifdef IDSCAN_DEBUG
      std::cout << "layer1: " << filled[i] << std::endl;
#endif

      f[0] = (*iit)->rotatedPhi();
      r[0] = (*iit)->rho();
      z[0] = (*iit)->z();
      e[0] = (*iit)->eta();
      rr1 = r[0]*r[0];
      fr1 = r[0]*f[0];
      for ( long j = i+1; j < nf-1; ++j ) {
	for ( std::list<IdScanSpPoint *>::const_iterator jit = ll[filled[j]].begin(); 
	     jit != ll[filled[j]].end(); ++jit ) {

#ifdef IDSCAN_DEBUG
	  std::cout << "layer1/2: " << filled[i] << "/" << filled[j] << std::endl;
#endif

	  e[1] = (*jit)->eta();

#ifdef IDSCAN_DEBUG
	  std::cout << " d_etacutInner/21: " << 1000*(params->etaCutInner) << " / " << 1000*(e[1]-e[0]) << std::endl;
#endif

 	  if ( std::fabs(e[1]-e[0]) > params->etaCutInner ) continue;
 	  f[1] = (*jit)->rotatedPhi();
	  r[1] = (*jit)->rho();
	  z[1] = (*jit)->z();
	  dzdr21 = dzdr( r, z );
	  k21 = ktemp( r, f );
	  for ( long k = j+1; k < nf; ++k ) {
	    for ( std::list<IdScanSpPoint *>::const_iterator kit = ll[filled[k]].begin(); 
		 kit != ll[filled[k]].end(); ++kit ) {

#ifdef IDSCAN_DEBUG
	      std::cout << "layer1/2/3: " << filled[i] << "/" << filled[j] << "/" << filled[k] << std::endl;
#endif

	      e[2] = (*kit)->eta();

#ifdef IDSCAN_DEBUG
	      std::cout << " d_etacutOuter/321: " << 1000*(params->etaCutOuter) << " / " << 1000*(e[2]-e[1]) << std::endl;
#endif

   	      if ( std::fabs(e[2]-e[1]) > params->etaCutOuter ) continue;
	      r[2] = (*kit)->rho();
	      z[2] = (*kit)->z();
	      ddzdr = dzdr21-dzdr( &r[1], &z[1] );

#ifdef IDSCAN_DEBUG
	      std::cout << "   1000ddzdr, 1000deltadZdRCut " << 1000*params->deltadZdRCut << 1000*ddzdr << " / "  << std::endl;
#endif

 	      if ( std::fabs(ddzdr) > params->deltadZdRCut ) continue;
 	      f[2] = (*kit)->rotatedPhi();
	      sp_key = this->makeHashKey( r, f, k21, rr1, fr1, params );
	      if ( sp_key ) {
		(*m_binMap)[sp_key].AddTriplet( *iit, *jit, *kit );

#ifdef IDSCAN_DEBUG
		std::cout << "                Made sp_key: " << sp_key << std::endl;
#endif

	      }
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


void iHitFilter::fillMapGC2(GroupList::iterator& inputItr, groupCleanerParams* params ) const
{
//   Make hit triplets from different layer and if they satisfy certain cuts 
//   fill the 2D histogram/map in (phi0,dPhidRho).
//   This is the brute force way of making triplets, which is the fastest for nHits < ~20
//
  long nHits = inputItr->NumberOfHits();
  std::list<IdScanSpPoint *> hitList = inputItr->groupHits();


  std::list<IdScanSpPoint *>::const_iterator Itr0( hitList.begin() );
  std::list<IdScanSpPoint *>::const_iterator Itr1, Itr2;

  long sp_key, l[3];
  double rr1, fr1, dzdr21, k21, ddzdr;
  double r[3],z[3],f[3],e[3];

  for ( long i=0; i<nHits-2; ++i, ++Itr0 ) {
    f[0] = (*Itr0)->rotatedPhi();
    r[0] = (*Itr0)->rho();
    z[0] = (*Itr0)->z();
    e[0] = (*Itr0)->eta();
    l[0] = (*Itr0)->layer();
    rr1 = r[0]*r[0];
    fr1 = r[0]*f[0];

    Itr1 = Itr0; ++Itr1;
    for ( long j=i+1; j<nHits-1; ++j, ++Itr1 ) {
      l[1] = (*Itr1)->layer();
      if ( l[1]==l[0] ) continue;

      e[1] = (*Itr1)->eta();
      if ( std::fabs(e[1]-e[0])>params->etaCutInner ) continue;

      f[1] = (*Itr1)->rotatedPhi();
      r[1] = (*Itr1)->rho();
      z[1] = (*Itr1)->z();
      dzdr21 = dzdr( r, z );
      k21 = ktemp( r, f );

      Itr2 = Itr1; ++Itr2;
      for ( long k=j+1; k<nHits; ++k, ++Itr2 ) {
	l[2] = (*Itr2)->layer();
	if ( l[2]==l[1] ) continue;

	e[2] = (*Itr2)->eta();
	if ( std::fabs(e[2]-e[1]) > params->etaCutOuter ) continue;

	r[2] = (*Itr2)->rho();
	z[2] = (*Itr2)->z();
	ddzdr = dzdr21-dzdr( &r[1], &z[1] );
	if ( std::fabs(ddzdr) > params->deltadZdRCut ) continue;

	f[2] = (*Itr2)->rotatedPhi();
	sp_key = this->makeHashKey( r, f, k21, rr1, fr1, params );

	if (sp_key) {
	  (*m_binMap)[sp_key].AddTriplet( *Itr0, *Itr1, *Itr2 );
	}
      }
    }
  }

}


void iHitFilter::clusterBins( GroupList& gList )
{
//   Loop over all the bins in the map and cluster them into groups. As soon as
//   bins are processed they get deleted with the fancy m_binMap->erase( binItr++ ) 
//   so at the end of this loop m_binMap is empty and there is no need to .clear()!
//
  FilterMap::iterator binItr( m_binMap->begin() ), mapEnd( m_binMap->end() );
  for ( ;  binItr != mapEnd; m_binMap->erase( binItr++ ) ) {
    if ( (binItr->second).NumberOfHits() > 0 ) {
      m_binCounter = 0;
      this->joinGroup( binItr->first, binItr->second, gList.end(), gList );
      //      int binCounter = 0;
      //      this->joinGroup( binItr->first, binItr->second, gList.end(), gList, bincounter);
    }
  }
}


void iHitFilter::joinGroup( long key, FilterBin& bin, 
			   GroupList::iterator giter, GroupList& glist )
{
//   1. Find the 8 neighbouring bins and exchange layer numbers
//
  long np[8], Nneigh( 0 );
  this->neighborKeys(key, np );

#ifdef IDSCAN_DEBUG
  std::cout << "IDSCAN_DEBUG: in join groups - bincounter/key = " << m_binCounter << " / " << key << std::endl;
#endif

  FilterMap::iterator miter, mneigh[8], mEnd( m_binMap->end() );
  for( long i = 0; i < 8; ++i ) {

#ifdef IDSCAN_DEBUG
    std::cout << "IDSCAN_DEBUG: neighbour = " << i << " / " << np[i] << std::endl;
#endif

    if ( ( miter=m_binMap->find(np[i]) ) != mEnd ){
      mneigh[Nneigh++] = miter;
      bin.exchangeBits( miter->second ); 
    }
  }
  
//   2. Knowing the total number of layers in this bin, decide whether to proceed in 
//   forming/joining a group; if not, get out of here!
//
  if ( bin.NumberOfLayers() > m_layerThresholdTmp ) {

//   if the GroupList iterator giter is in the end, it means that this bin should start
//   the formation of a new group; if not, it means that it should join the group pointed
//   to by giter
// 
    if (giter == glist.end()){
      glist.push_back( Group() );
      giter-- = glist.end();
    }
    long bin1 = key/HashKeyCoeff;
    long bin2 = key%HashKeyCoeff;
    giter->AddHits( bin.hitList(), bin1, bin2 ); //   pass all hits from the bin to the group...
    
//   Now all the neighbours should follow the same procedure...
//
    for(long i=0; i<Nneigh; ++i){
      if ( (mneigh[i]->second).NumberOfHits() > 0 && m_binCounter<25){
	m_binCounter++;
	this->joinGroup( mneigh[i]->first, mneigh[i]->second, giter, glist );
      }
    }      
  }    
  
}


long iHitFilter::makeHashKey( double* r,  double* f, 
			     double k21, double rr1, 
			     double fr1, groupCleanerParams* params ) const
{
//   Makes the hash key for the (phi0,dPhidRho) map of the group cleaner.
//   Unlike before, here we calculate d0 and dPhidRho and make cuts on them,
//   so hash key = 0 means "ignore this hit-triplet". 
//
  double k32 = ktemp(&r[1],&f[1]);

  double dfdr = (k32-k21)/(r[2]-r[0]);

#ifdef IDSCAN_DEBUG
  std::cout << "makeHashKey 1: 1000*dfdrcut(<)dfdr " << 1000*(params->dPhidRCut) << " / " << 1000*dfdr << std::endl;
#endif

  if ( std::fabs(dfdr)>params->dPhidRCut ) return 0;

  double f0 = k21 - dfdr*(r[0]+r[1]);

  double d0 = fr1-dfdr*rr1-f0*r[0];

#ifdef IDSCAN_DEBUG
  std::cout << "makeHashKey 2: 1000*d0cut/d0 " << 1000*(params->d0Cut) << " / " << 1000*d0 << std::endl;
#endif

  if ( std::fabs(d0)>params->d0Cut ) return 0;

  f0 += dfdr*params->phi0Offset;

  long dfdrbin = (long) ( ( dfdr + params->dPhidRCut ) * params->invdPhidRBinSize );
  long f0bin = (long) ( f0*params->invPhi0BinSize );

#ifdef IDSCAN_DEBUG
  std::cout << "f0bin/dfdrbin:  " << f0bin << " / " << dfdrbin << std::endl;
#endif

  return HashKeyCoeff*f0bin + dfdrbin;
}


TrackCandidateClass::TrackCandidateClass(Group* grp)
{
  m_pGroup=grp;
  m_nHitShift=0;
  m_nSharedHits=0;
  m_nSharedTotal=0;
  m_fom=1.0*grp->groupHits().size();
}

TrackFilterClass::TrackFilterClass(bool usePtSign)
{
  m_vpTCC.clear();
  m_usePtSign=usePtSign;
}

TrackFilterClass::~TrackFilterClass()
{
  for(std::vector<TrackCandidateClass*>::iterator pTCCIt=m_vpTCC.begin();
      pTCCIt!=m_vpTCC.end();++pTCCIt)
    {
      delete (*pTCCIt);
    }
  m_vpTCC.clear();
}

void TrackFilterClass::addNewTrackCandidate(Group* pGrp)
{
  TrackCandidateClass* pTCC;
  pTCC=new TrackCandidateClass(pGrp);
  pTCC->m_pt=1.0/pGrp->getPtInv();
  m_vpTCC.push_back(pTCC);
}

bool compareTrackCandidates(const TrackCandidateClass* pTCC1, const TrackCandidateClass* pTCC2)
{
  if ( pTCC1->m_fom == pTCC2->m_fom ) return (std::fabs(pTCC1->m_pt) > std::fabs(pTCC2->m_pt) );
  return (pTCC1->m_fom > pTCC2->m_fom );
}

void TrackFilterClass::removeClones(int cut)
{
  int nTrackCounter=1;
  int nSign,nBinNum;
  char nContribHisto[1000];

  std::sort(m_vpTCC.begin(),m_vpTCC.end(),compareTrackCandidates);
  for(std::vector<TrackCandidateClass*>::iterator pTCCIt=m_vpTCC.begin();
      pTCCIt!=m_vpTCC.end();++pTCCIt)
    { 
      nTrackCounter++;
      nSign=((*pTCCIt)->m_pt<0)?-1:1;
      (*pTCCIt)->m_nHitShift=nTrackCounter*nSign;
      memset(nContribHisto,0,sizeof(nContribHisto));

      for (IdScanSpPoint* pSP : (*pTCCIt)->m_pGroup->groupHits())
	{
	  if(pSP->used()==0) continue;
	  if(pSP->used()==1)
	    pSP->setUsed((*pTCCIt)->m_nHitShift);
	  else
	    {
	      if((((pSP->used()*nSign)>0)&&(m_usePtSign))||(!m_usePtSign))
	      {
		nBinNum=abs(pSP->used())-1;
		if(nBinNum<1000)
		  {
		    nContribHisto[nBinNum]++;
		    if(nContribHisto[nBinNum]>(*pTCCIt)->m_nSharedHits)
		      (*pTCCIt)->m_nSharedHits++;
		  }
	      }
	      (*pTCCIt)->m_nSharedTotal++;
	    }
	}
    }
  for(std::vector<TrackCandidateClass*>::iterator pTCCIt=m_vpTCC.begin();
      pTCCIt!=m_vpTCC.end();++pTCCIt)
    {
      if( (*pTCCIt)->m_nSharedHits>=cut || (*pTCCIt)->m_nSharedTotal > 0.45*(*pTCCIt)->m_pGroup->NumberOfHits() )
	(*pTCCIt)->m_pGroup->groupHits().clear();
    }
}

typedef std::vector<IdScanSpPoint*> HIT_VECTOR;

bool CompareSpByLayer(const IdScanSpPoint* point1, const IdScanSpPoint* point2)
{
  return (point1->layer() < point2->layer());
}

void iHitFilter::fitGroup(Group& G)
{
  const double C=0.029997;
  const double maxChi2=500.0;
  const double maxChi2inGroup=6000.0;
  const double minPt=600.0;

  double Rk[5],Gk[5][5],Hk[2][5],Kk[5][2],resid[2],
    Bk[5][2],Vk[2][2],sigma[2][2],Dk[2][2];
  double chi2,dchi2,Bz,fConst,x,y,detr,bestChi2,averagePhi0=0.0,theta;
  int i,j,m,nAmbiLayers=0,nHits=0;

  HIT_VECTOR arrAmbiLayers[20];
  HIT_VECTOR spVec;
  int arrAmbiIdx[20];

  IdScanSpPoint* pBestSP;
  spVec.clear();

  theta=2.0*atan(exp(-G.getEta()));

  Rk[0]=0.0;
  //  Rk[1]=0.0;
  Rk[1]=G.getz0();
  Rk[2]=0.0;
  Rk[3]=cos(theta)/sin(theta);
  Rk[4]=G.getPtInv();

  memset(&Gk[0][0],0,sizeof(Gk));
  Gk[0][0]=100.0;
  Gk[1][1]=100.0;
  Gk[2][2]=100.0;
  Gk[3][3]=100.0;
  Gk[4][4]=0.01;
  memset(&arrAmbiIdx[0],0,sizeof(arrAmbiIdx));

  chi2=0.0;Bz=20.84;fConst=C*Bz;

  double minPhi=(*G.groupHits().begin())->phi();

  for (IdScanSpPoint* pSP : G.groupHits())
    {
      if(minPhi>pSP->phi()) minPhi=pSP->phi();
      spVec.push_back(pSP);
    }
  std::sort(spVec.begin(),spVec.end(),CompareSpByLayer);

  for (IdScanSpPoint* pSP : spVec)
    {
      nHits++;
      double delta=pSP->phi()-minPhi;
      if(std::fabs(delta)>M_PI) delta-=2.0*M_PI;
      averagePhi0+=delta;
    }
  averagePhi0/=nHits;averagePhi0+=minPhi;

#ifdef IDSCAN_DEBUG
  printf("Average PHI0=%f Initial Pt=%f\n",averagePhi0,1.0/G.getPtInv());
#endif

  for(std::vector<IdScanSpPoint *>::reverse_iterator hitItr=spVec.rbegin();hitItr!=spVec.rend();++hitItr) 
    {
      IdScanSpPoint* pSP=(*hitItr);
      if(pSP->used()!=0)
	{
	  x=pSP->rho()*cos(pSP->phi()-averagePhi0);
	  y=pSP->rho()*sin(pSP->phi()-averagePhi0);
#ifdef IDSCAN_DEBUG
	  printf("x=%f y=%f z=%f\n",x,y,pSP->z());
#endif
	  Hk[0][0]=1.0;Hk[0][1]=0.0;Hk[0][2]=x;
	  Hk[0][3]=0.0;Hk[0][4]=-0.5*x*x*fConst;
	  Hk[1][0]=0.0;Hk[1][1]=1.0;
	  Hk[1][2]=0.0;Hk[1][3]=x;Hk[1][4]=0.0;
	  for(i=0;i<5;i++)
	    for(j=0;j<2;j++) 
	      {
		Bk[i][j]=0.0;for(m=0;m<5;m++) Bk[i][j]+=Gk[i][m]*Hk[j][m];
	      }
	  for(i=0;i<2;i++)
	    for(j=0;j<2;j++)
	      {
		Vk[i][j]=0.0;for(m=0;m<5;m++) Vk[i][j]+=Hk[i][m]*Bk[m][j];
	      }

	  if(pSP->layer()<7)
	    {
	      sigma[0][0]=x*pSP->dphi();
	      sigma[1][1]=pSP->dz();
	      for(i=0;i<2;i++)
		Vk[i][i]+=sigma[i][i]*sigma[i][i];
	    }
	  else
	    {
	      sigma[1][1]=pSP->dr()*Rk[3]/cos(Rk[2]);
	      sigma[1][1]=sigma[1][1]*sigma[1][1];
	      double a=Rk[2]-x*fConst*Rk[4]-sin(pSP->phi()-averagePhi0);
	      sigma[0][0]=x*x*pSP->dphi()*pSP->dphi()+
		a*a*pSP->dr()*pSP->dr();
	      sigma[0][1]=sigma[1][0]=a*Rk[3]*pSP->dr()*pSP->dr();
	      for(i=0;i<2;i++)
		for(j=0;j<2;j++)
		  Vk[i][j]+=sigma[i][j];
	    }

	  detr=Vk[0][0]*Vk[1][1]-Vk[0][1]*Vk[1][0];detr=1.0/detr;
	  Dk[0][0]=Vk[1][1]*detr;Dk[0][1]=Dk[1][0]=-Vk[0][1]*detr;
	  Dk[1][1]=Vk[0][0]*detr;
	  
	  for(i=0;i<2;i++)
	    {
	      resid[i]=0.0;for(j=0;j<5;j++) resid[i]+=Hk[i][j]*Rk[j];
	    }
	  resid[0]=y-resid[0];
	  resid[1]=pSP->z()-resid[1];
	  dchi2=resid[0]*resid[0]*Dk[0][0]+2.0*Dk[0][1]*resid[0]*resid[1]+
	    resid[1]*resid[1]*Dk[1][1];
	  //	  printf("r1=%f r2=%f dchi2=%f\n",resid[0],resid[1],dchi2);
	  //printf("dchi2phi=%f dchi2z=%f\n",
	  //	 resid[0]*resid[0]*Dk[0][0],resid[1]*resid[1]*Dk[1][1]);
	  if(dchi2<maxChi2)
	    {
	      chi2+=dchi2;
	      for(i=0;i<5;i++)
		for(j=0;j<2;j++)
		  {
		    Kk[i][j]=0.0;for(m=0;m<2;m++) Kk[i][j]+=Bk[i][m]*Dk[m][j];
		  }
	      for(i=0;i<5;i++)
		Rk[i]+=Kk[i][0]*resid[0]+Kk[i][1]*resid[1];
	      for(i=0;i<5;i++)
		for(j=i;j<5;j++) 
		  {
		    Gk[i][j]-=Kk[i][0]*Bk[j][0]+Kk[i][1]*Bk[j][1];
		    Gk[j][i]=Gk[i][j];
		  }
	    }
	}
      else
	{
	  int LayId=pSP->layer();
	  arrAmbiIdx[LayId]++;
	  arrAmbiLayers[LayId].push_back(pSP);
	}
    }
#ifdef IDSCAN_DEBUG        
  printf("Estimates before amb.resolving : d0=%f z0=%f phi0=%f theta=%f pT=%f\n",
	 Rk[0],Rk[1],Rk[2],theta,1.0/Rk[4]);
  printf("Chi2 = %f Total %d ambiguities\n",
	 chi2,nAmbiLayers);
#endif

  for(i=0;i<20;i++) nAmbiLayers+=arrAmbiIdx[i];
  if(nAmbiLayers!=0)
    {
      for(int layId=19;layId>=0;layId--) 
	{
	  if(arrAmbiIdx[layId]==0) continue;
	  pBestSP=NULL;
	  bestChi2=maxChi2;
#ifdef IDSCAN_DEBUG
	  printf("Analyzing layer %d ...\n",layId);
#endif
	  for(std::vector<IdScanSpPoint *>::iterator hitItr=arrAmbiLayers[layId].begin();
	      hitItr!=arrAmbiLayers[layId].end();++hitItr)
	    {
	      IdScanSpPoint* pSP=(*hitItr);
	      x=pSP->rho()*cos(pSP->phi()-averagePhi0);
	      y=pSP->rho()*sin(pSP->phi()-averagePhi0);
	      Hk[0][0]=1.0;Hk[0][1]=0.0;Hk[0][2]=x;
	      Hk[0][3]=0.0;Hk[0][4]=-0.5*x*x*fConst;
	      Hk[1][0]=0.0;Hk[1][1]=1.0;
	      Hk[1][2]=0.0;Hk[1][3]=x;Hk[1][4]=0.0;
	      for(i=0;i<5;i++)
		for(j=0;j<2;j++) 
		  {
		    Bk[i][j]=0.0;for(m=0;m<5;m++) Bk[i][j]+=Gk[i][m]*Hk[j][m];
		  }
	      for(i=0;i<2;i++)
		for(j=0;j<2;j++)
		  {
		    Vk[i][j]=0.0;for(m=0;m<5;m++) Vk[i][j]+=Hk[i][m]*Bk[m][j];
		  }
	      if(pSP->layer()<7)
		{
		  sigma[0][0]=pSP->rho()*pSP->dphi();
		  sigma[1][1]=pSP->dz();
		  for(i=0;i<2;i++)
		    Vk[i][i]+=sigma[i][i]*sigma[i][i];
		}
	      else
		{
		  sigma[1][1]=pSP->dr()*Rk[3]/cos(Rk[2]);
		  sigma[1][1]=sigma[1][1]*sigma[1][1];
		  double a=Rk[2]-x*fConst*Rk[4]-sin(pSP->phi()-averagePhi0);
		  sigma[0][0]=x*x*pSP->dphi()*pSP->dphi()+
		    a*a*pSP->dr()*pSP->dr();
		  sigma[0][1]=sigma[1][0]=a*Rk[3]*pSP->dr()*pSP->dr();
		  for(i=0;i<2;i++)
		    for(j=0;j<2;j++)
		      Vk[i][j]+=sigma[i][j];
		}

	      detr=Vk[0][0]*Vk[1][1]-Vk[0][1]*Vk[1][0];detr=1.0/detr;
	      Dk[0][0]=Vk[1][1]*detr;Dk[0][1]=Dk[1][0]=-Vk[0][1]*detr;
	      Dk[1][1]=Vk[0][0]*detr;
	      for(i=0;i<2;i++)
		{
		  resid[i]=0.0;for(j=0;j<5;j++) resid[i]+=Hk[i][j]*Rk[j];
		}
	      resid[0]=y-resid[0];
	      resid[1]=pSP->z()-resid[1];
	      dchi2=resid[0]*resid[0]*Dk[0][0]+2.0*Dk[0][1]*resid[0]*resid[1]+
		resid[1]*resid[1]*Dk[1][1];
#ifdef IDSCAN_DEBUG
	      printf("          dchi2=%f\n",dchi2);
#endif
	      if(dchi2<bestChi2)
		{
		  bestChi2=dchi2;
		  pBestSP=pSP;
		}
	    }
	  if(pBestSP!=NULL)
	    {
#ifdef IDSCAN_DEBUG
	      printf("Best spacepoint with dchi2=%f\n",bestChi2);
#endif
	      IdScanSpPoint* pSP=pBestSP;
	      pSP->setUsed(1);
	      x=pSP->rho()*cos(pSP->phi()-averagePhi0);
	      y=pSP->rho()*sin(pSP->phi()-averagePhi0);
	      Hk[0][0]=1.0;Hk[0][1]=0.0;Hk[0][2]=x;
	      Hk[0][3]=0.0;Hk[0][4]=-0.5*x*x*fConst;
	      Hk[1][0]=0.0;Hk[1][1]=1.0;
	      Hk[1][2]=0.0;Hk[1][3]=x;Hk[1][4]=0.0;
	      for(i=0;i<5;i++)
		for(j=0;j<2;j++) 
		  {
		    Bk[i][j]=0.0;for(m=0;m<5;m++) Bk[i][j]+=Gk[i][m]*Hk[j][m];
		  }
	      for(i=0;i<2;i++)
		for(j=0;j<2;j++)
		  {
		    Vk[i][j]=0.0;for(m=0;m<5;m++) Vk[i][j]+=Hk[i][m]*Bk[m][j];
		  }
	      if(pSP->layer()<7)
		{
		  sigma[0][0]=x*pSP->dphi();
		  sigma[1][1]=pSP->dz();
		  for(i=0;i<2;i++)
		    Vk[i][i]+=sigma[i][i]*sigma[i][i];
		}
	      else
		{
		  sigma[1][1]=pSP->dr()*Rk[3]/cos(Rk[2]);
		  sigma[1][1]=sigma[1][1]*sigma[1][1];
		  double a=Rk[2]-x*fConst*Rk[4]-sin(pSP->phi()-averagePhi0);
		  sigma[0][0]=x*x*pSP->dphi()*pSP->dphi()+
		    a*a*pSP->dr()*pSP->dr();
		  sigma[0][1]=sigma[1][0]=a*Rk[3]*pSP->dr()*pSP->dr();
		  for(i=0;i<2;i++)
		    for(j=0;j<2;j++)
		      Vk[i][j]+=sigma[i][j];
		}

	      detr=Vk[0][0]*Vk[1][1]-Vk[0][1]*Vk[1][0];detr=1.0/detr;
	      Dk[0][0]=Vk[1][1]*detr;Dk[0][1]=Dk[1][0]=-Vk[0][1]*detr;
	      Dk[1][1]=Vk[0][0]*detr;
	      
	      for(i=0;i<2;i++)
		{
		  resid[i]=0.0;for(j=0;j<5;j++) resid[i]+=Hk[i][j]*Rk[j];
		}
	      resid[0]=y-resid[0];
	      resid[1]=pSP->z()-resid[1];
	      dchi2=resid[0]*resid[0]*Dk[0][0]+2.0*Dk[0][1]*resid[0]*resid[1]+
		resid[1]*resid[1]*Dk[1][1];
	      //  printf("r1=%f r2=%f dchi2=%f\n",resid[0],resid[1],dchi2);
	      chi2+=dchi2;
	      for(i=0;i<5;i++)
		for(j=0;j<2;j++)
		  {
		    Kk[i][j]=0.0;for(m=0;m<2;m++) Kk[i][j]+=Bk[i][m]*Dk[m][j];
		  }
	      for(i=0;i<5;i++)
		Rk[i]+=Kk[i][0]*resid[0]+Kk[i][1]*resid[1];
	      for(i=0;i<5;i++)
		for(j=i;j<5;j++) 
		  {
		    Gk[i][j]-=Kk[i][0]*Bk[j][0]+Kk[i][1]*Bk[j][1];
		    Gk[j][i]=Gk[i][j];
		  }
	    }
#ifdef IDSCAN_DEBUG
	  else 
	    printf("All SPs are rejected\n");
#endif
	}
    }
  theta=atan(1.0/Rk[3]);
  if(theta<0.0) theta+=M_PI;
  Rk[2]+=averagePhi0;
#ifdef IDSCAN_DEBUG        
  printf("Final estimates: d0=%f z0=%f phi0=%f theta=%f pT=%f\n",
	 Rk[0],Rk[1],Rk[2],theta,1.0/Rk[4]);
  printf("Chi2 = %f Total %d ambiguities\n",
	 chi2,nAmbiLayers);
#endif
  if((chi2>maxChi2inGroup)||(std::fabs(1.0/Rk[4])<minPt))
    {
      G.groupHits().clear();
#ifdef IDSCAN_DEBUG        
      printf("Large Chi2 or very low pT - the group is rejected\n");
#endif
    }
  G.setPtInv(Rk[4]);
  G.setChi2(chi2);
  G.setPhi0(Rk[2]);
  G.setz0(Rk[1]);
  G.setEta(-log(sin(0.5*theta)/cos(0.5*theta)));

  for(i=0;i<20;i++)
    {
      if(arrAmbiIdx[i]!=0) 
	{
	  arrAmbiLayers[i].clear();
	}
    }
}
void iHitFilter::fitGroupWithoutHitRemoval(Group& G)
{
  const double C=0.029997;
  const double maxChi2=500.0;
  const double maxChi2inGroup=6000.0;
  const double minPt=600.0;

  double Rk[5],Gk[5][5],Hk[2][5],Kk[5][2],resid[2],
    Bk[5][2],Vk[2][2],sigma[2][2],Dk[2][2];
  double chi2,dchi2,Bz,fConst,x,y,z,detr,averagePhi0=0.0,theta,
    phiErr,rhoErr,zErr;
  int i,j,m,nAmbiLayers=0,nHits=0;

  HIT_VECTOR arrAmbiLayers[20];
  HIT_VECTOR spVec;
  int arrAmbiIdx[20];

  spVec.clear();

  theta=2.0*atan(exp(-G.getEta()));

  Rk[0]=0.0;
  //  Rk[1]=0.0;
  Rk[1]=G.getz0();
  Rk[2]=0.0;
  Rk[3]=cos(theta)/sin(theta);
  Rk[4]=G.getPtInv();

  memset(&Gk[0][0],0,sizeof(Gk));
  Gk[0][0]=100.0;
  Gk[1][1]=100.0;
  Gk[2][2]=100.0;
  Gk[3][3]=100.0;
  Gk[4][4]=0.01;
  memset(&arrAmbiIdx[0],0,sizeof(arrAmbiIdx));

  chi2=0.0;Bz=20.84;fConst=C*Bz;

  double minPhi=(*G.groupHits().begin())->phi();

  for (IdScanSpPoint* pSP : G.groupHits())
    {
      if(minPhi>pSP->phi()) minPhi=pSP->phi();
      spVec.push_back(pSP);
    }
  std::sort(spVec.begin(),spVec.end(),CompareSpByLayer);

  for (IdScanSpPoint* pSP : spVec)
    {
      if(pSP->used()==0)
	pSP->setUsed(1);
      nHits++;
      double delta=pSP->phi()-minPhi;
      if(std::fabs(delta)>M_PI) delta-=2.0*M_PI;
      averagePhi0+=delta;
    }
  averagePhi0/=nHits;averagePhi0+=minPhi;

#ifdef IDSCAN_DEBUG
  printf("Average PHI0=%f Initial Pt=%f\n",averagePhi0,1.0/G.getPtInv());
#endif

  for(std::vector<IdScanSpPoint *>::reverse_iterator hitItr=spVec.rbegin();hitItr!=spVec.rend();++hitItr) 
    {
      IdScanSpPoint* pSP=(*hitItr);
      int LayId=pSP->layer();
      arrAmbiIdx[LayId]++;
      arrAmbiLayers[LayId].push_back(pSP);
    }
  for(int LayId=0;LayId<20;LayId++)
    {
      if(arrAmbiIdx[LayId]==0) continue;

      if(arrAmbiIdx[LayId]==1)
	{
	  IdScanSpPoint* pSP=(*arrAmbiLayers[LayId].begin());
	  x=pSP->rho()*cos(pSP->phi()-averagePhi0);
	  y=pSP->rho()*sin(pSP->phi()-averagePhi0);
	  z=pSP->z();
	  phiErr=pSP->dphi();
	  rhoErr=pSP->dr();
	  zErr=pSP->dz();
#ifdef IDSCAN_DEBUG
	  printf("x=%f y=%f z=%f of a spacepoint\n",x,y,z);
#endif
	}
      else
	{
	  x=0.0;y=0.0;z=0.0;
	  int nHitLayer=0;
	  for(HIT_VECTOR::iterator hitItr=arrAmbiLayers[LayId].begin();hitItr!=arrAmbiLayers[LayId].end();++hitItr)
	    {
	      IdScanSpPoint* pSP=(*hitItr);
	      nHitLayer++;
	      x+=pSP->rho()*cos(pSP->phi()-averagePhi0);
	      y+=pSP->rho()*sin(pSP->phi()-averagePhi0);
	      z+=pSP->z();nAmbiLayers++;
	    }
	  x=x/nHitLayer;y=y/nHitLayer;z=z/nHitLayer;
#ifdef IDSCAN_DEBUG
	  printf("x=%f y=%f z=%f of spacepoint combination\n",x,y,z);
#endif
	  IdScanSpPoint* pSP=(*arrAmbiLayers[LayId].begin());
	  phiErr=10.0*pSP->dphi();
	  rhoErr=10.0*pSP->dr();
	  zErr=10.0*pSP->dz();
	}
      Hk[0][0]=1.0;Hk[0][1]=0.0;Hk[0][2]=x;
      Hk[0][3]=0.0;Hk[0][4]=-0.5*x*x*fConst;
      Hk[1][0]=0.0;Hk[1][1]=1.0;
      Hk[1][2]=0.0;Hk[1][3]=x;Hk[1][4]=0.0;
      for(i=0;i<5;i++)
	for(j=0;j<2;j++) 
	  {
	    Bk[i][j]=0.0;for(m=0;m<5;m++) Bk[i][j]+=Gk[i][m]*Hk[j][m];
	  }
      for(i=0;i<2;i++)
	for(j=0;j<2;j++)
	  {
	    Vk[i][j]=0.0;for(m=0;m<5;m++) Vk[i][j]+=Hk[i][m]*Bk[m][j];
	  }
      if(LayId<7)
	{
	  sigma[0][0]=x*phiErr;
	  sigma[1][1]=zErr;
	  for(i=0;i<2;i++)
	    Vk[i][i]+=sigma[i][i]*sigma[i][i];
	}
      else
	{
	  sigma[1][1]=rhoErr*Rk[3]/cos(Rk[2]);
	  sigma[1][1]=sigma[1][1]*sigma[1][1];
	  double phii = atan2(y,x);
	  double a=Rk[2]-x*fConst*Rk[4]-sin(phii);
	  sigma[0][0]=x*x*phiErr*phiErr+a*a*rhoErr*rhoErr;
	  sigma[0][1]=sigma[1][0]=a*Rk[3]*rhoErr*rhoErr;
	  for(i=0;i<2;i++)
	    for(j=0;j<2;j++)
	      Vk[i][j]+=sigma[i][j];
	    }

	  detr=Vk[0][0]*Vk[1][1]-Vk[0][1]*Vk[1][0];detr=1.0/detr;
	  Dk[0][0]=Vk[1][1]*detr;Dk[0][1]=Dk[1][0]=-Vk[0][1]*detr;
	  Dk[1][1]=Vk[0][0]*detr;
	  
	  for(i=0;i<2;i++)
	    {
	      resid[i]=0.0;for(j=0;j<5;j++) resid[i]+=Hk[i][j]*Rk[j];
	    }
	  resid[0]=y-resid[0];
	  resid[1]=z-resid[1];
	  dchi2=resid[0]*resid[0]*Dk[0][0]+2.0*Dk[0][1]*resid[0]*resid[1]+
	    resid[1]*resid[1]*Dk[1][1];
	  //	  printf("r1=%f r2=%f dchi2=%f\n",resid[0],resid[1],dchi2);
	  //printf("dchi2phi=%f dchi2z=%f\n",
	  //	 resid[0]*resid[0]*Dk[0][0],resid[1]*resid[1]*Dk[1][1]);
	  if(dchi2<maxChi2)
	    {
	      chi2+=dchi2;
	      for(i=0;i<5;i++)
		for(j=0;j<2;j++)
		  {
		    Kk[i][j]=0.0;for(m=0;m<2;m++) Kk[i][j]+=Bk[i][m]*Dk[m][j];
		  }
	      for(i=0;i<5;i++)
		Rk[i]+=Kk[i][0]*resid[0]+Kk[i][1]*resid[1];
	      for(i=0;i<5;i++)
		for(j=i;j<5;j++) 
		  {
		    Gk[i][j]-=Kk[i][0]*Bk[j][0]+Kk[i][1]*Bk[j][1];
		    Gk[j][i]=Gk[i][j];
		  }
	    }
    }
  theta=atan(1.0/Rk[3]);
  if(theta<0.0) theta+=M_PI;
  Rk[2]+=averagePhi0;
#ifdef IDSCAN_DEBUG        
  printf("Final estimates: d0=%f z0=%f phi0=%f theta=%f pT=%f\n",
	 Rk[0],Rk[1],Rk[2],theta,1.0/Rk[4]);
  printf("Chi2 = %f Total %d ambiguities\n",
	 chi2,nAmbiLayers);
#endif
  if((chi2>maxChi2inGroup)||(std::fabs(1.0/Rk[4])<minPt))
    {
      G.groupHits().clear();
#ifdef IDSCAN_DEBUG        
      printf("Large Chi2 or very low pT - the group is rejected\n");
#endif
    }
  G.setPtInv(Rk[4]);
  G.setChi2(chi2);
  G.setPhi0(Rk[2]);
  G.setz0(Rk[1]);
  G.setEta(-log(sin(0.5*theta)/cos(0.5*theta)));

  for(i=0;i<20;i++)
    {
      if(arrAmbiIdx[i]!=0) 
	{
	  arrAmbiLayers[i].clear();
	}
    }
}
