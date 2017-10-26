// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//==============================================
//  File TileMu.h
//  define the muon tagged by Tile object TileMu 
//=============================================

#ifndef TILEMU_H
#define TILEMU_H

#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>

/**
 * @class TileMu
 * @brief Class to store TileMuId quantities
 * @author G. Usai (giulio.usai@cern.ch)
 */

class TileMu { 

 public:

  /** Constructor */
  TileMu()
    : m_eta(0.0),
      m_phi(0.0),
      m_quality_factor(0.0)
  {}

  /** Constructor */
  TileMu ( float eta, float phi, std::vector<float> & ener, float qual );

  /** Destructor */
  ~TileMu() {}

  void Set( float eta, float phi, std::vector<float> & ener, float qual ); 

  /** Eta (computed as the average of the eta values of the TileCal cells where the muon goes through) */
  float eta() const { return m_eta; }

  /** Phi (average value at the TileCal radius) */
  float phi() const { return m_phi; }

  /** Energy deposition by the muons in TileCal:
   * 1st component: energy deposited in innermost layer (A cells),
   * 2nd component: energy deposited in central layer (BC cells),
   * 3rd component: energy deposited in outermost layer (D cells),
   * 4th component: energy deposited in the surrounding cells
   */
  std::vector<float> enedep() const { return m_energy_deposited; }

  /** Quality flag (0 or 1):
   * set to 0 if the "energy deposition path" is MIP like in all three samples (tight muon),
   * set to 1 if the "energy deposition path" is MIP like in two samples and a larger energy deposition is found in the remaining cell (loose muon)
   */
  float quality() const      { return m_quality_factor; }

  /** Return identification */
  std::string whoami (void) const { return "TileMu"; }

  /** Printing for debugging */
  void print (void) const;

  /** Convertion operator to a std::string,
   * can be used in a cast operation : (std::string) TileMu */
  operator std::string() const;

 private:

  /** Muon eta */
  float m_eta;

  /** Muon phi */
  float m_phi;

  /** Energy deposited by the muons in TileCal */
  std::vector<float> m_energy_deposited;

  /** Quality flag for tight and loose muon selection */
  float m_quality_factor;
};

CLASS_DEF(TileMu,        2938, 0)

#endif    
