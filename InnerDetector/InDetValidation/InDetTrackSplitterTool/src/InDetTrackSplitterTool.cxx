/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   InDetTrackSplitterTool.cxx
   AUTHOR: John Alison <johnda@hep.upenn.edu>
 */

#include "InDetTrackSplitterTool/InDetTrackSplitterTool.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
//#include "TrkFitterUtils/FitterTypes.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
//#include "TrkParameters/MeasuredPerigee.h" //use perigee
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "GeoPrimitives/GeoPrimitives.h" 
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ThreadLocalContext.h"

/** @class InDetTrackSplitterTool 
    
  This tool is used for splitting tracks in different segments and returning fitted tracks that
  have the corrisponding hits associated with them.
  
  Currently it is for seen to split tracks in:
  
     - Upper and Lower tracks (for cosmics)
    
     - Tracks using alternating hits. (odd and even)
  
     - Strip a combined (Si+TRT) track of its Si hits
        refit remianing TRT-Only hits with theta and z0
  constraint from input track.

     - Strip a combined (Si+TRT) track of its TRT hits
        refit remianing Si-Only hits with qOverP
  constraint from input track.

  Usages:
  1) If you have a single track and want to split in to upper and lower do:

     m_trackSplitterTool->splitInUpperLowerTrack(inputtrack);
                        ->splitInOddEvenHitsTrack(inputtrack);
  
     this call return a std::pair containing the split tracks
  
  3) If you have a single track and want to strip ether Si or TRT hits do:
     
     m_trackSplitterTool->stripTrack(inputtrack,true); //For removing the Si Hits
     m_trackSplitterTool->stripTrack(inputtrack,false);//For removing the TRT Hits

  2) If you have a track collection and want to create track collections with upper and lower tracks do
  
     m_trackSplitterTool->splitTracks(inputTracks);
  
  then retrieve the collection from storeGate with the names m_outputUpperTracksName, m_outputLowerTracksName
    
  @author  John Alison
*/  

//namespace InDetTrackSplitterHelpers{
//  /** A method to determine which half track the hit belongs to  
//   */
//  bool inline CompareYPosition(Trk::MeasurementBase const* mb1, Trk::MeasurementBase const* mb2){
//    return mb1->globalPosition().y() > mb2->globalPosition().y();
//  }
//}


InDet::InDetTrackSplitterTool::InDetTrackSplitterTool( std::string const & type, std::string const & name, IInterface const* parent ) :
  AthAlgTool( type, name, parent ),
  m_trkfitter("Trk::GlobalChi2Fitter/InDetTrackFitter")
{
  
  declareInterface<IInDetTrackSplitterTool>(this);
  declareProperty("TrackFitter", m_trkfitter);
  declareProperty("OutputUpperTracksName", m_outputUpperTracksName="splitUpperTracks");
  declareProperty("OutputLowerTracksName", m_outputLowerTracksName="splitLowerTracks");
  declareProperty("KeepMaterialStates",m_keepmaterial=true);
}

//=========================================================
InDet::InDetTrackSplitterTool::~InDetTrackSplitterTool() { }


