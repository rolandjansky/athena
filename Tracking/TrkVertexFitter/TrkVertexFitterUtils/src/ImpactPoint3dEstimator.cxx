/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          ImpactPoint3dEstimator.cxx - Description in header file
*********************************************************************/

#include "ImpactPoint3dEstimator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "MagFieldElements/AtlasFieldCache.h"

// #define IMPACTPOINT3DESTIMATOR_DEBUG

//added for cuts in case of displaced vertex
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/PerigeeSurface.h"

#include <cmath>

namespace Trk
{

  ImpactPoint3dEstimator::ImpactPoint3dEstimator(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_extrapolator("Trk::Extrapolator"),
    m_maxiterations(20),
    m_precision(1e-10)//DeltaPhi
  {   
    declareProperty("Extrapolator",m_extrapolator);
    declareProperty("MaxIterations",m_maxiterations);
    declareProperty("Precision",m_precision);
  }
  
  ImpactPoint3dEstimator::~ImpactPoint3dEstimator() = default;
  
  StatusCode ImpactPoint3dEstimator::initialize() 
  { 
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );

    ATH_MSG_DEBUG( "Initialize successful"  );
    return StatusCode::SUCCESS;
  }
  
  StatusCode ImpactPoint3dEstimator::finalize() 
  {
    ATH_MSG_DEBUG( "Finalize successful"  );
    return StatusCode::SUCCESS;
  }


  template<typename T>
  std::unique_ptr<PlaneSurface>
  ImpactPoint3dEstimator::Estimate3dIPNoCurvature(const T* thePerigee,
                                                  const Amg::Vector3D* theVertex,
                                                  double& distance) const
  {
      const Amg::Vector3D  momentumUnit = thePerigee->momentum().unit();
      double pathLength  =  ( *theVertex  - thePerigee->position() ).dot( momentumUnit )
                                                                       / (  momentumUnit.dot( momentumUnit )) ;
       //first vector at 3d impact point


      Amg::Vector3D POCA   =  thePerigee->position()  + pathLength * momentumUnit;// Position of closest approach
      Amg::Vector3D DeltaR =  *theVertex  - POCA;
      distance=DeltaR.mag();
      DeltaR=DeltaR.unit();


      //correct DeltaR from small deviations from orthogonality to DeltaR -- DeltaR.dot(momentumUnit) should equal 0 if the above is correct

      Amg::Vector3D DeltaRcorrected=DeltaR-(DeltaR.dot(momentumUnit))*momentumUnit;

      if ((DeltaR-DeltaRcorrected).mag()>1e-4)
      {
        ATH_MSG_WARNING( " DeltaR and MomentumDir are not orthogonal " );
        ATH_MSG_DEBUG( std::setprecision(10) << " DeltaR-DeltaRcorrected: "  << (DeltaR-DeltaRcorrected).mag() );
      }

      Amg::Vector3D YDir=momentumUnit.cross(DeltaRcorrected);

      ATH_MSG_VERBOSE( "final minimal distance is: " << distance);
      ATH_MSG_DEBUG( "POCA in 3D is: " << POCA );

      //store the plane...
      ATH_MSG_VERBOSE( "plane to which to extrapolate X " << DeltaRcorrected << " Y " << YDir << " Z " << momentumUnit);

      auto thePlane = std::make_unique<Amg::Transform3D>(DeltaRcorrected, YDir, momentumUnit, *theVertex);

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG
      std::cout << "the translation is, directly from Transform3d: " << thePlane->getTranslation() << endmsg;
#endif

      return std::make_unique<PlaneSurface>(std::move (thePlane));

  }

  std::unique_ptr<PlaneSurface>
  ImpactPoint3dEstimator::Estimate3dIP(const NeutralParameters* neutralPerigee,
                                       const Amg::Vector3D* theVertex,
                                       double& distance) const
  {
    ATH_MSG_DEBUG("Neutral particle --  propagate like a straight line");
    return Estimate3dIPNoCurvature(neutralPerigee, theVertex, distance);
  }

  std::unique_ptr<PlaneSurface>
  ImpactPoint3dEstimator::Estimate3dIP(const TrackParameters* trackPerigee,
                                       const Amg::Vector3D* theVertex,
                                       double& distance) const
  {
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, Gaudi::Hive::currentContext()};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache (fieldCache);

    double magnFieldVect[3];
    fieldCache.getField(trackPerigee->associatedSurface().center().data(),magnFieldVect);
    if(magnFieldVect[2] == 0 ){
      ATH_MSG_DEBUG("Magnetic field in the Z direction is 0 --  propagate like a straight line");
      return Estimate3dIPNoCurvature(trackPerigee, theVertex, distance);
    }


