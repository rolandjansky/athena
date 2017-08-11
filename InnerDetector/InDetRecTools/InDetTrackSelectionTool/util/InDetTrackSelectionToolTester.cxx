/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// a testing utility for the track selection tool mostly copied from CPToolTests.cxx

// System include(s):
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TH2F.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// Local include(s):
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

using std::vector;
using std::string;
using std::unique_ptr;
using TrkSelTool = InDet::InDetTrackSelectionTool;

struct HistFamily
{
public:
  HistFamily(const string&);
  void fill(const xAOD::TrackParticle&, const xAOD::Vertex*);
private:
  TH2* h_pt = nullptr; // these show eta dependence
  TH2* h_si_hits_phys = nullptr;
  TH2* h_si_hits = nullptr;
  TH2* h_si_holes = nullptr;
  TH2* h_si_shared = nullptr;
  TH2* h_ibl_hits = nullptr;
  TH2* h_bl_hits = nullptr;
  TH2* h_ibl_expected = nullptr;
  TH2* h_bl_expected = nullptr;
  TH2* h_pix_hits_phys = nullptr;
  TH2* h_pix_hits = nullptr;
  TH2* h_pix_holes = nullptr;
  TH2* h_pix_shared = nullptr;
  TH2* h_sct_hits_phys = nullptr;
  TH2* h_sct_hits = nullptr;
  TH2* h_sct_holes = nullptr;
  TH2* h_sct_shared = nullptr;
  TH2* h_trt_hits = nullptr;
  TH2* h_trt_outlier_fraction = nullptr;

  TH2* h_d0 = nullptr; // these show IP vs. uncertainty in IP
  TH2* h_z0sintheta = nullptr;

  // a helper function to return the summary
  uint8_t getSum(const xAOD::TrackParticle&, xAOD::SummaryType) const;
};


int main( int argc, char* argv[] ) {

   // The application's name:
   const char* APP_NAME = argv[ 0 ];
#define CHECK( ARG ) do {ASG_CHECK_SA( APP_NAME, ARG );} while (false)

   string fileName;
   // Check if we received a file name:
   if( argc < 2 ) {
     fileName = getenv("ROOTCORE_TEST_FILE");
     if (fileName.empty()) {
       Error( APP_NAME, "No file name received!" );
       Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
       return 1;
     }
   } else {
     fileName = argv[ 1 ];
   }

   // fail on an unchecked StatusCode
   StatusCode::enableFailure();

   // Initialise the application:
   CHECK( static_cast<StatusCode>(xAOD::Init( APP_NAME )) );

   // Open the input file:
   Info( APP_NAME, "Opening file: %s", fileName.data() );
   unique_ptr< TFile > ifile( TFile::Open( fileName.data(), "READ" ) );
   StatusCode gotFile = ifile.get()!=nullptr ? StatusCode::SUCCESS : StatusCode::FAILURE;
   CHECK( gotFile );

   // Create a TEvent object:
   xAOD::TEvent event( static_cast<TFile*>(nullptr), xAOD::TEvent::kAthenaAccess );
   CHECK( static_cast<StatusCode>(event.readFrom( ifile.get() )) );
   Info( APP_NAME, "Number of events in the file: %llu", event.getEntries() );

   // Decide how many events to run over:
   Long64_t entries = event.getEntries();
   if( argc > 2 ) {
      const Long64_t e = atoll( argv[ 2 ] );
      if( e < entries ) {
         entries = e;
      }
   }

   unique_ptr< TFile > outFile(new TFile("IDTrackSelectionToolTestOut.root", "RECREATE"));
   Info( APP_NAME, "Creating output file %s", outFile->GetName() );

   const vector<string> cutNames = {"NoCut", "Loose", "LoosePrimary", "TightPrimary", "LooseMuon", "LooseElectron", "MinBias", "HILoose", "HITight"}; // these are names of pre-defined selections
   const vector<string> otherCutNames = {"IP", "IPSigma", "IPSignificance"}; // other configurations we will define manually
   std::map<string, unique_ptr<TrkSelTool> > selToolMap;
   std::map<string, unique_ptr<HistFamily> > histFamilyMap;
   for (const auto& cut : cutNames) {
     selToolMap[cut] = unique_ptr<TrkSelTool>(new TrkSelTool( (cut+"TrackSelection"), cut ));
     CHECK( selToolMap[cut]->initialize() );
     histFamilyMap[cut] = unique_ptr<HistFamily>(new HistFamily(cut));
   }
   selToolMap["IP"] = unique_ptr<TrkSelTool>(new TrkSelTool("IPTrackSelection"));
   CHECK( selToolMap["IP"]->setProperty( "maxD0", 1.5 ) );
   CHECK( selToolMap["IP"]->setProperty( "maxZ0SinTheta", 1.5 ) );
   CHECK( selToolMap["IP"]->initialize() );
   histFamilyMap["IP"] = unique_ptr<HistFamily>(new HistFamily("IP"));
   selToolMap["IPSigma"] = unique_ptr<TrkSelTool>(new TrkSelTool("IPSigmaTrackSelection"));
   CHECK( selToolMap["IPSigma"]->setProperty( "maxSigmaD0", 1.5 ) );
   CHECK( selToolMap["IPSigma"]->setProperty( "maxSigmaZ0SinTheta", 1.5 ) );
   CHECK( selToolMap["IPSigma"]->initialize() );
   histFamilyMap["IPSigma"] = unique_ptr<HistFamily>(new HistFamily("IPSigma"));
   selToolMap["IPSignificance"] = unique_ptr<TrkSelTool>(new TrkSelTool("IPSignificanceTrackSelection"));
   CHECK( selToolMap["IPSignificance"]->setProperty( "maxD0overSigmaD0", 3.0 ) );
   CHECK( selToolMap["IPSignificance"]->setProperty( "maxZ0SinThetaoverSigmaZ0SinTheta", 3.0 ) );
   CHECK( selToolMap["IPSignificance"]->initialize() );
   histFamilyMap["IPSignificance"] = unique_ptr<HistFamily>(new HistFamily("IPSignificance"));

   // Loop over the events:
   for( Long64_t entry = 0; entry < entries; ++entry ) {
     
     // Tell the object which entry to look at:
     CHECK( !event.getEntry( entry ) );
          
     // Get the InDetTrackParticles from the event:
     const xAOD::TrackParticleContainer* tracks = nullptr;
     CHECK( static_cast<StatusCode>(event.retrieve( tracks, "InDetTrackParticles" )) );
     
     const xAOD::VertexContainer* vertices = nullptr;
     CHECK( static_cast<StatusCode>(event.retrieve( vertices, "PrimaryVertices" )) );
     const auto it_pv = std::find_if(vertices->cbegin(), vertices->cend(),
				     [](const xAOD::Vertex* vtx)
				     {return vtx->vertexType() == xAOD::VxType::PriVtx;});
     const xAOD::Vertex* primaryVertex = (it_pv == vertices->cend()) ? nullptr : *it_pv;
     if (primaryVertex == nullptr) Warning( APP_NAME, "No primary vertex found." );

     for( const xAOD::TrackParticle* track : *tracks ) {
       if (track == nullptr) {
	 Error( APP_NAME, "Null pointer to track!" );
	 continue;
       }
       for (const auto& cut : cutNames) {
	 if (selToolMap[cut]->accept(*track, primaryVertex)) histFamilyMap[cut]->fill(*track, primaryVertex);
       }
       for (const auto& cut : otherCutNames) {
	 if (selToolMap[cut]->accept(*track, primaryVertex)) histFamilyMap[cut]->fill(*track, primaryVertex);	 
       }
       
     } // end loop over tracks
     
   } // end loop over events
   
   for (const auto& selTool : selToolMap) CHECK( selTool.second->finalize() );

   // draw histogram
   outFile->Write();

   // Return gracefully:
   return 0;
}


