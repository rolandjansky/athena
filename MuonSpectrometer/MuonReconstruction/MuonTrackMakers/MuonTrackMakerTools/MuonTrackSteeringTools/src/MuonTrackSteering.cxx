/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonTrackSteering.h"
#include "MuonTrackSteeringStrategy.h"

#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuonSegment/MuonSegment.h"
//#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonSegment/MuonSegmentCombination.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentInOverlapResolvingTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMerger.h"
#include "MuonRecToolInterfaces/IMuonTrackBuilder.h"
#include "MuonRecToolInterfaces/IMuonSegmentFittingTool.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"

#include "MuonTrackFindingEvent/MuPatSegment.h"
#include "MuPatCandidateTool.h"
#include "MuonTrackFindingEvent/MuPatCandidateBase.h"
#include "MuonTrackFindingEvent/MuPatTrack.h"
#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "MooTrackBuilder.h"
#include "MooCandidateMatchingTool.h"

#include <sstream>
#include <iomanip>
#include <cmath>
#include <set>
#include <string>
#include <algorithm>
#include <utility>

namespace Muon {

  //----------------------------------------------------------------------------------------------------------
  
  MuonTrackSteering::MuonTrackSteering(const std::string& t,const std::string& n,const IInterface* p) 
    : AthAlgTool(t,n,p)
      , m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool")
        , m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
          , m_candidateTool("Muon::MuPatCandidateTool/MuPatCandidateTool")
            , m_trackBTool("Muon::MooTrackBuilder/MooMuonTrackBuilder")
              , m_ambiTool("Trk::SimpleAmbiguityProcessorTool/MuonAmbiProcessor")
                , m_mooBTool("Muon::MooTrackBuilder/MooMuonTrackBuilder")
                  , m_candidateMatchingTool("Muon::MooCandidateMatchingTool/MooCandidateMatchingTool")
                    , m_trackRefineTool("Muon::MooTrackBuilder/MooMuonTrackBuilder")
                      , m_segmentFitter("Muon::MuonSegmentFittingTool/MuonSegmentFittingTool")
                        , m_segmentMerger("")
                          , m_trackSelector("Muon::MuonTrackSelectorTool/MuonTrackSelectorTool")
                            , m_muonHoleRecoverTool("Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool")
                              , m_combinedSLOverlaps(false)
                                , m_findingDepth(0)
                                  , m_seedCombinatorics(0)
  {
    declareInterface<IMuonTrackFinder>(this);

    declareProperty( "StrategyList" , m_stringStrategies , "List of strategies to be used by the track steering" );

    declareProperty( "SegSeedQCut" , m_segQCut[0]=-2 , "Required quality for segments to be a seed" );
    declareProperty( "Seg2ndQCut" , m_segQCut[1]=-2 , "Required quality for segments to be the second on a track" );
    declareProperty( "SegOtherQCut" , m_segQCut[2]=-2 , "Required quality for segments to be added to a track" );

    declareProperty( "EDMHelperTool" , m_helper );
    declareProperty( "MuPatCandidateTool" , m_candidateTool );
    declareProperty( "TrackBuilderTool" , m_trackBTool );
    declareProperty( "AmbiguityTool",   m_ambiTool);
    declareProperty( "MooBuilderTool", m_mooBTool);
    declareProperty( "CandidateMatchingTool", m_candidateMatchingTool);
    declareProperty( "TrackRefinementTool", m_trackRefineTool);
    declareProperty( "MuonSegmentFittingTool", m_segmentFitter );
    declareProperty( "MuonSegmentMerger", m_segmentMerger );
    declareProperty( "OutputSingleStationTracks", m_outputSingleStationTracks = false );
    declareProperty( "DoSummary", m_doSummary = false );
    declareProperty( "MuonTrackSelector",   m_trackSelector );
    declareProperty( "HoleRecoveryTool",   m_muonHoleRecoverTool);
    declareProperty( "WriteMergedSegments", m_writeMergedSegments = false );
    declareProperty( "UseTightSegmentMatching", m_useTightMatching = true );
    declareProperty( "SegmentThreshold", m_segThreshold = 8);
    declareProperty( "OnlyMdtSeeding", m_onlyMDTSeeding = true );
  }
    
  MuonTrackSteering::~MuonTrackSteering() {
  }
  
  StatusCode MuonTrackSteering::initialize() {

    ATH_CHECK( m_helper.retrieve() );
    ATH_CHECK (m_printer.retrieve() );
    ATH_CHECK( m_candidateTool.retrieve() );
    ATH_CHECK( m_candidateMatchingTool.retrieve() );
    ATH_CHECK( m_trackBTool.retrieve() );
    ATH_CHECK( m_ambiTool.retrieve() );
    ATH_CHECK( m_mooBTool.retrieve() );
    ATH_CHECK( m_trackRefineTool.retrieve() );
    ATH_CHECK( decodeStrategyVector( m_stringStrategies ) );
    if( m_outputSingleStationTracks ){
      ATH_CHECK( m_segmentFitter.retrieve() );
      ATH_MSG_INFO("Single station track enabled ");
    }
    ATH_CHECK( m_muonHoleRecoverTool.retrieve() );
    if( !m_trackSelector.empty() ){
      ATH_CHECK( m_trackSelector.retrieve() );
      ATH_MSG_INFO("Track selection enabled: " << m_trackSelector );
    }
    if( !m_segmentMerger.empty() ){
      ATH_CHECK( m_segmentMerger.retrieve() );
      ATH_MSG_INFO("Segment merging enabled: " << m_segmentMerger);
    }

    return StatusCode::SUCCESS;
  }
  
  //-----------------------------------------------------------------------------------------------------------
 
  StatusCode MuonTrackSteering::finalize(){
 
    for (unsigned int i=0;i<m_strategies.size();i++) delete m_strategies[i];
     
    return StatusCode::SUCCESS;
  }

  //-----------------------------------------------------------------------------------------------------------
 
  TrackCollection* MuonTrackSteering::find( const MuonSegmentCollection& coll ) const {
    TrackCollection* result = 0;
          
    // Initialise vectors etc.
    init();
    
    // Extract segments into work arrays
    if (extractSegments( coll )) {
      
      // Perform the actual track finding
      result = findTracks();      
    }
    // Tracking complete - cleanup   
    cleanUp();

    return result;
  } 
     
  //-----------------------------------------------------------------------------------------------------------
 