//=========================================================
StatusCode InDet::InDetTrackSplitterTool::initialize()
{
  
  msg(MSG::DEBUG) << "In initialize()" << endmsg;
  
  /** Get TRT helper
   */      
  if ((detStore()->retrieve(m_trtid)).isFailure()) {
    msg(MSG::WARNING) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::SUCCESS;
  }

  /** Get SCT helper
   */
  if ((detStore()->retrieve(m_sctid)).isFailure()) {
    msg(MSG::WARNING) << "Problem retrieving SCT ID helper" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  /** Fitter
   */
  if(m_trkfitter.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Could not find refit tool " << m_trkfitter  << endmsg;
    return StatusCode::SUCCESS;
  } 

  ATH_CHECK( m_outputUpperTracksName.initialize() );
  ATH_CHECK( m_outputLowerTracksName.initialize() );

  msg(MSG::DEBUG) << "InDetTrackSplitterTool initialized" << endmsg;
  return StatusCode::SUCCESS;

}

//=========================================================
StatusCode InDet::InDetTrackSplitterTool::finalize()
{
  msg(MSG::DEBUG) << "In finalize() "<< endmsg;
  return StatusCode::SUCCESS;
}

/** Splits a single input track into upper and lower parts (based on global y) 
    returns a pair of track the first being the upper
*/
std::pair<Trk::Track*, Trk::Track*> InDet::InDetTrackSplitterTool::splitInUpperLowerTrack(Trk::Track const& input, 
                        bool siliconHitsOnly) const {
  //std::cout << "input: " << input << std::endl;
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " In splitInUpperLowerTrack" <<endmsg;

  //**The returned tracks */
  Trk::Track* upperTrack(0);
  Trk::Track* lowerTrack(0);
  
  /**Get the initial perigee parameters*/
  Trk::Perigee const* originalPerigee = input.perigeeParameters();
  
  /**Get the track particle hypothisis*/
  Trk::ParticleHypothesis hypo = input.info().particleHypothesis();

  /** Get the  measurements */
  DataVector<const Trk::TrackStateOnSurface>* uppertraj = new DataVector<const Trk::TrackStateOnSurface>;
  DataVector<const Trk::TrackStateOnSurface>* lowertraj = new DataVector<const Trk::TrackStateOnSurface>;
  
  unsigned int totalNumberHits = 0;

  unsigned int totalNumberPixelHits = 0;
  unsigned int totalNumberSCTHits = 0;
  unsigned int totalNumberTRTHits = 0;

  unsigned int numberUpperPixelHits = 0; 
  unsigned int numberUpperSCTHits = 0; 
  unsigned int numberUpperTRTHits = 0; 
  unsigned int numberUpperPseudoMeas = 0; 
  
  unsigned int numberLowerPixelHits = 0; 
  unsigned int numberLowerSCTHits = 0; 
  unsigned int numberLowerTRTHits = 0; 
  unsigned int numberLowerPseudoMeas = 0; 

  //DataVector<Trk::MeasurementBase const>::const_iterator meas = input.measurementsOnTrack()->begin();
  //DataVector<Trk::MeasurementBase const>::const_iterator measEnd = input.measurementsOnTrack()->end(); 
  //for(;meas != measEnd; ++meas){
  //Trk::RIO_OnTrack const* rio = dynamic_cast<Trk::RIO_OnTrack const*>(*meas);
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosit = input.trackStateOnSurfaces()->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosEnd = input.trackStateOnSurfaces()->end();
  bool perigeeseen=false; 
  for(;tsosit != tsosEnd; ++tsosit){
    if ((**tsosit).type(Trk::TrackStateOnSurface::Outlier)) continue;
    if (originalPerigee==(**tsosit).trackParameters()){
      perigeeseen=true;
      uppertraj->push_back((**tsosit).clone());
      lowertraj->push_back((**tsosit).clone());
      continue;
    }
    
    if (m_keepmaterial && ((**tsosit).type(Trk::TrackStateOnSurface::Scatterer) || (**tsosit).type(Trk::TrackStateOnSurface::BremPoint) || (**tsosit).type(Trk::TrackStateOnSurface::CaloDeposit))){
      if (!perigeeseen) uppertraj->push_back((**tsosit).clone());
      else lowertraj->push_back((**tsosit).clone());
      continue;
    }
    const Trk::RIO_OnTrack *rio = 0;
    const Trk::MeasurementBase *measb=(**tsosit).measurementOnTrack();
    if (measb) rio=dynamic_cast<const Trk::RIO_OnTrack*>(measb);

    if(rio){
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "an rio..." <<endmsg; 
      
      totalNumberHits++;

      Identifier const& surfaceid = (rio->identify());
      
      if(m_trtid->is_pixel(surfaceid))
  ++totalNumberPixelHits;
      if(m_trtid->is_sct(surfaceid))
  ++totalNumberSCTHits;
      if(m_trtid->is_trt(surfaceid))
  ++totalNumberTRTHits;
      
      //if( (*meas)->globalPosition().y() > 0){
      if( !perigeeseen){
  
  if(m_trtid->is_pixel(surfaceid))
    ++numberUpperPixelHits;
  if(m_trtid->is_sct(surfaceid))
    ++numberUpperSCTHits;
  if(m_trtid->is_trt(surfaceid))
    ++numberUpperTRTHits;

  if(!siliconHitsOnly || m_trtid->is_sct(surfaceid) || m_trtid->is_pixel(surfaceid))
    uppertraj->push_back((**tsosit).clone());
      }

      //if( (*meas)->globalPosition().y() < 0){
      if( perigeeseen){

  if(m_trtid->is_pixel(surfaceid))
    ++numberLowerPixelHits;
  if(m_trtid->is_sct(surfaceid))
    ++numberLowerSCTHits;
  if(m_trtid->is_trt(surfaceid))
    ++numberLowerTRTHits;
  
  if(!siliconHitsOnly || m_trtid->is_sct(surfaceid) || m_trtid->is_pixel(surfaceid))
    //m_lowerHits.push_back( *meas);
    lowertraj->push_back((**tsosit).clone());
      }
    }else{
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Not an rio" <<endmsg; 
      //Trk::PseudoMeasurementOnTrack const* ps = dynamic_cast<Trk::PseudoMeasurementOnTrack const*>(*measb);
      const Trk::PseudoMeasurementOnTrack *ps = dynamic_cast<const Trk::PseudoMeasurementOnTrack *>(measb);

      if(ps){
  if (!perigeeseen || totalNumberHits==totalNumberTRTHits) {
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Adding an upper pseudoMeasurement" <<endmsg; 
          ++numberUpperPseudoMeas;
          uppertraj->push_back((**tsosit).clone());
  }
        if (perigeeseen || totalNumberHits==totalNumberTRTHits) {
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Adding a lower pseudoMeasurement" <<endmsg; 
          ++numberLowerPseudoMeas;
          lowertraj->push_back((**tsosit).clone());
        }
      }

      const Trk::CompetingRIOsOnTrack *crot=dynamic_cast<const Trk::CompetingRIOsOnTrack *>(measb);
      if (crot){
        if (!perigeeseen) uppertraj->push_back((**tsosit).clone());
        else lowertraj->push_back((**tsosit).clone());
      }

    }
    

  }

  /** Sort the hits. */
  //std::sort(m_upperHits.begin(), m_upperHits.end(), InDetTrackSplitterHelpers::CompareYPosition );
  //std::sort(m_lowerHits.begin(), m_lowerHits.end(), InDetTrackSplitterHelpers::CompareYPosition );
  Trk::Track upperorigtrack(input.info(),uppertraj,0);
  Trk::Track lowerorigtrack(input.info(),lowertraj,0);
  
  /** Upper track */
  if(isConstrained(numberUpperPixelHits,numberUpperSCTHits,numberUpperTRTHits,numberUpperPseudoMeas)){
    
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "before calling upper fit" << endmsg;
    //m_upperTrack = m_trkfitter->fit(m_upperHits, *originalPerigee, true, hypo);
    upperTrack = m_trkfitter->fit(upperorigtrack, false, hypo);
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "after calling upper fit" << endmsg;
    
    if(!upperTrack){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Upper Fit Failed!" << endmsg ;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
               << numberUpperPixelHits << " upper Pixel hits, " 
               << numberUpperSCTHits << " upper SCT hits, " 
               << numberUpperTRTHits << "upper TRT hits"
               << numberUpperPseudoMeas << "upper Pseudomeasurements"
               << endmsg;

      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
               << totalNumberPixelHits << " total Pixel hits, " 
               << totalNumberSCTHits << " total SCT hits, " 
               << totalNumberTRTHits << "total TRT hits"
               << endmsg;
      
    }    
    
  }else{
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not enough measurements on upper track. Fit fails." << endmsg ;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
               << numberUpperPixelHits << " upper Pixel hits, " 
               << numberUpperSCTHits << " upper SCT hits, " 
               << numberUpperTRTHits << "upper TRT hits"
               << numberUpperPseudoMeas << "upper Pseudomeasurements"
               << endmsg;
  }  
  
  /** lower track */
  if(isConstrained(numberLowerPixelHits,numberLowerSCTHits,numberLowerTRTHits,numberLowerPseudoMeas)){
    
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "before calling lower fit" << endmsg;
    lowerTrack = m_trkfitter->fit(lowerorigtrack, false, hypo);
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "after calling lower fit" << endmsg;
    
    if(!lowerTrack){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Lower Fit Failed!" << endmsg ;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
               << numberLowerPixelHits << " upper Pixel hits, " 
               << numberLowerSCTHits << " upper SCT hits, " 
               << numberLowerTRTHits << "upper TRT hits"
               << numberLowerPseudoMeas << "upper Pseudomeasurements"
               << endmsg;
      
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
               << totalNumberPixelHits << " total Pixel hits, " 
               << totalNumberSCTHits << " total SCT hits, " 
               << totalNumberTRTHits << "total TRT hits"
               << endmsg;
    }
    
  }else{
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not enough measurements on lower track. Fit fails." << endmsg ;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
             << numberLowerPixelHits << " upper Pixel hits, " 
             << numberLowerSCTHits << " upper SCT hits, " 
             << numberLowerTRTHits << "upper TRT hits"
             << numberLowerPseudoMeas << "upper Pseudomeasurements"
             << endmsg;
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Leaving splitInUpperLowerTrack" <<endmsg;
  return std::make_pair(upperTrack, lowerTrack);  
}

/** Strips either the Si or the TRT and refits the track using constrants 
 */
Trk::Track* InDet::InDetTrackSplitterTool::stripTrack(Trk::Track const& input, 
                  bool removeSilicon, bool applyConstraint ) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " In stripTrack" <<endmsg;

  /** The returned track */
  Trk::Track* outputTrack(0);

  /** The strip the Si */
  if(removeSilicon){
    outputTrack = stripSiFromTrack(input);
  }
  /** The strip the TRT */
  else{
    outputTrack = stripTRTFromTrack(input, applyConstraint);
  }

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Leaving stripTrack" <<endmsg;
  return outputTrack;
}

/** Strip the Si hits, fit the remaining with a theta, z0 constraint.
 */
Trk::Track* InDet::InDetTrackSplitterTool::stripSiFromTrack(Trk::Track const& input) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In stripSiFromTrack" <<endmsg;
  
  /** The returned track */
  Trk::Track* outputTrack(0);

  /** The measurements */
  Trk::MeasurementSet TRTHits;

  /** Get the initial perigee parameters */
  Trk::Perigee const* originalPerigee = input.perigeeParameters();

  /** Get the track particle hypothisis */
  Trk::ParticleHypothesis hypo = input.info().particleHypothesis();

  /** Create the psuesdoMeasurement with the constraints */
  Trk::PseudoMeasurementOnTrack const* constraint = makeThetaZ0Constraint(originalPerigee);

  /** Adding the constraint first */
  TRTHits.push_back(constraint);
  
  unsigned int totalNumberTRTHits = 0;
  
  DataVector<Trk::MeasurementBase const>::const_iterator meas = input.measurementsOnTrack()->begin();
  DataVector<Trk::MeasurementBase const>::const_iterator measEnd = input.measurementsOnTrack()->end(); 
  for(;meas != measEnd; ++meas){
    Trk::RIO_OnTrack const* rio = dynamic_cast<Trk::RIO_OnTrack const*>(*meas);
    
    if(!rio){
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "No ROT skipping measurement." <<endmsg; 
      continue;
    }

    Identifier const& surfaceid = (rio->identify());
    
    if(!m_trtid->is_trt(surfaceid)){
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Removing Non-TRT hit." <<endmsg; 
      continue;
    }
    
    ++totalNumberTRTHits;
    TRTHits.push_back(*meas);
 
  }

  /** Fit track */
  // Should be Constrained !!!
  //if(isConstrained(numberUpperPixelHits,numberUpperSCTHits,numberUpperTRTHits,numberUpperPseudoMeas)){
    
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "before calling fit" << endmsg;
  outputTrack = m_trkfitter->fit(TRTHits, *originalPerigee, true, hypo);
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "after calling fit" << endmsg;
    
  if(!outputTrack){
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Fit Failed!" << endmsg ;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There were: " 
             << totalNumberTRTHits << "TRT hits"
             << endmsg;
  }    
    
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving stripSiFromTrack" <<endmsg;
  return outputTrack;
}


