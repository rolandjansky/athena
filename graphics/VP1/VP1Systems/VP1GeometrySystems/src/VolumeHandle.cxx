/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GeometrySystems/VolumeHandle.h"
#include "VP1GeometrySystems/VolumeHandleSharedData.h"
#include "VP1GeometrySystems/PhiSectorManager.h"
#include "VP1GeometrySystems/VisAttributes.h"
#include "VP1GeometrySystems/VP1GeoTreeView.h"
#include "VP1GeometrySystems/GeoSysController.h"

#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Msg.h"
// #include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1HEPVis/nodes/SoTransparency.h"
#include "VP1HEPVis/nodes/SoPolyhedron.h"
#include "VP1HEPVis/nodes/SoPcons.h"
#include "VP1HEPVis/VP1HEPVisUtils.h"

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>

#include <QHash>
#include <QBuffer>
#include <QByteArray>
#include <QPair>
#include <QSet>
#include <QMap>
#include <QDataStream>

// Suppress warning from Coin4 headers.
#if __GNUC__ >= 10
# pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif

//____________________________________________________________________
class VolumeHandle::Imp {
public:
  Imp(VolumeHandleSharedData * the_cd, const GeoPVConstLink& the_pV, const SbMatrix& the_ac)
    : commondata(the_cd), pV(the_pV),accumTrans(the_ac),attachsepHelper(0),attachlabelSepHelper(0),nodesep(0), material(0), label_sep(0), labels(0), isattached(false) {}

  VolumeHandleSharedData * commondata;
  GeoPVConstLink pV;
  const SbMatrix accumTrans;//FIXME: Use pointer - and free once children are created AND nodesep has been build. Or just construct on the fly!

  VP1ExtraSepLayerHelper * attachsepHelper;
  VP1ExtraSepLayerHelper * attachlabelSepHelper;

  SoSeparator * nodesep;
  SoMaterial * material;
  SoSeparator* label_sep;
  int labels;
  QList<int> labelsPosOffsets;

  bool isattached;
  void attach(VolumeHandle*vh);
  void detach();

  class VolState {
  public:
    int nZappedChildren() const { return m_zappedChildren.count(); }
    int nExpandedChildren() const { return m_expandedChildren.count(); }
    QMap<quint32,VolState> m_expandedChildren;
    QSet<quint32> m_zappedChildren;
  };

  static VolState getChildStates(const VolumeHandle*theclass );
  static void applyChildStates(const VolState&,VolumeHandle*theclass);
  static bool hasNonStandardShapeChildren(const SoGroup*g);

};


QDataStream & operator<< ( QDataStream & out, const VolumeHandle::Imp::VolState & vs ) {
  out << vs.m_expandedChildren;
  out << vs.m_zappedChildren;
  return out;
}
QDataStream & operator>> ( QDataStream & in, VolumeHandle::Imp::VolState & vs ) {
  in >> vs.m_expandedChildren;
  in >> vs.m_zappedChildren;
  return in;
}

//____________________________________________________________________
VolumeHandle::VolumeHandle(VolumeHandleSharedData * cd,VolumeHandle * parent, const GeoPVConstLink& pV, int childNumber, const MuonChamberState& mcs, const SbMatrix& accumTrans)
  : m_d(new Imp(cd,pV,accumTrans)), m_childNumber(childNumber), m_nchildren(childNumber>=0?pV->getNChildVols():0), m_muonChamberState(mcs), m_parent(parent),
    m_state(VP1GeoFlags::CONTRACTED)
{
  // std::cout<<"VolumeHandle ctor for "<<this<<" with parent="<<parent<<" and GeoPVConstLink @"<<&pV<<std::endl;
  //commondata might be 0 in the special case where VolumeHandle is
  //used as a base class for other reasons inside
  //VolumeTreeModel. Just make sure we dont crash in that case.
  if (cd) {
    cd->ref();
    if (!haveParentsNotExpanded())
      m_d->attach(this);
  }
}

