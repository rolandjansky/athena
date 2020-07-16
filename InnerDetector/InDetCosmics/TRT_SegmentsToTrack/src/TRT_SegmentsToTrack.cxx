/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//======================================================
//
// Algorithm for creating Tracks out of TrackSegments
// by feeding them through a TrackFitter
//
// Author: Christian Schmitt <Christian.Schmitt@cern.ch>
//
//=======================================================

#include <fstream>
#include <iostream>
#include <sstream>


#include "AthContainers/DataVector.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkSegment/TrackSegment.h"
#include "TrkParameters/TrackParameters.h"

#include "InDetIdentifier/TRT_ID.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"

// Geometry Stuff
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TRT_SegmentsToTrack/TRT_SegmentsToTrack.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"




InDet::TRT_SegmentsToTrack::TRT_SegmentsToTrack(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_idHelper(nullptr),
  m_trtid(nullptr),
  m_nTracksReal(0),
  m_nTracksFake(0),
  m_noiseratio(0.),
  m_events(0),
  m_n_combined_fit(0)
{
  declareProperty("NoiseCut",                m_noiseCut                    = -1.);
  declareProperty("MinNHit",                 m_minTRTHits                  =  1);
  declareProperty("MaterialEffects",         m_materialEffects             = false);
  declareProperty("OutlierRemoval",          m_outlierRemoval              = false);
  declareProperty("CombineSegments",         m_combineSegments             = false);
}

InDet::TRT_SegmentsToTrack::~TRT_SegmentsToTrack()
{
}

StatusCode InDet::TRT_SegmentsToTrack::initialize()
{
 
  ATH_MSG_DEBUG("TrkSegmenttoTrack initialize()");
  
  m_n_combined_fit=0;

  m_nTracksReal=0;
  m_nTracksFake=0;
  m_events=0;
  m_noiseratio=0.;

  // Initialize handle keys
  ATH_CHECK(m_inputSegmentCollectionName.initialize());
  ATH_CHECK(m_multiTruthCollectionTRTName.initialize());
  ATH_CHECK(m_barrelSegments.initialize());
  ATH_CHECK(m_endcapSegments.initialize());
  ATH_CHECK(m_outputTrackCollectionName.initialize());
  ATH_CHECK(m_BECCollectionName.initialize());

  //--------- Set up fitter -------------
  CHECK( m_trackFitter.retrieve());

  CHECK(detStore()->retrieve(m_trtid));

  CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));

  CHECK(m_extrapolator.retrieve());
  ATH_CHECK(m_trkSummaryTool.retrieve( DisableTool{ m_trkSummaryTool.empty() } ));
  ATH_CHECK(m_assoTool.retrieve( DisableTool{ m_assoTool.empty() || (m_trkSummaryTool.empty() && m_assoMapName.key().empty()) } ));
  ATH_CHECK(m_inputAssoMapName.initialize( !m_inputAssoMapName.key().empty() && m_assoTool.isEnabled()));
  ATH_CHECK(m_assoMapName.initialize( !m_assoMapName.key().empty() && m_assoTool.isEnabled()));
  return StatusCode::SUCCESS;
}

StatusCode InDet::TRT_SegmentsToTrack::finalize()
{
  ATH_MSG_INFO("Summary of" << m_events << " Events");
  ATH_MSG_INFO("Found Real Tracks : " << m_nTracksReal);
  ATH_MSG_INFO("Found Fake Tracks : " << m_nTracksFake);

  if(m_nTracksReal>0) {
    ATH_MSG_INFO("Average noise percentage " << m_noiseratio/double(m_nTracksReal));
  }

  for (const auto& mitr : m_MapReal) {
    ATH_MSG_INFO("Real tracks with " << mitr.first << " hits: " << mitr.second);
  }
  for (const auto& mitr : m_MapFake) {
    ATH_MSG_INFO("Fake tracks with " << mitr.first << " hits: " << mitr.second);
  }

  if(m_combineSegments){
    ATH_MSG_INFO("Number of combined Barrel+Endcap tracks: "<<m_n_combined_fit);
  }

  ATH_MSG_DEBUG(name() << " finalize() successful ");

  return StatusCode::SUCCESS;
}

