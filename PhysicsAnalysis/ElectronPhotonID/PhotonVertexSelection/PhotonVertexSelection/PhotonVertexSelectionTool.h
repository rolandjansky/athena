/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_PhotonVertexSelectionTool_h
#define PhotonVertexSelection_PhotonVertexSelectionTool_h

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgDataHandles/ReadHandleKey.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h" 
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/PhotonContainer.h"

// Local includes
#include "PhotonVertexSelection/IPhotonVertexSelectionTool.h"

// Forward declarations
namespace TMVA { class Reader; }
namespace CP { class IPhotonPointingTool; }

namespace CP {

  /// Implementation for the photon vertex selection tool
  ///
  /// Takes a list of photons (for example, to two leading photons) and
  /// the most likely primary vertex, based on an MVA.
  ///
  /// @author Christopher Meyer <chris.meyer@cern.ch>
  /// @author Bruno Lenzi <bruno.lenzi@cern.ch>
  ///
  class PhotonVertexSelectionTool : public virtual IPhotonVertexSelectionTool,
                                    public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS(PhotonVertexSelectionTool, CP::IPhotonVertexSelectionTool)

  private:
    /// Configuration variables
    std::string m_configFileCase1; 
    std::string m_configFileCase2; 
    float       m_convPtCut;
    bool        m_updatePointing;
    std::string m_vertexContainerName;
    std::string m_derivationPrefix;

    /// PhotonPointingTool
    ToolHandle<CP::IPhotonPointingTool> m_pointingTool {this, "PhotonPointingTool","CP::PhotonPointingTool/PhotonVertexSelection",""};


    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer {this, "VertexContainer", "PrimaryVertices", "Vertex container name"};  
   
    /// MVA readers
    // Ideally these would be const but the main method called, EvaluateMVA, is non const.
    std::unique_ptr<TMVA::Reader> m_mva1;
    std::unique_ptr<TMVA::Reader> m_mva2;

  private:
    /// Get combined 4-vector of photon container
    TLorentzVector getEgammaVector(const xAOD::EgammaContainer *egammas, FailType& failType) const;

    /// Sort MLP results
    static bool sortMLP(const std::pair<const xAOD::Vertex*, float> &a, const std::pair<const xAOD::Vertex*, float> &b);

   
    /// Given a list of photons, return the MLPs of all vertices in the event
    StatusCode getVertexImp(const xAOD::EgammaContainer &egammas, const xAOD::Vertex* &vertex, bool ignoreConv, std::vector<std::pair<const xAOD::Vertex*, float> >&, yyVtxType& , FailType& ) const;
    
  
  public:
    PhotonVertexSelectionTool(const std::string &name);
    virtual ~PhotonVertexSelectionTool();

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{
    
    /// Function initialising the tool
    virtual StatusCode initialize();
      
    /// @}

    /// @name Function(s) implementing the IPhotonVertexSelectionTool interface
    /// @{
    
    /// Given a list of photons, decorate vertex container with MVA variables
    StatusCode decorateInputs(const xAOD::EgammaContainer &egammas, FailType* failType = nullptr) const;

    /// Given a list of photons, return the most likely vertex based on MVA likelihood
    StatusCode getVertex(const xAOD::EgammaContainer &egammas, const xAOD::Vertex* &vertex, bool ignoreConv = false) const;

    /// Given a list of photons, return the MLPs of all vertices in the event
    std::vector<std::pair<const xAOD::Vertex*, float> > getVertex(const xAOD::EgammaContainer &egammas, bool ignoreConv = false, yyVtxType* vtxCase = nullptr, FailType* failType = nullptr) const;
    
    /// Return the last case treated:
    //  Deprecated no longer use this function
    int getCase() const { return -1; }
    
    /// Get possible vertex directly associated with photon conversions
    const xAOD::Vertex* getPrimaryVertexFromConv(const xAOD::PhotonContainer *photons) const;

    /// Get access to the photon pointing tool
    const CP::IPhotonPointingTool* photonPointingTool() const;

    /// @}

  }; // class PhotonVertexSelectionTool

} // namespace CP


#endif // PhotonVertexSelection_PhotonVertexSelectionTool_h
