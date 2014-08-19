/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigCombinatorialTrackFinding
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
#include <set>

#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetPattRecoEvent/TrigInDetTracklet.h"
#include "TrigInDetPattRecoEvent/TrigL2LayerSetLUT.h"
#include "IRegionSelector/IRoiDescriptor.h"

#include "TrigInDetPattRecoTools/TrigCombinatorialSettings.h"

#include "TrigInDetPattRecoTools/TrigCombinatorialTrackFinding.h"

#include "TrigInDetPattRecoTools/CommonAlgorithms.h"
#include "TrigInDetPattRecoTools/TripletMerging.h"
#include "TrigInDetPattRecoTools/TripletClusterFitter.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


TrigCombinatorialTrackFinding::TrigCombinatorialTrackFinding(const TrigCombinatorialSettings& tcs) : 
  m_settings(tcs), 
  m_lsh(tcs.m_maxBarrelPix,tcs.m_minEndcapPix,tcs.m_maxEndcapPix,tcs.m_maxSiliconLayer) {

  // Define RZ cut 

  for(int l=0;m_lsh.isValid(l);l++) {
    if(m_lsh.isPixel(l)) 
      m_cut_rz[l]=m_settings.m_tripletRZMaxPixel;
    else 
      m_cut_rz[l]=m_settings.m_tripletRZMax;
  }

  //StrB-style cuts
  float cuteta = 0.15 + 490.0/m_settings.m_tripletPtMin;
  //Boundaries generally wider than roidescriptor widths

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// NB: This is bad, and will ONLY work for RoIs that open in eta
  ///     For a perfectly respectable parallel sided RoI, the etaHalfWidth = 0
  ///     so this sort of "eta" width processing will simply fail
  ///
  ///     Should use the *explicit* etaMinus() and etaPlus() methods rather than
  ///     spurious angles based on calculating eta directions and eta widths 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////

  double etaHalfWidth = 0.5*(m_settings.roiDescriptor->etaPlus() - m_settings.roiDescriptor->etaMinus());

  if(cuteta<etaHalfWidth) cuteta=etaHalfWidth; 
  if(cuteta>10)cuteta=10;
  float expmin = std::exp(-(m_settings.roiDescriptor->eta()-cuteta));
  float expmax = std::exp(-(m_settings.roiDescriptor->eta()+cuteta));

  ////////////////////////////////////////////////////////////////////////////////////////////////////////

  m_mindzdr = 0.5*(1.0/expmin-expmin);//Avoid tan and arctan - trig identity
  m_maxdzdr = 0.5*(1.0/expmax-expmax);

}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigCombinatorialTrackFinding::~TrigCombinatorialTrackFinding() {

}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigCombinatorialTrackFinding::findDoublets(const std::vector<const TrigSiSpacePointBase*>& sp1, 
						       const std::vector<const TrigSiSpacePointBase*>& sp2, 
						       std::vector<TrigInDetDoublet*>& doublets) {

  // Create return status
  StatusCode sc(StatusCode::SUCCESS);

  // Check that we are processing meaningful SP vectors
  if(sp1.empty() || sp2.empty()) return sc;

  doublets.reserve(5000);
  // Prepare cuts and variables



  float cutd02 = m_settings.m_doubletD0Max*m_settings.m_doubletD0Max;

  for(auto s1 : sp1) {
    
    // Get first SP info
    float phi1 = s1->phi();
    float r1   = s1->r();
    float x1   = s1->x();
    float y1   = s1->y();
    float z1   = s1->z();

    for(auto s2 : sp2) {
      
      // Get sp2 info
      float phi2 = s2->phi();
      float r2   = s2->r();
      float x2   = s2->x();
      float y2   = s2->y();
      float z2   = s2->z();
      float x12  = x2 - x1;
      float y12  = y2 - y1;
      float z12  = z2 - z1;
      float r12  = r2 - r1;

      //bool goodDoublet = ((s1->barCode()==11) && (s2->barCode()==11)) || ((s1->barCode()==12) && (s2->barCode()==12));
      //      bool goodDoublet = (s1->barCode()==10001) && (s2->barCode()==10001);

      // Cut on seed's d0
      float d0tmp = (y12*x1 - x12*y1);
      if(d0tmp*d0tmp > cutd02*(y12*y12+x12*x12)) {
	/*
	if(goodDoublet) {
	  std::cout<<"DOUBLET REJECTED by d0 cut = "<<(d0tmp*d0tmp/((y2-y1)*(y2-y1)+(x1-x2)*(x1-x2)))<<std::endl;
	}
	*/
	continue;
      }

      // Cut on phi/r
      float dphi = phi2-phi1;
      correct_phi(dphi);
      if(dphi>m_settings.m_doubletDphiOverDrMax*r12 || dphi<-m_settings.m_doubletDphiOverDrMax*r12) {
	/*
	if(goodDoublet) {
	  std::cout<<"DOUBLET REJECTED by phi cut = "<<(dphi/(r2-r1))<<std::endl;
	}
	*/
	continue;
      }

      // Cut on dr
      if(r2<=r1) continue;
      
      // Create doublet
      float tau = z12/r12;
      float z0 = z1 - r1*tau;

      if(fabs(z0)>m_settings.m_doubletZ0Max) {
        /*
           if(goodDoublet) {
           std::cout<<"DOUBLET REJECTED by z0 cut = "<<z0<<std::endl;
           }
           */
        continue;
      }

      if(tau<m_mindzdr || tau>m_maxdzdr) {
        //std::cout << "does not pass doublet cuts" << std::endl;
        //if(goodDoublet) {
        //  std::cout<<"DOUBLET REJECTED by tau cut = "<<tau<<" mindzdr="<<mindzdr<<" maxdzdr="<<maxdzdr<<std::endl;
        //  std::cout<<"RoI eta="<<m_settings.m_roiEta<<" cuteta="<<cuteta<<" etahalfW="<<m_settings.m_etaHalfWidth<<std::endl;
        //}
        continue;
      }
      TrigInDetDoublet* doublet = new TrigInDetDoublet(*s1, *s2, z0, tau);
      doublets.push_back(doublet);
    }
  }

  // Return
  return sc;

}

