/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/TSelectionSet.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "MissingETGoodness/Goodies.h"
#include <algorithm>


ClassImp(MET::TSelectionSet)


MET::TSelectionSet::TSelectionSet( const Quality& quality, const Int_t& version )
  : std::vector<TSelection>()
  , TObject()
  , m_quality(quality)
  , m_version(version)
  , m_eventsrunnr(-1)
  , m_logger( "TSelectionSet" )
  , m_uptodate(kFALSE)
{
}


MET::TSelectionSet::~TSelectionSet()
{
}


MET::TSelectionSet::TSelectionSet(const MET::TSelectionSet& other)
  : std::vector<TSelection>(other)
  , TObject(other)
  , m_quality(other.m_quality)
  , m_version(other.m_version)
  , m_eventsrunnr(other.m_eventsrunnr)
  , m_uptodate(other.m_uptodate)
{
}


MET::TSelectionSet&
MET::TSelectionSet::operator=(const MET::TSelectionSet& other)
{
  if (&other==this) {
    return *this ;
  } 
  std::vector<TSelection>::operator=(other);
  m_quality = other.m_quality;
  m_version = other.m_version;
  m_eventsrunnr = other.m_eventsrunnr;
  m_uptodate = other.m_uptodate;

  return *this ;
}


Bool_t 
MET::TSelectionSet::ContainsRun( const Int_t& runnr )  const
{
  std::vector<TSelection>::const_iterator itr = this->begin();
  std::vector<TSelection>::const_iterator end = this->end();

  Bool_t pass(kFALSE);
  for (; itr!=end; ++itr) {
    pass = pass || itr->ContainsRun(runnr) ;
    if (pass) break;
  }

  return pass;
}


Bool_t 
MET::TSelectionSet::ContainsAnyRun() const
{
  std::vector<TSelection>::const_iterator itr = this->begin();
  std::vector<TSelection>::const_iterator end = this->end();

  Bool_t canyrun(kFALSE);
  for (; itr!=end; ++itr) {
    canyrun = itr->ContainsAnyRun() ;
    if (canyrun) return kTRUE;
  }

  std::vector<TRunRange> runrange = this->GetCompressedRunRange();
  if (runrange.size()==1) { canyrun = runrange.begin()->ContainsAnyRun(); }

  return canyrun;
}


Bool_t 
MET::TSelectionSet::ContainsThisRun()
{
  return this->ContainsRun( this->GetEventsRunNumber() );
}


std::vector< MET::TSelection >::iterator
MET::TSelectionSet::Find( const Int_t& runnr )
{
  std::vector<TSelection>::iterator itr = this->begin();

  Bool_t pass(kFALSE);
  for (; itr!=this->end(); ++itr) {
    pass = itr->ContainsRun(runnr) ;
    if (pass) break;
  }

  return itr;
}


std::vector< MET::TSelection >::const_iterator
MET::TSelectionSet::Find( const Int_t& runnr ) const
{
  std::vector<TSelection>::const_iterator itr = this->begin();

  Bool_t pass(kFALSE);
  for (; itr!=this->end(); ++itr) {
    pass = itr->ContainsRun(runnr) ;
    if (pass) break;
  }

  return itr;
}


void
MET::TSelectionSet::AddSelection ( const TSelection& selection )
{
  this->push_back(selection);
  this->Sort();
}


void 
MET::TSelectionSet::SetMETKey( const TString& metKey )
{
  std::vector<TSelection>::iterator itr = this->begin();
  for (; itr!=this->end(); ++itr) { itr->SetMETKey(metKey); }
}


void 
MET::TSelectionSet::SetJetKey( const TString& jetKey )
{
  std::vector<TSelection>::iterator itr = this->begin();
  for (; itr!=this->end(); ++itr) { itr->SetJetKey(jetKey); }
}


void
MET::TSelectionSet::SetUptodate( const Bool_t& uptodate )
{
  m_uptodate = uptodate;
  std::vector<TSelection>::iterator itr = this->begin();
  for (; itr!=this->end(); ++itr) { itr->SetUptodate(uptodate); }
}


Bool_t
MET::TSelectionSet::Evaluate( const std::vector<TString>& parList )
{
  Bool_t pass = kFALSE;

  // update runnumber and inputs?
  Update();

  // evaluate corresponding selections
  std::vector<TSelection>::iterator itr = this->Find(m_eventsrunnr);
  if (itr!=this->end()) pass = itr->Evaluate(parList);

  return pass;
}


const char* 
MET::TSelectionSet::GetMETKey() const
{
  const char* key=0;
  std::vector<TSelection>::const_iterator itr = this->Find(m_eventsrunnr);
  if (itr!=this->end()) key = itr->GetMETKey();
  else {
    m_logger << kWARNING << "Current run number <" << m_eventsrunnr << "> not defined for this selection set." << GEndl;
  }
  return key;
}


