/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          ImpactPoint3dEstimator.cxx - Description in header file
*********************************************************************/

#include "ImpactPoint3dEstimator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

// #define IMPACTPOINT3DESTIMATOR_DEBUG

//added for cuts in case of displaced vertex
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/PerigeeSurface.h"

#include <math.h>

namespace Trk
{

  ImpactPoint3dEstimator::ImpactPoint3dEstimator(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_extrapolator("Trk::Extrapolator"),
    m_magFieldSvc("AtlasFieldSvc", n),
    m_maxiterations(20),
    m_precision(1e-10),//DeltaPhi
//    m_planeSurface(0),
    m_vertex(0),
    m_distance(0.)
  {   
    declareProperty("Extrapolator",m_extrapolator);
    declareProperty("MagFieldSvc",     m_magFieldSvc);
    declareProperty("MaxIterations",m_maxiterations);
    declareProperty("Precision",m_precision);
    declareInterface<IImpactPoint3dEstimator>(this);
  }
  
  ImpactPoint3dEstimator::~ImpactPoint3dEstimator() {
//    if (m_planeSurface!=0) delete m_planeSurface;
    if (m_vertex!=0) delete m_vertex;
  }
  
  StatusCode ImpactPoint3dEstimator::initialize() 
  { 
    if ( m_extrapolator.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endreq;
      return StatusCode::FAILURE;
    }

    if (m_magFieldSvc.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Could not find magnetic field service." << endreq;
      return StatusCode::FAILURE;
    }

    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }
  
  StatusCode ImpactPoint3dEstimator::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }


  template<typename T> PlaneSurface* ImpactPoint3dEstimator::_Estimate3dIPNoCurvature(const T* thePerigee,const Amg::Vector3D* theVertex) const  {

      const Amg::Vector3D  momentumUnit = thePerigee->momentum().unit();
      double pathLength  =  ( *theVertex  - thePerigee->position() ).dot( momentumUnit )
                                                                       / (  momentumUnit.dot( momentumUnit )) ;
       //first vector at 3d impact point


      Amg::Vector3D POCA   =  thePerigee->position()  + pathLength * momentumUnit;// Position of closest approach
      Amg::Vector3D DeltaR =  *theVertex  - POCA;
      m_distance=DeltaR.mag();
      DeltaR=DeltaR.unit();


      //correct DeltaR from small deviations from orthogonality to DeltaR -- DeltaR.dot(momentumUnit) should equal 0 if the above is correct

      Amg::Vector3D DeltaRcorrected=DeltaR-(DeltaR.dot(momentumUnit))*momentumUnit;

      if ((DeltaR-DeltaRcorrected).mag()>1e-4)
      {
        ATH_MSG_WARNING( " DeltaR and MomentumDir are not orthogonal " );
        ATH_MSG_DEBUG( std::setprecision(10) << " DeltaR-DeltaRcorrected: "  << (DeltaR-DeltaRcorrected).mag() );
      }

      Amg::Vector3D YDir=momentumUnit.cross(DeltaRcorrected);

      //store the impact 3d point
      m_vertex=new Amg::Vector3D( POCA );

      ATH_MSG_VERBOSE( "final minimal distance is: " << m_distance);

      ATH_MSG_DEBUG( "POCA in 3D is: " << *m_vertex );

      //store the plane...
      ATH_MSG_VERBOSE( "plane to which to extrapolate X " << DeltaRcorrected << " Y " << YDir << " Z " << momentumUnit);

      Amg::Transform3D* thePlane = new Amg::Transform3D(DeltaRcorrected, YDir, momentumUnit, *theVertex);

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG
      std::cout << "the translation is, directly from Transform3d: " << thePlane.getTranslation() << endreq;
#endif

      return new PlaneSurface(thePlane);

  }

  PlaneSurface* ImpactPoint3dEstimator::Estimate3dIP(const NeutralParameters* neutralPerigee,const Amg::Vector3D* theVertex) const {
    // clean up before any sanity checks so a return 0 corresponds to internal members reset too
    if (m_vertex!=0) {
      delete m_vertex;
      m_vertex=0;
    }

    ATH_MSG_DEBUG("Neutral particle --  propergate like a straight line");
    return _Estimate3dIPNoCurvature(neutralPerigee, theVertex);
  }