struct compareTriplets{
  bool operator()(const std::shared_ptr<TrigInDetTriplet> p1, const std::shared_ptr<TrigInDetTriplet> p2) {
    return (p1->chi2()<p2->chi2());
  }
};

struct compareTripletRadius{
  bool operator()(const std::shared_ptr<TrigInDetTriplet> p1, const std::shared_ptr<TrigInDetTriplet> p2) {
    return (p1->s3().r() < p2->s3().r());
  }
};

StatusCode TrigCombinatorialTrackFinding::findTripletClusters(const std::vector<TrigInDetDoublet*>& doublets, 
						       const std::vector<const TrigSiSpacePointBase*>& sp3,
						       std::vector<TrigInDetTripletCluster*>& tripletClusters,
                   const TrigL2LayerSetLUT* pLUT) {

  const float mu = 0.2;//average number of noise spacepoints in r-z cut volume
  const float Pd = 0.8;//average spacepoint finding efficiency
  const float lPd = log(1.0-Pd);


  // Create return status
  StatusCode sc(StatusCode::SUCCESS);

  // Check that we are processing meaningful SP vectors
  if(sp3.empty()) return sc;

  std::vector<LAYERED_STORAGE_BASE*> layeredStorage;
  layeredStorage.reserve(m_lsh.getNumberOfLayers());

  //TO-DO : use layer numbering from the tool 

  for(int l=0;m_lsh.isValid(l);l++) {
    if(m_lsh.isBarrel(l)) {
      layeredStorage.push_back(new LAYERED_STORAGE_BARREL(l));
    }
    else {
      layeredStorage.push_back(new LAYERED_STORAGE_ENDCAP(l));
    }
  }

  for(const auto & sp : sp3) layeredStorage[sp->layer()]->addSpacePoint(sp);
  for(auto& l : layeredStorage) l->sort();

  // Loop on doublets

  //std::cout << "doublets.size() " << doublets.size() << std::endl;

  //CDF_APPROXIMATOR approximator;


  for(const auto& doublet : doublets) {

    // Prepare preliminary vectors of triplets
    std::vector<std::shared_ptr<TrigInDetTriplet>> unfilteredTriplets;
    // Create triplets
    sc = createTriplets(*doublet, layeredStorage, unfilteredTriplets);
    //std::cout << "unfilteredTriplets.size() " << unfilteredTriplets.size() << std::endl;

    // Check minimum triplet size is reached
    if(int(unfilteredTriplets.size()) < m_settings.m_minTriplets) continue;

    // Filter triplets
    std::vector<std::shared_ptr<TrigInDetTriplet>> acceptedTriplets;
    if(m_settings.m_tripletFilter && unfilteredTriplets.size() > 1) {
      //std::cout << "Filtering triplets" << std::endl;
      filterTriplets(m_lsh, unfilteredTriplets, acceptedTriplets);
    }
    else {
      //std::cout << "Not filtering triplets" << std::endl;
      acceptedTriplets = std::move(unfilteredTriplets);
      std::sort(acceptedTriplets.begin()+1, acceptedTriplets.end(), compareTriplets());//keep the first in place
    }

    if(int(acceptedTriplets.size()) < m_settings.m_minTriplets) continue;


    //Weight multiple triplets by likelihood

    float sum_eta=0, sum_phi=0, sum_z0=0, sum_d0=0, sum_pt=0, sum_weight=0, sum_chi2=0;
    for(const auto& triplet : acceptedTriplets) {
      //weight = likelihood of chi^2
      float weight = triplet->weight();
      //std::cout<<"w="<<weight<<" pT="<<triplet->pt()<<" chi2="<<triplet->chi2() << std::endl;
      sum_eta      += triplet->eta() * weight;
      sum_phi      += triplet->phi() * weight;
      sum_z0       += triplet->z0()  * weight;
      sum_d0       += triplet->d0()  * weight;
      sum_pt       += triplet->pt()  * weight;
      sum_chi2     += triplet->chi2();
      sum_weight   += weight;
      //vWC.push_back(WEIGHTED_COORDINATE(1/triplet->pt(), weight));
    }
    float sum_weight_inv = 1.0/sum_weight;
    float eta  = sum_eta  * sum_weight_inv;
    float phi  = sum_phi  * sum_weight_inv;
    float z0   = sum_z0   * sum_weight_inv;
    float d0   = sum_d0   * sum_weight_inv;
    float pt   = sum_pt   * sum_weight_inv;
    float chi2 = sum_chi2 / acceptedTriplets.size();

    //double medianInvPt = (acceptedTriplets.size() == 1) ? 1/pt : approximator.findMedian(vWC);
    //std::cout<<"median pT estimate = "<<1.0/medianInvPt<<std::endl;

    float llr = 0.0;

    if(pLUT!=NULL) {
      const short* layerSet = pLUT->getSet(doublet->z0(), doublet->tau());
      if(layerSet!=NULL) {

        // std::cout<<"predicted set of layers:"<<std::endl;
        // for(int l=0;l<pLUT->m_nLayers;l++) {
        //	if(layerSet[l]==999) break;
        //	std::cout<<abs(layerSet[l])<<" ";
        // }
        //std::cout<<std::endl;
        //std::cout<<"actual set of layers:"<<std::endl;
        //std::cout<<doublet->s1().layer()<<" "<<doublet->s2().layer()<<" ";
        //for(auto triplet : acceptedTriplets) {
        //	std::cout<<triplet->s3().layer()<<" ";
        //}
        //std::cout<<std::endl;

        int predictedSet[20];
        int observedSet[20];

        memset(&predictedSet[0],0,sizeof(predictedSet));
        memset(&observedSet[0],0,sizeof(observedSet));

        bool start=false;
        for(int l=0;l<pLUT->m_nLayers;l++) {
          if(layerSet[l]==999) break;
          int layerId = abs(layerSet[l]);
          if(layerId==doublet->s2().layer()) {
            start = true;continue;
          }
          if(start) predictedSet[layerId] = 1;
        }
        for(const auto& triplet : acceptedTriplets) {
          int layerId = triplet->s3().layer();
          observedSet[layerId]++;
        }
        //calculate log-likelihood ratio (LLR)

        for(int k=0;k<20;k++) {
          if(predictedSet[k]==0) continue;
          if(observedSet[k]==0) {
            llr += lPd;
          }
          else {
            float sum = 1.0 - Pd;
            for(const auto& triplet : acceptedTriplets) {
              int layerId = triplet->s3().layer();
              if(layerId!=k) continue;
              sum += (Pd/(mu*sqrt(2.0*M_PI*triplet->V())))*triplet->weight();
            }
            llr += log(sum);
          }
        }
        // std::cout<<"LLR="<<llr<<std::endl;
      } else llr = -100.0;
    }
    // Create cluster

    if(llr>=m_settings.m_tripletMinLLR) {
      int nTriplets = (int)acceptedTriplets.size();
      bool passesFit = true;
      if(m_settings.m_prefitTripletClusters) {

        TRIPLET_CLUSTER_FITTER tcFit(m_lsh, m_settings.m_magFieldZ);

        nTriplets = tcFit.fit(acceptedTriplets, eta, z0, pt);
        if (fabs(pt) < m_settings.m_tripletPtMin) {
          passesFit = false;
        }
      }
      if (nTriplets>=m_settings.m_minTriplets && passesFit) { 
        TrigInDetTripletCluster* tripletCluster = new TrigInDetTripletCluster(acceptedTriplets, eta, phi, z0, d0, pt, chi2, llr);
        tripletClusters.push_back(tripletCluster);
      }
    }
    acceptedTriplets.clear();
  }

  for(auto l : layeredStorage) delete l;
  layeredStorage.clear();
  // Return
  return sc;
}

