/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// ROOT
#include "TMath.h"

// Local
#include "TrigCostBase/EventSvc.h"
//#include "TrigCostBase/Factory.h"

// Local
#include "TrigCostRate/ScalePileup.h"

REGISTER_ANP_OBJECT(AlgEvent,ScalePileup)

using namespace std;

//-----------------------------------------------------------------------------
Anp::ScalePileup::ScalePileup()
  :fDebug(false),
   fUseTruth(false),
   fIsValid(true),
   fPredPileup(0.0),
   fScale(0.0),
   fMinProbCut(-1.0),
   fVtxEffA(0.601),
   fVtxEffB(0.0),
   fPowerOffset(1.0),
   fWeightNtupleName(""),
   fFileWeights(0),
   fTreeWeights(0),
   fRunNumber(-1),
   fEventNumber(-1),
   fLumiblock(-1),
   fNV(-1),
   fMuIn(-1),
   fTotalScale(0),
   fNullDenomMsg(false),
   fNNullDenom(0)
{
}

//-----------------------------------------------------------------------------
Anp::ScalePileup::~ScalePileup()
{
	//if(fTreeWeights) delete fTreeWeights ;
	if(fFileWeights) delete fFileWeights ;
}

//-----------------------------------------------------------------------------
void Anp::ScalePileup::Config(const Registry &reg)
{
  //
  // Configure self and register input data
  //
  reg.Get("ScalePileup", "Debug"   , fDebug   );
  reg.Get("ScalePileup", "useTruth", fUseTruth);

  //
  // Read configuration - can override values set in XML
  //
  reg.Get("ScalePileup::PredictPileup"    , fPredPileup       );
  reg.Get("ScalePileup::WeightNtupleName" , fWeightNtupleName );
  reg.Get("ScalePileup::MinProbCut"       , fMinProbCut       );
  reg.Get("ScalePileup::VtxEffA"          , fVtxEffA          );
  reg.Get("ScalePileup::VtxEffB"          , fVtxEffB          );
  reg.Get("ScalePileup::PowerOffset"      , fPowerOffset      );
  
  //
  // Add self to global list - used by MakeRates
  //
  Factory<ScaleABC>::Instance().Hold(AlgEvent::GetName(), Handle<ScaleABC>(this));

  //
  // Register input data
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  //
  // Declare output tree for saving weight information
  //
  if(fWeightNtupleName!=""){
    fFileWeights = new TFile(fWeightNtupleName.c_str(), "RECREATE") ;
    fFileWeights->cd() ;
    fTreeWeights = new TTree("ScalePileupWeight", "ScalePileup weight information") ;
    fTreeWeights->Branch("RunNumber"   , &fRunNumber     , "RunNumber/I"  ) ;
    fTreeWeights->Branch("EventNumber" , &fEventNumber   , "EventNumber/I") ;
    fTreeWeights->Branch("Lumiblock"   , &fLumiblock     , "Lumiblock/I"  ) ;
    fTreeWeights->Branch("muIn"        , &fMuIn          , "muIn/F"       ) ;
    fTreeWeights->Branch("NV"          , &fNV            , "NV/I"         ) ;
    fTreeWeights->Branch("w"           , &fTotalScale    , "w/F"          ) ;
  }

  if(fDebug) {
    reg.Print() ;
  }

  log() << "PredictPileup    : " << fPredPileup       << endl ;
  log() << "WeightNtupleName : " << fWeightNtupleName << endl ;
  log() << "MinProbCut       : " << fMinProbCut       << endl ;
  log() << "VtxEffA          : " << fVtxEffA          << endl ;
  log() << "VtxEffB          : " << fVtxEffB          << endl ;
}

