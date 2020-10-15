/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// RungeKuttaUtils.cxx , (c) ATLAS Detector software
// author Igor Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TrkExUtils/RungeKuttaUtils.h"
#include "TrkPatternParameters/PatternTrackParameters.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/DistanceSolution.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"

#include "CxxUtils/vec.h"
#include "CxxUtils/vectorize.h"
ATH_ENABLE_VECTORIZATION;


namespace{
/*
 * Hide all internal implementation methods
 * inside an anonymous namespace
 */
/*The notation of this package
in array P[42].
                 /dL0    /dL1    /dPhi   /dThe   /dCM
X  ->P[0]  dX /   P[ 7]   P[14]   P[21]   P[28]   P[35]
Y  ->P[1]  dY /   P[ 8]   P[15]   P[22]   P[29]   P[36]
Z  ->P[2]  dZ /   P[ 9]   P[16]   P[23]   P[30]   P[37]
Ax ->P[3]  dAx/   P[10]   P[17]   P[24]   P[31]   P[38]
Ay ->P[4]  dAy/   P[11]   P[18]   P[25]   P[32]   P[39]
Az ->P[5]  dAz/   P[12]   P[19]   P[26]   P[33]   P[40]
CM ->P[6]  dCM/   P[13]   P[20]   P[27]   P[34]   P[41]
*/

inline void
globalToLocalVecHelper(double* ATH_RESTRICT P,
                       const double s0,
                       const double s1,
                       const double s2,
                       const double s3,
                       const double s4)
{
  using namespace CxxUtils;
  using vec2 = CxxUtils::vec<double, 2>;
  /* The calculation (original form)
      P[ 7]-=(s0*P[ 3]); P[ 8]-=(s0*P[ 4]); P[ 9]-=(s0*P[ 5]);
      P[10]-=(s0*P[42]); P[11]-=(s0*P[43]); P[12]-=(s0*P[44]);
      P[14]-=(s1*P[ 3]); P[15]-=(s1*P[ 4]); P[16]-=(s1*P[ 5]);
      P[17]-=(s1*P[42]); P[18]-=(s1*P[43]); P[19]-=(s1*P[44]);
      P[21]-=(s2*P[ 3]); P[22]-=(s2*P[ 4]); P[23]-=(s2*P[ 5]);
      P[24]-=(s2*P[42]); P[25]-=(s2*P[43]); P[26]-=(s2*P[44]);
      P[28]-=(s3*P[ 3]); P[29]-=(s3*P[ 4]); P[30]-=(s3*P[ 5]);
      P[31]-=(s3*P[42]); P[32]-=(s3*P[43]); P[33]-=(s3*P[44]);
      P[35]-=(s4*P[ 3]); P[36]-=(s4*P[ 4]); P[37]-=(s4*P[ 5]);
      P[38]-=(s4*P[42]); P[39]-=(s4*P[43]); P[40]-=(s4*P[44]);
      */
  /*
   * The naming convention we follow is
   * A_B -->SIMD vector of
   * of size 2 containing
   * {A,B}
   * For example :
   * dZdTheta_dAxdTheta --> {dZ/dTheta, dAx/dTheta}
   * --> {P[30],P[31]}
   */
  vec2 Pmult1 = { P[3], P[4] };
  vec2 Pmult2 = { P[5], P[42] };
  vec2 Pmult3 = { P[43], P[44] };

  vec2 dXdL0_dYdL0;
  vload(dXdL0_dYdL0, &P[7]);
  vec2 dZdL0_dAxdL0;
  vload(dZdL0_dAxdL0, &P[9]);
  vec2 dAydL0_dAzdL0;
  vload(dAydL0_dAzdL0, &P[11]);
  dXdL0_dYdL0 -= s0 * Pmult1;
  dZdL0_dAxdL0 -= s0 * Pmult2;
  dAydL0_dAzdL0 -= s0 * Pmult3;
  vstore(&P[7], dXdL0_dYdL0);
  vstore(&P[9], dZdL0_dAxdL0);
  vstore(&P[11], dAydL0_dAzdL0);

  vec2 dXdL1_dYdL1;
  vload(dXdL1_dYdL1, &P[14]);
  vec2 dZdL1_dAxdL1;
  vload(dZdL1_dAxdL1, &P[16]);
  vec2 dAydL1_dAzdL1;
  vload(dAydL1_dAzdL1, &P[18]);
  dXdL1_dYdL1 -= s1 * Pmult1;
  dZdL1_dAxdL1 -= s1 * Pmult2;
  dAydL1_dAzdL1 -= s1 * Pmult3;
  vstore(&P[14], dXdL1_dYdL1);
  vstore(&P[16], dZdL1_dAxdL1);
  vstore(&P[18], dAydL1_dAzdL1);

  vec2 dXdPhi_dYdPhi;
  vload(dXdPhi_dYdPhi, &P[21]);
  vec2 dZdPhi_dAxdPhi;
  vload(dZdPhi_dAxdPhi, &P[23]);
  vec2 dAydPhi_dAzdPhi;
  vload(dAydPhi_dAzdPhi, &P[25]);
  dXdPhi_dYdPhi -= s2 * Pmult1;
  dZdPhi_dAxdPhi -= s2 * Pmult2;
  dAydPhi_dAzdPhi -= s2 * Pmult3;
  vstore(&P[21], dXdPhi_dYdPhi);
  vstore(&P[23], dZdPhi_dAxdPhi);
  vstore(&P[25], dAydPhi_dAzdPhi);

  vec2 dXdTheta_dYdTheta;
  vload(dXdTheta_dYdTheta, &P[28]);
  vec2 dZdTheta_dAxdTheta;
  vload(dZdTheta_dAxdTheta, &P[30]);
  vec2 dAydTheta_dAzdTheta;
  vload(dAydTheta_dAzdTheta, &P[32]);
  dXdTheta_dYdTheta -= s3 * Pmult1;
  dZdTheta_dAxdTheta -= s3 * Pmult2;
  dAydTheta_dAzdTheta -= s3 * Pmult3;
  vstore(&P[28], dXdTheta_dYdTheta);
  vstore(&P[30], dZdTheta_dAxdTheta);
  vstore(&P[32], dAydTheta_dAzdTheta);

  vec2 dXdCM_dYdCM;
  vload(dXdCM_dYdCM, &P[35]);
  vec2 dZdCM_dAxdCM;
  vload(dZdCM_dAxdCM, &P[37]);
  vec2 AydCM_dAzdCM;
  vload(AydCM_dAzdCM, &P[39]);
  dXdCM_dYdCM -= s4 * Pmult1;
  dZdCM_dAxdCM -= s4 * Pmult2;
  AydCM_dAzdCM -= s4 * Pmult3;
  vstore(&P[35], dXdCM_dYdCM);
  vstore(&P[37], dZdCM_dAxdCM);
  vstore(&P[39], AydCM_dAzdCM);

}

/* Helper to replace repeated calculation  of
 * 5x1 = 5x3 * 3X1
 * for the Jacobian
 *
 * E.g a calculation like :
 * Jac[ 0] = Ax[0]*P[ 7]+Ax[1]*P[ 8]+Ax[2]*P[ 9]; // dL0/dL0
 * Jac[ 1] = Ax[0]*P[14]+Ax[1]*P[15]+Ax[2]*P[16];  // dL0/dL1
 * Jac[ 2] = Ax[0]*P[21]+Ax[1]*P[22]+Ax[2]*P[23]; // dL0/dPhi
 * Jac[ 3] = Ax[0]*P[28]+Ax[1]*P[29]+Ax[2]*P[30]; // dL0/dThe
 * Jac[ 4] = Ax[0]*P[35]+Ax[1]*P[36]+Ax[2]*P[37]; // dL0/dCM
 * Jac[ 5] = Ay[0]*P[ 7]+Ay[1]*P[ 8]+Ay[2]*P[ 9]; // dL1/dL0
 * Jac[ 6] = Ay[0]*P[14]+Ay[1]*P[15]+Ay[2]*P[16]; // dL1/dL1
 * Jac[ 7] = Ay[0]*P[21]+Ay[1]*P[22]+Ay[2]*P[23];  // dL1/dPhi
 * Jac[ 8] = Ay[0]*P[28]+Ay[1]*P[29]+Ay[2]*P[30]; // dL1/dThe
 * Jac[ 9] = Ay[0]*P[35]+Ay[1]*P[36]+Ay[2]*P[37]; // dL1/dCM
 * is replaces with
 *  mutl3x5Helper(&Jac[0],Ax,&P[7]);
 *  mutl3x5Helper(&Jac[5],Ay,&P[7]);
 */
inline void
mutl3x5Helper(double* ATH_RESTRICT Jac,
              const double* ATH_RESTRICT V,
              const double* ATH_RESTRICT P)
{
  using vec2 = CxxUtils::vec<double, 2>;
   /*
   * |Jac[0] |= |V[0]| * |P[0]| + |V[1]| * |P[1] | + |V[2]| * |P[2] |
   * |Jac[1] |= |V[0]| * |P[7]| + |V[1]| * |P[8] | + |V[2]| * |P[9] |

   * |Jac[2] |= |V[0]| * |P[14]| + |V[1]| * |P[15]| +  |V[2]| * |P[16]|
   * |Jac[3] |= |V[0]| * |P[21]| + |V[1]| * |P[22]| +  |V[2]| *  |P[23]|
   *
   * Jac[4] = V[0] * P[28] + V[1] * P[29] + V[2] * P[30];
   *
   * The first do we do in vertical SIMD (128 bit ) fashion
   *
   * {Jac[0] | Jac[1]} =
   * {V[0] | V[0]} * {P[0]  | P[7]} +
   * {V[1] | V[1]} * {P[1]  | P[8]}
   * {V[2] | V[2]} * {P[16] | P[23}
   *
   * {Jac[2] | Jac[3]} =
   * {V[0] | V[0]} * {P[14] | P[21]} +
   * {V[1] | V[1]} * {P[15] | P[22]} +
   * {V[2] | V[2]} * {P[16] | P[23}
   *
   * Where  {} is a SIMD size 2 vector
   *
   * The remaining odd element is done at the end
   * Jac[4] = V[0] * P[28] + V[1] * P[29] + V[2] * P[30];
   */
  using vec2 = CxxUtils::vec<double, 2>;
  vec2 V1 = { V[0], V[0] };
  vec2 V2 = { V[1], V[1] };
  vec2 V3 = { V[2], V[2] };

  // 1st/2nd element
  vec2 P1v1 = { P[0], P[7] };
  vec2 P1v2 = { P[1], P[8] };
  vec2 P1v3 = { P[2], P[9] };
  vec2 res1 = V1 * P1v1 + V2 * P1v2 + V3 * P1v3;

  // 3th/4th element
  vec2 P2v1 = { P[14], P[21] };
  vec2 P2v2 = { P[15], P[22] };
  vec2 P2v3 = { P[16], P[23] };
  vec2 res2 = V1 * P2v1 + V2 * P2v2 + V3 * P2v3;

  //store results
  CxxUtils::vstore(&Jac[0], res1);
  CxxUtils::vstore(&Jac[2], res2);
  // The 5th element
  Jac[4] = V[0] * P[28] + V[1] * P[29] + V[2] * P[30];
}

void
transformGlobalToPlane(const Amg::Transform3D&  T,
                       bool useJac,
                       double* ATH_RESTRICT P,
                       double* ATH_RESTRICT par,
                       double* ATH_RESTRICT Jac)
{
  const double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  const double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  double d[3] = {P[0]-T(0,3),P[1]-T(1,3),P[2]-T(2,3)};

  par[0]      = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  par[1]      = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];

