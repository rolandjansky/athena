/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GlobalFitTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <vector>
#include "DataModel/DataVector.h"
#include "MuGirlGlobalFit/GlobalFitTool.h"
#include "MuonSegment/MuonSegment.h"
#include "iPatTrackParameters/PerigeeParameters.h"
//#include "Particle/TrackParticle.h"
//#include "TrkEventPrimitives/CovarianceMatrix.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/Track.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
//#include "TrkParameters/AtaCylinder.h"
//#include "TrkParameters/AtaDisc.h"
#include "MuonRecToolInterfaces/IMuonTrackFinder.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "TrkTrack/TrackCollection.h"

using MuGirlNS::GlobalFitTool;
//================ Constructor =================================================

GlobalFitTool::GlobalFitTool(const std::string& t,
                             const std::string& n,
                             const IInterface*  p)
        :
        AthAlgTool(t,n,p),
        m_trackFitter ("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"),
        m_MuonIdHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
        m_compClusterCreator("Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"),
        m_pIntersector("Trk::RungeKuttaIntersector/Trk::RungeKuttaIntersector"),
        m_probCut(0.01),
        m_nrefits(0),
        m_nfailedRefits(0)
{
    declareInterface<IGlobalFitTool>(this);
    //  template for property decalration
    declareProperty("trackFitter",  m_trackFitter);
    declareProperty("ProbabilityCut", m_probCut);
}

//================ Destructor =================================================

GlobalFitTool::~GlobalFitTool()
{}

//================ Initialisation =================================================

StatusCode GlobalFitTool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endmsg;
    StatusCode sc = AthAlgTool::initialize();
    if (sc.isFailure()) return sc;


   if ( m_trackFitter.retrieve().isFailure() ) 
   {
        msg(MSG::WARNING) << "Failed to retrieve trackFitter tool " << m_trackFitter << endmsg;
         return StatusCode::RECOVERABLE;
    } 
    msg(MSG::INFO) << "Retrieved TrackFitter tool " << m_trackFitter << endmsg;
   
    if ( m_MuonIdHelperTool.retrieve().isFailure() )
    {
        msg(MSG::WARNING) << "Failed to retrieve tool " << m_MuonIdHelperTool << endmsg;
        return StatusCode::RECOVERABLE;
    }
    msg(MSG::INFO) << "Retrieved tool " << m_MuonIdHelperTool << endmsg;

    if ( m_compClusterCreator.retrieve().isFailure() )
    {
        msg(MSG::WARNING) << "Failed to retrieve tool " << m_compClusterCreator << endmsg;
        return StatusCode::RECOVERABLE;
    }
    if (m_pIntersector.retrieve().isFailure())
        return StatusCode::RECOVERABLE;

    msg(MSG::INFO) << "Retrieved tool " << m_compClusterCreator << endmsg;
    msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
    return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode GlobalFitTool::finalize()
{
    msg(MSG::INFO) << "Finalizing " << name() << " - package version " << PACKAGE_VERSION << endmsg;
    float fracFailed = m_nrefits != 0 ? m_nfailedRefits/m_nrefits : 0.;
    msg(MSG::INFO) << " Total refits " << m_nrefits << " failed fits " << m_nfailedRefits << " fraction " << fracFailed << endmsg;
    return StatusCode::SUCCESS;
}

//============================================================================================

const Trk::Perigee* GlobalFitTool::calculateTrackParameters(const MuonSegmentList & pMuonSegments, double qoverp)
{

 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"in GlobalFitTool::calculateTrackParameters" << endmsg;
    msg(MSG::DEBUG) << "in GlobalFitTool::calculateTrackParameters"<<endmsg;
    const Trk::TrackParameters* Track = NULL;


    double radius = 4200.;
    double halfLength = 6800.;

    // create the mean surface for the barrel
    Trk::CylinderSurface barrelCylinder(radius, halfLength);

    // create the mean surface for the endcap
    Trk::DiscSurface forwardDisc( new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,halfLength))),0, radius );
    Trk::DiscSurface backwardDisc( new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-halfLength))),0, radius );
