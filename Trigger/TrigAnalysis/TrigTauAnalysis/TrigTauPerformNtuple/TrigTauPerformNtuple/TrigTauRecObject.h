/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAURECOBJECT_H
#define TRIGTAURECOBJECT_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauRecObject.h
/// Package : TrigTauPerformNtuple
/// Author  : Olya Igonkina (University of Oregon)
/// Created : April 2006
///
/// DESCRIPTION:
///           copy of tauObject to be dumped in root ntuple
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TVector.h"
#include "TRefArray.h"
#include "TLorentzVector.h"
#include "TrigTauPerformNtuple/TrigTauTauObject.h"

class TrigTauRecObject : public TrigTauTauObject
{
 public :

  TrigTauRecObject();
  TrigTauRecObject(const TrigTauRecObject & other);


  virtual ~TrigTauRecObject();
  void reset();
  
  virtual int charge() const { return  m_charge;};

  bool isGood() const;
  bool isBad() const;
  bool isUgly() const;
  
  //----------- public data members

  // tauRec part

  int              m_nEMCell;
  float            m_energy;  // which energy is this one?
  float            m_EMenergy;
  float            m_stripET;


  // EM sums are computed in tauRec in tauCellInfo.cxx treating each cell
  // as a massless particle.
  TLorentzVector   m_sumEM;  

  float            m_etEMCalib; //   -- Recalibrated energy --
  float            m_etHadCalib;
  int              m_nTracksdrdR;
  int              m_charge;
  
  float            m_leadPt;
  float            m_ipSig;
  float            m_trFlightPathSig;
  

  float            m_hadRadius;
  float            m_sumHadCellEt;
  float            m_sumEmCellEt;
  float            m_centralityFraction;
  

  // tau13p

  TLorentzVector  m_sumEM_1p3p;  // sumPi0

  float           m_etEMCL;
  float           m_etNeuEM;
  float           m_etChrgEM;
  float           m_etResNeuEM;

  float           m_etChrgHAD; 

  float           m_ETcalo;    //p_details->etEMAtEMScale() + p_details->etHadAtEMScale()
  float           m_MTrk3P;
  float           m_NAssocTracksCore;
  float           m_NAssocTracksIsol;
  float           m_RWidth2Trk3P;
  float           m_SignD0Trk3P;


  float           m_etChrgEM01Trk[3];  //energy collected close to the vicinity of the track per EM sampling
  float           m_etResChrgEMTrk[3]; //subtraction term  for energy collected close to the vicinity of the track per EM sampling

  float           m_etEMAtEMScale; // calculate etHadAtEMScale as  m_ETcalo -  m_etEMAtEMScale
  float           m_etIsolEM;
  float           m_etIsolHAD;

  float           m_EMRadius_1p3p;
  int             m_nStrips_1p3p;
  float           m_stripWidth2_1p3p;
  float           m_isoFrac_1p3p;

  float           m_trFlightPathSig_1p3p;
  float           m_z0SinThetaSig;
  float           m_etIsolFrac;
  float           m_etChrgHADoverPttot;
  float           m_etEFlow;
  float           m_sumPTTracks;
  float           m_MvisEflow;

  float           m_nPi0; //
  int             m_nLooseTracksRecord; // record number of loose tracks which are used to cut on

  float           m_jetEta;
  float           m_jetPhi;
  int             m_jetn90cells;        // # cells carrying >90% energy 
  int             m_jetn90constituents; // # constituents carrying >90% energy
  float           m_jetQuality;         // jet quality
  float           m_jetTimeClusters;    // cluster timing
  float           m_jetTimeCells;       // cell timing
  float           m_jetEMf;             // EM fraction
  float           m_jetHECf;            // efrac in hadronic end cap
  float           m_jetTG3f;            // efrac in TileGap3
  float           m_jetCorf;            // efrac in dead cells (BCH_CORR_CELL)
  
  int             m_nTopoClusters;
  float           m_nEffTopoClusters;
  float           m_topoInvMass;
  float           m_effTopoInvMass;
  float           m_topoMeanDeltaR;
  float           m_effTopoMeanDeltaR;

  float           m_trkAvgDist;
  float           m_trkRmsDist;
  
  float           m_etOverPtLeadTrk;
  
  float           m_massTrkSys;

  int             m_nSeedCaloTracks; // original number from SG
  
 private:
  ClassDef(TrigTauRecObject, 13);
};

#endif //TRIGTAURECOBJECT_H
