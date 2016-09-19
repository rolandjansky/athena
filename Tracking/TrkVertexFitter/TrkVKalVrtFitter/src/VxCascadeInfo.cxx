/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/VxCascadeInfo.h"
//-------------------------------------------------

namespace Trk {



  VxCascadeInfo::~VxCascadeInfo(){}

  VxCascadeInfo::VxCascadeInfo() :  VxSecVertexInfo(), m_nDoF(0), m_fullChi2(0) {}

  VxCascadeInfo::VxCascadeInfo(const std::vector<xAOD::Vertex*> & vertices,
                               const std::vector< std::vector<TLorentzVector> > & moms,
                               const std::vector< Amg::MatrixX > & covs,
			       int nDoF, double Chi2    ) :
    VxSecVertexInfo(vertices)
  {
    m_nDoF=nDoF;   m_fullChi2=Chi2;
    for( int iv=0; iv<(int)moms.size(); iv++ ){
         m_particleMomAtVertex.push_back( moms[iv] );
         m_covarianceAtVertex.push_back( covs[iv] );
    }
  }


  const std::vector< std::vector<TLorentzVector> >   & VxCascadeInfo::getParticleMoms() const 
  {     
/*        std::vector< std::vector<TLorentzVector> >  result;
        for(int iv=0; iv<(int)m_particleMomAtVertex.size(); iv++){
	   std::vector<TLorentzVector> tmpMoms =  m_particleMomAtVertex[iv];
	   result.push_back( tmpMoms );}
        return  result; */
	
	return m_particleMomAtVertex;

  }

  const std::vector< Amg::MatrixX >   & VxCascadeInfo::getCovariance() const 
  {     return m_covarianceAtVertex; }

  int VxCascadeInfo::nDoF() const 
  {     return m_nDoF; }

  double VxCascadeInfo::fitChi2() const 
  {     return m_fullChi2; }


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