/* 
    Trk::CylinderSurface barrelCylinder(new HepGeom::Transform3D, radius, halfLength);
    CLHEP::HepRotation discRotation;
    HepGeom::Transform3D* transform;
    CLHEP::Hep3Vector forwardDiscPosition(0.,0.,halfLength);
    transform = new HepGeom::Transform3D(discRotation,forwardDiscPosition);
    Trk::DiscSurface forwardDisc(transform, 0., radius);
    CLHEP::Hep3Vector backwardDiscPosition(0.,0.,-halfLength);
    transform= new HepGeom::Transform3D(discRotation,backwardDiscPosition);
    Trk::DiscSurface backwardDisc(transform, 0., radius);
*/
    const Muon::MuonSegment* pSegment = NULL;
    MuonSegmentList newSegs;

    SortSegments(pMuonSegments,newSegs);
    for (MuonSegmentList::const_iterator MuonSegItr = newSegs.begin(); MuonSegItr != newSegs.end(); MuonSegItr++)
    {

       const Identifier& id = (*MuonSegItr)->rioOnTrack(0)->identify();
       if ((m_MuonIdHelperTool->isMdt(id)))  
       {
          pSegment = (*MuonSegItr);
          if (msgLvl(MSG::DEBUG))
              msg(MSG::DEBUG) <<"calculateTrackParamenters :: found MDT segment "
                              << " Global Position: " << pSegment->globalPosition() 
                              << " Distance: " << pSegment->globalPosition().mag() << endmsg;
          break;
       }
    }
    if (pSegment == NULL)
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"in calculateTrackParameters:  NULL segment" << endmsg;
      return NULL;
    }
    const Trk::TrackSurfaceIntersection *pSegIsect = new Trk::TrackSurfaceIntersection(pSegment->globalPosition(),pSegment->globalDirection().unit(),0.0);
    if (pSegIsect == NULL)
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"in calculateTrackParameters:  NULL segment intersection" << endmsg;
          return NULL;
    }

     
    const Trk::TrackSurfaceIntersection* pEntranceIsect=NULL;
    double charge = 1.0;
    if ( qoverp <0) charge = -1.0;
    double mom = fabs(1./qoverp);
    if (fabs(pSegIsect->position().eta()) <= 1.0)
    {
        pEntranceIsect = m_pIntersector->intersectCylinderSurface(barrelCylinder, pSegIsect, qoverp);
  
        if (pEntranceIsect == NULL)
        {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"in calculateTrackParameters:  NULL cylinder isect "  << endmsg;
           return NULL;
        }
        else 
          Track = new Trk::AtaCylinder(pEntranceIsect->position(),pEntranceIsect->direction()*mom,charge,barrelCylinder ) ;                 
    }
    else if (pSegIsect->position().eta() > 1.0)
    { 
        pEntranceIsect = m_pIntersector->intersectDiscSurface(forwardDisc, pSegIsect, qoverp);
        if (pEntranceIsect == NULL )
        {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"in calculateTrackParameters:  NULL fwdDisc isect "  << endmsg;
            return NULL;
        }
        else 
           Track = new Trk::AtaDisc(pEntranceIsect->position(),pEntranceIsect->direction()*mom,charge,forwardDisc ) ;                 
    }
    else
    {
        pEntranceIsect = m_pIntersector->intersectDiscSurface(backwardDisc, pSegIsect, qoverp);
        if (pEntranceIsect == NULL )
        {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"in calculateTrackParameters:  NULL bkwdDisc isect "  << endmsg;
            return NULL;
        }
        else 
           Track = new Trk::AtaDisc(pEntranceIsect->position(),pEntranceIsect->direction()*mom,charge,backwardDisc ) ;                 
    }


    Amg::Vector3D    origin( pEntranceIsect->position());
    Trk::PerigeeSurface* perigeeSurface = new Trk::PerigeeSurface(origin); 
    const Trk::Perigee* trkPerigee = new Trk::Perigee(origin,Track->momentum(),Track->charge(),*perigeeSurface);
    delete Track;
    return trkPerigee;
}


