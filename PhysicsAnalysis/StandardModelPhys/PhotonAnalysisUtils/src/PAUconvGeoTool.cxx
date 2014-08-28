/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tool to compute geometrical selection variables for converted photons.
// Author: Kerstin Tackmann, Thomas Koffas 

#include "PhotonAnalysisUtils/PAUconvGeoTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "InDetConversionFinderTools/VertexPointEstimator.h"

#include "TMath.h"


using CLHEP::Hep3Vector;


#define UNKNOWN -99999

// constructor
PAUconvGeoTool::PAUconvGeoTool(const std::string& t, const std::string& n, const IInterface* p) : 
  AthAlgTool(t,n,p)
{
  declareInterface<IPAUconvGeoTool>(this);
  declareProperty("DistanceTool", m_distanceTool);
  declareProperty("VertexPointEstimator", m_vertexEstimator);
}

// destructor
PAUconvGeoTool::~PAUconvGeoTool()
{}

// Athena standard methods

// initialize
StatusCode PAUconvGeoTool::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initializing PAUconvGeoTool ..." << endreq;

  //Get the distance tool from ToolsSvc
  if(m_distanceTool.retrieve().isFailure()) {
    log << MSG::WARNING << "Could not get " << m_distanceTool << endreq; 
    return StatusCode::SUCCESS;
  }else{
    log << MSG::INFO << "Got the distance tool " << m_distanceTool << endreq;
  }

  //Get the vertex estimator from ToolsSvc
  if(m_vertexEstimator.retrieve().isFailure()) {
    log << MSG::WARNING << "Could not get " << m_vertexEstimator << endreq; 
    return StatusCode::SUCCESS;
  }else{
    log << MSG::INFO << "Got the vertex estimator " << m_vertexEstimator << endreq;
  }

  return StatusCode::SUCCESS;
}

// finalize
StatusCode PAUconvGeoTool::finalize()
{
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  return StatusCode::SUCCESS;
}


const std::vector<float> PAUconvGeoTool::convGeoVars(const PAU::egamma* g) const {
  const Trk::VxCandidate* vtx = (g ? g->convVx():0); 
  return convGeoVars(vtx);
}

const std::vector<float> PAUconvGeoTool::convGeoVars(const Analysis::Photon* g) const {
  const Trk::VxCandidate* vtx = (g ? g->conversion():0); 
  return convGeoVars(vtx);
}

