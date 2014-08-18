/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETSUMS_H
#define JETREC_JETSUMS_H
/*****************************************************************************
Filename : JetSums.h 
Author   : Frank Paige
Created  : Sep, 2002

DESCRIPTION:

     This class holds the jet kinematics plus the partial sums of cell
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

class JetSums 
{
 public:

  // Constructor/destructor
  JetSums(double e, double et, double eta, double phi, double m);
  ~JetSums();

  // Access functions
  double jetE() const { return m_jetE; }
  double jetEt() const { return m_jetEt; }
  double jetEta() const { return m_jetEta; }
  double jetPhi() const { return m_jetPhi; }
  double jetM() const { return m_jetM; }

  double eEMBCell(const int ireg) const { return m_eEMB0Cell[ireg]; }
  double eEMBCell(const int ireg, const int ie) const { 
    return m_eEMB1Cell[ireg][ie]; }
  double eEMECell(const int ireg) const { return m_eEME0Cell[ireg]; }
  double eEMECell(const int ireg, const int ie) const { 
    return m_eEME1Cell[ireg][ie]; }
  double eTileCell(const int ireg, const int ie) const { 
    return m_eTileCell[ireg][ie]; }
  double eHecCell(const int ireg, const int ie) const { 
    return m_eHecCell[ireg][ie]; }
  double eFCalCell(const int ireg, const int ie) const { 
    return m_eFCalCell[ireg][ie]; }
  double eCryo() const { return m_eCryo; }
  double eGap() const { return m_eGap; }
  double eNull() const { return m_eNull; }

  // Set functions
  void set_eEMBCell(const int ireg, double e) { m_eEMB0Cell[ireg] = e; }
  void set_eEMBCell(const int ireg, const int ie, double e) { 
    m_eEMB1Cell[ireg][ie] = e; }
  void set_eEMECell(const int ireg, double e) { m_eEME0Cell[ireg] = e; }
  void set_eEMECell(const int ireg, const int ie, double e) { 
    m_eEME1Cell[ireg][ie] = e; }
  void set_eTileCell(const int ireg, const int ie, double e) { 
    m_eTileCell[ireg][ie] = e; }
  void set_eHecCell(const int ireg, const int ie, double e) { 
    m_eHecCell[ireg][ie] = e; }
  void set_eFCalCell(const int ireg, const int ie, double e) { 
    m_eFCalCell[ireg][ie] = e; }
  void set_eCryo(double e) { m_eCryo = e; }
  void set_eGap(double e) { m_eGap = e; }
  void set_eNull(double e) { m_eNull = e; }

 private:

  // Jet Kinematics
  double m_jetE;
  double m_jetEt;
  double m_jetEta;
  double m_jetPhi;
  double m_jetM;

  // Cell information
  static const int m_dim = 20;
  double m_eEMB0Cell[2];
  double m_eEMB1Cell[2][m_dim];
  double m_eEME0Cell[2];
  double m_eEME1Cell[2][m_dim];
  double m_eTileCell[2][m_dim];
  double m_eHecCell[2][m_dim];
  double m_eFCalCell[2][m_dim];
  double m_eCryo;
  double m_eGap;
  double m_eNull;

};
#endif //JETREC_COMBINEDJET_H

