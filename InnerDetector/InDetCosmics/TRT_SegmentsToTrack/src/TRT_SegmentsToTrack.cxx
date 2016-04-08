/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


//#include "StoreGate/StoreGateSvc.h"
//#include "DataModel/DataVector.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkFitterInterfaces/ITrackFitter.h" 
#include "TrkSegment/SegmentCollection.h"
#include "TrkSegment/TrackSegment.h"
#include "TrkParameters/TrackParameters.h"

#include "InDetIdentifier/TRT_ID.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"

#include "TrkExInterfaces/IExtrapolator.h"

// Geometry Stuff
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TRT_SegmentsToTrack/TRT_SegmentsToTrack.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"




InDet::TRT_SegmentsToTrack::TRT_SegmentsToTrack(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_trackFitter("Trk::KalmanFitter/TrkKalmanFitter"),
  m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
  m_truthCollectionTRT(0)
{
  declareProperty("InputSegmentsCollection", m_inputSegmentCollectionName  = "TrackSegments");  
  declareProperty("OutputTrackCollection",   m_outputTrackCollectionName   =  "SegmentTracks");
  declareProperty("TrackFitter",             m_trackFitter);
  declareProperty("ExtrapolationTool",       m_extrapolator);
  declareProperty("PRDTruthCollectionTRT",   m_multiTruthCollectionTRTName = "PRD_MultiTruthTRT"); 
  declareProperty("NoiseCut",                m_noiseCut                    = -1.);
  declareProperty("MinNHit",                 m_minTRTHits                  =  1);
  declareProperty("MaterialEffects",         m_materialEffects             = false);
  declareProperty("OutlierRemoval",          m_outlierRemoval              = false);
  declareProperty("BarrelSegments",          m_barrelSegments              = "TRTBarrelSegments");
  declareProperty("EndcapSegments",          m_endcapSegments              = "TRTEndcapSegments");
  declareProperty("BarrelEndcapTracks",      m_BECCollectionName           = "TRT_Barrel_EC");
  declareProperty("CombineSegments",         m_combineSegments             = false);
  //to be deleted
  declareProperty("InputSCTCollection",m_dummy);
  declareProperty("OutputCombiCollection",m_dummy);
  declareProperty("CombineTracks",m_dummy_bool);

}

InDet::TRT_SegmentsToTrack::~TRT_SegmentsToTrack()
{
}

StatusCode InDet::TRT_SegmentsToTrack::initialize()
{
 
  msg(MSG::INFO) << "TrkSegmenttoTrack initialize()" << endreq;
  
  m_n_combined_fit=0;

  m_nTracksReal=0;
  m_nTracksFake=0;
  m_events=0;
  m_noiseratio=0.;

  StatusCode status = StatusCode::SUCCESS;
  
  //--------- Set up fitter -------------
  status = m_trackFitter.retrieve();
  if (status.isFailure())
    {
      msg(MSG::FATAL)  << "Could not find tool " << m_trackFitter 
	  << ". Exiting." << endreq;
      return status;
    }
  else{
    msg(MSG::DEBUG)  << " Got " << m_trackFitter << " as TrackFitter. " << endreq;
  }


  if ((detStore()->retrieve(m_trtid)).isFailure()) {
    msg(MSG::ERROR)  << "Problem retrieving TRTID helper" << endreq;
    return StatusCode::FAILURE;
  }
  


  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_extrapolator.retrieve().isFailure()) {  
     msg(MSG::FATAL) << "Could not get " << m_extrapolator.type() << endreq; 
     return StatusCode::FAILURE;  
   } 

  return status;

}