void TrigCombinatorialTrackFinding::processClusters(std::vector<TrigInDetTripletCluster*>& tripletClusters) {
  //TO-DO : use layer numbering from the tool 
  processClusters(m_lsh, tripletClusters);
}

StatusCode TrigCombinatorialTrackFinding::createTriplets(const TrigInDetDoublet& doublet, const std::vector<LAYERED_STORAGE_BASE*>& layeredStorage, 
							 std::vector<std::shared_ptr<TrigInDetTriplet>>& triplets) {

  const double radLength = 0.036;//assumed radiation length of a pixel or double SCT layer


  // Create return status
  StatusCode sc(StatusCode::SUCCESS);
  triplets.reserve(100);

  // Prepare cuts and variables
  const float roiEta = m_settings.roiDescriptor->eta();
  const float cut_d0 = m_settings.m_tripletD0Max;
  const float cut_pt = m_settings.m_tripletPtMin;
  const float cut_rphi = m_settings.m_tripletRPhiMax;
  float cut_eta = 0.15 + 490/m_settings.m_tripletPtMin;
  const float magField_scale = -0.3 * m_settings.m_magFieldZ;

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  /// NB: This is bad, and will ONLY work for RoIs that open in eta
  ///     For a perfectly respectable parallel sided RoI, the etaHalfWidth = 0
  ///     so this sort of "eta" width processing will simply fail
  ///
  ///     Should use the *explicit* etaMinus() and etaPlus() methods rather than
  ///     spurious angles based on calculating eta directions and eta widths 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////

  double etaHalfWidth = 0.5*(m_settings.roiDescriptor->etaPlus() - m_settings.roiDescriptor->etaMinus());

  if(cut_eta<etaHalfWidth) cut_eta = etaHalfWidth;

  // Get sp1 and sp2 info
  const TrigSiSpacePointBase& s1 = doublet.s1();
  const TrigSiSpacePointBase& s2 = doublet.s2();
  const float r1 = s1.r();
  const float r2 = s2.r();
  const float z1 = s1.z();
  const float z2 = s2.z();
  const float x1 = s1.x();
  const float x2 = s2.x();
  const float y1 = s1.y();
  const float y2 = s2.y();
  const float x12 = x2 - x1;
  const float y12 = y2 - y1;
  float z12 = z2 - z1;
  float r12 = r2 - r1;

  // Protect against division by zero
  if(r12>-0.00001 && r12<0.00001) {
    float sign=1; if(r12<0) sign=-1;
    r12=sign*0.00001;
  }

  // Protect against division by zero
  if(z12>-0.00001 && z12<0.00001) {
    float sign=1; if(z12<0) sign=-1;
    z12=sign*0.00001;
  }

  float x2prime = sqrt(x12*x12+y12*y12);
  const float r12inv = 1.0 / r12;
  const float r12_squ = r12*r12;
  const float r12inv_squ = 1.0/r12_squ;
  const float x12_scaled = x12*r12inv;
  const float y12_scaled = y12*r12inv;

  const float L = sqrt(z12*z12+r12_squ);

  // rz fit variables: z0 and cotan(theta) estimated from first two spacepoints only

  float X_hat[2], C_hat[2][2];

  X_hat[0] = (r2*z1-z2*r1)*r12inv;
  X_hat[1] = z12*r12inv;
  const float ctg2 = X_hat[1]*X_hat[1];

  const float S1 = s1.dz()*s1.dz()+ctg2*s1.dr()*s1.dr();
  const float S2 = s2.dz()*s2.dz()+ctg2*s2.dr()*s2.dr();

  C_hat[1][1] = (S1+S2)*r12inv_squ;
  C_hat[0][1] = C_hat[1][0] = (-S1*r2-S2*r1)*r12inv_squ;
  C_hat[0][0] = (S2*r1*r1 + S1*r2*r2)*r12inv_squ;

  double mult_scatt2 = (13.6*13.6)*(1+ctg2)*radLength*sqrt(1+ctg2);
  if(s2.dr()>s2.dz()) mult_scatt2 /= fabs(X_hat[1]);//for endcap

  //bool goodDoublet = (s1.barCode()==11) && (s2.barCode()==11);
  //if (!goodDoublet) {
  //  goodDoublet = (s1.barCode()==12) && (s2.barCode()==12);
  //}
  //if(goodDoublet) { 
  //  //std::cout<<"GOOD DOUBLET FOUND"<<std::endl;
  //}

  
  //Preliminary parts of phi and d0 calculations
  //Transform into coordinate system rotated so that y1'=y2'=0, at angle phi
  const float phi0 = std::atan2(y12,x12);
  const float sin_phi0 = std::sin(-phi0);//minus sign needed to keep y1'=y2'
  const float cos_phi0 = std::cos(phi0);
  //const float L0 = std::fabs(x1*cos_phi0 - y1*sin(phi0));//Distance to 1st spacepoint from origin
  float y1_star = x1*sin_phi0 + y1*cos_phi0;
  float x1_star = x1*cos_phi0 - y1*sin_phi0;
  float x2_star = x2*cos_phi0 - y2*sin_phi0;

  // Loop on third SP container
  int layerCounter=0;

  for(auto l : layeredStorage) {

    if(!m_settings.m_tripletDoMaxLayers) {
      if(layerCounter>=m_settings.m_tripletMaxLayers) break;
    }

    if(l->isEmpty()) continue;

    //bool hasGoodSP = false;
    //if(goodDoublet) {
    //  hasGoodSP = l->checkBarcode(10001);
    //}

    SP_RANGE spr = l->getSpacePoints(&doublet,2.5*m_cut_rz[l->layerId()]);

    if(std::distance(spr.first,spr.second)==0) continue;

    //int nGoodSPinLayer=0;

    layerCounter++;

    for(std::vector<SORTED_SP>::const_iterator spIt = spr.first; spIt!=spr.second;++spIt) {

      const TrigSiSpacePointBase* s3 = (*spIt).m_pSP;

      //bool goodSPinLayer=false;
      //if(s3->barCode()==10001) {
      //  nGoodSPinLayer++;
      //  goodSPinLayer=true;
      //}

      const float r3 = s3->r();
      const float z3 = s3->z();
      const float r13 = r3-r1;
      const float z13 = z3-z1;
      const float miss = r12*z13 - z12*r13;
      const int layer3 = s3->layer();
      //bool rejected_by_rz = false;
      if(fabs(miss) > m_cut_rz[layer3]*L) {

        //if(goodDoublet && goodSPinLayer) {
        //  std::cout<<"rejected by rz, miss="<<miss/L<<std::endl;
        //}
        //rejected_by_rz = true;

        continue;
      }

      // Get sp3 info
      const float x3 = s3->x();
      const float y3 = s3->y();
      const float r23 = r3-r2;
      
      // Define RPhi cut
      const float cut = cut_rphi*r23*r23*r23;
      const float deltaX = x1 + x12_scaled*r13 - x3;
      const float deltaY = y1 + y12_scaled*r13 - y3;
      const float value = deltaX*deltaX + deltaY*deltaY;

      //bool rejected_by_rphi = false;
      // Cut on rphi
      if(value > cut) {
        //rejected_by_rphi = true;

        //if(goodDoublet && goodSPinLayer) {
        //  std::cout<<"rejected by rphi, rphi="<<value/((r3-r2)*(r3-r2)*(r3-r2))<<std::endl;
        //}

        continue;
      }


      //Transform into line with s1 and s2 on x-axis, s1 at (0,0)
      const float x13 = x3-x1;
      const float y13 = y3-y1;
      float x3prime = (x13*x12 + y13*y12)/x2prime;
      float y3prime = (y13*x12 - x13*y12)/x2prime;
      

      const float alpha = 2*y3prime/(x3prime*(x3prime-x2prime));
      float pT = 1e9;
      if (alpha!=0.0) {
        pT = magField_scale/alpha;
      }

      //bool rejected_by_pT = false;
      if(fabs(pT)<cut_pt) {

        //rejected_by_pT = true;
        //if(goodDoublet && goodSPinLayer) {
        //  std::cout<<"rejected by pT, pT="<<pt<<std::endl;
        //}

        continue;
      }

      const float d0 = y1_star + 0.5*alpha*x1_star*x2_star;

      //bool rejected_by_d0 = false;
      if(fabs(d0) > cut_d0) {

        //if(goodDoublet && goodSPinLayer) {
        //  std::cout<<"rejected by d0, d0="<<d0<<std::endl;
        //}

        //rejected_by_d0 = true;
        continue;
      }


      double varMS = mult_scatt2/(pT*pT);

      // Preparatory fit steps
      float D[2], K[2];
      // C*H^T
      D[0] = C_hat[0][0] + r3*C_hat[0][1];
      D[1] = C_hat[0][1] + r3*(C_hat[1][1]+varMS);

      float S3 = s3->dz()*s3->dz()+ctg2*s3->dr()*s3->dr();
      float V = D[0]+r3*D[1]+S3;
      float K_denom = 1.0/V;
      float res = -X_hat[0]-X_hat[1]*r3+z3;
      float chi2 = res*res*K_denom;
      //bool rejected_by_chi2 = false;
      if (chi2 > m_settings.m_tripletChi2Max) {
        //rejected_by_chi2 = true;
        continue;
      }

      K[0] = D[0]* K_denom;
      K[1] = D[1]* K_denom;


      float X_hat_update[2];

      X_hat_update[0] = X_hat[0] + K[0]*res;
      X_hat_update[1] = X_hat[1] + K[1]*res;
      // RZ fit: update with extrapolation to third spacepoint
      float z0 = X_hat_update[0];
      float cotan_theta_hat_update = X_hat_update[1];
      float eta = asinh(cotan_theta_hat_update); // use identity sinh(eta)==cotan(theta)


      //bool rejected_by_eta_phi = false;
      float diff_eta = std::fabs(eta - roiEta);
      if(m_settings.m_tripletCutRoiEta && (diff_eta > cut_eta)) {
        //rejected_by_eta_phi = true;
        //if(goodDoublet && (s3->barCode() == trueBarCode)) {
          //std::cout<<"rejected by eta-phi cut" << std::endl;
          //std::cout << std::endl;
        //}

        continue;
      }
      /*
      float phi = phi0 -alpha*(0.5*x2prime + L0);
      correct_phi(phi);
      */

      const float x23 = x3-x2;
      const float y23 = y3-y2;
      double kA = x13*x23+y13*y23;
      double kB = y13*x12-x13*y12;

      double phi = atan2(kA*y12-kB*(x1+x2),kA*x12+kB*(y1+y2)); 

      if(m_settings.m_tripletCutRoiPhi && !m_settings.roiDescriptor->containsPhi(phi)) {
        continue;
      }
      //std::cout << std::boolalpha << "rejected_by_rz: " << rejected_by_rz << std::endl;
      //std::cout << std::boolalpha << "rejected_by_pT: " << rejected_by_pT << std::endl;
      //std::cout << std::boolalpha << "rejected_by_d0: " << rejected_by_d0 << std::endl;
      //std::cout << std::boolalpha << "rejected_by_rphi: " << rejected_by_rphi << std::endl;
      //std::cout << std::boolalpha << "rejected_by_chi2: " << rejected_by_chi2 << std::endl;
      //std::cout << std::boolalpha << "rejected_by_eta_phi: " << rejected_by_eta_phi << std::endl;


      //std::cout << "Triplet pass/reject: " << std::boolalpha << rejected_by_rz  << "\t" << rejected_by_pT  << "\t" << rejected_by_d0  << "\t" << rejected_by_rphi  << "\t" << rejected_by_chi2  << "\t" << rejected_by_eta_phi  << "\t" << std::endl;

      // Create triplet
      auto triplet = std::make_shared<TrigInDetTriplet>(TrigInDetTriplet(s1, s2, *s3, eta, phi, z0, d0, pT, chi2, V));
      triplets.push_back(triplet);
    }

    //if (goodDoublet && hasGoodSP && (nGoodSPinLayer==0)) {
    //  std::cout<<"LAYER HAS GOOD SP WHICH WAS NOT FOUND"<<std::endl;

    //  SP_RANGE spr = l->getSpacePoints(doublet,m_cut_rz[l->layerId()],true);

    //  std::cout<<"spacepoint range:"<<std::endl;

    //  for(std::vector<SORTED_SP>::const_iterator spIt = spr.first; spIt!=spr.second;++spIt) {
    //    const TrigSiSpacePointBase* s3 = (*spIt).m_pSP;
    //    std::cout<<"r="<<s3->r()<<" z="<<s3->z()<<std::endl;
    //  }
    //}


  }

  // Return
  return sc;

}

