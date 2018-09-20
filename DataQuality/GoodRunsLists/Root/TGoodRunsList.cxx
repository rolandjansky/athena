/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TMsgLogger.h"


ClassImp(Root::TGoodRunsList)


Root::TGoodRunsList::TGoodRunsList()
  : std::map<Int_t,TGoodRun>()
  , TNamed("noname","notitle")
  , m_checkGRLInfo(kFALSE)
  , m_hasRun(kFALSE)
  , m_hasLB(kFALSE)
  , m_prevRun(-10)
  , m_prevLB(-10)
{
}


Root::TGoodRunsList::TGoodRunsList( const char* name )
  : std::map<Int_t,TGoodRun>()
  , TNamed(name,"notitle")
  , m_checkGRLInfo(kFALSE)
  , m_hasRun(kFALSE)
  , m_hasLB(kFALSE)
  , m_prevRun(-10)
  , m_prevLB(-10)
{
}


Root::TGoodRunsList::~TGoodRunsList()
{
}


Root::TGoodRunsList::TGoodRunsList(const Root::TGoodRunsList& other)
  : std::map<Int_t,TGoodRun>(other)
  , TNamed(other)
  , m_version(other.m_version)
  , m_metadata(other.m_metadata)
  , m_checkGRLInfo(other.m_checkGRLInfo)
  , m_hasRun(other.m_hasRun)
  , m_hasLB(other.m_hasLB)
  , m_prevRun(other.m_prevRun)
  , m_prevLB(other.m_prevLB)
{
}


Root::TGoodRunsList&
Root::TGoodRunsList::operator=(const Root::TGoodRunsList& other)
{
  if (&other==this) {
    return *this ;
  } 
  std::map<Int_t,TGoodRun>::operator=(other);
  TNamed::operator=(other);
  m_version = other.m_version;
  m_metadata = other.m_metadata;
  m_checkGRLInfo = other.m_checkGRLInfo;
  m_hasRun = other.m_hasRun;
  m_hasLB = other.m_hasLB;
  m_prevRun = other.m_prevRun;
  m_prevLB = other.m_prevLB;
  return *this ;
}


void 
Root::TGoodRunsList::AddGRL(const TGoodRunsList& other)
{
  // ensure version and name are identical
  Bool_t same = this->HasSameGRLInfo(other);
  if (m_checkGRLInfo || other.GetCheckGRLInfo()) {
    if (!same) {
      TMsgLogger m_logger("TGoodRunsList");
      m_logger << kWARNING << "Incompatible NamedLumiRanges: " << GEndl;
      this->Summary(kFALSE);
      other.Summary(kFALSE);
      m_logger << kWARNING << "Not adding GoodRunsList." << GEndl;
      return;
    }
  }

  // merge metadata
  for (const auto& othermditem : other.GetMetaData()) {
    auto thismditem = m_metadata.find(othermditem.first);
    if (thismditem == m_metadata.end()) {
      m_metadata.insert(othermditem);
    } else {
      auto& nameStr = othermditem.first;
      auto& thisvaluestr = thismditem->second;
      if (thisvaluestr != othermditem.second) {
	if (nameStr == "RunList") {
	  thisvaluestr += ",";
	} else {
	  thisvaluestr += " | ";
	}
	thisvaluestr += othermditem.second;
      }
    }
  }

  const TGoodRunsList& diffgrl = this->GetPartOnlyIn(other);

  std::map<Int_t,Root::TGoodRun>::const_iterator itr;
  // Store diffgrl goodrunslist
  for (itr = diffgrl.begin(); itr!=diffgrl.end(); ++itr) {
    if (this->find(itr->first)!=this->end()) { // overlapping runnumbers
      Root::TGoodRun grun = itr->second.GetSumWith( this->find(itr->first)->second ) ;
      if (!grun.IsEmpty()) (*this)[itr->first] = grun;
    } else if (!itr->second.IsEmpty()) { // store rest directly
      (*this)[itr->first] = itr->second;
    }
  }

  return;
}

