/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsLists/TLumiBlockRange.h"
#include "GoodRunsLists/TMsgLogger.h"

ClassImp(Root::TLumiBlockRange)


Root::TLumiBlockRange::TLumiBlockRange()
  : TObject()
  , m_begin(0)
  , m_end(2147483647)
{
}


Root::TLumiBlockRange::TLumiBlockRange( const Int_t& begin, const Int_t& end )
  : TObject()
  , m_begin(begin)
  , m_end(end)
{
}


Root::TLumiBlockRange::~TLumiBlockRange()
{
}


Root::TLumiBlockRange::TLumiBlockRange(const Root::TLumiBlockRange& other)
  : TObject(other)
  , m_begin(other.m_begin)
  , m_end(other.m_end)
{
}


Root::TLumiBlockRange&
Root::TLumiBlockRange::operator=(const Root::TLumiBlockRange& other)
{
  if (&other==this) {
    return *this ;
  } 
  m_begin = other.m_begin ;
  m_end = other.m_end ;

  return *this ;
}


const Root::TLumiBlockRange 
Root::TLumiBlockRange::GetOverlapWith(const TLumiBlockRange& other) const
{
  Int_t begin = ( this->Begin()>=other.Begin() ? this->Begin() : other.Begin() );
  Int_t end   = ( this->End()<=other.End() ? this->End() : other.End() );
  return Root::TLumiBlockRange(begin,end);
}


const std::vector<Root::TLumiBlockRange> 
Root::TLumiBlockRange::GetPartOnlyIn(const Root::TLumiBlockRange& other) const
{
  return other.GetPartNotIn(*this);
}


const std::vector<Root::TLumiBlockRange> 
Root::TLumiBlockRange::GetPartNotIn(const Root::TLumiBlockRange& other) const
{
  std::vector<Root::TLumiBlockRange> notinother;

  Int_t begin(-1), end(-1);

  if ( other.Begin()>this->Begin() ) {
    begin = this->Begin() ;
    end = ( other.Begin()>this->End() ? this->End() : other.Begin()-1 ) ; 
    if (end>=begin) notinother.push_back( Root::TLumiBlockRange(begin,end) ) ;
  }

  if ( other.End()<this->End() ) {
    begin = ( other.End()>=this->Begin() ? other.End()+1 : this->Begin() ) ;
    end = this->End();
    if (end>=begin) notinother.push_back( Root::TLumiBlockRange(begin,end) ) ;
  }

  return notinother;
}


Bool_t 
Root::TLumiBlockRange::Contains( const Int_t& number )  const
{
  if (number>=m_begin && number<=m_end) return kTRUE;
  else return kFALSE;
}


void
Root::TLumiBlockRange::Summary() const 
{
  Root::TMsgLogger m_logger("TLumiBlockRange");
  m_logger << kINFO << "Begin: " << m_begin << " End: " << m_end << GEndl;  
}

