/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class MuonChamberProjectionHelper       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: January 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PRDSystems/MuonChamberProjectionHelper.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Base/IVP1System.h"

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelUtilities/GeoModelExperiment.h"

#include <map>

//____________________________________________________________________
class MuonChamberProjectionHelper::Imp {
public:
  Imp( MuonChamberProjectionHelper* tc,
       StoreGateSvc * ds ) : theclass(tc),
			     detectorStore(ds),
			     initattempted(false),
			     initsucceeded(false) {}
  MuonChamberProjectionHelper* theclass;
  StoreGateSvc * detectorStore;
  bool initattempted;
  bool initsucceeded;
  bool ensureInited();//Returns false if can't init.
  bool init();//Returns false if can't init.

  //MDT chambers:
  class MDTChamberInfo {
    public:
    MDTChamberInfo(const Amg::Transform3D& l2g, const GeoTrd* t)
      : localToGlobal(l2g), globalToLocal(0), trd(t) {}
    Amg::Transform3D localToGlobal;
    Amg::Transform3D * globalToLocal;//Only created on demand - saving ~100KB in a typical job.
    const GeoTrd * trd;
    void ensureInitGlobalToLocal() { if (!globalToLocal) globalToLocal = new Amg::Transform3D(localToGlobal.inverse()); }
    ~MDTChamberInfo() { delete globalToLocal; }
  };
  std::map<GeoPVConstLink,MDTChamberInfo>::iterator itLastMDTChamberLookedUp;
  std::map<GeoPVConstLink,MDTChamberInfo> mdtchambervolinfo;//Map typically has around 1124 entries.
  typedef std::map<GeoPVConstLink,MDTChamberInfo>::iterator ChamberInfoMapItr;
  inline bool nameIsMDTChamber( const std::string& n );
  bool getMDTChamberVolInfo(const GeoPVConstLink& mdtChamber,ChamberInfoMapItr& itChamberInfo, bool silent = false );
  void getMDTChamberXAndZ(ChamberInfoMapItr& itChamberInfo, double & trdX, double & trdZ );

  //Todo: CSC and TGC chambers also.


  //Utility:
  inline double pointToPlaneDistAlongLine( const Amg::Vector3D& point, const Amg::Vector3D& lineDirection,
						  const Amg::Vector3D& planePoint, const Amg::Vector3D& planeNormal );
  void projectXZPointToTrdAlongYAxis( const double& x, const double& z,const GeoTrd* trd,
		                  Amg::Vector3D & firstEndWall_point, Amg::Vector3D & secondEndWall_point );

  static bool constrainPointToRectangleAlongLine( const double& trdX, const double& trdZ, const double& x0, const double& z0,
						  double& x1, double& z1 );//Moves x1,z1 towards x0,z0 so (x1,z1) ends up on edge of
                                                                           //rectangle defined by |x|<=trdX && |z|<=trdZ. Returns false if
                                                                           //not possible (i.e. line segment doesn't cross rectangle edges).
  bool clip2DLineSegmentToRectangle( const double& rectX, const double& rectY,
					    double& x0, double& y0, double& x1, double& y1 );
  //Given P0 = (x0,y0) and P1 = (x1,y1), we have to constrain the line
  //segment P0-P1 to the rectangle R given by x<|rectX|, y<|rectY|.
  //
  //Returns false if length of clipped line segment is not positive
  //(in that case, don't trust the returned values of the parameters).

  const GeoTrd * findTRDInShape(const GeoShape * shape)
  {
    if (shape->typeID()==GeoTrd::getClassTypeID())
      return static_cast<const GeoTrd*>(shape);
    if (shape->typeID() == GeoShapeShift::getClassTypeID() ) {
      const GeoShapeShift* theShift = static_cast<const GeoShapeShift*>(shape);
      return findTRDInShape(theShift->getOp());
    }
    if (shape->typeID() == GeoShapeSubtraction::getClassTypeID() ) {
      const GeoShapeSubtraction* theSubtraction = static_cast<const GeoShapeSubtraction*>(shape);
      const GeoTrd * trd = findTRDInShape(theSubtraction->getOpA());
      return trd ? trd : findTRDInShape(theSubtraction->getOpB());
    }
    if (shape->typeID() == GeoShapeUnion::getClassTypeID() ) {
      const GeoShapeUnion* theUnion = static_cast<const GeoShapeUnion*>(shape);
      const GeoTrd * trd = findTRDInShape(theUnion->getOpA());
      return trd ? trd : findTRDInShape(theUnion->getOpB());
    }
    if (shape->typeID() == GeoShapeIntersection::getClassTypeID() ) {
      const GeoShapeIntersection* theIntersection = static_cast<const GeoShapeIntersection*>(shape);
      const GeoTrd * trd = findTRDInShape(theIntersection->getOpA());
      return trd ? trd : findTRDInShape(theIntersection->getOpB());
    }
    return 0;
  }

};



