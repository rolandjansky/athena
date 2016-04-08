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

  m_pStore->sortSpacePoints(m_settings.m_layerGeometry);
  m_SoA.resize(vSP.size());
  /*
  int nSP=0;
  int nLayers = (int)m_settings.m_layerGeometry.size();
  int nPhi = m_settings.m_nMaxPhiSlice;

  for(int L = 0;L<nLayers;L++) {
    const L_PHI_SECTOR& S = m_pStore->m_layers[L];
    for(int Phi=0;Phi<nPhi;Phi++) {
      const std::vector<const TrigSiSpacePointBase*>& spV = S.m_phiSlices[Phi];
      std::cout<<spV.size()<<" ";
      nSP += spV.size();
    }
    std::cout<<std::endl;
  } 
  std::cout<<"Total nSP="<<nSP<<" vs "<<vSP.size()<<std::endl;
  */

}

void TrigTrackSeedGenerator::createSeeds() {


  float zMinus = m_settings.roiDescriptor->zedMinus() - m_zTol;
  float zPlus  = m_settings.roiDescriptor->zedPlus() + m_zTol;
  
  for(INTERNAL_TRIPLET_BUFFER::reverse_iterator it=m_triplets.rbegin();it!=m_triplets.rend();++it) {
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

  for(int layerI=1;layerI<nLayers;layerI++) {//skip layer 0 for middle spacepoint search
    //    if(layerI == lastBarrel) continue;//skip the last barrel layer for middle spacepoint search

    const L_PHI_SECTOR& S = m_pStore->m_layers[layerI];
    if(S.m_nSP==0) continue;

    bool isSct = (m_settings.m_layerGeometry[layerI].m_subdet == 2);
    int typeI = m_settings.m_layerGeometry[layerI].m_type;
    float refCoordI = m_settings.m_layerGeometry[layerI].m_refCoord;

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

	  //    const std::pair<IdentifierHash, IdentifierHash>& deIds = spm->offlineSpacePoint()->elementIdList();
	    
	  int nSP=0;
	  int nInner = 0;
	  int nOuter = 0;

	  for(int layerJ=0;layerJ<nLayers;layerJ++) {

	    if(layerJ==layerI) continue;//skip the same layer ???
	    if(m_pStore->m_layers[layerJ].m_nSP==0) continue;

	    int typeJ = m_settings.m_layerGeometry[layerJ].m_type;
	    float refCoordJ = m_settings.m_layerGeometry[layerJ].m_refCoord;
	    float minBoundJ = m_settings.m_layerGeometry[layerJ].m_minBound;
	    float maxBoundJ = m_settings.m_layerGeometry[layerJ].m_maxBound;
	    bool isPixel2 = (m_settings.m_layerGeometry[layerJ].m_subdet == 1);

	    if((typeI!=0) && (typeJ!=0) && refCoordI*refCoordJ<0.0) continue;
	    
	    //project beamline interval to the ref. coord of the layer
	    
	    bool isBarrel = (typeJ == 0);
	  
	    if(isBarrel && fabs(refCoordJ-rm)>m_maxDeltaRadius) continue;

	    //boundaries for nextLayer

	    float minCoord = 10000.0;
	    float maxCoord =-10000.0;
	    
	    if(isBarrel) {
	      minCoord = zMinus + refCoordJ*(zm-zMinus)/rm;
	      maxCoord = zPlus + refCoordJ*(zm-zPlus)/rm;
	    }
	    else {
	      minCoord = rm*(refCoordJ-zMinus)/(zm-zMinus);
	      maxCoord = rm*(refCoordJ-zPlus)/(zm-zPlus);
	    }

	    if(minCoord>maxCoord) {
	      float tmp = maxCoord;maxCoord = minCoord;minCoord = tmp;
	    }
	    
	    if(maxBoundJ<minCoord || minBoundJ>maxCoord) continue;

	    for(auto phiJ : phiVec) {
	      const std::vector<const TrigSiSpacePointBase*>& spVec = m_pStore->m_layers[layerJ].m_phiSlices.at(phiJ);
	      if(spVec.empty()) continue;
	      float minSpCoord = (isBarrel) ? (*spVec.begin())->z() : (*spVec.begin())->r();

	      if(minSpCoord > maxCoord) continue;

	      float maxSpCoord = (isBarrel) ? (*spVec.rbegin())->z() : (*spVec.rbegin())->r();

	      if(maxSpCoord < minCoord) continue;

	      //identify the range of spacepoints in the layer

	      std::vector<const TrigSiSpacePointBase*>::const_iterator it1 = spVec.end();
	      std::vector<const TrigSiSpacePointBase*>::const_iterator it2 = spVec.end();

	      if(isBarrel) {
		it1 = std::lower_bound(spVec.begin(), spVec.end(), minCoord, L_PHI_SECTOR::smallerThanZ());
		it2 = std::upper_bound(spVec.begin(), spVec.end(), maxCoord, L_PHI_SECTOR::greaterThanZ());
	      }
	      else {
		it1 = std::lower_bound(spVec.begin(), spVec.end(), minCoord, L_PHI_SECTOR::smallerThanR());
		it2 = std::upper_bound(spVec.begin(), spVec.end(), maxCoord, L_PHI_SECTOR::greaterThanR());
	      }

	      if(std::distance(it1, it2)==0) continue;
	    
	      for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt= it1; spIt!=it2; ++spIt) {

		// if(deIds == (*spIt)->offlineSpacePoint()->elementIdList()) continue;
	      
		float zsp = (*spIt)->z();
		float rsp = (*spIt)->r();
	      
		float dr = rsp - rm;
	      
		if(fabs(dr)>m_maxDeltaRadius ) continue;
		if(fabs(dr)<m_minDeltaRadius ) continue;
		
		if(dr<0) {//inner doublet
		  if (!m_settings.m_tripletDoPSS) {//Allow PSS seeds? 
		    if(isSct && isPixel2) continue; 
		  }
		}
	      
		float dz = zsp - zm;
		float tau = dz/dr;
		if (fabs(tau) > 7.41) continue;
		
		float z0  = zm - rm*tau;
		if (m_settings.m_doubletFilterRZ) {
		  if (!RoiUtil::contains( *(m_settings.roiDescriptor), z0, tau)) continue;
		}
		if(dr<0) m_SoA.m_spi[nInner++] = *spIt;
		else m_SoA.m_spo[nOuter++] = *spIt;
		nSP++;
	      }
	    }
	  }	  

	  //std::cout<<"middle sp : r="<<rm<<" phi="<<spm->phi()<<" z="<<zm<<" has "<<nOuter<<" outer neighbours and "<<nInner<<" inner neighbours"<<std::endl;
	
	  if(nInner != 0 && nOuter != 0) {
	    INTERNAL_TRIPLET_BUFFER tripletMap;
	    
	    createTriplets(spm, nInner, nOuter, tripletMap);
	    
	    if(!tripletMap.empty()) storeTriplets(tripletMap);	
	    tripletMap.clear();
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


void TrigTrackSeedGenerator::createTriplets(const TrigSiSpacePointBase* pS, int nInner, int nOuter,
					    INTERNAL_TRIPLET_BUFFER& output) {

  

  output.clear();
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
        INTERNAL_TRIPLET_BUFFER::iterator it = output.begin();
        if( Q >= (*it).first) continue;
        delete (*it).second;
        output.erase(it);
      }

      TrigInDetTriplet* t = new TrigInDetTriplet(*m_SoA.m_spi[innIdx], *pS, *m_SoA.m_spo[outIdx-nInner], Q);


      output.insert(std::pair<double, TrigInDetTriplet*>(Q,t));
    }
  }
  //m_SoA.clear();
}


void TrigTrackSeedGenerator::storeTriplets(INTERNAL_TRIPLET_BUFFER& tripletMap) {
  for(INTERNAL_TRIPLET_BUFFER::iterator it=tripletMap.begin();it!=tripletMap.end();++it) {
    double Q = (*it).first;
    if((*it).second->s3().isSCT()) {
      Q += (*it).second->s1().isSCT() ? 1000.0 : 10000.0;
    }
    m_triplets.insert(std::pair<double, TrigInDetTriplet*>(Q, (*it).second));
  }
}

void TrigTrackSeedGenerator::getSeeds(std::vector<TrigInDetTriplet*>& vs) {
  vs.clear();
  vs.reserve(m_triplets.size());
  for(INTERNAL_TRIPLET_BUFFER::reverse_iterator it=m_triplets.rbegin();it!=m_triplets.rend();++it) {
    vs.push_back((*it).second);
    (*it).second = NULL;//ownership transferred
  }
  m_triplets.clear();
}
