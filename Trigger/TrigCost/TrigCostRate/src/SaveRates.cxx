/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// ROOT
#include "TFile.h"
#include "TTree.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostRate/SaveRates.h"

REGISTER_ANP_OBJECT(AlgEvent,SaveRates)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
Anp::SaveRates::SaveRates()
  :fDebug(false),
   fFile(0),
   fTreeConfig(0),
   fTreeEvent(0),
   fTreeOnline(0),
   fConfig(0),
   fEvent(0),
   fOnline(0)
{
}

//-----------------------------------------------------------------------------
Anp::SaveRates::~SaveRates()
{
}

//-----------------------------------------------------------------------------
void Anp::SaveRates::Config(const Registry &)
{
  //
  // Book tree branches before Init()
  //
  fConfg       = NtupleSvc::Instance("config").Branch<TrigMonConfig>("conf");
  fEntry       = NtupleSvc::Instance("event") .Branch<TrigMonEvent> ("data");
  fOnlineEntry = NtupleSvc::Instance("online").Branch<TrigMonEvent> ("online");
}

//-----------------------------------------------------------------------------
void Anp::SaveRates::Signal(const Registry &reg, const string &signal)
{
  if(signal != "NEW_FILE") return;

  if(fFile) { 
    fFile->Write();
    fFile->Close();
    fFile = 0; fTreeConfig = 0; fTreeEvent = 0;
  }

  string filename;
  if(!reg.Get("SIGNAL::FileName", filename) || filename.empty()) {
    return;
  }

  const string savefile = "SaveRates."+filename;
  fFile = new TFile(savefile.c_str(), "RECREATE");

  if(!fFile || !fFile->IsOpen()) {
    fFile = 0;
    return;
  }
  
  cout << "SaveRates::Signal - opened new file: " << savefile << endl;

  fTreeConfig = new TTree("config", "config");
  fTreeEvent  = new TTree("event",  "event" );
  fTreeOnline = new TTree("online", "online");

  fTreeConfig -> SetDirectory(fFile);
  fTreeEvent  -> SetDirectory(fFile);
  fTreeOnline -> SetDirectory(fFile);

  if(!fConfig) fConfig = new TrigMonConfig();
  if(!fEvent)  fEvent  = new TrigMonEvent();
  if(!fOnline) fOnline = new TrigMonEvent();

  fTreeConfig->Branch("conf", "TrigMonConfig", &fConfig);  
  fTreeEvent ->Branch("data", "TrigMonEvent",  &fEvent);
  fTreeOnline->Branch("online", "TrigMonEvent",  &fOnline);
}

//-----------------------------------------------------------------------------
bool Anp::SaveRates::Init()
{
  return true;
}

//-----------------------------------------------------------------------------
void Anp::SaveRates::Exec()
{
  //
  // Process one event from ntuple
  //
  if(fConfg.valid() && fConfg.GetState() == Ntuple::kREAD) { 

    if(fTreeConfig) {
      (*fConfig) = fConfg.GetRef();
      fTreeConfig -> Fill();
    }
  }

  //
  // Make new event entry
  //
  if(fEntry.valid() && fEntry.GetState() == Ntuple::kREAD) {

    if(fTreeEvent) {
      //(*fEvent) = fEntry.GetRef();

      fEvent->getVec<TrigMonROB>().clear();
      fEvent->getVec<TrigMonRoi>().clear();
      fEvent->getVec<TrigMonSeq>().clear();
      fEvent->getVec<TrigMonTE>().clear();

      //fEvent->getL1Item().clear();
      //fEvent->getChain().clear();
      //fEvent->getWord().clear();
      
      fEvent -> setEventID(fEntry.GetRef().getEvent(),
			   fEntry.GetRef().getLumi(),
			   fEntry.GetRef().getBunchId(),
			   fEntry.GetRef().getRun(),
			   fEntry.GetRef().getSec(),
			   fEntry.GetRef().getNanoSec());

      /*
      cout << "-------------------" << endl;
      const std::vector<uint32_t> &vars = fEntry.GetRef().getVarKey();
      for(unsigned i = 0; i < vars.size(); ++i) {
	cout << vars[i] << endl;
      }
      */

      fTreeEvent -> Fill();
    }
  }

  //
  // Make new online entry
  //
  if(fOnlineEntry.valid() && fOnlineEntry.GetState() == Ntuple::kREAD) {

    if(fTreeOnline) {
      //(*fOnline) = fOnline.GetRef();

      fOnline->getVec<TrigMonROB>().clear();
      fOnline->getVec<TrigMonRoi>().clear();
      fOnline->getVec<TrigMonSeq>().clear();
      fOnline->getVec<TrigMonTE>().clear();

      //fOnline->getL1Item().clear();
      //fOnline->getChain().clear();
      //fOnline->getWord().clear();
      
      fOnline -> setEventID(fOnlineEntry.GetRef().getEvent(),
			   fOnlineEntry.GetRef().getLumi(),
			   fOnlineEntry.GetRef().getBunchId(),
			   fOnlineEntry.GetRef().getRun(),
			   fOnlineEntry.GetRef().getSec(),
			   fOnlineEntry.GetRef().getNanoSec());

      /*
      cout << "-------------------" << endl;
      const std::vector<uint32_t> &vars = fOnline.GetRef().getVarKey();
      for(unsigned i = 0; i < vars.size(); ++i) {
	cout << vars[i] << endl;
      }
      */

      fTreeOnline -> Fill();
    }
  }

}

//-----------------------------------------------------------------------------
void Anp::SaveRates::Done()
{
  if(fFile) { 
    fFile->Write();
    fFile->Close();
  }
}