StatusCode InDet::TRT_SegmentsToTrack::finalize()
{

  msg(MSG::INFO) << "Summary of"<<m_events<<" Events"<<endreq;
  
  if(m_truthCollectionTRT){
    msg(MSG::INFO) << "Found Real Tracks : "<<m_nTracksReal<<endreq;
    msg(MSG::INFO) << "Found Fake Tracks : "<<m_nTracksFake<<endreq;
  }else{
    msg(MSG::INFO) << "Found Tracks : "<<m_nTracksFake<<endreq;
  }


  if(m_nTracksReal>0)
    msg(MSG::INFO) << "Average noise percentage " << m_noiseratio/double(m_nTracksReal)<<endreq;

  std::map<int,int>::iterator mit;
  
  if(m_truthCollectionTRT){
    for(mit=m_MapReal.begin();mit!=m_MapReal.end();++mit){
      int key=(*mit).first;
      int val=(*mit).second;
      
      msg(MSG::INFO) <<" Real tracks with "<<key<<" hits: "<<val<<endreq;
    }
  }

  for(mit=m_MapFake.begin();mit!=m_MapFake.end();++mit){
    int key=(*mit).first;
    int val=(*mit).second;
    
    if(m_truthCollectionTRT)
      msg(MSG::INFO) <<" Fake tracks with "<<key<<" hits: "<<val<<endreq;
    else
      msg(MSG::INFO) <<" Tracks with "<<key<<" hits: "<<val<<endreq;
  }


  if(m_combineSegments){
    msg(MSG::INFO)<<"Number of combined Barrel+Endcap tracks: "<<m_n_combined_fit<<endreq;
  }

  msg(MSG::INFO) << name() << " finalize() successful " << endreq;


  return StatusCode::SUCCESS;
}

