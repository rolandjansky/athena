/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>

#include "InDetSecVtxTrackSelectionTool/InDetSecVtxTrackSelectionTool.h"
#include "InDetSecVtxTrackCut.h"

#include "AsgTools/Check.h"
#include "xAODTracking/TrackingPrimitives.h"
#ifndef XAOD_ANALYSIS
#include "TrkTrack/Track.h"
#include "VxVertex/Vertex.h"
#include "TrkTrackSummary/TrackSummary.h"
//#include "TrkEventPrimitives/FitQuality.h"
#endif

#include <memory>

namespace {
  // define a local make_unique to use in gcc version < 4.9
#if __cplusplus > 201103L
  using std::make_unique;
#else
  // this version of make_unique is taken from Scott Meyers Effective Modern C++, item 21
  template<typename T, typename... Ts>
  std::unique_ptr<T> make_unique(Ts&&... params)
  {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
  }
#endif
}

InDet::InDetSecVtxTrackSelectionTool::InDetSecVtxTrackSelectionTool(const std::string& name )
  : asg::AsgTool(name) 
  , m_acceptInfo( "InDetSecVtxTrackSelection" )
//#ifndef XAOD_ANALYSIS
//  , m_trkFilter( "InDet::InDetTrackSelection", this )
//#else
//  , m_trkFilter( "InDet::InDetTrackSelection", this )
//#endif
{

#ifndef XAOD_STANDALONE
  declareInterface<IInDetTrackSelectionTool>(this);
#endif
//  declareProperty("TrackSelector",  m_trkFilter );

  declareProperty("minD0", m_minD0, "Minimum transverse separation");
  declareProperty("minNPixelHitsAtZeroTRT", m_NPixel0TRT , "Minimum number of Pixel hit upon zero TRT hit" ) ;
  declareProperty("minTotalHits", m_minInDetHits , "Minimum number of Pixel + Sct + TRT hits" ) ;

}

// we must define the destructor in order to use forward-declaration with unique_ptr
InDet::InDetSecVtxTrackSelectionTool::~InDetSecVtxTrackSelectionTool() = default;

