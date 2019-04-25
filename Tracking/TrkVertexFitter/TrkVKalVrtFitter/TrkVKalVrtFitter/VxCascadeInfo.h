/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************

    VxCascadeInfo.h - Description
           -------------------

    begin: June 2008
    author: V.Kostyukhin
    e-mail: vadim.kostyukhin@cern.ch
    changes: new!

 Base VxSecVertexInfo class contains a vector of pointers
 to xAOD::Vertex's of cascade vertices. There all tracking 
 information is kept. VxCascadeInfo class itself contains physics
 momenta and their covariances of particles and pseudoparticles
 in each cascade vertex. Structure of cascade (order of vertices) 
 is the same in vector<xAOD::Vertex*>, vector<vector<TLorentzVector> and
 vector< Amg::MatrixX >.
 
   For vertex iv in VxCascadeInfo:
  
  vertices[iv]     - xAOD::Vertex of fitted vertex
  momenta[iv][...] - TLorentzVectors for all particles in vertex (real+pseudotracks from cascade vertices)
  covariance[iv]   - Amg::MatrixX for given vertex in form (Vx,Vy,Vz,Px1,Py1,Pz1,.....Pxn,Pyn,Pzn)
  
   vector< Amg::MatrixX > may contain also as NVertex_In_Cascade+1 entry (the very last one) the FULL covariance
         matrix of cascade including all vertices and particles. Order of parameters in the same as above and all
	 cross correlations are present. 

********************************************/

#ifndef TRKVKALVRTFITTER_VXCASCADEINFO_H
#define TRKVKALVRTFITTER_VXCASCADEINFO_H

#include "VxSecVertex/VxSecVertexInfo.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include <vector>

namespace Trk {

  //class VxCandidate;

  class VxCascadeInfo : public VxSecVertexInfo {

  public:

    /* default constructor for persistency */
    VxCascadeInfo();

    /* constructor */
    VxCascadeInfo(const std::vector<xAOD::Vertex*> & vertices,
                  const std::vector<std::vector<TLorentzVector> > & moms,
                              const std::vector< Amg::MatrixX >   & covs,
			       int nDoF, double Chi2);


    /* copy constructor */
    VxCascadeInfo(const VxCascadeInfo &);
    VxCascadeInfo(VxCascadeInfo &&) noexcept = default;

    /* clone method */
    //virtual VxCascadeInfo* clone() const;
	
    /* assignment operator */
    VxCascadeInfo operator= (const VxCascadeInfo &);
    VxCascadeInfo& operator= (VxCascadeInfo &&) noexcept = default;

    /* destructor */
    ~VxCascadeInfo();


    /* get list of particle momenta at vertices */
    const std::vector< std::vector<TLorentzVector> >   & getParticleMoms() const  { return m_particleMomAtVertex;}
    const std::vector< Amg::MatrixX >                    & getCovariance() const  { return m_covarianceAtVertex; }
    int                                                  nDoF() const      {     return m_nDoF;     }
    double                                               fitChi2() const   {     return m_fullChi2; }
    void                                                 setFullCascadeCovariance(const Amg::MatrixX &);


  private:

    std::vector< std::vector<TLorentzVector> >    m_particleMomAtVertex;
    std::vector< Amg::MatrixX >                   m_covarianceAtVertex;
    int m_nDoF;
    double m_fullChi2;
 
  };



  /* clone method */
  //inline VxCascadeInfo* VxCascadeInfo::clone() const {
  //  return new VxCascadeInfo(*this);
  //}


}//end namespace Trk

#endif
