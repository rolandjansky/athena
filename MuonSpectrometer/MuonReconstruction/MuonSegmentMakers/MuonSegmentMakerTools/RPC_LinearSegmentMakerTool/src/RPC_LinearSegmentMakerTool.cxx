/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_LinearSegmentMakerTool/RPC_LinearSegmentMakerTool.h"
#include "MuonLinearSegmentMakerUtilities/Fit2D.h"
#include "MuonLinearSegmentMakerUtilities/MuonLinearSegmentMakerUtilities.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkRoad/TrackRoad.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

RPC_LinearSegmentMakerTool::RPC_LinearSegmentMakerTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* pIID) :
AthAlgTool(type, name, pIID),
m_pIdHelper(NULL)
{
    declareInterface<Muon::IMuonSegmentMaker>(this);
    declareProperty("OutlierChi2", m_fExclChi2 = 10.0);
}

StatusCode RPC_LinearSegmentMakerTool::initialize()
{
    ATH_MSG_INFO("Initializing RPC_LinearSegmentMakerTool - package version " << PACKAGE_VERSION );
    StatusCode sc = StatusCode::SUCCESS;

    // initialize RpcIdHelper
    StoreGateSvc* pDetStore = NULL;
    sc = svcLoc()->service("DetectorStore", pDetStore);
    if (sc.isFailure() || pDetStore == NULL)
    {
        ATH_MSG_ERROR("Cannot locate DetectorStore");
        return sc;
    }
    sc = pDetStore->retrieve(m_pMuonMgr);
    if (sc.isFailure() || m_pMuonMgr == NULL)
    {
        ATH_MSG_ERROR("Cannot retrieve MuonDetectorManager");
        return sc;
    }
    m_pIdHelper = m_pMuonMgr->rpcIdHelper();
    return StatusCode::SUCCESS;
}

StatusCode RPC_LinearSegmentMakerTool::Finalize()
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
    oss << "eta=" << dir.eta() << ",phi=" << dir.phi() << ",perp=" << dir.perp() << ",z=" << dir.z();
    return oss.str();
}

typedef std::pair<std::string, int> LayerID;

static inline double getFirstPointR(const Muon::Fit2D::PointArray& points)
{
    for (auto pPoint : points)
    {
        if (!pPoint->bExclude)
        {
            auto pRIO = static_cast<const Muon::MuonClusterOnTrack*>(pPoint->pData);
            return pRIO->globalPosition().perp();
        }
    }
    return 0.0;
}

