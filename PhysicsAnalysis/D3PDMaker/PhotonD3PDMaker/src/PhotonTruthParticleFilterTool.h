// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/PhotonTruthParticleFilterTool.h
 * @author Mike Hance <michael.hance@cern.ch>
 * @date August 2010
 * @brief Filter truth particles for writing to D3PD.  Based on TruthParticleFilterTool in EventCommonD3PDMaker
 */


#ifndef PHOTOND3PDMAKER_PHOTONTRUTHPARTICLEFILTERTOOL_H
#define PHOTOND3PDMAKER_PHOTONTRUTHPARTICLEFILTERTOOL_H

#include "D3PDMakerUtils/SGKeyResolver.h"
#include "McParticleKernel/ITruthParticleFilterTool.h"
#include "PhotonD3PDMaker/ITruthParticleFilterCutTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
class McEventCollection;
namespace HepMC {
  class GenEvent;
  class GenVertex;
  class GenParticle;
}


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
 *
 * Most of the code here is just copying the @c McEventCollection ---
 * that should be factored out somehow.
 */
class PhotonTruthParticleFilterTool
  : public ITruthParticleFilterTool, public AthAlgTool
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PhotonTruthParticleFilterTool (const std::string& type,
				 const std::string& name,
				 const IInterface* parent);


  /// Standard Gaudi @c queryInterface method.
  virtual StatusCode queryInterface( const InterfaceID& riid,
                                     void** ppvIf );

  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();

  /// Standard Gaudi finalize  method.
  virtual StatusCode finalize();

  /// Run the tool.
  virtual StatusCode execute();


  /** This method will check the validity of the input McEventCollection 
   *  and build a filtered one from the strategy implemented by the 
   *  concrete tool.
   *  It is not const to allow derived tools to build statistics during
   *  the filtering process.
   */
  virtual StatusCode buildMcAod( const McEventCollection* in,
				 McEventCollection* filtered );


private:
  /// Filter a single @c GenEvent.
  StatusCode filterEvent (const HepMC::GenEvent* ev_in,
                          HepMC::GenEvent* ev_out);

  /// Add a @c GenParticle (and its production vertex) to a @c GenEvent.
  StatusCode addParticle (const HepMC::GenParticle* p,
                          HepMC::GenEvent* ev);

  /// Add a @c GenVertex to a @c GenEvent.
  StatusCode addVertex (const HepMC::GenVertex* p,
                        HepMC::GenEvent* ev);

  /// Test to see if we want to keep a particle.
  bool acceptParticle (const HepMC::GenParticle* p);

  /// Parameter: SG key for input @c McEventCollection.
  std::string m_mcEventsName;

  /// Helper to resolve SG key for input collection.
  SGKeyResolver m_resolver;

  /// Parameter: SG key for output @c McEventCollection.
  std::string m_mcEventsOutputName;

  /// Parameter: Keep pileup @c GenEvent's?
  bool m_doPileup;

  /// Parameter: Remove @c GenEvent's with no particles?
  bool m_removeEmpty;

  /// Unused parameter (but required by configuration code).
  bool m_doEtIsolations;

  /// Parameter: Keep partons?
  bool m_writePartons;

  /// Parameter: Keep hadrons?
  bool m_writeHadrons;

  /// Parameter: Keep geant particles?
  bool m_writeGeant;

  /// Set when we see the first hadron.
  bool m_haveSeenAHadron;

  /// Set when we see the first hadron.
  int m_firstHadronBarcode;

  /// Particle filters
  ToolHandleArray<ITruthParticleFilterCutTool> m_particleFilters;
};


} // namespace D3PD


#endif // not PHOTONTRUTHPARTICLEFILTERTOOL_H
