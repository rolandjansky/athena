/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1CaloClusterCollection          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2007                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1CaloClusterSystems/VP1CaloClusterCollection.h"
#include "VP1CaloClusterSystems/CaloClusterSysController.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/IVP13DSystemSimple.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"

#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoSeparator.h>
#include "VP1HEPVis/nodes/SoGenericBox.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

//_____________________________________________________________________________________
QList<VP1CaloClusterCollection*> VP1CaloClusterCollection::createCollections(IVP1System*sys,CaloClusterSysController*controller)
{
  QList<VP1CaloClusterCollection*> cols;
  foreach (QString key, VP1SGContentsHelper(sys).getKeys<CaloClusterContainer>()) {
    VP1CaloClusterCollection * col = new VP1CaloClusterCollection(key,sys,controller);
    col->init();
    cols << col;
  }
  return cols;
}

//____________________________________________________________________
class VP1CaloClusterCollection::Imp {
public:
  VP1CaloClusterCollection * theclass = nullptr;
  CaloClusterSysController*controller = nullptr;
  VP1ExtraSepLayerHelper * sephelper = nullptr;
  QString key;

  bool considerTransverseEnergies = false;
  QPair<bool,double> scale;
  bool showOutlines = false;
  VP1Interval allowedEnergies;
  VP1Interval allowedEta;
  QList<VP1Interval> allowedPhi;
  double last_highestEnergy = 0.0;
  double calculateHighestVisibleClusterEnergy() const;

  double energyToLength(const double&energy) {
    return std::max(1*CLHEP::mm, scale.second*(scale.first?log(1+fabs(energy)):energy));
  }

  static double calo_start_r;
  static double calo_start_z;
  static double calo_crack_eta;

  class ClusterHandle {
  public:
    ClusterHandle(const CaloCluster* c) : m_attached(false), m_cluster(c), m_genericBox(0)
    {
      m_transverseEnergy = sin(2*atan(exp(-fabs(eta()))))*energy();
    }
    ~ClusterHandle() { if (m_genericBox) m_genericBox->unref(); }
    void attach(VP1CaloClusterCollection::Imp * d) {
      if (!m_genericBox) {
	m_genericBox = new SoGenericBox();
	m_genericBox->drawEdgeLines = d->showOutlines;
	m_genericBox->forceEdgeLinesInBaseColour = true;
	m_genericBox->ref();
      }
      m_attached = true;
      updateShapePars(d);
      d->sephelper->addNode(m_genericBox);
    }
    void detach(VP1ExtraSepLayerHelper * sh) {
      if (m_genericBox) {
	sh->removeNode(m_genericBox);
      }
      m_attached = false;
    }
    void updateShapePars(VP1CaloClusterCollection::Imp * d) {
      if (!m_attached)
	return;
      const double dEta = 0.05;//FIXME
      const double dPhi = 0.05;//FIXME
      const double theEta = eta();
      const double thePhi = phi();
      const double cellDepth = d->energyToLength(energyForLengthAndCuts(d));
      if (fabs(theEta) < VP1CaloClusterCollection::Imp::calo_crack_eta) {
	m_genericBox->setParametersForBarrelEtaPhiCell( theEta-0.5*dEta, theEta+0.5*dEta,thePhi-0.5*dPhi, thePhi+0.5*dPhi,
							cellDepth, VP1CaloClusterCollection::Imp::calo_start_r );
      } else {
	m_genericBox->setParametersForEndCapEtaPhiCell( theEta-0.5*dEta, theEta+0.5*dEta,thePhi-0.5*dPhi, thePhi+0.5*dPhi,
							cellDepth, VP1CaloClusterCollection::Imp::calo_start_z );
      }
    }