/** Strip the TRT hits, fit the remaining with a qOverP constraint
 */
Trk::Track* InDet::InDetTrackSplitterTool::stripTRTFromTrack(Trk::Track const& input, bool applyConstraint) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In stripTRTFromTrack" <<endmsg;

  /** The returned track */
  Trk::Track* outputTrack(0);

  /** The measurements */
  Trk::MeasurementSet SiHits;

  /** Get the initial perigee parameters */
  Trk::Perigee const* originalPerigee = input.perigeeParameters();

  /** Get the track particle hypothisis */
  Trk::ParticleHypothesis hypo = input.info().particleHypothesis();

  /** Only add the constraint once */
  bool addedConstraint = false;
  
  unsigned int totalNumberSiHits = 0;
  
  DataVector<Trk::MeasurementBase const>::const_iterator meas = input.measurementsOnTrack()->begin();
  DataVector<Trk::MeasurementBase const>::const_iterator measEnd = input.measurementsOnTrack()->end(); 
  for(;meas != measEnd; ++meas){
    Trk::RIO_OnTrack const* rio = dynamic_cast<Trk::RIO_OnTrack const*>(*meas);
    
    if(!rio){
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "No ROT skipping measurement." <<endmsg; 
      continue;
    }

    Identifier const& surfaceid = (rio->identify());
    
    if(applyConstraint && !addedConstraint && m_trtid->is_trt(surfaceid)){
      
      /** Get the position in space were the constraint will be added 
    We'll chose the first TRT hit. 
    (I dont think this matters, but a surface needs to be specified)
      */
      Trk::StraightLineSurface const* trtSurf = dynamic_cast<Trk::StraightLineSurface const*>(&(rio->associatedSurface()));
      /** Create the psuesdoMeasurement with the constraints */
      Trk::PseudoMeasurementOnTrack const* constraint = makePConstraint(originalPerigee,trtSurf);
      
      /** Adding the constraint first */
      SiHits.push_back(constraint);
      
      /** Only add the constraint once */
      addedConstraint = true;
    }

    if(!m_trtid->is_sct(surfaceid) && !m_trtid->is_pixel(surfaceid)){
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Removing Non-Si hit." <<endmsg; 
      continue;
    }
    
    ++totalNumberSiHits;
    SiHits.push_back(*meas);
 
  }

  /** Fit track */
  // Should be Constrained !!!
  //if(isConstrained(numberUpperPixelHits,numberUpperSCTHits,numberUpperTRTHits,numberUpperPseudoMeas)){
    
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "before calling fit" << endmsg;
  outputTrack = m_trkfitter->fit(SiHits, *originalPerigee, false, hypo);
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "after calling fit" << endmsg;
    
  if(!outputTrack){
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Fit Failed!" << endmsg ;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There were: " << totalNumberSiHits << "TRT hits" << endmsg;
  }    
    
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving stripTRTFromTrack" <<endmsg;
  return outputTrack;
}

