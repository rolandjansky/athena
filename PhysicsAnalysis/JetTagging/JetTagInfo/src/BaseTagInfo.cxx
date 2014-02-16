/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           BaseTagInfo.cxx  -  Description
                             -------------------
    begin   : January 2005
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

 ***************************************************************************/

#include "JetTagInfo/BaseTagInfo.h"

namespace Analysis
{

/** Default constructor */
BaseTagInfo::BaseTagInfo() : JetTagInfoBase(),
        m_isValid(false),
        m_tagLikelihood(std::vector<double>()),
        m_weight(0.),
        m_tagJetInfoType("NoInfoType")
{
    m_tagLikelihood.clear();
}

/** constructor with info type */
BaseTagInfo::BaseTagInfo(TagInfoType tagJetInfoType) : JetTagInfoBase(),
        m_isValid(false),
        m_tagLikelihood(std::vector<double>()),
        m_weight(0.),
        m_tagJetInfoType(tagJetInfoType)
{
    m_tagLikelihood.clear();
}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
BaseTagInfo::BaseTagInfo(const BaseTagInfo& rhs) : JetTagInfoBase(rhs),
        m_isValid(rhs.m_isValid),
        m_tagLikelihood(rhs.m_tagLikelihood),
        m_weight(rhs.m_weight),
        m_tagJetInfoType(rhs.m_tagJetInfoType)
{}

/** assigenment operator */
BaseTagInfo& BaseTagInfo::operator= (const BaseTagInfo& rhs)
{
    if (this!=&rhs)
    {
        m_isValid=rhs.m_isValid;
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_weight=rhs.m_weight;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
    }
    return *this;
}

/** Default destructor */
BaseTagInfo::~BaseTagInfo()
{}

}