//____________________________________________________________________
MuonChamberProjectionHelper::MuonChamberProjectionHelper( StoreGateSvc * detectorStore )
: VP1HelperClassBase(0,"MuonChamberProjectionHelper"), m_d(new Imp(this,detectorStore))
{
  if (!detectorStore)
    message("ERROR: Received NULL detectorstore");
}

//____________________________________________________________________
MuonChamberProjectionHelper::MuonChamberProjectionHelper( IVP1System * sys )
  : VP1HelperClassBase(0,"MuonChamberProjectionHelper"), m_d(new Imp(this,(sys?sys->detectorStore():0)))
{
  if (!sys)
    message("ERROR: Received NULL system pointer (and thus can't get detector store pointer");
  else if (!m_d->detectorStore)
    message("ERROR: Could not get detectorStore pointer from system pointer");
}

//____________________________________________________________________
MuonChamberProjectionHelper::~MuonChamberProjectionHelper()
{
  Imp::ChamberInfoMapItr itMDT, itMDTE(m_d->mdtchambervolinfo.end());
  for ( itMDT = m_d->mdtchambervolinfo.begin(); itMDT!=itMDTE; ++itMDT )
    itMDT->second.trd->unref();
  delete m_d;
}

//____________________________________________________________________
Amg::Vector3D& MuonChamberProjectionHelper::applyTransformToVector( const Amg::Transform3D& m, Amg::Vector3D& v)
{
  //It is a vector, so we apply the rotation part only:
  double vx = v.x(), vy = v.y(), vz = v.z();
//  v.set(m.xx()*vx + m.xy()*vy + m.xz()*vz,
//	m.yx()*vx + m.yy()*vy + m.yz()*vz,
//	m.zx()*vx + m.zy()*vy + m.zz()*vz);
  Amg::setVector3DCartesian( v,
		  m(0,0)*vx + m(0,1)*vy + m(0,2)*vz,
		  m(1,0)*vx + m(1,1)*vy + m(1,2)*vz,
		  m(2,0)*vx + m(2,1)*vy + m(2,2)*vz);
  return v;
}

//____________________________________________________________________
bool MuonChamberProjectionHelper::Imp::ensureInited()
{
  if (initattempted)
    return initsucceeded;
  initattempted = true;
  initsucceeded = init();
  return initsucceeded;
}

//____________________________________________________________________
inline bool MuonChamberProjectionHelper::Imp::nameIsMDTChamber( const std::string& n )
{
  //MDT chamber volumes starts with:
  //    Endcap: EI, EM, EO or EE
  //    Inner barrel: BI or BEE
  //    Middle barrel: BM
  //    Outer barrel:BO
  if (n.size()<3)
    return false;
  if (n[0]=='E')
    return n[1]=='I' || n[1]=='M' || n[1]=='O' || n[1]=='E';
  if (n[0]=='B')
    return n[1]=='I' || n[1]=='M' || n[1]=='O' || (n[1]=='E'&&n[2]=='E');
  return false;
}