    SoGenericBox * genericBox() const { return m_genericBox; }
    double phi() const { return m_cluster->phi(); }
    double eta() const { return m_cluster->eta(); }
    double transverseEnergy() const { return m_transverseEnergy; }
    double energy() const { return m_cluster->e(); }
    double energyForLengthAndCuts(const VP1CaloClusterCollection::Imp * d) { return d->considerTransverseEnergies ? transverseEnergy() : energy(); }
    const CaloCluster* cluster() const { return m_cluster; }
    bool attached() const { return m_attached; }
  private:
    bool m_attached;
    double m_transverseEnergy;
    const CaloCluster* m_cluster;
    SoGenericBox* m_genericBox;
  };
  bool cut(ClusterHandle* c) {
    if (!allowedEnergies.contains(c->energyForLengthAndCuts(this)))
      return false;
    if (!allowedEta.contains(c->eta()))
      return false;
    double phi(c->phi());
    foreach(VP1Interval i,allowedPhi) {
      if (i.contains(phi)||i.contains(phi+2*M_PI)||i.contains(phi-2*M_PI))
	return true;
    }
    return false;
  }

  void recheckCut(ClusterHandle* c) {
    const bool cutval = cut(c);
    if (cutval!=c->attached()) {
      if (cutval)
	c->attach(this);
      else
	c->detach(sephelper);
    }
  }

  QList<ClusterHandle*> clusters;

};

//Fixme: Just some approximate values for now:
double VP1CaloClusterCollection::Imp::calo_start_r = 1.1*CLHEP::m + 0.05*CLHEP::m;
double VP1CaloClusterCollection::Imp::calo_start_z = 3.671*CLHEP::m + 0.05*CLHEP::m;
double VP1CaloClusterCollection::Imp::calo_crack_eta = fabs(log(tan(0.5*atan(calo_start_r/calo_start_z))));

//____________________________________________________________________
double VP1CaloClusterCollection::crackEta()
{
  return VP1CaloClusterCollection::Imp::calo_crack_eta;
}

//____________________________________________________________________
VP1CaloClusterCollection::VP1CaloClusterCollection(const QString& key, IVP1System * sys,CaloClusterSysController*controller)
 : VP1StdCollection(sys,"VP1CaloClusterCollection_"+key), m_d(new Imp)
{
  SoGenericBox::initClass();

  m_d->theclass = this;
  m_d->controller = controller;
  m_d->key = key;
  m_d->sephelper = 0;
  //We start out with no clusters visible:
  m_d->last_highestEnergy = 0;
  m_d->showOutlines = false;
  m_d->considerTransverseEnergies = true;

  connect(this,SIGNAL(highestVisibleClusterEnergyChanged()),controller,SLOT(possibleChange_scale()));
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(recheckHighestVisibleClusterEnergy()));

  connect(controller,SIGNAL(useTransverseEnergiesChanged(bool)),this,SLOT(setConsiderTransverseEnergies(bool)));
  setConsiderTransverseEnergies(controller->useTransverseEnergies());
  connect(controller,SIGNAL(scaleChanged(const QPair<bool,double>&)),this,SLOT(setScale(const QPair<bool,double>&)));
  setScale(controller->scale());
  connect(controller,SIGNAL(showVolumeOutLinesChanged(bool)),this,SLOT(setShowVolumeOutLines(bool)));
  setShowVolumeOutLines(controller->showVolumeOutLines());
  connect(controller,SIGNAL(cutAllowedEnergiesChanged(const VP1Interval&)),this,SLOT(setAllowedEnergies(const VP1Interval&)));
  setAllowedEnergies(controller->cutAllowedEnergies());
  connect(controller,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setAllowedEta(const VP1Interval&)));
  setAllowedEta(controller->cutAllowedEta());
  connect(controller,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setAllowedPhi(const QList<VP1Interval>&)));
  setAllowedPhi(controller->cutAllowedPhi());


}

//____________________________________________________________________
VP1CaloClusterCollection::~VP1CaloClusterCollection()
{
  if (m_d->sephelper) {
    SoSeparator * sep = m_d->sephelper->topSeparator();
    delete m_d->sephelper;
    sep->unref();
  }
  foreach(Imp::ClusterHandle*cluster,m_d->clusters)
    delete cluster;
  delete m_d;
}