/** Make the qOverP constraint
 */
Trk::PseudoMeasurementOnTrack const* InDet::InDetTrackSplitterTool::makePConstraint(Trk::Perigee const* perigee,Trk::StraightLineSurface const* trtSurf) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In makePConstraint" <<endmsg;
  
  if( !perigee->covariance() ) return 0;
  
  /** Define the constraint parameter
   */
  Trk::DefinedParameter inputPMeasurement(perigee->parameters()[Trk::qOverP],Trk::qOverP);
  std::vector<Trk::DefinedParameter> constraints;
  constraints.push_back(inputPMeasurement);

  /** Define the uncertainty on the constraint parameter*/
  Amg::MatrixX constraintErrMatrix(1,1);
  constraintErrMatrix(0,0)=(*perigee->covariance())(Trk::qOverP,Trk::qOverP);

  /** Create the constraint   */
  Trk::PseudoMeasurementOnTrack* psmom = new Trk::PseudoMeasurementOnTrack(Trk::LocalParameters(constraints),constraintErrMatrix  ,*trtSurf);
  
  /** Debugging
   */
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Momentum constraint: "<<perigee->parameters()[Trk::qOverP] <<" cov-value: "<<constraintErrMatrix(0,0) <<endmsg;
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving makePConstraint" <<endmsg;
  return psmom;
}

