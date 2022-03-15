/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidCaloEnergyTool
//  AlgTool for estimating the energy loss of a muon and its error.
//  The estimation can be based on the parameterization of the energy loss,
//  on the measurement of the calorimeter or a combination of the two (the
//  hybrid method).
//
//  Please refer to:
//  K. Nikolopoulos, D. Fassouliotis, C. Kourkoumelis, and A. Poppleton,
//  "Event-by-Event Estimate of Muon Energy Loss in ATLAS",
//  IEEE Trans. Nucl. Sci., vol. 54, no. 5, pt. 2, pp. 1792-1796, Oct. 2007.
//
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuidCaloEnergyTool.h"

#include <cmath>

#include "AthenaKernel/Units.h"
#include "MuidEvent/CaloMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"
#include "MuidInterfaces/IMuidTrackIsolation.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "muonEvent/CaloEnergy.h"

namespace Units = Athena::Units;

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

namespace Rec {

    MuidCaloEnergyTool::MuidCaloEnergyTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent),
        m_cosmics(false),
        m_energyLossMeasurement(true),
        m_forceIsolationFailure(false),
        m_FSRtreatment(true),
        m_MOPparametrization(true),
        m_trackIsolation(false),
        m_emEtCut(2.5 * Units::GeV),
        m_emF1Cut(0.15),
        m_emMinEnergy(2. * Units::GeV),
        m_hecMinEnergy(10. * Units::GeV),
        m_maxNTracksIso(2),
        m_minFinalEnergy(0. * Units::GeV),
        m_minMuonPt(15. * Units::GeV) {
        declareInterface<IMuidCaloEnergy>(this);
        declareProperty("Cosmics", m_cosmics);
        declareProperty("EnergyLossMeasurement", m_energyLossMeasurement);
        declareProperty("ForceIsolationFailure", m_forceIsolationFailure);
        declareProperty("FSRtreatment", m_FSRtreatment);
        declareProperty("MopParametrization", m_MOPparametrization);
        declareProperty("TrackIsolation", m_trackIsolation);
        declareProperty("EmEtCut", m_emEtCut);
        declareProperty("EmF1Cut", m_emF1Cut);
        declareProperty("EmMinEnergy", m_emMinEnergy);
        declareProperty("HecMinEnergy", m_hecMinEnergy);
        declareProperty("MaxNTracksIso", m_maxNTracksIso);
        declareProperty("MinFinalEnergy", m_minFinalEnergy);
        declareProperty("MinMuonPt", m_minMuonPt);
    }

    MuidCaloEnergyTool::~MuidCaloEnergyTool() = default;

    //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

    StatusCode MuidCaloEnergyTool::initialize() {
        if (m_energyLossMeasurement) {
            ATH_MSG_INFO("Initializing MuidCaloEnergyTool - measured calorimeter energy deposition ");
        } else {
            ATH_MSG_INFO("Initializing MuidCaloEnergyTool - parametrized calorimeter energy deposition ");
        }

        // get the Tools
        ATH_CHECK(m_caloParamTool.retrieve());
        ATH_MSG_DEBUG("Retrieved tool " << m_caloParamTool);

        if (m_energyLossMeasurement) {
            ATH_CHECK(m_caloMeasTool.retrieve());
            ATH_MSG_DEBUG("Retrieved tool " << m_caloMeasTool);

            if (m_trackIsolation) {
                ATH_CHECK(m_trackIsolationTool.retrieve());
                ATH_MSG_DEBUG("Retrieved tool " << m_trackIsolationTool);
            } else {
                ATH_MSG_WARNING(" Using energy measurement without trackIsolation ");
                m_trackIsolationTool.disable();
            }
        } else {
            m_trackIsolationTool.disable();
            m_caloMeasTool.disable();
        }

        return StatusCode::SUCCESS;
    }

    StatusCode MuidCaloEnergyTool::finalize() {
        ATH_MSG_INFO("Finalizing MuidCaloEnergyTool."
                     << "  Tracks used mean: " << m_countMean << ",  tracks used mop: " << m_countMop
                     << ",  tracks used measurement: " << m_countMeasurement);

        return StatusCode::SUCCESS;
    }
    std::unique_ptr<CaloEnergy> MuidCaloEnergyTool::energyLoss(const EventContext& ctx, double trackMomentum, double eta,
                                                               double phi) const {
        // debug
        ATH_MSG_VERBOSE("Muon with : p = " << trackMomentum / Units::GeV << " Phi = " << phi << " Eta =  " << eta);
        std::unique_ptr<CaloEnergy> caloEnergy;
        if (m_energyLossMeasurement) {
            // Energy Dep/Iso from calorimeters (projective assumption)
            std::unique_ptr<CaloMeas> caloMeas{m_caloMeasTool->energyMeasurement(ctx, eta, phi, eta, phi)};
            if (caloMeas) { caloEnergy = measurement(ctx, trackMomentum, eta, phi, *caloMeas); }
        }

        if (!caloEnergy) {
            if (m_MOPparametrization) {
                ++m_countMop;
                caloEnergy.reset(m_caloParamTool->mopParametrizedEnergy(trackMomentum, eta, phi));
                ATH_MSG_VERBOSE("Selected the Mop Parametrization value! ==> ");
            } else {
                ++m_countMean;
                caloEnergy.reset(m_caloParamTool->meanParametrizedEnergy(trackMomentum, eta, phi));
                ATH_MSG_VERBOSE("Selected the Mean Parametrization value! ==> ");
            }
        }

        if (msgLvl(MSG::DEBUG)) {
            std::string eLossType = "  no Calo !!";
            switch (caloEnergy->energyLossType()) {
                case CaloEnergy::Parametrized: eLossType = "Parametrized"; break;
                case CaloEnergy::NotIsolated: eLossType = "NotIsolated "; break;
                case CaloEnergy::MOP: eLossType = "MOP         "; break;
                case CaloEnergy::Tail: eLossType = "Tail        "; break;
                case CaloEnergy::FSRcandidate: eLossType = "FSRcandidate"; break;
                default: break;
            };
            ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                          << " energyLoss with"
                          << " momentum =" << std::setw(6) << std::setprecision(1) << trackMomentum / Units::GeV
                          << "  phi =" << std::setw(6) << std::setprecision(3) << phi << "  eta =" << std::setw(6) << std::setprecision(3)
                          << eta << ".  CaloEnergy: deltaE = " << std::setw(8) << std::setprecision(3) << caloEnergy->deltaE() / Units::GeV
                          << "  +" << std::setw(5) << std::setprecision(3) << caloEnergy->sigmaPlusDeltaE() / Units::GeV << "  -"
                          << std::setw(5) << std::setprecision(3) << caloEnergy->sigmaMinusDeltaE() / Units::GeV << " (" << std::setw(5)
                          << std::setprecision(3) << caloEnergy->sigmaDeltaE() / Units::GeV << ") GeV,  CaloEnergy::Type " << eLossType);
        }

        return caloEnergy;
    }
    std::unique_ptr<Trk::TrackStateOnSurface> MuidCaloEnergyTool::trackStateOnSurface(const EventContext& ctx,
                                                                                      const Trk::TrackParameters& middleParameters,
                                                                                      const Trk::TrackParameters* innerParameters,
                                                                                      const Trk::TrackParameters* outerParameters) const {
        ATH_MSG_VERBOSE("Muon with : p = " << middleParameters.momentum().mag() / Units::GeV << " Phi = "
                                           << middleParameters.position().phi() << " Eta =  " << middleParameters.position().eta());

        std::unique_ptr<CaloEnergy> caloEnergy;
        if (m_energyLossMeasurement) {
            // energy deposition according to the calo measurement
            const double eta = middleParameters.position().eta();
            const double phi = middleParameters.position().phi();
            const double etaEM = innerParameters ? innerParameters->position().eta() : eta;
            const double phiEM = innerParameters ? innerParameters->position().phi() : phi;
            const double etaHad = outerParameters ? outerParameters->position().eta() : eta;
            const double phiHad = outerParameters ? outerParameters->position().phi() : phi;
            std::unique_ptr<CaloMeas> caloMeas{m_caloMeasTool->energyMeasurement(ctx, etaEM, phiEM, etaHad, phiHad)};
            if (caloMeas) { caloEnergy = measurement(ctx, middleParameters.momentum().mag(), eta, phi, *caloMeas); }
        }

        if (!caloEnergy) {
            // parametrized energy deposition
            ATH_MSG_DEBUG("Calculate parametrized energy loss");
            caloEnergy =
                energyLoss(ctx, middleParameters.momentum().mag(), middleParameters.position().eta(), middleParameters.position().phi());
            /// WARN in case of anomalously high loss
            if (caloEnergy->deltaE() > 8. * Units::GeV && middleParameters.momentum().mag() < 500. * Units::GeV)
                ATH_MSG_WARNING(" high parametrized energy loss: " << caloEnergy->deltaE() / Units::GeV << " GeV"
                                                                   << "   for p " << middleParameters.momentum().mag() / Units::GeV
                                                                   << " GeV"
                                                                   << "   eta " << middleParameters.position().eta());
        }
        // create MEOT owning CaloEnergy
        std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> typePattern(0);
        typePattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
        /// The Trk::MaterialEffectsOnTrack contain an observer pointer of the associated surface instead of
        /// an real copy. Given that the method does no longer take ownership. Let's clone the Track parameters here,
        /// which have a real copy and pipe their surface to the ctor.

        std::unique_ptr<Trk::TrackParameters> middle_clone = middleParameters.uniqueClone();
        /// Cache the pointer for the debug message
        const CaloEnergy* calo_observer = caloEnergy.get();
        const Trk::MaterialEffectsOnTrack* materialEffects =
            new const Trk::MaterialEffectsOnTrack(0., caloEnergy.release(), middle_clone->associatedSurface(), typePattern);

        // create TSOS
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> pattern(0);
        pattern.set(Trk::TrackStateOnSurface::CaloDeposit);

        // debugging
        if (msgLvl(MSG::DEBUG)) {
            std::string eLossType = "  no Calo !!";
            switch (calo_observer->energyLossType()) {
                case CaloEnergy::Parametrized: eLossType = "Parametrized"; break;
                case CaloEnergy::NotIsolated: eLossType = "NotIsolated "; break;
                case CaloEnergy::MOP: eLossType = "MOP         "; break;
                case CaloEnergy::Tail: eLossType = "Tail        "; break;
                case CaloEnergy::FSRcandidate: eLossType = "FSRcandidate"; break;
                default: break;
            };
            ATH_MSG_DEBUG(std::setiosflags(std::ios::fixed)
                          << " trackStateOnSurface with"
                          << " momentum =" << std::setw(6) << std::setprecision(1) << middleParameters.momentum().mag() / Units::GeV
                          << "  phi =" << std::setw(6) << std::setprecision(3) << middleParameters.position().phi()
                          << "  eta =" << std::setw(6) << std::setprecision(3) << middleParameters.position().eta()
                          << ".  CaloEnergy: deltaE = " << std::setw(8) << std::setprecision(3) << calo_observer->deltaE() / Units::GeV
                          << "  +" << std::setw(5) << std::setprecision(3) << calo_observer->sigmaPlusDeltaE() / Units::GeV << "  -"
                          << std::setw(5) << std::setprecision(3) << calo_observer->sigmaMinusDeltaE() / Units::GeV << " (" << std::setw(5)
                          << std::setprecision(3) << calo_observer->sigmaDeltaE() / Units::GeV << ") GeV,  CaloEnergy::Type " << eLossType);
        }
        return std::make_unique<Trk::TrackStateOnSurface>(nullptr, middle_clone.release(), nullptr, materialEffects, pattern);
    }

    //<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

    std::unique_ptr<CaloEnergy> MuidCaloEnergyTool::measurement(const EventContext& ctx, double trackMomentum, double eta, double phi,
                                                                const CaloMeas& caloMeas) const {
        // Mean Energy Loss parametrization
        std::unique_ptr<CaloEnergy> caloParamMean{m_caloParamTool->meanParametrizedEnergy(trackMomentum, eta, phi)};
        // Mop Energy Loss parametrization
        std::unique_ptr<CaloEnergy> caloParamMop{m_caloParamTool->mopParametrizedEnergy(trackMomentum, eta, phi)};
        // Mip Energy Loss
        std::unique_ptr<CaloEnergy> caloParamMip{m_caloParamTool->meanParametrizedEnergy(10. * Units::GeV, eta, phi)};
        // Mop Energy Loss peak
        std::unique_ptr<CaloEnergy> mopPeak{m_caloParamTool->mopPeakEnergy(trackMomentum, eta, phi)};

        // // flag upper hemisphere cosmic
        // bool cosmic			= false;
        // if (caloParamMop->deltaE() < 0.) cosmic = true;

        // mop energy deposition
        double MopLoss = std::abs(caloParamMop->deltaE());
        // mop energy deposition uncertainty
        double MopError = mopPeak->sigmaDeltaE();
        // mop energy deposition corrected
        double MopLossCorrected = paramCorrection(trackMomentum, eta, MopLoss, MopError);

        // percentage of inert material
        const double InertMaterial = m_caloParamTool->x0mapInertMaterial(eta);
        // percentage of em calorimeter material
        const double EmMaterial = m_caloParamTool->x0mapEmMaterial(eta);
        // percentage of hec calorimeter material
        const double HECMaterial = m_caloParamTool->x0mapHecMaterial(eta);
        // correction for the inert material
        double MaterialCorrection = InertMaterial * MopLossCorrected;
        // scale to get mop loss in em calo
        double MopLossEm = MopLoss * m_caloParamTool->emMopFraction(eta);
        // fraction of Tile used for the measurement
        const double TileMeasurementMaterial = caloMeas.Tile_SamplingFraction();
        // fraction of LArHEC used for the measurement
        const double LArHECMeasurementMaterial = caloMeas.LArHEC_SamplingFraction();
        // fraction of LArEM used for the measurement
        const double LArEmMeasurementMaterial = caloMeas.LArEM_SamplingFraction();
        // Measured energy deposition in Tile
        const double TileEnergy = caloMeas.Tile_EnergyMeasured();
        // Measured energy deposition in E/M
        const double EmEnergy = caloMeas.LArEM_EnergyMeasured();

        // Correction for forward calorimetry
        double ForwardHECCorrection = 0.;
        if (std::abs(eta) > 2. && caloMeas.LArHEC_EnergyMeasured() > 100.)
            ForwardHECCorrection = (1. - LArHECMeasurementMaterial) * HECMaterial * MopLossCorrected;
        const double LArHECEnergy = caloMeas.LArHEC_EnergyMeasured() + ForwardHECCorrection;  // Measured energy deposition in LArHEC

        double TotalMeasuredEnergy = TileEnergy + EmEnergy + LArHECEnergy;

        ATH_MSG_VERBOSE("Energy Deposition:Tile= " << TileEnergy / Units::GeV << ",LArHEC= " << LArHECEnergy / Units::GeV
                                                   << ",EM= " << EmEnergy / Units::GeV << "  ForwardHECCorrection "
                                                   << ForwardHECCorrection / Units::GeV << "  HECMaterial " << HECMaterial
                                                   << "  MopLossCorrected " << MopLossCorrected / Units::GeV);

        bool bHEC = false;  // performed HEC measurement?
        bool bEM = false;   // performed Em measurement?

        // If muon isolated, and no significant measurement is made then use the mop parameterization, else the mean
        if (std::abs(eta) < 1.4) {
            if (LArHECEnergy + TileEnergy > 0.1 * MopLoss * HECMaterial) bHEC = true;
        } else if (std::abs(eta) > 1.8) {
            if (LArHECEnergy + TileEnergy > 0.2 * MopLoss * HECMaterial) bHEC = true;
        } else {
            if (LArHECEnergy + TileEnergy > 0.25 * MopLoss * HECMaterial) bHEC = true;
        }
        if (EmEnergy > 0.5 * MopLoss * EmMaterial) bEM = true;

        if (!bHEC) {
            //	TotalMeasuredEnergy -= TileEnergy + LArHECEnergy;
            //	MaterialCorrection  += HECMaterial * MopLossCorrected;
        }
        if (!bEM) {
            //	TotalMeasuredEnergy -= EmEnergy;
            //	MaterialCorrection  += EmMaterial * MopLossCorrected;
        }

        double MeasCorrected = TotalMeasuredEnergy + MaterialCorrection;
        // Need to calculate the corresponding mip energy deposition
        // Muons of 10 GeV are already in the relativistic rise region
        // in order to obtain the mip deposition from the mean energy deposition of 10 GeV muons
        // should divide by approximately 1.4 (Review of Particle Physics Figure 27.3 p.243)
        const double IonizationLoss = (1. / 1.4) * std::abs(caloParamMip->deltaE());

        double eOverMipCorrectionEm = 0.;
        double eOverMipCorrectionHEC = 0.;

        // Etrue = emip * Emeas
        // -DE = Emeas - Etrue = Etrue ( 1./emip -1.)
        if (bEM) {
            const double emipEM = 0.78;
            eOverMipCorrectionEm = -(1. / emipEM - 1.) * IonizationLoss * EmMaterial * LArEmMeasurementMaterial;
            if (EmEnergy + eOverMipCorrectionEm < 0.) eOverMipCorrectionEm = 0.;
        }
        if (bHEC) {
            const double emipTile = 0.86;
            const double emipLAr = 0.94;
            const double HECEnergy = TileEnergy + LArHECEnergy;
            const double eOverMipCorrectionTile =
                -(1. / emipTile - 1.) * TileEnergy / HECEnergy * IonizationLoss * HECMaterial * TileMeasurementMaterial;
            const double eOverMipCorrectionLAr =
                -(1. / emipLAr - 1.) * LArHECEnergy / HECEnergy * IonizationLoss * HECMaterial * LArHECMeasurementMaterial;
            eOverMipCorrectionHEC = eOverMipCorrectionTile + eOverMipCorrectionLAr;
            if (LArHECEnergy + TileEnergy + eOverMipCorrectionHEC < 0.) eOverMipCorrectionHEC = 0.;
        }
        const double eOverMipCorrection = eOverMipCorrectionEm + eOverMipCorrectionHEC;

        //  additional offset from high-statistics Z->mumu MC (measured by Peter K
        //  30/11/2011)
        constexpr double fix1FromPeter[26] = {0.424104,   0.479637,   0.483419,  0.490242, 0.52806,  0.573582, 0.822098,
                                              0.767301,   0.809919,   0.658745,  0.157187, 0.413214, 0.771074, 0.61815,
                                              0.350113,   0.322785,   0.479294,  0.806183, 0.822161, 0.757731, -0.0857186,
                                              -0.0992693, -0.0492252, 0.0650174, 0.261538, 0.360413};
        //  (update from Peter K 09/12/2011)
        constexpr double fix2FromPeter[26] = {-0.647703, -0.303498,  -0.268645, -0.261292, -0.260152, -0.269253, -0.266212,
                                              -0.240837, -0.130172,  -0.111638, -0.329423, -0.321011, -0.346050, -0.305592,
                                              -0.313293, -0.317111,  -0.428393, -0.524839, -0.599547, -0.464013, -0.159663,
                                              -0.140879, -0.0975618, 0.0225352, 0.0701925, -0.24778};
        int ieta = static_cast<int>(std::abs(eta) / 0.10);
        if (ieta > 25) ieta = 25;
        double FinalMeasuredEnergy = MeasCorrected + eOverMipCorrection + (fix1FromPeter[ieta] + fix2FromPeter[ieta]) * Units::GeV;

        ATH_MSG_VERBOSE("Sum of cells " << (TileEnergy + EmEnergy + LArHECEnergy) / Units::GeV << " Total energy deposition "
                                        << TotalMeasuredEnergy / Units::GeV << " corrected energy deposition " << MeasCorrected / Units::GeV
                                        << " e/mip corre " << FinalMeasuredEnergy / Units::GeV << std::endl
                                        << "\nFinal Energy Measurement  = "
                                        << FinalMeasuredEnergy / Units::GeV
                                        //<< "\nMean Energy Deposition    = " << MeanLoss/Units::GeV
                                        //<< " - " << MeanErrorLeft/Units::GeV << " + "<< MeanErrorRight/Units::GeV
                                        << "\nMop Energy Deposition     = " << MopLoss / Units::GeV << " +- "
                                        << MopError / Units::GeV
                                        //<< "\nOld parametrization energy= " << m_caloParamOld->deltaE()/Units::GeV
                                        //<< " +- " <<  m_caloParamOld->sigmaDeltaE()/Units::GeV
                                        //<< "\nTrack Momentum            = "  <<  trackMomentum/Units::GeV
                                        //<< " Eta= " << eta << "  Phi= " << phi
                                        << std::endl
                                        << "Final Meas = " << FinalMeasuredEnergy / Units::GeV << " Mop Dep = " << MopLoss / Units::GeV
                                        << " +- " << MopError / Units::GeV);

        const double HECIso = caloMeas.Tile_Isolation() + caloMeas.LArHEC_Isolation();
        const double EmIso = caloMeas.LArEM_Isolation();
        const double sinTheta = 1. / std::cosh(eta);
        const double pT = trackMomentum * sinTheta * Units::MeV;

        constexpr double OneOver85 = 1. / 85;
        constexpr double FifteenGeV = 15. * Units::GeV;
        const double EmCut = m_emMinEnergy + OneOver85 * (pT - FifteenGeV);
        const double HECCut = m_hecMinEnergy;
        const double pTCut = m_minMuonPt;
        bool PassCut = true;
        if (m_forceIsolationFailure || EmIso > EmCut || HECIso > HECCut || pT < pTCut || FinalMeasuredEnergy < m_minFinalEnergy)
            PassCut = false;

        int nTracks = 0;
        // double tracksEnergy = 0.;
        if (m_trackIsolation) {
            std::pair<int, double> inner = m_trackIsolationTool->trackIsolation(ctx, eta, phi);
            nTracks = inner.first;
            // tracksEnergy	= inner.second - maxP;
            if (pT < 100. * Units::GeV && nTracks > m_maxNTracksIso) PassCut = false;
        }

        ATH_MSG_VERBOSE("pT= " << pT / Units::GeV << ",HECIso= " << HECIso / Units::GeV << ",EmIso= " << EmIso / Units::GeV
                               << ", nTracks= " << nTracks << ",PassCut= " << PassCut);

        CaloEnergy::EnergyLossType lossType = CaloEnergy::NotIsolated;
        std::unique_ptr<CaloEnergy> caloEnergy;

        // choose between lossTypes MOP, Tail, FSR and NotIsolated according
        // to measured energy, isolation cut and Et in em
        if (FinalMeasuredEnergy < MopLoss + 2. * MopError && FinalMeasuredEnergy > m_minFinalEnergy) {
            ++m_countMop;
            caloEnergy.swap(mopPeak);
        } else if (PassCut) {
            //  tail offset from high-statistics Z->mumu MC (measured by Peter K 09/12/2011),
            //  but next we try to separate any FSR contribution from the Landau tail
            double F1 = 0.;
            if (caloMeas.LArEM_EnergyMeasured() > m_emEtCut) F1 = caloMeas.LArEM_FirstCompartmentEnergy() / caloMeas.LArEM_EnergyMeasured();
            ATH_MSG_VERBOSE(" start Tail and FSR treatment: Et in e.m. " << EmEnergy * sinTheta / Units::GeV << "  F1 ratio " << F1);
            if (!m_FSRtreatment || EmEnergy * sinTheta < m_emEtCut || F1 < m_emF1Cut) {
                ++m_countMeasurement;
                double FinalEnergyErrorPlus = 0.50 * std::sqrt(FinalMeasuredEnergy / Units::GeV) * Units::GeV;
                double FinalEnergyErrorMinus = FinalEnergyErrorPlus;

                // overall also have 50% resolution in EC rather than the 70% naively expected from LArHEC
                if (LArHECEnergy > 1. * Units::GeV) {
                    FinalEnergyErrorMinus = FinalEnergyErrorPlus = 0.50 * std::sqrt(FinalMeasuredEnergy / Units::GeV) * Units::GeV;
                }
                double FinalEnergyError = 0.5 * (FinalEnergyErrorMinus + FinalEnergyErrorPlus);
                lossType = CaloEnergy::Tail;
                caloEnergy = std::make_unique<CaloEnergy>(FinalMeasuredEnergy, FinalEnergyError, FinalEnergyErrorMinus,
                                                          FinalEnergyErrorPlus, lossType);
                ATH_MSG_VERBOSE(" CaloEnergy Tail : " << FinalMeasuredEnergy);
            } else {
                // significant e.m. deposit and high F1
                double MopErrorEm = MopError * m_caloParamTool->emMopFraction(eta);
                double FinalMeasuredEnergyNoEm = FinalMeasuredEnergy - EmEnergy + MopLossEm;
                // lossType				= CaloEnergy::FSRcandidate;

                ATH_MSG_VERBOSE(" CaloEnergy FSR :  EmEnergy " << EmEnergy << "  FinalMeasuredEnergy " << FinalMeasuredEnergy
                                                               << "  MopLossEm " << MopLossEm << "  MopErrorEm " << MopErrorEm
                                                               << "  FinalMeasuredEnergyNoEm " << FinalMeasuredEnergyNoEm);
                if (FinalMeasuredEnergyNoEm < MopLoss + 2. * MopError) {
                    // small hadronic energy deposit: like NotIsolated (MOP or Mean according to configuration)
                    lossType = CaloEnergy::NotIsolated;
                    if (m_MOPparametrization) {
                        ++m_countMop;
                        caloEnergy =
                            std::make_unique<CaloEnergy>(caloParamMop->deltaE(), caloParamMop->sigmaDeltaE(),
                                                         caloParamMop->sigmaMinusDeltaE(), caloParamMop->sigmaPlusDeltaE(), lossType);
                    } else {
                        ++m_countMean;
                        caloEnergy =
                            std::make_unique<CaloEnergy>(caloParamMean->deltaE(), caloParamMean->sigmaDeltaE(),
                                                         caloParamMean->sigmaMinusDeltaE(), caloParamMean->sigmaPlusDeltaE(), lossType);
                    }
                    ATH_MSG_VERBOSE(" CaloEnergy FSR : Small deposit, FinalMeasuredEnergyNoEm " << FinalMeasuredEnergyNoEm
                                                                                                << " using Eloss " << caloEnergy->deltaE());
                } else {
                    //  significant hadronic energy deposit
                    ++m_countMeasurement;
                    lossType = CaloEnergy::FSRcandidate;
                    double FinalEnergyErrorNoEm = 0.50 * std::sqrt(FinalMeasuredEnergyNoEm / Units::GeV) * Units::GeV;
                    FinalEnergyErrorNoEm = std::hypot(FinalEnergyErrorNoEm, MopErrorEm);
                    caloEnergy = std::make_unique<CaloEnergy>(FinalMeasuredEnergyNoEm, FinalEnergyErrorNoEm, FinalEnergyErrorNoEm,
                                                              FinalEnergyErrorNoEm, lossType);
                    ATH_MSG_VERBOSE(" CaloEnergy FSR : Large deposit, FinalMeasuredEnergyNoEm " << FinalMeasuredEnergyNoEm);
                }
            }
        } else {
            // lossType NotIsolated: MOP or Mean according to configuration
            if (m_MOPparametrization) {
                ++m_countMop;
                caloEnergy = std::make_unique<CaloEnergy>(caloParamMop->deltaE(), caloParamMop->sigmaDeltaE(),
                                                          caloParamMop->sigmaMinusDeltaE(), caloParamMop->sigmaPlusDeltaE(), lossType);
            } else {
                ++m_countMean;
                caloEnergy = std::make_unique<CaloEnergy>(caloParamMean->deltaE(), caloParamMean->sigmaDeltaE(),
                                                          caloParamMean->sigmaMinusDeltaE(), caloParamMean->sigmaPlusDeltaE(), lossType);
            }
        }

        caloEnergy->set_fsrCandidateEnergy(MopLossEm);
        return caloEnergy;
    }

    double MuidCaloEnergyTool::muSpecResolParam(double trackMomentum, double eta) const {
        const double pT = trackMomentum / std::cosh(eta) / Units::GeV;  // pt in GeV
        double a = 0.;
        double b = 0.;
        if (std::abs(eta) < 1.) {
            a = 0.02255;
            b = 7.708e-5;
        } else if (std::abs(eta) > 1. && std::abs(eta) < 2.) {
            a = 0.04198;
            b = 8.912e-5;
        } else {
            a = 0.02181;
            b = 7.282e-5;
        }
        return std::hypot(a, (b * pT));
    }

    double MuidCaloEnergyTool::paramCorrection(double trackMomentum, double eta, double MopLoss, double MopLossSigma) const {
        const double Nsigma = 2.;
        double MSres = muSpecResolParam(trackMomentum, eta);
        double MSsigma = trackMomentum * MSres;
        double sigma = std::hypot(MSsigma, MopLossSigma);
        double sum = 0.;
        double weight = 0.;
        double xlow = MopLoss - Nsigma * sigma;
        if (xlow < 0.) xlow = 0.;
        double xup = MopLoss + Nsigma * sigma;
        int Na = 50;
        const double inv_Na = 1. / static_cast<double>(Na);
        for (int j = 0; j < Na; ++j) {
            double x = xlow + j * (xup - xlow) * inv_Na;
            double w = landau(x, MopLoss, MopLossSigma, true);
            sum += x * w;
            weight += w;
        }
        double MopStat = sum / weight;
        return MopStat;
    }

    double MuidCaloEnergyTool::landau(double x, double mpv, double sigma, bool norm) const {
        // The LANDAU function with mpv(most probable value) and sigma.
        // This function has been adapted from the CERNLIB routine G110 denlan.
        // If norm=kTRUE (default is kFALSE) the result is divided by sigma

        constexpr double p1[5] = {0.4259894875, -0.1249762550, 0.03984243700, -0.006298287635, 0.001511162253};
        constexpr double q1[5] = {1.0, -0.3388260629, 0.09594393323, -0.01608042283, 0.003778942063};

        constexpr double p2[5] = {0.1788541609, 0.1173957403, 0.01488850518, -0.001394989411, 0.0001283617211};
        constexpr double q2[5] = {1.0, 0.7428795082, 0.3153932961, 0.06694219548, 0.008790609714};

        constexpr double p3[5] = {0.1788544503, 0.09359161662, 0.006325387654, 0.00006611667319, -0.000002031049101};
        constexpr double q3[5] = {1.0, 0.6097809921, 0.2560616665, 0.04746722384, 0.006957301675};

        constexpr double p4[5] = {0.9874054407, 118.6723273, 849.2794360, -743.7792444, 427.0262186};
        constexpr double q4[5] = {1.0, 106.8615961, 337.6496214, 2016.712389, 1597.063511};

        constexpr double p5[5] = {1.003675074, 167.5702434, 4789.711289, 21217.86767, -22324.94910};
        constexpr double q5[5] = {1.0, 156.9424537, 3745.310488, 9834.698876, 66924.28357};

        constexpr double p6[5] = {1.000827619, 664.9143136, 62972.92665, 475554.6998, -5743609.109};
        constexpr double q6[5] = {1.0, 651.4101098, 56974.73333, 165917.4725, -2815759.939};

        constexpr double a1[3] = {0.04166666667, -0.01996527778, 0.02709538966};

        constexpr double a2[2] = {-1.845568670, -4.284640743};

        if (sigma <= 0) return 0.;
        double v = (x - mpv) / sigma;
        double u, ue, us, den;
        if (v < -5.5) {
            u = exp(v + 1.0);
            if (u < 1e-10) return 0.0;
            ue = std::exp(-1 / u);
            us = std::sqrt(u);
            den = 0.3989422803 * (ue / us) * (1 + (a1[0] + (a1[1] + a1[2] * u) * u) * u);
        } else if (v < -1) {
            u = std::exp(-v - 1);
            den = exp(-u) * std::sqrt(u) * (p1[0] + (p1[1] + (p1[2] + (p1[3] + p1[4] * v) * v) * v) * v) /
                  (q1[0] + (q1[1] + (q1[2] + (q1[3] + q1[4] * v) * v) * v) * v);
        } else if (v < 1) {
            den = (p2[0] + (p2[1] + (p2[2] + (p2[3] + p2[4] * v) * v) * v) * v) /
                  (q2[0] + (q2[1] + (q2[2] + (q2[3] + q2[4] * v) * v) * v) * v);
        } else if (v < 5) {
            den = (p3[0] + (p3[1] + (p3[2] + (p3[3] + p3[4] * v) * v) * v) * v) /
                  (q3[0] + (q3[1] + (q3[2] + (q3[3] + q3[4] * v) * v) * v) * v);
        } else if (v < 12) {
            u = 1 / v;
            den = u * u * (p4[0] + (p4[1] + (p4[2] + (p4[3] + p4[4] * u) * u) * u) * u) /
                  (q4[0] + (q4[1] + (q4[2] + (q4[3] + q4[4] * u) * u) * u) * u);
        } else if (v < 50) {
            u = 1 / v;
            den = u * u * (p5[0] + (p5[1] + (p5[2] + (p5[3] + p5[4] * u) * u) * u) * u) /
                  (q5[0] + (q5[1] + (q5[2] + (q5[3] + q5[4] * u) * u) * u) * u);
        } else if (v < 300) {
            u = 1 / v;
            den = u * u * (p6[0] + (p6[1] + (p6[2] + (p6[3] + p6[4] * u) * u) * u) * u) /
                  (q6[0] + (q6[1] + (q6[2] + (q6[3] + q6[4] * u) * u) * u) * u);
        } else {
            u = 1 / (v - v * log(v) / (v + 1));
            den = u * u * (1 + (a2[0] + a2[1] * u) * u);
        }
        if (!norm) return den;
        return den / sigma;
    }

}  // namespace Rec
