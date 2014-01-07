/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "TrkRoad/TrackRoad.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkExInterfaces/IIntersector.h"

#include "TGC_LinearSegmentMakerTool/TGC_LinearSegmentMakerTool.h"
#include "MuonLinearSegmentMakerUtilities/Fit2D.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include <sstream>
#include <set>


TGC_LinearSegmentMakerTool::TGC_LinearSegmentMakerTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* pIID) :
AlgTool(type, name, pIID),
m_pIdHelper(NULL),
m_pIntersector("Trk::RungeKuttaIntersector/Trk::RungeKuttaIntersector")
{
    declareInterface<Muon::IMuonSegmentMaker>(this);
    declareProperty("OutlierChi2", m_fExclChi2 = 10.0);    
    declareProperty("Intersector", m_pIntersector);
}

StatusCode TGC_LinearSegmentMakerTool::initialize()
{
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "Initializing TGC_LinearSegmentMakerTool - package version " << PACKAGE_VERSION << endreq;
    StatusCode sc = StatusCode::SUCCESS;

    // initialize TgcIdHelper
    StoreGateSvc* pDetStore = NULL;
    sc = svcLoc()->service("DetectorStore", pDetStore);
    if (sc.isFailure() || pDetStore == NULL)
    {
        log << MSG::ERROR << "Cannot locate DetectorStore" << endreq;
        return sc;
    }
    sc = pDetStore->retrieve(m_pMuonMgr);
    if (sc.isFailure() || m_pMuonMgr == NULL)
    {
        log << MSG::ERROR << "Cannot retrieve MuonDetectorManager" << endreq;
        return sc;
    }
    m_pIdHelper = m_pMuonMgr->tgcIdHelper(); 
    sc = m_pIntersector.retrieve();   
    if (sc.isFailure())
    {    
        log << MSG::ERROR << "Cannot retrieve IIntersector" << endreq;
        return sc;
    }
    return StatusCode::SUCCESS;
}

StatusCode TGC_LinearSegmentMakerTool::Finalize()
{
    return StatusCode::SUCCESS;
}

static std::string point2String(const Amg::Vector3D& pt)
{
    std::ostringstream oss;
    oss << "eta=" << pt.eta() << ",phi=" << pt.phi() << ",perp=" << pt.perp() << ",z=" << pt.z();
    return oss.str();
}

static std::string dir2String(const Amg::Vector3D& dir)
{
    std::ostringstream oss;
    oss << "eta=" << dir.eta() << ",phi=" << dir.phi()<< ",z=" << dir.z();
    return oss.str();
}

