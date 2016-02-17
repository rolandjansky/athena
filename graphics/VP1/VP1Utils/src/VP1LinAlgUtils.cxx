/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1LinAlgUtils                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Base/VP1Msg.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoTransform.h>


#include <math.h>


class VP1LinAlgUtils::Imp {
public:
  static double atlasR;
  static bool isBad( const double& x, const QString& ctxStr, const double& limit = 1.0e20 )
  {
    if (x==x && fabs(x) < limit)
      return false;
    if (!ctxStr.isEmpty())
      VP1Msg::messageDebug("VP1LinAlgUtils ("+ctxStr+") ERROR: Saw suspicious number "+VP1Msg::str(x));
    return true;
  }
};
double VP1LinAlgUtils::Imp::atlasR = 1.0*CLHEP::km;

//______________________________________________________________________________________
SoTransform * VP1LinAlgUtils::toSoTransform(const HepGeom::Transform3D & transformation, SoTransform * tin)
{
  HepGeom::Vector3D<double> translation = transformation.getTranslation();
  CLHEP::HepRotation rotation   = transformation.getRotation();
  SoTransform *myTransform = tin ? tin : new SoTransform();
  myTransform->translation = SbVec3f(translation.x(),
				     translation.y(),
				     translation.z());
  myTransform->rotation = SbRotation(SbMatrix(rotation.xx(),rotation.yx(),rotation.zx(),0,
					      rotation.xy(),rotation.yy(),rotation.zy(),0,
					      rotation.xz(),rotation.yz(),rotation.zz(),0,
					      0,0,0,1));
  if (VP1Msg::verbose()) {
    bool inputok = isSane( transformation, "input HepGeom::Transform3D" );
    bool outputok = isSane( myTransform, "output SoTransform" );
    if (!inputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform ERROR: Problems observed in input HepGeom::Transform3D");
    if (!outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform ERROR: Problems observed in output SoTransform");
    if (inputok!=outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform Error introduced in HepGeom::Transform3D -> SoTransform conversion!");
  }

  return myTransform;
}

//______________________________________________________________________________________
SoTransform * VP1LinAlgUtils::toSoTransform(const Amg::Transform3D & transformation, SoTransform * tin)
{
  Amg::Vector3D	 translation = transformation.translation();
  Amg::RotationMatrix3D rotation   = transformation.rotation();

  SoTransform *myTransform = tin ? tin : new SoTransform();
  myTransform->translation = SbVec3f(translation.x(),
				     translation.y(),
				     translation.z());
//  myTransform->rotation = SbRotation(SbMatrix(rotation.xx(),rotation.yx(),rotation.zx(),0,
//					      rotation.xy(),rotation.yy(),rotation.zy(),0,
//					      rotation.xz(),rotation.yz(),rotation.zz(),0,
//					      0,0,0,1));
  myTransform->rotation = SbRotation(SbMatrix(rotation(0,0),rotation(1,0),rotation(2,0),0,
					      rotation(0,1),rotation(1,1),rotation(2,1),0,
					      rotation(0,2),rotation(1,2),rotation(2,2),0,
					      0,0,0,1));

  // FIXME: to be updated to Eigen
  if (VP1Msg::verbose()) {
    bool inputok = isSane( transformation, "input HepGeom::Transform3D" );
    bool outputok = isSane( myTransform, "output SoTransform" );
    if (!inputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform ERROR: Problems observed in input HepGeom::Transform3D");
    if (!outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform ERROR: Problems observed in output SoTransform");
    if (inputok!=outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform Error introduced in HepGeom::Transform3D -> SoTransform conversion!");
  }

  return myTransform;
}

//______________________________________________________________________________________
void VP1LinAlgUtils::transformToMatrix(SoTransform * xf, SbMatrix& result) {
  result.makeIdentity();
  result.setTransform(xf->translation.getValue(), xf->rotation.getValue(),
		      xf->scaleFactor.getValue(), xf->scaleOrientation.getValue(), xf->center.getValue());
  if (VP1Msg::verbose()) {
    bool inputok = isSane( xf, "input SoTransform" );
    bool outputok = isSane( result, "output SbMatrix" );
    if (!inputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::transformToMatrix(SoTransform*,..) ERROR: Problems observed in input SoTransform");
    if (!outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::transformToMatrix(SoTransform*,..) ERROR: Problems observed in output SbMatrix");
    if (inputok!=outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::transformToMatrix(SoTransform*,..) Error introduced in SoTransform -> SbMatrix conversion!");
  }
}

//______________________________________________________________________________________
void VP1LinAlgUtils::transformToMatrix(const HepGeom::Transform3D & heptr, SbMatrix& result)
{
  result = SbMatrix(heptr.xx(),heptr.yx(),heptr.zx(),0,
 		    heptr.xy(),heptr.yy(),heptr.zy(),0,
 		    heptr.xz(),heptr.yz(),heptr.zz(),0,
 		    heptr.dx(),heptr.dy(),heptr.dz(),1);
  if (VP1Msg::verbose()) {
    bool inputok = isSane( heptr, "input HepGeom::Transform3D" );
    bool outputok = isSane( result, "output SbMatrix" );
    if (!inputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::transformToMatrix(HepGeom::Transform3D,..) ERROR: Problems observed in input HepGeom::Transform3D");
    if (!outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::transformToMatrix(HepGeom::Transform3D,..) ERROR: Problems observed in output SbMatrix");
    if (inputok!=outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::transformToMatrix(HepGeom::Transform3D,..) Error introduced in HepGeom::Transform3D -> SbMatrix conversion!");
  }
}

//______________________________________________________________________________________
SoTransform * VP1LinAlgUtils::toSoTransform(const SbMatrix& matr, SoTransform * tin)
{
  SbVec3f translation, scaleFactor;
  SbRotation rotation, scaleOrientation;
  matr.getTransform(translation, rotation, scaleFactor, scaleOrientation, SbVec3f(0,0,0));
  SoTransform * t = tin ? tin : new SoTransform;
  t->translation.setValue(translation);
  t->rotation.setValue(rotation);
  t->scaleFactor.setValue(scaleFactor);
  t->scaleOrientation.setValue(scaleOrientation);
  t->center.setValue(0,0,0);
  if (VP1Msg::verbose()) {
    bool inputok = isSane( matr, "input SbMatrix" );
    bool outputok = isSane( t, "output SoTransform" );
    if (!inputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform ERROR: Problems observed in input SoTransform");
    if (!outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform ERROR: Problems observed in output SbMatrix");
    if (inputok!=outputok)
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform Error introduced in SoTransform -> SbMatrix conversion!");
  }
  return t;
}

//______________________________________________________________________________________
void VP1LinAlgUtils::decodeTransformation( const SbMatrix& matr,
					   float& translation_x, float& translation_y, float& translation_z,
					   float& rotaxis_x, float& rotaxis_y, float& rotaxis_z, float& rotangle_radians )
{
  SbVec3f translation, scaleFactor, rotaxis;
  SbRotation rotation, scaleOrientation;
  matr.getTransform (translation, rotation,scaleFactor,scaleOrientation,SbVec3f(0,0,0));
  translation.getValue(translation_x,translation_y,translation_z);
  rotation.getValue(rotaxis,rotangle_radians);
  rotaxis.getValue(rotaxis_x,rotaxis_y,rotaxis_z);
  if (VP1Msg::verbose()) {
    if (!isSane( matr, "input SbMatrix" ))
      VP1Msg::messageVerbose("VP1LinAlgUtils::decodeTransformation ERROR: Problems observed in output SbMatrix");
  }
}

#ifdef BAD
#undef BAD
#endif
#define BAD(x) { ok=false; if (!ctxStr.isEmpty()) VP1Msg::messageVerbose("VP1LinAlgUtils ("+ctxStr+") ERROR: "+QString(x)); };

//______________________________________________________________________________________
bool VP1LinAlgUtils::isSane( const HepGeom::Transform3D & t, const QString& ctxStr )
{

  HepGeom::Vector3D<double> translation = t.getTranslation();
  CLHEP::HepRotation rotation   = t.getRotation();
  CLHEP::Hep3Vector  rotationAxis;
  double      rotationAngle;
  rotation.getAngleAxis(rotationAngle,rotationAxis);

  bool ok(true);

  if (Imp::isBad(t.xx(),ctxStr)||Imp::isBad(t.xy(),ctxStr)||Imp::isBad(t.xz(),ctxStr)
      ||Imp::isBad(t.yx(),ctxStr)||Imp::isBad(t.yy(),ctxStr)||Imp::isBad(t.yz(),ctxStr)
      ||Imp::isBad(t.zx(),ctxStr)||Imp::isBad(t.zy(),ctxStr)||Imp::isBad(t.zz(),ctxStr)
      ||Imp::isBad(t.dx(),ctxStr)||Imp::isBad(t.dy(),ctxStr)||Imp::isBad(t.dz(),ctxStr))
    BAD("Problem in raw 4x4 transformation matrix!");
  if (Imp::isBad(translation.x(),ctxStr)||Imp::isBad(translation.y(),ctxStr)||Imp::isBad(translation.z(),ctxStr))
    BAD("Problem in translation!");
  if (Imp::isBad(rotation.xx(),ctxStr)||Imp::isBad(rotation.xy(),ctxStr)||Imp::isBad(rotation.xz(),ctxStr)
      ||Imp::isBad(rotation.yx(),ctxStr)||Imp::isBad(rotation.yy(),ctxStr)||Imp::isBad(rotation.yz(),ctxStr)
      ||Imp::isBad(rotation.zx(),ctxStr)||Imp::isBad(rotation.zy(),ctxStr)||Imp::isBad(rotation.zz(),ctxStr))
    BAD("Problem in rotation");
  if (Imp::isBad(rotationAxis.x(),ctxStr)||Imp::isBad(rotationAxis.y(),ctxStr)||Imp::isBad(rotationAxis.z(),ctxStr))
    BAD("Problem in rotationAxis");
  if (Imp::isBad(rotationAngle,ctxStr))
    BAD("Problem in rotationAngle");

  //Check that input rotation has det=1
  const double det = t.xx()*t.yy()*t.zz()
    +t.zx()*t.xy()*t.yz()
    +t.xz()*t.yx()*t.zy()
    -t.xx()*t.zy()*t.yz()
    -t.yx()*t.xy()*t.zz()
    -t.zx()*t.yy()*t.xz();
  const double eps = 1.0e-5;
  if (det<1.0-eps||det>1.0+eps)
    BAD("Determinant of rotation part is not consistent with 1 (det="+QString::number(det)+")!");

  return ok;
}


//______________________________________________________________________________________
bool VP1LinAlgUtils::isSane( const Amg::Transform3D & t, const QString& ctxStr )
{
  Amg::Vector3D translation = t.translation();

  Amg::RotationMatrix3D rotation   = t.rotation();

  double      rotationAngle;
  Amg::Vector3D  rotationAxis;

  Amg::getAngleAxisFromRotation(rotation, rotationAngle, rotationAxis);

//  double xx = rotation(0,0);
//  double yy = rotation(1,1);
//  double zz = rotation(2,2);
//
//  double cosa  = 0.5 * (xx + yy + zz - 1);
//  double cosa1 = 1 - cosa;
//
//  if (cosa1 <= 0) {
//       rotationAngle = 0;
//       rotationAxis  = Amg::Vector3D(0,0,1);
//     }
//  else{
//       double x=0, y=0, z=0;
//       if (xx > cosa) x = sqrt((xx-cosa)/cosa1);
//       if (yy > cosa) y = sqrt((yy-cosa)/cosa1);
//       if (zz > cosa) z = sqrt((zz-cosa)/cosa1);
//       if (rotation(2,1) < rotation(1,2)) x = -x;
//       if (rotation(0,2) < rotation(2,0)) y = -y;
//       if (rotation(1,0) < rotation(0,1)) z = -z;
//       rotationAngle = (cosa < -1.) ? acos(-1.) : acos(cosa);
//       rotationAxis  = Amg::Vector3D(x,y,z);
//     }




  bool ok(true);

  if (Imp::isBad(t(0,0),ctxStr)||Imp::isBad(t(0,1),ctxStr)||Imp::isBad(t(0,2),ctxStr)
      ||Imp::isBad(t(1,0),ctxStr)||Imp::isBad(t(1,1),ctxStr)||Imp::isBad(t(1,2),ctxStr)
      ||Imp::isBad(t(2,0),ctxStr)||Imp::isBad(t(2,1),ctxStr)||Imp::isBad(t(2,2),ctxStr)
//      ||Imp::isBad(t.dx(),ctxStr)||Imp::isBad(t.dy(),ctxStr)||Imp::isBad(t.dz(),ctxStr))
      ||Imp::isBad(t(0,3),ctxStr)||Imp::isBad(t(1,3),ctxStr)||Imp::isBad(t(2,3),ctxStr))
    BAD("Problem in raw 4x4 transformation matrix!");
  if (Imp::isBad(translation.x(),ctxStr)||Imp::isBad(translation.y(),ctxStr)||Imp::isBad(translation.z(),ctxStr))
    BAD("Problem in translation!");
  if (Imp::isBad(rotation(0,0),ctxStr)||Imp::isBad(rotation(0,1),ctxStr)||Imp::isBad(rotation(0,2),ctxStr)
      ||Imp::isBad(rotation(1,0),ctxStr)||Imp::isBad(rotation(1,1),ctxStr)||Imp::isBad(rotation(1,2),ctxStr)
      ||Imp::isBad(rotation(2,0),ctxStr)||Imp::isBad(rotation(2,1),ctxStr)||Imp::isBad(rotation(2,2),ctxStr))
    BAD("Problem in rotation");
  if (Imp::isBad(rotationAxis.x(),ctxStr)||Imp::isBad(rotationAxis.y(),ctxStr)||Imp::isBad(rotationAxis.z(),ctxStr))
    BAD("Problem in rotationAxis");
  if (Imp::isBad(rotationAngle,ctxStr))
    BAD("Problem in rotationAngle");

  //Check that input rotation has det=1
//  const double det = t.xx()*t.yy()*t.zz()
//    +t.zx()*t.xy()*t.yz()
//    +t.xz()*t.yx()*t.zy()
//    -t.xx()*t.zy()*t.yz()
//    -t.yx()*t.xy()*t.zz()
//    -t.zx()*t.yy()*t.xz();
  const double det = t.matrix().determinant();
  const double eps = 1.0e-5;
  if ( det < 1.0-eps || det > 1.0+eps )
    BAD("Determinant of rotation part is not consistent with 1 (det="+QString::number(det)+")!");

  return ok;
}


//______________________________________________________________________________________
bool VP1LinAlgUtils::isSane( const SoTransform * t, const QString& ctxStr )
{
  if (!t) {
    if (!ctxStr.isEmpty())
      VP1Msg::messageVerbose("VP1LinAlgUtils::toSoTransform ("+ctxStr+") ERROR: NULL SoTransform!");
    return false;
  }

  bool ok(true);
  float q0,q1, q2,q3;
  t->rotation.getValue().getValue(q0,q1, q2,q3);
  if (Imp::isBad(q0,ctxStr)||Imp::isBad(q1,ctxStr)||Imp::isBad(q2,ctxStr)||Imp::isBad(q3,ctxStr))
    BAD("Problem in quarternion representation of rotation!");
  t->scaleOrientation.getValue().getValue(q0,q1, q2,q3);
  if (Imp::isBad(q0,ctxStr)||Imp::isBad(q1,ctxStr)||Imp::isBad(q2,ctxStr)||Imp::isBad(q3,ctxStr))
    BAD("Problem in quarternion representation of scaleOrientation!");
  float x,y,z;
  t->translation.getValue().getValue(x,y,z);
  if (Imp::isBad(x,ctxStr,Imp::atlasR)||Imp::isBad(y,ctxStr,Imp::atlasR)||Imp::isBad(z,ctxStr,Imp::atlasR))
    BAD("Problem in translation!");
  t->scaleFactor.getValue().getValue(x,y,z);
  if (Imp::isBad(x,ctxStr,Imp::atlasR)||Imp::isBad(y,ctxStr,Imp::atlasR)||Imp::isBad(z,ctxStr,Imp::atlasR))
    BAD("Problem in scaleFactor!");
  t->center.getValue().getValue(x,y,z);
  if (Imp::isBad(x,ctxStr,Imp::atlasR)||Imp::isBad(y,ctxStr,Imp::atlasR)||Imp::isBad(z,ctxStr,Imp::atlasR))
    BAD("Problem in center!");

  return ok;
}

//______________________________________________________________________________________
bool VP1LinAlgUtils::isSane( const SbMatrix& matr, const QString& ctxStr )
{
  bool ok(true);

  bool baseok(true);
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      if (Imp::isBad(matr[i][j],ctxStr)) {
	baseok=false;
	break;
      }
  if (!baseok)
    BAD("Problem in raw 4x4 matrix!");

  SbVec3f translation, scaleFactor, rotaxis;
  SbRotation rotation, scaleOrientation;
  matr.getTransform(translation, rotation,scaleFactor,scaleOrientation,SbVec3f(0,0,0));
  float q0,q1, q2,q3;
  rotation.getValue(q0,q1, q2,q3);
  if (Imp::isBad(q0,ctxStr)||Imp::isBad(q1,ctxStr)||Imp::isBad(q2,ctxStr)||Imp::isBad(q3,ctxStr))
    BAD("Problem in quarternion representation of rotation!");
  scaleOrientation.getValue(q0,q1, q2,q3);
  if (Imp::isBad(q0,ctxStr)||Imp::isBad(q1,ctxStr)||Imp::isBad(q2,ctxStr)||Imp::isBad(q3,ctxStr))
    BAD("Problem in quarternion representation of scaleOrientation!");
  float x,y,z;
  translation.getValue(x,y,z);
  if (Imp::isBad(x,ctxStr,Imp::atlasR)||Imp::isBad(y,ctxStr,Imp::atlasR)||Imp::isBad(z,ctxStr,Imp::atlasR))
    BAD("Problem in translation!");
  scaleFactor.getValue(x,y,z);
  if (Imp::isBad(x,ctxStr,Imp::atlasR)||Imp::isBad(y,ctxStr,Imp::atlasR)||Imp::isBad(z,ctxStr,Imp::atlasR))
    BAD("Problem in scaleFactor!");

  const float eps = 1.0e-5;
  const float det3 = matr.det3();
  if (det3<1.0-eps||det3>1.0+eps)
    BAD("Determinant of rotation part is not consistent with 1 (det3="+QString::number(det3)+")!");
  const float det4 = matr.det4();
  if (det4<1.0-eps||det4>1.0+eps)
    BAD("Determinant of rotation part is not consistent with 1 (det4="+QString::number(det4)+")!");
  return ok;
}

//_____________________________________________________________________________________
double VP1LinAlgUtils::phiFromXY(const double& x, const double&y )
{
  //Special case: Returns 0 if both x and y are 0
  double phi = (x == 0.0f)? (y==0.0f?0.0:0.5*M_PI) : std::abs(atan(std::abs(y/static_cast<double>(x))));
  if (x>=0.0f&&y<0.0f)
    phi = 2*M_PI-phi;
  else if (x<0.0f&&y>=0.0f)
    phi = M_PI-phi;
  else if (x<0.0f&&y<0.0f)
    phi = M_PI+phi;
  return phi;
}


//_____________________________________________________________________________________
/* http://www.mycplus.com/tutorial.asp?TID=80 */
void VP1LinAlgUtils::distPointLineParam(const Amg::Vector3D& point, const Amg::Vector3D& point0, const Amg::Vector3D& point1, double& s)
{
//  s = ((point - point0)*(point1 - point0)) / (((point1 - point0)*(point1 - point0)));

     s = ( ((point)[0]-(point0)[0])*((point1)[0]-(point0)[0]) +
           ((point)[1]-(point0)[1])*((point1)[1]-(point0)[1]) +
           ((point)[2]-(point0)[2])*((point1)[2]-(point0)[2]) ) /
         ( ((point1)[0]-(point0)[0])*((point1)[0]-(point0)[0]) +
           ((point1)[1]-(point0)[1])*((point1)[1]-(point0)[1]) +
           ((point1)[2]-(point0)[2])*((point1)[2]-(point0)[2]) );

  return;
}

//_____________________________________________________________________________________
/* http://www.mycplus.com/tutorial.asp?TID=80 */
double VP1LinAlgUtils::distPointLine2(const Amg::Vector3D& point, const Amg::Vector3D& point0, const Amg::Vector3D& point1, double& s)
{
  VP1LinAlgUtils::distPointLineParam(point, point0, point1, s);

  //   double dx = (*point)[0]-((*point0)[0] + s*((*point1)[0]-(*point0)[0]));
  //   double dy = (*point)[1]-((*point0)[1] + s*((*point1)[1]-(*point0)[1]));
  //   double dz = (*point)[2]-((*point0)[2] + s*((*point1)[2]-(*point0)[2]));

  Amg::Vector3D d = point-(point0 + s*(point1-point0));

  //return dx*dx + dy*dy + dz*dz;
  return d.dot(d);
}

//_____________________________________________________________________________________
/* http://www.mycplus.com/tutorial.asp?TID=81 */
void VP1LinAlgUtils::distLineLineParam(const Amg::Vector3D& point0, const Amg::Vector3D& point1, const Amg::Vector3D& point2, const Amg::Vector3D& point3, double& s, double& t)
{
  //Actual distance not calculated, only parameters s and t.

  Amg::Vector3D p32 = point3 - point2;
  double d3232 = p32.dot(p32);
  Amg::Vector3D p10 = point1 - point0;
  double d1010 = p10.dot(p10);
  if (d3232==0.0||d1010==0.0||d3232!=d3232||d1010!=d1010) {
    //One input line ill-defined:
    s=0.5;
    t=0.5;
    VP1Msg::messageDebug("VP1LinAlgUtils distLineLineParam ERROR: One or both input lines ill defined");
    return;
  }

  double d3210 = p32.dot(p10);
  double denom = d1010 * d3232 - d3210 * d3210;
  Amg::Vector3D p02 = point0 - point2;
  double d0232 = p02.dot(p32);

  if (denom==0) {
    //Parallel input!
    VP1Msg::messageDebug("VP1LinAlgUtils distLineLineParam Warning: Input lines are parallel. Choosing arbitrary point of closest approach");
    s = 0.5;//We choose an arbitrary point on one of the two lines.
  } else {
    double d0210 = p02.dot(p10);
    double numer = d0232 * d3210 - d0210 * d3232;
    s = numer / denom;
  }
  t =  (d0232 + d3210 * s) / d3232;

  return;
}
