/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1TrackSystem                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

//TODO: Features in old system that are missing now (+ stuff really needed):
//
//  --> short term:
//        colour by clicked vertex
//        check inner detector projections
//
//  --> medium term:
//        refit tracks
//
//  --> longer term:
//        object browser
//        sdo's
//
//  soonish: detail levels depending on meas. type, and colour-by
//  depending on collection.

#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/TrackHandleBase.h"
#include "VP1TrackSystems/TrackLODManager.h"
#include "VP1TrackSystems/TrackCollWidget.h"
#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/AssociatedObjectHandleBase.h"
#include "VP1TrackSystems/AscObj_TSOS.h"
#include "VP1TrackSystems/TrackSysCommonData.h"

#include "VP1TrackSystems/TrackCollHandle_TrkTrack.h"
#include "VP1TrackSystems/TrackCollHandle_TrkSegment.h"
// #include "VP1TrackSystems/TrackCollHandle_FatrasTruthTracks.h"
#include "VP1TrackSystems/TrackCollHandle_TruthTracks.h"
#include "VP1TrackSystems/TrackCollHandle_SimulationTracks.h"
#include "VP1TrackSystems/TrackCollHandle_TrackParticle.h"
#include "VP1TrackSystems/TrackCollHandle_xAODTrackParticle.h"
// #include "VP1TrackSystems/TrackCollHandle_Muon.h"
// #include "VP1TrackSystems/TrackCollHandle_Egamma.h"
#include "VP1TrackSystems/TrackHandle_TrkTrack.h"
#include "VP1TrackSystems/TrackCollHandle_RefittedTracks.h"

#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VisibleObjectToMaterialHelper.h"
#include "VP1Base/SoCooperativeSelection.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "VP1Utils/VP1DetInfo.h"

#include "VP1GuideLineSystems/InDetProjHelper.h"

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

//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/GlobalMomentum.h"
#include "TrkEventPrimitives/FitQuality.h"
#include <sstream>

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonChamberT0s/ChamberT0s.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "StoreGate/StoreGateSvc.h"

////////////////////////////////////////////////////////
//FIXME: ONLY HERE TEMPORARILY. WE NEED A HELPER CLASS!!
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "VP1Utils/VP1ToolAccessHelper.h"
#include "VP1Utils/VP1AvailableToolsHelper.h"
////////////////////////////////////////////////////////


// FOR DEBUG
//#include "MuonIdHelpers/MdtIdHelper.h"
//#include "MuonIdHelpers/MuonIdHelper.h"



//____________________________________________________________________
class VP1TrackSystem::Imp {
public:
  VP1TrackSystem * theclass;
  TrackSysCommonData * common;
  SoCooperativeSelection * sel_tracks;
  AscObjSelectionManager * ascObjSelManager;
  TrackCommonFlags::SELECTIONMODE selMode;

  InDetProjFlags::DetTypeFlags lastEmittedUsedIDProjections;
  InDetProjFlags::DetTypeFlags currentUsedIDProjections() const;

  SoSeparator * totmomsep;
  SoLineSet * totmomline;
  Amg::Vector3D totmomgev;
  double totmass;

  QList<const Trk::PrepRawData*> selectedPRDs;

  //Return value is number of track handles used for input (e.g. those without valid momentum info are ignored)
  unsigned calcTotalMomentumOfSelectedHandles(Amg::Vector3D& totmom, Amg::Vector3D& totpos, double& mass);

  template <class T>
  QList<TrackCollHandleBase*> createSpecificCollections() {
    QList<TrackCollHandleBase*> l;
    foreach (QString name, T::availableCollections(theclass)) {
      T * col = new T(common,name);
      col->init();
      l << col;
    }
    return l;
  }
  QList<TrackCollHandleBase*> createCollections() {
    QList<TrackCollHandleBase*> l;
    l << createSpecificCollections<TrackCollHandle_TrkTrack>();
    l << createSpecificCollections<TrackCollHandle_TrkSegment>();
    // theclass->messageVerbose("Creating coll handle for TrackCollHandle_FatrasTruthTracks");
    // l << createSpecificCollections<TrackCollHandle_FatrasTruthTracks>();
    // theclass->messageVerbose("Creating coll handle for TrackCollHandle_TruthTracks");
    l << createSpecificCollections<TrackCollHandle_TruthTracks>();
    l << createSpecificCollections<TrackCollHandle_SimulationTracks>();
    // theclass->messageVerbose("Creating coll handle for TrackCollHandle_TrackParticle");
    l << createSpecificCollections<TrackCollHandle_TrackParticle>();
    l << createSpecificCollections<TrackCollHandle_xAODTrackParticle>();
    // theclass->messageVerbose("Creating coll handle for Muons");
    // l << createSpecificCollections<TrackCollHandle_Muon>();
    // l << createSpecificCollections<TrackCollHandle_Egamma>();
    
    return l;
  }
  std::vector<std::map<GeoPVConstLink, float> > chamberT0s;
  std::map<const Trk::Track*, SoMaterial*> vertexMaterialForTrackBases;
};

//____________________________________________________________________
VP1TrackSystem::VP1TrackSystem(QString name)
  : IVP13DSystemSimple(name,
           "System showing all track-like objects.",
           "Edward.Moyse@cern.ch, Thomas.Kittelmann@cern.ch"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->sel_tracks = 0;
  m_d->common = 0;
  m_d->totmomsep = 0;
  m_d->totmomline = 0;
  m_d->totmomgev = Amg::Vector3D(0,0,0);
  m_d->ascObjSelManager = 0;
  m_d->selMode = TrackCommonFlags::SINGLEOBJECT;
  m_d->lastEmittedUsedIDProjections = InDetProjFlags::NoDet;
  const unsigned n_chamber_t0_sources=2;
  m_d->chamberT0s.resize(n_chamber_t0_sources);
}