//____________________________________________________________________
VolumeHandle::~VolumeHandle()
{
  if (m_d->commondata) {
    setState(VP1GeoFlags::ZAPPED);
    m_d->commondata->removeZappedVolumesFromGui(this);
    if (childrenAreInitialised()) {
      VolumeHandleListItr it, itE = m_children.end();
      for (it = m_children.begin(); it!=itE; ++it)
        delete *it;
      m_children.clear();
    }
    if (m_d->material)
      m_d->material->unref();
    if (m_d->nodesep)
      m_d->nodesep->unref();
    if (m_d->label_sep)
      m_d->label_sep->unref();
    m_d->commondata->unref();
  }
  delete m_d;
}

//____________________________________________________________________
void VolumeHandle::initialiseChildren()
{
  if (childrenAreInitialised())
    return;

  assert(m_nchildren);

  //Loop over children:
  m_children.reserve(m_nchildren);
  unsigned ichild(0);
  GeoVolumeCursor av(m_d->pV);
  while (!av.atEnd()) {
    
    //Add transformation between parent and child to find the complete transformation of the child:
    SbMatrix matr;
    VP1LinAlgUtils::transformToMatrix(Amg::EigenTransformToCLHEP(av.getTransform()), matr);
    matr.multRight(m_d->accumTrans);
    m_children.push_back(new VolumeHandle(m_d->commondata,this,av.getVolume(),ichild++,(isInMuonChamber()?MUONCHAMBERCHILD:NONMUONCHAMBER),matr));
    //std::cout << "initialised: " << av.getName() << " - " << m_children.back()->getName().toStdString() << " - " << m_children.back() << std::endl;
    av.next();
  }

  assert(ichild==m_nchildren&&m_children.size()==m_nchildren);

}

//____________________________________________________________________
GeoPVConstLink VolumeHandle::geoPVConstLink() const
{
  return m_d->pV;
}

//____________________________________________________________________
QString VolumeHandle::getName() const
{
  return m_d->pV->getLogVol()->getName().c_str();
}

//____________________________________________________________________
std::string VolumeHandle::getNameStdString() const
{
  return m_d->pV->getLogVol()->getName();
}

//____________________________________________________________________
bool VolumeHandle::hasName(const std::string& n) const
{
  return m_d->pV->getLogVol()->getName() == n;
}

//____________________________________________________________________
SoMaterial * VolumeHandle::material()
{
  if (m_d->material)
    return m_d->material;

  //First see if the "databases" of volumes/material know about this volume:

  SoMaterial * mat = m_d->commondata->volVisAttributes()->get(m_d->pV->getLogVol()->getName());
  if (mat) {
    m_d->material = mat;
    m_d->material->ref();
    return m_d->material;
  }
  mat = m_d->commondata->matVisAttributes()->get(m_d->pV->getLogVol()->getMaterial()->getName());
  if (mat) {
    m_d->material = mat;
    m_d->material->ref();
    return m_d->material;
  }

  //Apparently not. We now have two ways of finding a material: We can
  //take a system dependent fallback material, or we can take the
  //material of the parent.

  mat = m_d->commondata->fallBackTopLevelMaterial();
  if (mat) {
    m_d->material = mat;
    m_d->material->ref();
    return m_d->material;
  }

  if (m_parent) {
    m_d->material = m_parent->material();
    assert(m_d->material);
    m_d->material->ref();
    return m_d->material;
  }

  m_d->material = m_d->commondata->matVisAttributes()->get("DEFAULT");
  assert(m_d->material);
  m_d->material->ref();
  return m_d->material;
}

//____________________________________________________________________
SoSeparator * VolumeHandle::nodeSoSeparator() const
{
  return m_d->nodesep;
}

