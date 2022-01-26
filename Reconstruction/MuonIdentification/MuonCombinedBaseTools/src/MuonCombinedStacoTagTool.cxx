/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCombinedStacoTagTool
//  AlgTool performing statistical combination of ID and MS tracks (STACO)
//  A StacoTag is added to the InDetCandidate object.
//
//////////////////////////////////////////////////////////////////////////////

#include "MuonCombinedStacoTagTool.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/StacoTag.h"
#include "FourMomUtils/xAODP4Helpers.h"
namespace MuonCombined {

    MuonCombinedStacoTagTool::MuonCombinedStacoTagTool(const std::string& type, const std::string& name, const IInterface* parent) :
        AthAlgTool(type, name, parent) {
        declareInterface<IMuonCombinedTagTool>(this);
    }

    StatusCode MuonCombinedStacoTagTool::initialize() {
        ATH_MSG_INFO("Initializing MuonCombinedStacoTagTool");
        ATH_CHECK(m_printer.retrieve());
        ATH_CHECK(m_tagTool.retrieve());
        ATH_CHECK(m_extrapolator.retrieve());
        return StatusCode::SUCCESS;
    }

    void MuonCombinedStacoTagTool::combine(const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates,
                                           InDetCandidateToTagMap& tagMap, TrackCollection* combTracks, TrackCollection* ,
                                           const EventContext& ctx) const {
        if (!combTracks) {
            ATH_MSG_WARNING("No TrackCollection passed");
            return;
        }
        // only combine if the back extrapolation was successfull
        if (!muonCandidate.extrapolatedTrack() || !muonCandidate.extrapolatedTrack()->perigeeParameters() ||
            !muonCandidate.extrapolatedTrack()->perigeeParameters()->covariance())
            return;

        std::unique_ptr<const Trk::Perigee> bestPerigee;
        const InDetCandidate* bestCandidate = nullptr;
        double bestChi2{FLT_MAX};

        // loop over ID candidates
        for (const MuonCombined::InDetCandidate* const idTP : indetCandidates) {
            // skip tracklets
            if (idTP->isSiliconAssociated()) continue;

            // ensure that also the id has a perigee with covariance
            if (!idTP->indetTrackParticle().perigeeParameters().covariance()) continue;

            // ensure that id tp can be extrapolated to something
            const Trk::CaloExtension* caloExtension = idTP->getCaloExtension();
            if (!caloExtension || caloExtension->caloLayerIntersections().empty()) continue;

            const Trk::Perigee* idPer = &idTP->indetTrackParticle().perigeeParameters();
            const Trk::Perigee* msPer = muonCandidate.extrapolatedTrack()->perigeeParameters();
            std::unique_ptr<const Trk::TrackParameters> exPars;
            // check that the to perigee surfaces are the same
            if (idPer->associatedSurface() != msPer->associatedSurface()) {
                // extrapolate to id surface
                exPars = m_extrapolator->extrapolate(ctx, *muonCandidate.extrapolatedTrack(), idPer->associatedSurface());
                if (!exPars) {
                    ATH_MSG_DEBUG("The ID and MS candidates are not expressed at the same surface: id r "
                                    << idTP->indetTrackParticle().perigeeParameters().associatedSurface().center().perp() << " z "
                                    << idTP->indetTrackParticle().perigeeParameters().associatedSurface().center().z() << " ms r "
                                    << muonCandidate.extrapolatedTrack()->perigeeParameters()->associatedSurface().center().perp() << " z "
                                    << muonCandidate.extrapolatedTrack()->perigeeParameters()->associatedSurface().center().z()
                                    << " and extrapolation failed");
                    continue;
                }
                msPer = dynamic_cast<const Trk::Perigee*>(exPars.get());
                if (!msPer) {
                    ATH_MSG_WARNING("Extrapolation did not return a perigee!");
                    continue;
                }
            }
            double chi2 = 0;
            std::unique_ptr<const Trk::Perigee> perigee = theCombIdMu(*idPer, *msPer, chi2);
            if (!perigee || !perigee->covariance() || !Amg::saneCovarianceDiagonal(*perigee->covariance())) {
                ATH_MSG_DEBUG("Combination failed");
                continue;
            }
            if (chi2 < bestChi2) {
                bestChi2 = chi2;
                bestPerigee = std::move(perigee);
                bestCandidate = idTP;
            }
        }
        if (bestCandidate) {
            const double outerMatchChi2 = m_tagTool->chi2(*bestCandidate->indetTrackParticle().track(), *muonCandidate.extrapolatedTrack(), ctx);
            ATH_MSG_DEBUG("Combined Muon with ID " << m_printer->print(*bestPerigee) << " match chi2 " << bestChi2 << " outer match "
                                                   << outerMatchChi2);
            StacoTag* tag = new StacoTag(muonCandidate, bestPerigee, bestChi2);
            tagMap.addEntry(bestCandidate, tag);
        }
    }

    std::unique_ptr<Trk::Perigee> MuonCombinedStacoTagTool::theCombIdMu(const Trk::Perigee& indetPerigee,
                                                                              const Trk::Perigee& extrPerigee, double& chi2) const {
        chi2 = 1e20;
        if (!indetPerigee.covariance() || !extrPerigee.covariance()) {
            ATH_MSG_WARNING("Perigee parameters without covariance");
            return nullptr;
        }

        const AmgSymMatrix(5)& covID = *indetPerigee.covariance();
        const AmgSymMatrix(5) weightID = covID.inverse();
        if (weightID.determinant() == 0.) {
            ATH_MSG_DEBUG(" ID weight matrix computation failed     ");
            return nullptr;
        }

        const AmgSymMatrix(5)& covMS = *extrPerigee.covariance();
        const AmgSymMatrix(5) weightMS = covMS.inverse();
        if (weightMS.determinant() == 0.) {
            ATH_MSG_DEBUG("weightMS computation failed      ");
            return nullptr;
        }

        AmgSymMatrix(5) weightCB = weightID + weightMS;

        if (weightCB.determinant() == 0) {
            ATH_MSG_DEBUG(" Inversion of weightCB failed ");
            return nullptr;
        }
        AmgSymMatrix(5) covCB = AmgSymMatrix(5)(weightCB.inverse());
        if (covCB.determinant() == 0) {
            ATH_MSG_DEBUG(" Inversion of weightCB failed ");
            return nullptr;
        }

        AmgSymMatrix(5) covSum = covID + covMS;
        AmgSymMatrix(5) invCovSum = covSum.inverse();

        if (invCovSum.determinant() == 0) {
            ATH_MSG_DEBUG(" Inversion of covSum failed ");
            return nullptr;
        }

        AmgVector(5) parsMS(extrPerigee.parameters());
       
        AmgVector(5) diffPars = indetPerigee.parameters() - parsMS;
        chi2  = (diffPars.transpose() * invCovSum * diffPars);
        chi2 /= 5.;
       
        AmgVector(5) parsCB = (covCB) * (weightID * indetPerigee.parameters() + weightMS * parsMS);

        parsCB[Trk::phi] = xAOD::P4Helpers::deltaPhi(parsCB[Trk::phi] ,0.);
        return indetPerigee.associatedSurface().createUniqueParameters<5, Trk::Charged>(
            parsCB[Trk::locX], parsCB[Trk::locY], parsCB[Trk::phi], parsCB[Trk::theta], parsCB[Trk::qOverP], covCB);
    }

}  // namespace MuonCombined
