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
  /// Takes a list of photons (for example, to two leading photons), decorates them and
  /// calculates the most likely primary vertex, based on an MVA.
  ///
  /// @author Christopher Meyer <chris.meyer@cern.ch>
  /// @author Bruno Lenzi <bruno.lenzi@cern.ch>
  /// @author Davide Mungo <davide.pietro.mungo at cern.ch>
  ///
  /// WARNING: THIS TOOL AND ALL ITS CLIENTS SHOULD BE REWRITTEN TO COPE WITH REL22 STANDARDS
  ///         - Cannot cache anymore objects from the event store inside member variables
  ///         - Cannot used mthod to retrieve the result of the last call, like getFail

  class PhotonVertexSelectionTool : public virtual IPhotonVertexSelectionTool,
                                    public asg::AsgTool {

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS(PhotonVertexSelectionTool, CP::IPhotonVertexSelectionTool)

  private:
    // enum indicating where the tool has failed
    enum FailType {
      Unkown        = -99,  // Init value
      NoFail        =   0,  // Ok to run the MVA algorithm
      NoVxCont      =   1,  // No vertex container
      NoEventInfo   =   2,  // No EventInfo
      FailPointing  =   3,  // Calo pointing failed
      FailEgamVect  =   4,  // No diphoton event
      NoGdCandidate =   5,  // Pointing succeded but too distant from any other vertex
      MatchedTrack  =   6,  // Conversion photon has a track attached to a primary/pileup vertex
    };


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

    /// Vertex container
    const xAOD::VertexContainer* m_vertices;

    /// Accessors
    std::unique_ptr<const SG::AuxElement::ConstAccessor<float> > m_acc_sumPt;
    std::unique_ptr<const SG::AuxElement::ConstAccessor<float> > m_acc_sumPt2;
    std::unique_ptr<const SG::AuxElement::ConstAccessor<float> > m_acc_deltaPhi;
    std::unique_ptr<const SG::AuxElement::ConstAccessor<float> > m_acc_deltaZ;

    /// MVA readers
    std::shared_ptr<TMVA::Reader> m_mva1;
    std::shared_ptr<TMVA::Reader> m_mva2;

    /// MVA attached discriminating variables
    float m_sumPt2;
    float m_sumPt;
    float m_deltaPhi;
    float m_deltaZ;
    
    /// Last case treated (see below)
    int m_case;

    /// If MVA variables has been decorated
    bool m_varsDecorated;

    /// Last fail encountered
    FailType m_fail;

    /// Store results of MVA output
    std::vector<std::pair<const xAOD::Vertex*, float> > m_vertexMLP;

  private:
    /// Get combined 4-vector of photon container
    TLorentzVector getEgammaVector(const xAOD::EgammaContainer *egammas);

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

    /// Calculate and decorate vertex with MVA variables
    StatusCode decorateInputs(const xAOD::EgammaContainer &egammas);
    
    /// Given a list of photons, return the most likely vertex based on MVA likelihood
    StatusCode getVertex(const xAOD::EgammaContainer &egammas, const xAOD::Vertex* &vertex, bool ignoreConv = false);

    /// Given a list of photons, return the MLPs of all vertices in the event
    std::vector<std::pair<const xAOD::Vertex*, float> >& getVertex(const xAOD::EgammaContainer &egammas, bool ignoreConv = false);
    
    /// Return the last case treated:
    /// 0=conv track associated to vertex,
    /// 1=at least one conv track with Si hits, 
    /// 2=no tracks with Si hits or conversions ignored
    int getCase() const { return m_case; }

    /// Return the last fail encountered
    int getFail() const { return m_fail; }
    
    /// Get possible vertex directly associated with photon conversions
    const xAOD::Vertex* getPrimaryVertexFromConv(const xAOD::PhotonContainer *photons) const;

    /// Get access to the photon pointing tool
    const CP::IPhotonPointingTool* photonPointingTool() const;

    /// @}

  }; // class PhotonVertexSelectionTool

} // namespace CP


#endif // PhotonVertexSelection_PhotonVertexSelectionTool_h