//____________________________________________________________________
bool MuonChamberProjectionHelper::Imp::init()
{
  if (!VP1JobConfigInfo::hasMuonGeometry()) {
    theclass->messageDebug("Warning: Can't init since muon geometry information is not present." );
    return false;
  }

  VP1SGAccessHelper * sgaccess(0);
  if (theclass->systemBase())
    sgaccess = new VP1SGAccessHelper(theclass->systemBase());
  else
    sgaccess = new VP1SGAccessHelper(detectorStore);

 //Locate the world volume if possible:
  const GeoModelExperiment * theExpt;
  if (!sgaccess->retrieve( theExpt, "ATLAS" )) {
    theclass->message("MuonChamberProjectionHelper Error: Can't retrieve"
		      " the ATLAS GeoModelExperiment from detector store.");
    delete sgaccess;
    return false;
  }
  delete sgaccess;

  PVConstLink world(theExpt->getPhysVol());
  GeoVolumeCursor av(world);
  const GeoLogVol * logvol(0);
  const GeoShape * shape(0);
  while (!av.atEnd()) {

    if (av.getName()!="Muon") {
      av.next();
      continue;
    }
    Amg::Transform3D toptransform = Amg::CLHEPTransformToEigen(av.getTransform());
    GeoVolumeCursor av2(av.getVolume());
    while (!av2.atEnd()) {
      if (nameIsMDTChamber(av2.getName())) {
	logvol = av2.getVolume()->getLogVol();
	if (!logvol) {
	  theclass->message("MuonChamberProjectionHelper Error: Chamber has null logvol");
	  av2.next(); // increment volume cursor.
	  continue;
	}
	shape = logvol->getShape();
	if (!shape) {
	  theclass->message("MuonChamberProjectionHelper Error: Chamber has null shape");
	  av2.next(); // increment volume cursor.
	  continue;
	}
	const GeoTrd * trd = findTRDInShape(shape);
	if (trd) {
	  //Fixme: Test for nan's and clearly crazy numbers (if more than 1km for instance).
	  if ( trd->getZHalfLength()>0.0
	       && trd->getXHalfLength1() > 0.0
	       && trd->getXHalfLength2() > 0.0
	       && trd->getYHalfLength1() > 0.0
	       && trd->getYHalfLength2() > 0.0 ) {
	    trd->ref();
	    Amg::Transform3D geovolume_transf = Amg::CLHEPTransformToEigen(av2.getTransform());
	    mdtchambervolinfo.insert(std::pair<const GeoPVConstLink,MDTChamberInfo>( av2.getVolume(), Imp::MDTChamberInfo(toptransform * geovolume_transf, trd) ));
	  } else {
	    theclass->message("MuonChamberProjectionHelper Error: Chamber trd has non-positive shape parameters!");
	  }
	} else {
	  theclass->message("MuonChamberProjectionHelper Error: Chamber shape is not a GeoTrd, and is not a boolean with a Trd somewhere");
	}
      }
      av2.next(); // increment volume cursor.
    }
    av.next(); // increment volume cursor.
  }

  if (mdtchambervolinfo.empty()) {
    theclass->message("MuonChamberProjectionHelper Error: Found no MDT chambers");
    return false;
  }

  itLastMDTChamberLookedUp = mdtchambervolinfo.begin();

  return true;
}

//____________________________________________________________________
bool MuonChamberProjectionHelper::isKnownMDTChamber( const GeoPVConstLink& mdtChamber )
{
  Imp::ChamberInfoMapItr itChamberInfo;
  return m_d->getMDTChamberVolInfo( mdtChamber, itChamberInfo, true );
}

