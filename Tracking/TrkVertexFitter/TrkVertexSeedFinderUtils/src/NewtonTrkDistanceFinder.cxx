/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
    NewtonTrkDistanceFinder.cxx - Description in header file
*********************************************************************/

//#define TrkDistance_DEBUG

#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include <math.h>



namespace {
#if 0
  inline double sgn(const double value) {
    return value/fabs(value);
  }
  inline double square(const double tosquare) {
    return std::pow(tosquare,2);
  }
#endif
  //inline double getphipoca(const Trk::Perigee & myPerigee) {
  //  return myPerigee.parameters()[Trk::d0]>=0 ? myPerigee.parameters()[Trk::phi0]+M_PI/2. :
  //    myPerigee.parameters()[Trk::phi0]-M_PI/2.;
  //}
  inline double getRadiusOfCurvature(const Trk::Perigee & myPerigee,const double Bzfield) {
    return sin(myPerigee.parameters()[Trk::theta])/(Bzfield*myPerigee.parameters()[Trk::qOverP]);
  }
#if 0
  inline const Amg::Vector3D getCenterOfCurvature(const Trk::Perigee & myPerigee,const double RadiusOfCurvature,const double phipoca) {
    return Amg::Vector3D(myPerigee.associatedSurface().center().x()-RadiusOfCurvature*cos(phipoca),
			 myPerigee.associatedSurface().center().y()-RadiusOfCurvature*sin(phipoca),
			 myPerigee.associatedSurface().center().z()+RadiusOfCurvature*
			 myPerigee.parameters()[Trk::phi0]/tan(myPerigee.parameters()[Trk::theta]));
  }
#endif
  //  inline const Amg::Vector3D getSeedPoint(const Trk::Perigee & myPerigee,const Amg::Vector3D & center,
  //					      const double radius,const double newphi) {
  // short int sgnd0=(short int)(myPerigee.parameters()[Trk::d0]/fabs(myPerigee.parameters()[Trk::d0]));
  //return Amg::Vector3D(center.x()+radius*cos(newphi),
  //		         center.y()+radius*sin(newphi),
  //		         center.z()-radius*(newphi-sgnd0*M_PI/2.)/tan(myPerigee.parameters()[Trk::theta]));
  //}
}

namespace Trk
{
  NewtonTrkDistanceFinder::NewtonTrkDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_precision(1e-8),
    m_maxloopnumber(20),
    m_magFieldSvc("AtlasFieldSvc", n)
  {   
    declareProperty("MagFieldSvc",     m_magFieldSvc);
    declareProperty("Precision",m_precision);
    declareProperty("MaxLoops",m_maxloopnumber);
    declareInterface<NewtonTrkDistanceFinder>(this);
  }

  NewtonTrkDistanceFinder::~NewtonTrkDistanceFinder() {}