//____________________________________________________________________
VP1TrackSystem::~VP1TrackSystem()
{
  delete m_d;
}

//____________________________________________________________________
void VP1TrackSystem::systemcreate(StoreGateSvc* /*detstore*/)
{
  messageVerbose("systemcreate");
  ensureBuildController();
  m_d->common->controller()->initTools();

  connect(m_d->common->controller(),SIGNAL(shownTrackPartsChanged(TrackCommonFlags::TrackPartsFlags)),
    this,SLOT(possiblyEmitUsedIDProjectionsChanged()));
  connect(m_d->common->controller()->collWidget(),SIGNAL(visibleStdCollectionsChanged(const QList<VP1StdCollection*>&)),
    this,SLOT(possiblyEmitUsedIDProjectionsChanged()));
  possiblyEmitUsedIDProjectionsChanged();

}

//____________________________________________________________________
void VP1TrackSystem::systemuncreate()
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
QWidget * VP1TrackSystem::buildController()
{
  messageVerbose("buildController start");
  TrackSystemController * controller = new TrackSystemController(this);
  m_d->common = new TrackSysCommonData(this,controller);//Fixme: Do this on-demand in buildeventscenegraph!!
  controller->setCommonData(m_d->common);

  connect(controller,SIGNAL(selectionModeChanged(TrackCommonFlags::SELECTIONMODE)),this,SLOT(updateSelectionMode()));
  updateSelectionMode();
  connect(controller,SIGNAL(showTotMomentumOnMultiTrackSelectionChanged(bool)),this,SLOT(updateShownTotMomentum()));
  updateShownTotMomentum();

  connect(controller,SIGNAL(refit()),this,SLOT(refit()));//temp.

  controller->setNumberOfSelectedPRDsAndTracks(m_d->selectedPRDs.count(),0);

  messageVerbose("buildController end");
  return controller;
}

//____________________________________________________________________
void VP1TrackSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  messageVerbose("buildEventSceneGraph start2");

  // set complexity to a lower value, so that e.g. the straws are manageable
  SoComplexity * complexity = new SoComplexity;
  complexity->value.setValue(0.3f);//Fixme: Hardcoded here and elsewhere (fixme: Recheck all complexity values!)
  root->addChild(complexity);
  root->addChild(m_d->common->controller()->ascObjDrawStyle());
  root->addChild(m_d->common->controller()->ascObjComplexity());//this will be inherited to the tracks
                                                              // (but giving no effect)

  m_d->sel_tracks = new SoCooperativeSelection;
  m_d->sel_tracks->activePolicy = SoCooperativeSelection::ACTIVE;
  m_d->sel_tracks->ref();
  registerSelectionNode(m_d->sel_tracks);
  // m_d->sel_tracks->addChild(m_d->common->controller()->trackLightModel());
  // m_d->sel_tracks->addChild(m_d->common->controller()->trackDrawStyle());

  m_d->ascObjSelManager = new AscObjSelectionManager (root,this,m_d->common->controller());
  m_d->common->setEventData(m_d->ascObjSelManager);//commondata assumes ownership of ascObjMgr.
  m_d->common->trackLODManager()->setAttachNode(m_d->ascObjSelManager->getAscObjAttachSep());

