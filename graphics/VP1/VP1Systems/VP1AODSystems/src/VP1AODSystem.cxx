/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1AODSystem                      //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

// Local
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1AODSystems/AODSystemController.h"
#include "IParticleCollHandleBase.h"
#include "AODCollHandleBase.h"
#ifndef BUILDVP1LIGHT
    #include "IParticleHandle_TrackParticle.h"
#endif
#include "IParticleCollHandle_Electron.h"
#include "IParticleCollHandle_Muon.h"
#include "IParticleCollHandle_TrackParticle.h"
#include "IParticleCollHandle_CaloCluster.h"
#include "IParticleCollHandle_Jet.h"
#include "MissingEtCollHandle.h"
#include "VertexCollHandle.h"
#include "AODSysCommonData.h"
#include "AODCollWidget.h"

//VP1
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VisibleObjectToMaterialHelper.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1DetInfo.h"
#ifndef BUILDVP1LIGHT
  #include "VP1Utils/VP1AvailableToolsHelper.h"
  #include "VP1Utils/VP1ToolAccessHelper.h"
#endif

// SoCoin (see if we can shift this to external file?)
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoFont.h>

// Std
#include <sstream>
#include <typeinfo>

#ifndef BUILDVP1LIGHT
  // Athena
  #include "StoreGate/StoreGateSvc.h"
  #include "TrkExInterfaces/IExtrapolationEngine.h"
#endif

// Qt
#include <QTreeWidget>

// XAOD
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/IOStats.h"

// ROOT include(s):
#include <TFile.h>
#include <TError.h>


class VP1AODSystem::Imp {
public:
  VP1AODSystem * theclass = nullptr;
  AODSysCommonData * common = nullptr;
  SoCooperativeSelection * selObjects = nullptr;
  // AODHandleSelectionManager * ascObjSelManager;

  SoSeparator * totmomsep = nullptr;
  SoLineSet * totmomline = nullptr;
  Amg::Vector3D totmomgev;
  double totmass = 0.0;


  // TrackCommonFlags::SELECTIONMODE selMode;

  //Return value is number of track handles used for input (e.g. those without valid momentum info are ignored)
  unsigned calcTotalMomentumOfSelectedHandles(Amg::Vector3D& totmom, Amg::Vector3D& totpos, double& mass);

  //  template <class T>
  //  QList<IParticleCollHandleBase*> createSpecificCollections(xAOD::Type::ObjectType type) {
  //    QList<IParticleCollHandleBase*> l;
  //    foreach (QString name, T::availableCollections(theclass)) {
  //      T * col = new T(common,name,type);
  //      col->init();
  //      l << col;
  //    }
  //    return l;
  //  }
  template <class T>
  QList<AODCollHandleBase*> createSpecificCollections(xAOD::Type::ObjectType type) {
    QList<AODCollHandleBase*> l;
      foreach (QString name, T::availableCollections(theclass)) {
        T * col = new T(common,name,type);
        col->init();
        l << col;
      }
    return l;
  }

  /*
  * here we load the different systems (Muon, Jet, TrackParticle, ...)
  *
  * types from:
  * http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODBase/Root/ObjectType.cxx
  *
  */
  QList<AODCollHandleBase*> createCollections() {
    QList<AODCollHandleBase*> l;
   l << createSpecificCollections<IParticleCollHandle_TrackParticle>(xAOD::Type::TrackParticle);
   l << createSpecificCollections<IParticleCollHandle_Jet>(xAOD::Type::Jet);
   l << createSpecificCollections<IParticleCollHandle_Muon>(xAOD::Type::Muon);
   l << createSpecificCollections<IParticleCollHandle_Electron>(xAOD::Type::Electron);
   l << createSpecificCollections<IParticleCollHandle_CaloCluster>(xAOD::Type::CaloCluster); // Calorimeter Clusters
   l << createSpecificCollections<MissingEtCollHandle>(xAOD::Type::Other);
   l << createSpecificCollections<VertexCollHandle>(xAOD::Type::Vertex);
    return l;
  }
};

