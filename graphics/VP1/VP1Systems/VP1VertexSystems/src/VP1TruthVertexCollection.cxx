/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1TruthVertexCollection          //
//                                                            //
//  Author: Andreas.Wildauer@cern.ch                          //
//          Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//          Ben Zastovnik                                     //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1VertexSystems/VP1TruthVertexCollection.h"
#include "VP1VertexSystems/VertexSysController.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1MaterialButton.h"

#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoSeparator.h>

#include <QStringList>

#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"

//#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>

//____________________________________________________________________
QList<VP1StdCollection*> VP1TruthVertexCollection::createCollections(VertexSysController*controller)
{
  QList<VP1StdCollection*> l;
  foreach (QString key, VP1SGContentsHelper(controller->systemBase()).getKeys<McEventCollection>()) {
    VP1TruthVertexCollection * col = new VP1TruthVertexCollection(controller,key);
    col->init();
    l << col;
  }
  return l;
}


//____________________________________________________________________
class VP1TruthVertexCollection::Imp {
public:
  double mag(const HepMC::FourVector& v) const {
    return std::sqrt( v.x()*v.x() + v.y()*v.y() + v.z()*v.z() );
  }
  QString key;
  VP1TruthVertexCollection * theclass;
  VertexSysController*controller;
  std::map <SoNode *, const HepMC::GenVertex *> nodeToVertexMap;
  SoLineSet * createCross(const double& x, const double& y, const double& z, const double& extent = 10*Gaudi::Units::mm ); // 10*CLHEP::mm );

  class VertexHandle {
  public:
    VertexHandle(const HepMC::GenVertex* v,VP1TruthVertexCollection::Imp * dd) : m_attached(false), m_vertex(v), m_line(0),m_d(dd) {}
    ~VertexHandle() { if (m_line) m_line->unref(); }

    void recheckCutStatus() {
      bool cutval(cut());
      if (cutval!=m_attached) {
	if (cutval)
	  ensureAttach(m_d->theclass->collSep());
	else
	  ensureDetach(m_d->theclass->collSep());
      }
    }
    void updateShape() {
      if (!m_line)
	return;
      if (!m_attached) {
	//invalidate...
      }

      SoVertexProperty *vertices = static_cast<SoVertexProperty *>(m_line->vertexProperty.getValue());
      if (!vertices) {
	vertices = new SoVertexProperty();
	m_line->numVertices.set1Value(0,2);
	m_line->numVertices.set1Value(1,2);
	m_line->numVertices.set1Value(2,2);
	m_line->vertexProperty = vertices;
      }

      QPair<VertexCommonFlags::QUANTITY,double> p = m_d->controller->truthCrossLength();
      double extent;
      if (p.second<0)
	extent = -p.second;
      else
	extent = p.second * quantityVal(p.first);

      double x(m_vertex->position().x()), y(m_vertex->position().y()), z(m_vertex->position().z());
      vertices->vertex.set1Value ( 0,x-extent, y, z );
      vertices->vertex.set1Value ( 1,x+extent, y,z );
      vertices->vertex.set1Value ( 2, x,y-extent, z);
      vertices->vertex.set1Value ( 3, x,y+extent, z );
      vertices->vertex.set1Value ( 4, x, y,z-extent );
      vertices->vertex.set1Value ( 5, x, y,z+extent );
    }
    const HepMC::GenVertex * vertex() const { return m_vertex; }
    SoLineSet * line() const { return m_line; }

