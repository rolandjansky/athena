/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        GbbNNTagInfo.h  -  Description
                             -------------------
    begin   : April 2011
    authors : Maria Laura Gonzalez Silva
    email   : laugs@cern.ch
    changes :

 ***************************************************************************/

#include "JetTagInfo/GbbNNTagInfo.h"

namespace Analysis
{

/** Default constructor */
GbbNNTagInfo::GbbNNTagInfo() : BaseTagInfo(),
				   m_nMatchingTracks(-1),
				   m_trkJetWidth(-1),
				   m_trkJetMaxDeltaR(-1)
{
  
}
  



/** constructor with infotype */
GbbNNTagInfo::GbbNNTagInfo(TagInfoType tagJetInfoType) : 
  BaseTagInfo(tagJetInfoType),
  m_nMatchingTracks(-1),
  m_trkJetWidth(-1),
  m_trkJetMaxDeltaR(-1)
{
  
}

/** Copy constructor */
GbbNNTagInfo::GbbNNTagInfo(const GbbNNTagInfo& rhs) : 
  BaseTagInfo(rhs),
  m_nMatchingTracks(rhs.m_nMatchingTracks),
  m_trkJetWidth(rhs.m_trkJetWidth),
  m_trkJetMaxDeltaR(rhs.m_trkJetMaxDeltaR)
{

}

/** assigenment operator */
GbbNNTagInfo& GbbNNTagInfo::operator= (const GbbNNTagInfo& rhs)
{
    if (this!=&rhs)
    {
      m_nMatchingTracks=rhs.m_nMatchingTracks;
      m_trkJetWidth=rhs.m_trkJetWidth;
      m_trkJetMaxDeltaR=rhs.m_trkJetMaxDeltaR;
    }
    return *this;
}

/** Default destructor */
GbbNNTagInfo::~GbbNNTagInfo()
{}

}
