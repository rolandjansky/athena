/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandle_LUCID            //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataHandle_LUCID.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataColl_LUCID.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/SoVisualizeAction.h"
#include "VP1Base/VP1Msg.h"

#include "VP1HEPVis/nodes/SoTubs.h"

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoTube.h"
#include "InDetBCM_RawData/BCM_RawData.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include <QPair>
#include <map>


//____________________________________________________________________
class VP1RawDataHandle_LUCID::Imp {
public:
   class TubeInfo {
   public:
     TubeInfo(int tubeID,const Amg::Transform3D& heptransform,const GeoTube* geotube)
       : m_tubeID(tubeID), m_group(0),m_transform(0), m_heptransform(heptransform), m_geotube(geotube) {}
     ~TubeInfo() { if (m_group) m_group->unref();if (m_transform) m_transform->unref(); }
     int tubeID() const { return m_tubeID; }
     SoTransform * getTransform() {
       if (!m_transform)
	 init();
       return m_transform;
     }
     SoGroup * get3DObjects() {
       if (!m_group)
	 init();
       return m_group;
     }
   private:
     void init() {
       m_group = new SoGroup;
       m_group->ref();

       SoVisualizeAction a;
       a.handleTube(m_geotube);

       SoShape * shape(a.getShape() ? a.getShape() : new SoCylinder);
       m_group->addChild(shape);

       SoTubs::initClass();
       if (shape->getTypeId().isDerivedFrom(SoTubs::getClassTypeId())) {
	 SoTubs * tubs = static_cast<SoTubs*>(shape);
	 const double halfz(tubs->pDz.getValue());
	 tubs->pDz.setValue(halfz + 0.2*CLHEP::mm);
	 //Fixme:: Add line (0.1mm shorter than the geotube) under m_group
	 SoLineSet * line = new SoLineSet;
	 SoVertexProperty * vertices = new SoVertexProperty;
	 line->vertexProperty = vertices;
	 vertices->vertex.set1Value(0,0,0,+(halfz-0.1*CLHEP::mm));
	 vertices->vertex.set1Value(1,0,0,-(halfz-0.1*CLHEP::mm));
	 line->numVertices.set1Value(0,2);
	 m_group->addChild(line);
       }
       m_transform = VP1LinAlgUtils::toSoTransform(m_heptransform);
       m_transform->ref();
     }
     int m_tubeID;
     SoGroup * m_group;
     SoTransform * m_transform;
     const Amg::Transform3D m_heptransform;
     const GeoTube* m_geotube;
  };
  static std::map<int,TubeInfo*> * tubeID2TubeInfo;
  static void ensureInitTubeInfo();
};

std::map<int,VP1RawDataHandle_LUCID::Imp::TubeInfo*> * VP1RawDataHandle_LUCID::Imp::tubeID2TubeInfo = 0;

//____________________________________________________________________
void VP1RawDataHandle_LUCID::Imp::ensureInitTubeInfo()
{
  if (tubeID2TubeInfo)
    return;

  Imp::tubeID2TubeInfo = new std::map<int,TubeInfo*>;//Fixme: Gives valgrind problems!
  const GeoPVConstLink * world = VP1JobConfigInfo::geoModelWorld();
  if (world) {
    GeoVolumeCursor av(*world);
    while (!av.atEnd()) {
      QString topvolname(av.getName().c_str());
      if (topvolname.startsWith("LucidSide")) {
	const bool sideA(topvolname.contains("SideA"));
	GeoVolumeCursor av2(av.getVolume());
	while (!av2.atEnd()) {
	  if (av2.getVolume()->getLogVol()->getName()=="lvVessel") {
	    GeoVolumeCursor av3(av2.getVolume());
	    while (!av3.atEnd()) {
	      if (av3.getVolume()->getLogVol()->getName()=="lvVesselGas") {
		GeoVolumeCursor av4(av3.getVolume());
		while (!av4.atEnd()) {
		  if (av4.getVolume()->getLogVol()->getName()=="lvTubeGas") {
		    //Decode tube-id:
		    int tubeID = -1;
		    QString tubegasname(av4.getName().c_str());
		    if (tubegasname.startsWith("LucidTubeGas")) {
		      tubegasname.remove(0,12);
		      bool ok;
		      int t(tubegasname.toInt(&ok));
		      if (ok) {
			tubeID = t + (sideA?0:20);
		      }
		    }
		    if (tubeID>=0) {
		      const GeoTube * geotube = av4.getVolume()->getLogVol()->getShape()->typeID()==GeoTube::getClassTypeID() ?
			static_cast<const GeoTube*>(av4.getVolume()->getLogVol()->getShape()) : 0;
		      Amg::Transform3D tr( Amg::CLHEPTransformToEigen(av.getTransform()*av2.getTransform()*av3.getTransform()*av4.getTransform() ) );
		      (*tubeID2TubeInfo)[tubeID] = new TubeInfo(tubeID,tr,geotube);
		      if (VP1Msg::verbose())
			VP1Msg::messageVerbose("VP1RawDataHandle_LUCID Added TubeInfo for tubeID = "+QString::number(tubeID));
		    } else {
		      VP1Msg::message("VP1RawDataHandle_LUCID ERROR: Can't decode tube id from PMT volume name");
		    }
		  }
		  av4.next();
		}//end lvVesselGas loop
	      }
	      av3.next();
	    }//end lvVessel loop
	  }
	  av2.next();
	}//end lucid-topvol loop
      }
      av.next();
    }//end world loop
  }
}


//____________________________________________________________________
VP1RawDataHandle_LUCID::VP1RawDataHandle_LUCID(VP1RawDataCollBase* coll)
  : VP1RawDataHandleBase(coll)
{
  Imp::ensureInitTubeInfo();
}

//____________________________________________________________________
VP1RawDataHandle_LUCID::~VP1RawDataHandle_LUCID()
{
}

//____________________________________________________________________
SoNode * VP1RawDataHandle_LUCID::buildShape()
{
  std::map<int,Imp::TubeInfo*>::iterator it = Imp::tubeID2TubeInfo->find(getTubeID());
  SoNode * n = (it==Imp::tubeID2TubeInfo->end() ? 0 : it->second->get3DObjects());
  if (!n)
    return common()->nodeManager()->getShapeNode_Point();//fixme: warn
  return n;
}

//____________________________________________________________________
SoTransform * VP1RawDataHandle_LUCID::buildTransform()
{
  std::map<int,Imp::TubeInfo*>::iterator it = Imp::tubeID2TubeInfo->find(getTubeID());
  SoTransform * t = (it==Imp::tubeID2TubeInfo->end() ? 0 : it->second->getTransform());
  if (!t)
    return common()->nodeManager()->getUnitTransform();//fixme: warn
  return t;
}

//____________________________________________________________________
QStringList VP1RawDataHandle_LUCID::commonClicked(const QString& prefix) const
{
  QStringList l;
  l << prefix+"TubeID: "+QString::number(getTubeID());
  l << prefix+"QDC: "+QString::number(getQDC());
  l << prefix+"TDC: "+QString::number(getTDC());
  l << prefix+"SideA: "+(isSideA()?"Yes":"No");
  l << prefix+"SideC: "+(isSideC()?"Yes":"No");
  return l;
}
