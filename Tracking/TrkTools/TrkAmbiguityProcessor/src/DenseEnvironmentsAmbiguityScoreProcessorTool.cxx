/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DenseEnvironmentsAmbiguityScoreProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackInfo.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <map>
#include <iterator>
#include "TString.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetIdentifier/PixelID.h"
#include "AmbiguityProcessorUtility.h"

//==================================================================================================
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::DenseEnvironmentsAmbiguityScoreProcessorTool(const std::string& t, 
                                const std::string& n,
                                const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
  m_selectionTool("InDet::InDetDenseEnvAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"),
  m_splitProbTool("InDet::NnPixelClusterSplitProbTool/NnPixelClusterSplitProbTool"),
  m_etaBounds{0.8, 1.6, 2.5,4.0},
  m_stat(m_etaBounds)
{

  declareInterface<ITrackAmbiguityScoreProcessorTool>(this);
  declareProperty("ScoringTool"          , m_scoringTool);
  declareProperty("SelectionTool"        , m_selectionTool);
  declareProperty("SplitProbTool"        , m_splitProbTool);
  declareProperty("SplitClusterMap_old"  , m_splitClusterMapKey_last);
  declareProperty("SplitClusterMap_new"  , m_splitClusterMapKey);
  declareProperty("sharedProbCut"        , m_sharedProbCut           = 0.3);
  declareProperty("sharedProbCut2"       , m_sharedProbCut2          = 0.3);
  declareProperty("etaBounds"            , m_etaBounds,"eta intervals for internal monitoring");

}
//==================================================================================================

Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::~DenseEnvironmentsAmbiguityScoreProcessorTool(){
}
//==================================================================================================

StatusCode 
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::initialize(){
  StatusCode sc = StatusCode::SUCCESS;
  ATH_CHECK( m_scoringTool.retrieve());
  ATH_CHECK( m_assoTool.retrieve()) ;
  ATH_CHECK( m_assoToolNotGanged.retrieve( DisableTool{m_assoToolNotGanged.empty()} )) ;
  ATH_CHECK( m_assoMapName.initialize(!m_assoMapName.key().empty()) );

  ATH_CHECK( m_selectionTool.retrieve());

  ATH_CHECK(m_splitProbTool.retrieve( DisableTool{m_splitProbTool.empty()} ));

  ATH_CHECK( m_splitClusterMapKey_last.initialize(!m_splitClusterMapKey_last.key().empty()) );
  ATH_CHECK( m_splitClusterMapKey.initialize(!m_splitClusterMapKey.key().empty()) );

  if (m_etaBounds.size() != Counter::nRegions) {
     ATH_MSG_FATAL("There must be exactly " << (Counter::nRegions) << " eta bounds but "
                   << m_etaBounds.size() << " are set." );
     return StatusCode::FAILURE;
  }

  return sc;
}
//==================================================================================================

StatusCode 
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::finalize(){
  return StatusCode::SUCCESS;
}

void 
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::statistics() {
   if (msgLvl(MSG::INFO)) {
      MsgStream &out=msg(MSG::INFO);
      out << " -- statistics " << "\n";
      std::lock_guard<std::mutex> lock( m_statMutex );
      dumpStat(out);
      out << endmsg;
   }
}


//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

void 
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::process(const TrackCollection & tracks,
                                                    Trk::TracksScores* trackScoreTrackMap) const{
  InDet::PixelGangedClusterAmbiguities *splitClusterMap = nullptr;
  if(!m_splitClusterMapKey.key().empty()){
    if(!m_splitClusterMapKey_last.key().empty()){
      SG::ReadHandle<InDet::PixelGangedClusterAmbiguities> splitClusterMapHandle_last(m_splitClusterMapKey_last);
      if ( !splitClusterMapHandle_last.isValid() ){
        ATH_MSG_ERROR("Could not read last splitClusterMap.");
      }
      splitClusterMap =  new InDet::PixelGangedClusterAmbiguities(*splitClusterMapHandle_last);
    }
    else{
      splitClusterMap =  new InDet::PixelGangedClusterAmbiguities();
    }
  }
  addNewTracks(tracks, trackScoreTrackMap);
  std::unique_ptr<Trk::PRDtoTrackMap> prdToTrackMap( m_assoToolNotGanged.isEnabled()
                                                        ? m_assoToolNotGanged->createPRDtoTrackMap()
                                                        : m_assoTool->createPRDtoTrackMap() );
  overlappingTracks(trackScoreTrackMap, splitClusterMap, *prdToTrackMap);
  if (!m_assoMapName.key().empty()) {
     if (SG::WriteHandle<Trk::PRDtoTrackMap>(m_assoMapName).record(
                (m_assoToolNotGanged.isEnabled()
                 ? m_assoToolNotGanged->reduceToStorableMap(std::move(prdToTrackMap))
                 : m_assoTool->reduceToStorableMap(std::move(prdToTrackMap)) )).isFailure()) {
        ATH_MSG_FATAL("Failed to add PRD to track association map " << m_assoMapName.key() << ".");
     }
  }
  if(!m_splitClusterMapKey.key().empty()){
    SG::WriteHandle<InDet::PixelGangedClusterAmbiguities> splitClusterMapHandle(m_splitClusterMapKey);
    splitClusterMapHandle = std::unique_ptr<InDet::PixelGangedClusterAmbiguities>(splitClusterMap);
    if( !splitClusterMapHandle.isValid() ){
      ATH_MSG_ERROR("Could not record splitClusterMap.");
    }
  }
}


//==================================================================================================
void 
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::addNewTracks(const TrackCollection & tracks,
                                                          Trk::TracksScores* trackScoreTrackMap) const{
  Counter stat(m_etaBounds);
  stat.newEvent();
  std::unique_ptr<Trk::PRDtoTrackMap> prdToTrackMap( m_assoTool->createPRDtoTrackMap() );
  PrdSignatureSet prdSigSet; 
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks.size());
  const std::array<ScoreCategory, 3> categoryMapping {ScoreCategory::kNcandScoreZero, ScoreCategory::kNcandDouble, ScoreCategory::kNaccept};
  constexpr bool dropDuplicateTracks{true};
  for(const Track* pThisTrack : tracks) {
    ATH_MSG_VERBOSE ("Processing track candidate "<<pThisTrack);
    stat.incrementCounterByRegion(ScoreCategory::kNcandidates,pThisTrack); // @TODO should go to the score processor
    TrackScore score = m_scoringTool->score( *pThisTrack, true);
    const auto category = AmbiguityProcessor::categoriseTrack(*pThisTrack, score, dropDuplicateTracks, m_assoTool, *prdToTrackMap, prdSigSet);
    if (category<categoryMapping.size()) stat.incrementCounterByRegion(categoryMapping[category],pThisTrack);
    ATH_MSG_DEBUG(AmbiguityProcessor::debugMessage[category]);
    if (category == AmbiguityProcessor::TrackAccepted){
      ATH_MSG_VERBOSE ("Track  ("<< pThisTrack <<") has score "<<score);
      trackScoreTrackMap->push_back(std::make_pair(pThisTrack, -score));
    }
  }
  {
     std::lock_guard<std::mutex> lock(m_statMutex);
     m_stat += stat;
  }
}

