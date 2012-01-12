/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL2CalSel.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Class to apply L2 selection to performance ntuple objects
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TRIGTAUL2CALSEL_H
#define _TRIGTAUL2CALSEL_H

#include "TrigTauPerformNtuple/TrigTauFlags.h"
#include "TrigTauPerformAnalysis/TrigTauHistogram.h"

class TrigTauL2Object;
class TrigTauHistogram;

// Bits defining pattern returned by TrigTauL2CalSel::cutWord()
const unsigned long L2CUT_ET      = 0x01;
const unsigned long L2CUT_ETCALIB = 0x02;
const unsigned long L2CUT_NSTRIP  = 0x04;
const unsigned long L2CUT_STRWID  = 0x08;
const unsigned long L2CUT_EMRAD   = 0x10;
const unsigned long L2CUT_ISOFRAC = 0x20;
const unsigned long L2CUT_EMWID   = 0x40;
const unsigned long L2CUT_ETNAR   = 0x80;
const unsigned long L2CUT_ETNOR   = 0x100;

//
// L2 Calo trigger selector
//

class TrigTauL2CalSel {
 public:
  TrigTauL2CalSel();
  TrigTauL2CalSel(const TrigTauL2CalSel& other);

  virtual ~TrigTauL2CalSel();

  void setCuts(float Et, 
               float EtCalib, 
               int   nStrips=0,
               float stripWidth=0.055, 
               float EMRadius=0.0127, 
               float isoFrac=0.55, 
               float EMEnWidth2=0,
	       float EtNar = 0.,
               int   nStripsMultiProng=-1,  // if <0, the same cuts as for 1prong are applied
               float stripWidthMultiProng=-1,
               float EMRadiusMultiProng=-1, 
               float isoFracMultiProng=-1, 
               float EMEnWidth2MultiProng=-1
	       );

  void setNewCuts(float RawEtNor, 
                  float EMRadius, 
                  float EMRadiusMultiProng=-1);

  // Individual cut set functions
  void setCutEt        (float et)    {cut_Et = et;}
  void setCutEtNar     (float et)    {cut_EtNar = et;}
  void setCutEtCalib   (float et)    {cut_EtCalib = et;}
  void setCutNStrips   (int n)       {cut_nStrips = n;}
  void setCutStripWidth(float width) {cut_stripWidth = width;}
  void setCutEMRadius  (float r)     {cut_EMRadius = r;}
  void setCutIsoFrac   (float frac)  {cut_IsoFrac = frac;}
  void setCutEMWidth   (float wid)   {cut_EMEnergyWidth2 = wid;}

  // Use trigger decision bits instead
  void useBits(TrigTauFlags::TRIGGERMENU type) {menu=type;}

  void print() const;

  bool passed(const TrigTauL2Object& tau) const;

  // Return bits indicating cuts failed
  unsigned long cutWord(const TrigTauL2Object& tau) const;
  unsigned long cutWordNew(const TrigTauL2Object& tau) const;

  void setDebug(bool value=true) {_debug = value;}
  void setHistDebug(TrigTauHistogram::HISTTYPE type=TrigTauHistogram::PTHIST);

  TrigTauHistogram * histManager() { return m_histManager;};

 private:
  float            cut_Et;
  float            cut_EtCalib;
  float            cut_EtNar;
  float            cut_EtNor;
 
  int              cut_nStrips;
  float            cut_stripWidth;
  float            cut_EMRadius;
  float            cut_IsoFrac;
  float            cut_EMEnergyWidth2;

  int              cut_nStripsMultiProng;
  float            cut_stripWidthMultiProng;
  float            cut_EMRadiusMultiProng;
  float            cut_IsoFracMultiProng;
  float            cut_EMEnergyWidth2MultiProng;

  bool _debug;

  // For bit-level selection
  TrigTauFlags::TRIGGERMENU menu;

  TrigTauHistogram * m_histManager;

};

#endif