void TrigCombinatorialTrackFinding::filterTriplets(const LAYERED_STORAGE_HELPER& lsh, 
						    const std::vector<std::shared_ptr<TrigInDetTriplet>>& input,
						    std::vector<std::shared_ptr<TrigInDetTriplet>>& accepted) {

  if (input.empty()) return;
  unsigned int const inputSize = input.size();

  std::vector<MERGED_TRIPLETS> vM;
  vM.reserve(inputSize);
  int nLayers = lsh.getNumberOfLayers();

  for(int tIdx = 0;tIdx<(int)inputSize;tIdx++) {
    vM.push_back(MERGED_TRIPLETS(nLayers,tIdx,input[tIdx]));
  }

  //create pairs

  std::vector<std::tuple<float, unsigned int, unsigned int> > ambVec;
  ambVec.reserve(100);//Hand-tuned
  float const invFilterEtaScale = 1.0/m_settings.m_tripletFilterEtaScale;
  float const invFilterPhiScale = 1.0/m_settings.m_tripletFilterPhiScale;

  //int nGoodTriplets = 0;
  //int barCode = 10001;

  for(unsigned int i=0;i<inputSize-1;i++) {
    auto tr1 = input[i];

    //bool found1 = (barCode == tr1->s1().barCode()) && (barCode == tr1->s2().barCode()) && (barCode == tr1->s3().barCode());
    //if(found1) nGoodTriplets++;

    for(unsigned int j=i+1;j<inputSize;j++) {
      auto tr2 = input[j];
      // Evaluate eta-phi distance
      // Phi distance
      float dphi = tr1->phi() - tr2->phi();
      correct_phi(dphi);
      dphi *= invFilterPhiScale;
      // Eta distance
      float deta = tr1->eta() - tr2->eta();
      deta *= invFilterEtaScale;
      // Squared distance
      float distance = dphi*dphi + deta*deta;
      /*
      bool found2 = (barCode == tr2->s1().barCode()) && (barCode == tr2->s2().barCode()) && (barCode == tr2->s3().barCode());
      if(found1 && found2) {
	std::cout<<"Good pair: dphi="<<dphi<<" deta="<<deta<<" distance="<<distance<<" cut="<<m_settings.m_tripletFilterDRMax<<std::endl;
      }
      */
      // Filter on distance
      if(distance<m_settings.m_tripletFilterDRMax) {
        ambVec.push_back(std::make_tuple(distance, i,j));
      }
    }
  }

  //std::cout<<"nGoodTriplets="<<nGoodTriplets<<" Total="<<input.size()<<" nPairs="<<ambVec.size()<<std::endl;

  if(ambVec.empty()) return;//no pairing triplets -> reject all

  std::sort(ambVec.begin(), ambVec.end(),
      [](const std::tuple<double, unsigned int, unsigned int>& lhs, const std::tuple<double, unsigned int, unsigned int>& rhs) {
        return std::get<0>(lhs) < std::get<0>(rhs); } );//Sort by distance

  TRIPLET_MERGER tm(lsh);

  for(const auto& a : ambVec) {
    unsigned int const i = std::get<1>(a);
    unsigned int const j = std::get<2>(a);
    if ( vM[j] < vM[i] ) {
      tm.merge( vM[j], vM[i] );
    }
    else {
      tm.merge( vM[i], vM[j] );
    }
  }

  // Find best ambiguity

  unsigned  int iBest = 0;
  for(unsigned int idx = 0;idx<vM.size();idx++) {
    if(vM[idx]<vM[iBest]) {
      iBest = idx;
    }
  }

  std::vector<bool> vDecision(inputSize,false);
  vM[iBest].fill_decision(vDecision);
  accepted.clear();
  accepted.reserve(inputSize);
  for(unsigned int idx=0;idx<vDecision.size();idx++) {
    if(vDecision[idx]) accepted.push_back(input[idx]);
  }
  //input.clear();
  std::sort(accepted.begin(), accepted.end(), compareTriplets());
}

