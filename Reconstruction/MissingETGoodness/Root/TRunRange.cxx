/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/TRunRange.h"
#include "MissingETGoodness/TMsgLogger.h"

ClassImp(MET::TRunRange)


MET::TRunRange::TRunRange()
  : TObject()
  , m_begin(0)
  , m_end(2147483647)
{
}


MET::TRunRange::TRunRange( const Int_t& begin, const Int_t& end )
  : TObject()
  , m_begin(begin)
  , m_end(end)
{
}


MET::TRunRange::~TRunRange()
{
}


MET::TRunRange::TRunRange(const MET::TRunRange& other)
  : TObject(other)
  , m_begin(other.m_begin)
  , m_end(other.m_end)
{
}


MET::TRunRange&
MET::TRunRange::operator=(const MET::TRunRange& other)
{
  if (&other==this) {
    return *this ;
  } 
  m_begin = other.m_begin ;
  m_end = other.m_end ;

  return *this ;
}


const MET::TRunRange 
MET::TRunRange::GetOverlapWith(const TRunRange& other) const
{
  Int_t begin = ( this->Begin()>=other.Begin() ? this->Begin() : other.Begin() );
  Int_t end   = ( this->End()<=other.End() ? this->End() : other.End() );
  return MET::TRunRange(begin,end);
}


const std::vector<MET::TRunRange> 
MET::TRunRange::GetPartOnlyIn(const MET::TRunRange& other) const
{
  return other.GetPartNotIn(*this);
}


const std::vector<MET::TRunRange> 
MET::TRunRange::GetPartNotIn(const MET::TRunRange& other) const
{
  std::vector<MET::TRunRange> notinother;

  Int_t begin(-1), end(-1);

  if ( other.Begin()>this->Begin() ) {
    begin = this->Begin() ;
    end = ( other.Begin()>this->End() ? this->End() : other.Begin()-1 ) ; 
    if (end>=begin) notinother.push_back( MET::TRunRange(begin,end) ) ;
  }

  if ( other.End()<this->End() ) {
    begin = ( other.End()>=this->Begin() ? other.End()+1 : this->Begin() ) ;
    end = this->End();
    if (end>=begin) notinother.push_back( MET::TRunRange(begin,end) ) ;
  }

  return notinother;
}


Bool_t 
MET::TRunRange::ContainsRun( const Int_t& number )  const
{
  if (this->ContainsAnyRun()) return kTRUE;
  else if (number>=m_begin && number<=m_end) return kTRUE;
  else return kFALSE;
}


Bool_t
MET::TRunRange::ContainsAnyRun() const
{
  return (m_begin==0 && m_end==2147483647) ;
}

void
MET::TRunRange::Summary() const 
{
  MET::TMsgLogger m_logger("TRunRange");
  m_logger << kINFO << "Begin: " << m_begin << " End: " << m_end << GEndl;  
}