StatusCode InDet::TRT_SegmentsToTrack::execute()
{
  int segmentCounter=0;
  const EventContext& ctx = Gaudi::Hive::currentContext();
  m_events++;

  ATH_MSG_DEBUG(name() << " execute() start");

  StatusCode sc                          = StatusCode::SUCCESS;

  //combine segments
  if( m_combineSegments)
    combineSegments(ctx);

  // input TrackSegment Collection

  SG::ReadHandle<Trk::SegmentCollection> inputSegments(m_inputSegmentCollectionName,ctx);
  if (!inputSegments.isValid()) {
    ATH_MSG_ERROR("Could not open: " <<m_inputSegmentCollectionName.key());
    sc =  StatusCode::FAILURE;
    return sc;
  }


  //output collections for fitted tracks
  
  SG::WriteHandle<TrackCollection> final_outputTrackCollection(m_outputTrackCollectionName,ctx);
  ATH_CHECK(final_outputTrackCollection.record(std::make_unique<TrackCollection>()));

  std::vector<std::unique_ptr<Trk::Track> > output_track_collection;
  //try to get truth information

  SG::ReadHandle<PRD_MultiTruthCollection> truthCollectionTRT(m_multiTruthCollectionTRTName,ctx);

  if (!truthCollectionTRT.isValid()){
    ATH_MSG_INFO("Could not open PRD_MultiTruthCollection : " <<  m_multiTruthCollectionTRTName.key());
    sc=StatusCode::SUCCESS; // not having truth information is not a failure
  }



  Trk::SegmentCollection::const_iterator iseg = inputSegments->begin();
  Trk::SegmentCollection::const_iterator isegEnd = inputSegments->end();



 for(iseg=inputSegments->begin(); iseg != isegEnd; ++ iseg) {
    
    segmentCounter++;
    
    
    if((*iseg)->numberOfMeasurementBases()<10) continue; //only convert segments that are large enough
    
    Trk::MeasurementSet myset;
    for(unsigned int i=0;i<(*iseg)->numberOfMeasurementBases();++i){
      const Amg::VectorX& LocalParameters = (*iseg)->measurement(i)->localParameters();
      const Amg::MatrixX& LocalErrorMatrix = (*iseg)->measurement(i)->localCovariance();
      double z=(*iseg)->measurement(i)->globalPosition().z();
      ATH_MSG_DEBUG("Segment "<<segmentCounter<<" rioOnTrack "<<i<<" (z="<<z<<") : "<<LocalParameters[0]
				   <<" ; "<<sqrt(LocalErrorMatrix(Trk::locR,Trk::locR)));
      myset.push_back((*iseg)->measurement(i));
    } //end of loop over measurements

    if((*iseg)->numberOfMeasurementBases()>0){
      ATH_MSG_DEBUG("numberOfContainedRots: " << (*iseg)->numberOfMeasurementBases());
      
      const Trk::StraightLineSurface* testSf
      	= dynamic_cast<const Trk::StraightLineSurface*>(&((*iseg)->associatedSurface()));
      
      
      const Trk::AtaStraightLine* inputMatchLine =0;
      const Trk::Perigee* inputMatchPerigee =0;
      const Amg::VectorX &p = dynamic_cast<const Amg::VectorX&>((**iseg).localParameters());      
      
      if(!testSf){
	ATH_MSG_DEBUG("No straightLineSurface !! Trying Perigee ...");
	const Trk::PerigeeSurface *testPSf=dynamic_cast<const Trk::PerigeeSurface*>(&((*iseg)->associatedSurface()));
	
	if(!testPSf){
	  ATH_MSG_DEBUG("Associated surface dynamic_cast into PerigeeSurface failed: "<<(*iseg)->associatedSurface());
	  ATH_MSG_DEBUG("Leaving input matching perigee as nullptr, will not get a fittedTrack");
	}else{
	  ATH_MSG_DEBUG("Ok, it seems to be a PerigeeSurface");
	  inputMatchPerigee = new Trk::Perigee(p(0),p(1),p(2),p(3),p(4), *testPSf);
	}
      }else{
	inputMatchLine = new Trk::AtaStraightLine(p(0),p(1),p(2),p(3),p(4),*testSf);   
       
	ATH_MSG_DEBUG("Created testSf : " << (*inputMatchLine));
        int nmeas=(*iseg)->numberOfMeasurementBases();
        Amg::Vector3D surfpos(.5*((*iseg)->measurement(nmeas/2)->globalPosition()+(*iseg)->measurement(nmeas/2+1)->globalPosition()));
        Trk::PerigeeSurface persurf(surfpos);
        inputMatchPerigee = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(ctx,*inputMatchLine,persurf));
      }
      ATH_MSG_DEBUG("Created inputMatchLine");
      
      std::unique_ptr<Trk::Track> fittedTrack;
      const Trk::TrackParameters *inputpar=0;
      if (inputMatchPerigee) inputpar=inputMatchPerigee;
      else if (inputMatchLine) inputpar=inputMatchLine;

      if (inputpar) {
         fittedTrack = m_trackFitter->fit(ctx,
                                          myset,
                                          *inputpar,
                                          m_outlierRemoval,
                                          m_materialEffects ? Trk::muon : Trk::nonInteracting);
      }
      if(fittedTrack){
        DataVector<const Trk::TrackStateOnSurface>::const_iterator itSet = fittedTrack->trackStateOnSurfaces()->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator itSetEnd = fittedTrack->trackStateOnSurfaces()->end();
        const Trk::TrackParameters *measpar=0;
        double mindist=9999;
        for ( ; itSet!=itSetEnd; ++itSet) {
          if ((**itSet).type(Trk::TrackStateOnSurface::Measurement) && (**itSet).trackParameters()->position().perp()<mindist) {
            mindist=(**itSet).trackParameters()->position().perp();
            measpar=(**itSet).trackParameters();
          }
        }
        const Trk::TrackParameters *myper{};
        if (measpar){
          myper=m_extrapolator->extrapolate(ctx,*measpar,Trk::PerigeeSurface(),Trk::anyDirection,false, m_materialEffects ? Trk::muon : Trk::nonInteracting);
        }
        if (!myper){
          delete myper;
          fittedTrack.reset();
        }
        else {
          DataVector<const Trk::TrackStateOnSurface>* trajectory = new DataVector<const Trk::TrackStateOnSurface>;
          itSet = fittedTrack->trackStateOnSurfaces()->begin();
          for ( ; itSet!=itSetEnd; ++itSet) {
            if (!(**itSet).type(Trk::TrackStateOnSurface::Perigee)) {
              const Trk::TrackParameters *trackpar=(**itSet).trackParameters() ? (**itSet).trackParameters()->clone() : 0;
              const Trk::MeasurementBase *measurement=(**itSet).measurementOnTrack() ? (**itSet).measurementOnTrack()->clone() : 0;
              const Trk::FitQuality *fitQual=(**itSet).fitQualityOnSurface() ? (**itSet).fitQualityOnSurface()->clone() : 0;
              const Trk::MaterialEffectsBase *mateff=(**itSet).materialEffectsOnTrack() ? (**itSet).materialEffectsOnTrack()->clone() : 0;
              std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
              if ((**itSet).type(Trk::TrackStateOnSurface::Measurement)) typePattern.set(Trk::TrackStateOnSurface::Measurement);
              else if ((**itSet).type(Trk::TrackStateOnSurface::Outlier)) typePattern.set(Trk::TrackStateOnSurface::Outlier);
              else if ((**itSet).type(Trk::TrackStateOnSurface::Scatterer)) typePattern.set(Trk::TrackStateOnSurface::Scatterer);
              else if ((**itSet).type(Trk::TrackStateOnSurface::BremPoint)) typePattern.set(Trk::TrackStateOnSurface::BremPoint);
              trajectory->push_back(new Trk::TrackStateOnSurface(measurement, trackpar, fitQual, mateff, typePattern));

            }
          }
          bool peradded=false;
          itSet = trajectory->begin()+1;
          itSetEnd = trajectory->end();
	  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
	  typePattern.set(Trk::TrackStateOnSurface::Perigee);
          const Trk::TrackStateOnSurface *pertsos=new Trk::TrackStateOnSurface(0,myper,0,0,typePattern);

	  int index=1;
          for ( ; itSet!=itSetEnd; ++itSet) {
            double inprod1=((**itSet).trackParameters()->position()-myper->position()).dot(myper->momentum());
            itSet--;
            double inprod2=((**itSet).trackParameters()->position()-myper->position()).dot(myper->momentum());
            itSet++;
            if (inprod1>0 && inprod2<0) {
              trajectory->insert(trajectory->begin()+index,pertsos);
              peradded=true;
              break;
            }
	    index++;
          }
          if (!peradded){
            itSet = trajectory->begin();
            double inprod=((**itSet).trackParameters()->position()-myper->position()).dot(myper->momentum());
            if (inprod>0) trajectory->insert(trajectory->begin(),pertsos);
            else trajectory->push_back(pertsos);
          }
          std::unique_ptr<Trk::Track> track=std::make_unique<Trk::Track>(fittedTrack->info(),trajectory,fittedTrack->fitQuality()->clone());
          fittedTrack = std::move(track);
        }
      }
      if(fittedTrack){

	int nHT=nHTHits(fittedTrack.get());
	ATH_MSG_DEBUG("Number of HT hits: "<<nHT);
	ATH_MSG_DEBUG(" Successful fit of track.");
	if ((*iseg)->fitQuality()) ATH_MSG_DEBUG("Quality of Segment: chi^2 /ndof "<<(*iseg)->fitQuality()->chiSquared()<<" / "<<(*iseg)->fitQuality()->numberDoF());
	ATH_MSG_DEBUG("Quality of Track:  chi^2 /ndof "<<fittedTrack->fitQuality()->chiSquared()<<" / "<<fittedTrack->fitQuality()->numberDoF());
	ATH_MSG_DEBUG("Noise probability: "<<getNoiseProbability(fittedTrack.get()));
	ATH_MSG_DEBUG((*fittedTrack));
	
	if( nTRTHits(fittedTrack.get())>=m_minTRTHits){
          if(getNoiseProbability(fittedTrack.get())>m_noiseCut){
            double truefraction=getRealFractionTRT(fittedTrack.get(),ctx);

	    int nhits=(*iseg)->numberOfMeasurementBases();
	    ATH_MSG_DEBUG("Real/Noise : "<< truefraction << " chi2="<<fittedTrack->fitQuality()->chiSquared()/double(fittedTrack->fitQuality()->numberDoF()));
	    if(truefraction>0.5){
	      ATH_MSG_DEBUG("==> Real Track");

	      if(m_MapReal.find(nhits)==m_MapReal.end()){
		m_MapReal[nhits]=1;
	      }else{
		m_MapReal[nhits]++;
	      }

	      m_nTracksReal++;
	      m_noiseratio+=(1-truefraction);
	    }else{
	      ATH_MSG_DEBUG(" ==> Fake Track");

	      if(m_MapFake.find(nhits)==m_MapFake.end()){
		m_MapFake[nhits]=1;
	      }else{
		m_MapFake[nhits]++;
	      }

	      m_nTracksFake++;
	    }
	  }
	  output_track_collection.push_back(std::move(fittedTrack));
	}else{
          ATH_MSG_DEBUG("This tracks has only "<<nTRTHits(fittedTrack.get())<<" Hits! Will reject it!");
	}
        
      }else{
	ATH_MSG_DEBUG("Fit did not converge!");
      } //end of if(fittedTrack)

      delete inputMatchLine;
      delete inputMatchPerigee;
      
    } //end of if: (*iseg)->numberOfMeasurementBases()>0
  } //end of loop over segments

  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map;
  if (m_assoTool.isEnabled()) {
     // create internal PRD-to-track map
     prd_to_track_map = m_assoTool->createPRDtoTrackMap();
     if (!m_inputAssoMapName.key().empty()) {
        SG::ReadHandle<Trk::PRDtoTrackMap> input_asso_map(m_inputAssoMapName,ctx);
        if (!input_asso_map.isValid()) {
           ATH_MSG_FATAL("No valid input PRT-to-track map with key " << m_inputAssoMapName.key());
        }
        *prd_to_track_map = *input_asso_map;
        ATH_MSG_INFO("PRD to track map input " << m_inputAssoMapName.key()  );
     }
     for (const std::unique_ptr<Trk::Track> &track : output_track_collection) {
        StatusCode sc = m_assoTool->addPRDs(*prd_to_track_map, *track);
     }
  }
  // @TODO sort output track collection ? 
  final_outputTrackCollection->reserve(output_track_collection.size());
  if (m_trkSummaryTool.isEnabled()) {
     for (std::unique_ptr<Trk::Track> &track : output_track_collection) {
        m_trkSummaryTool->computeAndReplaceTrackSummary(*track,prd_to_track_map.get());
        final_outputTrackCollection->push_back(std::move(track));
     }
  }
  else {
     for (std::unique_ptr<Trk::Track> &track : output_track_collection) {
        final_outputTrackCollection->push_back(std::move(track));
     }
  }
  if (!m_assoMapName.key().empty()) {
     ATH_CHECK(SG::WriteHandle<Trk::PRDtoTrackMap>(m_assoMapName,ctx).record(std::move(prd_to_track_map)));
  }

  if (!final_outputTrackCollection.isValid()) {
     // @TODO never reached (?)
    ATH_MSG_ERROR("Could not write track collection " << m_outputTrackCollectionName.key());
    return StatusCode::FAILURE;
    //return sc;
  }

  return sc;
}

