/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        HadFlavorTagInfo.h  -  Description
                             -------------------
    begin   : April 2011
    authors : Maria Laura Gonzalez Silva
    email   : laugs@cern.ch
    changes :

 ***************************************************************************/

#include "JetTagInfo/HadFlavorTagInfo.h"

namespace Analysis
{

/** Default constructor */
HadFlavorTagInfo::HadFlavorTagInfo() : BaseTagInfo(),
				   m_nMatchingTracks(-1),
				   m_trkJetWidth(-1),
				   m_trkJetDRKtaxes(-1)
{
  
}
  



/** constructor with infotype */
HadFlavorTagInfo::HadFlavorTagInfo(TagInfoType tagJetInfoType) : 
  BaseTagInfo(tagJetInfoType),
  m_nMatchingTracks(-1),
  m_trkJetWidth(-1),
  m_trkJetDRKtaxes(-1)
{
  
}

/** Copy constructor */
HadFlavorTagInfo::HadFlavorTagInfo(const HadFlavorTagInfo& rhs) : 
  BaseTagInfo(rhs),
  m_nMatchingTracks(rhs.m_nMatchingTracks),
  m_trkJetWidth(rhs.m_trkJetWidth),
  m_trkJetDRKtaxes(rhs.m_trkJetDRKtaxes)
{

}

/** assigenment operator */
HadFlavorTagInfo& HadFlavorTagInfo::operator= (const HadFlavorTagInfo& rhs)
{
    if (this!=&rhs)
    {
      m_nMatchingTracks=rhs.m_nMatchingTracks;
      m_trkJetWidth=rhs.m_trkJetWidth;
      m_trkJetDRKtaxes=rhs.m_trkJetDRKtaxes;
    }
    return *this;
}

/** Default destructor */
HadFlavorTagInfo::~HadFlavorTagInfo()
{}

}