//==================================================================================================
void 
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::updatePixelSplitInformationForCluster(const std::pair<const InDet::PixelCluster* const,
                                                                                             const Trk::TrackParameters*> & clusterTrkPara,
                                                                                              InDet::PixelGangedClusterAmbiguities *splitClusterMap) const{

  // Recalculate the split prob with the use of the track parameters
  InDet::PixelClusterSplitProb splitProb = m_splitProbTool->splitProbability( *clusterTrkPara.first, *clusterTrkPara.second );
  // update the split prob information on the cluster --  the use of the split flag is now questionable -- possible it will now indicate if the cluster is shared between multiple tracks
  InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*> ( clusterTrkPara.first );    
  //TODO: const_cast?

  ATH_MSG_DEBUG (  "---- "<< pixelCluster->globalPosition().perp() 
                             <<" Updating split probs 1:  Old " << pixelCluster->splitProbability1() << "  New " << splitProb.splitProbability(2) 
                             <<" Probs 2:  Old " << pixelCluster->splitProbability2() << "  New " << splitProb.splitProbability(3) 
                             << "\n"
                             << " --- pixelCluster: " <<  *pixelCluster
                             << "\n"
                             << " --- trk params: " << *clusterTrkPara.second  );

  if ( splitProb.splitProbability(2)  < 0 ){
    pixelCluster->packSplitInformation( false, 0.0, 0.0 );    
    pixelCluster->setTooBigToBeSplit( true );    
  } else {  
    pixelCluster->packSplitInformation( false, splitProb.splitProbability(2), splitProb.splitProbability(3) ) ;
    pixelCluster->setTooBigToBeSplit( false );    
  }
  
  if(splitClusterMap){
    if(  pixelCluster->splitProbability2()  >=  m_sharedProbCut2){
      splitClusterMap->insert(std::make_pair( pixelCluster, pixelCluster ) );
      splitClusterMap->insert(std::make_pair( pixelCluster, pixelCluster ) );
    } else if ( pixelCluster->splitProbability1()  >=  m_sharedProbCut ){  
      splitClusterMap->insert(std::make_pair( pixelCluster, pixelCluster ) );
    }
  }

}

