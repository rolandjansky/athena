/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentCleaner/MuonPhiHitSelector.h"

#include <Eigen/LU>
#include <ctime>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

#include "CxxUtils/sincos.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/PlaneSurface.h"

MuonPhiHitSelector::MuonPhiHitSelector(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
{
    declareInterface<IMuonHitSelector>(this);

    m_cosmics = false;
    declareProperty("DoCosmics", m_cosmics);

    m_summary = false;
    declareProperty("DoSummary", m_summary);

    // Create and write out the phi clusters or hits

    m_makeClusters = false;
    declareProperty("MakeClusters", m_makeClusters);
    m_competingRios = false;

    declareProperty("CompetingRios", m_competingRios);
}

StatusCode
MuonPhiHitSelector::initialize()
{
    ATH_MSG_VERBOSE("MuonPhiHitSelector::Initializing");
    ATH_CHECK(m_competingRIOsOnTrackTool.retrieve());
    ATH_CHECK(m_clusterCreator.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_MSG_VERBOSE("End of Initializing");
    return StatusCode::SUCCESS;
}

std::vector<const Trk::MeasurementBase*>*
MuonPhiHitSelector::select_rio(const double pmom, const std::vector<const Trk::RIO_OnTrack*>& associatedHits,
                               const std::vector<const Trk::PrepRawData*>& unassociatedHits) const
{
    // Make Rios on Track

    int time_start = std::clock() / 1000;

    std::vector<const Trk::MeasurementBase*>* selectedHits     = new std::vector<const Trk::MeasurementBase*>();
    std::vector<const Trk::MeasurementBase*>* selectedClusters = new std::vector<const Trk::MeasurementBase*>();

    ATH_MSG_VERBOSE("Executing MuonPhiHitSelectorTool select_rio ");

    int nhits = associatedHits.size() + unassociatedHits.size();

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Executing MuonPhiHitSelectorTool nhits select_rio " << nhits);

    std::vector<double>                  phiHitx(nhits);
    std::vector<double>                  phiHity(nhits);
    std::vector<double>                  phiHitz(nhits);
    std::vector<double>                  phiError(nhits);
    std::vector<Identifier>              phiId(nhits);
    std::vector<double>                  phiPull(nhits);
    std::vector<int>                     phiSelect(nhits);
    std::vector<int>                     phiMult(nhits);
    std::vector<int>                     quality(nhits);
    std::vector<const Trk::PrepRawData*> phiPrep(nhits);

    std::map<Identifier, int>                            phiMapId;
    int                                                  nphi   = 0;
    std::vector<const Trk::RIO_OnTrack*>::const_iterator it     = associatedHits.begin();
    std::vector<const Trk::RIO_OnTrack*>::const_iterator it_end = associatedHits.end();

    for (; it != it_end; ++it) {
        const Trk::PrepRawData* prd = (*it)->prepRawData();
        if (!prd) {
            ATH_MSG_WARNING("prepRawData of associatedHits is nullptr, continuing...");
            continue;
        }
        Identifier id         = prd->identify();
        phiId[nphi]           = id;
        Amg::Vector3D gHitPos = (*it)->globalPosition();
        if (m_idHelperSvc->isRpc(id)) {
            phiSelect[nphi] = 1;
        } else if (m_idHelperSvc->isTgc(id)) {
            phiSelect[nphi] = 2;
        } else if (m_idHelperSvc->isCsc(id)) {
            phiSelect[nphi] = 3;
        }
        phiHitx[nphi] = gHitPos.x();
        phiHity[nphi] = gHitPos.y();
        phiHitz[nphi] = gHitPos.z();

        const Amg::MatrixX& cov   = (*it)->localCovariance();
        double              error = cov(0, 0);

        // for the TGCs diagonalize the error matrix and use the smallest component
        if (cov.cols() > 1) {
            AmgMatrix(2, 2) Er;
            Er(0, 0) = cov(0, 0);
            Er(0, 1) = cov(0, 1);
            Er(1, 1) = cov(1, 1);
            Er(1, 0) = Er(0, 1);

            double chi = Er(0, 0) != Er(1, 1) ? std::atan(-2 * Er(0, 1) / (Er(0, 0) - Er(1, 1))) / 2. : 0.;

            CxxUtils::sincos scchi(chi);

            AmgMatrix(2, 2) Rot;
            Rot(0, 0)         = scchi.cs;
            Rot(1, 1)         = Rot(0, 0);
            Rot(0, 1)         = scchi.sn;
            Rot(1, 0)         = -Rot(0, 1);
            AmgMatrix(2, 2) D = Rot.transpose() * Er * Rot;
            if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Diagonalized error matrix " << D);
            error = D(0, 0) < D(1, 1) ? D(0, 0) : D(1, 1);
        }
        phiError[nphi] = std::sqrt(error);
        quality[nphi]  = 1000;
        phiMapId[id]   = 1;
        phiPrep[nphi]  = prd;
        double phipos  = std::atan2(phiHity[nphi], phiHitx[nphi]);
        if (msgLvl(MSG::DEBUG))
            ATH_MSG_DEBUG("phi Segment Hit " << nphi << " det " << phiSelect[nphi] << " phi " << phipos);
        nphi++;
    }
    int nphiseg = nphi;

    std::vector<const Trk::PrepRawData*>::const_iterator itu     = unassociatedHits.begin();
    std::vector<const Trk::PrepRawData*>::const_iterator itu_end = unassociatedHits.end();

    for (; itu != itu_end; ++itu) {
        Identifier id = (*itu)->identify();
        phiId[nphi]   = id;
        // Skip phi hits already on segment
        if (phiMapId.count(id) > 0) continue;
        const Muon::MuonCluster* clus = dynamic_cast<const Muon::MuonCluster*>(*itu);
        if (!clus) continue;
        if (m_idHelperSvc->isRpc(id))
            phiSelect[nphi] = 1;
        else if (m_idHelperSvc->isTgc(id))
            phiSelect[nphi] = 2;
        else if (m_idHelperSvc->isCsc(id))
            phiSelect[nphi] = 3;
        Amg::Vector3D gHitPos = clus->globalPosition();
        phiHitx[nphi]         = gHitPos.x();
        phiHity[nphi]         = gHitPos.y();
        phiHitz[nphi]         = gHitPos.z();
        phiError[nphi]        = (*itu)->localCovariance()(Trk::locX);
        quality[nphi]         = 10;
        phiPrep[nphi]         = *itu;
        double phipos         = std::atan2(phiHity[nphi], phiHitx[nphi]);
        if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("phi Pattern Hit " << nphi << " phi " << phipos);
        nphi++;
    }

    double              chi2(0);
    double              r0(0);
    int                 nfit;
    std::vector<double> errorM(4);
    double              phi(DBL_MAX);
    fitRecPhi(pmom, phiId, phiHitx, phiHity, phiHitz, phiError, quality, nphi, phiPull, phiMult, phiSelect, chi2, r0,
              phi, errorM, nfit);

    // Define global track parameters (not used 27-8 JS)

    for (int i = 0; i < nphi; ++i) {
        if (phiSelect[i] > 0) {
            if (phiSelect[i] == 1) {
                const Muon::RpcPrepData*        prd = dynamic_cast<const Muon::RpcPrepData*>(phiPrep[i]);
                const Amg::Vector3D             globalpos(phiHitx[i], phiHity[i], phiHitz[i]);
                const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                if (rio) selectedHits->push_back(rio);
            } else if (phiSelect[i] == 2) {
                const Muon::TgcPrepData*        prd = dynamic_cast<const Muon::TgcPrepData*>(phiPrep[i]);
                const Amg::Vector3D             globalpos(phiHitx[i], phiHity[i], phiHitz[i]);
                const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                if (rio) selectedHits->push_back(rio);
            } else if (phiSelect[i] == 3) {
                const Muon::CscPrepData*        prd = dynamic_cast<const Muon::CscPrepData*>(phiPrep[i]);
                const Amg::Vector3D             globalpos(phiHitx[i], phiHity[i], phiHitz[i]);
                const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                if (rio) selectedHits->push_back(rio);
            }

            if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Make ONE rio per PrepData");
        }
    }
    if (msgLvl(MSG::DEBUG) || m_summary) {
        ATH_MSG_DEBUG("Fit hit results phi " << phi << " chi2 " << chi2 << " segment hits  " << nphiseg
                                             << " pattern hits " << nphi - nphiseg << " nfit " << nfit << " rio size "
                                             << selectedHits->size());
    }

    std::vector<double>     clusterX(nphi);
    std::vector<double>     clusterY(nphi);
    std::vector<double>     clusterZ(nphi);
    std::vector<double>     clusterError(nphi);
    std::vector<Identifier> clusterId(nphi);
    std::vector<int>        clusterHits(nphi);
    std::vector<double>     clusterPull(nphi);
    std::vector<int>        clusterSelect(nphi);
    // Link from hit to cluster
    std::vector<int> clusterInt(nphi);

    int                 ncl, imax;
    double              chi2cl, r0cl, phicl;
    std::vector<double> errorMcl(4);
    clusterPhi(phiId, phiHitx, phiHity, phiHitz, phiError, phiPull, phiSelect, nphi, clusterX, clusterY, clusterZ,
               clusterError, clusterId, clusterHits, clusterSelect, clusterInt, ncl);


    for (int ic = 0; ic < ncl; ++ic) {
        std::list<const Trk::PrepRawData*> prdList;
        int                                iic     = -1;
        double                             avError = 0.;
        int                                ip      = -1;
        int                                np      = 0;
        for (int i = 0; i < nphi; ++i) {
            if (clusterInt[i] == ic) {
                ip = i;
                prdList.push_back(phiPrep[i]);
                avError += 1. / (phiError[i] * phiError[i]);
                if (clusterId[ic] == phiId[i]) iic = i;
                np++;
            }
        }
        if (iic > -1) {
            if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi cluster found np " << np << " ip " << ip);
            if (np == 1) {
                // Only one PrepData: create RIO on Track
                const Amg::Vector3D globalpos(clusterX[ic], clusterY[ic], clusterZ[ic]);
                if (phiSelect[ip] == 1) {
                    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi RPC rio");
                    const Muon::RpcPrepData*        prd = dynamic_cast<const Muon::RpcPrepData*>(phiPrep[ip]);
                    const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                    if (rio) selectedClusters->push_back(rio);
                } else if (phiSelect[ip] == 2) {
                    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi TGC rio");
                    const Muon::TgcPrepData*        prd = dynamic_cast<const Muon::TgcPrepData*>(phiPrep[ip]);
                    const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                    if (rio) selectedClusters->push_back(rio);
                } else if (phiSelect[ip] == 3) {
                    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi CSC rio");
                    const Muon::CscPrepData*        prd = dynamic_cast<const Muon::CscPrepData*>(phiPrep[ip]);
                    const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                    if (rio) selectedClusters->push_back(rio);
                }
            } else {

                if (m_competingRios) {
                    // More PrepData's: create Competing RIOs on Track
                    avError                                      = std::sqrt(1. / avError);
                    double                           scaleFactor = clusterError[ic] / avError;
                    const Trk::CompetingRIOsOnTrack* rio =
                        m_competingRIOsOnTrackTool->createBroadCluster(prdList, scaleFactor);
                    if (rio) selectedClusters->push_back(rio);
                    if (msgLvl(MSG::DEBUG))
                        ATH_MSG_DEBUG("Make competing rio/cluster "
                                      << " scale factor " << scaleFactor << " number of rios " << prdList.size());
                } else {
                    // Make one Rio for central cluster
                    ip = iic;
                    const Amg::Vector3D globalpos(clusterX[ic], clusterY[ic], clusterZ[ic]);
                    if (phiSelect[ip] == 1) {
                        if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi RPC rio central cluster");
                        const Muon::RpcPrepData*        prd = dynamic_cast<const Muon::RpcPrepData*>(phiPrep[ip]);
                        const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                        if (rio) selectedClusters->push_back(rio);
                    } else if (phiSelect[ip] == 2) {
                        if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi TGC rio central cluster");
                        const Muon::TgcPrepData*        prd = dynamic_cast<const Muon::TgcPrepData*>(phiPrep[ip]);
                        const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                        if (rio) selectedClusters->push_back(rio);
                    } else if (phiSelect[ip] == 3) {
                        if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi CSC rio central cluster");
                        const Muon::CscPrepData*        prd = dynamic_cast<const Muon::CscPrepData*>(phiPrep[ip]);
                        const Muon::MuonClusterOnTrack* rio = m_clusterCreator->createRIO_OnTrack(*prd, globalpos);
                        if (rio) selectedClusters->push_back(rio);
                    }
                }
            }
        } else {
            if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Phi cluster NOT found ");
        }
    }

    fitPhiSL(pmom, clusterId, clusterX, clusterY, clusterZ, clusterError, clusterSelect, ncl, clusterPull, imax, chi2cl,
             r0cl, phicl, errorMcl, false);


    if (msgLvl(MSG::DEBUG) || m_summary) {
        ATH_MSG_DEBUG("PhiHitSelector Time spent " << std::clock() / 1000 - time_start << " nhits " << nhits
                                                   << " segment hits " << associatedHits.size() << " nfit " << nfit
                                                   << " nclusters " << ncl);
        ATH_MSG_DEBUG("Fit cluster results phi " << phicl << " chi2 " << chi2cl << " number of clusters  " << ncl
                                                 << " size cluster Hits " << selectedClusters->size());
    }
    if (m_makeClusters) {
        std::vector<const Trk::MeasurementBase*>::iterator mit = selectedHits->begin();
        for (; mit != selectedHits->end(); ++mit) {
            delete *mit;
        }
        delete selectedHits;
        return selectedClusters;
    }

    std::vector<const Trk::MeasurementBase*>::iterator mit = selectedClusters->begin();
    for (; mit != selectedClusters->end(); ++mit) {
        delete *mit;
    }
    delete selectedClusters;
    return selectedHits;
}


void
MuonPhiHitSelector::clusterPhi(const std::vector<Identifier>& id, const std::vector<double>& hitx,
                               const std::vector<double>& hity, const std::vector<double>& hitz,
                               const std::vector<double>& error, const std::vector<double>& pull,
                               std::vector<int>& select, const int n, std::vector<double>& clusterX,
                               std::vector<double>& clusterY, std::vector<double>& clusterZ,
                               std::vector<double>& clusterError, std::vector<Identifier>& clusterId,
                               std::vector<int>& clusterHits, std::vector<int>& clusterSelect,
                               std::vector<int>& clusterInt, int& ncl) const
{

    //
    // Use hits (select > 0) and pulls to make clusters
    //
    //
    // Inputs
    //        id             = identifiers hits
    //        hitx hity hitz = position in space
    //        error          = associated error (in x-y plane)
    //        pull (from fit)= residual (hit -fit) /error
    //        select         = > 0 for selected hits
    //        n              = total number of hits

    // Outputs
    //        clusterX Y Z   = cluster positions
    //        clusterError   =          errors
    //        clusterId      = cluster identifier (smallest pull)
    //        clusterHits    = number of hits per cluster
    //        ncl            = number of clusters
    //         chi2          = total chi2
    //         r0            = perigee parameter of fit (0,0)
    //         phi           = azimuthal angle of fit at perigee


    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Start phi clustering");

    ncl = 0;
    if (n == 0) return;

    std::vector<int> scode(n);

    for (int i = 0; i < n; ++i) {
        Identifier idi  = id[i];
        int        code = 0;
        if (m_idHelperSvc->isRpc(idi)) {
            int doubZ   = m_idHelperSvc->rpcIdHelper().doubletZ(idi);
            int doubPhi = m_idHelperSvc->rpcIdHelper().doubletPhi(idi);
            code        = 100000000 * (m_idHelperSvc->rpcIdHelper().stationName(idi))
                   + 1000000 * (m_idHelperSvc->rpcIdHelper().stationPhi(idi))
                   + 10000 * ((m_idHelperSvc->rpcIdHelper().stationEta(idi)) + 1000);
            code += 1000 * (doubZ - 1) + 100 * (doubPhi - 1);
            code += 2 * ((m_idHelperSvc->rpcIdHelper().doubletR(idi)) - 1)
                    + 16 * ((m_idHelperSvc->rpcIdHelper().gasGap(idi)) - 1);
        } else if (m_idHelperSvc->isTgc(idi)) {
            code = 1000000 * (m_idHelperSvc->tgcIdHelper().stationName(idi))
                   + 10000 * (m_idHelperSvc->tgcIdHelper().stationPhi(idi))
                   + 100 * ((m_idHelperSvc->tgcIdHelper().stationEta(idi)) + 10);
            code = code + m_idHelperSvc->tgcIdHelper().gasGap(idi);
        } else if (m_idHelperSvc->isCsc(idi)) {
            code = 1000000 * (m_idHelperSvc->cscIdHelper().stationName(idi))
                   + 10000 * (m_idHelperSvc->cscIdHelper().stationPhi(idi))
                   + 100 * ((m_idHelperSvc->cscIdHelper().stationEta(idi)) + 10);
            code = code + m_idHelperSvc->cscIdHelper().wireLayer(idi);
        }
        scode[i] = code;
    }

    //    std::vector<int> clusterInt(n);

    for (int i = 0; i < n; ++i) {
        clusterInt[i] = -1;
    }

    int icl = -1;
    for (int i = 0; i < n; ++i) {
        if (error[i] != 0 && select[i] > 0 && clusterInt[i] == -1) {
            icl++;
            clusterInt[i] = icl;
            for (int j = i + 1; j < n; ++j) {
                if (clusterInt[j] == -1) {
                    if (error[j] != 0 && select[j] > 0) {
                        if (scode[i] == scode[j]) clusterInt[j] = icl;
                    }
                }
            }
        }
    }

    std::vector<double> clusterCommon2Error(icl + 1);
    std::vector<int>    clusterCode(icl + 1);
    ncl = icl + 1;
    for (int ic = 0; ic < icl + 1; ++ic) {
        clusterX[ic]            = 0.;
        clusterY[ic]            = 0.;
        clusterZ[ic]            = 0.;
        clusterError[ic]        = 0.;
        clusterCommon2Error[ic] = 0.;
        clusterHits[ic]         = 0;
        clusterCode[ic]         = 0;
        clusterSelect[ic]       = 0;
        double pullMax          = 10.;
        for (int i = 0; i < n; ++i) {
            if (select[i] > 0) {
                if (ic == clusterInt[i]) {
                    clusterSelect[ic] = select[i];
                    double w          = 1. / (error[i] * error[i]);
                    clusterX[ic] += hitx[i] * w;
                    clusterY[ic] += hity[i] * w;
                    clusterZ[ic] += hitz[i] * w;
                    clusterError[ic] += w;
                    if (std::abs(pull[i]) < std::abs(pullMax)) {
                        pullMax           = pull[i];
                        clusterId[ic]     = id[i];
                        clusterCode[ic]   = scode[i];
                        clusterSelect[ic] = select[i];
                    }
                    clusterHits[ic]++;
                    if (clusterHits[ic] == 1) clusterCommon2Error[ic] = 0.;
                }
            }
        }
        clusterX[ic] = clusterX[ic] / clusterError[ic];
        clusterY[ic] = clusterY[ic] / clusterError[ic];
        clusterZ[ic] = clusterZ[ic] / clusterError[ic];
        // Don't assume improvement on errors due to clustering
        clusterError[ic] = std::sqrt(clusterHits[ic]) / std::sqrt(clusterError[ic]);
        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("cluster phi " << ic << " x " << clusterX[ic] << " y " << clusterY[ic] << " z "
                                         << clusterZ[ic] << " error " << clusterError[ic] << " hits " << clusterHits[ic]
                                         << " select " << clusterSelect[ic] << " Code " << clusterCode[ic]);
        }
    }
}
void
MuonPhiHitSelector::fitRecPhi(const double pmom, const std::vector<Identifier>& phiId,
                              const std::vector<double>& phiHitx, const std::vector<double>& phiHity,
                              const std::vector<double>& phiHitz, const std::vector<double>& phiError,
                              std::vector<int>& quality, const int nphi, std::vector<double>& phiPull,
                              std::vector<int>& phiMult, std::vector<int>& phiSelect, double& chi2, double& r0,
                              double& phi, std::vector<double>& errorM, int& nfit) const
{

    //
    // Use reconstructed hits to perform fit for phi
    //

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Start phi fit reconstruction");

    chi2 = 0.;
    r0   = 0.;
    nfit = 0;
    phi  = 0.;

    int ncsc = 0;
    int ntgc = 0;
    int nrpc = 0;


    if (nphi == 0) return;

    std::vector<double> error0(nphi);
    std::vector<double> error(nphi);
    std::vector<double> errorf(nphi);
    std::vector<int>    scode(nphi);
    std::vector<int>    srcode(nphi);
    std::vector<int>    phiSelectKeep(nphi);
    std::map<int, int>  clusters;
    std::map<int, int>  clustersr;
    std::map<int, int>  clusterspat;

    for (int i = 0; i < nphi; ++i) {

        Identifier idi   = phiId[i];
        int        code  = 0;
        int        rcode = 0;
        if (m_idHelperSvc->isRpc(idi)) {
            code = 1000000 * (m_idHelperSvc->rpcIdHelper().stationName(idi))
                   + 10000 * (m_idHelperSvc->rpcIdHelper().stationPhi(idi))
                   + 100 * ((m_idHelperSvc->rpcIdHelper().stationEta(idi)) + 10);
            code = code + 2 * ((m_idHelperSvc->rpcIdHelper().doubletR(idi)) - 1)
                   + 16 * ((m_idHelperSvc->rpcIdHelper().gasGap(idi)) - 1);
            rcode = 1000000 * (m_idHelperSvc->rpcIdHelper().stationName(idi))
                    + 10000 * (m_idHelperSvc->rpcIdHelper().stationPhi(idi))
                    + 0 * ((m_idHelperSvc->rpcIdHelper().stationEta(idi)) + 10);
            rcode = rcode + 2 * ((m_idHelperSvc->rpcIdHelper().doubletR(idi)) - 1)
                    + 16 * ((m_idHelperSvc->rpcIdHelper().gasGap(idi)) - 1);
        } else if (m_idHelperSvc->isTgc(idi)) {
            code = 1000000 * (m_idHelperSvc->tgcIdHelper().stationName(idi))
                   + 10000 * (m_idHelperSvc->tgcIdHelper().stationPhi(idi))
                   + 100 * ((m_idHelperSvc->tgcIdHelper().stationEta(idi)) + 10);
            code  = code + m_idHelperSvc->tgcIdHelper().gasGap(idi);
            rcode = 1000000 * (m_idHelperSvc->tgcIdHelper().stationName(idi))
                    + 10000 * (m_idHelperSvc->tgcIdHelper().stationPhi(idi))
                    + 0 * ((m_idHelperSvc->tgcIdHelper().stationEta(idi)) + 10);
            rcode = rcode + m_idHelperSvc->tgcIdHelper().gasGap(idi);
        } else if (m_idHelperSvc->isCsc(idi)) {
            code = 1000000 * (m_idHelperSvc->cscIdHelper().stationName(idi))
                   + 10000 * (m_idHelperSvc->cscIdHelper().stationPhi(idi))
                   + 100 * ((m_idHelperSvc->cscIdHelper().stationEta(idi)) + 10);
            code  = code + m_idHelperSvc->cscIdHelper().wireLayer(idi);
            rcode = 1000000 * (m_idHelperSvc->cscIdHelper().stationName(idi))
                    + 10000 * (m_idHelperSvc->cscIdHelper().stationPhi(idi))
                    + 0 * ((m_idHelperSvc->cscIdHelper().stationEta(idi)) + 10);
            rcode = rcode + m_idHelperSvc->cscIdHelper().wireLayer(idi);
        }

        scode[i]  = code;
        srcode[i] = rcode;
        int idet  = 0;
        if (m_idHelperSvc->isRpc(idi))
            idet = 1;
        else if (m_idHelperSvc->isTgc(idi))
            idet = 2;
        else if (m_idHelperSvc->isCsc(idi))
            idet = 3;
        phiSelect[i]     = idet;
        phiSelectKeep[i] = idet;
    }
    // Hits on segments
    for (int i = 0; i < nphi; ++i) {
        if (phiError[i] != 0 && quality[i] > 100) {
            clusters[scode[i]]++;
            clustersr[srcode[i]]++;
        }
    }
    // Drop hits on patterns that are in same station and layer as segment hit
    //  Avoid adding again (solved) ambiguous hits

    for (int i = 0; i < nphi; ++i) {
        if (phiError[i] != 0 && quality[i] > 0 && quality[i] < 100) {
            if (clustersr.count(srcode[i]) > 0) {
                quality[i] = 0;
            } else {
                clusterspat[scode[i]]++;
            }
        }
    }

    // Assign errors according to multiplicities
    if (msgLvl(MSG::DEBUG))
        ATH_MSG_DEBUG("phi hits " << nphi << " segment clusters " << clusters.size() << " pattern clusters "
                                  << clusterspat.size());

    for (int i = 0; i < nphi; ++i) {
        error0[i]     = 0;
        Identifier id = phiId[i];
        phiMult[i]    = 0;
        if (phiError[i] != 0 && quality[i] > 0) {
            int n = 0;
            if (quality[i] > 100) {
                n = clusters[scode[i]];
                // Treat phi hits from segment with high multiplicity > 10 as lying on patterm
                if (n > 10) quality[i] = 10;
            } else if (quality[i] < 100) {
                n = clusterspat[scode[i]];
                // Drop phi hits patterns with high multiplicity
                if (clusters.count(scode[i]) == 1 || n > 10) {
                    n = 0;
                    // drop phi hits from pattern if already segment hits in same layer
                    quality[i]       = 0;
                    phiSelect[i]     = 0;
                    phiSelectKeep[i] = 0;
                    continue;
                }
            }
            phiMult[i]  = n;
            double fact = 1.;
            if (m_idHelperSvc->isRpc(id))
                fact = 1.2;
            else if (m_idHelperSvc->isTgc(id))
                n = 1;
            else if (m_idHelperSvc->isCsc(id))
                n = 1;

            error0[i]     = phiError[i] * std::sqrt(n) * fact;
            error[i]      = phiError[i] * std::sqrt(n) * fact;
            double phiHit = std::atan2(phiHity[i], phiHitx[i]);
            if (msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG(i << " Station " << int(scode[i] / 1000000) << " Hit x " << phiHitx[i] << " Hit y "
                                << phiHity[i] << " Hit z " << phiHitz[i] << " error " << phiError[i] << " phi Hit "
                                << phiHit);
                ATH_MSG_DEBUG("station " << phiSelect[i]);
                ATH_MSG_DEBUG("code " << scode[i] << " multiplicity " << n << " error " << error0[i] << " quality "
                                      << quality[i]);
                if (error0[i] < 1.) ATH_MSG_DEBUG("TOO small error ");
            }
        }
    }

    // Count layers hit

    std::map<int, int> layersHit;
    for (int i = 0; i < nphi; ++i) {
        if (phiError[i] != 0 && quality[i] > 0) {
            layersHit[srcode[i]]++;
        }
    }
    int    allLayerHits     = layersHit.size();
    int    allLayerRecoHits = 0;
    double pfit             = 20000.;

    for (int iqua = 0; iqua < 3; ++iqua) {

        double quacut = 10;
        if (iqua == 1)
            quacut = 0;
        else if (iqua == 2) {
            quacut = 10;
            pfit   = pmom;
        }

        if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Quality loop " << iqua << " quality cut " << quacut);
        int nsel    = 0;
        int nselseg = 0;
        for (int i = 0; i < nphi; ++i) {

            if (iqua == 1) phiSelect[i] = phiSelectKeep[i];

            if (phiError[i] != 0 && quality[i] > quacut) {
                nsel++;
                if (quality[i] > 100) nselseg++;
                if (quality[i] == 10 && iqua == 1) quality[i] = 11;
            } else {
                phiSelect[i] = 0;
            }
            if (msgLvl(MSG::DEBUG))
                ATH_MSG_DEBUG("index i " << i << " phiSelect " << phiSelect[i] << " Quality " << quality[i] << " error "
                                         << error[i]);
        }

        int imax = -1;
        if (iqua == 1 && nselseg > 0) {
            // Test and drop pattern Hits if far off
            double           errorScaleFactor = 25.;
            std::vector<int> phiPatSelect(nphi, 0);
            for (int i = 0; i < nphi; ++i) {
                phiPatSelect[i] = 0;
                if (phiSelect[i] > 0 && quality[i] > 0 && quality[i] < 100) {
                    phiPatSelect[i] = 1;
                    error[i]        = errorScaleFactor * error[i];
                }
                if (msgLvl(MSG::DEBUG))
                    ATH_MSG_DEBUG("select " << phiSelect[i] << " quality " << quality[i] << " error " << error[i]);
            }
            if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("performing outlier removal for pattern hits ");
            fitPhiSL(pfit, phiId, phiHitx, phiHity, phiHitz, error, phiSelect, nphi, phiPull, imax, chi2, r0, phi,
                     errorM, false);
            for (int i = 0; i < nphi; ++i) {
                if (phiPatSelect[i] == 1) {
                    error[i]            = error[i] / errorScaleFactor;
                    double rescaledPull = phiPull[i] * errorScaleFactor;
                    // 3 sigma cut
                    if (std::abs(rescaledPull) < 3.) {
                        phiSelect[i] = phiSelectKeep[i];
                    } else {
                        phiSelect[i]     = 0;
                        phiSelectKeep[i] = 0;
                        if (msgLvl(MSG::DEBUG))
                            ATH_MSG_DEBUG("Drop Pattern Hits with Quality == 1 "
                                          << i << " quality " << quality[i] << " Pull " << rescaledPull << " phiSelect "
                                          << phiSelect[i]);
                    }
                }
            }
        }

        const double pfitc = pfit;
        imax               = -1;

        if (iqua == 2) {
            // low momentum fit with scaled error (factor 10) for dropped segment hits
            std::vector<int> phiReSelect(nphi);
            for (int i = 0; i < nphi; ++i) {
                if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("select " << phiSelect[i] << " quality " << quality[i]);
                phiReSelect[i] = 0;
                if (phiSelect[i] == 0 && quality[i] > 99) {
                    phiReSelect[i] = 1;
                    phiSelect[i]   = phiSelectKeep[i];
                    error[i]       = 10. * error[i];
                }
            }
            fitPhiSL(pfitc, phiId, phiHitx, phiHity, phiHitz, error, phiSelect, nphi, phiPull, imax, chi2, r0, phi,
                     errorM, false);
            for (int i = 0; i < nphi; ++i) {
                if (phiReSelect[i] == 1) {
                    error[i] = error[i] / 10.;
                    // 10 sigma cut (error rescale = 10)
                    if (std::abs(phiPull[i]) < 1) {
                        phiSelect[i] = phiSelectKeep[i];
                    } else {
                        phiSelect[i] = 0;
                    }
                    if (msgLvl(MSG::DEBUG))
                        ATH_MSG_DEBUG("Low momentum Quality == 2 add hit  nr "
                                      << i << " quality " << quality[i] << " Pull " << phiPull[i] << " phiSelect "
                                      << phiSelect[i]);
                }
            }
        }
        if (iqua == 1 && msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Quality loop ");
        nsel = 0;
        for (int i = 0; i < nphi; ++i) {
            errorf[i] = error[i];
            if (iqua == 1) phiSelect[i] = phiSelectKeep[i];

            if (phiError[i] != 0 && quality[i] > quacut) {
                nsel++;
                if (quality[i] == 10 && iqua == 1) quality[i] = 11;
            } else {
                phiSelect[i] = 0;
            }
        }

        if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Selected PHI hits in fit " << nsel << " iqua " << iqua);
        if (nsel == 0) continue;

        int niter = -1;
        // do hit dropping in maximal 10 iterations by putting quality to 0

        for (int iter = 0; iter < 100; ++iter) {
            niter++;
            double power = (iter - 10) / 20.;
            if (power < 0.) power = 0.;
            chi2 = 0.;
            nfit = 0;
            if (iter > 10) {
                // Shower treatment inflate errors with multiplicity
                for (int i = 0; i < nphi; ++i) {
                    errorf[i] = error[i] * std::pow(phiMult[i], power);
                }
            }
            fitPhiSL(pfitc, phiId, phiHitx, phiHity, phiHitz, errorf, phiSelect, nphi, phiPull, imax, chi2, r0, phi,
                     errorM, false);

            ncsc = 0;
            ntgc = 0;
            nrpc = 0;

            // Count layers hit in Reconstruction

            std::map<int, int> layersRecoHit;

            for (int i = 0; i < nphi; ++i) {
                Identifier id = phiId[i];
                if (error[i] == 0 || quality[i] < quacut) phiSelect[i] = 0;
                if (error[i] != 0 && quality[i] > quacut) {
                    layersRecoHit[srcode[i]]++;
                    if (msgLvl(MSG::DEBUG)) {
                        if (m_idHelperSvc->isRpc(id))
                            nrpc++;
                        else if (m_idHelperSvc->isTgc(id))
                            ntgc++;
                        else if (m_idHelperSvc->isCsc(id))
                            ncsc++;
                    }
                    nfit++;
                }
            }
            allLayerRecoHits = layersRecoHit.size();
            double frac      = allLayerRecoHits / (allLayerHits + 0.0001);

            if (nfit == 1) break;

            if (imax < 0 || imax > nphi) {
                if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Fitphi imax " << imax);
                break;
            }

            if (chi2 < 5 * (nfit + 1) || std::abs(phiPull[imax]) < 3.0) {

                if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Final phi " << phi << " frac " << frac << " chi2 " << chi2);
                break;
            }

            phiSelect[imax] = 0;

            if (msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG("Start hit dropping " << imax << " pullmax " << phiPull[imax] << " phi " << phi
                                                    << " chi2 " << chi2);
            }
        }

        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("Fit results phi " << phi << " chi2 " << chi2 << " ndof " << nfit);
            ATH_MSG_DEBUG("Reco RPC " << nrpc << " TGC " << ntgc << " CSC " << ncsc);
        }


        int nacc        = 0;
        int nshowerdrop = 0;
        for (int i = 0; i < nphi; ++i) {
            double power = (niter - 10) / 20.;
            if (power < 0.) power = 0.;
            double pull = phiPull[i] * std::pow(phiMult[i], power);
            if (niter > 10 && std::abs(pull) > 3.0 && phiSelect[i] > 0) {
                phiSelect[i] = 0;
                quality[i]   = 0;
                nshowerdrop++;
                if (msgLvl(MSG::DEBUG))
                    ATH_MSG_DEBUG("Drop shower hit i " << i << " with pull " << pull << " iterations " << niter
                                                       << " power " << power);
            }
            if (phiSelect[i] != 0) nacc++;
        }
        if (msgLvl(MSG::DEBUG))
            ATH_MSG_DEBUG("phi hits " << nphi << " selected for fit " << nfit << " iqua " << iqua << " iterations "
                                      << niter << " accepted hits " << nacc << " nshower drop " << nshowerdrop);
    }
}

void
MuonPhiHitSelector::fitPhiSL(const double pmom, const std::vector<Identifier>& /*id*/, const std::vector<double>& hitx,
                             const std::vector<double>& hity, const std::vector<double>& hitz,
                             const std::vector<double>& error, std::vector<int>& select, const int n,
                             std::vector<double>& pull, int& imax, double& chi2, double& r0, double& phi,
                             std::vector<double>& errorM, bool fast) const
{

    // Perform straight line fit to hits: good hits have select > 0
    // in the fit scattering centres are added for nfit-1 angles
    //            WITH  beamspot constraint
    //                degrees of freedom = 2*nfit

    // Fit is based on matrix inversions formulae

    // Inputs pmom           = estimate of momentum
    //        id             = identifiers hits
    //        hitx hity hitz = position in space
    //        error          = associated error (in x-y plane)
    //        select         = > 0 for selected hits
    //        n              = total number of hits


    // Outputs pull          = residual (hit -fit) /error
    //         imax          = index for hit with maximum pull
    //         chi2          = total chi2
    //         r0            = perigee parameter of fit (0,0)
    //         phi           = azimuthal angle of fit at perigee

    double pest = pmom;
    if (pest > 20000.) pest = 20000.;

    r0   = 0.;
    phi  = 0.;
    chi2 = 0.;
    imax = 0;

    // Calculate mean position
    double xm   = 0.;
    double ym   = 0.;
    double dtot = 0.;
    double em   = 0.;
    for (int i = 0; i < n; ++i) {
        if (error[i] != 0 && select[i] > 0) {
            double inver2 = 1. / (error[i] * error[i]);
            xm += hitx[i] * inver2;
            ym += hity[i] * inver2;
            dtot += std::sqrt(hitx[i] * hitx[i] + hity[i] * hity[i] + hitz[i] * hitz[i]) * inver2;
            em += inver2;
        }
    }

    if (em == 0) return;

    dtot = dtot / em;

    //    Beamspot error 10 mm for cosmics 10000

    double ebs = 0.1;
    if (m_cosmics) ebs = 10000.;

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("pmom " << pmom << " error beam " << ebs);
    double ebs2    = ebs * ebs;
    double invebs2 = 1. / ebs2;
    double xmc     = xm / (em + invebs2);
    double ymc     = ym / (em + invebs2);
    xm             = xm / em;
    ym             = ym / em;

    // Constraint on beam spot

    double len2 = xmc * xmc + ymc * ymc;
    double xcc  = len2 * xmc * invebs2;
    double ycc  = len2 * ymc * invebs2;

    for (int i = 0; i < n; ++i) {
        if (error[i] != 0 && select[i] > 0) {
            double inver2 = 1. / (error[i] * error[i]);
            double xdiff  = hitx[i] - xmc;
            double ydiff  = hity[i] - ymc;
            double xdiff2 = xdiff * xdiff;
            double ydiff2 = ydiff * ydiff;
            len2          = xdiff2 + ydiff2;
            double sign   = 1.;
            // Non Cosmics assume IP at 0 0
            if (xdiff * hitx[i] + ydiff * hity[i] < 0 && !m_cosmics) sign = -1;
            // Cosmics assume down going
            if (ydiff < 0 && m_cosmics) sign = -1;
            xcc += len2 * sign * xdiff * inver2;
            ycc += len2 * sign * ydiff * inver2;
        }
    }

    if (em > 0) phi = std::atan2(ycc, xcc);
    CxxUtils::sincos scphi(phi);

    r0        = xmc * scphi.sn - ymc * scphi.cs;
    double x0 = r0 * scphi.sn;
    double y0 = -r0 * scphi.cs;

    if (msgLvl(MSG::DEBUG))
        ATH_MSG_DEBUG("Constraint r0 " << r0 << " xpos " << xmc << " ypos " << ymc << " phi " << phi);
    // assume 0,0
    std::vector<double>   d(n);
    std::vector<double>   dist(n);
    std::map<double, int> distanceSort;
    double                pullmax = 0.;
    for (int i = 0; i < n; ++i) {
        if (error[i] != 0 && select[i] > 0) {
            double xdiff          = hitx[i] - x0;
            double ydiff          = hity[i] - y0;
            double xdiff2         = xdiff * xdiff;
            double ydiff2         = ydiff * ydiff;
            d[i]                  = std::sqrt(xdiff2 + ydiff2);
            dist[i]               = std::sqrt(xdiff2 + ydiff2 + hitz[i] * hitz[i]);
            distanceSort[dist[i]] = i;
            pull[i]               = hitx[i] * scphi.sn - hity[i] * scphi.cs - r0;
            if (std::abs(pull[i]) > std::abs(pullmax)) {
                pullmax = pull[i];
                imax    = i;
            }
        }
    }

    if (fast) return;

    std::map<double, int>::iterator it     = distanceSort.begin();
    std::map<double, int>::iterator it_end = distanceSort.end();

    int                 nfit = 0;
    std::vector<double> xf(2 * n);
    std::vector<double> lf(2 * n);
    std::vector<double> yf(2 * n);
    std::vector<double> ef(2 * n);
    std::vector<int>    indexf(n);
    //
    //      measurements yf error ef at distance xf (0:nfit)
    //      scattering centra angle zero yf error ef at distance xf(nfit+1..2nfit-1)
    //      beamspot at yf(2 nfit) = 0 error ebs2 at distance xf(2 nfit)

    for (; it != it_end; ++it) {
        int index    = it->second;
        xf[nfit]     = d[index];
        lf[nfit]     = dist[index];
        yf[nfit]     = (hitx[index] - xmc) * scphi.sn - (hity[index] - ymc) * scphi.cs;
        ef[nfit]     = error[index];
        indexf[nfit] = index;
        nfit++;
    }

    // NB start at 1 to add scattering centra

    double erang = 0.030 * 5000. / (pest + 1000.);
    for (int i = 1; i < nfit; ++i) {
        xf[nfit + i - 1] = xf[i - 1];
        yf[nfit + i - 1] = 0.;
        double scale     = 1.;
        if (select[i] == 1)
            scale = 1.;
        else if (select[i] == 3)
            scale = 0.5;
        else if (select[i] == 2)
            scale = 2.5;
        ef[nfit + i - 1] = scale * erang;
    }
    // Beamspot
    yf[2 * nfit - 1] = 0.;
    xf[2 * nfit - 1] = 0.;
    ef[2 * nfit - 1] = ebs;

    Amg::MatrixX v(nfit + 1, 1);
    v.setIdentity();

    if (msgLvl(MSG::DEBUG))
        ATH_MSG_DEBUG("fitPhiSL "
                      << " nfit " << nfit);

    for (int i = 0; i < nfit + 1; ++i) {
        v(i, 0) = 0.;
        for (int j = 0; j < nfit; ++j) {
            double inver2 = 1. / (ef[j] * ef[j]);
            if (i == 0)
                v(i, 0) += yf[j] * inver2;
            else if (i == 1)
                v(i, 0) += yf[j] * xf[j] * inver2;
            else if (i > 1 && j > i - 2) {
                v(i, 0) += yf[j] * (lf[j] - lf[i - 2]) * inver2;
            }
        }
    }

    // Track Model Matrix

    Amg::MatrixX model(nfit + 1, 2 * nfit);
    model.setIdentity();
    // Measurements related to position and slope

    for (int i = 0; i < nfit + 1; ++i) {
        for (int j = 0; j < nfit; ++j) {
            model(i, j) = 0.;
            if (i == 0)
                model(i, j) = 1.;
            else if (i == 1)
                model(i, j) = xf[j];
            // scattering angle
            else if (i > 1 && j > i - 2)
                model(i, j) = lf[j] - lf[i - 2];
        }
    }

    // Constraints on scattering angles and beamspot

    for (int i = 0; i < nfit + 1; ++i) {
        for (int j = nfit; j < 2 * nfit; ++j) {
            model(i, j) = 0.;
            // scattering angle
            if (i == j - nfit + 2) model(i, j) = 1.;
            // Beam spot
            if (i == 0 && j == 2 * nfit - 1) model(i, j) = 1.;
        }
    }

    // Covariance Inverse of Track parameters

    Amg::MatrixX covT(nfit + 1, nfit + 1);
    for (int i = 0; i < nfit + 1; ++i) {
        for (int j = 0; j < nfit + 1; ++j) {
            covT(i, j) = 0.;
            for (int k = 0; k < 2 * nfit; ++k) {
                double er2 = ef[k] * ef[k];
                covT(i, j) += model(i, k) * model(j, k) / er2;
            }
        }
    }

    // Invert covariance matrix and replace it (should be CovT)
    Amg::MatrixX covTI = covT.inverse();

    Amg::MatrixX t(nfit + 1, 1);
    // Solution for Track parameters
    t = covTI * v;

    if (msgLvl(MSG::DEBUG) && std::abs(t(1, 0)) > 0.2) {
        ATH_MSG_DEBUG("Don't trust fit result " << t(1, 0) << " Keep Old result");
    }
    if (std::abs(t(1, 0)) > 0.2) return;

    // calculate residuals and chi2
    std::vector<double> resi(2 * nfit);
    std::vector<double> pulli(2 * nfit);
    std::vector<double> errf(2 * nfit);
    std::vector<double> pullf(2 * nfit);
    std::vector<double> resiOut(2 * nfit);
    std::vector<double> pullOut(2 * nfit);
    pullmax  = 0.;
    int jmax = 0;

    errorM[0] = covTI(0, 0);
    errorM[1] = covTI(0, 1);
    errorM[2] = covTI(1, 1);
    errorM[3] = 0.;
    if (nfit > 2) {
        double invlt = 1. / (lf[nfit - 1] - lf[1]);
        for (int i = 1; i < nfit - 1; ++i) {
            double w = (lf[nfit - 1] - lf[i]) * invlt;
            errorM[3] += covTI(i + 1, i + 1) * w * w;
        }
    }

    if (msgLvl(MSG::DEBUG)) {
        if (nfit >= 3) {
            ATH_MSG_DEBUG("Error angle " << covTI(3, 3));
        }  // covTI has dim nfit+1
        ATH_MSG_DEBUG("errorM[3] " << errorM[3]);
    }

    for (int i = 0; i < 2 * nfit; ++i) {

        // Calculate         prediction at each measurement i
        //           propagate error of track parameters to measurement i
        double error2 = 0.;
        double ypred  = 0.;
        for (int j = 0; j < nfit + 1; ++j) {
            if (msgLvl(MSG::DEBUG) && i == 0) ATH_MSG_DEBUG("Parameter j " << j << " t(j,0) " << t(j, 0));
            if (msgLvl(MSG::DEBUG) && model(j, i) != 0) ATH_MSG_DEBUG("i " << i << " model ij " << model(j, i));
            ypred += model(j, i) * t(j, 0);
            for (int k = 0; k < nfit + 1; ++k) {
                error2 += model(j, i) * covTI(j, k) * model(k, i);
            }
        }
        double ef_i2     = ef[i] * ef[i];
        double inv_ef_i2 = 1. / ef_i2;

        resi[i]  = ypred - yf[i];
        pulli[i] = resi[i] / ef[i];

        // errf propagated error and pullf
        errf[i]  = std::sqrt(error2);
        pullf[i] = resi[i] / errf[i];

        // calculate residual without hit and error without hit
        //    Think of Kalmanm method to exclude hit and error
        double err2invOut = 1. / error2 - inv_ef_i2;
        if (err2invOut > 0) {
            resiOut[i] = (ypred / error2 - yf[i] * inv_ef_i2) / err2invOut - yf[i];
            pullOut[i] = resiOut[i] / std::sqrt(1. / err2invOut + ef_i2);
        }

        if (std::abs(pullOut[i]) > std::abs(pullmax) && i < nfit) {
            imax    = indexf[i];
            jmax    = i;
            pullmax = pullOut[i];
        }
        chi2 += resi[i] * resi[i] * inv_ef_i2;

        if (i < nfit) {
            pull[indexf[i]] = pullOut[i];
        }
        if (msgLvl(MSG::DEBUG) && i < nfit)
            ATH_MSG_DEBUG("i " << i << " index " << indexf[i] << " det " << select[indexf[i]] << " ypred " << ypred
                               << " mst " << yf[i] << " residual " << resi[i] << " error " << ef[i] << " dist "
                               << dist[i] << " hitz " << hitz[i] << " Pull " << pulli[i] << " Pullf " << pullf[i]
                               << " resi out " << resiOut[i] << " pull out " << pullOut[i]);
        if (msgLvl(MSG::DEBUG) && i > nfit)
            ATH_MSG_DEBUG("i " << i << " ypred " << ypred << " mst " << yf[i] << " residual " << resi[i] << " error "
                               << ef[i]);
    }
    r0  = r0 + t(0, 0);
    phi = phi + t(1, 0);

    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("delta phi " << t(1, 0));
    if (msgLvl(MSG::DEBUG) && std::abs(t(1, 0)) > 0.1) ATH_MSG_DEBUG("ALARM delta phi " << t(1, 0));

    if (msgLvl(MSG::DEBUG))
        ATH_MSG_DEBUG("Track parameters r0 " << r0 << " phi " << phi << " chi2 " << chi2 << " jmax " << jmax << " imax "
                                             << imax << " pullmax " << pullmax);
}
