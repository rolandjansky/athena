/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlCandidate/Candidate.h"
#include "MuGirlCandidate/CandidateTool.h"

#include "TrkParameters/TrackParameters.h"

//#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/IIntersector.h"

#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "xAODTracking/TrackParticle.h"
#include "MuidInterfaces/IMuidCaloEnergy.h"
#include "muonEvent/CaloEnergy.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "MuonSegment/MuonSegment.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "RoiDescriptor/RoiDescriptor.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "TrkRoad/TrackRoad.h"
#include "TrkDetElementBase/TrkDetElementBase.h"

#include "MuGirlCandidate/Utilities.h"
#include "MuGirlCandidate/Distance.h"
#include "MuGirlCandidate/Technology.h"
#include "MuGirlCandidate/Intersection.h"
#include "MuGirlCandidate/Chamber.h"
#include "MuGirlCandidate/Station.h"
#include "MuGirlCandidate/StationManager.h"
#include "MuGirlCandidate/Segment.h"
#include "MuGirlCandidate/Hit.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "MuGirlInterfaces/SegmentManager.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"

#define dbg std::cout <<"sofia::"<<__FILE__<<" "<<__LINE__<< std::endl;
#define printvar(x) std::cout<<"sofia " <<#x<<x<<std::endl;

const double COS1DEG = cos(M_PI / 180.0);

using MuGirlNS::Candidate;
using MuGirlNS::Intersection;
using MuGirlNS::Segment;
using MuGirlNS::Chamber;

std::set
    <const Trk::TrackSurfaceIntersection*> Intersection::m_TrkIsects;

Candidate::Candidate(CandidateTool* pMuGirl) :
        m_pMuGirl(pMuGirl),
        m_pIDPerigee(NULL),
        m_pIDTrkIsect(NULL),
        m_pIDTrk(NULL),
        m_pRefittedTrack(NULL),
        m_pTrkRefittedTrack(NULL),
        m_bSaveMdtSegmentMakerInfo(false),
        m_passAnn(false),
        m_passStau(false),
        m_pSegmentManager(NULL)
{
  clear();
}

Candidate::~Candidate()
{
    if (m_pIDPerigee!=0) {
        delete m_pIDPerigee;
    }
}

void Candidate::Cell::clear()
{
    pIsect = NULL;
    chambers.clear();
    clusters.clear();
    clustersOnSegment.clear();
}
void Candidate::clear()
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::clear" << endmsg;
    if (m_pIDPerigee!=0) {
        // delete m_pIDPerigee;   it is not a clone of Perigee anymore
        m_pIDPerigee = 0;
    }
    m_muonEFTrack = NULL;
    m_muonFeature = NULL;
    m_startFromMuonEntry  = false;
    m_startFromMF         = false;
    m_startFromEF         = false;
    m_pIDTrk              = NULL;
    m_pRefittedTrack      = NULL;
    m_pTrkRefittedTrack   = NULL;
    //delete m_pRefittedTrack;
    delete m_pIDTrkIsect;
    if (m_pIDTrkIsect != NULL)
    {
        m_pIDTrkIsect = NULL;
    }
    m_passAnn = 0;
    m_passStau = 0;
    m_annBarrel = 0.0;
    m_annEndCap = 0.0;
    m_innAngle  = 999.0;
    m_innDelta  = 999.0;
    m_midAngle  = 999.0;
    m_midDelta  = 999.0;
    m_lhr       = -1.0;
    m_isolation = -1;
    m_bSaveMdtSegmentMakerInfo = false;
    for (unsigned iTech = 0; iTech < ALL_TECHNOLOGIES; iTech++)
    {
        for (unsigned iDist = 0; iDist < DISTANCES; iDist++)
        {
            for (unsigned iReg = 0; iReg < REGIONS; iReg++)
            {
                m_cells[iTech][iDist][iReg].clear();
            }
        }
    }
    for (HitList::iterator itHit = m_hits.begin();
            itHit != m_hits.end(); itHit++)
    {
        delete *itHit;
    }
    m_hits.clear();
    for (IntersectionList::iterator itIsect = m_iSections.begin();
            itIsect != m_iSections.end(); itIsect++)
    {
        delete *itIsect;
    }
    m_iSections.clear();
    for (SegmentList::iterator itSeg = m_segments.begin();
            itSeg != m_segments.end(); itSeg++)
    {
        delete *itSeg;
    }
    m_segments.clear();
    m_muonSegments.clear();
    m_muonT0Segments.clear();
    for (ChamberList::iterator itCh = m_chambers.begin();
            itCh != m_chambers.end(); itCh++)
    {
        delete *itCh;
    }
    m_chambers.clear();
    for (MdtSegmentMakerInfoList::iterator itSmi = m_mdtSegmentMakerInfo.begin();
            itSmi != m_mdtSegmentMakerInfo.end(); itSmi++)
    {
        delete *itSmi;
    }
    m_mdtSegmentMakerInfo.clear();
    for (RIO_OnTrackLists::iterator itROTL = m_RpcRIO_OnTrackLists.begin();
            itROTL != m_RpcRIO_OnTrackLists.end(); itROTL++)
    {
        for (RIO_OnTrackList::iterator itROT = (*itROTL)->begin(); 
             itROT != (*itROTL)->end(); itROT++  )
        {
             delete *itROT;
        } 
        
        delete *itROTL;
    }
    m_RpcRIO_OnTrackLists.clear();
    for (RIO_OnTrackLists::iterator itROTL = m_TgcRIO_OnTrackLists.begin();
            itROTL != m_TgcRIO_OnTrackLists.end(); itROTL++)
    {
        delete *itROTL;
    }
    m_TgcRIO_OnTrackLists.clear();
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::clear ended" << endmsg;
}

void Candidate::setIDPerigee(const Trk::TrackParameters* pPerigee)
{
    /** Type of processing */
    m_startFromMF        = false;
    m_startFromEF        = false;
    m_startFromBetaRefit = false;
    /** Clear output Tracks and set the correct input seed (clearing the unneeded ones) */
    m_pLowBetaTrack      = NULL;
    m_pTrkLowBetaTrack   = NULL;
    m_pIDPerigee         = pPerigee;
    m_muonEFTrack        = NULL;
    m_muonFeature        = NULL;
    m_IDQOverP = pPerigee->parameters()[Trk::qOverP];
    m_pIDTrkIsect = new Trk::TrackSurfaceIntersection(m_pIDPerigee->position(),
                    m_pIDPerigee->momentum().unit(), 0.);

    if(m_pMuGirl->msgLvl(MSG::DEBUG)) {
        m_pMuGirl->msg() << "Intersection set by setIDPerigee:" << endmsg;
        m_pMuGirl->msg() <<  *(m_pIDTrkIsect) << endmsg;
        m_pMuGirl->msg() << "setIDPerigee: m_IDQOverP = " << m_IDQOverP << endmsg;
    }

    // check if the perigee in in the muon entry level
    if ( pPerigee->position().perp()>=4000 || std::abs(pPerigee->position().z())>=4000. ) {
        m_startFromMuonEntry = true;
    } else {
        m_startFromMuonEntry = false;
    }
}

// MuonBetaRefitTool

void Candidate::setMuonBetaRefitTrack(const Trk::Track* lowBetaTrack)
{
    /** Type of processing */
    m_startFromMF        = false;
    m_startFromEF        = false;
    m_startFromBetaRefit = true;
    m_startFromMuonEntry = false;
    /** Clear the output Tracks and set the correct input seed (clearing the unneeded ones) */
    if (m_pIDPerigee!=0) {
        //delete m_pIDPerigee;   it is not a clone of Perigee anymore
        m_pIDPerigee = 0;
    }
    m_muonEFTrack = NULL;
    m_muonFeature = NULL;
    m_pIDTrk      = NULL;

    /** Unset the TrackParticle and set the Trk::Track, new schema */
    m_pLowBetaTrack    = NULL;
    m_pTrkLowBetaTrack = lowBetaTrack;
}

void Candidate::setMuonBetaRefitTrack(const xAOD::TrackParticle* lowBetaTrack)
{
    /** Type of processing */
    m_startFromMF        = false;
    m_startFromEF        = false;
    m_startFromBetaRefit = true;
    m_startFromMuonEntry = false;
    /** Clear the output Tracks and set the correct input seed (clearing the unneeded ones) */
    if (m_pIDPerigee!=0) {
        //delete m_pIDPerigee;
        m_pIDPerigee = 0;
    }
    m_muonEFTrack = NULL;
    m_muonFeature = NULL;
    m_pIDTrk      = NULL;
    /** Set the Trackparticle and unset the Trk::Track,  old schema  */
    m_pLowBetaTrack    = lowBetaTrack;
    m_pTrkLowBetaTrack = NULL;

/*
    float mf_cot  = 1/tan(2*atan(exp (-muonFeature->eta())));
    //float mf_cot  = muonFeature->dir_zeta();
    float mf_px   = 1000*fabs(muonFeature->pt())*cos(muonFeature->phi());
    float mf_py   = 1000*fabs(muonFeature->pt())*sin(muonFeature->phi());
    float mf_pz   = 1000*fabs(muonFeature->pt())*mf_cot;
    float mf_p    = sqrt(mf_px*mf_px+ mf_py*mf_py + mf_pz*mf_pz);
    double dir_x = mf_px/mf_p;
    double dir_y = mf_py/mf_p;
    double dir_z = mf_pz/mf_p;

    double r= muonFeature->radius();
    double pos_x = 0;
    double pos_y = 0;
    double pos_z =0;
    if (r>0)
    {
        m_MFstartStation = 0;
        pos_x = r * cos(muonFeature->phi());
        pos_y = r * sin(muonFeature->phi());
        pos_z = muonFeature->zeta();
    }
    if (r== 0)
    {
       m_MFstartStation = 2;
       pos_x = fabs(muonFeature->zeta())/tan(muonFeature->phi());
       pos_y = fabs(muonFeature->zeta())/tan(2*atan(exp(-muonFeature->eta())));
       pos_z = muonFeature->zeta();
    }
    if (fabs(muonFeature->eta())<1)
    {
       pos_x = pos_x*10.;  //muFeature have distances in cm in the Barrel
       pos_y = pos_y*10.;
       pos_z = pos_z*10.;
    }
    Trk::GlobalPosition* MFpos = new Trk::GlobalPosition(pos_x,pos_y,pos_z);
    Trk::GlobalDirection* MFdir = new Trk::GlobalDirection(dir_x,dir_y,dir_z);
//     Trk::GlobalMomentum* MFmom = new Trk::GlobalMomentum(mf_p*dir_x,mf_p*dir_y,mf_p*dir_z);

    m_IDQOverP = 1./mf_p;
    if (muonFeature->pt()<0.)
    m_IDQOverP = -1./mf_p;
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg(MSG::DEBUG) << "Candidate::setMuonFeature mf_p "<<mf_p<<" qoverp "<<m_IDQOverP <<endmsg;

    if (m_pIDTrkIsect != NULL) delete m_pIDTrkIsect;
    m_pIDTrkIsect = new Trk::TrackSurfaceIntersection(*MFpos, *MFdir,0);
*/

}


