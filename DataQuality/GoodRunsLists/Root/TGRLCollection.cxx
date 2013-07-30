/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsLists/TGRLCollection.h"
#include "GoodRunsLists/TMsgLogger.h"

#include <algorithm>

ClassImp(Root::TGRLCollection)


Root::TGRLCollection::TGRLCollection( Bool_t checkGRLInfo )
 : std::vector<TGoodRunsList>()
 , TObject()
 , m_checkGRLInfo( checkGRLInfo )
{
}


Root::TGRLCollection::~TGRLCollection()
{
  this->Reset();
}


Root::TGRLCollection::TGRLCollection(const Root::TGRLCollection& other)
 : std::vector<TGoodRunsList>(other)
 , TObject(other)
 , m_checkGRLInfo(other.m_checkGRLInfo)
{
}


Root::TGRLCollection&
Root::TGRLCollection::operator=(const Root::TGRLCollection& other)
{
  if (&other==this) {
    return *this ;
  } 
  std::vector<TGoodRunsList>::operator=(other);
  m_checkGRLInfo=other.m_checkGRLInfo;

  return *this ;
}


void
Root::TGRLCollection::Reset()
{
  this->clear();
  m_checkGRLInfo=kFALSE;
}


void
Root::TGRLCollection::SetVersion( const TString& version )
{
  std::vector<TGoodRunsList>::iterator itr = this->begin();
  std::vector<TGoodRunsList>::iterator end = this->end();
  for (; itr!=end; ++itr) itr->SetVersion(version);
}


void 
Root::TGRLCollection::SetMetaData( const std::map<TString,TString>& metadata )
{
  std::vector<TGoodRunsList>::iterator itr = this->begin();
  std::vector<TGoodRunsList>::iterator end = this->end();
  for (; itr!=end; ++itr) itr->SetMetaData(metadata);
}

void
Root::TGRLCollection::Summary (Bool_t verbose /*= kFALSE*/) const
{
  std::vector<TGoodRunsList>::const_iterator itr = this->begin();
  std::vector<TGoodRunsList>::const_iterator end = this->end();

  for (; itr!=end; ++itr)
    itr->Summary(verbose) ;
}


Bool_t 
Root::TGRLCollection::HasRun( const Int_t& runnr )  const
{
  std::vector<TGoodRunsList>::const_iterator itr = this->begin();
  std::vector<TGoodRunsList>::const_iterator end = this->end();

  Bool_t pass(kFALSE);
  for (; itr!=end && !pass; ++itr)
    pass = itr->HasRun(runnr);

  return pass;
}


Bool_t
Root::TGRLCollection::HasRunLumiBlock( const Int_t& runnr, const Int_t& lumiblocknr ) const
{
  std::vector<TGoodRunsList>::const_iterator itr = this->begin();
  std::vector<TGoodRunsList>::const_iterator end = this->end();

  Bool_t pass(kFALSE);
  for (; itr!=end && !pass; ++itr)
    pass = itr->HasRunLumiBlock(runnr,lumiblocknr);

  return pass;
}


Bool_t 
Root::TGRLCollection::IsEmpty() const
{
  if (this->empty()) return kTRUE;

  Bool_t isEmpty(kTRUE);
  std::vector< TGoodRunsList >::const_iterator litr = this->begin();
  for (; litr!=this->end() && isEmpty; ++litr)
    isEmpty = isEmpty && litr->IsEmpty();

  return isEmpty;
}


const Root::TGoodRunsList 
Root::TGRLCollection::GetMergedGoodRunsList( const Root::BoolOperation& operation ) const 
{
  // nothing interpreted. Return empty grl.
  if (this->empty()) return Root::TGoodRunsList();

  // set first goodrunslist
  std::vector<Root::TGoodRunsList>::const_iterator itr = this->begin();
  Root::TGoodRunsList grl(*itr);
  if (this->size()==1) {
    grl.Compress();
    return grl;
  }

  TMsgLogger mylogger( "TGRLCollection" );
  mylogger << kINFO << "Now merging GRLs." << GEndl;

  // check version and metadata when merging goodrunslists?
  grl.SetCheckGRLInfo(m_checkGRLInfo);

  if (!m_checkGRLInfo)
    mylogger << kINFO << "Metadata and other info not required to be identical between GRLs." << GEndl;

  // start AND-ing or OR-ring with following goodrunslists 
  for (++itr; itr!=this->end(); ++itr) {
    switch (operation) {
      case OR :
        if ( grl.HasOverlapWith(*itr,false/*verbose*/) ) { // MB 22-june: LB splitting across files, turn off warning.
          //mylogger << kWARNING << "Merging GRLs with overlapping lumi-blocks! Overlapping LBs rejected." << GEndl;
          //mylogger << kWARNING << "IMPORTANT : Check your analysis for possible duplicate events!" << GEndl;
        }
        grl.AddGRL( *itr );
      break;
      case AND :
        grl = grl.GetOverlapWith(*itr);
      break;
    }
  }

  grl.Compress(); // cleanup, safe space
  return grl;  
}


