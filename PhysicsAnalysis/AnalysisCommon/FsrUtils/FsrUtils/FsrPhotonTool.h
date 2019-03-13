/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FSRUTILS_FsrPhotonTool_H
#define FSRUTILS_FsrPhotonTool_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"

// Local include(s):
#include "FsrUtils/IFsrPhotonTool.h"

namespace CP
{
    class IIsolationSelectionTool;
    class IIsolationCorrectionTool;
    class IIsolationCloseByCorrectionTool;
    class IEgammaCalibrationAndSmearingTool;
}

namespace FSR {

    /// Implementation for the "FSR" provider tool
    ///
    /// @author Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
    ///
    /// $Date: 2014-07-18 $

    class FsrPhotonTool : public virtual IFsrPhotonTool,
                          public asg::AsgTool {

        /// Create a proper constructor for Athena
        ASG_TOOL_CLASS( FsrPhotonTool, FSR::IFsrPhotonTool )

        public:
        /// Create a constructor for standalone usage
        FsrPhotonTool( const std::string& name );

        /// Create a destructor
        ~FsrPhotonTool();

        /// @name Function(s) implementing the asg::IAsgTool interface
        /// @{

        /// Function initialising the tool
        virtual StatusCode initialize();

        /// @}

        /// @name Function(s) implementing the IFsrPhotonTool interface
        /// @{

        /// Get the "FSR candidate" as a return value for a muon (collinar and far FSR)
        virtual CP::CorrectionCode getFsrPhoton(const xAOD::IParticle* part, FsrCandidate& candidate,
                                                const xAOD::PhotonContainer* photons,
                                                const xAOD::ElectronContainer* electrons);

        /// Find ALL FSR candidates for a given particle (electron or muon)
        ///   providing newly calibrated photon and electron containers
        virtual std::vector<FsrCandidate>* getFsrCandidateList(const xAOD::IParticle* part,
                                                               const xAOD::PhotonContainer* photons,
                                                               const xAOD::ElectronContainer* electrons);

        /// Find and Return ALL FAR FSR candidates
        virtual std::vector<FsrCandidate>* getFarFsrCandidateList(const xAOD::IParticle* part,
                                                                  const xAOD::PhotonContainer* photons_cont);

        /// Find and Return ALL NEAR FSR candidates
        virtual std::vector<FsrCandidate>* getNearFsrCandidateList(const xAOD::Muon* part,
                                                                   const xAOD::PhotonContainer* photons_cont,
                                                                   const xAOD::ElectronContainer* electrons_cont);

        /// @}
    private:
        /// Need for the FSR search
        std::vector<FsrCandidate>* sortFsrCandidates( std::vector< std::pair <const xAOD::IParticle*, double> > FsrCandList,
                                                      std::string option="ET");
        bool isOverlap(const xAOD::Electron_v1* electron, std::vector< std::pair <const xAOD::IParticle*, double> > phfsr,
                       unsigned int nofPhFsr);
        double deltaR(float muonEta, float muonPhi, float phEta, float phPhi) const;
        double deltaPhi(float phi1, float phi2) const;
        static bool compareEt(FsrCandidate c1, FsrCandidate c2) { return (c1.Et > c2.Et); }

        //const xAOD::PhotonContainer* m_photons;
        //const xAOD::ElectronContainer* m_electrons;

        double m_high_et_min;
        double m_overlap_el_ph;
        double m_overlap_el_mu;

        double m_far_fsr_drcut;
        double m_far_fsr_etcut;
        std::string m_far_fsr_isoWorkingPoint;

        double m_drcut;
        double m_etcut;
        double m_f1cut;
        double m_topo_drcut;
        double m_topo_f1cut;


        std::vector<FsrCandidate> m_fsrPhotons;
        FsrCandidate::FsrType     m_fsr_type;

        ToolHandle<CP::IIsolationSelectionTool>           m_isoSelTool;
        ToolHandle<CP::IIsolationCorrectionTool>          m_isoCorrTool;
        ToolHandle<CP::IIsolationCloseByCorrectionTool>   m_isoCloseByCorrTool;
        std::string                                       m_energyRescalerName;
        ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_energyRescaler;

        xAOD::PhotonContainer*    m_photonContainer;
        xAOD::PhotonAuxContainer* m_photonAuxContainer;
        xAOD::Photon*             m_photon;
        
    }; // class FsrPhotonTool

} // namespace FSR

#endif // FSRUTILS_FsrPhotonTool_H
