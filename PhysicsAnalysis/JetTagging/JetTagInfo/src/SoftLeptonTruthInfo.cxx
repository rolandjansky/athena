/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SoftLeptonTruthInfo.h"

namespace Analysis
{

/** Default constructor */
SoftLeptonTruthInfo::SoftLeptonTruthInfo() : BaseTagInfo(), m_sltrueinfo() {}

/** constructor with info type */
SoftLeptonTruthInfo::SoftLeptonTruthInfo(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
								       m_sltrueinfo()
{
  m_sltrueinfo.clear();
}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
SoftLeptonTruthInfo::SoftLeptonTruthInfo(const SoftLeptonTruthInfo& rhs) : BaseTagInfo(rhs),
									   m_sltrueinfo(rhs.m_sltrueinfo)
{}

/** assigenment operator */
SoftLeptonTruthInfo& SoftLeptonTruthInfo::operator= (const SoftLeptonTruthInfo& rhs)
{
    if (this!=&rhs)
    {
      m_sltrueinfo = rhs.m_sltrueinfo;
    }
    return *this;
}

/** Default destructor */
SoftLeptonTruthInfo::~SoftLeptonTruthInfo() {}

}
