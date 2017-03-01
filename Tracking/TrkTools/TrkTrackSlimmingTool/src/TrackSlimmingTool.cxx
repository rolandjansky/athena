/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSlimmingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrackSlimmingTool/TrackSlimmingTool.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
  //================ Constructor =================================================

Trk::TrackSlimmingTool::TrackSlimmingTool(const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
AthAlgTool(t,n,p),
  m_keepCaloDeposit(true),
  m_keepOutliers(false),
  m_keepParameters(false),
  m_setPersistificationHints(false),
  m_detID{} 
{
  declareInterface<ITrackSlimmingTool>(this);

    //  template for property decalration
  declareProperty("KeepCaloDeposit",m_keepCaloDeposit,
    "If this is set to true, any CaloDeposit with its adjacent MEOT's will be kept on the slimmed track (combined muon property)");
  declareProperty("KeepOutliers",   m_keepOutliers,   "If this is set to true, Outliers will be kept on the slimmed track");
  declareProperty("KeepParameters", m_keepParameters, "If this is set to true, the first and last parameters will be kept on the slimmed track");
  declareProperty("OnlySetPersistificationHints", m_setPersistificationHints, "Only set persistification hints in each track state on surface");
}

  //================ Destructor =================================================

Trk::TrackSlimmingTool::~TrackSlimmingTool()
  {}


  //================ Initialisation =================================================

StatusCode Trk::TrackSlimmingTool::initialize()
{

  StatusCode sc = AlgTool::initialize();
  if (sc.isFailure()) return sc;

  sc = detStore()->retrieve(m_detID, "AtlasID" );
  if (sc.isFailure()) {
    ATH_MSG_FATAL ("Could not get AtlasDetectorID ");
    return sc;
  }else{
    ATH_MSG_DEBUG ("Found AtlasDetectorID");
  }

  msg(MSG::INFO) << "initialize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}

  //================ Finalisation =================================================

StatusCode Trk::TrackSlimmingTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

Trk::Track* Trk::TrackSlimmingTool::slim(const Trk::Track& track)
{
  // //std::cout<<"^^^^^^^^^^^ Input track is: "<<track<<std::endl;

  const DataVector<const TrackStateOnSurface>* oldTrackStates = track.trackStateOnSurfaces();
  if (oldTrackStates == 0)
  {
    msg(MSG::WARNING) << "Track has no TSOS vector! Skipping track, returning 0." << endmsg;
    return 0;
  }
  // create vector for new TSOS (the ones which are kept)
  DataVector<const TrackStateOnSurface>* trackStates = (!m_setPersistificationHints ? new DataVector<const TrackStateOnSurface> : nullptr); 

  // These variables are filled with pointers
  const TrackStateOnSurface* firstValidIDTSOS(0),* lastValidIDTSOS(0),* firstValidMSTSOS(0),* lastValidMSTSOS(0)/**,* lastIDMeasTSOS(0)**/;
  /**TrackStateOnSurface *copiedIDtsosWithMeas(0); **/
  
  if (m_keepParameters)
  {
    // search last valid TSOS first (as won't be found in later loop)
    for ( DataVector<const TrackStateOnSurface>::const_reverse_iterator rItTSoS = oldTrackStates->rbegin(); rItTSoS != oldTrackStates->rend(); ++rItTSoS)
    {
      if ( (*rItTSoS)->type(TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0 && !dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((*rItTSoS)->measurementOnTrack()))
      {
        if (m_detID->is_indet((*rItTSoS)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier() ) ) {
          lastValidIDTSOS = (*rItTSoS);
          break;
        } else if (m_detID->is_muon((*rItTSoS)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier() ) ) {
          lastValidMSTSOS = (*rItTSoS);
          break;
        }
      }
    }
  }
  
 

  // If m_keepParameters is true, then we want to keep the first and last parameters of ID & MS. 
  const Trk::MeasurementBase* rot = 0;
  const Trk::TrackParameters* parameters = 0;
  bool keepParameter=false;
  std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0); 

  bool isIDmeas = false; 
  bool isMSmeas = false;
  
//  unsigned int tsosNum=0;
   // looping over all TSOS
  DataVector<const TrackStateOnSurface>::const_iterator itTSoS = oldTrackStates->begin();
  for (   ; itTSoS!=oldTrackStates->end(); ++itTSoS)
  {
    if (m_setPersistificationHints) {
      (**itTSoS).setHint(Trk::TrackStateOnSurface::PartialPersistification);
    }
    parameters=0; 
    rot=0;
    //std::cout<<"Looking at TSOS#"<<tsosNum++<<" of type: "<<(**itTSoS).dumpType()<<std::endl;
    // if requested: keep calorimeter TSOS with adjacent scatterers (on combined muons)
    if (m_keepCaloDeposit && (**itTSoS).type(TrackStateOnSurface::CaloDeposit))
    {
        // preceding TSOS (if Scatterer)
      if (itTSoS != oldTrackStates->begin())
      {
        --itTSoS;
        if ((**itTSoS).type(TrackStateOnSurface::Scatterer))
        {
          if (m_setPersistificationHints) {
            (**itTSoS).resetHint(Trk::TrackStateOnSurface::PartialPersistification);
          }
          else {
            trackStates->push_back((**itTSoS).clone());
          }
          //std::cout<<"Keeping prior scatterer from TSOS "<<tsosNum-1<<std::endl;
        }
        ++itTSoS;
      }

        // copy removes CaloEnergy (just keep base EnergyLoss) 
        // (for explanation see bug #72762)
      const MaterialEffectsOnTrack* meot  =
        dynamic_cast<const MaterialEffectsOnTrack*>((**itTSoS).materialEffectsOnTrack());
      if (meot && meot->energyLoss())
      {
        if (m_setPersistificationHints) {
          // (**itTSoS).resetHint(Trk::TrackStateOnSurface::PartialPersistification);
          (**itTSoS).setHint(Trk::TrackStateOnSurface::PersistifySlimCaloDeposit);
          (**itTSoS).setHint(Trk::TrackStateOnSurface::PersistifyTrackParameters);
        }
        else {
        trackStates->push_back(
          new TrackStateOnSurface(0,
          (**itTSoS).trackParameters()->clone(),
          0,
          new MaterialEffectsOnTrack(meot->thicknessInX0(),
          0,
          new EnergyLoss(*meot->energyLoss()),
          meot->associatedSurface()),
          (**itTSoS).types()));
        }
      }

        // following TSOS (if Scatterer)
      ++itTSoS;
      if (itTSoS != oldTrackStates->end() && (**itTSoS).type(TrackStateOnSurface::Scatterer))
      {
        if (m_setPersistificationHints) {
          (**itTSoS).resetHint(Trk::TrackStateOnSurface::PartialPersistification);
        }
        else {
          trackStates->push_back((**itTSoS).clone());
        }
        //std::cout<<"Keeping post scatterer from TSOS "<<tsosNum+1<<std::endl;
        
      }
      --itTSoS;
    }

    // We only keep TSOS if they either contain a perigee, OR are a measurement
    if ((*itTSoS)->measurementOnTrack()==0 && !(*itTSoS)->type(TrackStateOnSurface::Perigee)) continue;

    typePattern.reset(); 

    isIDmeas=false; isMSmeas=false;
    checkForValidMeas((*itTSoS), isIDmeas, isMSmeas);
    
    // if ((itTSoS+1)!=oldTrackStates->end()) {
    //   checkIfInDet(*(itTSoS+1), nextIsIDmeas);
    // }
    
    // if (isIDmeas) std::cout<<"isIDmeas"<<std::endl;
    // if (isMSmeas) std::cout<<"isMSmeas"<<std::endl;

    keepParameter=false; // Will be used to decide whether to store parameter, after the following checks...

    if ( (*itTSoS)->trackParameters() != 0 && (*itTSoS)->type(TrackStateOnSurface::Perigee) ) keepParameter=true;

    // Now do checks for first/last ID/MS measurement (isIDmeas and isMSmeas)
    if (m_keepParameters){
    // entering ID? 
      if (isIDmeas && !firstValidIDTSOS && (*itTSoS)->type(TrackStateOnSurface::Measurement) ) {
        firstValidIDTSOS=*itTSoS;
        if ( (*itTSoS)->trackParameters() != 0) keepParameter=true;
        //std::cout<<"*** Entering ID - keeping parameter"<<std::endl;
      }
			/** coverity 13545
      // Check if we're no longer in ID, 
      if (!isIDmeas && !lastValidIDTSOS && lastIDMeasTSOS ) {
        lastValidIDTSOS=lastIDMeasTSOS;
        if (lastIDMeasTSOS->trackParameters() != 0) {
          copiedIDtsosWithMeas= 
             new (copiedIDtsosWithMeas) Trk::TrackStateOnSurface(copiedIDtsosWithMeas->measurementOnTrack(), lastIDMeasTSOS->trackParameters()->clone(),0, 0,copiedIDtsosWithMeas->types());
        } else {
          ATH_MSG_DEBUG("Last ID measurement didn't have a track parameter!");
          msg(MSG::DEBUG)<<*lastIDMeasTSOS<<endmsg;
        }        
      }
    **/
    
    // entering MS? 
      if (isMSmeas && !firstValidMSTSOS ) {
        firstValidMSTSOS=*itTSoS;
        if ( (*itTSoS)->trackParameters() != 0) keepParameter=true;
        //std::cout<<"*** Entering MS - keeping parameter"<<std::endl;
        
      }
      
      // Is this the last TSOS on the track?
      if (lastValidIDTSOS==(*itTSoS) || lastValidMSTSOS == (*itTSoS)){
        if ( (*itTSoS)->trackParameters() != 0) keepParameter=true;
        //std::cout<<"*** Last meas ID/MS - keeping parameter"<<std::endl;
      } 
    }

    if (keepParameter) {
      parameters=(m_setPersistificationHints
                  ? (*itTSoS)->trackParameters()
                  : (*itTSoS)->trackParameters()->clone()); // make sure we add a new parameter by cloning
      if ((*itTSoS)->type(TrackStateOnSurface::Perigee))  typePattern.set(TrackStateOnSurface::Perigee);
    }

    // It'd be nice if there was a way to access the bitset directly, so instead of the below I could
    // just do if ( referenceBitSet&(*itTSoS)->typeBitSet() )
    // EJWM        
    
    if ( (*itTSoS)->measurementOnTrack()!=0 &&  
      ( (*itTSoS)->type(TrackStateOnSurface::Measurement) || ( m_keepOutliers && (*itTSoS)->type(TrackStateOnSurface::Outlier) ) ) ) 
    { 
      if ((*itTSoS)->type(TrackStateOnSurface::Measurement))  typePattern.set(TrackStateOnSurface::Measurement);
      if ((*itTSoS)->type(TrackStateOnSurface::Outlier))      typePattern.set(TrackStateOnSurface::Outlier);
      rot=( m_setPersistificationHints
            ?(*itTSoS)->measurementOnTrack()
            :(*itTSoS)->measurementOnTrack()->clone() );

    } 
    
    Trk::TrackStateOnSurface* newTSOS = 0;
    if (rot!=0 || parameters!=0) {
      if (m_setPersistificationHints) {
        if (rot) {
          (**itTSoS).setHint(Trk::TrackStateOnSurface::PersistifyMeasurement);
        }
        if (parameters) {
          (**itTSoS).setHint(Trk::TrackStateOnSurface::PersistifyTrackParameters);
        }
      }
      else {
        newTSOS = new Trk::TrackStateOnSurface(rot, parameters, 0, 0, typePattern);
        trackStates->push_back( newTSOS );
      }
    } 
    /** coverity 13544
    if (isIDmeas && copiedIDtsosWithMeas) {
       copiedIDtsosWithMeas = newTSOS; // Keep track unless we need to add parameter to it, if it turns out to have been last ID TSOS
       lastIDMeasTSOS=*itTSoS;
     }
     **/
  }       

  if (!m_setPersistificationHints) {
  Trk::Track* newTrack = new Trk::Track(Trk::TrackInfo(track.info()),
    trackStates,
    track.fitQuality()->clone()
    );

  newTrack->info().setTrackProperties(TrackInfo::SlimmedTrack);

      // next could be done without cloning (but maybe accesses the tracksummary of the old track??)
      // only attempt to copy the summary if the track has it!
  if (track.trackSummary()!=0)
  {
    newTrack->m_trackSummary = new Trk::TrackSummary(*(track.trackSummary()));
  }
  //std::cout<<"_______________ Slimmed track is: "<<*newTrack<<std::endl;
  
  return newTrack;
  }
  else {
    return nullptr;
  }
}

void Trk::TrackSlimmingTool::checkForValidMeas(const Trk::TrackStateOnSurface* tsos, bool& isIDmeas, bool& isMSmeas){
  if (tsos->measurementOnTrack()!=0){
    bool isPseudo = (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(tsos->measurementOnTrack())!=0);
    // Handle horrible cROTs
    const Trk::CompetingRIOsOnTrack* cROT = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(tsos->measurementOnTrack());
    Identifier id;
    if (cROT) {
      id = cROT->rioOnTrack( cROT->indexOfMaxAssignProb() ).identify();
    } else {
      id = tsos->measurementOnTrack()->associatedSurface().associatedDetectorElementIdentifier();
    }    
    isIDmeas =  !isPseudo && m_detID->is_indet(id ); 
    isMSmeas = tsos->measurementOnTrack()!=0 && !isPseudo && m_detID->is_muon(id ); 
  }
}

void Trk::TrackSlimmingTool::checkIfInDet(const Trk::TrackStateOnSurface* tsos, bool& isIDmeas){
  if (tsos->trackParameters()) 
    isIDmeas=m_detID->is_indet(tsos->trackParameters()->associatedSurface().associatedDetectorElementIdentifier() );
  else {
    bool dummy; // Not so elegant.
    checkForValidMeas(tsos, isIDmeas, dummy);
  }
}

  //============================================================================================

