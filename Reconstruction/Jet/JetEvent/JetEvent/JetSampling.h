/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETSAMPLING_H
#define JETEVENT_JETSAMPLING_H
/* ****************************************************************************** 
 *    @file JetSampling.h
 *    @brief Definition of JetSampling class
 *   
 *    JetSampling is a simple data object that holds information used by different jet 
 * weighting schemes. It is difficult to loop over large number of jet objects, 
 * back navigate for calorimeter and other sub system information in one job in 
 * order to build the function that can be minimized to derieve jet weights. In 
 * order to solve that, JetSampling  object is created with the relevant 
 * information for a particular weighting scheme and saved as pool object. It 
 * can then be read back and looped over fairly quickly.  
 * 
 * The interface here is not absolute and duplicates the interfaces used by the 
 * three jet weighting schemes in ATLAS. Therefore there is redundant information 
 * here. It should be tailored to the need of a weighting scheme.
 *
 * Oct. 2006: Expand information for H1:
 * (1) Separate gap into eGap [TileGap1] and eScint [TileGap2,3]. Need
 * to change JetClassifier but not JetSamplingCalibAlg.
 *
 * Dic. 2008: 
 * (1) Finer longitudinal granularity for JetECS
 * (2) Remove members of JetSum, modify methods to calculate them from JetECS
 * (3) Add distance to 1st and 2nd Nearest-Truth-Jet
 * (4) Radial profiles from: Calo Cells and Tracks
 * (5) Adding Doxygen comments
 * 
 * @author A.Gupta
 * @author B.Salvachua
 *
 * ****************************************************************************** */

#include "CLHEP/Vector/LorentzVector.h"

#include <boost/array.hpp>

class JetSampling 
{
 public:
  
  JetSampling();  //!< JetSampling constructor
  virtual ~JetSampling(); //!< JetSampling destructor
  
  const CLHEP::HepLorentzVector& hlv_rec() const { return m_pr; }     //!< Uncalibrated jet at EM scale
  const CLHEP::HepLorentzVector& hlv_pic() const { return m_pt; }     //!< PIC: Particle-In-Cone Jet  (Truth/Reference jet)
  const CLHEP::HepLorentzVector& hlv_ntj() const { return m_pn; }     //!< NTJ: Nearest-Truth-Jet (Truth/Reference jet)
  const CLHEP::HepLorentzVector& hlv_ref() const { return m_pd; }     //!< Data-driven jets kinematics, Reference jet
  
  const CLHEP::HepLorentzVector& hlv_h1() const { return m_h1; }      //!< Calibrated Jets H1 (could be filled with zeros)
  const CLHEP::HepLorentzVector& hlv_pisa() const { return m_pisa; }  //!< Calibrated Jets PISA (could be filled with zeros)
  const CLHEP::HepLorentzVector& hlv_samp() const { return m_samp; }  //!< Calibrated Jets Sampling/Layer (could be filled with zeros)
  
  void set_hlv_rec( const CLHEP::HepLorentzVector& v ) { m_pr = v; } //!< Set 4-momentum for uncalibrated jet
  void set_hlv_pic( const CLHEP::HepLorentzVector& v ) { m_pt = v; } //!< Set 4-momentum for Particle-In-Cone jet
  void set_hlv_ntj( const CLHEP::HepLorentzVector& v ) { m_pn = v; } //!< Set 4-momentum for Nearest-Truth-Jet
  void set_hlv_ref( const CLHEP::HepLorentzVector& v ) { m_pd = v; } //!< Set 4-momentum for data-driven jets kinematics
  
  void set_hlv_h1(   const CLHEP::HepLorentzVector& v ) { m_h1 = v; } //!< Set 4-momentum for Jets H1
  void set_hlv_pisa( const CLHEP::HepLorentzVector& v ) { m_pisa = v; } //!< Set 4-momentum for Jets PISA
  void set_hlv_samp( const CLHEP::HepLorentzVector& v ) { m_samp = v; } //!< Set 4-momentum for Jets Sampling/Layer