// reset last selected trk
  m_d->common->setLastSelectedTrack(0);
  updateSelectionMode();
  
  if (!m_d->common->m_textSep) {
    // FIXME!
    //    std::cout<<"Making new Text sep"<<std::endl;
    m_d->common->m_textSep = new SoSeparator;
    m_d->common->m_textSep->setName("TextSep");
    m_d->common->m_textSep->ref();
  }
  root->addChild(m_d->common->m_textSep);
  
  // Fixme - what if font is missing?
  SoFont *myFont = new SoFont;
  myFont->name.setValue("Arial");
  myFont->size.setValue(12.0);
  m_d->common->m_textSep->addChild(myFont);

  messageVerbose("createCollections start");
  
  //Create collection list based on contents of event store, populate gui and apply states:
  m_d->common->controller()->collWidget()->setCollections(m_d->createCollections());

  //Add collections to event scenegraph:
  foreach (VP1StdCollection* col,m_d->common->controller()->collWidget()->collections<VP1StdCollection>())
    m_d->sel_tracks->addChild(col->collSwitch());

  root->addChild(m_d->sel_tracks);
  if (!m_d->totmomsep) {
    m_d->totmomsep = new SoSeparator;
    m_d->totmomsep->ref();
  }
  // m_d->totmomsep->addChild(m_d->common->controller()->trackDrawStyle());
  root->addChild(m_d->totmomsep);
  
  //
  // Loading T0 measurements of muon chambers
  //
  // This will loop over all the T0 measurements found for muon chambers.
  // And it will store them into the 'm_d->chamberT0s' vector.
  // TODO! But do we still need it here?? Allow this to be configured?
  //
  std::vector<std::string> key;
  key.push_back("MooreMuonChamberT0s");
  key.push_back("MboyMuonChamberT0s");
  assert(m_d->chamberT0s.size()==key.size());

  for (unsigned int i=0; i<m_d->chamberT0s.size(); i++){
    assert(i<key.size());
    m_d->chamberT0s.at(i).clear();
    const Muon::ChamberT0s* chamberT0s(0);
    const MuonGM::MuonDetectorManager * muonDetManager = VP1DetInfo::muonDetMgr();

    bool isThere = sg->contains<Muon::ChamberT0s>(key[i]);
    if (isThere && sg->retrieve(chamberT0s, key[i]).isFailure())
      message("Problems loading ChamberT0s!");
    else
      if (!isThere) messageVerbose("No " +QString(key[i].c_str())+ " in event");

    if (chamberT0s ){
      messageVerbose("Found " +QString(key[i].c_str())+ " Chamber T0s");

      std::vector< std::pair< Identifier, float > >::const_iterator it, itEnd=chamberT0s->getAllT0s().end();
      for (it=chamberT0s->getAllT0s().begin(); it!=itEnd; it++){

    	  // chamber name
    	  std::string stationName( muonDetManager->mdtIdHelper()->stationNameString (muonDetManager->mdtIdHelper()->stationName( it->first )) );
    	  messageVerbose("station="+QString(stationName.c_str()));


    	  if (muonDetManager->mdtIdHelper()->is_mdt(it->first)){
    		  messageDebug("---> getting MDT T0s");

    		  std::string techStr( muonDetManager->mdtIdHelper()->technologyString(muonDetManager->mdtIdHelper()->technology(it->first)) );
    		  //int stName = muonDetManager->mdtIdHelper()->stationName(it->first);
    		  float stEta  = muonDetManager->mdtIdHelper()->stationEta(it->first);
    		  float stPhi  = muonDetManager->mdtIdHelper()->stationPhi(it->first);
    		  int ml     = muonDetManager->mdtIdHelper()->multilayer(it->first);
    		  //std::cout << "MDT chamber Info - Identifier: " << it->first << " - technology: " <<techStr <<   " - stName: " << stName << " - stEta: " << stEta << " - stPhi: " << stPhi << " - ml: " << ml << std::endl;
    		  messageDebug( "MDT chamber Info - technology: " + QString::fromStdString(techStr) + " - stName: " +  QString::fromStdString(stationName) + " - stEta: " + QString::number(stEta) + " - stPhi: " + QString::number(stPhi) + " - ml: " + QString::number(ml) );

    		  const MuonGM::MuonReadoutElement* muonDetEl = dynamic_cast<const MuonGM::MuonReadoutElement*>(muonDetManager->getMdtReadoutElement (it->first));
    		  if (muonDetEl) m_d->chamberT0s[i][muonDetEl->parentStationPV()]=it->second;
    	  }
    	  else if (muonDetManager->cscIdHelper()->is_csc(it->first)){
    		  messageDebug("---> getting CSC T0s");

    		  std::string techStr( muonDetManager->cscIdHelper()->technologyString(muonDetManager->cscIdHelper()->technology(it->first)) );
    		  //int stName = muonDetManager->cscIdHelper()->stationName(it->first);
    		  float stEta  = muonDetManager->cscIdHelper()->stationEta(it->first);
    		  float stPhi  = muonDetManager->cscIdHelper()->stationPhi(it->first);
    		  //std::cout << "CSC chamber Info - Identifier: " << it->first << " - technology: " <<techStr <<   " - stName: " << stationName << " - stEta: " << stEta << " - stPhi: " << stPhi << std::endl;
    		  messageDebug( "MDT chamber Info - technology: " + QString::fromStdString(techStr) + " - stName: " +  QString::fromStdString(stationName) + " - stEta: " + QString::number(stEta) + " - stPhi: " + QString::number(stPhi) );

    		  const MuonGM::CscReadoutElement* muonDetEl = dynamic_cast<const MuonGM::CscReadoutElement*>(muonDetManager->getCscReadoutElement (it->first));
    		  if (muonDetEl) m_d->chamberT0s[i][muonDetEl->parentStationPV()]=it->second;
    	  }
      }
      emit muonChamberT0sChanged(m_d->chamberT0s[i],i);
    }
  }

  messageVerbose("buildEventSceneGraph end");
}


//____________________________________________________________________
void VP1TrackSystem::systemerase()
{
  messageVerbose("systemErase begin");

  m_d->common->controller()->collWidget()->clear();
  messageVerbose("collWidget cleared");

  if (m_d->common->controller()->trackObjBrowser()) m_d->common->controller()->trackObjBrowser()->clear();

  m_d->common->clearEventData();
  if (m_d->sel_tracks) {
    unregisterSelectionNode(m_d->sel_tracks);
    m_d->sel_tracks->unref();
    m_d->sel_tracks=0;
  }

  if (m_d->totmomsep)
    m_d->totmomsep->removeAllChildren();
    
  if (m_d->common->m_textSep) // FIXME!
    m_d->common->m_textSep->removeAllChildren();
    
  m_d->totmomgev = Amg::Vector3D(0,0,0);

  if (verbose()) {
    if (AssociatedObjectHandleBase::numberOfInstances()!=0)
      message("Warning: "+str(AssociatedObjectHandleBase::numberOfInstances())
        +" instances of AssociatedObjectHandleBase alive at end of systemerase(). "
        "(ignore this warning if there is more than one track system instance).");
    if (TrackHandleBase::numberOfInstances()!=0)
      message("Warning: "+str(TrackHandleBase::numberOfInstances())
        +" instances of TrackHandleBase alive at end of systemerase(). "
        "(ignore this warning if there is more than one track system instance).");
  }
  
  std::map<const Trk::Track*, SoMaterial*>::iterator itMat = m_d->vertexMaterialForTrackBases.begin();
  std::map<const Trk::Track*, SoMaterial*>::iterator itMatEnd = m_d->vertexMaterialForTrackBases.end();
  for(;itMat!=itMatEnd;++itMat) {
    SoMaterial* curmat = itMat->second;
    curmat->unref();
  }
  m_d->vertexMaterialForTrackBases.clear();
  for (unsigned int i=0;i<m_d->chamberT0s.size();++i)
    m_d->chamberT0s.at(i).clear();

  messageVerbose("systemErase end");
}

