/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/TruthInfo.h"
#include <algorithm>
#include <utility>

namespace Analysis
{

/** Default constructor */
TruthInfo::TruthInfo()
  : BaseTagInfo(),
    m_jetTruthLabel("N/A"),
    m_distanceToBQuark (999.),
    m_distanceToCQuark (999.),
    m_distanceToTLepton (999.),
    m_BHadronPdg (0)
{}

/** constructor with info type */
TruthInfo::TruthInfo(TagInfoType tagJetInfoType)
  : BaseTagInfo(tagJetInfoType),
    m_jetTruthLabel("N/A"),
    m_distanceToBQuark (999.),
    m_distanceToCQuark (999.),
    m_distanceToTLepton (999.),
    m_BHadronPdg (0)
{}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
TruthInfo::TruthInfo(const TruthInfo& rhs) : BaseTagInfo(rhs),
					     m_jetTruthLabel(rhs.m_jetTruthLabel),
					     m_distanceToBQuark(rhs.m_distanceToBQuark),
					     m_distanceToCQuark(rhs.m_distanceToCQuark),
					     m_distanceToTLepton(rhs.m_distanceToTLepton),
					     m_BHadronDecayVertex(rhs.m_BHadronDecayVertex),
					     m_BHadronPdg(rhs.m_BHadronPdg)
{}

/** assigenment operator */
TruthInfo& TruthInfo::operator= (const TruthInfo& rhs)
{
    if (this!=&rhs)
    {
        m_jetTruthLabel=rhs.m_jetTruthLabel;
	m_distanceToBQuark=rhs.m_distanceToBQuark;
	m_distanceToCQuark=rhs.m_distanceToCQuark;
	m_distanceToTLepton=rhs.m_distanceToTLepton;
	m_BHadronDecayVertex=rhs.m_BHadronDecayVertex;
	m_BHadronPdg=rhs.m_BHadronPdg;
    }
    return *this;
}

/** Default destructor */
TruthInfo::~TruthInfo()
{}

void TruthInfo::deltaRMinTo(const std::string& key, double dR) {
  if( "B"==key || "b"==key ) m_distanceToBQuark  = dR;
  if( "C"==key || "c"==key ) m_distanceToCQuark  = dR;
  if( "T"==key || "t"==key ) m_distanceToTLepton = dR;
}

double TruthInfo::deltaRMinTo(const std::string& key) const {
  double dR = 9999.;
  if( "B"==key || "b"==key ) dR = m_distanceToBQuark;
  if( "C"==key || "c"==key ) dR = m_distanceToCQuark;
  if( "T"==key || "t"==key ) dR = m_distanceToTLepton;
  return dR;
}

}
