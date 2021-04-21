/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
MuonCaloEnergyTool.h  -  Description
-------------------
begin   : Summer 2014
authors : Niels van Eldik (CERN PH-ATC)
***************************************************************************/
#ifndef MUONCALOENERGYTOOL_H
#define MUONCALOENERGYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloConditions/CaloNoise.h"
#include "GaudiKernel/ToolHandle.h"
#include "ParticleCaloExtension/ParticleCellAssociation.h"
#include "PathLengthUtils.h"
#include "RecoToolInterfaces/IMuonCaloEnergyTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrackToCalo/CaloCellSelectorLayerdR.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"

namespace Rec {
    class IParticleCaloCellAssociationTool;
}

namespace Trk {
    class IParticleCaloExtensionTool;
    class ITrackParticleCreatorTool;
    class CaloExtension;
}  // namespace Trk

namespace Rec {

    class MuonCaloEnergyTool : virtual public IMuonCaloEnergyTool, public AthAlgTool {
    public:
        MuonCaloEnergyTool(const std::string&, const std::string&, const IInterface*);

        virtual ~MuonCaloEnergyTool();

        virtual StatusCode initialize() override;
     
        void calculateMuonEnergies(const Trk::Track* trk, double deltaE, double meanIoni, double sigmaIoni, double& E, double& sigma,
                                   double& E_FSR, double& E_expected, double& E_em_meas, double& E_em_exp, double& E_tile_meas,
                                   double& E_tile_exp, double& E_HEC_meas, double& E_HEC_exp, double& E_dead_exp,
                                   std::vector<Identifier>* crossedCells = 0, std::vector<double>* sigmaNoise_cell = 0,
                                   std::vector<double>* E_exp_cell = 0) const override;

    private:
        ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool;              //!< Tool to make the step-wise extrapolation
        ToolHandle<Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool;  //!< Tool to make the step-wise extrapolation
        ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator;                 /**< The CB Particle Creator Tool */

        SG::ReadCondHandleKey<CaloNoise> m_caloNoiseCDOKey{this, "CaloNoiseKey", "totalNoise", "SG Key of CaloNoise data object"};

        // DATA MEMBERS
        double m_sigmasAboveNoise;  // 4.
        double m_emEtCut;           // 2.5 GeV
        double m_emF1Cut;           // 0.15
        const double m_emipEM;      // 0.78
        const double m_emipTile;    // 0.86
        const double m_emipHEC;     // 0.94

        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_indetTrackParticleLocation{this, "TrackParticleLocation", "InDetTrackParticles",
                                                                                     "ID track particles"};
        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_muonTrackParticleLocation{this, "MuonSpectrometerTrackPArticleLocation",
                                                                                    "MuonSpectrometerTrackParticles", "MS track particles"};

        // PRIVATE METHODS
        double thresholdCorrection(double E_observed, double E_expected, double sigma_Noise) const;
        double etaCorr(double eta) const;
    };

}  // namespace Rec

#endif