/** Make the theta and z0 constraint 
 */
Trk::PseudoMeasurementOnTrack const* InDet::InDetTrackSplitterTool::makeThetaZ0Constraint(Trk::Perigee const* perigee) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In makeThetaZ0Constraint" <<endmsg;  

  if( !perigee->covariance() ) return 0;
  
  /** Define the constraint parameters
   */
  Trk::DefinedParameter inputTheta0Measurement(perigee->parameters()[Trk::theta],Trk::theta);
  Trk::DefinedParameter inputZ0Measurement(perigee->parameters()[Trk::z0],Trk::z0);
  std::vector<Trk::DefinedParameter> constraints;
  constraints.push_back(inputZ0Measurement);
  constraints.push_back(inputTheta0Measurement);
  
  /** Define the uncertainty on the constraint parameters
   */
  Amg::MatrixX constraintErrMatrix(2,2);
  constraintErrMatrix(0,0) = (*perigee->covariance())(Trk::z0,Trk::z0);
  constraintErrMatrix(0,1) = (*perigee->covariance())(Trk::z0,Trk::theta);
  constraintErrMatrix(1,0) = (*perigee->covariance())(Trk::theta, Trk::z0);
  constraintErrMatrix(1,1) = (*perigee->covariance())(Trk::theta, Trk::theta);

  /** Get the surface were the constraint will be applied
      (we got the constraints from the perigee, apply it there too) 
   */
  const Trk::PerigeeSurface & surface = perigee->associatedSurface();

  /** Create the constriant
   */
  Trk::PseudoMeasurementOnTrack* psmom = new Trk::PseudoMeasurementOnTrack(Trk::LocalParameters(constraints),constraintErrMatrix  , surface);
  
  /** Debugging 
   */
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"theta constraint: "<<perigee->parameters()[Trk::theta] <<" cov-value: "<<constraintErrMatrix(1,1) <<endmsg;
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"z0 constraint: "<<perigee->parameters()[Trk::z0] <<" cov-value: "<<constraintErrMatrix(0,0) <<endmsg;
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving makeThetaZ0Constraint" <<endmsg;
  return psmom;
}