const Root::TGoodRunsList 
Root::TGRLCollection::GetGoodRunsList( unsigned int idx ) const 
{ 
  // invalid idx. Return empty grl.
  if (idx>=this->size()) return Root::TGoodRunsList();
  
  return (*this)[idx]; 
}


const Root::TGRLCollection
Root::TGRLCollection::GetMergedGRLCollection( const Root::BoolOperation& operation ) const
{
  if (this->empty() /*|| this->size()==1*/) return *this;  // nothing to merge, return this

  TMsgLogger mylogger( "TGRLCollection" );
  mylogger << kINFO << "Now merging GRLs where possible. Metadata required to be identical." << GEndl;

  Root::TGRLCollection mergevec;

  std::vector<TGoodRunsList>::const_iterator itr = this->begin();
  std::vector<TGoodRunsList>::const_iterator end = this->end();
  std::vector<TGoodRunsList>::iterator mitr;

  for (; itr!=end; ++itr) {
    bool matchFound(false);
    for (mitr=mergevec.begin(); mitr!=mergevec.end() && !matchFound ; ++mitr) {
      if (mitr->HasSameGRLInfo(*itr)) {
        matchFound = true;
        switch (operation) {
          case OR :
            if ( mitr->HasOverlapWith(*itr,false/*verbose*/) ) { // // MB 22-june: LB splitting across files, turn off warning.
              //mylogger << kWARNING << "Merging GRLs with overlapping lumi-blocks! Overlapping LBs rejected." << GEndl;
              //mylogger << kWARNING << "IMPORTANT : Check your analysis for possible duplicate events!" << GEndl;
            }
            mitr->AddGRL( *itr );
          break;
          case AND :
            *mitr = mitr->GetOverlapWith( *itr );
          break;
        }
        mitr->Compress(); // safe space
      }
    }
    if (!matchFound) {
      mergevec.push_back(*itr);
      mergevec.rbegin()->Compress(); // safe space
    }
  }

  return mergevec;
}


std::vector<Root::TGoodRunsList>::iterator 
Root::TGRLCollection::find( const TString& name )
{
  Bool_t found(false);

  std::vector<TGoodRunsList>::iterator itr = this->begin();
  std::vector<TGoodRunsList>::iterator end = this->end();

  for (; itr!=end; ++itr) {
    found = ( name==TString(itr->GetName()) ) ;
    if (found) break;
  }

  return itr;
}


std::vector<Root::TGoodRunsList>::const_iterator 
Root::TGRLCollection::find( const TString& name ) const 
{
  Bool_t found(false);

  std::vector<TGoodRunsList>::const_iterator itr = this->begin();
  std::vector<TGoodRunsList>::const_iterator end = this->end();

  for (; itr!=end; ++itr) {
    found = ( name==TString(itr->GetName()) ) ;
    if (found) break;
  }

  return itr;
}


Bool_t
Root::TGRLCollection::HasGoodRunsList( const TString& name ) const 
{
  return (this->find(name)!=this->end());
}


const Root::TGRLCollection
Root::TGRLCollection::GetOverlapWith( const TGoodRunsList& other ) const
{
  Root::TGRLCollection overlapvec;

  std::vector<TGoodRunsList>::const_iterator itr = this->begin();
  for (; itr!=this->end(); ++itr) {
    Root::TGoodRunsList overlapgrl = itr->GetOverlapWith(other);
    overlapgrl.SetName(itr->GetName());
    overlapgrl.SetVersion(itr->GetVersion());
    overlapgrl.SetMetaData(itr->GetMetaData());
    overlapgrl.Compress();
    overlapvec.push_back(overlapgrl); // also push_back if empty!
  }

  return overlapvec;
}