/// 
StatusCode InDet::InDetSecVtxTrackSelectionTool::initialize() {
  // Make sure we haven't already been initialized - this would be a sign of improper usage.
  StatusCode ini = StatusCode::SUCCESS ;
  if (m_isInitialized) {
    ATH_MSG_ERROR( "Tool has already been initialized. This is illegitimate." );
    ATH_MSG_ERROR( "This call to initialize() will do nothing." );
    return ini;
  }

  // Greet the user:
  ATH_MSG_INFO( "Initializing SecVtx track selection tool." );
  ATH_CHECK( asg::AsgTool::initialize() );

//#ifndef XAOD_ANALYSIS
//  ATH_CHECK( m_trkFilter.retrieve() ) ;
//  ATH_MSG_DEBUG( "Retrieved original InDetTrackSelectionTool : " << m_trkFilter.typeAndName() ) ;
//#endif

  // if the CutLevel string is set to something recognizable,
  // then do a soft set on the cuts (i.e. not overwriting those already set)

  ATH_MSG_DEBUG( " SecVtx trackSelector " << " local cuts initializing" ) ;

  if ( m_minD0 >= 0 ) 
  {
    ATH_MSG_INFO( "  Maximum on d0: " << m_minD0 << " mm" );
    m_trackCuts["D0"].push_back(make_unique<D0minCut>(this, m_minD0));
  }

  if ( m_NPixel0TRT > 0 )
  {
    ATH_MSG_INFO( "  Minimum number of Pixel hit when TRT has zero hit: " << m_NPixel0TRT  );

    auto minPixelHits = make_unique< FuncSummaryValueCut<3> >
    (  this, std::array<xAOD::SummaryType,3>
             (  
               { xAOD::numberOfTRTHits, xAOD::numberOfPixelHits, xAOD::numberOfPixelDeadSensors } 
//  eta acceptance and outliers are ignored for TRT  
             ) 
    );

    minPixelHits->setFunction( [=] (const std::array<uint8_t, 3>& vals )
     {  return ( vals[0] > 0 ) || ( ( vals[1] >= m_NPixel0TRT ) || ( vals[2] >= m_NPixel0TRT ) ) ; } 
                             );

    m_trackCuts["minPixelHits0TRT"].push_back( std::move( minPixelHits ) );

  }

  if ( m_minInDetHits > 0 )
  {
    ATH_MSG_INFO( "  Minimum number of Pixel + SCT + TRT hits: " << m_minInDetHits  );

    auto mintotHits = make_unique< FuncSummaryValueCut<4> >
    (  this, std::array<xAOD::SummaryType,4>
             (
               { xAOD::numberOfInnermostPixelLayerHits, xAOD::numberOfPixelHits, 
                 xAOD::numberOfSCTHits, xAOD::numberOfTRTHits }
             )
    );

    mintotHits->setFunction( [=] (const std::array<uint8_t, 4>& vals )
      {  return ( vals[0] + vals[1] + vals[2] + vals[3] ) >= m_minInDetHits ; }
    );

    m_trackCuts["minTotalHits"].push_back( std::move( mintotHits ) );
  }


  for (const auto& cutFamily : m_trackCuts) {
    for (const auto& cut : cutFamily.second) {
      ATH_CHECK( cut->initialize() );
    }
    const std::string& cutFamilyName = cutFamily.first;
    m_numTracksPassedCuts.push_back(0);
    if (m_acceptInfo.addCut( cutFamilyName, "Selection of SecVtx tracks according to " + cutFamilyName ) < 0) {
      ATH_MSG_ERROR( "Failed to add cut family " << cutFamilyName << " because the TAccept object is full." );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Adding cut family " << cutFamilyName);
  }

  m_isInitialized = true;
  
  return ini ;
}
  
StatusCode InDet::InDetSecVtxTrackSelectionTool::finalize()
{
  StatusCode fin = StatusCode::SUCCESS ;

  ATH_MSG_INFO("Finalizing track selection tool.");
  if (!m_isInitialized) {
    ATH_MSG_ERROR( "You are attempting to finalize a tool that has not been initialized()." );
  }

  if (m_numTracksProcessed == 0) {
    ATH_MSG_INFO( "No tracks processed in selection tool." );
    return fin ;
  }
  ATH_MSG_INFO( m_numTracksPassed << " / " << m_numTracksProcessed << " = "
		<< m_numTracksPassed*100./m_numTracksProcessed << "% passed all cuts." );
/**
  for (const auto& cutFamily : m_trackCuts) {
    ULong64_t numPassed = m_numTracksPassedCuts.at(m_acceptInfo.getCutPosition(cutFamily.first));
    ATH_MSG_INFO( numPassed << " = " << numPassed*100./m_numTracksProcessed << "% passed "
		  << cutFamily.first << " cut." );
  }
**/

  return fin ; 

}

 
const asg::AcceptInfo& InDet::InDetSecVtxTrackSelectionTool::getAcceptInfo() const
{  
  return m_acceptInfo;
}


asg::AcceptData
InDet::InDetSecVtxTrackSelectionTool::accept( const xAOD::IParticle* p ) const
{
  asg::AcceptData acceptData(&m_acceptInfo);     
  // Check if this is a track:
  if( p->type() != xAOD::Type::TrackParticle ) {
    ATH_MSG_ERROR( "accept(...) Function received a non-track" );
    return acceptData;
  }
    
  // Cast it to a track (we have already checked its type so we do not have to dynamic_cast):
  const xAOD::TrackParticle* trk = static_cast< const xAOD::TrackParticle* >( p );
    
  // Let the specific function do the work:
//#ifndef XAOD_ANALYSIS
//  m_accept = m_trkFilter->accept( *trk, nullptr );
//  if ( ! (bool)( m_accept ) ) return m_accept ;
//#endif

  return accept( *trk, nullptr );
}


/// 
asg::AcceptData InDet::InDetSecVtxTrackSelectionTool::accept( const xAOD::TrackParticle& trk,
							     const xAOD::Vertex* vtx ) const
{
  if (!m_isInitialized) {
    if (!m_warnInit) {
      ATH_MSG_WARNING( "Tool is not initialized! Calling accept() will not be very helpful." );
      m_warnInit = true;
    }
  }

  asg::AcceptData acceptData(&m_acceptInfo);

  bool passAll = true;

//#ifndef XAOD_ANALYSIS
//  m_accept = m_trkFilter->accept( trk, vtx ) ;
//  if ( ! (bool)( m_accept ) ) 
//  {
//    ATH_MSG_DEBUG( "SecVtx Failed at usual track selection ! " ) ;
//    return m_accept ;
//  }
//#endif

  for ( auto& accessor : m_trackAccessors ) {
    if( !  accessor.second->access( trk, vtx ).isSuccess() ) {
      ATH_MSG_WARNING("Track access for " << accessor.first << " unsuccessful.");
    }
  }

  // loop over all cuts
  UShort_t cutFamilyIndex = 0;
  for ( const auto& cutFamily : m_trackCuts ) {
    bool pass = true;

    for ( const auto& cut : cutFamily.second ) {
      if (! cut->result() ) {
	pass = false;
	passAll = false;
	break;
      }
    }
    acceptData.setCutResult( cutFamilyIndex, pass );
    if (pass) m_numTracksPassedCuts.at(cutFamilyIndex)++; // number of tracks that pass each cut family
    cutFamilyIndex++;
  }


  if (passAll) m_numTracksPassed++;

  m_numTracksProcessed++;
  
  return acceptData;
}

/// Function to set the cut level within standalone ROOT
/// 
/// This function can be used to set the cut selection of the tool to a
/// pre-defined level, as definied in the twiki for
/// InDetTrackingPerformanceGuidelines. It is left public for use in
/// standalone ROOT. In athena, this should be set through the jobOptions
/// via the "CutLevel" property instead.
///
/// @param level The CutLevel enumeration that picks the preset level
///              to set the cuts to.
/// @param overwrite A boolean (default true) that indicates whether
///                  to force an overwrite of each cut. If false, this
///                  function does not change the cut levels if they
///                  have been altered from their default (no cut) value.
///
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
void InDet::InDetSecVtxTrackSelectionTool::setCutLevel(InDet::CutLevel /* level */, Bool_t /* overwrite */)
{
#ifndef XAOD_STANDALONE
  ATH_MSG_WARNING( "InDetTrackSelectionTool::setCutLevel() is not designed to be called manually in Athena." );
  ATH_MSG_WARNING( "It may not behave as intended. Instead, configure it in the job options through the CutLevel property." );
#endif // XAOD_STANDALONE
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


#ifndef XAOD_ANALYSIS
///

asg::AcceptData
InDet::InDetSecVtxTrackSelectionTool::accept( const Trk::Track& /* track */,
					      const Trk::Vertex* /* vertex */ ) const
{
  if (!m_isInitialized) ATH_MSG_WARNING( "Tool is not initialized! Calling accept() will not be very helpful." );

  asg::AcceptData acceptData(&m_acceptInfo);
  
  bool passAll = true;

//  m_accept = m_trkFilter->accept( track, vertex ) ;
//  if ( ! (bool)( m_accept ) ) return m_accept ;
  
  // for faster lookup in setCutResult we will keep track of the index explicitly
  UShort_t cutFamilyIndex = 0;
  for ( const auto& cutFamily : m_trackCuts ) {
    bool pass = true;
    for ( const auto& cut : cutFamily.second ) {
      if (! cut->result() ) {
	pass = false;
	passAll = false;
	break;
      }
    }
    acceptData.setCutResult( cutFamilyIndex, pass );
    if (pass)
      m_numTracksPassedCuts.at(cutFamilyIndex)++; // increment the number of tracks that passed this cut family
    cutFamilyIndex++;
  }
  
  if (passAll)
    m_numTracksPassed++;

  m_numTracksProcessed++;

  return acceptData;
}

#endif // XAOD_ANALYSIS

