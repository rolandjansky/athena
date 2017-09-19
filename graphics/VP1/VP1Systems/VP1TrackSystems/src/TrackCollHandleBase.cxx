/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandleBase               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackCollHandleBase.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1TrackSystems/TrackHandleBase.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/AscObj_TSOS.h"
#include "VP1TrackSystems/TrackCollectionSettingsButton.h"

#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1CustomTourEditor.h"


#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <QComboBox>
#include <QTreeWidgetItem>

#include <qdatetime.h>
#include <vector>

// #include "VP1TrackSystems/TrackObjectBrowserController.h"


//____________________________________________________________________
class TrackCollHandleBase::Imp {
public:
  TrackCollHandleBase * theclass;
  QString name;

  //Vector of track handles:
  std::vector<TrackHandleBase*> trackhandles;
  //For iteration:
  std::vector<TrackHandleBase*>::iterator itTrackHandles;
  std::vector<TrackHandleBase*>::iterator itTrackHandlesEnd;

  Trk::IExtrapolator *  lastUsedPropagator;
  bool notifystatesave;

  TrackCommonFlags::TSOSPartsFlags shownTSOSParts;
  TrackCommonFlags::TSOSPartsFlags customColouredTSOSParts;
  bool tsos_useShortTRTMeasurements;
  bool tsos_useShortMDTMeasurements;
  bool tsos_drawMeasGlobalPositions;
  double tsos_measurementsShorttubesScale;
  double tsos_nStdDevForParamErrors;
  int tsos_numberOfPointsOnCircles;
  double tsos_materialEffectsOnTrackScale;
  bool tsos_parTubeErrorsDrawCylinders;

  //Extra widgets:
  QComboBox * comboBox_colourby;

  static QString comboBoxEntry_ColourByCollection() { return "Uniform"; }
  static QString comboBoxEntry_ColourByRandom() { return "Random"; }
  static QString comboBoxEntry_ColourByMomentum() { return "Mom."; }
  static QString comboBoxEntry_ColourByCharge() { return "Charge"; }
  static QString comboBoxEntry_ColourByPID() { return "PID"; }
  static QString comboBoxEntry_ColourByDistanceFromSelectedTrack() { return "Dist. Sel."; }
  static QString comboBoxEntry_ColourByVertex() { return "Vertex"; }

  QTreeWidgetItem* objBrowseTree;
  TrackCollectionSettingsButton * matButton;
  bool last_useDefaultCuts;
};



//____________________________________________________________________
TrackCollHandleBase::TrackCollHandleBase( TrackSysCommonData * cd,
					  const QString& name,
					  TrackType::Type type)
  : VP1StdCollection(cd->system(),"TrackCollHandle_"+TrackType::typeToString(type)+"_"+name), m_d(new Imp),
    m_nshownhandles(0), m_type(type),
    m_commonData(cd),
    m_sephelper(0),
    m_propagator(0),
    m_propagationOptions(TrackSystemController::NoPropOptions),
    m_propMaxRadius(0.0),
    m_parts(TrackCommonFlags::NoParts),
    m_labels(TrackSystemController::NoLabels), m_labelsTrkOffset(0.0),
    m_trackTubeRadius(0.0),
    m_colourby(COLOUR_PERCOLLECTION),
    m_cut_allowedPtSq(VP1Interval()),
    m_cut_allowedEta(VP1Interval()),
    m_cut_allowedPhi(QList<VP1Interval>()),
    m_cut_requiredNHits(QList<unsigned>()),
    m_cut_pt_allowall(false),
    m_cut_eta_allowall(false),
    m_cut_phi_allowall(false),
    m_cut_etaptphi_allwillfail(true)
{
  m_d->theclass = this;
  m_d->name = name;
  m_d->lastUsedPropagator = 0;
  m_d->notifystatesave = false;
  m_d->shownTSOSParts = TrackCommonFlags::TSOS_NoObjects;
  m_d->customColouredTSOSParts = TrackCommonFlags::TSOS_NoObjects;
  //Fixme: to avoid double work on startup, these should have the same
  //values here as the controller defaults:
  m_d->tsos_useShortTRTMeasurements = true;
  m_d->tsos_useShortMDTMeasurements = true;
  m_d->tsos_drawMeasGlobalPositions = false;  
  m_d->tsos_measurementsShorttubesScale = 2.0;
  m_d->tsos_nStdDevForParamErrors = false;
  m_d->tsos_numberOfPointsOnCircles = 12;
  m_d->tsos_materialEffectsOnTrackScale = 2.0;
  m_d->tsos_parTubeErrorsDrawCylinders = false;

  m_d->comboBox_colourby = new QComboBox;
  m_d->objBrowseTree = 0;
  m_d->matButton=0;
}

//____________________________________________________________________
void TrackCollHandleBase::init(VP1MaterialButtonBase*)
{
  m_d->matButton = new TrackCollectionSettingsButton;
  m_d->matButton->setText(text());
  VP1StdCollection::init(m_d->matButton);//this call is required. Passing in TrackCollectionSettingsButton means we have the more complex button. 

  setupSettingsFromController(common()->controller());

  //Setup colour by:
  m_d->comboBox_colourby->setToolTip("Determine how tracks from this collection are coloured.\n The 'Dist. Sel.' option means the tracks will be coloured by distance from the last selected track. ");
  QStringList l;
  l << Imp::comboBoxEntry_ColourByCollection();
  l << Imp::comboBoxEntry_ColourByRandom();
  if (allowColourByPID())
    l << Imp::comboBoxEntry_ColourByPID();
  if (allowColourByMomentum())
    l << Imp::comboBoxEntry_ColourByMomentum();
  if (allowColourByCharge())
    l << Imp::comboBoxEntry_ColourByCharge();
  if (allowColourByDistanceFromSelectedTrack())
    l << Imp::comboBoxEntry_ColourByDistanceFromSelectedTrack();
  if (allowColourByVertex())  {
    // messageVerbose("Allow colour by vertex!");
   l << Imp::comboBoxEntry_ColourByVertex(); 
  }  
  m_d->comboBox_colourby->addItems(l);
  m_d->comboBox_colourby->setCurrentIndex(0);//corresponds to per collection.
  connect(m_d->comboBox_colourby,SIGNAL(currentIndexChanged(int)),
	  this,SLOT(colourByComboBoxItemChanged()));
  
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
  setColourBy(defaultColourBy());
  
  // add stuff to control per collection settings.
  collSwitch()->addChild(m_d->matButton->trackLightModel());
  collSwitch()->addChild(m_d->matButton->trackDrawStyle());
}

