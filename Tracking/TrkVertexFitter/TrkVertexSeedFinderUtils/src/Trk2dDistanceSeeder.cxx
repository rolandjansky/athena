/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
    Trk2dDistanceSeeder.cxx - Description in header file
*********************************************************************/

//#define TRK2DDISTANCESEEDER_DEBUG

#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
#include "TrkVertexSeedFinderUtils/TwoTracks.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <math.h>
#include <TMath.h>

namespace {
  inline double distance2d(const Amg::Vector3D & a, const Amg::Vector3D & b) {
    return std::sqrt(std::pow(a.x()-b.x(),2)+std::pow(a.y()-b.y(),2));
  }
#if 0
  inline double abs2d(const Amg::Vector3D& point) {
    return std::sqrt(std::pow(point.x(),2)+std::pow(point.y(),2));
  }
#endif
  inline double takenearest(const double of, const double to) {
        if (of-to>=M_PI) return of-2*M_PI;
	if (of-to<-M_PI) return of+2*M_PI;
	return of;
  }
  inline double getangle(const double Dy, const double Dx) {//get the right angle between -M_PI and M_PI
    return TMath::ATan2(Dy,Dx);
    //    if (Dx==0.) return  Dy>=0 ?  M_PI/2. : -M_PI/2.;
    //    if (Dx<0) {
    //      if (Dy<0) {
    //	return atan(Dy/Dx)-M_PI;
    //      } else {
    //	return atan(Dy/Dx)+M_PI;;
    //      }
    //    }
    //    return atan(Dy/Dx);
  }
  inline double oppositeangle(const double angle) {
    return  angle>0. ? angle-M_PI : angle+M_PI;
  }
#if 0
  inline const std::pair<double,double> gettwoanglesfromcos(const double cos) {
    return std::pair<double,double>(acos(cos),-acos(cos)); 
  }
  inline double shiftangle(const double angle,const double of) {
    if (angle+of>M_PI) return angle+of-2*M_PI;
    if (angle+of<=-M_PI) return angle+of+2*M_PI;
    return angle+of;
  }
#endif
  inline double square(const double tosquare) {
    return std::pow(tosquare,2);
  }
  //inline double getphipoca(const Trk::Perigee & myPerigee) {
  //  return myPerigee.parameters()[Trk::d0]>=0 ? myPerigee.parameters()[Trk::phi0]+M_PI/2. :
  //    myPerigee.parameters()[Trk::phi0]-M_PI/2.;
  //}
  inline double getphipoca(const Trk::Perigee & myPerigee) {
    return myPerigee.parameters()[Trk::phi0]+M_PI/2;
  }
  inline double setphipoca(const double phi) {
    return phi-M_PI/2.;
  }
  inline double goFromPhipocaToPhi(const double phipoca)
  {
    return phipoca+M_PI/2.;
  }

  inline double getRadiusOfCurvature(const Trk::Perigee & myPerigee,const double Bzfield) {
    return sin(myPerigee.parameters()[Trk::theta])/(Bzfield*myPerigee.parameters()[Trk::qOverP]);
  }
  inline const Amg::Vector3D getCenterOfCurvature(const Trk::Perigee & myPerigee,const double RadiusOfCurvature,const double phipoca) {
    return Amg::Vector3D(myPerigee.associatedSurface().center().x()+myPerigee.parameters()[Trk::d0]*cos(phipoca)-RadiusOfCurvature*cos(phipoca),
			 myPerigee.associatedSurface().center().y()+myPerigee.parameters()[Trk::d0]*sin(phipoca)-RadiusOfCurvature*sin(phipoca),
			 myPerigee.associatedSurface().center().z()+myPerigee.parameters()[Trk::z0]+RadiusOfCurvature*
			 myPerigee.parameters()[Trk::phi0]/tan(myPerigee.parameters()[Trk::theta]));
  }
  inline const Amg::Vector3D getSeedPoint(const Trk::Perigee & myPerigee,const Amg::Vector3D & center,
					  const double radius,const double newphi) {
    //    short int sgnd0=(short int)(myPerigee.parameters()[Trk::d0]/fabs(myPerigee.parameters()[Trk::d0]));
    return Amg::Vector3D(center.x()+radius*cos(newphi+M_PI/2.),
			       center.y()+radius*sin(newphi+M_PI/2.),
			       //		eliminated sgnd0 from center.z()-radius*(newphi-sgnd0*M_PI/2.)/tan(myPerigee.parameters()[Trk::theta]));
			       center.z()-radius*newphi/tan(myPerigee.parameters()[Trk::theta]));
  }
  inline const Amg::Vector3D getSeedPoint(const Amg::Vector3D & center,
					  const double radius,const double newphi) {
    //    short int sgnd0=(short int)(myPerigee.parameters()[Trk::d0]/fabs(myPerigee.parameters()[Trk::d0]));
    return Amg::Vector3D(center.x()+radius*cos(newphi+M_PI/2.),
			 center.y()+radius*sin(newphi+M_PI/2.),
			 0.);
  }
  
}
  