//____________________________________________________________________
QByteArray VP1TrackSystem::saveState()
{
  //Version & base state:
  VP1Serialise serialise(1/*version*/,this);
  serialise.save(IVP13DSystemSimple::saveState());//Info from base class

  // Actual state info:
  ensureBuildController();
  serialise.save(m_d->common->controller()->saveSettings());
  serialise.save((VP1CollectionWidget*)m_d->common->controller()->collWidget());

  serialise.disableUnsavedChecks();//We do the testing in the controller

  return serialise.result();
}

//____________________________________________________________________
void VP1TrackSystem::restoreFromState(QByteArray ba)
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

  m_d->common->controller()->restoreSettings(state.restoreByteArray());
  state.restore((VP1CollectionWidget*)m_d->common->controller()->collWidget());

  state.disableUnrestoredChecks();//We do the testing in the controller
}

//____________________________________________________________________
void VP1TrackSystem::emitTouchedMuonChambersChanged(const std::set<GeoPVConstLink>& s)
{
  messageVerbose("Emitting touchedMuonChambersChanged (with ntouchedchambers = "+QString::number(s.size())+")" );
  emit touchedMuonChambersChanged(s);
}

//____________________________________________________________________
void VP1TrackSystem::visibleObjectsChanged()
{
  VisibleObjectToMaterialHelper<Trk::Track>* trackhelper(dynamic_cast< VisibleObjectToMaterialHelper<Trk::Track>* >(sender()));
  if (trackhelper) {
    //Fixme: For now we re-encode in a vector, to get old signal/slot signatures. We should change that!
    //       -> and remember not to put const in front of the so materials!!
    std::vector< std::pair<const Trk::Track*, const SoMaterial*> > vistracks;
    vistracks.reserve(trackhelper->visibleObjects().size());
    std::map<const Trk::Track*,SoMaterial*>::const_iterator it, itE = trackhelper->visibleObjects().end();
    for (it = trackhelper->visibleObjects().begin();it!=itE;++it)
      vistracks.push_back(std::pair<const Trk::Track*, const SoMaterial*>(it->first,it->second));
    messageVerbose("Emitting visibleTracksChanged (with nvistracks = "+QString::number(vistracks.size())+")" );
    emit visibleTracksChanged(vistracks);
    return;
  }
  VisibleObjectToMaterialHelper<Trk::Segment>* segmenthelper(dynamic_cast<VisibleObjectToMaterialHelper<Trk::Segment>*>(sender()));
  if (segmenthelper) {
    //Fixme: For now we re-encode in a vector, to get old signal/slot signatures. We should change that!
    //       -> and remember not to put const in front of the so materials!!
    std::vector< std::pair<const Trk::Segment*, const SoMaterial*> > vissegments;
    vissegments.reserve(segmenthelper->visibleObjects().size());
    std::map<const Trk::Segment*,SoMaterial*>::const_iterator it, itE = segmenthelper->visibleObjects().end();
    for (it = segmenthelper->visibleObjects().begin();it!=itE;++it)
      vissegments.push_back(std::pair<const Trk::Segment*, const SoMaterial*>(it->first,it->second));
    messageVerbose("Emitting visibleSegmentsChanged (with nvissegments = "+QString::number(vissegments.size())+")" );
    emit visibleSegmentsChanged(vissegments);
    return;
  }
  message("unexpected signal received in visibleObjectsChanged slot");
}

//____________________________________________________________________
void VP1TrackSystem::setApplicableIDProjections( InDetProjFlags::InDetProjPartsFlags pixel,
                InDetProjFlags::InDetProjPartsFlags sct,
                InDetProjFlags::InDetProjPartsFlags trt )
{
  messageVerbose("Signal received in setApplicableIDProjections slot");
  ensureBuildController();
  if (!m_d->common)//After uncreate.
    return;
  bool changes(false);
  if (m_d->common->indetProjHelper_Pixel()->parts() != pixel) {
    m_d->common->indetProjHelper_Pixel()->setParts(pixel);
    changes = true;
  }
  if (m_d->common->indetProjHelper_SCT()->parts() != sct) {
    m_d->common->indetProjHelper_SCT()->setParts(sct);
    changes = true;
  }
  if (m_d->common->indetProjHelper_TRT()->parts() != trt) {
    m_d->common->indetProjHelper_TRT()->setParts(trt);
    changes = true;
  }

  if (changes) {
    messageVerbose("  => Flags changed.");
    if (m_d->common->controller())
      foreach( TrackCollHandleBase* collhandle, m_d->common->controller()->collWidget()->collections<TrackCollHandleBase>() )
      collhandle->updateInDetProjectionsOfAllHandles();
  } else {
    messageVerbose("  => Flags unchanged.");
  }
}


