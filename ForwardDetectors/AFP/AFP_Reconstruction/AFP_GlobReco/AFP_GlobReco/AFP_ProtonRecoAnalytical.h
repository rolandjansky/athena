/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GLOBRECO_AFP_PROTONRECOANALYTICAL_H
#define AFP_GLOBRECO_AFP_PROTONRECOANALYTICAL_H

#include "AFP_GlobReco/AFP_ProtonRecoBase.h"
#include "AFP_GlobReco/Parameterization.h"

// FrameWork includes
#include "Gaudi/Property.h"
#include "PathResolver/PathResolver.h"

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
#include <memory>
#include <algorithm>


/// Tool for proton reconstruction by directly using transport parameterisation.
class AFP_ProtonRecoAnalytical : public AFP_ProtonRecoBase {

  public:
    /// Default constructor
    AFP_ProtonRecoAnalytical(const std::string& type, const std::string& name, const IInterface* parent);
    
    ~AFP_ProtonRecoAnalytical() = default;
    
    /// Loads parameterization
    StatusCode initialize() override;

  private:
    
    /// @copydoc AFP::ITool::configInfo()
    StatusCode configInfo() const;
    
    /// @brief Reconstructs single proton from pair of tracks
    ///
    /// * Sets up measurement and calculates slopes and postitions
    /// * Reconstructs energy of proton with bisection method
    /// * Calculates initial proton slopes
    /// * Adds proton to outputContainer and sets its properties
    /// 
    /// @return Pointer to reconstructed AFPProton
    virtual
    xAOD::AFPProton * reco(const xAOD::AFPTrack* trkNear, const xAOD::AFPTrack* trkFar, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const override;

    /// Reconstructs single proton using only one track from far station
    virtual
    xAOD::AFPProton * reco (const xAOD::AFPTrack* trkFar, std::unique_ptr<xAOD::AFPProtonContainer>& outputContainer) const override;

    /// Calculates chi2 for reconstructed proton
    virtual
    double chi2(double energy, double sx, double sy, const Measurement& my_measAFP) const override;

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
};


#endif
