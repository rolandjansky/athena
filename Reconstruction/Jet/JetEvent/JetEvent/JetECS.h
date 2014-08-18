/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETECS_H
#define JETREC_JETECS_H
/*****************************************************************************
Filename : JetECS.h 
Author   : Iacopo Vivarelli
Created  : Jul,  2004

DESCRIPTION:

     This class holds a CombinedJet* plus the partial sums of cell
energies in layers and E/volume ranges, information needed for an
H1-style energy/volume calibration:

     eEMBCell(ireg)             EM barrel, all energies
                                ireg=0     presampler
                                ireg=1     strips
     eEMBCell(ireg,ie)          EM barrel, binned in energy
                                ireg=0     eta<0.8
                                ireg=1     eta>0.8

     eEMECell(ireg)             EM endcap, all energies
                                ireg=0     presampler
                                ireg=1     strips
     eEMECell(ireg,ie)          EM endcap, binned in energy
                                ireg=0     eta<2.5
                                ireg=1     eta>2.5

     eTileCell(ireg,ie)         Tile, binned in energy
                                ireg=0     barrel
                                ireg=1     extended barrel

     eHecCell(ireg,ie)          HEC, binned in energy
                                ireg=0     eta<2.5
                                ireg=1     eta>2.5

     eFCalCell(ireg,ie)         FCAL, binned in energy
                                ireg=0     EM section
                                ireg=1     Had sections

     eCryo                      Cryo correction for cluster
     eGap                       Gap E for cluster
     eNull                      Cells with zero volume

The energy index ie is given by
     ie = log(abs((E/MeV)/(V/mm^3)))/log(2) + 26, 0<=i<16
The region index is not very intuitive but lets us change the regions
easily.

June 2004: Modified from CombinedJetSums to use Jet and new Navigation.
We cannot save the parent Jet* because the JetCollection gets copied by
JetAlgorithm. Hence just save the kinematics. Thanks, Hong.

*****************************************************************************/


class JetECS
{
 public:

  // Constructor/destructor
  JetECS(double e, double eta, double phi, double m);
  ~JetECS();

  // Access functions
  double jetRawE() const { return m_jetRawE; }
  double jetEta() const { return m_jetEta; }
  double jetPhi() const { return m_jetPhi; }
  double jetM() const { return m_jetM; }
  double jetCalE() const { return m_jetCalE; }
  double ePreSamBCell(const int ireg) const { return m_ePreSamBCell[ireg];}
  double ePreSamECell(const int ireg) const { return m_ePreSamECell[ireg];}
  double eEMB1Cell(const int ireg) const { return m_eEMB1Cell[ireg]; }
  double eEMB2Cell(const int ireg) const { return m_eEMB2Cell[ireg]; }
  double eEMB3Cell(const int ireg) const { return m_eEMB3Cell[ireg]; }
  double eEME1Cell(const int ireg) const { return m_eEME1Cell[ireg]; }
  double eEME2Cell(const int ireg) const { return m_eEME2Cell[ireg]; }
  double eEME3Cell(const int ireg) const { return m_eEME3Cell[ireg]; }
  double eTileBar0Cell(const int ireg) const { return m_eTileBar0Cell[ireg]; }
  double eTileBar1Cell(const int ireg) const { return m_eTileBar1Cell[ireg]; }
  double eTileBar2Cell(const int ireg) const { return m_eTileBar2Cell[ireg]; }
  double eTileExt0Cell(const int ireg) const { return m_eTileExt0Cell[ireg]; }
  double eTileExt1Cell(const int ireg) const { return m_eTileExt1Cell[ireg]; }
  double eTileExt2Cell(const int ireg) const { return m_eTileExt2Cell[ireg]; }
  double eHec0Cell(const int ireg) const { return m_eHec0Cell[ireg]; }
  double eHec1Cell(const int ireg) const { return m_eHec1Cell[ireg]; }
  double eHec2Cell(const int ireg) const { return m_eHec2Cell[ireg]; }
  double eHec3Cell(const int ireg) const { return m_eHec3Cell[ireg]; }
  double eFCal0Cell(const int ireg) const {return m_eFCal0Cell[ireg]; }
  double eFCal1Cell(const int ireg) const {return m_eFCal1Cell[ireg]; }
  double eFCal2Cell(const int ireg) const {return m_eFCal2Cell[ireg]; }
  double eCryo() const { return m_eCryo; }
  double eGap() const { return m_eGap; }
  double eNull() const { return m_eNull; }