//____________________________________________________________________
VP1AODSystem::VP1AODSystem(QString name)
  : IVP13DSystemSimple(name,
"System showing all (x)AOD objects.",
"Edward.Moyse@cern.ch, Riccardo.maria.bianchi@cern.ch, Sebastian.Andreas.Merkt@cern.ch"), m_d(new Imp)

{
  m_d->theclass = this;
  m_d->selObjects = 0;
  m_d->common = 0;
  m_d->totmomsep = 0;
  m_d->totmomline = 0;
  m_d->totmomgev = Amg::Vector3D(0,0,0);
}

//____________________________________________________________________
VP1AODSystem::~VP1AODSystem()
{
  if(m_d) {
    if(m_d->common) {
      if(m_d->common->controller()) {
        if (m_d->common->controller()->objBrowser()) {
          m_d->common->controller()->objBrowser()->close();
        }
      }
    }
  }
  delete m_d; m_d=0;
}

//____________________________________________________________________
void VP1AODSystem::systemcreate(StoreGateSvc* /*detstore*/)
{
  messageVerbose("systemcreate");
  ensureBuildController();

#ifndef BUILDVP1LIGHT
  //Get available extrapolators:
  QString tooltype("Trk::ExtrapolationEngine");
  VP1AvailableToolsHelper availTools(this);
  availTools.addMonitoredType(tooltype);
  QStringList existingExtrapolators =  availTools.availableTools();

  foreach (QString value, existingExtrapolators)
    messageVerbose(value);

  VP1ToolAccessHelper toolaccess(this);
  Trk::IExtrapolationEngine * extrapolator = toolaccess.getToolPointer<Trk::IExtrapolationEngine>("Trk::ExtrapolationEngine/AtlasExtrapolation",false/*silent*/,true/*create if not exists*/);
  m_d->common->setExtrapolator(extrapolator);
#endif

#ifdef BUILDVP1LIGHT
  // Load event from xAOD once AODSystem is created
  emit signalLoadEvent(this);
#endif // BUILDVP1LIGHT
  
  // m_d->common->controller()->initTools();
}

//____________________________________________________________________
void VP1AODSystem::systemuncreate()
{
  messageVerbose("systemuncreate");
  if (m_d->totmomsep) {
    m_d->totmomsep->unref();
    m_d->totmomsep=0;
  }
  if (m_d->totmomline) {
    m_d->totmomline->unref();
    m_d->totmomline = 0;
  }
  delete m_d->common; m_d->common = 0;
}

//____________________________________________________________________
QWidget * VP1AODSystem::buildController()
{
  messageVerbose("buildController start");
  AODSystemController * controller = new AODSystemController(this);
  m_d->common = new AODSysCommonData(this,controller);//Fixme: Do this on-demand in buildeventscenegraph!!
  controller->setCommonData(m_d->common);

  // connect(controller,SIGNAL(selectionModeChanged(TrackCommonFlags::SELECTIONMODE)),this,SLOT(updateSelectionMode()));
  // updateSelectionMode();
  // connect(controller,SIGNAL(showTotMomentumOnMultiTrackSelectionChanged(bool)),this,SLOT(updateShownTotMomentum()));
  // updateShownTotMomentum();

  messageVerbose("buildController end");
  return controller;
}

