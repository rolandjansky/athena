/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrackSummaryNtupleTool.cxx
//   Source file for class TrackSummaryNtupleTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Pavel.Ruzicka -at- cern.ch, Wolfgang.Liebig -at- cern.ch
///////////////////////////////////////////////////////////////////
#include "TTree.h"
// Trk
#include "TrackSummaryNtupleTool.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
//Truth
#include "TrkTruthData/TrackTruth.h"
#include "AtlasHepMC/GenParticle.h"


// constructor
Trk::TrackSummaryNtupleTool::TrackSummaryNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
         m_addParticleID(true),
         m_numberOfPixelHits{},            
         m_numberOfContribPixelLayers{},   
         m_numberOfPixelHoles{},           
         m_numberOfPixelDeadSensors{},     
         m_numberOfSCTHits{},              
         m_numberOfSCTHoles{},             
         m_numberOfSCTDeadSensors{},       
         m_numberOfTRTHits{},              
         m_numberOfTRTHoles{},             
         m_numberOfTRTDeadStraws{},        
         m_numberOfTRTHighThresholdHits{}, 

         m_dedx{},                         
         m_ngoodhitsdedx{},                
         m_massdedx{},                     
         m_likelihoodsdedx{},
         m_idHitPattern{}          

{
  declareInterface<ITrackValidationNtupleTool>(this);
  declareProperty("AddParticleID", m_addParticleID,      "Toggle if to also Write electron ID and dEdx data");
  m_likelihoodsdedx=new std::vector<float>();
}

// destructor
Trk::TrackSummaryNtupleTool::~TrackSummaryNtupleTool() {}