//____________________________________________________________________
TrackCollHandleBase::~TrackCollHandleBase()
{
  messageVerbose("destructor start");
  cleanupPtrContainer(m_d->trackhandles);
  if (m_sephelper) {
    SoSeparator * sep = m_sephelper->topSeparator();
    sep->ref();
    delete m_sephelper;
    sep->unref();
  }
  delete m_d;
  messageVerbose("destructor end");
}

//____________________________________________________________________
void TrackCollHandleBase::setupSettingsFromController(TrackSystemController* controller)
{
  messageVerbose("setupSettingsFromController start");

  assert(controller);
  largeChangesBegin();

  connect(controller->customTourEditor(),SIGNAL(clipVolumeRadiusChanged(double)),this,SLOT(clipVolumeChanged(double)));

  connect(controller,SIGNAL(propagatorChanged(Trk::IExtrapolator *)),this,SLOT(setPropagator(Trk::IExtrapolator *)));
  setPropagator(controller->propagator());
  
  connect(controller,SIGNAL(propagationOptionsChanged(TrackSystemController::PropagationOptionFlags)),this,SLOT(setPropagationOptions(TrackSystemController::PropagationOptionFlags)));
  setPropagationOptions(controller->propagationOptions());

  connect(controller,SIGNAL(propMaxRadiusChanged(float)),this,SLOT(setPropMaxRadius(float)));
  setPropMaxRadius(controller->propMaxRadius());

  // Doing this in a slightly different way to avoid making lots of new stuff in controller.
  connect(controller,SIGNAL(extrapolateToThisVolumeChanged(void)),this,SLOT(extrapolateToThisVolumeChanged(void)));

  connect(controller,SIGNAL(shownTrackPartsChanged(TrackCommonFlags::TrackPartsFlags)),this,SLOT(setParts(TrackCommonFlags::TrackPartsFlags)));
  connect(m_d->matButton,SIGNAL(hideActualTrackPathChanged(bool)),this,SLOT(setActualPathShown(bool)));
  setParts(controller->shownTrackParts());

  connect(controller,SIGNAL(vertexProjectionAngleChanged(int)),this,SLOT(setParts(int)));
  setParts(controller->vertexProjectionAngle());

  // connect(controller,SIGNAL(trackTubeRadiusChanged(const double&)),this,SLOT(setTrackTubeRadius(const double&)));
  connect(m_d->matButton,SIGNAL(trackTubeRadiusChanged(const double&)),this,SLOT(setTrackTubeRadius(const double&)));
  setTrackTubeRadius(m_d->matButton->trackTubeRadius());

  connect(controller,SIGNAL(rerandomise()),this,SLOT(rerandomiseRandomTrackColours()));
  
  connect(common()->system(),SIGNAL(newTrackSelected( const TrackHandleBase&)),this,SLOT(trackSelectionChanged()));

  //cuts
  connect(m_d->matButton,SIGNAL(useDefaultCutsChanged(bool)),this,SLOT(setUseDefaultCuts(bool)));
  setUseDefaultCuts(m_d->matButton->useDefaultCuts());
  
  connect(controller,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  connect(m_d->matButton,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
  setCutAllowedPt(useDefaultCuts()?controller->cutAllowedPt():m_d->matButton->cutAllowedPt());

  connect(controller,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  connect(m_d->matButton,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
  setCutAllowedEta(useDefaultCuts()?controller->cutAllowedEta():m_d->matButton->cutAllowedEta());

  connect(controller,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  connect(m_d->matButton,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
  setCutAllowedPhi(useDefaultCuts()?controller->cutAllowedPhi():m_d->matButton->cutAllowedPhi());

  connect(controller,SIGNAL(cutRequiredNHitsChanged(const QList<unsigned>&)),this,SLOT(setRequiredNHits(const QList<unsigned>&)));
  connect(m_d->matButton,SIGNAL(cutRequiredNHitsChanged(const QList<unsigned>&)),this,SLOT(setRequiredNHits(const QList<unsigned>&)));
  setRequiredNHits(useDefaultCuts()?controller->cutRequiredNHits():m_d->matButton->cutRequiredNHits());
  
  connect(controller,SIGNAL(cutOnlyVertexAssocTracksChanged(bool)),this,SLOT(setOnlyVertexAssocTracks(bool)));
  connect(m_d->matButton,SIGNAL(cutOnlyVertexAssocTracksChanged(bool)),this,SLOT(setOnlyVertexAssocTracks(bool)));
  setOnlyVertexAssocTracks(useDefaultCuts()?controller->cutOnlyVertexAssocTracks():m_d->matButton->cutOnlyVertexAssocTracks());

  connect(controller,SIGNAL(customColouredTSOSPartsChanged(TrackCommonFlags::TSOSPartsFlags)),
	  this,SLOT(setCustomColouredTSOSParts(TrackCommonFlags::TSOSPartsFlags)));
  setCustomColouredTSOSParts(controller->customColouredTSOSParts());

  connect(controller,SIGNAL(useShortTRTMeasurementsChanged(bool)),this,SLOT(setUseShortTRTMeasurements(bool)));
  setUseShortTRTMeasurements(controller->useShortTRTMeasurements());

  connect(controller,SIGNAL(useShortMDTMeasurementsChanged(bool)),this,SLOT(setUseShortMDTMeasurements(bool)));
  setUseShortMDTMeasurements(controller->useShortMDTMeasurements());

  connect(controller,SIGNAL(drawMeasGlobalPositionsChanged(bool)),this,SLOT(setDrawMeasGlobalPositions(bool)));
  setDrawMeasGlobalPositions(controller->drawMeasGlobalPositions());

  connect(controller,SIGNAL(measurementsShorttubesScaleChanged(double)),this,SLOT(setMeasurementsShorttubesScale(double)));
  setMeasurementsShorttubesScale(controller->measurementsShorttubesScale());

  connect(controller,SIGNAL(nStdDevForParamErrorsChanged(const double&)),this,SLOT(setNStdDevForParamErrors(const double&)));
  setNStdDevForParamErrors(controller->nStdDevForParamErrors());

  connect(controller,SIGNAL(numberOfPointsOnCirclesChanged(int)),this,SLOT(setNumberOfPointsOnCircles(int)));
  setNumberOfPointsOnCircles(controller->numberOfPointsOnCircles());

  connect(controller,SIGNAL(materialEffectsOnTrackScaleChanged(double)),this,SLOT(setMaterialEffectsOnTrackScale(double)));
  setMaterialEffectsOnTrackScale(controller->materialEffectsOnTrackScale());

  connect(controller,SIGNAL(parTubeErrorsDrawCylindersChanged(bool)),this,SLOT(setParTubeErrorsDrawCylinders(bool)));
  setParTubeErrorsDrawCylinders(controller->parTubeErrorsDrawCylinders());

  connect(controller,SIGNAL(shownTSOSPartsChanged(TrackCommonFlags::TSOSPartsFlags)),
	  this,SLOT(setShownTSOSParts(TrackCommonFlags::TSOSPartsFlags)));
  setShownTSOSParts(controller->shownTSOSParts());

  connect(controller,SIGNAL(customColouredTSOSPartsChanged(TrackCommonFlags::TSOSPartsFlags)),
	  this,SLOT(setCustomColouredTSOSParts(TrackCommonFlags::TSOSPartsFlags)));
  setCustomColouredTSOSParts(controller->customColouredTSOSParts());

  connect(controller,SIGNAL(trackLabelsChanged(TrackSystemController::TrackLabelModes)),this,SLOT(setLabels(TrackSystemController::TrackLabelModes)));
  connect(controller,SIGNAL(trackLabelTrkOffsetChanged(float)),this,SLOT(setLabelTrkOffset(float)));
  connect(controller,SIGNAL(trackLabelPosOffsetChanged(QList<int>)),this,SLOT(setLabelPosOffsets(QList<int>)));
  setLabels(controller->trackLabels());
  
  setupSettingsFromControllerSpecific(controller);

  largeChangesEnd();
  messageVerbose("setupSettingsFromController end");

}

//____________________________________________________________________
QString TrackCollHandleBase::name() const
{
  return m_d->name;
}

//____________________________________________________________________
bool TrackCollHandleBase::cut(TrackHandleBase* handle)
{
   if (m_cut_etaptphi_allwillfail)
    return false;

  // messageVerbose("TrackCollHandleBase::cut - checking hit cuts.");
  if (mightHaveSubSystemHitInfo()&&!m_cut_requiredNHits.isEmpty()&&handle->hasSubSystemHitInfo()) {
    assert(m_cut_requiredNHits.count()==4);
    // Only apply ID cuts to tracks which have ID hits
    if (handle->isIDTrack()){
      if (handle->getNPixelHits()<m_cut_requiredNHits[0]) return false;
      if (handle->getNSCTHits()<m_cut_requiredNHits[1]) return false;
      if (handle->getNTRTHits()<m_cut_requiredNHits[2]) return false;
    }
    if (handle->getNMuonHits()<m_cut_requiredNHits[3]) return false;
  }

  if (!m_cut_pt_allowall||!m_cut_eta_allowall||!m_cut_phi_allowall)
  {
//    Trk::GlobalMomentum mom(handle->momentum());
	  Amg::Vector3D mom(handle->momentum());
    
    // convention is that if interval is real and negative, then P cut rather than pT
    bool isPCut = (!m_cut_allowedPtSq.noLowerBound() && m_cut_allowedPtSq.lower()<=0) 
      || (!m_cut_allowedPtSq.noUpperBound() && m_cut_allowedPtSq.upper()<=0);
    
    if (mom.mag2()!=0.0) {
      if (!m_cut_pt_allowall&&!isPCut && !m_cut_allowedPtSq.contains(mom.perp2()))
        return false;
      if (!m_cut_pt_allowall&& isPCut && !m_cut_allowedPtSq.contains(-mom.mag2()))
        return false;
        
//      if (!m_cut_eta_allowall&&!m_cut_allowedEta.contains(mom.pseudoRapidity())){
      if (!m_cut_eta_allowall&&!m_cut_allowedEta.contains(mom.eta())){ // migration to Amg (Eigen)
        return false;
      }
      if (!m_cut_phi_allowall) {
        double phi = VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() );
        bool ok(false);
        foreach (VP1Interval phirange, m_cut_allowedPhi) {
          if (phirange.contains(phi)||phirange.contains(phi+2*M_PI)||phirange.contains(phi-2*M_PI)) {
            ok = true;
            break;
          }
        }
        if (!ok)
          return false;
      }
    }
  }
  
  if (cutOnlyVertexAssocTracks()){
    // std::cout<<"cutOnlyVertexAssocTracks: "<<handle<<",\t: "<<common()->system()->materialFromVertex(handle)<<std::endl;
    return common()->system()->materialFromVertex(handle)!=0; // return false if no vertex material associated to this track
  }  
  
  return true;
}

//____________________________________________________________________
void TrackCollHandleBase::recheckCutStatus(TrackHandleBase* handle)
{
  handle->setVisible( visible() && cut(handle));
}

//____________________________________________________________________
void TrackCollHandleBase::recheckCutStatusOfAllVisibleHandles()
{
  if (!isLoaded())
    return;

  //This method is called when a cut is tightened - thus we better start by deselectAll to avoid weird highlighting issues.
  common()->system()->deselectAll();

  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it) {
    if ((*it)->visible())
      recheckCutStatus(*it);
  }
  updateObjectBrowserVisibilityCounts();
  largeChangesEnd();

  message("recheckCutStatusOfAllVisibleHandles:  "+str(nShownHandles())+"/"+str(m_d->trackhandles.size())+" pass cuts");
}

//____________________________________________________________________
void TrackCollHandleBase::recheckCutStatusOfAllNotVisibleHandles()
{
  if (!isLoaded())
    return;

  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it) {
    if (!(*it)->visible())
      recheckCutStatus(*it);
  }
  updateObjectBrowserVisibilityCounts();
  largeChangesEnd();

  message("recheckCutStatusOfAllNotVisibleHandles:  "+str(nShownHandles())+"/"+str(m_d->trackhandles.size())+" pass cuts");
}

//____________________________________________________________________
void TrackCollHandleBase::recheckCutStatusOfAllHandles()
{
  if (!isLoaded())
    return;
  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it)
    recheckCutStatus(*it);
  updateObjectBrowserVisibilityCounts();
  largeChangesEnd();

  message("recheckCutStatusOfAllHandles:  "+str(nShownHandles())+"/"+str(m_d->trackhandles.size())+" pass cuts");
}

//____________________________________________________________________
void TrackCollHandleBase::update3DObjectsOfAllHandles(bool onlythosetouchingmuonchambers, bool invalidatePropagatedPoints, float maxR )
{
  if (!isLoaded())
    return;
  if (verbose())
    messageVerbose("update3DObjectsOfAllHandles start (onlythosetouchingmuonchambers = "
                   +str(onlythosetouchingmuonchambers)+", invalidatePropagatedPoints = "
                   +str(invalidatePropagatedPoints)+")");
  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  if (onlythosetouchingmuonchambers) {
    for (;it!=itE;++it)
      if (!(*it)->touchedMuonChambers().empty())
        (*it)->update3DObjects(invalidatePropagatedPoints, maxR);
  } else {
    for (;it!=itE;++it)
      (*it)->update3DObjects(invalidatePropagatedPoints, maxR);
  }
  largeChangesEnd();
  messageVerbose("update3DObjectsOfAllHandles end");
}



//____________________________________________________________________
void TrackCollHandleBase::updateInDetProjectionsOfAllHandles()
{
  if (!isLoaded())
    return;
  messageVerbose("updateInDetProjectionsOfAllHandles start");
  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it)
    (*it)->updateInDetProjections();
  largeChangesEnd();
  messageVerbose("updateInDetProjectionsOfAllHandles end");

}

//____________________________________________________________________
void TrackCollHandleBase::updateMuonProjectionsOfAllHandles()
{
  if (!isLoaded())
    return;
  messageVerbose("updateMuonProjectionsOfAllHandles start");
  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it)
    (*it)->updateMuonProjections();
  largeChangesEnd();
  messageVerbose("updateMuonProjectionsOfAllHandles end");

}

