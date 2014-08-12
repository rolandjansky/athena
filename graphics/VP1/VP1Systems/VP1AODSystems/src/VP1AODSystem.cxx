/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "IParticleCollHandle_TrackParticle.h"
#include "IParticleCollHandle_Jet.h"
#include "IParticleCollHandle_Muon.h"
#include "AODSysCommonData.h"
#include "IParticleCollWidget.h"

//VP1
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VisibleObjectToMaterialHelper.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Utils/VP1DetInfo.h"

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

// Athena
#include "StoreGate/StoreGateSvc.h"

// Qt
#include <QTreeWidget>

class VP1AODSystem::Imp {
public:
  VP1AODSystem * theclass;
  AODSysCommonData * common;
  SoCooperativeSelection * selObjects;
  
  SoSeparator * totmomsep;
  SoLineSet * totmomline;
  Amg::Vector3D totmomgev;
  double totmass;

  //Return value is number of track handles used for input (e.g. those without valid momentum info are ignored)
  unsigned calcTotalMomentumOfSelectedHandles(Amg::Vector3D& totmom, Amg::Vector3D& totpos, double& mass);

  template <class T>
  QList<IParticleCollHandleBase*> createSpecificCollections(xAOD::Type::ObjectType type) {
    QList<IParticleCollHandleBase*> l;
    foreach (QString name, T::availableCollections(theclass)) {
      T * col = new T(common,name,type);
      col->init();
      l << col;
    }
    return l;
  }

  /*
   * here we load the different systems (Muon, Jet, TrackParticle, ...)
   */
  QList<IParticleCollHandleBase*> createCollections() {
    QList<IParticleCollHandleBase*> l;
    l << createSpecificCollections<IParticleCollHandle_TrackParticle>(xAOD::Type::TrackParticle);
    l << createSpecificCollections<IParticleCollHandle_Jet>(xAOD::Type::Jet);
    l << createSpecificCollections<IParticleCollHandle_Muon>(xAOD::Type::Muon);
    return l;
  }
};

//____________________________________________________________________
VP1AODSystem::VP1AODSystem(QString name)
  : IVP13DSystemSimple(name,
"System showing all track-like objects.",
"Edward.Moyse@cern.ch, Riccardo.maria.bianchi@cern.ch"), d(new Imp)
{
  d->theclass = this;
  d->selObjects = 0;
  d->common = 0;
  d->totmomsep = 0;
  d->totmomline = 0;
  d->totmomgev = Amg::Vector3D(0,0,0);
}

//____________________________________________________________________
VP1AODSystem::~VP1AODSystem()
{
  delete d;
}

//____________________________________________________________________
void VP1AODSystem::systemcreate(StoreGateSvc* /*detstore*/)
{
  messageVerbose("systemcreate");
  ensureBuildController();
  // d->common->controller()->initTools();
}

//____________________________________________________________________
void VP1AODSystem::systemuncreate()
{
  messageVerbose("systemuncreate");
  if (d->totmomsep) {
    d->totmomsep->unref();
    d->totmomsep=0;
  }
  if (d->totmomline) {
    d->totmomline->unref();
    d->totmomline = 0;
  }
  delete d->common; d->common = 0;
}

//____________________________________________________________________
QWidget * VP1AODSystem::buildController()
{
  messageVerbose("buildController start");
  AODSystemController * controller = new AODSystemController(this);
  d->common = new AODSysCommonData(this,controller);//Fixme: Do this on-demand in buildeventscenegraph!!
  controller->setCommonData(d->common);

  // connect(controller,SIGNAL(selectionModeChanged(TrackCommonFlags::SELECTIONMODE)),this,SLOT(updateSelectionMode()));
  // updateSelectionMode();
  // connect(controller,SIGNAL(showTotMomentumOnMultiTrackSelectionChanged(bool)),this,SLOT(updateShownTotMomentum()));
  // updateShownTotMomentum();

  messageVerbose("buildController end");
  return controller;
}

