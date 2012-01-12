/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauRecObject.h"

#include <cmath>

ClassImp(TrigTauRecObject)  

TrigTauRecObject::TrigTauRecObject()
{
  reset();
}

TrigTauRecObject::TrigTauRecObject(const TrigTauRecObject & other) :
  TrigTauTauObject(other),  
  m_nEMCell       (other.m_nEMCell      ),
  m_energy        (other.m_energy       ),  // which energy is this one?
  m_EMenergy      (other.m_EMenergy     ),
  m_stripET       (other.m_stripET      ),
  m_sumEM         (other.m_sumEM        ),  
  m_etEMCalib     (other.m_etEMCalib    ),//   -- Recalibrated energy --
  m_etHadCalib    (other.m_etHadCalib   ),
  m_nTracksdrdR   (other.m_nTracksdrdR  ),
  m_charge        (other.m_charge       ),
  			 		 
  m_leadPt        (other.m_leadPt       ),
  m_ipSig        (other.m_ipSig       ),
  m_trFlightPathSig        (other.m_trFlightPathSig       ),

  m_hadRadius     (other.m_hadRadius),
  m_sumHadCellEt    (other.m_sumHadCellEt),
  m_sumEmCellEt    (other.m_sumEmCellEt),
  m_centralityFraction    (other.m_centralityFraction),

  // tau13p

  m_sumEM_1p3p   (other.m_sumEM_1p3p),

  m_etEMCL     (other.m_etEMCL),
  m_etNeuEM     (other.m_etNeuEM),
  m_etChrgEM     (other.m_etChrgEM), 
  m_etResNeuEM    (other.m_etResNeuEM),

  m_etChrgHAD      (other.m_etChrgHAD),

  m_ETcalo     (other.m_ETcalo),
  m_MTrk3P     (other.m_MTrk3P),
  m_NAssocTracksCore   (other.m_NAssocTracksCore),
  m_NAssocTracksIsol    (other.m_NAssocTracksIsol),
  m_RWidth2Trk3P     (other.m_RWidth2Trk3P),
  m_SignD0Trk3P   (other.m_SignD0Trk3P),

  m_etEMAtEMScale    (other.m_etEMAtEMScale),
  m_etIsolEM  (other.m_etIsolEM),
  m_etIsolHAD   (other.m_etIsolHAD),

  m_EMRadius_1p3p  (other.m_EMRadius_1p3p),
  m_nStrips_1p3p    (other.m_nStrips_1p3p),
  m_stripWidth2_1p3p   (other.m_stripWidth2_1p3p),
  m_isoFrac_1p3p   (other.m_isoFrac_1p3p),

  m_trFlightPathSig_1p3p   (other.m_trFlightPathSig_1p3p),
  m_z0SinThetaSig   (other.m_z0SinThetaSig),
  m_etIsolFrac  (other.m_etIsolFrac),
  m_etChrgHADoverPttot    (other.m_etChrgHADoverPttot),
  m_etEFlow    (other.m_etEFlow),
  m_sumPTTracks    (other.m_sumPTTracks),
  m_MvisEflow   (other.m_MvisEflow),
  m_nPi0 (other.m_nPi0),
  m_nLooseTracksRecord( other.m_nLooseTracksRecord),
  m_jetEta(other.m_jetEta),
  m_jetPhi(other.m_jetPhi),

  m_jetn90cells(other.m_jetn90cells),
  m_jetn90constituents(other.m_jetn90constituents),
  m_jetQuality(other.m_jetQuality),
  m_jetTimeClusters(other.m_jetTimeClusters),
  m_jetTimeCells(other.m_jetTimeCells),
  m_jetEMf(other.m_jetEMf),
  m_jetHECf(other.m_jetHECf),
  m_jetTG3f( other.m_jetTG3f),
  m_jetCorf( other.m_jetCorf),

  m_nTopoClusters(other.m_nTopoClusters),
  m_nEffTopoClusters(other.m_nEffTopoClusters),
  m_topoInvMass(other.m_topoInvMass),
  m_effTopoInvMass(other.m_effTopoInvMass),
  m_topoMeanDeltaR(other.m_topoMeanDeltaR),
  m_effTopoMeanDeltaR(other.m_effTopoMeanDeltaR),

  m_trkAvgDist( other.m_trkAvgDist),
  m_trkRmsDist( other.m_trkRmsDist),
  m_etOverPtLeadTrk( other.m_etOverPtLeadTrk),
  m_massTrkSys( other.m_massTrkSys),
  m_nSeedCaloTracks( other.m_nSeedCaloTracks)
 {

   for(unsigned int i = 0; i<3 ; ++i)
     {
       m_etChrgEM01Trk[i]  = other.m_etChrgEM01Trk[i];
       m_etResChrgEMTrk[i] = other.m_etResChrgEMTrk[i];
     }


}

