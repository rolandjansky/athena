/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigDiMuon/TrigDiMuonExtrapolator.h"


const double PI_OVER_4      = M_PI/4.;
const double PI_OVER_8      = M_PI/8.;


TrigDiMuonExtrapolator::TrigDiMuonExtrapolator( const std::string& t, const std::string& n, const IInterface* p) :
    AlgTool(t,n,p)
{

  declareInterface< TrigDiMuonExtrapolator >( this );

  declareProperty("UseTiltedBField", m_tilted_B_field= false);

  declareProperty("RpcEtaFactor",         m_RpcEtaFactor  = 2.);
  declareProperty("RpcPhiFactor",         m_RpcPhiFactor  = 2.);
  declareProperty("TgcEtaFactor",         m_TgcEtaFactor  = 2.);
  declareProperty("TgcPhiFactor",         m_TgcPhiFactor  = 2.);
  declareProperty("MdtEtaFactor",         m_MdtEtaFactor  = 2.);

  //extrapolator constants

  // Inner station *********************************
  //Barrel
  declareProperty("InnerBarrel_c0", m_ib_c0 = 0.002);
  declareProperty("InnerBarrel_c1", m_ib_c1 = 508.4);

  //Endcap
  //bin 1 : fabs(etaId)>1.6
  declareProperty("InnerEndcap_etaBin1_c0", m_ie_etaBin1_c0 = 0.0007);
  declareProperty("InnerEndcap_etaBin1_c1", m_ie_etaBin1_c1 = 303.2);

  //bin 2 : fabs(etaId)>1.3 && fabs(etaId)<=1.6
  declareProperty("InnerEndcap_etaBin2_c0", m_ie_etaBin2_c0 = 0.001);
  declareProperty("InnerEndcap_etaBin2_c1", m_ie_etaBin2_c1 = 402.1);

  //bin 3 : fabs(etaId)<=1.3
  declareProperty("InnerEndcap_etaBin3_c0", m_ie_etaBin3_c0 = 0.0005);
  declareProperty("InnerEndcap_etaBin3_c1", m_ie_etaBin3_c1 = 478.6);

  // Middle station ********************************
  //Barrel
  declareProperty("MiddleBarrel_c0", m_mb_c0 = 0.0015);
  declareProperty("MiddleBarrel_c1", m_mb_c1 = 57.4);
  declareProperty("MiddleBarrel_c2", m_mb_c2 = 699121.5);

  //Endcap
  //etaBin1 : fabs(etaId)>2.
  declareProperty("MiddleEndcap_etaBin1_phiBin1_c0", m_me_etaBin1_phiBin1_c0 = 0.002);
  declareProperty("MiddleEndcap_etaBin1_phiBin1_c1", m_me_etaBin1_phiBin1_c1 = 463.8);
  declareProperty("MiddleEndcap_etaBin1_phiBin1_c2", m_me_etaBin1_phiBin1_c2 = 404225.8);

  declareProperty("MiddleEndcap_etaBin1_phiBin2_c0", m_me_etaBin1_phiBin2_c0 = 0.0016);
  declareProperty("MiddleEndcap_etaBin1_phiBin2_c1", m_me_etaBin1_phiBin2_c1 = 407.6);
  declareProperty("MiddleEndcap_etaBin1_phiBin2_c2", m_me_etaBin1_phiBin2_c2 = 379832.4);

  declareProperty("MiddleEndcap_etaBin1_phiBin3_c0", m_me_etaBin1_phiBin3_c0 = 0.0017);
  declareProperty("MiddleEndcap_etaBin1_phiBin3_c1", m_me_etaBin1_phiBin3_c1 = 361.6);
  declareProperty("MiddleEndcap_etaBin1_phiBin3_c2", m_me_etaBin1_phiBin3_c2 = 447396.5);

  //etaBin2 : fabs(etaId)>1.65 && fabs(etaId)<=2.
  declareProperty("MiddleEndcap_etaBin2_phiBin1_c0", m_me_etaBin2_phiBin1_c0 = 0.0023);
  declareProperty("MiddleEndcap_etaBin2_phiBin1_c1", m_me_etaBin2_phiBin1_c1 = 349.7);
  declareProperty("MiddleEndcap_etaBin2_phiBin1_c2", m_me_etaBin2_phiBin1_c2 = 590774.7);

  declareProperty("MiddleEndcap_etaBin2_phiBin2_c0", m_me_etaBin2_phiBin2_c0 = 0.002);
  declareProperty("MiddleEndcap_etaBin2_phiBin2_c1", m_me_etaBin2_phiBin2_c1 = 259.1);
  declareProperty("MiddleEndcap_etaBin2_phiBin2_c2", m_me_etaBin2_phiBin2_c2 = 565283.6);

  declareProperty("MiddleEndcap_etaBin2_phiBin3_c0", m_me_etaBin2_phiBin3_c0 = 0.0016);
  declareProperty("MiddleEndcap_etaBin2_phiBin3_c1", m_me_etaBin2_phiBin3_c1 = 210.1);
  declareProperty("MiddleEndcap_etaBin2_phiBin3_c2", m_me_etaBin2_phiBin3_c2 = 580269.5);

  //etaBin3 : fabs(etaId)>1.35 && fabs(etaId)<=1.65
  declareProperty("MiddleEndcap_etaBin3_phiBin1_c0", m_me_etaBin3_phiBin1_c0 = 0.0018);
  declareProperty("MiddleEndcap_etaBin3_phiBin1_c1", m_me_etaBin3_phiBin1_c1 = 212.3);
  declareProperty("MiddleEndcap_etaBin3_phiBin1_c2", m_me_etaBin3_phiBin1_c2 = 435331.3);

  declareProperty("MiddleEndcap_etaBin3_phiBin2_c0", m_me_etaBin3_phiBin2_c0 = 0.00084);
  declareProperty("MiddleEndcap_etaBin3_phiBin2_c1", m_me_etaBin3_phiBin2_c1 = 155.8);
  declareProperty("MiddleEndcap_etaBin3_phiBin2_c2", m_me_etaBin3_phiBin2_c2 = 379710.6);

  declareProperty("MiddleEndcap_etaBin3_phiBin3_c0", m_me_etaBin3_phiBin3_c0 = 0.0003);
  declareProperty("MiddleEndcap_etaBin3_phiBin3_c1", m_me_etaBin3_phiBin3_c1 = 23.7);
  declareProperty("MiddleEndcap_etaBin3_phiBin3_c2", m_me_etaBin3_phiBin3_c2 = 521576.8);

  //etaBin4 : fabs(etaId)>1.2 && fabs(etaId)<=1.35
  declareProperty("MiddleEndcap_etaBin4_phiBin1_c0", m_me_etaBin4_phiBin1_c0 = 0.0011);
  declareProperty("MiddleEndcap_etaBin4_phiBin1_c1", m_me_etaBin4_phiBin1_c1 = 260.);
  declareProperty("MiddleEndcap_etaBin4_phiBin1_c2", m_me_etaBin4_phiBin1_c2 = 1049470.8);

  declareProperty("MiddleEndcap_etaBin4_phiBin2_c0", m_me_etaBin4_phiBin2_c0 = 0.003);
  declareProperty("MiddleEndcap_etaBin4_phiBin2_c1", m_me_etaBin4_phiBin2_c1 = 178.9);
  declareProperty("MiddleEndcap_etaBin4_phiBin2_c2", m_me_etaBin4_phiBin2_c2 = 1290924.);

  declareProperty("MiddleEndcap_etaBin4_phiBin3_c0", m_me_etaBin4_phiBin3_c0 = 0.002);
  declareProperty("MiddleEndcap_etaBin4_phiBin3_c1", m_me_etaBin4_phiBin3_c1 = 86.2);
  declareProperty("MiddleEndcap_etaBin4_phiBin3_c2", m_me_etaBin4_phiBin3_c2 = 1668021.3);

  //etaBin5 : fabs(etaId)<=1.2
  declareProperty("MiddleEndcap_etaBin5_phiBin1_c0", m_me_etaBin5_phiBin1_c0 = 0.0032);
  declareProperty("MiddleEndcap_etaBin5_phiBin1_c1", m_me_etaBin5_phiBin1_c1 = 269.9);
  declareProperty("MiddleEndcap_etaBin5_phiBin1_c2", m_me_etaBin5_phiBin1_c2 = 2258893.9);

  declareProperty("MiddleEndcap_etaBin5_phiBin2_c0", m_me_etaBin5_phiBin2_c0 = 0.005);
  declareProperty("MiddleEndcap_etaBin5_phiBin2_c1", m_me_etaBin5_phiBin2_c1 = 173.4);
  declareProperty("MiddleEndcap_etaBin5_phiBin2_c2", m_me_etaBin5_phiBin2_c2 = 2848828.3);

  declareProperty("MiddleEndcap_etaBin5_phiBin3_c0", m_me_etaBin5_phiBin3_c0 = 0.0048);
  declareProperty("MiddleEndcap_etaBin5_phiBin3_c1", m_me_etaBin5_phiBin3_c1 = 153.1);
  declareProperty("MiddleEndcap_etaBin5_phiBin3_c2", m_me_etaBin5_phiBin3_c2 = 3344228.2);

  //unbinned constants for phi extrapolation
  declareProperty("MiddleEndcap_c0", m_me_c0 = 0.00066);
  declareProperty("MiddleEndcap_c1", m_me_c1 = 179.6);

  // hit matching cut-cone constants
  declareProperty("Cone_Endcap_mdt_inner_dEta_c0",  m_cone_Endcap_mdt_inner_dEta_c0   = 44.638 );
  declareProperty("Cone_Endcap_mdt_inner_dEta_c1",  m_cone_Endcap_mdt_inner_dEta_c1   = 0.9109 );

  declareProperty("Cone_Endcap_mdt_middle_dEta_c0", m_cone_Endcap_mdt_middle_dEta_c0  = 131.57 );
  declareProperty("Cone_Endcap_mdt_middle_dEta_c1", m_cone_Endcap_mdt_middle_dEta_c1  = 0.978 );

  declareProperty("Cone_Barrel_mdt_inner_dEta_c0",  m_cone_Barrel_mdt_inner_dEta_c0   = 44.638 );
  declareProperty("Cone_Barrel_mdt_inner_dEta_c1",  m_cone_Barrel_mdt_inner_dEta_c1   = 0.9109 );

  declareProperty("Cone_Barrel_mdt_middle_dEta_c0", m_cone_Barrel_mdt_middle_dEta_c0  = 42.654 );
  declareProperty("Cone_Barrel_mdt_middle_dEta_c1", m_cone_Barrel_mdt_middle_dEta_c1  = 0.8662 );

  declareProperty("Cone_rpc_middle_dEta_c0",        m_cone_rpc_middle_dEta_c0         = 42.654 );
  declareProperty("Cone_rpc_middle_dEta_c1",        m_cone_rpc_middle_dEta_c1         = 0.8662 );

  declareProperty("Cone_rpc_middle_dPhi_c0",        m_cone_rpc_middle_dPhi_c0         = 241.76 );
  declareProperty("Cone_rpc_middle_dPhi_c1",        m_cone_rpc_middle_dPhi_c1         = 1.038 );

  declareProperty("Cone_tgc_wire_inner_dEta_c0",    m_cone_tgc_wire_inner_dEta_c0     = 9.6 );
  declareProperty("Cone_tgc_wire_inner_dEta_c1",    m_cone_tgc_wire_inner_dEta_c1     = 0.7472 );

  declareProperty("Cone_tgc_wire_middle_dEta_c0",   m_cone_tgc_wire_middle_dEta_c0    = 131.57 );
  declareProperty("Cone_tgc_wire_middle_dEta_c1",   m_cone_tgc_wire_middle_dEta_c1    = 0.978 );

  declareProperty("Cone_tgc_strip_inner_dPhi_c0",   m_cone_tgc_strip_inner_dPhi_c0    = 24.11 );
  declareProperty("Cone_tgc_strip_inner_dPhi_c1",   m_cone_tgc_strip_inner_dPhi_c1    = 0.85588 );

  declareProperty("Cone_tgc_strip_middle_dPhi_c0",  m_cone_tgc_strip_middle_dPhi_c0   = 1264.3 );
  declareProperty("Cone_tgc_strip_middle_dPhi_c1",  m_cone_tgc_strip_middle_dPhi_c1   = 1.1423 );
  declareProperty("Cone_tgc_strip_middle2_dPhi_c0", m_cone_tgc_strip_middle2_dPhi_c0  = 82.017 );
  declareProperty("Cone_tgc_strip_middle2_dPhi_c1", m_cone_tgc_strip_middle2_dPhi_c1  = 0.9231 );


}

