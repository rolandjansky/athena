/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauEFSel.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply EF selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TRIGTAUEFSEL_H
#define _TRIGTAUEFSEL_H

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauHistogram.h"

class TrigTauRecEFObject;

// Bits defining pattern returned by TrigTauL2CalSel::cutWord()
const unsigned long EFCUT_EFIDNONZERO = 0x01;
const unsigned long EFCUT_NTRKMIN     = 0x02;
const unsigned long EFCUT_NTRKMAX     = 0x04;
const unsigned long EFCUT_EMRAD       = 0x08;
const unsigned long EFCUT_ISOFRAC     = 0x10;
const unsigned long EFCUT_EMFRAC      = 0x20;
const unsigned long EFCUT_LEADPT      = 0x40;
const unsigned long EFCUT_ETCORR      = 0x80;
const unsigned long EFCUT_EMET        = 0x100;
const unsigned long EFCUT_MASS        = 0x200;
const unsigned long EFCUT_DRTRKAVG    = 0x400;
const unsigned long EFCUT_ETOVERPTTRK = 0x800;

//
// EF trigger selector
//

class TrigTauEFSel {
 public:
  TrigTauEFSel();
  TrigTauEFSel( const TrigTauEFSel& other);
  ~TrigTauEFSel();

  void setCuts(int NTrackMin = 1,
	       int NTrackMax = 3,
	       float EMRadiusMax = 0.08,
	       float IsoFracMax = 0.31,
	       float EMFracMin = 0.,
	       float PtMaxTrackMin = 5000.,
	       float EtCalibMin = -10000.,
	       float EMEtMin = -10000.,
	       float EMRadiusMaxMultiProng = -1, // if *MultiProng <0, same value as for 1-prong is used
	       float IsoFracMaxMultiProng = -1,
	       float EMFracMinMultiProng = -1,
	       float PtMaxTrackMinMultiProng = -1,
	       float EtCalibMinMultiProng = -10000.,
	       float EMEtMinMultiProng = -10000.,
	       float MassMax = -1,
               float DrTrkAvgMax = -1,
               float DrTrkAvgMaxMultiProng = -1);
  
  class CutFunctor
  {
  public:
    CutFunctor( float am1=-999, float a0=-999, float a1=-999)
            : m_am1( am1), m_a0( a0), m_a1( a1)
          {}
    
    float eval( float pt) const
          {
            if( pt < 0)
                return -1;
            return m_am1/pt + m_a0 + m_a1*pt;
          }
  private:
    float m_am1;
    float m_a0;
    float m_a1;
  };
  
  void setParamSafeCuts( const CutFunctor& LowEMRadSP,
                         const CutFunctor& LowEMRadMP,
                         const CutFunctor& LowTrkAvgDistSP,
                         const CutFunctor& LowTrkAvgDistMP,
                         float LowEtOverPtLeadTrkSP = -1,
                         float LowEtOverPtLeadTrkMP = -1,
                         float HighEMRadSP = 0,
                         float HighEMRadMP = 0,
                         float HighTrkAvgDistSP = 0,
                         float HighTrkAvgDistMP = 0,
                         float HighEtOverPtLeadTrkSP = -1,
                         float HighEtOverPtLeadTrkMP = -1,
                         int NTrackMin = 1,
                         int NTrackMax = 3,
                         float EtCalibMinSP = -1,
                         float EtCalibMinMP = -1,
                         float ETThresh = 80000);
  
  // Use trigger decision bits instead
  void useBits(TrigTauFlags::TRIGGERMENU type) {menu=type;}

  void print() const;

  bool passed(const TrigTauRecEFObject& tau) const;

  // Return bits indicating cuts failed
  unsigned long cutWord(const TrigTauRecEFObject& tau) const;
  unsigned long cutWordParamSafeCuts(const TrigTauRecEFObject& tau) const;

  void setDebug(bool value=true) {_debug = value;}
  void setHistDebug(TrigTauHistogram::HISTTYPE type=TrigTauHistogram::PTHIST);

  TrigTauHistogram * histManager() { return m_histManager;};

 private:

  unsigned int m_NTrackMin;
  unsigned int m_NTrackMax;
  float m_EMRadiusMax;
  float m_IsoFracMax;
  float m_EMFracMin;
  float m_PtMaxTrackMin;
  float m_EtCalibMin;
  float m_EMEtMin;

  float m_EMRadiusMaxMultiProng;
  float m_IsoFracMaxMultiProng;
  float m_EMFracMinMultiProng;
  float m_PtMaxTrackMinMultiProng;
  float m_EtCalibMinMultiProng;
  float m_EMEtMinMultiProng;
  float m_MassMax;
  float m_DrTrkAvgMax;
  float m_DrTrkAvgMaxMultiProng;
  
  // pt parametrized cuts
  CutFunctor m_LowEMRadSP;
  CutFunctor m_LowEMRadMP;
  CutFunctor m_LowTrkAvgDistSP;
  CutFunctor m_LowTrkAvgDistMP;
  float m_LowEtOverPtLeadTrkSP;
  float m_LowEtOverPtLeadTrkMP;
  float m_HighEtOverPtLeadTrkSP;
  float m_HighEtOverPtLeadTrkMP;
  float m_ETThresh;
    
  bool _debug;

  // For bit-level selection
  TrigTauFlags::TRIGGERMENU menu;

  TrigTauHistogram * m_histManager;
};

#endif
