/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// A unit test for the track selection tool. Currently this compares the pre-defined cut levels
// to hard-coded versions that can be easily read. Does not check every possible cut option.

// System include(s):
#include <memory>
#include <cstdlib>
#include <cassert>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// Local include(s):
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

using std::string;
using std::vector;
using std::map;
using std::unique_ptr;
using InDet::InDetTrackSelectionTool;
using xAOD::TrackParticle;
using tool_ptr = unique_ptr<InDetTrackSelectionTool>;

bool passNoCut( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passLoose( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passLoosePrimary( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passTightPrimary( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passLooseMuon( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passLooseElectron( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passLooseTau( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passMinBias( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passHILoose( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passHITight( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passITkLoose( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
bool passExpPix( const TrackParticle& trk, const xAOD::Vertex* vtx = nullptr );
uint8_t getSum(const TrackParticle&, xAOD::SummaryType);
void dumpTrack( const TrackParticle& );

int main( int argc, char* argv[] ) {
  
   // The application's name:
   const char* APP_NAME = argv[ 0 ];
#define CHECK( ARG ) do {ASG_CHECK_SA( APP_NAME, ARG );} while (false)


   string filename;
   filename = getenv("ROOTCORE_TEST_FILE");
   if (filename.empty()) {
     Error( APP_NAME, "Could not find $ROOTCORE_TEST_FILE." );
     return 1;
   }
 
  // fail on an unchecked StatusCode
   StatusCode::enableFailure();

   // Initialise the application:
   ASG_CHECK_SA( APP_NAME, static_cast<StatusCode>(xAOD::Init( APP_NAME )) );

   map<string, tool_ptr> selTools;
   map<string, bool (*)(const TrackParticle&, const xAOD::Vertex*)> cutFuncs;

#define FUNC_HELP( CUT ) do {cutFuncs[ #CUT ] = pass##CUT;} while (false)
   FUNC_HELP( NoCut );
   FUNC_HELP( Loose );
   FUNC_HELP( LoosePrimary );
   FUNC_HELP( TightPrimary );
   FUNC_HELP( LooseMuon );
   FUNC_HELP( LooseElectron );
   FUNC_HELP( LooseTau );
   FUNC_HELP( MinBias );
   FUNC_HELP( HILoose );
   FUNC_HELP( HITight );
   FUNC_HELP( ITkLoose );
#undef FUNC_HELP

   for (const auto& cutLevelPair : cutFuncs) {
     const auto& cutLevel = cutLevelPair.first;
     string toolName = "TrackSel";
     toolName += cutLevel;
     selTools[cutLevel] = tool_ptr( new InDetTrackSelectionTool(toolName, cutLevel) );
     CHECK( selTools[cutLevel]->initialize() );
   }
   // handle the experimental one differently: add the map entry after initializing the others because it is not a selection level
   cutFuncs["ExpPix"] = passExpPix;
   selTools["ExpPix"] = tool_ptr( new InDetTrackSelectionTool("TrackSelExpPix") );
   CHECK( selTools["ExpPix"]->setProperty( "useExperimentalInnermostLayersCut", 1 ) );
   CHECK( selTools["ExpPix"]->initialize() );

   // Open the input file:
   Info( APP_NAME, "Opening file: %s", filename.data() );
   unique_ptr< TFile > ifile( TFile::Open( filename.data(), "READ" ) );
   StatusCode gotFile = ifile.get()!=nullptr ? StatusCode::SUCCESS : StatusCode::FAILURE;
   CHECK( gotFile );

   // Create a TEvent object:
   // xAOD::TEvent event( static_cast<TFile*>(nullptr), xAOD::TEvent::kClassAccess );
   // ASG_CHECK_SA( APP_NAME, static_cast<StatusCode>(event.readFrom( ifile.get() )) );
   xAOD::TEvent event( ifile.get(), xAOD::TEvent::kAthenaAccess );
   Info( APP_NAME, "Number of events in the file: %llu", event.getEntries() );

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }
   
   for (Long64_t entry = 0; entry < entries; ++entry) {
     CHECK( !event.getEntry(entry) );

     // the MinBias cut has IP cuts w.r.t. the primary vertex
     const xAOD::VertexContainer* vertices = nullptr;
     if ( !event.retrieve( vertices, "PrimaryVertices" ) ) {
       Error( APP_NAME, "Could not retrieve primary vertices." );
       return 2;
     }
     const xAOD::Vertex* primaryVertex = nullptr;
     auto find_vtx = std::find_if(vertices->cbegin(), vertices->cend(),
				  [](const xAOD::Vertex* vtx)
				  {return vtx->vertexType() == xAOD::VxType::PriVtx;} );
     if (find_vtx == vertices->cend()) Warning( APP_NAME, "No primary vertex." );
     else primaryVertex = *find_vtx;

     const xAOD::TrackParticleContainer* tracks = nullptr;
     if ( !event.retrieve( tracks, "InDetTrackParticles" ) ) {
       Error( APP_NAME, "Could not retrieve track particles." );
       return 2;
     }

     for (const auto track : *tracks) {
       for (const auto& cutLevelPair : cutFuncs) {
	 const auto& cutLevel = cutLevelPair.first;
	 if ( selTools[cutLevel]->accept( *track, primaryVertex )
	      != cutFuncs[cutLevel]( *track, primaryVertex ) ) {
	   Error( APP_NAME, "Track selection tool at %s cut level does not", cutLevel.data() );
	   Error( APP_NAME, "  match hard-coded test function." );
	   const auto& taccept = selTools[cutLevel]->getTAccept();
	   if (taccept) Error( APP_NAME, "Passes tool but not function" );
	   else Error( APP_NAME, "Passes function but not tool" );

	   for (size_t i_cut=0; i_cut < taccept.getNCuts(); ++i_cut) {
	     const auto& cutName = taccept.getCutName(i_cut);
	     Info( APP_NAME, "Result of %s = %i", cutName.Data(), taccept.getCutResult(i_cut) );
	   }

	   dumpTrack( *track ); // output track info

	   assert(false);
	 }
       }
     }

   } // end loop over events

   // finalize all the tools
   for (const auto& cutLevelPair : cutFuncs) {
     CHECK( selTools[cutLevelPair.first]->finalize() );
   }

   // Return gracefully:
   return 0;
}

// The functions are implemented in an easy-to-read form
bool passNoCut( const TrackParticle&, const xAOD::Vertex* )
{
  return true;
}

bool passLoose( const TrackParticle& trk, const xAOD::Vertex* )
{
  if (std::fabs(trk.eta()) > 2.5) return false;
  
  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  if (nPixHits + nSctHits < 7) return false;

  uint8_t nPixShared = getSum(trk, xAOD::numberOfPixelSharedHits);
  uint8_t nSctShared = getSum(trk, xAOD::numberOfSCTSharedHits); // need two shared SCT hits to count for one
  if (nPixShared + nSctShared/2 > 1) return false;

  uint8_t nPixHoles = getSum(trk, xAOD::numberOfPixelHoles);
  uint8_t nSiHoles = nPixHoles + getSum(trk, xAOD::numberOfSCTHoles);
  if (nSiHoles > 2) return false;
  if (nPixHoles > 1) return false;

  return true;  
}

bool passLoosePrimary( const TrackParticle& trk, const xAOD::Vertex* )
{
  if (!passLoose(trk)) return false;
  uint8_t nPixShared = getSum(trk, xAOD::numberOfPixelSharedHits);
  uint8_t nSctShared = getSum(trk, xAOD::numberOfSCTSharedHits); // need two shared SCT hits to count for one
  if (nPixShared + nSctShared > 0) {
    uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits)
      + getSum(trk, xAOD::numberOfPixelDeadSensors);
    uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits)
      + getSum(trk, xAOD::numberOfSCTDeadSensors);
    if (nPixHits + nSctHits < 10) return false;
  }

  return true;
}

bool passTightPrimary( const TrackParticle& trk, const xAOD::Vertex* )
{
  if (!passLoose(trk)) return false;

  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  uint8_t minSiHits = (std::fabs(trk.eta()) <= 1.65) ? 9 : 11;
  if (nPixHits + nSctHits < minSiHits) return false;

  bool expectInnermost = getSum(trk, xAOD::expectInnermostPixelLayerHit);
  bool expectNextToInnermost = getSum(trk, xAOD::expectNextToInnermostPixelLayerHit);
  if (expectInnermost && expectNextToInnermost) {
    uint8_t nInnermost = getSum(trk, xAOD::numberOfInnermostPixelLayerHits);
    uint8_t nNextToInnermost = getSum(trk, xAOD::numberOfNextToInnermostPixelLayerHits);
    if (nInnermost + nNextToInnermost < 1) return false;
  }

  uint8_t nPixHoles = getSum(trk, xAOD::numberOfPixelHoles);
  if (nPixHoles > 0) return false;

  return true;
}

bool passLooseMuon( const TrackParticle& trk, const xAOD::Vertex* )
{
  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  if (nPixHits < 1) return false;
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  if (nSctHits < 5) return false;
  uint8_t nSiHoles = getSum(trk, xAOD::numberOfPixelHoles) + getSum(trk, xAOD::numberOfSCTHoles);
  if (nSiHoles > 2) return false;
  auto absEta = std::fabs(trk.eta());
  if (0.1 < absEta && absEta < 1.9) {
    uint8_t nTrtHits = getSum(trk, xAOD::numberOfTRTHits);
    uint8_t nTrtOutliers = getSum(trk, xAOD::numberOfTRTOutliers);
    if (nTrtHits + nTrtOutliers < 6) return false;
    if (nTrtOutliers >= 0.9*(nTrtHits + nTrtOutliers)) return false;
  }

  return true;
}

bool passLooseElectron( const TrackParticle& trk, const xAOD::Vertex* )
{
  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  if (nPixHits < 1) return false;
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  if (nPixHits + nSctHits < 7) return false;

  return true;
}

bool passLooseTau( const TrackParticle& trk, const xAOD::Vertex* vtx )
{
  if (trk.pt() < 1000.0) return false; // pT cut at 1 GeV

  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  if (nPixHits < 2) return false;
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  if (nPixHits + nSctHits < 7) return false;

  if (std::fabs(trk.d0()) > 1.0) return false;
  if (vtx != nullptr) {
    if (std::fabs(trk.z0() + trk.vz() - vtx->z()) > 1.5) return false;
  }

  return true;
}

bool passMinBias( const TrackParticle& trk, const xAOD::Vertex* vtx )
{
  if (std::fabs(trk.eta()) > 2.5) return false;
  if (trk.pt() < 500.) return false;

  bool expectIBL = getSum(trk, xAOD::expectInnermostPixelLayerHit);
  bool expectBL = getSum(trk, xAOD::expectNextToInnermostPixelLayerHit);
  uint8_t nIBL = getSum(trk, xAOD::numberOfInnermostPixelLayerHits);
  uint8_t nBL = getSum(trk, xAOD::numberOfNextToInnermostPixelLayerHits);
  if (expectIBL) {
    if (nIBL < 1) return false;
  } else {
    if (expectBL && nBL < 1) return false;
  }

  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  if (nPixHits < 1) return false;
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  if (nSctHits < 6) return false;
  
  if (trk.pt() > 10.0*1e3 && TMath::Prob(trk.chiSquared(), trk.numberDoF()) < 0.01) return false;

  if (std::fabs(trk.d0()) > 1.5) return false;
  if (vtx != nullptr) {
    if (std::fabs(trk.z0() + trk.vz() - vtx->z())*std::sin(trk.theta()) > 1.5) return false;
  }

  return true;
}

bool passHILoose( const TrackParticle& trk, const xAOD::Vertex* vtx )
{
  if (std::fabs(trk.eta()) > 2.5) return false;
  bool expectIBL = getSum(trk, xAOD::expectInnermostPixelLayerHit);
  bool expectBL = getSum(trk, xAOD::expectNextToInnermostPixelLayerHit);
  uint8_t nIBL = getSum(trk, xAOD::numberOfInnermostPixelLayerHits);
  uint8_t nBL = getSum(trk, xAOD::numberOfNextToInnermostPixelLayerHits);
  if (expectIBL) {
    if (nIBL < 1) return false;
  } else {
    if (expectBL && nBL < 1) return false;
  }

  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  if (nPixHits < 1) return false;
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  auto pt = trk.pt()*1e-3;
  if (pt >= 0.4 && nSctHits < 6) return false;
  else if (pt >= 0.3 && nSctHits < 4) return false;
  else if (nSctHits < 2) return false;
  
  if (std::fabs(trk.d0()) > 1.5) return false;
  if (vtx != nullptr) {
    if (std::fabs(trk.z0() + trk.vz() - vtx->z())*std::sin(trk.theta()) > 1.5) return false;
  }

  return true;
}

bool passHITight( const TrackParticle& trk, const xAOD::Vertex* vtx )
{
  if (std::fabs(trk.eta()) > 2.5) return false;
  bool expectIBL = getSum(trk, xAOD::expectInnermostPixelLayerHit);
  bool expectBL = getSum(trk, xAOD::expectNextToInnermostPixelLayerHit);
  uint8_t nIBL = getSum(trk, xAOD::numberOfInnermostPixelLayerHits);
  uint8_t nBL = getSum(trk, xAOD::numberOfNextToInnermostPixelLayerHits);
  if (expectIBL) {
    if (nIBL < 1) return false;
  } else {
    if (expectBL && nBL < 1) return false;
  }

  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits) + getSum(trk, xAOD::numberOfPixelDeadSensors);
  if (nPixHits < 2) return false;
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits) + getSum(trk, xAOD::numberOfSCTDeadSensors);
  auto pt = trk.pt()*1e-3;
  if (pt >= 0.4 && nSctHits < 8) return false;
  else if (pt >= 0.3 && nSctHits < 6) return false;
  else if (nSctHits < 4) return false;
  
  if (std::fabs(trk.d0()) > 1.0) return false;
  if (vtx != nullptr) {
    if (std::fabs(trk.z0() + trk.vz() - vtx->z())*std::sin(trk.theta()) > 1.0) return false;
  }

  if (trk.chiSquared() / trk.numberDoF() > 6.0) return false;
  
  return true;
}

bool ITkLoose( const TrackParticle& trk, const xAOD::Vertex* vtx )
{
  return true;
}

// whether the track passes the experimental pixel cut
bool passExpPix( const TrackParticle& trk, const xAOD::Vertex* )
{
  uint8_t nPixHoles = getSum(trk, xAOD::numberOfPixelHoles);
  if (nPixHoles == 0) return true; // if there are no pixel holes, then the track passes regardless
  if (nPixHoles > 1) return false; // if there is more than 1 hole, the track fails regardless

  uint8_t nIBLHits = getSum(trk, xAOD::numberOfInnermostPixelLayerHits);
  uint8_t expectBL = getSum(trk, xAOD::expectNextToInnermostPixelLayerHit);
  uint8_t nBLHits = getSum(trk, xAOD::numberOfNextToInnermostPixelLayerHits);
  
  // make an exception is there is an IBL hit, and the hole is in the BLayer
  if (nIBLHits >= 1) {
    if (expectBL && nBLHits ==0) return true;
  }

  return false;
}

uint8_t getSum( const TrackParticle& trk, xAOD::SummaryType sumType )
{
  uint8_t sumVal=0;
  if (!trk.summaryValue(sumVal, sumType)) {
    Error( "getSum()", "Could not get summary type %i", sumType );
  }
  return sumVal;
}

void dumpTrack( const TrackParticle& trk )
{
  Info( "dumpTrack()", "pt = %f", trk.pt() );
  Info( "dumpTrack()", "eta = %f", trk.eta() );
  Info( "dumpTrack()", "expect ibl = %u", getSum(trk, xAOD::expectInnermostPixelLayerHit) );
  Info( "dumpTrack()", "ibl hit = %u", getSum(trk, xAOD::numberOfInnermostPixelLayerHits) );
  Info( "dumpTrack()", "expect bl = %u", getSum(trk, xAOD::expectNextToInnermostPixelLayerHit) );
  Info( "dumpTrack()", "bl hit = %u", getSum(trk, xAOD::numberOfNextToInnermostPixelLayerHits) );
  Info( "dumpTrack()", "pix hit = %u", getSum(trk, xAOD::numberOfPixelHits) );
  Info( "dumpTrack()", "pix dead = %u", getSum(trk, xAOD::numberOfPixelDeadSensors) );
  Info( "dumpTrack()", "pix holes = %u", getSum(trk, xAOD::numberOfPixelHoles) );
  Info( "dumpTrack()", "sct hit = %u", getSum(trk, xAOD::numberOfSCTHits) );
  Info( "dumpTrack()", "sct dead = %u", getSum(trk, xAOD::numberOfSCTDeadSensors) );
  Info( "dumpTrack()", "sct holes = %u", getSum(trk, xAOD::numberOfSCTHoles) );
  Info( "dumpTrack()", "trt hits = %u", getSum(trk, xAOD::numberOfTRTHits) );
  Info( "dumpTrack()", "trt outliers = %u", getSum(trk, xAOD::numberOfTRTOutliers) );
}
