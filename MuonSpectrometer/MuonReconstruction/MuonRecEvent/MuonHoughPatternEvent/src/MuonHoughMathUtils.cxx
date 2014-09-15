/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"
#include "CxxUtils/sincos.h"

#include <sstream>
#include <iostream>
#include <cassert>

MuonHoughMathUtils::MuonHoughMathUtils()
{
}

MuonHoughMathUtils::~MuonHoughMathUtils()
{
}

int MuonHoughMathUtils::sgn(double d)const
{
  if (d<0) {return -1;}
  //  if (d==0) {return 0;} //i know, its the definition, but we dont want it
  if (d>=0) {return 1;}
  return 666;
}

int MuonHoughMathUtils::step(double d, double x0)const
{
  if (d==x0) {std::cout << "WARNING: Possible mistake in Step function" << std::endl;}
  if (d<=x0) {return 0;}
  if (d>x0) {return 1;}
  return -1;
}

double MuonHoughMathUtils::signedDistanceToLine(double x0, double y0, double r0, double phi)const //distance from (x0,y0) to the line (r0,phi) , phi in [-Pi,Pi] ,different phi than in calculateangle() (==angle(2))
{
  CxxUtils::sincos scphi(phi);
  double distance = scphi.apply(x0,-y0) - r0; //x0*scphi.sn-y0*scphi.cs-r0;
  return distance;
}

double MuonHoughMathUtils::distanceToLine(double x0,  double y0, double r0, double phi)const
{
  return std::abs(signedDistanceToLine(x0,y0,r0,phi));
}

double MuonHoughMathUtils::incrementTillAbove0(double x, double inc, double zero)const
{
  while(x > inc + zero ) 
    {x-=inc;}
  while(x < zero )
    {x+=inc;}
  return x;
}

double MuonHoughMathUtils::angleFrom0To360(double angle)const
{
  return incrementTillAbove0(angle,360.);
}

double MuonHoughMathUtils::angleFrom0To180(double angle)const
{
  return incrementTillAbove0(angle,180.);
}

double MuonHoughMathUtils::angleFrom0ToPi(double angle)const
{
  return incrementTillAbove0(angle,MuonHough::Pi);
}

double MuonHoughMathUtils::angleFromMinusPiToPi(double angle)const
{
  return incrementTillAbove0(angle,2*MuonHough::Pi,-MuonHough::Pi);
}

std::string MuonHoughMathUtils::intToString(int i)const
{
  std::string s;
  std::stringstream ss;
  ss << i;
  ss >> s;

  return s;
}

const char * MuonHoughMathUtils::stringToChar(std::string& string)const
{
  const char * constcharstar = string.data();
  return constcharstar;
}

const char * MuonHoughMathUtils::intToChar(int i)const
{
  std::string string = intToString(i);
  const char * constcharstar = stringToChar(string);
  return constcharstar;
}

double MuonHoughMathUtils::distanceToLine2D(double x0, double y0, double r0, double phi)const // distance from (x0,y0) to line (r,phi) // from mathworld.wolfram.com/Point-LineDistance2-Dimensional.html // better to use distancetoline() 
{
  // need two points on line:
  
  CxxUtils::sincos scphi(phi);

  double x1 = - r0 * scphi.sn; // point closest to origin
  double y1 = r0 * scphi.cs;

  std::vector <double> v(3); // (p1 - p0)
  
  v[0] = x1-x0;
  v[1] = y1-y0;

  std::vector <double> r(3); // vector perpendicular to line
  
  r[0] = x1; 
  r[1] = y1;

  double distance = dotProduct(r,v)/abs(r);
  
  return distance;
}

double MuonHoughMathUtils::distanceToLine3D(double x0,double y0,double z0, double x, double y, double z, double phi, double theta)const // from wolfram: http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
{
  std::vector <double> x1(3); // x1-x0
  std::vector <double> x3(3); // x2-x1 = e_r

  x1[0]=x-x0;
  x1[1]=y-y0;
  x1[2]=z-z0;

  CxxUtils::sincos scphi(phi);
  CxxUtils::sincos sctheta(theta);

  x3[0]= scphi.cs*sctheta.sn;
  x3[1]= scphi.sn*sctheta.sn;
  x3[2]= sctheta.cs;

  // sqrt(x3^2) == 1; !
 
  double distance;
  std::vector<double> x4(3);
  x4 = crossProduct(x3,x1);

  distance = std::sqrt(dotProduct(x4,x4))/(std::sqrt(dotProduct(x3,x3)));
  
  return distance;
}

double MuonHoughMathUtils::distanceOfLineToOrigin2D(double a, double b)const
{
  //  return signedDistanceOfLineToOrigin2D(0,b,std::atan(a));

  return std::abs(b/(std::sqrt(a*a+1)));
}

double MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(double x, double y, double phi)const
{
  CxxUtils::sincos scphi(phi);
  return scphi.apply(x,-y); //(-y*scphi.cs+x*scphi.sn);
}

std::vector<double> MuonHoughMathUtils::shortestPointOfLineToOrigin3D(double x, double y, double z, double phi, double theta)const // actually this is the 3d-point closest to origin in xy-plane
{
  std::vector <double> p(3);

  double r0 = signedDistanceOfLineToOrigin2D(x,y,phi);

  CxxUtils::sincos scphi(phi);

  double x0 = r0*scphi.sn;
  double y0 = -r0* scphi.cs;

  double radius = std::sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
  
  if ( std::abs((y-y0) - scphi.sn*radius) > std::abs((y-y0) + scphi.cs*radius) ) // also possible for x
    {
      radius=-radius;
    }

  double z0 = z - radius / std::tan(theta);
  
  p[0]=x0;
  p[1]=y0;
  p[2]=z0;

  return p;
}

std::vector<double> MuonHoughMathUtils::shortestPointOfLineToOrigin(double x, double y, double z, double phi, double theta)const // from wolfram: http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
{
  // origin: 
  std::vector <double> x0(3);
  x0[0]=0;
  x0[1]=0;
  x0[2]=0;

  std::vector <double> x1(3); // x1-x0
  std::vector <double> x3(3); // x2-x1

  x1[0]=x-x0[0];
  x1[1]=y-x0[1];
  x1[2]=z-x0[2];

  CxxUtils::sincos scphi(phi);
  CxxUtils::sincos sctheta(theta);

  x3[0]= scphi.cs*sctheta.sn;
  x3[1]= scphi.sn*sctheta.sn;
  x3[2]= sctheta.cs;

  // sqrt(x3^2) == 1; !

  double time=0;
  double x5=0;
  x5 = dotProduct(x1,x3);

  time = - x5 / (dotProduct(x3,x3));

  std::vector <double> p(3);

  p[0]=x1[0]+x3[0]*time;
  p[1]=x1[1]+x3[1]*time;  
  p[2]=x1[2]+x3[2]*time;

  return p;
}

bool MuonHoughMathUtils::lineThroughCylinder(double x_0, double y_0, double z_0, double phi, double theta, double r_cyl, double z_cyl)const
{
   // if there is one, then track will be split
  // C:x^2+y^2=r_cyl^2 , |z|<z_cyl
  // l:(x_0,y_0,z_0)+t*(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta))

  //  assert(z_cyl>=0);
  assert(r_cyl>=0);

  CxxUtils::sincos scphi(phi);
  CxxUtils::sincos sctheta(theta);
  
  // 1 -- check if there is an intersection at z0=+-c0
  double p_1 = z_0 - z_cyl;
  double p_2 = z_0 + z_cyl;
  double tantheta = sctheta.sn/sctheta.cs;

  double x_1 = x_0 - p_1*scphi.cs*tantheta;
  double y_1 = y_0 - p_1*scphi.sn*tantheta;
  double r_1 = std::sqrt(x_1*x_1+y_1*y_1);
  if (r_1<r_cyl) {return true;}

  double x_2 = x_0 - p_2*scphi.cs*tantheta;
  double y_2 = y_0 - p_2*scphi.sn*tantheta;
  double r_2 = std::sqrt(x_2*x_2+y_2*y_2);
  if (r_2<r_cyl) {return true;}

  // 2 -- check if there is an intersection with the circle x^2 + y^2 = r_cyl^2 and the line y=px+q, p = tan(phi), q = y_0 - x_0 tan(phi) <--> r_cyl^2 = (px+q)^2 + x^2
  // D = b^2-4ac   a= (p^2+1) = (1/cos^2), b = 2pq, c = q^2-r_cyl^2 
  // D = 4 (r_cyl^2 * a - q^2)
  // D = 4 /cos^2 (r_cyl^2 - (y_0*cos - x_0*sin)^2)
  
  double r_0 = scphi.apply(-x_0,y_0); // y_0*scphi.cs - x_0*scphi.sn;

  // D = 4* (scphi.cs*scphi.cs)* ( r_cyl*r_cyl  - r_0*r_0)
  
  // D>0 <--> |r_0| < r_cyl

  if (std::abs(r_0) > r_cyl) return false; 

  // 3 -- check if the intersection is cylinder: for -z_cyl<z<z_cyl

  // x_1,2 = (-b+-sqrt(D) / 2a) = .. = - sin * r_0 +- cos * sqrt(r_cyl^2-r_0^2)

  double s_1 = - scphi.sn * r_0;
  double s_2 = scphi.cs * std::sqrt(r_cyl*r_cyl-r_0*r_0);

  x_1 = s_1 + s_2;

  // t = (x-x_0)/cos(phi)* sin(theta)
  // z = z_0 + t * cos(theta) = z_0 + (x-x_0) * cos(theta) / (cos(phi) * sin(theta) )

  double inv_angle = 1/(scphi.cs * tantheta);

  double z_1 = z_0 + (x_1-x_0) * inv_angle;

  if (std::abs(z_1) < z_cyl) return true; 

  x_2 = s_1 - s_2;
  double z_2 = z_0 + (x_2-x_0) * inv_angle;

  if (std::abs(z_2) < z_cyl) return true; 

  return false;

}

