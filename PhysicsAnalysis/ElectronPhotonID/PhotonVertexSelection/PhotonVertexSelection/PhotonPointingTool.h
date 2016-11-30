/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonVertexSelection_PhotonPointingTool_h
#define PhotonVertexSelection_PhotonPointingTool_h

// Framework includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "PhotonVertexSelection/IPhotonPointingTool.h"

// Forward declarations
class TH1F;
namespace CP { class ShowerDepthTool; }

namespace CP {

  /// Implementation for the photon pointing tool
  ///
  /// Takes photon shower shape and/or conversion vertex
  /// and extrapolates back to beamline
  ///
  /// @author Christopher Meyer <chris.meyer@cern.ch>
  ///
  class PhotonPointingTool : public virtual IPhotonPointingTool,
                             public asg::AsgTool {

    /// Create a proper constructor for Athena
    ASG_TOOL_CLASS(PhotonPointingTool, CP::IPhotonPointingTool)

  private:
    /// Tools
    CP::ShowerDepthTool *m_showerTool;

    /// Correction histogram
    TH1F *m_zCorrection;

    /// Static Decorators
    static SG::AuxElement::Decorator<float> zvertex;
    static SG::AuxElement::Decorator<float> errz;
    static SG::AuxElement::Decorator<float> HPV_zvertex;
    static SG::AuxElement::Decorator<float> HPV_errz;

  private:
    ///
    float getCorrectedZ(float zPointing, float etas2) const;
    bool m_isMC;
    std::string m_zOscFileMC, m_zOscFileData;

  public:
    PhotonPointingTool(const std::string &name);
    virtual ~PhotonPointingTool();

    /// @name Function(s) implementing the asg::IAsgTool interface
    /// @{
    
    /// Function initialising the tool
    virtual StatusCode initialize();
      
    /// @}

    /// @name Function(s) implementing the IPhotonPointingTool interface
    /// @{
    
    /// Add calo and conversion (HPV) pointing variables
    StatusCode updatePointingAuxdata(const xAOD::EgammaContainer &egammas) const ;

    /// Return calo pointing variables
    std::pair<float, float> getCaloPointing(const xAOD::Egamma *egamma) const ;
    
    /// Return conversion (HPV) pointing variables
    std::pair<float, float> getConvPointing(const xAOD::Photon *photon) const ;
    
    /// Return the corrected eta for primary vertex z-position
    float getCorrectedEta(const xAOD::CaloCluster &cluster, float PVz) const ;

    /// Correct Egamma vector for primary vertex z-position
    StatusCode correctPrimaryVertex(xAOD::Egamma &egamma, float PVz) const ;

    /// Get Egamma TLorentzVector corrected for primary vertex z-position
    TLorentzVector getCorrectedLV(const xAOD::Egamma &egamma, float PVz) const ;
    
    /// Return the invariant mass, using corrected Pt/Eta of each object for the
    /// given vertex z-position
    float getCorrectedMass(const xAOD::EgammaContainer &egammas, float PVz) const;

    /// @}

  }; // class PhotonPointingTool

} // namespace CP


#endif // PhotonVertexSelection_PhotonPointingTool_H