//____________________________________________________________________
void VolumeHandle::ensureBuildNodeSep()
{
  VP1Msg::messageDebug("VolumeHandle::ensureBuildNodeSep()");
  if (m_d->nodesep && m_d->label_sep)
    return;

  m_d->label_sep = new SoSeparator;
  m_d->label_sep->ref();

  m_d->nodesep = new SoSeparator;//FIXME: rendercaching??
  //   m_d->nodesep->renderCaching.setValue(SoSeparator::ON);
  //   m_d->nodesep->boundingBoxCaching.setValue(SoSeparator::ON);
  m_d->nodesep->ref();//Since we are keeping it around irrespective of whether it is attached or not.

  //Transform:
  m_d->nodesep->addChild(VP1LinAlgUtils::toSoTransform(m_d->accumTrans));

  //VP1Msg::messageDebug("calling toShapeNode()...");
  SoNode * shape = m_d->commondata->toShapeNode(m_d->pV);//NB: Ignore contained transformation of GeoShapeShifts.
  if (!shape) {
    m_d->nodesep->removeAllChildren();
    std::cout << "Geomsys/VolumeHandle Error: Null shape!!!"<<std::endl;
    return;
  }

  //What phi sector do we belong in?
  int iphi = m_d->commondata->phiSectorManager()->getVolumeType(m_d->accumTrans, shape);

  if (iphi >= -1 ) {
    //VP1Msg::messageDebug("Cylinders [iphi >= -1]...");
    //Substitute shapes that are essentially cylinders with such. This
    //can be done safely since this tube won't need
    //phi-slicing and is done to gain render performance.
    if ( m_d->pV->getLogVol()->getShape()->typeID()==GeoTube::getClassTypeID() ) 
    {
      //VP1Msg::messageDebug("GeoTube...");
      const GeoTube * geotube = static_cast<const GeoTube*>(m_d->pV->getLogVol()->getShape());
      if (geotube->getRMin()==0.0)
        shape = m_d->commondata->getSoCylinderOrientedLikeGeoTube(geotube->getRMax(),geotube->getZHalfLength());
    } 
    else if ( m_d->pV->getLogVol()->getShape()->typeID()==GeoTubs::getClassTypeID() ) 
    {
      //VP1Msg::messageDebug("GeoTubs...");
      const GeoTubs * geotubs = static_cast<const GeoTubs*>(m_d->pV->getLogVol()->getShape());
      if (geotubs->getRMin()==0.0 && geotubs->getDPhi() >= 2*M_PI-1.0e-6)
        shape = m_d->commondata->getSoCylinderOrientedLikeGeoTube(geotubs->getRMax(),geotubs->getZHalfLength());
    }
  }

  //In the case of a GeoShapeShift we add its contained transformation here:
  //Fixme: Remember to use this extra transformation for phisector cuts also!
  if (m_d->pV->getLogVol()->getShape()->typeID()==GeoShapeShift::getClassTypeID())
    m_d->nodesep->addChild(VP1LinAlgUtils::toSoTransform(Amg::EigenTransformToCLHEP(dynamic_cast<const GeoShapeShift*>(m_d->pV->getLogVol()->getShape())->getX())));

  //Add shape child(ren) and get the separator (helper) where we attach the nodesep when volume is visible:
  if (iphi >= -1) {
    m_d->nodesep->addChild(shape);
    m_d->attachsepHelper = m_d->commondata->phiSectorManager()->getSepHelperForNode(m_d->commondata->subSystemFlag(), iphi);
    m_d->attachlabelSepHelper = m_d->commondata->phiSectorManager()->getLabelSepHelperForNode(m_d->commondata->subSystemFlag(), iphi);
  } else {
    SoSwitch * sw = new SoSwitch;
    sw->addChild(shape);
    SoSeparator * sep_slicedvols = new SoSeparator;
    sw->addChild(sep_slicedvols);
    sw->whichChild = 0;
    m_d->nodesep->addChild(sw);
    m_d->attachsepHelper = m_d->commondata->phiSectorManager()->registerVolumeAroundZAxis( m_d->commondata->subSystemFlag(), sw, m_d->accumTrans );
    //FIXME - what about labels?
  }

  //Register shape to volume handle connection (for user interactions):
  //NB: "shape" might be shared between several volumes, so we use the separator above for the actual link!
  //(this must be done last as it also sets outline defaults)
  m_d->commondata->registerNodeSepForVolumeHandle(m_d->nodesep,this);

  //VP1Msg::messageDebug("VolumeHandle::ensureBuildNodeSep() - DONE.");
}