const std::vector<float> PAUconvGeoTool::convGeoVars(const Trk::VxCandidate* vtx) const {

  std::vector<float> geoVars;

  if(vtx){
    std::vector<Trk::VxTrackAtVertex*> trks(*(vtx->vxTrackAtVertex()));
    int numTracksAtVertex = trks.size();
    if(numTracksAtVertex==2){//only double-track conversions
      
      //get the vertex chi2 first
      geoVars.push_back((float)vtx->recVertex().fitQuality().chiSquared());
      
      //get the perigees before the vertex fit
      std::vector<const Trk::Perigee*> initPerigee;
      for(int i=0; i<numTracksAtVertex; i++){
	Trk::VxTrackAtVertex* trk = trks[i];
	const Trk::TrackParameters* perigeeparm = trk->initialPerigee();
	if(Trk::Surface::Perigee == perigeeparm->associatedSurface().type()){
	  
	  const Trk::PerigeeSurface* surf = (const Trk::PerigeeSurface*) &(perigeeparm->associatedSurface());
	  const Trk::Perigee* perigee = new Trk::Perigee(perigeeparm->parameters()[Trk::loc1], 
							 perigeeparm->parameters()[Trk::loc2], 
							 perigeeparm->parameters()[Trk::phi], 
							 perigeeparm->parameters()[Trk::theta], 
							 perigeeparm->parameters()[Trk::qOverP], 
							 *surf);
	  if (perigee != 0) initPerigee.push_back(perigee);
	  else  initPerigee.push_back(0);
	}
	else  initPerigee.push_back(0);
      }
      
      if(initPerigee.at(0) && initPerigee.at(1)){
	//minimum distance between the two tracks
	//if(m_distanceTool && m_distanceTool->CalculateMinimumDistance(*(initPerigee.at(0)), *(initPerigee.at(1))))
	//  geoVars.push_back((float)m_distanceTool->GetDistance());  
	//else
	geoVars.push_back(UNKNOWN);
	
	//opening angle in z--eta plane 
	geoVars.push_back(dCotTheta((float)initPerigee.at(0)->parameters()[Trk::theta], (float)initPerigee.at(1)->parameters()[Trk::theta]));
	
	// dphi and D-R1-R2 at the intersection point
	double pt0 = -999999; 
	double pt1 = -999999;
	double m_ctvDr   = -999999;
	double m_ctvRmax = -999999;
	double m_ctvDz   = -999999;
	double m_ctvArc  = -999999;
	double m_DR1R2  = -999999;
	double m_area    = -999999;
	double m_h       = -999999;
	double m_hl      = -999999;
	double m_ddphi   = -999999;
	int tt = 0;
	
	if(initPerigee.at(0)->parameters()[Trk::qOverP] !=0) pt0=TMath::Abs(TMath::Sin(initPerigee.at(0)->parameters()[Trk::theta])/initPerigee.at(0)->parameters()[Trk::qOverP]);
	if(initPerigee.at(1)->parameters()[Trk::qOverP] !=0) pt1=TMath::Abs(TMath::Sin(initPerigee.at(1)->parameters()[Trk::theta])/initPerigee.at(1)->parameters()[Trk::qOverP]);

	Amg::Vector3D startingPoint(0.,0.,0.);
	int sflag = 0;
	if(m_vertexEstimator){
	  if (pt0 < pt1) {
	    m_vertexEstimator->getStartingPoint(initPerigee.at(0),initPerigee.at(1),startingPoint,sflag,tt,m_ctvDr,m_ctvRmax,m_ctvDz,m_ctvArc,m_DR1R2,m_area,m_h,m_hl,m_ddphi);
	  } else {
	    m_vertexEstimator->getStartingPoint(initPerigee.at(1),initPerigee.at(0),startingPoint,sflag,tt,m_ctvDr,m_ctvRmax,m_ctvDz,m_ctvArc,m_DR1R2,m_area,m_h,m_hl,m_ddphi);
	  }
	}
	geoVars.push_back((float)m_ddphi);
	geoVars.push_back((float)m_DR1R2);
      }else{ //don't have both perigees, so only have the chi2
	for(int i=0; i<4; i++)
	  geoVars.push_back(UNKNOWN);
      }
    }else{//non-double-track conversions push back dummy values
      for(int i=0; i<5; i++)
	geoVars.push_back(UNKNOWN);
    }
  }else{//zero-pointer for VxCandidate
    for(int i=0; i<5; i++)
      geoVars.push_back(UNKNOWN);
  }
  return geoVars;
}


// calculates the difference between the cot(theta) of two tracks
float PAUconvGeoTool::dCotTheta(float theta1, float theta2) const{
  if (tan(theta1) != 0. && tan(theta2) != 0.){
    return (1./tan(theta1) - 1./tan(theta2));
  }
  return -999999999.;
}

// 	/** circles intersection point calculation */
// ///////////////////////////////////////////////////////////
// /// this is what appears in the ConversionFinder code
// ///
// ///
// void PAUconvGeoTool::getStartingPoint(const Trk::TrackParameters *per1, const Trk::TrackParameters *per2, int sit, double& m_MDdr, double& m_ddphi) const
// {
//   /*
//     Calculates the initial approximation to the vertex position. Based on CTVMFT.
//     Tries to intersect circles that are (R,Phi) projections of the helical trajectories of the two tracks
//     If the circles intersect, the intersection with smaller z difference between the 2 helices extrapolated to the intersection is chosen.
//     If the circles do not intersect, the vertex approximation is taken as the point of closest appraoch of the two circles.
//     per1 should be the track with the smaller radius
//   */
  
//   int sflag(0);
//   double m_ctvDr, m_ctvRmax, m_ctvDz, m_ctvArc, m_area, m_h, m_hl;
  
//   double bmagnt = 2.083;
//   double aconst = -1.49898*bmagnt;
//   double DRMAX ; // = 0.; // maximum XY separation, non-intersecting circles
//   double DZMAX ; // = 0.; // maximum allowed track Z separation at the vertex
//   double RVMAX ; // = 0.; // maximum allowed vertex radius
//   double DSMIN ; // = 1000.;
//   double DSMAX ; // = -1000.;
//   if(sit == 0) {
//     DRMAX =  999999999;
//     DZMAX =  999999999; 
//     RVMAX =  999999999; 
//     DSMIN = -999999999;
//     DSMAX =  999999999; 
//   }
//   if(sit == 1) {
//     DRMAX =  999999999;
//     DZMAX =  999999999;
//     RVMAX =  999999999;
//     DSMIN = -999999999;
//     DSMAX =  999999999;
//   }
//   if(sit == 2) {
//     DRMAX =  999999999;
//     DZMAX =  999999999;
//     RVMAX =  999999999;
//     DSMIN = -999999999;
//     DSMAX =  999999999;
//   }
//   if (sit>2){
//     DRMAX =  999999999;
//     DZMAX =  999999999;
//     RVMAX =  999999999;
//     DSMIN = -999999999;
//     DSMAX =  999999999;
//   }
  