void TrigCombinatorialTrackFinding::collectTriplets(const std::vector<TrigInDetTripletCluster*>& clusters, std::vector<std::shared_ptr<TrigInDetTriplet>>& triplets) {
  triplets.clear();
  triplets.reserve(m_settings.m_minTriplets*clusters.size());
  for(const auto& cluster : clusters) {
    triplets.insert(triplets.end(), cluster->triplets().begin(), cluster->triplets().end());
  }
}

void TrigCombinatorialTrackFinding::collectBestTriplets(const TrigInDetTripletCluster* cluster, std::vector<std::shared_ptr<TrigInDetTriplet>>& triplets) {
  const std::vector<std::shared_ptr<TrigInDetTriplet>>& vpTr = cluster->triplets();
  unsigned int nBest = (m_settings.m_tripletNBest < vpTr.size()) ? m_settings.m_tripletNBest : vpTr.size();
  triplets.reserve(nBest);
  triplets.insert(triplets.end(), vpTr.begin(), vpTr.begin()+nBest);
}

void TrigCombinatorialTrackFinding::collectBestTriplets(const std::vector<TrigInDetTripletCluster*>& clusters, std::vector<std::shared_ptr<TrigInDetTriplet>>& triplets) {
  triplets.clear();
  triplets.reserve(m_settings.m_minTriplets*clusters.size());
  for(const auto& cluster : clusters) 
    collectBestTriplets(cluster, triplets);
}

