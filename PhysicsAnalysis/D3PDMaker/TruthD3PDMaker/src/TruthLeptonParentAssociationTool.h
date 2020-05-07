// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:
/**
 * @file TruthD3PDMaker/src/TruthTauDecayAssociationTool.h
 * @author Zach Marshall <zach.marshall@cern.ch>
 * @date June, 2013
 * @brief Associate from a tau to its hadronic decay products.
 */

#ifndef EVENTCOMMOND3PDMAKER_TRUTHLEPTONPARENTASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_TRUTHLEPTONPARENTASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "McParticleEvent/TruthParticle.h"


#include "AtlasHepMC/GenParticle_fwd.h"
namespace D3PD {

/**
 * @brief Associate from a true hadronic tau to its decay products
 *
 * This is a multiple association tool.
 */
class TruthLeptonParentAssociationTool
  : public MultiAssociationTool<TruthParticle>
{
public:
  typedef MultiAssociationTool<TruthParticle> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthLeptonParentAssociationTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent);

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const TruthParticle& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const TruthParticle* next();

private:

  /// Function for association to a specific lepton 
  void addLeptonParent(const HepMC::GenParticle*);

  /// TruthParticle iterator
  std::vector<const TruthParticle*> m_parents;

  /// Name of the container
  std::string m_truth_container_name;

  /// TruthParticle barcodes
  std::vector<int> m_parent_barcodes;

  /// for keeping trace of barcodes in order to detect loops
  std::vector<int> m_barcode_trace;

  /// Is the primary a tau?
  bool m_primary_is_tau;

  /// Current index
  unsigned int m_i;

};

} // namespace D3PD

#endif // not EVENTCOMMOND3PDMAKER_TRUTHLEPTONPARENTASSOCIATIONTOOL_H