std::vector<double> MuonHoughMathUtils::crossProduct(std::vector <double> x, std::vector<double> y)const
{
  std::vector<double> z(3);
  z[0]=y[1]*x[2]-y[2]*x[1];
  z[1]=y[2]*x[0]-y[0]*x[2];
  z[2]=y[0]*x[1]-y[1]*x[0];

  return z;
}

double MuonHoughMathUtils::dotProduct(std::vector <double> x, std::vector<double> y) const
{
  double z;
  z = y[0]*x[0] + y[1]*x[1] + y[2]*x[2];

  return z;
}
double MuonHoughMathUtils::signedDistanceCurvedToHit(double z0, double theta, double invcurvature, double hitx, double hity , double hitz ) const
{
  double hitr = std::sqrt(hitx*hitx+hity*hity);

  CxxUtils::sincos sctheta(theta);

  double sdistance = 100000000.;
  // if angle rotation larger than Pi/2 then return large distance (formulas don't work for flip in z!)
  if (sctheta.apply(hitr,hitz) < 0) return sdistance; //  hitr*sctheta.sn + hitz*sctheta.cs < 0 

  int sign = 1;
  if (hitz < 0) sign = -1;

  if (std::abs(hitr/hitz)>MuonHough::tan_barrel) {
    // Barrel Extrapolation
    if (std::abs(sctheta.sn) > 1e-7) {
      double diffr = hitr-MuonHough::radius_cylinder;
      double zext = z0 + (hitr*sctheta.cs + diffr*diffr*invcurvature)/sctheta.sn;
      sdistance = (zext - hitz);
    }

  } else {
    if (std::abs(sctheta.sn) > 1e-7) {
      double rext=0.;
      if ( std::abs(hitz) < MuonHough::z_end) {
	// Forward in Toroid
	double diffz = hitz-sign*MuonHough::z_cylinder;
	rext = ((hitz-z0)*sctheta.sn - diffz*diffz*invcurvature)/sctheta.cs;
	
      } else {
	// Forward OutSide EC Toroid
	rext = ((hitz-z0)*sctheta.sn + (MuonHough::z_magnetic_range_squared - sign*2*hitz*(MuonHough::z_magnetic_range))*invcurvature)/sctheta.cs;
      }
      sdistance = (rext - hitr);
    }
  }
  return sdistance;
}

double MuonHoughMathUtils::thetaForCurvedHit(double invcurvature, MuonHoughHit* hit) const
{
  double ratio = hit->getMagneticTrackRatio()*invcurvature;
  if (std::abs(ratio) < 1.) return hit->getTheta() + std::asin (ratio); 
  else return -1;
}

void MuonHoughMathUtils::thetasForCurvedHit(double ratio, MuonHoughHit* hit, double& theta1, double& theta2)const
{
  /** returns angle for positive and negative curvature (positive first) */

  if (std::abs(ratio) < 1.) {
    double asin_ratio = std::asin(ratio);
    theta1 = hit->getTheta() + asin_ratio;
    theta2 = hit->getTheta() - asin_ratio;
  }
}

