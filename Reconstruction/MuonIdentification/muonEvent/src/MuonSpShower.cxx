/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                         MuonSpShower.cxx  -  Description
                             -------------------
    begin   : 28-07-2006
    authors : Ketevi A. Assamagan
    email   : Ketevi@bnl.gov
    
 ***************************************************************************/

#include "muonEvent/MuonSpShower.h"

namespace Rec {

/** default constructor */
MuonSpShower::MuonSpShower() :
   m_eta(0),
   m_phi(0),
   m_numberOfTriggerHits(0),
   m_numberOfInnerHits(0),
   m_numberOfInnerSegments(0),
   m_numberOfMiddleHits(0),
   m_numberOfMiddleSegments(0),
   m_numberOfOuterHits(0),
   m_numberOfOuterSegments(0)
{}

/** Full constructor */
MuonSpShower::MuonSpShower(
   const float eta, const float phi,
   const unsigned short numOfTriggerHits,
   const unsigned short numOfInnerHits,
   const unsigned short numOfMiddleHits,
   const unsigned short numOfOuterHits,
   const unsigned short numOfInnerSegments,
   const unsigned short numOfMiddleSegments,
   const unsigned short numOfOuterSegments) {

   m_eta                    = eta;
   m_phi                    = phi;
   m_numberOfTriggerHits    = numOfTriggerHits;
   m_numberOfInnerHits      = numOfInnerHits;
   m_numberOfMiddleHits     = numOfMiddleHits;
   m_numberOfOuterHits     = numOfOuterHits;
   m_numberOfInnerSegments  = numOfInnerSegments;
   m_numberOfMiddleSegments = numOfMiddleSegments;
   m_numberOfOuterSegments = numOfOuterSegments;
}

/** Copy Constructor */
MuonSpShower::MuonSpShower( const MuonSpShower& rhs ) :
   m_eta                    ( rhs.m_eta ),
   m_phi                    ( rhs.m_phi ),
   m_numberOfTriggerHits    ( rhs.m_numberOfTriggerHits ),
   m_numberOfInnerHits      ( rhs.m_numberOfInnerHits ),
   m_numberOfInnerSegments  ( rhs.m_numberOfInnerSegments ),
   m_numberOfMiddleHits     ( rhs.m_numberOfMiddleHits ),
   m_numberOfMiddleSegments ( rhs.m_numberOfMiddleSegments ),
   m_numberOfOuterHits     ( rhs.m_numberOfOuterHits ),
   m_numberOfOuterSegments ( rhs.m_numberOfOuterSegments )
{}

/** Assignement operator */
MuonSpShower& MuonSpShower::operator=( const MuonSpShower& rhs )
{
  if ( this != &rhs ) {
     m_eta                    = rhs.m_eta;
     m_phi                    = rhs.m_phi;
     m_numberOfTriggerHits    = rhs.m_numberOfTriggerHits;
     m_numberOfInnerHits      = rhs.m_numberOfInnerHits;
     m_numberOfMiddleHits     = rhs.m_numberOfMiddleHits;
     m_numberOfOuterHits     = rhs.m_numberOfOuterHits;
     m_numberOfInnerSegments  = rhs.m_numberOfInnerSegments;
     m_numberOfMiddleSegments = rhs.m_numberOfMiddleSegments;
     m_numberOfOuterSegments = rhs.m_numberOfOuterSegments;
  }
  return *this;
}

/** Destructor */
MuonSpShower::~MuonSpShower() 
{}

}


