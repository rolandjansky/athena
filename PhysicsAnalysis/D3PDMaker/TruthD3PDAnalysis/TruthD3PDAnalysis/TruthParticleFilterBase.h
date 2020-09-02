// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDAnalysis/TruthParticleFilterBase.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Filter truth particles for writing to D3PD: base class.
 */


#ifndef TRUTHD3PDANALYSIS_TRUTHPARTICLEFILTERBASE_H
#define TRUTHD3PDANALYSIS_TRUTHPARTICLEFILTERBASE_H


#include "D3PDMakerUtils/SGKeyResolver.h"
#include "McParticleKernel/ITruthParticleFilterTool.h"
#include "McParticleKernel/ITruthIsolationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
class ITruthIsolationTool;
class McEventCollection;
class TruthEtIsolationsContainer;
#include "AtlasHepMC/GenParticle_fwd.h"
#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/GenVertex_fwd.h"


namespace D3PD {


/**
 * @brief Filter truth particles for writing to D3PD: base class.
 *
 * This is used as part of @c TruthParticleBuilder.
 * This contains the boilerplate code to  retrieve a @c McEventCollection,
 * filter it, and write a new one.
 *
 * The @c TruthParticleBuilder will then turn the filtered
 * @c McEventCollection into a @c TruthParticleContainer.
 *
 * Most of the code here is just copying the @c McEventCollection ---
 * would be better to try to combine with that.
 *
 * You need to supply an implementation of isAccepted.
 */
class TruthParticleFilterBase
  : public ITruthParticleFilterTool, public AthAlgTool
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleFilterBase (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


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


  /// Filter a single @c GenEvent.
  virtual StatusCode filterEvent (const HepMC::GenEvent* ev_in,
                                  HepMC::GenEvent* ev_out);

  /// Add a @c GenParticle (and its production vertex) to a @c GenEvent.
  virtual StatusCode addParticle (const HepMC::GenParticle* p,
                                  HepMC::GenEvent* ev);

  /// Add a @c GenVertex to a @c GenEvent.
  virtual StatusCode addVertex (const HepMC::GenVertex* p,
                                HepMC::GenEvent* ev);

  /// Test to see if we want to keep a particle.
  virtual bool isAccepted (const HepMC::GenParticle* p);

  /// Return the container of isolation energies that we built.
  StatusCode isolations (TruthEtIsolationsContainer const* & isocont);


private:
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

  /// Should isolation cone energies be calculated?
  bool m_doEtIsolations;

  typedef ToolHandle<ITruthIsolationTool> IsolTool_t;
  /** Pointer to the TruthIsolationTool to be able to compute
   *  transverse energy isolations for various isolation cones cuts.
   *  See McParticleEvent/TruthParticleParameters.h for cone cuts.
   */
  IsolTool_t m_isolationTool;
};


} // namespace D3PD


#endif // not TRUTHD3PDANALYSIS_TRUTHPARTICLEFILTERBASE_H
