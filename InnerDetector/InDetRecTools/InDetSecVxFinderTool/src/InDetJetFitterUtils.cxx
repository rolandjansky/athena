/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetJetFitterUtils.cxx  -  Description
                             -------------------

    begin : December 2007
    authors: Giacinto Piacquadio (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch
    changes: new!
 
  2007 (c) Atlas Detector Software

  Look at the header file for more information.
     
 ***************************************************************************/

#include "InDetSecVxFinderTool/InDetJetFitterUtils.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/LinearizedTrack.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkEventPrimitives/JacobianPxyzToPhiThetaQoverPspherical.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include <TMath.h>
//#include "TrkExInterfaces/IExtrapolator.h"

//#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"


#if 0
namespace
{
    CLHEP::HepMatrix getPhiThetaQOverPToPxPyPzJacobian(double qOverP,double theta,double phi) {
      CLHEP::HepMatrix transform(3,3,0);
      const double sinTheta(sin(theta));
      const double sinPhi(sin(phi));
      const double cosTheta(cos(theta));
      const double cosPhi(cos(phi));
      const double absQoP(fabs(qOverP));
      //
      transform[0][0]=-1./absQoP*sinTheta*sinPhi;
      transform[0][1]=1./absQoP*sinTheta*cosPhi;
      transform[0][2]=0.;
      transform[1][0]=1./absQoP*cosTheta*cosPhi;
      transform[1][1]=1./absQoP*cosTheta*sinPhi;
      transform[1][2]=-1./absQoP*sinTheta;
      transform[2][0]=-1./qOverP/absQoP*sinTheta*cosPhi; //?? why not just take sign of qOverP
      transform[2][1]=-1./qOverP/absQoP*sinTheta*sinPhi; //??
      transform[2][2]=-1./qOverP/absQoP*cosTheta; //??
      return transform.T();
    }

}
#endif

namespace InDet
{

  InDetJetFitterUtils::InDetJetFitterUtils(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p),
    m_extrapolatorIsAvailable(false),
    m_linearizedTrackFactoryIsAvailable(false)
  { 
    
    declareInterface< InDetJetFitterUtils >(this) ;

  }
  

  InDetJetFitterUtils::~InDetJetFitterUtils() {}
  

  StatusCode InDetJetFitterUtils::initialize() {
    
    
    StatusCode sc = AlgTool::initialize();
    if(sc.isFailure())
      {
	msg(MSG::ERROR) <<" Unable to initialize the AlgTool"<<endmsg;
	return sc;
      }

    if (!m_LinearizedTrackFactory.empty())
    {
      sc=m_LinearizedTrackFactory.retrieve();
      if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not find TrackLinearizer tool." << endmsg;
        return StatusCode::FAILURE;
      }
      else
      {
        m_linearizedTrackFactoryIsAvailable=true;
      }
      
    }
    
    if (!m_extrapolator.empty())
    {
      sc=m_extrapolator.retrieve();
      if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not find Extrapolator tool." << endmsg;
        return StatusCode::FAILURE;
      }
      else
      {
        m_extrapolatorIsAvailable=true;
      }
    }
    
    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  

  StatusCode InDetJetFitterUtils::finalize() {
    
    msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
    
  } 
 
  std::pair<AmgMatrix(3,3),AmgSymMatrix(3)>  InDetJetFitterUtils::getPosMomentumAndMomentumCovMatrix(const Trk::LinearizedTrack* linTrack,
												   const AmgSymMatrix(3) & vrt_cov,
												   const AmgSymMatrix(3) & vrt_weight) const
  {

    const AmgMatrix(5,3) & A = linTrack->positionJacobian();
    const AmgMatrix(5,3) & B = linTrack->momentumJacobian();
    const AmgSymMatrix(5) & trkParametersWeight = linTrack->expectedWeightAtPCA();
    AmgSymMatrix(3) Sm = B.transpose() * trkParametersWeight * B;
    //using determinant as protection - better solution to come...
    Sm = Sm.inverse().eval();   
    if(Sm.determinant()==0){
      msg(MSG::WARNING) << "Inversion of S matrix fails in track refit" << endmsg; 
      msg(MSG::WARNING) << " This track is returned not refitted" << endmsg; 
      throw std::string("Inversion of S matrix fails in track parameters refit"); 
    }
    AmgMatrix(3,3) posMomentumCovariance = -vrt_cov * A.transpose() * trkParametersWeight * B *Sm;
    AmgSymMatrix(3) momentumCovariance = Sm + vrt_weight.similarityT(posMomentumCovariance);
    
    return std::pair<AmgMatrix(3,3),AmgSymMatrix(3)>(posMomentumCovariance,momentumCovariance);
  }  

  // std::pair<double,double> InDetJetFitterUtils::compatibility(const Trk::ParametersBase<5,Trk::Charged> & measPerigee,
  //const Trk::RecVertex & vertex) const
  std::pair<double,double> InDetJetFitterUtils::compatibility(const Trk::TrackParameters & measPerigee,
                                                              const Trk::RecVertex & vertex) const 
  {
    
    if (m_linearizedTrackFactoryIsAvailable==false)
    {
      msg(MSG::ERROR) << " No LinearizedTrackFactory defined. Cannot calculate compatibility. 0 compatibility returned" << endmsg;
      return std::pair<double,double>(0,0);
    }
    

    Trk::LinearizedTrack* myLinearizedTrack=m_LinearizedTrackFactory->linearizedTrack(&measPerigee,vertex.position());
    
    Amg::Vector3D vertexPosition;
    vertexPosition[0]=vertex.position()[0];
    vertexPosition[1]=vertex.position()[1];
    vertexPosition[2]=vertex.position()[2];
    //  CLHEP::HepVector residual=myLinearizedTrack->positionJacobian()*vertexPosition+
    //  myLinearizedTrack->momentumJacobian()*myLinearizedTrack->expectedMomentumAtPCA()+
    //  myLinearizedTrack->constantTerm()-measPerigee.parameters();
    

    const AmgSymMatrix(5) & ExpectedCovariance=myLinearizedTrack->expectedCovarianceAtPCA();
    
    AmgSymMatrix(2) weightReduced=ExpectedCovariance.block<2,2>(0,0);

    AmgSymMatrix(2) errorVertexReduced=vertex.covariancePosition().similarity(myLinearizedTrack->positionJacobian()).block<2,2>(0,0);

    weightReduced+=errorVertexReduced;
    
    weightReduced.inverse().eval();

    //using determinant as protection - better solution to come...
    if(weightReduced.determinant()==0)
    {
      msg(MSG::WARNING) <<  " Problem inverting cov matrix in compatibility method" << endmsg; 
    }
    //double returnv2=weightReduced.similarity(myLinearizedTrack->expectedParametersAtPCA().block<2,2>(0,0));

    Amg::Vector2D paramsReduced((myLinearizedTrack->expectedParametersAtPCA())[0],(myLinearizedTrack->expectedParametersAtPCA())[1]);
    
    double returnv2= paramsReduced.transpose() * weightReduced.inverse() * paramsReduced; 

    delete myLinearizedTrack;
    myLinearizedTrack=0;
    
    return std::pair<double,double>(returnv2,0);
  }
  
  std::pair<double,double> InDetJetFitterUtils::compatibility(const Trk::ParametersBase<5,Trk::Neutral> & measPerigee,
                                                              const Trk::RecVertex & vertex) const
  {

    if (m_linearizedTrackFactoryIsAvailable==false)
    {
      msg(MSG::ERROR) << " No LinearizedTrackFactory defined. Cannot calculate compatibility. 0 compatibility returned" << endmsg;
      return std::pair<double,double>(0,0);
    }


    Trk::LinearizedTrack* myLinearizedTrack=m_LinearizedTrackFactory->linearizedTrack(&measPerigee,vertex.position());
    Amg::Vector3D vertexPosition(3);
    vertexPosition[0]=vertex.position()[0];
    vertexPosition[1]=vertex.position()[1];
    vertexPosition[2]=vertex.position()[2];
    //  CLHEP::HepVector residual=myLinearizedTrack->positionJacobian()*vertexPosition+
    //  myLinearizedTrack->momentumJacobian()*myLinearizedTrack->expectedMomentumAtPCA()+
    //  myLinearizedTrack->constantTerm()-measPerigee.parameters();

    const AmgSymMatrix(5) & ExpectedCovariance=myLinearizedTrack->expectedCovarianceAtPCA();
    
    AmgSymMatrix(2) weightReduced=ExpectedCovariance.block<2,2>(0,0);

    AmgSymMatrix(2) errorVertexReduced=vertex.covariancePosition().similarity(myLinearizedTrack->positionJacobian()).block<2,2>(0,0);

    weightReduced+=errorVertexReduced;
    
     weightReduced.inverse().eval();
     //using determinant as protection - better solution to come...
     if(weightReduced.determinant()==0)
       {
	 msg(MSG::WARNING) <<  " Problem inverting cov matrix in compatibility method" << endmsg; 
       }
     Amg::Vector2D paramsReduced((myLinearizedTrack->expectedParametersAtPCA())[0],(myLinearizedTrack->expectedParametersAtPCA())[1]);
     
     double returnv2= paramsReduced.transpose() * weightReduced * paramsReduced;
    

    delete myLinearizedTrack;
    myLinearizedTrack=0;
    
    return std::pair<double,double>(returnv2,0);
  }
    
        
  double InDetJetFitterUtils::get3DLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                                       const Amg::Vector3D & jetMomentum,
                                                       const Trk::RecVertex & primaryVertex) const
  {
    //the formula in the end is VERY simple!
    //(the calculation to get this result a bit harder!!!)
    //This is also equivalent to the calculaton implemented in the IP3D Tagger 
    //but it is expressed in a MUCH MUCH MUCH simpler way!!!
    const Amg::Vector3D & primaryPos=primaryVertex.position();
    const Amg::Vector3D & trackPos=track.position();
    const Amg::Vector3D & trackMom=track.momentum();
    
    double sign=(jetMomentum.cross(trackMom)).dot(trackMom.cross(primaryPos-trackPos));
    return sign>=0.?1.:-1;
  }
  
  int InDetJetFitterUtils::getTwoTrackVtxCharge(const Trk::VxCandidate & myVxCandidate) const 
  {
    
    //now obtain the daughters of the two vertex
    std::vector<Trk::VxTrackAtVertex*>::const_iterator vtxIter=myVxCandidate.vxTrackAtVertex()->begin();
    
    //obtain first track
    Trk::VxTrackAtVertex* firstTrack(*vtxIter);
    
    //obtain second track
    ++vtxIter;
    Trk::VxTrackAtVertex* secondTrack(*vtxIter);
    
    //now obtain the momentum at the track (refitted)
    const Trk::TrackParameters* firstTrackPerigee=firstTrack->perigeeAtVertex();
    
    //second
    const Trk::TrackParameters* secondTrackPerigee=secondTrack->perigeeAtVertex();
    
    if (firstTrackPerigee==0 ||secondTrackPerigee==0)
    {
      msg(MSG::WARNING) <<  " No Perigee in one of the two tracks at vertex. No sensible charge returned." << endmsg;
      return -100;
    }
    
    return (int)(
        std::floor(
            firstTrackPerigee->parameters()[Trk::qOverP]/
            fabs(firstTrackPerigee->parameters()[Trk::qOverP])+
            secondTrackPerigee->parameters()[Trk::qOverP]/
            fabs(secondTrackPerigee->parameters()[Trk::qOverP])+0.5 )
        );
  }
  

  int InDetJetFitterUtils::getTwoTrackVtxCharge(const xAOD::Vertex & myVxCandidate) const 
  {
    
    //now obtain the daughters of the two vertex
    std::vector<Trk::VxTrackAtVertex>::const_iterator vtxIter=myVxCandidate.vxTrackAtVertex().begin();
    
    //obtain first track
    Trk::VxTrackAtVertex firstTrack(*vtxIter);
    
    //obtain second track
    ++vtxIter;
    Trk::VxTrackAtVertex secondTrack(*vtxIter);
    
    //now obtain the momentum at the track (refitted)
    const Trk::TrackParameters* firstTrackPerigee=firstTrack.perigeeAtVertex();
    
    //second
    const Trk::TrackParameters* secondTrackPerigee=secondTrack.perigeeAtVertex();
    
    if (firstTrackPerigee==0 ||secondTrackPerigee==0)
    {
      msg(MSG::WARNING) <<  " No Perigee in one of the two tracks at vertex. No sensible charge returned." << endmsg;
      return -100;
    }
    
    return (int)(
        std::floor(
            firstTrackPerigee->parameters()[Trk::qOverP]/
            fabs(firstTrackPerigee->parameters()[Trk::qOverP])+
            secondTrackPerigee->parameters()[Trk::qOverP]/
            fabs(secondTrackPerigee->parameters()[Trk::qOverP])+0.5 )
        );
  }
  
  
  double InDetJetFitterUtils::getTwoTrackVtxMass(const Trk::VxCandidate & myVxCandidate,
                                                 double highestMomMass,
                                                 double lowestMomMass) const
  {
    
    
    //now obtain the daughters of the two vertex
    std::vector<Trk::VxTrackAtVertex*>::const_iterator vtxIter=myVxCandidate.vxTrackAtVertex()->begin();
    
    //obtain first track
    Trk::VxTrackAtVertex* firstTrack(*vtxIter);
    
    //obtain second track
    ++vtxIter;
    Trk::VxTrackAtVertex* secondTrack(*vtxIter);
    
    //now obtain the momentum at the track (refitted)
    const Trk::TrackParameters* firstTrackPerigee=firstTrack->perigeeAtVertex();
    
    //second
    const Trk::TrackParameters* secondTrackPerigee=secondTrack->perigeeAtVertex();
    
    if (firstTrackPerigee==0 ||secondTrackPerigee==0)
    {
      msg(MSG::WARNING) <<  " No Perigee in one of the two tracks at vertex. No sensible mass returned." << endmsg;
      return -100;
    }
    
    Amg::Vector3D firstMomentum=firstTrackPerigee->momentum();
    Amg::Vector3D secondMomentum=secondTrackPerigee->momentum();
    
    
    CLHEP::HepLorentzVector first4Mom;
    CLHEP::HepLorentzVector second4Mom;
    
    if (firstMomentum.mag2()>secondMomentum.mag2())
    {
      first4Mom=CLHEP::HepLorentzVector(firstMomentum.x(),firstMomentum.y(),firstMomentum.z(),TMath::Sqrt(highestMomMass*highestMomMass+firstMomentum.mag()*firstMomentum.mag()));
      second4Mom=CLHEP::HepLorentzVector(secondMomentum.x(),secondMomentum.y(),secondMomentum.z(),TMath::Sqrt(lowestMomMass*lowestMomMass+secondMomentum.mag()*secondMomentum.mag()));
    }
    else
    {
      first4Mom=CLHEP::HepLorentzVector(firstMomentum.x(),firstMomentum.y(),firstMomentum.z(),TMath::Sqrt(lowestMomMass*highestMomMass+firstMomentum.mag()*firstMomentum.mag()));
      second4Mom=CLHEP::HepLorentzVector(secondMomentum.x(),secondMomentum.y(),secondMomentum.z(),TMath::Sqrt(highestMomMass*lowestMomMass+secondMomentum.mag()*secondMomentum.mag()));
    }
    
    return (first4Mom+second4Mom).mag();
  }



  
  double InDetJetFitterUtils::getTwoTrackVtxMass(const xAOD::Vertex & myVxCandidate,
                                                 double highestMomMass,
                                                 double lowestMomMass) const
  {
    
    
    //now obtain the daughters of the two vertex
    std::vector<Trk::VxTrackAtVertex>::const_iterator vtxIter=myVxCandidate.vxTrackAtVertex().begin();
    
    //obtain first track
    Trk::VxTrackAtVertex firstTrack(*vtxIter);
    
    //obtain second track
    ++vtxIter;
    Trk::VxTrackAtVertex secondTrack(*vtxIter);
    
    //now obtain the momentum at the track (refitted)
    const Trk::TrackParameters* firstTrackPerigee=firstTrack.perigeeAtVertex();
    
    //second
    const Trk::TrackParameters* secondTrackPerigee=secondTrack.perigeeAtVertex();
    
    if (firstTrackPerigee==0 ||secondTrackPerigee==0)
    {
      msg(MSG::WARNING) <<  " No Perigee in one of the two tracks at vertex. No sensible mass returned." << endmsg;
      return -100;
    }
    
    Amg::Vector3D firstMomentum=firstTrackPerigee->momentum();
    Amg::Vector3D secondMomentum=secondTrackPerigee->momentum();
    
    
    CLHEP::HepLorentzVector first4Mom;
    CLHEP::HepLorentzVector second4Mom;
    
    if (firstMomentum.mag2()>secondMomentum.mag2())
    {
      first4Mom=CLHEP::HepLorentzVector(firstMomentum.x(),firstMomentum.y(),firstMomentum.z(),TMath::Sqrt(highestMomMass*highestMomMass+firstMomentum.mag()*firstMomentum.mag()));
      second4Mom=CLHEP::HepLorentzVector(secondMomentum.x(),secondMomentum.y(),secondMomentum.z(),TMath::Sqrt(lowestMomMass*lowestMomMass+secondMomentum.mag()*secondMomentum.mag()));
    }
    else
    {
      first4Mom=CLHEP::HepLorentzVector(firstMomentum.x(),firstMomentum.y(),firstMomentum.z(),TMath::Sqrt(lowestMomMass*highestMomMass+firstMomentum.mag()*firstMomentum.mag()));
      second4Mom=CLHEP::HepLorentzVector(secondMomentum.x(),secondMomentum.y(),secondMomentum.z(),TMath::Sqrt(highestMomMass*lowestMomMass+secondMomentum.mag()*secondMomentum.mag()));
    }
    
    return (first4Mom+second4Mom).mag();
  }

  

  std::pair<double,double> InDetJetFitterUtils::getDistanceAndErrorBetweenTwoVertices(const xAOD::Vertex & first ,
                                                                                      const Trk::RecVertex & second) const
  {
    
    Amg::Vector3D difference=second.position()-first.position();

    double distance=difference.mag();
    
    AmgSymMatrix(3) sumErrorsThenInverted=first.covariancePosition()+
	second.covariancePosition();
    
    
    sumErrorsThenInverted.inverse().eval();
    //using determinant as protection - better solution to come...
    Amg::Vector3D differenceUnit(difference);
    differenceUnit.normalize();
    
    double error=1000;

    if (sumErrorsThenInverted.determinant()>0 && distance>0)
    {
      double temp=differenceUnit.transpose() * sumErrorsThenInverted * differenceUnit;
      if (temp>0)
      {
        error=1./std::sqrt(temp  );
      }
      else
      {
        msg(MSG::WARNING) << " The significance of the distance to the PV is negative or zero definite: " << endmsg;
	//MU	msg(MSG::WARNING) << std::scientific << temp << " two-trk vertex : " << first << " PV " << second << std::fixed << endmsg;
      }
    }
    else
    {
      if (sumErrorsThenInverted.determinant()<=0)
      {
        msg(MSG::WARNING) <<  " Sum of cov matrices of PV + single vertex fit is zero or negative. Error on distance is returned as 1000mm." << endmsg; 
      }
      else
      {
        msg(MSG::DEBUG) << "The distance between the vertices is: " << endmsg;
      }
    }
    return std::pair<double,double>(distance,error);
  }
  
  std::pair<double,double> InDetJetFitterUtils::getD0andZ0IP(const Trk::TrackParameters & trackPerigee,
                                                             const Trk::Vertex & vertexToExtrapolateTo) const
  {
    
    if (m_linearizedTrackFactoryIsAvailable==false)
    {
      msg(MSG::ERROR) << "Cannot perform requested extrapolation. No extrapolator defined...Returning 0 compatibility..." << endmsg;
      return std::pair<double,double>(0,0);
    }
    

    Trk::PerigeeSurface mySurface(vertexToExtrapolateTo.position());
    const Trk::TrackParameters* newMeasPerigee= m_extrapolator->extrapolateDirectly(trackPerigee,mySurface);
    if (newMeasPerigee==0)
    {
      msg(MSG::WARNING) <<  " Extrapolation failed. Wrong d0 and z0 returned " << endmsg;
      return std::pair<double,double>
          (trackPerigee.parameters()[Trk::d0],
           trackPerigee.parameters()[Trk::z0]*sin(trackPerigee.parameters()[Trk::theta]));
    }
    
    double IPd0=newMeasPerigee->parameters()[Trk::d0];
    double IPz0=newMeasPerigee->parameters()[Trk::z0]*
        sin(newMeasPerigee->parameters()[Trk::theta]);

    delete newMeasPerigee;
    newMeasPerigee=0;
    
    return std::pair<double,double>(IPd0,IPz0);
  }
    

  std::pair<double,double> InDetJetFitterUtils::getD0andZ0IPSig(const Trk::TrackParameters & trackPerigee,
								const Trk::RecVertex & vertex) const
  {
    
    if (m_linearizedTrackFactoryIsAvailable==false)
    {
      ATH_MSG_ERROR( "Cannot perform requested extrapolation. No extrapolator defined...Returning 0 compatibility..." );
      return std::pair<double,double>(0,0);
    }
    
    Trk::LinearizedTrack* myLinearizedTrack=m_LinearizedTrackFactory->linearizedTrack(&trackPerigee,vertex.position());
    Amg::Vector3D vertexPosition;
    vertexPosition[0]=vertex.position()[0];
    vertexPosition[1]=vertex.position()[1];
    vertexPosition[2]=vertex.position()[2];
    
    const AmgSymMatrix(5) & ExpectedCovariance=myLinearizedTrack->expectedCovarianceAtPCA();
    AmgSymMatrix(2) weightReduced=ExpectedCovariance.block<2,2>(0,0);
    //AmgSymMatrix(2) errorVertexReduced=vertex.covariancePosition().similarity(myLinearizedTrack->positionJacobian()).block<2,2>(0,0);
    //weightReduced+=errorVertexReduced;

    double IPd0Sig=(myLinearizedTrack->expectedParametersAtPCA())[0]/sqrt( weightReduced(0,0) );
    double IPz0Sig=(myLinearizedTrack->expectedParametersAtPCA())[1]/sqrt( weightReduced(1,1) );
   
    /*
    std::cout << " " << std::endl;
    std::cout << " --> ExpectedCovariance : " << sqrt( weightReduced(0,0)) << " , " << sqrt( weightReduced(1,1)) << std::endl;
    std::cout << " --> Covarianceposition : " << sqrt(vertex.covariancePosition()(0,0)) << " , " << sqrt(vertex.covariancePosition()(1,1)) << std::endl;
    std::cout << " --> d0/z0              : " << myLinearizedTrack->expectedParametersAtPCA()[0] << " , " << myLinearizedTrack->expectedParametersAtPCA()[1] << std::endl;
    std::cout << " --> d0Sig/z0Sig        : " << IPd0Sig << " , " << IPz0Sig << std::endl;
    */    

    delete myLinearizedTrack;
    myLinearizedTrack=0;
        
    return std::pair<double,double>(IPd0Sig,IPz0Sig);
  }
    


  const Trk::LinkToTrackParticleBase* InDetJetFitterUtils::findNeutralTrackParticleBase(const std::vector<const Trk::LinkToTrackParticleBase*> & /*neutralTracks*/,
                                                                                        const xAOD::Vertex & /*myVxCandidate*/) const 
  {
    //THIS WILL ANYWAY NOT WORK WITH NEW EDM! NEEDS TO BE FIXED!
    /*    
    std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator neutralsBegin=neutralTracks.begin();
    std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator neutralsEnd=neutralTracks.end();
    
    for (std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator neutralsIter=neutralsBegin;
         neutralsIter!=neutralsEnd;++neutralsIter)
    {
      if ((****neutralsIter).reconstructedVertex()==&myVxCandidate)
      {
        return *neutralsIter;
      }
    }
    //if not found, returns 0
    */
    return 0;
  }
  
  bool InDetJetFitterUtils::checkIfTrackIsInVector(const Trk::ITrackLink * trackToCheck,
                                                   const std::vector<const Trk::ITrackLink*> & vectorOfTracks) const 
  {
    
    std::vector<const Trk::ITrackLink*>::const_iterator vectorOfTracksBegin=vectorOfTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator vectorOfTracksEnd=vectorOfTracks.end();

    for (std::vector<const Trk::ITrackLink*>::const_iterator vectorOfTracksIter=vectorOfTracksBegin;
         vectorOfTracksIter!=vectorOfTracksEnd;++vectorOfTracksIter) 
    {    
      if (*vectorOfTracksIter==trackToCheck)
      {
        return true;
      }
    }
    return false;
  }
  
  bool InDetJetFitterUtils::checkIfTrackIsInVector(const Trk::ITrackLink * trackToCheck,
                                                   const std::vector<const Trk::LinkToTrackParticleBase*> & vectorOfTracks) const 
  {
    
    std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator vectorOfTracksBegin=vectorOfTracks.begin();
    std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator vectorOfTracksEnd=vectorOfTracks.end();

    for (std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator vectorOfTracksIter=vectorOfTracksBegin;
         vectorOfTracksIter!=vectorOfTracksEnd;++vectorOfTracksIter) 
    {    
      if (*vectorOfTracksIter==trackToCheck)
      {
        return true;
      }
    }
    return false;
  }

  bool InDetJetFitterUtils::checkIfTrackIsInNeutralTrackVector(const Trk::ITrackLink * /*trackToCheck*/,
                                                               const std::vector<const Trk::LinkToTrackParticleBase*> & /*vectorOfNeutrals*/) const
  {
    //W
    /*
    std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator vectorOfNeutralsBegin=vectorOfNeutrals.begin();
    std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator vectorOfNeutralsEnd=vectorOfNeutrals.end();
    
    for (std::vector<const Trk::LinkToTrackParticleBase*>::const_iterator vectorOfNeutralsIter=vectorOfNeutralsBegin;
         vectorOfNeutralsIter!=vectorOfNeutralsEnd;
         ++vectorOfNeutralsIter)
    {
      const Trk::LinkToTrackParticleBase* myTPBlink=*vectorOfNeutralsIter;

      if (myTPBlink==0)
      {
        msg(MSG::WARNING) <<  " null pointer (TPBlink). Skipping neutral candidate... " << endmsg;
        continue;
      }

      const Trk::TrackParticleBase* myTPB=**myTPBlink;
      
      if (myTPB==0)
      {
        msg(MSG::WARNING) <<  " null pointer (TPB). Skipping neutral candidate... " << endmsg;
        continue;
      }
      
      const Trk::VxCandidate* myV0Candidate=myTPB->reconstructedVertex();
      
      if (myV0Candidate==0) 
      {
        msg(MSG::WARNING) << " neutral TP Base has no original Vx Candidate " << endmsg;
        continue;
      }
      
      const Trk::VxTrackAtVertex* firstTrack((*(myV0Candidate->vxTrackAtVertex()))[0]);
      const Trk::VxTrackAtVertex* secondTrack((*(myV0Candidate->vxTrackAtVertex()))[1]);
      
      const Trk::ITrackLink* trackLink1=firstTrack->trackOrParticleLink();
      const Trk::ITrackLink* trackLink2=secondTrack->trackOrParticleLink();
      
      if (trackLink1==trackToCheck || trackLink2==trackToCheck )
      {
        return true;
      }
    }
    */
    return false;
  }
  
  
  bool InDetJetFitterUtils::checkIfVxCandidateIsInVector(const xAOD::Vertex * vertexToCheck,
                                                         const std::vector<const xAOD::Vertex*> & vectorOfCandidates) const
  {

    std::vector<const xAOD::Vertex*>::const_iterator vectorOfCandidatesBegin=vectorOfCandidates.begin();
    std::vector<const xAOD::Vertex*>::const_iterator vectorOfCandidatesEnd=vectorOfCandidates.end();
    
    for (std::vector<const xAOD::Vertex*>::const_iterator vectorOfCandidatesIter=vectorOfCandidatesBegin;
         vectorOfCandidatesIter!=vectorOfCandidatesEnd;
         ++vectorOfCandidatesIter)
    {
      if (*vectorOfCandidatesIter==vertexToCheck)
      {
        return true;
      }
    }
    return false;
  }
  
  bool InDetJetFitterUtils::checkIfTrackIsInV0CandidatesVector(const Trk::ITrackLink * trackToCheck,
                                                               const std::vector<const xAOD::Vertex*> & vectorOfVxCandidates) const 
  {
    
    std::vector<const xAOD::Vertex*>::const_iterator verticesToVetoBegin=vectorOfVxCandidates.begin();
    std::vector<const xAOD::Vertex*>::const_iterator verticesToVetoEnd=vectorOfVxCandidates.end();
    
    for (std::vector<const xAOD::Vertex*>::const_iterator verticesToVetoIter=verticesToVetoBegin;
         verticesToVetoIter!=verticesToVetoEnd;++verticesToVetoIter) 
    {
      
      Trk::VxTrackAtVertex  firstTrack((((*verticesToVetoIter)->vxTrackAtVertex()))[0]);
      Trk::VxTrackAtVertex secondTrack((((*verticesToVetoIter)->vxTrackAtVertex()))[1]);
    
      const Trk::ITrackLink* linkToTP1= firstTrack.trackOrParticleLink();
      const Trk::ITrackLink* linkToTP2=secondTrack.trackOrParticleLink();

      if (trackToCheck==linkToTP1||
          trackToCheck==linkToTP2)
      {
        return true;
      }
    }
    
    return false;
    
  }
  
  CLHEP::HepLorentzVector  InDetJetFitterUtils::fourMomentumAtVertex(const Trk::VxVertexOnJetAxis & myVxVertexOnJetAxis) const
  {
    
 
    const double s_pion=139.57018;
    //hard coded pion mass
 
    CLHEP::HepLorentzVector massVector(0,0,0,0);
 
    const std::vector<Trk::VxTrackAtVertex*> & tracksOfVertex=myVxVertexOnJetAxis.getTracksAtVertex();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackBegin=tracksOfVertex.begin();
    std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackEnd=tracksOfVertex.end();
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator clustersOfTrackIter=clustersOfTrackBegin;
         clustersOfTrackIter!=clustersOfTrackEnd;
         ++clustersOfTrackIter)
    {
      if (dynamic_cast<const Trk::Perigee*>((*clustersOfTrackIter)->perigeeAtVertex())!=0)
      {
        
        const Trk::TrackParameters* aMeasPer=(*clustersOfTrackIter)->perigeeAtVertex();
        Amg::Vector3D mytrack(aMeasPer->momentum());
        massVector+=CLHEP::HepLorentzVector(mytrack.x(),mytrack.y(),mytrack.z(),TMath::Sqrt(s_pion*s_pion+mytrack.mag()*mytrack.mag()));
      }
    }
   
    return massVector;
  }
  
 
}
//end namespace InDet
