/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_PhotonVertexSelectionTool_h
#define PhotonVertexSelection_PhotonVertexSelectionTool_h

// Framework includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

// EDM includes
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
    asg::AnaToolHandle<CP::IPhotonPointingTool> m_pointingTool;

    /// MVA readers
    TMVA::Reader *m_mva1;
    TMVA::Reader *m_mva2;

    /// MVA attached discriminating variables
    float m_sumPt2;
    float m_sumPt;
    float m_deltaPhi;
    float m_deltaZ;
    
    /// Last case treated (see below)
    int m_case;

    /// Store results of MVA output
    std::vector<std::pair<const xAOD::Vertex*, float> > m_vertexMLP;

  private:
    /// Get combined 4-vector of photon container
    TLorentzVector getEgammaVector(const xAOD::EgammaContainer *egammas) const;

    /// Sort MLP results
    static bool sortMLP(const std::pair<const xAOD::Vertex*, float> &a, const std::pair<const xAOD::Vertex*, float> &b);
    

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
    
    /// Given a list of photons, return the most likely vertex based on MVA likelihood
    StatusCode getVertex(const xAOD::EgammaContainer &egammas, const xAOD::Vertex* &vertex, bool ignoreConv = false);

    /// Given a list of photons, return the MLPs of all vertices in the event
    std::vector<std::pair<const xAOD::Vertex*, float> >& getVertex(const xAOD::EgammaContainer &egammas, bool ignoreConv = false);
    
    /// Return the last case treated:
    /// 0=conv track associated to vertex,
    /// 1=at least one conv track with Si hits, 
    /// 2=no tracks with Si hits or conversions ignored
    int getCase() const { return m_case; }
    
    /// Get possible vertex directly associated with photon conversions
    const xAOD::Vertex* getPrimaryVertexFromConv(const xAOD::PhotonContainer *photons) const;

    /// Get access to the photon pointing tool
    const CP::IPhotonPointingTool* photonPointingTool() const;

    /// @}

  }; // class PhotonVertexSelectionTool

} // namespace CP


#endif // PhotonVertexSelection_PhotonVertexSelectionTool_h