//____________________________________________________________________
void VP1AODSystem::buildEventSceneGraph(StoreGateSvc* /*sg*/, SoSeparator *root)
{
  // Note: 'sg' is not used here, so it's commented out to prevent compiler warnings

  messageVerbose("VP1AODSystem::buildEventSceneGraph start");

  // set complexity to a lower value, so that e.g. the straws are manageable
  // SoComplexity * complexity = new SoComplexity;
  // complexity->value.setValue(0.9f);//Fixme: Hardcoded here and elsewhere (fixme: Recheck all complexity values!)
  // root->addChild(complexity);
  // root->addChild(m_d->common->controller()->ascObjDrawStyle());
  // root->addChild(m_d->common->controller()->ascObjComplexity());//this will be inherited to the tracks
  //                                                             // (but giving no effect)

  m_d->selObjects = new SoCooperativeSelection;
  m_d->selObjects->activePolicy = SoCooperativeSelection::ACTIVE;
  m_d->selObjects->ref();
  registerSelectionNode(m_d->selObjects);

  // reset last selected trk
  m_d->common->setLastSelectedHandle(0);
  updateSelectionMode();

  // if (!m_d->common->m_textSep) {
  //   // FIXME!
  //   //    std::cout<<"Making new Text sep"<<std::endl;
  //   m_d->common->m_textSep = new SoSeparator;
  //   m_d->common->m_textSep->setName("TextSep");
  //   m_d->common->m_textSep->ref();
  // }
  // root->addChild(m_d->common->m_textSep);

  // Fixme - what if font is missing?
  // SoFont *myFont = new SoFont;
  // myFont->name.setValue("Arial");
  // myFont->size.setValue(13.0);
  // m_d->common->m_textSep->addChild(myFont);

  messageVerbose("createCollections start");

  //Create collection list based on contents of event store, populate gui and apply states:
  m_d->common->controller()->collWidget()->setCollections(m_d->createCollections());

  //Add collections to event scenegraph:
  foreach (VP1StdCollection* col,m_d->common->controller()->collWidget()->collections<VP1StdCollection>()){
    // std::cout<<"Add collswitch="<<col->collSwitch()<< " to EventSceneGraph"<<std::endl;
    m_d->selObjects->addChild(col->collSwitch());
  }

  root->addChild(m_d->selObjects);
  if (!m_d->totmomsep) {
    m_d->totmomsep = new SoSeparator;
    m_d->totmomsep->ref();
  }
  // m_d->totmomsep->addChild(m_d->common->controller()->trackDrawStyle());
  root->addChild(m_d->totmomsep);

  messageVerbose("buildEventSceneGraph end");
}


//____________________________________________________________________
void VP1AODSystem::systemerase()
{
  messageVerbose("systemErase begin");

  m_d->common->controller()->collWidget()->clear();
  if (m_d->common->controller()->objBrowser()) m_d->common->controller()->objBrowser()->clear();

  // m_d->common->clearEventData();
  if (m_d->selObjects) {
    unregisterSelectionNode(m_d->selObjects);
    m_d->selObjects->unref();
    m_d->selObjects=0;
  }

  if (m_d->totmomsep)
    m_d->totmomsep->removeAllChildren();

  // if (m_d->common->m_textSep) // FIXME!
  //   m_d->common->m_textSep->removeAllChildren();

  m_d->totmomgev = Amg::Vector3D(0,0,0);

  // if (verbose()) {
  //   if (AODHandleBase::numberOfInstances()!=0)
  //     message("Warning: "+str(AODHandleBase::numberOfInstances())
  //       +" instances of AODHandleBase alive at end of systemerase(). "
  //       "(ignore this warning if there is more than one track system instance).");
  //   if (AODHandleBase::numberOfInstances()!=0)
  //     message("Warning: "+str(AODHandleBase::numberOfInstances())
  //       +" instances of AODHandleBase alive at end of systemerase(). "
  //       "(ignore this warning if there is more than one track system instance).");
  // }

  messageVerbose("systemErase end");
}

//____________________________________________________________________
QByteArray VP1AODSystem::saveState()
{
  messageDebug("VP1AODSystem::saveState()");

  //Version & base state:
  VP1Serialise serialise(1/*version*/,this);

  messageDebug("saving the base class...");
  serialise.save(IVP13DSystemSimple::saveState());//Info from base class

  // Actual state info:
  messageDebug("saving the controller...");
  ensureBuildController();
  serialise.save(m_d->common->controller()->saveSettings());

  messageDebug("saving the collectionWidget...");
  serialise.save((VP1CollectionWidget*)m_d->common->controller()->collWidget());



  serialise.disableUnsavedChecks();//We do the testing in the controller

  return serialise.result();
}

