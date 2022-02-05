/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibTrackSummary.h"

#include <sstream>

#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"

namespace MuonCalib {

    std::string MuonCalibTrackSummary::dump() const {
        MuonFixedIdPrinter printer{};

        std::ostringstream sout;
        if (hasId()) {
            sout << " Pixel: " << npixel << " SCT " << nsct << " TRT barrel " << ntrtBarrel << " TRT endcap " << ntrtEndcap;
            if (hasMuon()) sout << std::endl;
        }

        if (hasMuon()) {
            sout << " Layers: eta " << precisionStationLayers.size() << " good " << goodPrecisionStationLayers.size() << "    phi "
                 << phiStationLayers.size() << " eta confirmed " << phiEtaStationLayers.size() << std::endl
                 << " Hits: eta " << nmdtHits + ncscEtaHits << "  phi " << nrpcPhiHits + ntgcPhiHits + ncscPhiHits << "  eta trig "
                 << nrpcEtaHits + ntgcEtaHits << "  holes " << nholes << "  outliers " << noutliers << "  pseudo " << npseudo
                 << "  scatters " << nscatters << std::endl;
        }

        sout.setf(std::ios::left);
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        std::vector<ChamberHitSummary>::const_iterator chit_last = chit_end - 1;
        for (; chit != chit_end; ++chit) {
            const MuonFixedId& chId = chit->chId;
            bool isMdt = chit->isMdt();

            sout << "  " << std::setw(35) << printer.chamberIdentifier(chId);

            const ChamberHitSummary::Projection& first = chit->etaProjection();
            const ChamberHitSummary::Projection& second = chit->phiProjection();

            std::string firstString = isMdt ? "ml1 " : "eta ";
            std::string secondString = isMdt ? "ml2 " : "phi ";

            sout << " Hits:  " << firstString << std::setw(3) << first.nhits << "   " << secondString << std::setw(3) << second.nhits;

            if (first.nholes || second.nholes) {
                sout << "  Holes:  ";
                if (first.nholes != 0) sout << firstString << std::setw(3) << first.nholes;
                if (second.nholes != 0) {
                    if (first.nholes != 0) sout << "  ";
                    sout << secondString << std::setw(3) << second.nholes;
                }
            }
            if (first.noutliers || second.noutliers) {
                sout << "  Outliers:  ";
                if (first.noutliers != 0) sout << firstString << std::setw(3) << first.noutliers << "  ";
                if (second.noutliers != 0) {
                    if (first.noutliers != 0) sout << "  ";
                    sout << secondString << std::setw(3) << second.noutliers;
                }
            }

            if (first.ndeltas || second.ndeltas) {
                sout << "  Deltas:  ";
                if (first.ndeltas != 0) sout << firstString << std::setw(3) << first.ndeltas << "  ";
                if (second.ndeltas != 0) {
                    if (first.ndeltas != 0) sout << "  ";
                    sout << secondString << std::setw(3) << second.ndeltas;
                }
            }

            if (chit != chit_last) sout << std::endl;
        }

        return sout.str();
    }

    unsigned int MuonCalibTrackSummary::nmdtHitsPerMl(int ml) const {
        unsigned int nhits = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_mdt()) {
                if (ml == 1)
                    nhits += chit->nMdtHitsMl1();
                else
                    nhits += chit->nMdtHitsMl2();
            }
        }
        return nhits;
    }

    unsigned int MuonCalibTrackSummary::nmdtCh() const {
        unsigned int nCh = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_mdt()) ++nCh;
        }
        return nCh;
    }

    unsigned int MuonCalibTrackSummary::nrpcCh(bool phi) const {
        unsigned int nCh = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_rpc()) {
                if (phi && chit->nphiHits() > 0)
                    ++nCh;
                else if (!phi && chit->netaHits() > 0)
                    ++nCh;
            }
        }
        return nCh;
    }

    unsigned int MuonCalibTrackSummary::ntgcCh(bool phi) const {
        unsigned int nCh = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_tgc()) {
                if (phi && chit->nphiHits() > 0)
                    ++nCh;
                else if (!phi && chit->netaHits() > 0)
                    ++nCh;
            }
        }
        return nCh;
    }

    unsigned int MuonCalibTrackSummary::ncscCh(bool phi) const {
        unsigned int nCh = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_csc()) {
                if (phi && chit->nphiHits() > 0)
                    ++nCh;
                else if (!phi && chit->netaHits() > 0)
                    ++nCh;
            }
        }
        return nCh;
    }

    unsigned int MuonCalibTrackSummary::nrpcEtaPhiCh() const {
        unsigned int nCh = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_rpc()) {
                if (chit->nphiHits() > 0 && chit->netaHits() > 0) ++nCh;
            }
        }
        return nCh;
    }

    unsigned int MuonCalibTrackSummary::ntgcEtaPhiCh() const {
        unsigned int nCh = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_tgc()) {
                if (chit->nphiHits() > 0 && chit->netaHits() > 0) ++nCh;
            }
        }
        return nCh;
    }

    unsigned int MuonCalibTrackSummary::ncscEtaPhiCh() const {
        unsigned int nCh = 0;
        std::vector<ChamberHitSummary>::const_iterator chit = chamberHitSummary.begin();
        std::vector<ChamberHitSummary>::const_iterator chit_end = chamberHitSummary.end();
        for (; chit != chit_end; ++chit) {
            if (chit->chId.is_csc()) {
                if (chit->nphiHits() > 0 && chit->netaHits() > 0) ++nCh;
            }
        }
        return nCh;
    }

}  // namespace MuonCalib
