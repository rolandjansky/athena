/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// to get AmgMatrix plugin:
#include "GeoPrimitives/GeoPrimitives.h"

#include "MagFieldElements/AtlasFieldCache.h"
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
  m_maxloopnumber(20)
{
  declareProperty("Precision",m_precision);
  declareProperty("MaxLoops",m_maxloopnumber);
  declareInterface<TrkDistanceFinderNeutralCharged>(this);
  
}


StatusCode TrkDistanceFinderNeutralCharged::initialize() 
  { 
    StatusCode s = AthAlgTool::initialize();
    ATH_MSG_INFO("Initialize successful");
    return StatusCode::SUCCESS;
  }

StatusCode TrkDistanceFinderNeutralCharged::finalize() 
{
  ATH_MSG_INFO("Finalize successful");
  return StatusCode::SUCCESS;
}


TrkDistanceFinderNeutralCharged::~TrkDistanceFinderNeutralCharged() = default;

std::pair<Amg::Vector3D,double>  
TrkDistanceFinderNeutralCharged::getPointAndDistance(const Trk::NeutralTrack& neutraltrk,
                                                     const Trk::Perigee& chargedtrk,
                                                     double & distanceOnAxis,
                                                     MagField::AtlasFieldCache &fieldCache) const {

  double b_phi0=chargedtrk.parameters()[Trk::phi0];
  double b_cosphi0=cos(b_phi0);
  double b_sinphi0=sin(b_phi0);
  double b_x0=chargedtrk.associatedSurface().center().x()-
    chargedtrk.parameters()[Trk::d0]*b_sinphi0;
  double b_y0=chargedtrk.associatedSurface().center().y()+
    chargedtrk.parameters()[Trk::d0]*b_cosphi0;
  double b_z0=chargedtrk.associatedSurface().center().z()+
    chargedtrk.parameters()[Trk::z0];

  Amg::Vector3D magnFieldVect;
  fieldCache.getField(chargedtrk.associatedSurface().center().data(),magnFieldVect.data());

  //Magnetic field at (x0,y0,z0)
  double Bz=magnFieldVect.z()*299.792;//B field in Gev/mm

  double b_Rt=getRadiusOfCurvature(chargedtrk,Bz);

  double b_cottheta=1./tan(chargedtrk.parameters()[Trk::theta]);
  
  //for neutrals
  double a_x0=neutraltrk.position()[0];
  double a_y0=neutraltrk.position()[1];
  double a_z0=neutraltrk.position()[2];
  double a_px=neutraltrk.momentum()[0];
  double a_py=neutraltrk.momentum()[1];
  double a_pz=neutraltrk.momentum()[2];

  
  //more elaborate pieces for later
  double DxNoVar=b_x0+b_Rt*b_sinphi0-a_x0;
  double DyNoVar=b_y0-b_Rt*b_cosphi0-a_y0;
  double DzNoVar=b_z0+b_Rt*b_cottheta*b_phi0-a_z0;

  //start in both cases from the vertex
  double b_phi=b_phi0;
  double b_cosphi=cos(b_phi);
  double b_sinphi=sin(b_phi);
  double a_lambda=0.;

  double deltab_phi=0.;
  double deltaa_lambda=0.;

  int loopsnumber=0;

  bool isok=false;

  bool secondTimeSaddleProblem=false;

  while (!isok) {

    //    std::cout << "Loop number: " << loopsnumber << std::endl;
    //    std::cout << "phi is: " << b_phi << " lambda " << a_lambda << std::endl;

    //ONLY FOR DEBUG * from here

    double x1fin=b_x0+b_Rt*(sin(b_phi0)-sin(b_phi));
    double y1fin=b_y0+b_Rt*(cos(b_phi)-cos(b_phi0));
    double z1fin=b_z0+b_Rt*b_cottheta*(b_phi0-b_phi);
    
    double x2fin=a_x0+a_px*a_lambda;
    double y2fin=a_y0+a_py*a_lambda;
    double z2fin=a_z0+a_pz*a_lambda;
    
    ATH_MSG_VERBOSE ("x1fin " << x1fin << "y1fin " << y1fin << "z1fin " << z1fin);
    ATH_MSG_VERBOSE ("x2fin " << x2fin << "y2fin " << y2fin << "z2fin " << z2fin);
    
    double distance = sqrt((x1fin-x2fin)*(x1fin-x2fin)+
			   (y1fin-y2fin)*(y1fin-y2fin)+
			   (z1fin-z2fin)*(z1fin-z2fin));

    ATH_MSG_VERBOSE ("distance " << distance);

    //to here (please delete afterwards)

    loopsnumber+=1;

    double d1db_phi=((DxNoVar-a_px*a_lambda)*(-b_Rt*b_cosphi)+
		       (DyNoVar-a_py*a_lambda)*(-b_Rt*b_sinphi)+
		       (DzNoVar-b_Rt*b_phi*b_cottheta-a_pz*a_lambda)*(-b_Rt*b_cottheta));

    double d1da_lambda=((DxNoVar-b_Rt*b_sinphi-a_px*a_lambda)*(-a_px)+
			  (DyNoVar+b_Rt*b_cosphi-a_py*a_lambda)*(-a_py)+
			  (DzNoVar-b_Rt*b_cottheta*b_phi-a_pz*a_lambda)*(-a_pz));

    //second derivatives (d^2/d^2(a) d^2/d^2(b) d^2/d(a)d(b) )

    double d2db_phi2=((DxNoVar-a_px*a_lambda)*(b_Rt*b_sinphi)+
			(DyNoVar-a_py*a_lambda)*(-b_Rt*b_cosphi)+
			(b_Rt*b_cottheta*b_Rt*b_cottheta));
    
    double d2dadb_lambdaphi=a_px*b_Rt*b_cosphi+a_py*b_Rt*b_sinphi+a_pz*b_Rt*b_cottheta;

    //    double m_d2da_lambda2=((DxNoVar-b_Rt*b_sinphi-a_px*a_lambda)*(a_px*a_px)+
    //		   (DyNoVar+b_Rt*b_cosphi-a_py*a_lambda)*(a_py*a_py)+
    //		   (DzNoVar-b_Rt*b_cottheta*b_phi-a_pz*a_lambda)*(a_pz*a_pz));

    //small error :-)

    double d2da_lambda2=((a_px*a_px)+
			   (a_py*a_py)+
			   (a_pz*a_pz));
    

    ATH_MSG_VERBOSE (" d1db_phi: " << d1db_phi << " d1da_lambda " << d1da_lambda);

    ATH_MSG_VERBOSE (" d2phi2 " << d2db_phi2 << " d2lambda2 " <<d2da_lambda2  << 
        " d2lambdaphi " << d2dadb_lambdaphi);

    
    double det=d2da_lambda2*d2db_phi2-d2dadb_lambdaphi*d2dadb_lambdaphi;

    if (det<0) {
      if (!secondTimeSaddleProblem) 
      {
        secondTimeSaddleProblem=true;

        ATH_MSG_DEBUG ("Now try first to recover from the problem");
        double denominator=1./((a_px*a_px)+(a_py*a_py));
        double firstTerm=(DxNoVar*a_px+DyNoVar*a_py)*denominator;
        double toSquare=(a_px*DyNoVar-a_py*DxNoVar);
        double toSqrt=b_Rt*b_Rt/denominator-toSquare*toSquare;
        if (toSqrt<0)
        {
          ATH_MSG_WARNING ("No intersection between neutral and charged track can be found. SKipping...");
          continue;
        }
        
        double secondTerm=TMath::Sqrt(toSqrt)*denominator;
        double lambda1=firstTerm+secondTerm;
        double lambda2=firstTerm-secondTerm;

        ATH_MSG_VERBOSE ("first solution: " << lambda1 << " second solution " << lambda2);
        
        double phi1=TMath::ATan2((DxNoVar-a_px*lambda1)/b_Rt,-(DyNoVar-a_py*lambda1)/b_Rt);
        double phi2=TMath::ATan2((DxNoVar-a_px*lambda2)/b_Rt,-(DyNoVar-a_py*lambda2)/b_Rt);

        double x1case1=b_x0+b_Rt*(sin(b_phi0)-sin(phi1));
        double y1case1=b_y0+b_Rt*(cos(phi1)-cos(b_phi0));
        double z1case1=b_z0+b_Rt*b_cottheta*(b_phi0-phi1);
        
        double x2case1=a_x0+a_px*lambda1;
        double y2case1=a_y0+a_py*lambda1;
        double z2case1=a_z0+a_pz*lambda1;

        double x1case2=b_x0+b_Rt*(sin(b_phi0)-sin(phi2));
        double y1case2=b_y0+b_Rt*(cos(phi2)-cos(b_phi0));
        double z1case2=b_z0+b_Rt*b_cottheta*(b_phi0-phi2);
        
        double x2case2=a_x0+a_px*lambda2;
        double y2case2=a_y0+a_py*lambda2;
        double z2case2=a_z0+a_pz*lambda2;

        ATH_MSG_VERBOSE ("solution1 x1: " << x1case1 << " x2: " << x2case1 << " y1: " << y1case1 << " y2: " << y2case1);
        ATH_MSG_VERBOSE ("solution2 x1: " << x1case2 << " x2: " << x2case2 << " y1: " << y1case2 << " y2: " << y2case2);

        double deltaz1=fabs(z2case1-z1case1);
        double deltaz2=fabs(z2case2-z1case2);

        ATH_MSG_VERBOSE (" deltaz1: " << deltaz1 << " deltaz2: " << deltaz2);

        if (deltaz1<=deltaz2)
        {
          b_phi=phi1;
          a_lambda=lambda1;
        }
        else
        {
          b_phi=phi2;
          a_lambda=lambda2;
        }
        //store cos and sin of phi
        b_cosphi=cos(b_phi);
        b_sinphi=sin(b_phi);
        continue;
      }
      
      
        ATH_MSG_WARNING ("Hessian is negative: saddle point");
        throw Error::NewtonProblem("Hessian is negative");
      
    }
    if (det>0&&d2da_lambda2<0) {
      ATH_MSG_WARNING("Hessian indicates a maximum: derivative will be zero but result incorrect");
      throw Error::NewtonProblem("Maximum point found");
    }

    //Now apply the Newton formula in more than one dimension
    deltaa_lambda=-(d2db_phi2*d1da_lambda-d2dadb_lambdaphi*d1db_phi)/det;
    deltab_phi=-(-d2dadb_lambdaphi*d1da_lambda+d2da_lambda2*d1db_phi)/det;

    a_lambda+=deltaa_lambda;
    b_phi+=deltab_phi;
    
    //store cos and sin of phi
    b_cosphi=cos(b_phi);
    b_sinphi=sin(b_phi);

    if (sqrt(std::pow(deltaa_lambda,2)+std::pow(deltab_phi,2))<m_precision || 
	loopsnumber>m_maxloopnumber) isok=true;

  }
  
  if (loopsnumber>m_maxloopnumber) throw Error::NewtonProblem("Could not find minimum distance: max loops number reached"); //now return error, see how to do it...
  
  double x1fin=b_x0+b_Rt*(sin(b_phi0)-sin(b_phi));
  double y1fin=b_y0+b_Rt*(cos(b_phi)-cos(b_phi0));
  double z1fin=b_z0+b_Rt*b_cottheta*(b_phi0-b_phi);
  
  double x2fin=a_x0+a_px*a_lambda;
  double y2fin=a_y0+a_py*a_lambda;
  double z2fin=a_z0+a_pz*a_lambda;

  ATH_MSG_VERBOSE ("x1fin " << x1fin << "y1fin " << y1fin << "z1fin " << z1fin);
  ATH_MSG_VERBOSE ("x2fin " << x2fin << "y2fin " << y2fin << "z2fin " << z2fin);

  double distance = sqrt((x1fin-x2fin)*(x1fin-x2fin)+
			 (y1fin-y2fin)*(y1fin-y2fin)+
			 (z1fin-z2fin)*(z1fin-z2fin));

  ATH_MSG_DEBUG ("distance " << distance << std::endl 
                 << "fitted dist from primary vertex " <<
                 a_lambda/fabs(a_lambda)*sqrt(std::pow(a_px*a_lambda,2)+
                 std::pow(a_py*a_lambda,2)+std::pow(a_pz*a_lambda,2)) );

  distanceOnAxis=a_lambda/fabs(a_lambda)*sqrt(std::pow(a_px*a_lambda,2)+
						  std::pow(a_py*a_lambda,2)+
						  std::pow(a_pz*a_lambda,2));

  Amg::Vector3D r1(x1fin,y1fin,z1fin);
  Amg::Vector3D r2(x2fin,y2fin,z2fin);

  return std::pair<Amg::Vector3D,double>(r2,distance);//give back position on neutral track... (jet axis)
  
}
  
}