//____________________________________________________________________
void VP1AODSystem::restoreFromState(QByteArray ba)
{
  messageDebug("VP1AODSystem::restoreFromState()");
  //Version & base state:
  VP1Deserialise state(ba,this);
  if (state.version()==0) {
    //We simply won't support .vp1 files from the old track system.
    message("Warning: State data has obsolete format - ignoring!");
    return;
  }
  if (state.version()!=1) {
    message("Warning: State data has wrong format - ignoring!");
    return;
  }

  ensureBuildController();

  IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

  m_d->common->controller()->restoreSettings(state.restoreByteArray());

  state.restore((VP1CollectionWidget*)m_d->common->controller()->collWidget());

  state.disableUnrestoredChecks();//We do the testing in the controller
}

//____________________________________________________________________
void VP1AODSystem::visibleObjectsChanged()
{
  // VisibleObjectToMaterialHelper<Trk::Track>* trackhelper(dynamic_cast< VisibleObjectToMaterialHelper<Trk::Track>* >(sender()));
  // if (trackhelper) {
  //   //Fixme: For now we re-encode in a vector, to get old signal/slot signatures. We should change that!
  //   //       -> and remember not to put const in front of the so materials!!
  //   std::vector< std::pair<const Trk::Track*, const SoMaterial*> > vistracks;
  //   vistracks.reserve(trackhelper->visibleObjects().size());
  //   std::map<const Trk::Track*,SoMaterial*>::const_iterator it, itE = trackhelper->visibleObjects().end();
  //   for (it = trackhelper->visibleObjects().begin();it!=itE;++it)
  //     vistracks.push_back(std::pair<const Trk::Track*, const SoMaterial*>(it->first,it->second));
  //   messageVerbose("Emitting visibleTracksChanged (with nvistracks = "+QString::number(vistracks.size())+")" );
  //   emit visibleTracksChanged(vistracks);
  //   return;
  // }
  // VisibleObjectToMaterialHelper<Trk::Segment>* segmenthelper(dynamic_cast<VisibleObjectToMaterialHelper<Trk::Segment>*>(sender()));
  // if (segmenthelper) {
  //   //Fixme: For now we re-encode in a vector, to get old signal/slot signatures. We should change that!
  //   //       -> and remember not to put const in front of the so materials!!
  //   std::vector< std::pair<const Trk::Segment*, const SoMaterial*> > vissegments;
  //   vissegments.reserve(segmenthelper->visibleObjects().size());
  //   std::map<const Trk::Segment*,SoMaterial*>::const_iterator it, itE = segmenthelper->visibleObjects().end();
  //   for (it = segmenthelper->visibleObjects().begin();it!=itE;++it)
  //     vissegments.push_back(std::pair<const Trk::Segment*, const SoMaterial*>(it->first,it->second));
  //   messageVerbose("Emitting visibleSegmentsChanged (with nvissegments = "+QString::number(vissegments.size())+")" );
  //   emit visibleSegmentsChanged(vissegments);
  //   return;
  // }
  message("unexpected signal received in visibleObjectsChanged slot");
}


//____________________________________________________________________
void VP1AODSystem::updateSelectionMode()
{
  messageVerbose("updateSelectionMode start");
  // if (!m_d->selObjects||!m_d->ascObjSelManager) {
  //   messageVerbose("updateSelectionMode Warning: Ignoring due to null pointers.");
  //   return;
  // }
  deselectAll();
  // m_d->ascObjSelManager->deselectAll();
  // m_d->selMode = m_d->common->controller()->selectionMode();//NB: Don't abort if unchanged (we call this method to init)

  // if (m_d->selMode==TrackCommonFlags::TRACKFIT) {
  //   messageVerbose("updateSelectionMode => TRACKFIT");
  //   m_d->selObjects->policy = SoCooperativeSelection::SINGLE;
  //   m_d->ascObjSelManager->setMode(AscObjSelectionManager::SHIFT);
  // } else if (m_d->selMode==TrackCommonFlags::MULTITRACK) {
  //   messageVerbose("updateSelectionMode => MULTITRACK");
  //   m_d->selObjects->policy = SoCooperativeSelection::TOGGLE;
  //   m_d->ascObjSelManager->setMode(AscObjSelectionManager::SINGLE);
  // } else {
  //   if (m_d->selMode!=TrackCommonFlags::SINGLEOBJECT)
  //     message("updateSelectionMode ERROR: Unexpected selection mode flag");
  //   messageVerbose("updateSelectionMode => SINGLEOBJECT");
  m_d->selObjects->policy = SoCooperativeSelection::SINGLE;
  //   m_d->ascObjSelManager->setMode(AscObjSelectionManager::SINGLE);
  // }
  messageVerbose("updateSelectionMode end");
}