// sofia ////////////

void Candidate::setMuonFeature(const MuonFeature* muonFeature)
{
    /** Type of processing */
    m_startFromMF        = true;
    m_startFromEF        = false;
    m_startFromBetaRefit = false;
    m_startFromMuonEntry = false;
    /** Clear the output Tracks and set the correct input seed (clearing the unneeded ones) */
    m_pLowBetaTrack = NULL;
    if (m_pIDPerigee!=0) {
        //delete m_pIDPerigee;
        m_pIDPerigee = 0;
    }
    m_pIDTrk        = NULL;
    m_pLowBetaTrack = NULL;
    m_muonEFTrack   = NULL;
    m_muonFeature   = muonFeature;
    
    float mf_cot  = 1/tan(2*atan(exp (-muonFeature->eta())));
    //float mf_cot  = muonFeature->dir_zeta();
    float mf_px   = 1000*fabs(muonFeature->pt())*cos(muonFeature->phi());
    float mf_py   = 1000*fabs(muonFeature->pt())*sin(muonFeature->phi());
    float mf_pz   = 1000*fabs(muonFeature->pt())*mf_cot;
    float mf_p    = sqrt(mf_px*mf_px+ mf_py*mf_py + mf_pz*mf_pz);
    double dir_x = mf_px/mf_p;
    double dir_y = mf_py/mf_p;
    double dir_z = mf_pz/mf_p;

    double r= muonFeature->radius();
    double pos_x = 0;
    double pos_y = 0;
    double pos_z =0;
    if (r>0)
    {
        m_MFstartStation = 0;
        pos_x = r * cos(muonFeature->phi());
        pos_y = r * sin(muonFeature->phi());
        pos_z = muonFeature->zeta();
    }
    if (r== 0)
    {
       m_MFstartStation = 2;
       pos_x = fabs(muonFeature->zeta())/tan(muonFeature->phi());
       pos_y = fabs(muonFeature->zeta())/tan(2*atan(exp(-muonFeature->eta())));
       pos_z = muonFeature->zeta();
    }
    if (fabs(muonFeature->eta())<1)
    {
       pos_x = pos_x*10.;  //muFeature have distances in cm in the Barrel
       pos_y = pos_y*10.;
       pos_z = pos_z*10.;
    }
    Amg::Vector3D* MFpos = new Amg::Vector3D(pos_x,pos_y,pos_z);
    Amg::Vector3D* MFdir = new Amg::Vector3D(dir_x,dir_y,dir_z);
//     Trk::GlobalMomentum* MFmom = new Trk::GlobalMomentum(mf_p*dir_x,mf_p*dir_y,mf_p*dir_z);

    m_IDQOverP = 1./mf_p;
    if (muonFeature->pt()<0.)
    m_IDQOverP = -1./mf_p;
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg(MSG::DEBUG) << "Candidate::setMuonFeature mf_p "<<mf_p<<" qoverp "<<m_IDQOverP <<endmsg;

    if (m_pIDTrkIsect != NULL) delete m_pIDTrkIsect;
    m_pIDTrkIsect = new Trk::TrackSurfaceIntersection(*MFpos, *MFdir,0);
}



// ==================================================
void Candidate::setMuonEFTrack(const TrigMuonEFTrack* muonEFTrack)
{
    /** Type of processing */
    m_startFromMF        = false;
    m_startFromEF        = true;
    m_startFromBetaRefit = false;
    m_startFromMuonEntry = false;
    /** Clear the output Tracks and set the correct input seed (clearing the unneeded ones) */
    if (m_pIDPerigee!=0) {
        // delete m_pIDPerigee;
        m_pIDPerigee = 0;
    }
    m_pIDTrk        = NULL;
    m_pLowBetaTrack = NULL;
    m_muonFeature   = NULL;
    m_muonEFTrack   = muonEFTrack;

    float mf_cot  = 1/tan(2*atan(exp (-muonEFTrack->eta())));
    float mf_px   = fabs(muonEFTrack->pt())*cos(muonEFTrack->phi());
    float mf_py   = fabs(muonEFTrack->pt())*sin(muonEFTrack->phi());
    float mf_pz   = fabs(muonEFTrack->pt())*mf_cot;
    float mf_p    = sqrt(mf_px*mf_px+ mf_py*mf_py + mf_pz*mf_pz);
    double dir_x = mf_px/mf_p;
    double dir_y = mf_py/mf_p;
    double dir_z = mf_pz/mf_p;

    Amg::Vector3D* MFpos = new Amg::Vector3D(muonEFTrack->posX(),muonEFTrack->posY(),muonEFTrack->posZ());
    Amg::Vector3D* MFdir = new Amg::Vector3D(dir_x,dir_y,dir_z);
//     Trk::GlobalMomentum* MFmom = new Trk::GlobalMomentum(mf_p*dir_x,mf_p*dir_y,mf_p*dir_z);

    m_IDQOverP = muonEFTrack->charge()/mf_p;
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg(MSG::DEBUG) << "Candidate::setMuonEFTrack: momentum "<<mf_p<<" qoverp "<<m_IDQOverP <<endmsg;

    if (m_pIDTrkIsect != NULL) delete m_pIDTrkIsect;
    m_pIDTrkIsect = new Trk::TrackSurfaceIntersection(*MFpos, *MFdir,0);

}
////////////////////

void Candidate::setIDTrk(const xAOD::TrackParticle* pTrackParticle)
{
    m_pIDTrk = pTrackParticle;
}
void Candidate::setRefittedTrackToSummary(const xAOD::TrackParticle* RefittedTrack, const Trk::Track* TrkRefittedTrack, CandidateSummary* Summary)
{
    Summary->pRefittedTrack = RefittedTrack;
    Summary->pTrkRefitted   = TrkRefittedTrack;
}
void Candidate::setMSTrackToSummary(std::unique_ptr<const xAOD::TrackParticle> RefittedTrack,
                                    std::unique_ptr<const Trk::Track> TrkRefittedTrack,
                                    CandidateSummary* Summary)
{
    Summary->pMSRefittedTrack = std::move(RefittedTrack);
    Summary->pTrkMSRefitted   = std::move(TrkRefittedTrack);

}
void Candidate::setRefittedTrack(const xAOD::TrackParticle* pRefittedTrack)
{
    m_pRefittedTrack = pRefittedTrack;
}

//void Candidate::setMSTrack(const xAOD::TrackParticle* pRefittedTrack)
//{
//  m_pMSRefittedTrack = pRefittedTrack;
//}

//void Candidate::setMSTrack(const Trk::Track* pTrkRefittedTrack)                         
//{
//  m_pMSTrkRefittedTrack = pTrkRefittedTrack;
//}

void Candidate::setRefittedTrack(const Trk::Track* pTrkRefittedTrack)
{
    m_pTrkRefittedTrack = pTrkRefittedTrack;
}