//   double d0[2],z0[2],phi[2],cotTheta[2],qOverPt[2],RC[2],XC[2],YC[2],RA[2],AB[2];
//   double XSVI[2],YSVI[2],ZSVI[2],RVI[2],DZSVI[2],SS1[2],SS2[2],ZZ1[2],ZZ2[2];
  
//   double X=0.,Y=0.,Z=0.;
//   for (int I=0; I<2; ++I) {
//     XSVI[I] = 0.;
//     YSVI[I] = 0.;
//     ZSVI[I] = 0.;
//     RVI[I] = 2.0*RVMAX;
//     DZSVI[I] = 2.0*DZMAX;
//     SS1[I] = 0.;
//     SS2[I] = 0.;
//     ZZ1[I] = 0.;
//     ZZ2[I] = 0.;
//   }
  
//   const HepVector& perigeeParms1 = per1->parameters();
//   d0[0] = perigeeParms1[Trk::d0];
//   z0[0] = perigeeParms1[Trk::z0];
//   phi[0] = perigeeParms1[Trk::phi];
//   double theta1 = perigeeParms1[Trk::theta];
//   cotTheta[0] = 1./tan(theta1);
//   double qOverP1 = perigeeParms1[Trk::qOverP];
//   qOverPt[0] = qOverP1 / per1->sinTheta();
//   RC[0] = 10.*0.5/(qOverPt[0]*aconst);
//   double U = RC[0] + d0[0];
//   XC[0] = -1*U*sin(phi[0]);
//   YC[0] = U*cos(phi[0]);
//   RA[0] = fabs(RC[0]);
  
//   const HepVector& perigeeParms2 = per2->parameters();
//   d0[1] = perigeeParms2[Trk::d0];
//   z0[1] = perigeeParms2[Trk::z0];
//   phi[1] = perigeeParms2[Trk::phi];
//   double theta2 = perigeeParms2[Trk::theta];
//   cotTheta[1] = 1./tan(theta2);
//   double qOverP2 = perigeeParms2[Trk::qOverP];
//   qOverPt[1] = qOverP2 / per2->sinTheta();
//   RC[1] = 10.*0.5/(qOverPt[1]*aconst);
//   U = RC[1] + d0[1];
//   XC[1] = -1*U*sin(phi[1]);
//   YC[1] = U*cos(phi[1]);
//   RA[1] = fabs(RC[1]);
  
//   //  get the circle centre separation
//   double DX = XC[1] - XC[0];
//   double DY = YC[1] - YC[0];
//   double D = DX*DX + DY*DY;
//   D = sqrt(D);
//   if (D < 0.) {                                     // the circles are concentric
//     sflag = 1;
//   }
//   U = D - RA[0] - RA[1];                            // signed separation, if > 0., the circles do not intersect, if < 0., they might
  
//   m_MDdr = U;
  
//   m_area = areaVar(XC[0], YC[0], RA[0], XC[1], YC[1], RA[1], m_h, m_hl, m_ddphi);
  
//   //  rotate, translate to a system, where the two circle centres lie on the X axis
//   double COST = DX/D;
//   double SINT = DY/D;
//   double Y0 = (-1*XC[0]*YC[1] + XC[1]*YC[0])/D;
//   for (int I=0; I<2; ++I) {
//     AB[I] = COST*XC[I] + SINT*YC[I];
//   }
  
//   U = (XC[1] + XC[0])*(XC[1] - XC[0]) + (YC[1] + YC[0])*(YC[1] - YC[0]);
//   double V = (RA[1] + RA[0])*(RA[1] - RA[0]);
//   double XX = 0.5*(U - V)/D;            // X of intersection
//   if ((XX - AB[0])*(XX - AB[0]) > 0.) {
//     U = sqrt((XX - AB[0])*(XX - AB[0]));
//   }
//   double YY2 = (RA[0] + U)*(RA[0] - U);
  