//==================================================================================================
void 
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::overlappingTracks(const TracksScores* scoredTracks,
                                                                          InDet::PixelGangedClusterAmbiguities *splitClusterMap,
                                                                          Trk::PRDtoTrackMap &prdToTrackMap) const{
  const Trk::IPRDtoTrackMapTool *the_asso_tool = (m_assoToolNotGanged.isEnabled() ? &(*m_assoToolNotGanged) : &(*m_assoTool));
  // Function currnetly does nothing useful expect for printout debug information
  ATH_MSG_DEBUG ("Starting to resolve overlapping tracks");
  // Map to add all pixel clusters on track to
  std::map< const InDet::PixelCluster*, const Trk::TrackParameters* > setOfPixelClustersOnTrack;
  std::map< const InDet::PixelCluster*, const Trk::Track* > setOfPixelClustersToTrackAssoc;
  // Fill pixel cluster into the above map
  // Fill all PRD infromation into the association tool
  for( const std::pair<const Track*, float>& scoredTracksItem : *scoredTracks ){
    // clean it out to make sure not to many shared hits
    ATH_MSG_VERBOSE ("--- Adding next track "<<scoredTracksItem.first
                     << ":" << (scoredTracksItem.first->trackParameters() ? scoredTracksItem.first->trackParameters()->front()->pT() : -1.)
                     << ", " << scoredTracksItem.first->measurementsOnTrack()->size()
                     <<"\t with score "<<-scoredTracksItem.second << " to PRD map");
    
    //  This should only be done in region defined by Jets 
    //  ..... for now let do the whole detector coudl be slow
    if(the_asso_tool->addPRDs( prdToTrackMap, *scoredTracksItem.first ).isSuccess()){
      ATH_MSG_VERBOSE("--- Added hits to the association tool");
    } else {
      ATH_MSG_VERBOSE("--- Failed to add hits to the association tool");
      continue;    
    }  
    // get all prds on 'track'
    const DataVector<const TrackStateOnSurface>* tsosVec = scoredTracksItem.first->trackStateOnSurfaces();  
    if(!tsosVec){
      ATH_MSG_WARNING("TSOS vector does not exist");
      continue;   
    }
    ATH_MSG_VERBOSE("---> Looping over TSOS's to allow  for cluster updates: "<< tsosVec->size() );
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos = tsosVec->begin();
    for (; tsos != tsosVec->end(); ++tsos) {
      const MeasurementBase* measurement = (*tsos)->measurementOnTrack();
      if(!measurement || ! (*tsos)->trackParameters()){
        ATH_MSG_VERBOSE("---- TSOS has either no measurement or parameters: "<< measurement << "  " << (*tsos)->trackParameters() );
        continue;
      }
      if (!measurement->type(Trk::MeasurementBaseType::RIO_OnTrack)) continue;
      const Trk::RIO_OnTrack* rio = static_cast<const Trk::RIO_OnTrack*>(measurement);
      if (rio->rioType(Trk::RIO_OnTrackType::PixelCluster)) {
         const InDet::PixelCluster* pixel = static_cast<const InDet::PixelCluster*> ( rio->prepRawData() );
         assert( pixel);
         //Update the pixel split information if the element is unique (The second element of the pair indiciates if the element was inserted into the map)
         auto ret =  setOfPixelClustersOnTrack.insert(std::make_pair( pixel, (*tsos)->trackParameters() ));
         if (ret.second && m_splitProbTool.isEnabled()) {
            updatePixelSplitInformationForCluster( *(ret.first), splitClusterMap);
         }
         setOfPixelClustersToTrackAssoc.insert( std::make_pair( pixel, scoredTracksItem.first ) );
      }
    }
  }
  // now loop as long as map is not empty
  std::vector< std::pair< const InDet::PixelCluster*, const Trk::Track* > > sorted;
  sorted.reserve( setOfPixelClustersToTrackAssoc.size() );
  for( const std::pair< const InDet::PixelCluster* const, const Trk::Track* > &pixelTrackItem : setOfPixelClustersToTrackAssoc ) {
    sorted.emplace_back(pixelTrackItem );
  }
  std::sort( sorted.begin(), sorted.end(), [](const std::pair< const InDet::PixelCluster*, const Trk::Track* > &a,
                                              const std::pair< const InDet::PixelCluster*, const Trk::Track* > &b) {
               return a.first->getHashAndIndex().collHash() < b.first->getHashAndIndex().collHash()
                 || ( a.first->getHashAndIndex().collHash() == b.first->getHashAndIndex().collHash()
                      &&  a.first->getHashAndIndex().objIndex() < b.first->getHashAndIndex().objIndex() );
  });
  for (const std::pair< const InDet::PixelCluster*, const Trk::Track* >  &pixelTrackItem :  sorted) {
    ATH_MSG_VERBOSE ("---- Checking if track shares pixel hits if other tracks: " << pixelTrackItem.first << " with R " << pixelTrackItem.first->globalPosition().perp() );
    // find out how many tracks use this hit already
    Trk::PRDtoTrackMap::ConstPrepRawDataTrackMapRange range = prdToTrackMap.onTracks( *pixelTrackItem.first );
    int numberOfTracksWithThisPrd = std::distance(range.first,range.second);
    if (msgLvl(MSG::VERBOSE)) {
      TString tracks("---- number of tracks with this shared Prd: ");
      tracks += numberOfTracksWithThisPrd;
      for (Trk::IPRD_AssociationTool::ConstPRD_MapIt it =range.first; it != range.second;++it ){
        tracks += "    ";
        tracks += Form( " %p",(void*)(it->second));
        double pt = (it->second->trackParameters() ? it->second->trackParameters()->front()->pT() : -1);
        tracks += Form(":%.3f", pt);
        tracks += Form(",%i",static_cast<int>(it->second->measurementsOnTrack()->size()));
      }
      ATH_MSG_VERBOSE (tracks);
    }
  }
}