namespace Trk
{
  Trk2dDistanceSeeder::Trk2dDistanceSeeder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_solveAmbiguityUsingZ(true),
    m_magFieldSvc("AtlasFieldSvc", n)
  {   
    declareProperty("MagFieldSvc",     m_magFieldSvc);
    declareProperty("SolveAmbiguityUsingZ",m_solveAmbiguityUsingZ);
    declareInterface<Trk2dDistanceSeeder>(this);
  }

  Trk2dDistanceSeeder::~Trk2dDistanceSeeder() {}

  StatusCode Trk2dDistanceSeeder::initialize() 
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
    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  StatusCode Trk2dDistanceSeeder::finalize() 
  {
    msg(MSG::INFO) << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }


  const TwoPointOnTrack Trk2dDistanceSeeder::GetSeed(const TwoTracks & mytracks) 
  {
    
  
    double magnFieldVect[3];
    double posXYZ[3];
    posXYZ[0] = mytracks.getFirstPerigee().associatedSurface().center().x();
    posXYZ[1] = mytracks.getFirstPerigee().associatedSurface().center().y();
    posXYZ[2] = mytracks.getFirstPerigee().associatedSurface().center().z();
    m_magFieldSvc->getField(posXYZ,magnFieldVect);
    m_bfield1=magnFieldVect[2]*299.792;//should be in GeV/mm
    if (m_bfield1==0.||isnan(m_bfield1)) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not find a magnetic field different from zero: very very strange" << endmsg;
      m_bfield1=0.60407; //Value in GeV/mm (ATLAS units)
    } else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::DEBUG) << "Magnetic field projection of z axis in the perigee position track 1 is: " << m_bfield1 << " GeV/mm " << endmsg;
    }

    posXYZ[0] = mytracks.getSecondPerigee().associatedSurface().center().x();
    posXYZ[1] = mytracks.getSecondPerigee().associatedSurface().center().y();
    posXYZ[2] = mytracks.getSecondPerigee().associatedSurface().center().z();    
    m_magFieldSvc->getField(posXYZ,magnFieldVect);
    m_bfield2= magnFieldVect[2]*299.792;//should be in GeV/mm
    if (m_bfield2==0.||isnan(m_bfield2)) {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not find a magnetic field different from zero: very very strange" << endmsg;
      m_bfield2=0.60407; //Value in GeV/mm (ATLAS units)
    } else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Magnetic field projection of z axis in the perigee position track 2 is: " << m_bfield2 << " GeV/mm " << endmsg;
    }

    //phitanpoca here means not the tan to poca (which is phi0) but the direction from perigee to the center of curvature (I don't know 
    //why I chose this strange name, sorry!)
    m_phitanpoca1=getphipoca(mytracks.getFirstPerigee());
    m_phitanpoca2=getphipoca(mytracks.getSecondPerigee());
    
    //Temporary m_abs1 and m_abs2 are the signed radius quantities (later we will reduce it to unsigned quantities)
    m_abs1=getRadiusOfCurvature(mytracks.getFirstPerigee(),m_bfield1);
    m_abs2=getRadiusOfCurvature(mytracks.getSecondPerigee(),m_bfield2);
    
    m_sgnr1=m_abs1>0?1.:-1.;
    m_sgnr2=m_abs2>0?1.:-1.;

    //component x of center of curv of first particle
    m_centersofcurv=std::pair<Amg::Vector3D,Amg::Vector3D>(getCenterOfCurvature(mytracks.getFirstPerigee(),m_abs1,m_phitanpoca1),
							   getCenterOfCurvature(mytracks.getSecondPerigee(),m_abs2,m_phitanpoca2));
