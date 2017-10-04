/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"
#include "TrigInDetPattRecoEvent/TrigInDetSiLayer.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RoiUtil.h"

TrigTrackSeedGenerator::TrigTrackSeedGenerator(const TrigCombinatorialSettings& tcs) 
  : m_settings(tcs), 
    m_maxSoaSize(1000),
    m_maxOuterRadius(550.0), 
    //  m_minRadius(10.0),
    // m_maxRadius(600.0), 
    m_minDeltaRadius(10.0), 
    m_zTol(3.0), 
    m_pStore(NULL)
{
  m_maxDeltaRadius = m_settings.m_doublet_dR_Max;
  //m_radBinWidth = m_settings.m_seedRadBinWidth;
  m_phiSliceWidth = 2*M_PI/m_settings.m_nMaxPhiSlice;
  //m_nMaxRadBin = 1+(int)((m_maxRadius-m_minRadius)/m_radBinWidth);
  m_pStore = new L_PHI_STORAGE(m_settings.m_nMaxPhiSlice, (int)m_settings.m_layerGeometry.size());

  //mult scatt. variance for doublet matching
  const double radLen = 0.036;
  m_CovMS = std::pow((13.6/m_settings.m_tripletPtMin),2)*radLen;
  const double ptCoeff = 0.29997*1.9972/2.0;// ~0.3*B/2 - assumes nominal field of 2*T
  m_minR_squ = m_settings.m_tripletPtMin*m_settings.m_tripletPtMin/std::pow(ptCoeff,2);
  m_dtPreCut = std::tan(2.0*m_settings.m_tripletDtCut*sqrt(m_CovMS));
}

TrigTrackSeedGenerator::~TrigTrackSeedGenerator() {
  for(INTERNAL_TRIPLET_BUFFER::iterator it=m_triplets.begin();it!=m_triplets.end();++it) {
    delete (*it).second;
  }
  m_triplets.clear();
  delete m_pStore;

  m_SoA.clear();

}

void TrigTrackSeedGenerator::loadSpacePoints(const std::vector<TrigSiSpacePointBase>& vSP) {

  m_pStore->reset();

  m_SoA.clear();

  for(std::vector<TrigSiSpacePointBase>::const_iterator it = vSP.begin();it != vSP.end();++it) {
    //  if((*it).r()>m_maxRadius || (*it).r() < m_minRadius) continue;
    // int rIdx = ((*it).r()-m_minRadius)/m_radBinWidth;
    int layerId = (*it).layer();
    int phiIdx = ((*it).phi()+M_PI)/m_phiSliceWidth;
    if (phiIdx >= m_settings.m_nMaxPhiSlice) {
      phiIdx %= m_settings.m_nMaxPhiSlice;
    }
    else if (phiIdx < 0) {
      phiIdx += m_settings.m_nMaxPhiSlice;
      phiIdx %= m_settings.m_nMaxPhiSlice;
    }
    m_pStore->addSpacePoint(phiIdx, layerId, &(*it));
  }

  m_pStore->sortSpacePoints2(m_settings.m_layerGeometry);

  //m_pStore->sortSpacePoints(m_settings.m_layerGeometry);

  m_SoA.resize(vSP.size());

  /*  
  int nSP=0;
  int nLayers = (int)m_settings.m_layerGeometry.size();
  int nPhi = m_settings.m_nMaxPhiSlice;

  for(int L = 0;L<nLayers;L++) {
    std::cout<<"Layer "<<L<<std::endl;
    bool isBarrel = m_settings.m_layerGeometry[L].m_type==0;
    const L_PHI_SECTOR& S = m_pStore->m_layers[L];
    for(int Phi=0;Phi<nPhi;Phi++) {
      const std::vector<const TrigSiSpacePointBase*>& spV = S.m_phiThreeSlices[Phi];
      //std::cout<<spV.size()<<" ";
      nSP += spV.size();
      for(auto sp : spV) {
	if(isBarrel) 
	  std::cout<<sp->z()<<" ";
	else std::cout<<sp->r()<<" ";
      }
      std::cout<<std::endl;
    }
    //std::cout<<std::endl;
  } 
  std::cout<<"Total nSP="<<nSP<<" vs "<<vSP.size()<<std::endl;
  */

}