const Root::TGoodRunsList 
Root::TGoodRunsList::GetOverlapWith(const TGoodRunsList& other) const
{
  Root::TGoodRunsList overlapgrl;
  overlapgrl.SetName("Overlap of GRLs");

  // ensure version and metadata are identical
  Bool_t same = this->HasSameGRLInfo(other);
  if (m_checkGRLInfo || other.GetCheckGRLInfo()) { 
    if (!same) {
      TMsgLogger m_logger("TGoodRunsList");
      m_logger << kWARNING << "Incompatible NamedLumiRanges: " << GEndl; 
      this->Summary(kFALSE);
      other.Summary(kFALSE);
      m_logger << kWARNING << "Returning empty GoodRunsList." << GEndl;
      return overlapgrl;
    }
  }
  if (same) {
    overlapgrl.SetName(this->GetName());
    overlapgrl.SetVersion(this->GetVersion());
    overlapgrl.SetMetaData(this->GetMetaData());
  }

  // Store goodruns
  std::map<Int_t,Root::TGoodRun>::const_iterator itr = this->begin();
  for (; itr!=this->end(); ++itr) {
    if (other.find(itr->first)!=other.end()) { // check runnumbers
      Root::TGoodRun grun = itr->second.GetOverlapWith( other.find(itr->first)->second ) ;
      if (!grun.IsEmpty()) {
        grun.Sort(); // sort lumiblock ranges before storage
        overlapgrl[itr->first] = grun;
      }
    }
  }
  
  return overlapgrl;
}


const Root::TGoodRunsList
Root::TGoodRunsList::GetSumWith(const TGoodRunsList& other) const
{
  Root::TGoodRunsList sumgrl;
  sumgrl.SetName("Sum of GRLs"/*this->GetName()*/);

  // ensure version and metadata are identical
  Bool_t same = this->HasSameGRLInfo(other);
  if (m_checkGRLInfo || other.GetCheckGRLInfo()) { 
    if (!same) { 
      TMsgLogger m_logger("TGoodRunsList");
      m_logger << kWARNING << "Incompatible NamedLumiRanges: " << GEndl; 
      this->Summary(kFALSE);
      other.Summary(kFALSE);
      m_logger << kWARNING << "Returning empty GoodRunsList." << GEndl;
      return sumgrl;
    }
  }
  if (same) {
    sumgrl.SetName(this->GetName());
    sumgrl.SetVersion(this->GetVersion());
    sumgrl.SetMetaData(this->GetMetaData());
  }

  std::map<Int_t,Root::TGoodRun>::const_iterator itr;
  // Store this goodruns
  for (itr = this->begin(); itr!=this->end(); ++itr) {
    if (other.find(itr->first)!=other.end()) { // check runnumbers
      Root::TGoodRun grun = itr->second.GetSumWith( other.find(itr->first)->second ) ;
      if (!grun.IsEmpty()) sumgrl[itr->first] = grun;
    } else if (!itr->second.IsEmpty()) { // store difference
      sumgrl[itr->first] = itr->second;
    }
  }
  // Store remaining other goodruns
  for (itr = other.begin(); itr!=other.end(); ++itr) {
    if (sumgrl.find(itr->first)==sumgrl.end() && !itr->second.IsEmpty()) { // check for remaining runnumbers
      sumgrl[itr->first] = itr->second;
    }
  }

  return sumgrl;
}


const Root::TGoodRunsList
Root::TGoodRunsList::GetPartOnlyIn(const Root::TGoodRunsList& other) const
{
  return other.GetPartNotIn(*this);
}


const Root::TGoodRunsList
Root::TGoodRunsList::GetPartNotIn(const Root::TGoodRunsList& other) const
{
  Root::TGoodRunsList notinother;
  notinother.SetName("Difference of GRLs");  

  // ensure version and metadata are identical
  Bool_t same = this->HasSameGRLInfo(other);
  if (m_checkGRLInfo || other.GetCheckGRLInfo()) { 
    if (!same) {
      TMsgLogger m_logger("TGoodRunsList");
      m_logger << kWARNING << "Incompatible NamedLumiRanges: " << GEndl;
      this->Summary(kFALSE);
      other.Summary(kFALSE);
      m_logger << kWARNING << "Returning empty GoodRunsList." << GEndl;
      return notinother;
    }
  }
  if (same) {
    notinother.SetName(this->GetName());
    notinother.SetVersion(this->GetVersion());
    notinother.SetMetaData(this->GetMetaData());
  }

  std::map<Int_t,Root::TGoodRun>::const_iterator itr;
  // How to store this goodrun ?
  for (itr = this->begin(); itr!=this->end(); ++itr) {
    if (other.find(itr->first)!=other.end()) { // check runnumbers
      Root::TGoodRun notinrun = itr->second.GetPartNotIn( other.find(itr->first)->second ) ;
      if (!notinrun.IsEmpty()) notinother[itr->first] = notinrun;
    } else if (!itr->second.IsEmpty()) { // store difference
      notinother[itr->first] = itr->second;
    }
  }

  return notinother;
}