//____________________________________________________________________
void TrackCollHandleBase::updateMaterialOfAllHandles()
{
  if (!isLoaded())
    return;
  messageVerbose("updateMaterialOfAllHandles start");
  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it)
    (*it)->updateMaterial();
  largeChangesEnd();
  messageVerbose("updateMaterialOfAllHandles end");
}

//____________________________________________________________________
void TrackCollHandleBase::hintNumberOfTracksInEvent(unsigned n)
{
  m_d->trackhandles.reserve(n);
}

//____________________________________________________________________
void TrackCollHandleBase::addTrackHandle(TrackHandleBase* handle)
{
  m_d->trackhandles.push_back(handle);
}

//____________________________________________________________________
void TrackCollHandleBase::setPropagator( Trk::IExtrapolator * p)
{
  bool ignoreMEOT = common()->controller()->ignoreMeasurementEffectsOnTrackInProp();
  messageVerbose("setPropagator ("+str(p)+") "+ (ignoreMEOT?str("ignoring MEOT"):str("using MEOT")) );
  if (m_propagator==p)
    return;
  messageVerbose("setPropagator  ==> Changed");
  m_propagator=p;
  bool mustUpdatePropagatedPoints = (p && m_d->lastUsedPropagator!=p);
  update3DObjectsOfAllHandles(false,mustUpdatePropagatedPoints);
  if (p)
    m_d->lastUsedPropagator = p;
}