//____________________________________________________________________
void VP1AODSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  messageVerbose("VP1AODSystem::buildEventSceneGraph start");

  // set complexity to a lower value, so that e.g. the straws are manageable
  SoComplexity * complexity = new SoComplexity;
  complexity->value.setValue(0.9f);//Fixme: Hardcoded here and elsewhere (fixme: Recheck all complexity values!)
  root->addChild(complexity);
  // root->addChild(d->common->controller()->ascObjDrawStyle());
  // root->addChild(d->common->controller()->ascObjComplexity());//this will be inherited to the tracks
  //                                                             // (but giving no effect)

  d->selObjects = new SoCooperativeSelection;
  d->selObjects->activePolicy = SoCooperativeSelection::ACTIVE;
  d->selObjects->ref();
  registerSelectionNode(d->selObjects);

  // reset last selected trk
  d->common->setLastSelectedTrack(0);
  updateSelectionMode();
  
  if (!d->common->m_textSep) {
    // FIXME!
    //    std::cout<<"Making new Text sep"<<std::endl;
    d->common->m_textSep = new SoSeparator;
    d->common->m_textSep->setName("TextSep");
    d->common->m_textSep->ref();
  }
  root->addChild(d->common->m_textSep);
  
  // Fixme - what if font is missing?
  SoFont *myFont = new SoFont;
  myFont->name.setValue("Arial");
  myFont->size.setValue(13.0);
  d->common->m_textSep->addChild(myFont);

  messageVerbose("createCollections start");
  
  //Create collection list based on contents of event store, populate gui and apply states:
  d->common->controller()->collWidget()->setCollections(d->createCollections());

  //Add collections to event scenegraph:
  foreach (VP1StdCollection* col,d->common->controller()->collWidget()->collections<VP1StdCollection>()){
    std::cout<<"Add collswitch="<<col->collSwitch()<< " to EventSceneGraph"<<std::endl;
    d->selObjects->addChild(col->collSwitch());
  }

  root->addChild(d->selObjects);
  if (!d->totmomsep) {
    d->totmomsep = new SoSeparator;
    d->totmomsep->ref();
  }
  // d->totmomsep->addChild(d->common->controller()->trackDrawStyle());
  root->addChild(d->totmomsep);

  messageVerbose("buildEventSceneGraph end");
}


//____________________________________________________________________
void VP1AODSystem::systemerase()
{
  messageVerbose("systemErase begin");

  d->common->controller()->collWidget()->clear();
  if (d->common->controller()->objBrowser()) d->common->controller()->objBrowser()->clear();

  // d->common->clearEventData();
  if (d->selObjects) {
    unregisterSelectionNode(d->selObjects);
    d->selObjects->unref();
    d->selObjects=0;
  }

  if (d->totmomsep)
    d->totmomsep->removeAllChildren();
    
  if (d->common->m_textSep) // FIXME!
    d->common->m_textSep->removeAllChildren();
    
  d->totmomgev = Amg::Vector3D(0,0,0);

  // if (verbose()) {
  //   if (AssociatedObjectHandleBase::numberOfInstances()!=0)
  //     message("Warning: "+str(AssociatedObjectHandleBase::numberOfInstances())
  //       +" instances of AssociatedObjectHandleBase alive at end of systemerase(). "
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
  //Version & base state:
  VP1Serialise serialise(1/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());//Info from base class

  // Actual state info:
  ensureBuildController();
  serialise.save(d->common->controller()->saveSettings());
  serialise.save((VP1CollectionWidget*)d->common->controller()->collWidget());

  serialise.disableUnsavedChecks();//We do the testing in the controller

  return serialise.result();
}

//____________________________________________________________________
void VP1AODSystem::restoreFromState(QByteArray ba)
{
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

  d->common->controller()->restoreSettings(state.restoreByteArray());
  state.restore((VP1CollectionWidget*)d->common->controller()->collWidget());

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
  // if (!d->selObjects||!d->ascObjSelManager) {
  //   messageVerbose("updateSelectionMode Warning: Ignoring due to null pointers.");
  //   return;
  // }
  // deselectAll();
  // d->ascObjSelManager->deselectAll();
  // d->selMode = d->common->controller()->selectionMode();//NB: Don't abort if unchanged (we call this method to init)
  // if (d->selMode==TrackCommonFlags::TRACKFIT) {
  //   messageVerbose("updateSelectionMode => TRACKFIT");
  //   d->selObjects->policy = SoCooperativeSelection::SINGLE;
  //   d->ascObjSelManager->setMode(AscObjSelectionManager::SHIFT);
  // } else if (d->selMode==TrackCommonFlags::MULTITRACK) {
  //   messageVerbose("updateSelectionMode => MULTITRACK");
  //   d->selObjects->policy = SoCooperativeSelection::TOGGLE;
  //   d->ascObjSelManager->setMode(AscObjSelectionManager::SINGLE);
  // } else {
  //   if (d->selMode!=TrackCommonFlags::SINGLEOBJECT)
  //     message("updateSelectionMode ERROR: Unexpected selection mode flag");
  //   messageVerbose("updateSelectionMode => SINGLEOBJECT");
  //   d->selObjects->policy = SoCooperativeSelection::SINGLE;
  //   d->ascObjSelManager->setMode(AscObjSelectionManager::SINGLE);
  // }
  messageVerbose("updateSelectionMode end");
}