/** 
    Splits a single input track into upper and lower parts (based on global y) 
    returns a pair of track the first being the upper
*/
std::pair<Trk::Track*, Trk::Track*> InDet::InDetTrackSplitterTool::splitInOddEvenHitsTrack(Trk::Track const& input) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " In splitInOddEvenHitsTrack" <<endmsg;

  /** The returned tracks */
  Trk::Track* oddTrack(0);
  Trk::Track* evenTrack(0);
  
  /** Get the initial perigee parameters */
  Trk::Perigee const* originalPerigee = input.perigeeParameters();

  /** Get the track particle hypothisis */
  Trk::ParticleHypothesis hypo = input.info().particleHypothesis();

  /** Get the  measurements */
  Trk::MeasurementSet oddHits;
  Trk::MeasurementSet evenHits;
  
  unsigned int totalNumberPixelHits = 0;
  unsigned int totalNumberSCTHits = 0;
  unsigned int totalNumberTRTHits = 0;
  unsigned int totalNumberHits = 0;

  unsigned int numberOddSCTHits = 0; 
  unsigned int numberOddPixelHits = 0; 
  unsigned int numberOddTRTHits = 0; 
  unsigned int numberOddPseudoMeas = 0; 
  
  unsigned int numberEvenPixelHits = 0; 
  unsigned int numberEvenSCTHits = 0; 
  unsigned int numberEvenTRTHits = 0; 
  unsigned int numberEvenPseudoMeas = 0; 
  
  /** Get SCT hits (ordered) */
  std::vector<Trk::MeasurementBase const*> unusedSCTHits = getSCTHits(input);
  
  DataVector<Trk::MeasurementBase const>::const_iterator meas = input.measurementsOnTrack()->begin();
  DataVector<Trk::MeasurementBase const>::const_iterator measEnd = input.measurementsOnTrack()->end(); 
  for(;meas != measEnd; ++meas){
    Trk::RIO_OnTrack const* rio = dynamic_cast<Trk::RIO_OnTrack const*>(*meas);
    if(rio){
      Identifier const& surfaceid = (rio->identify());
      if(m_trtid->is_pixel(surfaceid)) ++totalNumberPixelHits;
      if(m_trtid->is_sct(surfaceid)) ++totalNumberSCTHits;
      if(m_trtid->is_trt(surfaceid)) ++totalNumberTRTHits;
      
      /** The odd hits */
      if( totalNumberHits % 2 == 1){
  
        if(m_trtid->is_pixel(surfaceid)) ++numberOddPixelHits;
        if(m_trtid->is_sct(surfaceid)) ++numberOddSCTHits;
        if(m_trtid->is_trt(surfaceid)) ++numberOddTRTHits;
  
        if(!m_trtid->is_sct(surfaceid)){
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "adding odd hit" <<endmsg;
          oddHits.push_back( *meas);
          ++totalNumberHits;
          }else{
    
            /** protect against breaking up space points
                find the sct hit in unusedSCTHits */
            std::vector<Trk::MeasurementBase const* >::iterator result = find(unusedSCTHits.begin(), unusedSCTHits.end(),  *meas);
    
            /** if the SCT hits hasn't been assigned to a tracks */
            if(result != unusedSCTHits.end()){
      
              if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "add the (odd) SCT hit" <<endmsg;
              oddHits.push_back( *meas);
              ++totalNumberHits;

              /** remove hit from unused list */
              if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "removing the SCT hit from unused list" <<endmsg;
              unusedSCTHits.erase(result);
      
              /** find any other hits with same r phi */
              result = findSCTHitsFromSameSpacePoint( *meas, unusedSCTHits);
              if(result != unusedSCTHits.end()){
                if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "adding the other (odd) SCT hit in the spacepoint" <<endmsg;
                oddHits.push_back( *result);
                if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "removing the SCT hit from unused list" <<endmsg;
                unusedSCTHits.erase(result);
              }
            }
          }
      }else{/** The even hits */
        if(m_trtid->is_pixel(surfaceid))
          ++numberEvenPixelHits;
        if(m_trtid->is_sct(surfaceid))
          ++numberEvenSCTHits;
        if(m_trtid->is_trt(surfaceid))
          ++numberEvenTRTHits;
  
        if(!m_trtid->is_sct(surfaceid)){
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "add the (even) hit" <<endmsg;
          evenHits.push_back( *meas);
          ++totalNumberHits;
        }else{
          /** protect against breaking up space points
              find the sct hit in unusedSCTHits */
          std::vector<Trk::MeasurementBase const* >::iterator result = find(unusedSCTHits.begin(), unusedSCTHits.end(), *meas);
    
          /** if the SCT hits hasn't been assigned to a tracks */
          if(result != unusedSCTHits.end()){
      
            if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "add the (even) SCT hit" <<endmsg;
            evenHits.push_back( *meas);
            ++totalNumberHits;

            /** remove hit from umused list */
            if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "removing the SCT hit from unused list" <<endmsg;
            unusedSCTHits.erase(result);
      
            /** find any other hits with same r phi */
            result = findSCTHitsFromSameSpacePoint( *meas, unusedSCTHits);
            if(result != unusedSCTHits.end()){
              if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "adding the other (even) SCT hit in the spacepoint" <<endmsg;
              evenHits.push_back( *result);
              if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "removing the SCT hit from unused list" <<endmsg;
              unusedSCTHits.erase(result);
            }
          }
        }
      }
    }else{
      Trk::PseudoMeasurementOnTrack const* ps = dynamic_cast<Trk::PseudoMeasurementOnTrack const*>(*meas);
      
      if(ps){
        if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Adding an odd pseudoMeasurement" <<endmsg; 
        ++numberOddPseudoMeas;
        oddHits.push_back( ps);

        if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Adding an even pseudoMeasurement" <<endmsg; 
        ++numberEvenPseudoMeas;
        evenHits.push_back( ps);
      }
    }
  }

  /** Sort the hits. */
  //std::sort(oddHits.begin(), oddHits.end(), InDetTrackSplitterHelpers::CompareYPosition );
  //std::sort(evenHits.begin(), evenHits.end(), InDetTrackSplitterHelpers::CompareYPosition );
  
  /** Upper track */
  /** track must be overconstrained */
  if(isConstrained(numberOddPixelHits,numberOddSCTHits,numberOddTRTHits,numberOddPseudoMeas)){  
    
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "before calling odd fit" << endmsg;
    oddTrack = m_trkfitter->fit(oddHits, *originalPerigee, false, hypo);
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "after calling odd fit" << endmsg;
    
    if(!oddTrack){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Odd Fit Failed!" << endmsg ;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
               << numberOddPixelHits << " odd Pixel hits, " 
               << numberOddSCTHits << " odd SCT hits, " 
               << numberOddTRTHits << " odd TRT hits"
               << numberOddPseudoMeas << "odd Pseudomeasurements"
               << endmsg;

      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " 
               << totalNumberPixelHits << " total Pixel hits, " 
               << totalNumberSCTHits << " total SCT hits, " 
               << totalNumberTRTHits << "total TRT hits"
               << endmsg;
    }    
    
  }else
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not enough measurements on odd track. Fit fails." << endmsg ;
  
  /** lower track */
  if(isConstrained(numberEvenPixelHits,numberEvenSCTHits,numberEvenTRTHits,numberEvenPseudoMeas)){
    
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "before calling even fit" << endmsg;
    evenTrack = m_trkfitter->fit(evenHits, *originalPerigee, false, hypo);
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "after calling even fit" << endmsg;
    
    if(!evenTrack){
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Even Fit Failed!" << endmsg ;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There was: " << numberEvenSCTHits << " even si hits and " 
               << numberEvenTRTHits << "even TRT hits"<< endmsg;
      
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There were: " << totalNumberSCTHits << " total Si hits and " 
               << totalNumberTRTHits << "total TRT hits"<< endmsg;
    }
    
  }else
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not enough measurements on even track. Fit fails." << endmsg ;
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Leaving splitInOddEvenHitsTrack" <<endmsg;
  return std::make_pair(oddTrack, evenTrack);  
}
  