void TrackCollHandleBase::setPropagationOptions( TrackSystemController::PropagationOptionFlags options){
  messageVerbose("setPropagationOptions  ==> Changed");
  m_propagationOptions=options;
  update3DObjectsOfAllHandles(false,true);
}

void TrackCollHandleBase::setPropMaxRadius( float radius){
  messageVerbose("setPropMaxRadius  ==> Changed to "+str(radius));
  m_propMaxRadius=radius;
  update3DObjectsOfAllHandles(false,true);
}

void TrackCollHandleBase::extrapolateToThisVolumeChanged(void){
  messageVerbose("extrapolateToThisVolume  ==> Changed");
  update3DObjectsOfAllHandles(false,true);
}

void TrackCollHandleBase::clipVolumeChanged(double radius){
  messageVerbose("clipVolumeChanged  ==> Changed");
  update3DObjectsOfAllHandles(false, false, radius);
}

//____________________________________________________________________
void TrackCollHandleBase::setParts( TrackCommonFlags::TrackPartsFlags parts )
{
  messageVerbose("setParts called");
  if (m_parts==parts)
    return;
  messageVerbose("setParts ==> Changed");
  bool onlymuonprojflagchanged = ( (parts^m_parts) == TrackCommonFlags::MuonProjections );
  m_parts = parts;
  update3DObjectsOfAllHandles(onlymuonprojflagchanged);
}

//____________________________________________________________________
void TrackCollHandleBase::setParts( int angle)
{
  messageVerbose("setParts(int) called with angle="+str(angle));
  //bit of a hack this- didn't want to make entire new machinery just to track angle.
  // FIXME
  update3DObjectsOfAllHandles(false,true);
}

//____________________________________________________________________
void TrackCollHandleBase::setActualPathShown( bool hide)
{
  messageVerbose("setActualPathShown(bool) called with hide="+str(hide));
  
  TrackCommonFlags::TrackPartsFlags parts = m_parts;
  if (hide) parts &= ~TrackCommonFlags::ActualPath;
  else      parts |= TrackCommonFlags::ActualPath;
  setParts(parts);
}