HistFamily::HistFamily(const string& cut_name)
{
  auto* currentFile = TFile::CurrentFile();
  currentFile->mkdir(cut_name.data())->cd(); // create a directory for this cut type and cd into it
#define HIST_INIT( NAME, AXIS_LABEL, AXIS_N, AXIS_XL, AXIS_XH )		\
  do{									\
    h_##NAME = new TH2F(#NAME, #NAME ";#eta;" AXIS_LABEL, 50,-2.5,2.5,	\
			AXIS_N, AXIS_XL, AXIS_XH);			\
  } while (false)

  HIST_INIT( pt, "p_{T}  [GeV]", 100, 0, 20 );
  HIST_INIT( si_hits_phys, "Si physical hits", 24, 0, 24 );
  HIST_INIT( si_hits, "Si hits", 24, 0, 24 );
  HIST_INIT( si_holes, "Si holes", 6, 0, 6 );
  HIST_INIT( si_shared, "Si shared hits", 6, 0, 6 );
  HIST_INIT( pix_hits_phys, "Pixel physical hits", 10, 0, 10 );
  HIST_INIT( pix_hits, "Pixel hits", 10, 0, 10 );
  HIST_INIT( pix_holes, "Pixel holes", 4, 0, 4 );
  HIST_INIT( pix_shared, "Pixel shared hits", 4, 0, 4 );
  HIST_INIT( sct_hits_phys, "SCT physical hits", 16, 0, 16 );
  HIST_INIT( sct_hits, "SCT hits", 16, 0, 16 );
  HIST_INIT( sct_holes, "SCT holes", 4, 0, 4 );
  HIST_INIT( sct_shared, "SCT shared hits", 4, 0, 4 );
  HIST_INIT( ibl_hits, "IBL hits", 4, 0, 4 );
  HIST_INIT( ibl_expected, "expect IBL hit", 2, 0, 2 );
  HIST_INIT( bl_hits, "BLayer hits", 4, 0, 4 );
  HIST_INIT( bl_expected, "expect BLayer hit", 2, 0, 2 );
  HIST_INIT( trt_hits, "TRT hits + outliers", 60, 0, 60 );
  HIST_INIT( trt_outlier_fraction, "TRT outlier fraction", 25, 0., 1. );
  
  h_d0 = new TH2F( "d0", "d_{0}^{BL};d_{0}^{BL}  [mm];#sigma_{d_{0}}  [mm]", 60, -3., 3., 60, 0., 3. );
  h_z0sintheta = new TH2F( "z0sintheta", "z_{0}^{PV} sin #theta;z_{0}^{PV} sin #theta  [mm];#sigma_{z_{0} sin #theta}  [mm]", 100, -5., 5., 50, 0., 5. );

