/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VertexHandle                   //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VertexHandle.h"
#include "VertexCollHandle.h"
#include "VertexCollectionSettingsButton.h"
#include "VP1AODSystems/AODSystemController.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1QtUtils.h"

#include "VP1Utils/SurfaceToSoNode.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbMatrix.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>

#include "GeoPrimitives/AmgStringHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h" 

#include <cassert>

//____________________________________________________________________
class VertexHandle::Imp {
public:
  static double dist(const SbVec3f& p1,const SbVec3f& p2);

  static int nvtxhandles;

  Imp(VertexHandle*tc, const xAOD::Vertex* vtx) : theclass(tc), vertex(vtx), collHandle(nullptr),sep(nullptr), sphere(nullptr){}
  ~Imp() { }
  VertexHandle * theclass;
  const xAOD::Vertex* vertex;
  VertexCollHandle* collHandle;

  SoSeparator * sep; // everything hangs from this.
  SoSphere * sphere;//This represents the cone representing the jet.
};


//____________________________________________________________________
int VertexHandle::Imp::nvtxhandles = 0;

//____________________________________________________________________
VertexHandle::VertexHandle(VertexCollHandle*ch, const xAOD::Vertex *vertex)
  : AODHandleBase(ch), m_d(new Imp(this,vertex))
{
  m_d->collHandle=ch;
  ++Imp::nvtxhandles;
}

//____________________________________________________________________
VertexHandle::~VertexHandle()
{
  delete m_d;
  --Imp::nvtxhandles;
}

//____________________________________________________________________
int VertexHandle::numberOfInstances()
{
  return Imp::nvtxhandles;
}


//____________________________________________________________________
bool VertexHandle::has3DObjects() 
{
  return 0!=m_d->sep;
}

//____________________________________________________________________
void VertexHandle::clear3DObjects(){
  //	VP1Msg::messageVerbose("VertexHandle::clear3DObjects()");

  if (m_d->sphere) {
    m_d->sphere->unref();
    m_d->sphere = 0;
  }
  if (m_d->sep) {
    m_d->sep->unref();
    m_d->sep = 0;
  }

}

//____________________________________________________________________
SoNode* VertexHandle::nodes(){

  VP1Msg::messageVerbose("VertexHandle::nodes()");

  if (m_d->sep) {
    VP1Msg::messageVerbose("d->sep already defined (" + VP1Msg::str(m_d->sep) + "). Returning d->sep.");
    return m_d->sep; // FIXME - do we need to check if anything need to be redrawn?
  }
  if (!m_d->sep) {
    VP1Msg::messageVerbose("d->sep not defined. Creating shapes and a new d->sep.");
    m_d->sep = new SoSeparator();
    m_d->sep->ref();
  }

  // SbVec3f origin(0.,0.,0.);
  /* TODO: ask if origin info is present in xAOD, like in the old Jet class
  if ( m_d->m_jet->origin() ) {
  origin.setValue(m_d->m_jet->origin()->position().x(),
  m_d->m_jet->origin()->position().y(),
  m_d->m_jet->origin()->position().z());
  }
  */

  VP1Msg::messageVerbose("creating the shapes");

  SoTranslation * translation = new SoTranslation;
  translation->translation.setValue ( m_d->vertex->x(), m_d->vertex->y(), m_d->vertex->z() );
  m_d->sep->addChild ( translation );
  m_d->sphere = new SoSphere;
  m_d->sphere->radius = m_d->collHandle->collSettingsButton().vertexSize();
  m_d->sep->addChild ( m_d->sphere );

  return m_d->sep;
}

//____________________________________________________________________
SoMaterial * VertexHandle::determineMaterial() {
  // By default we use the collection material.
  // std::cout<<"VertexHandle::determineMaterial() - collHandle()->material()"<<collHandle()->material()<<std::endl;
  return collHandle()->material();
}

//____________________________________________________________________
QStringList VertexHandle::baseInfo() const
{
  QStringList l;
  l << shortInfo() ;
  // vertex position
  l << "Vx: " << VP1Msg::str(m_d->vertex->x()) << "Vy: " << VP1Msg::str(m_d->vertex->y()) << "Vz: " << VP1Msg::str(m_d->vertex->z());
  return l;
}

//____________________________________________________________________
QString VertexHandle::vertexType() const {
  using namespace xAOD;

  switch (m_d->vertex->vertexType()){
    case xAOD::VxType::NoVtx:
      return QString("Type: Dummy");
    case xAOD::VxType::PriVtx:
      return QString("Type: Primary");
    case xAOD::VxType::SecVtx:
      return QString("Type: Secondary");
    case xAOD::VxType::PileUp:
      return QString("Type: Pileup");
    case xAOD::VxType::ConvVtx:
      return QString("Type: Conversion");
    case xAOD::VxType::KinkVtx:
      return QString("Type: Kink");
    case xAOD::VxType::NotSpecified:
      return QString("Type: Not specified (default)");
    default:
      return QString("Unknown vertex type - probably VP1 needs updating!");
    }
}

//____________________________________________________________________
double VertexHandle::getPositionX() const {
    return m_d->vertex->x();
}
//____________________________________________________________________
double VertexHandle::getPositionY() const {
    return m_d->vertex->y();
}
//____________________________________________________________________
double VertexHandle::getPositionZ() const {
    return m_d->vertex->z();
}


//____________________________________________________________________
QString VertexHandle::shortInfo() const {
  QString l = vertexType();  
  return l;
}

//____________________________________________________________________
QStringList VertexHandle::clicked() const
{
  QStringList l;
  l << "--Vertex:";
  l << VertexHandle::baseInfo();
  #ifndef BUILDVP1LIGHT
    try {
      l<< "Has "<<VP1Msg::str(m_d->vertex->vxTrackAtVertex ().size()) <<" associated tracks.";
    } catch ( SG::ExcBadAuxVar& ) {
      l<<"Vertex is missing links to tracks!";
    }
  #endif
  return l;
}