void TrigTrackSeedGenerator::createSeeds() {

  m_zMinus = m_settings.roiDescriptor->zedMinus() - m_zTol;
  m_zPlus  = m_settings.roiDescriptor->zedPlus() + m_zTol;
  
  for(INTERNAL_TRIPLET_BUFFER::iterator it=m_triplets.begin();it!=m_triplets.end();++it) {
    delete (*it).second;
  }
  m_triplets.clear();

  int nLayers = (int) m_settings.m_layerGeometry.size();

  //int lastBarrel=0;
  //for(int l=0;l<nLayers;l++) {
  //  if(m_settings.m_layerGeometry[l].m_type==0) lastBarrel = l;
  //}
  
  
  //TO-DO: create vector of MIDDLE layers and for each middle layer create a list of 
  //ALLOWED layers for doublet making
  //for example coaxial barrel layers should be separated by more than DR_Max
  // no simultaneous +/- or -/+ endcaps

  m_innerMarkers.clear();
  m_outerMarkers.clear();
  
  for(int layerI=1;layerI<nLayers;layerI++) {//skip layer 0 for middle spacepoint search

    //    if(layerI == lastBarrel) continue;//skip the last barrel layer for middle spacepoint search

    const L_PHI_SECTOR& S = m_pStore->m_layers[layerI];
    if(S.m_nSP==0) continue;

    bool isSct = (m_settings.m_layerGeometry[layerI].m_subdet == 2);
    
    for(int phiI=0;phiI<m_settings.m_nMaxPhiSlice;phiI++) {

      /*      
	std::vector<int> phiVec;
	phiVec.reserve(4);

	phiVec.push_back(phiI);

	if(phiI>0) phiVec.push_back(phiI-1);
	else phiVec.push_back(m_settings.m_nMaxPhiSlice-1);

	if(phiI<m_settings.m_nMaxPhiSlice-1) phiVec.push_back(phiI+1);
	else phiVec.push_back(0);
    
	//Remove duplicates
	std::sort(phiVec.begin(), phiVec.end());
	phiVec.erase(std::unique(phiVec.begin(), phiVec.end()), phiVec.end());
      */
      for(auto spm : S.m_phiSlices[phiI]) {//loop over middle spacepoints

	float zm = spm->z();
	float rm = spm->r();

	//    const std::pair<IdentifierHash, IdentifierHash>& deIds = spm->offlineSpacePoint()->elementIdList();
	    
	int nSP=0;

	m_nInner = 0;
	m_nOuter = 0;

	m_innerMarkers.clear();
	m_outerMarkers.clear();

	for(int layerJ=0;layerJ<nLayers;layerJ++) {
	    
	  if(!validateLayerPairNew(layerI, layerJ, rm, zm)) continue; 
	  
	  bool isPixel2 = (m_settings.m_layerGeometry[layerJ].m_subdet == 1);
	    
	  bool checkPSS = (!m_settings.m_tripletDoPSS) && (isSct && isPixel2);

	  //for(auto phiJ : phiVec) {

	  //  const std::vector<const TrigSiSpacePointBase*>& spVec = m_pStore->m_layers[layerJ].m_phiSlices.at(phiJ);

	  const std::vector<const TrigSiSpacePointBase*>& spVec = m_pStore->m_layers[layerJ].m_phiThreeSlices.at(phiI);

	  if(spVec.empty()) continue;

	  SP_RANGE delta;

	  if(!getSpacepointRange(layerJ, spVec, delta)) continue;

	  int nI = m_nInner;
	  int nO = m_nOuter;
	      
	  nSP += processSpacepointRange(layerJ, rm, zm, checkPSS, delta);

	  if(m_nInner > nI) m_innerMarkers.push_back(m_nInner);
	  if(m_nOuter > nO) m_outerMarkers.push_back(m_nOuter);

	  //  }//loop over phi bins
	    
	    //if(m_nInner > nI) m_innerMarkers.push_back(m_nInner);
	    //if(m_nOuter > nO) m_outerMarkers.push_back(m_nOuter);

	}//loop over inner/outer layers

	//std::cout<<"middle sp : r="<<rm<<" phi="<<spm->phi()<<" z="<<zm<<" has "<<nOuter<<" outer neighbours and "<<nInner<<" inner neighbours"<<std::endl;
	
	if(m_nInner != 0 && m_nOuter != 0) {
	  INTERNAL_TRIPLET_BUFFER tripletVec;
	    
	  createTripletsNew(spm, m_nInner, m_nOuter, tripletVec);
	  //createTriplets(spm, m_nInner, m_nOuter, tripletVEc);
	    
	  if(!tripletVec.empty()) storeTriplets(tripletVec);	
	  tripletVec.clear();
	}
	else continue;
      }
    }
  }

  /*
  for(auto& tm : m_triplets) {
    std::cout<<"Q="<<tm.first<<" barcodes: "<<tm.second->s1().barCode()<<" "<<
      tm.second->s2().barCode()<<" "<<tm.second->s3().barCode()<<std::endl;
  }
  */

}

void TrigTrackSeedGenerator::createSeedsZv() {
  
  for(INTERNAL_TRIPLET_BUFFER::iterator it=m_triplets.begin();it!=m_triplets.end();++it) {
    delete (*it).second;
  }
  m_triplets.clear();

  if(m_settings.m_vZv.empty()) return;

  int nLayers = (int) m_settings.m_layerGeometry.size();

 
  for(int layerI=1;layerI<nLayers;layerI++) {//skip layer 0 for middle spacepoint search

    const L_PHI_SECTOR& S = m_pStore->m_layers[layerI];
    if(S.m_nSP==0) continue;

    bool isSct = (m_settings.m_layerGeometry[layerI].m_subdet == 2);
    
    for(int phiI=0;phiI<m_settings.m_nMaxPhiSlice;phiI++) {

	std::vector<int> phiVec;
	phiVec.reserve(4);

	phiVec.push_back(phiI);

	if(phiI>0) phiVec.push_back(phiI-1);
	else phiVec.push_back(m_settings.m_nMaxPhiSlice-1);

	if(phiI<m_settings.m_nMaxPhiSlice-1) phiVec.push_back(phiI+1);
	else phiVec.push_back(0);
    
	//Remove duplicates
	std::sort(phiVec.begin(), phiVec.end());
	phiVec.erase(std::unique(phiVec.begin(), phiVec.end()), phiVec.end());

	for(auto spm : S.m_phiSlices[phiI]) {//loop over middle spacepoints

	  float zm = spm->z();
	  float rm = spm->r();

	  INTERNAL_TRIPLET_BUFFER tripletVec;

	  for(auto zVertex : m_settings.m_vZv) {//loop over zvertices

	    int nSP=0;
	    m_nInner = 0;
	    m_nOuter = 0;

	    m_zMinus = zVertex - m_settings.m_zvError;
	    m_zPlus = zVertex + m_settings.m_zvError;

	    for(int layerJ=0;layerJ<nLayers;layerJ++) {//loop over other layers

	      if(!validateLayerPairNew(layerI, layerJ, rm, zm)) continue; 

	      bool isPixel2 = (m_settings.m_layerGeometry[layerJ].m_subdet == 1);
	      bool checkPSS = (!m_settings.m_tripletDoPSS) && (isSct && isPixel2);

	      for(auto phiJ : phiVec) {

		const std::vector<const TrigSiSpacePointBase*>& spVec = m_pStore->m_layers[layerJ].m_phiSlices.at(phiJ);

		if(spVec.empty()) continue;

		SP_RANGE delta;

		if(!getSpacepointRange(layerJ, spVec, delta)) continue;
	      
		nSP += processSpacepointRangeZv(rm, zm, checkPSS, delta);
	      }//loop over phi bins

	    }//loop over inner/outer layers
	    if(m_nInner != 0 && m_nOuter != 0) {
	      createTriplets(spm, m_nInner, m_nOuter, tripletVec);
	    }
	  }//loop over zvertices
	  if(!tripletVec.empty()) storeTriplets(tripletVec);
	  tripletVec.clear();
	}
    }
  }

  /*
  for(auto& tm : m_triplets) {
    std::cout<<"Q="<<tm.first<<" barcodes: "<<tm.second->s1().barCode()<<" "<<
      tm.second->s2().barCode()<<" "<<tm.second->s3().barCode()<<std::endl;
  }
  */

}