Bool_t 
Root::TGoodRunsList::HasRun( const Int_t& runnr )  const
{
  // run searched same as previous run?
  if ( runnr==m_prevRun )
  {
    return m_hasRun;
  }

  // rationale: I need to reset the previous lumiblock here in order
  //   to ensure that the first lumiblock on each run gets tested.
  m_prevRun=runnr;
  m_prevLB = -10;
  m_hasRun = ( this->find(runnr)!=this->end() );
  return m_hasRun;
}


Bool_t
Root::TGoodRunsList::HasRunLumiBlock( const Int_t& runnr, const Int_t& lumiblocknr ) const
{
  Bool_t pass = kFALSE;

  if (this->HasRun(runnr)) {
    // rationale: this uses the cached result of the last lumi-block
    //   query.  if we switched runs, m_prevLB will have been set to
    //   an invalid value in HasRun to force a re-read.
    if (lumiblocknr==m_prevLB)
    {
      return m_hasLB;
    }
    m_prevLB = lumiblocknr;
    m_hasLB  = (this->find(runnr))->second.HasLB( lumiblocknr );
    pass = m_hasLB;
  }

  return pass;
}


Bool_t 
Root::TGoodRunsList::HasTriggerInfo() const
{
  std::map<TString,TString>::const_iterator itr = m_metadata.begin(); 
  for (; itr!=m_metadata.end(); ++itr) {
    TString triggername = itr->first;
    triggername.ToLower();
    if (triggername.BeginsWith("trigger") && itr->second.Length()>0) return kTRUE;
  }
  return kFALSE;
}


Bool_t 
Root::TGoodRunsList::HasOverlapWith(const TGoodRunsList& other, bool verb) const 
{ 
  const Root::TGoodRunsList overlapgrl = this->GetOverlapWith(other);
  bool isEmpty = overlapgrl.IsEmpty();
  if (!isEmpty && verb) overlapgrl.Summary(true);

  return !isEmpty; 
}


void
Root::TGoodRunsList::Summary(Bool_t verbose /*= kFALSE*/) const 
{
  // TNamed print
  Print();

  TMsgLogger m_logger("TGoodRunsList");
  // Versioning and metadata info
  m_logger << kINFO << "Version: " << m_version << GEndl;
  std::map<TString,TString>::const_iterator titr = m_metadata.begin();
  for (titr = m_metadata.begin(); titr!=m_metadata.end(); ++titr) 
    m_logger << kINFO << "Metadata: " << (titr->first) << " : " << (titr->second) << GEndl;

  m_logger << kINFO << "Number of runs: " << this->size() << GEndl;

  // Info about runs
  if (verbose) {
    std::map<Int_t,Root::TGoodRun>::const_iterator itr = this->begin();
    std::map<Int_t,Root::TGoodRun>::const_iterator end = this->end();
    for (; itr!=end; ++itr) itr->second.Summary();
  }
}


const std::vector<int>
Root::TGoodRunsList::GetRunlist() const
{
  std::vector<int> runlist;

  std::map<Int_t,Root::TGoodRun>::const_iterator itr = this->begin();
  std::map<Int_t,Root::TGoodRun>::const_iterator end = this->end();
  for (; itr!=end; ++itr) runlist.push_back(itr->first);

  return runlist;
}


const std::vector<Root::TGoodRun>
Root::TGoodRunsList::GetGoodRuns() const
{
  std::vector<Root::TGoodRun> runlist;

  std::map<Int_t,Root::TGoodRun>::const_iterator itr = this->begin();
  std::map<Int_t,Root::TGoodRun>::const_iterator end = this->end();
  for (; itr!=end; ++itr) runlist.push_back(itr->second);

  return runlist;
}


const std::vector<std::string>
Root::TGoodRunsList::GetTriggerList() const
{
  std::vector<std::string> triggerchains;

  std::map<TString,TString>::const_iterator itr = m_metadata.begin();
  for (; itr!=m_metadata.end(); ++itr) {
    TString triggername = itr->first;
    triggername.ToLower();
    if (triggername.BeginsWith("trigger") && itr->second.Length()>0) triggerchains.push_back(itr->second.Data());
  }
  return triggerchains;
}


