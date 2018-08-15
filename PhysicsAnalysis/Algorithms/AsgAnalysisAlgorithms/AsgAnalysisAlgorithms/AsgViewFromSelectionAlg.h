/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_VIEW_FROM_SELECTION_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_VIEW_FROM_SELECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <xAODBase/IParticleContainer.h>
#include <SelectionHelpers/ISelectionAccessor.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <SystematicsHandles/SysWriteHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <limits>

namespace CP
{
  /// \brief create a view container based on selection decorations
  ///
  /// This is a generic algorithm that works for all object types to
  /// read our standard selection decorations and create a view
  /// container.  This avoids the need for the algorithm/tool to read
  /// the selection decoration on the input, instead it can just
  /// assume that all its input objects are meant to be processed.

  class AsgViewFromSelectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    AsgViewFromSelectionAlg (const std::string& name, 
                             ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;


    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the input collection we run on
  private:
    SysReadHandle<xAOD::IParticleContainer> m_inputHandle {
      this, "input", "", "the input collection to run on"};

    /// \brief the output view container we produce
  private:
    SysWriteHandle<xAOD::IParticleContainer> m_outputHandle {
      this, "output", "", "the output view container to produce"};

  private:
    std::vector<std::string> m_selection;

  private:
    std::vector<SelectionType> m_ignore;

    /// \brief Sort the output (view) container by pT
  private:
    bool m_sortPt {false};

    /// \brief Allow the input container to be missing
  private:
    bool m_allowMissing {false};

    /// \brief Perform a deep copy for creating the output container
  private:
    bool m_deepCopy {false};

  private:
    std::size_t m_sizeLimit {std::numeric_limits<std::size_t>::max()};

    /// the list of accessors and cut ignore list
  private:
    std::vector<std::pair<std::unique_ptr<ISelectionAccessor>,SelectionType> > m_accessors;

    /// \brief the templated version of execute for a single systematic
  private:
    template<typename Type> StatusCode
    executeTemplate (const CP::SystematicSet& sys);

    /// \brief the version of execute to find the type
  private:
    StatusCode executeFindType (const CP::SystematicSet& sys);

    /// \brief The version of execute for missing input containers
  private:
    StatusCode executeMissing (const CP::SystematicSet& sys);

  private:
    StatusCode (AsgViewFromSelectionAlg::* m_function) (const CP::SystematicSet& sys) {&AsgViewFromSelectionAlg::executeFindType};
  };
}

#endif
