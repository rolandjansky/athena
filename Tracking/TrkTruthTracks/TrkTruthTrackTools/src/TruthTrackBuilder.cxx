/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
// TruthTrackBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// package include
#include "TruthTrackBuilder.h"
// Trk includes
#include "TrkTrack/Track.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
// Gaudi
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
// HepMC
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "HepPDT/ParticleDataTable.hh"

#include "AtlasDetDescr/AtlasDetectorID.h"

/** Constructor **/
Trk::TruthTrackBuilder::TruthTrackBuilder(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p),
  m_trackFitter(""),
  m_extrapolator(""),
  m_rotcreator(""),
  m_rotcreatorbroad(""),
  m_DetID(nullptr),
  m_particlePropSvc("PartPropSvc",n),
  m_particleDataTable(nullptr),
  m_minNdof(6),
  m_onlyPrimaries(false),
  m_primaryBarcodeCutOff(100000),
  m_minSiHits(7),
  m_minSiHitsForward(m_minSiHits),
  m_forwardBoundary(2.5)
{
    declareInterface<Trk::ITruthTrackBuilder>(this);
    // TrackFitter
    declareProperty("TrackFitter",                       m_trackFitter);
    declareProperty("ExtrapolationTool",                 m_extrapolator);
    declareProperty("RotCreatorTool",                    m_rotcreator);
    declareProperty("BroadRotCreatorTool",               m_rotcreatorbroad);
    declareProperty("MinDegreesOfFreedom",               m_minNdof);
    declareProperty("OnlyPrimaries",                     m_onlyPrimaries); 
    declareProperty("PrimaryBarcodeCutOff",              m_primaryBarcodeCutOff);
    declareProperty("MinSiHits",                         m_minSiHits);
    declareProperty("MinSiHitsForward",                  m_minSiHitsForward);
    declareProperty("ForwardBoundary",                   m_forwardBoundary);

}


// Athena algtool's Hooks
StatusCode  Trk::TruthTrackBuilder::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");
    if (m_rotcreator.retrieve().isFailure()) { 
      msg(MSG::FATAL) << "Could not get " << m_rotcreator.type() << endmsg;
      return StatusCode::FAILURE; 
    }

    if (m_rotcreatorbroad.retrieve().isFailure()) { 
      msg(MSG::FATAL) << "Could not get " << m_rotcreatorbroad.type() << endmsg;
      return StatusCode::FAILURE; 
    }

    // track fitter
    if (m_trackFitter.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_trackFitter << ". Aborting ...");
        return StatusCode::FAILURE;
    }
    
    // extrapolator
    if (m_extrapolator.retrieve().isFailure()){
        ATH_MSG_ERROR("Could not retrieve " << m_extrapolator << ". Aborting ...");
        return StatusCode::FAILURE;
    }
        
    // particle property service
    if (m_particlePropSvc.retrieve().isFailure())
    {
        ATH_MSG_ERROR("Can not retrieve " << m_particlePropSvc << " . Aborting ... " );
        return StatusCode::FAILURE;
    }
    
    // and the particle data table 
    m_particleDataTable = m_particlePropSvc->PDT();
    if (m_particleDataTable==nullptr)
    {
      ATH_MSG_ERROR( "Could not get ParticleDataTable! Cannot associate pdg code with charge. Aborting. " );
      return StatusCode::FAILURE;
    }    
    
    // need an Atlas id-helper to identify sub-detectors, take the one from detStore
    if (detStore()->retrieve(m_DetID, "AtlasID").isFailure()) {
      ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

                                    
StatusCode  Trk::TruthTrackBuilder::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}