//____________________________________________________________________
void VP1AODSystem::userPickedNode(SoNode* pickedNode, SoPath* /*pickedPath*/)
{
  messageVerbose("userPickedNode");
  if (pickedNode==m_d->totmomline) {
    message("Total momentum of selected tracks [GeV]: p = "+str(m_d->totmomgev)+", m = "+str(m_d->totmass/1000));
    return;
  }

  message("Unknown object clicked");
}

//____________________________________________________________________
void VP1AODSystem::userSelectedSingleNode( SoCooperativeSelection* sel, SoNode* node, SoPath* pickedPath )
{
  messageVerbose("userSelectedSingleNode");

  if (sel!=m_d->selObjects) {
    message("userSelectedSingleNode - ERROR, sel=!d->selObjects");
    return;
  }

  AODHandleBase* handle = m_d->common->getHandleFromNode(pickedPath);
  if (!handle) handle = m_d->common->getHandleFromNode(node);
  if (!handle) {
    message("ERROR: Unknown handle from path or node.");
  }

  if (handle) message(handle->clicked());

  // AODHandleBase* pickedHandle(0);
  // if (!m_d->ascObjSelManager->handleUserSelectedSingleNode(sel,node,pickedPath,pickedHandle)) {
  //   if (sel==m_d->selObjects) {
  // //Hack to get selections working when representing tracks with tubes:
  //     if (node->getTypeId().isDerivedFrom(SoCylinder::getClassTypeId())) {
  //       pickedPath->pop();
  //       node=pickedPath->getTail();
  //     }
  //     AODHandleBase * handle = m_d->common->getHandleFromNode(node);
  //     if (!handle) {
  //       message("ERROR: Unknown track.");
  //       return;
  //     }
  //     TrackHandle_TrkTrack * handle_trktrack = dynamic_cast<TrackHandle_TrkTrack *>(handle);
  //     if (handle_trktrack&&m_d->selMode==TrackCommonFlags::TRACKFIT) {
  //       messageVerbose("userSelectedSingleNode - find measurements for track fit");
  //
  //       QList<AODHandleBase*> trackmeas = handle_trktrack->getVisibleMeasurements();
  //
  //       if (trackmeas.size()==0) message("In refit mode, but no visible measurements found so can't do anything. Perhaps they're not enabled in 'Details'?");
  //       QList<AODHandleBase*> currentsel = m_d->ascObjSelManager->currentSelection();
  // //If at least one of the track measurements is unselected, we
  // //select them all. Otherwise we deselect them.
  //       bool oneunselected(false);
  //       foreach(AODHandleBase* meas,trackmeas) {
  //         if (!currentsel.contains(meas)) {
  //           oneunselected = true;
  //           break;
  //         }
  //       }
  //       QList<const Trk::PrepRawData*> prdSet;
  //       if (oneunselected) {
  //         messageVerbose("userSelectedSingleNode - selecting " +QString::number(trackmeas.size()) + " measurements.");
  //         m_d->ascObjSelManager->ensureSelected(trackmeas);
  //
  //         // Add PRDs. Need to be careful as they might not exist.
  //         foreach(AODHandleBase* meas,trackmeas) {
  //           AscObj_TSOS* tsosAsc = dynamic_cast<AscObj_TSOS*>(meas);
  //           if (tsosAsc && tsosAsc->rioOnTrack() && tsosAsc->rioOnTrack()->prepRawData()) prdSet.append(tsosAsc->rioOnTrack()->prepRawData());
  //         }
  //       } else {
  //         messageVerbose("userSelectedSingleNode - deselecting " +QString::number(trackmeas.size()) + " measurements.");
  //         m_d->ascObjSelManager->ensureDeselected(trackmeas);
  //       }
  //       setSelectedPRDs(prdSet); // FIXME - maybe I should append/remove from existing list?
  //
  //       m_d->selObjects->deselectAll();
  //     } else {
  //       if (m_d->common->controller()->printInfoOnSingleSelection()){
  //         message(handle->clicked());
  //         messageVerbose("Emitting newTrackSelected ");
  //         m_d->common->setLastSelectedHandle(handle);
  //         emit newTrackSelected(*handle);
  //         m_d->common->controller()->setNumberOfSelectedPRDsAndTracks(m_d->selectedPRDs.count(),1); // FIXME - we can do this more cleanly?
  //       }
  //     }
  //   } else {
  //     message("ERROR: Unknown selection.");
  //     return;
  //   }
  // }
  // if (m_d->common->controller()->orientAndZoomOnSingleSelection()) {
  //   if (!pickedHandle||!pickedHandle->initiatesOwnZooms()) {
  //     std::set<SoCamera*> cameras = getCameraList();
  //     std::set<SoCamera*>::iterator it,itE = cameras.end();
  //     for (it=cameras.begin();it!=itE;++it)
  //       VP1CameraHelper::animatedZoomToPath(*it,sel,pickedPath,2.0,1.0);
  //   }
  // }
}

