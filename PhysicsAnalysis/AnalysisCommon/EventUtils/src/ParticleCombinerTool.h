/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef EVENTUTILS_PARTICLECOMBINERTOOL_H
#define EVENTUTILS_PARTICLECOMBINERTOOL_H

//============================================================================
// Name: ParticleCombinerTool.h
//
/**
   @class ParticleCombinerTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date July 2014

   @brief Combine particles to composite particles

   Class to combine two or more particles from a given list of
   input containers. All combinations will be tried and the unique ones will
   be written to StoreGate.
*/
//=============================================================================


// STL includes
#include <string>
#include <vector>

// Athena includes
#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// CompositeParticle
#include "xAODParticleEvent/CompositeParticleContainerFwd.h"
#include "xAODParticleEvent/CompositeParticleFwd.h"
#include "xAODParticleEvent/IParticleLinkContainer.h"
#include "xAODParticleEvent/IParticleLink.h"
#include "xAODMissingET/MissingET.h"

// MC Truth
// #include "GeneratorObjects/McEventCollection.h"


class ParticleCombinerTool : public AthAlgTool,
                             virtual public DerivationFramework::IAugmentationTool
{
public:

  /** Default constructor */
  ParticleCombinerTool(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  /** Default destructor */
  virtual ~ParticleCombinerTool() ;


public:
  /** Gaudi Service Interface method implementations - initialize */
  virtual StatusCode initialize() override;

  /** Gaudi Service Interface method implementations - finalize */
  virtual StatusCode finalize() override;

  /// Implement the method from the IAugmentationTool interface
  virtual StatusCode addBranches() const final override;




private:
  /// The test if two particles are equal.
  /// One would need a barcode, i.e., particle-unique identifier, to do this proper
  bool isEqual( const xAOD::IParticle* part1, const xAOD::IParticle* part2 ) const;

  /** Build the composite candidates */
  StatusCode buildComposite( xAOD::CompositeParticleContainer* outContainer,
                             xAOD::IParticleLinkContainer& anIPartLinkList,
                             const xAOD::MissingET* metObject ) const;

  /** Check if the composite particle at hand was already found before */
  bool compositeParticleAlreadyFound( xAOD::CompositeParticleContainer* compContainer,
                                      xAOD::CompositeParticle* compPart ) const;

  /** Check that two particles are not the same or, if they are
      composite particles, that they don't share the same constitutents */
  bool shareSameConstituents( const xAOD::IParticle* part1,
                              const xAOD::IParticle* part2 ) const;

  /** Check that two particles are not the same or, if they are
      composite particles, that they don't share the same constitutents */
  bool shareSameConstituents( const xAOD::IParticle* part1,
                              const xAOD::CompositeParticle* compPart2 ) const;

  /** Check that two particles are not the same or, if they are
      composite particles, that they don't share the same constitutents */
  bool shareSameConstituents( const xAOD::CompositeParticle* compPart1,
                              const xAOD::CompositeParticle* compPart2 ) const;

  /// Try to get the charge for all types
  float getCharge( const xAOD::IParticleLink& aParticleLink, bool& hasCharge ) const;

  /** Do the MC Truth selections */
  // bool mcTruthSelections( const xAOD::CompositeParticle* compPart );



private:
  //------------------------------------------------------
  // Tools and Containers
  //------------------------------------------------------

  /// List of input collection names
  StringArrayProperty m_inCollKeyList;

  /// The name of the missing ET object, e.g., "Final"
  StringProperty m_metName;

  /// Output collection name (the composite particles)
  StringProperty m_outCollKey;

  /// Decide if we want to write a fully-split AuxContainer such that we can remove any variables
  BooleanProperty m_writeSplitAux;

  /// The PDG_ID of the CompositeParticle
  IntegerProperty m_pdgId;

  /// If true: sort the constituents in decending pt order
  BooleanProperty m_sortConstit;

  // /** MCEventCollection name */
  // StringProperty m_mcEventCollKey;
  //
  // /** MCEventCollection */
  // const McEventCollection* m_mcEventColl;



private:
  /** Internal event counter */
  mutable unsigned long m_nEventsProcessed;


private:
  //------------------------------------------------------
  // Internal variables
  //------------------------------------------------------
  /** Create a vector that will hold all the INav4MomLinks for one CompositeParticle */
  mutable xAOD::IParticleLinkContainer m_anIPartLinkList;


  /** Create a vector of LinkContainers and a map of their names */
  mutable std::vector< const xAOD::IParticleLinkContainer* > m_inputLinkContainerList;

  /** create a vector that will contain pointers to the
   *  xAOD::IParticleLinkContainers that need to be deleted manually.
   *  This is because these containers never get registered to StoreGate and
   *  thus, the ownership resides inside this class instance. */
  mutable std::vector< const xAOD::IParticleLinkContainer* > m_inputLinkContainerListToDelete;

  /** A map to store the name associated to each input link container object */
  mutable std::map< const xAOD::IParticleLinkContainer*, const std::string* > m_inputLinkContainerNames;

  /** A map to store the input container that are already used.
   *  use a map to determine if a given input container
   *  has already been seen, and simply point to the
   *  link container corresponding to  that already-used
   *  input container   */
  mutable std::map< const xAOD::IParticleContainer*, const xAOD::IParticleLinkContainer* > m_alreadyUsedInputContainers;


  /** Vector of the labels/keys of all input containers (all converted to link containers) */
  mutable std::vector<std::string> m_containerLabels;

  /** A map to store the size of each input container */
  mutable std::map<std::string, int> m_containerMaxN;

  /** A map to store the container that are already used */
  mutable std::map< const xAOD::IParticleLinkContainer*, std::string > m_alreadyUsedContainers;


}; // End class ParticleCombinerTool


#endif // EVENTUTILS_PARTICLECOMBINERTOOL_H