void
RPC_LinearSegmentMakerTool::find(const Trk::TrackRoad& road,
                                 const std::vector< std::vector< const Muon::MdtDriftCircleOnTrack* > >&,
                                 const std::vector< std::vector< const Muon::MuonClusterOnTrack* > >& clusters,
				 Trk::SegmentCollection*,
                                 bool,double) const
{
    ATH_MSG_DEBUG("RPC_LinearSegmentMakerTool::find");
    
    std::vector<const Muon::MuonSegment*>* pMuonSegs = NULL;
    double dChi2 = 0.0;
    int nDegf = 0;
    Muon::Fit2D::PointArray zPoints, phiPoints;
    std::vector<const Muon::MuonClusterOnTrack*> rios;
    for (std::vector<std::vector<const Muon::MuonClusterOnTrack*> >::const_iterator itClusters = clusters.begin();
        itClusters != clusters.end();
        itClusters++)
    {
        const std::vector<const Muon::MuonClusterOnTrack*>& cluster = *itClusters;
        rios.insert(rios.end(), cluster.begin(), cluster.end());
    }

    const MuonGM::RpcReadoutElement* pReadoutElement =
        dynamic_cast<const MuonGM::RpcReadoutElement*>(rios.front()->detectorElement());

    if (!pReadoutElement) {
       ATH_MSG_WARNING("Cannot get a RpcReadoutElement via dynamic cast.");
	 //return pMuonSegs;
    }

    std::set<LayerID> zStations, phiStations;
    int iHit = 0;
    for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator itHit = rios.begin();
        itHit != rios.end();
        itHit++, iHit++)
    {
        const Muon::MuonClusterOnTrack* pHit = *itHit;
        const Amg::MatrixX & errMat = pHit->localCovariance();
        Amg::MatrixX wheight= errMat.inverse();
        double w = wheight(Trk::locX,Trk::locX) /2.0;
        if (m_pIdHelper->measuresPhi(pHit->identify()))
        {
            double rho = pHit->globalPosition().perp();
            w *= rho * rho;
            Muon::Fit2D::Point* pPt = new Muon::Fit2D::Point(iHit,
                                                             pHit->globalPosition().perp(),
                                                             pHit->globalPosition().phi(),
                                                             w,
                                                             (void*)pHit);
            phiPoints.push_back(pPt);
	    std::string iStationName = pReadoutElement->getStationName();
	    int iDoubletR = pReadoutElement->getDoubletR();
            // const MuonGM::RpcReadoutElement* pReadoutElement =
            //    dynamic_cast<const MuonGM::RpcReadoutElement*>(pHit->detectorElement());
            phiStations.insert(LayerID(iStationName, iDoubletR));
        }
        else
        {
            Muon::Fit2D::Point* pPt = new Muon::Fit2D::Point(iHit,
                                                             pHit->globalPosition().perp(),
                                                             pHit->globalPosition().z(),
                                                             w,
                                                             (void*)pHit);
            zPoints.push_back(pPt);
	    std::string iStationName = pReadoutElement->getStationName();
	    int iDoubletR = pReadoutElement->getDoubletR();
            // const MuonGM::RpcReadoutElement* pReadoutElement =
            //    dynamic_cast<const MuonGM::RpcReadoutElement*>(pHit->detectorElement());
            zStations.insert(LayerID(iStationName, iDoubletR));
        }
    }
    double zCov[2][2], phiCov[2][2];
    memset(zCov, 0, sizeof(zCov));
    memset(phiCov, 0, sizeof(phiCov));

    if (!zPoints.empty() && !phiPoints.empty())
    {
        Muon::Fit2D fit;
        Amg::Vector3D pos;
        Amg::Vector3D dir = road.globalDirection();
        Muon::Fit2D::SimpleStats zSimpleStats, phiSimpleStats;
        Muon::Fit2D::LinStats zLinStats, phiLinStats;
        if (zStations.size() == 1 || zPoints.size() < 4)
        {
//            if (stations.size() == 1 && zPoints.size() >= 4)
//                std::cout << "RPC_LinearSegmentMakerTool::find 1 station with "
//                << zPoints.size() <<" eta points" << std::endl;
            fit.fitPoint(zPoints,   m_fExclChi2, msgLvl(MSG::DEBUG), zSimpleStats);
            fit.fitPoint(phiPoints, m_fExclChi2, msgLvl(MSG::DEBUG), phiSimpleStats);
            ATH_MSG_DEBUG("  Z: " << zSimpleStats.toString() );
            double baseR = getFirstPointR(zPoints);
            ATH_MSG_DEBUG("    baseR=" << baseR);
            ATH_MSG_DEBUG("Phi: " << phiSimpleStats.toString() );
            pos[0]=baseR;
            pos[1]=0.;
            pos[2]=zSimpleStats.fMean;
            Amg::setPhi(pos, phiSimpleStats.fMean);
            dChi2 = zSimpleStats.fChi2 + phiSimpleStats.fChi2;
            nDegf = zSimpleStats.n + phiSimpleStats.n - 2;
            zCov[0][0]=(zSimpleStats.fStd)*(zSimpleStats.fStd);
        }
        else
        {
            double z, zErr, phi, phiErr;
            fit.fitLine(zPoints, m_fExclChi2, msgLvl(MSG::DEBUG), zLinStats);
            ATH_MSG_DEBUG("Z: " << zLinStats.toString() );
            double baseR = getFirstPointR(zPoints);
            ATH_MSG_DEBUG("    baseR=" << baseR);
            zLinStats.eval(baseR, z, zErr);
            dChi2 = zLinStats.fChi2;
            nDegf = zLinStats.n - 2;
            memcpy(zCov, zLinStats.fCov, sizeof(zCov));

            if (phiStations.size() == 1 || phiPoints.size() < 4)
            {
                fit.fitPoint(phiPoints, m_fExclChi2, msgLvl(MSG::DEBUG), phiSimpleStats);
                ATH_MSG_DEBUG("Phi: " << phiSimpleStats.toString() );
                phi = phiSimpleStats.fMean;
                phiLinStats.fSlope = 0.0;
                dChi2 += phiSimpleStats.fChi2;
                nDegf += phiSimpleStats.n - 1;
                phiCov[0][0]=(phiSimpleStats.fStd)*(phiSimpleStats.fStd);

            }
            else
            {
                fit.fitLine(phiPoints, m_fExclChi2, msgLvl(MSG::DEBUG), phiLinStats);
                ATH_MSG_DEBUG("Phi: " << phiLinStats.toString() );
                phiLinStats.eval(baseR, phi, phiErr);
                dChi2 += phiLinStats.fChi2;
                nDegf += phiLinStats.n - 2;
                memcpy(phiCov, phiLinStats.fCov, sizeof(phiCov));

            }
            pos[0]=baseR;
            pos[1]=0.;
            pos[2]=z;
            Amg::setPhi(pos, phi);
            double rho1 = baseR + 1.0;
            zLinStats.eval(rho1, z, zErr);
            Amg::Vector3D p1(rho1, 0.0, z);
            phiLinStats.eval(rho1, phi, phiErr);
            Amg::setPhi(p1, phi);
            dir = (p1 - pos).unit();
        }
        ATH_MSG_DEBUG("After fit: pos=" << point2String(pos) << ", dir=" << dir2String(dir));
        const Trk::Surface& surface = pReadoutElement->surface();
        Trk::Intersection intersection = surface.straightLineIntersection(pos,dir);
        // Intersect segment with surface
        double u =
            (pReadoutElement->normal().dot(pReadoutElement->center() - pos)) /
            (pReadoutElement->normal().dot(dir));
        pos += dir * u;

        ATH_MSG_DEBUG("Segment position " << point2String(pos) << " from intersection " << point2String(intersection.position));
        ATH_MSG_DEBUG("Segment direction " << dir2String(dir) );

        if (msgLvl(MSG::DEBUG))
        {
          Amg::Vector3D loc3Dframe = surface.transform().inverse()*pos;

//             surface.dump(log);
//             const Amg::Vector3D& pos = surface.center();
          ATH_MSG_DEBUG("Surface at  pos: " << surface.center().perp() << " " << surface.center().z() 
                        << " RE " << pReadoutElement->center().perp() << " " << pReadoutElement->center().z()
                        << " loc (" << loc3Dframe.x() << "," << loc3Dframe.y() << "," << loc3Dframe.z() << ")" );
        }
        const Trk::PlaneSurface* pSurface = dynamic_cast<const Trk::PlaneSurface*>(&surface);
        if (pSurface == NULL)
        {
            ATH_MSG_WARNING("Cannot get a PlaneSurface from RpcClusterOnTrack");
            goto done;
        }
        Amg::Vector2D pSegPos;
        if (!pSurface->globalToLocal(pos, Amg::Vector3D(0., 0., 0.), pSegPos))
        {
            ATH_MSG_WARNING("Cannot get LocalPosition from surface for " << point2String(pos) );
            goto done;
        }
        ATH_MSG_DEBUG("Segment local position is" << " x=" << pSegPos[Trk::locX] << ",y=" << pSegPos[Trk::locY] );
        Trk::LocalDirection pSegDir;
        pSurface->globalToLocalDirection(dir, pSegDir);
        ATH_MSG_DEBUG("Segment local direction is" << " angleXZ=" << pSegDir.angleXZ() << ",angleYZ=" << pSegDir.angleYZ() );

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
        for (Muon::Fit2D::PointArray::const_iterator itPt = zPoints.begin(); itPt != zPoints.end(); itPt++)
        {
            Muon::Fit2D::Point* pPt = *itPt;
            if (!pPt->bExclude)
            {
              
              pRios->push_back(static_cast<const Trk::MeasurementBase*>((const Muon::MuonClusterOnTrack*)(pPt->pData))->clone());
              Trk::Intersection intersection = pRios->back()->associatedSurface().straightLineIntersection(pos,dir);
              Amg::Vector3D loc3Dframe = pRios->back()->associatedSurface().transform().inverse()*intersection.position;
              
              ATH_MSG_DEBUG("Hit on segment " << loc3Dframe.x() << " " << loc3Dframe.y() << " " << loc3Dframe.z() );
            }
        }
        for (Muon::Fit2D::PointArray::const_iterator itPt = phiPoints.begin(); itPt != phiPoints.end(); itPt++)
        {
            Muon::Fit2D::Point* pPt = *itPt;
            if (!pPt->bExclude)
            {
                pRios->push_back(static_cast<const Trk::MeasurementBase*>((const Muon::MuonClusterOnTrack*)(pPt->pData))->clone());
            }
        }
        Muon::MuonSegment* pMuonSeg = new Muon::MuonSegment(pSegPos,
                                                            pSegDir,
                                                            pcov/*new Trk::ErrorMatrix()*/,
                                                            const_cast<Trk::PlaneSurface*>(pSurface->clone()),
                                                            pRios,
                                                            pFitQuality);
        if (msgLvl(MSG::DEBUG))
//            pMuonSeg->dump(log);
            ATH_MSG_DEBUG("Created a new Muon::MuonSegment");
        pMuonSegs = new std::vector<const Muon::MuonSegment*>(1);
        (*pMuonSegs)[0] = pMuonSeg;
//        delete pSegDir;
    }
    ATH_MSG_DEBUG("RPC_LinearSegmentMakerTool::find ended");
done:
    for (unsigned iZ = 0; iZ < zPoints.size(); iZ++)
        delete zPoints[iZ];
    for (unsigned iPhi = 0; iPhi < phiPoints.size(); iPhi++)
        delete phiPoints[iPhi];
      //return pMuonSegs;
}