    double quantityVal(const VertexCommonFlags::QUANTITY& q) {
      //Find total 4-momentum of incoming particles
      double px(0), py(0), pz(0), e(0);//HepMC::FourVector does NOT support operators + or += !!!!!!
      HepMC::GenVertex::particles_in_const_iterator itPartIn,itPartInE(m_vertex->particles_in_const_end());
      for ( itPartIn = m_vertex->particles_in_const_begin();itPartIn!=itPartInE;++itPartIn) {
	px += (*itPartIn)->momentum().px();
	py += (*itPartIn)->momentum().py();
	pz += (*itPartIn)->momentum().pz();
	e += (*itPartIn)->momentum().e();
      }
      const HepMC::FourVector mom(px,py,pz,e);
      switch(q) {
      case VertexCommonFlags::ENERGY: return e;
      case VertexCommonFlags::MOMENTUM: return m_d->mag(mom);
      case VertexCommonFlags::TRANSVERSE_MOM: return mom.perp();
      default: // VertexCommonFlags::MASS:
	return mom.m();
      }
    }
  private:
    void ensureAttach(SoSeparator * collsep) {
      if (m_attached)
	return;
      m_attached = true;
      if (!m_line) {
	m_line = new SoLineSet;
	m_line->ref();
      }
      updateShape();
      collsep->addChild(m_line);
    }
    void ensureDetach(SoSeparator * collsep) {
      if (!m_attached)
	return;
      m_attached = false;
      collsep->removeChild(m_line);
    }
    bool cut() {

      if (m_d->controller->truthCutPrimaryVertexOnly()&&this!=m_d->vertices.at(0))
	return false;

      if (!m_d->controller->truthCutAllowedEta().contains(m_vertex->position().eta()))
	return false;

      QPair<VertexCommonFlags::QUANTITY,VP1Interval> p = m_d->controller->truthCutQuantity();
      if (!p.second.isAllR()) {
	if (!p.second.contains(quantityVal(p.first)))
	  return false;
      }

      //We handle phi cut last:
      double phi(m_vertex->position().phi());
      foreach(VP1Interval i,m_d->controller->truthCutAllowedPhi()) {
	if (i.contains(phi)||i.contains(phi+2*M_PI)||i.contains(phi-2*M_PI))
	  return true;
      }
      return false;
    }
    bool m_attached;
    const HepMC::GenVertex * m_vertex;
    SoLineSet * m_line;
    VP1TruthVertexCollection::Imp * m_d;
  };

  QList<VertexHandle*> vertices;
};


