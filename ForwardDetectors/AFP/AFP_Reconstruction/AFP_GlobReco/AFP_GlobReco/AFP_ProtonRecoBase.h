/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBRECO_AFP_PROTONRECOBASE_H
#define AFP_GLOBRECO_AFP_PROTONRECOBASE_H

#include "AFP_GlobReco/IAFP_ProtonRecoTool.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "StoreGate/ReadHandleKey.h"

// xAOD includes
#include "xAODForward/AFPProton.h"
#include "xAODForward/AFPProtonContainer.h"
#include "xAODForward/AFPProtonAuxContainer.h"
#include "xAODForward/AFPTrack.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"
#include "xAODForward/AFPProtonRecoAlgID.h"

#include <string>
#include <vector>
#include <iterator>
#include <algorithm>



/// @brief Base class for all proton reconstruction tools.
///
class AFP_ProtonRecoBase : public extends<AthAlgTool, IAFP_ProtonRecoTool> {

  public:
    /// Default constructor
    AFP_ProtonRecoBase(const std::string& type, const std::string& name, const IInterface* parent);
        
    // return name of the output container; needed by AFP_GlobRecoTool to make write handle keys
    const std::string& outputContainerName () const override {return m_protonsContainerName;}
    
    // find proton candidates and save them; fitting is done in the derived class
    StatusCode doProtonReco(std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer, const EventContext& ctx) const override;

  protected:

    /// Local class for storing tracks positions
    struct Measurement {
      /// Default constructor - sets everything to zero
      Measurement() : xN {0.}, yN {0.}, xF {0.}, yF {0.} {}

      /// Takes xy positions of two tracks in [mm]
      /// and stores them in [m]
      Measurement(double xNear, double yNear, double xFar, double yFar)
          : xN {xNear * 1e-3}, yN {yNear * 1e-3}, xF {xFar * 1e-3}, yF {yFar * 1e-3} {}

      /// @name Near tracks positions [m]
      ///@{
      double xN, yN;
      ///@}

      /// @name Far tracks positions [m]
      ///@{
      double xF, yF;
      ///@}
    };


    Gaudi::Property<std::vector<double> > m_detectorPositions{this, "detectorPositions", {}, "absolute values of detector positions for each station on one side"};
    
    Gaudi::Property<int> m_side{this, "side", 0, "side id, A=0, C=1"};
  
    Gaudi::Property<double> m_trackDistance{this, "trackDistance", 2.0, "Maximum distance between tracks in the near and the far station on xy-plane"};

    Gaudi::Property<bool> m_allowSingleStationReco{this, "allowSingleStationReco", false, "Switch for allowing proton reconstruction using only far station"};
  
    SG::ReadHandleKey<xAOD::AFPTrackContainer> m_trackContainerKey{this, "AFPTrackContainerKey", "AFPTrackContainer", "Name of the container with tracks of hits from which protons are to be reconstructed"};
  
    Gaudi::Property<std::string> m_protonsContainerName{this, "protonsContainerName", "AFPProtonContainer", "Name of the container in which protons are saved"};
  
    /// 3-momentum of reconstructed proton
    using Momentum = std::array<double, 3>;

    /// Creates and sets up a proton
    xAOD::AFPProton * createProton (const Momentum& momentum, const Measurement& my_measAFP, const int algID, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const;

    /// Links track pair to reconstructed proton
    void linkTracksToProton (const xAOD::AFPTrack* track, SG::ReadHandle<xAOD::AFPTrackContainer>& trackContainer, xAOD::AFPProton * proton) const;

    /// Default position of AFP near station
    double m_detectorPositionNear;  // [m]

    /// Default position of AFP far station
    double m_detectorPositionFar;  // [m]

    /// x-Sigma value
    static constexpr double m_xSigma = 10e-6; // [m]

    /// y-Sigma value
    static constexpr double m_ySigma = 30e-6;

    /// Vertex position
    // TODO: get it from PV or BS?
    const std::vector<double> m_vertexIP = {0, 0, 0};
    
    // the reco methods should be defined in the derived class
    virtual xAOD::AFPProton * reco(const xAOD::AFPTrack* /*trkNear*/, const xAOD::AFPTrack* /*trkFar*/, std::unique_ptr<xAOD::AFPProtonContainer>& /*outputContainer*/) const
    {
      ATH_MSG_ERROR("this is reco(trkNear, trkFar, outputContainer) in AFP_ProtonRecoBase, shall not be used, returning 0");
      return 0;
    }
    virtual xAOD::AFPProton * reco (const xAOD::AFPTrack* /*trkFar*/, std::unique_ptr<xAOD::AFPProtonContainer>& /*outputContainer*/) const
    {
      ATH_MSG_ERROR("this is reco(trkFar, outputContainer) in AFP_ProtonRecoBase, shall not be used, returning 0");
      return 0;
    }

    // chi2 method should be defined in the derived class
    virtual double chi2 (double /*energy*/, double /*sx*/, double /*sy*/, const Measurement& /*my_measAFP*/) const
    {
      ATH_MSG_ERROR("this is chi2(energy, sx, sy, sz, my_measAFP) in AFP_ProtonRecoBase, shall not be used, returning 0");
      return 0.;
    }

};


#endif