//____________________________________________________________________
void VP1AODSystem::userClickedOnBgd()
{
  messageVerbose("userClickedOnBgd");
  // if (m_d->ascObjSelManager)
  //   m_d->ascObjSelManager->userClickedOnBgd();
  // m_d->common->setLastSelectedHandle(0);
}

//____________________________________________________________________
void VP1AODSystem::userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*)
{
  messageVerbose("userDeselectedSingleNode");
}

//____________________________________________________________________
unsigned VP1AODSystem::Imp::calcTotalMomentumOfSelectedHandles(Amg::Vector3D& /*totmom*/, Amg::Vector3D& /*totpos*/, double& /*mass*/)
{
  // TODO: to be implemented?
  
  
  

  // totmom = Amg::Vector3D(0,0,0);
  // totpos = Amg::Vector3D(0,0,0);
  // mass = 0;
  // if (!selObjects)
  //   return 0;
  // unsigned nused(0);
  // double totenergy(0);
  // for (int i = 0; i < selObjects->getList()->getLength(); ++i) {
  //   SoFullPath *fPath = static_cast<SoFullPath *>((*(selObjects->getList()))[i]);
  //   AODHandleBase * handle = common->trackHandle(fPath?fPath->getTail():0);
  //   if (!handle)
  //     continue;
  //   Amg::Vector3D mom = handle->momentum();
  //   if (mom.mag2()==0.0)
  //     continue;
  //   //Fixme: Get actual position of perigee!!
  //   const Amg::Vector3D * pos = handle->startPoint();
  //   if (!pos)
  //     continue;
  //   ++nused;
  //   totmom += mom;
  //   totpos += *pos;
  //   mass = handle->hasMass() ? handle->mass() : 0;
  //   totenergy += sqrt(mom.mag2() + mass*mass);
  // }
  // if (nused>1) {
  //   double msq = totenergy*totenergy - totmom.mag2();
  //   mass = (msq<0?-1.0:1.0)*sqrt(fabs(msq));
  //   totpos /= nused;
  // }
  // return nused;
  return 0;
}

SoCooperativeSelection * VP1AODSystem::selObjects() const{
  return m_d->selObjects;
}