//____________________________________________________________________
void VolumeHandle::Imp::attach(VolumeHandle*vh)
{
  VP1Msg::messageDebug("VolumeHandle::Imp::attach() - name: " + vh->getName());
  if (!isattached) {
    vh->ensureBuildNodeSep();
    if (attachsepHelper) {
      VP1Msg::messageDebug("adding node..."); 
      attachsepHelper->addNodeUnderMaterial(nodesep,vh->material());
    }
    if (attachlabelSepHelper) {
      VP1Msg::messageDebug("adding label..."); 
      attachlabelSepHelper->addNode(label_sep);
    }
    isattached=true;
    commondata->volumeBrowser()->scheduleUpdateOfAllNonHiddenIndices();//Browser need to change e.g. colour for this volume
  }
  //VP1Msg::messageDebug("attach: DONE.");
}

//____________________________________________________________________
void VolumeHandle::Imp::detach()
{
  if (isattached) {
    if (attachsepHelper)
      attachsepHelper->removeNodeUnderMaterial(nodesep,material);
    if (attachlabelSepHelper)
      attachlabelSepHelper->removeNode(label_sep);
    isattached=false;
    commondata->volumeBrowser()->scheduleUpdateOfAllNonHiddenIndices();//Browser need to change e.g. colour for this volume
  }
}

//____________________________________________________________________
void VolumeHandle::setState( const VP1GeoFlags::VOLSTATE& state )
{
  if (m_state==state)
    return;

  //Mark muon chamber as dirty.
  if (isInMuonChamber()) {
    VolumeHandle *tp = topLevelParent();
    if (tp->m_muonChamberState == MUONCHAMBER)
      tp->m_muonChamberState = MUONCHAMBER_DIRTY;
  }

  //Update state flag and presence in GUI lists:
  VP1GeoFlags::VOLSTATE oldstate = m_state;
  m_state = state;
  if (oldstate==VP1GeoFlags::ZAPPED)
    m_d->commondata->removeZappedVolumesFromGui(this);
  else if (state==VP1GeoFlags::ZAPPED)
    m_d->commondata->addZappedVolumeToGui(this);

  //Only thing left is visibility updates (i.e. attachment to 3D scenegraph).

  if (haveParentsNotExpanded()) {
    //No visibility updates necessary
    assert(!m_d->isattached);
    return;
  }

  //We might need visibility updates. Which ones depend on the
  //particular change of state:

  if (state==VP1GeoFlags::CONTRACTED) {
    // VP1GeoFlags::EXPANDED/VP1GeoFlags::ZAPPED -> VP1GeoFlags::CONTRACTED: Show this and hide all children.
    m_d->attach(this);
    detachAllContractedChildren();
  } else if (state==VP1GeoFlags::EXPANDED) {
    // VP1GeoFlags::CONTRACTED -> VP1GeoFlags::EXPANDED: Hide this. Allow children to be shown.
    // VP1GeoFlags::ZAPPED -> VP1GeoFlags::EXPANDED: Allow children to be shown.
    if (oldstate==VP1GeoFlags::CONTRACTED)
      m_d->detach();
    attachAllContractedChildren();
  } else {
    assert(state==VP1GeoFlags::ZAPPED);
    // VP1GeoFlags::CONTRACTED -> VP1GeoFlags::ZAPPED: Hide this.
    // VP1GeoFlags::EXPANDED -> VP1GeoFlags::ZAPPED: Hide all children.
    if (oldstate==VP1GeoFlags::CONTRACTED)
      m_d->detach();
    else
      detachAllContractedChildren();
  }
}

//____________________________________________________________________
void VolumeHandle::contractDaughtersRecursively()
{
  if (!childrenAreInitialised())
    return;//Fine to abort since children starts in contracted state
	   //if initialised at a later time.
  VolumeHandleListItr childItrE = m_children.end();
  for (VolumeHandleListItr childItr = m_children.begin(); childItr!=childItrE; ++childItr) {
    (*childItr)->setState(VP1GeoFlags::CONTRACTED);
    (*childItr)->contractDaughtersRecursively();
  }
}