//____________________________________________________________________
void VP1AODSystem::userPickedNode(SoNode* pickedNode, SoPath* /*pickedPath*/)
{
  messageVerbose("userPickedNode");
  if (pickedNode==d->totmomline) {
    message("Total momentum of selected tracks [GeV]: p = "+str(d->totmomgev)+", m = "+str(d->totmass/1000));
    return;
  }

  message("Unknown object clicked");
}

//____________________________________________________________________
void VP1AODSystem::userSelectedSingleNode( SoCooperativeSelection* sel, SoNode* node, SoPath* pickedPath )
{
  messageVerbose("userSelectedSingleNode");
  // AssociatedObjectHandleBase* pickedHandle(0);
  // if (!d->ascObjSelManager->handleUserSelectedSingleNode(sel,node,pickedPath,pickedHandle)) {
  //   if (sel==d->selObjects) {
  // //Hack to get selections working when representing tracks with tubes:
  //     if (node->getTypeId().isDerivedFrom(SoCylinder::getClassTypeId())) {
  //       pickedPath->pop();
  //       node=pickedPath->getTail();
  //     }
  //     AODHandleBase * handle = d->common->trackHandle(node);
  //     if (!handle) {
  //       message("ERROR: Unknown track.");
  //       return;
  //     }
  //     TrackHandle_TrkTrack * handle_trktrack = dynamic_cast<TrackHandle_TrkTrack *>(handle);
  //     if (handle_trktrack&&d->selMode==TrackCommonFlags::TRACKFIT) {
  //       messageVerbose("userSelectedSingleNode - find measurements for track fit");
  //       
  //       QList<AssociatedObjectHandleBase*> trackmeas = handle_trktrack->getVisibleMeasurements();
  //       
  //       if (trackmeas.size()==0) message("In refit mode, but no visible measurements found so can't do anything. Perhaps they're not enabled in 'Details'?");
  //       QList<AssociatedObjectHandleBase*> currentsel = d->ascObjSelManager->currentSelection();
  // //If at least one of the track measurements is unselected, we
  // //select them all. Otherwise we deselect them.
  //       bool oneunselected(false);
  //       foreach(AssociatedObjectHandleBase* meas,trackmeas) {
  //         if (!currentsel.contains(meas)) {
  //           oneunselected = true;
  //           break;
  //         }
  //       }
  //       QList<const Trk::PrepRawData*> prdSet;
  //       if (oneunselected) {
  //         messageVerbose("userSelectedSingleNode - selecting " +QString::number(trackmeas.size()) + " measurements.");
  //         d->ascObjSelManager->ensureSelected(trackmeas);
  //         
  //         // Add PRDs. Need to be careful as they might not exist.
  //         foreach(AssociatedObjectHandleBase* meas,trackmeas) {
  //           AscObj_TSOS* tsosAsc = dynamic_cast<AscObj_TSOS*>(meas);
  //           if (tsosAsc && tsosAsc->rioOnTrack() && tsosAsc->rioOnTrack()->prepRawData()) prdSet.append(tsosAsc->rioOnTrack()->prepRawData());
  //         }
  //       } else {
  //         messageVerbose("userSelectedSingleNode - deselecting " +QString::number(trackmeas.size()) + " measurements.");
  //         d->ascObjSelManager->ensureDeselected(trackmeas);
  //       }
  //       setSelectedPRDs(prdSet); // FIXME - maybe I should append/remove from existing list?
  //       
  //       d->selObjects->deselectAll();
  //     } else {
  //       if (d->common->controller()->printInfoOnSingleSelection()){
  //         message(handle->clicked());
  //         messageVerbose("Emitting newTrackSelected ");
  //         d->common->setLastSelectedTrack(handle);
  //         emit newTrackSelected(*handle);
  //         d->common->controller()->setNumberOfSelectedPRDsAndTracks(d->selectedPRDs.count(),1); // FIXME - we can do this more cleanly?
  //       }
  //     }
  //   } else {
  //     message("ERROR: Unknown selection.");
  //     return;
  //   }
  // }
  // if (d->common->controller()->orientAndZoomOnSingleSelection()) {
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
  // if (d->ascObjSelManager)
  //   d->ascObjSelManager->userClickedOnBgd();
  // d->common->setLastSelectedTrack(0);
}