///////////////////////////////////////
// initialize
///////////////////////////////////////
StatusCode Trk::TrackSummaryNtupleTool::initialize() {

  ATH_MSG_DEBUG ("nothing specific initialized in " << name()); 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////
// finalize
///////////////////////////////////////
StatusCode Trk::TrackSummaryNtupleTool::finalize() {

   delete m_likelihoodsdedx;
   m_likelihoodsdedx=0;
  ATH_MSG_DEBUG ("finalized " << name());
  return StatusCode::SUCCESS;
}

StatusCode Trk::TrackSummaryNtupleTool::addNtupleItems( TTree* tree ) const {
    if (!tree) return StatusCode::FAILURE;
    //-----------------
    // add items  *** Note: Documentation is in the header file, doxygen and wikis! ***
    // 
    //track particle summary parameters
    tree->Branch("nPixelHits",            &m_numberOfPixelHits,            "numberOfPixelHits/b");
    tree->Branch("nContribPixelLayers",   &m_numberOfContribPixelLayers,   "numberOfContribPixelLayers/b");
    tree->Branch("nPixelHoles",           &m_numberOfPixelHoles,           "numberOfPixelHoles/b");
    tree->Branch("nPixelDeadSensors",     &m_numberOfPixelDeadSensors,     "numberOfPixelDeadSensors/b");
    tree->Branch("nSCTHits",              &m_numberOfSCTHits,              "numberOfSCTHits/b");
    tree->Branch("nSCTHoles",             &m_numberOfSCTHoles,             "numberOfSCTHoles/b");
    tree->Branch("nSCTDeadSensors"    ,   &m_numberOfSCTDeadSensors,       "numberOfSCTDeadSensors/b");
    tree->Branch("nTRTHits"  ,            &m_numberOfTRTHits,              "numberOfTRTHits/b");
    tree->Branch("nTRTHoles",             &m_numberOfTRTHoles,             "numberOfTRTHoles/b");
    tree->Branch("nTRTDeadStraws",        &m_numberOfTRTDeadStraws,        "numberOfTRTDeadStraws/b");
    tree->Branch("nTRTHighThresholdHits", &m_numberOfTRTHighThresholdHits, "numberOfTRTHighThresholdHits/b");

    tree->Branch("idHitPattern",          &m_idHitPattern,                 "idHitPattern/l");

    // additional entries for Pixel dEdx (and yet to come TRT HT) particle ID 
    if ( m_addParticleID ){
      tree->Branch("dEdx",                &m_dedx,                         "dEdx/F");
      tree->Branch("nGoodHitsdEdx",       &m_ngoodhitsdedx,                "nGoodHitsdEdx/I");
      tree->Branch("massdEdx",            &m_massdedx,                     "massdEdx/F");
      tree->Branch("likelihoodsdEdx",     &m_likelihoodsdedx);
    }
    
    ATH_MSG_VERBOSE ("added branches to ntuple");
    return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill track data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::TrackSummaryNtupleTool::fillTrackData (
    const Trk::Track& track,
    const int  /*iterationIndex*/,
    const unsigned int /*fitStatCode*/ ) const {
//    const Trk::FitterStatusCode /*fitStatCode*/ ) const {

  ATH_MSG_VERBOSE ("in fillTrackData(trk, indx)");
   
  const Trk::TrackSummary* summary = track.trackSummary();
  if((!summary) || fillTrackSummary(summary).isFailure())
    ATH_MSG_WARNING ("Summary could not be written to ntuple");

  // FIXME could think of giving this filler a TrackSummaryTool

  return StatusCode::SUCCESS;
}

//////////////////////////////////////
/// fill trackparticle data into variables without actually writing the record
//////////////////////////////////////
StatusCode Trk::TrackSummaryNtupleTool::fillTrackParticleData 
( const Trk::TrackParticleBase& particle) const
{

  const Trk::TrackSummary* summary = particle.trackSummary();
  if((!summary) || fillTrackSummary(summary).isFailure())
    ATH_MSG_WARNING ("Summary could not be written to ntuple");
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// fill ntuple data of a given proto-trajectory (function used for fitter validation)
//////////////////////////////////////
StatusCode Trk::TrackSummaryNtupleTool::fillProtoTrajectoryData 
(  const Trk::ProtoTrajectory&,
   const int,
   const Trk::Perigee*,
   const unsigned int ) const
   //const Trk::FitterStatusCode) const
{
  ATH_MSG_WARNING ("this method has no function");
  return StatusCode::SUCCESS;
}

//////////////////////////////////////
// reset variables
//////////////////////////////////////
void Trk::TrackSummaryNtupleTool::resetVariables() const {
  // reset the counters
   m_likelihoodsdedx->clear();
}

StatusCode Trk::TrackSummaryNtupleTool::fillTrackTruthData ( const TrackParameters*&,
                                                             const TrackTruth&, const int ) const
{
  ATH_MSG_VERBOSE ("Starting fillTrackTruthData() - has no function here");
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
/// fill a TrackSummary to the ntuple
  /////////////////////////////////////////////////////////////////////////////
StatusCode Trk::TrackSummaryNtupleTool::fillTrackSummary(const Trk::TrackSummary* summary) const {

  ATH_MSG_VERBOSE ("in fillTrackSummary");

  if (!summary) {
    ATH_MSG_WARNING ("Something is wrong - consistency check against NULL symmary bypassed?");
    return StatusCode::FAILURE;
  }
  m_numberOfPixelHits            = (unsigned char)summary->get(Trk::numberOfPixelHits);
  m_numberOfContribPixelLayers   = (unsigned char)summary->get(Trk::numberOfContribPixelLayers);
  m_numberOfPixelHoles           = (unsigned char)summary->get(Trk::numberOfPixelHoles);
  m_numberOfPixelDeadSensors     = (unsigned char)summary->get(Trk::numberOfPixelDeadSensors);
  m_numberOfSCTHits              = (unsigned char)summary->get(Trk::numberOfSCTHits);
  m_numberOfSCTHoles             = (unsigned char)summary->get(Trk::numberOfSCTHoles);
  m_numberOfSCTDeadSensors       = (unsigned char)summary->get(Trk::numberOfSCTDeadSensors);
  m_numberOfTRTHits              = (unsigned char)summary->get(Trk::numberOfTRTHits);
  m_numberOfTRTHoles             = (unsigned char)summary->get(Trk::numberOfTRTHoles);
  m_numberOfTRTDeadStraws        = (unsigned char)summary->get(Trk::numberOfTRTDeadStraws);
  m_numberOfTRTHighThresholdHits = (unsigned char)summary->get(Trk::numberOfTRTHighThresholdHits);

  std::bitset<Trk::numberOfDetectorTypes> hitPattern; // all position are set to 0
  for (int i=0; i<Trk::numberOfDetectorTypes; ++i) {
    if (summary->isHit(static_cast<Trk::DetectorType>(i))) hitPattern.set(i,1); // set position i to 1
  }
  m_idHitPattern = hitPattern.to_ulong();
  
  if (m_addParticleID) {
     m_dedx=summary->getPixeldEdx();
     m_ngoodhitsdedx=summary->numberOfUsedHitsdEdx();
     m_massdedx=-1;
     if (summary->indetTrackSummary()){
       m_massdedx=summary->indetTrackSummary()->massPixeldEdx();
       std::vector<float> likelihoods=summary->indetTrackSummary()->likelihoodsPixeldEdx();
       m_likelihoodsdedx->assign(likelihoods.begin(),likelihoods.end());
     }   
  }
  
  return StatusCode::SUCCESS;
}
