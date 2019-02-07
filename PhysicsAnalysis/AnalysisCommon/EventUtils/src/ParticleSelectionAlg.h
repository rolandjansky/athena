///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleSelectionAlg.h
// Header file for class ParticleSelectionAlg
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef EVENTUTILS_PARTICLESELECTIONALG_H
#define EVENTUTILS_PARTICLESELECTIONALG_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
// #include "GaudiKernel/ServiceHandle.h"
// #include "AthenaBaseComps/AthAlgorithm.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "xAODBase/IParticleContainer.h"
//#include "TrigDecisionTool/TrigDecisionTool.h"

// // Forward declarations
// namespace Trig{
//   class TrigDecisionTool;
// }
// Forward declarations
namespace ExpressionParsing {
  class ExpressionParser;
}


class ParticleSelectionAlg
  : public ::AthAnalysisAlgorithm
{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  // Copy constructor:

  /// Constructor with parameters:
  ParticleSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~ParticleSelectionAlg();


  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's beginRun hook
  /// (called once before running over the events, after initialize)
  virtual StatusCode  beginRun();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();


 private:
  /// Private function to perform the actualy work
  template<class CONT, class AUXCONT>
  StatusCode selectParticles(const xAOD::IParticleContainer* inContainer,
                             const std::vector<int>& resultVec) const;


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// Name of the EventInfo object
  StringProperty m_evtInfoName;

  /// Name of the PrimaryVertex container
  StringProperty m_inPrimVtxCont;

  /// Input container name
  StringProperty m_inCollKey;

  /// Output collection name (deep copies of the original ones)
  StringProperty m_outCollKey;

  /// Decide if we want to write a fully-split AuxContainer such that we can remove any variables
  BooleanProperty m_writeSplitAux;

  /// Defines the ownership policy of the output container
  /// (default: 'VIEW_ELEMENTS'; also allowed: 'OWN_ELEMENTS')".
  /// TO see what this means, go here:
  /// https://twiki.cern.ch/twiki/bin/view/AtlasComputing/SoftwareTutorialxAODAnalysisInAthena#Understanding_the_different_type
  StringProperty m_outOwnPolicyName;

  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection;

  /// If true (deault: false), do the bookkeeping of how many particles passed
  /// which selection cuts
  bool m_doCutFlow;

  /// The name of the resulting xAOD::CutBookkeeperContainer.
  /// If an empty name is given (default), the name of the algorithm instance is used.
  StringProperty m_cutBKCName;



  /// @name Internal members
  /// @{

  /// The expression parser
  ExpressionParsing::ExpressionParser *m_parser;

  // /// The trigger decision tool
  // ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

  /// Internal event counter
  unsigned long m_nEventsProcessed;

  /// The internally used translation for the ownership policy
  SG::OwnershipPolicy m_outOwnPolicy;

  /// An enumaration for the actual container type
  enum contType_t {
    UNKNOWN,
    PHOTON,
    ELECTRON,
    MUON,
    TAU,
    JET,
    PARITCLEFLOW,
    NEUTRALPARTICLE,
    TRACKPARTICLE,
    TRUTHPARTICLE,
    COMPOSITEPARTICLE,
    PARTICLE,
    CALOCLUSTER
  };

  /// The variable that holds the value that we find for the input container
  contType_t m_contType;

  /// The starting index of where in the CutBookkeeperContainer our new CutBookkeepers start
  std::size_t m_cutBKStartIdx;

  /// The list of pairs of the tool index of the AsgSelectionTools and the
  /// starting index of the corresponding CutBookKeeper inside the CutBookkeeperContainer.
  std::vector<std::size_t> m_selToolIdxOffset;

  /// The list of pairs of the tool index of the AsgSelectionWithVertexTools and the
  /// starting index of the corresponding CutBookKeeper inside the CutBookkeeperContainer.
  std::vector<std::size_t> m_selWPVToolIdxOffset;

  /// Store the index of the CutBookKeeper in the CutBookkeeperContainer for the
  /// selection using the ExpressionParser
  std::size_t m_idxSelParster;

  /// @}

};

// Include the templated code here. This must be done from this header file.
#include "ParticleSelectionAlg.icc"


#endif //> !EVENTUTILS_PARTICLESELECTIONALG_H