StatusCode TrigDiMuonExtrapolator::initialize()
{
  StatusCode sc = AlgTool::initialize();
  return sc;

}

StatusCode TrigDiMuonExtrapolator::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;

}

void TrigDiMuonExtrapolator::propagationInnerBarrel(Candidate& t)
{
  MsgStream log(msgSvc(), name());

  double ptId = t.Pt();
  double etaId = t.Eta();
  double phiId = t.Phi();
  int charge = t.Charge();
    //if (m_tilted_B_field) charge = -charge;

  double eta_ext_inner = etaId;
  double phi_ext_inner = phiId - charge*(m_ib_c0 + (m_ib_c1/ptId));

  t.setEtaExtrapolatedInnerBarrel(eta_ext_inner);
  t.setPhiExtrapolatedInnerBarrel(phi_ext_inner);

  log << MSG::DEBUG << " Eta extrapolated Inner Barrel = " << t.EtaExtrapolatedInnerBarrel()
      << " Phi extrapolated Inner Barrel = " << t.PhiExtrapolatedInnerBarrel() << endreq;

  return;
}

void TrigDiMuonExtrapolator::propagationInnerEndCap(Candidate& t)
{
  MsgStream log(msgSvc(), name());

  double ptId = t.Pt();
  double etaId = t.Eta();
  double phiId = t.Phi();
  int charge = t.Charge();

  double eta_ext_inner = etaId;
  double phi_ext_inner = phiId;

  if (fabs(etaId)>1.6)
  {
    phi_ext_inner = phiId - charge*(m_ie_etaBin1_c0 + (m_ie_etaBin1_c1 /ptId));
  }
  if (fabs(etaId)>1.3 && fabs(etaId)<=1.6)
  {
    phi_ext_inner = phiId - charge*(m_ie_etaBin2_c0 + (m_ie_etaBin2_c1 /ptId));
  }
  if (fabs(etaId)<=1.3)
  {
    phi_ext_inner = phiId - charge*(m_ie_etaBin3_c0 + (m_ie_etaBin3_c1 /ptId));
  }

  t.setEtaExtrapolatedInnerEndCap(eta_ext_inner);
  t.setPhiExtrapolatedInnerEndCap(phi_ext_inner);

  log << MSG::DEBUG << " Eta extrapolated Inner EndCap = " << t.EtaExtrapolatedInnerEndCap()
      << " Phi extrapolated Inner EndCap = " << t.PhiExtrapolatedInnerEndCap() << endreq;

  return;
}