void
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::dumpStat(MsgStream &out) const
{
   auto parseFileName=[](const std::string & fullname){
     auto dotPosition = fullname.rfind('.');
     auto slashPosition = fullname.rfind('/');
     auto stringLength = dotPosition - slashPosition;
     return fullname.substr(slashPosition, stringLength);
   };
   std::streamsize ss = out.precision();
   int iw=9;
   out << "Output from ";
   out << parseFileName(__FILE__);
   out << "::";
   out << __func__;
   out << "\n";
   out << "------------------------------------------------------------------------------------" << "\n";
   out << "  Number of events processed      :   "<< m_stat.globalCount(Counter::nEvents) << "\n";
   if (m_stat.globalCount(Counter::nInvalidTracks)>0) {
      out << "  Number of invalid tracks        :   "<< m_stat.globalCount(Counter::nInvalidTracks) << "\n";
   }
   if (m_stat.globalCount(Counter::nTracksWithoutParam)>0) {
      out << "  Tracks without parameters       :   "<< m_stat.globalCount(Counter::nTracksWithoutParam) << "\n";
   }
   out << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- Forwrd-- " << "\n";
   out << "------------------------------------------------------------------------------------" << "\n";
   out << m_stat.dumpRegions("  Number of candidates at input   :",    ScoreCategory::kNcandidates,iw);
   out << m_stat.dumpRegions("  - candidates rejected score 0   :",    ScoreCategory::kNcandScoreZero,iw);
   out << m_stat.dumpRegions("  - candidates rejected as double :",    ScoreCategory::kNcandDouble,iw);
   out << "------------------------------------------------------------------------------------" << "\n";
   out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
       << "    definition: ( 0.0 < Barrel < " << m_etaBounds[Counter::iBarrel] << " < Transition < " << m_etaBounds[Counter::iTransi]
       << " < Endcap < " << m_etaBounds[Counter::iEndcap] << " < Forward < " << m_etaBounds[Counter::iForwrd] << " )" << "\n";
   out << "------------------------------------------------------------------------------------" << "\n";
   out << std::setprecision(ss);
}


