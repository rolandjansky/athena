// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDAnalysis/src/TruthParticleFilterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2009
 * @brief Filter truth particles for writing to D3PD.
 */


#ifndef TRUTHD3PDANALYSIS_TRUTHPARTICLEFILTERTOOL_H
#define TRUTHD3PDANALYSIS_TRUTHPARTICLEFILTERTOOL_H


#include "TruthD3PDAnalysis/TruthParticleFilterBase.h"
#include "AtlasHepMC/GenParticle_fwd.h"


namespace D3PD {


/**
 * @brief Filter truth particles for writing to D3PD.
 *
 * This is used as part of @c TruthParticleBuilder.
 *
 * This tool retrieves a @c McEventCollection, filters it, and
 * writes a new one.
 *
 * The @c TruthParticleBuilder will then turn the filtered
 * @c McEventCollection into a @c TruthParticleContainer.
 */
class TruthParticleFilterTool
  : public TruthParticleFilterBase
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleFilterTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /// Run the tool.
  virtual StatusCode execute();


  /// Test to see if we want to keep a particle.
  virtual bool isAccepted (const HepMC::GenParticle* p);

  // New member functions.
  bool isLeptonFromTau(const HepMC::GenParticle* part) const;
  bool isFsrFromLepton(const HepMC::GenParticle* part) const;
  bool isFromTau(const HepMC::GenParticle* part);
  bool isBSM(const HepMC::GenParticle* part) const;
  bool isBoson(const HepMC::GenParticle* part) const;

private:
  /// Parameter: Keep partons?
  bool m_writePartons;

  /// Parameter: Keep hadrons?
  bool m_writeHadrons;

  /// Parameter: Keep b-hadrons?
  bool m_writeBHadrons;

  /// Parameter: Keep geant particles?
  bool m_writeGeant;

  /// Parameter: Write Geant photons with Pt above this threshold.
  /// Set to < 0 to not write any.
  float m_geantPhotonPtThresh;

  /// Parameter: Keep hadronic tau decays?
  bool m_writeTauHad;

  /// Parameter: Keep BSM particles?
  bool m_writeBSM;

  /// Parameter: Keep bosons?
  bool m_writeBosons;

  /// Parameter: Write partons with Pt above this threshold.
  float m_partonPtThresh;

  /// Parameter: Write absolutely everything
  bool m_writeEverything;

  /// Parameter: Write boson decay products
  bool m_writeBosonProducts;

  /// Parameter: Write BSM decay products
  bool m_writeBSMProducts;

  /// Parameter: Write top and decay products
  bool m_writeTopAndDecays;

  /// Parameter: Write all leptons
  bool m_writeAllLeptons;

  /// Parameter: Write particles with status code 3
  bool m_writeStatus3;

  /// Parameter: First N particles to write
  int m_writeFirstN;
  int m_particleCountSoFar;

  /// for keeping trace of barcodes in order to detect loops
  std::vector<int> m_barcode_trace;
};


} // namespace D3PD


#endif // not TRUTHD3PDANALYSIS_TRUTHPARTICLEFILTERTOOL_H
