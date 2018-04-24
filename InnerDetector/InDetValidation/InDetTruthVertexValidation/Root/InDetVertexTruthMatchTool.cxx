/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"

#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"
using namespace InDetVertexTruthMatchUtils;

InDetVertexTruthMatchTool::InDetVertexTruthMatchTool( const std::string & name ) : asg::AsgTool(name) {
  declareProperty("trackMatchProb", m_trkMatchProb = 0.7 );
  declareProperty("vertexMatchWeight", m_vxMatchWeight = 0.7 );
  declareProperty("trackPtCut", m_trkPtCut = 100. );
}

StatusCode InDetVertexTruthMatchTool::initialize() {
  ATH_MSG_INFO("Initializing");

  return StatusCode::SUCCESS;
}


namespace {
  //Helper methods for this file only

  //output typedef
  // this is defined like this in InDetTruthMatchUtils:
  // typedef std::pair<ElementLink<xAOD::TruthEventBaseContainer>, float> VertexTruthMatchInfo;
  // std::pair<ElementLink<>, T> is special templated pair in ElementLink.h even
  // pair of link to a truth event with relative weight of matched tracks


  // Create a truth map by decoarting truth particles with a back link to the truth event they live in
  // Needed because the track->truth assoc gives us the particles but they don't store event normally
  // Add as decoration to avoid full loop for every track ( this time only once per event )
  // Use a vector so any number of truth event collections can be used at once -- but the pointers need to be valid
  void createTruthMap(std::vector<const xAOD::TruthEventBaseContainer *> truthEventContainers ) {

    xAOD::TruthParticle::Decorator<ElementLink<xAOD::TruthEventBaseContainer> > backLinkDecor("TruthEventLink");

    for( auto cit : truthEventContainers ) {
      const xAOD::TruthEventBaseContainer & truthEvents = *cit;
      
      for( size_t i=0; i< truthEvents.size(); ++i) {
        for( auto & tkit : truthEvents[i]->truthParticleLinks() ) { //std::vector<ElementLink...
          backLinkDecor(**tkit) = ElementLink<xAOD::TruthEventBaseContainer>( truthEvents, i );
        }
      }

    }

  }

  void createTrackTruthMap(std::vector<const xAOD::TruthEventBaseContainer *> truthEventContainers,
			   const xAOD::TrackParticleContainer & trackParticleContainer,
			   float matchCut)
  {
    createTruthMap(truthEventContainers);

    xAOD::TrackParticle::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > trk_truthPartAcc("truthParticleLink");
    xAOD::TrackParticle::ConstAccessor<float> trk_truthProbAcc("truthMatchProbability");
    xAOD::TrackParticle::Decorator<bool> selected("GoodTrack");
    xAOD::TruthParticle::Decorator<ElementLink<xAOD::TruthEventBaseContainer> > backLinkDecor("TruthEventLink");
    xAOD::TrackParticle::Decorator<ElementLink<xAOD::TruthEventBaseContainer> > trackLinkDecor("TrackEventLink");

    int nGood = 0;
    int nMatch = 0;
    int nLink = 0;
    for (auto trk : trackParticleContainer)
    {
      if (selected.isAvailable(*trk) && selected(*trk))
      {
	nGood++;
	if (trk_truthPartAcc.isAvailable(*trk) && trk_truthProbAcc.isAvailable(*trk) 
	    && trk_truthPartAcc(*trk).isValid() && trk_truthProbAcc(*trk) >= matchCut)
	{
	  nMatch++;
	  auto truthParticle = trk_truthPartAcc(*trk);
	  if (backLinkDecor.isAvailable(**truthParticle) && backLinkDecor(**truthParticle).isValid())
	  {
	    nLink++;
	    trackLinkDecor(*trk) = backLinkDecor(**truthParticle);
	  }
	}
      }
    }
    // won't compile, no idea why
    //ATH_MSG_DEBUG("Linked/Matched/Good/All: " << nLink << " / " << nMatch << " / " << nGood << " / " << trackParticleContainer.size());
  }

  //In the vector of match info, find the element corresponding to link and return its index; create a new one if necessary
  size_t indexOfMatchInfo( std::vector<VertexTruthMatchInfo> & matches, ElementLink<xAOD::TruthEventBaseContainer> & link ) {
    for( size_t i = 0; i< matches.size(); ++i ) {
      if( link.key() == matches[i].first.key() && link.index() == matches[i].first.index() )
        return i;
    }
    // This is the first time we've seen this truth vertex, so make a new entry
    matches.push_back(  std::make_pair( link, 0. ) );
    return matches.size() - 1;
  }


  //for sorting the container -> highest relative match weight first
  bool compareMatchPair( VertexTruthMatchInfo a, VertexTruthMatchInfo b ) { return a.second > b.second; }


}