  PlaneSurface* ImpactPoint3dEstimator::Estimate3dIP(const TrackParameters* trackPerigee,const Amg::Vector3D* theVertex) const
  {
    // clean up before any sanity checks so a return 0 corresponds to internal members reset too
    if (m_vertex!=0) {
      delete m_vertex;
      m_vertex=0;
    }

    double magnFieldVect[3];
    m_magFieldSvc->getField(trackPerigee->associatedSurface().center().data(),magnFieldVect);
    if(magnFieldVect[2] == 0 ){
      ATH_MSG_DEBUG("Magnetic field in the Z dierction is 0 --  propergate like a straight line");
      return _Estimate3dIPNoCurvature(trackPerigee, theVertex);
    }


    const Trk::Perigee* thePerigee=dynamic_cast<const Trk::Perigee*>(trackPerigee);
    if (thePerigee==0)
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " ImpactPoint3dEstimator didn't get a Perigee* as ParametersBase*: cast not possible. Need to EXTRAPOLATE..." << endreq;
      
      /* run-1 code had not been adapted for Neutral parameters:
      const TrackParameters* ch_params = dynamic_cast<const TrackParameters*>(trackPerigee);
      if (ch_params == 0) {
        msg(MSG::ERROR) << " Cannot cast to charged track parameters. Neutrals are not supported... " << endreq;
        return 0;
      }*/

      Trk::PerigeeSurface perigeeSurface(*theVertex);
      thePerigee=dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(*trackPerigee,
                                                                                       perigeeSurface));
      if (thePerigee == NULL) return 0;
    }

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)<< " Now running ImpactPoint3dEstimator::Estimate3dIP" << endreq;
    double phi0=thePerigee->parameters()[Trk::phi0];
    double cosphi0=-sin(phi0);
    double sinphi0=cos(phi0);
    double theta=thePerigee->parameters()[Trk::theta];
    double cottheta=1./tan(thePerigee->parameters()[Trk::theta]);
    double d0=thePerigee->parameters()[Trk::d0];

    //I need the radius (magnetic field...)
    double Bz=magnFieldVect[2]*299.792;
    double Rt=sin(theta)/(Bz*thePerigee->parameters()[Trk::qOverP]);

    double x0=thePerigee->associatedSurface().center().x()+(d0-Rt)*cosphi0;
    double y0=thePerigee->associatedSurface().center().y()+(d0-Rt)*sinphi0;
    double z0=thePerigee->associatedSurface().center().z()+thePerigee->parameters()[Trk::z0]+Rt*phi0*cottheta;

    if (thePerigee!=trackPerigee) {
      delete thePerigee;
      thePerigee=0;
    }

    double xc=theVertex->x();
    double yc=theVertex->y();
    double zc=theVertex->z();

    double phiactual=phi0;
    double cosphiactual=-sin(phiactual);
    double sinphiactual=cos(phiactual);

    double secderivative=0.;
    double derivative=0.;

    int ncycle=0;
    bool isok=false;

    double deltaphi=0.;

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG
    std::cout << std::setprecision(25) << "actual distance  before cycle is: " << std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
                                                                                            std::pow(y0-yc+Rt*sinphiactual,2)+
                                                                                            std::pow(z0-zc-Rt*cottheta*phiactual,2)) << std::endl;
