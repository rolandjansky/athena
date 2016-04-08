/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           LifetimeInfo.cxx  -  Description
                             -------------------
    begin   : Spring 2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

 ***************************************************************************/

#include "JetTagInfo/LifetimeInfo.h"

namespace Analysis
{

/** Default constructor */
LifetimeInfo::LifetimeInfo() : BaseTagInfo(),
        m_trackSIP(std::vector<double>()),
        m_trackSignificance(std::vector<double>()),
        m_vectorOfTrackProb(std::vector<double>()),
        m_nTrackProb(-1.)
{
    m_vectorOfTrackProb.clear();
    m_trackSIP.clear();
    m_trackSignificance.clear();
}

/** constructor with info type */
LifetimeInfo::LifetimeInfo(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
        m_trackSIP(std::vector<double>()),
        m_trackSignificance(std::vector<double>()),
        m_vectorOfTrackProb(std::vector<double>()),
        m_nTrackProb(-1.)
{
    m_tagLikelihood.clear();
    m_vectorOfTrackProb.clear();
    m_trackSIP.clear();
    m_trackSignificance.clear();
}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
LifetimeInfo::LifetimeInfo(const LifetimeInfo& rhs) : BaseTagInfo(rhs),
        m_trackSIP(rhs.m_trackSIP),
        m_trackSignificance(rhs.m_trackSignificance),
        m_vectorOfTrackProb(rhs.m_vectorOfTrackProb),
        m_nTrackProb(rhs.m_nTrackProb)
{}

/** assigenment operator */
LifetimeInfo& LifetimeInfo::operator= (const LifetimeInfo& rhs)
{
    if (this!=&rhs)
    {
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_vectorOfTrackProb=rhs.m_vectorOfTrackProb;
        m_nTrackProb=rhs.m_nTrackProb;
        m_trackSIP=rhs.m_trackSIP;
        m_trackSignificance=rhs.m_trackSignificance;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
    }
    return *this;
}

/** Default destructor */
LifetimeInfo::~LifetimeInfo()
{}

}