const char* 
MET::TSelectionSet::GetJetKey() const
{
  const char* key=0;
  std::vector<TSelection>::const_iterator itr = this->Find(m_eventsrunnr);
  if (itr!=this->end()) key = itr->GetJetKey();
  else {
    m_logger << kWARNING << "Current run number <" << m_eventsrunnr << "> not defined for this selection set." << GEndl;
  }
  return key;
}


Long64_t
MET::TSelectionSet::GetEventsBitMask()
{
  Long64_t eventsbitmask=0;

  // update evaluations?
  Update();

  // get bitmask corresponding to selections
  std::vector<TSelection>::iterator itr = this->Find(m_eventsrunnr);
  if (itr!=this->end()) eventsbitmask = itr->GetEventsBitMask();  
  else {
    m_logger << kWARNING << "Current run number <" << m_eventsrunnr << "> not defined in selection set." << GEndl;
  }

  return eventsbitmask;
}


Int_t 
MET::TSelectionSet::GetEventsRunNumber()
{
  // update runnumber (and corresponding selections)?
  UpdateRunNumber();
  return m_eventsrunnr;
}


void 
MET::TSelectionSet::Update()
{
  if (!m_uptodate) {
    // update run number
    this->UpdateRunNumber();
    // update corresponding selections
    std::vector<TSelection>::iterator itr = this->Find(m_eventsrunnr);
    if (itr!=this->end()) itr->Update();
    // set uptodate flag
    m_uptodate = kTRUE;
  }
}


void 
MET::TSelectionSet::UpdateRunNumber()
{
  if (!m_uptodate) {
    // reference to MET Goodies map for storing derived quantities
    MET::Goodies& goodies = MET::Goodies::instance();
    // get event number of this event;
    Int_t runNumber = goodies.getIntValue("RunNumber");
    if (runNumber<0) runNumber=goodies.runNumber();
    this->SetEventsRunNumber( runNumber );
  }
}


void
MET::TSelectionSet::Summary () const
{
  m_logger << kINFO << "Quality  : " << m_quality << GEndl;
  m_logger << kINFO << "Version  : " << m_version << GEndl;
  m_logger << kINFO << "Uptodate : " << m_uptodate << GEndl;

  std::vector<TSelection>::const_iterator itr = this->begin();
  for (; itr!=this->end(); ++itr) { itr->Summary(); }
}


void 
MET::TSelectionSet::PrintEvent()
{
  // update runnumber and inputs?
  (void) this->Evaluate(); 

  // print corresponding selections
  std::vector<TSelection>::iterator itr = this->Find(m_eventsrunnr);
  if (itr!=this->end()) itr->PrintEvent();
  else {
    m_logger << kINFO << "Event's run number <" << m_eventsrunnr << "> not defined in selection set." << GEndl;
  }

}


void
MET::TSelectionSet::Sort ()
{
  std::sort(this->begin(),this->end(),SorterL2H());
}


std::vector<MET::TRunRange>
MET::TSelectionSet::GetCompressedRunRange() const
{
  // compressed runrange
  std::vector<MET::TRunRange> rrcompressed;

  std::vector<MET::TSelection>::const_iterator itr, cbegin, cend;
  int fbegin(-1), fend(-1), tbegin(-1), tend(-1), lbegin(-1), lend(-1);

  for (itr=this->begin(); itr!=this->end();) {
    cbegin = itr;
    cend = itr;

    // first lumiblock of merge
    fbegin = cbegin->Begin();
    fend = cbegin->End();

    // search all connecting lumi ranges
    bool go_on(true);
    for (++itr; itr!=this->end() && go_on; ++itr) {
      tbegin = cend->Begin();
      tend   = cend->End();
      lbegin = itr->Begin();
      lend   = itr->End();
      if ( lbegin>=tbegin && lend>=tend ) { // should always be true
        if (tend+1<lbegin) { go_on=false; }
        else { cend = itr; }
      } 
    }

    // reset iterator
    itr=cend; ++itr;

    // need to merge run ranges
    if (cbegin!=cend) {
      // first lumiblock of merge
      fbegin = cbegin->Begin();
      fend = cbegin->End();
      // last lumiblock of merge
      lbegin = cend->Begin();
      lend = cend->End();
      // insert merge lbrange before itr
      rrcompressed.push_back( MET::TRunRange(fbegin,lend) );
    // no need to merge run ranges
    } else {
      rrcompressed.push_back( MET::TRunRange(fbegin,fend) );
    }
  }

  return rrcompressed;
}


void 
MET::TSelectionSet::ParseBitMask(const Int_t& runnr, const Long64_t bitmask) const
{
  std::vector<MET::TSelection>::const_iterator itr = this->Find(runnr);
  if (itr!=this->end()) itr->ParseBitMask(bitmask) ;
  else {
    m_logger << kWARNING << "ParseBitMask() : Cannot associate run number " << runnr << " with set of cuts." << GEndl;
  }
}