  double dR1() { return m_dr1;} //!< Distance (eta,phi) from reconstructed jet to closest NTJ. -1 implies jet does not exist 
  double dR2() { return m_dr2;} //!< Distantce (eta,phi) from reconstructed jet to 2nd closest NTJ. -1 implies jet does not exist
  void set_dR1( double r ) {m_dr1 = r;} //!< Set dR1 value
  void set_dR2( double r ) {m_dr2 = r;} //!< Set dR2 value 
  
  /////////////////////////////////////////////////////////////////
  // Jet Layers : Used by sampling (or layer)  weighting scheme  //
  /////////////////////////////////////////////////////////////////
  
  // Methods to access the values of the layers stored in JetSampling class
  double ejsPreSamplerB() const { return m_ejsPreSamplerB; } //!> Returns jet energy in PreSamplerB
  double ejsPreSamplerE() const { return m_ejsPreSamplerE; } //!> Returns jet energy in PreSamplerE
  double ejsEMB1() const { return m_ejsEMB1; } //!> Returns jet energy in layer EMB1 
  double ejsEME1() const { return m_ejsEME1; } //!> Returns jet energy in layer EME1
  double ejsEMB2() const { return m_ejsEMB2; } //!> Returns jet energy in layer EMB2
  double ejsEME2() const { return m_ejsEME2; } //!> Returns jet energy in layer EME2
  double ejsEMB3() const { return m_ejsEMB3; } //!> Returns jet energy in layer EMB3
  double ejsEME3() const { return m_ejsEME3; } //!> Returns jet energy in layer EME3
  double ejsTileBar0() const { return m_ejsTileBar0; } //!> Returns jet energy in layer TileBar0
  double ejsTileExt0() const { return m_ejsTileExt0; } //!> Returns jet energy in layer TileExt0
  double ejsTileBar1() const { return m_ejsTileBar1; } //!> Returns jet energy in layer TileBar1
  double ejsTileExt1() const { return m_ejsTileExt1; } //!> Returns jet energy in layer TileExt1
  double ejsTileBar2() const { return m_ejsTileBar2; } //!> Returns jet energy in layer TileBar2
  double ejsTileExt2() const { return m_ejsTileExt2; } //!> Returns jet energy in layer TielExt2
  double ejsHEC0() const { return m_ejsHEC0; } //!> Returns jet energy in layer HEC0
  double ejsHEC1() const { return m_ejsHEC1; } //!> Returns jet energy in layer HEC1
  double ejsHEC2() const { return m_ejsHEC2; } //!> Returns jet energy in layer HEC2
  double ejsHEC3() const { return m_ejsHEC3; } //!> Returns jet energy in layer HEC3
  double ejsTileGap1() const { return m_ejsTileGap1; } //!> Returns jet energy in TileGap1
  double ejsTileGap2() const { return m_ejsTileGap2; } //!> Returns jet energy in TileGap2
  double ejsTileGap3() const { return m_ejsTileGap3; } //!> Returns jet energy in TileGap3
  double ejsFCAL0() const { return m_ejsFCAL0; } //!> Returns jet energy in layer FCAL0
  double ejsFCAL1() const { return m_ejsFCAL1; } //!> Returns jet energy in layer FCAL1
  double ejsFCAL2() const { return m_ejsFCAL2; } //!> Returns jet energy in layer FCAL2
  
  // Two methods to cross-check the layer values, filled using layer variables content
  double jet_had_sum() { return m_ehad; } //!< Returns the jet energy deposited in the hadronic calorimeters
  double jet_em_sum() { return m_eem; }   //!< Returns the jet energy deposited in the electromagnetic calorimeters
  
