/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignEvent/AlignVertex.h"
#include <iostream>

namespace Trk {

  //________________________________________________________________________
  AlignVertex::AlignVertex() 
    : m_matrix(nullptr)
    , m_vector(nullptr)
    , m_original(nullptr)
    , m_originalPosition(nullptr)
    , m_alignTracks(0)
    , m_position(nullptr)
    , m_covariance(nullptr)
    , m_derivatives(nullptr)
    , m_qMatrix(nullptr)
    , m_vVector(nullptr)
    , m_constrained(false)
    , m_type(Unknown)
  {
    m_vector=new Amg::Vector3D(0.,0.,0.); 
    m_matrix=new AmgSymMatrix(3);
    m_derivatives=new std::vector<AlignModuleVertexDerivatives >(0);
    m_qMatrix= new AmgSymMatrix(3);
    m_vVector=new Amg::Vector3D(0.,0.,0.); 
    m_nTracks=0;
  }
  
  //copy constructor
  AlignVertex::AlignVertex(const AlignVertex & rhs) 
    : m_nTracks(rhs.m_nTracks)
    , m_matrix(new AmgSymMatrix(3)(*(rhs.m_matrix)))
    , m_vector(new Amg::Vector3D(*(rhs.m_vector)))
    , m_original(new xAOD::Vertex(*(rhs.m_original)))
    , m_originalPosition(new Amg::Vector3D( *(rhs.m_originalPosition )))
    , m_alignTracks(rhs.m_alignTracks)
    , m_position(new Amg::Vector3D(*(rhs.m_position)))
    , m_covariance(new AmgSymMatrix(3) (*(rhs.m_covariance)) )
    , m_derivatives(new auto(*(rhs.m_derivatives)))
    , m_qMatrix(new AmgSymMatrix(3) (*(rhs.m_qMatrix)) )
    , m_vVector(new Amg::Vector3D(*(rhs.m_vVector)))
    , m_constrained(rhs.m_constrained)
    , m_type(rhs.m_type)
  {
   
  }
  
  //assignment
  AlignVertex & AlignVertex::operator =(const AlignVertex & rhs) {
    if (&rhs!=this){
      m_nTracks = rhs.m_nTracks;
      delete m_matrix; m_matrix = new AmgSymMatrix(3)(*(rhs.m_matrix));
      delete m_vector; m_vector = new Amg::Vector3D(*(rhs.m_vector));
      delete m_original; m_original = new xAOD::Vertex(*(rhs.m_original));
    	delete m_originalPosition; m_originalPosition = new Amg::Vector3D( *(rhs.m_originalPosition ));
    	m_alignTracks = rhs.m_alignTracks;
    	delete m_position; m_position = new Amg::Vector3D(*(rhs.m_position));
    	delete m_covariance; m_covariance = new AmgSymMatrix(3) (*(rhs.m_covariance));
      delete m_derivatives; m_derivatives = new auto(*(rhs.m_derivatives));
      delete m_qMatrix; m_qMatrix = new AmgSymMatrix(3) (*(rhs.m_qMatrix));
      delete m_vVector; m_vVector = new Amg::Vector3D(*(rhs.m_vVector));
      m_constrained = rhs.m_constrained;
      m_type = rhs.m_type;
  	}
  	return *this;
  }
  
  


  //________________________________________________________________________
  AlignVertex::AlignVertex(const xAOD::Vertex* vertex) 
    : m_matrix(nullptr)
    , m_vector(nullptr)
    , m_original(vertex)
    , m_originalPosition(nullptr)
    , m_alignTracks(0)
    , m_position(nullptr)
    , m_covariance(nullptr)
    , m_derivatives(nullptr)
    , m_qMatrix(nullptr)
    , m_vVector(nullptr)
    , m_constrained(false)
    , m_type(Unknown)
  {
    m_vector=new Amg::Vector3D(0.,0.,0.); 
    m_matrix=new AmgSymMatrix(3);
    m_original = vertex;
    m_originalPosition = new Amg::Vector3D( vertex->position() );
    m_derivatives=new std::vector<AlignModuleVertexDerivatives >(0);
    m_qMatrix=new AmgSymMatrix(3);
    m_vVector=new Amg::Vector3D(0.,0.,0.); 
    m_nTracks=0;
  }