bool TrigTrackSeedGenerator::validateLayerPairNew(int layerI, int layerJ, float rm, float zm) {

  const float deltaRefCoord = 5.0;

  if(layerJ==layerI) return false;//skip the same layer ???

  if(m_pStore->m_layers[layerJ].m_nSP==0) return false;

  int typeI = m_settings.m_layerGeometry[layerI].m_type;

  float refCoordI = m_settings.m_layerGeometry[layerI].m_refCoord;

  int typeJ = m_settings.m_layerGeometry[layerJ].m_type;

  float refCoordJ = m_settings.m_layerGeometry[layerJ].m_refCoord;

  if((typeI!=0) && (typeJ!=0) && refCoordI*refCoordJ<0.0) return false;//only ++ or -- EC combinations are allowed

  //project beamline interval to the ref. coord of the layer

  bool isBarrel = (typeJ == 0);

  if(isBarrel && fabs(refCoordJ-rm)>m_maxDeltaRadius) return false;

  //boundaries for nextLayer

  m_minCoord = 10000.0;
  m_maxCoord =-10000.0;

  if(isBarrel) {

    if(refCoordJ<rm){//inner layer

      m_minCoord = m_zMinus + refCoordJ*(zm-m_zMinus)/rm;//+deltaRefCoord
      m_maxCoord = m_zPlus + refCoordJ*(zm-m_zPlus)/rm;//-deltaRefCoord
      m_minCoord -= deltaRefCoord*fabs(zm-m_zMinus)/rm;//corrections due to the layer width
      m_maxCoord += deltaRefCoord*fabs(zm-m_zPlus)/rm;
      //if(m_minCoord>m_maxCoord) {
      //std::cout<<"1 WRONG ORDER: m_minCoord="<<m_minCoord<<" m_maxCoord="<<m_maxCoord<<std::endl;
      //}
    }
    else {//outer layer

      m_minCoord = m_zPlus + refCoordJ*(zm-m_zPlus)/rm;//+deltaRefCoord
      m_maxCoord = m_zMinus + refCoordJ*(zm-m_zMinus)/rm;//-deltaRefCoord
      m_minCoord -= deltaRefCoord*fabs(zm-m_zPlus)/rm;
      m_maxCoord += deltaRefCoord*fabs(zm-m_zMinus)/rm;
      //if(m_minCoord>m_maxCoord) {
      //std::cout<<"2 WRONG ORDER: m_minCoord="<<m_minCoord<<" m_maxCoord="<<m_maxCoord<<std::endl;
      //}
    }
  }
  else {//endcap
    float maxB =m_settings.m_layerGeometry[layerJ].m_maxBound;
    float minB =m_settings.m_layerGeometry[layerJ].m_minBound;
    if(maxB<rm) return false;// This currently rejects SP type doublets
                              // Could correct this by retrieving if layers are pix or SCT, and not performing this check for SCt->pix doublets
    if(refCoordJ>0) {//positive EC
      //float zMax = (zm*maxB-rm*refCoordJ)/(maxB-rm);
      //float zMin = (zm*minB-rm*refCoordJ)/(minB-rm);
      //if(m_zPlus<zMin || m_zMinus > zMax) return false;
      if(refCoordJ > zm) {//outer layer
	
	if(zm <= m_zMinus) return false;
	
	float zMax = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	if( m_zMinus > zMax) return false;
	if (rm < minB) {
	  float zMin = (zm*minB-rm*refCoordJ)/(minB-rm);
	  if(m_zPlus<zMin) return false;
	}

	m_minCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_maxCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_minCoord -= deltaRefCoord*rm/fabs(zm-m_zMinus);
	m_maxCoord += deltaRefCoord*rm/fabs(zm-m_zPlus);

	if(zm <= m_zPlus) m_maxCoord = maxB;
	
	if(m_minCoord > maxB) return false;
	if(m_maxCoord < minB) return false;
	
	//if(m_minCoord>m_maxCoord) {
	//std::cout<<"31 WRONG ORDER: m_minC="<<m_minCoord<<" m_maxC="<<m_maxCoord<<" rm="<<rm<<" zm="<<zm<<" refC="<<refCoordJ<<" zminus="<<m_zMinus<<" zplus="<<m_zPlus<<std::endl;
	//}
      }
      else {//inner layer

	float zMax = (zm*minB-rm*refCoordJ)/(minB-rm);
	if( m_zMinus > zMax) return false;
	if (rm>maxB) {// otherwise, intersect of line from maxB through middle sp will be on the wrong side of the layer
	  float zMin = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	  if(m_zPlus<zMin) return false;
	}

	m_minCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_maxCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_minCoord -= deltaRefCoord*rm/fabs(zm-m_zPlus);
	m_maxCoord += deltaRefCoord*rm/fabs(zm-m_zMinus);
	//if(m_minCoord>m_maxCoord) {
	//std::cout<<"32 WRONG ORDER: m_minC="<<m_minCoord<<" m_maxC="<<m_maxCoord<<" rm="<<rm<<" zm="<<zm<<" refC="<<refCoordJ<<" zminus="<<m_zMinus<<" zplus="<<m_zPlus<<std::endl;
	//}
      }
    }
    else {//negative EC
      //float zMin = (zm*maxB-rm*refCoordJ)/(maxB-rm);
      //float zMax = (zm*minB-rm*refCoordJ)/(minB-rm);
      //if(m_zPlus<zMin || m_zMinus > zMax) return false;
      if(refCoordJ < zm) {//outer layer

	if(zm > m_zPlus) return false;
	
	float zMin = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	if( m_zPlus < zMin) return false;
	if (rm<minB) {// otherwise, intersect of line from minB through middle sp will be on the wrong side of the layer
	  float zMax = (zm*minB-rm*refCoordJ)/(minB-rm);
	  if(m_zMinus>zMax) return false;
	}

	m_minCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_maxCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_minCoord -= deltaRefCoord*rm/fabs(zm-m_zPlus);
	m_maxCoord += deltaRefCoord*rm/fabs(zm-m_zMinus);
	if(zm > m_zMinus) m_maxCoord = maxB;	
	if(m_minCoord > maxB) return false;
	if(m_maxCoord < minB) return false;
	
	//if(m_minCoord>m_maxCoord) {
	//std::cout<<"41 WRONG ORDER: m_minC="<<m_minCoord<<" m_maxC="<<m_maxCoord<<" rm="<<rm<<" zm="<<zm<<" refC="<<refCoordJ<<" zminus="<<m_zMinus<<" zplus="<<m_zPlus<<std::endl;
	//}
      }
      else {//inner layer

	float zMin = (zm*minB-rm*refCoordJ)/(minB-rm);
	if( m_zPlus < zMin) return false;
	if (rm>maxB) {// otherwise, intersect of line from maxB through middle sp will be on the wrong side of the layer
	  float zMax = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	  if(m_zMinus>zMax) return false;
	}

	m_minCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_maxCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_minCoord -= deltaRefCoord*rm/fabs(zm-m_zMinus);
	m_maxCoord += deltaRefCoord*rm/fabs(zm-m_zPlus);
	//if(m_minCoord>m_maxCoord) {
	//std::cout<<"42 WRONG ORDER: m_minC="<<m_minCoord<<" m_maxC="<<m_maxCoord<<" rm="<<rm<<" zm="<<zm<<" refC="<<refCoordJ<<" zminus="<<m_zMinus<<" zplus="<<m_zPlus<<std::endl;
	//}
      }
    }
  }
  //if(m_minCoord>m_maxCoord) {
  // std::cout<<"WRONG ORDER: m_minCoord="<<m_minCoord<<" m_maxCoord="<<m_maxCoord<<std::endl;
  // }
  //float tmp = m_maxCoord;m_maxCoord = m_minCoord;m_minCoord = tmp;
  //}
  float minBoundJ = m_settings.m_layerGeometry[layerJ].m_minBound;
  float maxBoundJ = m_settings.m_layerGeometry[layerJ].m_maxBound;
  if(maxBoundJ<m_minCoord || minBoundJ>m_maxCoord) return false;
  return true;
}



