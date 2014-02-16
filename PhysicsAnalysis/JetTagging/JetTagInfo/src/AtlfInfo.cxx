/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/AtlfInfo.h"
#
namespace Analysis {

  /** Default constructor */
  AtlfInfo::AtlfInfo() : BaseTagInfo(), 
			 m_isBTagged(false), 
			 m_isTauTagged(false), 
			 m_isTau1PTagged(false), 
			 m_isTau3PTagged(false), 
			 m_calFactLight(1.),
			 m_calFactTau(1.),
			 m_calFactTau1P3P(1.),
			 m_calFactB(1.),
			 m_distanceToBQuark(9999.),
			 m_distanceToCQuark(9999.),
			 m_distanceToTLepton(9999.) { 
    m_tagLikelihood.clear();
  }

  /** constructor with info type */
  AtlfInfo::AtlfInfo(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType), 
						   m_isBTagged(false), 
						   m_isTauTagged(false), 
						   m_isTau1PTagged(false), 
						   m_isTau3PTagged(false), 
						   m_calFactLight(1.),
						   m_calFactTau(1.),
						   m_calFactTau1P3P(1.),
						   m_calFactB(1.),
						   m_distanceToBQuark(9999.),
						   m_distanceToCQuark(9999.),
						   m_distanceToTLepton(9999.) { 
    m_tagLikelihood.clear();
  }

  /** Copy constructor
      not really needed now, but the object might get more complicated ...
  */
  AtlfInfo::AtlfInfo(const AtlfInfo& rhs) : BaseTagInfo(rhs), 
					    m_isBTagged(rhs.m_isBTagged), 
					    m_isTauTagged(rhs.m_isTauTagged), 
					    m_isTau1PTagged(rhs.m_isTau1PTagged), 
					    m_isTau3PTagged(rhs.m_isTau3PTagged), 
					    m_calFactLight(rhs.m_calFactLight),
					    m_calFactTau(rhs.m_calFactTau),
					    m_calFactTau1P3P(rhs.m_calFactTau1P3P),
					    m_calFactB(rhs.m_calFactB),
					    m_distanceToBQuark(rhs.m_distanceToBQuark),
					    m_distanceToCQuark(rhs.m_distanceToCQuark),
					    m_distanceToTLepton(rhs.m_distanceToTLepton) {}

  /** assigenment operator */
  AtlfInfo& AtlfInfo::operator= (const AtlfInfo& rhs)
  {
    if (this!=&rhs)
      {
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
	m_isBTagged=rhs.m_isBTagged;
	m_isTauTagged=rhs.m_isTauTagged;
	m_isTau1PTagged=rhs.m_isTau1PTagged;
	m_isTau3PTagged=rhs.m_isTau3PTagged;
	m_calFactLight=rhs.m_calFactLight;
	m_calFactTau=rhs.m_calFactTau;
	m_calFactTau1P3P=rhs.m_calFactTau1P3P;
	m_calFactB=rhs.m_calFactB;
	m_distanceToBQuark=rhs.m_distanceToBQuark;
	m_distanceToCQuark=rhs.m_distanceToCQuark;
	m_distanceToTLepton=rhs.m_distanceToTLepton;
      }
    return *this;
  }

  /** Default destructor */
  AtlfInfo::~AtlfInfo() {
  }

  void AtlfInfo::deltaRMinTo(const std::string& key, float dR) {
    if ( "B"==key || "b"==key) m_distanceToBQuark  = dR;
    if ( "C"==key || "c"==key) m_distanceToCQuark  = dR;
    if ( "T"==key || "t"==key) m_distanceToTLepton = dR;
  }

  float AtlfInfo::deltaRMinTo(const std::string& key) const {
    float dR = 9999.;
    if ( "B"==key || "b"==key) dR = m_distanceToBQuark;
    if ( "C"==key || "c"==key) dR = m_distanceToCQuark;
    if ( "T"==key || "t"==key) dR = m_distanceToTLepton;
    return dR;
  }
}