//____________________________________________________________________
void VP1AODSystem::userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*)
{
  messageVerbose("userDeselectedSingleNode");
}

//____________________________________________________________________
unsigned VP1AODSystem::Imp::calcTotalMomentumOfSelectedHandles(Amg::Vector3D& totmom, Amg::Vector3D& totpos, double& mass)
{
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
}


//____________________________________________________________________
void VP1AODSystem::updateShownTotMomentum()
{
  // messageVerbose("updateShownTotMomentum");
  // if (!d->common->controller()->showTotMomentumOnMultiTrackSelection()) {
  //   //ensure detach:
  //   messageVerbose("  => detach");
  //   if (d->totmomsep&&d->totmomline&&d->totmomsep->findChild(d->totmomline)>-1)
  //     d->totmomsep->removeChild(d->totmomline);
  //   return;
  // }
  // Amg::Vector3D totmom;
  // Amg::Vector3D totpos;
  // double totmass;
  // unsigned nused = d->calcTotalMomentumOfSelectedHandles(totmom,totpos,totmass);
  // if (nused==0) {
  //   //ensure detach:
  //   messageVerbose("  => detach");
  //   if (d->totmomsep&&d->totmomline&&d->totmomsep->findChild(d->totmomline)>-1)
  //     d->totmomsep->removeChild(d->totmomline);
  // } else {
  //   //ensure correct lineset:
  //   Amg::Vector3D p2 = totpos+totmom.unit()*1*CLHEP::m;
  //   if (!d->totmomline) {
  //     d->totmomline = new SoLineSet;
  //     d->totmomline->ref();
  //     SoVertexProperty * vertices = new SoVertexProperty;
  //     d->totmomline->vertexProperty = vertices;
  //     d->totmomline->numVertices.set1Value(0,2);
  // 
  //   }
  //   SoVertexProperty * vertices = static_cast<SoVertexProperty*>(d->totmomline->vertexProperty.getValue());
  //   vertices->vertex.set1Value(0,totpos.x(),totpos.y(),totpos.z());
  //   vertices->vertex.set1Value(1,p2.x(),p2.y(),p2.z());
  //   d->totmomgev = totmom / CLHEP::GeV;
  //   d->totmass = totmass;
  //   //ensure attach:
  //   messageVerbose("  => attach");
  //   if (d->totmomsep&&d->totmomline&&d->totmomsep->findChild(d->totmomline)<0)
  //     d->totmomsep->addChild(d->totmomline);
  //   return;
  // }
}

//____________________________________________________________________
void VP1AODSystem::userChangedSelection(SoCooperativeSelection* sel, QSet<SoNode*> /*nodes*/, QSet<SoPath*>/*paths*/)
{
  messageVerbose("userChangedSelection begin");
  // if (sel!=d->selObjects)
  //   return;
  // messageVerbose("userChangedSelection => selObjects!!");
  // 
  // 
  // if (d->common->controller()->printTotMomentumOnMultiTrackSelection()) {
  //   Amg::Vector3D totmom;
  //   Amg::Vector3D totpos;
  //   double totmass;
  //   if (d->calcTotalMomentumOfSelectedHandles(totmom,totpos,totmass)>0) {
  //     Amg::Vector3D totmomgev = totmom;
  //     totmomgev /= CLHEP::GeV;
  //     message("Total momentum [GeV] : "+str(totmomgev));//Fixme: Eta/phi/etc...
  //     message("Total mass [GeV] : "+str(totmass/CLHEP::GeV));//Fixme: Eta/phi/etc...
  //   }
  // }
  // 
  // updateShownTotMomentum();
}






