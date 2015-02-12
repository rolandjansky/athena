/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SvxSummary.h"

namespace Analysis {

/** Default constructor */
SvxSummary::SvxSummary() : BaseTagInfo(),
			   m_Svx(Trk::RecVertex()),
			   m_Results(std::vector<double>()),
			   m_TrkFromV0(std::vector<const Rec::TrackParticle*>()) {
  m_Results.clear();
  m_TrkFromV0.clear();
}

/** constructor with info type */
SvxSummary::SvxSummary(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
						     m_Svx(Trk::RecVertex()),
						     m_Results(std::vector<double>()),
						     m_TrkFromV0(std::vector<const Rec::TrackParticle*>()) {
  m_Results.clear();
  m_TrkFromV0.clear();
}

/** Copy constructor */
SvxSummary::SvxSummary(const SvxSummary& rhs) : BaseTagInfo(rhs),
						m_Svx(rhs.m_Svx),
						m_Results(rhs.m_Results),
						m_TrkFromV0(rhs.m_TrkFromV0) {}

/** assigenment operator */
SvxSummary& SvxSummary::operator= (const SvxSummary& rhs)
{
    if (this!=&rhs)
    {
      m_Svx       = rhs.m_Svx;
      m_Results   = rhs.m_Results;
      m_TrkFromV0 = rhs.m_TrkFromV0;
      m_tagJetInfoType=rhs.m_tagJetInfoType;
    }
    return *this;
}

}

