/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuFastTrackExtrapolator.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/TrigMuonDefs.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MuFastTrackExtrapolator::MuFastTrackExtrapolator(const std::string& type, 
                                                               const std::string& name,
                                                               const IInterface*  parent): 
  AthAlgTool(type,name,parent)
{

  setMuFastRes(m_muFastRes_barrel,  0.042, -0.00046, 3.5, -1.8, 0.35, -0.017);
  setMuFastRes(m_muFastRes_endcap1, 0.098, -0.000097, 77.0, -47.0, 9.8, -0.67);
  setMuFastRes(m_muFastRes_endcap2, 0.19, -0.00043, 10.4, -5.21, 1.14, -0.056);
  setMuFastRes(m_muFastRes_endcap3, 0.087, -0.0013, 98.0, -60.0, 12.0, -0.80);
  setMuFastRes(m_muFastRes_endcap4, 0.060, -0.0014, 101.0, -61.0, 12.0, -0.80);
  
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MuFastTrackExtrapolator::extrapolateTrack(std::vector<TrigL2MuonSA::TrackPattern>& v_trackPatterns,
                                                                   double winPt)
{
  ATH_MSG_DEBUG("in extrapolateTrack");
  
  StatusCode sc = StatusCode::SUCCESS;
  
  std::vector<TrigL2MuonSA::TrackPattern>::iterator itTrack;
  for (itTrack=v_trackPatterns.begin(); itTrack!=v_trackPatterns.end(); itTrack++) {

    int inner = (itTrack->s_address==-1)? xAOD::L2MuonParameters::Chamber::EndcapInner:
      xAOD::L2MuonParameters::Chamber::BarrelInner;

    xAOD::L2StandAloneMuon* muonSA = new xAOD::L2StandAloneMuon();
    muonSA->makePrivateStore();
    muonSA->setSAddress(itTrack->s_address);
    muonSA->setPt((itTrack->pt)*(itTrack->charge));
    muonSA->setEtaMS(itTrack->etaMap);
    muonSA->setPhiMS(itTrack->phiMS);
    muonSA->setRMS(itTrack->superPoints[inner].R);
    muonSA->setZMS(itTrack->superPoints[inner].Z);

    double etaVtx = 0.;
    double phiVtx = 0.;
    double sigEta = 0.;
    double sigPhi = 0.;

    double eptinv = getMuFastRes(m_muFastRes_barrel, (itTrack->pt)*(itTrack->charge), itTrack->s_address, itTrack->etaMap, itTrack->phiMS);

    if (m_backExtrapolatorTool && muonSA) {

      sc = (*m_backExtrapolatorTool)->give_eta_phi_at_vertex(muonSA, etaVtx, sigEta, phiVtx, sigPhi, winPt);
    
      if (sc.isFailure()) {
        ATH_MSG_DEBUG ("BackExtrapolator problem: "
              << "Pt of Muon Feature out of BackExtrapolator range.");
        ATH_MSG_DEBUG ("Use Muon Feature position to fill the "
		       << "TrigRoiDescriptor for IDSCAN.");
        etaVtx = itTrack->etaMap;
        phiVtx = itTrack->phiMS;
      }

    } else {

      ATH_MSG_ERROR("Null pointer to ITrigMuonBackExtrapolator");
	return StatusCode::FAILURE;

    }

    itTrack->deltaPt     = (eptinv!=0.)? eptinv * (itTrack->pt) * (itTrack->pt): 1.0e10;
    itTrack->etaVtx      = etaVtx;
    itTrack->phiVtx      = phiVtx;
    itTrack->deltaEtaVtx = sigEta;
    itTrack->deltaPhiVtx = sigPhi;

    if (muonSA) delete muonSA;
  }
  return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MuFastTrackExtrapolator::setMuFastRes(std::vector<double>& vec, double p1,double p2,
                                                         double p3,double p4,double p5,double p6) {
  vec.clear();
  vec.push_back(p1);
  vec.push_back(p2);
  vec.push_back(p3);
  vec.push_back(p4);
  vec.push_back(p5);
  vec.push_back(p6);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

// Calculation of error on pT from L2 Muon SA
// Original author: Stefano Giagu
// Copied from TrigmuComb/muCombUtil.cxx
double TrigL2MuonSA::MuFastTrackExtrapolator::getMuFastRes(std::vector<double> vec,
                                                           const double pt, const int add,
                                                           const double eta, const double phi) {
  
  if (pt == 0) return 1.0e30;
  
  double AbsPtInv = fabs(1./pt);
  double AbsEta   = fabs(eta);
  
  if ( add != -1) {
    if (AbsPtInv < 0.186) {
      return vec[0]*AbsPtInv + vec[1];
    }
    else {
      double AbsPtInv3 = AbsPtInv*AbsPtInv*AbsPtInv;
      double AbsPtInv2 = AbsPtInv*AbsPtInv;
      return vec[2]*AbsPtInv3 + vec[3]*AbsPtInv2 + vec[4]*AbsPtInv +vec[5];
    }
  }
  else {//Takuya/Kunihiro updated numbers 
    
    const int N_PARAMS = 5;
    const double vparEC1[N_PARAMS] = {0.291483, -6.11348,  65.1099, -285.664,  440.041};
    const double vparEC2[N_PARAMS] = {0.286307, -4.6759,   43.2815, -163.185,  210.786};
    const double vparEC3[N_PARAMS] = {0.330699, -6.70755,  70.4725, -291.85,   408.739};
    const double vparEC4[N_PARAMS] = {0.261738, -4.69971,  47.4762, -183.98,   236.813};
    const double vparEC5[N_PARAMS] = {0.196301, -3.57276,  38.3744, -159.808,  228.256};
    const double vparEC6[N_PARAMS] = {0.172939, -3.10788,  33.3823, -142.996,  212.957};
    const double vparEC7[N_PARAMS] = {0.233017, -4.377,    42.5691, -171.752,  245.702};
    const double vparEC8[N_PARAMS] = {0.22389,  -4.16259,  40.1369, -162.824,  236.39};
    const double vparEC9[N_PARAMS] = {0.197992, -3.52117,  33.5997, -136.014,  197.474};
    const double vparECA[N_PARAMS] = {0.417289, -0.852254,-31.9257,  308.873, -719.591};
    const double vparECB[N_PARAMS] = {0.526612, -8.04087,  82.1906, -336.87,   462.973};
        
    const double AbsPtInvMin = 5e-3;   // 200 GeV
    const double AbsPtInvMax = 0.25;   //   4 GeV
    if( AbsPtInv < AbsPtInvMin ) AbsPtInv = AbsPtInvMin;
    if( AbsPtInv > AbsPtInvMax ) AbsPtInv = AbsPtInvMax;
      
    const double* vpar;
    xAOD::L2MuonParameters::ECRegions reg = xAOD::L2MuonParameters::whichECRegion(AbsEta,phi);
    if     ( reg ==  xAOD::L2MuonParameters::ECRegions::WeakBFieldA ) { vpar = vparECA; }
      else if( reg == xAOD::L2MuonParameters::ECRegions::WeakBFieldB ) { vpar = vparECB; }
      else {
        if     ( AbsEta < 1.20) { vpar = vparEC1; }
        else if( AbsEta < 1.35) { vpar = vparEC2; }
        else if( AbsEta < 1.50) { vpar = vparEC3; }
        else if( AbsEta < 1.65) { vpar = vparEC4; }
        else if( AbsEta < 1.80) { vpar = vparEC5; }
        else if( AbsEta < 1.95) { vpar = vparEC6; }
        else if( AbsEta < 2.10) { vpar = vparEC7; }
        else if( AbsEta < 2.35) { vpar = vparEC8; }
        else                    { vpar = vparEC9; }
      }
    
    double fracRes = vpar[0] + vpar[1]*AbsPtInv
      + vpar[2]*AbsPtInv*AbsPtInv 
      + vpar[3]*AbsPtInv*AbsPtInv*AbsPtInv
      + vpar[4]*AbsPtInv*AbsPtInv*AbsPtInv*AbsPtInv;
    
    return fabs(fracRes * AbsPtInv);
  }
}
        
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

// Original author: Stefano Giagu
// Copied from TrigmuComb/muCombUtil.cxx
/*
int TrigL2MuonSA::MuFastTrackExtrapolator::whichECRegion( const double eta, const double phi ) {
  // 0: bulk
  // 1: WeakBfield A
  // 2: WeakBfield B
  
  float absEta = fabs(eta);
  
  if ( ( 1.3 <= absEta && absEta < 1.45) &&
       ( (0          <= fabs(phi) && fabs(phi) < M_PI/48. )     ||
       (M_PI*11./48. <= fabs(phi) && fabs(phi) < M_PI*13./48. ) ||
       (M_PI*23./48. <= fabs(phi) && fabs(phi) < M_PI*25./48. ) ||
       (M_PI*35./48. <= fabs(phi) && fabs(phi) < M_PI*37./48. ) ||
       (M_PI*47./48. <= fabs(phi) && fabs(phi) < M_PI )
             )
       ) return 1;
  
  else if ( ( 1.5 <= absEta && absEta < 1.65 ) &&
            ( (M_PI*3./32.  <= fabs(phi) && fabs(phi) < M_PI*5./32. ) ||
              (M_PI*11./32. <= fabs(phi) && fabs(phi) < M_PI*13./32.) ||
              (M_PI*19./32. <= fabs(phi) && fabs(phi) < M_PI*21./32.) ||
              (M_PI*27./32. <= fabs(phi) && fabs(phi) < M_PI*29./32.)
              )
            ) return 2;
  
  else return 0;
}
*/


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

