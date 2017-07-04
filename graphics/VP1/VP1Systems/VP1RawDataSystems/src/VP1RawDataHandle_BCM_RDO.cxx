/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1RawDataHandle_BCM_RDO          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1RawDataSystems/VP1RawDataHandle_BCM_RDO.h"
#include "VP1RawDataSystems/VP1RawDataCommonData.h"
#include "VP1RawDataSystems/VP1RawDataColl_BCM_RDO.h"
#include "VP1RawDataSystems/VP1RawDataSysController.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/SoVisualizeAction.h"

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoBox.h"
#include "InDetBCM_RawData/BCM_RawData.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>

#include <QPair>

#include <map>


//____________________________________________________________________
class VP1RawDataHandle_BCM_RDO::Imp {
public:
  static QStringList describeHit(const BCM_RawData * data, QString prefix = "")
  {
    QStringList l;
    l << "Word1: " + VP1RawDataHandleBase::unsignedToHex(data->getWord1());
    l << "Word2: " + VP1RawDataHandleBase::unsignedToHex(data->getWord2());
    l << "Channel ID [0-15]: " + QString::number(data->getChannel())+" ("+QString(isHighAttenuationChannel(data->getChannel())?"High":"Low")+" attenuation channel)";
    l << "Position of first pulse [0-63]: " + QString::number(data->getPulse1Position());
    l << "Width of first pulse [0-31]: " + QString::number(data->getPulse1Width());
    l << "Position of second pulse [0-63]: " + QString::number(data->getPulse2Position());
    l << "Width of second pulse [0-31]: " + QString::number(data->getPulse2Width());
    l << "Level one accept [0-63]: " + QString::number(data->getLVL1A());
    l << "Bunch crossing ID [0-4096]: " + QString::number(data->getBCID());
    l << "Level 1 ID [0+]: " + QString::number(data->getLVL1ID());
    if (prefix.isEmpty())
      return l;
    QStringList l2;
    foreach (QString s,l)
      l2 << (prefix + s);
    return l2;
  }


  class ModuleInfo {
  public:
    ModuleInfo(int moduleID) : m_moduleID(moduleID), m_group(0),m_transform(0) {}
    ~ModuleInfo() { if (m_group) m_group->unref();if (m_transform) m_transform->unref(); }
    void addBox(const Amg::Transform3D& t,const GeoBox* b)//Call at least once to init (all before the first call to get3DObjects).
                                              //Call either with whole module, or diamond boxes, or whatever...
    {
      m_transformsAndBoxes << QPair< Amg::Transform3D,const GeoBox *>(t,b);
    }
    int moduleID() const { return m_moduleID; }
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
      SoVisualizeAction a;
      m_group = new SoGroup;
      m_group->ref();
      Amg::Transform3D accumTransf;
      QPair< Amg::Transform3D,const GeoBox *> p;
      foreach(p, m_transformsAndBoxes) {
	if (!m_transform) {
	  m_transform = VP1LinAlgUtils::toSoTransform(p.first);
	  m_transform->ref();
	  accumTransf = p.first;
	} else {
 	  m_group->addChild(VP1LinAlgUtils::toSoTransform(accumTransf.inverse()*p.first));
	  accumTransf = (accumTransf*p.first);
	}
	a.handleBox(p.second);
	if (a.getShape())
	  m_group->addChild(a.getShape());
      }
      m_transformsAndBoxes.clear();
      if (!m_transform) {
	m_transform = new SoTransform;
	m_transform->ref();
      }
    }
    int m_moduleID;
    SoGroup * m_group;
    SoTransform * m_transform;
    QList<QPair< Amg::Transform3D,const GeoBox *> > m_transformsAndBoxes;
  };
  static std::map<int,ModuleInfo*> * moduleID2ModuleInfo;
  static void ensureInitModuleInfo();
};

std::map<int,VP1RawDataHandle_BCM_RDO::Imp::ModuleInfo*> * VP1RawDataHandle_BCM_RDO::Imp::moduleID2ModuleInfo = 0;

//____________________________________________________________________
void VP1RawDataHandle_BCM_RDO::Imp::ensureInitModuleInfo()
{
  if (moduleID2ModuleInfo)
    return;

  Imp::moduleID2ModuleInfo = new std::map<int,ModuleInfo*>;//Fixme: Gives valgrind problems!
  const GeoPVConstLink * world = VP1JobConfigInfo::geoModelWorld();
  if (world) {
    GeoVolumeCursor av(*world);
    while (!av.atEnd()) {
      if (av.getName()=="Pixel") {
	Amg::Transform3D tr_pix = Amg::CLHEPTransformToEigen( av.getTransform() );
	GeoVolumeCursor pv(av.getVolume());
	while (!pv.atEnd()) {
	  int bcmModLogCopyNumber(-1);
	  if (pv.getVolume()->getLogVol()->getName()=="bcmModLog") {
	    Amg::Transform3D tr_bcmmod = Amg::CLHEPTransformToEigen( pv.getTransform() );
	    Query<int> Qint = pv.getId();
	    if (Qint.isValid()) {
	      bcmModLogCopyNumber = int(Qint);
	      ModuleInfo * modInfo = new ModuleInfo(bcmModLogCopyNumber - 951);
	      GeoVolumeCursor bv(pv.getVolume());
	      while (!bv.atEnd()) {
		if (bv.getVolume()->getLogVol()->getName()=="bcmDiamondLog") {
		  Amg::Transform3D tr_diamond( Amg::CLHEPTransformToEigen( bv.getTransform() ) );
		  const GeoBox * box = bv.getVolume()->getLogVol()->getShape()->typeID()==GeoBox::getClassTypeID() ?
		    static_cast<const GeoBox*>(bv.getVolume()->getLogVol()->getShape()) : 0;
		  modInfo->addBox(tr_pix*tr_bcmmod*tr_diamond,box);
		}
		bv.next();
	      }//end bcm mod loop
	      (*moduleID2ModuleInfo)[modInfo->moduleID()] = modInfo;
	    }
	  }
	  pv.next();
	}//end pixel loop
      }
      av.next();
    }//end world loop
  }
}