#undef HIST_INIT

  currentFile->cd(); // move back to the parent directory
}

void HistFamily::fill(const xAOD::TrackParticle& trk, const xAOD::Vertex* vtx)
{
  uint8_t nPixHits = getSum(trk, xAOD::numberOfPixelHits);
  uint8_t nPixDead = getSum(trk, xAOD::numberOfPixelDeadSensors);
  uint8_t nPixShared = getSum(trk, xAOD::numberOfPixelSharedHits);
  uint8_t nPixHoles = getSum(trk, xAOD::numberOfPixelHoles);
  uint8_t nSctHits = getSum(trk, xAOD::numberOfSCTHits);
  uint8_t nSctDead = getSum(trk, xAOD::numberOfSCTDeadSensors);
  uint8_t nSctShared = getSum(trk, xAOD::numberOfSCTSharedHits);
  uint8_t nSctHoles = getSum(trk, xAOD::numberOfSCTHoles);
  uint8_t nIblHits = getSum(trk, xAOD::numberOfInnermostPixelLayerHits);
  uint8_t expectIbl = getSum(trk, xAOD::expectInnermostPixelLayerHit);
  uint8_t nBlHits = getSum(trk, xAOD::numberOfNextToInnermostPixelLayerHits);
  uint8_t expectBl = getSum(trk, xAOD::expectNextToInnermostPixelLayerHit);
  uint8_t nTrtHits = getSum(trk, xAOD::numberOfTRTHits);
  uint8_t nTrtOutliers = getSum(trk, xAOD::numberOfTRTOutliers);

  double eta = trk.eta();
  h_pt->Fill(eta, trk.pt()*1e-3);
  h_ibl_hits->Fill(eta, nIblHits);
  h_ibl_expected->Fill(eta, expectIbl);
  h_bl_hits->Fill(eta, nBlHits);
  h_bl_expected->Fill(eta, expectBl);
  h_pix_hits_phys->Fill(eta, nPixHits);
  h_pix_hits->Fill(eta, nPixHits + nPixDead);
  h_pix_holes->Fill(eta, nPixHoles);
  h_pix_shared->Fill(eta, nPixShared);
  h_sct_hits_phys->Fill(eta, nSctHits);
  h_sct_hits->Fill(eta, nSctHits + nSctDead);
  h_sct_holes->Fill(eta, nSctHoles);
  h_sct_shared->Fill(eta, nSctShared);
  h_si_hits_phys->Fill(eta, nPixHits + nSctHits);
  h_si_hits->Fill(eta, nPixHits + nPixDead + nSctHits + nSctDead);
  h_si_holes->Fill(eta, nPixHoles + nSctHoles);
  h_si_shared->Fill(eta, nPixShared + nSctShared);
  h_trt_hits->Fill(eta, nTrtHits + nTrtOutliers);
  h_trt_outlier_fraction->Fill(eta, nTrtOutliers / static_cast<double>(nTrtHits + nTrtOutliers));

  const auto& covMatrix = trk.definingParametersCovMatrix();
  h_d0->Fill(trk.d0(), std::sqrt(covMatrix(0,0)) );
  double z0 = trk.z0();
  if (vtx != nullptr) {
    z0 += trk.vz() - vtx->z();
  }
  double sinTheta = std::sin(trk.theta());
  double cosTheta = std::cos(trk.theta());
  double varZ0SinTheta = 0.;
  varZ0SinTheta += covMatrix(1,1)*sinTheta*sinTheta;
  varZ0SinTheta += 2*covMatrix(1,3)*z0*sinTheta*cosTheta;
  varZ0SinTheta += covMatrix(3,3)*z0*z0*cosTheta*cosTheta;
  h_z0sintheta->Fill(z0*sinTheta, std::sqrt(varZ0SinTheta));

  return;
}

uint8_t HistFamily::getSum(const xAOD::TrackParticle& trk, xAOD::SummaryType sum_type) const
{
  uint8_t sum_val = 0;
    if (!trk.summaryValue(sum_val, sum_type)) {
    Error( "HistFamily::getSum()", "Could not get summary type %i", sum_type );
  }
  return sum_val;
}