//   double YY;
//   int nsol = 0;
//   if (YY2 > 0.) {                       // the circles intersect
//     YY = sqrt(YY2);                     // two intersection points (+/- Y)
//     for (int I=0; I<2; ++I) {
//       U = YY + Y0;                      // invert the translation, rotation
//       XSVI[I] = COST*XX - SINT*U;
//       YSVI[I] = SINT*XX + COST*U;
//       YY = -1*YY;
//     }
//     nsol = 2;
//     m_ctvDr = 0.;
//   } else                                  // circles do not intersect - find how close they approach each other
//     {                                       // in the XY plane and take the point half way between them
//       U = D - RA[0] - RA[1];
//       if (U > 0.) {
// 	V = U;
// 	XX = AB[1] + RA[1];
// 	if (AB[0] < AB[1]) XX = AB[0] + RA[0];
//       } 
//       else
// 	{
// 	  if (AB[0] < AB[1]) {
// 	    XX = AB[1] - RA[1];
// 	    V = AB[0] - RA[0] - XX;
// 	  }
// 	  else
// 	    {
// 	      XX = AB[0] + RA[0];
// 	      V = AB[1] + RA[1] - XX;
// 	    }
// 	}
      
//       XX = XX + 0.5*V;
//       XSVI[0] = COST*XX - SINT*Y0;           // rotate back to the original system
//       YSVI[0] = SINT*XX + COST*Y0;           // one solution
//       nsol = 1;
      
//       m_ctvDr = V;
//       if (V > DRMAX) {
// 	sflag = 2;
//       }
//     }
  
//   for (int J=0; J<nsol; ++J) {              // loop over solutions
//     U = (XSVI[J] - XC[0])/RC[0];
//     V = -1*(YSVI[J] - YC[0])/RC[0];
//     U = atan2(U,V) - phi[0];                // turning angle from the track origin
//     if (U < -1*pi) U = U + twopi;
//     if (U > pi) U = U - twopi;
//     SS1[J] = RC[0]*U;                       // arc length
//     ZZ1[J] = z0[0] + SS1[J]*cotTheta[0];
//     U = (XSVI[J] - XC[1])/RC[1];
//     V = -1*(YSVI[J] - YC[1])/RC[1];
//     U = atan2(U,V) - phi[1];
//     if (U < -1*pi) U = U + twopi;
//     if (U > pi) U = U - twopi;
//     SS2[J] = RC[1]*U;
//     ZZ2[J] = z0[1] + SS1[J]*cotTheta[1];
//     RVI[J] = sqrt(XSVI[J]*XSVI[J] + YSVI[J]*YSVI[J]);
//     DZSVI[J] = ZZ2[J] - ZZ1[J];
//   }
//   ZSVI[0] = 0.5*(ZZ1[0] + ZZ2[0]);
//   ZSVI[1] = 0.5*(ZZ1[1] + ZZ2[1]);
  
//   m_ctvRmax  = std::min(RVI[0],RVI[1]);
//   if (std::min(RVI[0],RVI[1]) > RVMAX) {                        // check the vertex radius is acceptable
//     sflag = 3;
//   }
  
//   m_ctvDz = std::min(fabs(DZSVI[0]),fabs(DZSVI[1]));
//   if (std::min(fabs(DZSVI[0]),fabs(DZSVI[1])) > DZMAX) {         // check the Z difference
//     sflag = 4;
//   }
  
//   double A = std::min(SS1[0],SS2[0]);                            // minimum track arc length, solution 1
//   double B = std::min(SS1[1],SS2[1]);                            // minimum track arc length, solution 2
//   m_ctvArc = std::max(A,B);
//   if (std::max(A,B) < DSMIN || std::max(A,B) > DSMAX) {          // limit the minimum arc length
//     sflag = 5;
//   }

//   int J = 0;
//   if ( nsol == 2 && (fabs(DZSVI[1]) < fabs(DZSVI[0])) ) J = 1;
  
//   X = XSVI[J];
//   Y = YSVI[J];
//   Z = ZSVI[J];
// }

// //////////////////////////////////////////////////////////////////////////////////
// /// geometry utils
// /// 
// ///
// double PAUconvGeoTool::areaVar(double xc1, double yc1, double r1, double xc2, double yc2, double r2, double& h, double& hl, double &ddphi) const{
  
