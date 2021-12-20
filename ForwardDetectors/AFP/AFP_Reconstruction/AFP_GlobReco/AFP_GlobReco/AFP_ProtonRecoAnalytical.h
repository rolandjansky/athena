/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBRECO_AFP_PROTONRECOTOOL_H
#define AFP_GLOBRECO_AFP_PROTONRECOTOOL_H

#include "AFP_GlobReco/IAFP_ProtonRecoTool.h"
#include "AFP_GlobReco/Parameterization.h"

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "StoreGate/ReadHandleKey.h"
#include "PathResolver/PathResolver.h"

// xAOD includes
#include "xAODForward/AFPProton.h"
#include "xAODForward/AFPProtonContainer.h"
#include "xAODForward/AFPProtonAuxContainer.h"
#include "xAODForward/AFPTrack.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"

#include <string>
#include <vector>
#include <memory>
#include <iterator>
#include <algorithm>


/// Tool for proton reconstruction by directly using transport parameterisation.
class AFP_ProtonRecoAnalytical : public extends<AthAlgTool, IAFP_ProtonRecoTool> {

  public:
    /// Default constructor
    AFP_ProtonRecoAnalytical(const std::string& type, const std::string& name, const IInterface* parent);
    
    ~AFP_ProtonRecoAnalytical() = default;
    
    /// Loads parameterization
    StatusCode initialize() override;
  
    // return name of the output container; needed by AFP_GlobRecoTool to make write handle keys
    const std::string& outputContainerName () const override {return m_protonsContainerName;}

    // TODO: put doProtonReco() in a separate "base" class
    StatusCode doProtonReco(std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer, const EventContext& ctx) const override;
    

  private:
    
    /// @copydoc AFP::ITool::configInfo()
    StatusCode configInfo() const;
    
    
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
    
    /// @brief Reconstructs single proton from pair of tracks
    ///
    /// * Sets up measurement and calculates slopes and postitions
    /// * Reconstucts energy of proton with bisection method
    /// * Calculates initial proton slopes
    /// * Adds proton to outputContainer and sets it's properties
    /// 
    /// @return Poiner to reconstucted AFPProton
    xAOD::AFPProton * reco(const xAOD::AFPTrack* trkNear, const xAOD::AFPTrack* trkFar, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const;

    /// Reconstructs single proton using only one track from far staton
    xAOD::AFPProton * reco (const xAOD::AFPTrack* trkFar, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const;

    /// Calculates root of given function
    double bisection(double (AFP_ProtonRecoAnalytical::*fun)(double,const Measurement&,std::vector<double>&, std::vector<double>&) const, const Measurement& my_measAFP, std::vector<double>& my_slopeCalculated, std::vector<double>& my_positionCalculated) const;

    /// Function obtained from parameterization equation.
    /// Used in bisection method.
    double bothStations(double energy,const  Measurement& my_measAFP, std::vector<double>& my_slopeCalculated, std::vector<double>& my_positionCalculated) const;

    /// Function obtained from parameterization equation.
    /// Used in bisection method for single station reconstruction.
    double singleStation(double energy, const Measurement& my_measAFP, std::vector<double>& my_slopeCalculated, std::vector<double>& my_positionCalculated) const;

    /// Calculates ininial slope based on measurements and reconstructed energy
    double calculateSlope(double energy, int XorY, std::vector<double>& my_slopeCalculated) const;

    /// Calculates initial horizontal slope
    /// Calls calculateSlope(energy, 0)
    double calculateXslope(double energy, std::vector<double>& my_slopeCalculated) const;

    /// Calculates initial vertical slope
    /// Calls calculateSlope(energy, 1)
    double calculateYslope(double energy, std::vector<double>& my_slopeCalculated) const;

    /// Calculates chi2 for reconstructed proton
    double chi2(double energy, double sx, double sy, const Measurement& my_measAFP) const;

    /// Pointer to parameterization
    std::unique_ptr<AFP::Parameterization> m_parametrization;

    /// Distance between near and far station
    double m_distanceBetweenStations; // [m]

    /// Name of the file containing parameterization
    Gaudi::Property<std::string> m_parametrizationFileName{this, "parametrizationFileName", "none.txt", "Name of the file containing parameterization"};

    /// Position for which parameterization was performed
    double m_parametrizationPosition; // [m]

    /// Parameterization energy
    double m_parametrizationEnergy; // [GeV]
    
// TODO: put the stuff below in a separate "base" class
    
    Gaudi::Property<std::vector<double> > m_detectorPositions{this, "detectorPositions", {}, "absolute values of detector positions for each station on one side"};
    
    Gaudi::Property<int> m_side{this, "side", 0, "side id, A=0, C=1"};
  
    Gaudi::Property<double> m_trackDistance{this, "trackDistance", 2.0, "Maximum distance between tracks in the near and the far station on xy-plane"};

    Gaudi::Property<bool> m_allowSingleStationReco{this, "allowSingleStationReco", false, "Switch for allowing proton reconstruction using only far station"};
  
    SG::ReadHandleKey<xAOD::AFPTrackContainer> m_trackContainerKey{this, "AFPTrackContainerKey", "AFPTrackContainer", "Name of the container with tracks of hits from which protons are to be reconstructed"};
  
    Gaudi::Property<std::string> m_protonsContainerName{this, "protonsContainerName", "AFPProtonContainer", "Name of the container in which protons are saved"};
  
    /// 3-momentum of reconstructed proton
    using Momentum = std::array<double, 3>;

    /// Creates and sets up a proton
    xAOD::AFPProton * createProton (const Momentum& momentum, const Measurement& my_measAFP, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const;

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

// TODO: put the stuff above in a separate "base" class
};


#endif