  // Set functions
  void set_ePreSamBCell(const int ireg, double e) { m_ePreSamBCell[ireg] = e; }
  void set_ePreSamECell(const int ireg, double e) { m_ePreSamECell[ireg] = e; }
  void set_eEMB1Cell(const int ireg, double e) { m_eEMB1Cell[ireg] = e; }
  void set_eEMB2Cell(const int ireg, double e) { m_eEMB2Cell[ireg] = e; }
  void set_eEMB3Cell(const int ireg, double e) { m_eEMB3Cell[ireg] = e; }
  void set_eEME1Cell(const int ireg, double e) { m_eEME1Cell[ireg] = e; }
  void set_eEME2Cell(const int ireg, double e) { m_eEME2Cell[ireg] = e; }
  void set_eEME3Cell(const int ireg, double e) { m_eEME3Cell[ireg] = e; }
  void set_eTileBar0Cell(const int ireg, double e) { m_eTileBar0Cell[ireg]=e; }
  void set_eTileBar1Cell(const int ireg, double e) { m_eTileBar1Cell[ireg]=e; }
  void set_eTileBar2Cell(const int ireg, double e) { m_eTileBar2Cell[ireg]=e; }
  void set_eTileExt0Cell(const int ireg, double e) { m_eTileExt0Cell[ireg]=e; }
  void set_eTileExt1Cell(const int ireg, double e) { m_eTileExt1Cell[ireg]=e; }
  void set_eTileExt2Cell(const int ireg, double e) { m_eTileExt2Cell[ireg]=e; }
  void set_eHec0Cell(const int ireg, double e) { m_eHec0Cell[ireg] = e; }
  void set_eHec1Cell(const int ireg, double e) { m_eHec1Cell[ireg] = e; }
  void set_eHec2Cell(const int ireg, double e) { m_eHec2Cell[ireg] = e; }
  void set_eHec3Cell(const int ireg, double e) { m_eHec3Cell[ireg] = e; }
  void set_eFCal0Cell(const int ireg, double e) {m_eFCal0Cell[ireg] = e; }
  void set_eFCal1Cell(const int ireg, double e) {m_eFCal1Cell[ireg] = e; }
  void set_eFCal2Cell(const int ireg, double e) {m_eFCal2Cell[ireg] = e; }
  void set_eCryo(double e) { m_eCryo = e; }
  void set_eGap(double e) { m_eGap = e; }
  void set_eNull(double e) { m_eNull = e; }
  void set_e(double e) {m_jetRawE = e;}
  void set_CalibratedEnergy(double e){m_jetCalE = e;}
 private:

  // Jet Kinematics
  double m_jetRawE;
  double m_jetEta;
  double m_jetPhi;
  double m_jetM;
  double m_jetCalE;

  // Cell information
  static const int m_dim = 17;
  double m_ePreSamBCell[m_dim];
  double m_ePreSamECell[m_dim];
  double m_eEMB1Cell[m_dim];
  double m_eEMB2Cell[m_dim];
  double m_eEMB3Cell[m_dim];
  double m_eEME1Cell[m_dim];
  double m_eEME2Cell[m_dim];
  double m_eEME3Cell[m_dim];
  double m_eTileBar0Cell[m_dim];
  double m_eTileBar1Cell[m_dim];
  double m_eTileBar2Cell[m_dim];
  double m_eTileExt0Cell[m_dim];
  double m_eTileExt1Cell[m_dim];
  double m_eTileExt2Cell[m_dim];
  double m_eHec0Cell[m_dim];
  double m_eHec1Cell[m_dim];
  double m_eHec2Cell[m_dim];
  double m_eHec3Cell[m_dim];
  double m_eFCal0Cell[m_dim];
  double m_eFCal1Cell[m_dim];
  double m_eFCal2Cell[m_dim];
  double m_eCryo;
  double m_eGap;
  double m_eNull;

};
#endif //JETREC_COMBINEDJET_H