StatusCode InDet::TRT_SegmentsToTrack::execute()
{
  int segmentCounter=0;

  m_events++;

  if (msgLvl(MSG::DEBUG)) msg() << name() << " execute() start" << endreq;

  StatusCode sc                          = StatusCode::SUCCESS;

  //combine segments
  if( m_combineSegments)
    combineSegments();



  // input TrackSegment Collection
  const Trk::SegmentCollection *inputSegments   = 0;


  //output collections for fitted tracks
  TrackCollection *outputTrackCollection   = new TrackCollection();

  sc = evtStore()->retrieve(inputSegments, m_inputSegmentCollectionName);
  if(sc.isFailure()){
    msg(MSG::FATAL) << " could not open segments collection : " << m_inputSegmentCollectionName << endreq;
    return sc;
  }


  //try to get truth information

  if(evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionTRTName)){

    sc = evtStore()->retrieve(m_truthCollectionTRT, m_multiTruthCollectionTRTName);
    if(sc.isFailure()){
      msg(MSG::INFO) << " could not open PRD_MultiTruthCollection : " <<  m_multiTruthCollectionTRTName << endreq;
      sc=StatusCode::SUCCESS; // not having truth information is not a failure
      m_truthCollectionTRT=0;
    }
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
      if (msgLvl(MSG::DEBUG)) msg()<<"Segment "<<segmentCounter<<" rioOnTrack "<<i<<" (z="<<z<<") : "<<LocalParameters[0]
				   <<" ; "<<sqrt(LocalErrorMatrix(Trk::locR,Trk::locR))<<endreq;
      myset.push_back((*iseg)->measurement(i));
    } //end of loop over measurements

    if((*iseg)->numberOfMeasurementBases()>0){
      if (msgLvl(MSG::DEBUG)) msg() << " numberOfContainedRots: " << (*iseg)->numberOfMeasurementBases() << endreq;
      
      const Trk::StraightLineSurface* testSf
      	= dynamic_cast<const Trk::StraightLineSurface*>(&((*iseg)->associatedSurface()));
      
      
      const Trk::AtaStraightLine* inputMatchingPar =0;
      const Trk::Perigee* inputMatchingPer =0;
      const Amg::VectorX &p = dynamic_cast<const Amg::VectorX&>((**iseg).localParameters());      
      
      if(!testSf){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"No straightLineSurface !! Trying Perigee ..."<<endreq;
	
	const Trk::PerigeeSurface *testPSf=dynamic_cast<const Trk::PerigeeSurface*>(&((*iseg)->associatedSurface()));
	
	if(!testPSf){
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Also PerigeeSurface failed? What is it? "<<(*iseg)->associatedSurface()<<endreq;
	}else{
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Ok, it seems to be a PerigeeSurface"<<endreq;            
	}
	
	
	inputMatchingPer = new Trk::Perigee(p(0),p(1),p(2),p(3),p(4), *testPSf);
	
      }else{
	
	inputMatchingPar = new Trk::AtaStraightLine(p(0),p(1),p(2),p(3),p(4),*testSf);   
       
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " created testSf : " << (*inputMatchingPar)<< endreq;

   
        int nmeas=(*iseg)->numberOfMeasurementBases();
        Amg::Vector3D surfpos(.5*((*iseg)->measurement(nmeas/2)->globalPosition()+(*iseg)->measurement(nmeas/2+1)->globalPosition()));
        Trk::PerigeeSurface persurf(surfpos);
        inputMatchingPer = dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(*inputMatchingPar,persurf));
       
      }
      
      
      
      
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " created inputMatchingPar " << endreq;
      
      Trk::Track *fittedTrack=0;
      const Trk::TrackParameters *inputpar=0;
      if (inputMatchingPer) inputpar=inputMatchingPer;
      else if (inputMatchingPar) inputpar=inputMatchingPar;

      if (inputpar) {
	
	fittedTrack=m_trackFitter->fit(myset,
                                         *inputpar,
					 m_outlierRemoval,
					 m_materialEffects ? Trk::muon : Trk::nonInteracting
					 );
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
        const Trk::TrackParameters *myper=m_extrapolator->extrapolate(*measpar,Trk::PerigeeSurface(),Trk::anyDirection,false, m_materialEffects ? Trk::muon : Trk::nonInteracting);

        if (!myper){
          delete myper;
          delete fittedTrack;
          fittedTrack=0;
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
          Trk::Track *track=new Trk::Track(fittedTrack->info(),trajectory,fittedTrack->fitQuality()->clone());
          delete fittedTrack;
          fittedTrack=track;
        }
      }
      if(fittedTrack){
	bool deltrack=true;
	
	int nHT=nHTHits(fittedTrack);
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"NUmber of HT hits: "<<nHT<<endreq;
	
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Successfull fit of track. " << endreq;
	if (msgLvl(MSG::DEBUG) && (*iseg)->fitQuality()) msg(MSG::DEBUG) << " Quality of Segment: "<<(*iseg)->fitQuality()->chiSquared()<<" "<<(*iseg)->fitQuality()->numberDoF()<< endreq;
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Quality of Track:   "<<fittedTrack->fitQuality()->chiSquared()<<" "<<fittedTrack->fitQuality()->numberDoF()<< endreq;
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Noise probability: "<<getNoiseProbability(fittedTrack)<<endreq;        

	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< (*fittedTrack) <<endreq;        
	
	if( nTRTHits(fittedTrack)>=m_minTRTHits){
	  deltrack=false;
	  outputTrackCollection->push_back(fittedTrack);
	}else{
	  deltrack=true;
	}
        
	if( nTRTHits(fittedTrack)<m_minTRTHits){
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<" This tracks has only "<<nTRTHits(fittedTrack)<<" Hits ! Will reject it!"<<endreq;
	}else{
            
	  //check if this track is real or fake
	  if(getNoiseProbability(fittedTrack)>m_noiseCut){
	    double truefraction=getRealFractionTRT(fittedTrack);
	    
	    int nhits=(*iseg)->numberOfMeasurementBases();
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Real/Noise : "<< truefraction << " chi2="<<fittedTrack->fitQuality()->chiSquared()/double(fittedTrack->fitQuality()->numberDoF()) <<endreq;
	    if(truefraction>0.5){
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " ==> Real Track"<<endreq;
	      
	      if(m_MapReal.find(nhits)==m_MapReal.end()){
		m_MapReal[nhits]=1;
	      }else{
		m_MapReal[nhits]++;
	      }
	      
	      m_nTracksReal++;
	      m_noiseratio+=(1-truefraction);
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " ==> Fake Track"<<endreq;
	      
	      if(m_MapFake.find(nhits)==m_MapFake.end()){
		m_MapFake[nhits]=1;
	      }else{
		m_MapFake[nhits]++;
	      }
	      
	      m_nTracksFake++;
	    }
	  }
	}
	
	if(deltrack)
	  delete fittedTrack;
      }else{
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Fit did not converge!"<<endreq;
      } //end of if(fittedTrack)

      if(inputMatchingPar)
	delete inputMatchingPar;
      if(inputMatchingPer)
	delete inputMatchingPer;
      
    } //end of if: (*iseg)->numberOfMeasurementBases()>0
  } //end of loop over segments

  sc = evtStore()->record(outputTrackCollection, m_outputTrackCollectionName);
  if(sc.isFailure()){
    msg(MSG::ERROR)  << " Could not write track collection " 
	<<  m_outputTrackCollectionName << endreq;
    return sc;
  }

  return sc;
}