typedef struct SortedTripletCluster {
public:
  SortedTripletCluster() : m_quality(0.0), m_index(0), m_groupId(-1), m_delete(false) {};
  SortedTripletCluster(float q, unsigned int idx) : m_quality(q), m_index(idx), m_groupId(-1), m_delete(false) {};
  SortedTripletCluster(const SortedTripletCluster& stc) : m_quality(stc.m_quality), m_llr(stc.m_llr), m_index(stc.m_index), m_groupId(stc.m_groupId),
							  m_delete(stc.m_delete) {};

  void check_delete() {
    m_delete = (int)m_index!=m_groupId;
  }

  float m_quality;
  float m_llr;
  unsigned int m_index;
  int m_groupId;
  bool m_delete;
} SORTED_TRIPLET_CLUSTER;

struct compareTripletClusterQuality {
  bool operator()(const SORTED_TRIPLET_CLUSTER& tc1, const SORTED_TRIPLET_CLUSTER& tc2) {
    return (tc1.m_quality > tc2.m_quality);
  }
};

struct compareTripletClusterLikelihood {
  bool operator()(const SORTED_TRIPLET_CLUSTER& tc1, const SORTED_TRIPLET_CLUSTER& tc2) {
    return (tc1.m_llr > tc2.m_llr);
  }
};