//____________________________________________________________________
QString VP1CaloClusterCollection::provideSection() const
{
  return "";
}

//____________________________________________________________________
QString VP1CaloClusterCollection::provideText() const
{
  return m_d->key;
}

//____________________________________________________________________
void VP1CaloClusterCollection::assignDefaultMaterial(SoMaterial* m) const
{
  VP1MaterialButton::setMaterialParameters( m, 1.0, 0.9, 0.0, 0.15/*brightness*/, 0.5/*transp*/ );
}


//____________________________________________________________________
double VP1CaloClusterCollection::Imp::calculateHighestVisibleClusterEnergy() const
{
  double e = 0;
  if (theclass->visible()) {
    foreach(Imp::ClusterHandle*cluster,clusters) {
      if (cluster->attached()&&e<cluster->energyForLengthAndCuts(this))
	e=cluster->energyForLengthAndCuts(this);
    }
  }
  return e;
}

//____________________________________________________________________
void VP1CaloClusterCollection::recheckHighestVisibleClusterEnergy()
{
  //When visibility or cut changes:
  double newhighest = m_d->calculateHighestVisibleClusterEnergy();
  if (m_d->last_highestEnergy == newhighest)
    return;
  m_d->last_highestEnergy = newhighest;
  emit highestVisibleClusterEnergyChanged();
}

//____________________________________________________________________
double VP1CaloClusterCollection::highestVisibleClusterEnergy() const
{
  return m_d->last_highestEnergy;
}

//____________________________________________________________________
bool VP1CaloClusterCollection::load()
{
  //Get collection:
  const CaloClusterContainer *theClusterCollection(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(theClusterCollection,m_d->key))
    return false;

  SoSeparator * sep = new SoSeparator;
  sep->setName("ClusterSepHelperTopSep");
  sep->ref();
  m_d->sephelper = new VP1ExtraSepLayerHelper(sep);
  largeChangesBegin();
  int i(0);
  CaloClusterContainer::const_iterator it(theClusterCollection->begin()),itE(theClusterCollection->end());
  for ( ; it != itE; ++it) {
    if (i++%40==0)
      systemBase()->updateGUI();
    Imp::ClusterHandle * clusterHandle = new Imp::ClusterHandle(*it);
    m_d->clusters << clusterHandle;
    m_d->recheckCut(clusterHandle);
  }
  largeChangesEnd();
  collSep()->addChild(sep);

  return true;
}