StatusCode Candidate::crudeExtrapolate()
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::crudeExtrapolate" << endmsg;

    const Trk::TrackSurfaceIntersection* pLastIsect = m_pIDTrkIsect;
    const Trk::TrackSurfaceIntersection* pBarrelTrkIsect = NULL;
    const Trk::TrackSurfaceIntersection* pEndcapTrkIsect = NULL;

    double radius = 0;
    double halfLength= 0;
    m_QOverP = m_IDQOverP;

    for (unsigned iDist = 0; iDist < DISTANCES; iDist++)
    {
        DistanceType eDist = (DistanceType)iDist;
        Distance* pDist = m_pMuGirl->distance(eDist);
        pBarrelTrkIsect = pEndcapTrkIsect = NULL;
        // create the barrel surfaces (cylinder) and 2 endcap discs)

        radius = pDist->crudeR;
        halfLength= pDist->crudeZ;

        // create the mean surface for the barrel detectors
        Trk::CylinderSurface barrelCylinder(radius, halfLength);

        // create the mean surface for the endcap detectors
        //CLHEP::Hep3Vector forwardDiscPosition(0.,0.,halfLength);
        //transform = new HepGeom::Transform3D(discRotation,forwardDiscPosition);
        Trk::DiscSurface forwardDisc( new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,halfLength))),0, radius ); 

        Trk::DiscSurface backwardDisc( new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-halfLength))),0, radius );
        //CLHEP::Hep3Vector backwardDiscPosition(0.,0.,-halfLength);
        //transform= new HepGeom::Transform3D(discRotation,backwardDiscPosition);
        //Trk::DiscSurface backwardDisc(transform, 0., radius);

        if (fabs(pLastIsect->direction().eta()) < 1.5)
        {
            bool bValid = false;
            if (eDist == EE_DIST)
            {
                if (fabs(pLastIsect->direction().eta()) > 0.5)
                {
                    double z = copysign(pDist->crudeZ, pLastIsect->direction().z());

                    //pBarrelTrkIsect = rungeKutta.intersectDiscSurface(z, pLastIsect, m_QOverP);
                    if (z>0)
                        pBarrelTrkIsect = m_pMuGirl->Intersector()->intersectDiscSurface(forwardDisc, pLastIsect, m_QOverP);
                    else
                        pBarrelTrkIsect = m_pMuGirl->Intersector()->intersectDiscSurface(backwardDisc, pLastIsect, m_QOverP);

                    if (pBarrelTrkIsect != NULL)
                    {
                        if ((pBarrelTrkIsect->position().perp() <= pDist->maxR) &&
                                (pBarrelTrkIsect->position().perp() >= pDist->minR))
                            bValid = true;
                        else
                        {
                            delete pBarrelTrkIsect;
                            pBarrelTrkIsect = 0;
                        }
                    }
                }
            }
            else
            {
                //pBarrelTrkIsect = rungeKutta.intersectCylinderSurface(pDist->crudeR, pLastIsect, m_QOverP);
                pBarrelTrkIsect = m_pMuGirl->Intersector()->intersectCylinderSurface(barrelCylinder, pLastIsect, m_QOverP);
                if (pBarrelTrkIsect != NULL)
                {
                    if ((fabs(pBarrelTrkIsect->position().z()) <= pDist->maxZ) &&
                            (fabs(pBarrelTrkIsect->position().z()) >= pDist->minZ))
                        bValid = true;
                    else
                    {
                        delete pBarrelTrkIsect;
                        pBarrelTrkIsect = 0;
                    }
                }
            }
            if (bValid)
            {
                Intersection* pIsect =
                    addIntersection(CRUDE_INTERSECTION,
                                    pBarrelTrkIsect,
                                    ID_TECH,
                                    eDist,
                                    ID_TECH,
                                    eDist,
                                    BARREL_REGION,
                                    NULL);
                setCellIntersection(ID_TECH, eDist, BARREL_REGION, pIsect);
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Crude intersection " << DistanceTypeName(eDist)
                    << " Barrel " << pBarrelTrkIsect << endmsg;
            }
        }

        if (fabs(pLastIsect->direction().eta()) > 0.5)
        {
            bool bValid = false;
            if (eDist == EE_DIST)
            {
                if (fabs(pLastIsect->direction().eta()) < 2.2)
                {
                    //pEndcapTrkIsect = rungeKutta.intersectCylinderSurface(pDist->crudeR, pLastIsect, m_QOverP);
                    pEndcapTrkIsect= m_pMuGirl->Intersector()->intersectCylinderSurface(barrelCylinder, pLastIsect, m_QOverP);
                    if (pEndcapTrkIsect != NULL)
                    {
                        if ((fabs(pEndcapTrkIsect->position().z()) <= pDist->maxZ) &&
                                (fabs(pEndcapTrkIsect->position().z()) >= pDist->minZ))
                            bValid = true;
                        else
                        {
                            delete pEndcapTrkIsect;
                            pEndcapTrkIsect = 0;
                        }
                    }
                }
            }
            else
            {
                double z = copysign(pDist->crudeZ, pLastIsect->direction().z());
                //pEndcapTrkIsect = rungeKutta.intersectDiscSurface(z, pLastIsect, m_QOverP);
                if (z>0)
                    pEndcapTrkIsect = m_pMuGirl->Intersector()->intersectDiscSurface(forwardDisc, pLastIsect, m_QOverP);
                else
                    pEndcapTrkIsect = m_pMuGirl->Intersector()->intersectDiscSurface(backwardDisc, pLastIsect, m_QOverP);
                if (pEndcapTrkIsect != NULL)
                {
                    if ((pEndcapTrkIsect->position().perp() <= pDist->maxR) &&
                            (pEndcapTrkIsect->position().perp() >= pDist->minR))
                        bValid = true;
                    else
                    {
                        delete pEndcapTrkIsect;
                        pEndcapTrkIsect = 0;
                    }
                }
            }
            if (bValid)
            {
                Intersection* pIsect =
                    addIntersection(CRUDE_INTERSECTION,
                                    pEndcapTrkIsect,
                                    ID_TECH,
                                    eDist,
                                    ID_TECH,
                                    eDist,
                                    ENDCAP_REGION,
                                    NULL);
                setCellIntersection(ID_TECH, eDist, ENDCAP_REGION, pIsect);
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Crude intersection " << DistanceTypeName(eDist)
                    << " Endcap " << pEndcapTrkIsect << endmsg;
            }
        }
        const Trk::TrackSurfaceIntersection* pTrkIsect =
            pBarrelTrkIsect != NULL ? pBarrelTrkIsect : pEndcapTrkIsect;
        if (pTrkIsect != NULL)
            pLastIsect = pTrkIsect;
        if (eDist == INNER_DIST)
        {
            if (pTrkIsect == NULL)
                m_pMuGirl->msg(MSG::WARNING) << "No intersection at INNER distance!" << endmsg;
            else
            {
                if (!m_startFromMF && !m_startFromEF)
                {
                    /**
                      * Correct the muon momentum with the ELoss in the Calorimeter
                      */
                    double P = 0.;
                    if (m_startFromMuonEntry)
                    {
                         const Trk::Perigee pPerigee = this->IDTrk()->perigeeParameters();
                         P = fabs(1.0/ pPerigee.parameters()[Trk::qOverP]);
                    }
                    else P = fabs(1.0 / m_QOverP);

                    CaloEnergy* pCaloEnergy = m_pMuGirl->caloEnergyTool()->energyLoss(P,
                                              pTrkIsect->position().eta(),
                                              pTrkIsect->position().phi());
                    m_MuidELoss = pCaloEnergy->deltaE();

                    if (!m_startFromMuonEntry)
                    {
                        double newP = P - m_MuidELoss;

                        if (newP<500.0)
                        {
                            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                                m_pMuGirl->msg() << "Candidate::crudeExtrapolate ended. NewP lower than 500MeV" << endmsg;
                            delete pCaloEnergy;
                            return StatusCode::RECOVERABLE;
                        }
                        else
                        {
                            m_QOverP = copysign(1.0 / newP, m_QOverP);
                            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                                m_pMuGirl->msg() << "Changing P from " << P << " to " << newP << endmsg;
                        }
                    }
                    delete pCaloEnergy;
                }

            }
        }
    }


        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "Candidate::crudeExtrapolate ended" << endmsg;
        return StatusCode::SUCCESS;
}