#endif

    do {

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG
      msg(MSG::VERBOSE)<< "Cycle number: " << ncycle << " old phi: " << phiactual << endreq;
#endif
      //      distance=std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
      //                 std::pow(y0-yc+Rt*sinphiactual,2)+
      //                 std::pow(z0-zc-Rt*cottheta*phiactual));

      derivative=(x0-xc)*(-Rt*sinphiactual)+(y0-yc)*Rt*cosphiactual+(z0-zc-Rt*phiactual*cottheta)*(-Rt*cottheta);
      secderivative=Rt*(-(x0-xc)*cosphiactual-(y0-yc)*sinphiactual+Rt*cottheta*cottheta);
#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
      msg(MSG::VERBOSE)<< "derivative is: " << derivative << " sec derivative is: " << secderivative << endreq;
#endif

      deltaphi=-derivative/secderivative;

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
      std::cout << std::setprecision(25) << "deltaphi: " << deltaphi << std::endl;
#endif

      phiactual+=deltaphi;
      cosphiactual=-sin(phiactual);
      sinphiactual=cos(phiactual);

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
      msg(MSG::VERBOSE)<< "derivative is: " << derivative << " sec derivative is: " << secderivative << endreq;
      std::cout << std::setprecision(25) << std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
                                                      std::pow(y0-yc+Rt*sinphiactual,2)+
                                                      std::pow(z0-zc-Rt*cottheta*phiactual,2)) << std::endl;
      msg(MSG::VERBOSE)<< "actual distance is: " << std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
                                                                 std::pow(y0-yc+Rt*sinphiactual,2)+
                                                                 std::pow(z0-zc-Rt*cottheta*phiactual,2)) << endreq;
#endif

      if (secderivative<0) throw error::ImpactPoint3dEstimatorProblem("Second derivative is negative");

      if (ncycle>m_maxiterations) throw error::ImpactPoint3dEstimatorProblem("Too many loops: could not find minimu distance to vertex");

      ncycle+=1;
      if (ncycle>m_maxiterations||fabs(deltaphi)<m_precision) {
#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
        msg(MSG::VERBOSE)<< "found minimum at: " << phiactual << endreq;
#endif
        isok=true;
      }

    } while (isok==false);

    //now you have to construct the plane with PlaneSurface

    //first vector at 3d impact point

    Amg::Vector3D MomentumDir(cos(phiactual)*sin(theta),sin(phiactual)*sin(theta),cos(theta));

    Amg::Vector3D DeltaR(x0-xc+Rt*cosphiactual,y0-yc+Rt*sinphiactual,z0-zc-Rt*cottheta*phiactual);
    m_distance=DeltaR.mag();
    DeltaR=DeltaR.unit();


    //correct DeltaR from small deviations from orthogonality to DeltaR

    Amg::Vector3D DeltaRcorrected=DeltaR-(DeltaR.dot(MomentumDir))*MomentumDir;

    if ((DeltaR-DeltaRcorrected).mag()>1e-4)
    {
      msg(MSG::WARNING) << " DeltaR and MomentumDir are not orthogonal " << endreq;
      msg(MSG::DEBUG)<< std::setprecision(10) << " DeltaR-DeltaRcorrected: "  << (DeltaR-DeltaRcorrected).mag() << endreq;
    }

    Amg::Vector3D YDir=MomentumDir.cross(DeltaRcorrected);

    //store the impact 3d point
    m_vertex=new Amg::Vector3D(x0+Rt*cosphiactual,y0+Rt*sinphiactual,z0-Rt*cottheta*phiactual);

    if(msgLvl(MSG::VERBOSE))
    {

      msg(MSG::VERBOSE)<< "final minimal distance is: " << std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
                                                                     std::pow(y0-yc+Rt*sinphiactual,2)+
                                                                     std::pow(z0-zc-Rt*cottheta*phiactual,2)) << endreq;
    }

    if(msgLvl(MSG::DEBUG))
    {
      msg(MSG::DEBUG) << "POCA in 3D is: " << *m_vertex << endreq;
    }


    //store the plane...
    if (msgLvl(MSG::VERBOSE))
        msg(MSG::VERBOSE)<< "plane to which to extrapolate X " << DeltaRcorrected << " Y " << YDir << " Z " << MomentumDir << endreq;

    Amg::Transform3D* thePlane = new Amg::Transform3D(DeltaRcorrected, YDir, MomentumDir, *theVertex);

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
    std::cout << "the translation is, directly from Transform3d: " << thePlane.getTranslation() << endreq;