/** 
    Takes a trackCollection, splits them according to upper and lower parts and fills
    two track collections, one with upper tracks and one with lower tracks
    which are accessed via getUpperTracks and getLowerTracks 
    
    Using this method requires track to pass trackIsCandidate 
    (for the moment this is just a d0 cut requiring the track went through TRT cavity
*/
void InDet::InDetTrackSplitterTool::splitTracks(TrackCollection const* inputTracks) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " In splitTracks" <<endmsg;
  
  //  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::WriteHandle<TrackCollection> upperTracks(m_outputUpperTracksName);
  if ( upperTracks.record( std::make_unique<TrackCollection>() ).isFailure() ) {
     ATH_MSG_FATAL( "Failed to record upper tracks collection " <<  m_outputUpperTracksName.key() );
  }

  SG::WriteHandle<TrackCollection> lowerTracks(m_outputLowerTracksName);
  if ( lowerTracks.record( std::make_unique<TrackCollection>() ).isFailure() ) {
     ATH_MSG_FATAL( "Failed to record upper tracks collection " <<  m_outputLowerTracksName.key() );
  }

  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "There are: " << inputTracks->size() << " input tracks."<< endmsg;
  
  TrackCollection::const_iterator it = inputTracks->begin();
  TrackCollection::const_iterator itE = inputTracks->end();
  for (; it != itE; ++it){
    
    /** if went though TRT cavity  */
    if(trackIsCandidate( **it) ){
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track is a candidate."<< endmsg;
      
      std::pair<Trk::Track*, Trk::Track*> splitTracks = this->splitInUpperLowerTrack(**it);
      //For debugging
      //std::pair<Trk::Track*, Trk::Track*> splitTracks = this->splitInOddEvenHitsTrack(**it);
      
      Trk::Track* upperTrack = splitTracks.first;
      if(upperTrack){
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Upper track fit suceeded"<< endmsg;
  upperTracks->push_back( upperTrack);
      }else
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Upper track fit failed!"<< endmsg;

      Trk::Track* lowerTrack = splitTracks.second;
      if(lowerTrack){
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Lower track fit suceeded"<< endmsg;
  lowerTracks->push_back( lowerTrack);
      }else
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Lower track fit failed!"<< endmsg;

    } else
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track is NOT a candidate."<< endmsg;
  }

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Leaving splitTracks" <<endmsg;
  return;
}