void Candidate::selectChambers()
{
    unsigned iDist, iTech;

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::selectChambers" << endmsg;
    Road narrow_roads[DISTANCES], wide_roads[TECHNOLOGIES];

    double pt;
    if (m_startFromEF) pt = fabs(m_muonEFTrack->pt());
    else  if (m_startFromMF) pt = fabs(m_muonFeature->pt()*1000);
    else  pt =m_pIDPerigee->pT();
    double dEta = roadDeltaEta(pt) * m_pMuGirl->roadScaleFactor();
    // Narrow roads
    for (iDist = 0; iDist < DISTANCES; iDist++)
    {
        DistanceType eDist = (DistanceType)iDist;
        Road& road = narrow_roads[iDist];
        updateRoad(road, cellIntersection(ID_TECH, eDist, BARREL_REGION));
        updateRoad(road, cellIntersection(ID_TECH, eDist, ENDCAP_REGION));
        if (!road.isNull())
        {
            road.expandBy(dEta, (m_pMuGirl->roadDeltaPhi()));

            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "narrow_roads[" << DistanceTypeName((DistanceType)iDist)
                << "] " << road.toString() << endmsg;
        }
    }

    // TGC Road
    updateRoad(wide_roads[TGC_TECH], cellIntersection(ID_TECH, INNER_DIST, ENDCAP_REGION));
    updateRoad(wide_roads[TGC_TECH], cellIntersection(ID_TECH, MIDDLE_DIST, ENDCAP_REGION));

    // CSC Road
    if (m_pMuGirl->doCSC())
        updateRoad(wide_roads[CSC_TECH], cellIntersection(ID_TECH, INNER_DIST, ENDCAP_REGION));

    // RPC Road
    updateRoad(wide_roads[RPC_TECH], cellIntersection(ID_TECH, MIDDLE_DIST, BARREL_REGION));
    updateRoad(wide_roads[RPC_TECH], cellIntersection(ID_TECH, OUTER_DIST, BARREL_REGION));

    // MDT Road
    for (iDist = 0; iDist < DISTANCES; iDist++)
    {
        DistanceType eDist = (DistanceType)iDist;
        updateRoad(wide_roads[MDT_TECH], cellIntersection(ID_TECH, eDist, BARREL_REGION));
        updateRoad(wide_roads[MDT_TECH], cellIntersection(ID_TECH, eDist, ENDCAP_REGION));
    }

    // sTGC Road
    updateRoad(wide_roads[STGC_TECH], cellIntersection(ID_TECH, INNER_DIST, ENDCAP_REGION));

    // MM Road
    updateRoad(wide_roads[MM_TECH], cellIntersection(ID_TECH, INNER_DIST, ENDCAP_REGION));


    int nChambers = 0;
    for (iTech = 0; iTech < TECHNOLOGIES; iTech++)
    {
        TechnologyType eTech = (TechnologyType)iTech;
        if (!m_pMuGirl->doCSC() && eTech == CSC_TECH) continue;

        //exclude sTGC and MM technologies for the time being
        //if ( eTech == STGC_TECH || eTech == MM_TECH ) continue;

        Technology* pTech = m_pMuGirl->technology(eTech);
        Road& road = wide_roads[iTech];

        if (!road.isNull())
        {
            road.expandBy(dEta, 0.2);

            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg(MSG::DEBUG) << "wide_roads[" << TechnologyTypeName(eTech) << "] "
                << road.toString() << endmsg;

            std::vector<IdentifierHash> chambers;

            if ( eTech == STGC_TECH ) {
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                  m_pMuGirl->msg(MSG::DEBUG) << "Region selector is not available for the sTGC technology, returning the full list of avalible collections" << endmsg;
                chambers = pTech->retrieveAvailableCollections();
                
            } else if ( eTech == MM_TECH ) {
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                  m_pMuGirl->msg(MSG::DEBUG) << "Region selector is not available for the MM technology, returning the full list of avaliable collections" << endmsg;
                chambers = pTech->retrieveAvailableCollections();
            } else {
                // use region selector for MDT, RPC, TGC, CSC
	        RoiDescriptor roi( road.etaMin(), road.etaMax(), road.phiMin(), road.phiMax() );
	        m_pMuGirl->regionSelector()->DetHashIDList(pTech->detId(), roi, chambers);
            }
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg(MSG::DEBUG) << "Got " << chambers.size() << " HashIDs" << endmsg;

            for (std::vector<IdentifierHash>::iterator itCh = chambers.begin();
                    itCh != chambers.end();
                    itCh++)
            {
                IdentifierHash idHash = *itCh;
                //if (TechnologyTypeName(eTech)=="MDT"&&fabs(road.etaMin())>1.2) idHash=idHash-2;
                Identifier id;
                if (pTech->id(idHash, id) != 0)
                {
                    m_pMuGirl->msg(MSG::WARNING) << "Cannot convert HashId " << idHash << " to id" << endmsg;
                    continue;
                }
                Station* pStation = pTech->station(id);
                if (pStation == NULL)
                {
                    m_pMuGirl->msg(MSG::WARNING) << "Cannot find station for chamber id "
                    << idHash << ":" << TechnologyTypeName(eTech) << endmsg;
                    continue;
                }

                const MuonGM::MuonReadoutElement* pReadoutElement = pTech->readoutElement(id);
                if (pReadoutElement == NULL)
                {
                    if(m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg(MSG::DEBUG) << "Cannot find ReadoutElement for "
                            << TechnologyTypeName(eTech) << ", id=" << id.getString()
                            << ", Hash id=" << idHash << ", station=" << pStation->name() << endmsg;
                }

                Chamber* pChamber = new Chamber(id, idHash, pStation, pReadoutElement);

                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg(MSG::DEBUG) << "Chamber " << pChamber->toString() << " bbox=" << pChamber->bbox().toString() << endmsg;
                DistanceType eDist= pChamber->distanceType();
                RegionType eReg= pChamber->regionType();

                if (pChamber->fillPrepData() == 0)
                {
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg(MSG::DEBUG) << "  -> No hits in chamber. Chamber removed." << endmsg;
                    delete pChamber;
                    continue;
                }

                if (pReadoutElement == NULL)
                {
                    pReadoutElement = pChamber->readoutElement();
                    if (pReadoutElement == NULL)
                    {
                        if(m_pMuGirl->msgLvl(MSG::DEBUG))
                            m_pMuGirl->msg(MSG::WARNING) << "After fillPrepData(), cannot find ReadoutElement for "
                                << TechnologyTypeName(eTech) << ", id=" << id.getString()
                                << ", Hash id=" << idHash << ", station=" << pStation->name() << endmsg;
                        continue;
                    }
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg(MSG::DEBUG) << "Chamber " << pChamber->toString() << " bbox=" << pChamber->bbox().toString() << endmsg;
                }

                Intersection* pCrudeIsect = m_cells[ID_TECH][eDist][eReg].pIsect;
                if (pCrudeIsect == NULL)
                {
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg(MSG::DEBUG) << "  -> No crude intersection!!! Chamber removed." << endmsg;
                    delete pChamber;
                    continue;
                }
                const Trk::TrackSurfaceIntersection* pTrkIsect =
                    extrapolatePlane(pCrudeIsect->trkIntersection(), pReadoutElement);
                if (pTrkIsect == NULL)
                {
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg(MSG::DEBUG) << "  -> Cannot extrapolate intersection to chamber. Chamber removed." << endmsg;
                    delete pChamber;
                    continue;
                }

                addChamber(pChamber);
                Intersection* pIsect = addIntersection(FINE_INTERSECTION,
                                                       pTrkIsect,
                                                       ID_TECH,
                                                       eDist,
                                                       eTech,
                                                       eDist,
                                                       eReg,
                                                       pChamber);
                pChamber->setIntersection(ID_TECH, Chamber::SAME_DISTANCE, pIsect);

                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                {
                    m_pMuGirl->msg(MSG::DEBUG) << "Selected chamber " << pChamber->toString()
                    << " iEta=" << pReadoutElement->getStationEta()
                    << " iPhi=" << pReadoutElement->getStationPhi()
                    << " pos="  << pReadoutElement->globalPosition()
                    << endmsg;
                    m_pMuGirl->msg(MSG::DEBUG) << "    Fine intersection at " << pIsect->position() << endmsg;
                }

                m_cells[iTech][eDist][eReg].chambers.push_back(pChamber);
                nChambers++;
            }
        }
    }
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg(MSG::DEBUG) << "Candidate::selectChambers ended with " << nChambers << " Chambers." << endmsg;
}

void Candidate::collectHits(TechnologyType eTech,
                            DistanceType eDist,
                            RegionType eReg,
                            TechnologyType eOrigTech)
{
    if (!m_pMuGirl->doCSC() && eTech == CSC_TECH) return;
    Technology* pTech = m_pMuGirl->technology(eTech);
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::collectHits"
        << "(" << TechnologyTypeName(eTech)
        << ", " << DistanceTypeName(eDist)
        << ", " << RegionTypeName(eReg)
        << ", " << TechnologyTypeName(eOrigTech) << ")"
        << endmsg;

    double pt;
    if (m_startFromEF)
    {
        pt = fabs(m_muonEFTrack->pt());
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "Start from MuonEF "<< endmsg;
    }
    else if (m_startFromMF)
    {
        pt = fabs(m_muonFeature->pt())*1000;
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "Start from MuonFeature "<< endmsg;
    }
    else  pt =m_pIDPerigee->pT();

    double  dPhi = roadDeltaPhi(pt);
    ChamberList& chambers = m_cells[eTech][eDist][eReg].chambers;
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Visiting " << chambers.size() << " chambers" << endmsg;
    for (ChamberList::iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
    {
        Chamber* pChamber = *itCh;
        const PrepDataList& prepDataArray = pChamber->prepData();

        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "Processing chamber " << pChamber->toString()
            << ". Collection contains " << prepDataArray.size() << " hits" << endmsg;
        Intersection* pIsect = pChamber->intersection(eOrigTech, Chamber::SAME_DISTANCE);
        if (pIsect == NULL)
        {
            eOrigTech = COMB_TECH;
            pIsect = pChamber->intersection(eOrigTech, Chamber::OTHER_DISTANCE);
        }
        if (pIsect == NULL)
        {
            eOrigTech = ID_TECH;
            pIsect = pChamber->intersection(eOrigTech, Chamber::SAME_DISTANCE);
        }
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << TechnologyTypeName(eOrigTech)
            << " intersection at " << pIsect->position() << endmsg;

        //double dEta = roadDeltaEta(eTech, eDist, eReg, pIsect->position().eta());
        double dEta = roadDeltaEta(eTech, eDist, eReg, pIsect->position().eta(),pt) * m_pMuGirl->roadScaleFactor();
        if (m_startFromMF || m_startFromEF) dEta = dEta*5;
//         else dEta += 0.01;

        Road baseRoad;
        updateRoad(baseRoad, pIsect);
        if (!baseRoad.isNull())
        {
            baseRoad.expandBy(dEta, dPhi);
            // The following check determines if the cahmber intersects our road.
            // Since the RPC 'chamber' is actually a collection of real chambers,
            // and the associated surface is of only one of these chaambers, this
            // test does not work.
            if (eTech != RPC_TECH)
            {
                if (!pChamber->inChamber(baseRoad))
                {
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg() << "Candidate::collectHits - BaseRoad (" << baseRoad.toString() << ") not on chamber." << endmsg;
                    continue;
                }
            }
        }

        for (PrepDataList::const_iterator itPrepData = prepDataArray.begin();
                itPrepData != prepDataArray.end();
                itPrepData++)
        {
            const Trk::PrepRawData* pPrepData = *itPrepData;

            if (eTech == MDT_TECH)
            {
                if (pPrepData == NULL)
                {
                    continue;
                }
                const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepData);
                if (pMdtPrepData == NULL)
                {
                    continue;
                }
                if (pMdtPrepData->tdc() == 0)
                    continue;
            }
            const Trk::TrkDetElementBase* pDetElement = pPrepData->detectorElement();


            // For RPC, test each PRD if the associated DetectorElement surface overlaps
            // our road
            if (eTech == RPC_TECH && !baseRoad.isNull())
            {
                BBox bbox;
                PrepData2Bbox(pPrepData, bbox);
                if (!bbox.on(baseRoad))
                {
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    {
                        m_pMuGirl->msg() << "Exclude PrepData at " << pTech->hitPosition(pPrepData) << endmsg;
                        m_pMuGirl->msg() << "  Road: " << baseRoad.toString() << ", PRD bbox: " << bbox.toString() << endmsg;
                    }
                    continue;
                }
            }

            const Trk::Surface& surface = pDetElement->surface(pPrepData->identify());
            const Trk::TrackSurfaceIntersection* pHitTrkIsect =
                m_pMuGirl->Intersector()->intersectSurface(surface, pIsect->trkIntersection(), m_QOverP);
            if (pHitTrkIsect == NULL)
            {
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << " Candidate::collectHits - pHitTrkIsect is NULL" << endmsg;
                continue;
            }

            bool bIsEta = pTech->isEtaHit(pPrepData);
            const Trk::RIO_OnTrack* pRIO = NULL;
            if (eTech == MDT_TECH)
            {
                const Muon::MdtPrepData* pMdtPrepData = dynamic_cast<const Muon::MdtPrepData*>(pPrepData);
                if (pMdtPrepData == NULL) continue;
                const Amg::Vector3D* gdir = new Amg::Vector3D(pHitTrkIsect->direction());

                pRIO = m_pMuGirl->mdtDriftCircleCreator()->createRIO_OnTrack(*pMdtPrepData, pHitTrkIsect->position(),gdir);
                delete gdir;
            }
            else if (eTech == CSC_TECH)
                pRIO = m_pMuGirl->cscClusterCreator()->createRIO_OnTrack(*pPrepData, pHitTrkIsect->position(),pHitTrkIsect->direction());
            else
                pRIO = m_pMuGirl->muonClusterCreator()->createRIO_OnTrack(*pPrepData, pHitTrkIsect->position());
            if (pRIO == NULL)
            {
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg(MSG::DEBUG) << "Cannot create Trk::RIO_OnTrack" << endmsg;
                delete pHitTrkIsect;
                continue;
            }

            ///bool bIsEta = pTech->isEtaHit(pPrepData);
            //                HepGeom::Point3D<double> pos = pTech->hitPosition(pPrepData);
            const Amg::Vector3D& pos = pRIO->globalPosition();
            //                double origEta = pos.eta();
            double deltaPhi = canonicPhi(pos.phi() - pHitTrkIsect->position().phi());
            //                if (bIsEta)
            //                    correctEtaforPhi(pos, pHitTrkIsect->position().phi());
            double deltaEta = pos.eta() - pHitTrkIsect->position().eta();
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
            {
                m_pMuGirl->msg() << "PrepData position   " << pTech->hitPosition(pPrepData) << endmsg;
                m_pMuGirl->msg() << "Isect with DetElem  " << pHitTrkIsect->position() << endmsg;
                m_pMuGirl->msg() << "Trk::RIO_OnTrack at " << pos << endmsg;
                if (bIsEta)
                    m_pMuGirl->msg() << "fabs(deltaEta)=" << fabs(deltaEta) << " dEta=" << dEta << endmsg;
                else
                    m_pMuGirl->msg() << "fabs(deltaPhi)=" << fabs(deltaPhi) << " dPhi=" << dPhi << endmsg;
            }
            if ((bIsEta && fabs(deltaEta) < dEta) || (!bIsEta && fabs(deltaPhi) < dPhi))
            {
                Hit* pHit = new Hit(pos, deltaEta, deltaPhi, bIsEta, pChamber);
                if (!pChamber->addHit(pHit))
                {
                    delete pHit;
                    delete pRIO;
                    delete pHitTrkIsect;
                    continue;
                }
                pChamber->addRIO(pRIO, pHit);
                pHit->setRIO(pRIO);
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Hit is selected" << endmsg;
            }
            else
                delete pRIO;
            delete pHitTrkIsect;
        }
        if (!pChamber->hits().empty())
        {
            pChamber->setBaseIntersection(pIsect);
            Trk::TrackRoad* pRoad = new Trk::TrackRoad(pIsect->position(), pIsect->direction(), dEta, dPhi);
            pChamber->setBaseRoad(pRoad);
        }
    }

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::collectHits ended" << endmsg;
}

void Candidate::buildSegments(TechnologyType eTech,
                              DistanceType eDist,
                              RegionType eReg,
                              TechnologyType eOrigTech)
{
    if (!m_pMuGirl->doCSC() && eTech == CSC_TECH) return;
    Technology* pTech = m_pMuGirl->technology(eTech);
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::buildSegments"
        << "(" << TechnologyTypeName(eTech)
        << ", " << DistanceTypeName(eDist)
        << ", " << RegionTypeName(eReg)
        << ", " << TechnologyTypeName(eOrigTech) << ")"
        << endmsg;

    collectHits(eTech, eDist, eReg, eOrigTech);
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg(MSG::DEBUG) << "Candidate::buildSegments after collectHits" << endmsg;

    ChamberList& chambers = m_cells[eTech][eDist][eReg].chambers;
    if (!chambers.empty())
    {
        for (ChamberList::iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
        {
            Chamber* pChamber = *itCh;
            const HitSet& hits = pChamber->hits();
            for (HitSet::iterator itHit = hits.begin(); itHit != hits.end(); itHit++)
            {
                Hit* pHit = *itHit;
                pHit->setIndex(m_hits.size());
                m_hits.push_back(pHit);
            }
            //            m_hits.insert(m_hits.end(), hits.begin(), hits.end());
        }

        if (m_pMuGirl->doSegments())
        {
            pTech->buildSegments(this, chambers, m_QOverP);
        }
    }

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::buildSegments ended" << endmsg;
}

Intersection* Candidate::addIntersection(IntersectionType eType,
        const Trk::TrackSurfaceIntersection* pTrkIsect,
        TechnologyType eOrigTech,
        DistanceType eOrigDist,
        TechnologyType eTargTech,
        DistanceType eTargDist,
        RegionType eReg,
        Chamber* pChamber)
{
    Intersection* pIsect = new Intersection(eType,
                                            pTrkIsect,
                                            eOrigTech,
                                            eOrigDist,
                                            eTargTech,
                                            eTargDist,
                                            eReg,
                                            pChamber);
    pIsect->setIndex(m_iSections.size());
    m_iSections.push_back(pIsect);
    return pIsect;
}

Segment* Candidate::addSegment(SegmentType eType,
                               const Muon::MuonSegment* pMuonSegment,
                               TechnologyType eTech,
                               DistanceType eDist,
                               RegionType eReg,
                               Station* pStation)
{
    Segment* pSegment = new Segment(eType, pMuonSegment, eTech, eDist, eReg, pStation);
    pSegment->setIndex(m_segments.size());
    m_segments.push_back(pSegment);
    if (eTech != MDT_TECH) m_muonSegments.push_back(pMuonSegment);
    SegmentManager::Type segType = SegmentManager::NONE;
    switch (eTech)
    {
    case CSC_TECH: segType = SegmentManager::CSC; break;
    case RPC_TECH: segType = SegmentManager::RPC; break;
    case TGC_TECH: segType = SegmentManager::TGC; break;
    case MDT_TECH: segType = SegmentManager::MDT; break;
    default:       segType = SegmentManager::NONE; break;
    }
    if (segType != SegmentManager::NONE)
        m_pSegmentManager->addSegment(pMuonSegment, segType);
    return pSegment;
}

void Candidate::addMDTSegmenttoMuonSegments(const Muon::MuonSegment* pMuonSegment)
{
    m_muonSegments.push_back(pMuonSegment);
}

void Candidate::addT0Segments(const Muon::MuonSegment* pMuonSegment)
{
    m_muonT0Segments.push_back(pMuonSegment);
    m_pSegmentManager->addSegment(pMuonSegment,SegmentManager::NONE);
}


void Candidate::computeChamberIntersections(TechnologyType eOrigTech,
        DistanceType eOrigDist,
        DistanceType eTargDist,
        RegionType eReg)
{
    Intersection* pOrigIsect = cellIntersection(eOrigTech, eOrigDist, eReg);
    if (pOrigIsect == NULL)
        return;

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::computeChamberIntersections("
        << TechnologyTypeName(eOrigTech) << ", "
        << DistanceTypeName(eOrigDist) << ", "
        << DistanceTypeName(eTargDist) << ", "
        << RegionTypeName(eReg) << ")"
        << endmsg;

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Using " << pOrigIsect->toString() << endmsg;

    Chamber::Origin eOrig = (eOrigDist == eTargDist ? Chamber::SAME_DISTANCE : Chamber::OTHER_DISTANCE);
    for (unsigned iTech = 0; iTech < TECHNOLOGIES; iTech++)
    {
        ChamberList& chambers = m_cells[iTech][eTargDist][eReg].chambers;
        for (ChamberList::iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
        {
            Chamber* pChamber = *itCh;
         //   if (pChamber->RIOs().empty())
         //       continue;
            const Trk::TrackSurfaceIntersection* pTrkIsect =
                extrapolatePlane(pOrigIsect->trkIntersection(),
                                 pChamber->readoutElement());
            if (pTrkIsect == NULL)
                continue;
            Intersection* pIsect = addIntersection(EVALUATED_INTERSECTION,
                                                   pTrkIsect,
                                                   pOrigIsect->originTechnologyType(),
                                                   eOrigDist,
                                                   (TechnologyType)iTech,
                                                   eTargDist,
                                                   eReg,
                                                   pChamber);
            pChamber->setIntersection(eOrigTech, eOrig, pIsect);
            if (m_pMuGirl->msgLvl(MSG::DEBUG))
                m_pMuGirl->msg() << "Chamber " << pChamber->toString()
                << " " << TechnologyTypeName(eOrigTech)
                << " intersection at " << pIsect->position()
                << endmsg;
        }
    }

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::computeChamberIntersections ended" << endmsg;
}

double Candidate::roadDeltaPhi(double pt) const
{
//    if (m_startFromMF) return (200.0/(pt -1500.0)+ 0.02)*2.5;
//    return 200.0 / (pt - 1500.0) + 0.02;
    return roadDeltaEta(pt) * 2.0;
}

double Candidate::roadDeltaEta(double pt) const
{
    if (m_startFromMF || m_startFromEF) return (300.0/(pt  + 4000.0)+ m_pMuGirl->hitDeltaEta())*2.5;
    return 300.0 / (pt + 4000.0) + m_pMuGirl->hitDeltaEta();
}

double Candidate::roadDeltaEta(TechnologyType eTech,
                               DistanceType eDist,
                               RegionType eReg,
                               double /*dEta*/, double pt)
{

   switch (eDist)

    {



    case INNER_DIST:
     if (eReg == BARREL_REGION)
          //return  180 /pt + 0.004;                                                        // ID intersection
          return  215 /pt + 0.004;                 //21-05-2010                                       // ID intersection
     else                                                                                         // ENDCAP
     {
        if (eTech == TGC_TECH || cellIntersection(TGC_TECH, INNER_DIST, ENDCAP_REGION) == NULL)   //ID intersection
          //return  140.0 /(pt - 90) + 0.0045;
          return  200.0 /pt + 0.004;   //21-05-2010
        else
          //return  100.0 /pt + 0.007;                                                      // TGC intersection
          return  150.0 /pt + 0.007;    //21-05-2010                                                     // TGC intersection

     }
     break;

     case EE_DIST:
        if (eReg == BARREL_REGION)
            return 320 / pt + 0.004;
       else
            return 240 / pt + 0.004;
     break;

     case MIDDLE_DIST:
        if (eReg == BARREL_REGION)
        {
            if (eTech == RPC_TECH)
            {
                 if (cellIntersection(MDT_TECH, INNER_DIST, BARREL_REGION) == NULL)              // ID intersection
                    return 350.0 / pt + 0.004;
                 else                                                                            // MDT INNER intersection
                    //return 175.0 / pt + 0.003;
                    return 175.0 / pt + 0.004; //21-05-2010
            }
            else                                                                                 // MDT chamber
            {
                 if (!(cellIntersection(RPC_TECH, MIDDLE_DIST, BARREL_REGION) == NULL))            // RPC MIDDLE intersection
                    //return 130.0 / pt + 0.005;
                    return 150.0 / pt + 0.005;
                 else if (!(cellIntersection(MDT_TECH, INNER_DIST, BARREL_REGION) == NULL))        // MDT INNER intersection
                    //return 175.0 / pt + 0.003;
                    return 175.0 / pt + 0.004;
                 else                                                                            //ID intersection
                    return 350.0 / pt + 0.004;
            }
        }
        else                                                                                     // ENDCAP
        {

            if (eTech == TGC_TECH)
            {

                if (cellIntersection(MDT_TECH, INNER_DIST, ENDCAP_REGION) == NULL)               // ID intersection
                    //return 326.0 / (pt  + 78) + 0.004;
                    //return 400.0 / pt + 0.004;
                    return 420.0 / pt + 0.004; //21-05-2010
                else                                                                             // MDT INNER intersection
                    //return 200.0 / pt + 0.003;
                    return 200.0 / pt + 0.004;  //21-05-2010
            }
            else                                                                                 // MDT chamber
            {
                 if (!(cellIntersection(TGC_TECH, MIDDLE_DIST, ENDCAP_REGION) == NULL))            // TGC MIDDLE intersection
                    //return 100.0 / pt + 0.004;
                    return 150.0 / pt + 0.005;
                 else if (cellIntersection(MDT_TECH, INNER_DIST, ENDCAP_REGION) == NULL)        // MDT INNER intersection
                    //return 160.0 / (pt + 78.0) + 0.004;
                    //return 400.0 / pt + 0.004;
                    return 420.0 / pt + 0.004;
                 else                                                                            // ID intersection
                    //return 200.0 / pt + 0.003;
                    return 200.0 / pt + 0.004;
            }
        }
        break;
    case OUTER_DIST:
        if (eReg == BARREL_REGION)
        {
            if (!(cellIntersection(MDT_TECH, MIDDLE_DIST, BARREL_REGION) == NULL))                // MDT MIDDLE intersection
                //return 175.0 / pt + 0.003;
                return 175.0 / pt + 0.004;
            else if (!(cellIntersection(MDT_TECH, INNER_DIST, BARREL_REGION) == NULL))                 // MDT INNER intersection
                //return 175.0 / pt + 0.003;
                return 175.0 / pt + 0.004;
            else                                                                                // ID intersection
                //return 380.0 / pt + 0.004;
                return 400.0 / pt + 0.004;

        }
        else                                                                                    // ENDCAP
        {

            if (!(cellIntersection(MDT_TECH, MIDDLE_DIST, ENDCAP_REGION) == NULL))                // MDT MIDDLE intersection
                //return 160./(pt - 447) + 0.004;
                //return 100./ pt + 0.003;
                return 150./ pt + 0.004;
            else if (!(cellIntersection(TGC_TECH, MIDDLE_DIST, ENDCAP_REGION) == NULL))           // TGC MIDDLE intersection
                //return 100./ pt + 0.004;
                return 150./ pt + 0.0075;
            else if (!(cellIntersection(MDT_TECH, INNER_DIST, ENDCAP_REGION) == NULL))            // MDT INNER intersection
                //return 160./(pt - 447) + 0.004;
                //return 100./pt + 0.004;
                return 150./pt + 0.004;
            else                                                                                  // ID intersection
                //return 337./(pt - 447) + 0.004;
                //return 400.0/ pt + 0.004;
                return 420.0/ pt + 0.005;
        }
        break;
    default:
        break;

    }



    return roadDeltaEta(pt);
}


void Candidate::updateRoad(Road& road, const Intersection* pIsect)
{
    if (pIsect != NULL)
    {
        if (pIsect->trkIntersection() == NULL)
            m_pMuGirl->msg(MSG::WARNING) << "Candidate::updateRoad - Intersection has a NULL Trk::TrackSurfaceIntersection*" << endmsg;
        else
            road.update(pIsect->position());
    }
}

void Candidate::addChamber(Chamber* pChamber)
{
    pChamber->setIndex(m_chambers.size());
    m_chambers.push_back(pChamber);
}

const Trk::TrackSurfaceIntersection*
Candidate::extrapolatePlane(const Trk::TrackSurfaceIntersection* pBaseTrkIsect,
                            const MuonGM::MuonReadoutElement* pReadoutElement)
{

    const Trk::Surface& surface = pReadoutElement->surface();

    //RungeKutta rungeKutta;
    //return rungeKutta.intersectPlaneSurface(pReadoutElement->center(),pReadoutElement->normal(),pBaseTrkIsect,m_QOverP);
    return m_pMuGirl->Intersector()->intersectSurface(surface, pBaseTrkIsect, m_QOverP);
}

void Candidate::markHits(ChamberList& chambers, Segment* pSegment)
{
    const RIOList& rios = pSegment->muonSegment()->containedROTs();
    for (RIOList::const_iterator itRIO = rios.begin(); itRIO != rios.end(); itRIO++)
    {
        const Trk::RIO_OnTrack* pRIO = *itRIO;
        for (ChamberList::iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
        {
            Chamber* pChamber = *itCh;
            Hit* pHit = pChamber->findHitByRIO(pRIO);
            if (pHit != NULL)
            {
                pHit->setInSegment();
                pSegment->addHit(pHit);
                break;
            }
        }
    }
    TechnologyType eTech = pSegment->technologyType();
    if (eTech == RPC_TECH || eTech == TGC_TECH)
    {
        Clusters& clusters = getClustersOnSegment(eTech, pSegment->distanceType(), pSegment->regionType());
        clusters.resize(1);
        const HitList& hits = pSegment->hits();
        for (HitList::const_iterator itHit = hits.begin(); itHit != hits.end(); ++itHit)
        {
            const Muon::MuonClusterOnTrack* mcot = dynamic_cast<const Muon::MuonClusterOnTrack*>((*itHit)->RIO());
            clusters[0].push_back(mcot);
        }
    }
}

void Candidate::computeCombinedIntersection(DistanceType eDist,
        RegionType eReg)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::computeCombinedIntersection("
        << DistanceTypeName(eDist) << "," << RegionTypeName(eReg) << ")" << endmsg;

    Intersection* pInIsect = NULL;
    TechnologyType inTech = ALL_TECHNOLOGIES;
    DistanceType inDist = eDist;
    if (cellIntersection(MDT_TECH, eDist, eReg) != NULL)
    {
        Intersection* pMdtIsect = cellIntersection(MDT_TECH, eDist, eReg);
        Segment* pMdtSeg = pMdtIsect->segment();
        if (pMdtSeg == NULL)
            m_pMuGirl->msg(MSG::WARNING) << "MDT intersection does not have a segment!" << endmsg;
        else if (pMdtSeg->fitProbability() > 0.01)
        {
            pInIsect = pMdtIsect;
            inTech = MDT_TECH;
        }
    }

    if (pInIsect == NULL &&
            eReg == BARREL_REGION &&
            (eDist == MIDDLE_DIST || eDist == OUTER_DIST) &&
            cellIntersection(MDT_TECH, eDist, eReg) != NULL &&
            cellIntersection(RPC_TECH, eDist, eReg) != NULL)
    {
        Intersection* pRpcIsect = cellIntersection(RPC_TECH, eDist, eReg);
        Intersection* pMdtIsect = cellIntersection(MDT_TECH, eDist, eReg);
        double cosa = (pRpcIsect->direction()).dot(pMdtIsect->direction());
        bool aux_quantities_set = false;

        Segment* pRpcSeg = pRpcIsect->segment();
        if (pRpcSeg == NULL)
            m_pMuGirl->msg(MSG::WARNING) << "RPC intersection does not have a segment!" << endmsg;
        else if (pRpcSeg->fitProbability() > 0.05 && cosa <= COS1DEG)
        {
            pInIsect = pRpcIsect;
            inTech = RPC_TECH;
            aux_quantities_set = true;
        }

        Segment* pMdtSeg = pMdtIsect->segment();
        if (pMdtSeg == NULL)
            m_pMuGirl->msg(MSG::WARNING) << "MDT intersection does not have a segment!" << endmsg;
        else if (cosa > COS1DEG && pMdtSeg->fitProbability() > 0.001 && !aux_quantities_set)
        {
            pInIsect = pMdtIsect;
            inTech = MDT_TECH;
        }
    }

    if (pInIsect == NULL &&
            eReg == ENDCAP_REGION &&
            (eDist == INNER_DIST || eDist == MIDDLE_DIST) &&
            cellIntersection(TGC_TECH, eDist, eReg) != NULL)
    {
        pInIsect = cellIntersection(TGC_TECH, eDist, eReg);
        inTech = TGC_TECH;
    }

    if (pInIsect == NULL)
    {
        pInIsect = cellIntersection(ID_TECH, eDist, eReg);
        inTech = ID_TECH;
        if (eDist != INNER_DIST)
        {
            DistanceType ePrevDist = PreviousDistance(eDist);
            if (cellIntersection(COMB_TECH, ePrevDist, eReg) != NULL)
            {
                pInIsect = cellIntersection(COMB_TECH, ePrevDist, eReg) ;
                inTech = COMB_TECH;
                inDist = ePrevDist;
            }
        }
    }
    if (pInIsect != NULL)
    {
/*        Intersection* pIsect = addIntersection(COMBINED_INTERSECTION,
                                               pInIsect->trkIntersection(),
                                               inTech,
                                               inDist,
                                               COMB_TECH,
                                               eDist,
                                               eReg,
                                               NULL);*/
        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "Combined " << DistanceTypeName(eDist)
            << " " << RegionTypeName(eReg)
//            << " based on " << TechnologyTypeName(inTech)
            << " set to " << TechnologyTypeName(inTech)
            << " " << DistanceTypeName(inDist)
            << " " << pInIsect->trkIntersection() << endmsg;

//        setCellIntersection(COMB_TECH, eDist, eReg, pIsect);
        setCellIntersection(COMB_TECH, eDist, eReg, pInIsect);

        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << cellToString(COMB_TECH, eDist, eReg) << endmsg;

        if (m_pMuGirl->msgLvl(MSG::DEBUG))
            m_pMuGirl->msg() << "Candidate::computeCombinedIntersection done" << endmsg;
    }
}

bool Candidate::hasInnerStationSegments()
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::hasInnerStationHits" << endmsg;

    const Chamber* pChamber;
    Intersection* pCombIsectBarrel = NULL;
    Intersection* pCombIsectEndCap = NULL;

    bool hasMDT = true;
    bool hasCSC = true;

    // MDT
    pChamber = findFirstValidChamber(MDT_TECH, INNER_DIST, BARREL_REGION);
    if (pChamber != NULL)
        pCombIsectBarrel = pChamber->intersection(MDT_TECH, Chamber::SAME_DISTANCE);
    pChamber = findFirstValidChamber(MDT_TECH, INNER_DIST, ENDCAP_REGION);
    if (pChamber != NULL)
        pCombIsectEndCap = pChamber->intersection(MDT_TECH, Chamber::SAME_DISTANCE);
    if (pCombIsectBarrel == NULL && pCombIsectEndCap == NULL) hasMDT = false;
    //CSC
    pChamber = findFirstValidChamber(CSC_TECH, INNER_DIST, ENDCAP_REGION);
    if (pChamber != NULL)
        pCombIsectEndCap = pChamber->intersection(CSC_TECH, Chamber::SAME_DISTANCE);
    if (pCombIsectEndCap == NULL) hasCSC = false;

    if (hasCSC || hasMDT) return true;
    else  return false;

}
void Candidate::computeAngles()
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::computeAngles" << endmsg;
    const Chamber* pChamber;
    Intersection* pIdIsect;
    Intersection* pCombIsect;

    // Inner Barrel
    pChamber = findFirstValidChamber(MDT_TECH, INNER_DIST, BARREL_REGION);
    if (pChamber != NULL)
    {
        pIdIsect = pChamber->intersection(ID_TECH, Chamber::SAME_DISTANCE);
        if (pIdIsect != NULL)
        {
            pCombIsect = pChamber->intersection(MDT_TECH, Chamber::SAME_DISTANCE);
            //if (pCombIsect == NULL)
            //    pCombIsect = pChamber->intersection(COMB_TECH, Chamber::OTHER_DISTANCE);
            if (pCombIsect != NULL)
            {
                double argo = (pIdIsect->direction()).dot( pCombIsect->direction() );
                if (argo>1) argo = 1;
                m_innAngle = acos(argo);
                m_innDelta = fabs(pIdIsect->position().z() - pCombIsect->position().z());
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Inner Barrel angle=" << m_innAngle << " delta=" << m_innDelta << endmsg;
            }
        }
    }
    // Inner Endcap
    if (m_innAngle == 999.0)
    {
        pChamber = findFirstValidChamber(MDT_TECH, INNER_DIST, ENDCAP_REGION);
        if (pChamber != NULL)
        {
            pIdIsect = pChamber->intersection(ID_TECH, Chamber::SAME_DISTANCE);
            if (pIdIsect != NULL)
            {
                pCombIsect = pChamber->intersection(MDT_TECH, Chamber::SAME_DISTANCE);
                //if (pCombIsect == NULL)
                //    pCombIsect = pChamber->intersection(COMB_TECH, Chamber::OTHER_DISTANCE);
                if (pCombIsect != NULL)
                {
                    double argo = (pIdIsect->direction()).dot( pCombIsect->direction() );
                    if (argo>1) argo = 1;
                    m_innAngle = acos(argo);
                    m_innDelta = fabs(pIdIsect->position().perp() - pCombIsect->position().perp());
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg() << "Inner Endcap angle=" << m_innAngle << " delta=" << m_innDelta << endmsg;
                }
            }
        }
        else
        {
            pChamber = findFirstValidChamber(TGC_TECH, INNER_DIST, ENDCAP_REGION);
            if (pChamber != NULL)
            {
                pIdIsect = pChamber->intersection(ID_TECH, Chamber::SAME_DISTANCE);
                if (pIdIsect != NULL)
                {
                    //pCombIsect = pChamber->intersection(COMB_TECH, Chamber::OTHER_DISTANCE);
                    pCombIsect = pChamber->intersection(COMB_TECH, Chamber::SAME_DISTANCE);
                    if (pCombIsect != NULL)
                    {
                        double argo = (pIdIsect->direction()).dot( pCombIsect->direction() );
                        if (argo>1) argo = 1;
                        m_innAngle = acos(argo);
                        m_innDelta = fabs(pIdIsect->position().perp() - pCombIsect->position().perp());
                        if (m_pMuGirl->msgLvl(MSG::DEBUG))
                            m_pMuGirl->msg() << "Inner Endcap angle=" << m_innAngle << " delta=" << m_innDelta << endmsg;
                    }
                }
            }
        }
    }
    // Middle Barrel
    pChamber = findFirstValidChamber(MDT_TECH, MIDDLE_DIST, BARREL_REGION);
    if (pChamber == NULL)
        pChamber = findFirstValidChamber(RPC_TECH, MIDDLE_DIST, BARREL_REGION);
    if (pChamber != NULL)
    {
        pIdIsect = pChamber->intersection(ID_TECH, Chamber::SAME_DISTANCE);
        if (pIdIsect != NULL)
        {
            pCombIsect = pChamber->intersection(COMB_TECH, Chamber::SAME_DISTANCE);
            if (pCombIsect != NULL && pCombIsect->originTechnologyType() != ID_TECH)
            {
                double argo = (pIdIsect->direction()).dot( pCombIsect->direction() );
                if (argo>1) argo = 1;
                m_midAngle = acos(argo);
                m_midDelta = fabs(pIdIsect->position().z() - pCombIsect->position().z());
                if (m_pMuGirl->msgLvl(MSG::DEBUG))
                    m_pMuGirl->msg() << "Middle Barrel angle=" << m_midAngle << " delta=" << m_midDelta << endmsg;
            }
        }
    }
    // Middle Endcap
    if (m_midAngle == 999.0)
    {
        pChamber = findFirstValidChamber(MDT_TECH, MIDDLE_DIST, ENDCAP_REGION);
        if (pChamber == NULL)
            pChamber = findFirstValidChamber(TGC_TECH, MIDDLE_DIST, ENDCAP_REGION);
        if (pChamber != NULL)
        {
            pIdIsect = pChamber->intersection(ID_TECH, Chamber::SAME_DISTANCE);
            if (pIdIsect != NULL)
            {
                pCombIsect = pChamber->intersection(COMB_TECH, Chamber::SAME_DISTANCE);
                if (pCombIsect != NULL && pCombIsect->originTechnologyType() != ID_TECH)
                {
                    double argo = (pIdIsect->direction()).dot( pCombIsect->direction() );
                    if (argo>1) argo = 1;
                    m_midAngle = acos(argo);
                    m_midDelta = fabs(pIdIsect->position().perp() - pCombIsect->position().perp());
                    if (m_pMuGirl->msgLvl(MSG::DEBUG))
                        m_pMuGirl->msg() << "Middle Endcap angle=" << m_midAngle << " delta=" << m_midDelta << endmsg;
                }
            }
        }
    }

    if (std::isnan(m_innAngle))
        m_innAngle = 999.;  //sofia
    if (std::isnan(m_midAngle))
        m_midAngle = 999.;

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::computeAngles ended" << endmsg;
}

StatusCode Candidate::fillSummaryFromBetaRefit(CandidateSummary* pSummary)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::fillSummaryFromBetaRefit" << endmsg;

    memset(pSummary, 0, sizeof(CandidateSummary));
    pSummary->startFromMF        = m_startFromMF;
    pSummary->startFromBetaRefit = m_startFromBetaRefit;
    pSummary->startFromEF        = m_startFromEF;
    pSummary->startFromMuonEntry = m_startFromMuonEntry;
    pSummary->pTrackParticle     = m_pIDTrk;
    pSummary->pMuonEFTrack       = m_muonEFTrack;
    pSummary->pMuonFeature       = m_muonFeature;
    pSummary->pRefittedTrack     = m_pRefittedTrack;
    pSummary->pTrkRefitted       = m_pTrkRefittedTrack;
    pSummary->pMSRefittedTrack.reset();
    pSummary->pTrkMSRefitted.reset();
    pSummary->pLowBetaTrack      = m_pLowBetaTrack;
    pSummary->pTrkLowBeta        = m_pTrkLowBetaTrack;
    pSummary->pLowBetaExtrpTrack = NULL;
    //    pSummary->pTrkLowBetaExtr    = NULL;

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::fillSummaryFromBetaRefit ended" << endmsg;

    return StatusCode::SUCCESS;
}
//StatusCode Candidate::fillSummary(CandidateSummary* pSummary, const Trk::MeasuredPerigee* pPerigee)
StatusCode Candidate::fillSummary(CandidateSummary* pSummary)
{
    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::fillSummary" << endmsg;

    memset(pSummary, 0, sizeof(CandidateSummary));

    //pSummary->eta = m_pIDPerigee->eta();
    //pSummary->pt = m_pIDPerigee->pT();
    pSummary->startFromMF        = m_startFromMF;
    pSummary->startFromBetaRefit = m_startFromBetaRefit;
    pSummary->startFromEF        = m_startFromEF;
    pSummary->startFromMuonEntry = m_startFromMuonEntry;
    pSummary->pTrackParticle     = m_pIDTrk;
    pSummary->pMuonEFTrack       = m_muonEFTrack;
    pSummary->pMuonFeature       = m_muonFeature;
    pSummary->pRefittedTrack     = m_pRefittedTrack;
    pSummary->pTrkRefitted       = m_pTrkRefittedTrack;
    pSummary->pMSRefittedTrack.reset();
    pSummary->pTrkMSRefitted.reset();
    pSummary->pLowBetaTrack      = m_pLowBetaTrack;
    pSummary->pTrkLowBeta        = m_pTrkLowBetaTrack;
    pSummary->pLowBetaExtrpTrack = NULL;
    //    pSummary->pTrkLowBetaExtr    = NULL;

    for (std::vector<const Muon::MuonSegment*>::iterator MuonSegItr2=m_muonSegments.begin();  MuonSegItr2!= m_muonSegments.end(); MuonSegItr2++)
    {
        (pSummary->muonSegmentList).push_back(*MuonSegItr2);
    }
    //calculate Number of hits
    for (HitList::const_iterator itHit = m_hits.begin(); itHit != m_hits.end(); itHit++)
    {
        const Hit* pHit = *itHit;
        Chamber* pChamber = pHit->chamber();

        switch (pChamber->technologyType())
        {
        case MDT_TECH:
            pSummary->numMdtHits++;
            break;
        case RPC_TECH:
            if (pHit->isEta())
            {
                pSummary->numRpcEtaHits++;
                pSummary->numTgcRpcEtaHits++;
            }
            else
            {
                pSummary->numRpcPhiHits++;
                pSummary->numTgcRpcPhiHits++;
            }
            break;
        case TGC_TECH:
            if (pHit->isEta())
            {
                pSummary->numTgcEtaHits++;
                if (pSummary->numTgcRpcEtaHits == 0)
                    pSummary->numTgcRpcEtaHits++;
            }
            else
            {
                pSummary->numTgcPhiHits++;
                if (pSummary->numTgcRpcPhiHits == 0)
                    pSummary->numTgcRpcPhiHits++;
            }
            break;
        default:
            break;
        }
    }
    //loop over Segments
    for (SegmentList::const_iterator itSeg = m_segments.begin(); itSeg != m_segments.end(); itSeg++)
    {
        const Segment* pSegment = *itSeg;
        const Intersection* pIsect = pSegment->intersection();
        if (pIsect != NULL)
            switch (pIsect->originTechnologyType())
            {
            case MDT_TECH:
                pSummary->numMdtSegs++;
                pSummary->numMdtHitsInSeg += pSegment->hits().size();
                break;
            case TGC_TECH:
                if (pIsect->targetDistanceType() != INNER_DIST)
                {
                    pSummary->numTgcSegs++;
                    pSummary->numTgcRpcSegs++;
                    pSummary->numTgcHitsInSeg += pSegment->hits().size();
                    pSummary->numTgcRpcHitsInSeg += pSegment->hits().size();
                }
                break;
            case RPC_TECH:
                pSummary->numRpcSegs++;
                pSummary->numRpcHitsInSeg += pSegment->hits().size();
                if (pSummary->numTgcRpcSegs == 0)
                    pSummary->numTgcRpcSegs++;
                if (pSummary->numTgcRpcHitsInSeg == 0)
                    pSummary->numTgcRpcHitsInSeg += pSegment->hits().size();
                break;
            case CSC_TECH:
                pSummary->numCscSegs++;
            default:
                break;
            }
    }

    // include INNER DIST intersection positions

    const Intersection* pIsect;
    pIsect = cellIntersection(ID_TECH, INNER_DIST, BARREL_REGION);
    if (pIsect != NULL)
       pSummary->innerIsectPositions.push_back(pIsect->trkIntersection()->position());
    pIsect = cellIntersection(ID_TECH, INNER_DIST, ENDCAP_REGION);
    if (pIsect != NULL)
       pSummary->innerIsectPositions.push_back(pIsect->trkIntersection()->position());




    pSummary->innAngle = m_innAngle;
    pSummary->innDelta = m_innDelta;
    pSummary->midAngle = m_midAngle;
    pSummary->midDelta = m_midDelta;
    pSummary->qOverP = m_IDQOverP;
    pSummary->pIDTrkIsect = m_pIDTrkIsect;

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::fillSummary qQverP"<<pSummary->qOverP << endmsg;

    pSummary->lhr = m_lhr;

    if (m_pMuGirl->msgLvl(MSG::DEBUG))
        m_pMuGirl->msg() << "Candidate::fillSummary ended" << endmsg;

    return StatusCode::SUCCESS;
}

const MuGirlNS::RIO_OnTrackLists& Candidate::getRpcHitsInSegments()
{
    if (m_RpcRIO_OnTrackLists.empty())
    {
        for (SegmentList::iterator itSeg = m_segments.begin(); itSeg != m_segments.end(); itSeg++)
        {
            Segment* pSeg = *itSeg;
            if (pSeg->technologyType() == RPC_TECH)
            {
                MuGirlNS::RIO_OnTrackList* pList = new RIO_OnTrackList();
                m_RpcRIO_OnTrackLists.push_back(pList);
                for (HitList::const_iterator itHit = pSeg->hits().begin(); itHit != pSeg->hits().end(); itHit++)
                {
                  const Trk::PrepRawData* pPrepData = (*itHit)->RIO()->prepRawData(); 
                  if (pPrepData == NULL)
                  {
                      continue;
                  }
                  BBox bbox;
                  PrepData2Bbox(pPrepData, bbox);

                  Chamber* pChamber = (*itHit)->chamber();
                  if (pChamber == NULL)
                  {
                      continue;
                  }
                  const Trk::TrkDetElementBase* pDetElement = pPrepData->detectorElement();
                  if (pDetElement == NULL)
                  {
                     continue;
                  }
                  const Trk::Surface& surface = pDetElement->surface(pPrepData->identify());
                  Intersection* pCombIsect =  pChamber->intersection(COMB_TECH, Chamber::SAME_DISTANCE);
                  if (pCombIsect == NULL)
                  {
                     continue;
                  }  
                  const Trk::TrackSurfaceIntersection* pHitTrkIsect =
                  m_pMuGirl->Intersector()->intersectSurface(surface, pCombIsect->trkIntersection(), m_QOverP);
                  if (pHitTrkIsect == NULL)
                  {
                     continue;
                  }
		  const Trk::RIO_OnTrack*  pRIO = m_pMuGirl->muonClusterCreator()->createRIO_OnTrack(*pPrepData, pHitTrkIsect->position()); 
                  delete pHitTrkIsect;
                  if (pRIO == NULL)
                  {
                      continue;
                  }
                  bool inDet = bbox.inside(pRIO->globalPosition().eta(),pRIO->globalPosition().phi());
                  if (!inDet)
                  {
                      if (m_pMuGirl->msgLvl(MSG::DEBUG))
                      {
                              m_pMuGirl->msg() <<  "RPC hit not in chamber, removing hit: " <<  (*itHit)->toString() << 
                               " bbox: "  << bbox.toString() << " RIO eta,phi " << pRIO->globalPosition().eta() << "," << pRIO->globalPosition().phi() << endmsg;
                      }  
                      delete pRIO;
                      continue; 
                  } 
                  else
                  { 
                       pList->push_back(pRIO);
                  } 
                }
            }
        }
    }
    return(m_RpcRIO_OnTrackLists); 
}

const MuGirlNS::RIO_OnTrackLists& Candidate::getTgcHitsInSegments()
{
    if (m_TgcRIO_OnTrackLists.empty())
    {
        for (SegmentList::iterator itSeg = m_segments.begin(); itSeg != m_segments.end(); itSeg++)
        {
            Segment* pSeg = *itSeg;
            if (pSeg->technologyType() == TGC_TECH)
            {
                MuGirlNS::RIO_OnTrackList* pList = new RIO_OnTrackList();
                m_TgcRIO_OnTrackLists.push_back(pList);
                for (HitList::const_iterator itHit = pSeg->hits().begin(); itHit != pSeg->hits().end(); itHit++)
                {
                    pList->push_back((*itHit)->RIO());
                }
            }
        }
    }
    return(m_TgcRIO_OnTrackLists);
}

std::string Candidate::cellToString(TechnologyType eTech,
                                    DistanceType eDist,
                                    RegionType eReg) const
{
    const Intersection* pIsect = cellIntersection(eTech, eDist, eReg);
    std::ostringstream oss;
    oss << "cellIntersection"
    << "(" << TechnologyTypeName(eTech)
    << ", " << DistanceTypeName(eDist)
    << ", " << RegionTypeName(eReg)
    << ") " << (pIsect == NULL ? "Null" : pIsect->toString());
    return(oss.str());
}

const Chamber* Candidate::findFirstValidChamber(TechnologyType eTech,
        DistanceType eDist,
        RegionType eReg) const
{
    const ChamberList& chambers = cellChambers(eTech, eDist, eReg);
    for (ChamberList::const_iterator itCh = chambers.begin(); itCh != chambers.end(); itCh++)
    {
        const Chamber* pChamber = *itCh;
        if (!pChamber->RIOs().empty())
            return pChamber;
    }
    return NULL;
}



std::pair<int,int> Candidate::hitsInMultilayer( const Muon::MuonSegment& segment ) const
{
   int nMl1(0);
   int nMl2(0);
   const std::vector< const Trk::MeasurementBase* >& measurements = segment.containedMeasurements();
   std::vector< const Trk::MeasurementBase* >::const_iterator it = measurements.begin(), itEnd = measurements.end();
   for(;it!=itEnd;++it)
   {
       const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(*it);
       if( mdt )
       {
          int ml = (m_pMuGirl->muonManager())->mdtIdHelper()->multilayer(mdt->identify());
          if( ml == 1 ) ++nMl1;
          if( ml == 2 ) ++nMl2;
       }
   }
   return std::make_pair(nMl1,nMl2);
}

bool Candidate::firstIsBest( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const
{

    std::pair<int,int> hitsInMl_1 = hitsInMultilayer(seg1);
    std::pair<int,int> hitsInMl_2 = hitsInMultilayer(seg2);

    unsigned int nmdtHits1 = hitsInMl_1.first + hitsInMl_1.second;
    unsigned int nmdtHits2 = hitsInMl_2.first + hitsInMl_2.second;
    if( nmdtHits1 > nmdtHits2 ) return true;
    if( nmdtHits1 < nmdtHits2 ) return false;

// prefer segments with fit quality (always expected)
    const Trk::FitQuality* fq1 = seg1.fitQuality();
    const Trk::FitQuality* fq2 = seg2.fitQuality();
    if( !fq1 && !fq2 ) {
      m_pMuGirl->msg(MSG::WARNING) << "None of the two segments has a fit quality! Choosing arbitrarily first to be the best!" << endmsg;
      return true;
    }
    else if( !fq1 && fq2 ) return false;
    else if( fq1  && !fq2 ) return true;
    else {
// select candidate with smallest chi2
      double chi2Ndof1 = fq1->chiSquared()/fq1->numberDoF();
      double chi2Ndof2 = fq2->chiSquared()/fq2->numberDoF();
      return chi2Ndof1 < chi2Ndof2;
    }
}

int Candidate::segmentQuality(const Muon::MuonSegment* pSegment) const
{
    return m_pMuGirl->segmentSelectionTool()->quality(*pSegment);
}
