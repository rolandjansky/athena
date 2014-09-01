///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSortingTool.h
// Header file for class ParticleSortingTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_PARTICLESORTINGTOOL_H
#define EVENTUTILS_PARTICLESORTINGTOOL_H 1

// STL includes
#include <vector>
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// EDM inlcudes
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"



class ParticleSortingTool
  : virtual public ::DerivationFramework::IAugmentationTool,
            public ::AthAlgTool
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Copy constructor:

  /// Constructor with parameters:
  ParticleSortingTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor:
  virtual ~ParticleSortingTool();

  /// Athena algtool's initialize
  virtual StatusCode  initialize();

  /// Athena algtool's finalize
  virtual StatusCode  finalize();


  /// Implement the method from the ISkimmingTool interface
  virtual StatusCode addBranches() const final override;



// Private methods
private:

  /// Helper method that implements the call to the right sort function
  StatusCode doSort( xAOD::IParticleContainer* cont ) const;

  /// The method to compare the particle's pt
  bool comparePt( const xAOD::IParticle* partA, const xAOD::IParticle* partB ) const;

  /// The method to compare the particle's eta
  bool compareEta( const xAOD::IParticle* partA, const xAOD::IParticle* partB ) const;

  /// The method to compare the particle's phi
  bool comparePhi( const xAOD::IParticle* partA, const xAOD::IParticle* partB ) const;

  /// The method to compare the particle's mass
  bool compareMass( const xAOD::IParticle* partA, const xAOD::IParticle* partB ) const;

  /// The method to compare the particle's energy
  bool compareEnergy( const xAOD::IParticle* partA, const xAOD::IParticle* partB ) const;

  /// The method to compare the particle's rapidity
  bool compareRapidity( const xAOD::IParticle* partA, const xAOD::IParticle* partB ) const;

  /// Method to compare two doubles
  inline bool compareDouble( double a, double b ) const;


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  /// Input container name
  StringProperty m_inCollKey;

  /// Output collection name (deep copies of the original ones)
  StringProperty m_outCollKey;

  /// Define by what parameter to sort (default: 'pt')
  StringProperty m_sortVar;

  /// Define if the container should be sorted in a decending order (default=true)
  BooleanProperty m_sortDecending;


  /// Internal container type identifier
  mutable unsigned int m_contID;

  /// Internal identifier for the type of sorting
  mutable int m_sortID;

  /// Internal event counter
  mutable unsigned long m_nEventsProcessed;

};


inline bool ParticleSortingTool::compareDouble( double a, double b ) const
{
  if ( m_sortID < 0 ) { return ( a > b ); }
  else { return ( a < b ); }
}

#endif //> !EVENTUTILS_PARTICLESORTINGTOOL_H