//____________________________________________________________________
bool VolumeHandle::haveParentsNotExpanded() const
{
  return m_parent ? ( m_parent->state()==VP1GeoFlags::EXPANDED ? m_parent->haveParentsNotExpanded() : true )  : false;
}

//____________________________________________________________________
void VolumeHandle::attachAllContractedChildren() {

  if (!m_nchildren)
    return;
  if (!childrenAreInitialised())
    initialiseChildren();

  VolumeHandleListItr childItrE = m_children.end();
  for (VolumeHandleListItr childItr = m_children.begin(); childItr!=childItrE; ++childItr) {
    if ((*childItr)->state()==VP1GeoFlags::CONTRACTED)
      (*childItr)->m_d->attach(*childItr);
    else if ((*childItr)->state()==VP1GeoFlags::EXPANDED)
      (*childItr)->attachAllContractedChildren();
  }
}

//____________________________________________________________________
void VolumeHandle::detachAllContractedChildren() {
  if (!m_nchildren)
    return;
  if (!childrenAreInitialised())
    return;//Since children not initialised won't have been attached!

  VolumeHandleListItr childItrE = m_children.end();
  for (VolumeHandleListItr childItr = m_children.begin(); childItr!=childItrE; ++childItr) {
    if ((*childItr)->state()==VP1GeoFlags::CONTRACTED)
      (*childItr)->m_d->detach();
    else if ((*childItr)->state()==VP1GeoFlags::EXPANDED)
      (*childItr)->detachAllContractedChildren();
  }
}

//____________________________________________________________________
int VolumeHandle::copyNumber() const
{
  GeoPVConstLink parent_pV;
  if (m_parent) {
    parent_pV = m_parent->geoPVConstLink();
  } else {
    parent_pV = m_d->commondata->geoPVConstLinkOfTreeTopsMother();
    if (parent_pV==m_d->pV) {
      //To follow historical behaviour, we always assume that
      //tree-tops (in the GeoModel sense) have a copy number of 0.
      return 0;
    }
  }
  GeoVolumeCursor av(parent_pV);
  int i(0);//We need to check the childNumber since volumes in a replica have same volume link
  while (!av.atEnd()) {
    if (m_childNumber==i&&m_d->pV==av.getVolume()) {
       Query<int> Qint = av.getId();
       return Qint.isValid() ? int(Qint) : -1;//-1 for "Invalid"
    }
    av.next();
    ++i;
  }
  return -2;//error
}

//____________________________________________________________________
quint32 VolumeHandle::hashID() const
{
  //  return qHash( QPair<QString,quint32>(getName(),copyNumber()));//NOT UNIQUE!!
  return qHash( QPair<QString,quint32>(getName(),childNumber()));
}

//____________________________________________________________________
const SbMatrix& VolumeHandle::getGlobalTransformToVolume() const {
  return m_d->accumTrans;
}

//____________________________________________________________________
SbMatrix VolumeHandle::getLocalTransformToVolume() const {
  if (!m_parent)
    return m_d->accumTrans;
  return m_parent->getGlobalTransformToVolume().inverse().multRight(m_d->accumTrans);
}

//____________________________________________________________________
bool VolumeHandle::isAttached() const
{
  return m_d->isattached;
}

//____________________________________________________________________
const GeoMaterial * VolumeHandle::geoMaterial() const {
  return geoPVConstLink()->getLogVol()->getMaterial();
}

//____________________________________________________________________
bool VolumeHandle::isEther() const
{
  return !isMuonChamber() && QString(geoMaterial()->getName().c_str()).endsWith("Ether");
}

