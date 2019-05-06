/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/VxCascadeInfo.h"
//-------------------------------------------------

namespace Trk {



  VxCascadeInfo::~VxCascadeInfo(){}

  VxCascadeInfo::VxCascadeInfo() :  VxSecVertexInfo(), m_nDoF(0), m_fullChi2(0) {}

  VxCascadeInfo::VxCascadeInfo(std::vector<xAOD::Vertex*> vertices,
                               std::vector< std::vector<TLorentzVector> > moms,
                               std::vector< Amg::MatrixX >  covs,
			       int nDoF, double Chi2    ) :
    VxSecVertexInfo(std::move(vertices)), m_particleMomAtVertex(std::move(moms)),
     m_covarianceAtVertex(std::move(covs)),  m_nDoF(nDoF), m_fullChi2(Chi2)
  {
  }



  void VxCascadeInfo::setFullCascadeCovariance(const Amg::MatrixX & COV) 
  {
     int NV=m_particleMomAtVertex.size(); //Cascade size for control
     while( (int) m_covarianceAtVertex.size() > NV ) m_covarianceAtVertex.pop_back(); //remove previous full matrices 
     m_covarianceAtVertex.push_back( COV );
  }

  VxCascadeInfo::VxCascadeInfo(const VxCascadeInfo & rhs):
    VxSecVertexInfo(rhs),
    m_particleMomAtVertex(rhs.m_particleMomAtVertex),
    m_covarianceAtVertex(rhs.m_covarianceAtVertex),
    m_nDoF(rhs.m_nDoF),
    m_fullChi2(rhs.m_fullChi2)
  {}


} // End Of Namespace