void MuonHoughMathUtils::extrapolateCurvedRoad(const Amg::Vector3D& roadpos, const Amg::Vector3D& roadmom,  const Amg::Vector3D& pos, Amg::Vector3D& roadpose , Amg::Vector3D& roaddire)const
{
  /** Extrapolate pattern given by a roadpos and roadmom (should be perigee)
      to a position in space pos
      And determine extrapolated position: roadpose 
      and direction: roaddire 
      using the curved track model
  */

  //  m_log<< MSG::VERBOSE << "Extrapolate the road to the segment (hit)" <<endreq;

  const double theta = roadmom.theta(); 
  const double phi = roadmom.phi(); 

  CxxUtils::sincos scphi(phi);
  CxxUtils::sincos sctheta(theta);
  
  double tantheta = sctheta.sn/sctheta.cs;

  double r0 = scphi.apply(roadpos.x(),-roadpos.y()); // - roadpos.y()*scphi.cs + roadpos.x()*scphi.sn ;
  double charge = 1.;
  if ( r0 < 0) charge = -1.;
  double invcurvature =  charge/roadmom.mag();
  // No momentum estimate 
  if (roadmom.mag() < 2 ) invcurvature = 0.;

  double posr = std::sqrt(pos.x()*pos.x()+pos.y()*pos.y());
  double thetan = theta;

  int sign = 1;
  if (pos.z() < 0) sign = -1;
  
  double xe = pos.x();
  double ye = pos.y();
  double ze = pos.z();
  double rotationangle=0.;

  if (std::abs(posr/pos.z())>MuonHough::tan_barrel) {
    // Barrel Extrapolation
    if ( (posr*posr-r0*r0) > 0) {
      double lenr = std::sqrt(posr*posr-r0*r0);
      double len = posr - fabs(r0);
      double diffr = posr-MuonHough::radius_cylinder;
      rotationangle = diffr*invcurvature/sctheta.sn;
      //      std::cout << " road x " << roadpos.x()<< " y "  << roadpos.y() << " z " << roadpos.z() << std::endl;
      //      std::cout << " Position xe " << xe << " y " << ye << " z " << ze << std::endl;
      xe = roadpos.x() + lenr * scphi.cs;
      ye = roadpos.y() + lenr * scphi.sn;
      ze = roadpos.z() + len/tantheta + diffr*rotationangle;
      thetan = std::atan2(1.,1/tantheta + 2*rotationangle);
      //      std::cout << " Extrapolated Position  xe " << xe << " y " << ye << " z " << ze << std::endl;
      //     m_log << MSG::VERBOSE << " Barrel extrapolation " <<endreq;
    }
  } else {
    double lext=0., rotationangle=0.;
    if ( std::abs(pos.z()) < MuonHough::z_end) {
      // Forward in Toroid
      double diffz = pos.z()-sign*MuonHough::z_cylinder;
      rotationangle = diffz*invcurvature/sctheta.cs;
      lext = (pos.z()-roadpos.z())*tantheta - diffz*rotationangle;
      // m_log << MSG::VERBOSE << " Forward extrapolation " <<endreq;
      
    } else {
      // Forward OutSide EC Toroid
      double effcurv = invcurvature/sctheta.cs;
      rotationangle = sign*(MuonHough::z_end-MuonHough::z_cylinder)*effcurv;
      lext = (pos.z()-roadpos.z())*tantheta + (MuonHough::z_magnetic_range_squared - 2*sign*pos.z()*MuonHough::z_magnetic_range)*effcurv;
      // m_log << MSG::VERBOSE << " Forward Outside EC Toroid extrapolation " <<endreq;
    }
    xe = roadpos.x() + lext * scphi.cs;
    ye = roadpos.y() + lext * scphi.sn;
    //ze = pos.z();
    double dx = tantheta - 2*rotationangle;
    if (dx !=0)  thetan = std::atan2(1.,1/dx);
  }

// In case direction theta is rotated for low momenta: flip direction vector 
  CxxUtils::sincos scthetan(thetan);

   //   std::cout << " thetan: " << thetan << " rotationangle: " << rotationangle << std::endl;

   if (sctheta.cs*scthetan.cs+sctheta.sn*scthetan.sn < 0) {
     //     std::cout<< " flip direction" << std::endl;
     roaddire = Amg::Vector3D(scthetan.sn*scphi.cs,scthetan.sn*scphi.sn,-scthetan.cs);
   } else {
     //     std::cout<< " no flip" << std::endl;
     roaddire = Amg::Vector3D(scthetan.sn*scphi.cs,scthetan.sn*scphi.sn,scthetan.cs);
   }
   roadpose = Amg::Vector3D(xe,ye,ze);
   
//    std::cout << " direction theta " << theta << " new " << thetan << std::endl;
//    if (std::abs(theta-thetan) > 0.1 ) std::cout << " Large theta difference " << std::endl;
//    std::cout << " position x " << pos.x() << " y  " << pos.y() << " r " << posr << " z " << pos.z() << std::endl;
//    double dr = std::sqrt(roadpose.x()*roadpose.x() + roadpose.y()*roadpose.y()) - posr;
//    double dz = roadpose.z() - pos.z();
//    double dist = std::sqrt(dr*dr+dz*dz); 
//    std::cout << " Extrapol position x " << roadpose.x() << " y  " << roadpose.y() << " r " << std::sqrt(roadpose.x()*roadpose.x() + roadpose.y()*roadpose.y()) <<" z " << roadpose.z() << " Distance " << dist << std::endl;
//    if ( dist > 100.) std::cout << " Large Distance " << dist << " dR " << dr << " dZ " << dz << std::endl;    
//    std::cout << " charge " << charge << " curvature " << roadmom.mag() << " inv curvature " << invcurvature << std::endl;

}
