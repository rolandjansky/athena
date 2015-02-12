/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     SoftElectronInfo.cxx
PACKAGE:  offline/PhysicsAnalysis/BTaggingID/BTaggingUtils

AUTHORS:  F. Derue, A. Kaczmarska, M. Wolter
CREATED:  Nov 2005 

PURPOSE:  EDM for b-tagging based on soft lepton identification
       
COMMENTS: evolved from BJetLifetimeInfo

UPDATE:

********************************************************************/

#include "JetTagInfo/SoftElectronInfo.h"

namespace Analysis
{

/** Default constructor */
SoftElectronInfo::SoftElectronInfo()
  : BaseTagInfo(), 
    m_nTrackProb(0),
    m_trackinfo()
{

}

/** constructor with info type */
SoftElectronInfo::SoftElectronInfo(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
        m_vectorOfTrackProb(std::vector<double>()),
        m_nTrackProb(-1.), m_trackinfo()

{
    m_vectorOfTrackProb.clear();
    m_trackinfo.clear();
}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
SoftElectronInfo::SoftElectronInfo(const SoftElectronInfo& rhs) : BaseTagInfo(rhs),
        m_vectorOfTrackProb(rhs.m_vectorOfTrackProb),
        m_nTrackProb(rhs.m_nTrackProb), m_trackinfo(rhs.m_trackinfo) 

{}

/** assigenment operator */
SoftElectronInfo& SoftElectronInfo::operator= (const SoftElectronInfo& rhs)
{
    if (this!=&rhs)
    {
        m_trackinfo=rhs.m_trackinfo;
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_vectorOfTrackProb=rhs.m_vectorOfTrackProb;
        m_nTrackProb=rhs.m_nTrackProb;
        m_tagJetInfoType=rhs.m_tagJetInfoType;

    }
    return *this;
}

/** Default destructor */
SoftElectronInfo::~SoftElectronInfo()
{}

}

