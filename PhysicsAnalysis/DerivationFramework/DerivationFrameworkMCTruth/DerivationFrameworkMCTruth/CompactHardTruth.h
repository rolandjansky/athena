///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CompactHardTruth.h
// Header file for class CompactHardTruth
// Author: Frank Paige <paige@bnl.gov>
//
// Algorithm to construct small, consistent hard-scattering HepMC truth
// record from GEN_EVENT. Only final partons have physical momenta, so
// must recluster them. Main steps:
// (1)  Identify hadronization vertices with partons in, hadrons out.
// (2)  Detach their incoming partons and delete their descendants.
// (3)  Iteratively recombine 1->2 branchings of final partons with
//      mass M < Mcut. Split 2->1 nonperturbative color recombinations.
// (4)  Remove partons not connected to hard process. May make apparent
//      anomalies, e.g., dropping soft q from incoming g -> g q.
//
// New final partons have correct momenta and ancestors/descendants.
// CompactHardTruth changes particles/vertices, so it cannot be used directly 
// with thinning, but it can be used in parallel.
//
/////////////////////////////////////////////////////////////////// 
#ifndef DERIVATIONFRAMEWORK_COMPACTHARDTRUTH_H
#define DERIVATIONFRAMEWORK_COMPACTHARDTRUTH_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

// fwd declares
class IMcVtxFilterTool;
class TH1F;


namespace DerivationFramework {

class CompactHardTruth
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CompactHardTruth( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CompactHardTruth(); 

  // Assignment operator: 
  //CompactHardTruth &operator=(const CompactHardTruth &alg); 

  // Athena algorithm Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();


  // Parton is quark or gluon
  virtual bool isParton( HepMC::ConstGenParticlePtr );

  // Final parton is quark or gluon ending in vertex giving !isParton
  virtual bool isFinalParton( HepMC::ConstGenParticlePtr );

  // Hadron excludes leptons and BSM particles
  virtual bool isHadron( HepMC::ConstGenParticlePtr p );

  // Total in/out FourVector for vertex
  virtual HepMC::FourVector vtxInMom(HepMC::ConstGenVertexPtr);
  virtual HepMC::FourVector vtxOutMom(HepMC::ConstGenVertexPtr);

 private: 

  /// Default constructor: 
  CompactHardTruth();

  /// Containers
  std::string m_mcEventsName;
  std::string m_thinnedMcEventsName;

  // Variables
  int m_evtCount = 0;
  int m_missCount = 0;
  float m_partonCut;
  float m_hardCut;

  int m_dangleFound = 0;
  int m_dangleRemoved = 0;
  float m_danglePtMax = 0.0F;
  float m_danglePtCut;

  int m_maxCount;

  int m_thinParticles = 0;
  int m_thinVertices = 0;

}; 


} //> end namespace DerivationFramework
#endif //> !DERIVATIONFRAMEWORK_COMPACTHARDTRUTH_H