//____________________________________________________________________
void VP1TrackSystem::updateSelectionMode()
{
  messageVerbose("updateSelectionMode start");
  if (!m_d->sel_tracks||!m_d->ascObjSelManager) {
    messageVerbose("updateSelectionMode Warning: Ignoring due to null pointers.");
    return;
  }
  deselectAll();
  m_d->ascObjSelManager->deselectAll();
  m_d->selMode = m_d->common->controller()->selectionMode();//NB: Don't abort if unchanged (we call this method to init)
  if (m_d->selMode==TrackCommonFlags::TRACKFIT) {
    messageVerbose("updateSelectionMode => TRACKFIT");
    m_d->sel_tracks->policy = SoCooperativeSelection::SINGLE;
    m_d->ascObjSelManager->setMode(AscObjSelectionManager::SHIFT);
  } else if (m_d->selMode==TrackCommonFlags::MULTITRACK) {
    messageVerbose("updateSelectionMode => MULTITRACK");
    m_d->sel_tracks->policy = SoCooperativeSelection::TOGGLE;
    m_d->ascObjSelManager->setMode(AscObjSelectionManager::SINGLE);
  } else {
    if (m_d->selMode!=TrackCommonFlags::SINGLEOBJECT)
      message("updateSelectionMode ERROR: Unexpected selection mode flag");
    messageVerbose("updateSelectionMode => SINGLEOBJECT");
    m_d->sel_tracks->policy = SoCooperativeSelection::SINGLE;
    m_d->ascObjSelManager->setMode(AscObjSelectionManager::SINGLE);
  }
  messageVerbose("updateSelectionMode end");
}

//____________________________________________________________________
void VP1TrackSystem::userPickedNode(SoNode* pickedNode, SoPath* /*pickedPath*/)
{
  messageVerbose("userPickedNode");
  if (pickedNode==m_d->totmomline) {
    message("Total momentum of selected tracks [GeV]: p = "+str(m_d->totmomgev)+", m = "+str(m_d->totmass/CLHEP::GeV));
    return;
  }

  message("Unknown object clicked");
}

//____________________________________________________________________
void VP1TrackSystem::userSelectedSingleNode( SoCooperativeSelection* sel, SoNode* node, SoPath* pickedPath )
{
  messageVerbose("userSelectedSingleNode");
  AssociatedObjectHandleBase* pickedHandle(0);
  if (!m_d->ascObjSelManager->handleUserSelectedSingleNode(sel,node,pickedPath,pickedHandle)) {
    if (sel==m_d->sel_tracks) {
  //Hack to get selections working when representing tracks with tubes:
      if (node->getTypeId().isDerivedFrom(SoCylinder::getClassTypeId())) {
        pickedPath->pop();
        node=pickedPath->getTail();
      }
      TrackHandleBase * handle = m_d->common->trackHandle(node);
      if (!handle) {
        message("ERROR: Unknown track.");
        return;
      }
      TrackHandle_TrkTrack * handle_trktrack = dynamic_cast<TrackHandle_TrkTrack *>(handle);
      if (handle_trktrack&&m_d->selMode==TrackCommonFlags::TRACKFIT) {
        messageVerbose("userSelectedSingleNode - find measurements for track fit");
        
        QList<AssociatedObjectHandleBase*> trackmeas = handle_trktrack->getVisibleMeasurements();
        
        if (trackmeas.size()==0) message("In refit mode, but no visible measurements found so can't do anything. Perhaps they're not enabled in 'Details'?");
        QList<AssociatedObjectHandleBase*> currentsel = m_d->ascObjSelManager->currentSelection();
  //If at least one of the track measurements is unselected, we
  //select them all. Otherwise we deselect them.
        bool oneunselected(false);
        foreach(AssociatedObjectHandleBase* meas,trackmeas) {
          if (!currentsel.contains(meas)) {
            oneunselected = true;
            break;
          }
        }
        QList<const Trk::PrepRawData*> prdSet;
        if (oneunselected) {
          messageVerbose("userSelectedSingleNode - selecting " +QString::number(trackmeas.size()) + " measurements.");
          m_d->ascObjSelManager->ensureSelected(trackmeas);
          
          // Add PRDs. Need to be careful as they might not exist.
          foreach(AssociatedObjectHandleBase* meas,trackmeas) {
            AscObj_TSOS* tsosAsc = dynamic_cast<AscObj_TSOS*>(meas);
            if (tsosAsc && tsosAsc->rioOnTrack() && tsosAsc->rioOnTrack()->prepRawData()) prdSet.append(tsosAsc->rioOnTrack()->prepRawData());
          }
        } else {
          messageVerbose("userSelectedSingleNode - deselecting " +QString::number(trackmeas.size()) + " measurements.");
          m_d->ascObjSelManager->ensureDeselected(trackmeas);
        }
        setSelectedPRDs(prdSet); // FIXME - maybe I should append/remove from existing list?
        
        m_d->sel_tracks->deselectAll();
      } else {
        if (m_d->common->controller()->printInfoOnSingleSelection()){
          message(handle->clicked());
          messageVerbose("Emitting newTrackSelected ");
          m_d->common->setLastSelectedTrack(handle);
          emit newTrackSelected(*handle);
          m_d->common->controller()->setNumberOfSelectedPRDsAndTracks(m_d->selectedPRDs.count(),1); // FIXME - we can do this more cleanly?
        }
      }
    } else {
      message("ERROR: Unknown selection.");
      return;
    }
  }
  if (m_d->common->controller()->orientAndZoomOnSingleSelection()) {
    if (!pickedHandle||!pickedHandle->initiatesOwnZooms()) {
      std::set<SoCamera*> cameras = getCameraList();
      std::set<SoCamera*>::iterator it,itE = cameras.end();
      for (it=cameras.begin();it!=itE;++it)
        VP1CameraHelper::animatedZoomToPath(*it,sel,pickedPath,2.0,1.0);
    }
  }
}

//____________________________________________________________________
void VP1TrackSystem::userClickedOnBgd()
{
  messageVerbose("userClickedOnBgd");
  if (m_d->ascObjSelManager)
    m_d->ascObjSelManager->userClickedOnBgd();
  m_d->common->setLastSelectedTrack(0);
  QList<const Trk::PrepRawData*> prdSet;
  setSelectedPRDs(prdSet ); // pass in empty collection. FIXME - this should depend on mode?
}

