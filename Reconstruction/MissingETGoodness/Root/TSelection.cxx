/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/TSelection.h"
#include "MissingETGoodness/TMsgLogger.h"
#include "MissingETGoodness/Goodies.h"

ClassImp(MET::TSelection)


MET::TSelection::TSelection()
  : TRunRange()
  , std::vector<TEasyFormula>()
  , m_eventsbitmask(0)
  , m_metKey("MET_Base")
  , m_jetKey("AntiKt4H1TopoJets")
  , m_uptodate(kFALSE)
{
}


MET::TSelection::TSelection( const Int_t& begin, const Int_t& end )
  : TRunRange(begin,end)
  , std::vector<TEasyFormula>()
  , m_eventsbitmask(0)
  , m_metKey("MET_Base")
  , m_jetKey("AntiKt4H1TopoJets")
  , m_uptodate(kFALSE)
{
}


MET::TSelection::~TSelection()
{
}


MET::TSelection::TSelection(const MET::TSelection& other)
  : TRunRange(other)
  , std::vector<TEasyFormula>(other)
  , m_eventsbitmask(other.m_eventsbitmask)
  , m_metKey(other.m_metKey)
  , m_jetKey(other.m_jetKey)
  , m_uptodate(other.m_uptodate)
{
}


MET::TSelection&
MET::TSelection::operator=(const MET::TSelection& other)
{
  if (&other==this) {
    return *this ;
  } 
  TRunRange::operator = (other);
  std::vector<TEasyFormula>::operator=(other);
  m_eventsbitmask = other.m_eventsbitmask ;
  m_metKey = other.m_metKey ;
  m_jetKey = other.m_jetKey ;
  m_uptodate = other.m_uptodate ;

  return *this ;
}


std::vector< MET::TEasyFormula >::iterator
MET::TSelection::Find( const TString& name )
{
  std::vector<TEasyFormula>::iterator itr = this->begin();

  Bool_t pass(kFALSE);
  for (; itr!=this->end(); ++itr) {
    pass = ( name==TString(itr->GetFormula()) );
    if (pass) break;
  }

  return itr;
}


std::vector< MET::TEasyFormula >::const_iterator
MET::TSelection::Find( const TString& name ) const
{
  std::vector<TEasyFormula>::const_iterator itr = this->begin();

  Bool_t pass(kFALSE);
  for (; itr!=this->end(); ++itr) {
    pass = ( name==TString(itr->GetFormula()) );
    if (pass) break;
  }

  return itr;
}


void
MET::TSelection::Summary () const
{
  MET::TMsgLogger m_logger("TSelection");
  m_logger << kINFO << "MET key : " << m_metKey << GEndl;
  m_logger << kINFO << "Jet collection : " << m_jetKey << GEndl;

  this->TRunRange::Summary() ;

  std::vector<TEasyFormula>::const_iterator itr = this->begin();
  std::vector<TEasyFormula>::const_iterator end = this->end();
  for (; itr!=end; ++itr) { itr->Summary(); }
}


void
MET::TSelection::PrintEvent()
{
  // update inputs, only done if needed.
  (void) this->Evaluate();

  MET::TMsgLogger m_logger("TSelection");
  m_logger << kINFO << "Bit mask of current event : " << m_eventsbitmask << GEndl;

  std::vector<TEasyFormula>::iterator itr = this->begin();
  for (; itr!=this->end(); ++itr)
    m_logger << kINFO << "o) Pass = " << itr->Evaluate() << " : <" << itr->GetFormula() << ">" << GEndl;
}


void 
MET::TSelection::AddCut( const TString& cut, const TString& cutname )
{
  // simple cleaning up before adding cut
  TString newcut = cut.Strip();
  newcut = newcut.ReplaceAll(" ","");

  TString thename(cutname);
  if (cutname.Length()==0) {
    size_t cursize = this->size() + 1;
    thename = "cut";
    thename += cursize ; 
  }

  MET::TEasyFormula formula(newcut);
  formula.SetTitle(thename);

  this->push_back( formula ); 
}


void
MET::TSelection::Update()
{
  if (!m_uptodate) {
    // reference to MET Goodies map for storing derived quantities
    MET::Goodies& goodies = MET::Goodies::instance();
    // update inputs to all selections
    std::vector<TString> parList;
    std::vector<TString>::iterator pitr;
    std::vector<TEasyFormula>::iterator itr = this->begin();
    // set the values
    for (; itr!=this->end(); ++itr) {
      parList = itr->GetParList();
      for (pitr=parList.begin(); pitr!=parList.end(); ++pitr)
        itr->SetValue( *pitr, goodies.getDoubleValue(*pitr) );
    }
    // set uptodate flag
    m_uptodate = kTRUE;
  }
}


Bool_t
MET::TSelection::Evaluate(const std::vector<TString>& parList)
{
  // update inputs?
  Update();

  // evaluation
  Bool_t pass(kFALSE);
  std::vector<TEasyFormula>::iterator itr = this->begin();

  for (int idx=0; itr!=this->end(); ++itr, ++idx) {
    if ( itr->Contains(parList) ) { // true for empty parList
      pass = itr->GetBoolValue();
      if (!pass) break;
    }
  }

  return pass;
}


Long64_t 
MET::TSelection::GetEventsBitMask()
{ 
  // update inputs, only done if needed
  Update();

  // reset selection word
  m_eventsbitmask = 0;

  std::vector<TEasyFormula>::iterator itr = this->begin();
  for (int idx=0; itr!=this->end(); ++itr, ++idx) {
    if (itr->GetBoolValue()) {
      Long64_t pos = (((Long64_t)1) << idx);
      m_eventsbitmask = m_eventsbitmask | pos;
    }
  }

  return m_eventsbitmask; 
}


void 
MET::TSelection::ParseBitMask(const Long64_t bitmask) const
{
  MET::TMsgLogger m_logger("TSelection");
  m_logger << kINFO << "Input bit mask : " << bitmask << GEndl;

  Bool_t passcut(kFALSE);
  std::vector<TEasyFormula>::const_iterator itr = this->begin();
  for (int idx=0; itr!=this->end(); ++itr, ++idx) {
    Long64_t pos = (((Long64_t)1) << idx);
    passcut = (m_eventsbitmask & pos)!=0;
    m_logger << kINFO << "o) Pass = " << passcut << " : <" << itr->GetFormula() << ">" << GEndl;
  }
}