Trk::MeasurementSet GlobalFitTool::prepareMeasurementSet(const MuonSegmentList & pMuonSegments,std::vector<const Muon::CompetingMuonClustersOnTrack*>& competingRIO )
{

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in GlobalFitTool::prepareMeasurementSet" << endmsg;
    std::string SegmentTechnology;
    double segmentProbability = 0;
    Trk::MeasurementSet measurementSet;
    int counter = 0;
    
    int nseg_ok = 0;

    for (MuonSegmentList::const_iterator MuonSegItr = pMuonSegments.begin(); MuonSegItr != pMuonSegments.end(); MuonSegItr++)
    {
        const Muon::MuonSegment* pSegment = *MuonSegItr;
	
	auto checkSegment = pSegment->rioOnTrack(0);
	if (!checkSegment) continue;

        const Identifier& id = pSegment->rioOnTrack(0)->identify();
        if (m_MuonIdHelperTool->isMdt(id)) SegmentTechnology = "MDT";
        if (m_MuonIdHelperTool->isRpc(id)) SegmentTechnology = "RPC";
        if (m_MuonIdHelperTool->isTgc(id)) SegmentTechnology = "TGC";
        if (m_MuonIdHelperTool->isCsc(id)) SegmentTechnology = "CSC";
        int ndf;
        double chisq;
        if (pSegment->fitQuality() != NULL)
        {
            ndf = pSegment->fitQuality()->numberDoF();
            chisq = pSegment->fitQuality()->chiSquared();
            if (ndf > 0 && chisq > 0)
                segmentProbability = 1. - Genfun::CumulativeChiSquare(ndf)(chisq);
            else continue;
        }
        else continue;
        if (msgLvl(MSG::DEBUG)) msg() << SegmentTechnology
            << " Segment Chi2: " << chisq
            << " N DOF: " << ndf
            << " Probability: " << segmentProbability
            << " Global Position: " << pSegment->globalPosition() 
            << " Distance: " << pSegment->globalPosition().mag() << endmsg;


        if (SegmentTechnology == "MDT" && segmentProbability < m_probCut ) continue;

        if (SegmentTechnology == "RPC" || SegmentTechnology == "TGC")
        {
            std::list<const Trk::PrepRawData*> prds;
            std::vector<unsigned int> usedHits;
            if (msgLvl(MSG::DEBUG)) msg() << "Creating competing Rios" << endmsg;
            for (unsigned int i = 0; i < pSegment->numberOfContainedROTs(); i++)
            {
	        counter++;
                bool isPhiHit = m_MuonIdHelperTool->measuresPhi(pSegment->rioOnTrack(i)->identify());
                if (std::find(usedHits.begin(),usedHits.end(),i) != usedHits.end()) continue;
                prds.push_back(pSegment->rioOnTrack(i)->prepRawData());
                for (unsigned int j = i + 1; j < pSegment->numberOfContainedROTs(); j++)
                {
                    if (isPhiHit != m_MuonIdHelperTool->measuresPhi(pSegment->rioOnTrack(j)->identify())) continue;
                    if (std::find(usedHits.begin(),usedHits.end(),j) != usedHits.end()) continue;
                    if (pSegment->rioOnTrack(j)->detectorElement() == pSegment->rioOnTrack(i)->detectorElement())
                    {
                        prds.push_back(pSegment->rioOnTrack(j)->prepRawData());
                        usedHits.push_back(j);
                    }
                }
                if (prds.size() > 0)
                {
                    const Muon::CompetingMuonClustersOnTrack* rot = m_compClusterCreator->createBroadCluster(prds, 1.0);
                    competingRIO.push_back(rot);
                    if (rot != NULL)
                    {
                        measurementSet.push_back(dynamic_cast<const Trk::MeasurementBase*>(rot));
			m_pRefittedTrkNumTrigHits++;
                    }
                    prds.clear();
                }
            }
            usedHits.clear();
            prds.clear();
        }
        else
        {
            // if(SegmentTechnology == "MDT" ) m_pRefittedTrkNumMdtHits += pSegment->numberOfMeasurementBases();
            // for(unsigned int i = 0; i < pSegment->numberOfMeasurementBases(); i++)
            // {
                // measurementSet.push_back(pSegment->measurement(i));

                // if (msgLvl(MSG::DEBUG))
                    // msg() << "Global Position before sort: " << pSegment->measurement(i)->globalPosition() << " Magnitude: " << (pSegment->measurement(i)->globalPosition()).mag() << endmsg;
            // }
            if(SegmentTechnology == "MDT" ) m_pRefittedTrkNumMdtHits += pSegment->numberOfContainedROTs();
            for(unsigned int i = 0; i < pSegment->numberOfContainedROTs(); i++)
            {
                measurementSet.push_back(pSegment->rioOnTrack(i));

                if (msgLvl(MSG::DEBUG))
                    msg() << "Global Position before sort: " << pSegment->rioOnTrack(i)->globalPosition() << " Magnitude: " << (pSegment->rioOnTrack(i)->globalPosition()).mag() << endmsg;
            }
        }
    nseg_ok++;
    }


    return measurementSet;
}
//==========================================