TrigTauRecObject::~TrigTauRecObject()
{
}

void TrigTauRecObject::reset()
{
  TrigTauTauObject::reset();
  m_nEMCell       =0;
  m_energy         =0;
  m_EMenergy       =0;
  m_stripET        =0;
  m_sumEM.SetPxPyPzE(0,0,0,0)  ;
  m_etEMCalib      =0;
  m_etHadCalib     =0;
  m_nTracksdrdR    =0;
  m_charge         =0;
  m_leadPt=0;
  m_ipSig=0;
  m_trFlightPathSig=0;

  m_hadRadius=0;
  m_sumHadCellEt=0;
  m_sumEmCellEt =0;
  m_centralityFraction=0;

  m_sumEM_1p3p.SetPxPyPzE(0,0,0,0);
  m_etEMCL           =0;
  m_etNeuEM          =0;
  m_etChrgEM         =0;
  m_etResNeuEM       =0;
  m_etChrgHAD        =0;
  m_ETcalo           =0;
  m_MTrk3P           =0;
  m_NAssocTracksCore =0;
  m_NAssocTracksIsol =0;
  m_RWidth2Trk3P     =0;
  m_SignD0Trk3P      =0;
  
  for(unsigned int i = 0; i<3 ; ++i)
    {
      m_etChrgEM01Trk[i]  = 0;
      m_etResChrgEMTrk[i] = 0;
    }
  m_etEMAtEMScale   =0;
  m_etIsolEM        =0;
  m_etIsolHAD       =0;

  m_EMRadius_1p3p=0;
  m_nStrips_1p3p=0;
  m_stripWidth2_1p3p=0;
  m_isoFrac_1p3p=0;

  m_trFlightPathSig_1p3p =0;
  m_z0SinThetaSig =0;
  m_etIsolFrac=0;
  m_etChrgHADoverPttot=0;
  m_etEFlow =0;
  m_sumPTTracks =0;
  m_MvisEflow=0;
  m_nPi0 = 0;
  m_nLooseTracksRecord = 0;

  m_jetEta = 0;
  m_jetPhi = 0;
  m_jetn90cells = 0;
  m_jetn90constituents = 0;
  m_jetQuality = 0;
  m_jetTimeClusters = 0;
  m_jetTimeCells = 0;
  m_jetEMf = 0;
  m_jetHECf = 0;
  m_jetTG3f = 0;
  m_jetCorf = 0;
  
  m_nTopoClusters= 0;
  m_nEffTopoClusters= 0;
  m_topoInvMass= 0;
  m_effTopoInvMass= 0;
  m_topoMeanDeltaR= 0;
  m_effTopoMeanDeltaR= 0;
  
  m_trkAvgDist = 0;
  m_trkRmsDist = 0;

  m_etOverPtLeadTrk = 0;
  
  m_massTrkSys = 0;
  
  m_nSeedCaloTracks = 0;
  
  return ;
}


bool TrigTauRecObject::isGood() const
{
  return !isBad() && !isUgly();
}

bool TrigTauRecObject::isBad() const
{
  if( m_jetHECf > 0.8 && m_jetn90cells <= 5)
      return true;
  if( m_jetEMf > 0.95 && std::fabs( m_jetQuality)>0.8)
      return true;
  if( std::fabs( m_jetTimeCells) >= 50)
      return true;

  return false;
}

bool TrigTauRecObject::isUgly() const
{
  if( m_jetCorf>0.5)
      return true;
  if( m_jetTG3f > 0.5)
      return true;

  return false;
}