//   double ret = -999999;
//   double xi1, yi1, xi2, yi2;
//   h     = 0;
//   hl    = 0;
//   ddphi = 0.;
//   if (circleIntersection( xc1,  yc1,  r1,  xc2,  yc2,  r2, xi1, yi1, xi2, yi2 ))
//     {
//       // the two triangles have identical area
//       ret =  areaTriangle(xc1,yc1, xc2,yc2,  xi1,yi1) ;
      
//       h = 0.5*(sqrt( pow(xi1-xi2,2) + pow(yi1-yi2,2) ));

//       double  l = sqrt( pow(xc1-xc2,2) + pow(yc1-yc2,2) );
//       if (l!=0) hl = h/l;

//       // |AxB| = |A||B|sin(phi) => sinphi = |AxB| / |A||B|
//       // (xi1,yi1) = first  intersection point; this is the one I use
//       // (xi2,yi2) = second intersection point; this is symmetric to the first
//       // (xc1,yc1) = centre first  circle 
//       // (xc2,yc2) = centre second circle 
//       double norm1 = sqrt(pow(xi1-xc1,2)+pow(yi1-yc1,2));
//       double norm2 = sqrt(pow(xi1-xc2,2)+pow(yi1-yc2,2));
//       if ((norm1 != 0) && (norm2 != 0)){ // rejecting pathology
// 	// centres at the origin
// 	double xa = (xi1 - xc1)/norm1;
// 	double ya = (yi1 - yc1)/norm1;
// 	double xb = (xi1 - xc2)/norm2;
// 	double yb = (yi1 - yc2)/norm2;
// 	double costheta = xa*xb +ya*yb;
// 	double phi = TMath::Pi()-TMath::ACos(costheta);
// 	ddphi = phi;
//       }
//     }

//   return ret;
// }

// bool PAUconvGeoTool::circleIntersection(double xc1, double yc1, double r1, 
// 					  double xc2, double yc2, double r2, 
// 					  double& xi1, double& yi1, 
// 					  double& xi2, double& yi2) const {

//   // Calculate the intersection of the two circles:
//   //
//   // (x-xc1)^2 + (y-yc1)^2 = R1^2
//   // (x-xc2)^2 + (y-yc2)^2 = R2^2

//   xi1 = -999999999.;
//   xi2 = -999999999.;
//   yi1 = -999999999.;
//   yi2 = -999999999.;
  
//   if ( yc1 != yc2) {
//     double A = (xc1 - xc2) / (yc2- yc1);
//     double B = (r1*r1 - r2*r2 - xc1*xc1 + xc2*xc2 - yc1*yc1 + yc2*yc2) / 2. / ( yc2 -yc1);
//     double a  = 1 + A*A;
//     double b  = 2*A*B - 2*xc1 -2*A*yc1;
//     double c  = B*B - 2*B*yc1 + xc1*xc1 + yc1*yc1 - r1*r1;
//     if (secondDegree(a,b,c,xi1,xi2) ){
//       yi1 = A*xi1 + B;
//       yi2 = A*xi2 + B;
//       return true;
//     }    
//     else return false;
//   }
//   else if (xc1 != xc2){
//     double A = (yc1 - yc2) / (xc2- xc1);
//     double B = (r1*r1 - r2*r2 - xc1*xc1 + xc2*xc2 - yc1*yc1 + yc2*yc2) / 2. / ( xc2 -xc1);
//     double a  = 1 + A*A;
//     double b  = 2*A*B - 2*yc1 -2*A*xc1;
//     double c  = B*B - 2*B*xc1 + xc1*xc1 + yc1*yc1 - r1*r1;
//     if (secondDegree(a,b,c,yi1,yi2) ){
//       xi1 = A*yi1 + B;
//       xi2 = A*yi2 + B;
//       return true;
//     }    
//     else return false;
//   }
//   else {
//     // circles are concentric and we don't care
//     return false;
//   }
//   return false;
// }

// bool PAUconvGeoTool::secondDegree(double a, double b, double c, double& y1, double& y2) const{
  
//   y1 = -999999999;
//   y2 = -999999999;
//   double discr = b*b - 4*a*c;
//   if (discr < 0) return false;
//   y1 = (-b+sqrt(discr))/2./a;
//   y2 = (-b-sqrt(discr))/2./a;
//   return true;

// }

// double PAUconvGeoTool::areaTriangle(double a, double b, double d, double e, double g, double h) const{  
//   double c = 1;
//   double f = 1;
//   double i = 1;
//   return fabs(0.5* ( (a*e*i + d*h*c + b*f*g)  - (g*e*c + d*b*i + a*f*h) ) );  
// }


