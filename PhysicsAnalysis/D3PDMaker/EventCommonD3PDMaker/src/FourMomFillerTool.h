// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/FourMomFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for a four-momentum.
 */

#ifndef EVENTCOMMOND3PDMAKER_FOURMOMFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_FOURMOMFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODBase/IParticle.h"
class INavigable4Momentum;
class I4Momentum;
class TLorentzVector;
namespace CLHEP {
  class HepLorentzVector;
}


namespace D3PD {


/**
 * @brief Block filler tool for a four-momentum.
 *
 * This tool has a bunch of properties to control what gets written:
 *
 *  WriteE        - Write energy (E).                        Default: false.
 *  WriteP        - Write momentum (p).                      Default: false.
 *  WriteEt       - Write transverse energy (Et).            Default: false.
 *  WritePt       - Write transverse momentum (pt).          Default: true.
 *  WriteM        - Write mass (m).                          Default: true.
 *  WriteRapidity - Write rapidity (y).                      Default: false.
 *  WriteTanTh    - Write tan(theta) (tanth).                Default: false.
 *  WriteEtaPhi   - Write eta/phi (eta,phi).                 Default: true.
 *  WriteRect     - Write rectangular components (px,py,pz). Default: false.
 *
 * So the default settings give (pt, eta, phi, m).
 *
 * The input can be an @c I4Momentum, @c INavigable4Momentum,
 * or @c CLHEP::HepLorentzVector.
 */
class FourMomFillerTool
  : public BlockFillerTool<Types<INavigable4Momentum,
                                 I4Momentum,
                                 CLHEP::HepLorentzVector,
                                 xAOD::IParticle> >
{
public:
  typedef BlockFillerTool<Types<INavigable4Momentum,
                                 I4Momentum,
                                 CLHEP::HepLorentzVector,
                                 xAOD::IParticle> > Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  FourMomFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   */
  virtual StatusCode fill (const INavigable4Momentum& p);


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   */
  virtual StatusCode fill (const I4Momentum& p);


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   */
  virtual StatusCode fill (const CLHEP::HepLorentzVector& p);


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   */
  virtual StatusCode fill (const xAOD::IParticle& p);


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   */
  virtual StatusCode fill (const TLorentzVector& p);


private:
  /// Property: Signal state of the object to read.
  int m_signalState;

  /// Property: Should we fill E?
  bool m_do_E;

  /// Property: Should we fill p?
  bool m_do_p;

  /// Property: Should we fill Et?
  bool m_do_Et;

  /// Property: Should we fill pt?
  bool m_do_pt;

  /// Property: Should we fill m?
  bool m_do_m;

  /// Property: Should we fill rapidity?
  bool m_do_rapidity;

  /// Property: Should we fill tan(theta)?
  bool m_do_tanth;

  /// Property: Should we fill eta/phi?
  bool m_do_etaphi;

  /// Property: Should we fill px/py/pz?
  bool m_do_rect;


  /// Variable: Energy.
  float* m_E;

  /// Variable: Momentum.
  float* m_p;

  /// Variable: Transverse energy (E*sin(theta)).
  float* m_Et;

  /// Variable: Transverse momentum.
  float* m_pt;

  /// Variable: Mass.
  float* m_m;

  /// Variable: Rapidity.
  float* m_y;

  /// Variable: Tangent of polar angle.
  float* m_tanth;

  /// Variable: Pseudorapidity.
  float* m_eta;

  /// Variable: Azimuth.
  float* m_phi;

  /// Variable: x-component of momentum.
  float* m_px;

  /// Variable: y-component of momentum.
  float* m_py;

  /// Variable: z-component of momentum.
  float* m_pz;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_FOURMOMFILLERTOOL_H
