/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SVInfoPlus.h"

namespace Analysis
{

/** Default constructor */
SVInfoPlus::SVInfoPlus() : BaseTagInfo(),
		   m_recsvx(Trk::RecVertex()),
		   m_NGTinJet(-1),
		   m_NGTinSvx(-1),
		   m_N2Tpair(-1),
		   m_masssvx(-1.),
		   m_efracsvx(-1.),
		   m_normdist(-1.),
		   m_trackinfo(std::vector<SVTrackInfo>()) {}

/** constructor with info type */
SVInfoPlus::SVInfoPlus(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
  m_recsvx(Trk::RecVertex()),
  m_NGTinJet(-1),
  m_NGTinSvx(-1),
  m_N2Tpair(-1),
  m_masssvx(-1.),
  m_efracsvx(-1.),
  m_normdist(-1.),
  m_trackinfo(std::vector<SVTrackInfo>())
{
    m_tagLikelihood.clear();
}

/** Copy constructor
  not really needed now, but the object might get more complicated ...
*/
SVInfoPlus::SVInfoPlus(const SVInfoPlus& rhs) : BaseTagInfo(rhs),
  m_recsvx(rhs.m_recsvx), 
  m_NGTinJet(rhs.m_NGTinJet),
  m_NGTinSvx(rhs.m_NGTinSvx),
  m_N2Tpair(rhs.m_N2Tpair),
  m_masssvx(rhs.m_masssvx),
  m_efracsvx(rhs.m_efracsvx),
  m_normdist(rhs.m_normdist),
  m_trackinfo(rhs.m_trackinfo)
{}

/** assigenment operator */
SVInfoPlus& SVInfoPlus::operator= (const SVInfoPlus& rhs)
{
    if (this!=&rhs)
    {
        m_tagLikelihood=rhs.m_tagLikelihood;
	m_recsvx   = rhs.m_recsvx;  
        m_NGTinJet = rhs.m_NGTinJet;
	m_NGTinSvx = rhs.m_NGTinSvx;
	m_N2Tpair  = rhs.m_N2Tpair;
	m_masssvx  = rhs.m_masssvx;
	m_efracsvx = rhs.m_efracsvx;
	m_normdist = rhs.m_normdist;
        m_trackinfo = rhs.m_trackinfo;
        m_tagJetInfoType=rhs.m_tagJetInfoType;
    }
    return *this;
}

/** Default destructor */
SVInfoPlus::~SVInfoPlus()
{}

  float SVInfoPlus::getChi2Compatibility(const Trk::RecVertex priVertex, const Trk::RecVertex secVertex, const Amg::Vector3D jetDirection) const {
    // calculates the chi2 compatibility between the primary and secondary vertex.
    // this used to be the default tag weight until January 2010.
    
    Eigen::Vector3d Diff;
    Diff[0]=secVertex.position().x()-priVertex.position().x();
    Diff[1]=secVertex.position().y()-priVertex.position().y();
    Diff[2]=secVertex.position().z()-priVertex.position().z();
    
    AmgSymMatrix(3) covariance = secVertex.covariancePosition() + priVertex.covariancePosition();

    Eigen::Matrix3d weight;
    bool invertible;
    covariance.computeInverseWithCheck(weight, invertible);
    if (not invertible) {
	return 0;
    }
    
    Amg::Vector3D differenceNorm(Diff);
    differenceNorm.normalize(); 
    Amg::Vector3D jetDirectionNorm(jetDirection);
    jetDirectionNorm.normalize();

    double weight_similarity  = Diff.transpose()*weight*Diff;
    
    return std::sqrt(weight_similarity)*differenceNorm.dot(jetDirectionNorm);
    

  }
  
}