  if(!useJac) return;

  // Condition trajectory on surface
  //
  double S[3] =  {T(0,2),T(1,2),T(2,2)};

  double    A = P[3]*S[0]+P[4]*S[1]+P[5]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  double s[5]={};
  mutl3x5Helper(s,S,&P[7]);
  globalToLocalVecHelper(P, s[0], s[1], s[2], s[3], s[4]);
  // Jacobian production
  mutl3x5Helper(&Jac[0],Ax,&P[7]);
  mutl3x5Helper(&Jac[5],Ay,&P[7]);
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Disc system coordinate
/////////////////////////////////////////////////////////////////////////////////

void
transformGlobalToDisc(const Amg::Transform3D&  T,
                      bool useJac,
                      double* ATH_RESTRICT P,
                      double* ATH_RESTRICT par,
                      double* ATH_RESTRICT Jac)
{
  const double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  const double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  const double d[3] = {P[0]-T(0,3),P[1]-T(1,3),P[2]-T(2,3)};

  const double RC   = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  const double RS   = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];
  const double R2   = RC*RC+RS*RS;
  par[0]      = sqrt(R2);
  par[1]      = atan2(RS,RC);

  if(!useJac) return;

  // Condition trajectory on surface
  //
  double S[3] =  {T(0,2),T(1,2),T(2,2)};

  double    A = P[3]*S[0]+P[4]*S[1]+P[5]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  double s[5]={};
  mutl3x5Helper(s,S,&P[7]);
  globalToLocalVecHelper(P, s[0], s[1], s[2], s[3], s[4]);
  // Jacobian production
  //
  double Ri = 1./par[0];

  const double Av[3] = { (RC * Ax[0] + RS * Ay[0]) * Ri,
                         (RC * Ax[1] + RS * Ay[1]) * Ri,
                         (RC * Ax[2] + RS * Ay[2]) * Ri };
  const double Bv[3] = { (RC * Ay[0] - RS * Ax[0]) * (Ri = 1. / R2),
                         (RC * Ay[1] - RS * Ax[1]) * Ri,
                         (RC * Ay[2] - RS * Ax[2]) * Ri };

