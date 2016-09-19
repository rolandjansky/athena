/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//// label with (c) ATLAS Experiment Software
#include "MuonCombinedEvent/MuonOverlapDescriptor.h"
#include "GaudiKernel/MsgStream.h"

Rec::MuonOverlapDescriptor::MuonOverlapDescriptor (bool SharesIDTrack,
						   bool SharesSpectroTrack,
						   unsigned int SharedPrecisionHits,
						   unsigned int TotalPrecisionHits,
						   unsigned int SharedSpectroPhiHits,
						   unsigned int TotalSpectroPhiHits, 
						   std::set <Identifier> msIntersection)

  :   m_sharesIndetTrack (SharesIDTrack),
      m_sharesSpectroTrack (SharesSpectroTrack),
      m_sharedPrecisionHits(SharedPrecisionHits),
      m_totalPrecisionHits(TotalPrecisionHits),
      m_sharedSpectroPhiHits(SharedSpectroPhiHits),
      m_totalSpectroPhiHits(TotalSpectroPhiHits),
      m_intersection(msIntersection)
{}

/** defect constructor */
Rec::MuonOverlapDescriptor::MuonOverlapDescriptor() 
  :   m_sharesIndetTrack (false),
      m_sharesSpectroTrack (false),
      m_sharedPrecisionHits(0),
      m_totalPrecisionHits(0),
      m_sharedSpectroPhiHits(0),
      m_totalSpectroPhiHits(0),
      m_intersection()
{}

/** copy constructor */
Rec::MuonOverlapDescriptor::MuonOverlapDescriptor (const MuonOverlapDescriptor& input)
  :   m_sharesIndetTrack (input.m_sharesIndetTrack),
      m_sharesSpectroTrack (input.m_sharesSpectroTrack),
      m_sharedPrecisionHits(input.m_sharedPrecisionHits),
      m_totalPrecisionHits(input.m_totalPrecisionHits),
      m_sharedSpectroPhiHits(input.m_sharedSpectroPhiHits),
      m_totalSpectroPhiHits(input.m_totalSpectroPhiHits)
{}

/** destructor */
Rec::MuonOverlapDescriptor::~MuonOverlapDescriptor() 
{}

/** Assignment operator */
Rec::MuonOverlapDescriptor& Rec::MuonOverlapDescriptor::operator=(const MuonOverlapDescriptor& rhs) {
  if ( this != &rhs ) {
    m_sharesIndetTrack = rhs.m_sharesIndetTrack;
    m_sharesSpectroTrack = rhs.m_sharesSpectroTrack;
    m_sharedPrecisionHits= rhs.m_sharedPrecisionHits;
    m_totalPrecisionHits= rhs.m_totalPrecisionHits;
    m_sharedSpectroPhiHits= rhs.m_sharedSpectroPhiHits;
    m_totalSpectroPhiHits= rhs.m_totalSpectroPhiHits;
  }
  return *this;
}

/** summary method if there is any overlap */
bool Rec::MuonOverlapDescriptor::hasOverlap(unsigned int toleratedSharedhits) const {
  if ( sharesIndetTrack() || sharesSpectroTrack() || 
       nSharedPrecisionHits()>toleratedSharedhits ||
       nSharedSpectroPhiHits()>toleratedSharedhits )
    return true;
  else return false;
}

/** MsgStream output */
MsgStream& Rec::operator << ( MsgStream& sl, const MuonOverlapDescriptor& itself)
// if namespace, need to use MsgStream& Rec::operator :-(
{ 
  if (itself.hasOverlap()) {
    sl <<"Muon overlap desriptor: "<< endmsg;
    sl <<"ID pointers: " << (itself.sharesIndetTrack()? "identical":"different")
       <<",  MS pointers: " << (itself.sharesSpectroTrack()? "identical" : "different")
       <<endmsg;
    sl <<"MS hits: " << itself.nSharedPrecisionHits() << " shared prec. hits of "
       << itself.nTotalPrecisionHits()<<" total." << endmsg;
    sl <<"         " << itself.nSharedSpectroPhiHits() << " shared phi hits of "
       << itself.nTotalSpectroPhiHits()<<" totoal."<<endmsg;
  } else sl<<"Muon overlap desriptor: no Overlap."<<endmsg;
  return sl; 
}

/** MsgStream output */
std::ostream& Rec::operator << ( std::ostream& sl, const MuonOverlapDescriptor& itself)
// if namespace, need to use MsgStream& Rec::operator :-(
{ 
  if (itself.hasOverlap()) {
    sl <<"Muon overlap desriptor: "<< std::endl;
    sl <<"ID pointers: " << (itself.sharesIndetTrack()? "identical":"different")
       <<",  MS pointers: " << (itself.sharesSpectroTrack()? "identical" : "different")
       <<std::endl;
    sl <<"MS hits: " << itself.nSharedPrecisionHits() << " shared prec. hits of "
       << itself.nTotalPrecisionHits()<<" total." << std::endl;
    sl <<"         " << itself.nSharedSpectroPhiHits() << " shared phi hits of "
       << itself.nTotalSpectroPhiHits()<<" totoal."<<std::endl;
  } else sl<<"Muon overlap desriptor: no Overlap."<<std::endl;

  return sl; 
}