const xAOD::TrackParticleContainer* 
InDetVertexTruthMatchTool::findTrackParticleContainer( const xAOD::VertexContainer& vxContainer )
{
  for (auto vtx : vxContainer)
  {
    for (const ElementLink<xAOD::TrackParticleContainer>& tpLink : vtx->trackParticleLinks())
    {
      if (tpLink.isValid())
      {
	return tpLink.getStorableObjectPointer();
      }
    }
  }
  return 0;
}

StatusCode InDetVertexTruthMatchTool::matchVertices( const xAOD::VertexContainer & vxContainer ) {

  ATH_MSG_DEBUG("Start vertex matching");


  // Identify MC vertices to match to -- this is the collection for hard scatter
  const xAOD::TruthEventBaseContainer * truthEvents = 0;
  if( evtStore()->contains<xAOD::TruthEventBaseContainer>( "TruthEvents" ) )
    ATH_CHECK( evtStore()->retrieve( truthEvents, "TruthEvents" ) );
  else
    ATH_CHECK( evtStore()->retrieve( truthEvents, "TruthEvent" ) );

  std::vector<const xAOD::TruthEventBaseContainer *> truthContainers;
  truthContainers.push_back( truthEvents );

  // These are the pile-up truth -- don't want to fail if they don't exist
  const xAOD::TruthEventBaseContainer * truthPileup = 0;
  if( evtStore()->contains<xAOD::TruthEventBaseContainer>( "TruthPileupEvents" ) )
    ATH_CHECK( evtStore()->retrieve( truthPileup, "TruthPileupEvents" ) );
  if(truthPileup)
    truthContainers.push_back( truthPileup );

  // Find the trackParticle container associated with our reconstructed vertices
  // We could pass this, but it would break the original interface...
  const xAOD::TrackParticleContainer* tkContainer = findTrackParticleContainer(vxContainer);
  if (tkContainer == 0)
  {
    ATH_MSG_WARNING("Vertex container has no vertices with valid TrackParticle links");
    return StatusCode::SUCCESS;
  }

  // create the particle links to events to avoid excessive looping
  // also decorate reconstructed tracks passing selection with truthEvent links
  createTrackTruthMap( truthContainers, *tkContainer, m_trkMatchProb );

  // Accessor for the links we just created
  xAOD::TruthParticle::Decorator<ElementLink<xAOD::TruthEventBaseContainer> > backLinkDecor("TruthEventLink");

  //setup decorators for truth matching info
  xAOD::Vertex::Decorator<std::vector<VertexTruthMatchInfo> > matchInfoDecor("TruthEventMatchingInfos");
  xAOD::Vertex::Decorator<std::vector<VertexTruthMatchInfo> > rawMatchInfoDecor("TruthEventRawMatchingInfos");
  xAOD::Vertex::Decorator<VertexMatchType> matchTypeDecor("VertexMatchType");
  xAOD::Vertex::Decorator<std::vector<ElementLink<xAOD::VertexContainer> > > splitPartnerDecor("SplitPartners");

  //setup accessors 
  // can switch to built in method in xAOD::Vertex once don't have to deal with changing names anymore
  xAOD::Vertex::ConstAccessor<xAOD::Vertex::TrackParticleLinks_t> trkAcc("trackParticleLinks");
  xAOD::Vertex::ConstAccessor<std::vector<float> > weightAcc("trackWeights");

  xAOD::TrackParticle::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > trk_truthPartAcc("truthParticleLink");
  xAOD::TrackParticle::ConstAccessor<float> trk_truthProbAcc("truthMatchProbability");

  xAOD::TrackParticle::Decorator<ElementLink<xAOD::VertexContainer> > trk_recoVtx("RecoVertex");
  xAOD::TrackParticle::Decorator<float> trk_wtVtx("WeightVertex");

  xAOD::TrackParticle::Decorator<bool> selected("GoodTrack");

  //some variables to store
  size_t ntracks;
  xAOD::VxType::VertexType vxType;

  //=============================================================================
  //First loop over vertices: get tracks, then TruthParticles, and store relative
  //weights of contribution from each TruthEvent
  //=============================================================================
  size_t vxEntry = 0;
  for( auto vxit : vxContainer ) {
    vxEntry++;
    vxType = static_cast<xAOD::VxType::VertexType>( vxit->vertexType() );
    if(vxType == xAOD::VxType::NoVtx) { 
      //skip dummy vertices -> match info will be empty vector if someone tries to access later
      //type will be set ot dummy
      continue;
    }


    //create the vector we will add as matching info decoration later
    std::vector<VertexTruthMatchInfo> matchinfo;
    std::vector<VertexTruthMatchInfo> rawMatchinfo; //not normalized to one for each reco vertex

    //if don't have track particles
    if(!trkAcc.isAvailable(*vxit) || !weightAcc.isAvailable(*vxit) ) {
      ATH_MSG_DEBUG("trackParticles or trackWeights not available, setting fake");
      // Add invalid link for fakes
      matchinfo.push_back( std::make_pair( ElementLink<xAOD::TruthEventBaseContainer>(), 1. ) );
      matchInfoDecor( *vxit ) = matchinfo;
      rawMatchinfo.push_back( std::make_pair( ElementLink<xAOD::TruthEventBaseContainer>(), 1. ) );
      rawMatchInfoDecor( *vxit ) = rawMatchinfo;
      //      matchInfoDecor( *vxit ) = createMatchInfo( truthEventMatchWeights, *truthEvents, 1. );
      continue;
    } 

    //things we need to do the matching
    const xAOD::Vertex::TrackParticleLinks_t & trkParts = trkAcc( *vxit );
    ntracks = trkParts.size();
    const std::vector<float> & trkWeights = weightAcc( *vxit );

    //double check
    if( trkWeights.size() != ntracks ) {
      ATH_MSG_DEBUG("Vertex without same number of tracks and trackWeights, setting fake");
      matchinfo.push_back( std::make_pair( ElementLink<xAOD::TruthEventBaseContainer>(), 1. ) );
      matchInfoDecor( *vxit ) = matchinfo;
      rawMatchinfo.push_back( std::make_pair( ElementLink<xAOD::TruthEventBaseContainer>(), 1. ) );
      rawMatchInfoDecor( *vxit ) = rawMatchinfo;
      // // creation method will check size of map and add a fake match if needed
      // matchInfoDecor( *vxit ) = createMatchInfo( truthEventMatchWeights, *truthEvents, 1. );
      continue;
    }

    ATH_MSG_DEBUG("Matching new vertex at (" << vxit->x() << ", " << vxit->y() << ", " << vxit->z() << ")" << " with " << ntracks << " tracks:");
    
    float totalWeight = 0.;
    float totalFake = 0.;


    //loop element link to track particle
    for( size_t t = 0; t< ntracks; ++t ) {
      const xAOD::TrackParticle & trk = **trkParts[t];

      //don't include failing tracks in the total weight
      //      if( !pass( trk ) ) continue;
      if (!selected.isAvailable( trk ) || !selected( trk ) ) continue;

      totalWeight += trkWeights[t];
      trk_recoVtx(trk) = ElementLink<xAOD::VertexContainer>(vxContainer, vxEntry - 1);
      trk_wtVtx(trk) = trkWeights[t];

      const ElementLink<xAOD::TruthParticleContainer> & truthPartLink = trk_truthPartAcc( trk );
      float prob = trk_truthProbAcc( trk );
      
      if(truthPartLink.isValid() && prob > m_trkMatchProb) {
        const xAOD::TruthParticle & truthPart = **truthPartLink;
        //check if the truth particle is "good"
        if( pass( truthPart) ) {
          //use the TruthEvent matching to connect to event
          //int matchIdx = particleInEvent( truthPart, *truthEvents );
          ElementLink<xAOD::TruthEventBaseContainer> match = backLinkDecor( truthPart );
          //check we have an actual link
          if( match.isValid() ) {
            size_t matchIdx = indexOfMatchInfo( matchinfo, match );
            matchinfo[matchIdx].second += trkWeights[t];
	    matchIdx = indexOfMatchInfo( rawMatchinfo, match );
	    rawMatchinfo[matchIdx].second += trkWeights[t];
          } else {
            totalFake += trkWeights[t];
          }

          //if can't connect to an event then matching method already returned -1 for fake
          //          truthEventMatchWeights[matchIdx] += trkWeights[t];
          
        } else {
          //truth particle failed cuts -> add to fakes
          //          truthEventMatchWeights[-1] += trkWeights[t];
          totalFake += trkWeights[t];
        }
      } else {
        //not valid or low matching probability -> add to fakes
        //        truthEventMatchWeights[-1] += trkWeights[t];
        totalFake += trkWeights[t];
      }
    }//end loop over tracks in vertex
    
    //finalize the match info vector
    if( totalWeight < 1e-12 ) {// in case we don't have any passing tracks we want to make sure labeled fake
      ATH_MSG_DEBUG("   Declaring vertex fully fake (no passing tracks included)");
      totalWeight = 1.;
      totalFake = 1.;
    }
    if( totalFake > 0. )
    {
      matchinfo.push_back( std::make_pair( ElementLink<xAOD::TruthEventBaseContainer>(), totalFake ) );
      rawMatchinfo.push_back( std::make_pair( ElementLink<xAOD::TruthEventBaseContainer>(), totalFake ) );
    }

    for( auto & mit : matchinfo ) {
      mit.second /= totalWeight;
    }
    std::sort( matchinfo.begin(), matchinfo.end(), compareMatchPair );
    std::sort( rawMatchinfo.begin(), rawMatchinfo.end(), compareMatchPair );
    matchInfoDecor( *vxit ) = matchinfo;
    rawMatchInfoDecor( *vxit ) = rawMatchinfo;
  }


  //After first loop, all vertices have been decorated with their vector of match info (link to TruthEvent paired with weight)
  //now we want to use that information from the whole collection to assign types

  //keep track of whether a type is assigned 
  //useful since looking for splits involves a double loop, and then setting types ahead in the collection
  std::vector<bool> assignedType( vxContainer.size(), false );

  for( size_t i=0; i< vxContainer.size(); ++i ) {

    if( assignedType[i] ) continue;

    std::vector<VertexTruthMatchInfo> & info = matchInfoDecor( *vxContainer[i] );
    if(info.size() == 0) {
      matchTypeDecor( *vxContainer[i] ) = DUMMY;
    } else if ( !info[0].first.isValid() ) {
      matchTypeDecor( *vxContainer[i] ) = FAKE;
    } else if ( info[0].second > m_vxMatchWeight ) {
      matchTypeDecor( *vxContainer[i] ) = MATCHED;
    } else {
      matchTypeDecor( *vxContainer[i] ) = MERGED;
    }
    
    //check for splitting
    if( matchTypeDecor( *vxContainer[i] ) == MATCHED || matchTypeDecor( *vxContainer[i] ) == MERGED ) {
      std::vector<size_t> foundSplits;
      for( size_t j=i+1; j < vxContainer.size(); ++j ) {
        std::vector<VertexTruthMatchInfo> & info2 = matchInfoDecor( *vxContainer[j] );
        //check second vertex is not dummy or fake, and that it has same elementlink as first vertex
        //equality test is in code but doesnt seem to work for ElementLinks that I have?
        //so i am just checking that the contianer key hash and the index are the same
        if( info2.size() > 0 && info2[0].first.isValid() && info[0].first.key() == info2[0].first.key() && info[0].first.index() == info2[0].first.index() ) {


          matchTypeDecor( *vxContainer[i] ) = SPLIT;
          matchTypeDecor( *vxContainer[j] ) = SPLIT;
          
          //add split links; first between first one found and newest one
          splitPartnerDecor( *vxContainer[i] ).push_back( ElementLink<xAOD::VertexContainer>( vxContainer, j ) );
          splitPartnerDecor( *vxContainer[j] ).push_back( ElementLink<xAOD::VertexContainer>( vxContainer, i ) );
          //then between any others we found along the way
          for( auto k : foundSplits ) { //k is a size_t in the vector of splits
            splitPartnerDecor( *vxContainer[k] ).push_back( ElementLink<xAOD::VertexContainer>( vxContainer, j ) );
            splitPartnerDecor( *vxContainer[j] ).push_back( ElementLink<xAOD::VertexContainer>( vxContainer, k ) );
          }
          //then keep track that we found this one
          foundSplits.push_back(j);
          //and that we already set up its type
          assignedType[j] = true;
        } //if the two vertices match to same TruthEvent
      }//inner loop over vertices
    } //if matched or merged
  } //outer loop


  //DEBUG MATCHING
  for( auto vxit : vxContainer ) {
    ATH_MSG_DEBUG("Matched vertex to type " << matchTypeDecor(*vxit) << " with following info of size " << matchInfoDecor(*vxit).size() << ":");
    for( auto vit : matchInfoDecor(*vxit) ) {
      if( vit.first.isValid() ) {
        ATH_MSG_DEBUG("  GenEvent type " << (* vit.first)->type() << ", index " << vit.first.index() << " with relative weight " << vit.second );
      } else {
        ATH_MSG_DEBUG("  Fakes with relative weight " << vit.second );
      }
    }
    if(matchTypeDecor(*vxit)==SPLIT) {
      ATH_MSG_DEBUG("  Split partners are:");
      for(auto split : splitPartnerDecor( *vxit ) ) {
        if( split.isValid() )
          ATH_MSG_DEBUG("    Vertex " << split.index());
        else
          ATH_MSG_DEBUG("    ERROR");
      }
    }
  }
  
  return StatusCode::SUCCESS;

}


//Set up any cuts on either the tracks or truth particles to allow here
//A failing track is removed from consideration entirely
//If a passing track matches to a failing truth particle it will be considered "fake"

bool InDetVertexTruthMatchTool::pass( const xAOD::TruthParticle & truthPart ) {

  //remove the registered secondaries
  if( truthPart.barcode() > 200000 ) return false;
  
  return true;

}

/*
bool InDetVertexTruthMatchTool::pass( const xAOD::TrackParticle & trackPart ) {

  if( trackPart.pt() < m_trkPtCut ) return false;
  
  return true;

}
*/