//____________________________________________________________________
void VP1AODSystem::updateShownTotMomentum()
{
  // messageVerbose("updateShownTotMomentum");
  // if (!m_d->common->controller()->showTotMomentumOnMultiTrackSelection()) {
  //   //ensure detach:
  //   messageVerbose("  => detach");
  //   if (m_d->totmomsep&&m_d->totmomline&&m_d->totmomsep->findChild(m_d->totmomline)>-1)
  //     m_d->totmomsep->removeChild(m_d->totmomline);
  //   return;
  // }
  // Amg::Vector3D totmom;
  // Amg::Vector3D totpos;
  // double totmass;
  // unsigned nused = m_d->calcTotalMomentumOfSelectedHandles(totmom,totpos,totmass);
  // if (nused==0) {
  //   //ensure detach:
  //   messageVerbose("  => detach");
  //   if (m_d->totmomsep&&m_d->totmomline&&m_d->totmomsep->findChild(m_d->totmomline)>-1)
  //     m_d->totmomsep->removeChild(m_d->totmomline);
  // } else {
  //   //ensure correct lineset:
  //   Amg::Vector3D p2 = totpos+totmom.unit()*1*CLHEP::m;
  //   if (!m_d->totmomline) {
  //     m_d->totmomline = new SoLineSet;
  //     m_d->totmomline->ref();
  //     SoVertexProperty * vertices = new SoVertexProperty;
  //     m_d->totmomline->vertexProperty = vertices;
  //     m_d->totmomline->numVertices.set1Value(0,2);
  //
  //   }
  //   SoVertexProperty * vertices = static_cast<SoVertexProperty*>(m_d->totmomline->vertexProperty.getValue());
  //   vertices->vertex.set1Value(0,totpos.x(),totpos.y(),totpos.z());
  //   vertices->vertex.set1Value(1,p2.x(),p2.y(),p2.z());
  //   m_d->totmomgev = totmom / CLHEP::GeV;
  //   m_d->totmass = totmass;
  //   //ensure attach:
  //   messageVerbose("  => attach");
  //   if (m_d->totmomsep&&m_d->totmomline&&m_d->totmomsep->findChild(m_d->totmomline)<0)
  //     m_d->totmomsep->addChild(m_d->totmomline);
  //   return;
  // }
}

//____________________________________________________________________
void VP1AODSystem::userChangedSelection(SoCooperativeSelection* sel, QSet<SoNode*> /*nodes*/, QSet<SoPath*>/*paths*/)
{
  messageVerbose("userChangedSelection begin");
  if (sel!=m_d->selObjects)
    return;
  messageVerbose("userChangedSelection => selObjects!!");
  //
  //
  // if (m_d->common->controller()->printTotMomentumOnMultiTrackSelection()) {
  //   Amg::Vector3D totmom;
  //   Amg::Vector3D totpos;
  //   double totmass;
  //   if (m_d->calcTotalMomentumOfSelectedHandles(totmom,totpos,totmass)>0) {
  //     Amg::Vector3D totmomgev = totmom;
  //     totmomgev /= CLHEP::GeV;
  //     message("Total momentum [GeV] : "+str(totmomgev));//Fixme: Eta/phi/etc...
  //     message("Total mass [GeV] : "+str(totmass/CLHEP::GeV));//Fixme: Eta/phi/etc...
  //   }
  // }
  //
  // updateShownTotMomentum();
}

#ifndef BUILDVP1LIGHT
//____________________________________________________________________
void VP1AODSystem::updateAssociatedObjects(const QList<const xAOD::TrackParticle*>& trackparticles)
{
  messageVerbose("updateAssociatedObjects TrackParticle");
  std::cout<<"EJWM Got "<<trackparticles.size() << " from " <<typeid(sender()).name()<<std::endl;

  IParticleCollHandle_TrackParticle* newcoll = new  IParticleCollHandle_TrackParticle( m_d->common, "TrackParticlesFromMuons", xAOD::Type::Muon, false );

  std::cout<<"EJWM Adding handles "<<std::endl;
  for (auto tp : trackparticles){
    IParticleHandle_TrackParticle* newhandle = new IParticleHandle_TrackParticle(newcoll, tp);
    newcoll->addHandle( newhandle );
  }
  std::cout<<"EJWM init "<<std::endl;

  newcoll->init();
  // TrackCollHandle_RefittedTracks * newtrackcoll =
  // new TrackCollHandle_RefittedTracks(m_d->common,
  //            m_d->common->controller()->nameOfNewlyFittedCollections(),
  //            fittedtracks);

  QList<IParticleCollHandle_TrackParticle*> newcolls;
  newcolls << newcoll;

  std::cout<<"EJWM addCollections "<<std::endl;

  m_d->common->controller()->collWidget()->addCollections(newcolls);
  std::cout<<"EJWM addChild "<<std::endl;

  foreach (IParticleCollHandle_TrackParticle* col,newcolls) {
    m_d->selObjects->addChild(col->collSwitch());
    col->setVisible(true);
  }
  messageVerbose("updateAssociatedObjects TrackParticle end");

}
#endif // BUILDVP1LIGHT

