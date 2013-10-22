/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignEvent/AlignVertex.h"
#include <iostream>

namespace Trk {

  //________________________________________________________________________
  AlignVertex::AlignVertex() 
    : theMatrix(0)
    , theVector(0)
    , theOriginal(0)
    , theOriginalPosition(0)
    , theAlignTracks(0)
    , thePosition(0)
    , theCovariance(0)
    , theDerivatives(0)
    , theQmatrix(0)
    , theVvector(0)
    , theConstrained(false)
    , theType(Unknown)
  {
    theVector=new Amg::Vector3D(0.,0.,0.); 
    theMatrix=new AmgSymMatrix(3);
    theDerivatives=new std::vector<AlignModuleVertexDerivatives >(0);
    theQmatrix= new AmgSymMatrix(3);
    theVvector=new Amg::Vector3D(0.,0.,0.); 
    theNtracks=0;
  }

  //________________________________________________________________________
  AlignVertex::AlignVertex(const VxCandidate* vertex) 
    : theMatrix(0)
    , theVector(0)
    , theOriginal(vertex)
    , theOriginalPosition(0)
    , theAlignTracks(0)
    , thePosition(0)
    , theCovariance(0)
    , theDerivatives(0)
    , theQmatrix(0)
    , theVvector(0)
    , theConstrained(false)
    , theType(Unknown)
  {
    theVector=new Amg::Vector3D(0.,0.,0.); 
    theMatrix=new AmgSymMatrix(3);
    theOriginal = vertex;
    theOriginalPosition = new Amg::Vector3D( vertex->recVertex().position() );
    theDerivatives=new std::vector<AlignModuleVertexDerivatives >(0);
    theQmatrix=new AmgSymMatrix(3);
    theVvector=new Amg::Vector3D(0.,0.,0.); 
    theNtracks=0;
  }


  //________________________________________________________________________
  AlignVertex::~AlignVertex()
  {
    if( theMatrix )           delete theMatrix;
    if( theVector )           delete theVector;

    if( theOriginalPosition ) delete theOriginalPosition;

    if( thePosition )         delete thePosition;
    if( theCovariance )       delete theCovariance;
    if( theDerivatives )      delete theDerivatives;

    if( theQmatrix )          delete theQmatrix;
    if( theVvector )          delete theVvector;
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
    (*theQmatrix) = (*Q);
    (*theVvector) = (*V);
    theConstrained = true;
  }


  //________________________________________________________________________
  void AlignVertex::addAlignTrack(AlignTrack* alignTrack) 
  {
    theAlignTracks.push_back(&alignTrack);
    alignTrack->setVtx(this);
    alignTrack->setRefitD0(false);
    alignTrack->setRefitZ0(false);
    //    alignTrack->setType(AlignTrack::VertexMember);
    theNtracks++;
  }

  //________________________________________________________________________
  void AlignVertex::addDerivatives(std::vector<AlignModuleVertexDerivatives>* vec)
  {    
    std::vector<AlignModuleVertexDerivatives>::iterator derivIt     = vec->begin();
    std::vector<AlignModuleVertexDerivatives>::iterator derivIt_end = vec->end();

    for ( ; derivIt!=derivIt_end ; ++derivIt) {
      theDerivatives->push_back(*derivIt);
    }

    return;
  }

  //________________________________________________________________________
  void AlignVertex::fitVertex()
  {
    
    theType = Unknown;

    if( Ntracks() < 2 )   return;    // this is not a good vertex!


    if( !theMatrix ) {
      std::cout <<"NULL pointer to the matrix! Bailing out..."<< std::endl;
      return;
    }

    Amg::Vector3D  vec = (*theVector);
    AmgSymMatrix(3)   cov = (*theMatrix);
    AmgSymMatrix(3)   covcons = (*theMatrix);

    AmgSymMatrix(3)  Qinv;
    if( theConstrained && theQmatrix->determinant() > 1.0e-24 )   {     // just my guess sigma>0.1 micron ???
      
      bool invertible;
      theQmatrix->computeInverseWithCheck(Qinv,invertible);
  
      if(!invertible) {
        std::cout <<"fitVertex: Q inversion failed. " << std::endl;
        return;
      }

  
      Amg::Vector3D vtemp(3, 0);
      
      vtemp  = *theOriginalPosition;
      vtemp -= *theVvector;
 
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
 
    if ( !thePosition ) thePosition=new Amg::Vector3D(0.,0.,0.);   
    *thePosition = *theOriginalPosition;
    *thePosition -= delta;
    

    if ( !theCovariance ) theCovariance= new AmgSymMatrix(3);   
    (*theCovariance)=invcov;                       // this one is unconstrained!

    theType = Refitted;

    return;
  }
  //________________________________________________________________________
  void AlignVertex::dump(MsgStream& msg) 
  {
    msg<<"dumping AlignVertex:  "<<endreq;

    msg<<" vertex Position:    "<< (*thePosition) << endreq;
    msg<<" vertex Covariance:  "<< (*theCovariance) << endreq;
    msg<< endreq;

  }



} // end namespace
