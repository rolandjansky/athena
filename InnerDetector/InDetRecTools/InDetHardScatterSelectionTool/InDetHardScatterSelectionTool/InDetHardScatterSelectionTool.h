/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETHARDSCATTERSELECTIONTOOL_INDETHARDSCATTERSELECTIONTOOL_H
#define INDETHARDSCATTERSELECTIONTOOL_INDETHARDSCATTERSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AthLinks/ElementLink.h"
#include "GaudiKernel/ToolHandle.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"

// EDM include(s):
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODEventInfo/EventInfo.h"

// Tool include(s)
#include "InDetRecToolInterfaces/IInDetHardScatterSelectionTool.h"

// STL includes:
#include <memory>
#include <string>

// Forward declaration(s):
namespace InDet {
  class IInDetTrackSelectionTool;
}

/**
 * @class InDetHardScatterSelectionTool
 * Class for hardscatter vertex selection using sumpt2 or sumpt
 */

namespace InDet {

  /// Implementation of the hardscatter selection tool
  class InDetHardScatterSelectionTool
    : public asg::AsgTool,
      public virtual IInDetHardScatterSelectionTool
  {

    ASG_TOOL_CLASS(InDetHardScatterSelectionTool, IInDetHardScatterSelectionTool)

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /// @name Running mode(s) for hardscatter determination
    /// @{

    enum Mode {
      SumPt2 = 0,
      SumPt  = 1
    };

    /// @}

    /// @name Helper class for summing pt2 or pt

    class Sum {
    public:
      virtual ~Sum() = default;
      virtual float add(const float a, const float b) const = 0;
    };

    /// @}

    /// @name Constructor(s)/destructor(s)
    /// @{

    /// Constructor
    InDetHardScatterSelectionTool(const std::string& name);

    /// Destructor
    ~InDetHardScatterSelectionTool();

    /// @}

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{

    /// Function initialising the tool
    virtual StatusCode initialize() override;

    /// Function finalizing the tool
    virtual StatusCode finalize() override;
    
    /// @}
    
    /// @name Function(s) implementing the IInDetHardScatterSelectionTool interface
    /// @{

    /// Function for returning the hardscatter vertex
    const xAOD::Vertex* getHardScatter(const xAOD::VertexContainer* vtxCont) const override;
    const xAOD::Vertex* getHardScatter() const override;

    /// Function for returning the hardscatter vertex link
    ElementLink<xAOD::VertexContainer> getHardScatterLink(const xAOD::VertexContainer* vtxCont) const override;
    ElementLink<xAOD::VertexContainer> getHardScatterLink() const override;

    /// @}

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
   private:

    /// @name The properties that can be defined via the python job options
    /// @{

    /// Redo the hardscatter vertex determination (i.e., don't simply return the zeroth element of the vertex container)
    bool m_redoHardScatter;

    /// Running mode for the hardscatter determination (if m_redoHardScatter is specified to be true)
    int m_mode;

    /// ToolHandle for the InDetTrackSelectionTool (to optionally select on the vertices' used-in-fit tracks)
    ToolHandle<IInDetTrackSelectionTool> m_trkSelectTool;

    /// If true, return the hardscatter as the vertex decorated with some boolean
    bool m_returnDeco;

    /// The boolean decoration indicating which vertex is hardscatter
    std::string m_hardScatterDeco;

    /// Name of the primary vertex container (needed for container-less function calls)
    std::string m_vtxContName;

    /// @}

  private:

    /// @name Truly private internal data members
    /// @{

    /// A helper class which either sums pt2 or pt
    std::unique_ptr<Sum> m_sum;

    /// A flag which will be true if an InDetTrackSelectionTool was provided
    bool m_doTrkSelection;

    /// xAOD::EventInfo ReadHandleKey
    SG::ReadHandleKey<xAOD::EventInfo> m_evtInfoKey {this, "EventInfo", "EventInfo", "EventInfo key"};

    /// The decoration name of the ElementLink to the hardscatter vertex (applied to xAOD::EventInfo)
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_hardScatterDecoKey;

    /// ReadHandleKey for our primary vertex container
    SG::ReadHandleKey<xAOD::VertexContainer> m_vtxContKey;

    /// @}

  }; // class: InDetHardScatterSelectionTool
} // namespace: InDet

#endif // end: INDETHARDSCATTERSELECTIONTOOL_INDETHARDSCATTERSELECTIONTOOL_H