float TrigCombinatorialTrackFinding::calculateTripletClusterQuality(const TrigInDetTripletCluster* pTC) {

  //quality = number of unique crossed layers

  int layers[25];
  int nL=0;
  memset(&layers[0],0,sizeof(layers));

  auto pT = (*pTC->triplets().begin());
  layers[pT->s1().layer()]=1;
  layers[pT->s2().layer()]=1;
  nL += 2;
  for(const auto& t : pTC->triplets()) {
    int l = t->s3().layer();
    if(layers[l]==0) {
      nL++;
      layers[l]=1;
    }
  }
  return 1.0*nL;
}

bool TrigCombinatorialTrackFinding::checkClones(const TrigInDetTripletCluster* pTCi, const TrigInDetTripletCluster* pTCj) {
  bool result=false;

  //testing j against i

  std::vector<const TrigSiSpacePointBase*> iV = {&(*pTCi->triplets().begin())->s1(), &(*pTCi->triplets().begin())->s2()};
  

  iV.push_back(&(*pTCi->triplets().begin())->s1());
  iV.push_back(&(*pTCi->triplets().begin())->s2());
  for(const auto& t : pTCi->triplets()) {
    iV.push_back(&(t->s3()));
  }
  int nSP=2;
  std::vector<const TrigSiSpacePointBase*> jV = {&(*pTCi->triplets().begin())->s1(), &(*pTCi->triplets().begin())->s2()};
  for(const auto& t : pTCj->triplets()) {
    jV.push_back(&(t->s3()));
    nSP++;
  }
  //iV.sort();
  //jV.sort();

  std::sort(iV.begin(),iV.end());
  std::sort(jV.begin(),jV.end());

  std::vector<const TrigSiSpacePointBase*> commonV(100,nullptr);

  std::vector<const TrigSiSpacePointBase*>::iterator it = std::set_intersection(iV.begin(),iV.end(),jV.begin(),jV.end(),commonV.begin());
  commonV.resize(it-commonV.begin());
  unsigned int nCommon = commonV.size();

  result = (nCommon >= 0.65*nSP);

  return result;
}

void TrigCombinatorialTrackFinding::setSpacePointsUnused(std::vector<TrigSiSpacePointBase>& vSP) {
  for (auto & sp : vSP) {
    sp.usedBy(-1);
  }
}

void TrigCombinatorialTrackFinding::mergeClones(std::vector<TrigInDetTripletCluster*>& oldClusters, 
						std::vector<TrigInDetTripletCluster*>& newClusters) {

  int globalTag = 0;

  //initial tagging

  for(auto& pcl : oldClusters) {
    globalTag++;
    for(auto& ptr : pcl->triplets() ) {
      ptr->updateTag(globalTag);
    }
  }

  //finding pairs of neighbours

  std::vector<std::pair<std::shared_ptr<TrigInDetTriplet>, std::shared_ptr<TrigInDetTriplet>> > vPairs;
  vPairs.reserve(oldClusters.size());

  for(unsigned int iC=0;iC<oldClusters.size();iC++) {
    for(unsigned int jC=iC+1;jC<oldClusters.size();jC++) {
      for(auto& ip : oldClusters[iC]->triplets() ) {

	const TrigSiSpacePointBase* pAI[20];
	memset(&pAI[0],0,sizeof(pAI));
	pAI[ip->s1().layer()] = &(ip->s1());
	pAI[ip->s2().layer()] = &(ip->s2());
	pAI[ip->s3().layer()] = &(ip->s3());
	
	for(auto& jp : oldClusters[jC]->triplets() ) {

	  int nMatches=0;

	  nMatches += pAI[jp->s1().layer()] == (&(jp->s1()));
	  nMatches += pAI[jp->s2().layer()] == (&(jp->s2()));
	  nMatches += pAI[jp->s3().layer()] == (&(jp->s3()));
	  if((nMatches>1) && ((ip->pt()*jp->pt())>0.0)) {
	    vPairs.push_back(std::pair<std::shared_ptr<TrigInDetTriplet>, std::shared_ptr<TrigInDetTriplet>>(ip,jp));
	  }
	}
      }
    }
  }

  //iterative re-tagging

  int nFlips=0;
  do {
    nFlips=0;
    for(auto & pair : vPairs) {
      if(pair.first->tag() == pair.second->tag()) continue;
      if(pair.first->tag()<pair.second->tag()) 
	pair.second->updateTag(pair.first->tag());
      else pair.first->updateTag(pair.second->tag());
      ++nFlips;
    }
  } while (nFlips!=0);

  std::map<int, std::vector<std::shared_ptr<TrigInDetTriplet>> > clusterMap;
  for(unsigned int iC=0;iC<oldClusters.size();iC++) {
    for(auto& ip : oldClusters[iC]->triplets() ) {
      int tag = ip->tag();
      auto mIt = clusterMap.find(tag);
      if(mIt==clusterMap.end()) {
	clusterMap.insert(std::pair<int, std::vector<std::shared_ptr<TrigInDetTriplet>>>(tag, std::vector<std::shared_ptr<TrigInDetTriplet>>(0)));
	mIt = clusterMap.find(tag);
      }
      (*mIt).second.push_back(ip);
    }
  }

  //  std::cout<<"Will create "<<clusterMap.size()<<" new clusters"<<std::endl;

  newClusters.reserve(oldClusters.size());
  for(auto mIt=clusterMap.begin();mIt!=clusterMap.end();++mIt) {
    std::sort((*mIt).second.begin(), (*mIt).second.end(), compareTriplets());
    newClusters.push_back(new TrigInDetTripletCluster((*mIt).second, 0.0,0.0,0.0,0.0,0.0,0.0,0.0));
  }
  for(auto& pcl : oldClusters) {
    pcl->resetTriplets();
  }
}