int InDet::TRT_SegmentsToTrack::getNumberReal(const InDet::TRT_DriftCircle* driftcircle,const EventContext& ctx) const
{
  int numBarcodes = 0;
  typedef PRD_MultiTruthCollection::const_iterator iter;

  SG::ReadHandle<PRD_MultiTruthCollection> truthCollectionTRT(m_multiTruthCollectionTRTName,ctx);

  if(!truthCollectionTRT.isValid()) return 0;
  std::pair<iter,iter> range = truthCollectionTRT->equal_range(driftcircle->identify());
  for(iter i = range.first; i != range.second; i++){
    numBarcodes++;
  }
  return numBarcodes;
}



double InDet::TRT_SegmentsToTrack::getRealFractionTRT(const Trk::Track *track,const EventContext& ctx) const
{
  double fraction=0.;
  int nDriftReal=0;
  int nDriftNoise=0;

  //loop over the hits
  for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {

    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(tsos->measurementOnTrack());
    if (hitOnTrack) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID)){

	const InDet::TRT_DriftCircleOnTrack *dcot= dynamic_cast <const InDet::TRT_DriftCircleOnTrack*>(hitOnTrack);
	if(dcot) {
	  const InDet::TRT_DriftCircle *dc=dcot->prepRawData();

	  if(dc){
            int nreal=getNumberReal(dc,ctx);
    
	    if(nreal>0) nDriftReal++;
	    else nDriftNoise++;
  
	  }
	}
      }
    }
  }

  if(nDriftReal>0)
    fraction=double(nDriftReal)/double(nDriftNoise+nDriftReal);

  return fraction;
}