//____________________________________________________________________
void VolumeHandle::expandMothersRecursivelyToNonEther() {
  //std::cout<<"VolumeHandle::expandMothersRecursivelyToNonEther() for "<<this->getNameStdString() << " [" <<this<< "]" << " - n. children: " << nChildren() << std::endl;
  
  if (!nChildren()||!isEther()) {
    //VP1Msg::messageDebug("====> no children or not 'Ether' material. Skipping & returning.");
    return;
  }

  setState(VP1GeoFlags::ZAPPED);
  initialiseChildren();
  VolumeHandleListItr childItrE = m_children.end();
  for (VolumeHandleListItr childItr = m_children.begin(); childItr!=childItrE; ++childItr) {
    //std::cout << "\t-->expanding child " << (*childItr)->getNameStdString() << " - " << (*childItr) << std::endl ;
    (*childItr)->expandMothersRecursivelyToNonEther();
  }
  setState(VP1GeoFlags::EXPANDED);
  //std::cout<<"VolumeHandle::expandMothersRecursivelyToNonEther() for "<<this->getNameStdString() << " [" <<this<< "]" <<" DONE.\n\n"<<std::endl;
  
}

//____________________________________________________________________
QByteArray VolumeHandle::getPersistifiableState() const
{
  QByteArray ba;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);

  QDataStream out(&buffer);
  out << (qint32)0;//version
  out << qint32(m_state==VP1GeoFlags::CONTRACTED? 0 : (m_state==VP1GeoFlags::EXPANDED?1:2));
  out << Imp::getChildStates(this);

  buffer.close();
  return ba;

}

//____________________________________________________________________
void VolumeHandle::applyPersistifiableState( QByteArray ba )
{
  QBuffer buffer(&ba);
  buffer.open(QIODevice::ReadOnly);
  QDataStream state(&buffer);
  qint32 version;
  state >> version;
  if (version!=0)
    return;//We ignore wrong versions silently here.
  qint32 st;
  state >> st;
  VP1GeoFlags::VOLSTATE newstate(VP1GeoFlags::CONTRACTED);
  if (st == 2)
    newstate = VP1GeoFlags::ZAPPED;
  else if (st == 1)
    newstate = VP1GeoFlags::EXPANDED;

  Imp::VolState vs;
  state >> vs;
  buffer.close();

  if (newstate==VP1GeoFlags::EXPANDED) {
    setState(VP1GeoFlags::ZAPPED);
    m_d->applyChildStates(vs,this);
  }
  setState(newstate);
}

//____________________________________________________________________
VP1GeoFlags::SubSystemFlag VolumeHandle::subsystem() const
{
  return m_d->commondata->subSystemFlag();
}

//____________________________________________________________________
VolumeHandle::Imp::VolState VolumeHandle::Imp::getChildStates(const VolumeHandle* theclass)
{
  VolState vs;
  if (theclass->m_state!=VP1GeoFlags::EXPANDED||theclass->m_nchildren==0||!theclass->childrenAreInitialised())
    return vs;

  VolumeHandleListConstItr it, itE = theclass->m_children.end();
  for (it = theclass->m_children.begin(); it!=itE; ++it) {
    if ((*it)->state()==VP1GeoFlags::ZAPPED)
      vs.m_zappedChildren.insert((*it)->hashID());
    else if ((*it)->state()==VP1GeoFlags::EXPANDED)
      vs.m_expandedChildren.insert((*it)->hashID(),(*it)->m_d->getChildStates(*it));
  }
  return vs;
}

//____________________________________________________________________
void VolumeHandle::Imp::applyChildStates(const VolState& vs,VolumeHandle*theclass)
{
  bool hasExpandedChildren = !vs.m_expandedChildren.isEmpty();
  bool hasZappedChildren = !vs.m_zappedChildren.isEmpty();
  if (!hasExpandedChildren&&!hasZappedChildren)
    return;

  QMap<quint32,VolState>::const_iterator expItr, expItrEnd = vs.m_expandedChildren.end();
  QSet<quint32>::const_iterator zapItr, zapItrEnd = vs.m_zappedChildren.end();

  theclass->initialiseChildren();
  VolumeHandleListConstItr it, itE = theclass->m_children.end();
  for (it = theclass->m_children.begin(); it!=itE; ++it) {
    quint32 id = (*it)->hashID();
    zapItr = vs.m_zappedChildren.find(id);
    if (zapItr!=zapItrEnd) {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      continue;
    }
    expItr = vs.m_expandedChildren.find(id);
    if (expItr!=expItrEnd) {
      (*it)->setState(VP1GeoFlags::ZAPPED);
      applyChildStates(expItr.value(),*it);
      (*it)->setState(VP1GeoFlags::EXPANDED);
    }
  }
}