//-----------------------------------------------------------------------------
bool Anp::ScalePileup::Init()
{
  //
  // Check that pileup values are sensible
  //
  log() << "Init " << endl
	<< "   pred. pileup: " << fPredPileup  << endl;

  if(fPredPileup < 1.0) {
    log() << "Run - invalid pile-up parameters" << endl
	  << "   Both parameters must be greater or equal to 1" << endl
	  << "   These are the number of collisions per event, not the mean number of interactions per crossing" << endl
	  << "   Set event weight to 1" << endl;

    fScale   = 1.0;
    fIsValid = false;

    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::ScalePileup::Exec()
{
  //
  // Process event: compute weight - used by clients
  //
  if(!fIsValid) { 
    return; 
  }

  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    //
    // Compute scale correction
    //
      if(fUseTruth) fScale = ComputeCorrFromMC(fEntry.GetRef());
      else          fScale = ComputeCorrFromVx(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::ScalePileup::Done()
{
  fFileWeights->Write() ;
  fFileWeights->Close() ;
  log() << "Number of null denominator events: " << fNNullDenom << endl;
}

//-----------------------------------------------------------------------------
long double Anp::ScalePileup::ComputeCorrFromMC(const TrigMonEvent &event) const
{
  //
  // Compute scale correcrion using MC pileup truth
  //

  //
  // Get pileup info for event
  //
  float inputPileup ;
  const int nsignal_intime_varidx = 216; // magic number from TrigNtVarsTool.cxx = "signal" collisions in crossing
  const int npileup_intime_varidx = 217; // magic number from TrigNtVarsTool.cxx = "pileup" collisions in crossing
  const int pileup_varidx  = 260; // magic number from ReadTiny  
  event.getVar(pileup_varidx , inputPileup);

  float nsignal_intime = 0.0;
  float npileup_intime = 0.0;
  
  if (!event.getVar(nsignal_intime_varidx, nsignal_intime) || 
      !event.getVar(npileup_intime_varidx, npileup_intime)) {
    log() << "Error getting truth collsions counts from TrigMonEvent" << endl;
    return 0.0;
  }
  
  //
  // total number of collision in crossing
  //
  const int ncoll = rint(nsignal_intime) + rint(npileup_intime); 
  
  //
  // Calculate scale factor 
  //
  if(!(ncoll < 1.0)) { 
    return TMath::Poisson(ncoll-1,fPredPileup-1)/TMath::Poisson(ncoll-1,inputPileup-1);
  }
  
  log() << "Shouldn't get an event with out a collision" << endl;
  
  return 1.0 ;
}

//-----------------------------------------------------------------------------
long double Anp::ScalePileup::ComputeCorrFromVx(const TrigMonEvent &event)
{
  //
  // Compute scale correcrion using reco number of vertexes
  //
  float nvertex          = 0.0;   
  float inputPileup      = 0.0; 
  float outputPileup     = fPredPileup ;
  float runNumberFloat   = -1 ;
  float eventNumberFloat = -1 ;
  float lumiblockFloat   = -1 ;
  
  //
  // Magic numbers from ReadTiny
  //
  const int nvertex_varidx      = 251;
  const int pileup_varidx       = 260;
  const int runNumber_varidx    = 261;
  const int eventNumber_varidx  = 262;
  const int lumiblock_varidx    = 263;
  
  event.getVar( nvertex_varidx     , nvertex          );  
  event.getVar( pileup_varidx      , inputPileup      );
  event.getVar( runNumber_varidx   , runNumberFloat   );
  event.getVar( eventNumber_varidx , eventNumberFloat );
  event.getVar( lumiblock_varidx   , lumiblockFloat   );
  
  //
  // Apply NV weighting
  //
  double totalScaleFactor = 1.0 ;
  int    NV   = nvertex ;  
  double a    = fVtxEffA ;
  double b    = fVtxEffB ;
  double mu1  = outputPileup ;
  double mu0  =  inputPileup ;
  
  float eff1 = a+b*mu1 ;
  float eff0 = a+b*mu0 ;
  //totalScaleFactor = exp((mu0-mu1)*(a+b*(mu0+mu1)))*pow(mu1/mu0,NV-fPowerOffset) ;
  totalScaleFactor = (mu0/mu1)*pow((eff1*mu1)/(eff0*mu0),NV)*exp(eff0*mu0-eff1*mu1) ;
  if(NV==0) totalScaleFactor = 0 ;
  
  //if(TMath::Poisson(NV,mu0)<a*fMinProbCut) totalScaleFactor = 0 ;
  
  //
  // Save values to TTree
  //
  fRunNumber   = runNumberFloat   ;
  fEventNumber = eventNumberFloat ;
  fLumiblock   = lumiblockFloat   ;
  fMuIn        = inputPileup      ;
  fNV          = NV               ;
  fTotalScale  = totalScaleFactor ;
  
  fTreeWeights->Fill() ;
  
  if(fDebug){
    log() << "NV = " << NV << " , mu = " << inputPileup      << endl;
    log() << "overall scale factor  = "  << totalScaleFactor << endl ;
    log() << "Run, event = " << fRunNumber << " " << fEventNumber << endl ;
  }

  return totalScaleFactor ;
}