//____________________________________________________________________
bool MuonChamberProjectionHelper::getDistancesToMDTChamberWallsAlongLine( const GeoPVConstLink& mdtChamber,
									  const Amg::Vector3D & point, const Amg::Vector3D& lineDirection,
									  double& distanceToFirstEndPlane, double& distanceToSecondEndPlane,
									  const double& radius )
{
  Imp::ChamberInfoMapItr itChamberInfo;
  if (!m_d->getMDTChamberVolInfo( mdtChamber, itChamberInfo ))
    return false;

  const GeoTrd * trd = itChamberInfo->second.trd;
  double y1(trd->getYHalfLength1()), y2(trd->getYHalfLength2()), z(trd->getZHalfLength());

  //Normals and points on ends (transformed from local to global coords):

  Amg::Vector3D n1(0,+2*z,-(y2-y1));
  Amg::Vector3D n2(0,-2*z,-(y2-y1));
  applyTransformToVector(itChamberInfo->second.localToGlobal,n1);
  applyTransformToVector(itChamberInfo->second.localToGlobal,n2);
  const Amg::Vector3D p1(itChamberInfo->second.localToGlobal * Amg::Vector3D(0,y1,-z) );
  const Amg::Vector3D p2(itChamberInfo->second.localToGlobal * Amg::Vector3D(0,-y1,-z) );

  distanceToFirstEndPlane = m_d->pointToPlaneDistAlongLine(point,lineDirection,p1,n1);
  if (distanceToFirstEndPlane < 0.0 )
    return false;

  distanceToSecondEndPlane = m_d->pointToPlaneDistAlongLine(point,lineDirection,p2,n2);
  if (distanceToSecondEndPlane < 0.0 )
    return false;

  if (radius!=0.0) {
    double r(fabs(radius)); //Map negative radii to positive.
    Amg::Vector3D unitdir(lineDirection.unit());
    double costheta1 = unitdir.dot(n1.unit());
    double costheta2 = unitdir.dot(n2.unit());
    //remember that tan(theta) = sqrt((1-cos^2(theta))/cos(theta))
    distanceToFirstEndPlane += r*sqrt(fabs((1-costheta1*costheta1)/costheta1));
    distanceToSecondEndPlane += r*sqrt(fabs((1-costheta2*costheta2)/costheta2));
  }

  return true;
}
//____________________________________________________________________
inline double MuonChamberProjectionHelper::Imp::pointToPlaneDistAlongLine( const Amg::Vector3D& point, const Amg::Vector3D& lineDirection,
									   const Amg::Vector3D& planePoint, const Amg::Vector3D& planeNormal )
{

  double denominator(planeNormal.dot(lineDirection)*lineDirection.mag());
  if (denominator==0.0) {
    theclass->message("MuonChamberProjectionHelper Error: pointToPlaneDistAlongLine is undefined!");
    return -1.0;
  }
  double numerator(planeNormal.x() * (planePoint.x() - point.x())
		   + planeNormal.y() * (planePoint.y() - point.y())
		   + planeNormal.z() * (planePoint.z() - point.z()));
  return fabs(numerator/denominator);
}

//____________________________________________________________________
bool MuonChamberProjectionHelper::Imp::getMDTChamberVolInfo( const GeoPVConstLink& mdtChamber,
							     MuonChamberProjectionHelper::Imp::ChamberInfoMapItr& itChamberInfo,
							     bool silent ) {

  if (!ensureInited())
    return false;

  if (itLastMDTChamberLookedUp->first == mdtChamber) {
    itChamberInfo = itLastMDTChamberLookedUp;
    return true;
  }

  itChamberInfo = mdtchambervolinfo.find(mdtChamber);
  if (itChamberInfo == mdtchambervolinfo.end()) {
    if (!silent)
      theclass->message("MuonChamberProjectionHelper Error: Can't find MDT chamber among the "
			+QString::number(mdtchambervolinfo.size())+" registered");
    return false;
  }

  itLastMDTChamberLookedUp = itChamberInfo;

  return true;
}

//____________________________________________________________________
void MuonChamberProjectionHelper::Imp::getMDTChamberXAndZ(ChamberInfoMapItr& itChamberInfo, double & trdX, double & trdZ )
{
  const GeoTrd * trd = itChamberInfo->second.trd;
  trdX = trd->getXHalfLength1();
  trdZ = trd->getZHalfLength();
  if ( trdX != trd->getXHalfLength2() ) {
    theclass->message("MuonChamberProjectionHelper Warning: x1!=x2 in GeoTrd shape. Clippings etc. will be to a too large surface.");
    if ( trdX < trd->getXHalfLength2() )
      trdX = trd->getXHalfLength2();
  }

}