//____________________________________________________________________
void TrackCollHandleBase::setLabels( TrackSystemController::TrackLabelModes labels )
{
  messageVerbose("setLabels called");
  if (m_labels==labels)
    return;
  messageVerbose("setLabels ==> Changed");
  m_labels = labels;
  update3DObjectsOfAllHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::setLabelTrkOffset( float offset)
{
  messageVerbose("setLabelTrkOffset called");
  if (m_labelsTrkOffset==offset)
    return;
  messageVerbose("setLabelTrkOffset ==> Changed to "+QString::number(offset));
  m_labelsTrkOffset = offset;
  update3DObjectsOfAllHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::setLabelPosOffsets( QList<int> offsets)
{
  messageVerbose("setLabelPosOffsets called");
  if (m_labelsPosOffsets==offsets)
    return;
  messageVerbose("setLabelPosOffsets ==> Changed");
  m_labelsPosOffsets = offsets;
  update3DObjectsOfAllHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::setTrackTubeRadius(const double& r)
{
  messageVerbose("setTrackTubeRadius called");
  if (m_trackTubeRadius==r)
    return;
  m_trackTubeRadius=r;
  messageVerbose("setTrackTubeRadius ==> Changed");
  update3DObjectsOfAllHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::setColourBy( TrackCollHandleBase::COLOURBY cb )
{
  messageVerbose("setColourBy called");
  if (m_colourby==cb)
    return;
  messageVerbose("setColourBy ==> Changed");
  m_colourby=cb;

  //Update gui combobox:
  QString targetText;
  switch(cb) {
  case COLOUR_BYPID:
    targetText = Imp::comboBoxEntry_ColourByPID();
    break;
  case COLOUR_RANDOM:
    targetText = Imp::comboBoxEntry_ColourByRandom();
    break;
  case COLOUR_MOMENTUM:
    targetText = Imp::comboBoxEntry_ColourByMomentum();
    break;
  case COLOUR_CHARGE:
    targetText = Imp::comboBoxEntry_ColourByCharge();
    break;
  case COLOUR_DISTANCE:
    targetText = Imp::comboBoxEntry_ColourByDistanceFromSelectedTrack();
    break;
  case COLOUR_VERTEX:
    targetText = Imp::comboBoxEntry_ColourByVertex();
    break;
  default:
  case COLOUR_PERCOLLECTION:
    targetText = Imp::comboBoxEntry_ColourByCollection();
    break;
  }
  if (targetText!=m_d->comboBox_colourby->currentText()) {
    int i = m_d->comboBox_colourby->findText(targetText);
    if (i>=0&&i<m_d->comboBox_colourby->count()) {
      bool save = m_d->comboBox_colourby->blockSignals(true);
      m_d->comboBox_colourby->setCurrentIndex(i);
      m_d->comboBox_colourby->blockSignals(save);
    } else {
      message("ERROR: Problems finding correct text in combo box");
    }
  }

  //Actual material updates:
  largeChangesBegin();
  m_commonData->system()->deselectAll();//Todo: Reselect the selected track handles
                                        //afterwards (collhandles should know selected handles)
  updateMaterialOfAllHandles();
  largeChangesEnd();

}

//____________________________________________________________________
void TrackCollHandleBase::rerandomiseRandomTrackColours()
{
  if (!isLoaded())
    return;
  messageVerbose("rerandomiseRandomTrackColours start");
  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it)
    (*it)->rerandomiseRandomMaterial();
  largeChangesEnd();
  messageVerbose("rerandomiseRandomTrackColours end");
}

//____________________________________________________________________
void TrackCollHandleBase::trackSelectionChanged()
{
  if (!isLoaded() || colourBy()!= COLOUR_DISTANCE)
    return;
  messageVerbose("trackSelectionChanged start");
  largeChangesBegin();
  std::vector<TrackHandleBase*>::iterator it(m_d->trackhandles.begin()),itE(m_d->trackhandles.end());
  for (;it!=itE;++it)
    (*it)->updateMaterial();
  largeChangesEnd();
  messageVerbose("trackSelectionChanged end");
}

//____________________________________________________________________
void TrackCollHandleBase::trackHandleIterationBegin()
{
  m_d->itTrackHandles = m_d->trackhandles.begin();
  m_d->itTrackHandlesEnd = m_d->trackhandles.end();
}

//____________________________________________________________________
TrackHandleBase* TrackCollHandleBase::getNextTrackHandle() {
  if (m_d->itTrackHandles==m_d->itTrackHandlesEnd)
    return 0;
  else
    return *(m_d->itTrackHandles++);
}


void TrackCollHandleBase::setUseDefaultCuts(bool useDefaults){
  if (m_d->last_useDefaultCuts==useDefaults) return;
  messageVerbose("setUseDefaultCuts changed to "+str(useDefaults)+"- rechecking all handles");
  // recheckCutStatusOfAllHandles();
  
  setCutAllowedPt(useDefaultCuts()? common()->controller()->cutAllowedPt():m_d->matButton->cutAllowedPt());
  setCutAllowedEta(useDefaultCuts()?common()->controller()->cutAllowedEta():m_d->matButton->cutAllowedEta());
  setCutAllowedPhi(useDefaultCuts()?common()->controller()->cutAllowedPhi():m_d->matButton->cutAllowedPhi());
  setRequiredNHits(useDefaultCuts()?common()->controller()->cutRequiredNHits():m_d->matButton->cutRequiredNHits());
  setOnlyVertexAssocTracks(useDefaultCuts()?common()->controller()->cutOnlyVertexAssocTracks():m_d->matButton->cutOnlyVertexAssocTracks());
}

//____________________________________________________________________
void TrackCollHandleBase::setCutAllowedPt(const VP1Interval& allowedpt)
{
  if (!allowedpt.isSane())
    return;
  
  //why do we need to do this? Why does it matter if it's negative?
  //double minptsq = allowedpt.lower() <= 0.0 ? -std::numeric_limits<double>::infinity() : allowedpt.lower()*allowedpt.lower();
  //VP1Interval allowedptsq(minptsq,allowedpt.upper()*allowedpt.upper());
  
  //Modified code from EJWM
  double signMin = allowedpt.lower()<0.0 ? -1.0 : 1.0; 
  double signMax = allowedpt.upper()<0.0 ? -1.0 : 1.0; 
  double minptsq = allowedpt.lower() == -std::numeric_limits<double>::infinity() ? -std::numeric_limits<double>::infinity() : signMin*(allowedpt.lower()*allowedpt.lower());
  double maxptsq = allowedpt.upper() == std::numeric_limits<double>::infinity() ? std::numeric_limits<double>::infinity()   : signMax*(allowedpt.upper()*allowedpt.upper());
  
  VP1Interval allowedptsq(minptsq,maxptsq);
  // std::cout<<"allowedptsq:isAllR"<<allowedptsq.isAllR()<<std::endl;
  // std::cout<<"allowedptsq.isEmpty()"<<allowedptsq.isEmpty()<<std::endl;
 

  if (m_cut_allowedPtSq==allowedptsq)
    return;
  m_cut_pt_allowall = allowedptsq.isAllR();
  m_cut_etaptphi_allwillfail = allowedptsq.isEmpty() || m_cut_allowedEta.isEmpty() || m_cut_allowedPhi.isEmpty();
 
  if (!m_cut_allowedPtSq.contains(allowedptsq)&&!allowedptsq.contains(m_cut_allowedPtSq)) {
    m_cut_allowedPtSq = allowedptsq;
    recheckCutStatusOfAllHandles();
    return;
  }
  bool relaxcut = allowedptsq.contains(m_cut_allowedPtSq);
  m_cut_allowedPtSq = allowedptsq;
  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::setCutAllowedEta(const VP1Interval& allowedEta)
{
  messageVerbose("signal received in setCutAllowedEta ("+allowedEta.toString()+")");
  if (!allowedEta.isSane())
    return;

  if (m_cut_allowedEta==allowedEta)
    return;
  m_cut_eta_allowall = allowedEta.isAllR();
  m_cut_etaptphi_allwillfail = allowedEta.isEmpty() || m_cut_allowedPtSq.isEmpty() || m_cut_allowedPhi.isEmpty();

  if (!m_cut_allowedEta.contains(allowedEta)&&!allowedEta.contains(m_cut_allowedEta)) {
    m_cut_allowedEta = allowedEta;
    recheckCutStatusOfAllHandles();
    return;
  }
  bool relaxcut = allowedEta.contains(m_cut_allowedEta);
  m_cut_allowedEta = allowedEta;
  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::setCutAllowedPhi(const QList<VP1Interval>& allowedPhi)
{
  messageVerbose("signal received in setCutAllowedPhi.");

  if (m_cut_allowedPhi==allowedPhi)
    return;
  m_cut_allowedPhi = allowedPhi;

  m_cut_phi_allowall = m_cut_allowedPhi.count()==1 && m_cut_allowedPhi.at(0).isAllR();
  m_cut_etaptphi_allwillfail = m_cut_allowedPhi.isEmpty() || m_cut_allowedPtSq.isEmpty() || m_cut_allowedEta.isEmpty();

  if (m_cut_phi_allowall) {
    recheckCutStatusOfAllNotVisibleHandles();
    return;
  }
  if (m_cut_etaptphi_allwillfail) {
    recheckCutStatusOfAllVisibleHandles();
    return;
  }
  recheckCutStatusOfAllHandles();
  return;
}

//____________________________________________________________________
void TrackCollHandleBase::setRequiredNHits(const QList<unsigned>& l)
{
  if (!mightHaveSubSystemHitInfo())
    return;//never deal with this...
  if (m_cut_requiredNHits==l)
    return;

  //A bit complex to figure out if the cut change is pure relaxation or tightening:
  bool relaxed(false),tightened(false);
  if (m_cut_requiredNHits.isEmpty()) {
    relaxed = false;
    tightened = true;
  } else {
    if (l.isEmpty()) {
      relaxed = true;
      tightened = false;
    } else {
      assert(l.count()==m_cut_requiredNHits.count());
      relaxed = false;
      tightened = false;
      for (int i=0;i<l.count();++i) {
        if (m_cut_requiredNHits[i]>l[i])
          relaxed=true;
        else if (m_cut_requiredNHits[i]<l[i])
          tightened = true;
      }
    }
  }
  assert(relaxed||tightened);
  m_cut_requiredNHits=l;
  if (relaxed&&tightened) {
    recheckCutStatusOfAllHandles();
  } else {
    if (relaxed)
      recheckCutStatusOfAllNotVisibleHandles();
    else
      recheckCutStatusOfAllVisibleHandles();
  }
}

//____________________________________________________________________
void TrackCollHandleBase::setOnlyVertexAssocTracks(bool b){
  messageVerbose("setOnlyVertexAssocTracks changed to "+str(b));
  
  if (b)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::actualSetCustomColouredTSOSPartsOnHandles()
{
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->setCustomColouredTSOSParts(m_d->customColouredTSOSParts);
  largeChangesEnd();
}

//____________________________________________________________________
void TrackCollHandleBase::actualSetShownTSOSPartsOnHandles()
{
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->setShownTSOSParts(m_d->shownTSOSParts);
  largeChangesEnd();
  
  fillObjectBrowser(); // FIXME! Probably not the right place to call this
}

//____________________________________________________________________
void TrackCollHandleBase::setShownTSOSParts(TrackCommonFlags::TSOSPartsFlags f)
{
  if (m_d->shownTSOSParts==f)
    return;
  m_d->shownTSOSParts=f;
  if (visible()) messageVerbose("shown TSOS parts changed to "+str(f));
  actualSetShownTSOSPartsOnHandles();
}


//____________________________________________________________________
void TrackCollHandleBase::setCustomColouredTSOSParts(TrackCommonFlags::TSOSPartsFlags f)
{
  if (m_d->customColouredTSOSParts==f)
    return;
  m_d->customColouredTSOSParts=f;
  if (visible()) messageVerbose("custom coloured TSOS parts changed to "+str(f));
  actualSetCustomColouredTSOSPartsOnHandles();
}

//____________________________________________________________________
void TrackCollHandleBase::setUseShortTRTMeasurements(bool b)
{
  if (m_d->tsos_useShortTRTMeasurements==b)
    return;
  m_d->tsos_useShortTRTMeasurements=b;
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithMeasurements();
  largeChangesEnd();
}


//____________________________________________________________________
void TrackCollHandleBase::setUseShortMDTMeasurements(bool b)
{
  if (m_d->tsos_useShortMDTMeasurements==b)
    return;
  m_d->tsos_useShortMDTMeasurements=b;
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithMeasurements();
  largeChangesEnd();
}

//____________________________________________________________________
void TrackCollHandleBase::setDrawMeasGlobalPositions(bool b)
{
  // if (visible()) messageVerbose("TrackCollHandleBase::setDrawMeasGlobalPositions => "+str(b));
  
  if (m_d->tsos_drawMeasGlobalPositions==b)
    return;
  m_d->tsos_drawMeasGlobalPositions=b;
  
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithMeasurements();
  largeChangesEnd();
}

//____________________________________________________________________
void TrackCollHandleBase::setMeasurementsShorttubesScale(double n)
{
  if (m_d->tsos_measurementsShorttubesScale==n)
    return;
  m_d->tsos_useShortMDTMeasurements=n;
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithMeasurements();
  largeChangesEnd();
}

//____________________________________________________________________
void TrackCollHandleBase::setNStdDevForParamErrors(const double& nstddev)
{
  if (m_d->tsos_nStdDevForParamErrors==nstddev)
    return;
  m_d->tsos_nStdDevForParamErrors=nstddev;
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithErrors();
  largeChangesEnd();
}

//____________________________________________________________________
void TrackCollHandleBase::setNumberOfPointsOnCircles(int n)
{
  if (m_d->tsos_numberOfPointsOnCircles==n)
    return;
  m_d->tsos_numberOfPointsOnCircles=n;
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithErrors();
  largeChangesEnd();
}

//____________________________________________________________________
void TrackCollHandleBase::setMaterialEffectsOnTrackScale(double n)
{
  if (m_d->tsos_materialEffectsOnTrackScale==n)
    return;
  m_d->tsos_materialEffectsOnTrackScale=n;
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithMaterialEffects();
  largeChangesEnd();
}

//____________________________________________________________________
void TrackCollHandleBase::setParTubeErrorsDrawCylinders(bool b)
{
  if (m_d->tsos_parTubeErrorsDrawCylinders==b)
    return;
  m_d->tsos_parTubeErrorsDrawCylinders=b;
  largeChangesBegin();
  trackHandleIterationBegin();
  TrackHandleBase* handle;
  while ((handle=getNextTrackHandle()))
    handle->updateShapes_TSOSWithErrors();
  largeChangesEnd();
}

//____________________________________________________________________
qint32 TrackCollHandleBase::provideCollTypeID() const
{
  return TrackType::typeToInt(m_type);
}

//____________________________________________________________________
void TrackCollHandleBase::collVisibilityChanged(bool vis)
{
  if (!m_sephelper)
    m_sephelper = new VP1ExtraSepLayerHelper(collSep());

  if (verbose())
    messageVerbose("TrackCollHandleBase::collVisibilityChanged => "+str(vis));
  if (vis){
    recheckCutStatusOfAllNotVisibleHandles();//Fixme -> ofallhandles? All must be not visible anyway...
    fillObjectBrowser();
    if (m_d->objBrowseTree) m_d->objBrowseTree->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //  selectable,  enabled
  }else{
    recheckCutStatusOfAllVisibleHandles();
//    QTreeWidget* trkObjBrowser = common()->controller()->trackObjBrowser();
//    if (m_d->objBrowseTree && trkObjBrowser) {
//      trkObjBrowser->takeTopLevelItem(trkObjBrowser->indexOfTopLevelItem(m_d->objBrowseTree));
//      delete m_d->objBrowseTree; m_d->objBrowseTree=0;
//    }
    // FIXME - need to loop through handles setting pointers to deleted QTreeWidgetItems
    if (m_d->objBrowseTree) m_d->objBrowseTree->setFlags(0); // not selectable, not enabled
  }
  actualSetShownTSOSPartsOnHandles();
  actualSetCustomColouredTSOSPartsOnHandles();
} 

void TrackCollHandleBase::updateObjectBrowserVisibilityCounts() {
  messageVerbose("TrackCollHandleBase::updateObjectBrowserVisibilityCounts called for "+name());
  QTreeWidget* trkObjBrowser = common()->controller()->trackObjBrowser();
  if (!trkObjBrowser || !m_d->objBrowseTree) {
    messageVerbose("TrackCollHandleBase::updateObjectBrowserVisibilityCounts: no common()->controller()->trackObjBrowser() and/or m_d->objBrowseTree. Aborting");
    messageVerbose("trkObjBrowser: "+str(trkObjBrowser)+"\t m_d->objBrowseTree: "+str(m_d->objBrowseTree));
    return; 
  }
  QString text(QString(": (")+QString::number(nShownHandles())+QString("/")+QString::number(m_d->trackhandles.size())+QString(") visible"));
  m_d->objBrowseTree->setText(1, text);
}

void TrackCollHandleBase::fillObjectBrowser()
{
  QTime t;
  t.start();
  
  QTreeWidget* trkObjBrowser = common()->controller()->trackObjBrowser();
  if (!trkObjBrowser) {
    messageVerbose("TrackCollHandleBase::fillObjectBrowser: no common()->controller()->trackObjBrowser(). Aborting");
    return; 
  }
  if (!nShownHandles()) {
    //messageVerbose("TrackCollHandleBase::fillObjectBrowser: no shown handles, so leaving.");
    return; // don't bother with hidden collection
  }
  messageVerbose("TrackCollHandleBase::fillObjectBrowser called for "+name());
  
  trkObjBrowser->setUpdatesEnabled(false);  

  bool firstTime=false;
  if (!m_d->objBrowseTree) {
    m_d->objBrowseTree = new QTreeWidgetItem(0);
    firstTime=true;
    messageVerbose("TrackCollHandleBase::fillObjectBrowser: First time so creating QTreeWidgetItem.");
  } else {
    int index = trkObjBrowser->indexOfTopLevelItem(m_d->objBrowseTree);
    if (index==-1 ) { 
      messageVerbose("Missing from WidgetTree! Will continue but something must be wrong");
    } 
  }

  messageVerbose("TrackCollHandleBase::fillObjectBrowser about to start looping over tracks at "+QString::number(t.elapsed())+" ms");
  
  QList<QTreeWidgetItem *> list;
  trackHandleIterationBegin();
  TrackHandleBase* trkHandle=0;
  unsigned int i=0;
  unsigned int numVisible=0;
  while ((trkHandle=getNextTrackHandle()))
  {
    if (firstTime){
      trkHandle->fillObjectBrowser(list);
    } else {
      trkHandle->updateObjectBrowser();
    }

    // messageVerbose("TrackCollHandleBase::fillObjectBrowser for track completed in "+QString::number(t.elapsed()));

    if (trkHandle->visible() ) numVisible++;
    i++;
  }

  QString text(QString(": (")+QString::number(numVisible)+QString("/")+QString::number(i)+QString(") visible"));

  m_d->objBrowseTree->setText(0, name());
  m_d->objBrowseTree->setText(1, text);
  m_d->objBrowseTree->addChildren(list);
  trkObjBrowser->addTopLevelItem(m_d->objBrowseTree);
  trkObjBrowser->setUpdatesEnabled(true);

  messageVerbose("TrackCollHandleBase::fillObjectBrowser completed in "+QString::number(t.elapsed())+" ms");
  
}

//____________________________________________________________________
void TrackCollHandleBase::assignDefaultMaterial(SoMaterial * m) const
{
  VP1QtInventorUtils::setMatColor( m, defaultColor(), 0.18/*brightness*/ );
}

//____________________________________________________________________
QString TrackCollHandleBase::toString(const COLOURBY&cb)
{
  switch (cb) {
  case COLOUR_PERCOLLECTION: return "COLOUR_PERCOLLECTION";
  case COLOUR_RANDOM: return "COLOUR_RANDOM";
  case COLOUR_MOMENTUM: return "COLOUR_MOMENTUM";
  case COLOUR_CHARGE: return "COLOUR_CHARGE";
  case COLOUR_BYPID: return "COLOUR_BYPID";
  case COLOUR_DISTANCE: return "COLOUR_DISTANCE";
  default: return "Unknown (ERROR)";
  }
}
//____________________________________________________________________
QList<QWidget*> TrackCollHandleBase::provideExtraWidgetsForGuiRow() const
{
  return QList<QWidget*>() << m_d->comboBox_colourby;

}

//____________________________________________________________________
QByteArray TrackCollHandleBase::extraWidgetsState() const
{
  VP1Serialise serialise(0/*version*/,systemBase());
  serialise.save(m_d->comboBox_colourby);
  serialise.disableUnsavedChecks();
  return serialise.result();
}

//____________________________________________________________________
void TrackCollHandleBase::setExtraWidgetsState(const QByteArray& ba)
{
  VP1Deserialise state(ba, systemBase());
  if (state.version()!=0)
    return;//just ignore silently... i guess we ought to warn?
  state.restore(m_d->comboBox_colourby);
  state.disableUnrestoredChecks();
  colourByComboBoxItemChanged();
}

//____________________________________________________________________
void TrackCollHandleBase::colourByComboBoxItemChanged()
{
  messageVerbose("Collection detail level combo box changed index");
  if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByPID())
    setColourBy(COLOUR_BYPID);
  else if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByRandom())
    setColourBy(COLOUR_RANDOM);
  else if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByMomentum())
    setColourBy(COLOUR_MOMENTUM);
  else if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByCharge())
    setColourBy(COLOUR_CHARGE);  
  else if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByDistanceFromSelectedTrack())
    setColourBy(COLOUR_DISTANCE);  
  else if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByVertex())
    setColourBy(COLOUR_VERTEX);
  else
    setColourBy(COLOUR_PERCOLLECTION);
}