/** 
    Track selection applied in the splitTracks method
  (for the moment this is just a d0 cut requiring the track went through TRT cavity
*/ 
bool InDet::InDetTrackSplitterTool::trackIsCandidate(Trk::Track const& inputTrack) const{
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In trackIsCandidate."<< endmsg;
  Trk::Perigee const* trackPerigee = inputTrack.perigeeParameters();
  
  if (!trackPerigee)
    {
      msg(MSG::WARNING) << "Found track with invalid perigee parameters. Not splitting." << endmsg; 
      return false;
    }
    
  //Trk::Perigee const* measuredperigee = dynamic_cast<Trk::Perigee const*>( trackPerigee );
  double const d0 = trackPerigee->parameters()[Trk::d0];

  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "do is: "<<  d0 << endmsg;

  /** This can be fleshed out, to have better track selection */

  //width of the cavity in the TRT 
  if( fabs(d0) < 600){
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "is a candidate" << endmsg;
    return true;
  }

  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "is NOT a candidate" << endmsg;
  return false;
}

/** Return a vector of the SCT hits on track 
    (used for not breaking up SCT space points, when splitting Odd/Even)
 */
std::vector<Trk::MeasurementBase const*> InDet::InDetTrackSplitterTool::getSCTHits(Trk::Track const& input) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getSCTHits " << endmsg;
  std::vector<Trk::MeasurementBase const*> SCTHits;

  DataVector<Trk::MeasurementBase const>::const_iterator meas = input.measurementsOnTrack()->begin();
  DataVector<Trk::MeasurementBase const>::const_iterator measEnd = input.measurementsOnTrack()->end(); 
  for(;meas != measEnd; ++meas){
        
    Trk::RIO_OnTrack const* rio = dynamic_cast<Trk::RIO_OnTrack const*>(*meas);
    
    if(rio){
      Identifier const& surfaceid = (rio->identify());
      if(m_trtid->is_sct(surfaceid)){
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "we've found an SCT hit " << endmsg;
  SCTHits.push_back(*meas);
      }
    }
  }

  //if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "sorting the hits " << endmsg;
  /** order the hits in y */
  //std::sort(SCTHits.begin(), SCTHits.end(), InDetTrackSplitterHelpers::CompareYPosition );
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving getSCTHits " << endmsg;
  return SCTHits;
}

/** 
    Logic to check if there is another SCT hit associated with the input hit, which forms a space point
    (used for not breaking up SCT space points, when splitting Odd/Even)
*/
std::vector<Trk::MeasurementBase const*>::iterator InDet::InDetTrackSplitterTool::findSCTHitsFromSameSpacePoint(Trk::MeasurementBase const* sctHit, std::vector<Trk::MeasurementBase const*>& listOfSCTHits) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in findSCTHitsFromSameSpacePoint " << endmsg;
  std::vector<Trk::MeasurementBase const*>::iterator result = listOfSCTHits.end();
  
  Trk::RIO_OnTrack const* rio = dynamic_cast<Trk::RIO_OnTrack const*>(sctHit);
  if(rio){
    Identifier const& surfaceid = (rio->identify());  
    
    int targetEta = m_sctid->eta_module(surfaceid);
    int targetPhi = m_sctid->phi_module(surfaceid);
    
    std::vector<Trk::MeasurementBase const*>::const_iterator meas = listOfSCTHits.begin();
    std::vector<Trk::MeasurementBase const*>::const_iterator measEnd = listOfSCTHits.end(); 
    for(;meas != measEnd; ++meas){
      Trk::RIO_OnTrack const* candidateRio = dynamic_cast<Trk::RIO_OnTrack const*>(*meas);
      if(candidateRio){
  Identifier const& candidateSurfaceid = (candidateRio->identify());
  if(m_sctid->eta_module(candidateSurfaceid) == targetEta && m_sctid->phi_module(candidateSurfaceid) == targetPhi){
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found another hit in the SpacePoint " << endmsg;
    result = find(listOfSCTHits.begin(), listOfSCTHits.end(), *meas);
  }
      }
    }
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving findSCTHitsFromSameSpacePoint " << endmsg;
  return result;
}

/** 
    Logic to check if the track is constrained given the number of various types of hits
 */
bool InDet::InDetTrackSplitterTool::isConstrained(unsigned int nPixelHits, unsigned int nSCTHits, unsigned int nTRTHits, unsigned int nPseudomeasurements) const{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In isConstrained " << endmsg;
  
  /** Two cases to consider, tracks with Si hits (first case in the ||), and TRT Only tracks (second case) 
   */
  if( (( nPixelHits + nSCTHits) > 1 && (2*nPixelHits + nSCTHits + nTRTHits)  > 5) ||
      (nPseudomeasurements >= 1 && nTRTHits > 10)){
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving isConstrained (true)" << endmsg;
    return true;
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving isConstrained (false)" << endmsg;
  return false;
}

