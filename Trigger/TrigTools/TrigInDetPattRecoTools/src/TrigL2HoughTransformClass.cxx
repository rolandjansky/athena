/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetPattRecoTools/TrigL2HoughTransformClass.h"


void TrigL2HoughTransformClass::clusterBins( std::list<HoughBinCluster>& clusterList, double th ) {

  //   Loop over all the bins in the map and cluster them into groups. As soon as
  //   bins are processed they get deleted with the fancy m_binMap->erase( binItr++ ) 
  //   so at the end of this loop m_binMap is empty and there is no need to .clear()!
  //

  m_layerThreshold=th;

  HoughAccumulatorType::iterator binItr( m_binMap->begin() ), mapEnd( m_binMap->end() );

  for ( ;  binItr != mapEnd; m_binMap->erase( binItr++ ) ) {
    if ( (binItr->second).NumberOfHits() > 0 ) {
      m_binCounter = 0;
      joinGroup( binItr->first, binItr->second, clusterList.end(), clusterList );
    }
  }
}

void TrigL2HoughTransformClass::joinGroup( long key, 
					   HoughBin2D& bin, 
					   std::list<HoughBinCluster>::iterator clustIter, 
					   std::list<HoughBinCluster>& clusterList ) {

//   1. Find the 8 neighbouring bins and exchange layer numbers
//
  long np[8], Nneigh( 0 );
  neighborKeys(key, np );

#ifdef IDSCAN_DEBUG
  std::cout << "IDSCAN_DEBUG: in join groups - bincounter/key = " << m_binCounter << " / " << key << std::endl;
#endif

  HoughAccumulatorType::iterator miter, mneigh[8], mEnd( m_binMap->end() );
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
  if ( bin.NumberOfLayers() > m_layerThreshold ) {

//   if the Clusters iterator is in the end, it means that this bin should start
//   the formation of a new cluster; if not, it means that it should join the cluster pointed
//   to by clustIter
// 
    if (clustIter == clusterList.end()){
      clusterList.push_back( HoughBinCluster() );
      clustIter-- = clusterList.end();
    }
    long binX = key/m_hashKeyCoeff;
    long binY = key%m_hashKeyCoeff;
    clustIter->AddSpacePoints( bin.spacePoints(), binX, binY ); //   pass all SPs from the bin to the group...
    
//   Now all the neighbours should follow the same procedure...
//
    for(long i=0; i<Nneigh; ++i){
      if ( (mneigh[i]->second).NumberOfHits() > 0 && m_binCounter<25){
	m_binCounter++;
	joinGroup( mneigh[i]->first, mneigh[i]->second, clustIter, clusterList );
      }
    }      
  }      
}

void TrigL2HoughTransformClass::neighborKeys( long key, long* np ) {

//   Finds the hash keys for the 8 neighbouring bins. Look at HitFilter::makeHashKey 
//   to understand the decoding...
//

  long binX = key/m_hashKeyCoeff;  // eta bin for hitFilter, phi0 bin for groupCleaner
  long binY = key%m_hashKeyCoeff;  // phi bin for hitFilter, dphi/drho bin for groupCleaner

  long binXm1 = binX - 1;
  long binXp1 = binX + 1;

  long binYm1 = binY - 1;
  long binYp1 = binY + 1;

  if ( m_checkYwraparound ) {
    if ( binYm1 < 0 ) binYm1 += m_maxYbin;
    if ( binYp1 > m_maxYbin-1) binYp1 -= m_maxYbin;
  }

  np[0] = m_hashKeyCoeff*binXm1 + binYm1;
  np[1] = m_hashKeyCoeff*binXm1 + binY;
  np[2] = m_hashKeyCoeff*binXm1 + binYp1;
  np[3] = m_hashKeyCoeff*binX +   binYm1;
  np[4] = m_hashKeyCoeff*binX +   binYp1;
  np[5] = m_hashKeyCoeff*binXp1 + binYm1;
  np[6] = m_hashKeyCoeff*binXp1 + binY;
  np[7] = m_hashKeyCoeff*binXp1 + binYp1;

  /*
  for (long i=-1; i<2; ++i) {
    for (long j=-1; j<2; ++j) {
      if ( i||j ) {
	*np++ = m_hashKeyCoeff*(bin1+i) + (bin2+j);
      }
    }
  }
  */

}