    const Trk::Perigee* thePerigee=dynamic_cast<const Trk::Perigee*>(trackPerigee);
    if (thePerigee==nullptr)
    {
      ATH_MSG_DEBUG( " ImpactPoint3dEstimator didn't get a Perigee* as ParametersBase*: cast not possible. Need to EXTRAPOLATE...");
      
      /* run-1 code had not been adapted for Neutral parameters:
      const TrackParameters* ch_params = dynamic_cast<const TrackParameters*>(trackPerigee);
      if (ch_params == 0) {
        msg(MSG::ERROR) << " Cannot cast to charged track parameters. Neutrals are not supported... " << endmsg;
        return 0;
      }*/

      Trk::PerigeeSurface perigeeSurface(*theVertex);
      thePerigee=dynamic_cast<const Trk::Perigee*>(m_extrapolator->extrapolateDirectly(*trackPerigee,
                                                                                       perigeeSurface));
      if (thePerigee == nullptr) return nullptr;
    }

    ATH_MSG_VERBOSE( " Now running ImpactPoint3dEstimator::Estimate3dIP" );
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
      thePerigee=nullptr;
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
      ATH_MSG_VERBOSE( "Cycle number: " << ncycle << " old phi: " << phiactual  );
#endif
      //      distance=std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
      //                 std::pow(y0-yc+Rt*sinphiactual,2)+
      //                 std::pow(z0-zc-Rt*cottheta*phiactual));

      derivative=(x0-xc)*(-Rt*sinphiactual)+(y0-yc)*Rt*cosphiactual+(z0-zc-Rt*phiactual*cottheta)*(-Rt*cottheta);
      secderivative=Rt*(-(x0-xc)*cosphiactual-(y0-yc)*sinphiactual+Rt*cottheta*cottheta);
#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
      ATH_MSG_VERBOSE( "derivative is: " << derivative << " sec derivative is: " << secderivative  );
#endif

      deltaphi=-derivative/secderivative;

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
      std::cout << std::setprecision(25) << "deltaphi: " << deltaphi << std::endl;
#endif

      phiactual+=deltaphi;
      cosphiactual=-sin(phiactual);
      sinphiactual=cos(phiactual);

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
      ATH_MSG_VERBOSE( "derivative is: " << derivative << " sec derivative is: " << secderivative  );
      std::cout << std::setprecision(25) << std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
                                                      std::pow(y0-yc+Rt*sinphiactual,2)+
                                                      std::pow(z0-zc-Rt*cottheta*phiactual,2)) << std::endl;
      ATH_MSG_VERBOSE( "actual distance is: " << std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
                                                                 std::pow(y0-yc+Rt*sinphiactual,2)+
                                                           std::pow(z0-zc-Rt*cottheta*phiactual,2))  );
#endif

      if (secderivative<0) throw error::ImpactPoint3dEstimatorProblem("Second derivative is negative");

      if (ncycle>m_maxiterations) throw error::ImpactPoint3dEstimatorProblem("Too many loops: could not find minimum distance to vertex");

      ncycle+=1;
      if (ncycle>m_maxiterations||fabs(deltaphi)<m_precision) {
#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
        ATH_MSG_VERBOSE( "found minimum at: " << phiactual  );
#endif
        isok=true;
      }

    } while (!isok);

    //now you have to construct the plane with PlaneSurface
    //first vector at 3d impact point
    Amg::Vector3D MomentumDir(std::cos(phiactual)*std::sin(theta),std::sin(phiactual)*std::sin(theta),std::cos(theta));
    Amg::Vector3D DeltaR(x0-xc+Rt*cosphiactual,y0-yc+Rt*sinphiactual,z0-zc-Rt*cottheta*phiactual);
    distance=DeltaR.mag();
    if (distance==0.){
      ATH_MSG_WARNING("DeltaR is zero in ImpactPoint3dEstimator::Estimate3dIP, returning nullptr");
      return nullptr;
    }
    DeltaR=DeltaR.unit();


    //correct DeltaR from small deviations from orthogonality to DeltaR

    Amg::Vector3D DeltaRcorrected=DeltaR-(DeltaR.dot(MomentumDir))*MomentumDir;

    if ((DeltaR-DeltaRcorrected).mag()>1e-4)
    {
      ATH_MSG_WARNING( " DeltaR and MomentumDir are not orthogonal "  );
      ATH_MSG_DEBUG( std::setprecision(10) << " DeltaR-DeltaRcorrected: "  << (DeltaR-DeltaRcorrected).mag()  );
    }

    Amg::Vector3D YDir=MomentumDir.cross(DeltaRcorrected);

    //store the impact 3d point
    Amg::Vector3D vertex(x0+Rt*cosphiactual,y0+Rt*sinphiactual,z0-Rt*cottheta*phiactual);

    ATH_MSG_VERBOSE( "final minimal distance is: " << std::sqrt(std::pow(x0-xc+Rt*cosphiactual,2)+
                                                                std::pow(y0-yc+Rt*sinphiactual,2)+
                                                                std::pow(z0-zc-Rt*cottheta*phiactual,2))  );

    ATH_MSG_DEBUG( "POCA in 3D is: " << vertex  );


    //store the plane...
    ATH_MSG_VERBOSE( "plane to which to extrapolate X " << DeltaRcorrected << " Y " << YDir << " Z " << MomentumDir  );

    auto thePlane = std::make_unique<Amg::Transform3D>(DeltaRcorrected, YDir, MomentumDir, *theVertex);