const std::vector<std::string>
Root::TGoodRunsList::GetStreamList() const
{
  std::vector<std::string> streamlist;
  
  std::map<TString,TString>::const_iterator itr = m_metadata.begin();
  for (; itr!=m_metadata.end(); ++itr) {
    TString streamname = itr->first;
    streamname.ToLower(); 
    if (streamname.BeginsWith("stream") && itr->second.Length()>0) streamlist.push_back(itr->second.Data());
  }   
  return streamlist;
}     
    
  
Bool_t
Root::TGoodRunsList::HasSameGRLInfo(const TGoodRunsList& other) const
{
  TMsgLogger m_logger("TGoodRunsList");
  Bool_t same(kTRUE);

  /// check names
  same = same && (TString(this->GetName())==TString(other.GetName())) ;
  if (!same) {
    m_logger << kDEBUG << "Incompatible names: <" << this->GetName() << "> <" << other.GetName() << ">" << GEndl;
    return kFALSE;
  }
  /// check versions
  same = same && (this->GetVersion()==other.GetVersion()) ;
  if (!same) { 
    m_logger << kDEBUG << "Incompatible versions: <" << this->GetVersion() << "> <" << other.GetVersion() << ">" << GEndl; 
    return kFALSE;
  } 
  /// check RQ version
  /// <Metadata Name="RQTSVNVersion">CoolRunQuery-unknown</Metadata>
  std::map<TString,TString>::const_iterator mitr = this->GetMetaData().find("RQTSVNVersion");
  std::map<TString,TString>::const_iterator oitr = other.GetMetaData().find("RQTSVNVersion");
  if ( mitr==this->GetMetaData().end() && oitr==other.GetMetaData().end() ) {
    ; // ok, RQ version missing from both grls
  } else if ( mitr!=this->GetMetaData().end() && oitr!=other.GetMetaData().end() ) {
    same = same && ( mitr->second==oitr->second );
    if (!same) { m_logger << kDEBUG << "Incompatible run query versions: " << mitr->first
                          << " : <" << mitr->second << "> <" << oitr->second << ">" << GEndl; }
  } else { same=kFALSE; } // version is missing from one grl

/*
  /// check metadata size
  same = same && (this->GetMetaData().size()==other.GetMetaData().size()) ;
  if (!same) { 
    m_logger << kDEBUG << "Incompatible metadata sizes: <" << this->GetMetaData().size() << "> <" << other.GetMetaData().size() << ">" << GEndl; 
    return kFALSE;
  }
  /// check metadata elements
  std::map<TString,TString>::const_iterator mitr = this->GetMetaData().begin();
  for(; mitr!=this->GetMetaData().end() && same; ++mitr) {
    if (other.GetMetaData().find(mitr->first)!=other.GetMetaData().end()) {
      same = same && ( mitr->second==(other.GetMetaData().find(mitr->first))->second );
      if (!same) { m_logger << kDEBUG << "Incompatible metadata: " << mitr->first 
                            << " : <" << mitr->second << "> <" << (other.GetMetaData().find(mitr->first))->second << ">" << GEndl; }
    } else same=kFALSE; 
  }
*/
  return same;
}


Bool_t
Root::TGoodRunsList::IsEmpty() const
{
  if (this->empty()) return kTRUE;

  Bool_t isEmpty(kTRUE);
  std::map< Int_t, TGoodRun >::const_iterator litr = this->begin();
  for (; litr!=this->end() && isEmpty; ++litr)
    isEmpty = isEmpty && litr->second.IsEmpty();

  return isEmpty;
}


const TString
Root::TGoodRunsList::GetSuggestedName() const
{
  if (this->IsEmpty()) return "grl_empty";

  Int_t beginrun(-1), endrun(-1), beginlb(-1), endlb(-1);

  Root::TGoodRun begingr = this->begin()->second;
  Root::TGoodRun endgr   = this->rbegin()->second;

  if (!begingr.IsEmpty()) {
    beginrun = begingr.GetRunNumber();
    beginlb = begingr.begin()->Begin();
  }
  if (!endgr.IsEmpty()) {
    endrun = endgr.GetRunNumber();
    endlb = endgr.rbegin()->End(); 
  } 

  return Form("grl_%d.%d-%d.%d",beginrun,beginlb,endrun,endlb);
}


void 
Root::TGoodRunsList::AddRunLumiBlock( const Int_t& runnr, const Int_t& lumiblocknr )
{
  if (runnr<0 || lumiblocknr<0) return;
  if (this->HasRunLumiBlock(runnr,lumiblocknr)) return;

  std::map< Int_t, TGoodRun >::iterator itr = this->find(runnr);
  if (itr==this->end()) {
    this->insert( std::pair< Int_t, TGoodRun >(runnr,Root::TGoodRun(runnr)) );
    itr = this->find(runnr);
  }

  itr->second.AddLB(lumiblocknr);
}


void 
Root::TGoodRunsList::Compress()
{
  std::map< Int_t, TGoodRun >::iterator itr = this->begin();
  for (; itr!=this->end(); ++itr)
    itr->second.Compress();
}