int InDet::TRT_SegmentsToTrack::getNumberReal(const InDet::TRT_DriftCircle* driftcircle)
{
  int numBarcodes = 0;
  typedef PRD_MultiTruthCollection::const_iterator iter;

  if(!m_truthCollectionTRT) return 0;
  std::pair<iter,iter> range = m_truthCollectionTRT->equal_range(driftcircle->identify());
  for(iter i = range.first; i != range.second; i++){
    numBarcodes++;
  }
  return numBarcodes;
}

double InDet::TRT_SegmentsToTrack::getRealFraction(const Trk::Segment *segment)
{
  double fraction=0.;
  int nDriftReal=0;
  int nDriftNoise=0;


  for(unsigned int i=0;i<segment->numberOfMeasurementBases();++i){
    const Trk::RIO_OnTrack* rot=dynamic_cast<const Trk::RIO_OnTrack*>(segment->measurement(i));
    if(rot){
      const Trk::PrepRawData* prd=rot->prepRawData();
      if(prd!=0){
        const InDet::TRT_DriftCircle *dc=dynamic_cast<const InDet::TRT_DriftCircle*>(prd);
        if(dc!=0){
          int nreal=getNumberReal(dc);
          
          if(nreal>0) nDriftReal++;
          else nDriftNoise++;
        }
      }
    }
  }

  if(nDriftReal>0)
    fraction=double(nDriftReal)/double(nDriftNoise+nDriftReal);
    

  return fraction;
}


double InDet::TRT_SegmentsToTrack::getRealFractionTRT(const Trk::Track *track)
{
  double fraction=0.;
  int nDriftReal=0;
  int nDriftNoise=0;

  //loop over the hits
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsos=track->trackStateOnSurfaces()->begin();
       tsos!=track->trackStateOnSurfaces()->end(); ++tsos) {

    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>((*tsos)->measurementOnTrack());
    if (hitOnTrack != 0) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID)){

	const InDet::TRT_DriftCircleOnTrack *dcot= dynamic_cast <const InDet::TRT_DriftCircleOnTrack*>(hitOnTrack);
	const InDet::TRT_DriftCircle *dc=dcot->prepRawData();

	if(dc!=0){
	  int nreal=getNumberReal(dc);
	  
	  if(nreal>0) nDriftReal++;
	  else nDriftNoise++;
	  
	}
      }
    }
  }

  if(nDriftReal>0)
    fraction=double(nDriftReal)/double(nDriftNoise+nDriftReal);

  return fraction;
}


int InDet::TRT_SegmentsToTrack::nHTHits(const Trk::Track *track)
{

  int nHT=0;
  int nLT=0;

  //loop over the hits
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsos=track->trackStateOnSurfaces()->begin();
       tsos!=track->trackStateOnSurfaces()->end(); ++tsos) {
    
    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>((*tsos)->measurementOnTrack());
    if (hitOnTrack != 0) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID) && !(*tsos)->type(Trk::TrackStateOnSurface::Outlier)){
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



int InDet::TRT_SegmentsToTrack::nTRTHits(const Trk::Track *track)
{
  int nhits=0;

  //loop over the hits
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsos=track->trackStateOnSurfaces()->begin();
       tsos!=track->trackStateOnSurfaces()->end(); ++tsos) {
    
    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>((*tsos)->measurementOnTrack());
    if (hitOnTrack != 0) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID) && !(*tsos)->type(Trk::TrackStateOnSurface::Outlier)){
	nhits++;
      }
    }
  }

  return nhits;
}