  mutl3x5Helper(&Jac[0],Av,&P[7]);
  mutl3x5Helper(&Jac[5],Bv,&P[7]);
}
/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Cylinder system coordinate
/////////////////////////////////////////////////////////////////////////////////
void
transformGlobalToCylinder(const Amg::Transform3D&  T,
                          double R,
                          bool useJac,
                          double* ATH_RESTRICT P,
                          double* ATH_RESTRICT par,
                          double* ATH_RESTRICT Jac)
{


  const double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  const double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  const double Az[3] = {T(0,2),T(1,2),T(2,2)};


  double x  = P[0]-T(0,3);
  double y  = P[1]-T(1,3);
  double z  = P[2]-T(2,3);
  double RC = x*Ax[0]+y*Ax[1]+z*Ax[2];
  double RS = x*Ay[0]+y*Ay[1]+z*Ay[2];
  par[0]    = atan2(RS,RC)*R;
  par[1]    = x*Az[0]+y*Az[1]+z*Az[2];

  if(!useJac) return;

  // Condition trajectory on surface
  //
  double B  = par[1];
  double C  = P[3]*Az[0]+P[4]*Az[1]+P[5]*Az[2];
  const double ax = P[3]-Az[0]*C; x-=(B*Az[0]);
  const double ay = P[4]-Az[1]*C; y-=(B*Az[1]);
  const double az = P[5]-Az[2]*C; z-=(B*Az[2]);
  double A  =(ax*x+ay*y+az*z);  if(A!=0.) A=1./A; x*=A; y*=A; z*=A;

  const double S[3] = { x, y, z };
  double s[5] = {};
  mutl3x5Helper(s, S, &P[7]);
  globalToLocalVecHelper(P, s[0], s[1], s[2], s[3], s[4]);
  // Jacobian production
  //
  const double Av[3] = { (RC * Ay[0] - RS * Ax[0]) * (R = 1. / R),
                         (RC * Ay[1] - RS * Ax[1]) * R,
                         (RC * Ay[2] - RS * Ax[2]) * R };

  mutl3x5Helper(&Jac[0], Av, &P[7]);
  mutl3x5Helper(&Jac[5], Az, &P[7]);
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Straight line  system coordinate
/////////////////////////////////////////////////////////////////////////////////

void
transformGlobalToLine(const Amg::Transform3D&  T,
                      bool useJac,
                      double* ATH_RESTRICT P,
                      double* ATH_RESTRICT par,
                      double* ATH_RESTRICT Jac)
{
  const double A[3] = {T(0,2),T(1,2),T(2,2)};

  double Bx = A[1]*P[5]-A[2]*P[4];
  double By = A[2]*P[3]-A[0]*P[5];
  double Bz = A[0]*P[4]-A[1]*P[3];
  const double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz);
  Bx*=Bn; By*=Bn; Bz*=Bn;
  const double x  = P[0]-T(0,3);
  const double y  = P[1]-T(1,3);
  const double z  = P[2]-T(2,3);
  par[0]    = x*Bx  +y*By  +z*Bz  ;
  par[1]    = x*A[0]+y*A[1]+z*A[2];

  if(!useJac) return;

  // Condition trajectory on surface
  //
  const double d  = P[3]*A[0]+P[4]*A[1]+P[5]*A[2];
  double a  = (1.-d)*(1.+d); if(a!=0.) a=1./a;
  const double X = d*A[0]-P[3], Y = d*A[1]-P[4], Z = d*A[2]-P[5];

  const double d1 = P[10]*A[0]+P[11]*A[1]+P[12]*A[2];
  const double d2 = P[17]*A[0]+P[18]*A[1]+P[19]*A[2];
  const double d3 = P[24]*A[0]+P[25]*A[1]+P[26]*A[2];
  const double d4 = P[31]*A[0]+P[32]*A[1]+P[33]*A[2];
  const double d5 = P[38]*A[0]+P[39]*A[1]+P[40]*A[2];
  const double s1 = (((P[ 7]*X+P[ 8]*Y+P[ 9]*Z)+x*(d1*A[0]-P[10]))+(y*(d1*A[1]-P[11])+z*(d1*A[2]-P[12])))*a;
  const double s2 = (((P[14]*X+P[15]*Y+P[16]*Z)+x*(d2*A[0]-P[17]))+(y*(d2*A[1]-P[18])+z*(d2*A[2]-P[19])))*a;
  const double s3 = (((P[21]*X+P[22]*Y+P[23]*Z)+x*(d3*A[0]-P[24]))+(y*(d3*A[1]-P[25])+z*(d3*A[2]-P[26])))*a;
  const double s4 = (((P[28]*X+P[29]*Y+P[30]*Z)+x*(d4*A[0]-P[31]))+(y*(d4*A[1]-P[32])+z*(d4*A[2]-P[33])))*a;
  const double s5 = (((P[35]*X+P[36]*Y+P[37]*Z)+x*(d5*A[0]-P[38]))+(y*(d5*A[1]-P[39])+z*(d5*A[2]-P[40])))*a;

  //pass -1 (As we want do add rather subtract in the helper)
  globalToLocalVecHelper(P, -1.*s1, -1.*s2, -1.*s3, -1.*s4, -1.*s5);

  // Jacobian production
  //
  const double B[3]={Bx,By,Bz};
  mutl3x5Helper(&Jac[0],B,&P[7]);
  mutl3x5Helper(&Jac[5],A,&P[7]);
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Cone  system coordinate
/////////////////////////////////////////////////////////////////////////////////

void
transformGlobalToCone(const Amg::Transform3D&  T,
                      double tA,
                      bool useJac,
                      const double* ATH_RESTRICT P,
                      double* ATH_RESTRICT par,
                      double* ATH_RESTRICT Jac)
{

  const double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  const double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  const double Az[3] = {T(0,2),T(1,2),T(2,2)};

  const double x  = P[0]-T(0,3);
  const double y  = P[1]-T(1,3);
  const double z  = P[2]-T(2,3);
  const double RC = x*Ax[0]+y*Ax[1]+z*Ax[2];
  const double RS = x*Ay[0]+y*Ay[1]+z*Ay[2];
  par[1]    = x*Az[0]+y*Az[1]+z*Az[2];
  par[0]    = atan2(RS,RC)*(par[1]*tA);

  if(!useJac) return;

  Jac[ 0] = 0.;        // dL0/dL0
  Jac[ 1] = 0.;        // dL0/dL1
  Jac[ 2] = 0.;        // dL0/dPhi
  Jac[ 3] = 0.;        // dL0/dThe
  Jac[ 4] = 0.;        // dL0/dCM
  Jac[ 5] = 0.;        // dL1/dL0
  Jac[ 6] = 0.;        // dL1/dL1
  Jac[ 7] = 0.;        // dL1/dPhi
  Jac[ 8] = 0.;        // dL1/dThe
  Jac[ 9] = 0.;        // dL1/dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Plane local position transformation to global system coordinate
/////////////////////////////////////////////////////////////////////////////////

void
transformPlaneToGlobal(bool useJac,
                       const Amg::Transform3D& T,
                       const AmgVector(5)& ATH_RESTRICT p,
                       double* ATH_RESTRICT P)
{
  const double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  const double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  P[ 0] = p[0]*Ax[0]+p[1]*Ay[0]+T(0,3);                            // X
  P[ 1] = p[0]*Ax[1]+p[1]*Ay[1]+T(1,3);                            // Y
  P[ 2] = p[0]*Ax[2]+p[1]*Ay[2]+T(2,3);                            // Z

  if(!useJac) return;

  //    /dL1   |     /dL2     |   /dPhi   |  /dThe    |
  P[ 7] = Ax[0]; P[14] = Ay[0]; P[21] = 0.; P[28] = 0.;             // dX/
  P[ 8] = Ax[1]; P[15] = Ay[1]; P[22] = 0.; P[29] = 0.;             // dY/
  P[ 9] = Ax[2]; P[16] = Ay[2]; P[23] = 0.; P[30] = 0.;             // dZ/

}


/////////////////////////////////////////////////////////////////////////////////
// Disc local position transformation to global system coordinate
/////////////////////////////////////////////////////////////////////////////////

void
transformDiscToGlobal(bool useJac,
                      const Amg::Transform3D& T,
                      const AmgVector(5)& ATH_RESTRICT p,
                      double* ATH_RESTRICT P)
{
  const double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  const double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  double Sf,Cf; sincos(p[1],&Sf,&Cf);

  const double d0 = Cf*Ax[0]+Sf*Ay[0];
  const double d1 = Cf*Ax[1]+Sf*Ay[1];
  const double d2 = Cf*Ax[2]+Sf*Ay[2];
  P[ 0]     = p[0]*d0+T(0,3)   ;                                    // X
  P[ 1]     = p[0]*d1+T(1,3)   ;                                    // Y
  P[ 2]     = p[0]*d2+T(2,3)   ;                                    // Z

  if(!useJac) return;

  //  /dL1  |              /dL2               |   /dPhi |  /dThe  |
  P[ 7] = d0; P[14] = p[0]*(Cf*Ay[0]-Sf*Ax[0]); P[21]=0.; P[28]=0.; // dX/
  P[ 8] = d1; P[15] = p[0]*(Cf*Ay[1]-Sf*Ax[1]); P[22]=0.; P[29]=0.; // dY/
  P[ 9] = d2; P[16] = p[0]*(Cf*Ay[2]-Sf*Ax[2]); P[23]=0.; P[30]=0.; // dZ/
}

/////////////////////////////////////////////////////////////////////////////////
// Cylinder local position transformation to global system coordinate
/////////////////////////////////////////////////////////////////////////////////

void
transformCylinderToGlobal(bool useJac,
                          const Amg::Transform3D& T,
                          double R,
                          const AmgVector(5)& ATH_RESTRICT p,
                          double* ATH_RESTRICT P)
{
  const double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  const double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  const double Az[3] = {T(0,2),T(1,2),T(2,2)};

  const double fr = p[0]/R;
  double Sf,Cf; sincos(fr,&Sf,&Cf);

  P[ 0]     = R*(Cf*Ax[0]+Sf*Ay[0])+p[1]*Az[0]+T(0,3);              // X
  P[ 1]     = R*(Cf*Ax[1]+Sf*Ay[1])+p[1]*Az[1]+T(1,3);              // Y
  P[ 2]     = R*(Cf*Ax[2]+Sf*Ay[2])+p[1]*Az[2]+T(2,3);              // Z

  if(!useJac) return;

  //           /dL1        |    /dL2      |   /dPhi   |   /dThe   |
  P[ 7] = Cf*Ay[0]-Sf*Ax[0]; P[14] = Az[0]; P[21] = 0.; P[28] = 0.; // dX/
  P[ 8] = Cf*Ay[1]-Sf*Ax[1]; P[15] = Az[1]; P[22] = 0.; P[29] = 0.; // dY/
  P[ 9] = Cf*Ay[2]-Sf*Ax[2]; P[16] = Az[2]; P[23] = 0.; P[30] = 0.; // dZ/
}

/////////////////////////////////////////////////////////////////////////////////
// Straight line local position transformation to global system coordinate
/////////////////////////////////////////////////////////////////////////////////

void
transformLineToGlobal(bool useJac,
                      const Amg::Transform3D& T,
                      const AmgVector(5)& ATH_RESTRICT p,
                      double* ATH_RESTRICT P)
{
  const double A[3] = {T(0,2),T(1,2),T(2,2)};

  double Bx = A[1]*P[5]-A[2]*P[4];
  double By = A[2]*P[3]-A[0]*P[5];
  double Bz = A[0]*P[4]-A[1]*P[3];
  const double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz);
  Bx*=Bn; By*=Bn; Bz*=Bn;
  P[ 0]     = p[1]*A[0]+Bx*p[0]+T(0,3);                             // X
  P[ 1]     = p[1]*A[1]+By*p[0]+T(1,3);                             // Y
  P[ 2]     = p[1]*A[2]+Bz*p[0]+T(2,3);                             // Z

  if(!useJac) return;

  double Bx2 =           -A[2]*P[25], Bx3 = A[1]*P[33]-A[2]*P[32];
  double By2 = A[2]*P[24]           , By3 = A[2]*P[31]-A[0]*P[33];
  double Bz2 = A[0]*P[25]-A[1]*P[24], Bz3 = A[0]*P[32]-A[1]*P[31];
  double B2  = Bx*Bx2+By*By2+Bz*Bz2 , B3  = Bx*Bx3+By*By3+Bz*Bz3 ;
  Bx2        =(Bx2-Bx*B2)*Bn        ; Bx3 =(Bx3-Bx*B3)*Bn        ;
  By2        =(By2-By*B2)*Bn        ; By3 =(By3-By*B3)*Bn        ;
  Bz2        =(Bz2-Bz*B2)*Bn        ; Bz3 =(Bz3-Bz*B3)*Bn        ;

  //  /dL1  |     /dL2    |      /dPhi      |     /dThe       |
  P[ 7] = Bx; P[14] = A[0]; P[21] = Bx2*p[0]; P[28] = Bx3*p[0];     // dX/
  P[ 8] = By; P[15] = A[1]; P[22] = By2*p[0]; P[29] = By3*p[0];     // dY/
  P[ 9] = Bz; P[16] = A[2]; P[23] = Bz2*p[0]; P[30] = Bz3*p[0];     // dZ/
}


}//end of anonymous namespace for internal implementation methods