const Trk::Track* GlobalFitTool::globalFit(const MuonSegmentList & pMuonSegments,double qoverp, BooleanProperty doNTuple=false)
{
    std::vector<const Muon::CompetingMuonClustersOnTrack*> competingRIO;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Start GLOBALFIT for MF "<<endmsg;

    m_pRefittedTrkNumTrigHits = 0;
    m_pRefittedTrkNumMdtHits = 0;
    m_pRefittedTrkMeasurementSetSize = 0;    


    Trk::MeasurementSet measurementSet = prepareMeasurementSet(pMuonSegments,competingRIO);

    if (measurementSet.size() != 0)
    {
        m_pRefittedTrkMeasurementSetSize =  measurementSet.size();

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Measurement set built. Size: " <<measurementSet.size()<< endmsg;
        SortMeasurementSet (measurementSet);

  
       Trk::RunOutlierRemoval runOutlier = false;    
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Starting standalone FIT " << endmsg;

       const Trk::Perigee* trk1 = calculateTrackParameters(pMuonSegments,qoverp);
      
     if (trk1 == NULL)  
     {

         if (msgLvl(MSG::DEBUG)) 
             msg(MSG::DEBUG) << "Initial perigee is NULL "<<endmsg;
         return NULL;
     }
     if (msgLvl(MSG::DEBUG)) 
           msg(MSG::DEBUG) << "Track parameters: z0 "<<trk1->parameters()[Trk::z0]<<" phi "
                           << trk1->parameters()[Trk::phi0]<<" eta "<<trk1->eta()
                           << " qoverp "<< trk1->parameters()[Trk::qOverP]<<" pt "<<trk1->pT()<< endmsg;



       Trk::Track* RefittedTrack = m_trackFitter->fit(measurementSet,*trk1, runOutlier, Trk::muon);

       if (RefittedTrack!=NULL)
       {  
          if (msgLvl(MSG::DEBUG)) 
              msg(MSG::DEBUG) << "GlobalFitTool REFIT track pT "<<RefittedTrack->perigeeParameters()->pT() << endmsg;
          if  (doNTuple) 
              if (fillNTuple(RefittedTrack).isFailure())
                  if (msgLvl(MSG::DEBUG)) 
                      msg(MSG::DEBUG) << "GlobalFitTool Cannot fill Reffited Track in NTuple" << endmsg;
          measurementSet.clear();
          for (size_t i = 0; i < competingRIO.size(); i++)
              delete competingRIO[i];
          competingRIO.clear();
          RefittedTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuGirl);   /////  sign track
          return RefittedTrack;
       }
       else
       {
          if (msgLvl(MSG::DEBUG)) msg(MSG::WARNING) << "GlobalFitTool Failed to refit the track" << endmsg;
          // delete RefittedTrack; -- RefittedTrack is NULL due to the if condition and should therefore not be deleted!
          measurementSet.clear();
          for (size_t i = 0; i < competingRIO.size(); i++)
                delete competingRIO[i];
          competingRIO.clear();
          return NULL;
       }
    }
    else
    {
        if (msgLvl(MSG::DEBUG)) msg(MSG::WARNING) << "Measurement Set is NULL or IDTrack is NULL. Fit can't be done" << endmsg;
        measurementSet.clear();
        for (size_t i = 0; i < competingRIO.size(); i++)
                delete competingRIO[i];
        competingRIO.clear();
        return NULL;
    }
}


//==========================================
const Trk::Track* GlobalFitTool::globalFit(const MuonSegmentList & pMuonSegments, const Trk::Perigee *trk1, BooleanProperty doNTuple=false)
{
    std::vector<const Muon::CompetingMuonClustersOnTrack*> competingRIO;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Start GLOBALFIT for MuonEFTrack "<<endmsg;

    m_pRefittedTrkNumTrigHits = 0;
    m_pRefittedTrkNumMdtHits = 0;
    m_pRefittedTrkMeasurementSetSize = 0;    


    Trk::MeasurementSet measurementSet = prepareMeasurementSet(pMuonSegments,competingRIO);

    if (measurementSet.size() != 0)
    {
       m_pRefittedTrkMeasurementSetSize =  measurementSet.size();

       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Measurement set built. Size: " <<measurementSet.size()<< endmsg;
       SortMeasurementSet (measurementSet);

  
       Trk::RunOutlierRemoval runOutlier = false;    
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Starting standalone FIT " << endmsg;

       if (trk1 == NULL)  
       {

          if (msgLvl(MSG::DEBUG)) 
             msg(MSG::DEBUG) << "Initial perigee is NULL after calculateTrackParameters, return NULL"<<endmsg;
          return NULL;
       }
       if (msgLvl(MSG::DEBUG)) 
           msg(MSG::DEBUG) << "Track parameters: z0 "<<trk1->parameters()[Trk::z0]<<" phi "
                           << trk1->parameters()[Trk::phi0]<<" eta "<<trk1->eta()
                           << " qoverp "<< trk1->parameters()[Trk::qOverP]<<" pt "<<trk1->pT()<< endmsg;



       Trk::Track* RefittedTrack = m_trackFitter->fit(measurementSet,*trk1, runOutlier, Trk::muon);

       if (RefittedTrack!=NULL)
       {  
          if (msgLvl(MSG::DEBUG)) 
              msg(MSG::DEBUG) << "GlobalFitTool REFIT track pT "<<RefittedTrack->perigeeParameters()->pT() << endmsg;
          if  (doNTuple) 
              if (fillNTuple(RefittedTrack).isFailure())
                  if (msgLvl(MSG::DEBUG)) 
                      msg(MSG::DEBUG) << "GlobalFitTool Cannot fill Reffited Track in NTuple" << endmsg;
          measurementSet.clear();
          for (size_t i = 0; i < competingRIO.size(); i++)
              delete competingRIO[i];
          competingRIO.clear();
          RefittedTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuGirl);   /////  sign track
          return RefittedTrack;
       }
       else
       {
          if (msgLvl(MSG::DEBUG)) msg(MSG::WARNING) << "GlobalFitTool Failed to refit the track" << endmsg;
          // delete RefittedTrack; -- RefittedTrack is NULL due to the if condition and should therefore not be deleted!
          measurementSet.clear();
          for (size_t i = 0; i < competingRIO.size(); i++)
                delete competingRIO[i];
          competingRIO.clear();
          return NULL;
       }
    }
    else
    {
        if (msgLvl(MSG::DEBUG)) msg(MSG::WARNING) << "Measurement Set is NULL or IDTrack is NULL. Fit can't be done" << endmsg;
        measurementSet.clear();
        for (size_t i = 0; i < competingRIO.size(); i++)
                delete competingRIO[i];
        competingRIO.clear();
        return NULL;
    }
}