  //________________________________________________________________________
  AlignVertex::~AlignVertex()
  {
    if( m_matrix )           delete m_matrix;
    if( m_vector )           delete m_vector;

    if( m_originalPosition ) delete m_originalPosition;

    if( m_position )         delete m_position;
    if( m_covariance )       delete m_covariance;
    if( m_derivatives )      delete m_derivatives;

    if( m_qMatrix )          delete m_qMatrix;
    if( m_vVector )          delete m_vVector;
  }

  //________________________________________________________________________
  void AlignVertex::setConstraint(AmgSymMatrix(3)* Q, Amg::Vector3D* V) 
  {
    if( Q->rows()!=3 || V->rows()!=3 ) {
      std::cout <<"Unexpected size of the constraint:"<< std::endl;
      std::cout <<"  Q size = "<< Q->rows() << ",   V size = "<< V->rows() << std::endl;
      return;
    }
    if( Q->determinant() < 1.0e-24 ) {
      std::cout <<" Suspicious Q determinant:  detQ = "<< Q->determinant() << std::endl;
      return;
    }
    (*m_qMatrix) = (*Q);
    (*m_vVector) = (*V);
    m_constrained = true;
  }


  //________________________________________________________________________
  void AlignVertex::addAlignTrack(AlignTrack* alignTrack) 
  {
    m_alignTracks.push_back(&alignTrack);
    alignTrack->setVtx(this);
    alignTrack->setRefitD0(false);
    alignTrack->setRefitZ0(false);
    //    alignTrack->setType(AlignTrack::VertexMember);
    m_nTracks++;
  }

  //________________________________________________________________________
  void AlignVertex::addDerivatives(std::vector<AlignModuleVertexDerivatives>* vec)
  {    
    std::vector<AlignModuleVertexDerivatives>::iterator derivIt     = vec->begin();
    std::vector<AlignModuleVertexDerivatives>::iterator derivIt_end = vec->end();

    for ( ; derivIt!=derivIt_end ; ++derivIt) {
      m_derivatives->push_back(*derivIt);
    }

     }

  //________________________________________________________________________
  void AlignVertex::fitVertex()
  {
    
    m_type = Unknown;

    if( Ntracks() < 2 )   return;    // this is not a good vertex!


    if( !m_matrix ) {
      std::cout <<"NULL pointer to the matrix! Bailing out..."<< std::endl;
      return;
    }

    Amg::Vector3D  vec = (*m_vector);
    AmgSymMatrix(3)   cov = (*m_matrix);
    AmgSymMatrix(3)   covcons = (*m_matrix);

    AmgSymMatrix(3)  Qinv;
    if( m_constrained && m_qMatrix->determinant() > 1.0e-24 )   {     // just my guess sigma>0.1 micron ???
      
      bool invertible;
      m_qMatrix->computeInverseWithCheck(Qinv,invertible);
  
      if(!invertible) {
        std::cout <<"fitVertex: Q inversion failed. " << std::endl;
        return;
      }

  
      Amg::Vector3D vtemp(3, 0);
      
      vtemp  = *m_originalPosition;
      vtemp -= *m_vVector;
 
      covcons += 2.0*Qinv;
      vec     += 2.0*Qinv*vtemp;
    }

    // invert the constrained covariance:
    AmgSymMatrix(3)   invcovcons;
    
    bool invertible;
    covcons.computeInverseWithCheck(invcovcons,invertible);

    if(!invertible) {
      std::cout <<"fitVertex: covcons inversion failed. " << std::endl;
      return;
    }
    
    // invert the covariance matrix:
    AmgSymMatrix(3)   invcov;
    cov.computeInverseWithCheck(invcov,invertible);

    if(!invertible) {
      std::cout <<"fitVertex: cov inversion failed. " << std::endl;
      return;
    }



    // calculate corrections    (mind the sign!)
    Amg::Vector3D delta(invcovcons * vec);
 
    if ( !m_position ) m_position=new Amg::Vector3D(0.,0.,0.);   
    *m_position = *m_originalPosition;
    *m_position -= delta;
    

    if ( !m_covariance ) m_covariance= new AmgSymMatrix(3);   
    (*m_covariance)=invcov;                       // this one is unconstrained!

    m_type = Refitted;
 }
  //________________________________________________________________________
  void AlignVertex::dump(MsgStream& msg) 
  {
    msg<<"dumping AlignVertex:  "<<endmsg;

    msg<<" vertex Position:    "<< (*m_position) << endmsg;
    msg<<" vertex Covariance:  "<< (*m_covariance) << endmsg;
    msg<< endmsg;

  }



} // end namespace