// void VP1AODSystem::updateAssociatedObjects(QList<xAOD::CaloCluster*>& clusters)
// {
//   messageVerbose("updateAssociatedObjects CaloCluster");
//   std::cout<<"Got "<<clusters.size() << " from " <<typeid(sender()).name()<<std::endl;
//
// }
#ifndef BUILDVP1LIGHT
//____________________________________________________________________
void VP1AODSystem::updateAssociatedObjects(const QList<const xAOD::MuonSegment*>& segments)
{
  messageVerbose("updateAssociatedObjects Segment");
  std::cout<<"Got "<<segments.size() << " from " <<typeid(sender()).name()<<std::endl;

}
#endif // BUILDVP1LIGHT

//____________________________________________________________________
void VP1AODSystem::dumpToJSON()
{
  
  // Format should be as follows:
  // { "Type1Name" : { "Coll1name": {coll1data}, "Coll2name": {coll2data} } , "Type2Name" : { "Coll1name": {coll1data}, "Coll2name": {coll2data} } }
  // "Coll1name": {coll1data} is handled by the appropriate collHandle, called by ahandle->dumpToJSON(outfile);
  // All the outer dressing is done here i.e.
  // { "Type1Name" : { xxx, xxx }, "Type2Name" : { xxx, xxx } }
  
  std::ofstream outfile ("EventDump.json");
  if (outfile.is_open()){
    outfile << "{";
    outfile<< "\"event number\":234, \"run number\":234,\n"; //FIXME - make this into a metadata section?
    
    xAOD::Type::ObjectType lastType = xAOD::Type::EventInfo; // Set to this value because nothing that follows should use it i.e. can distinguish first loop.
    
    for (auto col : m_d->common->controller()->collWidget()->visibleStdCollections() ){
      AODCollHandleBase* ahandle = dynamic_cast<AODCollHandleBase*>(col);
      if (!ahandle){
        message("WAAAAH! Not an AODCollHandleBase! Should never happen!");
        continue;
      }
      
      //unsigned int collNum=0; // Count of collection numbers
      // std::cout<<"collNum="<<collNum++<<std::endl;
      
      if (ahandle->xAODType()!=lastType) {
        // std::cout<<"Last type = "<<lastType<<", new type="<<ahandle->xAODType()<<std::endl;
        // New type, so Terminate the 'type' enclosure add a new level of JSON enclosure. 
        if (lastType != xAOD::Type::EventInfo) {
          // std::cout<<"Not first loop, so do add },"<<std::endl;
          outfile << "},\n"; // This is the }, marked *},* : { "Type1Name" : { xxx, xxx } , "Type2Name" : { xxx, xxx } *},*. Don't do this on the first loop!
        } 
        lastType=ahandle->xAODType();
        
        // Format is "TYPE":{ coll1, coll2, coll3 }
        // Here we do just "TYPE":{
        outfile << "\""<<ahandle->xAODType()<<"\" : {";
        // std::cout<<"Dumping: "<< "\""<<ahandle->xAODType()<<"\" : {"<<std::endl;
      } else {
        // std::cout<<"Not the first loop, so add comma"<<std::endl;
        // This is the comma marked *,*: { "Type1Name" : { xxx, xxx } *,* "Type2Name" : { xxx, xxx } }. Don't do this on the first loop!
        outfile <<","; 
      }
      // Dump collection
      ahandle->dumpToJSON(outfile);
      
    } 
  
    outfile << "}\n}\n"; // First closes last TYPE, second closes overall object
  } else {
    message("WAAAAH! Problem opening EventDump.json for output!");
  }
  message("Wrote visible objects to \'EventDump.json\'");
}
