// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenParticleFillerTool.h 639304 2015-01-13 20:08:39Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/GenParticleFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for a HepMC GenParticle.
 */

#ifndef EVENTCOMMOND3PDMAKER_GENPARTICLEFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_GENPARTICLEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AtlasHepMC/GenParticle.h"


namespace D3PD {


/**
 * @brief Block filler tool for a HepMC GenParticle.
 *
 * This tool has properties to control what gets written:
 *
 *  WriteE        - Write energy (e).                        Default: false.
 *  WriteP        - Write momentum (p).                      Default: false.
 *  WritePt       - Write transverse momentum (pt).          Default: true.
 *  WriteM        - Write mass (m).                          Default: true.
 *
 * So the default settings give (pt, eta, phi, m).
 */
class GenParticleFillerTool
  : public BlockFillerTool<HepMC::GenParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  GenParticleFillerTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const HepMC::GenParticle& p);


private:
  /// Property: Should we fill E?
  bool m_do_E;

  /// Property: Should we fill p?
  bool m_do_p;

  /// Property: Should we fill pt?
  bool m_do_pt;

  /// Property: Should we fill m?
  bool m_do_m;

  /// Property: Should we fill px?
  bool m_do_px;

  /// Property: Should we fill py?
  bool m_do_py;

  /// Property: Should we fill pz?
  bool m_do_pz;

  /// Property: Should we fill mother type?
  bool m_do_mothertype;

  /// Property: Should we fill mother barcode?
  bool m_do_motherbarcode;

  /// Variable: Energy.
  float* m_E;

  /// Variable: Momentum.
  float* m_p;

  /// Variable: Transverse momentum.
  float* m_pt;

  /// Variable: Mass.
  float* m_m;

  /// Variable: Momentum in x.
  float* m_px;

  /// Variable: Momentum in y.
  float* m_py;

  /// Variable: Momentum in z.
  float* m_pz;

  /// Variable: Pseudorapidity.
  float* m_eta;

  /// Variable: Azimuth.
  float* m_phi;

  /// Variable: PDG ID.
  int* m_type;

  /// Variable: status.
  int* m_status;

  /// Variable: PDG ID of mother (first incoming particle of prod vertex).
  int* m_mothertype;

  /// Variable: HepMC barcode.
  int* m_barcode;

  /// Variable: HepMC barcode of mother (first incoming particle of prod vertex).
  int* m_motherbarcode;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_GENPARTICLEFILLERTOOL_H