//____________________________________________________________________
VP1RawDataHandle_BCM_RDO::VP1RawDataHandle_BCM_RDO(VP1RawDataCollBase* coll,int moduleID, QList<const BCM_RawData*>data)
  : VP1RawDataHandleBase(coll), m_moduleID(moduleID), m_nHighAttenuationHits(0), m_data(data)
{
  Imp::ensureInitModuleInfo();
  foreach(const BCM_RawData* hit,data) {
    if (isHighAttenuationChannel(hit->getChannel()))
      ++m_nHighAttenuationHits;
  }
}

//____________________________________________________________________
VP1RawDataHandle_BCM_RDO::~VP1RawDataHandle_BCM_RDO()
{
}

//____________________________________________________________________
QString VP1RawDataHandle_BCM_RDO::moduleName() const
{
  switch(moduleID()) {
  case 0: return "F410";
  case 1: return "F405";
  case 2: return "F413";
  case 3: return "F404";
  case 4: return "F424";
  case 5: return "F420";
  case 6: return "F422";
  case 7: return "F408";
  default: return "Unknown";
  }
}

//____________________________________________________________________
QString VP1RawDataHandle_BCM_RDO::moduleNick() const
{
  switch(moduleID()) {
  case 0: return "Marko";
  case 1: return "Helmut/Peter";
  case 2: return "William";
  case 3: return "Harris";
  case 4: return "Ewa";
  case 5: return "Heinz";
  case 6: return "Irena";
  case 7: return "Andrej";
  default: return "Unknown";
  }
}

//____________________________________________________________________
QString VP1RawDataHandle_BCM_RDO::modulePosDescription() const
{
  switch(moduleID()) {
  case 0: return "A, +x";
  case 1: return "A, +y";
  case 2: return "A, -x";
  case 3: return "A, -y";
  case 4: return "C, +x";
  case 5: return "C, +y";
  case 6: return "C, -x";
  case 7: return "C, -y";
  default: return "Unknown";
  }
}

//____________________________________________________________________
QStringList VP1RawDataHandle_BCM_RDO::clicked(bool verbose) const
{
  QStringList l;
  l << " ===> BCM Hit(s)";
  l << " --  Module ID: "+QString::number(moduleID());
  l << " --  Module Name: "+moduleName()+" (\""+moduleNick()+"\")";
  l << " --  Module Position: "+modulePosDescription();
  l << " --  Number of hits: "+QString::number(nHits());
  l << " --  Number of hits with High Attenuation: "+QString::number(numberOfHighAttenuationHits());
  if (verbose) {
    for (int i=0;i<m_data.count();++i) {
      l << " ------> BCM_RawData [hit "+QString::number(i+1)+"/"+QString::number(m_data.count())+"]";
      l += Imp::describeHit(m_data.at(i), " -- ");
    }
  } else {
    l << " --  (turn on verbose output for more information)";
  }


  return l;
}

//____________________________________________________________________
SoNode * VP1RawDataHandle_BCM_RDO::buildShape()
{
  std::map<int,Imp::ModuleInfo*>::iterator it = Imp::moduleID2ModuleInfo->find(m_moduleID);
  SoNode * n = (it==Imp::moduleID2ModuleInfo->end() ? 0 : it->second->get3DObjects());
  if (!n)
    return common()->nodeManager()->getShapeNode_Point();//fixme: warn

  if (numberOfHighAttenuationHits()>0 && static_cast<VP1RawDataColl_BCM_RDO*>(coll())->useSpecialBCMHighAttMaterial()) {
    SoGroup * gr = new SoGroup;
    gr->addChild(coll()->common()->controller()->bcmHighAttMaterial());
    gr->addChild(n);
    return gr;
  } else {
    return n;
  }
}

//____________________________________________________________________
SoTransform * VP1RawDataHandle_BCM_RDO::buildTransform()
{
  std::map<int,Imp::ModuleInfo*>::iterator it = Imp::moduleID2ModuleInfo->find(m_moduleID);
  SoTransform * t = (it==Imp::moduleID2ModuleInfo->end() ? 0 : it->second->getTransform());
  if (!t)
    return common()->nodeManager()->getUnitTransform();//fixme: warn
  return t;
}