const Trk::Track* GlobalFitTool::globalFit(const Trk::Track * IDTrack, const MuonSegmentList & pMuonSegments,BooleanProperty doNTuple=false)
{
    std::vector<const Muon::CompetingMuonClustersOnTrack*> competingRIO ;
    Trk::MeasurementSet measurementSet = prepareMeasurementSet(pMuonSegments,competingRIO);

    m_pRefittedTrkNumTrigHits = 0;
    m_pRefittedTrkNumMdtHits = 0;
    m_pRefittedTrkMeasurementSetSize = 0;    

    if (IDTrack!=NULL && measurementSet.size() != 0)
    {
        m_pRefittedTrkMeasurementSetSize =  measurementSet.size();

        if (msgLvl(MSG::DEBUG)) msg() << "Measurement set built. Size: " <<measurementSet.size()<< endmsg;
        SortMeasurementSet (measurementSet);
        if (msgLvl(MSG::DEBUG)) msg() << "Measurement set sorted " << endmsg;
        const Trk::Track* RefittedTrack= NULL;
	if (m_pRefittedTrkMeasurementSetSize > 2)
	  RefittedTrack=m_trackFitter->indetExtension(*IDTrack,measurementSet);
        if (msgLvl(MSG::DEBUG)) msg() << "Global Fit done" << endmsg;
        if (RefittedTrack!=NULL)
        {


            if (doNTuple) if (fillNTuple(RefittedTrack).isFailure())
                    msg(MSG::WARNING) << "GlobalFitTool Cannot fill Reffited Track in NTuple" << endmsg;
            measurementSet.clear();
            for (size_t i = 0; i < competingRIO.size(); i++)
                delete competingRIO[i];
            competingRIO.clear();
            RefittedTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuGirl);   /////  sign track
            return RefittedTrack;
        }
        else
        {
           if (msgLvl(MSG::DEBUG))  msg(MSG::WARNING) << "GlobalFitTool Failed to refit the track" << endmsg;
            // delete RefittedTrack; -- RefittedTrack is NULL due to the if condition and should therefore not be deleted!
            measurementSet.clear();
            for (size_t i = 0; i < competingRIO.size(); i++)
                delete competingRIO[i];
            competingRIO.clear();
           return NULL;
        }
    }
    else
    {
        if (msgLvl(MSG::DEBUG)) msg() << "Measurement Set is NULL or IDTrack is NULL. Fit can't be done" << endmsg;
        measurementSet.clear();
        for (size_t i = 0; i < competingRIO.size(); i++)
            delete competingRIO[i];
        competingRIO.clear();
        return NULL;
    }
}
//=================================  standAloneRefit

const Trk::Track* GlobalFitTool::standAloneRefit(const Trk::Track& cbtrack)
{
  ++m_nrefits;
  const Trk::Track* refittedTrack=m_trackFitter->standaloneRefit(cbtrack);
  if (refittedTrack == nullptr){
    ++m_nfailedRefits;
    if( m_nfailedRefits < 10 ) ATH_MSG_DEBUG("standaloneRefit failed ");
    return nullptr;
  }
  return refittedTrack;
}

