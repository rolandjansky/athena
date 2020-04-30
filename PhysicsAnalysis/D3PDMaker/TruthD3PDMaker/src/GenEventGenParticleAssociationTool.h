// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventGenParticleAssociationTool.h 639304 2015-01-13 20:08:39Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/GenEventGenParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Associate from a @c GenEvent to its contained particles.
 */


#ifndef EVENTCOMMOND3PDMAKER_GENEVENTGENPARTICLEASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_GENEVENTGENPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"


namespace D3PD {


/**
 * @brief Associate from a @c GenEvent to its contained particles.
 *
 * This is a multiple association tool.
 */
class GenEventGenParticleAssociationTool
  : public MultiAssociationTool<HepMC::GenEvent, HepMC::GenParticle>
{
public:
  typedef MultiAssociationTool<HepMC::GenEvent, HepMC::GenParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  GenEventGenParticleAssociationTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const HepMC::GenEvent& p);


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const HepMC::GenParticle* next();


private:
  static const int PARTONPDGMAX = 40;
  static const int PHOTOSMIN = 10000;
  static const int GEANTMIN = 200000;

  /// Property: Should we fill partons?
  bool m_doPartons;

  /// Property: Should we fill partons?
  bool m_doHadrons;

  /// Property: Should we fill partons?
  bool m_doGeant;

  /// have we already walked by a hadron?
  bool m_haveSeenAHadron;
  int m_firstHadronBarcode;

  /// GenParticle iterators.
  HepMC::GenEvent::particle_const_iterator m_it;
  HepMC::GenEvent::particle_const_iterator m_end;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_GENEVENTGENPARTICLEASSOCIATIONTOOL_H
