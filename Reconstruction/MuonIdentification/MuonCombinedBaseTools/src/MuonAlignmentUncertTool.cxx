/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentUncertTool.h"

#include <PathResolver/PathResolver.h>
#include <TFile.h>

#include <cmath>
#include <iomanip>

#include "AthenaKernel/Units.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"

namespace {
    constexpr float MeVtoGeV = 1.e-3;
}

namespace Muon {
    MuonAlignmentUncertTool::MuonAlignmentUncertTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent),
        m_histo(nullptr),
        m_dim(-1),
        m_x_reader(make_reader(nullptr)),
        m_y_reader(make_reader(nullptr)),
        m_z_reader(make_reader(nullptr)),
        m_calorimeterVolume(nullptr),
        m_indetVolume(nullptr) {}
    StatusCode MuonAlignmentUncertTool::initialize() {
        /// Open the TFile
        std::unique_ptr<TFile> r_file(TFile::Open(PathResolverFindCalibFile(m_in_file).c_str(), "READ"));
        if (!r_file || !r_file->IsOpen()) {
            ATH_MSG_FATAL("No such file or directory " << m_in_file);
            return StatusCode::FAILURE;
        }
        /// Read the histogram from the file
        TH1* histo_in_file = nullptr;
        r_file->GetObject(m_histo_name.value().c_str(), histo_in_file);
        m_histo = std::unique_ptr<TH1>(histo_in_file);
        if (!m_histo) {
            ATH_MSG_FATAL("File " << m_in_file << " does not contain the histogram " << m_histo_name << ".");
            return StatusCode::FAILURE;
        }
        m_histo->SetDirectory(nullptr);
        /// Set the dimension and the readers to be used
        m_dim = m_histo->GetDimension();
        m_x_reader = make_reader(m_histo->GetXaxis());
        m_y_reader = make_reader(m_histo->GetYaxis());
        m_z_reader = make_reader(m_histo->GetZaxis());

        ATH_CHECK(m_trackingVolumesSvc.retrieve());
        m_calorimeterVolume =
            std::make_unique<Trk::Volume>(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
        m_indetVolume = std::make_unique<Trk::Volume>(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
        return StatusCode::SUCCESS;
    }
    float MuonAlignmentUncertTool::get_uncertainty(const Trk::Track* trk) const {
        if (!trk) {
            ATH_MSG_ERROR("No track has been given. Return FLT_MAX");
            return FLT_MAX;
        }
        const int bin_x = m_x_reader(trk);
        /// The second and third arguments of the GetBin method
        /// are 0 as default
        const int bin_y = m_dim > 1 ? m_y_reader(trk) : 0;
        const int bin_z = m_dim > 2 ? m_z_reader(trk) : 0;
        return m_histo->GetBinContent(bin_x, bin_y, bin_z);
    }
    std::function<int(const Trk::Track* trk)> MuonAlignmentUncertTool::make_reader(const TAxis* axis) const {
        /// Dummy function just returning 0 if no axis is given
        if (!axis) {
            return [](const Trk::Track*) { return 0; };
        }
        std::string axis_title = axis->GetTitle();
        if (axis_title == "#eta" || axis_title == "eta") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(perigee->eta());
            };
        } else if (axis_title == "|#eta|" || axis_title == "abseta") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(std::abs(perigee->eta()));
            };
        } else if (axis_title == "#phi" || axis_title == "phi") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(perigee->parameters()[2]);
            };
        } else if (axis_title == "|#phi|" || axis_title == "absphi") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(std::abs(perigee->parameters()[2]));
            };
        } else if (axis_title == "#theta" || axis_title == "theta") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(perigee->parameters()[3]);
            };
        } else if (axis_title == "|#theta|" || axis_title == "abstheta") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(std::abs(perigee->parameters()[3]));
            };
        } else if (axis_title == "p_{T}" || axis_title == "pT") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(perigee->pT());
            };
        } else if (axis_title == "p_{T} [GeV]" || axis_title == "pTinGeV") {
            return [axis](const Trk::Track* trk) {
                const Trk::Perigee* perigee = trk->perigeeParameters();
                return axis->FindBin(perigee->pT() * MeVtoGeV);
            };
        } else if (axis_title == "#rho @ ID-exit") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* id_exit;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, id_exit, dummy, dummy, dummy);
                return axis->FindBin(id_exit->trackParameters()->position().perp());
            };
        } else if (axis_title == "R @ ID-exit") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* id_exit;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, id_exit, dummy, dummy, dummy);
                return axis->FindBin(id_exit->trackParameters()->position().mag());
            };
        } else if (axis_title == "z @ ID-exit") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* id_exit;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, id_exit, dummy, dummy, dummy);
                return axis->FindBin(id_exit->trackParameters()->position()[2]);
            };
        } else if (axis_title == "#rho @ Calo-entrance") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* calo_entrance;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, calo_entrance, dummy, dummy);
                return axis->FindBin(calo_entrance->trackParameters()->position().perp());
            };
        } else if (axis_title == "R @ Calo-entrance") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* calo_entrance;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, calo_entrance, dummy, dummy);
                return axis->FindBin(calo_entrance->trackParameters()->position().mag());
            };
        } else if (axis_title == "z @ Calo-entrance") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* calo_entrance;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, calo_entrance, dummy, dummy);
                return axis->FindBin(calo_entrance->trackParameters()->position()[2]);
            };
        } else if (axis_title == "#rho @ Calo-centre") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* calo_entrance;
                const Trk::TrackStateOnSurface* calo_exit;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, calo_entrance, calo_exit, dummy);
                const Amg::Vector3D calo_centre =
                    0.5 * (calo_entrance->trackParameters()->position() + calo_exit->trackParameters()->position());
                return axis->FindBin(calo_centre.perp());
            };
        } else if (axis_title == "R @ Calo-centre") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* calo_entrance;
                const Trk::TrackStateOnSurface* calo_exit;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, calo_entrance, calo_exit, dummy);
                const Amg::Vector3D calo_centre =
                    0.5 * (calo_entrance->trackParameters()->position() + calo_exit->trackParameters()->position());
                return axis->FindBin(calo_centre.mag());
            };
        } else if (axis_title == "z @ Calo-centre") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* calo_entrance;
                const Trk::TrackStateOnSurface* calo_exit;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, calo_entrance, calo_exit, dummy);
                const Amg::Vector3D calo_centre =
                    0.5 * (calo_entrance->trackParameters()->position() + calo_exit->trackParameters()->position());
                return axis->FindBin(calo_centre[2]);
            };
        } else if (axis_title == "#rho @ MS-entrance") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* ms_entrance;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, dummy, dummy, ms_entrance);
                return axis->FindBin(ms_entrance->trackParameters()->position().perp());
            };
        } else if (axis_title == "R @ MS-entrance") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* ms_entrance;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, dummy, dummy, ms_entrance);
                return axis->FindBin(ms_entrance->trackParameters()->position().mag());
            };
        } else if (axis_title == "z @ MS-entrance") {
            return [axis, this](const Trk::Track* trk) {
                const Trk::TrackStateOnSurface* ms_entrance;
                const Trk::TrackStateOnSurface* dummy;
                get_track_state_measures(trk, dummy, dummy, dummy, ms_entrance);
                return axis->FindBin(ms_entrance->trackParameters()->position()[2]);
            };
        }
        return make_reader(nullptr);
    }

    void MuonAlignmentUncertTool::get_track_state_measures(const Trk::Track* track, const Trk::TrackStateOnSurface*& id_exit,
                                                           const Trk::TrackStateOnSurface*& calo_entrance,
                                                           const Trk::TrackStateOnSurface*& calo_exit,
                                                           const Trk::TrackStateOnSurface*& ms_entrance) const {
        id_exit = calo_entrance = calo_exit = ms_entrance = nullptr;
        if (!track) {
            ATH_MSG_DEBUG("No track has been given. Where is my track?!");
            return;
        }
        for (const Trk::TrackStateOnSurface* trk_srf : *track->trackStateOnSurfaces()) {
            if (!trk_srf->trackParameters()) { continue; }
            if (m_indetVolume->inside(trk_srf->trackParameters()->position())) {
                if (trk_srf->type(Trk::TrackStateOnSurface::Measurement)) id_exit = trk_srf;
                continue;
            }
            /// Try to find the calorimeter entrance and leaving points
            if (m_calorimeterVolume->inside(trk_srf->trackParameters()->position())) {
                // first scattering plane in Calorimeter
                if (trk_srf->type(Trk::TrackStateOnSurface::Scatterer) && trk_srf->materialEffectsOnTrack()) {
                    if (trk_srf->materialEffectsOnTrack()->thicknessInX0() < 10) { continue; }
                    if (!calo_entrance) {
                        calo_entrance = trk_srf;
                    } else {
                        calo_exit = trk_srf;
                    }
                }
            }
            /// inside muon system
            else if (trk_srf->measurementOnTrack()) {
                ms_entrance = trk_srf;
                break;
            }
        }
    }

}  // namespace Muon