int InDet::TRT_SegmentsToTrack::nHTHits(const Trk::Track *track) const
{

  int nHT=0;
  int nLT=0;

  //loop over the hits
  for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {
    
    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(tsos->measurementOnTrack());
    if (hitOnTrack != 0) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID) && !tsos->type(Trk::TrackStateOnSurface::Outlier)){
        const InDet::TRT_DriftCircleOnTrack* trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(hitOnTrack);
        if(trtcirc) {
          //          const InDet::TRT_DriftCircle* rawhit = trtcirc->prepRawData();
          if(trtcirc->highLevel()){
            nHT++;
          }else{
            nLT++;
          }
        }
      }
    }
  }

  return nHT;
}



int InDet::TRT_SegmentsToTrack::nTRTHits(const Trk::Track *track) const
{
  int nhits=0;

  //loop over the hits
  for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {
    
    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(tsos->measurementOnTrack());
    if (hitOnTrack != 0) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID) && !tsos->type(Trk::TrackStateOnSurface::Outlier)){
	nhits++;
      }
    }
  }

  return nhits;
}


double InDet::TRT_SegmentsToTrack::getNoiseProbability(const Trk::Track *track) const
{
  double fraction=0.;
  int nDriftReal=0;
  int nDriftNoise=0;

  //loop over the hits
  for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {
    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>(tsos->measurementOnTrack());
    if (hitOnTrack) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID)){

	const InDet::TRT_DriftCircleOnTrack *dcot= dynamic_cast <const InDet::TRT_DriftCircleOnTrack*>(hitOnTrack);
	if (dcot) {
	  const InDet::TRT_DriftCircle *dc=dcot->prepRawData();
  
	  if(dc){
    
	    if(dc->isNoise())
	      nDriftNoise++;
	    else
	      nDriftReal++;
	  }
	}
      }
    }
  }

  if(nDriftReal>0)
    fraction=double(nDriftReal)/double(nDriftNoise+nDriftReal);

  return fraction;
}



