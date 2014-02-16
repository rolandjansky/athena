/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/QGPartonTruthInfo.h"
#include <algorithm>
#include <utility>

namespace Analysis
{

/** Default constructor */
QGPartonTruthInfo::QGPartonTruthInfo()
  : BaseTagInfo(),
    m_jetTruthLabel("N/A"),
    m_distanceToParton (999.)
{}

/** constructor with info type */
QGPartonTruthInfo::QGPartonTruthInfo(TagInfoType tagJetInfoType)
  : BaseTagInfo(tagJetInfoType),
    m_jetTruthLabel("N/A"),
    m_distanceToParton (999.)
{}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
QGPartonTruthInfo::QGPartonTruthInfo(const QGPartonTruthInfo& rhs) 
  : BaseTagInfo(rhs),
    m_jetTruthLabel(rhs.m_jetTruthLabel),
    m_distanceToParton(rhs.m_distanceToParton)
{}

/** assigenment operator */
QGPartonTruthInfo& QGPartonTruthInfo::operator= (const QGPartonTruthInfo& rhs)
{
    if (this!=&rhs)
    {
        m_jetTruthLabel=rhs.m_jetTruthLabel;
	m_distanceToParton=rhs.m_distanceToParton;
    }
    return *this;
}

/** Default destructor */
QGPartonTruthInfo::~QGPartonTruthInfo()
{}

void QGPartonTruthInfo::deltaRToParton(double dR) {
  m_distanceToParton = dR;
}

double QGPartonTruthInfo::deltaRToParton() const {
  double dR = 9999.;
  dR = m_distanceToParton;
  return dR;
}

}
