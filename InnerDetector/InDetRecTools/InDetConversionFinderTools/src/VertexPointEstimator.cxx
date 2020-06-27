/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                VertexPointEstimator.cxx  -  Description
                -------------------
		begin   : 01-01-2008
		authors : Tatjana Lenz, Thomas Koffas
		email   : tatjana.lenz@cern.ch, Thomas.Koffas@cern.ch
		changes : M.ELSING
***************************************************************************/ 
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TMath.h"

//Do we need to replace this?
using CLHEP::pi;
using CLHEP::twopi;

namespace InDet {

  //define some statics
  static const InterfaceID IID_IVertexPointEstimator("InDet::VertexPointEstimator", 1, 0);
  const double VertexPointEstimator::s_bmagnt = 2.083;

  // ----------------------------------
  VertexPointEstimator::VertexPointEstimator(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_maxChi2(20.)
  {
    declareInterface<VertexPointEstimator>(this);
    /// Cuts for selecting track pairs
    m_maxDR.push_back(10000.)	 ;
    m_maxDR.push_back(10000.)        ;
    m_maxDR.push_back(10000.)        ;
    
    m_maxDZ.push_back(10000.)        ;
    m_maxDZ.push_back(10000.)        ;
    m_maxDZ.push_back(10000.)        ;
    
    m_maxR.push_back(10000.)         ;
    m_maxR.push_back(10000.)         ;
    m_maxR.push_back(10000.)         ;

    m_minArcLength.push_back(-10000.);
    m_minArcLength.push_back(-10000.);
    m_minArcLength.push_back(-10000.);
    m_maxArcLength.push_back(10000.) ;
    m_maxArcLength.push_back(10000.) ;
    m_maxArcLength.push_back(10000.) ;
    
    m_minDr.push_back(-5.)           ;
    m_minDr.push_back(-25.)          ;
    m_minDr.push_back(-50.)          ;
    
    m_maxDr.push_back(5.)            ;
    m_maxDr.push_back(10.)           ;
    m_maxDr.push_back(10.)           ;

    m_maxHl.push_back(10000.)        ;
    m_maxHl.push_back(10000.)        ;
    m_maxHl.push_back(10000.)        ;
			
    m_maxPhi.push_back(0.05)         ;
    m_maxPhi.push_back(0.1)          ;
    m_maxPhi.push_back(0.1)          ;

    //some properties
    declareProperty("MaxTrkXYDiffAtVtx",      m_maxDR);
    declareProperty("MaxTrkZDiffAtVtx",       m_maxDZ);
    declareProperty("MaxTrkXYValue",          m_maxR);
    declareProperty("MinArcLength",           m_minArcLength);
    declareProperty("MaxArcLength",           m_maxArcLength);
    declareProperty("MaxChi2OfVtxEstimation", m_maxChi2);
    declareProperty("MinDeltaR",              m_minDr);
    declareProperty("MaxDeltaR",              m_maxDr);
    declareProperty("MaxHl",                  m_maxHl);
    declareProperty("MaxPhi",                 m_maxPhi);
  }

  // ----------------------------------
  VertexPointEstimator::~VertexPointEstimator() = default;

  // ----------------------------------
  const InterfaceID& VertexPointEstimator::interfaceID() {
    return IID_IVertexPointEstimator;
  }

  // ----------------------------------
  StatusCode VertexPointEstimator::initialize() {
    return StatusCode::SUCCESS;
  }

  // ----------------------------------
  StatusCode VertexPointEstimator::finalize() {
    return StatusCode::SUCCESS;
  }

  // ----------------------------------
  /** circles intersection point calculation */
  Amg::Vector3D
  VertexPointEstimator::getCirclesIntersectionPoint(const Trk::Perigee *per1,
                                                    const Trk::Perigee *per2,
                                                    unsigned int flag,
                                                    int& errorcode) const
  {
    float deltaPhi;
    float deltaR;
    return intersectionImpl (per1, per2, flag, errorcode, deltaPhi, deltaR);
  }


  Amg::Vector3D
  VertexPointEstimator::getCirclesIntersectionPoint(const Trk::Perigee *per1,
                                                    const Trk::Perigee *per2,
                                                    unsigned int flag,
                                                    int& errorcode,
                                                    Values_t& decors) const
  {
    decors.clear();
    return intersectionImpl (per1, per2, flag, errorcode,
                             decors["deltaPhiTracks"],
                             decors["DR1R2"]);
  }


  std::vector<std::string> VertexPointEstimator::decorKeys() const
  {
    return {"deltaPhiTracks", "DR1R2"};
  }


  // ----------------------------------
  /** internal implementation */
  Amg::Vector3D
  VertexPointEstimator::intersectionImpl(const Trk::Perigee *per1,
                                         const Trk::Perigee *per2,
                                         unsigned int flag,
                                         int& errorcode,
                                         float& deltaPhi,
                                         float& deltaR) const
  {
    /*
      Calculates the initial approximation to the vertex position. Based on CTVMFT.
      Tries to intersect circles that are (R,Phi) projections of the helical trajectories of the two tracks
      If the circles intersect, the intersection with smaller z difference between the 2 helices extrapolated to the intersection is chosen.
      If the circles do not intersect, the vertex approximation is taken as the point of closest approach of the two circles.
      per1 should be that of the track with the smaller radius
    */
    Amg::Vector3D intPoint(0.,0.,0.);
    double aconst = -1.49898*s_bmagnt; //is it correct???????????????????
    double DRMAX = 0.;         // maximum XY separation, non-intersecting circles
    double DZMAX = 0.;         // maximum allowed track Z separation at the vertex
    double RVMAX = 0.;         // maximum allowed vertex radius
    double minArcLength(0.);
    double maxArcLength(0.);
    double minDr(0.);
    double maxDr(0.);
    double maxHl(0.);
    double maxPhi(0.);
    if (flag <= 2) {
      DRMAX = m_maxDR[flag]; //maximum XY separation, non-intersecting circles
      DZMAX = m_maxDZ[flag]; //maximum allowed track Z separation at the vertex
      RVMAX = m_maxR[flag] ; //maximum allowed vertex radius
      minArcLength = m_minArcLength[flag];
      maxArcLength = m_maxArcLength[flag];
      minDr = m_minDr[flag];
      maxDr = m_maxDr[flag];
      maxHl = m_maxHl[flag];
      maxPhi= m_maxPhi[flag];
    }
    
    double d0[2],z0[2],phi[2],cotTheta[2],qOverPt[2],RC[2],XC[2],YC[2],RA[2],AB[2];
    double XSVI[2],YSVI[2],ZSVI[2],RVI[2],DZSVI[2],SS1[2],SS2[2],ZZ1[2],ZZ2[2];
    
    double X=0.,Y=0.,Z=0.;
    for (int I=0; I<2; ++I) {
      d0[I]       = -999.;
      z0[I]       = -999.;
      phi[I]      = -999.;
      cotTheta[I] = -999.;
      qOverPt[I]  = -999.;
      RC[I]       = -999.;
      XC[I]       = -999.;
      YC[I]       = -999.;
      RA[I]       = +999.;
      AB[I]       = +999.;
      XSVI[I]     = 0.;
      YSVI[I]     = 0.;
      ZSVI[I]     = 0.;
      RVI[I]      = 2.0*RVMAX;
      DZSVI[I]    = 2.0*DZMAX;
      SS1[I]      = 0.;
      SS2[I]      = 0.;
      ZZ1[I]      = 0.;
      ZZ2[I]      = 0.;
    }

    double U(0.);
    const AmgVector(5)& perParam1 = per1->parameters();
    double pt1 = fabs(sin(perParam1[Trk::theta])/perParam1[Trk::qOverP]);
    const AmgVector(5)& perParam2 = per2->parameters(); 
    double pt2 = fabs(sin(perParam2[Trk::theta])/perParam2[Trk::qOverP]);
    if (pt1 < pt2) {
      d0[0]  = perParam1[Trk::d0]; d0[1] = perParam2[Trk::d0];
      z0[0]  = perParam1[Trk::z0]; z0[1] = perParam2[Trk::z0];
      phi[0] = perParam1[Trk::phi]; phi[1] = perParam2[Trk::phi];
      double theta1  = perParam1[Trk::theta]; cotTheta[0] = 1./tan(theta1);
      double theta2  = perParam2[Trk::theta]; cotTheta[1] = 1./tan(theta2);
      double qOverP1 = perParam1[Trk::qOverP]; qOverPt[0] = qOverP1/sin(perParam1[Trk::theta]);
      double qOverP2 = perParam2[Trk::qOverP]; qOverPt[1] = qOverP2/sin(perParam2[Trk::theta]);
      RC[0] = 10.*0.5/(qOverPt[0]*aconst);  RC[1] = 10.*0.5/(qOverPt[1]*aconst);//Radius of curvature
      U     = RC[0] + d0[0];
      XC[0] = -1*U*sin(phi[0]);   //Circle center x-coordinate
      YC[0] = U*cos(phi[0]);      //Circle center y-coordinate
      RA[0] = fabs(RC[0]);
      U     = RC[1] + d0[1];
      XC[1] = -1*U*sin(phi[1]);  // Circle center x-coordinate
      YC[1] = U*cos(phi[1]);     // Circle center y-coordinate
      RA[1] = fabs(RC[1]);
    } else {
      d0[0]  = perParam2[Trk::d0]; d0[1] = perParam1[Trk::d0];
      z0[0]  = perParam2[Trk::z0]; z0[1] = perParam1[Trk::z0];
      phi[0] = perParam2[Trk::phi]; phi[1] = perParam1[Trk::phi];
      double theta2  = perParam2[Trk::theta]; cotTheta[0] = 1./tan(theta2);
      double theta1  = perParam1[Trk::theta]; cotTheta[1] = 1./tan(theta1);
      double qOverP2 = perParam2[Trk::qOverP]; qOverPt[0] = qOverP2/sin(perParam2[Trk::theta]);
      double qOverP1 = perParam1[Trk::qOverP]; qOverPt[1] = qOverP1/sin(perParam1[Trk::theta]);
      RC[0]    = 10.*0.5/(qOverPt[0]*aconst);  RC[1] = 10.*0.5/(qOverPt[1]*aconst);//Radius of curvature
      double U = RC[0] + d0[0];
      XC[0]    = -1*U*sin(phi[0]);   //Circle center x-coordinate
      YC[0]    = U*cos(phi[0]);      //Circle center y-coordinate
      RA[0]    = fabs(RC[0]);
      U        = RC[1] + d0[1];
      XC[1]    = -1*U*sin(phi[1]);  // Circle center x-coordinate
      YC[1]    = U*cos(phi[1]);     // Circle center y-coordinate
      RA[1]    = fabs(RC[1]);
    }

    //  get the circle centre separation
    double DX = XC[1] - XC[0];
    double DY = YC[1] - YC[0];
    double D = DX*DX + DY*DY;
    D = sqrt(D);
    if (D == 0.) {
      ATH_MSG_DEBUG("Concentric circles, should not happen return (0,0,0)");
      errorcode = 1;
    }
    U = D - RA[0] - RA[1]; // signed separation, if > 0., the circles do not intersect, if < 0., they might
    //  rotate, translate to a system, where the two circle centres lie on the X axis

    //New cut from Mauro
    deltaR = U; double PHI = -99999.;
    double hl = areaVar(XC[0], YC[0], RA[0], XC[1], YC[1], RA[1], PHI);
    
    double COST = DX/D;
    double SINT = DY/D;
    double Y0 = (-1*XC[0]*YC[1] + XC[1]*YC[0])/D; //Translation along the y-axis
    for (int I=0; I<2; ++I) {
      AB[I] = COST*XC[I] + SINT*YC[I];   // The new circle center positions in the rotated system. Essentially the new x-coordinates, since the y-coordinates are 0.
    }
    U = (XC[1] + XC[0])*(XC[1] - XC[0]) + (YC[1] + YC[0])*(YC[1] - YC[0]);
    double V = (RA[1] + RA[0])*(RA[1] - RA[0]);
    double XX = 0.5*(U - V)/D;            // X of intersection in rotated coordinate system (a+AB[0]). Again the y is 0.
    if ((XX - AB[0])*(XX - AB[0]) > 0.) {
      U = sqrt((XX - AB[0])*(XX - AB[0])); //This is the a in my logbook 
    }
    double YY2 = (RA[0] + U)*(RA[0] - U);  //This is the h^2 in my logbook
    double YY;
    int nsol = 0;
    if (YY2 > 0.) {

      // the circles intersect
      YY = sqrt(YY2); // two intersection points (+/- Y) or (+/- h) in my logbook

      for (int I=0; I<2; ++I) {
        U       = YY + Y0;  // invert the translation
        XSVI[I] = COST*XX - SINT*U;  // Invert the rotation
        YSVI[I] = SINT*XX + COST*U;
        YY      = -1*YY;
      }

      nsol = 2;

    } else {
      // circles do not intersect - find how close they approach each other
      // in the XY plane and take the point half way between them
      U = D - RA[0] - RA[1];
      if (U > 0.) {
        //Circles do not overlap at all and are outside from each other.See logbook
        V  = U;
        XX = AB[1] + RA[1];

        if (AB[0] < AB[1]) XX = AB[0] + RA[0];
            } else {                            
        //Circles inside each other but off-centered.See logbook
        if (AB[0] < AB[1]) {                //To the left
          XX = AB[1] - RA[1];
          V = AB[0] - RA[0] - XX;
        } else {
          //To the right
          XX = AB[0] + RA[0];
          V = AB[1] + RA[1] - XX;
        }
      }	
      XX      = XX + 0.5*V;
      XSVI[0] = COST*XX - SINT*Y0;           // rotate back to the original system
      YSVI[0] = SINT*XX + COST*Y0;           // one solution
      nsol    = 1;
      if (V > DRMAX) {                       
        //Cut if distance of minimum approach is too big
        ATH_MSG_DEBUG("XY distance of minimum approach is too large, return (0,0,0)");
        errorcode = 2;
      }
    }
    
    for (int J=0; J<nsol; ++J) {              // loop over solutions
      U = (XSVI[J] - XC[0])/RC[0];
      V = -1*(YSVI[J] - YC[0])/RC[0];
      U = atan2(U,V) - phi[0];                // turning angle from the track origin
      if (U < -1*pi) U = U + twopi;
      if (U > pi) U = U - twopi;
      SS1[J] = RC[0]*U;                       // arc length
      ZZ1[J] = z0[0] + SS1[J]*cotTheta[0];
      U = (XSVI[J] - XC[1])/RC[1];
      V = -1*(YSVI[J] - YC[1])/RC[1];
      U = atan2(U,V) - phi[1];
      if (U < -1*pi) U = U + twopi;
      if (U > pi) U = U - twopi;
      SS2[J] = RC[1]*U;
      ZZ2[J] = z0[1] + SS1[J]*cotTheta[1];
      RVI[J] = sqrt(XSVI[J]*XSVI[J] + YSVI[J]*YSVI[J]);
      DZSVI[J] = ZZ2[J] - ZZ1[J];
    }
    ZSVI[0] = 0.5*(ZZ1[0] + ZZ2[0]);
    ZSVI[1] = 0.5*(ZZ1[1] + ZZ2[1]);
    
    if (std::min(RVI[0],RVI[1]) > RVMAX) {                        // check the vertex radius is acceptable
      ATH_MSG_DEBUG("Unacceptable vertex radius");
      errorcode = 3;
    }
    
    if (std::min(fabs(DZSVI[0]),fabs(DZSVI[1])) > DZMAX) {         // check the Z difference
      ATH_MSG_DEBUG("Unacceptable Z difference");
      errorcode = 4;
    }
    
    double A = std::min(SS1[0],SS2[0]);                            // minimum track arc length, solution 1
    double B = std::min(SS1[1],SS2[1]);                            // minimum track arc length, solution 2
    if (std::max(A,B) < minArcLength || std::max(A,B) > maxArcLength) {          // limit the minimum arc length
      ATH_MSG_DEBUG("Unacceptable arc length");
      errorcode = 5;
    }
    
    int J = 0;
    if ( nsol == 2 && (fabs(DZSVI[1]) < fabs(DZSVI[0])) ) J = 1;
    
    X = XSVI[J];
    Y = YSVI[J];
    Z = ZSVI[J];
    intPoint(0)=X;	intPoint(1)=Y;	intPoint(2)=Z;
    
    if(deltaR>maxDr || deltaR<minDr){
      ATH_MSG_DEBUG("Unaceptable circle distance");
      errorcode = 6;
    }
    
    if(hl>maxHl){
      ATH_MSG_DEBUG("Unacceptable h/D ratio");
      errorcode = 7;
    }
    
    deltaPhi = PHI; // quick fix: cannot get rid of (double) PHI as it is passed by ref and deltaPhi is a float
    if(deltaPhi>maxPhi){
      ATH_MSG_DEBUG("Unacceptable difference in phi");
      errorcode = 8;
    }

    return intPoint;
  }
  
  // ----------------------------------------------------------------
  double VertexPointEstimator::areaVar(double xc1, double yc1, double r1, double xc2, double yc2, double r2, double& phi) const
  {
    double ret = -999999;
    double xi1, yi1, xi2, yi2;
    if (circleIntersection( xc1,  yc1,  r1,  xc2,  yc2,  r2, xi1, yi1, xi2, yi2 ))
    {
      double  h = 0.5*(sqrt( pow(xi1-xi2,2) + pow(yi1-yi2,2) ));
      double  l = sqrt( pow(xc1-xc2,2) + pow(yc1-yc2,2) );
      if (l!=0) ret = h/l;

      double norm1 = sqrt((xi1-xc1)*(xi1-xc1)+(yi1-yc1)*(yi1-yc1));
      double norm2 = sqrt((xi1-xc2)*(xi1-xc2)+(yi1-yc2)*(yi1-yc2));
      if((norm1!=0.) && (norm2!=0.)){ //rejecting pathology
        //centers at the origin
        norm1 = 1./norm1;
        norm2 = 1./norm2;
        double xa = (xi1-xc1)*norm1;
        double ya = (yi1-yc1)*norm1;
        double xb = (xi1-xc2)*norm2;
        double yb = (yi1-yc2)*norm2;
        double costheta = xa*xb + ya*yb;
        phi = M_PI-acos(costheta);
      }
    }
    return ret;
  }
  
  // ----------------------------------
  double VertexPointEstimator::areaVar(double xc1, double yc1, double r1, double xc2, double yc2, double r2, double& h, double& hl, double &ddphi) const
  {
    double ret = -999999;
    double xi1, yi1, xi2, yi2;
    h     = 0;
    hl    = 0;
    ddphi = 0.;
    if (circleIntersection( xc1,  yc1,  r1,  xc2,  yc2,  r2, xi1, yi1, xi2, yi2 )) {

      // the two triangles have identical area
      ret =  areaTriangle(xc1,yc1, xc2,yc2,  xi1,yi1) ;
      
      h = 0.5*(sqrt( pow(xi1-xi2,2) + pow(yi1-yi2,2) ));
      
      double  l = sqrt( pow(xc1-xc2,2) + pow(yc1-yc2,2) );
      if (l!=0) hl = h/l;
      
      // |AxB| = |A||B|sin(phi) => sinphi = |AxB| / |A||B|
      // (xi1,yi1) = first  intersection point; this is the one I use
      // (xi2,yi2) = second intersection point; this is symmetric to the first
      // (xc1,yc1) = centre first  circle 
      // (xc2,yc2) = centre second circle 
      double norm1 = sqrt(pow(xi1-xc1,2)+pow(yi1-yc1,2));
      double norm2 = sqrt(pow(xi1-xc2,2)+pow(yi1-yc2,2));
      if ((norm1 != 0) && (norm2 != 0)){ // rejecting pathology
        // centres at the origin
        double xa = (xi1 - xc1)/norm1;
        double ya = (yi1 - yc1)/norm1;
        double xb = (xi1 - xc2)/norm2;
        double yb = (yi1 - yc2)/norm2;
        double costheta = xa*xb +ya*yb;
        double phi = TMath::Pi()-TMath::ACos(costheta);
        ddphi = phi;
      }
    }
    
    return ret;
  }

  // ----------------------------------
  bool VertexPointEstimator::circleIntersection(double xc1, double yc1, double r1, 
						double xc2, double yc2, double r2, 
						double& xi1, double& yi1, 
						double& xi2, double& yi2) const
  {
    // Calculate the intersection of the two circles:
    //
    // (x-xc1)^2 + (y-yc1)^2 = R1^2
    // (x-xc2)^2 + (y-yc2)^2 = R2^2
    
    xi1 = -999999999.;
    xi2 = -999999999.;
    yi1 = -999999999.;
    yi2 = -999999999.;
    
    if ( yc1 != yc2) {
      double A = (xc1 - xc2) / (yc2- yc1);
      double B = (r1*r1 - r2*r2 - xc1*xc1 + xc2*xc2 - yc1*yc1 + yc2*yc2) / 2. / ( yc2 -yc1);
      double a  = 1 + A*A;
      double b  = 2*A*B - 2*xc1 -2*A*yc1;
      double c  = B*B - 2*B*yc1 + xc1*xc1 + yc1*yc1 - r1*r1;
      if (secondDegree(a,b,c,xi1,xi2) ){
        yi1 = A*xi1 + B;
        yi2 = A*xi2 + B;
        return true;
      }    
      return false;
    }
    if (xc1 != xc2){
      double A = (yc1 - yc2) / (xc2- xc1);
      double B = (r1*r1 - r2*r2 - xc1*xc1 + xc2*xc2 - yc1*yc1 + yc2*yc2) / 2. / ( xc2 -xc1);
      double a  = 1 + A*A;
      double b  = 2*A*B - 2*yc1 -2*A*xc1;
      double c  = B*B - 2*B*xc1 + xc1*xc1 + yc1*yc1 - r1*r1;
      if (secondDegree(a,b,c,yi1,yi2) ){
        xi1 = A*yi1 + B;
        xi2 = A*yi2 + B;
        return true;
      }    
      return false;
    }
    
      // circles are concentric and we don't care
      return false;
    
    return false;
  }

  // ----------------------------------
  bool VertexPointEstimator::secondDegree(double a, double b, double c, double& y1, double& y2) const
  {
    y1 = -999999999;
    y2 = -999999999;
    double discr = b*b - 4*a*c;
    if (discr < 0) return false;
    y1 = (-b+sqrt(discr))/2./a;
    y2 = (-b-sqrt(discr))/2./a;
    return true;
    
  }

  // ----------------------------------
  double VertexPointEstimator::areaTriangle(double a, double b,  // double c,
					    double d, double e,  // double f,
					    double g, double h) const { // double i)
    double c = 1;
    double f = 1;
    double i = 1;
    return fabs(0.5* ( (a*e*i + d*h*c + b*f*g)  - (g*e*c + d*b*i + a*f*h) ) );  
  }

}