//____________________________________________________________________
void VP1TrackSystem::userDeselectedSingleNode(SoCooperativeSelection*, SoNode* , SoPath*)
{
  messageVerbose("userDeselectedSingleNode");
}

//____________________________________________________________________
unsigned VP1TrackSystem::Imp::calcTotalMomentumOfSelectedHandles(Amg::Vector3D& totmom, Amg::Vector3D& totpos, double& mass)
{
  totmom = Amg::Vector3D(0,0,0);
  totpos = Amg::Vector3D(0,0,0);
  mass = 0;
  if (!sel_tracks)
    return 0;
  unsigned nused(0);
  double totenergy(0);
  for (int i = 0; i < sel_tracks->getList()->getLength(); ++i) {
    SoFullPath *fPath = static_cast<SoFullPath *>((*(sel_tracks->getList()))[i]);
    TrackHandleBase * handle = common->trackHandle(fPath?fPath->getTail():0);
    if (!handle)
      continue;
    Amg::Vector3D mom = handle->momentum();
    if (mom.mag2()==0.0)
      continue;
    //Fixme: Get actual position of perigee!!
    const Amg::Vector3D * pos = handle->startPoint();
    if (!pos)
      continue;
    ++nused;
    totmom += mom;
    totpos += *pos;
    mass = handle->hasMass() ? handle->mass() : 0;
    totenergy += sqrt(mom.mag2() + mass*mass);
  }
  if (nused>1) {
    double msq = totenergy*totenergy - totmom.mag2();
    mass = (msq<0?-1.0:1.0)*sqrt(fabs(msq));
    totpos /= nused;
  }
  return nused;
}


//____________________________________________________________________
void VP1TrackSystem::updateShownTotMomentum()
{
  messageVerbose("updateShownTotMomentum");
  if (!m_d->common->controller()->showTotMomentumOnMultiTrackSelection()) {
    //ensure detach:
    messageVerbose("  => detach");
    if (m_d->totmomsep&&m_d->totmomline&&m_d->totmomsep->findChild(m_d->totmomline)>-1)
      m_d->totmomsep->removeChild(m_d->totmomline);
    return;
  }
  Amg::Vector3D totmom;
  Amg::Vector3D totpos;
  double totmass;
  unsigned nused = m_d->calcTotalMomentumOfSelectedHandles(totmom,totpos,totmass);
  if (nused==0) {
    //ensure detach:
    messageVerbose("  => detach");
    if (m_d->totmomsep&&m_d->totmomline&&m_d->totmomsep->findChild(m_d->totmomline)>-1)
      m_d->totmomsep->removeChild(m_d->totmomline);
  } else {
    //ensure correct lineset:
    Amg::Vector3D p2 = totpos+totmom.unit()*1*CLHEP::m;
    if (!m_d->totmomline) {
      m_d->totmomline = new SoLineSet;
      m_d->totmomline->ref();
      SoVertexProperty * vertices = new SoVertexProperty;
      m_d->totmomline->vertexProperty = vertices;
      m_d->totmomline->numVertices.set1Value(0,2);

    }
    SoVertexProperty * vertices = static_cast<SoVertexProperty*>(m_d->totmomline->vertexProperty.getValue());
    vertices->vertex.set1Value(0,totpos.x(),totpos.y(),totpos.z());
    vertices->vertex.set1Value(1,p2.x(),p2.y(),p2.z());
    m_d->totmomgev = totmom / CLHEP::GeV;
    m_d->totmass = totmass;
    //ensure attach:
    messageVerbose("  => attach");
    if (m_d->totmomsep&&m_d->totmomline&&m_d->totmomsep->findChild(m_d->totmomline)<0)
      m_d->totmomsep->addChild(m_d->totmomline);
    return;
  }

}

//____________________________________________________________________
void VP1TrackSystem::userChangedSelection(SoCooperativeSelection* sel, QSet<SoNode*> /*nodes*/, QSet<SoPath*>/*paths*/)
{
  messageVerbose("userChangedSelection begin");
  if (sel!=m_d->sel_tracks)
    return;
  messageVerbose("userChangedSelection => sel_tracks!!");


  if (m_d->common->controller()->printTotMomentumOnMultiTrackSelection()) {
    Amg::Vector3D totmom;
    Amg::Vector3D totpos;
    double totmass;
    if (m_d->calcTotalMomentumOfSelectedHandles(totmom,totpos,totmass)>0) {
      Amg::Vector3D totmomgev = totmom;
      totmomgev /= CLHEP::GeV;
      message("Total momentum [GeV] : "+str(totmomgev));//Fixme: Eta/phi/etc...
      message("Total mass [GeV] : "+str(totmass/CLHEP::GeV));//Fixme: Eta/phi/etc...
    }
  }

  updateShownTotMomentum();
}




//____________________________________________________________________
InDetProjFlags::DetTypeFlags VP1TrackSystem::Imp::currentUsedIDProjections() const
{
  if (common->controller()->collWidget()->visibleStdCollections().isEmpty())
    return InDetProjFlags::NoDet;
  if (common->controller()->shownTrackParts() & TrackCommonFlags::InDetProjections)
    return InDetProjFlags::TRT_all;
  return InDetProjFlags::NoDet;
}

//____________________________________________________________________
void VP1TrackSystem::possiblyEmitUsedIDProjectionsChanged()
{
  InDetProjFlags::DetTypeFlags usedidprojs = m_d->currentUsedIDProjections();
  if (m_d->lastEmittedUsedIDProjections == usedidprojs)
    return;
  m_d->lastEmittedUsedIDProjections = usedidprojs;
  usedIDProjectionsChanged(usedidprojs);
}

