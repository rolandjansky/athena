// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetSignalStateFillerTool.h
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @date September, 2009
 * @brief Block filler tool for a four-momentum with signal states.
 * @brief Inspired directly by the common 4-momentum filler
 */

#ifndef JETD3PDMAKER_JETSIGNALSTATEFILLER_H
#define JETD3PDMAKER_JETSIGNALSTATEFILLER_H


#include "D3PDMakerUtils/BlockFillerTool.h"
class Jet;


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
 * FIXME: This tool really should be taking @c I4Momentum as input.
 * But the @c INavigable4Momentum bases haven't been declared to SGTools,
 * so we can't convert from @c INavigable4Momentum to @c I4Momentum.
 * So we use @c INavigable4Momentum for now.
 */
class JetSignalStateFillerTool
  : public BlockFillerTool<Jet>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetSignalStateFillerTool (const std::string& type,
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
  virtual StatusCode fill (const Jet& p);

  /**
   * @brief Add prefix to variable name
   *
   * This is a simple overloaded function for addVariable which will
   * append a the python Property prefix to the original name
   */

  template <class T>
  StatusCode addVariablePrefixed(const std::string& name,
				 T* & ptr,
				 const std::string& dim = "");
    
private:
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

  /// Property: Signal state Jets will be reverted to.
  int m_sigstate;

  /// Property: Prefix applied to FourMom variable names
  std::string m_prefix;

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
