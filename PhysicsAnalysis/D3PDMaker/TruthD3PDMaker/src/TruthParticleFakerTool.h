/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleFakerTool.h
 * @author Zach Marshall <zmarshal@caltech.edu>
 * @date April, 2011
 * @brief Block filler tool for TruthParticle's.
   Fakes the filling of reco-like branches
 */

#ifndef TRUTHD3PDMAKER_TRUTHPARTICLEFAKERTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLEFAKERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "AtlasHepMC/GenParticle.h"
#include <vector>


namespace D3PD {


class TruthParticleFakerTool : public BlockFillerTool<HepMC::GenParticle>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleFakerTool (const std::string& type,
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
  int m_filterID; /// Property: Particle ID for filter

  bool m_do_E;   /// Property: Should we fill E?
  bool m_do_p;  /// Property: Should we fill p?
  bool m_do_pt;  /// Property: Should we fill pt?
  bool m_do_m;  /// Property: Should we fill m?
  bool m_do_pn;  /// Property: Should we fill px, py, and pz?
  bool m_do_chg; /// Property: Should we fill charge?
  bool m_do_Et; /// Property: Should we fill transverse energy?
  double m_minPt; /// Property: minimum pT for a lepton

  //int m_n; /// Variable: Count of number of particles saved

  float* m_E;  /// Variable: Energy.
  float* m_p;  /// Variable: Momentum.
  //float* m_Et;  /// Variable: Transverse energy.
  float* m_pt;  /// Variable: Transverse momentum.
  float* m_px;  /// Variable: x-omentum.
  float* m_py;  /// Variable: y-momentum.
  float* m_pz;  /// Variable: z-momentum.
  float* m_m;  /// Variable: Mass.
  float* m_eta;  /// Variable: Pseudorapidity.
  float* m_phi;  /// Variable: Azimuth.

  //int* m_type;  /// Variable: PDG ID.
  int* m_status;  /// Variable: status.
  int* m_barcode;  /// Variable: HepMC barcode.
  int* m_charge; /// Variable: Particle charge.
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_TRUTHPARTICLEFAKERTOOL_H