#endif

   return new PlaneSurface(thePlane);

  }//end of estimate 3dIP method

  Amg::Vector3D* ImpactPoint3dEstimator::get3dIP() const
  {
    return m_vertex;
  }

  bool ImpactPoint3dEstimator::addIP3dAtaPlane(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const
  {
    if (vtxTrack.initialPerigee()) {
      AtaPlane* myPlane=IP3dAtaPlane(vtxTrack,vertex);
      if (myPlane)
        {
          vtxTrack.setImpactPoint3dAtaPlane(myPlane);
          return true;
        }
    } else { //for neutrals
      NeutralAtaPlane* myPlane=IP3dNeutralAtaPlane(vtxTrack.initialNeutralPerigee(),vertex);
      if (myPlane)      {
        ATH_MSG_VERBOSE ("Adding plane: " << myPlane->associatedSurface() );
        vtxTrack.setImpactPoint3dNeutralAtaPlane(myPlane);
        return true;
      }
    }
    return false;
  }
  

  Trk::AtaPlane * ImpactPoint3dEstimator::IP3dAtaPlane(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const
  {
    if (!vtxTrack.initialPerigee() && vtxTrack.initialNeutralPerigee())
      msg(MSG::WARNING) << "Calling ImpactPoint3dEstimator::IP3dAtaPlane cannot return NeutralAtaPlane" << endreq;

    const PlaneSurface* theSurfaceAtIP(0);

    try
    {
      theSurfaceAtIP = Estimate3dIP(vtxTrack.initialPerigee(),&vertex);
    }
    catch (error::ImpactPoint3dEstimatorProblem err)
    {
      msg(MSG::WARNING) << " ImpactPoin3dEstimator failed to find minimum distance between track and vertex seed: " << err.p << endreq;
      return 0;
    }
    if(!theSurfaceAtIP) msg(MSG::WARNING) << " ImpactPoin3dEstimator failed to find minimum distance and returned 0 "
<< endreq;

#ifdef ImpactPoint3dAtaPlaneFactory_DEBUG
    msg(MSG::VERBOSE) << "Original perigee was: " << *(vtxTrack.initialPerigee()) << endreq;
    msg(MSG::VERBOSE) << "The resulting surface is: " << *theSurfaceAtIP << endreq;
#endif

   Trk::AtaPlane* res = const_cast<Trk::AtaPlane *>(dynamic_cast<const Trk::AtaPlane *>
                              (m_extrapolator->extrapolate(*(vtxTrack.initialPerigee()),*theSurfaceAtIP)));
   delete theSurfaceAtIP;
   return res;
  }
  

  Trk::NeutralAtaPlane * ImpactPoint3dEstimator::IP3dNeutralAtaPlane(const NeutralParameters * initNeutPerigee,const Amg::Vector3D & vertex) const
  {
    const PlaneSurface* theSurfaceAtIP(0);

    try
    {
        theSurfaceAtIP = Estimate3dIP(initNeutPerigee,&vertex);
    }
    catch (error::ImpactPoint3dEstimatorProblem err)
    {
      msg(MSG::WARNING) << " ImpactPoin3dEstimator failed to find minimum distance between track and vertex seed: " << err.p << endreq;
      return 0;
    }
    if(!theSurfaceAtIP) msg(MSG::WARNING) << " ImpactPoin3dEstimator failed to find minimum distance and returned 0 "
<< endreq;

#ifdef ImpactPoint3dAtaPlaneFactory_DEBUG
    msg(MSG::VERBOSE) << "Original neutral perigee was: " << *initNeutPerigee << endreq;
    msg(MSG::VERBOSE) << "The resulting surface is: " << *theSurfaceAtIP << endreq;
#endif

    Trk::NeutralAtaPlane* res = const_cast<Trk::NeutralAtaPlane *>(dynamic_cast<const Trk::NeutralAtaPlane *>
                                                                   (m_extrapolator->extrapolate(*initNeutPerigee,*theSurfaceAtIP)));
   delete theSurfaceAtIP;
   return res;
  }
  

}