std::vector<const Muon::MuonSegment*>*
TGC_LinearSegmentMakerTool::find(const Trk::TrackRoad& road,
                                 const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >&,
                                 const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >& clusters,
                                 bool,double)
{
    MsgStream log(msgSvc(), name());
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG << "TGC_LinearSegmentMakerTool::find" << endreq;

    Muon::Fit2D::PointArray rhoPoints, phiPoints;
    std::vector<const Muon::MuonClusterOnTrack*> rios;
    for (std::vector<std::vector<const Muon::MuonClusterOnTrack*> >::const_iterator itClusters = clusters.begin();
        itClusters != clusters.end();
        itClusters++)
    {
        const std::vector<const Muon::MuonClusterOnTrack*>& cluster = *itClusters;
        rios.insert(rios.end(), cluster.begin(), cluster.end());
    }

    const MuonGM::TgcReadoutElement* pReadoutElement =
    dynamic_cast<const MuonGM::TgcReadoutElement*>(rios.front()->detectorElement());
    double baseZ = pReadoutElement->center().z();
//    bool bIsInner = pReadoutElement->stationType()[1] == '4';

    std::set<std::string> rhoStations, phiStations;
    int iHit = 0;
    for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator itHit = rios.begin();
        itHit != rios.end();
        itHit++, iHit++)
    {
        const Muon::MuonClusterOnTrack* pHit = *itHit;
        const Amg::MatrixX & errMat = pHit->localCovariance();
        if (errMat.determinant()==0.)
        {
//            log << MSG::INFO << "RIO has a singular covariance matrix. Skipping" << endreq;
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "RIO has a singular error matrix. Skipping" << endreq;
                continue;
        }
        Amg::MatrixX wheight= errMat.inverse();
        double w = wheight(Trk::locX,Trk::locX);
        if (m_pIdHelper->isStrip(pHit->identify()))
        {
            double rho = pHit->globalPosition().perp();
            w *= rho * rho;
            Muon::Fit2D::Point* pPt = new Muon::Fit2D::Point(iHit,
                                                             pHit->globalPosition().z(),
                                                             pHit->globalPosition().phi(),
                                                             w,
                                                             (void*)pHit);
            phiPoints.push_back(pPt);
            phiStations.insert(dynamic_cast<const MuonGM::TgcReadoutElement*>(pHit->detectorElement())->getStationName());
        }
        else
        {
            Muon::Fit2D::Point* pPt = new Muon::Fit2D::Point(iHit,
                                                             pHit->globalPosition().z(),
                                                             pHit->globalPosition().perp(),
                                                             w,
                                                             (void*)pHit);
            rhoPoints.push_back(pPt);
            rhoStations.insert(dynamic_cast<const MuonGM::TgcReadoutElement*>(pHit->detectorElement())->getStationName());
        }
    }
    std::vector<const Muon::MuonSegment*>* pMuonSegs = NULL;
    double dChi2 = 0.0;
    int nDegf = 0;
    double zCov[2][2], phiCov[2][2];
    memset(zCov, 0, sizeof(zCov));
    memset(phiCov, 0, sizeof(phiCov));
    if (!rhoPoints.empty() && !phiPoints.empty())
    {
        Muon::Fit2D fit;
        Amg::Vector3D pos;
        Amg::Vector3D dir = road.globalDirection();
        Muon::Fit2D::SimpleStats rhoSimpleStats, phiSimpleStats;
        Muon::Fit2D::LinStats rhoLinStats, phiLinStats;
        if (rhoStations.size() == 1 || rhoPoints.size() < 4)
        {
//            if (stations.size() == 1 && rhoPoints.size() >= 4)
//                std::cout << "TGC_LinearSegmentMakerTool::find 1 station with "
//                << rhoPoints.size() <<" eta points" << std::endl;
            fit.fitPoint(rhoPoints, m_fExclChi2, log.level() <= MSG::DEBUG, rhoSimpleStats);
            fit.fitPoint(phiPoints, m_fExclChi2, log.level() <= MSG::DEBUG, phiSimpleStats);
            if (log.level() <= MSG::DEBUG)
            {
                log << MSG::DEBUG << "Rho: " << rhoSimpleStats.toString() << endreq;
                log << MSG::DEBUG << "Phi: " << phiSimpleStats.toString() << endreq;
            }
            pos[0]=rhoSimpleStats.fMean;
            pos[1]=0.;
            pos[2]=baseZ;
            Amg::setPhi(pos, phiSimpleStats.fMean);
            dChi2 = rhoSimpleStats.fChi2 + phiSimpleStats.fChi2;
            nDegf = rhoSimpleStats.n + phiSimpleStats.n - 2;
            zCov[0][0]=(rhoSimpleStats.fStd)*(rhoSimpleStats.fStd);
        }
        else
        {
            double rho, rhoErr, phi, phiErr, z = baseZ;
            fit.fitLine(rhoPoints, m_fExclChi2, log.level() <= MSG::DEBUG, rhoLinStats);
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "Rho: " << rhoLinStats.toString() << endreq;
            rhoLinStats.eval(z, rho, rhoErr);
            pos[0]=rho;
            pos[1]=0.0;
            pos[2]=z;
            dChi2 = rhoLinStats.fChi2;
            nDegf = rhoLinStats.n - 2;
            if (phiStations.size() == 1 || phiPoints.size() < 4)
            {
                fit.fitPoint(phiPoints, m_fExclChi2, log.level() <= MSG::DEBUG, phiSimpleStats);
                if (log.level() <= MSG::DEBUG)
                    log << MSG::DEBUG << "Phi: " << phiSimpleStats.toString() << endreq;
                Amg::setPhi(pos, phiSimpleStats.fMean);
                phiLinStats.fSlope = 0.0;
                dChi2 += phiSimpleStats.fChi2;
                nDegf += phiSimpleStats.n - 1;
                phiCov[0][0]=(phiSimpleStats.fStd)*(phiSimpleStats.fStd);
            }
            else
            {
                fit.fitLine(phiPoints, m_fExclChi2, log.level() <= MSG::DEBUG, phiLinStats);
                if (log.level() <= MSG::DEBUG)
                    log << MSG::DEBUG << "Phi: " << phiLinStats.toString() << endreq;
                phiLinStats.eval(z, phi, phiErr);
                Amg::setPhi(pos, phi);
                dChi2 += phiLinStats.fChi2;
                nDegf += phiLinStats.n - 2;
                memcpy(phiCov, phiLinStats.fCov, sizeof(phiCov));
            }
            double z1 = pos.z() + copysign(1.0, pos.z());
            rhoLinStats.eval(z1, rho, rhoErr);
            Amg::Vector3D p1(rho, 0.0, z1);
            phiLinStats.eval(z1, phi, phiErr);
            Amg::setPhi(p1, phi);
            dir = p1 - pos;
        }
        if (log.level() <= MSG::DEBUG)
        {
            log << MSG::DEBUG << "Segment position " << point2String(pos) << endreq;
            log << MSG::DEBUG << "Segment direction " << dir2String(dir) << endreq;
        }

        const Trk::Surface& surface = pReadoutElement->surface();

        const Muon::TgcClusterOnTrack* pTgcCOT = dynamic_cast<const Muon::TgcClusterOnTrack*>(rios.front());
        if (pTgcCOT == NULL)
        {
            log << MSG::WARNING << "Cannot convert Muon::TgcClusterOnTrack to Muon::TgcClusterOnTrack" << endreq;
            goto done;
        }
//        const Trk::Surface& surface = pTgcCOT->associatedSurface();
        if (log.level() <= MSG::DEBUG)
        {
//             surface.dump(log);
//             const Amg::Vector3D& pos = surface.center();
            log << MSG::DEBUG << "Surface at " << point2String(surface.center()) << endreq;
        }
        const Trk::PlaneSurface* pSurface = dynamic_cast<const Trk::PlaneSurface*>(&surface);
        if (pSurface == NULL)
        {
            log << MSG::WARNING << "Cannot get a PlaneSurface from TgcClusterOnTrack" << endreq;
            goto done;
        }        
        Trk::TrackSurfaceIntersection baseIsect(pos, dir, 0.0);        
        const Trk::TrackSurfaceIntersection* pNewBaseIsect = m_pIntersector->intersectSurface(surface, &baseIsect, 0.0);        
        Amg::Vector2D pSegPos;
        bool loc_pos_ok(false);
	if(pNewBaseIsect) {
	  loc_pos_ok=pSurface->globalToLocal(pNewBaseIsect->position(), Amg::Vector3D(0., 0., 0.), pSegPos);
	  delete pNewBaseIsect;
	}
        if (!loc_pos_ok)
        {
            log << MSG::WARNING << "Cannot get LocalPosition from surface for " << point2String(pos) << endreq;
            goto done;
        }
        if (log.level() <= MSG::DEBUG)
            log << MSG::DEBUG << "Segment local position is"
            << " x=" << pSegPos[Trk::locX]
            << ",y=" << pSegPos[Trk::locY] << endreq;
        Trk::LocalDirection pSegDir;
        pSurface->globalToLocalDirection(dir.unit(), pSegDir);
        if (log.level() <= MSG::DEBUG)
            log << MSG::DEBUG << "Segment local direction is"
            << " angleXZ=" << pSegDir.angleXZ()
            << ",angleYZ=" << pSegDir.angleYZ() << endreq;

        AmgSymMatrix(4)  pcov;
        pcov(0,0) = zCov[0][0];
        pcov(0,1) = zCov[0][1];
        pcov(1,1) = zCov[1][1];
        pcov(2,2) = phiCov[0][0];
        pcov(2,3) = phiCov[0][1];
        pcov(3,3) = phiCov[1][1];
        pcov(0,2) = 0.0;
        pcov(0,3) = 0.0;
        pcov(1,2) = 0.0;
        pcov(1,3) = 0.0;

        Trk::FitQuality* pFitQuality = new Trk::FitQuality(dChi2, nDegf);
        DataVector<const Trk::MeasurementBase>* pRios = new DataVector<const Trk::MeasurementBase>;
        for (Muon::Fit2D::PointArray::const_iterator itPt = rhoPoints.begin(); itPt != rhoPoints.end(); itPt++)
        {
            Muon::Fit2D::Point* pPt = *itPt;
            if (!pPt->bExclude)
            {
                pRios->push_back(static_cast<const Trk::MeasurementBase*>(((const Muon::MuonClusterOnTrack*)(pPt->pData))->clone()));
            }
        }
        for (Muon::Fit2D::PointArray::const_iterator itPt = phiPoints.begin(); itPt != phiPoints.end(); itPt++)
        {
            Muon::Fit2D::Point* pPt = *itPt;
            if (!pPt->bExclude)
            {
                pRios->push_back(static_cast<const Trk::MeasurementBase*>(((const Muon::MuonClusterOnTrack*)(pPt->pData))->clone()));
            }
        }
        Muon::MuonSegment* pMuonSeg = new Muon::MuonSegment(pSegPos,
                                                            pSegDir,
                                                            pcov,
                                                            const_cast<Trk::PlaneSurface*>(pSurface->clone()),
                                                            pRios,
                                                            pFitQuality);
        if (log.level() <= MSG::DEBUG)
//            pMuonSeg->dump(log);
            log << MSG::DEBUG << "Created a new Muon::MuonSegment" << endreq;
        pMuonSegs = new std::vector<const Muon::MuonSegment*>(1);
        (*pMuonSegs)[0] = pMuonSeg;
//        delete pSegDir;
    }
done:
    for (Muon::Fit2D::PointArray::const_iterator itPt = rhoPoints.begin(); itPt != rhoPoints.end(); itPt++)
        delete *itPt;
    for (Muon::Fit2D::PointArray::const_iterator itPt = phiPoints.begin(); itPt != phiPoints.end(); itPt++)
        delete *itPt;

    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG << "TGC_LinearSegmentMakerTool::find ended" << endreq;
    return pMuonSegs;
}