void InDet::TRT_SegmentsToTrack::combineSegments(const EventContext& ctx) const
{
  //Idea: 
  // - get from endcap segment z-phi dependence
  // - get from barrel segment r-phi dependence and z dependence from endcap segment fit

  int n_combined_fit=0;
  SG::ReadHandle<Trk::SegmentCollection> BarrelSegments(m_barrelSegments,ctx);
  SG::ReadHandle<Trk::SegmentCollection> EndcapSegments(m_endcapSegments,ctx);

  StatusCode sc;

  SG::WriteHandle<TrackCollection> outputCombiCollection(m_BECCollectionName,ctx);
  sc = outputCombiCollection.record(std::make_unique<TrackCollection>());
  if (sc.isFailure()) return;

  if (!BarrelSegments.isValid()){
    ATH_MSG_FATAL("Could not open barrel segments collection : " <<   m_barrelSegments.key());
    return;
  }

  if (!EndcapSegments.isValid()) {
    ATH_MSG_FATAL("Could not open endcap segments collection : " <<   m_endcapSegments.key());
    return;
  }

  ATH_MSG_VERBOSE("Got both barrel and endcap segment collections of size "<<BarrelSegments->size()<<" "<<EndcapSegments->size());
  
  Trk::SegmentCollection::const_iterator iseg = BarrelSegments->begin();
  Trk::SegmentCollection::const_iterator isegE = BarrelSegments->end();

  int scount=0;
  for(;iseg!=isegE;++iseg,scount++){
    ATH_MSG_VERBOSE("Barrel Segment "<<scount<<" : phi="<<(*iseg)->localParameters()[Trk::phi]);
  }

  iseg  = EndcapSegments->begin();
  isegE = EndcapSegments->end();

  scount=0;
  for(;iseg!=isegE;++iseg,scount++){
    ATH_MSG_VERBOSE("Endcap Segment "<<scount<<" : phi="<<(*iseg)->localParameters()[Trk::phi]);
  }

  if(BarrelSegments->size()==1 && EndcapSegments->size()==1){
    ATH_MSG_VERBOSE("Here we go: one barrel segment and one endcap segment!");

    Trk::SegmentCollection::const_iterator isegBarrel = BarrelSegments->begin();
    Trk::SegmentCollection::const_iterator isegBarrelE = BarrelSegments->end();

    Trk::SegmentCollection::const_iterator isegEndcap = EndcapSegments->begin();
    Trk::SegmentCollection::const_iterator isegEndcapE = EndcapSegments->end();

    //loop over all Barrel Segments
    for(;isegBarrel!=isegBarrelE;++isegBarrel){
      for(;isegEndcap!=isegEndcapE;++isegEndcap){
	
	std::vector< Trk::PseudoMeasurementOnTrack*> tobedeleted;

	ATH_MSG_VERBOSE("Barrel Segment : phi="<<(*isegBarrel)->localParameters()[Trk::phi]<<" with "<<(*isegBarrel)->numberOfMeasurementBases()<<" hits");
	ATH_MSG_VERBOSE("Endcap Segment : phi="<<(*isegEndcap)->localParameters()[Trk::phi]<<" with "<<(*isegEndcap)->numberOfMeasurementBases()<<" hits");
	
	int count=0;
	Trk::MeasurementSet myset,myset2,echits=(*isegEndcap)->containedMeasurements();

        bool barreldown=((*isegBarrel)->measurement((*isegBarrel)->numberOfMeasurementBases()/2)->globalPosition().y()<0);	
        double theta=0;
        if((*isegEndcap)->localParameters().contains(Trk::theta)){
          theta = (*isegEndcap)->localParameters()[Trk::theta];
        }
              
        //correct theta to point towards the barrel
        if(( (*isegEndcap)->globalPosition().y()>0 && (*isegEndcap)->globalPosition().z()>0) ||
           ((*isegEndcap)->globalPosition().y()<0 && (*isegEndcap)->globalPosition().z()<0)
           ){
          //endcap A top pointing to barrel A bottom or Barrel C top pointing to Endcap C bottom
          //--> Theta between 90 and 180 degrees
          if(theta<M_PI/2.) {
            theta=M_PI-theta;
            std::reverse(echits.begin(),echits.end());
          }   
        }else{
          //Theta between 0 and 90 degrees   
          if(theta>M_PI/2.) {
            theta=M_PI-theta;
            std::reverse(echits.begin(),echits.end());
          }
        }

	//decide if barrel or endcap hits come first depending on global position of segments

	if(!barreldown && (*isegEndcap)->globalPosition().y()>0){
	  ATH_MSG_VERBOSE("Both barrel and endcap segments in top sectors, won't fit");
	  continue;
	}

	if(barreldown && (*isegEndcap)->globalPosition().y()<0){
	  ATH_MSG_VERBOSE("Both barrel and endcap segments in lower sectors, won't fit");
	  continue;
	}
        int firstechit=0,lastechit=0;
	if((*isegEndcap)->globalPosition().y()>0){
	  //first endcap then barrel
	  for(int i=0;i<(int)echits.size();++i){ 
            const Trk::MeasurementBase *meas=echits[i];
	    const Amg::VectorX LocalParameters = meas->localParameters();
	    const Amg::MatrixX LocalErrorMatrix = meas->localCovariance();
	    double z=meas->globalPosition().z();
	    double phi=meas->globalPosition().phi();
	    double r=meas->globalPosition().perp();
	    
	    const InDet::TRT_DriftCircleOnTrack* trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(meas);
	    if(trtcirc){
	      myset.push_back(meas);
	      ATH_MSG_VERBOSE("Endcap : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) ");
	      count++;
	    }
	  }
	  firstechit=0;
          lastechit=count-1;

	  for(unsigned int i=0;i<(*isegBarrel)->numberOfMeasurementBases();++i){
	    const Amg::VectorX LocalParameters = (*isegBarrel)->measurement(i)->localParameters();
	    const Amg::MatrixX LocalErrorMatrix = (*isegBarrel)->measurement(i)->localCovariance();
	    double z=(*isegBarrel)->measurement(i)->globalPosition().z();
	    double phi=(*isegBarrel)->measurement(i)->globalPosition().phi();
	    double r=(*isegBarrel)->measurement(i)->globalPosition().perp();
	  
	    const Trk::MeasurementBase *mesb=(*isegBarrel)->measurement(i);
	    const InDet::TRT_DriftCircleOnTrack* trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(mesb);
	    if(trtcirc){
	      myset.push_back((*isegBarrel)->measurement(i));
	      ATH_MSG_VERBOSE("Barrel : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) ");
	      count++;
	    }
	    
	  }
	}else{
	  //first barrel then endcap
	  for(unsigned int i=0;i<(*isegBarrel)->numberOfMeasurementBases();++i){
	    const Amg::VectorX LocalParameters = (*isegBarrel)->measurement(i)->localParameters();
	    const Amg::MatrixX LocalErrorMatrix = (*isegBarrel)->measurement(i)->localCovariance();
	    double z=(*isegBarrel)->measurement(i)->globalPosition().z();
	    double phi=(*isegBarrel)->measurement(i)->globalPosition().phi();
	    double r=(*isegBarrel)->measurement(i)->globalPosition().perp();
	  
	    const Trk::MeasurementBase *mesb=(*isegBarrel)->measurement(i);
	    const InDet::TRT_DriftCircleOnTrack* trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(mesb);
	    if(trtcirc){
	      myset.push_back((*isegBarrel)->measurement(i));
	      ATH_MSG_VERBOSE("Barrel : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) ");
	      count++;
	    }
	    
	  }
          firstechit=count;

	  for(int i=0;i<(int)echits.size();++i){
            const Trk::MeasurementBase *meas=echits[i];
	    const Amg::VectorX LocalParameters = meas->localParameters();
	    const Amg::MatrixX LocalErrorMatrix = meas->localCovariance();
	    double z=meas->globalPosition().z();
	    double phi=meas->globalPosition().phi();
	    double r=meas->globalPosition().perp();
	    
	    const InDet::TRT_DriftCircleOnTrack* trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(meas);
	    if(trtcirc){
	      myset.push_back(meas);
	      ATH_MSG_VERBOSE("Endcap : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) ");
	      count++;
	    }
	    
	  }
          lastechit=count-1;
	}
	
	
	
	Amg::Vector3D inputMatchingPos((*isegBarrel)->globalPosition());

	if((*isegEndcap)->globalPosition().y()>0)
	  inputMatchingPos=(*isegEndcap)->globalPosition();

	Amg::Vector3D inputMatchingMom;
	double p = 10e7;
	
	if((*isegBarrel)->localParameters().contains(Trk::qOverP)){
	  p = ((*isegBarrel)->localParameters()[Trk::qOverP]!=0.) ? fabs(1./((*isegBarrel)->localParameters()[Trk::qOverP])) : 10e7;
	}
	
	double phi=0.;
	if((*isegBarrel)->localParameters().contains(Trk::phi)){
	  phi = (*isegBarrel)->localParameters()[Trk::phi];
	}
	
	
	inputMatchingMom[Trk::px] = p * cos(phi) * sin(theta);
	inputMatchingMom[Trk::py] = p * sin(phi) * sin(theta);
	inputMatchingMom[Trk::pz] = p * cos(theta);
	
	
	ATH_MSG_VERBOSE("Global position: "<<inputMatchingPos<<" Globalmomentum: "<<inputMatchingMom);



	//add two pseudomeasurements at the beginning and at the end of the track
	Trk::MeasurementSet::const_iterator mit;
	Trk::MeasurementSet::const_iterator mitE=myset.end();

	int count2=0;
	for(mit=myset.begin();mit!=mitE;++mit){
	  myset2.push_back(*mit);
	  if(count2==firstechit || count2==lastechit){
	    const InDet::TRT_DriftCircleOnTrack* trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(*mit);
	    if(trtcirc){
	      const Trk::StraightLineSurface *line=dynamic_cast<const
		Trk::StraightLineSurface*>(&trtcirc->associatedSurface());
	      if (line) {
		//add pseudomeasurement
		double locz=-200.;
		if ((count2==firstechit && firstechit!=0) || (count2==lastechit && firstechit==0)) locz=200;
		Trk::DefinedParameter dp(locz,Trk::locZ);
		std::vector<Trk::DefinedParameter> defPar;
		defPar.push_back(dp);

		Trk::LocalParameters par(defPar);

		Amg::MatrixX cov(1,1);
		cov(0,0) = 16./12. ; // take actual length of straw instead !!!!!

		//create new surface
		Amg::Vector3D C = line->transform().translation();

		//decide on movement

		const Amg::Vector3D& gpos=(*mit)->globalPosition();

		if(fabs(gpos.z())>800){
		  if(theta>M_PI/2.){
		    C[2]-=1.;
		  }else{
		    C[2]+=1.;
		  }
		}else{
		  C[1]-=1.;
		}
      
      
		Amg::Transform3D* T    = new Amg::Transform3D();
		*T = line->transform().rotation();
		*T *= Amg::Translation3D(C.x(),C.y(),C.z());
		Trk::StraightLineSurface* surface = new Trk::StraightLineSurface(T);
     
		Trk::PseudoMeasurementOnTrack *pseudo=new Trk::PseudoMeasurementOnTrack( par,cov,*surface);
     
		tobedeleted.push_back(pseudo);

		delete surface;
      
		myset2.push_back(pseudo);
	      }
	    }
	  }
	  count2++;
	}

       
	
	const Trk::StraightLineSurface* testSf;
	if((*isegEndcap)->globalPosition().y()>0){
	  testSf= dynamic_cast<const Trk::StraightLineSurface*>(&((*isegEndcap)->associatedSurface()));
	}else{
	  testSf= dynamic_cast<const Trk::StraightLineSurface*>(&((*isegBarrel)->associatedSurface()));
	}
	
	const Trk::AtaStraightLine* inputMatchLine = nullptr;
	const Trk::Perigee* inputMatchPerigee = nullptr;
	
	if(!testSf){
	  ATH_MSG_VERBOSE("No straightLineSurface !! Trying Perigee ...");
	  
	  const Trk::PerigeeSurface *testPSf=dynamic_cast<const Trk::PerigeeSurface*>(&((*isegBarrel)->associatedSurface()));
	  
	  if(!testPSf){
	    ATH_MSG_VERBOSE("Associated surface dynamic_cast into PerigeeSurface failed. "<<(*isegBarrel)->associatedSurface());
	    ATH_MSG_VERBOSE("Leaving input matching perigee as nullptr, will not get a fittedTrack");
	  }else{
	    ATH_MSG_VERBOSE("Ok, it seems to be a PerigeeSurface");
	    inputMatchPerigee = new Trk::Perigee(inputMatchingPos,inputMatchingMom, 1., *testPSf);
	  }
	  
	}else{
	  inputMatchLine = new Trk::AtaStraightLine(inputMatchingPos,inputMatchingMom, 1., *testSf);          
	  ATH_MSG_VERBOSE("Created testSf : " << (*inputMatchLine));
	}

	Amg::Vector3D startMomentum( 0., 0., 1.);
        std::unique_ptr<Trk::Track> fittedTrack;
	if(m_materialEffects){
	  if(inputMatchLine){
            fittedTrack=m_trackFitter->fit(ctx,
                                           myset2,
					   *inputMatchLine,
					   false,
					   Trk::muon
					   );
	  }else if (inputMatchPerigee){
            fittedTrack=m_trackFitter->fit(ctx,
                                           myset2,
					   *inputMatchPerigee,
					   false,
					   Trk::muon
					   );
	  }
	}else{
	  if(inputMatchLine){
            fittedTrack=m_trackFitter->fit(ctx,
                                           myset2,
					   *inputMatchLine,
					   false,
					   Trk::nonInteracting
					   );
	  }else if (inputMatchPerigee){
            fittedTrack=m_trackFitter->fit(ctx,
                                           myset2,
					   *inputMatchPerigee,
					   false,
					   Trk::muon
					   );
	  }
	}
	
	if(fittedTrack){
	  n_combined_fit++;
	  outputCombiCollection->push_back(std::move(fittedTrack));
	  ATH_MSG_DEBUG("Successful Barrel+Endcap fit of segment. ");
	  ATH_MSG_DEBUG("Quality of Track:   "<<fittedTrack->fitQuality()->chiSquared()<<" / "<<fittedTrack->fitQuality()->numberDoF());
	  ATH_MSG_VERBOSE(*fittedTrack);
	}
	
	delete inputMatchPerigee;
	delete inputMatchLine;

	for(size_t i=0;i<tobedeleted.size();i++)
	  delete tobedeleted[i];
	tobedeleted.clear();
      }
    }
  }
  m_n_combined_fit += n_combined_fit;

  if (!outputCombiCollection.isValid()) {
    ATH_MSG_ERROR("Could not write track Barrel+EC collection TRT_Barrel_EC");
  }
}