bool TrigTrackSeedGenerator::getSpacepointRange(int lJ, const std::vector<const TrigSiSpacePointBase*>& spVec, SP_RANGE& delta) {
  
  int typeJ = m_settings.m_layerGeometry[lJ].m_type;
  bool isBarrel = (typeJ == 0);
  bool isPositiveEC = m_settings.m_layerGeometry[lJ].m_refCoord > 0; 
  float minSpCoord = (isBarrel) ? (*spVec.begin())->z() : (*spVec.begin())->r();
  float maxSpCoord = (isBarrel) ? (*spVec.rbegin())->z() : (*spVec.rbegin())->r();

  if(!isBarrel && isPositiveEC) {//forward endcap SPs are sorted differently
    float tmp = minSpCoord;minSpCoord = maxSpCoord;maxSpCoord = tmp;
  }

  if(minSpCoord > m_maxCoord) return false;
  if(maxSpCoord < m_minCoord) return false;

  //identify the range of spacepoints in the layer

  std::vector<const TrigSiSpacePointBase*>::const_iterator it1 = spVec.end();
  std::vector<const TrigSiSpacePointBase*>::const_iterator it2 = spVec.end();
  
  if(isBarrel) {
    it1 = std::lower_bound(spVec.begin(), spVec.end(), m_minCoord, L_PHI_SECTOR::smallerThanZ());
    it2 = std::upper_bound(spVec.begin(), spVec.end(), m_maxCoord, L_PHI_SECTOR::greaterThanZ());
  }
  else {
    if(isPositiveEC) {
      it1 = std::lower_bound(spVec.begin(), spVec.end(), m_maxCoord, L_PHI_SECTOR::greaterThanR_i());
      it2 = std::upper_bound(spVec.begin(), spVec.end(), m_minCoord, L_PHI_SECTOR::smallerThanR_i());
    } 
    else {
      it1 = std::lower_bound(spVec.begin(), spVec.end(), m_minCoord, L_PHI_SECTOR::smallerThanR());
      it2 = std::upper_bound(spVec.begin(), spVec.end(), m_maxCoord, L_PHI_SECTOR::greaterThanR());
    }
  }
  
  if(std::distance(it1, it2)==0) return false;

  delta.first = it1;delta.second = it2;
  return true;
}

