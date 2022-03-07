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
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetFwd.h"
#include "xAODJet/JetContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"

// Tool include(s)
#include "InDetRecToolInterfaces/IInDetHardScatterSelectionTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"


// STL includes:
#include <memory>
#include <string>

// Forward declaration(s):
namespace InDet {
  class IInDetTrackSelectionTool;
}

/**
 * @class InDetHardScatterSelectionTool
 * Class for hardscatter vertex selection using sumpt2 or sumpt or sumptw
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
      SumPt  = 1,
      SumPtw = 2
    };

    /// @}

    /// @name Helper class for summing pt2 or pt

    class Sum {
    public:
      virtual ~Sum() = default;
      ///
      /// The method is used within a loop of tracks, takes pT of the track and previous cumulative sum [sumpT2 or sumpTw] and add the sumpt [or sumpT2 or sumpTw] to cumulative sum
      // the first argument a is previous cumulative sum, b is track_pT, third argument is jet_pT for sumpTw and fourth argument is the mindR for sumpTw
      ///
      virtual float add(const float a, const float b, const float, const float )const = 0 ;
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

    /// @name Truly private internal data members
    /// @{

    /// A helper class which either sums pt2 or pt or ptw
    std::unique_ptr<Sum> m_sum;

    /// A flag which will be true if an InDetTrackSelectionTool was provided
    bool m_doTrkSelection;

    /// xAOD::EventInfo ReadHandleKey
    SG::ReadHandleKey<xAOD::EventInfo> m_evtInfoKey {this, "EventInfo", "EventInfo", "EventInfo key"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_vtxContKey{this, "VertexContainer", "PrimaryVertices", "Name of the primary vertex container"};
    SG::ReadHandleKey<xAOD::JetContainer> m_jetContKey{ this, "JetContainer", "AntiKt4EMTopoJets", "Name of the jet container" };

    BooleanProperty m_redoHardScatter{ this, "RedoHardScatter",  false,  "If true, redo the hardscatter selection using sumpt2 or sumpt or sumptw"};
    IntegerProperty m_mode{ this, "SelectionMode", InDet::InDetHardScatterSelectionTool::Mode::SumPt2, "Mode for selecting the hardscatter: sumpt2[0] or sumpt[1] or sumptw[2]"};
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectTool{this, "TrackSelectionTool", "InDet::InDetTrackSelectionTool/TrackSelectionTool", "Track selection tool to use"};
    BooleanProperty m_returnDeco{ this, "ReturnDeco", false,  "If true, return the hardscatter as the vertex decorated with some boolean"};
    StringProperty m_hardScatterDeco{this, "HardScatterLinkDeco", "hardScatterVertexLink", "The decoration name of the ElementLink to the hardscatter vertex (applied to xAOD::EventInfo)"};
    FloatProperty m_jetMinPt{this, "jetMinPt", 15000.0, "minimum jet pt"};
    FloatProperty m_jetTrkMaxDr{this, "jetTrkMaxDr", 0.8, "max dR between jet & Trk"};
    FloatProperty m_trkMaxPt{this, "trkMaxPt", 10045000, "max track pt"};
 
    /// The decoration name of the ElementLink to the hardscatter vertex (applied to xAOD::EventInfo)
    SG::ReadDecorHandleKey<xAOD::EventInfo> m_hardScatterDecoKey;

    /// @}

  }; // class: InDetHardScatterSelectionTool
} // namespace: InDet

#endif // end: INDETHARDSCATTERSELECTIONTOOL_INDETHARDSCATTERSELECTIONTOOL_H