//____________________________________________________________________
bool MuonChamberProjectionHelper::projectAndConstrainLineSegmentToMDTChamberEndWalls( const GeoPVConstLink& mdtChamber,
										      const Amg::Vector3D & pointA, const Amg::Vector3D & pointB,
										      Amg::Vector3D & firstEndWall_pointA, Amg::Vector3D & firstEndWall_pointB,
										      Amg::Vector3D & secondEndWall_pointA, Amg::Vector3D & secondEndWall_pointB,
										      bool& outsidechamber )
{
  Imp::ChamberInfoMapItr itChamberInfo;
  if (!m_d->getMDTChamberVolInfo( mdtChamber, itChamberInfo ))
    return false;

  double trdX, trdZ;
  m_d->getMDTChamberXAndZ(itChamberInfo, trdX, trdZ );

  //Get local chamber coordinates, A and B, of pointA and pointB:
  itChamberInfo->second.ensureInitGlobalToLocal();
  Amg::Vector3D A((*(itChamberInfo->second.globalToLocal))*pointA), B((*(itChamberInfo->second.globalToLocal))*pointB);
  double ax(A.x()), az(A.z()), bx(B.x()), bz(B.z());

  //We basically have to project the line segment AB to the
  //(x,z)-plane, and then constrain it to the rectangle given by
  //x_i<|trdX|, z_i<|trdZ|.

  outsidechamber = !(m_d->clip2DLineSegmentToRectangle( trdX, trdZ, ax, az, bx, bz ));
  if (outsidechamber)
    return true;

  //Project the points to the end of the Trd:
  m_d->projectXZPointToTrdAlongYAxis( ax, az,itChamberInfo->second.trd, firstEndWall_pointA, secondEndWall_pointA );
  m_d->projectXZPointToTrdAlongYAxis( bx, bz,itChamberInfo->second.trd, firstEndWall_pointB, secondEndWall_pointB );

  //Put points in global coordinates:
//  firstEndWall_pointA.transform(itChamberInfo->second.localToGlobal);
//  secondEndWall_pointA.transform(itChamberInfo->second.localToGlobal);
//  firstEndWall_pointB.transform(itChamberInfo->second.localToGlobal);
//  secondEndWall_pointB.transform(itChamberInfo->second.localToGlobal);
  Amg::transform(firstEndWall_pointA, itChamberInfo->second.localToGlobal);
  Amg::transform(secondEndWall_pointA, itChamberInfo->second.localToGlobal);
  Amg::transform(firstEndWall_pointB, itChamberInfo->second.localToGlobal);
  Amg::transform(secondEndWall_pointB, itChamberInfo->second.localToGlobal);

  outsidechamber = false;
  return true;
}

//____________________________________________________________________
void MuonChamberProjectionHelper::Imp::projectXZPointToTrdAlongYAxis(const double& x, const double& z,const GeoTrd* trd,
								     Amg::Vector3D & firstEndWall_point, Amg::Vector3D & secondEndWall_point )
{
  const double epsilon(0.1);//100micron
  const double trdY1(trd->getYHalfLength1()), trdY2(trd->getYHalfLength2());
  const double y( trdY1 + 0.5*(1.0+z/trd->getZHalfLength())*(trdY2-trdY1) );
  Amg::setVector3DCartesian(firstEndWall_point, x,y+epsilon,z);
  Amg::setVector3DCartesian(secondEndWall_point, x,-y-epsilon,z);
}

//____________________________________________________________________
bool MuonChamberProjectionHelper::Imp::constrainPointToRectangleAlongLine( const double& trdX, const double& trdZ,
									   const double& x0, const double& z0,
									   double& x1, double& z1 )
{
  //rectangle R : { (x,z) | abs(x)<=trdX && abs(z)<=trdZ }
  //
  //Returns false if segment from (x0,z0) to (x1,z1) doesn't cross R.

  if ( x1 < -trdX ) {
    if ( x0 < -trdX )
      return false;
    //Move (x1,z1) to x == -trdX line:
    z1 += (-trdX-x1)*(z1-z0)/(x1-x0);
    x1 = -trdX;
    if (fabs(z1)>trdZ)
      return constrainPointToRectangleAlongLine(trdX,trdZ,x0,z0,x1,z1);
  }

  if ( x1 > trdX ) {
    if ( x0 > trdX )
      return false;
    //Move (x1,z1) to x == trdX line:
    z1 += (trdX-x1)*(z1-z0)/(x1-x0);
    x1 = trdX;
    if (fabs(z1)>trdZ)
      return constrainPointToRectangleAlongLine(trdX,trdZ,x0,z0,x1,z1);
  }

  if ( z1 < -trdZ ) {
    if ( z0 < -trdZ )
      return false;
    //Move (x1,z1) to z == -trdZ line:
    x1 += (-trdZ-z1)*(x1-x0)/(z1-z0);
    z1 = -trdZ;
    if (fabs(x1)>trdX)
      return constrainPointToRectangleAlongLine(trdX,trdZ,x0,z0,x1,z1);
  }

  if ( z1 > trdZ ) {
    if ( z0 > trdZ )
      return false;
    //Move (x1,z1) to z == trdZ line:
    x1 += (trdZ-z1)*(x1-x0)/(z1-z0);
    z1 = trdZ;
    if (fabs(x1)>trdX)
      return constrainPointToRectangleAlongLine(trdX,trdZ,x0,z0,x1,z1);
  }

  //We were actually inside all along:
  return true;
}