//____________________________________________________________________
void VP1TrackSystem::setSelectedPRDs(const QList<const Trk::PrepRawData*>& s)
{
  m_d->selectedPRDs = s;
  if (m_d->common&&m_d->common->controller())
    m_d->common->controller()->setNumberOfSelectedPRDsAndTracks(m_d->selectedPRDs.count(),0);
}

//____________________________________________________________________
void VP1TrackSystem::refit()
{
  //FIXME: WE NEED A HELPER CLASS!!

  if (!m_d->common||!m_d->common->controller()||!m_d->sel_tracks)//To check that we are actually refreshed
    return;
  messageVerbose("Refit requested with mode="+TrackCommonFlags::toString(m_d->common->controller()->fitterMode()));
  const Trk::ITrackFitter* currentFitter = m_d->common->controller()->trackFitter();
  if (!currentFitter) {
    message("ERROR - Aborting refit as no fitter available.");
    return;
  }
  
  QList<const Trk::Track*> fittedtracks;
  switch (m_d->common->controller()->fitterMode()) {
    case TrackCommonFlags::FROMPRDS: refitFromPRDs(currentFitter, fittedtracks);break;
    case TrackCommonFlags::REFITSINGLETRACK: refitSingleTrack(currentFitter, fittedtracks);break;
    case TrackCommonFlags::EXTENDTRACKWITHPRDS: message("Not yet implemented");break;
    case TrackCommonFlags::COMBINETWOTRACKS: message("Not yet implemented");break;
  }
  
  if (fittedtracks.empty()) return;
  
  TrackCollHandle_RefittedTracks * newtrackcoll =
    new TrackCollHandle_RefittedTracks(m_d->common,
               m_d->common->controller()->nameOfNewlyFittedCollections(),
               fittedtracks);
  newtrackcoll->init();

  QList<TrackCollHandleBase*> newcols;
  newcols << newtrackcoll;

  m_d->common->controller()->collWidget()->addCollections(newcols);

  //Add new collections to event scenegraph and turn them on:
  foreach (TrackCollHandleBase* col,newcols) {
    m_d->sel_tracks->addChild(col->collSwitch());
    col->setVisible(true);
  }
}

void VP1TrackSystem::refitFromPRDs(const Trk::ITrackFitter* /**currentFitter*/, QList<const Trk::Track*>& /**fittedtracks*/)
{
  // FIXME (or remove, since no one used this I think - EJWM)
  
  // std::vector<const Trk::PrepRawData*> prdSet;
  //  prdSet.reserve(m_d->selectedPRDs.count());
  //  foreach (const Trk::PrepRawData* prd,m_d->selectedPRDs)
  //    prdSet.push_back(prd);
  // 
  //  if (prdSet.size()==0) {
  //    message("ERROR - Aborting refit as prdset is empty");
  //    return;
  //  }
  // 
  //  //Get direction from PRD locations
  //  unsigned i1(0),i2(prdSet.size()-1);
  //  const Amg::Vector3D* globPos0 = prdSet.at(i1)->detectorElement()->surface(prdSet.at(i1)->identify()).localToGlobal(prdSet.at(i1)->localPosition());
  //  const Amg::Vector3D* globPos1 = prdSet.at(i2)->detectorElement()->surface(prdSet.at(i2)->identify()).localToGlobal(prdSet.at(i2)->localPosition());
  //  Amg::Vector3D globMom = *globPos1 - *globPos0;
  // 
  //  // Loop and try several momenta
  //  double minMom = 3000.0;
  //  double maxMom = 23000.0;
  //  int numberIterations = 10;
  //  double stepSize = (maxMom-minMom)/static_cast<double>(numberIterations);
  // 
  //  const Trk::Track* bestTrack=0;
  //  bool outlier = m_d->common->controller()->fitterRemoveOutliers();
  //  Trk::ParticleHypothesis hypo = m_d->common->controller()->fitterParticleHypthesis();
  //  for ( double initialMom = minMom ; initialMom<=maxMom ; initialMom+=stepSize){
  //    globMom.setMag(initialMom);
  //    Trk::Perigee params(*globPos0, globMom, 1.0, Amg::Vector3D(0.0,0.0,0.0));
  // 
  //    const Trk::Track* fittedtrk = currentFitter->fit(prdSet, params, outlier,hypo );
  // 
  //    QString mom=QString::number(initialMom);
  //    if (!fittedtrk || !(fittedtrk->fitQuality() ) ) {
  //      message("Track fit failed for seed momentum: "+mom+", outlier="+QString::number(outlier));
  //      delete fittedtrk; //Discard tracks with no FQ (shouldn't ever happen, but need to test)
  //    } else {
  //      if (!bestTrack) {
  //        bestTrack=fittedtrk; // first valid track
  //        message("Track fit succeeded for seed momentum:"+mom+", outlier="+QString::number(outlier)+", and is new best track.");
  //      } else {
  //                // okay, have valid fit so now compare to previous track...
  //        const Trk::FitQuality* bestFQ   = bestTrack->fitQuality();
  //        const Trk::FitQuality* fittedFQ = fittedtrk->fitQuality();
  //        if ( (fittedFQ->chiSquared()/fittedFQ->doubleNumberDoF())>(bestFQ->chiSquared()/bestFQ->doubleNumberDoF()) ){
  //          delete bestTrack; bestTrack=fittedtrk; // Replace old best track with this one
  //          message("Track fit succeeded for seed momentum:"+mom+", outlier="+QString::number(outlier)+", and is new best track.");
  //        } else {
  //          delete fittedtrk; // fit not as good as previous fit.
  //          message("Track fit succeeded for seed momentum:"+mom+", outlier="+QString::number(outlier)+", but is not best track.");
  //        }
  //      }
  //    }
  // 
  //  }
  // 
  //  if (!bestTrack) {
  //      message("All fits failed - no resulting track.");
  //      return;
  //  }
  // 
  //  delete globPos0; delete globPos1;
  // 
  //  fittedtracks << bestTrack;
  // 
  //  std::ostringstream s;
  //  s << *bestTrack;
  //  messageVerbose( QString(s.str().c_str()).split('\n'));
}

