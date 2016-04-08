/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkJetVxFitter/TrkDistanceFinderNeutralCharged.h"
#include "TrkParameters/TrackParameters.h"
#include <TMath.h>

namespace {
  inline double getRadiusOfCurvature(const Trk::Perigee & myPerigee,const double Bzfield) {
    return sin(myPerigee.parameters()[Trk::theta])/(Bzfield*myPerigee.parameters()[Trk::qOverP]);
  }

}

namespace Trk {

TrkDistanceFinderNeutralCharged::TrkDistanceFinderNeutralCharged(const std::string& t, const std::string& n, const IInterface*  p) : 
  AthAlgTool(t,n,p),
  m_precision(1e-8),
  m_maxloopnumber(20),
  m_magFieldSvc("AtlasFieldSvc", n)
{
  declareProperty("Precision",m_precision);
  declareProperty("MaxLoops",m_maxloopnumber);
  declareInterface<TrkDistanceFinderNeutralCharged>(this);
  
}


StatusCode TrkDistanceFinderNeutralCharged::initialize() 
  { 
    StatusCode s = AthAlgTool::initialize();
    s = m_magFieldSvc.retrieve();
    if (s.isFailure())
    {
      msg(MSG::FATAL)<<"Could not find magnetic field service." << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

StatusCode TrkDistanceFinderNeutralCharged::finalize() 
{
  msg(MSG::INFO)  << "Finalize successful" << endreq;
  return StatusCode::SUCCESS;
}


TrkDistanceFinderNeutralCharged::~TrkDistanceFinderNeutralCharged() { }

std::pair<Amg::Vector3D,double>  
TrkDistanceFinderNeutralCharged::getPointAndDistance(const Trk::NeutralTrack& neutraltrk,
                                                     const Trk::Perigee& chargedtrk,
						     double & distanceOnAxis) {

  double m_b_phi0=chargedtrk.parameters()[Trk::phi0];
  double m_b_cosphi0=cos(m_b_phi0);
  double m_b_sinphi0=sin(m_b_phi0);
  double m_b_x0=chargedtrk.associatedSurface().center().x()-
    chargedtrk.parameters()[Trk::d0]*m_b_sinphi0;
  double m_b_y0=chargedtrk.associatedSurface().center().y()+
    chargedtrk.parameters()[Trk::d0]*m_b_cosphi0;
  double m_b_z0=chargedtrk.associatedSurface().center().z()+
    chargedtrk.parameters()[Trk::z0];

  Amg::Vector3D magnFieldVect;
  m_magFieldSvc->getField(&chargedtrk.associatedSurface().center(),&magnFieldVect);

  //Magnetic field at (x0,y0,z0)
  double m_Bz=magnFieldVect.z()*299.792;//B field in Gev/mm

  double m_b_Rt=getRadiusOfCurvature(chargedtrk,m_Bz);

  double m_b_cottheta=1./tan(chargedtrk.parameters()[Trk::theta]);
  
  //for neutrals
  double m_a_x0=neutraltrk.position()[0];
  double m_a_y0=neutraltrk.position()[1];
  double m_a_z0=neutraltrk.position()[2];
  double m_a_px=neutraltrk.momentum()[0];
  double m_a_py=neutraltrk.momentum()[1];
  double m_a_pz=neutraltrk.momentum()[2];

  
  //more elaborate pieces for later
  double m_DxNoVar=m_b_x0+m_b_Rt*m_b_sinphi0-m_a_x0;
  double m_DyNoVar=m_b_y0-m_b_Rt*m_b_cosphi0-m_a_y0;
  double m_DzNoVar=m_b_z0+m_b_Rt*m_b_cottheta*m_b_phi0-m_a_z0;

  //start in both cases from the vertex
  double m_b_phi=m_b_phi0;
  double m_b_cosphi=cos(m_b_phi);
  double m_b_sinphi=sin(m_b_phi);
  double m_a_lambda=0.;

  double m_deltab_phi=0.;
  double m_deltaa_lambda=0.;

  int m_loopsnumber=0;

  bool isok=false;

  bool secondTimeSaddleProblem=false;

  while (!isok) {

    //    std::cout << "Loop number: " << m_loopsnumber << std::endl;
    //    std::cout << "phi is: " << m_b_phi << " lambda " << m_a_lambda << std::endl;

    //ONLY FOR DEBUG * from here

    double x1fin=m_b_x0+m_b_Rt*(sin(m_b_phi0)-sin(m_b_phi));
    double y1fin=m_b_y0+m_b_Rt*(cos(m_b_phi)-cos(m_b_phi0));
    double z1fin=m_b_z0+m_b_Rt*m_b_cottheta*(m_b_phi0-m_b_phi);
    
    double x2fin=m_a_x0+m_a_px*m_a_lambda;
    double y2fin=m_a_y0+m_a_py*m_a_lambda;
    double z2fin=m_a_z0+m_a_pz*m_a_lambda;
    
    ATH_MSG_VERBOSE ("x1fin " << x1fin << "y1fin " << y1fin << "z1fin " << z1fin);
    ATH_MSG_VERBOSE ("x2fin " << x2fin << "y2fin " << y2fin << "z2fin " << z2fin);
    
    double distance = sqrt((x1fin-x2fin)*(x1fin-x2fin)+
			   (y1fin-y2fin)*(y1fin-y2fin)+
			   (z1fin-z2fin)*(z1fin-z2fin));

    ATH_MSG_VERBOSE ("distance " << distance);

    //to here (please delete afterwards)

    m_loopsnumber+=1;

    double m_d1db_phi=((m_DxNoVar-m_a_px*m_a_lambda)*(-m_b_Rt*m_b_cosphi)+
		       (m_DyNoVar-m_a_py*m_a_lambda)*(-m_b_Rt*m_b_sinphi)+
		       (m_DzNoVar-m_b_Rt*m_b_phi*m_b_cottheta-m_a_pz*m_a_lambda)*(-m_b_Rt*m_b_cottheta));

    double m_d1da_lambda=((m_DxNoVar-m_b_Rt*m_b_sinphi-m_a_px*m_a_lambda)*(-m_a_px)+
			  (m_DyNoVar+m_b_Rt*m_b_cosphi-m_a_py*m_a_lambda)*(-m_a_py)+
			  (m_DzNoVar-m_b_Rt*m_b_cottheta*m_b_phi-m_a_pz*m_a_lambda)*(-m_a_pz));

    //second derivatives (d^2/d^2(a) d^2/d^2(b) d^2/d(a)d(b) )

    double m_d2db_phi2=((m_DxNoVar-m_a_px*m_a_lambda)*(m_b_Rt*m_b_sinphi)+
			(m_DyNoVar-m_a_py*m_a_lambda)*(-m_b_Rt*m_b_cosphi)+
			(m_b_Rt*m_b_cottheta*m_b_Rt*m_b_cottheta));
    
    double m_d2dadb_lambdaphi=m_a_px*m_b_Rt*m_b_cosphi+m_a_py*m_b_Rt*m_b_sinphi+m_a_pz*m_b_Rt*m_b_cottheta;

    //    double m_d2da_lambda2=((m_DxNoVar-m_b_Rt*m_b_sinphi-m_a_px*m_a_lambda)*(m_a_px*m_a_px)+
    //		   (m_DyNoVar+m_b_Rt*m_b_cosphi-m_a_py*m_a_lambda)*(m_a_py*m_a_py)+
    //		   (m_DzNoVar-m_b_Rt*m_b_cottheta*m_b_phi-m_a_pz*m_a_lambda)*(m_a_pz*m_a_pz));

    //small error :-)

    double m_d2da_lambda2=((m_a_px*m_a_px)+
			   (m_a_py*m_a_py)+
			   (m_a_pz*m_a_pz));
    

    ATH_MSG_VERBOSE (" d1db_phi: " << m_d1db_phi << " d1da_lambda " << m_d1da_lambda);

    ATH_MSG_VERBOSE (" d2phi2 " << m_d2db_phi2 << " d2lambda2 " <<m_d2da_lambda2  << 
        " d2lambdaphi " << m_d2dadb_lambdaphi);

    
    double m_det=m_d2da_lambda2*m_d2db_phi2-m_d2dadb_lambdaphi*m_d2dadb_lambdaphi;

    if (m_det<0) {
      if (!secondTimeSaddleProblem) 
      {
        secondTimeSaddleProblem=true;

        ATH_MSG_DEBUG ("Now try first to recover from the problem");
        double denominator=1./((m_a_px*m_a_px)+(m_a_py*m_a_py));
        double firstTerm=(m_DxNoVar*m_a_px+m_DyNoVar*m_a_py)*denominator;
        double toSquare=(m_a_px*m_DyNoVar-m_a_py*m_DxNoVar);
        double toSqrt=m_b_Rt*m_b_Rt/denominator-toSquare*toSquare;
        if (toSqrt<0)
        {
          ATH_MSG_WARNING ("No intersection between neutral and charged track can be found. SKipping...");
          continue;
        }
        
        double secondTerm=TMath::Sqrt(toSqrt)*denominator;
        double lambda1=firstTerm+secondTerm;
        double lambda2=firstTerm-secondTerm;

        ATH_MSG_VERBOSE ("first solution: " << lambda1 << " second solution " << lambda2);
        
        double phi1=TMath::ATan2((m_DxNoVar-m_a_px*lambda1)/m_b_Rt,-(m_DyNoVar-m_a_py*lambda1)/m_b_Rt);
        double phi2=TMath::ATan2((m_DxNoVar-m_a_px*lambda2)/m_b_Rt,-(m_DyNoVar-m_a_py*lambda2)/m_b_Rt);

        double x1case1=m_b_x0+m_b_Rt*(sin(m_b_phi0)-sin(phi1));
        double y1case1=m_b_y0+m_b_Rt*(cos(phi1)-cos(m_b_phi0));
        double z1case1=m_b_z0+m_b_Rt*m_b_cottheta*(m_b_phi0-phi1);
        
        double x2case1=m_a_x0+m_a_px*lambda1;
        double y2case1=m_a_y0+m_a_py*lambda1;
        double z2case1=m_a_z0+m_a_pz*lambda1;

        double x1case2=m_b_x0+m_b_Rt*(sin(m_b_phi0)-sin(phi2));
        double y1case2=m_b_y0+m_b_Rt*(cos(phi2)-cos(m_b_phi0));
        double z1case2=m_b_z0+m_b_Rt*m_b_cottheta*(m_b_phi0-phi2);
        
        double x2case2=m_a_x0+m_a_px*lambda2;
        double y2case2=m_a_y0+m_a_py*lambda2;
        double z2case2=m_a_z0+m_a_pz*lambda2;

        ATH_MSG_VERBOSE ("solution1 x1: " << x1case1 << " x2: " << x2case1 << " y1: " << y1case1 << " y2: " << y2case1);
        ATH_MSG_VERBOSE ("solution2 x1: " << x1case2 << " x2: " << x2case2 << " y1: " << y1case2 << " y2: " << y2case2);

        double deltaz1=fabs(z2case1-z1case1);
        double deltaz2=fabs(z2case2-z1case2);

        ATH_MSG_VERBOSE (" deltaz1: " << deltaz1 << " deltaz2: " << deltaz2);

        if (deltaz1<=deltaz2)
        {
          m_b_phi=phi1;
          m_a_lambda=lambda1;
        }
        else
        {
          m_b_phi=phi2;
          m_a_lambda=lambda2;
        }
        //store cos and sin of phi
        m_b_cosphi=cos(m_b_phi);
        m_b_sinphi=sin(m_b_phi);
        continue;
      }
      else
      {
        ATH_MSG_WARNING ("Hessian is negative: saddle point");
        throw Error::NewtonProblem("Hessian is negative");
      }
    }
    if (m_det>0&&m_d2da_lambda2<0) {
      msg(MSG::WARNING) << "Hessian indicates a maximum: derivative will be zero but result incorrect" << endreq;
      throw Error::NewtonProblem("Maximum point found");
    }

    //Now apply the Newton formula in more than one dimension
    m_deltaa_lambda=-(m_d2db_phi2*m_d1da_lambda-m_d2dadb_lambdaphi*m_d1db_phi)/m_det;
    m_deltab_phi=-(-m_d2dadb_lambdaphi*m_d1da_lambda+m_d2da_lambda2*m_d1db_phi)/m_det;

    m_a_lambda+=m_deltaa_lambda;
    m_b_phi+=m_deltab_phi;
    
    //store cos and sin of phi
    m_b_cosphi=cos(m_b_phi);
    m_b_sinphi=sin(m_b_phi);

    if (sqrt(std::pow(m_deltaa_lambda,2)+std::pow(m_deltab_phi,2))<m_precision || 
	m_loopsnumber>m_maxloopnumber) isok=true;

  }
  
  if (m_loopsnumber>m_maxloopnumber) throw Error::NewtonProblem("Could not find minimum distance: max loops number reached"); //now return error, see how to do it...
  
  double x1fin=m_b_x0+m_b_Rt*(sin(m_b_phi0)-sin(m_b_phi));
  double y1fin=m_b_y0+m_b_Rt*(cos(m_b_phi)-cos(m_b_phi0));
  double z1fin=m_b_z0+m_b_Rt*m_b_cottheta*(m_b_phi0-m_b_phi);
  
  double x2fin=m_a_x0+m_a_px*m_a_lambda;
  double y2fin=m_a_y0+m_a_py*m_a_lambda;
  double z2fin=m_a_z0+m_a_pz*m_a_lambda;

  ATH_MSG_VERBOSE ("x1fin " << x1fin << "y1fin " << y1fin << "z1fin " << z1fin);
  ATH_MSG_VERBOSE ("x2fin " << x2fin << "y2fin " << y2fin << "z2fin " << z2fin);

  double distance = sqrt((x1fin-x2fin)*(x1fin-x2fin)+
			 (y1fin-y2fin)*(y1fin-y2fin)+
			 (z1fin-z2fin)*(z1fin-z2fin));

  ATH_MSG_DEBUG ("distance " << distance << endreq << "fitted dist from primary vertex " <<
                 m_a_lambda/fabs(m_a_lambda)*sqrt(std::pow(m_a_px*m_a_lambda,2)+
                 std::pow(m_a_py*m_a_lambda,2)+std::pow(m_a_pz*m_a_lambda,2)) );

  distanceOnAxis=m_a_lambda/fabs(m_a_lambda)*sqrt(std::pow(m_a_px*m_a_lambda,2)+
						  std::pow(m_a_py*m_a_lambda,2)+
						  std::pow(m_a_pz*m_a_lambda,2));

  Amg::Vector3D r1(x1fin,y1fin,z1fin);
  Amg::Vector3D r2(x2fin,y2fin,z2fin);

  return std::pair<Amg::Vector3D,double>(r2,distance);//give back position on neutral track... (jet axis)
  
}
  
}