/////////////////////////////////////////////////////////////////////////////////
// Common transformation from local to global system coordinates for all surfaces
// for charged track parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaUtils::transformLocalToGlobal
(bool useJac,const Trk::TrackParameters& Tp,double* P)
{
  const Trk::TrackParameters* pTp  = &Tp; if(!pTp) return false;

  return transformLocalToGlobal(useJac,&Tp.associatedSurface(),Tp.parameters(),P);
}

/////////////////////////////////////////////////////////////////////////////////
// Common transformation from local to global system coordinates for all surfaces
// for neutral track parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaUtils::transformLocalToGlobal
(bool useJac,const Trk::NeutralParameters& Tp,double* P)
{
  const Trk::NeutralParameters* pTp = &Tp; if(!pTp) return false;

  return transformLocalToGlobal(useJac,&Tp.associatedSurface(),Tp.parameters(),P);
}

/////////////////////////////////////////////////////////////////////////////////
// Common transformation from local to global system coordinates for all surfaces
// for pattern parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaUtils::transformLocalToGlobal
(bool useJac,const Trk::PatternTrackParameters& Tp,double* P)
{
  // TODO: Remove copies when Trk::PatternTrackParameters migrates uses AMG types.
  const double * p = Tp.par();
  AmgVector(5) tmp;
  tmp << p[0], p[1], p[2], p[3], p[4];

  return transformLocalToGlobal(useJac,Tp.associatedSurface(),tmp,P);
}

/////////////////////////////////////////////////////////////////////////////////
// Common transformation from global to local system coordinates for all surfaces
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToLocal
(double*ATH_RESTRICT P,double*ATH_RESTRICT par)
{
  par[2]  = atan2(P[4],P[3]);
  par[3]  = acos (P[5]);
  par[4]  = P[6];
}