int TrigTrackSeedGenerator::processSpacepointRange(int lJ, float rm, float zm, bool checkPSS, const SP_RANGE& delta) {

  int nSP=0;

  bool isBarrel = (m_settings.m_layerGeometry[lJ].m_type==0);
  float refCoord = m_settings.m_layerGeometry[lJ].m_refCoord;

  float dZ = refCoord-zm;
  float dR_i = isBarrel ? 1.0/(refCoord-rm) : 1.0;

  for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt=delta.first; spIt!=delta.second; ++spIt) {
    // if(deIds == (*spIt)->offlineSpacePoint()->elementIdList()) continue;
    float zsp = (*spIt)->z();
    float rsp = (*spIt)->r();
    
    float dr = rsp - rm;
    
    if(fabs(dr)>m_maxDeltaRadius ) continue;
    if(fabs(dr)<m_minDeltaRadius ) continue;
    
    //inner doublet check
    
    if (dr<0 && checkPSS) continue;//Allow PSS seeds ?  
    
    float dz = zsp - zm;
    float tau = dz/dr;
    if (fabs(tau) > 7.41) continue;
    
    
    float z0  = zm - rm*tau;
    if (m_settings.m_doubletFilterRZ) {
      if (!RoiUtil::contains( *(m_settings.roiDescriptor), z0, tau)) continue;
    }
    
    float t = isBarrel ? dz*dR_i : dZ/dr;
    
    if(dr<0) {
      m_SoA.m_spi[m_nInner] = *spIt;
      m_SoA.m_ti[m_nInner] = t;
      m_nInner++;
    }
    else {
      m_SoA.m_spo[m_nOuter] = *spIt;
      m_SoA.m_to[m_nOuter] = t;
      m_nOuter++;
    }
    nSP++;  
  }

  return nSP;
}

int TrigTrackSeedGenerator::processSpacepointRangeZv(float rm, float zm, bool checkPSS, const SP_RANGE& delta) {

  int nSP=0;	    
  for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt=delta.first; spIt!=delta.second; ++spIt) {

    float rsp = (*spIt)->r(); 
    float zsp = (*spIt)->z();  
    float dr = rsp - rm;
	      
    if(fabs(dr)>m_maxDeltaRadius ) continue;
    if(fabs(dr)<m_minDeltaRadius ) continue;
		
    //inner doublet check

    if (dr<0 && checkPSS) continue;//Allow PSS seeds ? 
 
    float dz = zsp - zm;
    float tau = dz/dr;
    if (fabs(tau) > 7.41) continue;//|eta|<2.7

    if(dr<0) {
      m_SoA.m_spi[m_nInner++] = *spIt;
    }
    else {
      m_SoA.m_spo[m_nOuter++] = *spIt;
    }
    nSP++;  
  }

  return nSP;
}

