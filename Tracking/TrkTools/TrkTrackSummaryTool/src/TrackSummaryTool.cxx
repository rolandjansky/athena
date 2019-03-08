/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "Identifier/Identifier.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"
#include "TrkToolInterfaces/IPixelToTPIDTool.h"

#include "TRT_ToT_Tools/ITRT_ToT_dEdx.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkParameters/TrackParameters.h"

#include <cassert>
#include <vector>
#include <algorithm>

//============================================================================================

Trk::TrackSummaryTool::TrackSummaryTool(const std::string& t,
  const std::string& n,
  const IInterface*  p )
  :
AthAlgTool(t,n,p),
  m_doHolesMuon(false),
  m_doHolesInDet(false),
  m_doSharedHits(false),
  m_detID{},
  m_idTool(""),
  m_eProbabilityTool(""),
  m_trt_dEdxTool(""),
  m_dedxtool(""),
  m_muonTool(""),
  m_idHoleSearch(""),
  m_pixelExists(true)
{
  declareInterface<ITrackSummaryTool>(this);
  declareProperty("doSharedHits",               m_doSharedHits);
  declareProperty("doHolesMuon",                m_doHolesMuon);
  declareProperty("AddDetailedInDetSummary",    m_addInDetDetailedSummary=true);
  declareProperty("AddDetailedMuonSummary",     m_addMuonDetailedSummary=true);
  declareProperty("InDetSummaryHelperTool",     m_idTool);
  declareProperty("TRT_ElectronPidTool",        m_eProbabilityTool);
  declareProperty("TRT_ToT_dEdxTool",           m_trt_dEdxTool);
  declareProperty("PixelToTPIDTool",            m_dedxtool);
  declareProperty("MuonSummaryHelperTool",      m_muonTool);
  declareProperty("InDetHoleSearchTool",        m_idHoleSearch);
  declareProperty("PixelExists",                m_pixelExists);

  declareProperty("TRTdEdx_DivideByL",        (m_TRTdEdx_DivideByL=true) );
  declareProperty("TRTdEdx_useHThits",        (m_TRTdEdx_useHThits=true) );
  declareProperty("TRTdEdx_corrected",        (m_TRTdEdx_corrected=true) );
  declareProperty("minTRThitsForTRTdEdx",     (m_minTRThitsForTRTdEdx=1) );

}


//============================================================================================

Trk::TrackSummaryTool::~TrackSummaryTool()
{
}

//============================================================================================