//____________________________________________________________________
bool MuonChamberProjectionHelper::clipLineSegmentToMDTChamber( const GeoPVConstLink& mdtChamber,
							       Amg::Vector3D & pointA, Amg::Vector3D & pointB, bool& outsidechamber,
							       const double & extradist )
{
  Imp::ChamberInfoMapItr itChamberInfo;
  if (!m_d->getMDTChamberVolInfo( mdtChamber, itChamberInfo ))
    return false;

  double trdX, trdZ;
  m_d->getMDTChamberXAndZ(itChamberInfo, trdX, trdZ );

  trdX += extradist;
  trdZ += extradist;

  if (trdX<=0.0||trdZ<=0.0)//Extradist must have been negative - and so much that effective chamber dimensions are collapsed.
    return false;

  //Get local chamber coordinates, A and B, of pointA and pointB:
  itChamberInfo->second.ensureInitGlobalToLocal();
  Amg::Vector3D A((*(itChamberInfo->second.globalToLocal))*pointA), B((*(itChamberInfo->second.globalToLocal))*pointB);
  double ax(A.x()), az(A.z()), bx(B.x()), bz(B.z());

  //Clip x and z dimensions:
  outsidechamber = !(m_d->clip2DLineSegmentToRectangle( trdX, trdZ, ax, az, bx, bz ));
  if (outsidechamber)
    return true;

  double ay(A.y()), by(B.y());
  //Fixme: We must also clip y dimensions!!

  //Put results back into points and in global coordinates:
  Amg::setVector3DCartesian(pointA, ax,ay,az);
  Amg::setVector3DCartesian(pointB, bx,by,bz);
  Amg::transform(pointA, itChamberInfo->second.localToGlobal);
  Amg::transform(pointB, itChamberInfo->second.localToGlobal);
  outsidechamber = false;
  return true;

}

//____________________________________________________________________
bool MuonChamberProjectionHelper::Imp::clip2DLineSegmentToRectangle( const double& rectX, const double& rectY,
								     double& x0, double& y0, double& x1, double& y1 )
{
  if ( fabs(x0)<=rectX && fabs(y0)<=rectY  ) {
    if ( fabs(x1)>rectX || fabs(y1)>rectY  ) {
      //P0 inside R, P1 outside R. We must change (x1,y1) so P0-P1 is inside R.
      if (!constrainPointToRectangleAlongLine( rectX, rectY, x0, y0, x1, y1 ))
	theclass->message("MuonChamberProjectionHelper Error: Should never happen (1)");
    }
  } else {
    if ( fabs(x1)<=rectX && fabs(y1)<=rectY  ) {
      //Point P1 inside R, P0 outside R. We must change (x0,y0) so P0-P1 is inside R.
      if (!constrainPointToRectangleAlongLine( rectX, rectY, x1, y1, x0, y0 ))
	theclass->message("MuonChamberProjectionHelper Error: Should never happen (2)");
    } else {
      //Both points outside - but the line might still R!

      //First attempt to put (x1,y1) on edge of R, by sliding towards P0
      if (!constrainPointToRectangleAlongLine( rectX, rectY, x0, y0, x1, y1 )) {
	//failed - thus P0-P1 doesn't intersect target rectangle.
	return false;
      }

      //Now change (x0,y0) so P0-P1 is inside the target rectangle.
      if (!constrainPointToRectangleAlongLine( rectX, rectY, x1, y1, x0, y0 ))
	theclass->message("MuonChamberProjectionHelper Error: Should never happen (3)");
    }
  }

  return true;

}