void TrigCombinatorialTrackFinding::processClusters(LAYERED_STORAGE_HELPER&, std::vector<TrigInDetTripletCluster*>& vC) {

  //std::cout<<"Total "<<vC.size()<<" clusters"<< " " << __FILE__ << std::endl;

  //1. create sorted cluster objects

  std::vector<SORTED_TRIPLET_CLUSTER> vSTC;
  vSTC.resize(vC.size());

  for(unsigned int i=0;i<vC.size();i++) {
    vSTC[i].m_index = i;
    vSTC[i].m_quality = calculateTripletClusterQuality(vC[i]); 
    vSTC[i].m_groupId = i;//each cluster forms a group on its own
    vSTC[i].m_llr = vC[i]->llr();//each cluster forms a group on its own
  }

  //2. sort the cluster objects

  if(m_settings.m_useLikelihood) 
    std::sort(vSTC.begin(), vSTC.end(), compareTripletClusterLikelihood());
  else 
    std::sort(vSTC.begin(), vSTC.end(), compareTripletClusterQuality());

  //3. spacepoint markup

  for(unsigned int i=0;i<vSTC.size();i++) {
    TrigInDetTripletCluster* pTC = vC[vSTC[i].m_index];
    TrigSiSpacePointBase& pSP1 = const_cast<TrigSiSpacePointBase&>((*pTC->triplets().begin())->s1());
    if(pSP1.usedBy()==-1) pSP1.usedBy(i);
    TrigSiSpacePointBase& pSP2 = const_cast<TrigSiSpacePointBase&>((*pTC->triplets().begin())->s2());
    if(pSP2.usedBy()==-1) pSP2.usedBy(i);
    for(auto it =  pTC->triplets().begin();it != pTC->triplets().end();++it) {
      TrigSiSpacePointBase& pSP3 = const_cast<TrigSiSpacePointBase&>((*it)->s3());
      if(pSP3.usedBy()==-1) pSP3.usedBy(i);
    }
  }

  //4. clone detection/analysis
  for(int i=0;i< int(vSTC.size());i++) {

    TrigInDetTripletCluster* pTC = vC[vSTC[i].m_index];

    int nTotal=2;
    int nUsed=0;

    const TrigSiSpacePointBase& pSP1 = (*pTC->triplets().begin())->s1();
    if(pSP1.usedBy()!=i) nUsed++;
    const TrigSiSpacePointBase& pSP2 = (*pTC->triplets().begin())->s2();
    if(pSP2.usedBy()!=i) nUsed++;
    for(auto it =  pTC->triplets().begin();it != pTC->triplets().end();++it) {
      nTotal++;
      if((*it)->s3().usedBy()!=i) nUsed++;
    }
    float threshold = m_settings.m_tripletCloneRemoveThreshold*nTotal;
    if(nUsed > threshold) {
      vSTC[i].m_delete=true;
    }
    else vSTC[i].m_delete=false;
  }
  
  std::vector<TrigInDetTripletCluster*> newvC;
  newvC.clear();
  for(auto stc : vSTC) { 
    //std::cout<<"delete "<<std::boolalpha<<stc.m_delete<< " " << __FILE__ << std::endl;
    if(stc.m_delete) delete vC[stc.m_index];
    else 
      {
	//   std::cout<<"retaining cluster"<<stc.m_index<< " " << __FILE__ << std::endl;
        newvC.push_back(vC[stc.m_index]);
      }
  }
  //std::cout<<newvC.size()<<" clusters will be retained"<< " " << __FILE__ << std::endl;
  vC.clear();
  std::copy(newvC.begin(), newvC.end(),std::back_inserter(vC));
  //std::cout<<vC.size()<<" clusters retained"<< " " << __FILE__ << std::endl;

}