//===================================================================================================
StatusCode GlobalFitTool::bookNTuple(NTuple::Tuple* pNtuple, const std::string type="")
{
    msg(MSG::INFO) << " Adding "+type+" Reffited Track to NTuple" << endmsg;

    if (pNtuple->addItem("MUGIRL/"+type+"RefittedTrkPt"     , m_RefittedTrkPt).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkEta"   , m_RefittedTrkEta).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkPhi"   , m_RefittedTrkPhi).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkTheta" , m_RefittedTrkTheta).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrka0"    , m_RefittedTrka0).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkz0"    , m_RefittedTrkz0).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkQOverP", m_RefittedTrkQOverP).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkCharge", m_RefittedTrkCharge).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkChi2"  , m_RefittedTrkChi2).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkNDofFr", m_RefittedTrkNDofFr).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkChi2pr", m_RefittedTrkChi2pr).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkCov11" , m_RefittedTrkCov11).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkCov22" , m_RefittedTrkCov22).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkCov33" , m_RefittedTrkCov33).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkCov44" , m_RefittedTrkCov44).isFailure() ||
            pNtuple->addItem("MUGIRL/"+type+"RefittedTrkCov55",  m_RefittedTrkCov55).isFailure()  ||
	    
	    pNtuple->addItem("MUGIRL/"+type+"RefittedTrkNumTrigHits", m_RefittedTrkNumTrigHits).isFailure() ||
	    pNtuple->addItem("MUGIRL/"+type+"RefittedTrkNumMdtHits", m_RefittedTrkNumMdtHits).isFailure() ||
	    pNtuple->addItem("MUGIRL/"+type+"RefittedTrkMeasurementSetSize", m_RefittedTrkMeasurementSetSize).isFailure() ||
	    pNtuple->addItem("MUGIRL/"+type+"RefittedTrkMeasurementsOnTrack", m_RefittedTrkMeasurementsOnTrack).isFailure() ||
	    pNtuple->addItem("MUGIRL/"+type+"RefittedTrkOutliesOnTrack", m_RefittedTrkOutliersOnTrack).isFailure())
    {
        msg(MSG::WARNING) << "Cannot addItem(MUGIRL/"+type+"_<Reffited Track Parameters>)" << endmsg;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode GlobalFitTool::fillNTuple(const Trk::Track* pRefittedTrack)
{
    if (msgLvl(MSG::DEBUG)) msg() << "Adding Refitted Track to NTuple" << endmsg;
    
    if (pRefittedTrack->measurementsOnTrack() != NULL)
        m_RefittedTrkMeasurementsOnTrack = pRefittedTrack->measurementsOnTrack()->size();
    else 
        m_RefittedTrkMeasurementsOnTrack = 0;
    if (pRefittedTrack->outliersOnTrack() != NULL)
       m_RefittedTrkOutliersOnTrack = pRefittedTrack->outliersOnTrack()->size();
    else
       m_RefittedTrkOutliersOnTrack = 0;
    m_RefittedTrkNumTrigHits = m_pRefittedTrkNumTrigHits;
    m_RefittedTrkNumMdtHits =  m_pRefittedTrkNumMdtHits;
    m_RefittedTrkMeasurementSetSize =  m_pRefittedTrkMeasurementSetSize;    
    
    const Trk::Perigee* pRefittedPerigee=pRefittedTrack->perigeeParameters();
    if (pRefittedPerigee!=NULL)
    {
        //Trk::JacobianThetaPToCotThetaPt jacob(pRefittedPerigee->parameters()[Trk::theta],pRefittedPerigee->parameters()[Trk::qOverP]);
        //const Trk::MeasuredPerigee* measuredPerigee =
        //    dynamic_cast<const Trk::MeasuredPerigee*>(pRefittedPerigee);
        //CLHEP::HepSymMatrix* covMatrixRefitted =
        //    new CLHEP::HepSymMatrix(measuredPerigee->localErrorMatrix().covariance().similarity(jacob));

        m_RefittedTrkPt     = pRefittedPerigee->pT();
        m_RefittedTrkEta    = pRefittedPerigee->eta();
        m_RefittedTrkPhi    = pRefittedPerigee->parameters()[Trk::phi0];
        m_RefittedTrkQOverP = pRefittedPerigee->parameters()[Trk::qOverP];
        m_RefittedTrkTheta  = 1./tan(pRefittedPerigee->parameters()[Trk::theta]);
        m_RefittedTrka0     = pRefittedPerigee->parameters()[Trk::d0];
        m_RefittedTrkz0     = pRefittedPerigee->parameters()[Trk::z0];
        m_RefittedTrkCharge = pRefittedPerigee->charge();
        m_RefittedTrkChi2   = pRefittedTrack->fitQuality()->chiSquared();
        m_RefittedTrkNDofFr = pRefittedTrack->fitQuality()->numberDoF();
        

        if (m_RefittedTrkNDofFr>0 && m_RefittedTrkChi2>0) m_RefittedTrkChi2pr = 1. - Genfun::CumulativeChiSquare(pRefittedTrack->fitQuality()->numberDoF())(pRefittedTrack->fitQuality()->chiSquared());
        else m_RefittedTrkChi2pr=-100.0;

        //Fill covariance parameters
        if (pRefittedPerigee->covariance()) {
            const AmgMatrix(5,5)* errorMatrix = pRefittedPerigee->covariance();
            AmgSymMatrix(5) covMatrixRefitted;
            Trk::JacobianThetaPToCotThetaPt jacob(pRefittedPerigee->parameters()[Trk::theta],pRefittedPerigee->parameters()[Trk::qOverP]);
            covMatrixRefitted = errorMatrix->similarity(jacob);

            m_RefittedTrkCov11  = (covMatrixRefitted)(Trk::d0,Trk::d0);
            m_RefittedTrkCov22  = (covMatrixRefitted)(Trk::z0,Trk::z0);
            m_RefittedTrkCov33  = (covMatrixRefitted)(2,2);
            m_RefittedTrkCov44  = (covMatrixRefitted)(3,3);
            m_RefittedTrkCov55  = (covMatrixRefitted)(4,4);
        } else {
            m_RefittedTrkCov11     = -100.0;
            m_RefittedTrkCov22     = -100.0;
            m_RefittedTrkCov33     = -100.0;
            m_RefittedTrkCov44     = -100.0;
            m_RefittedTrkCov55     = -100.0;
        }

        //m_RefittedTrkCov11  = (*covMatrixRefitted)[Trk::d0][Trk::d0];
        //m_RefittedTrkCov22  = (*covMatrixRefitted)[Trk::z0][Trk::z0];
        //m_RefittedTrkCov33  = (*covMatrixRefitted)[2][2];
        //m_RefittedTrkCov44  = (*covMatrixRefitted)[3][3];
        //m_RefittedTrkCov55  = (*covMatrixRefitted)[4][4];
	
        //delete covMatrixRefitted;
    }
    else
    {
        m_RefittedTrkPt        = -100.0;
        m_RefittedTrkEta       = -100.0;
        m_RefittedTrkPhi       = -100.0;
        m_RefittedTrkQOverP    = -100.0;
        m_RefittedTrkCharge    = -100.0;
        m_RefittedTrkChi2      = -100.0;
        m_RefittedTrkTheta     = -100.0;
        m_RefittedTrka0        = -100.0;
        m_RefittedTrkz0        = -100.0;
        m_RefittedTrkNDofFr    = -100.0;
        m_RefittedTrkChi2pr    = -100.0;
        m_RefittedTrkCov11     = -100.0;
        m_RefittedTrkCov22     = -100.0;
        m_RefittedTrkCov33     = -100.0;
        m_RefittedTrkCov44     = -100.0;
        m_RefittedTrkCov55     = -100.0;
    }
    msg(MSG::DEBUG) << "GlobalFitTool REFIT track pT "<<m_RefittedTrkPt << endmsg;
    return StatusCode::SUCCESS;
}

PerigeeParameters *
GlobalFitTool::convertPerigee (const Trk::Perigee* trkPerigee)
{
    double d0        = trkPerigee->parameters()[Trk::d0];
    double z0        = trkPerigee->parameters()[Trk::z0];
    double phi       = trkPerigee->parameters()[Trk::phi];
    double theta     = trkPerigee->parameters()[Trk::theta];
    double ctgtheta  = 1./tan(trkPerigee->parameters()[Trk::theta]);
    double qOverP    = trkPerigee->parameters()[Trk::qOverP];
    double choverpt  = trkPerigee->charge()/trkPerigee->pT();
    PerigeeParameters* perigee = new PerigeeParameters();
    perigee->fillPerigee(cos(phi),ctgtheta,choverpt,sin(phi),d0,z0);

    //const Trk::MeasuredPerigee* measuredPerigee =
    //    dynamic_cast<const Trk::MeasuredPerigee*>(trkPerigee);
    //if (! measuredPerigee) return perigee;

    if (trkPerigee->covariance()) {
        Trk::JacobianThetaPToCotThetaPt jacob(theta,qOverP);
        AmgSymMatrix(5)* newcov = new AmgSymMatrix(5);
        *newcov = trkPerigee->covariance()->similarity(jacob);
        perigee->covariance(newcov);
    }
    //CLHEP::HepSymMatrix* newcov =
    //    new CLHEP::HepSymMatrix(measuredPerigee->localErrorMatrix().covariance().similarity(jacob));
    //perigee->covariance(newcov);

    //delete newcov;
    return perigee;
}

bool ByGlobalPosition(  const Trk::MeasurementBase* cr1,  const Trk::MeasurementBase* cr2 )
{
    if ( cr1!=NULL && cr2!=NULL)
        return ((cr1->globalPosition()).mag() < (cr2->globalPosition()).mag());
    return false;
}

bool isMagEqual(  const Trk::MeasurementBase* cr1,  const Trk::MeasurementBase* cr2 )
{
    if ( cr1!=NULL && cr2!=NULL)
    {
        double GP1=(cr2->globalPosition()).mag();
        double GP2=(cr1->globalPosition()).mag();
        return (fabs(GP1-GP2)<0.01);
    }
    return false;
}

bool compareByDistance (const std::pair<double, const Trk::MeasurementBase*> &x,
                        const std::pair<double, const Trk::MeasurementBase*> &y)
{
    return x.first < y.first;
}
bool compareMagEqual (const std::pair<double, const Trk::MeasurementBase*> &x,
                      const std::pair<double, const Trk::MeasurementBase*> &y)
{
    return fabs(x.first - y.first)<0.01;
}

void GlobalFitTool::SortMeasurementSet (Trk::MeasurementSet& inputMbs)
{
    std::vector< std::pair<double, const Trk::MeasurementBase*> > hitOrder;
    for( Trk::MeasurementSet::const_iterator itSet = inputMbs.begin(); itSet != inputMbs.end(); ++itSet)
    {
        hitOrder.push_back(std::make_pair((*itSet)->globalPosition().mag(),*itSet));
    }
    std::sort(hitOrder.begin(), hitOrder.end(), compareByDistance);
    inputMbs.erase(inputMbs.begin(), inputMbs.end());



    std::vector< std::pair<double, const Trk::MeasurementBase*> >::iterator order_last;
    order_last = std::unique(hitOrder.begin(), hitOrder.end(),compareMagEqual);

    hitOrder.erase(order_last, hitOrder.end());

    for (std::vector< std::pair<double, const Trk::MeasurementBase*> >::const_iterator order = hitOrder.begin();
            order != hitOrder.end();
            ++order)
    {
        inputMbs.push_back(((*order).second));

    }
    hitOrder.clear();
    if (msgLvl(MSG::DEBUG))
    {
        for (Trk::MeasurementSet::const_iterator itSet = inputMbs.begin(); itSet != inputMbs.end(); ++itSet)
        {
            if (*itSet)
                msg(MSG::DEBUG) << "Measurement Global Position after sort: " << (*itSet)->globalPosition() << " Magnitude: " << ((*itSet)->globalPosition()).mag() << endmsg;
        }
    }

}
void GlobalFitTool::SortSegments(const MuonSegmentList & pMuonSegments, MuonSegmentList & pSortedSegments)
{
    std::vector< std::pair<double,const Muon::MuonSegment*> > segOrder;
    for (MuonSegmentList::const_iterator itSet = pMuonSegments.begin(); itSet != pMuonSegments.end(); ++itSet)
    {
        segOrder.push_back(std::make_pair((*itSet)->globalPosition().mag(),*itSet));
        
    }
    std::sort(segOrder.begin(), segOrder.end(), compareByDistance);
    pSortedSegments.clear();

    std::vector< std::pair<double, const Muon::MuonSegment*> >::iterator order_last;
    order_last = std::unique(segOrder.begin(), segOrder.end(),compareMagEqual);

    segOrder.erase(order_last, segOrder.end());

    for (std::vector< std::pair<double, const Muon::MuonSegment*> >::const_iterator order = segOrder.begin();
            order != segOrder.end();
            ++order)
    {
 
        //const Muon::MuonSegment* pSegment = (*order).second;
        pSortedSegments.push_back((*order).second);

    }
    segOrder.clear();
    if (msgLvl(MSG::DEBUG))
    {
        for( MuonSegmentList::const_iterator itSet = pSortedSegments.begin(); itSet != pSortedSegments.end(); ++itSet)
        {
            if (*itSet)
                msg(MSG::DEBUG) << "Segment Global Position after sort: " << (*itSet)->globalPosition() << " Magnitude: " << ((*itSet)->globalPosition()).mag() << endmsg;
        }
    }
}

