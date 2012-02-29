// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          MuonSpShower.h  -  Description
                             -------------------
    begin   : 28-07-2006
    authors : Ketevi A. Assamagan
    email   : ketevi@bnl.gov

    Store the numbers of hits and numbers of MuonSegments (MDT & CSC) in some
    cone around jet axes. Store these number for Inner, Middle and Outer
    multi-layers.

    April 2009: Riccardo Maria BIANCHI - Completing Doxygen docs for AOD Content Documentation
 ***************************************************************************/
#ifndef MUONEVENT_MUONSPSHOWER_H
#define MUONEVENT_MUONSPSHOWER_H 1

#include <vector>

namespace Rec {

  class MuonSpShower {

  public:

    /** Default Constructor  needed for persistency */
    MuonSpShower();

    /** constructor */
    MuonSpShower(
	const float eta, const float phi,
        const unsigned short numOfTriggerHits,
	const unsigned short numOfInnerHits,
	const unsigned short numOfMiddleHits,
	const unsigned short numOfOuterHits,
	const unsigned short numOfInnerSegments,
	const unsigned short numOfMiddleSegments,
	const unsigned short numOfOuterSegments);

    /** copy constructor */
    MuonSpShower(const MuonSpShower & rhs);

    /** Assignement operator */
    MuonSpShower &operator= (const MuonSpShower &);

    /** Destructor */
    virtual ~MuonSpShower();

    /** return the Jet axis - eta*/
    float eta() const { return m_eta; }
    /** return the Jet axis - phi*/
    float phi() const { return m_phi; }

    /** return the number of Trigger hits */
    unsigned short numberOfTriggerHits() const
    { return m_numberOfTriggerHits; }
    /** return the number of Inner hits */
    unsigned short numberOfInnerHits() const
    { return m_numberOfInnerHits; }
    /** return the number of Middle hits */
    unsigned short numberOfMiddleHits() const
    { return m_numberOfMiddleHits; }
    /** return the number of Outer hits */
    unsigned short numberOfOuterHits() const
    { return m_numberOfOuterHits; }

    /** return the number of Inner segments */
    unsigned short numberOfInnerSegments() const
    { return m_numberOfInnerSegments; }
    /** return the number of Middle segments */
    unsigned short numberOfMiddleSegments() const
    { return m_numberOfMiddleSegments; }
    /** return the number of Outer segments */
    unsigned short numberOfOuterSegments () const
    { return m_numberOfOuterSegments; }

    /** set the Jet axis - eta */
    void set_eta(const float axisEta) { m_eta = axisEta; }
    /** set the Jet axis - phi */
    void set_phi(const float axisPhi) { m_phi = axisPhi; }

    /** set the number of Trigger Hits */
    void set_numberOfTriggerHits  (const unsigned short nHit) { m_numberOfTriggerHits  = nHit; }
    /** set the number of Inner Hits */
    void set_numberOfInnerHits  (const unsigned short nHit) { m_numberOfInnerHits  = nHit; }
    /** set the number of Middle Hits */
    void set_numberOfMiddleHits (const unsigned short nHit) { m_numberOfMiddleHits = nHit; }
    /** set the number of Outer Hits */
    void set_numberOfOuterHits (const unsigned short nHit) { m_numberOfOuterHits = nHit; }

    /** set the number of Inner segments */
    void set_numberOfInnerSegments  (const unsigned short nSeg) { m_numberOfInnerSegments  = nSeg; }
    /** set the number of Middle segments */
    void set_numberOfMiddleSegments (const unsigned short nSeg) { m_numberOfMiddleSegments = nSeg; }
    /** set the number of Outer segments */
    void set_numberOfOuterSegments (const unsigned short nSeg) { m_numberOfOuterSegments = nSeg; }

  private:

    float m_eta;
    float m_phi;
    unsigned short m_numberOfTriggerHits;
    unsigned short m_numberOfInnerHits;
    unsigned short m_numberOfInnerSegments;
    unsigned short m_numberOfMiddleHits;
    unsigned short m_numberOfMiddleSegments;
    unsigned short m_numberOfOuterHits;
    unsigned short m_numberOfOuterSegments;

  };

}

#endif