StatusCode 
  Trk::TrackSummaryTool::initialize()
{
    // StatusCode sc=StatusCode::SUCCESS;
    //StatusCode sc = AlgTool::initialize();
    //if (sc.isFailure()) return sc;

    ATH_CHECK( detStore()->retrieve(m_detID, "AtlasID" ));

    if (m_idTool.empty() && m_muonTool.empty()) {
        ATH_MSG_ERROR ("Could get neither InDetHelperTool nor MuonHelperTool. Must abort.");
        return StatusCode::FAILURE;
    }

    if ( !m_idTool.empty() && m_idTool.retrieve().isFailure() ) {
        ATH_MSG_ERROR ("Failed to retrieve InDet helper tool "<< m_idTool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
        return StatusCode::FAILURE;
    } else {
        if ( !m_idTool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_idTool << endmsg;
    }

  // Troels.Petersen@cern.ch:
    if ( !m_eProbabilityTool.empty() && m_eProbabilityTool.retrieve().isFailure() ) 
    {
        ATH_MSG_ERROR ("Failed to retrieve electron probability tool " << m_eProbabilityTool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
        return StatusCode::FAILURE;
    } else { 
       if ( !m_eProbabilityTool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_eProbabilityTool << endmsg;
    }

    if (!m_trt_dEdxTool.empty()) {
      ATH_CHECK( m_trt_dEdxTool.retrieve() );
    }
 

    if ( !m_dedxtool.empty() && m_dedxtool.retrieve().isFailure() )
    {
        ATH_MSG_ERROR ("Failed to retrieve pixel dEdx tool " << m_dedxtool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
        return StatusCode::FAILURE;
    } else {
       if ( !m_dedxtool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_dedxtool << endmsg;
    }

    if ( !m_idHoleSearch.empty() ) {
        if (m_idHoleSearch.retrieve().isFailure() ) {
            ATH_MSG_ERROR ("Failed to retrieve InDet hole search tool "<< m_idHoleSearch);
            ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
            return StatusCode::FAILURE;
        } else {
            msg(MSG::INFO) << "Retrieved tool " << m_idHoleSearch<<endmsg;
            m_doHolesInDet = true;
        }
    }

    if ( !m_muonTool.empty() && m_muonTool.retrieve().isFailure() ) 
    {
        ATH_MSG_ERROR ("Failed to retrieve Muon helper tool "<< m_muonTool);
        ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
    } 
    else {
        if ( !m_muonTool.empty()) msg(MSG::INFO) << "Retrieved tool " << m_muonTool<<endmsg;
    }

    if (m_doHolesInDet) 
        ATH_MSG_INFO ("Search for InDet holes using external tool turned ON");
    else
        ATH_MSG_INFO ("Search for InDet holes using external tool turned OFF");

    if (m_doHolesMuon) 
        ATH_MSG_INFO ("Search for Muon holes using external tool turned ON");
    else
        ATH_MSG_INFO ("Search for Muon holes using external tool turned OFF");

    ATH_MSG_INFO ("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

//============================================================================================

StatusCode 
  Trk::TrackSummaryTool::finalize()
{
  //StatusCode sc = AlgTool::finalize();
  return StatusCode::SUCCESS;
}

//============================================================================================
const Trk::TrackSummary* Trk::TrackSummaryTool::createSummaryNoHoleSearch( const Track& track )  const
{
  return createSummary(track, false, false, false);
}

//============================================================================================

const Trk::TrackSummary* Trk::TrackSummaryTool::createSummary( const Track& track, bool onlyUpdateTrack ) const
{
  return createSummary( track, onlyUpdateTrack, m_doHolesInDet, m_doHolesMuon );
}

//============================================================================================

const Trk::TrackSummary*
Trk::TrackSummaryTool::createSummary( const Track& track,
                                      bool onlyUpdateTrack,
                                      bool doHolesInDet,
                                      bool doHolesMuon) const
{
  // first check if track has summary already and then return a clone
  // (remember the TrackSummaryTool is a factory!)
  if (track.trackSummary()!=0) {
    ATH_MSG_DEBUG ("Return cached summary for author : "<<track.info().dumpInfo());
    return new Trk::TrackSummary(*(track.trackSummary()));
  }
  // Create and initialize information vector and bit pattern
  unsigned int numberOfTrackSummaryTypes = Trk::numberOfTrackSummaryTypes+1;
  // put values to -1 of they are not evaluated
  std::vector<int> information(numberOfTrackSummaryTypes,-1);

  // Troels.Petersen@cern.ch:
  unsigned int numberOfeProbabilityTypes = Trk::numberOfeProbabilityTypes+1;
  std::vector<float> eProbability(numberOfeProbabilityTypes,0.5);

  float dedx=-1;
  int nhitsuseddedx=-1;
  int noverflowhitsdedx=-1;

  // Now set values to 0 for the ones we evaluate
  if (!m_idTool.empty()) {
    if (m_pixelExists)
    {
      information [numberOfContribPixelLayers]   = 0; 
      information [numberOfInnermostPixelLayerHits] = 0;
      information [numberOfInnermostPixelLayerOutliers] = 0;
      information [numberOfNextToInnermostPixelLayerHits] = 0;
      information [numberOfNextToInnermostPixelLayerOutliers] = 0;
      information [numberOfPixelHits]            = 0;
      information [numberOfPixelOutliers]        = 0;
      information [numberOfGangedPixels]         = 0;
      information [numberOfGangedFlaggedFakes]   = 0;
      information [numberOfPixelSpoiltHits]      = 0;
      information [numberOfGangedFlaggedFakes]   = 0;
      information [numberOfPixelSplitHits]       = 0;
      information [numberOfInnermostLayerSplitHits] = 0;
      information [numberOfNextToInnermostLayerSplitHits] = 0;
      if (track.info().trackFitter() != TrackInfo::Unknown && !m_dedxtool.empty()) {
        dedx = m_dedxtool->dEdx(track, nhitsuseddedx, noverflowhitsdedx);
      }
      information [Trk::numberOfDBMHits]                  = 0;
    }
    information [numberOfSCTHits]                  = 0;
    information [numberOfSCTSpoiltHits]            = 0;
    information [numberOfSCTOutliers]              = 0;
    information [numberOfTRTHits]                  = 0;
    information [numberOfTRTXenonHits]             = 0;
    information [numberOfTRTHighThresholdHits]     = 0;
    information [numberOfTRTHighThresholdHitsTotal]= 0;
    information [numberOfTRTOutliers]              = 0;
    information [numberOfTRTHighThresholdOutliers] = 0;
    information [numberOfTRTTubeHits]              = 0;

    // Troels.Petersen@cern.ch:
    if ( !m_eProbabilityTool.empty() ) {
      eProbability = m_eProbabilityTool->electronProbability(track);
    }
  }


  if (m_doSharedHits) {
    information [numberOfSCTSharedHits]      = 0;
    if (m_pixelExists) {
      information [numberOfInnermostPixelLayerSharedHits] = 0;
      information [numberOfNextToInnermostPixelLayerSharedHits] = 0;
      information [numberOfPixelSharedHits]  = 0;
    }
  }
  if (!m_muonTool.empty()) {
    information [numberOfMdtHits]    = 0;
    information [numberOfTgcPhiHits] = 0;
    information [numberOfTgcEtaHits] = 0;
    information [numberOfCscPhiHits] = 0;
    information [numberOfCscEtaHits] = 0;
    information [numberOfCscUnspoiltEtaHits] = 0;
    information [numberOfRpcPhiHits] = 0;
    information [numberOfRpcEtaHits] = 0;
    information[Trk::numberOfMdtHoles] =0; // no matter what, we either use an external tool or count holes on track, so set zero
    information[Trk::numberOfCscEtaHoles] =0;
    information[Trk::numberOfCscPhiHoles] =0;
    information[Trk::numberOfRpcEtaHoles] =0;
    information[Trk::numberOfRpcPhiHoles] =0;
    information[Trk::numberOfTgcEtaHoles] =0;
    information[Trk::numberOfTgcPhiHoles] =0;
    // New Small Wheel
    information[Trk::numberOfStgcEtaHits] =0;
    information[Trk::numberOfStgcPhiHits] =0;
    information[Trk::numberOfMmHits] =0;    
    information[Trk::numberOfStgcEtaHoles] =0;
    information[Trk::numberOfStgcPhiHoles] =0;
    information[Trk::numberOfMmHoles] =0;
  }

  std::bitset<numberOfDetectorTypes> hitPattern;

  ATH_MSG_DEBUG ("Produce summary for: "<<track.info().dumpInfo());

  if (track.trackStateOnSurfaces()!=0)
  {
    information[Trk::numberOfOutliersOnTrack] = 0;
    processTrackStates(track,track.trackStateOnSurfaces(), information, hitPattern,
                       doHolesInDet, doHolesMuon);
  }else{
    ATH_MSG_WARNING ("Null pointer to TSoS found on Track (author = "
      <<track.info().dumpInfo()<<"). This should never happen! ");
  }

  if (doHolesInDet || doHolesMuon) 
  {
    if (m_pixelExists) 
    {
      information [numberOfPixelHoles] = 0; 
    }
    information [numberOfSCTHoles]       = 0; 
    information [numberOfSCTDoubleHoles] = 0;
    searchHolesStepWise(track,information, doHolesInDet, doHolesMuon);
  }

  if (!m_trt_dEdxTool.empty()) {
    if (information[Trk::numberOfTRTHits]+information[Trk::numberOfTRTOutliers]>=m_minTRThitsForTRTdEdx) {
      int nhits = static_cast<int>( m_trt_dEdxTool->usedHits(&track, m_TRTdEdx_DivideByL, m_TRTdEdx_useHThits) );
      double fvalue = (nhits>0 ? m_trt_dEdxTool->dEdx(&track, m_TRTdEdx_DivideByL, m_TRTdEdx_useHThits, m_TRTdEdx_corrected) : 0.0);
      eProbability.push_back(fvalue);
      information[ numberOfTRTHitsUsedFordEdx] = static_cast<uint8_t>(std::max(nhits,0));
    }
    else {
      information[ numberOfTRTHitsUsedFordEdx]=0;
      eProbability.push_back(0.0);
    }
  }
  else {
    eProbability.push_back(0.0);
  }
  
  TrackSummary* ts = new TrackSummary(information,eProbability,hitPattern,dedx,nhitsuseddedx,noverflowhitsdedx);

  // add detailed summary for indet
  if( m_addInDetDetailedSummary && !m_idTool.empty() ){
    m_idTool->addDetailedTrackSummary(track,*ts);
  }

  // add detailed summary for muons
  if( m_addMuonDetailedSummary && !m_muonTool.empty() ){
    m_muonTool->addDetailedTrackSummary(track,*ts);
  }

  // move this part to VERBOSE
  ATH_MSG_VERBOSE ( *ts << endmsg << "Finished!");

  Trk::Track& nonConstTrack = const_cast<Trk::Track&>(track);
  if (onlyUpdateTrack) {
    // not returning summary, so add it to the track
    nonConstTrack.m_trackSummary = ts;
    ts=0; // returning zero
  } else {
    // need to return summary too, so add a copy to the track
    nonConstTrack.m_trackSummary = new Trk::TrackSummary(*ts);
  }
  
  return ts;
}

void Trk::TrackSummaryTool::updateTrack(Track& track) const
{
  // first check if track has summary already.
  if (track.m_trackSummary!=0) {
    delete track.m_trackSummary;
    track.m_trackSummary = 0;
  }

  createSummary( track, true );
  return;
}

void Trk::TrackSummaryTool::updateSharedHitCount(Track& track) const
{
  // first check if track has no summary - then it is recreated
  if (track.m_trackSummary==0) {
      createSummary( track, true );
      return;
  } 
  Trk::TrackSummary* tSummary = const_cast<Trk::TrackSummary*>(track.m_trackSummary);
  m_idTool->updateSharedHitCount(track, *tSummary);
  return;
}

void Trk::TrackSummaryTool::updateAdditionalInfo(Track& track) const
{
  // first check if track has no summary - then it is recreated
  if (track.m_trackSummary==0) {
      createSummary( track, true );
      return;
  } 
  Trk::TrackSummary* tSummary = const_cast<Trk::TrackSummary*>(track.m_trackSummary);
  
  unsigned int numberOfeProbabilityTypes = Trk::numberOfeProbabilityTypes+1;
  std::vector<float> eProbability(numberOfeProbabilityTypes,0.5); 
  if ( !m_eProbabilityTool.empty() ) eProbability = m_eProbabilityTool->electronProbability(track);
 
  if (!m_trt_dEdxTool.empty()) {
    if (tSummary->get(Trk::numberOfTRTHits)+tSummary->get(Trk::numberOfTRTOutliers)>=m_minTRThitsForTRTdEdx) {
      int nhits = static_cast<int>( m_trt_dEdxTool->usedHits(&track, m_TRTdEdx_DivideByL, m_TRTdEdx_useHThits) );
      double fvalue = (nhits>0 ? m_trt_dEdxTool->dEdx(&track, m_TRTdEdx_DivideByL, m_TRTdEdx_useHThits, m_TRTdEdx_corrected) : 0.0);
      eProbability.push_back(fvalue);
      if (!tSummary->update(Trk::numberOfTRTHitsUsedFordEdx, static_cast<uint8_t>(std::max(nhits,0)) )) {
        ATH_MSG_WARNING( "Attempt to update numberOfTRTHitsUsedFordEdx but this summary information is "
                         "already set. numberOfTRTHitsUsedFordEdx is:" << tSummary->get(numberOfTRTHitsUsedFordEdx)
                         << " =?= should:" << nhits );
      }
    }
    else {
      eProbability.push_back(0.0);
      if (!tSummary->update(Trk::numberOfTRTHitsUsedFordEdx, 0) ) {
        ATH_MSG_WARNING( "Attempt to update numberOfTRTHitsUsedFordEdx but this summary information is "
                         "already set. numberOfTRTHitsUsedFordEdx is:" << tSummary->get(numberOfTRTHitsUsedFordEdx)
                         << " =?= should:" << 0 );
      }
    }
  }

  float dedx=0;
  int nhitsuseddedx=0;
  int noverflowhitsdedx=0;

  if (track.info().trackFitter() != TrackInfo::Unknown && !m_dedxtool.empty()) {
    dedx = m_dedxtool->dEdx(track, nhitsuseddedx, noverflowhitsdedx);
  }

  m_idTool->updateAdditionalInfo(*tSummary, eProbability,dedx, nhitsuseddedx,noverflowhitsdedx);
  
  m_idTool->updateSharedHitCount(track, *tSummary);

   m_idTool->updateExpectedHitInfo(track, *tSummary);
  
  if (m_addInDetDetailedSummary) m_idTool->addDetailedTrackSummary(track,*tSummary);
  return;
}


//============================================================================================

void Trk::TrackSummaryTool::processTrackStates(const Track& track,
					       const DataVector<const TrackStateOnSurface>* tsos,
					       std::vector<int>& information,
					       std::bitset<numberOfDetectorTypes>& hitPattern,
                                               bool doHolesInDet,
                                               bool doHolesMuon) const
{
  ATH_MSG_DEBUG ("Starting to process " << tsos->size() << " track states");

  int measCounter = 0, cntAddChi2 = 0;
  float chi2Sum = 0, chi2Sum2 = 0;
  DataVector<const TrackStateOnSurface>::const_iterator it = tsos->begin(), itEnd = tsos->end();
  for ( ; it!=itEnd; ++it){
    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) || (*it)->type(Trk::TrackStateOnSurface::Outlier)){
      ++measCounter;
      const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
      if (!measurement) {
        ATH_MSG_WARNING ("measurementOnTrack == Null for a TrackStateOnSurface "
          << "of type Measurement or Outlier");
      } else {
        if ((*it)->type(Trk::TrackStateOnSurface::Outlier)) ++information[Trk::numberOfOutliersOnTrack]; // increment outlier counter
        ATH_MSG_VERBOSE ("analysing TSoS " << measCounter << " of type " << (*it)->dumpType() );
        processMeasurement(track, measurement, *it, information, hitPattern);
      } // if have measurement pointer
    } // if type measurement, scatterer or outlier

    if ((*it)->type(Trk::TrackStateOnSurface::Measurement) &&
      (*it)->fitQualityOnSurface() &&
      (*it)->fitQualityOnSurface()->numberDoF()>0 )
    {
      ++cntAddChi2;
      if ((*it)->fitQualityOnSurface()->chiSquared() > 1.e5) {// limit unphysical values and protect against FPE
        chi2Sum += 1.e5;
        chi2Sum2 += 1.e10;
        ATH_MSG_DEBUG ("TSOS has unphysical chi2: "<< (*it)->fitQualityOnSurface()->chiSquared());
      } else {
        float chi2add =(*it)->fitQualityOnSurface()->chiSquared()/
          (*it)->fitQualityOnSurface()->numberDoF();
        chi2Sum+=chi2add;
        chi2Sum2+=chi2add*chi2add;
      }
    }

    if ( (*it)->type(Trk::TrackStateOnSurface::Hole) && (*it)->trackParameters() ){
      if (!doHolesInDet || !doHolesMuon ){ // no dedicated hole search via extrapolation, but take what might be on the track already.
        if ( (*it)->trackParameters()->associatedSurface().associatedDetectorElement()!=0 ) {
          const Identifier& id = (*it)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
          if ( !doHolesInDet && m_detID->is_pixel( id ) ) ++information[Trk::numberOfPixelHoles];
          if ( !doHolesInDet && m_detID->is_sct( id ) )    ++information[Trk::numberOfSCTHoles];
          if ( !doHolesMuon && m_detID->is_mdt( id ) )    ++information[Trk::numberOfMdtHoles];
        }
      }
    }
  } // end loop

  float varChi2 = 0;
  if (cntAddChi2>0) varChi2=chi2Sum2/cntAddChi2 - (chi2Sum/cntAddChi2) *(chi2Sum/cntAddChi2) ;
  if (varChi2>0 && varChi2<1.e13) information[Trk::standardDeviationOfChi2OS] = int(sqrt(varChi2)*100);

  return;
}

void Trk::TrackSummaryTool::processMeasurement(const Track& track,
					       const Trk::MeasurementBase* meas,
					       const Trk::TrackStateOnSurface* tsos,
					       std::vector<int>& information,
					       std::bitset<numberOfDetectorTypes>& hitPattern) const
{
  const RIO_OnTrack* rot = dynamic_cast<const RIO_OnTrack*> (meas);
  
  if ( rot ){
    // have RIO_OnTrack
    const Trk::ITrackSummaryHelperTool* tool = getTool(rot->identify());
    if (tool==0){
      msg(MSG::WARNING)<<"Cannot find tool to match ROT. Skipping."<<endmsg;
    } else {

      tool->analyse(track,rot,tsos,information, hitPattern);
    }
  } else {
    // Something other than a ROT.
    const Trk::CompetingRIOsOnTrack *compROT = 
      dynamic_cast<const Trk::CompetingRIOsOnTrack*>(meas);
    if (compROT) {
      // if this works we have a CompetingRIOsOnTrack.
      rot = &compROT->rioOnTrack(0); // get 1st rot
      const Trk::ITrackSummaryHelperTool* tool = getTool(rot->identify()); // Use 'main' ROT to get detector type
      if (tool==0){
        msg(MSG::WARNING)<<"Cannot find tool to match cROT. Skipping."<<endmsg;
      } else {
        tool->analyse(track,compROT,tsos,information, hitPattern);
      }
    }
  }
}

Trk::ITrackSummaryHelperTool*  
Trk::TrackSummaryTool::getTool(const Identifier& id)
{
  if (m_detID->is_indet(id)){
    if (!m_idTool.empty()){
      return &*m_idTool;
    } else { 
      msg(MSG::WARNING)<<"getTool: Identifier is from ID but have no ID tool"<<endmsg;
    }
  } else if(m_detID->is_muon(id)) {
    if (!m_muonTool.empty()) {
      return &*m_muonTool;
    } else {
      msg(MSG::WARNING)<<"getTool: Identifier is from Muon but have no Muon tool"<<endmsg;
    }
  } else {
    msg(MSG::WARNING) <<"getTool: Identifier is of unknown type! id: "<<id.getString();
  }
  return 0;
}

const Trk::ITrackSummaryHelperTool*  
Trk::TrackSummaryTool::getTool(const Identifier& id) const
{
  if (m_detID->is_indet(id)){
    if (!m_idTool.empty()){
      return &*m_idTool;
    } else { 
      msg(MSG::WARNING)<<"getTool: Identifier is from ID but have no ID tool"<<endmsg;
    }
  } else if(m_detID->is_muon(id)) {
    if (!m_muonTool.empty()) {
      return &*m_muonTool;
    } else {
      msg(MSG::WARNING)<<"getTool: Identifier is from Muon but have no Muon tool"<<endmsg;
    }
  } else {
    msg(MSG::WARNING) <<"getTool: Identifier is of unknown type! id: "<<id.getString();
  }
  return 0;
}

//============================================================================================

void Trk::TrackSummaryTool::searchHolesStepWise( const Trk::Track& track,
                                                 std::vector<int>& information,
                                                 bool doHolesInDet,
                                                 bool doHolesMuon) const
{

  ATH_MSG_VERBOSE ("Entering Trk::TrackSummaryTool::searchHolesStepWise");

// -------- **Initialize stepwise Extrapolation**


// -------- obtain hits in Pixel and SCT only

  if (track.trackStateOnSurfaces()==0) 
  {
    ATH_MSG_DEBUG ("No trackStatesOnSurface!!!!");
    information [numberOfPixelHoles]           = -1;
    information [numberOfPixelDeadSensors]     = -1; 
    information [numberOfSCTHoles]             = -1;
    information [numberOfSCTDoubleHoles]       = -1;
    information [numberOfSCTDeadSensors]       = -1;
    information [numberOfTRTHoles]             = -1;
    information [numberOfTRTDeadStraws]        = -1;
    // NOTE: Eta holes was used twice instead of Phi holes
    information [numberOfCscEtaHoles]          = -1;
    information [numberOfCscPhiHoles]          = -1;    
    information [numberOfRpcEtaHoles]          = -1;
    information [numberOfRpcPhiHoles]          = -1;    
    information [numberOfTgcEtaHoles]          = -1;
    information [numberOfTgcPhiHoles]          = -1;
    // New Small Wheel
    information [numberOfStgcEtaHoles]         = -1;
    information [numberOfStgcPhiHoles]         = -1; 
    information [numberOfMmHoles]              = -1;
    return;
  }
  else
  {
    if (doHolesInDet)
    {
      // -------- perform the InDet hole search
      if (m_pixelExists) {
        information [numberOfPixelHoles]           = 0;
        information [numberOfPixelDeadSensors]     = 0;
      } 
      information [numberOfSCTHoles]             = 0;    
      information [numberOfSCTDoubleHoles]       = 0;
      information [numberOfSCTDeadSensors]       = 0;
      //information [numberOfTRTHoles]             = 0;  
      //information [numberOfTRTDeadStraws]        = 0;  

      // ME : revert to take the summary helper, this is a temporary thing for 16.0.X
      m_idTool->searchForHoles(track,information,Trk::pion);
      //m_idHoleSearch->countHoles(track,information,Trk::pion) ; //TC: shouldn't it take the particle hypothesis from the track?

    }
    if (!m_muonTool.empty() && doHolesMuon)
    {
// now do Muon hole search. It works completely differently to the above, so we need to make this all a bit more general
// and probably more efficient. But this hopefully works for now! EJWM
      information [numberOfMdtHoles]             = 0;
      information [numberOfCscEtaHoles]          = 0;
      information [numberOfCscPhiHoles]          = 0;    
      information [numberOfRpcEtaHoles]          = 0;
      information [numberOfRpcPhiHoles]          = 0;    
      information [numberOfTgcEtaHoles]          = 0;
      information [numberOfTgcPhiHoles]          = 0;
      // New Small Wheel
      information [numberOfStgcEtaHoles]         = 0;
      information [numberOfStgcPhiHoles]         = 0;  
      information [numberOfMmHoles] = 0;        
      m_muonTool->searchForHoles(track,information,Trk::muon) ;
    }
  }

  return;
}





//=========================================================================================================================================

