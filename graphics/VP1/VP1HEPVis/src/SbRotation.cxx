/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * VP1HEPVis
 * SbRotation.cxx
 *
 *  Created on: Dec 13, 2012
 *      Author: rbianchi <Riccardo.Maria.Bianchi@cern.ch>
 *
 */


/*
 *
 * New file for HEPVis
 *
 * taken from: http://fossies.org/dox/osc_vis_source_16.11.6/HEPVis_2include_2HEPVis_2SbRotation_8cxx_source.html
 *
 * R.M. Bianchi <rbianchi@cern.ch>
 *
 * 12.12.2012
 *
 *===================
 * VP1 Customization:
 *
 *  - look into the code for comments "// VP1 change [...] //---"
 *
 */

//---




// this :
// VP1 change
#include <VP1HEPVis/SbRotation.h>
//---

#include <math.h>

#include <Inventor/SbVec3d.h>

HEPVis::SbRotation::SbRotation()
//NOTE : the default HEPVis::SbRotation() corresponds to a unit matrix.
//WARNING : Inventor/SbRotation()::quad is not initialized !
//          So that the HEPVis::SbRotation() does not the same than
//          the Inventor::SbRotation().
:m_quat(0,0,0,1)
{
}

HEPVis::SbRotation::SbRotation(const SbVec3d& axis,double radians) {
  //FIXME : if(axis.length()==0) //throw
  m_quat[3] = ::cos(radians/2);
  double sineval = ::sin(radians/2);
  SbVec3d a = axis;
  a.normalize();
  m_quat[0] = a[0] * sineval;
  m_quat[1] = a[1] * sineval;
  m_quat[2] = a[2] * sineval;
}

// from SbRotation& SbRotation::setValue(const SbMatrix & m)

HEPVis::SbRotation::SbRotation(
 double a11, double a12,double a13, double a14
,double a21, double a22,double a23, double a24
,double a31, double a32,double a33, double a34
,double a41, double a42,double a43, double a44
) {

  double m[4][4] = { { a11, a12, a13, a14 },
                     { a21, a22, a23, a24 },
                     { a31, a32, a33, a34 },
                     { a41, a42, a43, a44 } };

  double scalerow = m[0][0] + m[1][1] + m[2][2];

  if (scalerow > 0.0) {
    double s = ::sqrt(scalerow + m[3][3]);
    m_quat[3] = s * 0.5;
    s = 0.5 / s;

    m_quat[0] = (m[1][2] - m[2][1]) * s;
    m_quat[1] = (m[2][0] - m[0][2]) * s;
    m_quat[2] = (m[0][1] - m[1][0]) * s;
  }
  else {
    int i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;

    int j = (i+1)%3;
    int k = (j+1)%3;

    double s = ::sqrt((m[i][i] - (m[j][j] + m[k][k])) + m[3][3]);

    m_quat[i] = s * 0.5;
    s = 0.5 / s;

    m_quat[3] = (m[j][k] - m[k][j]) * s;
    m_quat[j] = (m[i][j] + m[j][i]) * s;
    m_quat[k] = (m[i][k] + m[k][i]) * s;
  }

  if (m[3][3] != 1.0) {
    m_quat *= (1.0/::sqrt(m[3][3]));
  }
}

//HEPVis::SbRotation& HEPVis::SbRotation::operator*=(const double s){
//  m_quat *= s;
//  return *this;
//}

void HEPVis::SbRotation::multVec(const SbVec3d& src,SbVec3d& dst) const {
  //SbMatrix mat;
  //mat.setRotate(*this);

  double x = m_quat[0];
  double y = m_quat[1];
  double z = m_quat[2];
  double w = m_quat[3];

  double matrix[4][4];
  matrix[0][0] = w*w + x*x - y*y - z*z;
  matrix[0][1] = 2*x*y + 2*w*z;
  matrix[0][2] = 2*x*z - 2*w*y;
  matrix[0][3] = 0;

  matrix[1][0] = 2*x*y-2*w*z;
  matrix[1][1] = w*w - x*x + y*y - z*z;
  matrix[1][2] = 2*y*z + 2*w*x;
  matrix[1][3] = 0;

  matrix[2][0] = 2*x*z + 2*w*y;
  matrix[2][1] = 2*y*z - 2*w*x;
  matrix[2][2] = w*w - x*x - y*y + z*z;
  matrix[2][3] = 0;

  matrix[3][0] = 0;
  matrix[3][1] = 0;
  matrix[3][2] = 0;
  matrix[3][3] = w*w + x*x + y*y + z*z;

  //mat.multVecMatrix(src, dst);
  //if(SbMatrixP::isIdentity(this->matrix)) { dst = src; return; }

  double* t0 = matrix[0];
  double* t1 = matrix[1];
  double* t2 = matrix[2];
  double* t3 = matrix[3];

  SbVec3d s = src;

  double W = s[0]*t0[3] + s[1]*t1[3] + s[2]*t2[3] + t3[3];

  dst[0] = (s[0]*t0[0] + s[1]*t1[0] + s[2]*t2[0] + t3[0])/W;
  dst[1] = (s[0]*t0[1] + s[1]*t1[1] + s[2]*t2[1] + t3[1])/W;
  dst[2] = (s[0]*t0[2] + s[1]*t1[2] + s[2]*t2[2] + t3[2])/W;
}



