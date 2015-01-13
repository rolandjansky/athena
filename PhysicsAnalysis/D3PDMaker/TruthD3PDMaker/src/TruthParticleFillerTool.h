// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TruthD3PDMaker/src/TruthParticleFillerTool.h
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date Dec, 2009
 * @brief Block filler tool for TruthParticle's.
 */

#ifndef TRUTHD3PDMAKER_TRUTHPARTICLEFILLERTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLEFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "GaudiKernel/ServiceHandle.h"
class IPartPropSvc;


namespace D3PD {

class TruthParticleFillerTool
  : public BlockFillerTool<Types<TruthParticle, xAOD::TruthParticle> >
{
public:
  typedef BlockFillerTool<Types<TruthParticle, xAOD::TruthParticle> > Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleFillerTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;


  /// Book variables for this block.
  virtual StatusCode book() override;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const TruthParticle& p) override;


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::TruthParticle& p) override;


private:
  /// Property: Name for pdgId variable.
  std::string m_PDGIDVariable;

  /// Property: Particle property service.
  ServiceHandle<IPartPropSvc> m_ppsvc;

  /// Variable: Status code for the particle.
  int* m_status;

  /// Variable: Barcode for the particle.
  int* m_barcode;

  /// Variable: Particle ID code.
  int* m_pdgId;

  /// Variable: Particle charge.
  float* m_charge;
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_TRUTHPARTICLEFILLERTOOL_H