void TrigDiMuonExtrapolator::propagationMiddleBarrel(Candidate& t)
{
  MsgStream log(msgSvc(), name());

  double ptId = t.Pt();
  double etaId = t.Eta();
  double ptId2 = t.Pt2();
  int charge = t.Charge();
  if (m_tilted_B_field) charge = -charge;

  double eta_ext_middle = etaId - charge*(m_mb_c0 + ( m_mb_c1 /ptId)+( m_mb_c2 /ptId2));
  double phi_ext_middle = t.PhiExtrapolatedInnerBarrel();

  t.setEtaExtrapolatedMiddleBarrel(eta_ext_middle);
  t.setPhiExtrapolatedMiddleBarrel(phi_ext_middle);

  log << MSG::DEBUG << " Eta extrapolated RPC Middle = " << t.EtaExtrapolatedMiddleBarrel()
      << " Phi extrapolated RPC Middle = " << t.PhiExtrapolatedMiddleBarrel() << endreq;

  return;
}


void TrigDiMuonExtrapolator::propagationMiddleEndCap(Candidate& t)
{
  MsgStream log(msgSvc(), name());

  double ptId = t.Pt();
  double etaId = t.Eta();
  double phiId = t.Phi();
  double ptId2 = t.Pt2();
  int charge = t.Charge();
  if (m_tilted_B_field) charge = -charge;

  double eta_ext_middle = etaId;
  double phi_ext_middle = phiId;
    //double eta_ext_inner = t.EtaExtrapolatedInnerEndCap();
  double phi_ext_inner = t.PhiExtrapolatedInnerEndCap();

    //propagation to Middle stations

  int Octant = static_cast<int> (phi_ext_inner/PI_OVER_4);
  double PhiInOctant = fabs(phi_ext_inner-Octant*PI_OVER_4);
  if (PhiInOctant > PI_OVER_8) PhiInOctant = PI_OVER_4-PhiInOctant;
  int phiBin = static_cast<int>(PhiInOctant*30.557749073643904467625682567523);

    // propagation in eta

  if (fabs(etaId)>2.)
  {
    if (phiBin<=4)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin1_phiBin1_c0 + (m_me_etaBin1_phiBin1_c1 / ptId)+( m_me_etaBin1_phiBin1_c2 /ptId2));
    }
    if (phiBin>4 && phiBin<8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin1_phiBin2_c0 + (m_me_etaBin1_phiBin2_c1 / ptId)+( m_me_etaBin1_phiBin2_c2 /ptId2));
    }
    if (phiBin>=8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin1_phiBin3_c0 + (m_me_etaBin1_phiBin3_c1 / ptId)+( m_me_etaBin1_phiBin3_c2 /ptId2));
    }
  }
  if (fabs(etaId)>1.65 && fabs(etaId)<=2.)
  {
    if (phiBin<=4)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin2_phiBin1_c0 + (m_me_etaBin2_phiBin1_c1 / ptId)+( m_me_etaBin2_phiBin1_c2 /ptId2));
    }
    if (phiBin>4 && phiBin<8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin2_phiBin2_c0 + (m_me_etaBin2_phiBin2_c1 / ptId)+( m_me_etaBin2_phiBin2_c2 /ptId2));
    }
    if (phiBin>=8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin2_phiBin3_c0 + (m_me_etaBin2_phiBin3_c1 / ptId)+( m_me_etaBin2_phiBin3_c2 /ptId2));
    }
  }
  if (fabs(etaId)>1.35 && fabs(etaId)<=1.65)
  {
    if (phiBin<=4)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin3_phiBin1_c0 + (m_me_etaBin3_phiBin1_c1 / ptId)+( m_me_etaBin3_phiBin1_c2 /ptId2));
    }
    if (phiBin>4 && phiBin<8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin3_phiBin2_c0 + (m_me_etaBin3_phiBin2_c1 / ptId)+( m_me_etaBin3_phiBin2_c2 /ptId2));
    }
    if (phiBin>=8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin3_phiBin3_c0 + (m_me_etaBin3_phiBin3_c1 / ptId)+( m_me_etaBin3_phiBin3_c2 /ptId2));
    }

  }
  if (fabs(etaId)>1.2 && fabs(etaId)<=1.35)
  {
    if (phiBin<=4)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin4_phiBin1_c0 + (m_me_etaBin4_phiBin1_c1 / ptId)+( m_me_etaBin4_phiBin1_c2 /ptId2));
    }
    if (phiBin>4 && phiBin<8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin4_phiBin2_c0 + (m_me_etaBin4_phiBin2_c1 / ptId)+( m_me_etaBin4_phiBin2_c2 /ptId2));
    }
    if (phiBin>=8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin4_phiBin3_c0 + (m_me_etaBin4_phiBin3_c1 / ptId)+( m_me_etaBin4_phiBin3_c2 /ptId2));
    }
  }
  if (fabs(etaId)<=1.2)
  {
    if (phiBin<=4)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin5_phiBin1_c0 + (m_me_etaBin5_phiBin1_c1 / ptId)+( m_me_etaBin5_phiBin1_c2 /ptId2));
    }
    if (phiBin>4 && phiBin<8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin5_phiBin2_c0 + (m_me_etaBin5_phiBin2_c1 / ptId)+( m_me_etaBin5_phiBin2_c2 /ptId2));
    }
    if (phiBin>=8)
    {
      eta_ext_middle = etaId - charge*(m_me_etaBin5_phiBin3_c0 + (m_me_etaBin5_phiBin3_c1 / ptId)+( m_me_etaBin5_phiBin3_c2 /ptId2));
    }
  }

    // propagation in phi

  phi_ext_middle = phi_ext_inner;

  if (fabs(eta_ext_middle)>=2.)
  {
    phi_ext_middle = phiId - charge*(m_me_c0 + ( m_me_c1 /ptId));
  }

  t.setEtaExtrapolatedMiddleEndCap(eta_ext_middle);
  t.setPhiExtrapolatedMiddleEndCap(phi_ext_middle);

  log << MSG::DEBUG << " Eta extrapolated TGC Middle = " << t.EtaExtrapolatedMiddleEndCap()
      << " Phi extrapolated TGC Middle = " << t.PhiExtrapolatedMiddleEndCap() << endreq;

  return;

}
///===================================================
bool TrigDiMuonExtrapolator::matchMdtInner(double r, double z, Candidate& t)
{
  double ptId  = t.Pt();
  double etaId = t.Eta();
  double eta = t.EtaExtrapolatedInnerEndCap();//   propagationInnerEndCap(ptId, etaId, phiId, charge).X();

  double window_mdt_eta = m_MdtEtaFactor*m_cone_Endcap_mdt_inner_dEta_c0/pow(ptId,m_cone_Endcap_mdt_inner_dEta_c1);
  if(fabs(etaId) < 1.) {
    eta = t.EtaExtrapolatedInnerBarrel();   //propagationInnerBarrel(ptId, etaId, phiId, charge).X();
    window_mdt_eta = m_MdtEtaFactor*m_cone_Barrel_mdt_inner_dEta_c0/pow(ptId,m_cone_Barrel_mdt_inner_dEta_c1);
  }

  double eta_hit = getEta(r,z);

  return fabs(eta_hit - eta) < window_mdt_eta;

}
///===================================================
bool TrigDiMuonExtrapolator::matchMdtMiddle(double r, double z, Candidate& t)
{
  double ptId = t.Pt();
  double etaId = t.Eta();

  double eta = t.EtaExtrapolatedMiddleEndCap();     //propagationMiddleEndCap(ptId, etaId, phiId, charge).X();

  double window_mdt_eta_m = m_MdtEtaFactor*m_cone_Endcap_mdt_middle_dEta_c0/pow(ptId,m_cone_Endcap_mdt_middle_dEta_c1);
  if(fabs(etaId) < 1.) {
    eta = t.EtaExtrapolatedMiddleBarrel();        //propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
    window_mdt_eta_m = m_MdtEtaFactor*m_cone_Barrel_mdt_middle_dEta_c0/pow(ptId,m_cone_Barrel_mdt_middle_dEta_c1);
  }

  double eta_hit = getEta(r,z);

  return fabs(eta_hit - eta) < window_mdt_eta_m;
}
///===================================================
bool TrigDiMuonExtrapolator::matchRpcEtaPad(double eta_hit, Candidate& t)
{
  double ptId = t.Pt();
  double eta = t.EtaExtrapolatedMiddleBarrel();       //propagationMiddleBarrel(ptId, etaId, phiId, charge).X();
  double window_rpc_eta = m_RpcEtaFactor*m_cone_rpc_middle_dEta_c0/pow(ptId,m_cone_rpc_middle_dEta_c1);

  return fabs(eta_hit - eta) < window_rpc_eta;

}
///===================================================
bool TrigDiMuonExtrapolator::matchRpcPhiPad(double phi_hit, Candidate& t)
{
  double ptId = t.Pt();
  double phi = t.PhiExtrapolatedMiddleBarrel();       //propagationMiddleBarrel(ptId, etaId, phiId, charge).Y();
  double window_rpc_phi = m_RpcPhiFactor*m_cone_rpc_middle_dPhi_c0/pow(ptId,m_cone_rpc_middle_dPhi_c1);

  return fabs(dAngle(phi_hit, phi)) < window_rpc_phi;
}
///===================================================
bool TrigDiMuonExtrapolator::matchInnerTgcPhiHit(double phi_hit, Candidate& t)
{
  double ptId = t.Pt();
  double phi = t.PhiExtrapolatedInnerEndCap();         //propagationInnerEndCap(ptId, etaId, phiId, charge).Y();

  double window_phi_tgc_inner = m_TgcPhiFactor*0.005;
  if (ptId<20000.) window_phi_tgc_inner = m_TgcPhiFactor*m_cone_tgc_strip_inner_dPhi_c0/pow(ptId,m_cone_tgc_strip_inner_dPhi_c1);

  return fabs(dAngle(phi_hit, phi)) < window_phi_tgc_inner;

}
///===================================================
bool TrigDiMuonExtrapolator::matchInnerTgcRhoHit(double eta_hit, double phi_hit, Candidate& t)
{
  double ptId = t.Pt();
  double eta = t.EtaExtrapolatedInnerEndCap();       //propagationInnerEndCap(ptId, etaId, phiId, charge).X();
  double phi = t.PhiExtrapolatedInnerEndCap();       //propagationInnerEndCap(ptId, etaId, phiId, charge).Y();

  double window_eta_tgc_inner = m_TgcEtaFactor*0.01;
  if (ptId<10000.) window_eta_tgc_inner = m_TgcEtaFactor*m_cone_tgc_wire_inner_dEta_c0/pow(ptId,m_cone_tgc_wire_inner_dEta_c1);

  return fabs(eta_hit - eta) < window_eta_tgc_inner && fabs(dAngle(phi_hit, phi)) < 0.2;

}
///===================================================
bool TrigDiMuonExtrapolator::matchMiddleTgcPhiHit(double phi_hit, Candidate& t)
{
  double ptId = t.Pt();
  double eta = t.EtaExtrapolatedMiddleEndCap();        //propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
  double phi = t.PhiExtrapolatedMiddleEndCap();     //propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();

  double window_phi_tgc_middle = m_TgcPhiFactor*m_cone_tgc_strip_middle_dPhi_c0/pow(ptId,m_cone_tgc_strip_middle_dPhi_c1);
  if (fabs(eta)>1.7) window_phi_tgc_middle = m_TgcPhiFactor*m_cone_tgc_strip_middle2_dPhi_c0/pow(ptId,m_cone_tgc_strip_middle2_dPhi_c1);

  return fabs(dAngle(phi_hit, phi)) < window_phi_tgc_middle;

}
///===================================================
bool TrigDiMuonExtrapolator::matchMiddleTgcRhoHit(double eta_hit, double phi_hit, Candidate& t)
{
  double ptId = t.Pt();
  double eta = t.EtaExtrapolatedMiddleEndCap();     //propagationMiddleEndCap(ptId, etaId, phiId, charge).X();
  double phi = t.PhiExtrapolatedMiddleEndCap();       //propagationMiddleEndCap(ptId, etaId, phiId, charge).Y();

  double window_eta_tgc_middle = m_TgcEtaFactor*m_cone_tgc_wire_middle_dEta_c0/pow(ptId,m_cone_tgc_wire_middle_dEta_c1);

  return fabs(eta_hit - eta) < window_eta_tgc_middle && fabs(dAngle(phi_hit, phi)) < 0.2;
}
///===================================================
/// useful functions
///===================================================
double dAngle(double phi1, double phi2) {
  double dPhi = phi1 - phi2;
  while(dPhi >  M_PI) dPhi =  2*M_PI - dPhi;
  while(dPhi < -M_PI) dPhi = -2*M_PI - dPhi;

  return dPhi;
}
///===================================================
double getEta(double r, double z) {
  double theta = atan2(r,z);
  return -log(tan(theta/2));
}
///===================================================