double InDet::TRT_SegmentsToTrack::getNoiseProbability(const Trk::Track *track)
{
  double fraction=0.;
  int nDriftReal=0;
  int nDriftNoise=0;

  //loop over the hits
  for (std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsos=track->trackStateOnSurfaces()->begin();
       tsos!=track->trackStateOnSurfaces()->end(); ++tsos) {
    
    const Trk::RIO_OnTrack* hitOnTrack = dynamic_cast <const Trk::RIO_OnTrack*>((*tsos)->measurementOnTrack());
    if (hitOnTrack != 0) {
      const Identifier& surfaceID = hitOnTrack->identify();
      
      //take only TRT hits
      if(m_idHelper->is_trt(surfaceID)){

	const InDet::TRT_DriftCircleOnTrack *dcot= dynamic_cast <const InDet::TRT_DriftCircleOnTrack*>(hitOnTrack);
	const InDet::TRT_DriftCircle *dc=dcot->prepRawData();

	if(dc!=0){
	  
	  if(dc->isNoise())
	    nDriftNoise++;
	  else
	    nDriftReal++;
	}
      }
    }
  }

  if(nDriftReal>0)
    fraction=double(nDriftReal)/double(nDriftNoise+nDriftReal);

  return fraction;
}



 void InDet::TRT_SegmentsToTrack::combineSegments(void)
{
  //Idea: 
  // - get from endcap segment z-phi dependence
  // - get from barrel segment r-phi dependence and z dependence from endcap segment fit





  const Trk::SegmentCollection *BarrelSegments   = 0;
  const Trk::SegmentCollection *EndcapSegments   = 0;

  StatusCode sc;

  TrackCollection *outputCombiCollection   = new TrackCollection();

  sc = evtStore()->retrieve(BarrelSegments, m_barrelSegments);
  if(sc.isFailure()){
    msg(MSG::FATAL) << " could not open barrel segments collection : " <<   m_barrelSegments << endreq;
    return;
  }

  sc = evtStore()->retrieve(EndcapSegments, m_endcapSegments);
  if(sc.isFailure()){
    msg(MSG::FATAL) << " could not open endcap segments collection : " <<   m_endcapSegments << endreq;
    return;
  }

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Got both barrel and endcap segment collections of size "<<BarrelSegments->size()<<" "<<EndcapSegments->size()<<endreq;
  
  Trk::SegmentCollection::const_iterator iseg = BarrelSegments->begin();
  Trk::SegmentCollection::const_iterator isegE = BarrelSegments->end();

  int scount=0;
  for(;iseg!=isegE;++iseg,scount++){
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Barrel Segment "<<scount<<" : phi="<<(*iseg)->localParameters()[Trk::phi]<<endreq;
  }

  iseg  = EndcapSegments->begin();
  isegE = EndcapSegments->end();

  scount=0;
  for(;iseg!=isegE;++iseg,scount++){
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Endcap Segment "<<scount<<" : phi="<<(*iseg)->localParameters()[Trk::phi]<<endreq;
  }

  if(BarrelSegments->size()==1 && EndcapSegments->size()==1){
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Here we go: one barrel segment and one endcap segment!"<<endreq;

    Trk::SegmentCollection::const_iterator isegBarrel = BarrelSegments->begin();
    Trk::SegmentCollection::const_iterator isegBarrelE = BarrelSegments->end();

    Trk::SegmentCollection::const_iterator isegEndcap = EndcapSegments->begin();
    Trk::SegmentCollection::const_iterator isegEndcapE = EndcapSegments->end();

    //loop over all Barrel Segments
    for(;isegBarrel!=isegBarrelE;++isegBarrel){
      for(;isegEndcap!=isegEndcapE;++isegEndcap){
	
	std::vector< Trk::PseudoMeasurementOnTrack*> tobedeleted;

	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Barrel Segment : phi="<<(*isegBarrel)->localParameters()[Trk::phi]<<" with "<<(*isegBarrel)->numberOfMeasurementBases()<<" hits"<<endreq;
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Endcap Segment : phi="<<(*isegEndcap)->localParameters()[Trk::phi]<<" with "<<(*isegEndcap)->numberOfMeasurementBases()<<" hits"<<endreq;
	
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
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Both barrel and endcap segments in top sectors, won't fit"<<endreq;
	  continue;
	}

	if(barreldown && (*isegEndcap)->globalPosition().y()<0){
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Both barrel and endcap segments in lower sectors, won't fit"<<endreq;
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
	      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Endcap : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) "<<endreq;
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
	      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Barrel : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) "<<endreq;
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
	      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Barrel : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) "<<endreq;
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
	      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Endcap : (phi,r,z) = ( "<<phi<<" , "<<r<<" , "<<z<<" ) "<<endreq;
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
	
	
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Global position: "<<inputMatchingPos<<" Globalmomentum: "<<inputMatchingMom<<endreq;



	//add two pseudomeasurements at the beginning and at the end of the track
	Trk::MeasurementSet::const_iterator mit;
	Trk::MeasurementSet::const_iterator mitE=myset.end();

	int count2=0;
	for(mit=myset.begin();mit!=mitE;++mit){
	  myset2.push_back(*mit);
	  if(count2==firstechit || count2==lastechit){
	    const InDet::TRT_DriftCircleOnTrack* trtcirc = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(*mit);
	    if(trtcirc){
	      //add pseudomeasurement
	      double locz=-200.;
              if ((count2==firstechit && firstechit!=0) || (count2==lastechit && firstechit==0)) locz=200;
	      Trk::DefinedParameter dp(locz,Trk::locZ);
	      const Trk::StraightLineSurface *line=dynamic_cast<const
		Trk::StraightLineSurface*>(&trtcirc->associatedSurface());
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
	  count2++;
	}

       
	
	const Trk::StraightLineSurface* testSf;
	if((*isegEndcap)->globalPosition().y()>0){
	  testSf= dynamic_cast<const Trk::StraightLineSurface*>(&((*isegEndcap)->associatedSurface()));
	}else{
	  testSf= dynamic_cast<const Trk::StraightLineSurface*>(&((*isegBarrel)->associatedSurface()));
	}
	
	const Trk::AtaStraightLine* inputMatchingPar =0;
	const Trk::Perigee* inputMatchingPer =0;
	
	if(!testSf){
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"No straightLineSurface !! Trying Perigee ..."<<endreq;
	  
	  const Trk::PerigeeSurface *testPSf=dynamic_cast<const Trk::PerigeeSurface*>(&((*isegBarrel)->associatedSurface()));
	  
	  if(!testPSf){
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Also PerigeeSurface failed? What is it? "<<(*isegBarrel)->associatedSurface()<<endreq;
	  }else{
	    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<"Ok, it seems to be a PerigeeSurface"<<endreq;            
	  }
	  
	  
	  inputMatchingPer = new Trk::Perigee(inputMatchingPos,inputMatchingMom,
					      1., *testPSf);
	  
	}else{
	  inputMatchingPar = new Trk::AtaStraightLine(inputMatchingPos,inputMatchingMom,
						      1., *testSf);          
	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " created testSf : " << (*inputMatchingPar)<< endreq;
	}
	
	
	Amg::Vector3D startMomentum( 0., 0., 1.);
	Trk::Track *fittedTrack=0;
	if(m_materialEffects){
	  if(inputMatchingPar){
	    fittedTrack=m_trackFitter->fit(myset2,
					   *inputMatchingPar,
					   false,
					   Trk::muon
					   );
	  }else{
	    fittedTrack=m_trackFitter->fit(myset2,
					   *inputMatchingPer,
					   false,
					   Trk::muon
					   );
	  }
	}else{
	  if(inputMatchingPar){
	    fittedTrack=m_trackFitter->fit(myset2,
					   *inputMatchingPar,
					   false,
					   Trk::nonInteracting
					   );
	  }else{
	    fittedTrack=m_trackFitter->fit(myset2,
					   *inputMatchingPer,
					   false,
					   Trk::muon
					   );
	  }
	}
	
	if(fittedTrack){
	  m_n_combined_fit++;
	  outputCombiCollection->push_back(fittedTrack);
	  if (msgLvl(MSG::DEBUG)){ 
	    msg(MSG::DEBUG) << " Successfull Barrel+Endcap fit of segment. " << endreq;
	    msg(MSG::DEBUG)  << " Quality of Track:   "<<fittedTrack->fitQuality()->chiSquared()<<" / "<<fittedTrack->fitQuality()->numberDoF()<< endreq;
	  }

	  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<<*fittedTrack<<endreq;
	  //tobedeleted.clear(); // TILL : first delete its content (done further below)
	}
	
	if(inputMatchingPer)
	  delete inputMatchingPer;
	
	if(inputMatchingPar)
	  delete inputMatchingPar;
	

	for(size_t i=0;i<tobedeleted.size();i++)
	  delete tobedeleted[i];
	tobedeleted.clear();


      }
    }
  }
  sc = evtStore()->record(outputCombiCollection, m_BECCollectionName);
  if(sc.isFailure()){
    msg(MSG::ERROR) << " Could not write track Barrel+EC collection " 
	<<  "TRT_Barrel_EC" << endreq;
  }

}