void TrigTrackSeedGenerator::createTriplets(const TrigSiSpacePointBase* pS, int nInner, int nOuter,
					    INTERNAL_TRIPLET_BUFFER& output) {


  if(nInner==0 || nOuter==0) return;

  int nSP = nInner + nOuter;

  const double pS_r = pS->r();
  const double pS_x = pS->x();
  const double pS_y = pS->y();
  //  const double pS_z = pS->z();
  const double pS_dr = pS->dr();
  const double pS_dz = pS->dz();
  const double cosA = pS_x/pS_r;
  const double sinA = pS_y/pS_r;
  const double covZ = pS_dz*pS_dz;
  const double covR = pS_dr*pS_dr;
  const bool isPixel = pS->isPixel();

  //m_SoA.resizeComponents();//Resize m_r, mu etc to m_sp size
  int idx = 0;
  for(;idx<nInner;idx++) {
    const TrigSiSpacePointBase* pSP = m_SoA.m_spi[idx];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = pSP->z() - pS->z(); 
    const double t = Rinv*dz;

    //2. Conformal mapping

    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  for(int k=0;k<nOuter;k++,idx++) {
    const TrigSiSpacePointBase* pSP = m_SoA.m_spo[k];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = -pSP->z() + pS->z(); 
    const double t = Rinv*dz;

    //2. Conformal mapping
    
    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  //double loop

  for(int innIdx=0;innIdx<nInner;innIdx++) {

    //mult. scatt contribution due to the layer with middle SP
    
    const double r_inn = m_SoA.m_r[innIdx];
    const double t_inn = m_SoA.m_t[innIdx];
    const double v_inn = m_SoA.m_v[innIdx];
    const double u_inn = m_SoA.m_u[innIdx];
    const double tCov_inn = m_SoA.m_tCov[innIdx];
    const double dCov = m_CovMS*(1+t_inn*t_inn);

    //double z0 = pS->z() - m_SoA.m_t[innIdx]*pS->r();

    for(int outIdx=nInner;outIdx<nSP;outIdx++) {
      /*
      std::cout<<"Tr: "<<m_SoA.m_sp[innIdx]->offlineSpacePoint()->r()<<" "<<m_SoA.m_sp[innIdx]->offlineSpacePoint()->phi()
	       <<" "<<m_SoA.m_sp[innIdx]->offlineSpacePoint()->globalPosition().z()<<" | ";
      std::cout<<pS->offlineSpacePoint()->r()<<" "<<pS->offlineSpacePoint()->phi()<<" "<<pS->offlineSpacePoint()->globalPosition().z()<<" | ";
      std::cout<<m_SoA.m_sp[outIdx]->offlineSpacePoint()->r()<<" "<<m_SoA.m_sp[outIdx]->offlineSpacePoint()->phi()<<" "
	       <<m_SoA.m_sp[outIdx]->offlineSpacePoint()->globalPosition().z()<<std::endl;
      */

      //1. rz doublet matching
      const double t_out = m_SoA.m_t[outIdx];



      const double dt2 = std::pow((t_inn - t_out), 2)/9.0;

      //const double covdt = tCov_inn + m_SoA.m_tCov[outIdx]
      //                   + 2*r_inn*m_SoA.m_r[outIdx]*(t_inn*t_out*covR + covZ);

      double covdt = (t_inn*t_out*covR + covZ);
      covdt       *= 2*r_inn*m_SoA.m_r[outIdx];
      covdt       += tCov_inn + m_SoA.m_tCov[outIdx];

      //                   + 2*r_inn*m_SoA.m_r[outIdx]*(t_inn*t_out*covR + covZ);
      //const double covdt_old = tCov_inn + m_SoA.m_tCov[outIdx]
      //                   + 2*r_inn*m_SoA.m_r[outIdx]*(t_inn*t_out*covR + covZ);
      //std::cout << std::scientific << "covdt: " << covdt << std::endl;
      //std::cout << std::scientific << "covdt_old: " << covdt_old << std::endl;
      //std::cout << std::endl;
      //double covdt = m_SoA.m_tCov[innIdx] + m_SoA.m_tCov[outIdx];
      //covdt += 2*m_SoA.m_r[innIdx]*m_SoA.m_r[outIdx]*(m_SoA.m_t[innIdx]*m_SoA.m_t[outIdx]*covR + covZ);

      //const double dt2 = dt*dt/9.0;//i.e. 3-sigma cut

      if(dt2 > covdt+dCov) continue;

      //2. pT estimate

      const double du = m_SoA.m_u[outIdx] - u_inn;
      if(du==0.0) continue;
      const double A = (m_SoA.m_v[outIdx] - v_inn)/du;
      const double B = v_inn - A*u_inn;
      const double R_squ = (1 + A*A)/(B*B);

      if(R_squ < m_minR_squ) continue;

      //3. the 3-sigma cut with estimated pT

      const double frac = m_minR_squ/R_squ;
      if(dt2 > covdt+frac*dCov) continue;

      //4. d0 cut

      const double fabs_d0 = std::fabs(pS_r*(B*pS_r - A));

      if(fabs_d0 > m_settings.m_tripletD0Max) continue;
      
      if (m_SoA.m_spo[outIdx-nInner]->isSCT() && isPixel) {
        if(fabs_d0 > m_settings.m_tripletD0_PPS_Max) continue;
      }

      //5. phi0 cut

      if (!m_settings.roiDescriptor->isFullscan()) {
        const double uc = 2*B*pS_r - A;
        const double phi0 = atan2(sinA - uc*cosA, cosA + uc*sinA);

        // if(!m_settings.roiDescriptor->containsPhi(phi0)) {
        if ( !RoiUtil::containsPhi( *(m_settings.roiDescriptor), phi0 ) ) {
          continue;
        }
      }

      //6. add new triplet

      const double Q = fabs_d0*fabs_d0;
      if(output.size()>=m_settings.m_maxTripletBufferLength) {
        std::sort(output.begin(), output.end(), 
          [](const std::pair<float, TrigInDetTriplet*>& A, const std::pair<float, TrigInDetTriplet*>& B) {
            return A.first > B.first;
          }
        );

        INTERNAL_TRIPLET_BUFFER::iterator it = output.begin();
        if( Q >= (*it).first) continue;
        delete (*it).second;
        output.erase(it);
      }

      TrigInDetTriplet* t = new TrigInDetTriplet(*m_SoA.m_spi[innIdx], *pS, *m_SoA.m_spo[outIdx-nInner], Q);


      output.push_back(std::pair<double, TrigInDetTriplet*>(Q,t));
    }
  }
}

void TrigTrackSeedGenerator::createTripletsNew(const TrigSiSpacePointBase* pS, int nInner, int nOuter,
					       INTERNAL_TRIPLET_BUFFER& output) {

  if(nInner==0 || nOuter==0) return;

  int nSP = nInner + nOuter;
  output.reserve(m_settings.m_maxTripletBufferLength);
  /*    
  std::cout<<"++++ Doublet ordering test +++++"<<std::endl;
  std::cout<<"Middle SP r="<<pS->r()<<" z="<<pS->z()<<std::endl;
  std::cout<<"Inner doublets : "<<std::endl;

  int layerStart=0;

  for(unsigned int k=0;k<m_innerMarkers.size();k++) {
    int layerEnd=m_innerMarkers[k];
    std::cout<<"Layer: ";
    for(int idx=layerStart;idx<layerEnd;idx++) {
      std::cout<< m_SoA.m_ti[idx] <<" ";
    }
    layerStart = layerEnd;
    std::cout<<std::endl;
  }
  std::cout<<"Outer doublets : "<<std::endl;

  layerStart=0;

  for(unsigned int k=0;k<m_outerMarkers.size();k++) {
    int layerEnd=m_outerMarkers[k];
    std::cout<<"Layer: ";
    for(int idx=layerStart;idx<layerEnd;idx++) {
      std::cout<< m_SoA.m_to[idx] <<" ";
    }
    layerStart = layerEnd;
    std::cout<<std::endl;
  }
  std::cout<<"++++ Doublet ordering test +++++"<<std::endl;
      */

  int nL = m_outerMarkers.size() + m_innerMarkers.size();

  int nleft[64];
  int iter[64];
  int dirs[64];
  int type[64];
  double values[64];

  iter[0] = m_innerMarkers[0]-1;
  nleft[0] = m_innerMarkers[0];
  dirs[0] = -1;
  type[0] = 0;//inner sp
  int kL=1;
  for(unsigned int k=1;k<m_innerMarkers.size();k++) {
    iter[kL] = m_innerMarkers[k]-1;
    nleft[kL] = m_innerMarkers[k]-m_innerMarkers[k-1];
    dirs[kL] = -1;
    type[kL] = 0;
    kL++;
  }
  iter[kL] = 0;
  nleft[kL] = m_outerMarkers[0];
  dirs[kL] = 1;
  type[kL] = 1;//outer sp
  kL++;  
  for(unsigned int k=1;k<m_outerMarkers.size();k++) {
    iter[kL] = m_outerMarkers[k-1];
    nleft[kL] = m_outerMarkers[k] - m_outerMarkers[k-1];
    dirs[kL] = 1;
    type[kL] = 1;
    kL++;
  }
  kL--;
  for(int k=0;k<nL;k++) {
    values[k] = (type[k]==0) ? m_SoA.m_ti[iter[k]] : m_SoA.m_to[iter[k]];//initialization
  }

  /*    
  std::cout<<"Merged sort structures: "<<std::endl;
  int tsp = 0;
  for(int k=0;k<nL;k++) {
    std::cout<<"k="<<k<<" i="<<iter[k]<<" n="<<nleft[k]<<" d="<<dirs[k]<<" type"<<type[k]<<" v="<<values[k]<<std::endl;
    tsp += nleft[k];
  }
  std::cout<<"counter "<<tsp<<" nSP="<<nSP<<std::endl;
  */

  //merged sort

  int nActive=nL;
  int iSP=0;
  while(nActive!=0) {
    nActive = 0;
    //find min element
    double min_el=1000.0;
    int k_min=-1;
    for(int k=0;k<nL;k++) {
      if(nleft[k]==0) continue;
      nActive++;
      if(values[k]<min_el) {
        min_el = values[k];
        k_min = k;
      }
    }
    if(k_min==-1) break;
    //output

    int i_min = iter[k_min];

    if(type[k_min]==0){//inner 
      m_SoA.m_sorted_sp[iSP] = m_SoA.m_spi[i_min];
    } else {//outer 
      m_SoA.m_sorted_sp[iSP] = m_SoA.m_spo[i_min];
    }

    m_SoA.m_sorted_sp_type[iSP] = type[k_min];
    m_SoA.m_sorted_sp_t[iSP] = min_el;
    iSP++;

    //    std::cout<<min_el<<"/"<<type[k_min]<<" ";

    iter[k_min] += dirs[k_min];
    nleft[k_min]--;
    if(nleft[k_min]==0) {
      nActive--;
      continue;
    }
    values[k_min] = (type[k_min]==0) ? m_SoA.m_ti[iter[k_min]] : m_SoA.m_to[iter[k_min]];
  }
  // std::cout<<std::endl;

  // std::cout<<"iSP="<<iSP<<" nSP="<<nSP<<std::endl;

  /*  
  std::cout<<"++++ After merge-sort +++++"<<std::endl;

  for(int iter1 = 0;iter1<nSP;iter1++) {
    int type1 = sorted_sp_type[iter1];//0 - inner, 1 - outer
    double t1 = sorted_sp_t[iter1];
    std::cout<<type1<<" "<<t1<<std::endl;
  }
  */

  const double pS_r = pS->r();
  const double pS_x = pS->x();
  const double pS_y = pS->y();
  //const double pS_z = pS->z();
  const double pS_dr = pS->dr();
  const double pS_dz = pS->dz();
  const double cosA = pS_x/pS_r;
  const double sinA = pS_y/pS_r;
  const double covZ = pS_dz*pS_dz;
  const double covR = pS_dr*pS_dr;
  const bool isPixel = pS->isPixel();

  for(int idx=0;idx<nSP;idx++) {

    const TrigSiSpacePointBase* pSP = m_SoA.m_sorted_sp[idx];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = (m_SoA.m_sorted_sp_type[idx]==0) ? pSP->z() - pS->z() : -pSP->z() + pS->z();

    const double t = Rinv*dz;

    //2. Conformal mapping

    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  //double loop

  int iter1 = 0;

  while(iter1<nSP-1) {

    int type1 = m_SoA.m_sorted_sp_type[iter1];//0 - inner, 1 - outer
    double t1 = m_SoA.m_sorted_sp_t[iter1];
    double tcut = (1 + t1*t1)*m_dtPreCut;

    //mult. scatt contribution due to the layer with middle SP
    
    const double r_inn = m_SoA.m_r[iter1];
    const double t_inn = m_SoA.m_t[iter1];
    const double v_inn = m_SoA.m_v[iter1];
    const double u_inn = m_SoA.m_u[iter1];
    const double tCov_inn = m_SoA.m_tCov[iter1];
    const double dCov = m_CovMS*(1+t_inn*t_inn);

    for(int iter2=iter1+1;iter2<nSP;iter2++) {

      if(type1==m_SoA.m_sorted_sp_type[iter2]) continue;

      if(m_SoA.m_sorted_sp_t[iter2]-t1>tcut) break;

      const double t_out = m_SoA.m_t[iter2];

      const double dt2 = std::pow((t_inn - t_out), 2)*(1.0/9.0);

      double covdt = (t_inn*t_out*covR + covZ);
      covdt       *= 2*r_inn*m_SoA.m_r[iter2];
      covdt       += tCov_inn + m_SoA.m_tCov[iter2];

      if(dt2 > covdt+dCov) continue;

      //2. pT estimate

      const double du = m_SoA.m_u[iter2] - u_inn;
      if(du==0.0) continue;

      const double A = (m_SoA.m_v[iter2] - v_inn)/du;
      //Branchless version of (type1==0) ? v_inn - A*u_inn : m_SoA.m_v[iter2] - A*m_SoA.m_u[iter2];
      const double B = (1-type1)*(v_inn - A*u_inn) + type1*(m_SoA.m_v[iter2] - A*m_SoA.m_u[iter2]);
      const double R_squ = (1 + A*A)/(B*B);

      if(R_squ < m_minR_squ) continue;

      //3. the 3-sigma cut with estimated pT

      const double frac = m_minR_squ/R_squ;
      if(dt2 > covdt+frac*dCov) continue;

      //4. d0 cut

      const double d0_partial = B*pS_r - A;//Pre-calculate for use in phi check
      const double fabs_d0 = std::fabs(pS_r*(d0_partial));

      if(fabs_d0 > m_settings.m_tripletD0Max) continue;

      //Branchless version of (type1 == 1) ? m_SoA.m_sorted_sp[iter1]->isSCT() : m_SoA.m_sorted_sp[iter2]->isSCT();
      bool isSCT =  (1-type1)*m_SoA.m_sorted_sp[iter1]->isSCT() + type1*m_SoA.m_sorted_sp[iter2]->isSCT();
      
      if (isSCT && isPixel) {
        if(fabs_d0 > m_settings.m_tripletD0_PPS_Max) continue;
      }

      //5. phi0 cut

      if (!m_settings.roiDescriptor->isFullscan()) {

        const double uc = 2*d0_partial;
        const double phi0 = atan2(sinA - uc*cosA, cosA + uc*sinA);

        if ( !RoiUtil::containsPhi( *(m_settings.roiDescriptor), phi0 ) ) {
          continue;
        }
      }

      //6. add new triplet

      const double Q = fabs_d0*fabs_d0;
      if(output.size()>=m_settings.m_maxTripletBufferLength) {
        std::sort(output.begin(), output.end(), 
          [](const std::pair<float, TrigInDetTriplet*>& A, const std::pair<float, TrigInDetTriplet*>& B) {
            return A.first > B.first;
          }
        );
        //unsigned int count = 0;
        //std::cout << "Q: \t" << Q << std::endl;
        //for (auto entry : output) {
        //  std::cout << "BEFORE:\t" << count << "\t" << entry.first << std::endl;
        //  count++;
        //}

        INTERNAL_TRIPLET_BUFFER::iterator it = output.begin();
        if( Q >= (*it).first) {
          continue;
        }
        delete (*it).second;
        output.erase(it);
        //count = 0;
        //for (auto entry : output) {
        //  std::cout << "AFTER: \t" << count << "\t" << entry.first << std::endl;
        //  count++;
        //}
      }

      const TrigSiSpacePointBase* pSPI = (type1==0) ? m_SoA.m_sorted_sp[iter1] : m_SoA.m_sorted_sp[iter2];
      const TrigSiSpacePointBase* pSPO = (type1==0) ? m_SoA.m_sorted_sp[iter2] : m_SoA.m_sorted_sp[iter1];

      TrigInDetTriplet* t = new TrigInDetTriplet(*pSPI, *pS, *pSPO, Q);
      output.push_back(std::pair<double, TrigInDetTriplet*>(Q,t));
    }

    iter1++;
  }

}

void TrigTrackSeedGenerator::storeTriplets(INTERNAL_TRIPLET_BUFFER& tripletVec) {
  for(INTERNAL_TRIPLET_BUFFER::iterator it=tripletVec.begin();it!=tripletVec.end();++it) {
    double Q = (*it).first;
    if((*it).second->s3().isSCT()) {
      Q += (*it).second->s1().isSCT() ? 1000.0 : 10000.0;
    }
    m_triplets.push_back(std::pair<double, TrigInDetTriplet*>(Q, (*it).second));
  }
}

void TrigTrackSeedGenerator::getSeeds(std::vector<TrigInDetTriplet*>& vs) {
  vs.clear();
  vs.reserve(m_triplets.size());
  std::sort(m_triplets.begin(), m_triplets.end(), 
    [](const std::pair<float, TrigInDetTriplet*>& A, const std::pair<float, TrigInDetTriplet*>& B) {
      return A.first > B.first;
    }
  );
  for(INTERNAL_TRIPLET_BUFFER::reverse_iterator it=m_triplets.rbegin();it!=m_triplets.rend();++it) {
    vs.push_back((*it).second);
    (*it).second = NULL;//ownership transferred
  }
  m_triplets.clear();
}