//____________________________________________________________________
void TrackCollHandleBase::collMaterialTransparencyAndBrightnessChanged()
{
  messageVerbose("Collection colour changed");
  
  if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByRandom()) {
    largeChangesBegin();
    trackHandleIterationBegin();
    TrackHandleBase* handle;
    while ((handle=getNextTrackHandle())) 
      handle->updateRandomColourTransparencyAndBrightness();
    largeChangesEnd();
  } else {
    updateMaterialOfAllHandles();
  }
}

bool TrackCollHandleBase::useDefaultCuts() const
{
  if (!m_d->matButton) {
    message("ERROR: useDefaultCuts(..) called before init()");
    return true;
  }
  return m_d->matButton->useDefaultCuts();
}

bool TrackCollHandleBase::cutOnlyVertexAssocTracks() const
{
  if (!m_d->matButton) {
    message("ERROR: cutOnlyVertexAssocTracks(..) called before init()");
    return false;
  }
  if (m_d->matButton->useDefaultCuts()) return common()->controller()->cutOnlyVertexAssocTracks();
  return m_d->matButton->cutOnlyVertexAssocTracks();
}




//____________________________________________________________________
void TrackCollHandleBase::setState(const QByteArray&state)
{
  if (!m_d->matButton) {
    message("ERROR: setState(..) called before init()");
    return;
  }
  VP1Deserialise des(state);
  des.disableUnrestoredChecks();
  if (des.version()!=0&&des.version()!=1) {
    messageDebug("Warning: Ignoring state with wrong version");
    return;
  }
  bool vis = des.restoreBool();
    
  QByteArray matState = des.restoreByteArray();
  m_d->matButton->restoreFromState(matState);
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);
}

//____________________________________________________________________
QByteArray TrackCollHandleBase::persistifiableState() const
{
  if (!m_d->matButton) {
    message("ERROR: persistifiableState() called before init()");
    return QByteArray();
  }
  VP1Serialise serialise(1/*version*/);
  serialise.disableUnsavedChecks();
  serialise.save(visible());
  Q_ASSERT(m_d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  serialise.save(m_d->matButton->saveState());
  serialise.save(extraWidgetsState());//version 1+
  return serialise.result();
}