//____________________________________________________________________
QStringList VP1CaloClusterCollection::infoOnClicked(SoPath* pickedPath)
{
  SoNode * pickedNode = pickedPath->getNodeFromTail(0);
  const CaloCluster*cluster(0);
  Imp::ClusterHandle*clusterHandle(0);
  if (pickedNode->getTypeId()==SoGenericBox::getClassTypeId()) {
    foreach(Imp::ClusterHandle*c,m_d->clusters) {
      if (c->genericBox()==pickedNode) {
	cluster = c->cluster();
	clusterHandle = c;
	break;
      }
    }
  }

  if (!cluster||!clusterHandle)
    return QStringList() << "Error: Collection "+m_d->key+" does not have cluster information for picked node";

  QStringList l;
  if (m_d->controller->printInfoOnClick()) {
    l << "  Eta: "+str(cluster->eta());
    l << "  Phi: "+str(cluster->phi());
    l << "  Energy [GeV]: "+str(clusterHandle->energy()/CLHEP::GeV);
    l << "  Transverse Energy [GeV]: "+str(clusterHandle->transverseEnergy()/CLHEP::GeV);
    if (m_d->controller->printVerboseInfoOnClick()) {
      l << "(no verbose information available)";
      //   l << "  Lambda_center [CLHEP::cm]: "+str(cluster->getMoment(CaloClusterMoment::CENTER_LAMBDA).getValue()/CLHEP::cm);
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
void VP1CaloClusterCollection::largeChangesBegin()
{
  VP1StdCollection::largeChangesBegin();
  m_d->sephelper->largeChangesBegin();
}
//____________________________________________________________________
void VP1CaloClusterCollection::largeChangesEnd()
{
  VP1StdCollection::largeChangesEnd();
  m_d->sephelper->largeChangesEnd();
}

//____________________________________________________________________
void VP1CaloClusterCollection::setAllowedEnergies(const VP1Interval& i)
{
  if (m_d->allowedEnergies==i)
    return;
  m_d->allowedEnergies=i;
  if (!isLoaded())
    return;
  static_cast<IVP13DSystemSimple *>(systemBase())->deselectAll();
  largeChangesBegin();
  foreach(Imp::ClusterHandle*cluster,m_d->clusters)
    m_d->recheckCut(cluster);
  largeChangesEnd();
  //Fixme relaxed/tightened stuff

  recheckHighestVisibleClusterEnergy();
}

//____________________________________________________________________
void VP1CaloClusterCollection::setAllowedEta(const VP1Interval& i)
{
  if (m_d->allowedEta==i)
    return;
  m_d->allowedEta=i;
  if (!isLoaded())
    return;
  static_cast<IVP13DSystemSimple *>(systemBase())->deselectAll();
  largeChangesBegin();
  foreach(Imp::ClusterHandle*cluster,m_d->clusters)
    m_d->recheckCut(cluster);
  largeChangesEnd();
  //Fixme relaxed/tightened stuff

  recheckHighestVisibleClusterEnergy();
}

//____________________________________________________________________
void VP1CaloClusterCollection::setAllowedPhi(const QList<VP1Interval>& i)
{
  if (m_d->allowedPhi==i)
    return;
  m_d->allowedPhi=i;
  if (!isLoaded())
    return;
  static_cast<IVP13DSystemSimple *>(systemBase())->deselectAll();
  largeChangesBegin();
  foreach(Imp::ClusterHandle*cluster,m_d->clusters)
    m_d->recheckCut(cluster);
  largeChangesEnd();
  //Fixme relaxed/tightened stuff

  recheckHighestVisibleClusterEnergy();
}

#include <Inventor/nodes/SoMaterial.h>//Fixme: just for hack below
//____________________________________________________________________
void VP1CaloClusterCollection::setScale(const QPair<bool,double>& s)
{
  if (m_d->scale==s)
    return;
  m_d->scale=s;
  if (!isLoaded())
    return;
  largeChangesBegin();
  foreach(Imp::ClusterHandle*cluster,m_d->clusters)
    if (cluster->attached())
      cluster->updateShapePars(m_d);
  largeChangesEnd();

  //   if (verbose())
  //     static_cast<IVP13DSystemSimple *>(systemBase())->warnOnDisabledNotifications();

  material()->touch();//FIXME: This is needed (and nothing else seems to matter). BUT WHY?????? Where is the bug?!?!?
}

//____________________________________________________________________
void VP1CaloClusterCollection::setShowVolumeOutLines(bool b)
{
  if (m_d->showOutlines==b)
    return;
  m_d->showOutlines=b;
  if (!isLoaded())
    return;
  largeChangesBegin();
  foreach(Imp::ClusterHandle*cluster,m_d->clusters)
    if (cluster->genericBox())
      cluster->genericBox()->drawEdgeLines = b;
  largeChangesEnd();
}

//____________________________________________________________________
void VP1CaloClusterCollection::setConsiderTransverseEnergies(bool b)
{
  if (m_d->considerTransverseEnergies==b)
    return;
  m_d->considerTransverseEnergies=b;
  if (!isLoaded())
    return;
  largeChangesBegin();
  foreach(Imp::ClusterHandle*cluster,m_d->clusters) {
    bool attachPrev(cluster->attached());
    m_d->recheckCut(cluster);
    if (attachPrev&&cluster->attached())
      cluster->updateShapePars(m_d);
  }
  largeChangesEnd();
  recheckHighestVisibleClusterEnergy();
}