  TrackCollection* MuonTrackSteering::find( const MuonSegmentCombination& combi ) const {
      
    //+ 
    // This function is meant to provide backwards compatibility with the MuonCombiTrackMaker.
    // It creates a MuonSegmentCollection from the input MuonSegmentCombinationCollection and
    // then runs the find() on that to provide a vector of tracks
    //-
      
    // Initialise vectors etc.
    init();
    
    ATH_MSG_DEBUG("find: MuonSegmentCombination  " );

    TrackCollection* result = 0;
    MuonSegmentCollection muColl;
    MuonSegmentCollection segVec;

    // Loop over stations within combi
    unsigned int nstations = combi.numberOfStations();
    if ( nstations > 0 ) {                  
      for (unsigned int i=0; i < nstations; ++i) {

        // Loop over segments within station
        const MuonSegmentCombination::SegmentVec* stationSegs = combi.stationSegments( i );
        if ( 0 != stationSegs &&!stationSegs->empty() ) {
          MuonSegmentCombination::SegmentVec::const_iterator ipsg    =stationSegs->begin();
          MuonSegmentCombination::SegmentVec::const_iterator ipsg_end=stationSegs->end();
          for (; ipsg != ipsg_end; ++ipsg) {
            const MuonSegment* seg = dynamic_cast<const MuonSegment*>(*ipsg);
            if ( 0 != seg ) {

              // Add segment to the MuonSegmentCollection
              // This should be made more efficient
              segVec.push_back(seg);
            }
          }            
        }
      }
      muColl.insert(muColl.end(),segVec.begin(),segVec.end());
      result = find( muColl );
    }
    // Tracking complete - cleanup   
    cleanUp();

    return result;      
  }
  //-----------------------------------------------------------------------------------------------------------
 
  //-----------------------------------------------------------------------------------------------------------
  // Private Methods
  //-----------------------------------------------------------------------------------------------------------
 
  void MuonTrackSteering::cleanUp() const {
     
    std::for_each(m_constsegmentsToDelete.begin(),m_constsegmentsToDelete.end(),MuonDeleteObject<const MuonSegment>());
    m_constsegmentsToDelete.clear();
    std::for_each( m_tracksToDelete.begin(),m_tracksToDelete.end(),MuonDeleteObject<MuPatTrack>() );
    m_tracksToDelete.clear();

    // sanity check that the track/segment association is ok
    // at this point all MuPatTracks should be deleted so the segments should not be assigned to any track anymore
    SegColIt sit = m_segmentsToDelete.begin();
    SegColIt sit_end = m_segmentsToDelete.end();
    for( ;sit!=sit_end;++sit ){
      if( !(*sit)->tracks().empty() ) {
        ATH_MSG_WARNING("Detected segment/track association issue for segment ");// << std::endl
        //			<< m_candidateTool->print(**sit) );
        std::set<MuPatTrack*>::const_iterator tit = (*sit)->tracks().begin();
        std::set<MuPatTrack*>::const_iterator tit_end = (*sit)->tracks().end();
        for( ;tit!=tit_end; ++tit ){
          ATH_MSG_DEBUG(" Track ptr " << *tit );
        }
      }
    }
    std::for_each(m_segmentsToDelete.begin(),m_segmentsToDelete.end(),MuonDeleteObject<MuPatSegment>());
    m_segmentsToDelete.clear();

    // clean up entry handler tool
    m_candidateTool->cleanUp();

  }

  //-----------------------------------------------------------------------------------------------------------