//____________________________________________________________________
VP1TruthVertexCollection::VP1TruthVertexCollection(VertexSysController*controller,const QString& key)
  : VP1StdCollection(controller->systemBase(),"VP1TruthVertexCollection_"+key), m_d(new Imp)
{
  m_d->key = key;
  m_d->theclass = this;
  m_d->controller = controller;
  connect(controller,SIGNAL(truthCutAllowedEtaChanged(const VP1Interval&)),this,SLOT(recheckAllCuts()));
  connect(controller,SIGNAL(truthCutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(recheckAllCuts()));
  connect(controller,SIGNAL(truthCutPrimaryVertexOnlyChanged(bool)),this,SLOT(recheckAllCuts()));
  connect(controller,SIGNAL(truthCutQuantityChanged(const QPair<VertexCommonFlags::QUANTITY,VP1Interval>&)),this,SLOT(recheckAllCuts()));
  connect(controller,SIGNAL(truthCrossLengthChanged(QPair<VertexCommonFlags::QUANTITY,double>)),this,SLOT(updateAllShapes()));
}

//____________________________________________________________________
VP1TruthVertexCollection::~VP1TruthVertexCollection()
{
  foreach(Imp::VertexHandle*vh,m_d->vertices)
    delete vh;
  delete m_d;
}

//____________________________________________________________________
QString VP1TruthVertexCollection::provideText() const
{
  return m_d->key;
}

//____________________________________________________________________
void VP1TruthVertexCollection::assignDefaultMaterial(SoMaterial*m) const
{
  VP1MaterialButton::setMaterialParameters( m, 0.66667/*red*/, 0.66667/*green*/, 1.0/*blue*/, 0.15 /*brightness*/ );
}

//_____________________________________________________________________________________
SoLineSet * VP1TruthVertexCollection::Imp::createCross(const double& x, const double& y, const double& z, const double& extent )
{
  SoVertexProperty *vertices = new SoVertexProperty();
  vertices->vertex.set1Value ( 0,x-extent, y, z );
  vertices->vertex.set1Value ( 1,x+extent, y,z );
  vertices->vertex.set1Value ( 2, x,y-extent, z);
  vertices->vertex.set1Value ( 3, x,y+extent, z );
  vertices->vertex.set1Value ( 4, x, y,z-extent );
  vertices->vertex.set1Value ( 5, x, y,z+extent );
  SoLineSet * line = new SoLineSet();
  line->numVertices.set1Value(0,2);
  line->numVertices.set1Value(1,2);
  line->numVertices.set1Value(2,2);
  line->vertexProperty = vertices;
  return line;
}

//____________________________________________________________________
bool VP1TruthVertexCollection::load()
{
  const McEventCollection* mcEventColl;
  if (!VP1SGAccessHelper(systemBase()).retrieve(mcEventColl, m_d->key))
    return false;

  //Fixme: Here we take the first event in the event collection. Instead we should loop!
  if (mcEventColl->size()<1)//Fixme
    return false;

  McEventCollection::const_iterator itEvent, itEventEnd(mcEventColl->end());
  for (itEvent = mcEventColl->begin(); itEvent != itEventEnd; itEvent++ ) {
    const HepMC::GenEvent* genEvent(*itEvent);
    if (!genEvent)
      continue;

    HepMC::GenEvent::vertex_const_iterator itVertex, itVertexEnd(genEvent->vertices_end());
    for (itVertex = genEvent->vertices_begin(); itVertex != itVertexEnd; itVertex++ ) {
      const HepMC::GenVertex * vtx(*itVertex);
      if (!vtx)
	continue;
      m_d->vertices << new Imp::VertexHandle(vtx,m_d);
    }
  }

  recheckAllCuts();

  return true;
}

//____________________________________________________________________
QStringList VP1TruthVertexCollection::infoOnClicked(SoPath* pickedPath)
{
  //Get HepMC::GenVertex pointer associated with the clicked node:
  SoNode * pickedNode = (pickedPath ? (pickedPath->getLength()>0?pickedPath->getNodeFromTail(0):0): 0);

  Imp::VertexHandle* vertexHandle(0);
  foreach(Imp::VertexHandle* vh,m_d->vertices) {
    if (vh->line()==pickedNode) {
      vertexHandle = vh;
      break;
    }
  }
  if (!vertexHandle)
    return QStringList() << "ERROR: Could not get truth vertex information for picked Node";
  const HepMC::GenVertex * vtx = vertexHandle->vertex();

  QStringList l;
  if (m_d->controller->printInfoOnClick()) {

    //Make output:
    l <<"Truth vertex from collection "+text()+":" ;

    HepMC::GenVertex::particles_in_const_iterator itPartIn,itPartInE(vtx->particles_in_const_end());
    for ( itPartIn = vtx->particles_in_const_begin();itPartIn!=itPartInE;++itPartIn) {
      const int pdg = (*itPartIn)->pdg_id();
      bool ok;
      QString name = VP1ParticleData::particleName(pdg,ok);
      if (!ok)
	name = "<unknown>";
      l << "--> In: "+name+" ("+str(pdg)+")  [ P = "+str(m_d->mag((*itPartIn)->momentum())/Gaudi::Units::GeV)+" GeV ]";
    }
    HepMC::GenVertex::particles_out_const_iterator itPartOut,itPartOutE(vtx->particles_out_const_end());
    for ( itPartOut = vtx->particles_out_const_begin();itPartOut!=itPartOutE;++itPartOut) {
      const int pdg = (*itPartOut)->pdg_id();
      bool ok;
      QString name = VP1ParticleData::particleName(pdg,ok);
      if (!ok)
	name = "<unknown>";
      l << "--> Out: "+name+" ("+str(pdg)+")  [ P = "+str(m_d->mag((*itPartOut)->momentum())/Gaudi::Units::GeV)+" GeV ]";
    }

    if (m_d->controller->printVerboseInfoOnClick()) {
      l <<"======== Dump ========";
      std::ostringstream s;
      vtx->print(s);
      l << QString(s.str().c_str()).split('\n');
      l <<"======================";
    }

  }
  if (m_d->controller->zoomOnClick()) {
    std::set<SoCamera*> cameras = static_cast<IVP13DSystem*>(systemBase())->getCameraList();
    std::set<SoCamera*>::iterator it,itE = cameras.end();
    for (it=cameras.begin();it!=itE;++it)
      VP1CameraHelper::animatedZoomToPath(*it,collSep(),pickedPath,2.0,1.0);
  }

  return l;
}

//____________________________________________________________________
void VP1TruthVertexCollection::recheckAllCuts()
{
  static_cast<IVP13DSystem*>(systemBase())->deselectAll();
  largeChangesBegin();
  foreach(Imp::VertexHandle* vh,m_d->vertices)
    vh->recheckCutStatus();
  largeChangesEnd();
}

//____________________________________________________________________
void VP1TruthVertexCollection::updateAllShapes()
{
  largeChangesBegin();
  foreach(Imp::VertexHandle* vh,m_d->vertices)
    vh->updateShape();
  largeChangesEnd();
}