void Trk::RungeKuttaUtils::transformGlobalToLocal
(const Trk::Surface* su,bool useJac, double*ATH_RESTRICT P,double*ATH_RESTRICT par,double*ATH_RESTRICT Jac)
{
  par[2]  = atan2(P[4],P[3]);
  par[3]  = acos (P[5]);
  par[4]  = P[6];


  const unsigned int ty = su->type();
  const Amg::Transform3D&  T = su->transform();
  switch (ty) {
    case Trk::Surface::Plane: {
      transformGlobalToPlane(T, useJac, P, par, Jac);
      break;
    }
    // Perigee call the same as line
    case Trk::Surface::Line:
    case Trk::Surface::Perigee: {
      transformGlobalToLine(T, useJac, P, par, Jac);
      break;
    }
    case Trk::Surface::Cylinder: {
      transformGlobalToCylinder(
        T,
        static_cast<const Trk::CylinderSurface*>(su)->bounds().r(),
        useJac,
        P,
        par,
        Jac);
      break;
    }
    case Trk::Surface::Disc: {
      transformGlobalToDisc(T, useJac, P, par, Jac);
      break;
    }
    case Trk::Surface::Cone: {
      transformGlobalToCone(
        T,
        static_cast<const Trk::ConeSurface*>(su)->bounds().tanAlpha(),
        useJac,
        P,
        par,
        Jac);
      break;
    }
  }

  if(!useJac) return;

  double P3,P4, C = P[3]*P[3]+P[4]*P[4];
  if(C > 1.e-20) {C= 1./C ; P3 = P[3]*C; P4 =P[4]*C; C =-sqrt(C);}
  else           {C=-1.e10; P3 = 1.    ; P4 =0.    ;             }

  Jac[10] = P3*P[11]-P4*P[10];    // dPhi/dL0
  Jac[11] = P3*P[18]-P4*P[17];    // dPhi/dL1
  Jac[12] = P3*P[25]-P4*P[24];    // dPhi/dPhi
  Jac[13] = P3*P[32]-P4*P[31];    // dPhi/dThe
  Jac[14] = P3*P[39]-P4*P[38];    // dPhi/dCM
  Jac[15] = C*P[12];              // dThe/dL0
  Jac[16] = C*P[19];              // dThe/dL1
  Jac[17] = C*P[26];              // dThe/dPhi
  Jac[18] = C*P[33];              // dThe/dThe
  Jac[19] = C*P[40];              // dThe/dCM
  Jac[20] = P  [41];              // dCM /dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Plane system coordinate
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Main program for step estimation to surfaces
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimator
(int kind,double* Su,const double* P,bool& Q)
{
  switch (kind) {
    case 0: {
      return stepEstimatorToStraightLine(Su, P, Q);
    }
    case 1: {
      return stepEstimatorToPlane(Su, P, Q);
    }
    case 2: {
      return stepEstimatorToCylinder(Su, P, Q);
    }
    case 3: {
      return stepEstimatorToCone(Su, P, Q);
    }
    default: {
      return 1000000.;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Plane
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToPlane
(const double*ATH_RESTRICT S,const double*ATH_RESTRICT P,bool& Q)
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction

  const double  A = a[0]*S[0]+a[1]*S[1]+a[2]*S[2]; if(A==0.) {Q=false; return 1000000.;}
  const double  D = (S[3]-r[0]*S[0])-(r[1]*S[1]+r[2]*S[2]);
  Q=true;  return (D/A);
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Cylinder
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToCylinder
(double*ATH_RESTRICT S,const double*ATH_RESTRICT P,bool& Q)
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction

  double dx = r[0]-S[0];
  double dy = r[1]-S[1];
  double dz = r[2]-S[2];
  double B  = dx  *S[3]+dy  *S[4]+dz  *S[5];
  double C  = a[0]*S[3]+a[1]*S[4]+a[2]*S[5];
  double ax = a[0]-S[3]*C;
  dx-=(B*S[3]);
  double ay = a[1]-S[4]*C;
  dy-=(B*S[4]);
  double az = a[2]-S[5]*C;
  dz-=(B*S[5]);
  double A  = 2.*(ax*ax+ay*ay+az*az);
  if(A==0.) {Q=false; return 0.;}
  B         = 2.*(ax*dx+ay*dy+az*dz);
  double g  = dx+dy+dz;
  C         = (g-S[6])*(g+S[6])-2.*(dx*(dy+dz)+dy*dz);
  double Sq = B*B-2.*A*C;

  double Smin = -B/A, Smax = Smin;

  if( Sq > 0.) {

    Sq = sqrt(Sq)/A; if(B>0.) {Smin+=Sq; Smax-=Sq;} else {Smin-=Sq; Smax+=Sq;}
  }
  else {
    if(fabs(Smax) < .1) {Q = false; return 0.;}
  }

  Q = true;
  const double inside = Smin*Smax;

  if(S[8]!=0.) {

    if(inside > 0. || S[8] > 0.) return Smin;
    if(S[7] >=0. ) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  if(inside < 0.) {

    S[8]=-1.;
    if(S[7]>=0.) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  //if(fabs(Smin) < .001) {S[8]=-1.; return Smax;}

  S[8]= 1.; return Smin;
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Straight Line
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToStraightLine
(const double*ATH_RESTRICT S,const double*ATH_RESTRICT P,bool& Q)
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction

  double D = a[0]*S[3]+a[1]*S[4]+a[2]*S[5];
  const double A = (1.-D)*(1.+D);
  if(A==0.) {Q=true;  return 1000000.;}
  D        = (r[0]-S[0])*(D*S[3]-a[0])
            +(r[1]-S[1])*(D*S[4]-a[1])
            +(r[2]-S[2])*(D*S[5]-a[2]);
  Q=true; return (D/A);
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Cone
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToCone
(double*ATH_RESTRICT S,const double*ATH_RESTRICT P,bool& Q)
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction

  const double dx = r[0]-S[0];
  const double dy = r[1]-S[1];
  const double dz = r[2]-S[2];
  const double A  = dx  *S[3]+dy  *S[4]+dz  *S[5];
  const double B  = a[0]*S[3]+a[1]*S[4]+a[2]*S[5];
  const double C  = a[0]* dx +a[1]*dy  +a[2]*dz  ;
  const double k  = S[6]                         ;

  double KB   = 1.-k*B*B;
  double KABC = k*A*B-C ;
  double Smin,Smax      ;

  if(KB!=0.) {

    Smin       = KABC/KB; Smax = Smin;
    double Sq  = KABC*KABC+(k*A*A-dx*dx-dy*dy-dz*dz)*KB;
    if(Sq >= 0.) {
      Sq = sqrt(Sq)/KB; if(KABC>0.) {Smin-=Sq; Smax+=Sq;} else {Smin+=Sq; Smax-=Sq;}
      int n = 2;
      if( A+B*Smin < 0.) {--n; Smin = Smax;}
      if( A+B*Smax < 0.) {--n; Smax = Smin;}
      if(!n) {Q = false; return 0.;}

    }
    else         {
      Q = false; return 0.;
    }
  }
  else      {
    Smin = (dx*dx+dy*dy+dz*dz-k*A*A)/(2.*KABC); Smax = Smin;
    if( A+B*Smin < 0.) {Q = false; return 0.;}

  }

  Q = true;
  const double inside = Smin*Smax;

  if(S[8]!=0.) {

    if(inside > 0. || S[8] > 0.) return Smin;
    if(S[7] >=0. ) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  if(inside < 0.) {

    S[8]=-1.;
    if(S[7]>=0.) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  S[8]= 1.; return Smin;
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to surfaces
//
// Input iformation
//
// SU    - input vector surfaces and conditions for boundary check
// DN    - is work map
// Pinp  - array of track parameters in global system coordinates before some step
// Pout  - array of track parameters in global system coordinates before some step
// W     - trajectory total pass for Pinp position
// So    - max. step for straight line propagation model
// Nv    - surface number in vector SU  which we don't need to use
//
// Output information
//
// pair(step to surface,surface number from vector SU)
// next  - condition to use Pout for next step
/////////////////////////////////////////////////////////////////////////////////

std::pair<double,int> Trk::RungeKuttaUtils::stepEstimator
(std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >& SU,std::multimap<double,int>& DN,
 const double*ATH_RESTRICT Pinp,const double*ATH_RESTRICT Pout,double W,double So,int Nv,bool& next)
{
  W             = fabs(W)                                          ;
  next          = false                                            ;
  int N         = -1                                               ;
  double D [3]  = {Pout[0]-Pinp[0],Pout[1]-Pinp[1],Pout[2]-Pinp[2]};
  double Smax   = sqrt(D[0]*D[0]+D[1]*D[1]+D[2]*D[2])              ;
  double Sign   = D[0]*Pinp[3]+D[1]*Pinp[4]+D[2]*Pinp[5]           ;

  Amg::Vector3D pos( Pinp[0]  , Pinp[1]  , Pinp[2]  );
  Amg::Vector3D dir( D[0]/Smax, D[1]/Smax, D[2]/Smax);

  double Smin = 2.*Smax;

  std::list<std::pair<double,int> > LD;
  std::multimap<double,int>::iterator i = DN.begin(),ie = DN.end();

  for(; i!=ie; ++i) {

    if((*i).first > W+Smin) break;

    int j = (*i).second;
    Trk::DistanceSolution ds = SU[j].first->straightLineDistanceEstimate(pos,dir,SU[j].second);
    LD.emplace_back(ds.currentDistance(false)+W,j);

    int  n = ds.numberOfSolutions(); if(!n) continue;

    for(int i=0; i!=n; ++i) {
      double s; i == 0 ? s=ds.first() : s=ds.second();
      if(s < 0. || s > Smin || (j==Nv && s < So)) continue;
      Smin = s; N = j;
    }
  }

  if(!LD.empty()) {

    DN.erase(DN.begin(),i);
    std::list<std::pair<double,int> >::iterator l = LD.begin(), le = LD.end();
    for(; l!=le; ++l) DN.insert((*l));
  }

  if(N < 0) {next = true; return std::make_pair(0.,-1);}

  double Sm = Smin;

  if(Sign < 0.) Sm = -Sm;
  if(Smin < So || (Smax-Smin)  > 2.*So) return std::make_pair(Sm,N);

  Amg::Vector3D  posn(Pout[0],Pout[1],Pout[2]);
  Amg::Vector3D  dirn(Pout[3],Pout[4],Pout[5]);

  Trk::DistanceSolution ds = SU[N].first->straightLineDistanceEstimate(posn,dirn,SU[N].second);
  int  n = ds.numberOfSolutions(); if(!n) return std::make_pair(Sm,N);
  Sm = 10000.;

  for(int i=0; i!=n; ++i) {

    double sa,s; i == 0 ? s = ds.first() : s = ds.second(); sa = fabs(s);

    if(s*Sign < 0.) {
      //if(sa < So    ) {next = true; return std::make_pair(s,N);}
      next = true; return std::make_pair(s,N);
    }
    if(sa < Sm ) {
      Sm = s; next = true;
    }
    /*
    else if(sa < fabs(Sm) && Smin >= Smax) {
      Sm = s; next = true;
    }
    */
  }
  return std::make_pair(Sm, N);
}

/////////////////////////////////////////////////////////////////////////////////
// New covariance matrix calculation from old matrix and jacobian
/////////////////////////////////////////////////////////////////////////////////

AmgSymMatrix(5) * Trk::RungeKuttaUtils::newCovarianceMatrix(
                    const double* ATH_RESTRICT J,
                    const AmgSymMatrix(5) & M)
{
  AmgSymMatrix(5)* nM = new AmgSymMatrix(5);
  AmgSymMatrix(5)& m = (*nM);

  Eigen::Map<const AmgVector(5)> JacMap0(&J[0], 5, 1);
  const AmgVector(5) a1 = M * JacMap0; //(5x5 * 5x1)
  m(0, 0) = a1.dot(JacMap0);           // dot product

  Eigen::Map<const AmgVector(5)> JacMap5(&J[5]);
  const AmgVector(5) a2 = M * JacMap5;
  m(1, 0) = a2.dot(JacMap0);
  m(1, 1) = a2.dot(JacMap5);
  m(0, 1) = m(1, 0);

  Eigen::Map<const AmgVector(5)> JacMap10(&J[10], 5, 1);
  const AmgVector(5) a3 = M * JacMap10;
  m(2, 0) = a3.dot(JacMap0);
  m(2, 1) = a3.dot(JacMap5);
  m(2, 2) = a3.dot(JacMap10);
  m(0, 2) = m(2, 0);
  m(1, 2) = m(2, 1);

  Eigen::Map<const AmgVector(5)> JacMap15(&J[15], 5, 1);
  const AmgVector(5) a4 = M * JacMap15;
  m(3, 0) = a4.dot(JacMap0);
  m(3, 1) = a4.dot(JacMap5);
  m(3, 2) = a4.dot(JacMap10);
  m(3, 3) = a4.dot(JacMap15);
  m(0, 3) = m(3, 0);
  m(1, 3) = m(3, 1);
  m(2, 3) = m(3, 2);

  const AmgVector(5) a5 = M.row(4) * J[20];
  m(4, 0) = a5.dot(JacMap0);
  m(4, 1) = a5.dot(JacMap5);
  m(4, 2) = a5.dot(JacMap10);
  m(4, 3) = a5.dot(JacMap15);
  m(4, 4) = a5[4] * J[20];
  m(0, 4) = m(4, 0);
  m(1, 4) = m(4, 1);
  m(2, 4) = m(4, 2);
  m(3, 4) = m(4, 3);

  return nM;
}

/////////////////////////////////////////////////////////////////////////////////
// Tramsform from local to global for all track parameters
/////////////////////////////////////////////////////////////////////////////////

bool
Trk::RungeKuttaUtils::transformLocalToGlobal(bool useJac,
                                             const Trk::Surface* Su,
                                             const AmgVector(5)& ATH_RESTRICT p,
                                             double* ATH_RESTRICT P)
{
  if(!Su) return false;

  double Sf,Cf,Ce,Se;
  sincos(p[2],&Sf,&Cf);
  sincos(p[3],&Se,&Ce);

  P[ 3] = Cf*Se;                                                         // Ax
  P[ 4] = Sf*Se;                                                         // Ay
  P[ 5] = Ce;                                                            // Az
  P[ 6] = p[4];                                                          // CM
  if(fabs(P[6])<.000000000000001) {
    P[6]<0. ? P[6]=-.000000000000001 : P[6]= .000000000000001;
  }

  if(useJac) {

    //   /dL1   |   /dL2    |    /dPhi    |    /dThe     |    /dCM     |
                                                           P[35] =   0.; // dX /
                                                           P[36] =   0.; // dY /
                                                           P[37] =   0.; // dZ /
    P[10]  =  0.; P[17] = 0.; P[24] =-P[4]; P[31] = Cf*Ce; P[38] =   0.; // dAx/
    P[11]  =  0.; P[18] = 0.; P[25] = P[3]; P[32] = Sf*Ce; P[39] =   0.; // dAy/
    P[12]  =  0.; P[19] = 0.; P[26] =   0.; P[33] =   -Se; P[40] =   0.; // dAz/
    P[13]  =  0.; P[20] = 0.; P[27] =   0.; P[34] =    0.; P[41] =   1.; // dCM/
    P[42]  =  0.; P[43] = 0.; P[44] =   0.;                              // d(Ax,Ay,Az)/ds
  }

  const unsigned int ty = Su->type();
  const Amg::Transform3D& T = Su->transform();
  switch (ty) {
    case Trk::Surface::Plane: {
      transformPlaneToGlobal(useJac, T, p, P);
      return true;
    }
    case Trk::Surface::Line:
    case Trk::Surface::Perigee: {
      transformLineToGlobal(useJac, T, p, P);
      return true;
    }
    case Trk::Surface::Cylinder: {
      transformCylinderToGlobal(
        useJac,
        T,
        static_cast<const Trk::CylinderSurface*>(Su)->bounds().r(),
        p,
        P);
      return true;
    }
    case Trk::Surface::Disc: {
      transformDiscToGlobal(useJac, T, p, P);
      return true;
    }
    default: {
      return false;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to curvilinear system coordinate
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToCurvilinear
(bool useJac,double*ATH_RESTRICT P,double*ATH_RESTRICT par,double*ATH_RESTRICT Jac)
{
  par[0] = 0.;
  par[1] = 0.;

  if(!useJac) return;

  const double An    = sqrt(P[3]*P[3]+P[4]*P[4]);
  double Ax[3];
  if(An!=0.) {Ax[0] = -P[4]/An; Ax[1] = P[3]/An; Ax[2] = 0.;}
  else       {Ax[0] =       1.; Ax[1] = 0.;      Ax[2] = 0.;}

  const double Ay[3] = {-Ax[1]*P[5],Ax[0]*P[5],An  };
  double S [3] = {    P[3]   , P[4]     ,P[5]};

  double    A = P[3]*S[0]+P[4]*S[1]+P[5]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  double s[5]={};
  mutl3x5Helper(s,S,&P[7]);
  globalToLocalVecHelper(P, s[0], s[1], s[2], s[3], s[4]);

  double P3,P4,C = P[3]*P[3]+P[4]*P[4];
  if(C > 1.e-20) {C= 1./C ; P3 = P[3]*C; P4 =P[4]*C; C =-sqrt(C);}
  else           {C=-1.e10; P3 = 1.    ; P4 =0.    ;             }

  // Jacobian production
  //
  Jac[ 0] = Ax[0]*P[ 7]+Ax[1]*P[ 8];               // dL0/dL0
  Jac[ 1] = Ax[0]*P[14]+Ax[1]*P[15];               // dL0/dL1
  Jac[ 2] = Ax[0]*P[21]+Ax[1]*P[22];               // dL0/dPhi
  Jac[ 3] = Ax[0]*P[28]+Ax[1]*P[29];               // dL0/dThe
  Jac[ 4] = Ax[0]*P[35]+Ax[1]*P[36];               // dL0/dCM
  Jac[ 5] = Ay[0]*P[ 7]+Ay[1]*P[ 8]+Ay[2]*P[ 9];   // dL1/dL0
  Jac[ 6] = Ay[0]*P[14]+Ay[1]*P[15]+Ay[2]*P[16];   // dL1/dL1
  Jac[ 7] = Ay[0]*P[21]+Ay[1]*P[22]+Ay[2]*P[23];   // dL1/dPhi
  Jac[ 8] = Ay[0]*P[28]+Ay[1]*P[29]+Ay[2]*P[30];   // dL1/dThe
  Jac[ 9] = Ay[0]*P[35]+Ay[1]*P[36]+Ay[2]*P[37];   // dL1/dCM
  Jac[10] = P3*P[11]-P4*P[10];                     // dPhi/dL0
  Jac[11] = P3*P[18]-P4*P[17];                     // dPhi/dL1
  Jac[12] = P3*P[25]-P4*P[24];                     // dPhi/dPhi
  Jac[13] = P3*P[32]-P4*P[31];                     // dPhi/dThe
  Jac[14] = P3*P[39]-P4*P[38];                     // dPhi/dCM
  Jac[15] = C*P[12];                               // dThe/dL0
  Jac[16] = C*P[19];                               // dThe/dL1
  Jac[17] = C*P[26];                               // dThe/dPhi
  Jac[18] = C*P[33];                               // dThe/dThe
  Jac[19] = C*P[40];                               // dThe/dCM
  Jac[20] = P[41];                                 // dCM /dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Curvilinear covariance transformation to global system coordinate
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformCurvilinearToGlobal
(double*ATH_RESTRICT p, double*ATH_RESTRICT P)
{
  double Sf,Cf,Ce,Se; sincos(p[2],&Sf,&Cf);  sincos(p[3],&Se,&Ce);

  const double Ax[3] = {-Sf   , Cf   , 0.};
  const double Ay[3] = {-Cf*Ce,-Sf*Ce, Se};

  //   /dL1     |   /dL2       |    /dPhi     |    /dThe     |    /dCM     |
  //
  P[ 7]  = Ax[0]; P[14] = Ay[0]; P[21] =    0.; P[28] =    0.; P[35] =   0.; // dX /
  P[ 8]  = Ax[1]; P[15] = Ay[1]; P[22] =    0.; P[29] =    0.; P[36] =   0.; // dY /
  P[ 9]  = Ax[2]; P[16] = Ay[2]; P[23] =    0.; P[30] =    0.; P[37] =   0.; // dZ /
  P[10]  =    0.; P[17] =    0.; P[24] =-Sf*Se; P[31] =-Ay[0]; P[38] =   0.; // dAx/
  P[11]  =    0.; P[18] =    0.; P[25] = Cf*Se; P[32] =-Ay[1]; P[39] =   0.; // dAy/
  P[12]  =    0.; P[19] =    0.; P[26] =    0.; P[33] =-Ay[2]; P[40] =   0.; // dAz/
  P[13]  =    0.; P[20] =    0.; P[27] =    0.; P[34] =    0.; P[41] =   1.; // dCM/
  P[42]  =    0.; P[43] =    0.; P[44] =    0.;

}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to local system coordinates
// for  Trk::TrackParameters
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToLocal
(const Trk::TrackParameters& Tp,double* Jac)
{
  const AmgVector(5)& Vp = Tp.parameters();
  double P[23];
  P[0] = Vp[0];
  P[1] = Vp[1];
  P[2] = Vp[2];
  P[3] = Vp[3];
  jacobianTransformCurvilinearToLocal(P,&Tp.associatedSurface(),Jac);
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to local system coordinates
// for Trk::PatternTrackParameters
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToLocal
      (const Trk::PatternTrackParameters& Tp,double* Jac)
{
  double P[23];
  P[0] = Tp.par()[0];
  P[1] = Tp.par()[1];
  P[2] = Tp.par()[2];
  P[3] = Tp.par()[3];
  jacobianTransformCurvilinearToLocal(P,Tp.associatedSurface(),Jac);
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to local system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToLocal
(double*ATH_RESTRICT P, const Trk::Surface* su,double*ATH_RESTRICT Jac)
{
  // Common for all surfaces terms of jacobian
  //
  Jac[ 4] = 0.;   // dL0/dCM
  Jac[ 9] = 0.;   // dL1/dCM
  Jac[10] = 0.;   // dPhi/dL0
  Jac[11] = 0.;   // dPhi/dL1
  Jac[12] = 1.;   // dPhi/dPhi
  Jac[13] = 0.;   // dPhi/dThe
  Jac[14] = 0.;   // dPhi/dCM
  Jac[15] = 0.;   // dThe/dL0
  Jac[16] = 0.;   // dThe/dL1
  Jac[17] = 0.;   // dThe/dPhi
  Jac[18] = 1.;   // dThe/dThe
  Jac[19] = 0.;   // dThe/dCM
  Jac[20] = 1.;   // dCM /dCM

  const Amg::Transform3D& T = su->transform();

  double Sf,Cf,Ce,Se;
  sincos(P[2],&Sf,&Cf);
  sincos(P[3],&Se,&Ce);

  P[ 4] =  Cf*Se; P[ 5] =  Sf*Se; P[ 6] = Ce    ;   // At
  P[ 7] = -Sf   ; P[ 8] =  Cf   ; P[ 9] = 0.    ;   // Au
  P[10] = -Cf*Ce; P[11] = -Sf*Ce; P[12] = Se    ;   // Av
  P[13] = T(0,0); P[14] = T(1,0); P[15] = T(2,0);   // Ax
  P[16] = T(0,1); P[17] = T(1,1); P[18] = T(2,1);   // Ay
  P[19] = T(0,2); P[20] = T(1,2); P[21] = T(2,2);   // Az

  const unsigned int ty = su->type();

  if(ty == Trk::Surface::Plane   )  {jacobianTransformCurvilinearToPlane       (P,Jac); return;}
  if(ty == Trk::Surface::Line    )  {jacobianTransformCurvilinearToStraightLine(P,Jac); return;}
  if(ty == Trk::Surface::Cylinder)  {
    P[22] = static_cast<const Trk::CylinderSurface*>(su)->bounds().r();
                                     jacobianTransformCurvilinearToCylinder    (P,Jac); return;
  }
  if(ty == Trk::Surface::Perigee )  {jacobianTransformCurvilinearToStraightLine(P,Jac); return;}
  if(ty == Trk::Surface::Disc    )  {jacobianTransformCurvilinearToDisc        (P,Jac); return;}
  Jac[0] = Jac[3] = 1.;
  Jac[1] = Jac[2] = 0.;
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Plane system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToPlane
(double*ATH_RESTRICT P,double*ATH_RESTRICT Jac)
{
  double* At = &P[ 4];
  double* Au = &P[ 7];
  double* Av = &P[10];
  double* Ax = &P[13];
  double* Ay = &P[16];
  double* S  = &P[19];

  double    A = At[0]*S[0]+At[1]*S[1]+At[2]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  const double s1 = Au[0]*S[0]+Au[1]*S[1];
  const double s2 = Av[0]*S[0]+Av[1]*S[1]+Av[2]*S[2];

  Au[0]-=(s1*At[0]); Au[1]-=(s1*At[1]); Au[2]-=(s1*At[2]);
  Av[0]-=(s2*At[0]); Av[1]-=(s2*At[1]); Av[2]-=(s2*At[2]);

  Jac[ 0] = Ax[0]*Au[0]+Ax[1]*Au[1]+Ax[2]*Au[2]; // dL0/dL0
  Jac[ 1] = Ax[0]*Av[0]+Ax[1]*Av[1]+Ax[2]*Av[2]; // dL0/dL1
  Jac[ 2] = 0.                                 ; // dL0/dPhi
  Jac[ 3] = 0.                                 ; // dL0/dThe
  Jac[ 5] = Ay[0]*Au[0]+Ay[1]*Au[1]+Ay[2]*Au[2]; // dL1/dL0
  Jac[ 6] = Ay[0]*Av[0]+Ay[1]*Av[1]+Ay[2]*Av[2]; // dL1/dL1
  Jac[ 7] = 0.                                 ; // dL1/dPhi
  Jac[ 8] = 0.                                 ; // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Disc system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToDisc
(double*ATH_RESTRICT P,double*ATH_RESTRICT Jac)
{
  const double* p  = &P[ 0];
  const double* At = &P[ 4];
  double* Au = &P[ 7];
  double* Av = &P[10];
  const double* Ax = &P[13];
  const double* Ay = &P[16];
  double* S  = &P[19];

  // Condition trajectory on surface
  //
  double    A = At[0]*S[0]+At[1]*S[1]+At[2]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  const double s1 = Au[0]*S[0]+Au[1]*S[1];
  const double s2 = Av[0]*S[0]+Av[1]*S[1]+Av[2]*S[2];

  Au[0]-=(s1*At[0]); Au[1]-=(s1*At[1]); Au[2]-=(s1*At[2]);
  Av[0]-=(s2*At[0]); Av[1]-=(s2*At[1]); Av[2]-=(s2*At[2]);

  // Jacobian production
  //
  double Sf,Cf; sincos(p[1],&Sf,&Cf);

  const double Ri = 1./p[0]              ;
  const double A0 =(Cf*Ax[0]+Sf*Ay[0])   ;
  const double A1 =(Cf*Ax[1]+Sf*Ay[1])   ;
  const double A2 =(Cf*Ax[2]+Sf*Ay[2])   ;
  const double B0 =(Cf*Ay[0]-Sf*Ax[0])*Ri;
  const double B1 =(Cf*Ay[1]-Sf*Ax[1])*Ri;
  const double B2 =(Cf*Ay[2]-Sf*Ax[2])*Ri;

  Jac[ 0] = A0*Au[0]+A1*Au[1]+A2*Au[2];   // dL0/dL0
  Jac[ 1] = A0*Av[0]+A1*Av[1]+A2*Av[2];   // dL0/dL1
  Jac[ 2] = 0.                        ;   // dL0/dPhi
  Jac[ 3] = 0.                        ;   // dL0/dThe
  Jac[ 5] = B0*Au[0]+B1*Au[1]+B2*Au[2];   // dL1/dL0
  Jac[ 6] = B0*Av[0]+B1*Av[1]+B2*Av[2];   // dL1/dL1
  Jac[ 7] = 0.                        ;   // dL1/dPhi
  Jac[ 8] = 0.                        ;   // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Cylinder system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToCylinder
(double*ATH_RESTRICT P,double*ATH_RESTRICT Jac)
{
  const double* p  = &P[ 0];
  const double* At = &P[ 4];
  double* Au = &P[ 7];
  double* Av = &P[10];
  const double* Ax = &P[13];
  const double* Ay = &P[16];
  const double* Az = &P[19];
  const double  R  =  P[22];

  const double fr  = p[0]/R;
  double Sf,Cf;
  sincos(fr,&Sf,&Cf);

  const double x   = Cf*Ax[0]+Sf*Ay[0];
  const double y   = Cf*Ax[1]+Sf*Ay[1];
  const double z   = Cf*Ax[2]+Sf*Ay[2];

  // Condition trajectory on surface
  //
  const double C  = At[0]*Az[0]+At[1]*Az[1]+At[2]*Az[2];
  const double ax = At[0]-Az[0]*C;
  const double ay = At[1]-Az[1]*C;
  const double az = At[2]-Az[2]*C;
  double A  =(ax*x+ay*y+az*z); if(A!=0.) A=1./A;

  const double s1 = (Au[0]*x+Au[1]*y        )*A;
  const double s2 = (Av[0]*x+Av[1]*y+Av[2]*z)*A;

  Au[0]-=(s1*At[0]); Au[1]-=(s1*At[1]); Au[2]-=(s1*At[2]);
  Av[0]-=(s2*At[0]); Av[1]-=(s2*At[1]); Av[2]-=(s2*At[2]);

  // Jacobian production
  //
  const double A0 =(Cf*Ay[0]-Sf*Ax[0]);
  const double A1 =(Cf*Ay[1]-Sf*Ax[1]);
  const double A2 =(Cf*Ay[2]-Sf*Ax[2]);

  Jac[ 0] = A0*Au[0]+A1*Au[1]+A2*Au[2]         ;  // dL0/dL0
  Jac[ 1] = A0*Av[0]+A1*Av[1]+A2*Av[2]         ;  // dL0/dL1
  Jac[ 2] = 0.                                 ;  // dL0/dPhi
  Jac[ 3] = 0.                                 ;  // dL0/dThe
  Jac[ 5] = Az[0]*Au[0]+Az[1]*Au[1]+Az[2]*Au[2];  // dL1/dL0
  Jac[ 6] = Az[0]*Av[0]+Az[1]*Av[1]+Az[2]*Av[2];  // dL1/dL1
  Jac[ 7] = 0.                                 ;  // dL1/dPhi
  Jac[ 8] = 0.                                 ;  // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Straight Line Ssystem coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToStraightLine
(const double*ATH_RESTRICT P,double*ATH_RESTRICT Jac)
{
  const double* p  = &P[ 0];
  const double* At = &P[ 4];
  const double* Au = &P[ 7];
  const double* Av = &P[10];
  const double* A  = &P[19];

  double Bx = A[1]*At[2]-A[2]*At[1];
  double By = A[2]*At[0]-A[0]*At[2];
  double Bz = A[0]*At[1]-A[1]*At[0];
  const double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz);
  Bx*=Bn; By*=Bn; Bz*=Bn;

  // Condition trajectory on surface
  //
  const double d  = At[0]*A[0]+At[1]*A[1]+At[2]*A[2];
  double a  = (1.-d)*(1.+d);
  if(a!=0.) a=1./a;
  const double X  = d*A[0]-At[0], Y = d*A[1]-At[1], Z = d*A[2]-At[2];

  const double s1 =       (Au[0]*X+Au[1]*Y          )*a;
  const double s2 =       (Av[0]*X+Av[1]*Y+Av[2]*Z  )*a;
  const double s3 = p[0]*(Bx*At[1]-By*At[0]         )*a;
  const double s4 = p[0]*(Bx*Av[0]+By*Av[1]+Bz*Av[2])*a;

  // Jacobian production
  //
  Jac[ 0] = Bx*Au[0]+By*Au[1]         ;             // dL0/dL0
  Jac[ 1] = Bx*Av[0]+By*Av[1]+Bz*Av[2];             // dL0/dL1
  Jac[ 2] = 0.;                                     // dL0/dPhi
  Jac[ 3] = 0.;                                     // dL0/dThe
  Jac[ 5] = A[0]*Au[0]+A[1]*Au[1]            +s1*d; // dL1/dL0
  Jac[ 6] =(A[0]*Av[0]+A[1]*Av[1]+A[2]*Av[2])+s2*d; // dL1/dL1
  Jac[ 7] = s3*d;                                   // dL1/dPhi
  Jac[ 8] = s4*d;                                   // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Fill distances map (initial map<distance,N of SU> preparation
// Output
// Step[0] - min negative step
// Step[1] - min positive step
// Step[2] - max distance
// int     - vector SU element number with initial surface
/////////////////////////////////////////////////////////////////////////////////

int Trk::RungeKuttaUtils::fillDistancesMap
(std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >& SU,
 std::multimap<double,int>& DN,
 const double*ATH_RESTRICT Pinp,double W,const Trk::Surface* So,double*ATH_RESTRICT Step)
{
  int Ns = -1;
  DN.erase(DN.begin(),DN.end());

  Amg::Vector3D pos(Pinp[0],Pinp[1],Pinp[2]);
  Amg::Vector3D dir(Pinp[3],Pinp[4],Pinp[5]);

  Step[0] = -1.e+20;
  Step[1] =  1.e+20;
  Step[2] =      0.;

  int N = SU.size(); W=fabs(W);

  for(int i=0; i!=N; ++i ) {

    const Trk::Surface* s = SU[i].first; if(!s) continue; if(s==So) Ns = i;
    Trk::DistanceSolution ds = s->straightLineDistanceEstimate(pos,dir,SU[i].second);
    double dist = ds.currentDistance(false);
    DN.insert(std::make_pair(dist+W,i));

    if(dist > Step[2]) Step[2] = dist;

    int  n = ds.numberOfSolutions(); if(!n) continue;
    for(int i=0; i!=n; ++i) {

      double st; i == 0 ? st=ds.first() : st=ds.second();

      if(s==So && fabs(st)<=.001) continue;

      if(st < 0.) {if(st > Step[0]) Step[0] = st;}
      else        {if(st < Step[1]) Step[1] = st;}
    }
  }
  return Ns;
}