  bool MuonTrackSteering::extractSegments( const MuonSegmentCollection& coll ) const {
    
    if(coll.empty()) return false;
      
    MuonSegmentCollection theSegments;
    if( !m_segmentMerger.empty() ) {
      theSegments = m_segmentMerger->findDuplicates(coll);
      if( m_writeMergedSegments){
        Trk::SegmentCollection* segmentCollection = new Trk::SegmentCollection();
        for( MuonSegmentCollection::const_iterator sit=theSegments.begin();sit!=theSegments.end();++sit ){
          segmentCollection->push_back(const_cast<MuonSegment*>(*sit));
        }
        if (evtStore()->record(segmentCollection,"MergedMuonSegments").isFailure() ){
          ATH_MSG_WARNING("failed to recoded MergedMuonSegments");
          m_constsegmentsToDelete.insert(m_constsegmentsToDelete.end(),theSegments.begin(),theSegments.end());
        }
      }else{
        m_constsegmentsToDelete.insert(m_constsegmentsToDelete.end(),theSegments.begin(),theSegments.end());
      }
    }else{
      theSegments = coll;
    }
    ATH_MSG_DEBUG("New collection " << theSegments.size() );
      
    // Sort the input collection by chamber & station IDs
    MuonSegmentCollection::const_iterator iter = theSegments.begin();
    MuonSegmentCollection::const_iterator iend = theSegments.end();
    for (; iter != iend; ++iter) {
      
      ATH_MSG_DEBUG("Adding segment ");
      MuPatSegment* aSeg = m_candidateTool->createSegInfo( *(*iter) );
      ATH_MSG_DEBUG(" -> MuPatSegment " << m_candidateTool->print(*aSeg) );
      
      MuonStationIndex::ChIndex chIndex = aSeg->chIndex;
      MuonStationIndex::StIndex stIndex = aSeg->stIndex;
      if(chIndex<0 || stIndex<0)
      {
        ATH_MSG_WARNING("Chamber or station index invalid:" <<m_candidateTool->print(*aSeg));
        continue;
      }
      m_chambersWithSegments.insert(chIndex);
      m_stationsWithSegments.insert(stIndex);
      
      std::vector< MuPatSegment*>& segments = m_chamberSegments[chIndex];
      segments.push_back(aSeg);
      if( !m_combinedSLOverlaps ){
        std::vector< MuPatSegment*>& segments2 = m_stationSegments[stIndex];
        segments2.push_back(aSeg);      
      }
      m_segmentsToDelete.push_back(aSeg);
    }
    
    if( m_combinedSLOverlaps ){
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::BIS],m_chamberSegments[MuonStationIndex::BIL]);
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::BMS],m_chamberSegments[MuonStationIndex::BML]);
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::BOS],m_chamberSegments[MuonStationIndex::BOL]);
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::EIS],m_chamberSegments[MuonStationIndex::EIL]);
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::EMS],m_chamberSegments[MuonStationIndex::EML]);
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::EOS],m_chamberSegments[MuonStationIndex::EOL]);
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::EES],m_chamberSegments[MuonStationIndex::EEL]);
      combineOverlapSegments(m_chamberSegments[MuonStationIndex::CSS],m_chamberSegments[MuonStationIndex::CSL]);
      std::vector< MuPatSegment*>& segments = m_chamberSegments[MuonStationIndex::BEE];
      if( !segments.empty() ){
        m_chambersWithSegments.insert(MuonStationIndex::BEE);
        m_stationsWithSegments.insert(MuonStationIndex::BE);
        std::vector< MuPatSegment*>& segs = m_stationSegments[MuonStationIndex::BE];
        segs.insert(segs.end(),segments.begin(),segments.end());
      }
    }
    return true;
  } 

  void MuonTrackSteering::combineOverlapSegments( std::vector< MuPatSegment*>& ch1, std::vector< MuPatSegment*>& ch2 ) const 
  {
    /** try to find small/large overlaps, insert segment into stationVec */

    // if both empty there is nothing to be done
    if( ch1.empty() && ch2.empty() ) return;

    // get station index from the first segment in the first non empty vector
    MuonStationIndex::StIndex stIndex = !ch1.empty() ? ch1.front()->stIndex : ch2.front()->stIndex;

    SegCol& stationVec = m_stationSegments[stIndex];

    // vector to flag entries in the second station that were matched
    std::vector<bool> wasMatched2( ch2.size(), false );

    // loop over all possible combinations
    SegColIt sit1 = ch1.begin();
    SegColIt sit1_end = ch1.end();
    for( ;sit1!=sit1_end; ++sit1 ){
    
      // do not combine poor quality segments
      int qualityLevel1 = ch1.size() > 5 ? 1 : 2;
      if( (*sit1)->quality < qualityLevel1 ){
        ATH_MSG_VERBOSE("resolveSLOverlaps::bad segment1 q: " << (*sit1)->quality << " cut " << qualityLevel1 << std::endl
          << m_printer->print( *(*sit1)->segment ));
        stationVec.push_back(*sit1);
        continue;
      } 
      
      bool wasMatched1 = false;

      // apply looser cuts as we perform matching
      int qualityLevel2 = ch2.size() > 5 ? 1 : 2;
      SegColIt sit2 = ch2.begin();
      SegColIt sit2_end = ch2.end();
      for( ;sit2!=sit2_end; ++sit2 ){
        
        // do not combine poor quality segments AND require at least one of the segments to have a quality beter than 1
        if( (*sit2)->quality < qualityLevel2 ){
          ATH_MSG_VERBOSE("resolveSLOverlaps::bad segment2:  q " << (*sit2)->quality << " cut " << qualityLevel2 << std::endl
            << m_printer->print( *(*sit2)->segment ) );
          continue;
        }
        if( (*sit1)->quality < 2  && (*sit2)->quality < 2 ) {
          ATH_MSG_VERBOSE("resolveSLOverlaps:: combination of insufficient quality " << std::endl
            << " q1 " << (*sit1)->quality << " q2 " << (*sit1)->quality );
          continue;
        }
  
        ATH_MSG_VERBOSE(" combining entries: " << std::endl
          <<  m_printer->print( *(*sit1)->segment ) << std::endl
            <<  m_printer->print( *(*sit2)->segment ) );

        if( !m_candidateMatchingTool->match(**sit1,**sit2, false) ){ 
          ATH_MSG_VERBOSE(" overlap combination rejected based on matching" << std::endl
            << m_printer->print(*(*sit2)->segment) );       
          continue;
        }

        // create MuonSegment
        MuonSegment* newseg = m_mooBTool->combineToSegment(**sit1,**sit2);
        if( !newseg ){
          ATH_MSG_DEBUG(" Combination of segments failed " );
          continue;
        }       

        const Trk::FitQuality* fq = newseg->fitQuality();
        if( !fq || fq->numberDoF() == 0 ){
          ATH_MSG_WARNING(" no fit quality, dropping segment ");
          delete newseg;
          continue;
        }
        if( fq->chiSquared()/fq->numberDoF() > 2.5 ){
          ATH_MSG_DEBUG("bad fit quality, dropping segment " << fq->chiSquared()/fq->numberDoF() );
          delete newseg;
          continue;
        }


        MuPatSegment* segInfo = m_candidateTool->createSegInfo( *newseg );

        // check whether segment of good quality AND that its quality is equal or better than the input segments
        if( segInfo->quality < 2 || 
        (segInfo->quality < (*sit1)->quality || segInfo->quality < (*sit2)->quality)  ){
          ATH_MSG_VERBOSE("resolveSLOverlaps::bad segment " << std::endl
            << m_printer->print(*segInfo->segment) );
          delete segInfo;
          delete newseg;
          continue;
        }
        int shared_eta=0,shared_phi=0; //check for hits shared between segments
        const MuPatSegment* temp1=(*sit1);
        const MuPatSegment* temp2=(*sit2);
        MuPatHitCit lit1=temp1->hitList().begin(), lit1_end=temp1->hitList().end();
        for(;lit1!=lit1_end;++lit1){
          MuPatHitCit lit2=temp2->hitList().begin(), lit2_end=temp2->hitList().end();
          for(;lit2!=lit2_end;++lit2){
            if((*lit1)->info().id==(*lit2)->info().id){
              if((*lit1)->info().measuresPhi) shared_phi++;
              else shared_eta++;
            }
          }
        }
	
        if( (*sit1)->etaHits().size() + (*sit2)->etaHits().size() - shared_eta - segInfo->etaHits().size() > 1 ){
          ATH_MSG_VERBOSE("resolveSLOverlaps::more than one eta measurement removed, dropping track " << std::endl
            << m_printer->print(*segInfo->segment) );
          delete segInfo;
          delete newseg;
          continue;
        }

        int phiHitDiff=(*sit1)->phiHits().size() + (*sit2)->phiHits().size() - shared_phi - segInfo->phiHits().size();
        if( phiHitDiff > 1 || ((*sit1)->phiHits().size() + (*sit2)->phiHits().size() > 0 && segInfo->phiHits().size() == 0) ){
          ATH_MSG_VERBOSE("resolveSLOverlaps::more than one phi measurement removed, dropping track " << std::endl
            << m_printer->print(*segInfo->segment) );
          delete segInfo;
          delete newseg;
          continue;
        }

        double cosPointingAngle = (newseg->globalPosition().x()*newseg->globalDirection().x()+newseg->globalPosition().y()*newseg->globalDirection().y())/
          (newseg->globalPosition().perp()*newseg->globalDirection().perp());
        if( cosPointingAngle < 0.995 ){
          ATH_MSG_VERBOSE("resolveSLOverlaps: rejected due to too large pointing angle " << std::endl
            << m_printer->print(*segInfo->segment) );
          delete segInfo;
          delete newseg;
          continue;
        }
        ATH_MSG_VERBOSE("created SL overlap segment: cos pointing " << cosPointingAngle << std::endl << m_printer->print(*segInfo->segment));
        
        // store pointer to segment so it can be deleted at the end of track search
        m_constsegmentsToDelete.push_back(newseg);
        m_segmentsToDelete.push_back( segInfo );

        // flag segments as matched
        wasMatched1 = true;
        wasMatched2[ std::distance( ch2.begin(), sit2 ) ] = true;

        // add segment
        stationVec.push_back(segInfo);
      }
      
      // if entry was not associated with entry in other station add it to entries 
      if( !wasMatched1 ){
        stationVec.push_back(*sit1);
      }
    }

    // loop over entries in second station as add unassociated entries to candidate entries
    for( unsigned int i=0;i<wasMatched2.size(); ++i ){
      if( !wasMatched2[i] ) {
        stationVec.push_back( ch2[i] );
      }
    }

    // add station to list of stations with segments 
    if( !stationVec.empty() ){
      m_stationsWithSegments.insert(stIndex);
    }

    // sort segment according to their quality
    std::stable_sort(stationVec.begin(),stationVec.end(),SortSegInfoByQuality());

  }


  //-----------------------------------------------------------------------------------------------------------
 
  TrackCollection* MuonTrackSteering::findTracks() const {
    // Very basic : output all of the segments we are starting with
    ATH_MSG_DEBUG("List of all strategies: " << m_strategies.size() );
    for (unsigned int i=0;i<m_strategies.size();++i) ATH_MSG_DEBUG((*(m_strategies[i])));

    std::vector<MuPatTrack*>* resultAll = new std::vector<MuPatTrack*>();

    // Outermost loop over strategies!
    for (unsigned int i=0;i<m_strategies.size();++i){
      if (!m_strategies[i]) continue; // Check for empty strategy pointer
      
      const MuonTrackSteeringStrategy& strategy = *m_strategies[i];

      std::vector<MuPatTrack*>* result = new std::vector<MuPatTrack*>();


      // Segments that will be looped over...
      SegColVec mySegColVec( strategy.getAll().size() );
      
      //db+
      ATH_MSG_VERBOSE("Segments to be looped on: " << mySegColVec.size() );
      //-
      
      std::set<MuonStationIndex::StIndex> stations;
      // Preprocessing : loop over layers
      for (unsigned int lit=0;lit<strategy.getAll().size();++lit){

        std::vector< MuonStationIndex::ChIndex > chambers = strategy.getCh( lit );

        // Optional : combine segments in the same station but different chambers
        if (strategy.option(MuonTrackSteeringStrategy::CombineSegInStation)){
          // Loop over stations in the layer
          for (unsigned int chin=0;chin<chambers.size();++chin){
	    
            // get station index for the chamber
            MuonStationIndex::StIndex stIndex = MuonStationIndex::toStationIndex(chambers[chin]);

            // skip those that are already included
            if( stations.count(stIndex) ) continue;
            SegCol& segments = m_stationSegments[ stIndex ];
            // Add all of the MuPatSegments into the list for that layer
            //db
	    
            if (strategy.option(MuonTrackSteeringStrategy::BarrelEndcapFilter))
            {
              //SegCol filteredSegments;
              for (unsigned int iseg=0; iseg< segments.size(); iseg++ )
              {
                double thetaSeg = fabs((*segments[iseg]).segment->globalPosition().theta());
		
                // only select segments in barrel/endcap overlap 
                if ( (0.74159 > thetaSeg && thetaSeg > 0.51159) ||
                  (2.63 > thetaSeg && thetaSeg > 2.40) )  mySegColVec[lit].push_back(segments[iseg]);  
	       
              }
            }
            else{
              mySegColVec[lit].insert( mySegColVec[lit].end() , segments.begin() , segments.end() ); 
            }
	    
            stations.insert(stIndex);
          } // End of loop over chambers

        } else {
          // Loop over stations in the layer
          for (unsigned int chin=0;chin<chambers.size();++chin){
            SegCol& segments = m_chamberSegments[ chambers[chin] ];
            // Throw all of the MuPatSegments into the list for that layer
            mySegColVec[lit].insert( mySegColVec[lit].end() , segments.begin() , 
            segments.end() );
          } // End of loop over chambers
        } // End of if combine segments in a layer

      } // End of loop over layers

      // Preprocessing step two : sort all layers' segments by quality
      for (unsigned int lit=0;lit<mySegColVec.size();++lit){
        std::stable_sort( mySegColVec[lit].begin() , mySegColVec[lit].end() , SortSegInfoByQuality() );
      }

      if( m_doSummary || msgLvl(MSG::DEBUG) ){
        bool hasSegments = false;
        for (unsigned int lit=0;lit<mySegColVec.size();++lit){
          if( !mySegColVec[lit].empty() ){
            hasSegments = true;
            break;
          }
        }
        if( hasSegments ){
          msg( m_doSummary ? MSG::INFO : MSG::DEBUG ) << "For strategy: " << strategy.getName() << " segments are: ";
          for (unsigned int lit=0;lit<mySegColVec.size();++lit)
            for (unsigned int sit=0;sit<mySegColVec[lit].size();++sit)
              msg(m_doSummary ? MSG::INFO : MSG::DEBUG) << std::endl << "  " << m_candidateTool->print(*(mySegColVec[lit])[sit]);
          msg(m_doSummary ? MSG::INFO : MSG::DEBUG) << endmsg;
        }
      }

      // Hang on to whether we want to cut seeds or not
      bool cutSeeds = strategy.option(MuonTrackSteeringStrategy::CutSeedsOnTracks);

      // Now assign priority for the layers
      std::vector<unsigned int> seeds;

      // Assign seeds dynamically according to 
      if (strategy.option(MuonTrackSteeringStrategy::DynamicSeeding)){
        // Loop through layers and do a little sort
        std::vector< std::pair<int,unsigned int> > occupancy; // layer , occ
        for (unsigned int lit=0;lit<mySegColVec.size();++lit){
          occupancy.push_back( std::pair<int,unsigned int>(mySegColVec[lit].size(),lit) );
        }
        std::stable_sort( occupancy.begin() , occupancy.end() );
        for (unsigned int lit=0;lit<occupancy.size();++lit){
          seeds.push_back( occupancy[lit].second );
        }
      } else {
        seeds = strategy.seeds();
        if (0==seeds.size()){
          //cutSeeds = false; // If there are no seed layers, then we will loop through all layers and not cut seeds
          for (unsigned int j=0;j<mySegColVec.size();++j) seeds.push_back(j);
        }
      }
      ATH_MSG_VERBOSE("Selected seed layers " << seeds.size() );

      MuPatSegment* seedSeg = 0;
      // Loop over seed layers
      for (unsigned int lin=0; lin<seeds.size();++lin){
        // Loop over segments in that layer
        ATH_MSG_VERBOSE("New seed layer " << lin << " segments in layer " << mySegColVec[lin].size() );

        for (unsigned int sin=0; sin<mySegColVec[lin].size(); sin++){
          seedSeg = mySegColVec[lin].operator[](sin);
          if (!seedSeg) continue; // Check for empty poinnter

          // Optionally, if the seed is on a track we skip it
          if (cutSeeds && seedSeg->usedInFit) continue;

          // See if the seed passes our quality cut
          if (seedSeg->quality<m_segQCut[0] || (m_segQCut[0]==-99 && !(seedSeg->segQuality && seedSeg->segQuality->isStrict()))) continue;
          if( m_onlyMDTSeeding && !seedSeg->isMdt ) continue;

          int segsInCone = 0;
          double phiSeed  = seedSeg->segment->globalPosition().phi();
          double etaSeed  = seedSeg->segment->globalPosition().eta();
          for (unsigned int sin2=0; sin2<mySegColVec[lin].size(); sin2++){
            if( sin==sin2 ) continue;
            MuPatSegment* seg = mySegColVec[lin].operator[](sin2);;

            if (seg->quality<m_segQCut[0] || (m_segQCut[0]==-99 && !(seg->segQuality && seg->segQuality->isStrict()))) continue;

            double phiSeg  = seg->segment->globalPosition().phi();
            double etaSeg  = seg->segment->globalPosition().eta();
	    
            double deltaPhi=fabs(phiSeed-phiSeg);
            const double PI= acos(-1);
            if (deltaPhi>PI) deltaPhi = 2*PI-deltaPhi;
	     
            double deltaEta = fabs(etaSeed-etaSeg);
	     
            double deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
	     
            if (deltaR < 0.35) segsInCone++;	     
	    
          }
          ATH_MSG_VERBOSE("New seed " << sin << " segments in cone " << segsInCone );

          if ( segsInCone > m_segThreshold && seedSeg->quality<m_segQCut[0]+1 ) continue;

          std::vector<MuPatTrack*>* found = findTrackFromSeed( *seedSeg , *(m_strategies[i]) , seeds[lin] , mySegColVec );
	  
          if(found) {
            ATH_MSG_VERBOSE("  Tracks for seed: " << std::endl << " --- " <<  m_candidateTool->print(*result) );
            result->insert( result->end() , found->begin() , found->end() );
            delete found;
          }
        } // End of loop over segments in a layer
      } // Done with loop over seed layers
      
      // Post-processing : refinement
      if( result && !result->empty()){
        std::vector<MuPatTrack*>* refined = strategy.option(MuonTrackSteeringStrategy::DoRefinement) ? refineTracks( *result ) : 0;
        if(refined) {
          // if the refinement was ran and produced new candidates, loop over existing ones and delete those that are 
          // not in the refined collection. Once done delete the old result and replace it with the refined tracks
          std::vector<MuPatTrack*>::iterator tit = result->begin();
          std::vector<MuPatTrack*>::iterator tit_end = result->end();
          for( ;tit!=tit_end;++tit ){
            std::vector<MuPatTrack*>::iterator pos = std::find(refined->begin(),refined->end(),*tit);
            if( pos == refined->end() ) delete *tit;
          }
          delete result;
          result = refined;
        }
      }
      
      // Post-processing : ambiguity resolution
      if(msgLvl(MSG::DEBUG) && result && !result->empty()){
        msg(MSG::DEBUG)  << "Initial track collection for strategy: " << strategy.getName()
          << "  " << m_candidateTool->print(*result) << endmsg;
      }

      if( result && !result->empty()){
        std::vector<MuPatTrack*>* resolved = strategy.option(MuonTrackSteeringStrategy::DoAmbiSolving) ? solveAmbiguities( *result ) : 0;
        if(resolved) {
          // if the refinement was ran and produced new candidates, loop over existing ones and delete those that are 
          // not in the refined collection. Once done delete the old result and replace it with the refined tracks
          std::vector<MuPatTrack*>::iterator tit = result->begin();
          std::vector<MuPatTrack*>::iterator tit_end = result->end();
          for( ;tit!=tit_end;++tit ){
            std::vector<MuPatTrack*>::iterator pos = std::find(resolved->begin(),resolved->end(),*tit);
            if( pos == resolved->end() ) delete *tit;
          }
          delete result;
          result = resolved;
        }
      }
      
      if( result && !result->empty()) resultAll->insert(resultAll->end(),result->begin(),result->end());
          
      delete result;
      
    } // Done with loop over strategies

    if( resultAll && !resultAll->empty()){
      std::vector<MuPatTrack*>* resolved = solveAmbiguities( *resultAll );
      
      if(resolved) {
        if( m_doSummary ) ATH_MSG_INFO("Ambiguity solving: input " << resultAll->size() << " after ambi solving " << resolved->size() );
        else              ATH_MSG_DEBUG("Ambiguity solving: input " << resultAll->size() << " after ambi solving " << resolved->size() );
        // if the refinement was ran and produced new candidates, loop over existing ones and delete those that are 
        // not in the refined collection. Once done delete the old result and replace it with the refined tracks
        std::vector<MuPatTrack*>::iterator tit = resultAll->begin();
        std::vector<MuPatTrack*>::iterator tit_end = resultAll->end();
        
        for( ;tit!=tit_end;++tit ){
          std::vector<MuPatTrack*>::iterator pos = std::find(resolved->begin(),resolved->end(),*tit);
          if( pos == resolved->end() ) delete *tit;
        }
      } else {
        for (auto trk : *resultAll) delete trk; 
      }
      delete resultAll; 
      resultAll = resolved;
      
    }

    if( m_outputSingleStationTracks ){
      SegCol& emSegments = m_stationSegments[MuonStationIndex::EM];
      // loop over segments in EM stations
      if( !emSegments.empty() ){
        SegCol::const_iterator sit = emSegments.begin();
        SegCol::const_iterator sit_end = emSegments.end();
        for( ;sit!=sit_end;++sit ){
          // skip segments that are associated to a track
          if( !(*sit)->tracks().empty() ) continue;

          // only take highest quality segments
          if( (*sit)->quality < 2 ) continue;
	  
          // fit segment and add the track if fit ok
          Trk::Track* segmentTrack = m_segmentFitter->fit(*(*sit)->segment);
          if( segmentTrack ) {
            //Try to recover hits on the track
            Trk::Track* recoveredTrack = m_muonHoleRecoverTool->recover(*segmentTrack);
            if(recoveredTrack){
              delete segmentTrack;
              segmentTrack = recoveredTrack;
            }

            MuPatTrack* can = m_candidateTool->createCandidate( **sit, segmentTrack );
            if( can ) {
              if( !resultAll ) resultAll = new std::vector<MuPatTrack*>();
              resultAll->push_back(can);
            }else{
              ATH_MSG_WARNING("Failed to create MuPatTrack");
              delete segmentTrack;
            }
          }
        }
      }
    }


    // Output all the tracks that we are ending with
    if( resultAll && !resultAll->empty()){
      if( m_doSummary ) ATH_MSG_INFO("Final Output : " << m_candidateTool->print(*resultAll) << endmsg);
      else              ATH_MSG_DEBUG("Final Output : " << m_candidateTool->print(*resultAll) << endmsg);
    }
    TrackCollection* finalTrack = 0;
    if( resultAll && !resultAll->empty()){
      finalTrack = selectTracks(*resultAll,true);
      
      // clean up candidates
      std::for_each( resultAll->begin(),resultAll->end(),MuonDeleteObject<MuPatTrack>() );
    }
    delete resultAll;
    return finalTrack;
  }

  //-----------------------------------------------------------------------------------------------------------

  const MuonTrackSteering::SegColVec* mmm__segColVec = 0;

  std::string print( const MuPatSegment& seg ){
    if( !mmm__segColVec ) return "";
    const MuonTrackSteering::SegColVec& segs = *mmm__segColVec;
    int index = -1;
    for(unsigned int i=0;i<segs.size();++i ){
      for(unsigned int j=0;j<segs[i].size();++j ){
        if( &seg == segs[i][j] ){
          index = j;
          break;
        }
      }
      if( index != -1 ) break;
    }
    
    std::ostringstream s;
    s << "Seg:" << Muon::MuonStationIndex::chName(seg.chIndex) << "_" << index;
    return s.str();
  }

  std::string print( const std::vector<MuPatSegment*>& segVec ){
    std::ostringstream s;
    std::vector<MuPatSegment*>::const_iterator sit = segVec.begin();
    std::vector<MuPatSegment*>::const_iterator sit_end = segVec.end();
    for( ;sit!=sit_end;++sit ){
      s << " " << print(**sit);
    }
    return s.str();
  }
  std::string print( const MuPatTrack& track ){
    std::ostringstream s;
    s << "Track:" << print(track.segments());
    return s.str();
  }

  std::string print( const std::vector<MuPatTrack*>& tracks ){
    std::ostringstream s;
    std::vector<MuPatTrack*>::const_iterator tit = tracks.begin();
    std::vector<MuPatTrack*>::const_iterator tit_end = tracks.end();
    for( ;tit!=tit_end;++tit ) s << std::endl << print(**tit);

    return s.str();
  }

  std::vector<MuPatTrack*> * MuonTrackSteering::findTrackFromSeed( MuPatSegment& seedSeg , const MuonTrackSteeringStrategy & strat , const unsigned int layer , const SegColVec& segs ) const {
   
    //the resulting vector of tracks to be returned
    std::vector<MuPatTrack*> *result = new std::vector<MuPatTrack*>();
    ATH_MSG_DEBUG("Working on seed: " << std::endl << " --- " <<  m_candidateTool->print(seedSeg) );
    //mmm__segColVec = &segs;
    //std::cout << "Working on seed: " << std::endl << " --- " << print(seedSeg) << " layer " << layer << " max  " << strat.getAll().size() << std::endl;
    m_findingDepth = 0;
    m_seedCombinatorics = 0;
    const unsigned int endLayer = strat.getAll().size();
    ///  Loop over layers following the seed layer
    for (unsigned int ilayer=0; ilayer< strat.getAll().size();++ilayer )
    {
      if (ilayer==layer) continue; //don't include the layer of the seed

      if (segs[ilayer].empty()) continue;

      std::vector<MuPatSegment*> matchedSegs;
      matchedSegs.clear();
      bool tightCuts = false;
      //
      if (m_useTightMatching)
      {
        double phiSeed = (seedSeg.segment)->globalPosition().phi();
        double etaSeed = (seedSeg.segment)->globalPosition().eta();
	 
        int segsInCone = 0;
        for (unsigned int j=0; j< segs[ilayer].size() ; j++)
        {
          double phiSeg  = (*segs[ilayer][j]).segment->globalPosition().phi();
          double etaSeg  = (*segs[ilayer][j]).segment->globalPosition().eta();
	     
          double deltaPhi=fabs(phiSeed-phiSeg);
          const double PI= acos(-1);
          if (deltaPhi>PI) deltaPhi = 2*PI-deltaPhi;
	     
          double deltaEta = fabs(etaSeed-etaSeg);
	     
          double deltaR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
	     
          if (deltaR < 0.35) segsInCone++;	     
        }
             
        if (segsInCone > m_segThreshold )
        {
          for (unsigned int j=0; j< segs[ilayer].size() ; j++)
          {
            bool isMatched =  m_candidateMatchingTool->match(seedSeg,*segs[ilayer][j],true);
	       
            if (isMatched) matchedSegs.push_back(segs[ilayer][j]);
          }
          if (matchedSegs.size()==0) continue;
          tightCuts = true;
        }
      } 

      std::vector<MuPatTrack*> *tracks=0;

      if (matchedSegs.size()!=0 && m_useTightMatching)
      {  
        tracks = m_trackBTool->find(seedSeg, matchedSegs);
	   
        //ATH_MSG_DEBUG( std::setw(m_findingDepth) << " " << " Adding layer: " << ilayer << " with " 
        //		 <<  matchedSegs.size() << " matched segments " << m_candidateTool->print(matchedSegs) );  
      }
      else{
        tracks = m_trackBTool->find(seedSeg, segs[ilayer]);
        //        ATH_MSG_DEBUG( std::setw(m_findingDepth) << " " << " Adding layer: " << ilayer << " with " 
        // 		      <<  segs[ilayer].size() << " segments " << m_candidateTool->print(segs[ilayer]) );  
       
        //std::cout<< std::setw(m_findingDepth) << " " << " Adding layer: " << ilayer << " with " <<  segs[ilayer].size() << " segments " << print(segs[ilayer]) << std::endl;  
      }
      ++m_seedCombinatorics;
      ++m_findingDepth;
      if( tracks && !tracks->empty() )
      {
	 
        // if we reached the end of the sequence, we should save what we have else continue to next layer
        if( ilayer+1==strat.getAll().size()){
          result->insert(result->end(),tracks->begin(),tracks->end());
          delete tracks;
          break;
        }

        std::vector<MuPatTrack*>::iterator cit     = tracks->begin();
        std::vector<MuPatTrack*>::iterator cit_end = tracks->end();
        //std::cout << std::setw(m_findingDepth) << " " << "found tracks " << tracks->size() << print(*tracks) << std::endl;
	  
	  
        //loop on found tracks
        for( ; cit!=cit_end; ++cit )
        {
          //m_tracksToDelete.push_back(*cit);
          unsigned int nextLayer = ilayer+1;
	     
          if ( nextLayer < strat.getAll().size() )
          {
            int cutLevel = tightCuts ? 1 : 0;
            std::vector<MuPatTrack*>* nextTracks = extendWithLayer(**cit,segs,nextLayer,endLayer,cutLevel);
            if (nextTracks && !nextTracks->empty()) {
              //std::cout << std::setw(m_findingDepth) << " " << "found tracks ( " << nextLayer << ") " << nextTracks->size() << print(*nextTracks) << std::endl;
              //ATH_MSG_DEBUG( std::setw(m_findingDepth) << " " << " extension successful adding new candidates " << nextTracks->size() );  
              result->insert(result->end(),nextTracks->begin(),nextTracks->end());
              //std::vector<MuPatTrack*>::iterator ssit = nextTracks->begin();
              //for( ;ssit!=nextTracks->end(); ++ssit ) //std::cout << " adding track can " << *ssit << std::endl;
              std::vector<MuPatTrack*>::const_iterator pos =  std::find(result->begin(),result->end(),*cit );
              if( pos == result->end() ) delete *cit;
            }else{
              std::vector<MuPatTrack*>::const_iterator pos =  std::find(result->begin(),result->end(),*cit );
              if( pos == result->end() ){
                //std::cout << std::setw(m_findingDepth) << " " << "extension failed (" << nextLayer << ") for candidate " << *cit << print(**cit) << std::endl;
                result->push_back(*cit);
              }else{
                ATH_MSG_WARNING("Candidate already included in result");
              }
            }       
            delete nextTracks;
          }
        }
      }
      delete tracks;
    }
    //std::cout << "Result for seed: " << std::endl << " --- " <<  m_candidateTool->print(*result) <<std::endl;
    
    ATH_MSG_DEBUG("Constructed " << result->size() << " tracks with strategy " << strat.getName() );
    return result;
  }
 
  std::vector<MuPatTrack*> * MuonTrackSteering::extendWithLayer(MuPatTrack& candidate, const SegColVec& segs,unsigned int nextlayer, const unsigned int endlayer, int cutLevel ) const
  {
    ++m_findingDepth;
    //std::cout << std::setw(m_findingDepth) << " " << "extendWithLayer " << m_seedCombinatorics << "  " 
    //<< nextlayer << " end " << endlayer << " " << print(candidate) << std::endl;
	  
    std::vector<MuPatTrack*> *result = new std::vector<MuPatTrack*>();
    if ( nextlayer < endlayer )
    {
      for (;nextlayer!=endlayer;nextlayer++)
      {
        if (segs[nextlayer].empty()) continue;
	  
        // 	  ATH_MSG_DEBUG(std::setw(m_findingDepth) << " " << " Extending track with layer " << nextlayer  
        // 			<< " segments " << m_candidateTool->print(segs[nextlayer]) );
        //std::cout << std::setw(m_findingDepth) << " " << " Extending track with layer " << nextlayer << " segments " << print(segs[nextlayer]) << std::endl;
        std::vector<MuPatTrack*> *nextTracks=0;
        nextTracks = m_trackBTool->find(candidate, segs[nextlayer] );
        ++m_seedCombinatorics;

        if (nextTracks && !nextTracks->empty())
        {
          // 	    ATH_MSG_DEBUG( std::setw(m_findingDepth) << " " << " extension successful continuing to layer " << nextlayer
          // 			   << " " << nextTracks->size() );  
          //std::cout << std::setw(m_findingDepth) << " " << " extension successful continuing to layer " << nextlayer << " " << nextTracks->size() << std::endl;;  
          std::vector<MuPatTrack*>::iterator cit     = nextTracks->begin();
          std::vector<MuPatTrack*>::iterator cit_end = nextTracks->end();
          for(; cit!=cit_end; ++cit )
          {
            //m_tracksToDelete.push_back(*cit);
            std::vector<MuPatTrack*>* nextTracks2 = extendWithLayer((**cit), segs, nextlayer+1, endlayer, cutLevel);
            if (nextTracks2 &&  !nextTracks2->empty()) {
              // 		  ATH_MSG_DEBUG( std::setw(m_findingDepth) << " " << " extension successful adding new candidates " << nextTracks->size() );  
              //std::cout << std::setw(m_findingDepth) << " " << " extension successful adding new candidates " << nextTracks->size() << print(*nextTracks) << std::endl;
              //std::vector<MuPatTrack*>::iterator ssit = nextTracks2->begin();
              //for( ;ssit!=nextTracks2->end(); ++ssit ) //std::cout << " adding track can " << *ssit << std::endl;
              result->insert(result->end(),nextTracks2->begin(),nextTracks2->end());
              std::vector<MuPatTrack*>::const_iterator pos =  std::find(result->begin(),result->end(),*cit );
              if( pos == result->end() ) delete *cit;

            }else{
              std::vector<MuPatTrack*>::const_iterator pos =  std::find(result->begin(),result->end(),*cit );
              if( pos == result->end() ){

                // 		    ATH_MSG_DEBUG( std::setw(m_findingDepth) << " " << " extension failed, adding original" );  
                //std::cout << std::setw(m_findingDepth) << " " << " extension failed, adding original " << *cit << print(**cit) << std::endl;  
                result->push_back(*cit);
              }else{
                ATH_MSG_WARNING("Candidate already included in result");
              }

            }
            delete nextTracks2;
          }       
        }
        delete nextTracks;
      }
    }
    --m_findingDepth;
    return result;
  }
 
  //-----------------------------------------------------------------------------------------------------------
  TrackCollection* MuonTrackSteering::selectTracks(std::vector<MuPatTrack*>& candidates, bool takeOwnership ) const {
    TrackCollection* result = takeOwnership ? new TrackCollection() : new TrackCollection(SG::VIEW_ELEMENTS);
    result->reserve(candidates.size());

    std::vector<MuPatTrack*>::iterator cit     = candidates.begin();
    std::vector<MuPatTrack*>::iterator cit_end = candidates.end();
    for( ; cit!=cit_end; ++cit ){
      // if track selector is configured, use it and remove bad tracks
      if( !m_trackSelector.empty() && !m_trackSelector->decision((*cit)->track() ) ) continue;

      if( takeOwnership ) result->push_back(const_cast<Trk::Track*>( &(*cit)->releaseTrack()) );
      else                result->push_back(const_cast<Trk::Track*>( &(*cit)->track()) );
    }
    return result;
  }

  std::vector<MuPatTrack*> * MuonTrackSteering::refineTracks(std::vector<MuPatTrack*>& candidates) const {
    std::vector<MuPatTrack*> * result = new std::vector<MuPatTrack*>();
    
    std::vector<MuPatTrack*>::iterator cit     = candidates.begin();
    std::vector<MuPatTrack*>::iterator cit_end = candidates.end();
    for( ; cit!=cit_end; ++cit )
    {
      MuPatTrack *refinedMuPatTrack=0;
      refinedMuPatTrack =  m_trackRefineTool->refine(**cit);
       
      if (refinedMuPatTrack) {
        //std::cout << std::endl << " after " << m_candidateTool->print(*refinedMuPatTrack) << std::endl; 
        result->push_back(refinedMuPatTrack);
      }
    }
    return result;
  }
  
  //-----------------------------------------------------------------------------------------------------------

  std::vector<MuPatTrack*> * MuonTrackSteering::solveAmbiguities( std::vector< MuPatTrack* >& tracks , const MuonTrackSteeringStrategy* /*strat*/ ) const {

    //the resulting vector of tracks to be returned

    TrackCollection* trkColl = selectTracks(tracks,false);
    if( !trkColl || trkColl->empty() ){
      delete trkColl;
      return 0;
    }

    TrackCollection* resolvedTracks = m_ambiTool->process(trkColl);
    if( !resolvedTracks ){
      delete trkColl;
      return 0;
    }

    ATH_MSG_DEBUG("   resolved track candidates: old size " << trkColl->size()
      << " new size " << resolvedTracks->size() );

    std::vector<MuPatTrack*> *result = new std::vector<MuPatTrack*>();

    TrackCollection::iterator tit = resolvedTracks->begin();
    TrackCollection::iterator tit_end = resolvedTracks->end();
    std::vector<MuPatTrack*>::iterator pat = tracks.begin();
    std::vector<MuPatTrack*>::iterator pat_end = tracks.end();
    for( ;tit!=tit_end;++tit ){
      MuPatTrack* can = 0;
      for( pat=tracks.begin(); pat!=pat_end;++pat ){
        if ( &(*pat)->track() == *tit ) {
          can = *pat;
          break;
        }
      }
      if( can ) result->push_back( can );
      else {
        delete *pat;
      }	  
    }
    
    delete resolvedTracks;
    delete trkColl;
    return result;
  }


  //-----------------------------------------------------------------------------------------------------------
 
  void MuonTrackSteering::init() const {

    // initialize vectors
    m_chamberSegments.clear();
    m_stationSegments.clear();
    m_chamberSegments.resize(MuonStationIndex::ChIndexMax);
    m_stationSegments.resize(MuonStationIndex::StIndexMax);

    m_chambersWithSegments.clear();
    m_stationsWithSegments.clear();

  }

  //-----------------------------------------------------------------------------------------------------------

  StatusCode MuonTrackSteering::decodeStrategyVector(const std::vector<std::string>& strategy) {
    for (unsigned int i=0;i<strategy.size();++i){
      const MuonTrackSteeringStrategy* holder = decodeStrategy( strategy[i] );
      if (0==holder) { 
        // complain
        ATH_MSG_DEBUG("failed to decode strategy");
      }else{
        // flag whether segments should be combined
        if( holder->option(MuonTrackSteeringStrategy::CombineSegInStation) ) m_combinedSLOverlaps = true;
        m_strategies.push_back( holder );
      }
    }
    return StatusCode::SUCCESS;
  }

  //-----------------------------------------------------------------------------------------------------------

  const MuonTrackSteeringStrategy* MuonTrackSteering::decodeStrategy(const std::string& strategy) const {

    const std::string delims(" \t[],;");
  
    // The strategy name
    std::string name;
    
    // The strategy options (which should be a vector of enums, but I'll use strings now to check that I'm
    // decoding the stragegy correctly)
    std::vector<std::string> options;
    
    // The strategy sequence (which should be a vector of vector of station enumbs, but again I'll use 
    // strings instead of enums to test that I've got the decoding correct)
    typedef std::vector<std::string> ChamberGroup;
    std::vector<ChamberGroup> sequence;
    std::string seqStr;

    bool success = false;
    const MuonTrackSteeringStrategy* result = 0;

    std::string::size_type length = strategy.length();
    
    // Extract the strategy name and options
    std::string::size_type begIdx, endIdx;
    begIdx = strategy.find_first_not_of(delims);
    if (std::string::npos != begIdx) {
      endIdx = strategy.find(':', begIdx);
      if (std::string::npos != endIdx) {
        seqStr = strategy.substr(endIdx+1, length-endIdx-1);
        std::string nameopt = strategy.substr(begIdx, endIdx-begIdx);
        std::string::size_type bi = nameopt.find('[');
        if (std::string::npos != bi) {
          name = nameopt.substr(0, bi);
          
          // Decode options
          std::string::size_type ei = nameopt.find(']', bi);
          if (std::string::npos == ei) {
            ei = nameopt.length();
          }
          std::string inputOpt = nameopt.substr(bi+1, ei-bi-1);
          success = decodeList(inputOpt, options);
        } else {
          name = nameopt;
        }
      }
    }
    if( msgLvl(MSG::DEBUG) ){
      ATH_MSG_DEBUG("From strat: " << strategy << " with success " << success << " end " << endIdx << " beg " << begIdx << " Name: " << name << " options: ");
      for( std::vector<std::string>::iterator oit = options.begin(); oit != options.end(); ++oit ) msg(MSG::DEBUG) << "  " << *oit << endmsg;
    }
    // Name and options successfully decoded, now decode the sequence and groups
    if (success) {
      begIdx = endIdx + 1;
      do {
        endIdx = strategy.find(';', begIdx);
        std::string::size_type lstIdx = endIdx;
        if (std::string::npos == endIdx) {
          lstIdx = strategy.length();
        }
        std::string grpString = strategy.substr(begIdx, lstIdx-begIdx);
        ChamberGroup group;
        success = success && decodeList(grpString, group);
        sequence.push_back(group);
        begIdx = lstIdx+1;    
      } while (std::string::npos != endIdx && success);
    }
    
    if (success){
      std::vector< std::vector< MuonStationIndex::ChIndex > > path;
      for (unsigned int i=0;i<sequence.size();++i){
        std::vector< MuonStationIndex::ChIndex > idxGrp;
        for (unsigned int j=0;j<sequence[i].size();++j){
          MuonStationIndex::ChIndex idx = MuonStationIndex::chIndex( sequence[i][j] );
          if (MuonStationIndex::ChUnknown == idx){
            if ( sequence[i][j] != "all" && sequence[i][j] != "ALL" && sequence[i][j] != "All") {
              // Complain
              ATH_MSG_WARNING("I am complaining: Bad station index.");
            } else { // asked for all chambers
              idxGrp.clear();
              for (int all=MuonStationIndex::BIS; all!=MuonStationIndex::ChIndexMax; ++all) idxGrp.push_back( MuonStationIndex::ChIndex(all) );
            }
          } else {
            idxGrp.push_back( idx );
          }
        }
        path.push_back( idxGrp );
      }
      result = new MuonTrackSteeringStrategy(name, options, path);
    }
    
    return result;
  }

  //-----------------------------------------------------------------------------------------------------------

  bool MuonTrackSteering::decodeList(const std::string& input, std::vector<std::string>& list) const {
    bool result = true;
    std::string::size_type begIdx = 0;
    std::string::size_type endIdx = 0;
    do {
      endIdx = input.find(',', begIdx);
      std::string::size_type lstIdx = endIdx;
      if (std::string::npos == endIdx) {
        lstIdx = input.length();
      }
      std::string item = input.substr(begIdx, lstIdx-begIdx);
      list.push_back(item);
      begIdx = lstIdx+1;    
    } while (std::string::npos != endIdx);
    return result;
  }

} // End of namespace