Trk::Track* Trk::TruthTrackBuilder::createTrack(const PRD_TruthTrajectory& prdTraj, SegmentCollection* segs ) const 
{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    if( segs != nullptr ){
      ATH_MSG_WARNING("Requested to fill segment collection but mode not implemented");
    }
    ATH_MSG_VERBOSE("The PRD Truth trajectory contains " << prdTraj.prds.size() << " PRDs.");

    // get the associated GenParticle
    auto genPart = prdTraj.genParticle;
    if (!genPart) {
        ATH_MSG_WARNING("No GenParticle associated to this PRD_TruthTrajectory. Ignoring track creation.");
        return nullptr;
    }    
    ATH_MSG_VERBOSE("Got PRD Truth trajectory with " << prdTraj.nDoF << " degrees of freedom.");
    // check min degrees of freedom
    if ( m_minNdof > 0 &&  prdTraj.nDoF < m_minNdof) return nullptr;
    // get the startPosition : math library madness as usual
    Amg::Vector3D startPos = genPart->production_vertex() ?
      Amg::Vector3D(genPart->production_vertex()->position().x(),
                            genPart->production_vertex()->position().y(),
		    genPart->production_vertex()->position().z()) : Amg::Vector3D(0.,0.,0.);
    Amg::Vector3D startMom(genPart->momentum().x(),
                                 genPart->momentum().y(),
                                 genPart->momentum().z());
    //!< get the charge via the particle table ...
    int barcode = HepMC::barcode(genPart);
    int pdgCode = genPart->pdg_id();
    int absPdgCode = std::abs(pdgCode);
    // get the charge: ap->charge() is used later, DOES NOT WORK RIGHT NOW
    const HepPDT::ParticleData* ap =
        m_particleDataTable->particle( absPdgCode);
    double charge = 1.;
    if (ap) charge = ap->charge();
    // since the PDT table only has abs(PID) values for the charge
    charge *= (pdgCode > 0.) ?  1. : -1.;

    // ----------------------- get teh PRDS and start 
    const std::vector<const Trk::PrepRawData*> & clusters = prdTraj.prds;
    // nominal 0,0,0 position for track fit seeding
    Trk::PerigeeSurface persurf;
    Trk::CurvilinearParameters startParams(startPos,startMom,charge);
    //minimal conversion; ideally the extrapolator would return a unique_ptr
    auto per = std::unique_ptr<const Trk::TrackParameters>(
      m_extrapolator->extrapolate(ctx,
                                  startParams,
                                  persurf,
                                  Trk::anyDirection,
                                  false,
                                  Trk::nonInteracting));
    if (!per) {
      ATH_MSG_DEBUG("Perigee creation for genParticle start position failed. "
                    "Skipping track creation.");
      return nullptr;
    }
    // first TrackStateOnSurface is the Perigee 
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    
   const Trk::TrackStateOnSurface *pertsos=new Trk::TrackStateOnSurface(nullptr,std::move(per),nullptr,nullptr,typePattern);
   auto traj = DataVector<const Trk::TrackStateOnSurface>();
   traj.push_back(pertsos);
   
   
   
   std::unique_ptr<const Trk::TrackParameters> prevpar(startParams.uniqueClone());
   // First create a Trk::Track object 'traj' that will go into the fitter for refitting
   int i=0;
   for ( ;i<(int)clusters.size();i++){
        if (m_DetID->is_trt(clusters[i]->identify())) break;
        const Trk::Surface &surf=clusters[i]->detectorElement()->surface(clusters[i]->identify());	
        if (surf==prevpar->associatedSurface()) continue;
        bool ispixel=false;
        if (m_DetID->is_pixel(clusters[i]->identify())) ispixel=true;

        auto thispar = std::unique_ptr<const Trk::TrackParameters>(
          m_extrapolator->extrapolate(ctx,
                                      *prevpar,
                                      surf,
                                      Trk::alongMomentum,
                                      false,
                                      Trk::nonInteracting));
        if (!thispar)
          break;
        if (!surf.insideBounds(thispar->localPosition(),20*Gaudi::Units::mm,50*Gaudi::Units::mm)) {
          continue;
        }
        AmgVector(5) params=thispar->parameters();
        params[Trk::loc1]=clusters[i]->localPosition().x();
        if (ispixel) params[Trk::loc2]=clusters[i]->localPosition().y();
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
        std::unique_ptr<const Trk::RIO_OnTrack> rot{m_rotcreator->correct(*clusters[i],*thispar)};        
        if (!rot) {
          continue;
        }
        // create the ROTs for the reference trajectory
        const Trk::TrackStateOnSurface *tsos=new Trk::TrackStateOnSurface(std::move(rot),thispar->uniqueClone(),nullptr,nullptr,typePattern);
        traj.push_back(tsos);
        prevpar=std::move(thispar);
   }
   // this is the reference trajectory to be refitted  
   Trk::TrackInfo info;
   Trk::Track track(info, std::move(traj), nullptr);
   if (/* ndof<0 */ (track.measurementsOnTrack()->size() < m_minSiHits &&
                     std::abs(genPart->momentum().eta()) <= m_forwardBoundary) ||
       (track.measurementsOnTrack()->size() < m_minSiHitsForward &&
        std::abs(genPart->momentum().eta()) > m_forwardBoundary) ||
       (m_onlyPrimaries && barcode >= m_primaryBarcodeCutOff)) {
     ATH_MSG_VERBOSE(
       "Track does not fulfill requirements for refitting. Skipping it.");
     return nullptr;
   }
   // choose the material effects
   //!< @todo : if we need a dedicated electron fitter is has to go in here !
   
   // create the refitted track
   //
   //
   //
   Trk::ParticleSwitcher particleSwitch;
   Trk::ParticleHypothesis materialInteractions = particleSwitch.particle[m_matEffects];

   Trk::Track *refittedtrack=(m_trackFitter->fit(Gaudi::Hive::currentContext(),track,false,materialInteractions)).release();
  
   //!<  Refit a second time to add TRT hits
   Trk::Track *refittedtrack2=nullptr;
   if (refittedtrack && (int)clusters.size()-i>=9){
     Trk::MeasurementSet measset;
     std::unique_ptr<const Trk::TrackParameters> prevpar(refittedtrack->trackParameters()->back()->uniqueClone());
     for (;i<(int)clusters.size();i++) {
       const Trk::Surface *surf=&clusters[i]->detectorElement()->surface(clusters[i]->identify());
       std::unique_ptr<const Trk::TrackParameters> thispar(m_extrapolator->extrapolate(ctx,*prevpar,*surf,Trk::alongMomentum,false,Trk::nonInteracting));
       if (!thispar) break;
       const Trk::RIO_OnTrack *rot=m_rotcreatorbroad->correct(*clusters[i],*thispar);
       if (rot) measset.push_back(rot);
       prevpar=std::move(thispar);
     }
     refittedtrack2=(m_trackFitter->fit(Gaudi::Hive::currentContext(),*refittedtrack,measset,false,materialInteractions)).release();
     if (!refittedtrack2){
       auto traj2 = DataVector<const Trk::TrackStateOnSurface>();
       for (int j=0;j<(int)refittedtrack->trackStateOnSurfaces()->size();j++) traj2.push_back(new Trk::TrackStateOnSurface(*(*refittedtrack->trackStateOnSurfaces())[j]));
       std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern2;
       typePattern2.set(Trk::TrackStateOnSurface::Outlier);
       //measset needs to be unique_ptr before progress further
       for (int j=0;j<(int)measset.size();j++) traj2.push_back(new Trk::TrackStateOnSurface( measset[j],nullptr,nullptr,nullptr,typePattern2));
       refittedtrack2=new Trk::Track(refittedtrack->info(),std::move(traj2),refittedtrack->fitQuality()->clone());
     }
     else for (int j=0;j<(int)measset.size();j++) delete measset[j];
   } else if(!refittedtrack){
    ATH_MSG_VERBOSE("Track fit of truth trajectory NOT successful, NO track created. ");
    return nullptr;
   }

   if (refittedtrack2) delete refittedtrack;
   if (!refittedtrack2 && refittedtrack) refittedtrack2=refittedtrack;

   //         
   ATH_MSG_DEBUG("Track fit of truth trajectory successful, track created. ");
   // return what you have
   // Before returning, fix the creator
   refittedtrack2->info().setPatternRecognitionInfo( Trk::TrackInfo::Pseudotracking);

   return refittedtrack2;
}