#ifdef IMPACTPOINT3DESTIMATOR_DEBUG            
    std::cout << "the translation is, directly from Transform3d: " << thePlane->getTranslation() << endmsg;
#endif

    return std::make_unique<PlaneSurface>(std::move(thePlane));

  }//end of estimate 3dIP method

  bool ImpactPoint3dEstimator::addIP3dAtaPlane(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const
  {
    if (vtxTrack.initialPerigee()) {
      const AtaPlane* myPlane=IP3dAtaPlane(vtxTrack,vertex);
      if (myPlane)
        {
          vtxTrack.setImpactPoint3dAtaPlane(myPlane);
          return true;
        }
    } else { //for neutrals
      const NeutralAtaPlane* myPlane=IP3dNeutralAtaPlane(vtxTrack.initialNeutralPerigee(),vertex);
      if (myPlane)      {
        ATH_MSG_VERBOSE ("Adding plane: " << myPlane->associatedSurface() );
        vtxTrack.setImpactPoint3dNeutralAtaPlane(myPlane);
        return true;
      }
    }
    return false;
  }
  

  const Trk::AtaPlane * ImpactPoint3dEstimator::IP3dAtaPlane(VxTrackAtVertex & vtxTrack,const Amg::Vector3D & vertex) const
  {
    if (!vtxTrack.initialPerigee() && vtxTrack.initialNeutralPerigee())
      ATH_MSG_WARNING( "Calling ImpactPoint3dEstimator::IP3dAtaPlane cannot return NeutralAtaPlane"  );

    std::unique_ptr<PlaneSurface> theSurfaceAtIP;

    try
    {
      double distance = 0;
      theSurfaceAtIP = Estimate3dIP(vtxTrack.initialPerigee(),&vertex,distance);
    }
    catch (error::ImpactPoint3dEstimatorProblem err)
    {
      ATH_MSG_WARNING( " ImpactPoint3dEstimator failed to find minimum distance between track and vertex seed: " << err.p  );
      return nullptr;
    }
    if(!theSurfaceAtIP){ 
      ATH_MSG_WARNING( " ImpactPoint3dEstimator failed to find minimum distance and returned 0 " );
      return nullptr;
    }
#ifdef ImpactPoint3dAtaPlaneFactory_DEBUG
    ATH_MSG_VERBOSE( "Original perigee was: " << *(vtxTrack.initialPerigee())  );
    ATH_MSG_VERBOSE( "The resulting surface is: " << *theSurfaceAtIP  );
#endif

   const Trk::AtaPlane* res = dynamic_cast<const Trk::AtaPlane *>
                              (m_extrapolator->extrapolate(*(vtxTrack.initialPerigee()),*theSurfaceAtIP));
   return res;
  }
  

  const Trk::NeutralAtaPlane * ImpactPoint3dEstimator::IP3dNeutralAtaPlane(const NeutralParameters * initNeutPerigee,const Amg::Vector3D & vertex) const
  {
    std::unique_ptr<PlaneSurface> theSurfaceAtIP;

    try
    {
        double distance = 0;
        theSurfaceAtIP = Estimate3dIP(initNeutPerigee,&vertex,distance);
    }
    catch (error::ImpactPoint3dEstimatorProblem err)
    {
      ATH_MSG_WARNING( " ImpactPoint3dEstimator failed to find minimum distance between track and vertex seed: " << err.p  );
      return nullptr;
    }
    if(!theSurfaceAtIP) ATH_MSG_WARNING( " ImpactPoint3dEstimator failed to find minimum distance and returned 0 " );

#ifdef ImpactPoint3dAtaPlaneFactory_DEBUG
    ATH_MSG_VERBOSE( "Original neutral perigee was: " << *initNeutPerigee  );
    ATH_MSG_VERBOSE( "The resulting surface is: " << *theSurfaceAtIP  );
#endif

    const Trk::NeutralAtaPlane* res = dynamic_cast<const Trk::NeutralAtaPlane *>
      (m_extrapolator->extrapolate(*initNeutPerigee,*theSurfaceAtIP));
   return res;
  }
  

}