  StatusCode NewtonTrkDistanceFinder::initialize() 
  { 

    StatusCode s = AlgTool::initialize();
    if (s.isFailure() )
    {
      msg(MSG::FATAL) << "AlgTool::initialize() initialize failed!" << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_magFieldSvc.retrieve().isFailure() ) {
      msg(MSG::FATAL)<<"Could not find magnetic field service." << endmsg;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  StatusCode NewtonTrkDistanceFinder::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

const TwoPointOnTrack NewtonTrkDistanceFinder::GetClosestPoints(const PointOnTrack & firsttrack,const PointOnTrack & secondtrack) {

  
  //Now the direction of momentum at point of closest approach (but only direction, not versus)
  m_a_phi0=firsttrack.getPerigee().parameters()[Trk::phi0];
  m_a_cosphi0=-sin(m_a_phi0);//do i need it?
  m_a_sinphi0=cos(m_a_phi0);//~?
  
  //Now initialize the variable you need to go on
  m_a_x0=firsttrack.getPerigee().associatedSurface().center().x()+
    firsttrack.getPerigee().parameters()[Trk::d0]*m_a_cosphi0;
  m_a_y0=firsttrack.getPerigee().associatedSurface().center().y()+
    firsttrack.getPerigee().parameters()[Trk::d0]*m_a_sinphi0;
  m_a_z0=firsttrack.getPerigee().associatedSurface().center().z()+firsttrack.getPerigee().parameters()[Trk::z0];

#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_a_x0 " << m_a_x0 << " m_a_y0 " << m_a_y0 << " m_a_z0 " << m_a_z0 << endmsg;
#endif

#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_a_phi0 " << m_a_phi0 << endmsg;
#endif

  double magnFieldVect[3];
  double posXYZ[3];
  posXYZ[0] = firsttrack.getPerigee().associatedSurface().center().x();
  posXYZ[1] = firsttrack.getPerigee().associatedSurface().center().y();
  posXYZ[2] = firsttrack.getPerigee().associatedSurface().center().z();
  m_magFieldSvc->getField(posXYZ,magnFieldVect);

  
  //Magnetic field at (x0,y0,z0)
  m_a_Bz=magnFieldVect[2]*299.792;//B field in Gev/mm
  //EvaluateMagneticField(m_a_x0,b_y0,b_z0);  

  m_a_Rt=getRadiusOfCurvature(firsttrack.getPerigee(),m_a_Bz);
  m_a_cotantheta=1./tan(firsttrack.getPerigee().parameters()[Trk::theta]);
  
#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_a_Rt" << m_a_Rt << " m_a_cotantheta " << m_a_cotantheta << endmsg;
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Magnetic field at perigee is " << m_a_Bz << "GeV/mm " << endmsg;
#endif

  //Now the direction of momentum at point of closest approach (but only direction, not versus)
  m_b_phi0=secondtrack.getPerigee().parameters()[Trk::phi0];
  m_b_cosphi0=-sin(m_b_phi0);//do i need it?
  m_b_sinphi0=cos(m_b_phi0);//~?
  
  //Now initialize the variable you need to go on
  m_b_x0=secondtrack.getPerigee().associatedSurface().center().x()+
    secondtrack.getPerigee().parameters()[Trk::d0]*m_b_cosphi0;
  m_b_y0=secondtrack.getPerigee().associatedSurface().center().y()+
    secondtrack.getPerigee().parameters()[Trk::d0]*m_b_sinphi0;
  m_b_z0=secondtrack.getPerigee().associatedSurface().center().z()+
    secondtrack.getPerigee().parameters()[Trk::z0];
  
#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "b_x0 " << m_b_x0 << " m_b_y0 " << m_b_y0 << " m_b_z0 " << m_b_z0 << endmsg;
#endif

#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "b_phi0 " << m_b_phi0 << endmsg;
#endif


  posXYZ[0] = secondtrack.getPerigee().associatedSurface().center().x();
  posXYZ[1] = secondtrack.getPerigee().associatedSurface().center().y();
  posXYZ[2] = secondtrack.getPerigee().associatedSurface().center().z();  
  m_magFieldSvc->getField(posXYZ,magnFieldVect);
  
  //Magnetic field at (x0,y0,z0)
  m_b_Bz=magnFieldVect[2]*299.792;//B field in Gev/mm - for the moment use a constant field offline
  //use the right value expressed in GeV
  //EvaluateMagneticField(b_x0,b_y0,b_z0);

  m_b_Rt=getRadiusOfCurvature(secondtrack.getPerigee(),m_b_Bz);
  m_b_cotantheta=1./tan(secondtrack.getPerigee().parameters()[Trk::theta]);
  
#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "b_Rt" << m_b_Rt << " m_b_cotantheta " << m_b_cotantheta << endmsg;
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Magnetic field at perigee is " << m_b_Bz << " GeV/mm " << endmsg;
#endif


  //Now prepare some more elaborate pieces for later
  m_ab_Dx0=m_a_x0-m_b_x0-m_a_Rt*m_a_cosphi0+m_b_Rt*m_b_cosphi0;
  m_ab_Dy0=m_a_y0-m_b_y0-m_a_Rt*m_a_sinphi0+m_b_Rt*m_b_sinphi0;
  m_ab_Dz0=m_a_z0-m_b_z0+m_a_Rt*m_a_cotantheta*m_a_phi0-m_b_Rt*m_b_cotantheta*m_b_phi0;

#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_ab_Dx0 " << m_ab_Dx0 << " m_ab_Dy0 " << m_ab_Dy0 << " m_ab_Dz0 " << m_ab_Dz0 << endmsg;
#endif
 

  //Prepare the initial point that can be different from point of closest approach
  //If you don't specify any point the default will be the point of closest approach!!
  //Another subroutine will be implemented if you want to use 
  //a certain seed
  m_a_phi=firsttrack.getPhiPoint();//this has to be corrected as soon as you adjust the Trk2dDistanceSeeder...
  m_b_phi=secondtrack.getPhiPoint();

  //store cos and sin of phi
  m_a_cosphi=-sin(m_a_phi);
  m_a_sinphi=cos(m_a_phi);
  m_b_cosphi=-sin(m_b_phi);
  m_b_sinphi=cos(m_b_phi);
  

#ifdef TrkDistance_DEBUG
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Beginning phi is m_a_phi: " << m_a_phi << " m_b_phi " << m_b_phi << endmsg;
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "LOOP number 0" << endmsg;
#endif


  m_deltaa_phi=0.;
  m_deltab_phi=0.;

  m_loopsnumber=0;
  
  bool isok=false;

  while (!isok) {

  #ifdef TrkDistance_DEBUG
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Entered LOOP number: " << m_loopsnumber << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "actual value of m_a_phi: " << m_a_phi << " of m_b_phi " << m_b_phi << endmsg;
#endif
    
    
    //count the loop number
    m_loopsnumber+=1;
    
    
#ifdef TrkDistance_DEBUG
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "First point x: " << GetClosestPoints().first.x()  
	<< "y: " << GetClosestPoints().first.y()  
	<< "z: " << GetClosestPoints().first.z() << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Second point x: " << GetClosestPoints().second.x()  
	<< "y: " << GetClosestPoints().second.y()  
	<< "z: " << GetClosestPoints().second.z() << endmsg;
    
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ActualDistance: " << GetDistance() << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "real Dx0 " << m_ab_Dx0+m_a_Rt*m_a_cosphi-m_b_Rt*m_b_cosphi << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "real Dy0 " << m_ab_Dy0+m_a_Rt*m_a_sinphi-m_b_Rt*m_b_sinphi << endmsg;
#endif
    
    //I remove the factor two from the formula
    m_d1da_phi=(m_ab_Dx0-m_b_Rt*m_b_cosphi)*(-m_a_Rt*m_a_sinphi)+
      (m_ab_Dy0-m_b_Rt*m_b_sinphi)*m_a_cosphi*m_a_Rt+
      (m_ab_Dz0-m_a_Rt*m_a_cotantheta*m_a_phi+m_b_Rt*m_b_cotantheta*m_b_phi)*(-m_a_Rt*m_a_cotantheta);
    
    
    //same for second deriv respective to phi
    m_d1db_phi=(m_ab_Dx0+m_a_Rt*m_a_cosphi)*m_b_Rt*m_b_sinphi-//attention!MINUS here
      (m_ab_Dy0+m_a_Rt*m_a_sinphi)*m_b_cosphi*m_b_Rt+
      (m_ab_Dz0-m_a_Rt*m_a_cotantheta*m_a_phi+m_b_Rt*m_b_cotantheta*m_b_phi)*(+m_b_Rt*m_b_cotantheta);
    
    //second derivatives (d^2/d^2(a) d^2/d^2(b) d^2/d(a)d(b) )

    m_d2da_phi2=(m_ab_Dx0-m_b_Rt*m_b_cosphi)*(-m_a_Rt*m_a_cosphi)+
      (m_ab_Dy0-m_b_Rt*m_b_sinphi)*(-m_a_Rt*m_a_sinphi)+
      +m_a_Rt*m_a_Rt*(m_a_cotantheta*m_a_cotantheta);



    m_d2db_phi2=(m_ab_Dx0+m_a_Rt*m_a_cosphi)*(+m_b_Rt*m_b_cosphi)+
      (m_ab_Dy0+m_a_Rt*m_a_sinphi)*(+m_b_Rt*m_b_sinphi)+
      +m_b_Rt*m_b_Rt*(m_b_cotantheta*m_b_cotantheta);


    m_d2da_phib_phi=-m_a_Rt*m_b_Rt*(m_a_sinphi*m_b_sinphi+m_a_cosphi*m_b_cosphi+m_a_cotantheta*m_b_cotantheta);

    //Calculate the determinant of the jakobian

    m_det=m_d2da_phi2*m_d2db_phi2-m_d2da_phib_phi*m_d2da_phib_phi;



#ifdef TrkDistance_DEBUG
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_d1da_phi " << m_d1da_phi << " m_d1db_phi " << m_d1db_phi << " m_d2da_phi2 " << m_d2da_phi2 << " m_d2db_phi2 " << m_d2db_phi2 
	<< " m_d2da_phib_phi " << m_d2da_phib_phi << " det " << m_det << endmsg;
#endif
    
    //if the quadratic form is defined negative or is semidefined, throw the event
    //(you are in a maximum or in a saddle point)
    if (m_det<0) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Hessian is negative: saddle point" << endmsg;
      throw Error::NewtonProblem("Hessian is negative");
    }
    if (m_det>0&&m_d2da_phi2<0) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Hessian indicates a maximum: derivative will be zero but result incorrect" << endmsg;
      throw Error::NewtonProblem("Maximum point found");
    }

//Now apply the Newton formula in more than one dimension
m_deltaa_phi=-(m_d2db_phi2*m_d1da_phi-m_d2da_phib_phi*m_d1db_phi)/m_det;
m_deltab_phi=-(-m_d2da_phib_phi*m_d1da_phi+m_d2da_phi2*m_d1db_phi)/m_det;

  #ifdef TrkDistance_DEBUG
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_deltaa_phi: " << m_deltaa_phi << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_deltab_phi: " << m_deltab_phi << endmsg;
  #endif


    m_a_phi+=m_deltaa_phi;
    m_b_phi+=m_deltab_phi;

    //store cos and sin of phi
    m_a_cosphi=-sin(m_a_phi);
    m_a_sinphi=cos(m_a_phi);
    m_b_cosphi=-sin(m_b_phi);
    m_b_sinphi=cos(m_b_phi);
    
    if (std::sqrt(std::pow(m_deltaa_phi,2)+std::pow(m_deltab_phi,2))<m_precision || 
	m_loopsnumber>m_maxloopnumber) isok=true;
    
  }
	
  if (m_loopsnumber>m_maxloopnumber) throw Error::NewtonProblem("Could not find minimum distance: max loops number reached"); //now return error, see how to do it...
  

  TwoPointOnTrack myresult=TwoPointOnTrack(PointOnTrack(firsttrack),PointOnTrack(secondtrack));

  
  myresult.first.setPhiPoint(m_a_phi);
  myresult.second.setPhiPoint(m_b_phi);


  return myresult;

}

//these were only private method: now they are public!

const TwoPoints NewtonTrkDistanceFinder::GetClosestPoints() const {
  return TwoPoints(Amg::Vector3D(m_a_x0+m_a_Rt*(m_a_cosphi-m_a_cosphi0),
				 m_a_y0+m_a_Rt*(m_a_sinphi-m_a_sinphi0),
				 m_a_z0-m_a_Rt*(m_a_phi-m_a_phi0)*m_a_cotantheta),
		   Amg::Vector3D(m_b_x0+m_b_Rt*(m_b_cosphi-m_b_cosphi0),
				 m_b_y0+m_b_Rt*(m_b_sinphi-m_b_sinphi0),
				 m_b_z0-m_b_Rt*(m_b_phi-m_b_phi0)*m_b_cotantheta));
}

  const Amg::Vector3D NewtonTrkDistanceFinder::GetCrossingPoint() const {
    const TwoPoints thepoints=GetClosestPoints();
    return (thepoints.first+thepoints.second)/2.;
  }
  
  double NewtonTrkDistanceFinder::GetDistance() const {
    const TwoPoints thepoints=GetClosestPoints();
    return std::sqrt(std::pow(thepoints.first.x()-thepoints.second.x(),2)+
		     std::pow(thepoints.first.y()-thepoints.second.y(),2)+
		     std::pow(thepoints.first.z()-thepoints.second.z(),2));
  }
}
