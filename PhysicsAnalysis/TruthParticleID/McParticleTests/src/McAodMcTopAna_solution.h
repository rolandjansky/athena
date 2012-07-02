///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McTopAna.h 
// Header file for class McTopAna
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETESTS_MCAOD_MCTOPANASOLUTION_H
#define MCPARTICLETESTS_MCAOD_MCTOPANASOLUTION_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// fwd declares
class IMcVtxFilterTool;
class ITruthParticleCnvTool;
class TH1F;

namespace McAod {

class McTopAnaSolution
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  McTopAnaSolution( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~McTopAnaSolution(); 

  // Assignment operator: 
  //McTopAna &operator=(const McTopAna &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// perform the mc t->Wb filtering
  StatusCode doMcTopWb();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  McTopAnaSolution();

  // handle to the mc-vtx filter tool
  ToolHandle<IMcVtxFilterTool> m_vtxFilter;

  // handle to converter tool
  ToolHandle<ITruthParticleCnvTool> m_cnvTool;

  // Containers

  /// input McEventCollection container name
  std::string m_mcEventsName;

  /// output filtered McEventCollection container name
  std::string m_filteredMcEventsName;

  /// output filtered TruthParticleContainer name
  std::string m_filteredMcParticlesName;

  /// Mc top invariant mass.
  TH1F *m_h_mctop_mass;

  /// Mc W invariant mass
  TH1F *m_h_mcwqq_mass;

  /// Mc q1 energy
  TH1F *m_h_mcq1_ene;

  /// Mc q2 energy
  TH1F *m_h_mcq2_ene;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace McAod
#endif //> !MCPARTICLETESTS_MCAOD_MCTOPANASOLUTION_H