void VP1TrackSystem::refitSingleTrack(const Trk::ITrackFitter* currentFitter, QList<const Trk::Track*>& fittedtracks)
{
  const TrackHandleBase* handle = m_d->common->lastSelectedTrackHandle();
  const TrackHandle_TrkTrack* trkhandle = dynamic_cast<const TrackHandle_TrkTrack*>(handle);
  if (!trkhandle) return; // shouldn't ever happen
  const Trk::Track* track =trkhandle->trkTrackPointer();
  if (!track) return; // shouldn't ever happen
  
  bool outlier = m_d->common->controller()->fitterRemoveOutliers();
  Trk::ParticleHypothesis hypo = m_d->common->controller()->fitterParticleHypthesis();
  
  const Trk::Track* fittedtrk = currentFitter->fit(*track,outlier,hypo);
  
  if (!fittedtrk) {
      message("Fit failed - no resulting track.");
      return;
  }
  
  fittedtracks << fittedtrk;

  std::ostringstream s;
  s << *fittedtrk;
  messageVerbose( QString(s.str().c_str()).split('\n'));
}

SoCooperativeSelection * VP1TrackSystem::selTracks()
{
  return m_d->sel_tracks;
}

void VP1TrackSystem::updateAlignment(){
  messageVerbose("updateAlignment");
  
  std::vector<double> values = m_d->common->controller()->alignmentShiftValue();
  int level = m_d->common->controller()->alignmentShiftLevel();
  
  messageVerbose("updateAlignment called with level="+QString::number(level));
  assert (values.size()==6);
  messageVerbose("0="+QString::number(values[0]));
  messageVerbose("1="+QString::number(values[1]));
  messageVerbose("2="+QString::number(values[2]));
  messageVerbose("3="+QString::number(values[3]));
  messageVerbose("4="+QString::number(values[4]));
  messageVerbose("5="+QString::number(values[5]));
  
  const Trk::TrkDetElementBase* detEl = 0;  
  
  // Try to find last selected TSOS
  if (!m_d->ascObjSelManager) {
    // this shouldn't happen!
    message ("No selection manager - giving up.");
    return;
  }
  
  QList<AssociatedObjectHandleBase*> currentsel = m_d->ascObjSelManager->currentSelection();
  foreach(AssociatedObjectHandleBase* meas,currentsel) {
    AscObj_TSOS* tsosAsc = dynamic_cast<AscObj_TSOS*>(meas);
    if (tsosAsc){
      if (tsosAsc->rioOnTrack() ) 
        detEl= tsosAsc->rioOnTrack()->detectorElement ();
      else if (tsosAsc->trackStateOnSurface()->trackParameters() ) 
        detEl= tsosAsc->trackStateOnSurface()->trackParameters()->associatedSurface().associatedDetectorElement ();
    } 
    if (detEl) break; // set, so can skip rest
  }
  if (!detEl){
    messageVerbose("No det Element found");
    
    return;// Abort!
  } else {
    const MdtIdHelper* mdtIdHelper = VP1DetInfo::mdtIDHelper();
    std::string stationName(mdtIdHelper->stationNameString (mdtIdHelper->stationName(detEl->identify())));
    
    messageVerbose("Found det Element from station="+QString(stationName.c_str()));
  }
  
  // redraw everything;
  // FIXME
}

void VP1TrackSystem::tracksFromVertexChanged(QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > >& vertexList){
  messageVerbose("VP1TrackSystem::tracksFromVertexChanged. Got a list THIS big! "+QString::number(vertexList.size()));
  
  // firstly, by default option to cut tracks by vertex is disabled, so enable:
  m_d->common->controller()->vertexCutsAllowed(true);
  
  // do something with vertexList!
  std::pair<const SoMaterial*, QList< const Trk::Track*> > it;
  foreach(it, vertexList){
    SoMaterial* mat = const_cast<SoMaterial*>(it.first);
    messageVerbose("Number of tracks associated with this vertex= "+QString::number(it.second.size()));

    const Trk::Track* trk;
    foreach(trk, it.second) {
      m_d->vertexMaterialForTrackBases[trk]=mat;
      mat->ref();
//      messageVerbose("Adding TrackHandleBase with pointer= "+QString::number((unsigned int)handle)+" for trk="+QString::number((unsigned int)trk));
    }
  }
  
  // update track collections too.
  foreach(TrackCollHandleBase* coll,  m_d->common->controller()->collWidget()->collections<TrackCollHandleBase>())
    if (coll->allowColourByVertex()) coll->updateMaterialOfAllHandles();  
}

SoMaterial* VP1TrackSystem::materialFromVertex(const TrackHandleBase* trk) const {
  
  const TrackHandle_TrkTrack* handle = dynamic_cast<const TrackHandle_TrkTrack*>(trk);
  if (handle) {
    std::map<const Trk::Track*, SoMaterial*>::const_iterator it = m_d->vertexMaterialForTrackBases.find(handle->trkTrackPointer());

    if (it!=m_d->vertexMaterialForTrackBases.end()){  
      return it->second;
    }
  }
  messageVerbose("VP1TrackSystem::materialFromVertex. No matching track handle for handle with pointer="+QString::number((uintptr_t)trk));
  return 0;
}