  // Methods to set the layer variables content and m_ehad and m_eem
  void set_ejsPreSamplerB(double ejs) { m_ejsPreSamplerB = ejs; m_tot += ejs;  m_eem += ejs; } //!> Sets jet energy in layer PreSamplerB
  void set_ejsPreSamplerE(double ejs) { m_ejsPreSamplerE = ejs; m_tot += ejs;  m_eem += ejs; } //!> Sets jet energy in layer PreSamplerE
  void set_ejsEMB1(double ejs) { m_ejsEMB1 = ejs; m_tot += ejs; m_eem += ejs; } //!> Sets jet energy in layer EMB1
  void set_ejsEME1(double ejs) { m_ejsEME1 = ejs; m_tot += ejs; m_eem += ejs; }//!> Sets jet energy in layer EME1
  void set_ejsEMB2(double ejs) { m_ejsEMB2 = ejs; m_tot += ejs; m_eem += ejs; }//!> Sets jet energy in layer EMB2
  void set_ejsEME2(double ejs) { m_ejsEME2 = ejs; m_tot += ejs; m_eem += ejs; }//!> Sets jet energy in layer EME2
  void set_ejsEMB3(double ejs) { m_ejsEMB3 = ejs; m_tot += ejs; m_eem += ejs; }//!> Sets jet energy in layer EMB3
  void set_ejsEME3(double ejs) { m_ejsEME3 = ejs; m_tot += ejs; m_eem += ejs; }//!> Sets jet energy in layer EME3
  void set_ejsTileBar0(double ejs) { m_ejsTileBar0 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileBar0
  void set_ejsTileExt0(double ejs) { m_ejsTileExt0 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileExt0
  void set_ejsTileBar1(double ejs) { m_ejsTileBar1 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileBar1
  void set_ejsTileExt1(double ejs) { m_ejsTileExt1 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileExt1
  void set_ejsTileBar2(double ejs) { m_ejsTileBar2 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileBar2
  void set_ejsTileExt2(double ejs) { m_ejsTileExt2 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileExt2
  void set_ejsHEC0(double ejs) { m_ejsHEC0 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer HEC0
  void set_ejsHEC1(double ejs) { m_ejsHEC1 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer HEC1
  void set_ejsHEC2(double ejs) { m_ejsHEC2 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer HEC2
  void set_ejsHEC3(double ejs) { m_ejsHEC3 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer HEC3
  void set_ejsTileGap1(double ejs) { m_ejsTileGap1 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileGap1
  void set_ejsTileGap2(double ejs) { m_ejsTileGap2 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileGap2
  void set_ejsTileGap3(double ejs) { m_ejsTileGap3 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer TileGap3
  void set_ejsFCAL0(double ejs) { m_ejsFCAL0 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer FCAL0
  void set_ejsFCAL1(double ejs) { m_ejsFCAL1 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer FCAL1
  void set_ejsFCAL2(double ejs) { m_ejsFCAL2 = ejs; m_tot += ejs; m_ehad += ejs; }//!> Sets jet energy in layer FCAL2
  
  ///////////////////////////////////////////////////////
  // JetSums Interface : As used in H1-style weighting //
  ///////////////////////////////////////////////////////
  
  // Methods to access the JetSums, the return values have been calculated using JetECS variables
  double eEMBCell(const int ireg) const;                //!> Returns the jet energy  in PreSamplerB (ireg=0) and EMB1 (ireg=1)
  double eEMBCell(const int ireg, const int ie) const;  //!> Returns the jet energy in cell energy density bin ie for EMB2 (ireg=0) and EMB3 (ireg=1)
  double eEMECell(const int ireg) const;                //!> Returns the jet energy in PreSamplerE (ireg=0) and EME1 (ireg=1)
  double eEMECell(const int ireg, const int ie) const;  //!> Returns the jet energy in cell energy density bin ie for EME2 (ireg=0) and EME3 (ireg=1)
  double eTileCell(const int ireg, const int ie) const; //!> Returns the jet energy in cell energy density bin ie for TileBar (ireg=0) and TileExt (ireg=1) calorimeter
  double eHecCell(const int ireg, const int ie) const;  //!> Returns the jet energy in cell energy density bin ie for HEC, eta<2.5 (ireg=0) and eta>=2.5 (ireg=1)
  double eFCalCell(const int ireg, const int ie) const; //!> Returns the jet energy in cell energy density bin ie for FCAL0 (ireg=0) and FCAL1+FCAL2 (ireg=1) 
  //
  //
  double eCryo() const { return m_eCryo; }   //!> Returns energy in Cryo: this is sqrt( fabs( EMB3*TileBar0 ) )
  double eGap() const { return m_eGap; }     //!> Returns energy in Gap: this is TileGap3
  double eScint() const { return m_eScint; } //!> Returns energy in Scint: this is TileGap1+TileGap2
  double eNull() const { return m_eNull; }

  // Set JetSum have been removed, not needed, substitue call to JetECS
  //
  void set_eCryo(double e) { m_eCryo = e; }   //!> Sets energy in Cryo: this is be sqrt( fabs( EMB3*TileBar0) ) 
  void set_eGap(double e) { m_eGap = e; }     //!> Sets energy in Gap: this is TileGap3
  void set_eScint(double e) { m_eScint = e; } //!> Sets energy in Scint: this is TileGap1+TileGap2
  void set_eNull(double e) { m_eNull = e; }

  //////////////////////////////////////////////////////////////////////////////////
  // JetECS Interface : As used in H1-style with finer granularity (H1-layer method)
  //////////////////////////////////////////////////////////////////////////////////

  // Access functions
  double ePreSamBCell(const int ireg) const { return m_ePreSamBCell.at(ireg);} //!> Returns jet energy in cell energy density bin ireg for PreSamplerB
  double ePreSamECell(const int ireg) const { return m_ePreSamECell.at(ireg);} //!> Returns jet energy in cell energy density bin ireg for PreSamplerE
  //
  double eEMB1Cell(const int ireg) const { return m_eEMB1Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EMB1
  double eEME1Cell(const int ireg) const { return m_eEME1Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EME1
  //
  double eEMB2Cell1(const int ireg) const { return m_eEMB2Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EMB2 eta < 0.8
  double eEMB2Cell2(const int ireg) const { return m_eEMB2Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EMB2 eta >= 0.8
  // 
  double eEMB3Cell1(const int ireg) const { return m_eEMB3Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EMB3 eta < 0.8
  double eEMB3Cell2(const int ireg) const { return m_eEMB3Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EMB3 eta >= 0.8
  //
  double eEME2Cell1(const int ireg) const { return m_eEME2Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EME2 eta < 2.5
  double eEME2Cell2(const int ireg) const { return m_eEME2Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EME2 eta >= 2.5
  //
  double eEME3Cell1(const int ireg) const { return m_eEME3Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EME3 eta < 2.5
  double eEME3Cell2(const int ireg) const { return m_eEME3Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for EME3 eta >= 2.5
  //
  double eTileBar0Cell(const int ireg) const { return m_eTileBar0Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for TileBar0
  double eTileBar1Cell(const int ireg) const { return m_eTileBar1Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for TileBar1
  double eTileBar2Cell(const int ireg) const { return m_eTileBar2Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for TileBar2
  //
  double eTileExt0Cell(const int ireg) const { return m_eTileExt0Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for TileExt0
  double eTileExt1Cell(const int ireg) const { return m_eTileExt1Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for TileExt1
  double eTileExt2Cell(const int ireg) const { return m_eTileExt2Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for TileExt2
  //
  double eHec0Cell1(const int ireg) const { return m_eHec0Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC0 eta < 2.5
  double eHec1Cell1(const int ireg) const { return m_eHec1Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC1 eta < 2.5
  double eHec2Cell1(const int ireg) const { return m_eHec2Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC2 eta < 2.5
  double eHec3Cell1(const int ireg) const { return m_eHec3Cell1.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC3 eta < 2.5
  //
  double eHec0Cell2(const int ireg) const { return m_eHec0Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC0 eta >= 2.5
  double eHec1Cell2(const int ireg) const { return m_eHec1Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC1 eta >= 2.5
  double eHec2Cell2(const int ireg) const { return m_eHec2Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC2 eta >= 2.5
  double eHec3Cell2(const int ireg) const { return m_eHec3Cell2.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for HEC3 eta >= 2.5
  //
  double eFCal0Cell(const int ireg) const {return m_eFCal0Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for FCAL0
  double eFCal1Cell(const int ireg) const {return m_eFCal1Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for FCAL1
  double eFCal2Cell(const int ireg) const {return m_eFCal2Cell.at(ireg); } //!> Returns jet energy in cell energy density bin ireg for FCAL2

  // Set functions
  void set_ePreSamBCell(const int ireg, double e) { m_ePreSamBCell.at(ireg) = e; }
  void set_ePreSamECell(const int ireg, double e) { m_ePreSamECell.at(ireg) = e; }
  // 
  void set_eEMB1Cell(const int ireg, double e) { m_eEMB1Cell.at(ireg) = e; }
  void set_eEME1Cell(const int ireg, double e) { m_eEME1Cell.at(ireg) = e; }
  //
  void set_eEMB2Cell1(const int ireg, double e) { m_eEMB2Cell1.at(ireg) = e; }
  void set_eEMB2Cell2(const int ireg, double e) { m_eEMB2Cell2.at(ireg) = e; }
  //
  void set_eEMB3Cell1(const int ireg, double e) { m_eEMB3Cell1.at(ireg) = e; }
  void set_eEMB3Cell2(const int ireg, double e) { m_eEMB3Cell2.at(ireg) = e; }
  //
  void set_eEME2Cell1(const int ireg, double e) { m_eEME2Cell1.at(ireg) = e; }
  void set_eEME2Cell2(const int ireg, double e) { m_eEME2Cell2.at(ireg) = e; }
  //
  void set_eEME3Cell1(const int ireg, double e) { m_eEME3Cell1.at(ireg) = e; }
  void set_eEME3Cell2(const int ireg, double e) { m_eEME3Cell2.at(ireg) = e; }
  //
  void set_eTileBar0Cell(const int ireg, double e) { m_eTileBar0Cell.at(ireg)=e; }
  void set_eTileBar1Cell(const int ireg, double e) { m_eTileBar1Cell.at(ireg)=e; }
  void set_eTileBar2Cell(const int ireg, double e) { m_eTileBar2Cell.at(ireg)=e; }
  void set_eTileExt0Cell(const int ireg, double e) { m_eTileExt0Cell.at(ireg)=e; }
  void set_eTileExt1Cell(const int ireg, double e) { m_eTileExt1Cell.at(ireg)=e; }
  void set_eTileExt2Cell(const int ireg, double e) { m_eTileExt2Cell.at(ireg)=e; }
  //
  void set_eHec0Cell1(const int ireg, double e) { m_eHec0Cell1.at(ireg) = e; }
  void set_eHec1Cell1(const int ireg, double e) { m_eHec1Cell1.at(ireg) = e; }
  void set_eHec2Cell1(const int ireg, double e) { m_eHec2Cell1.at(ireg) = e; }
  void set_eHec3Cell1(const int ireg, double e) { m_eHec3Cell1.at(ireg) = e; }
  //
  void set_eHec0Cell2(const int ireg, double e) { m_eHec0Cell2.at(ireg) = e; }
  void set_eHec1Cell2(const int ireg, double e) { m_eHec1Cell2.at(ireg) = e; }
  void set_eHec2Cell2(const int ireg, double e) { m_eHec2Cell2.at(ireg) = e; }
  void set_eHec3Cell2(const int ireg, double e) { m_eHec3Cell2.at(ireg) = e; }
  //

  void set_eFCal0Cell(const int ireg, double e) {m_eFCal0Cell.at(ireg) = e; }
  void set_eFCal1Cell(const int ireg, double e) {m_eFCal1Cell.at(ireg) = e; }
  void set_eFCal2Cell(const int ireg, double e) {m_eFCal2Cell.at(ireg) = e; }
 
  // Fill energy in raddi around a jet, 10 radii max. Using Calo Cells
  void set_erad(const int i, double e) {m_erad_cells.at(i) += e; } //!> Sets upt to 10 radial profiles calculated from calo cells
  double get_erad( int i ) { return m_erad_cells.at(i); } //!> Returns up to 10 radial profiles calculated from calo cells

  // Fill eneryg in radii around a jet, 10 radii max. Using tracks 
  // FIXME: fill these values inside JetClassifier, now filled with zeros
  void set_erad_tracks(const int i, double e){m_erad_tracks.at(i) += e;} //!> Sets up to 10 radial profiles calculated from tracks
  double get_erad_tracks( int i) { return m_erad_tracks.at(i); } //!> Return up to 10 radial profiles calculated from tracks  

 private:
  void init(); //>! Initializes to zero all private members

  /// Reconstructed Jet at EM scale  kinematics.
  CLHEP::HepLorentzVector m_pr;

  /// Particle-In-Cone jet kinematics.
  CLHEP::HepLorentzVector m_pt;

  /// Nearest Truth Jet kinematics.
  CLHEP::HepLorentzVector m_pn;

  /// Data driven jet kinematics, reference jet kinematics
  /// It could be a CLHEP::HepLorentzVector with the kinematics of the matched gamma 
  CLHEP::HepLorentzVector m_pd;

  // Calibrated Jet kinematics.
  CLHEP::HepLorentzVector m_h1;  //!> Kinematics of the H1 calibrated jet
  CLHEP::HepLorentzVector m_pisa;//!> Kinematics of the PISA calibrated jet
  CLHEP::HepLorentzVector m_samp;//!> Kinematics of the Sampling calbirated jet

  double m_dr1; //!> Distance between reconstructed jet and closest NTJ jet
  double m_dr2; //!> Distance between reconstructed jet and 2nd closest NTJ jet

  // Jet sampling layer info
  double m_ejsPreSamplerB;
  double m_ejsPreSamplerE;
  double m_ejsEMB1;
  double m_ejsEME1;  
  double m_ejsEMB2;
  double m_ejsEME2;
  double m_ejsEMB3;
  double m_ejsEME3;
  double m_ejsTileBar0;
  double m_ejsTileExt0;
  double m_ejsTileBar1;
  double m_ejsTileExt1;
  double m_ejsTileBar2;
  double m_ejsTileExt2;  
  double m_ejsHEC0;
  double m_ejsHEC1;
  double m_ejsHEC2;
  double m_ejsHEC3;
  double m_ejsTileGap1;
  double m_ejsTileGap2;
  double m_ejsTileGap3;
  double m_ejsFCAL0;
  double m_ejsFCAL1;
  double m_ejsFCAL2;

  double m_tot;
  double m_ctot;
  double m_ehad; //!> Jet energy deposited in hadronic calorimeters
  double m_eem; //!> Jet energy deposited in electromagnetic calorimeters

  typedef         boost::array<double,16>    double_array16;
  typedef         boost::array<double,10>    double_array10;

  double m_eCryo;
  double m_eGap;
  double m_eScint;
  double m_eNull;
  
  // JetECS: Energy in each cell energy density bin, up to 16 bins
  double_array16  m_ePreSamBCell;
  double_array16  m_ePreSamECell;
  double_array16  m_eEMB1Cell;
  double_array16  m_eEME1Cell;
  double_array16  m_eEMB2Cell1;
  double_array16  m_eEMB2Cell2;
  double_array16  m_eEMB3Cell1;
  double_array16  m_eEMB3Cell2;
  double_array16  m_eEME2Cell1;
  double_array16  m_eEME2Cell2;
  double_array16  m_eEME3Cell1;
  double_array16  m_eEME3Cell2;
  double_array16  m_eTileBar0Cell;
  double_array16  m_eTileBar1Cell;
  double_array16  m_eTileBar2Cell;
  double_array16  m_eTileExt0Cell;
  double_array16  m_eTileExt1Cell;
  double_array16  m_eTileExt2Cell;
  double_array16  m_eHec0Cell1;
  double_array16  m_eHec1Cell1;
  double_array16  m_eHec2Cell1;
  double_array16  m_eHec3Cell1;
  double_array16  m_eHec0Cell2;
  double_array16  m_eHec1Cell2;
  double_array16  m_eHec2Cell2;
  double_array16  m_eHec3Cell2;
  double_array16  m_eFCal0Cell;
  double_array16  m_eFCal1Cell;
  double_array16  m_eFCal2Cell;

  // Energy in cone radii from Calo cells
  double_array10  m_erad_cells;

  // Energy in cone radii from tracks
  double_array10 m_erad_tracks;

};
#endif //JETEVENT_JETSAMPLING_H