#ifdef TRK2DDISTANCESEEDER_DEBUG
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Center of track number 1: " << m_centersofcurv.first << " center of track 2 " << m_centersofcurv.second << endmsg;
#endif

    m_distance2d=distance2d(m_centersofcurv.first,m_centersofcurv.second);

    m_abs1=fabs(m_abs1);
    m_abs2=fabs(m_abs2);

  
#ifdef TRK2DDISTANCESEEDER_DEBUG
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_abs1: " << m_abs1 << " m_abs2: " << m_abs2 << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "m_phitanpoca1: " << m_phitanpoca1 << " m_phitanpoca2: " << m_phitanpoca2 << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "distance2d " << m_distance2d << endmsg;
#endif


  //first case (the two circles are the one distinct from the other)
  if (m_distance2d>(m_abs1+m_abs2)) {

#ifdef TRK2DDISTANCESEEDER_DEBUG
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Distinct circles " << endmsg;
#endif

    m_phi1=getangle(m_centersofcurv.second.y()-m_centersofcurv.first.y(),m_centersofcurv.second.x()-m_centersofcurv.first.x());
    
    if (m_sgnr2<0) {
      m_phi2=m_phi1;
    } else {
      m_phi2=oppositeangle(m_phi1);
    }      
    
    if (m_sgnr1<0) {
      m_phi1=oppositeangle(m_phi1);
    }

    //Now you are worried that the solution has to be the one nearest to the original phi...
    m_phi1=takenearest(m_phi1,m_phitanpoca1);
    m_phi2=takenearest(m_phi2,m_phitanpoca2);

    return TwoPointOnTrack(PointOnTrack(mytracks.getFirstPerigee(),setphipoca(m_phi1)),
			   PointOnTrack(mytracks.getSecondPerigee(),setphipoca(m_phi2)));
  } else {
    //second case(if m_distance2d<m_abs1+m_abs2 we have still two cases
    //1) one circle inside the other (nobody is touched) (if |m_abs2-m_abs1|>d)
    //2) two intersections (if |m_abs2-m_abs1|<d)
    //   -->(case with one intersection seems me of no physical interest)
    if (fabs(m_abs2-m_abs1)>=m_distance2d) {

#ifdef TRK2DDISTANCESEEDER_DEBUG
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "  one circle inside the other " << endmsg;
#endif

      //1)
      if (m_abs2<m_abs1) {

#ifdef TRK2DDISTANCESEEDER_DEBUG
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "  second radius smaller than first " << endmsg;
#endif 
	m_phi1=getangle(m_centersofcurv.second.y()-m_centersofcurv.first.y(),m_centersofcurv.second.x()-m_centersofcurv.first.x());	
	m_phi2=m_phi1;
      } else {
	
#ifdef TRK2DDISTANCESEEDER_DEBUG
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "  first radius smaller than second " << endmsg;
#endif 
	
	
	m_phi1=getangle(-(m_centersofcurv.second.y()-m_centersofcurv.first.y()),-(m_centersofcurv.second.x()-m_centersofcurv.first.x()));
	m_phi2=m_phi1;
      }
      
      //adjust the sign, taking into account possible q(1,2)<0
      if (m_sgnr2<0) {
	m_phi2=oppositeangle(m_phi2);
      }      
      
      if (m_sgnr1<0) {
	m_phi1=oppositeangle(m_phi1);
      }

      m_phi1=takenearest(setphipoca(m_phi1),setphipoca(m_phitanpoca1));
      m_phi2=takenearest(setphipoca(m_phi2),setphipoca(m_phitanpoca2));      
    } else {

//Do it easier...
      double projection2=(square(m_abs1)-square(m_abs2)-square(m_distance2d))/2./m_distance2d;
      double cosinus2=projection2/m_sgnr2/m_abs2;


#ifdef TRK2DDISTANCESEEDER_DEBUG
      double projection1=(square(m_abs2)-square(m_abs1)-square(m_distance2d))/2./m_distance2d;
      double cosinus1=projection1/m_sgnr1/m_abs1;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "projection1: " << projection1 << " cosinus1 " << cosinus1
	  << "projection2: " << projection2 << " cosinus2 " << cosinus2 << endmsg;
#endif

      double phibase2=
	TMath::ATan2(m_centersofcurv.second.y()-m_centersofcurv.first.y(),
		     m_centersofcurv.second.x()-m_centersofcurv.first.x());


      double addtophi2=
	TMath::ACos(cosinus2);

#ifdef TRK2DDISTANCESEEDER_DEBUG
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " phibase2 is : " << phibase2 << " add to phi " << addtophi2<< endmsg;
#endif

      std::pair<double,double> possiblephiontrack2(phibase2+addtophi2,phibase2-addtophi2);

#ifdef TRK2DDISTANCESEEDER_DEBUG
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "the two phis are: " << possiblephiontrack2.first << " and " <<  possiblephiontrack2.second << endmsg;
#endif

      std::pair<double,double> 
	possiblecosphitrack1((m_centersofcurv.second.x()-m_centersofcurv.first.x()
			      +m_sgnr2*m_abs2*TMath::Cos(possiblephiontrack2.first))/m_sgnr1/m_abs1,
			     (m_centersofcurv.second.x()-m_centersofcurv.first.x()
			      +m_sgnr2*m_abs2*TMath::Cos(possiblephiontrack2.second))/m_sgnr1/m_abs1);

      std::pair<double,double> possiblesigntrack1(m_sgnr1*(m_centersofcurv.second.y()-m_centersofcurv.first.y()
						     +m_sgnr2*m_abs2*TMath::Sin(possiblephiontrack2.first))>0?1.:-1.,
						  m_sgnr1*(m_centersofcurv.second.y()-m_centersofcurv.first.y()
							   +m_sgnr2*m_abs2*TMath::Sin(possiblephiontrack2.second))>0?1.:-1.);

      std::pair<double,double> possiblephiontrack1(possiblesigntrack1.first*TMath::ACos(possiblecosphitrack1.first),
						   possiblesigntrack1.second*TMath::ACos(possiblecosphitrack1.second));
						   
						     
      std::pair<PointOnTrack,PointOnTrack> possiblepointsontrack1(PointOnTrack(mytracks.getFirstPerigee(),
									       takenearest(setphipoca(possiblephiontrack1.first),
											   setphipoca(m_phitanpoca1))),
								  PointOnTrack(mytracks.getFirstPerigee(),
									       takenearest(setphipoca(possiblephiontrack1.second),
											   setphipoca(m_phitanpoca1))));
      std::pair<PointOnTrack,PointOnTrack> possiblepointsontrack2(PointOnTrack(mytracks.getSecondPerigee(),
									       takenearest(setphipoca(possiblephiontrack2.first),
											   setphipoca(m_phitanpoca2))),
								  PointOnTrack(mytracks.getSecondPerigee(),
									       takenearest(setphipoca(possiblephiontrack2.second),
											   setphipoca(m_phitanpoca2))));


      std::pair<Amg::Vector3D,Amg::Vector3D> possiblepoints1(getSeedPoint(possiblepointsontrack1.first.getPerigee(),m_centersofcurv.first,
									  m_abs1*m_sgnr1,possiblepointsontrack1.first.getPhiPoint()),
							     getSeedPoint(possiblepointsontrack1.second.getPerigee(),m_centersofcurv.first,
									  m_abs1*m_sgnr1,possiblepointsontrack1.second.getPhiPoint()));
      std::pair<Amg::Vector3D,Amg::Vector3D> possiblepoints2(getSeedPoint(possiblepointsontrack2.first.getPerigee(),m_centersofcurv.second,
									  m_abs2*m_sgnr2,possiblepointsontrack2.first.getPhiPoint()),
							     getSeedPoint(possiblepointsontrack2.second.getPerigee(),m_centersofcurv.second,
									  m_abs2*m_sgnr2,possiblepointsontrack2.second.getPhiPoint()));


#ifdef TRK2DDISTANCESEEDER_DEBUG
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Point 1a: x " << possiblepoints1.first.x() << " y " << possiblepoints1.first.y() << endmsg;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Point 2a: x " << possiblepoints2.first.x() << " y " << possiblepoints2.first.y() << endmsg;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Point 1b: x " << possiblepoints1.second.x() << " y " << possiblepoints1.second.y() << endmsg;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Point 2b: x " << possiblepoints2.second.x() << " y " << possiblepoints2.second.y() << endmsg;
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Distance between 1a and 2a:" << fabs(possiblepoints1.first.z()-possiblepoints2.first.z())  << 
	"Distance between 1a and 2a:" << fabs(possiblepoints1.second.z()-possiblepoints2.second.z())  << endmsg;
#endif

      
      
      //now discover what couple of points are the nearest ones
      if (m_solveAmbiguityUsingZ)
      {
        if (fabs(possiblepoints1.first.z()-possiblepoints2.first.z())<
            fabs(possiblepoints1.second.z()-possiblepoints2.second.z())) {
          m_phi1=goFromPhipocaToPhi(possiblepointsontrack1.first.getPhiPoint());
          m_phi2=goFromPhipocaToPhi(possiblepointsontrack2.first.getPhiPoint());
          return std::pair<PointOnTrack,PointOnTrack>(possiblepointsontrack1.first,possiblepointsontrack2.first);
        } else {
          m_phi1=goFromPhipocaToPhi(possiblepointsontrack1.second.getPhiPoint());
          m_phi2=goFromPhipocaToPhi(possiblepointsontrack2.second.getPhiPoint());
          return std::pair<PointOnTrack,PointOnTrack>(possiblepointsontrack1.second,possiblepointsontrack2.second);
        }
      }
      else
      {
        if (sqrt(possiblepoints1.first.x()*possiblepoints1.first.x()+possiblepoints1.first.y()*possiblepoints1.first.y())<
            sqrt(possiblepoints2.first.x()*possiblepoints2.first.x()+possiblepoints2.first.y()*possiblepoints2.first.y()))
        {
          m_phi1=goFromPhipocaToPhi(possiblepointsontrack1.first.getPhiPoint());
          m_phi2=goFromPhipocaToPhi(possiblepointsontrack2.first.getPhiPoint());
          return std::pair<PointOnTrack,PointOnTrack>(possiblepointsontrack1.first,possiblepointsontrack2.first);
        } else {
          m_phi1=goFromPhipocaToPhi(possiblepointsontrack1.second.getPhiPoint());
          m_phi2=goFromPhipocaToPhi(possiblepointsontrack2.second.getPhiPoint());
          return std::pair<PointOnTrack,PointOnTrack>(possiblepointsontrack1.second,possiblepointsontrack2.second);
        }
      }
    }
  }
  return TwoPointOnTrack(PointOnTrack(mytracks.getFirstPerigee(),setphipoca(m_phi1)),
			 PointOnTrack(mytracks.getSecondPerigee(),setphipoca(m_phi2)));
  }
  

  
  const TwoPoints Trk2dDistanceSeeder::GetClosestPoints() const {

    TwoPoints theTwoPoints(getSeedPoint(m_centersofcurv.first,
                                        m_abs1*m_sgnr1,setphipoca(m_phi1)),
                           getSeedPoint(m_centersofcurv.second,
                                        m_abs2*m_sgnr2,setphipoca(m_phi2)));

    return theTwoPoints;

  }
  

  const Amg::Vector3D Trk2dDistanceSeeder::GetCrossingPoint() const {
    const TwoPoints thepoints=GetClosestPoints();
    return (thepoints.first+thepoints.second)/2.;
  }
  
  double Trk2dDistanceSeeder::GetDistance() const {
    const TwoPoints thepoints=GetClosestPoints();
    return std::sqrt(std::pow(thepoints.first.x()-thepoints.second.x(),2)+
		     std::pow(thepoints.first.y()-thepoints.second.y(),2)+
		     std::pow(thepoints.first.z()-thepoints.second.z(),2));
  }
  
}