//____________________________________________________________________
bool VolumeHandle::Imp::hasNonStandardShapeChildren(const SoGroup*g)
{
  const int n(g->getNumChildren());
  for (int i=0; i < n; ++i) {
    const SoNode*c = g->getChild(i);
    if (c->getTypeId().isDerivedFrom(SoShape::getClassTypeId())) {
      if (c->getTypeId().isDerivedFrom(SoPcons::getClassTypeId())
	  ||c->getTypeId().isDerivedFrom(SoPolyhedron::getClassTypeId())
	  ||c->getTypeId().isDerivedFrom(SoTransparency::getClassTypeId()))
	return true;
    } else if (c->getTypeId().isDerivedFrom(SoGroup::getClassTypeId())) {
      if (hasNonStandardShapeChildren(static_cast<const SoGroup*>(c)))
	return true;
    }
  }
  return false;
}

//____________________________________________________________________
bool VolumeHandle::isInitialisedAndHasNonStandardShape() const
{
  VP1HEPVisUtils::initAllCustomClasses();
  return m_d->nodesep ? Imp::hasNonStandardShapeChildren(m_d->nodesep) : false;
}

//____________________________________________________________________
bool VolumeHandle::isPositiveZ() const
{
  SbVec3f dir;
  m_d->accumTrans.multDirMatrix(SbVec3f(0,0,1),dir);
  float x,y,z;
  dir.getValue(x,y,z);
  return z>0;
}

void VolumeHandle::updateLabels() {
  int labels = m_d->commondata->controller()->labels();
  QList<int> offsets = m_d->commondata->controller()->labelPosOffset();

  m_d->attachlabelSepHelper->largeChangesBegin();

  if ( m_d->labels != labels || m_d->labelsPosOffsets != offsets){
    m_d->labels = labels;
    m_d->labelsPosOffsets = offsets;
    m_d->label_sep->removeAllChildren ();

    SoText2 *labelText = new SoText2;
    labelText->ref();
    QStringList text;
    if (labels&0x1){
      QString name = getDescriptiveName();
      text << name;
    }

    // only one label selected - must be better way to do this!
    bool singleLabel = (labels&0x2 && !(labels&0x4) && !(labels&0x8))
    || (labels&0x4 && !(labels&0x2) && !(labels&0x8))
    || (labels&0x8 && !(labels&0x2) && !(labels&0x4));
    if (isInMuonChamber() && labels&0xE){
      for (unsigned int i=0; i<3 ;++i){
        // Check to see which label we use.
        unsigned int mask=1<<(i+1);
        if (!(labels&mask)) continue; // bits 1,2,3 are used for labels

        QString output;
        if ( singleLabel ) {
          output+="t0=";
        } else {
          if (i==0) output+="Moore t0=";
          if (i==1) output+="Mboy t0=";
          if (i==2) output+="<unknown>=";
        }
        output += muonChamberT0(i);
        text << output;
      }
    }

    unsigned int row=0;
    foreach(QString str, text){
      QByteArray array = str.toLatin1();
      labelText->string.set1Value(row++,array.data());
    }

    SoTransform *labelTranslate = VP1LinAlgUtils::toSoTransform(m_d->accumTrans);
    assert(labelTranslate!=0);
    float offScale=10.0;
    float labx,laby,labz;
    labelTranslate->translation.getValue().getValue(labx,laby,labz);
//    std::cout <<getNameStdString()<<" offsets (x,y,z)=("<<offsets[0]<<","<<offsets[1]<<","<<offsets[2]<<")"<<std::endl;

    labelTranslate->translation.setValue(labx+(offsets[0]*offScale),laby+(offsets[1]*offScale),labz+(offsets[2]*offScale));

    m_d->label_sep->addChild(labelTranslate);
    m_d->label_sep->addChild(labelText);
  }

  m_d->attachlabelSepHelper->largeChangesEnd();
}

