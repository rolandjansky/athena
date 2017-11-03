/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackSystemController             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#define VP1IMPVARNAME m_d

// FIXME! Remove!
#include <Inventor/C/errors/debugerror.h>
#define protected public
#include <Inventor/nodes/SoSelection.h>
#undef protected

#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/TrackCollWidget.h"
// #include "VP1TrackSystems/TrackObjectBrowserController.h"
#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/AscObj_TSOS.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1TrackSystems/TrackSysCommonData.h"
#include "VP1TrackSystems/TrackHandle_TrkTrack.h"

#include "ui_vp1trackcontrollerform.h"
#include "ui_settings_ascobjs_form.h"
#include "ui_settings_colouring_form.h"
#include "ui_settings_extrapolation_form.h"
#include "ui_settings_interactions_form.h"
#include "ui_settings_projections_form.h"
#include "ui_settings_cuts_form.h"
#include "ui_shift_muon_chamber_form.h"
#include "ui_objectbrowser.h"

#include "VP1Utils/VP1AvailableToolsHelper.h"
#include "VP1Utils/VP1ToolAccessHelper.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1SoMaterialMixer.h"
#include "VP1Base/SoCooperativeSelection.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/Volume.h"

#include "Inventor/nodes/SoMaterial.h"
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoComplexity.h"
#include "Inventor/nodes/SoLightModel.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoSearchAction.h>

#include "CLHEP/Units/SystemOfUnits.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>




//____________________________________________________________________
class TrackSystemController::Imp {
public:
  TrackSystemController * theclass;
  Ui::VP1TrackControllerForm ui;
  TrackCollWidget * trackcollwidget;
  void initMaterials();
  SoMaterial * createMaterial(const int& r,const int& g,const int& b) const;
  SoMaterial * getMat(VP1MaterialButton*) const;
  SoMaterial * materialFallback;
  // SoDrawStyle * trackDrawStyle;
  SoDrawStyle * ascObjDrawStyle;
  SoComplexity * ascObjComplexity;
  // SoLightModel * trackLightModel;

  QString restoredLastPropagator;
  QString restoredLastFitter;
  QStringList lastUpdatedAvailableExtrapolators;
  QStringList lastUpdatedAvailableFitters;
  bool updateComboBoxContents(QComboBox*cb,QStringList l,QString& restoredSelection);
  static const QString noneAvailString;
  unsigned numberOfSelectedPRDs;
  unsigned numberOfSelectedTracks;
  void updateFitPRDButtonState();

  VP1SoMaterialMixer * matmixer;
  VP1AvailableToolsHelper *  toolhelper_extrapolators;
  VP1AvailableToolsHelper *  toolhelper_fitters;
  VP1ToolAccessHelper * toolaccesshelper;
  TrackCommonFlags::TrackPartsFlags last_shownTrackParts;
  int last_vertexProjectionAngle;
  double last_trackTubeRadius;
  TrackLabelModes last_trackLabels;
  float last_trackLabelTrkOffset;
  QList<int> last_trackLabelPosOffset;
  Trk::IExtrapolator * last_propagator;
  // bool last_ignoreMeasurementEffectsOnTrackInProp;  
  PropagationOptionFlags last_propagationOptions;
  float last_propMaxRadius;
  Trk::ITrackFitter * last_trackFitter;
  Muon::MuonEDMPrinterTool * muonedmprintertool;
  TrackCommonFlags::SELECTIONMODE last_selectionMode;
  bool last_showTruthAscObjs;
  TrackCommonFlags::DETAILLEVEL last_assocObjDetailLevel;
  TrackCommonFlags::TSOSPartsFlags last_shownTSOSParts;
  TrackCommonFlags::TSOSPartsFlags last_customColouredTSOSParts;
  bool last_useShortTRTMeasurements;
  bool last_useShortMDTMeasurements;
  bool last_drawMeasGlobalPositions;
  double last_measurementsShorttubesScale;
  double last_nStdDevForParamErrors;
  bool last_parTubeErrorsDrawCylinders;
  int last_numberOfPointsOnCircles;
  double last_materialEffectsOnTrackScale;
  VP1Interval last_cutAllowedPt;
  VP1Interval last_cutAllowedP;
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;
  QList<unsigned> last_cutRequiredNHits;
  bool last_cutTruthFromIROnly;
  bool last_cutExcludeBarcodeZero;
  bool last_cutTruthExcludeNeutrals;
  bool last_cutOnlyVertexAssocTracks;
  bool last_showTotMomentumOnMultiTrackSelection;

  Ui::TrackSysSettingsColouringForm ui_col;
  Ui::TrackSysSettingsExtrapolationsForm ui_extrap;
  Ui::TrackSysSettingsInteractionsForm ui_int;
  Ui::TrackSysSettingsProjectionsForm ui_proj;
  Ui::TrackSysSettingsCutsForm ui_cuts;
  Ui::TrackSysSettingsAscObjsForm ui_ascobjs;
  Ui::TrackSysShiftMuonChambersForm ui_shiftmuonchambers;
  Ui::ObjectBrowser ui_objBrowser;
  
  static bool extrapolatorsCreated;
  static void ensureExtrapolatorsCreated(IVP1System *);
  static bool fittersCreated;
  static void ensureFittersCreated(IVP1System *);
  
  QTreeWidget* objBrowserWidget;
  TrackSysCommonData * common;
  
  // Added because TrackingVolumeSvc doesn't work any more. Can remove when we move to new extrapolator
  Trk::Volume* calorimeterEntryLayer;
  Trk::Volume* muonSpectrometerEntryLayer;
  Trk::Volume* muonSpectrometerExitLayer;
};

const QString TrackSystemController::Imp::noneAvailString = QString("None available");


//____________________________________________________________________
QString TrackSystemController::toString( const QList<unsigned>& l )
{
  QString s;
  foreach(unsigned i,l) {
    if (!s.isEmpty()) s+=", ";
    s+=QString::number(i);
  }
  return "["+s+"]";
}

//____________________________________________________________________
QString TrackSystemController::toString( const QList<int>& l )
{
  QString s;
  foreach(int i,l) {
    if (!s.isEmpty()) s+=", ";
    s+=QString::number(i);
  }
  return "["+s+"]";
}

//____________________________________________________________________
bool TrackSystemController::Imp::extrapolatorsCreated = false;
void TrackSystemController::Imp::ensureExtrapolatorsCreated(IVP1System * sys) {
  if (extrapolatorsCreated||!sys)
    return;
  extrapolatorsCreated = true;

  //Get available extrapolators:
  QString tooltype("Trk::Extrapolator");
  VP1AvailableToolsHelper * availTools = new VP1AvailableToolsHelper(sys);
  availTools->addMonitoredType(tooltype);
  QStringList existingExtrapolators =  availTools->availableTools();
  delete availTools;

  VP1ToolAccessHelper toolaccess(sys);
  foreach (QString key,VP1QtUtils::environmentVariableValue("VP1_JOBCFG_EXTRA_VP1_EXTRAPOLATORS").split(';',QString::SkipEmptyParts)) {
    if (existingExtrapolators.contains(key))
      continue;
    sys->messageDebug("Attempting creation of fitter with tooltype/key "+key);
    Trk::IExtrapolator * tool = toolaccess.getToolPointer<Trk::IExtrapolator>(key,false/*silent*/,true/*create if not exists*/);
    if (!tool) {
      sys->message("WARNING: Could not create tool type/key "+key);
    }

  }
}

//____________________________________________________________________
bool TrackSystemController::Imp::fittersCreated = false;
void TrackSystemController::Imp::ensureFittersCreated(IVP1System * sys) {
  if (fittersCreated||!sys)
    return;
  fittersCreated = true;

  //Get available extrapolators:
  QStringList fittertypes;
  fittertypes << "Trk::KalmanFitter"
        << "Trk::GlobalChi2Fitter"
        << "Trk::GaussianSumFilter"
        << "Trk::DistributedKalmanFilter"
        << "Trk::DeterministicAnnealingFilter";
  VP1AvailableToolsHelper * availTools = new VP1AvailableToolsHelper(sys);
  availTools->addMonitoredTypes(fittertypes);
  QStringList existingFitters =  availTools->availableTools();
  delete availTools;

  VP1ToolAccessHelper toolaccess(sys);
  foreach (QString instance,VP1QtUtils::environmentVariableValue("VP1_JOBCFG_EXTRA_VP1_FITTERS").split(';',QString::SkipEmptyParts)) {
    if (existingFitters.contains(instance))
      continue;
    sys->messageDebug("Attempting creation of fitter with tooltype/key "+instance);
    Trk::ITrackFitter * tool = toolaccess.getToolPointer<Trk::ITrackFitter>(instance,false/*silent*/,true/*create if not exists*/);
    if (!tool) {
      sys->message("WARNING: Could not create tool with type/key "+instance);
    }
  }
}

//____________________________________________________________________
TrackSystemController::TrackSystemController(IVP1System * sys)
  : VP1Controller(sys,"TrackSystemController"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->lastUpdatedAvailableExtrapolators = QStringList("<dummy>");//special.
  m_d->lastUpdatedAvailableFitters = QStringList("<dummy>");//special.

  //Stuff with tools waits until ::initTools() is called:
  m_d->toolaccesshelper = 0;
  m_d->toolhelper_extrapolators = 0;
  m_d->toolhelper_fitters = 0;
  m_d->matmixer = 0;

  m_d->ui.setupUi(this);
  m_d->trackcollwidget = new TrackCollWidget;
  setupCollWidgetInScrollArea(m_d->ui.collWidgetScrollArea,m_d->trackcollwidget);

  initDialog(m_d->ui_col, m_d->ui.pushButton_settings_colouring);
  initDialog(m_d->ui_extrap, m_d->ui.pushButton_settings_extrapolation);
  initDialog(m_d->ui_int, m_d->ui.pushButton_settings_interactions);
  initDialog(m_d->ui_proj, m_d->ui.pushButton_settings_projections);
  initDialog(m_d->ui_cuts, m_d->ui.pushButton_settings_cuts);
  initDialog(m_d->ui_ascobjs, m_d->ui.pushButton_settings_details);
  initDialog(m_d->ui_objBrowser, m_d->ui.pushButton_ObjectBrowser);

  //init:
  m_d->numberOfSelectedPRDs=1;//anything != 0
  setNumberOfSelectedPRDsAndTracks(0,0);
  
  m_d->initMaterials();

  m_d->materialFallback = new SoMaterial;
  m_d->materialFallback->ref();

  m_d->ui_cuts.etaPhiCutWidget->setSystemBasePointer(systemBase());

  //Draw Styles / Complexity:
  // VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->ui_col.horizontalSlider_trackWidth);
  // VP1QtInventorUtils::setValueLineWidthSlider(m_d->ui_col.horizontalSlider_trackWidth,1.0);
  // VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->ui_ascobjs.horizontalSlider_linewidths);
  // VP1QtInventorUtils::setValueLineWidthSlider(m_d->ui_ascobjs.horizontalSlider_linewidths,1.0);
  // VP1QtInventorUtils::setLimitsPointSizeSlider(m_d->ui_ascobjs.horizontalSlider_pointsizes);
  // VP1QtInventorUtils::setValuePointSizeSlider(m_d->ui_ascobjs.horizontalSlider_pointsizes,3.0);

  // m_d->trackDrawStyle = new SoDrawStyle;
  // m_d->trackDrawStyle->setName("TrackDrawStyle");
  // m_d->trackDrawStyle->ref();
  //  updateTrackDrawStyle();

  // addUpdateSlot(SLOT(updateTrackDrawStyle()));
  // connectToLastUpdateSlot(m_d->ui_col.horizontalSlider_trackWidth);

  m_d->ascObjDrawStyle = new SoDrawStyle;
  m_d->ascObjDrawStyle->setName("AscObjDrawStyle");
  m_d->ascObjDrawStyle->ref();
  addUpdateSlot(SLOT(updateAscObjDrawStyle()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.horizontalSlider_linewidths);
  connectToLastUpdateSlot(m_d->ui_ascobjs.horizontalSlider_pointsizes);

  m_d->ascObjComplexity = new SoComplexity;
  m_d->ascObjComplexity->setName("AscObjComplexity");
  m_d->ascObjComplexity->ref();
  addUpdateSlot(SLOT(updateAscObjComplexity()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.horizontalSlider_complexity);

  // m_d->trackLightModel = new SoLightModel;
  // m_d->trackLightModel->setName("TrackLightModel");
  // m_d->trackLightModel->ref();
  // addUpdateSlot(SLOT(updateTrackLightModel()));
  // connectToLastUpdateSlot(m_d->ui_col.checkBox_tracksUseBaseLightModel);

  //Refit ui is dependent on env variable:
  m_d->ui_int.radioButton_selmode_trackfits->setVisible(VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_ENABLEREFIT"));
  m_d->ui_int.groupBox_refitting->setEnabled(VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_ENABLEREFIT"));
  connect(m_d->ui_int.comboBox_fitterMode,SIGNAL(currentIndexChanged(int)),this,SLOT(updateFitPRDButtonState()));
  
  //m_d->ui_ascobjs.checkBox_usecolour_meas_outliers->setEnabled(false);
  //m_d->ui_ascobjs.matButton_meas_outliers->setEnabled(false);
  //m_d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale->setEnabled(false);
  //m_d->ui_ascobjs.label_mateffects_scale->setEnabled(false);
  // m_d->ui_ascobjs.checkBox_materialeffectsontrack_forceposontrack->setEnabled(false);
  // m_d->ui_extrap.groupBox_otheroptions->setEnabled(false);
  // m_d->ui_extrap.radioButton_helical->setEnabled(false);


  //Disable elements based on job configuration:

  if (!VP1JobConfigInfo::hasMuonGeometry()) {
    m_d->ui_proj.checkBox_projections_muonchambers->setChecked(false);
    m_d->ui_proj.checkBox_projections_muonchambers->setEnabled(false);
  }
  if (!(VP1JobConfigInfo::hasPixelGeometry()
        &&VP1JobConfigInfo::hasSCTGeometry()
        &&VP1JobConfigInfo::hasTRTGeometry())) {
    m_d->ui_proj.checkBox_projections_indet->setChecked(false);
    m_d->ui_proj.checkBox_projections_indet->setEnabled(false);
  }

  //These we init explicitly since we might otherwise trigger a change
  //before initlastvars (fixme: is this true and the right fix??)
  m_d->last_propagator = 0;
  m_d->last_trackFitter = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // -> shownTrackParts
  addUpdateSlot(SLOT(possibleChange_shownTrackParts()));
  // connectToLastUpdateSlot(m_d->ui_col.checkBox_hideactualpaths);
  connectToLastUpdateSlot(m_d->ui_proj.checkBox_projections_muonchambers);
  connectToLastUpdateSlot(m_d->ui_proj.checkBox_projections_indet);
  connectToLastUpdateSlot(m_d->ui_proj.groupBox_projections_vertex);

  addUpdateSlot(SLOT(possibleChange_vertexProjectionAngle()));
  connectToLastUpdateSlot(m_d->ui_proj.spinBox_projections_vertex);


  // // -> trackTubeRadius
  // addUpdateSlot(SLOT(possibleChange_trackTubeRadius()));
  // connectToLastUpdateSlot(m_d->ui_col.checkBox_trackTubes);
  // connectToLastUpdateSlot(m_d->ui_col.doubleSpinBox_trackTubesRadiusMM);

  // -> track labels
  addUpdateSlot(SLOT(possibleChange_trackLabels()));
  connectToLastUpdateSlot(m_d->ui_col.checkBox_trkLabels_Pt);
  connectToLastUpdateSlot(m_d->ui_col.checkBox_trkLabels_p);
  connectToLastUpdateSlot(m_d->ui_col.checkBox_trkLabels_direction);
  connectToLastUpdateSlot(m_d->ui_col.checkBox_trkLabels_hits);
  connectToLastUpdateSlot(m_d->ui_col.checkBox_trkLabels_pid);
  connectToLastUpdateSlot(m_d->ui_col.checkBox_trkLabels_fitQuality);
  connectToLastUpdateSlot(m_d->ui_col.groupBox_labels);
  
  addUpdateSlot(SLOT(possibleChange_trackLabelTrkOffset()));
  connectToLastUpdateSlot(m_d->ui_col.horizontalSlider_labels_trkOffset);

  addUpdateSlot(SLOT(possibleChange_trackLabelPosOffset()));
  connectToLastUpdateSlot(m_d->ui_col.horizontalSlider_labels_xOffset);
  connectToLastUpdateSlot(m_d->ui_col.horizontalSlider_labels_yOffset);
  connectToLastUpdateSlot(m_d->ui_col.horizontalSlider_labels_zOffset);

  
  // -> shownTSOSParts
  addUpdateSlot(SLOT(possibleChange_shownTSOSParts()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.groupBox_measurements);
  connectToLastUpdateSlot(m_d->ui_ascobjs.groupBox_parameters);
  connectToLastUpdateSlot(m_d->ui_ascobjs.groupBox_errors);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_parametererrors);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_measurementerrors);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_parerror_hideperigeeerrors);
  connectToLastUpdateSlot(m_d->ui_ascobjs.groupBox_materialeffectsontrack);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE);
  connectToLastUpdateSlot(m_d->ui_ascobjs.groupBox_surfaces);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces);
  
  addUpdateSlot(SLOT(possibleChange_drawMeasGlobalPositions()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_measurements_drawGP);

  // -> customColouredTSOSParts
  addUpdateSlot(SLOT(possibleChange_customColouredTSOSParts()));

  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_usecolour_measurements);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_usecolour_meas_outliers);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_usecolour_parameters);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_useHoleColour_parameters);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_usecolour_parametererrors);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack);
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_usecolour_surfaces);

  // -> useShortTRTMeasurements
  addUpdateSlot(SLOT(possibleChange_useShortTRTMeasurements()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_measurements_shorttubes_trt);

  // -> useShortMDTMeasurements
  addUpdateSlot(SLOT(possibleChange_useShortMDTMeasurements()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_measurements_shorttubes_mdt);

  // -> useShortMDTMeasurements
  addUpdateSlot(SLOT(possibleChange_measurementsShorttubesScale()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale);

  // -> nStdDevForParamErrors
  addUpdateSlot(SLOT(possibleChange_nStdDevForParamErrors()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.doubleSpinBox_parerror_stddev);

  // -> parTubeErrorsDrawCylinders
  addUpdateSlot(SLOT(possibleChange_parTubeErrorsDrawCylinders()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_parerror_drawcylinder);

  // -> numberOfPointsOnCircles
  addUpdateSlot(SLOT(possibleChange_numberOfPointsOnCircles()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.horizontalSlider_complexity);

  // -> materialEffectsOnTrackScale
  addUpdateSlot(SLOT(possibleChange_materialEffectsOnTrackScale()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale);

  // -> selectionMode
  addUpdateSlot(SLOT(possibleChange_selectionMode()));
  connectToLastUpdateSlot(m_d->ui_int.radioButton_selmode_single);
  connectToLastUpdateSlot(m_d->ui_int.radioButton_selmode_multitracks);
  connectToLastUpdateSlot(m_d->ui_int.radioButton_selmode_trackfits);

  // -> showTruthAscObjs
  addUpdateSlot(SLOT(possibleChange_showTruthAscObjs()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.checkBox_truthtracks_display_points);

  // -> assocObjDetailLevel
  addUpdateSlot(SLOT(possibleChange_assocObjDetailLevel()));
  connectToLastUpdateSlot(m_d->ui_ascobjs.comboBox_assocobj_detaillevel);

  // -> cutAllowedP/Pt
  addUpdateSlot(SLOT(possibleChange_cutAllowedPt()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_minpt);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_maxpt);
  connectToLastUpdateSlot(m_d->ui_cuts.doubleSpinBox_cut_minpt_gev);
  connectToLastUpdateSlot(m_d->ui_cuts.doubleSpinBox_cut_maxpt_gev);
  connectToLastUpdateSlot(m_d->ui_cuts.comboBox_momtype);

  // -> cutAllowedEta
  addUpdateSlot(SLOT(possibleChange_cutAllowedEta()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  // -> cutAllowedPhi
  addUpdateSlot(SLOT(possibleChange_cutAllowedPhi()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  // -> cutRequiredNHits();
  addUpdateSlot(SLOT(possibleChange_cutRequiredNHits()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_nhits_pixel);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_nhits_sct);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_nhits_trt);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_nhits_muon);
  connectToLastUpdateSlot(m_d->ui_cuts.spinBox_cut_nhits_pixel);
  connectToLastUpdateSlot(m_d->ui_cuts.spinBox_cut_nhits_sct);
  connectToLastUpdateSlot(m_d->ui_cuts.spinBox_cut_nhits_trt);
  connectToLastUpdateSlot(m_d->ui_cuts.spinBox_cut_nhits_muon);

  // -> cutTruthFromIROnly
  addUpdateSlot(SLOT(possibleChange_cutTruthFromIROnly()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR);

  // -> cutExcludeBarcodeZero
  addUpdateSlot(SLOT(possibleChange_cutExcludeBarcodeZero()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0);

  // -> cutTruthExcludeNeutrals
  addUpdateSlot(SLOT(possibleChange_cutTruthExcludeNeutrals()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals);

  // -> cutOnlyVertexAssocTracks
  addUpdateSlot(SLOT(possibleChange_cutOnlyVertexAssocTracks()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_vertexAssociated);

  // -> showTotMomentumOnMultiTrackSelection
  addUpdateSlot(SLOT(possibleChange_showTotMomentumOnMultiTrackSelection()));
  connectToLastUpdateSlot(m_d->ui_int.checkBox_sel_showtotmom);

  // Special: -> rerandomise
  connect(m_d->ui_col.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));

  // Special: -> refit
  connect(m_d->ui_int.pushButton_refit,SIGNAL(clicked()),this,SLOT(emitRefit()));

  // TrackObjBrowser
  messageVerbose("Enabling object browser");
  m_d->objBrowserWidget = m_d->ui_objBrowser.treeWidget;
  m_d->objBrowserWidget->setSortingEnabled(false);
  QStringList l; 
  l<<"Object"<<"Information";
  m_d->objBrowserWidget->setHeaderLabels(l);
  connect(m_d->objBrowserWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(objectBrowserClicked(QTreeWidgetItem *, int)));
    
  // Special alignment tests
  if (VP1QtUtils::environmentVariableIsOn("VP1_TRKSYS_SHIFTCHAMBERS")) {
    messageVerbose("Enabling shifting of Muon Chambers");
    QWidget *widget = new QWidget;
    m_d->ui_shiftmuonchambers.setupUi(widget);
    messageVerbose("Making connection");
    widget->show();
    connect(m_d->ui_shiftmuonchambers.pushButton_Update,SIGNAL(clicked()),sys,SLOT(updateAlignment()));
  }

  // we want "Print information" on single track selection turned ON by default
  m_d->ui_int.checkBox_selsingle_printinfo->setChecked(true);
  
  // Since TrkVolumesSvc isn't working anymore, hardcode values. (Remove when we move to new extrapolator)
  m_d->calorimeterEntryLayer      = new Trk::Volume(0, new Trk::CylinderVolumeBounds(1100.0, 3200.0));
  m_d->muonSpectrometerEntryLayer = new Trk::Volume(0, new Trk::CylinderVolumeBounds(4250.0, 6779.0));
  m_d->muonSpectrometerExitLayer  = new Trk::Volume(0, new Trk::CylinderVolumeBounds(15000.0, 21000.0)); // FIXME! Put in correct values. EJWM
  initLastVars();
}

//____________________________________________________________________
void TrackSystemController::initTools()
{
  m_d->toolaccesshelper = new VP1ToolAccessHelper(systemBase());

  if (VP1JobConfigInfo::hasPixelGeometry()||VP1JobConfigInfo::hasSCTGeometry()
      ||VP1JobConfigInfo::hasTRTGeometry()||VP1JobConfigInfo::hasMuonGeometry()) {

    //Ensure special VP1 extrapolator and fitters are created:
    Imp::ensureExtrapolatorsCreated(systemBase());
    Imp::ensureFittersCreated(systemBase());

    messageVerbose("Setting up tool helper to monitor extrapolators - start");
    m_d->toolhelper_extrapolators = new VP1AvailableToolsHelper(systemBase(),this);
    //Watch for extrapolators (and Fatras extrapolators are no-go!)
    m_d->toolhelper_extrapolators->addMonitoredType("Trk::Extrapolator",(QStringList() << "*Fatras*"));
    connect(m_d->toolhelper_extrapolators,SIGNAL(availableToolsChanged(const QStringList&)),
	    this,SLOT(availableExtrapolatorsChanged(const QStringList&)));
    availableExtrapolatorsChanged(m_d->toolhelper_extrapolators->availableTools());
    messageVerbose("Setting up tool helper to monitor extrapolators - end");

    messageVerbose("Setting up tool helper to monitor fitters - start");
    m_d->toolhelper_fitters = new VP1AvailableToolsHelper(systemBase(),this);
    QStringList fittertypes;
    fittertypes << "Trk::KalmanFitter"
                << "Trk::GlobalChi2Fitter"
                << "Trk::GaussianSumFilter"
                << "Trk::DistributedKalmanFilter"
                << "Trk::DeterministicAnnealingFilter";
    m_d->toolhelper_fitters->addMonitoredTypes(fittertypes);
    connect(m_d->toolhelper_fitters,SIGNAL(availableToolsChanged(const QStringList&)),
	    this,SLOT(availableFittersChanged(const QStringList&)));
    availableFittersChanged(m_d->toolhelper_fitters->availableTools());
    messageVerbose("Setting up tool helper to monitor fitters - end");
  } else {
    m_d->toolhelper_extrapolators = 0;
    m_d->toolhelper_fitters = 0;
    m_d->ui_extrap.comboBox_propagator->clear();
    m_d->ui_extrap.comboBox_propagator->addItem(Imp::noneAvailString);
    m_d->ui_extrap.comboBox_propagator->setEnabled(false);
    m_d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(false);
    m_d->ui_extrap.radioButton_none->setChecked(true);
    m_d->ui_int.comboBox_fitters->clear();
    m_d->ui_int.comboBox_fitters->addItem(Imp::noneAvailString);
    m_d->ui_int.comboBox_fitters->setEnabled(false);
    //FIXME: Disable radioButton_selmode_trackfits, comboBox_fitters, lineEdit_fittedTrackCollName, pushButton_refit
  }

  // -> propagator
  //
  addUpdateSlot(SLOT(possibleChange_propagator()));
  connectToLastUpdateSlot(m_d->ui_extrap.radioButton_none);
  connectToLastUpdateSlot(m_d->ui_extrap.radioButton_athenaExtrapolator);
  connectToLastUpdateSlot(m_d->ui_extrap.comboBox_propagator);

  addUpdateSlot(SLOT(possibleChange_propagationOptions()));
  connectToLastUpdateSlot(m_d->ui_extrap.checkBox_ignoreMEoT); 
  connectToLastUpdateSlot(m_d->ui_extrap.checkBox_extendAllInDetTracks); 
  // connectToLastUpdateSlot(m_d->ui_extrap.comboBox_extendAllInDetTracksToHere); 
  m_d->last_propagationOptions=TrackSystemController::NoPropOptions;
  if (m_d->ui_extrap.checkBox_ignoreMEoT->isChecked())    m_d->last_propagationOptions |= TrackSystemController::IgnoreMEOT; //!< Really needed here??? FIXME
  if (m_d->ui_extrap.checkBox_extendAllInDetTracks->isChecked())    m_d->last_propagationOptions |= TrackSystemController::ExtendTrack; //!< Really needed here??? FIXME

  addUpdateSlot(SLOT(possibleChange_propMaxRadius()));
  connectToLastUpdateSlot(m_d->ui_extrap.checkBox_maxRadius); 
  connectToLastUpdateSlot(m_d->ui_extrap.spinBox_maxRadiusValue); 

  connect(m_d->ui_extrap.comboBox_extendAllInDetTracksToHere, SIGNAL(currentIndexChanged(int)),this, SLOT(emitExtrapolateToHereChanged(int)));
  // -> trackFitter
  addUpdateSlot(SLOT(possibleChange_trackFitter()));
  //(Nothing connects directly to this slot)

  //could also use testForChanges() here:
  possibleChange_propagator();
  possibleChange_trackFitter();
}

//____________________________________________________________________
SoMaterial * TrackSystemController::Imp::createMaterial(const int& r,const int& g,const int& b) const
{
  return VP1MaterialButton::createMaterial(r/255.0,g/255.0,b/255.0,0.2/*brightness*/);
}

//____________________________________________________________________
void TrackSystemController::Imp::initMaterials()
{
  //By PID => Electrons and muons:.
  ui_col.matButton_electrons->setMaterial(createMaterial(205,103,255));//purple
  ui_col.matButton_muons->setMaterial(createMaterial(71,255,51));//green

  //By PID => Charged: bluish colours
  ui_col.matButton_pions->setMaterial(createMaterial(74,146,255));
  ui_col.matButton_protons->setMaterial(createMaterial(56,49,255));
  ui_col.matButton_chargedkaons->setMaterial(createMaterial(116,255,228));
  ui_col.matButton_othercharged->setMaterial(createMaterial(142,163,255));

  //By PID => Neutral yellowish colours.
  ui_col.matButton_neutrons->setMaterial(createMaterial(245,255,42));
  ui_col.matButton_photons->setMaterial(createMaterial(255,227,82));
  ui_col.matButton_neutrinos->setMaterial(createMaterial(255,152,61));
  ui_col.matButton_otherneutrals->setMaterial(createMaterial(248,255,102));

  //By charge:
  ui_col.matButton_charge_pos->setMaterial(createMaterial(255,71,10));
  ui_col.matButton_charge_neg->setMaterial(createMaterial(116,255,10));
  ui_col.matButton_charge_neutral->setMaterial(createMaterial(255,227,82));

  //By momentum:
  ui_col.matButton_0GeV->setMaterial(createMaterial(0,50,0));
  ui_col.matButton_15GeV->setMaterial(createMaterial(0,255,0));

  //Associated objects:
  ui_ascobjs.matButton_measurements->setMaterial(createMaterial(55,200,55));
  ui_ascobjs.matButton_meas_outliers->setMaterial(createMaterial(155,155,155));
  ui_ascobjs.matButton_parameters->setMaterial(createMaterial(100,150,200));
  ui_ascobjs.matButton_holeParameters->setMaterial(createMaterial(100,100,100));
  ui_ascobjs.matButton_parerrors->setMaterial(createMaterial(255,100,0));
  SoMaterial * materialeffectsontrackMat = createMaterial(200,255,75);
  materialeffectsontrackMat->transparency=0.5;
  ui_ascobjs.matButton_materialeffectsontrack->setMaterial(materialeffectsontrackMat);
  ui_ascobjs.matButton_surfaces->setMaterial(createMaterial(228,210,165));
}

//____________________________________________________________________
TrackSystemController::~TrackSystemController()
{
  messageVerbose("~TrackSystemController begin");
  delete m_d->toolaccesshelper;
  delete m_d->matmixer;
  m_d->materialFallback->unref();
  // m_d->trackDrawStyle->unref();
  // m_d->trackLightModel->unref();
  m_d->ascObjDrawStyle->unref();
  m_d->ascObjComplexity->unref();
  
  delete m_d->calorimeterEntryLayer     ;
  delete m_d->muonSpectrometerEntryLayer;
  delete m_d->muonSpectrometerExitLayer ;
    
    // delete m_d->objBrowserWidget;
  delete m_d;
  messageVerbose("~TrackSystemController end");
}

//____________________________________________________________________
int TrackSystemController::currentSettingsVersion() const
{
  return 17;
}

//____________________________________________________________________
void TrackSystemController::actualSaveSettings(VP1Serialise&s) const
{
	messageVerbose("TrackSystemController::actualSaveSettings()");

	/*
	 * NOTE!!! Important, about serialization:
	 *
	 * - order has to be the same between saving-reading
	 *
	 * See: http://www.mimec.org/node/348
	 *
	 */

	messageDebug("Serialize - current version: "+QString::number( s.version() ));

	// saving a string, which will be checked while restoring the values
	s.save(QString("TrackSystemSettings"));

  //versions <= 3 saved an integer here

  //Display options: linewidth: 
  // s.save(VP1QtInventorUtils::getValueLineWidthSlider(m_d->ui_col.horizontalSlider_trackWidth));//Version 1+ GONE WITH VERSION 17  
  // s.widgetHandled(m_d->ui_col.horizontalSlider_trackWidth);

  //Tracks base light model:
  // s.save(m_d->ui_col.checkBox_tracksUseBaseLightModel);//version 4+ GONE WITH VERSION 17 

	// --- Projections options ---
	s.save(QString("Projections options"));
	s.save(m_d->ui_proj.checkBox_projections_indet);
	s.save(m_d->ui_proj.checkBox_projections_muonchambers);
	s.save(m_d->ui_proj.groupBox_projections_vertex); //v15
	s.save(m_d->ui_proj.spinBox_projections_vertex);//v15
	s.save(m_d->ui_proj.horizontalSlider_projections_vertex);//v15
	// s.save(m_d->ui_col.checkBox_hideactualpaths); GONE WITH VERSION 17
	// ---------------------------


  //Display options - track tubes
  // s.save(m_d->ui_col.checkBox_trackTubes);//version 8+ GONE WITH VERSION 17 
  // s.save(m_d->ui_col.doubleSpinBox_trackTubesRadiusMM);//version 8+ GONE WITH VERSION 17 

  //Version <= 3 had bool here


  // ----- Propagation options -----
  s.save(QString("Propagation options"));
  s.save(m_d->ui_extrap.radioButton_none,
		 m_d->ui_extrap.radioButton_helical,
		 m_d->ui_extrap.radioButton_athenaExtrapolator);//version 4+
  s.save(m_d->ui_extrap.comboBox_propagator);//NB: We restore this in a slightly special way
  // -----------------------------------




  // ----- Interactions options -----
  s.save(QString("Interactions options"));
  s.save(m_d->ui_int.lineEdit_fittedTrackCollName);//version 6+
  s.save(m_d->ui_int.comboBox_fitters);//version 6+
  // -----------------------------------



  //Version <=2 had bool here

  //version 0 output an integer followed by two bools here

  //Display options - Trk::Tracks
  //Version <=2 had bool here
  s.save(m_d->ui_ascobjs.comboBox_assocobj_detaillevel);
  //Version <=2 had two bool here

  //Display options - Truth tracks:
  s.save(m_d->ui_ascobjs.checkBox_truthtracks_display_points);

  // --- Display options - Colours ---
  s.save(QString("Colour options"));
  //Display options - Colour by pdg:
  s.save(m_d->ui_col.matButton_electrons);
  s.save(m_d->ui_col.matButton_muons);
  s.save(m_d->ui_col.matButton_pions);
  s.save(m_d->ui_col.matButton_protons);
  s.save(m_d->ui_col.matButton_chargedkaons);
  s.save(m_d->ui_col.matButton_othercharged);
  s.save(m_d->ui_col.matButton_neutrons);
  s.save(m_d->ui_col.matButton_photons);
  s.save(m_d->ui_col.matButton_neutrinos);
  s.save(m_d->ui_col.matButton_otherneutrals);

  //Display options - Colour by charge:
  s.save(m_d->ui_col.matButton_charge_neg);//version 2+
  s.save(m_d->ui_col.matButton_charge_pos);//version 2+
  s.save(m_d->ui_col.matButton_charge_neutral);//version 2+

  //Display options - Colour by momentum:
  s.save(m_d->ui_col.matButton_0GeV);//version 2+
  s.save(m_d->ui_col.matButton_15GeV);//version 2+
  // -----------------------------------








  // --- Cuts options ---
  s.save(QString("Cuts options"));
  //Cuts - general:
  s.save(m_d->ui_cuts.checkBox_cut_minpt);
  s.save(m_d->ui_cuts.doubleSpinBox_cut_minpt_gev);
  s.save(m_d->ui_cuts.checkBox_cut_maxpt);
  s.save(m_d->ui_cuts.doubleSpinBox_cut_maxpt_gev);
  s.save(m_d->ui_cuts.comboBox_momtype);// version 10+

  s.save(m_d->ui_cuts.etaPhiCutWidget);//Version 7+
  //Versions <=6 had 2*bool, 2*double, 1*bool, 1*obsoletephisectionstate here.

  //Cuts - number of hits:
  s.save(m_d->ui_cuts.checkBox_cut_nhits_pixel);//Version 9+
  s.save(m_d->ui_cuts.checkBox_cut_nhits_sct);//Version 9+
  s.save(m_d->ui_cuts.checkBox_cut_nhits_trt);//Version 9+
  s.save(m_d->ui_cuts.checkBox_cut_nhits_muon);//Version 9+
  s.save(m_d->ui_cuts.spinBox_cut_nhits_pixel);//Version 9+
  s.save(m_d->ui_cuts.spinBox_cut_nhits_sct);//Version 9+
  s.save(m_d->ui_cuts.spinBox_cut_nhits_trt);//Version 9+
  s.save(m_d->ui_cuts.spinBox_cut_nhits_muon);//Version 9+

  //Cuts - truth:
  s.save(m_d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR);
  s.save(m_d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals);
  s.save(m_d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0);

  s.save(m_d->ui_cuts.checkBox_vertexAssociated);//Version 14+
  // -----------------------------------






  // --- Interactions options ---
    s.save(QString("Interactions options"));
  //Interactions - selection mode:
  s.save(m_d->ui_int.radioButton_selmode_single,
		 m_d->ui_int.radioButton_selmode_multitracks,
		 m_d->ui_int.radioButton_selmode_trackfits);

  s.save(m_d->ui_int.checkBox_selsingle_printinfo);
  s.save(m_d->ui_int.checkBox_selsingle_printinfo_verbose);
  s.save(m_d->ui_int.checkBox_selsingle_orientzoom);
  s.save(m_d->ui_int.checkBox_sel_printtotmom);
  s.save(m_d->ui_int.checkBox_sel_showtotmom);
  s.save(m_d->ui_int.comboBox_fitterMode); // Version 12+
  s.save(m_d->ui_int.checkBox_removeOutliers);// Version 12+
  s.save(m_d->ui_int.comboBox_particleHypo);// Version 12+
  // -----------------------------------




  // --- AscObjs options ---
    s.save(QString("AscObjs options"));
  //AscObjs - TSOS:
  s.save(m_d->ui_ascobjs.checkBox_materialeffectsontrack_forceposontrack);
  s.save(m_d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE);
  s.save(m_d->ui_ascobjs.checkBox_measurements_shorttubes_mdt);
  s.save(m_d->ui_ascobjs.checkBox_measurements_shorttubes_trt);
  s.save(m_d->ui_ascobjs.checkBox_measurements_drawGP); // Version 11
  s.save(m_d->ui_ascobjs.checkBox_parerror_drawcylinder);
  s.save(m_d->ui_ascobjs.checkBox_parerror_hideperigeeerrors);
  s.save(m_d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces);
  s.save(m_d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces); // Version 11
  s.save(m_d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack);
  s.save(m_d->ui_ascobjs.checkBox_usecolour_meas_outliers);
  s.save(m_d->ui_ascobjs.checkBox_usecolour_measurements);
  s.save(m_d->ui_ascobjs.checkBox_usecolour_parametererrors);
  s.save(m_d->ui_ascobjs.checkBox_usecolour_parameters);
  s.save(m_d->ui_ascobjs.checkBox_useHoleColour_parameters);// Version 14
  s.save(m_d->ui_ascobjs.checkBox_usecolour_surfaces);
  
  s.save(m_d->ui_ascobjs.doubleSpinBox_parerror_stddev);
  s.save(m_d->ui_ascobjs.groupBox_materialeffectsontrack);
  s.save(m_d->ui_ascobjs.groupBox_measurements);
  s.save(m_d->ui_ascobjs.groupBox_errors);// Version 13
  s.save(m_d->ui_ascobjs.checkBox_parametererrors);// Version 13
  s.save(m_d->ui_ascobjs.checkBox_measurementerrors);// Version 13
  s.save(m_d->ui_ascobjs.groupBox_parameters);
  s.save(m_d->ui_ascobjs.groupBox_surfaces);
  s.save(m_d->ui_ascobjs.horizontalSlider_complexity);
  s.save(m_d->ui_ascobjs.horizontalSlider_linewidths);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.save(m_d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale);
  s.save(m_d->ui_ascobjs.horizontalSlider_pointsizes);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.save(m_d->ui_ascobjs.matButton_materialeffectsontrack);
  s.save(m_d->ui_ascobjs.matButton_meas_outliers);
  s.save(m_d->ui_ascobjs.matButton_measurements);
  s.save(m_d->ui_ascobjs.matButton_parameters);
  s.save(m_d->ui_ascobjs.matButton_holeParameters);// Version 14
  s.save(m_d->ui_ascobjs.matButton_parerrors);
  s.save(m_d->ui_ascobjs.matButton_surfaces);
  s.save(m_d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale);//Version 5+
  // -----------------------------------




  // ----- Colouring options -----
  s.save(QString("Colouring options"));
  // Version 12
  s.save(m_d->ui_col.groupBox_labels);
  s.save(m_d->ui_col.horizontalSlider_labels_trkOffset);
  s.save(m_d->ui_col.horizontalSlider_labels_xOffset);
  s.save(m_d->ui_col.horizontalSlider_labels_yOffset);
  s.save(m_d->ui_col.horizontalSlider_labels_zOffset);
  s.save(m_d->ui_col.checkBox_trkLabels_p);
  s.save(m_d->ui_col.checkBox_trkLabels_Pt);
  s.save(m_d->ui_col.checkBox_trkLabels_pid);
  s.save(m_d->ui_col.checkBox_trkLabels_hits);
  s.save(m_d->ui_col.checkBox_trkLabels_fitQuality);
  s.save(m_d->ui_col.checkBox_trkLabels_direction);
  // --------------------------






  // ----- Extrap options -----
  s.save(QString("Extrapolator options"));
  s.save(m_d->ui_extrap.checkBox_ignoreMEoT); // Version 14
  s.save(m_d->ui_extrap.checkBox_extendAllInDetTracks);
  s.save(m_d->ui_extrap.comboBox_extendAllInDetTracksToHere);
  s.save(m_d->ui_extrap.horizontalSlider_granularity);
  s.save(m_d->ui_extrap.checkBox_maxRadius); // Version 15
  s.save(m_d->ui_extrap.spinBox_maxRadiusValue);
  // --------------------------
  

  messageVerbose("TrackSystemController::actualSaveSettings() - DONE.");

}

//____________________________________________________________________
void TrackSystemController::actualRestoreSettings(VP1Deserialise& s)
{
	messageDebug("TrackSystemController::actualRestoreSettings()");

	/*
		 * NOTE!!! Important, about serialization:
		 *
		 * - order has to be the same between saving-reading
		 *
		 * See: http://www.mimec.org/node/348
		 *
		 */

	messageDebug("Deserialize - current version: "+QString::number( s.version() ));

  if (s.version()<0||s.version()>currentSettingsVersion()) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }
  
  if (s.version()<17) {
    message("Warning: the VP1 GUI has changed since .vp1 file format version ("+str(s.version())+"), which means you will have lost the line thickness etc. settings formally found in the Colouring pop-up dialogue.");
  }
  
  if (s.version()<17) {
	  s.ignoreString();
  } else {
	  QString tt = s.restoreString();
	  messageDebug("Check string: " + tt);
	  if (tt != "TrackSystemSettings") {
		  messageDebug("\n\nERROR!! Settings order does not match!!");
		  return;
	  }
  }

  if (s.version()<=3)
    s.ignoreInt();

  // //Display options: linewidth:
  // if (s.version()>=1) {
  //   VP1QtInventorUtils::setValueLineWidthSlider(m_d->ui_col.horizontalSlider_trackWidth,s.restoreDouble());
  //   s.widgetHandled(m_d->ui_col.horizontalSlider_trackWidth);
  // }
  if (s.version()<17)
     s.ignoreInt(); //m_d->ui_col.horizontalSlider_trackWidth

  //Tracks base light model:
  // if (s.version()>=4)
  //   s.restore(m_d->ui_col.checkBox_tracksUseBaseLightModel);
  if (s.version()<17)
    s.ignoreBool(); //m_d->ui_col.checkBox_tracksUseBaseLightModel
  




  // --- Projections options ---
  if (s.version()>=17 &&  s.restoreString() != "Projections options") messageDebug("\n\nERROR! --> 'Projections options'");
  s.restore(m_d->ui_proj.checkBox_projections_indet);
  s.restore(m_d->ui_proj.checkBox_projections_muonchambers);
  if (s.version()>=15){
    s.restore(m_d->ui_proj.groupBox_projections_vertex); 
    s.restore(m_d->ui_proj.spinBox_projections_vertex);
    s.restore(m_d->ui_proj.horizontalSlider_projections_vertex);
  }
  // s.restore(m_d->ui_col.checkBox_hideactualpaths);
  if (s.version()<17)
    s.ignoreBool(); //m_d->ui_col.checkBox_hideactualpaths  
  // --------------------------------------




  //Display options - track tubes
  // if (s.version()>=8) {
  //   s.restore(m_d->ui_col.checkBox_trackTubes);
  //   s.restore(m_d->ui_col.doubleSpinBox_trackTubesRadiusMM);
  // }
  if (s.version()<17){
    s.ignoreBool(); //m_d->ui_col.checkBox_trackTubes  
    s.ignoreDouble(); //m_d->ui_col.doubleSpinBox_trackTubesRadiusMM  
  }

  if (s.version()<=3)
    s.ignoreBool();




  // ----- Propagation options -----
  if (s.version()>=17 && s.restoreString() != "Propagation options") messageDebug("\n\nERROR! --> 'Propagation options'");
  if (s.version()>=4)
    s.restore(m_d->ui_extrap.radioButton_none,
	      m_d->ui_extrap.radioButton_helical,
	      m_d->ui_extrap.radioButton_athenaExtrapolator);
  m_d->restoredLastPropagator = s.restoreString();
  s.widgetHandled(m_d->ui_extrap.comboBox_propagator);
  // -----------------------------------




  // ----- Interactions options -----
  if (s.version()>=17 && s.restoreString() != "Interactions options") messageDebug("\n\nERROR! --> 'Interactions options'");
  if (s.version()>=6) {
    s.restore(m_d->ui_int.lineEdit_fittedTrackCollName);
    m_d->restoredLastFitter = s.restoreString();
    s.widgetHandled(m_d->ui_int.comboBox_fitters);
  }
  // -----------------------------------




  if (s.version()<=2)
    s.ignoreBool();

  if (s.version()==0) {
    s.ignoreInt();
    s.ignoreBool();
    s.ignoreBool();
  }

  //Display options - Trk::Tracks
  if (s.version()<=2)
    s.ignoreBool();
  s.restore(m_d->ui_ascobjs.comboBox_assocobj_detaillevel);
  if (s.version()<=2) {
    s.ignoreBool();
    s.ignoreBool();
  }

  //Display options - Truth tracks:
  s.restore(m_d->ui_ascobjs.checkBox_truthtracks_display_points);


  // --- Display options - Colours ---
  if (s.version()>=17 && s.restoreString() != "Colour options") messageDebug("\n\nERROR! --> 'Colour options'");
  //Display options - Colour by pdg:
  s.restore(m_d->ui_col.matButton_electrons);
  s.restore(m_d->ui_col.matButton_muons);
  s.restore(m_d->ui_col.matButton_pions);
  s.restore(m_d->ui_col.matButton_protons);
  s.restore(m_d->ui_col.matButton_chargedkaons);
  s.restore(m_d->ui_col.matButton_othercharged);
  s.restore(m_d->ui_col.matButton_neutrons);
  s.restore(m_d->ui_col.matButton_photons);
  s.restore(m_d->ui_col.matButton_neutrinos);
  s.restore(m_d->ui_col.matButton_otherneutrals);
  if (s.version()>=2) {
  //Display options - Colour by charge:
    s.restore(m_d->ui_col.matButton_charge_neg);
    s.restore(m_d->ui_col.matButton_charge_pos);
    s.restore(m_d->ui_col.matButton_charge_neutral);

    //Display options - Colour by momentum:
    s.restore(m_d->ui_col.matButton_0GeV);
    s.restore(m_d->ui_col.matButton_15GeV);
  }
  // -----------------------------------





  // --- Cuts options ---
  if (s.version()>=17 && s.restoreString() != "Cuts options") messageDebug("\n\nERROR! --> 'Cuts options'");
  //Cuts - general:
  s.restore(m_d->ui_cuts.checkBox_cut_minpt);
  s.restore(m_d->ui_cuts.doubleSpinBox_cut_minpt_gev);
  s.restore(m_d->ui_cuts.checkBox_cut_maxpt);
  s.restore(m_d->ui_cuts.doubleSpinBox_cut_maxpt_gev);

  if (s.version()>=10) {
    s.restore(m_d->ui_cuts.comboBox_momtype);
  }

  if (s.version()>=7) {
    s.restore(m_d->ui_cuts.etaPhiCutWidget);
  } else {
    s.ignoreBool();
    s.ignoreBool();
    s.ignoreDouble();
    s.ignoreDouble();
    s.ignoreBool();
    s.ignoreObsoletePhiSectionWidgetState();
  }

  if (s.version()>=9) {
    s.restore(m_d->ui_cuts.checkBox_cut_nhits_pixel);
    s.restore(m_d->ui_cuts.checkBox_cut_nhits_sct);
    s.restore(m_d->ui_cuts.checkBox_cut_nhits_trt);
    s.restore(m_d->ui_cuts.checkBox_cut_nhits_muon);
    s.restore(m_d->ui_cuts.spinBox_cut_nhits_pixel);
    s.restore(m_d->ui_cuts.spinBox_cut_nhits_sct);
    s.restore(m_d->ui_cuts.spinBox_cut_nhits_trt);
    s.restore(m_d->ui_cuts.spinBox_cut_nhits_muon);
  }

  //Cuts - truth:
  s.restore(m_d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR);
  s.restore(m_d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals);
  s.restore(m_d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0);

  if (s.version()>=14) s.restore(m_d->ui_cuts.checkBox_vertexAssociated);
  // -----------------------------------





  // --- Interactions options ---
  if (s.version()>=17 && s.restoreString() != "Interactions options") messageDebug("\n\nERROR! --> 'Interactions options'");
  //Interactions - selection mode:
  s.restore(m_d->ui_int.radioButton_selmode_single,
		m_d->ui_int.radioButton_selmode_multitracks,
		m_d->ui_int.radioButton_selmode_trackfits);

  s.restore(m_d->ui_int.checkBox_selsingle_printinfo);
  s.restore(m_d->ui_int.checkBox_selsingle_printinfo_verbose);
  s.restore(m_d->ui_int.checkBox_selsingle_orientzoom);
  s.restore(m_d->ui_int.checkBox_sel_printtotmom);
  s.restore(m_d->ui_int.checkBox_sel_showtotmom);
  if (s.version()>=12){
    s.restore(m_d->ui_int.comboBox_fitterMode); 
    s.restore(m_d->ui_int.checkBox_removeOutliers);
    s.restore(m_d->ui_int.comboBox_particleHypo);
  }
  // -----------------------------------






  
  // --- AscObjs options ---
  if (s.version()>=17 && s.restoreString() != "AscObjs options") messageDebug("\n\nERROR! --> 'AscObjs options'");
  //AscObjs - TSOS:
  s.restore(m_d->ui_ascobjs.checkBox_materialeffectsontrack_forceposontrack);
  s.restore(m_d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE);
  s.restore(m_d->ui_ascobjs.checkBox_measurements_shorttubes_mdt);
  s.restore(m_d->ui_ascobjs.checkBox_measurements_shorttubes_trt);
  if (s.version()>=11) s.restore(m_d->ui_ascobjs.checkBox_measurements_drawGP);
  s.restore(m_d->ui_ascobjs.checkBox_parerror_drawcylinder);
  s.restore(m_d->ui_ascobjs.checkBox_parerror_hideperigeeerrors);
  s.restore(m_d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces);
  if (s.version()>=11) s.restore(m_d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces);
  s.restore(m_d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack);
  s.restore(m_d->ui_ascobjs.checkBox_usecolour_meas_outliers);
  s.restore(m_d->ui_ascobjs.checkBox_usecolour_measurements);
  if (s.version()<12) s.restore(m_d->ui_ascobjs.checkBox_parametererrors); // was groupBox_parametererrors before...
  s.restore(m_d->ui_ascobjs.checkBox_usecolour_parametererrors);
  s.restore(m_d->ui_ascobjs.checkBox_usecolour_parameters);
  if (s.version()>=14)
    s.restore(m_d->ui_ascobjs.checkBox_useHoleColour_parameters);  
  s.restore(m_d->ui_ascobjs.checkBox_usecolour_surfaces);
  s.restore(m_d->ui_ascobjs.doubleSpinBox_parerror_stddev);
  s.restore(m_d->ui_ascobjs.groupBox_materialeffectsontrack);
  s.restore(m_d->ui_ascobjs.groupBox_measurements);
  if (s.version()>=12) {
    s.restore(m_d->ui_ascobjs.groupBox_errors);
    s.restore(m_d->ui_ascobjs.checkBox_parametererrors);
    s.restore(m_d->ui_ascobjs.checkBox_measurementerrors);
  }
  s.restore(m_d->ui_ascobjs.groupBox_parameters);
  s.restore(m_d->ui_ascobjs.groupBox_surfaces);
  s.restore(m_d->ui_ascobjs.horizontalSlider_complexity);
  s.restore(m_d->ui_ascobjs.horizontalSlider_linewidths);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.restore(m_d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale);
  s.restore(m_d->ui_ascobjs.horizontalSlider_pointsizes);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.restore(m_d->ui_ascobjs.matButton_materialeffectsontrack);
  s.restore(m_d->ui_ascobjs.matButton_meas_outliers);
  s.restore(m_d->ui_ascobjs.matButton_measurements);
  s.restore(m_d->ui_ascobjs.matButton_parameters);
  if (s.version()>=14)
   s.restore(m_d->ui_ascobjs.matButton_holeParameters);
  s.restore(m_d->ui_ascobjs.matButton_parerrors);
  s.restore(m_d->ui_ascobjs.matButton_surfaces);
  if (s.version()>=5)
    s.restore(m_d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale);
  // -----------------------------------






  //Interactions - track fits:
  


  // ----- Colouring options -----
  if (s.version()>=17 && s.restoreString() != "Colouring options") messageDebug("\n\nERROR! --> 'Colouring options'");
  if (s.version()>=12){
    s.restore(m_d->ui_col.groupBox_labels);
    s.restore(m_d->ui_col.horizontalSlider_labels_trkOffset);
    s.restore(m_d->ui_col.horizontalSlider_labels_xOffset);
    s.restore(m_d->ui_col.horizontalSlider_labels_yOffset);
    s.restore(m_d->ui_col.horizontalSlider_labels_zOffset);
    s.restore(m_d->ui_col.checkBox_trkLabels_p);
    s.restore(m_d->ui_col.checkBox_trkLabels_Pt);
    s.restore(m_d->ui_col.checkBox_trkLabels_pid);
    s.restore(m_d->ui_col.checkBox_trkLabels_hits);
    s.restore(m_d->ui_col.checkBox_trkLabels_fitQuality);
  }
  if (s.version()>=16)
    s.restore(m_d->ui_col.checkBox_trkLabels_direction);
  
  



  // ----- Extrap options -----
  // Version 14
  if (s.version()>=17 && s.restoreString() != "Extrapolator options") messageDebug("\n\nERROR! --> 'Extrapolator options'");
  if (s.version()>=14){
    s.restore(m_d->ui_extrap.checkBox_ignoreMEoT);
    s.restore(m_d->ui_extrap.checkBox_extendAllInDetTracks);
    s.restore(m_d->ui_extrap.comboBox_extendAllInDetTracksToHere); 
    s.restore(m_d->ui_extrap.horizontalSlider_granularity);
  }
  // version 15
  if (s.version()>=15){
    s.restore(m_d->ui_extrap.checkBox_maxRadius);
    s.restore(m_d->ui_extrap.spinBox_maxRadiusValue);  
  }
  // ------------------------------



  messageDebug("TrackSystemController::actualRestoreSettings() - DONE.");

}

//____________________________________________________________________
TrackCollWidget * TrackSystemController::collWidget() const
{
  return m_d->trackcollwidget;
}

//Access methods:

//____________________________________________________________________
SoMaterial * TrackSystemController::Imp::getMat(VP1MaterialButton* mb) const
{
  if (!mb) {
    theclass->message("ERROR: Null material button pointer");
    return materialFallback;
  }
  QList<SoMaterial*> mats = mb->handledMaterials();
  if (mats.count()!=1) {
    theclass->message("ERROR: No material in button");
    return materialFallback;
  }
  return mats.at(0);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::getMaterialForPDGCode(const int& pdgCode) const
{
  VP1MaterialButton * matbutton(0);
  int abspdg = abs(pdgCode);
  switch (abspdg) {
  case 211:  matbutton = m_d->ui_col.matButton_pions; break;
  case 11:   matbutton = m_d->ui_col.matButton_electrons; break;
  case 22:   matbutton = m_d->ui_col.matButton_photons; break;
  case 13:   matbutton = m_d->ui_col.matButton_muons; break;
  case 2212: matbutton = m_d->ui_col.matButton_protons; break;
  case 2112: matbutton = m_d->ui_col.matButton_neutrons; break;
  case 2: return m_d->materialFallback;
    //Only use CHARGED kaons here!!
    //   case 130://k-long
    //   case 310://k-short
    //   case 311://k0
  case 321://k+
    //   case 313://k*0
  case 323://k*+
    //Fixme: More (charged) kaon states???
    matbutton = m_d->ui_col.matButton_chargedkaons; break;
  case 12://nu_e
  case 14://nu_mu
  case 16://nu_tau
  case 18://fourth gen. neutrino:
    matbutton = m_d->ui_col.matButton_neutrinos; break;
  default:
    bool ok;
    double charge;
    charge = VP1ParticleData:: particleCharge(abspdg,ok);
    if (!ok) {
      message("WARNING: Could not look up charge of pdg code "+str(abspdg)+". Assuming charged.");
      charge = 1.0;
    }
    if (charge==0.0)
      matbutton = m_d->ui_col.matButton_otherneutrals;
    else
      matbutton = m_d->ui_col.matButton_othercharged;
    break;
  }
  return m_d->getMat(matbutton);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::getMaterialForCharge(const double& charge) const
{
  if (charge>0)
    return m_d->getMat(m_d->ui_col.matButton_charge_pos);
  else if (charge<0)
    return m_d->getMat(m_d->ui_col.matButton_charge_neg);
  return m_d->getMat(m_d->ui_col.matButton_charge_neutral);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::getMaterialForMomentum(const double& absmom) const
{
  static const double low=0*CLHEP::GeV;
  static const double high=15*CLHEP::GeV;
  if (absmom<=low)
    return m_d->getMat(m_d->ui_col.matButton_0GeV);
  else if (absmom>=high)
    return m_d->getMat(m_d->ui_col.matButton_15GeV);

  if (!m_d->matmixer)
    m_d->matmixer = new VP1SoMaterialMixer(systemBase());

  const double x = (absmom-low)/(high-low);
  return m_d->matmixer->getMixedMaterial( m_d->getMat(m_d->ui_col.matButton_0GeV), 1-x,
					m_d->getMat(m_d->ui_col.matButton_15GeV), x );
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatMeasurements() const
{
  return m_d->getMat(m_d->ui_ascobjs.matButton_measurements);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatMeasurementsOutliers() const
{
  return m_d->getMat(m_d->ui_ascobjs.matButton_meas_outliers);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatParameters() const
{
  return m_d->getMat(m_d->ui_ascobjs.matButton_parameters);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatHoleParameters() const
{
  return m_d->getMat(m_d->ui_ascobjs.matButton_holeParameters);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatParameterErrors() const
{
  return m_d->getMat(m_d->ui_ascobjs.matButton_parerrors);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatMaterialEffects() const
{
  return m_d->getMat(m_d->ui_ascobjs.matButton_materialeffectsontrack);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatSurfaces() const
{
  return m_d->getMat(m_d->ui_ascobjs.matButton_surfaces);
}

// //____________________________________________________________________
// void TrackSystemController::updateTrackDrawStyle()
// {
//   double val = VP1QtInventorUtils::getValueLineWidthSlider(m_d->ui_col.horizontalSlider_trackWidth);
//   if (m_d->trackDrawStyle->lineWidth.getValue()!=val)
//     m_d->trackDrawStyle->lineWidth = val;
// }

// //____________________________________________________________________
// void TrackSystemController::updateTrackLightModel()
// {
//   bool base = m_d->ui_col.checkBox_tracksUseBaseLightModel->isChecked();
//   if (m_d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
//     messageVerbose("TrackLightModel changed (base = "+str(base));
//     if (base)
//       m_d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
//     else
//       m_d->trackLightModel->model.setValue(SoLightModel::PHONG);
//   }
// }

//____________________________________________________________________
void TrackSystemController::updateAscObjDrawStyle()
{
  double val_lw = VP1QtInventorUtils::getValueLineWidthSlider(m_d->ui_ascobjs.horizontalSlider_linewidths);
  double val_ps = VP1QtInventorUtils::getValuePointSizeSlider(m_d->ui_ascobjs.horizontalSlider_pointsizes);
  if (m_d->ascObjDrawStyle->lineWidth.getValue()!=val_lw)
    m_d->ascObjDrawStyle->lineWidth = val_lw;
  if (m_d->ascObjDrawStyle->pointSize.getValue()!=val_ps)
    m_d->ascObjDrawStyle->pointSize = val_ps;
}


//____________________________________________________________________
void TrackSystemController::updateAscObjComplexity()
{
  //choose complexity in interval [0.01,1.0]
  const double val = std::min<double>(1.0,std::max<double>(0.0,0.01+0.991*
        (m_d->ui_ascobjs.horizontalSlider_complexity->value()
	-m_d->ui_ascobjs.horizontalSlider_complexity->minimum())/
        (m_d->ui_ascobjs.horizontalSlider_complexity->maximum()*1.0)));
  if (m_d->ascObjComplexity->value.getValue()!=val)
    m_d->ascObjComplexity->value.setValue(val);
}

// //____________________________________________________________________
// SoDrawStyle * TrackSystemController::trackDrawStyle() const
// {
//   return m_d->trackDrawStyle;
// }

// //____________________________________________________________________
// SoLightModel * TrackSystemController::trackLightModel() const
// {
//   return m_d->trackLightModel;
// }

//____________________________________________________________________
SoDrawStyle * TrackSystemController::ascObjDrawStyle() const
{
  return m_d->ascObjDrawStyle;
}

//____________________________________________________________________
SoComplexity * TrackSystemController::ascObjComplexity() const
{
  return m_d->ascObjComplexity;
}

//____________________________________________________________________
QString TrackSystemController::nameOfNewlyFittedCollections() const
{
  QString name = m_d->ui_int.lineEdit_fittedTrackCollName->text().simplified();
  return name.isEmpty() ? "<noname>" : name;
}

//____________________________________________________________________
TrackCommonFlags::TrackPartsFlags TrackSystemController::shownTrackParts() const
{
  TrackCommonFlags::TrackPartsFlags parts = TrackCommonFlags::NoParts;
  // if (!m_d->ui_col.checkBox_hideactualpaths->isChecked()) parts |= TrackCommonFlags::ActualPath;
  parts |= TrackCommonFlags::ActualPath; // sensible default.
  if (m_d->ui_proj.checkBox_projections_indet->isChecked()) parts |= TrackCommonFlags::InDetProjections;
  if (m_d->ui_proj.groupBox_projections_vertex->isChecked()) parts |= TrackCommonFlags::VertexProjections;
  if (VP1JobConfigInfo::hasMuonGeometry()&&m_d->ui_proj.checkBox_projections_muonchambers->isChecked()) parts |= TrackCommonFlags::MuonProjections;
  return parts;
}

int TrackSystemController::vertexProjectionAngle() const{
  messageVerbose("angleForVertexPlane"+str(m_d->ui_proj.spinBox_projections_vertex->value()));
  
  if (!m_d->ui_proj.groupBox_projections_vertex->isChecked()) return -1;
  return m_d->ui_proj.spinBox_projections_vertex->value();
}

// //____________________________________________________________________
// double TrackSystemController::trackTubeRadius() const
// {
//   return m_d->ui_col.checkBox_trackTubes->isChecked() ?
//     m_d->ui_col.doubleSpinBox_trackTubesRadiusMM->value()*CLHEP::mm : 0.0;
// }

//____________________________________________________________________
TrackCommonFlags::TSOSPartsFlags TrackSystemController::shownTSOSParts() const
{
  TrackCommonFlags::TSOSPartsFlags f(TrackCommonFlags::TSOS_NoObjects);
  if (m_d->ui_ascobjs.groupBox_parameters->isChecked())
    f |= TrackCommonFlags::TSOS_TrackPars;
  if (m_d->ui_ascobjs.groupBox_errors->isChecked()&&m_d->ui_ascobjs.checkBox_parametererrors->isChecked()) {
    f |= TrackCommonFlags::TSOS_TrackParsErrorsNotPerigee;
    if (!m_d->ui_ascobjs.checkBox_parerror_hideperigeeerrors->isChecked())
      f |= TrackCommonFlags::TSOS_TrackParsErrorsPerigee;
  }
  if (m_d->ui_ascobjs.groupBox_errors->isChecked()&&m_d->ui_ascobjs.checkBox_measurementerrors->isChecked()) {
    f |= TrackCommonFlags::TSOS_MeasError;
  }
  if (m_d->ui_ascobjs.groupBox_measurements->isChecked()) {
    f |= TrackCommonFlags::TSOS_AnyMeasRioOnTrack;
    f |= TrackCommonFlags::TSOS_AnyMeasCompetingRioOnTrack;
  }
  if (m_d->ui_ascobjs.groupBox_surfaces->isChecked()) {
    f |= TrackCommonFlags::TSOS_SurfacesDetElem;
    if (!m_d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces->isChecked())
      f |= TrackCommonFlags::TSOS_SurfacesCustom;
    if (!m_d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces->isChecked())
      f |= TrackCommonFlags::TSOS_TubeSurfaces;
  }
  if (m_d->ui_ascobjs.groupBox_materialeffectsontrack->isChecked()) {
    f |= TrackCommonFlags::TSOS_MaterialEffects;
    if (!m_d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE->isChecked())
      f |= TrackCommonFlags::TSOS_MaterialEffectsWithNoDeltaE;
  }

  return f;
}

//____________________________________________________________________
TrackCommonFlags::TSOSPartsFlags TrackSystemController::customColouredTSOSParts() const
{
  TrackCommonFlags::TSOSPartsFlags f(TrackCommonFlags::TSOS_NoObjects);
  if (m_d->ui_ascobjs.checkBox_usecolour_measurements->isChecked())
    f |= TrackCommonFlags::TSOS_AnyMeasurementNotOutlier;
  if (m_d->ui_ascobjs.checkBox_usecolour_meas_outliers->isChecked())
    f |= TrackCommonFlags::TSOS_AnyMeasurementOutlier;
  if (m_d->ui_ascobjs.checkBox_usecolour_parameters->isChecked())
    f |= TrackCommonFlags::TSOS_TrackPars;
  if (m_d->ui_ascobjs.checkBox_useHoleColour_parameters->isChecked())
    f |= TrackCommonFlags::TSOS_Hole;
  if (m_d->ui_ascobjs.checkBox_usecolour_parametererrors->isChecked())
    f |= TrackCommonFlags::TSOS_AnyParsErrors;
  if (m_d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack->isChecked())
    f |= TrackCommonFlags::TSOS_AnyMaterialEffects;
  if (m_d->ui_ascobjs.checkBox_usecolour_surfaces->isChecked())
    f |= TrackCommonFlags::TSOS_AnySurface;
  
  return f;
}

//____________________________________________________________________
bool TrackSystemController::useShortTRTMeasurements() const
{
  return m_d->ui_ascobjs.checkBox_measurements_shorttubes_trt->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::useShortMDTMeasurements() const
{
  return m_d->ui_ascobjs.checkBox_measurements_shorttubes_mdt->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::hideTubeSurfaces() const
{
  return m_d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::hideCustomSurfaces() const
{
  return m_d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::drawMeasGlobalPositions() const
{
  return m_d->ui_ascobjs.checkBox_measurements_drawGP->isChecked();
}


//____________________________________________________________________
double TrackSystemController::measurementsShorttubesScale() const
{
  return std::max(0.1*CLHEP::mm,m_d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale->value()*CLHEP::cm);
}

//____________________________________________________________________
double TrackSystemController::nStdDevForParamErrors() const
{
  return std::min<double>(1.0e3,std::max<double>(1.0e-3,m_d->ui_ascobjs.doubleSpinBox_parerror_stddev->value()));
}

//____________________________________________________________________
bool TrackSystemController::parTubeErrorsDrawCylinders() const
{
  return m_d->ui_ascobjs.checkBox_parerror_drawcylinder->isChecked();
}

//____________________________________________________________________
int TrackSystemController::numberOfPointsOnCircles() const
{
  const int val(m_d->ui_ascobjs.horizontalSlider_complexity->value());
  const int max(m_d->ui_ascobjs.horizontalSlider_complexity->maximum());
  const int min(m_d->ui_ascobjs.horizontalSlider_complexity->minimum());
  //special cases:
  if (val==max)
    return 80;
  else if (val==min)
    return 4;

  //roughly 4..32:
  const int n(1+static_cast<int>(0.5+7.0*(val-min)/(1.0*max)));//number between 1 and 8
  return std::min<int>(32,std::max<int>(4,4*n));;
}

//____________________________________________________________________
double TrackSystemController::materialEffectsOnTrackScale() const
{
  return m_d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale->value()/5.0;
}

//____________________________________________________________________
Trk::IExtrapolator * TrackSystemController::propagator() const
{
  if (!m_d->toolaccesshelper
      ||!m_d->ui_extrap.radioButton_athenaExtrapolator->isChecked()
      ||m_d->ui_extrap.comboBox_propagator->count()==0)
    return 0;

  QString key = m_d->ui_extrap.comboBox_propagator->currentText();
  if (key==Imp::noneAvailString)
    return 0;
  return key.isEmpty() ? 0 : m_d->toolaccesshelper->getToolPointer<Trk::IExtrapolator>(key);
}

TrackSystemController::PropagationOptionFlags TrackSystemController::propagationOptions() const{
  PropagationOptionFlags options=TrackSystemController::NoPropOptions;
  if (m_d->ui_extrap.checkBox_ignoreMEoT->isChecked())    options |= TrackSystemController::IgnoreMEOT;
  if (m_d->ui_extrap.checkBox_extendAllInDetTracks->isChecked())    options |= TrackSystemController::ExtendTrack;
  return options;
}

float TrackSystemController::propMaxRadius() const{
  if (!m_d->ui_extrap.checkBox_maxRadius->isChecked())   return -1.0;
  return m_d->ui_extrap.spinBox_maxRadiusValue->value(); 
}

void TrackSystemController::emitExtrapolateToHereChanged(int /**index*/){
  emit extrapolateToThisVolumeChanged();
}

bool TrackSystemController::ignoreMeasurementEffectsOnTrackInProp() 
{
  return m_d->ui_extrap.checkBox_ignoreMEoT->isChecked();
}

//____________________________________________________________________
Trk::ITrackFitter * TrackSystemController::trackFitter() const
{
  if (!m_d->toolaccesshelper
      ||m_d->ui_int.comboBox_fitters->count()==0)
  return 0;

  QString key = m_d->ui_int.comboBox_fitters->currentText();
  if (key==Imp::noneAvailString)
    return 0;
  return key.isEmpty() ? 0 : m_d->toolaccesshelper->getToolPointer<Trk::ITrackFitter>(key);
}

Muon::MuonEDMPrinterTool * TrackSystemController::muonEDMPrinterTool() const
{
  QString key = "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool";
  return m_d->toolaccesshelper->getToolPointer<Muon::MuonEDMPrinterTool>(key);
}

// ITrackingVolumesSvc * TrackSystemController::trackingVolumeSvc() const
// {
//   return m_trackingVolumesSvc.operator->();
// }

const Trk::Volume * TrackSystemController::extrapolateToThisVolume() const
{
  if (m_d->ui_extrap.comboBox_extendAllInDetTracksToHere->currentText()=="Calorimeter")
    return m_d->calorimeterEntryLayer;
  if (m_d->ui_extrap.comboBox_extendAllInDetTracksToHere->currentText()=="Muon Entrance")
    return m_d->muonSpectrometerEntryLayer;
  if (m_d->ui_extrap.comboBox_extendAllInDetTracksToHere->currentText()=="Muon Exit")
    return m_d->muonSpectrometerExitLayer;
  return 0;
}

//____________________________________________________________________
TrackCommonFlags::SELECTIONMODE TrackSystemController::selectionMode() const
{
  if (m_d->ui_int.radioButton_selmode_single->isChecked())
    return TrackCommonFlags::SINGLEOBJECT;
  else if (m_d->ui_int.radioButton_selmode_multitracks->isChecked())
    return TrackCommonFlags::MULTITRACK;
  else if (m_d->ui_int.radioButton_selmode_trackfits->isChecked())
    return TrackCommonFlags::TRACKFIT;
  message("selectionMode ERROR: Inconsistency detected.");
  return TrackCommonFlags::SINGLEOBJECT;
}

TrackCommonFlags::FITTERMODE TrackSystemController::fitterMode() const
{  
  if (m_d->ui_int.comboBox_fitterMode->currentText()=="Fit PRDs")
    return TrackCommonFlags::FROMPRDS;
  else if (m_d->ui_int.comboBox_fitterMode->currentText()=="Refit Track")
    return TrackCommonFlags::REFITSINGLETRACK;
  else if (m_d->ui_int.comboBox_fitterMode->currentText()=="Extend Track with PRDs")
    return TrackCommonFlags::EXTENDTRACKWITHPRDS;
  else if (m_d->ui_int.comboBox_fitterMode->currentText()=="Combine Two Tracks")
    return TrackCommonFlags::COMBINETWOTRACKS;
  
  message("fitterMode ERROR: Inconsistency detected. Mode not known.");
  
  return TrackCommonFlags::FROMPRDS; // FIXME!
}

bool TrackSystemController::fitterRemoveOutliers() const
{
  return m_d->ui_int.checkBox_removeOutliers->isChecked();
}

Trk::ParticleHypothesis TrackSystemController::fitterParticleHypthesis() const
{
  if (m_d->ui_int.comboBox_particleHypo->currentText()=="Pion")
     return Trk::pion;
   else if (m_d->ui_int.comboBox_particleHypo->currentText()=="Non Interacting")
     return Trk::nonInteracting;
   else if (m_d->ui_int.comboBox_particleHypo->currentText()=="Electron")
     return Trk::electron;
   else if (m_d->ui_int.comboBox_particleHypo->currentText()=="Muon")
     return Trk::muon;
   else if (m_d->ui_int.comboBox_particleHypo->currentText()=="Kaon")
     return Trk::kaon;
   else if (m_d->ui_int.comboBox_particleHypo->currentText()=="Proton")
     return Trk::proton;
   else if (m_d->ui_int.comboBox_particleHypo->currentText()=="Photon")
     return Trk::photon;
     
   message("fitterMode ERROR: Inconsistency detected. Mode not known.");
   return Trk::undefined; // FIXME!
}

//____________________________________________________________________
bool TrackSystemController::showTruthAscObjs() const
{
  return m_d->ui_ascobjs.checkBox_truthtracks_display_points->isChecked();
}

//____________________________________________________________________
TrackCommonFlags::DETAILLEVEL TrackSystemController::assocObjDetailLevel() const
{
  if (m_d->ui_ascobjs.comboBox_assocobj_detaillevel->currentText()=="Auto")
    return TrackCommonFlags::AUTO;
  else if (m_d->ui_ascobjs.comboBox_assocobj_detaillevel->currentText()=="Simple")
    return TrackCommonFlags::SIMPLE;
  else
    return TrackCommonFlags::DETAILED;
}

void TrackSystemController::vertexCutsAllowed(bool b){
  if (m_d->ui_cuts.checkBox_vertexAssociated->isEnabled()!=b) {
    m_d->ui_cuts.checkBox_vertexAssociated->setEnabled(b);
    emit cutOnlyVertexAssocTracksChanged(m_d->ui_cuts.checkBox_vertexAssociated->isChecked());
  }
}

bool TrackSystemController::cutOnlyVertexAssocTracks() const {
  if (!m_d->ui_cuts.checkBox_vertexAssociated->isEnabled()) return false;
  return m_d->ui_cuts.checkBox_vertexAssociated->isChecked();
}


//____________________________________________________________________
VP1Interval TrackSystemController::cutAllowedPt() const
{
  if (!m_d->ui_cuts.checkBox_cut_minpt)
    return VP1Interval();

  // will set range to negative if we have momcut=P
  // if minCut unset then min=-inf
  // if minCut set, and Pt selected, then min=-minCut
  // if minCut set, and P selected, then min=-maxCut
  // etc
  bool isPCut = m_d->ui_cuts.comboBox_momtype->currentText()=="P";
  
  const double minFromInterface=m_d->ui_cuts.doubleSpinBox_cut_minpt_gev->value()*CLHEP::GeV;
  const double maxFromInterface=m_d->ui_cuts.doubleSpinBox_cut_maxpt_gev->value()*CLHEP::GeV;
  
  double min=0.0,max=0.0;
  if (!isPCut) {
    //Pt cut
    min = (m_d->ui_cuts.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
    max = (m_d->ui_cuts.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
  } else {
    min = (m_d->ui_cuts.checkBox_cut_maxpt->isChecked() ? -maxFromInterface : -std::numeric_limits<double>::infinity());
    max = (m_d->ui_cuts.checkBox_cut_minpt->isChecked() ? -minFromInterface : std::numeric_limits<double>::infinity());
  }
  
  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
  
  if (max<min)
    return VP1Interval();
    
  return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval TrackSystemController::cutAllowedEta() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> TrackSystemController::cutAllowedPhi() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
QList<unsigned> TrackSystemController::cutRequiredNHits() const
{
  unsigned npixel = m_d->ui_cuts.checkBox_cut_nhits_pixel->isChecked() ? m_d->ui_cuts.spinBox_cut_nhits_pixel->value() : 0;
  unsigned nsct = m_d->ui_cuts.checkBox_cut_nhits_sct->isChecked() ? m_d->ui_cuts.spinBox_cut_nhits_sct->value() : 0;
  unsigned ntrt = m_d->ui_cuts.checkBox_cut_nhits_trt->isChecked() ? m_d->ui_cuts.spinBox_cut_nhits_trt->value() : 0;
  unsigned nmuon = m_d->ui_cuts.checkBox_cut_nhits_muon->isChecked() ? m_d->ui_cuts.spinBox_cut_nhits_muon->value() : 0;
  QList<unsigned> l;
  if (!npixel&&!nsct&&!ntrt&&!nmuon)
    return l;
  l << npixel << nsct << ntrt << nmuon;
  return l;
}

//____________________________________________________________________
bool TrackSystemController::cutTruthFromIROnly() const
{
  return m_d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::cutExcludeBarcodeZero() const
{
  return m_d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::cutTruthExcludeNeutrals() const
{
  return m_d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals->isChecked();
}

//____________________________________________________________________
//Returns false if "none available"
bool TrackSystemController::Imp::updateComboBoxContents(QComboBox*cb,QStringList l,QString& restoredSelection)
{
  //current selection:
  QString ct = cb->currentText();
  if (ct==Imp::noneAvailString)
    ct = "";

  bool save = cb->blockSignals(true);

  cb->clear();

  bool enabled = false;
  if (l.isEmpty()) {
    cb->addItem(Imp::noneAvailString);
    cb->setEnabled(false);
  } else {
    cb->addItems(l);

    int i = restoredSelection.isEmpty() ? -1 : cb->findText(restoredSelection);
    if (i<0)
      i = ct.isEmpty() ? -1 : cb->findText(ct);
    restoredSelection = "";

    if (i>=0) {
      cb->setCurrentIndex(i);
    } else {
      //Let's try to pick the default to be VP1Extrapolater over
      //AtlasExtrapolater over... whatever (same for fitters):
      int i_vp1(-1), i_atlas(-1);
      for (int j = 0; j <cb->count();++j) {
	if (i_vp1==-1&&cb->itemText(j).contains("vp1",Qt::CaseInsensitive))
	  i_vp1 = j;
	if (i_atlas==-1&&cb->itemText(j).contains("atlas",Qt::CaseInsensitive))
	  i_atlas = j;
      }
      if (i_vp1>=0)
	cb->setCurrentIndex(i_vp1);
      else if (i_atlas>=0)
	cb->setCurrentIndex(i_atlas);
    }
    //m_d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(true);
    enabled = true;
    cb->setEnabled(true);
  }

  if (!save)
    cb->blockSignals(false);
  return enabled;
}

//____________________________________________________________________
void TrackSystemController::availableFittersChanged(const QStringList& af)
{
  if (m_d->lastUpdatedAvailableFitters==af)
    return;
  m_d->lastUpdatedAvailableFitters=af;
  m_d->updateComboBoxContents(m_d->ui_int.comboBox_fitters,af,m_d->restoredLastFitter);//remember return val.
  m_d->updateFitPRDButtonState();

  possibleChange_trackFitter();
}

//____________________________________________________________________
void TrackSystemController::availableExtrapolatorsChanged(const QStringList& ae)
{
  if (m_d->lastUpdatedAvailableExtrapolators==ae)
    return;
  m_d->lastUpdatedAvailableExtrapolators=ae;

  if (!m_d->updateComboBoxContents(m_d->ui_extrap.comboBox_propagator,ae,m_d->restoredLastPropagator)) {
    m_d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(false);
    bool save1 = m_d->ui_extrap.radioButton_none->blockSignals(true);
    bool save2 = m_d->ui_extrap.radioButton_helical->blockSignals(true);
    bool save3 = m_d->ui_extrap.radioButton_athenaExtrapolator->blockSignals(true);
    m_d->ui_extrap.radioButton_none->setChecked(true);//Fixme: fall back to the helical instead!
    m_d->ui_extrap.radioButton_helical->setChecked(false);
    m_d->ui_extrap.radioButton_athenaExtrapolator->setChecked(false);
    if (!save1) m_d->ui_extrap.radioButton_none->blockSignals(false);
    if (!save2) m_d->ui_extrap.radioButton_helical->blockSignals(false);
    if (!save3) m_d->ui_extrap.radioButton_athenaExtrapolator->blockSignals(false);
  } else {
    m_d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(true);
  }

  possibleChange_propagator();
}

//____________________________________________________________________
bool TrackSystemController::orientAndZoomOnSingleSelection() const
{
  return m_d->ui_int.checkBox_selsingle_orientzoom->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::printInfoOnSingleSelection() const
{
  return m_d->ui_int.checkBox_selsingle_printinfo->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::printVerboseInfoOnSingleSelection() const
{
  return printInfoOnSingleSelection() && m_d->ui_int.checkBox_selsingle_printinfo_verbose->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::printTotMomentumOnMultiTrackSelection() const
{
  return m_d->ui_int.checkBox_sel_printtotmom->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::showTotMomentumOnMultiTrackSelection() const
{
  return m_d->ui_int.checkBox_sel_showtotmom->isChecked();
}

//____________________________________________________________________
void TrackSystemController::emitRerandomise()
{
  messageVerbose("Emitting rerandomise");
  emit rerandomise();
}

//____________________________________________________________________
void TrackSystemController::emitRefit()
{
  messageVerbose("Emitting refit");
  emit refit();
}

//____________________________________________________________________
void TrackSystemController::Imp::updateFitPRDButtonState()
{
  switch (theclass->fitterMode()) {
    case TrackCommonFlags::FROMPRDS: ui_int.pushButton_refit->setEnabled( numberOfSelectedPRDs>=3 && theclass->trackFitter() );break;
    case TrackCommonFlags::REFITSINGLETRACK: ui_int.pushButton_refit->setEnabled( numberOfSelectedTracks>=1 && theclass->trackFitter() );break;
    case TrackCommonFlags::EXTENDTRACKWITHPRDS: ui_int.pushButton_refit->setEnabled( numberOfSelectedTracks>=1 && numberOfSelectedPRDs>=1 && theclass->trackFitter() );break;
    case TrackCommonFlags::COMBINETWOTRACKS: ui_int.pushButton_refit->setEnabled( numberOfSelectedTracks==2 && theclass->trackFitter() );break;
  }
}

//____________________________________________________________________
void TrackSystemController::setNumberOfSelectedPRDsAndTracks(unsigned prds, unsigned trks)
{
  if (m_d->numberOfSelectedPRDs==prds && m_d->numberOfSelectedTracks==trks)
    return;
  m_d->numberOfSelectedPRDs=prds;
  m_d->numberOfSelectedTracks=trks;
  
  switch (fitterMode()) {
    case TrackCommonFlags::FROMPRDS:            m_d->ui_int.pushButton_refit->setText("Fit track from "+str(prds)+" PRDs");break;
    case TrackCommonFlags::REFITSINGLETRACK:    m_d->ui_int.pushButton_refit->setText("Fit track");break;
    case TrackCommonFlags::EXTENDTRACKWITHPRDS: m_d->ui_int.pushButton_refit->setText("Extend track with "+str(prds)+" PRDs");break;
    case TrackCommonFlags::COMBINETWOTRACKS:    m_d->ui_int.pushButton_refit->setText("Combine");break;
  }
  
  
  // FIXME Need to redo this method so that it can handle all fitter modes
  
  m_d->updateFitPRDButtonState();
}

void TrackSystemController::updateFitPRDButtonState(){
  messageVerbose("updateFitPRDButtonState");
  
  switch (fitterMode()) {
    case TrackCommonFlags::FROMPRDS:            m_d->ui_int.pushButton_refit->setText("Fit track from "+str(m_d->numberOfSelectedPRDs)+" PRDs");break;
    case TrackCommonFlags::REFITSINGLETRACK:    m_d->ui_int.pushButton_refit->setText("Fit track");break;
    case TrackCommonFlags::EXTENDTRACKWITHPRDS: m_d->ui_int.pushButton_refit->setText("Extend track with "+str(m_d->numberOfSelectedPRDs)+" PRDs");break;
    case TrackCommonFlags::COMBINETWOTRACKS:    m_d->ui_int.pushButton_refit->setText("Combine");break;
  }
  
  m_d->updateFitPRDButtonState();
}

QTreeWidget* TrackSystemController::trackObjBrowser() const
{
    return m_d->objBrowserWidget;
}

TrackSysCommonData * TrackSystemController::common() const {
  return m_d->common;
}
void TrackSystemController::setCommonData(TrackSysCommonData * common){
  m_d->common=common;
}

void TrackSystemController::objectBrowserClicked(QTreeWidgetItem * item, int){
  messageVerbose("objectBrowserClicked for "+item->text(0));
  
  VP1TrackSystem* sys = dynamic_cast<VP1TrackSystem*>(systemBase());
  if (!sys){
    messageVerbose("TrackSystemController::objectBrowserClicked: Couldn't get VP1TrackSystem pointer");
    return;
  }
  
  //sys->deselectAll(); // FIXME. necessary?

  SoNode* node = common()->node(item);
  if (node) {
    // okay, have track
    SoCooperativeSelection * sel = sys->selTracks();
    sel->select(node);
  } else {
    // maybe it's a TSOS? Check first that it has a parent 
    if (item->parent()) node = common()->node(item->parent());
    if ( !node && item->parent()->parent() ) node = common()->node(item->parent()->parent()); // Try one more up (ugly, but shouldn't ever be deeper than this)
    if (node) {
      // yes, so now get index within track, as we can hopefully use this to find the AscObj_TSOS
      unsigned int index = item->parent()->indexOfChild(item);// should correspond to the TSOS number
      messageVerbose("TrackSystemController::objectBrowserClicked: item has index of "+QString::number(index));
      TrackHandle_TrkTrack* trkHandle = dynamic_cast<TrackHandle_TrkTrack*>(common()->trackHandle(node));
      if (trkHandle && trkHandle->trkTrackPointer()) {
        if (index<trkHandle->trkTrackPointer()->trackStateOnSurfaces()->size() ){
          // in range
          const Trk::TrackStateOnSurface* tsos = (*trkHandle->trkTrackPointer()->trackStateOnSurfaces())[index];
          // now find matching AscObj_TSOS
          QList<AssociatedObjectHandleBase*> list = trkHandle->getVisibleMeasurements();
          for (int i = 0; i < list.size(); ++i) {
            messageVerbose("TrackSystemController::objectBrowserClicked: checking ASC "+QString::number(i));

            AscObj_TSOS* asc = dynamic_cast<AscObj_TSOS*>(list.at(i));
            if (asc && asc->trackStateOnSurface()==tsos) {
               messageVerbose("TrackSystemController::objectBrowserClicked: this ASC matches "+QString::number(i));
              //asc->zoomView();
               common()->ascObjSelectionManager()->pretendUserClicked(asc);          
            } else {
              messageVerbose("TrackSystemController::objectBrowserClicked: no matching Asc found");
            }
          } // for loop
        } else {
          messageVerbose("TrackSystemController::objectBrowserClicked: index of  "+QString::number(index)+" is greater than number of TSOSes:"+QString::number(trkHandle->trkTrackPointer()->trackStateOnSurfaces()->size()));
        }
      }
    } else {
      messageVerbose("TrackSystemController::objectBrowserClicked: Couldn't get node pointer. Maybe object not visible?");
    }
  }
  
  
  // if (selTrack){
  //   SoCooperativeSelection * sel = sys->selTracks();
  //   sel->select(node);
  // } else {
  //   common()->ascObjSelectionManager()->pretendUserClicked();
  // }
  // 
  // SoCooperativeSelection * sel = sys->selTracks();
  // SoCooperativeSelection * selAsc  = dynamic_cast<SoCooperativeSelection*>(common()->ascObjSelectionManager()->getAscObjAttachSep());
  // SoSeparator* eventRoot = common()->ascObjSelectionManager()->eventRoot();  
  // 
  // SoSearchAction mySearchAction;
  // mySearchAction.setNode(node);
  // // SoSeparator* eventRoot = common()->ascObjSelectionManager()->eventRoot();
  // mySearchAction.apply(eventRoot); 
  // SoPath* path=mySearchAction.getPath();
  // if ( !path ) { 
  //   messageVerbose("TrackSystemController::objectBrowserClicked: Couldn't get SoPath");
  //   return;
  // } else {
  //   messageVerbose("TrackSystemController::objectBrowserClicked: got SoPath. Will now try to select.");
  //   messageVerbose("pathlength="+QString::number(path->getLength()));
  // }
  // 
  // //sel->select(path); //Doesn't work.
  // // messageVerbose("findpath="+QString::number(sel->findPath(path)));
  // sel->select(node); // Doesn't work.
  // if (sel->getNumSelected()==0){
  //   messageVerbose("TrackSystemController::objectBrowserClicked: Couldn't select. Trying with ASC sel node.");
  //   
  //   selAsc->select(node);
  //   if (selAsc->getNumSelected()==0){
  //     messageVerbose("TrackSystemController::objectBrowserClicked: Couldn't select. Trying with ASC sel using path.");
  //     
  //     selAsc->select(path);
  //   }
  // }
  // // messageVerbose("findpath="+QString::number(sel->findPath(path)));
  // 
  // // sel->touch();
  // messageVerbose("numselected="+QString::number(sel->getNumSelected()));
  
 // path->unref();
}

std::vector<double> TrackSystemController::alignmentShiftValue() 
{
  std::vector<double> values(6,0.0);
  values[0]=m_d->ui_shiftmuonchambers.doubleSpinBox_tra_s->value();
  values[1]=m_d->ui_shiftmuonchambers.doubleSpinBox_tra_z->value();
  values[2]=m_d->ui_shiftmuonchambers.doubleSpinBox_tra_t->value();
  values[3]=m_d->ui_shiftmuonchambers.doubleSpinBox_rot_s->value();
  values[4]=m_d->ui_shiftmuonchambers.doubleSpinBox_rot_z->value();
  values[5]=m_d->ui_shiftmuonchambers.doubleSpinBox_rot_t->value();
  return values;
}
int TrackSystemController::alignmentShiftLevel()
{
  if (m_d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 0")
     return 0;
   else if (m_d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 1")
     return 1;
   else if (m_d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 2")
     return 2;
   else if (m_d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 3")
     return 3;
   return 0;
}

bool TrackSystemController::doTrackLabels() 
{
  return m_d->ui_col.groupBox_labels->isChecked();
}

float TrackSystemController::trackLabelTrkOffset() 
{
  return static_cast<float>(m_d->ui_col.horizontalSlider_labels_trkOffset->value())/static_cast<float>(m_d->ui_col.horizontalSlider_labels_trkOffset->maximum());
}

QList<int> TrackSystemController::trackLabelPosOffset() 
{
  QList<int> values;
  values << m_d->ui_col.horizontalSlider_labels_xOffset->value();
  values << m_d->ui_col.horizontalSlider_labels_yOffset->value();
  values << m_d->ui_col.horizontalSlider_labels_zOffset->value();
  return values;
}

int TrackSystemController::labelXOffset() 
{
  return m_d->ui_col.horizontalSlider_labels_xOffset->value();
}

int TrackSystemController::labelYOffset() 
{
  return m_d->ui_col.horizontalSlider_labels_yOffset->value();
}

int TrackSystemController::labelZOffset() 
{
  return m_d->ui_col.horizontalSlider_labels_zOffset->value();
}

TrackSystemController::TrackLabelModes TrackSystemController::trackLabels(){
  if (!m_d->ui_col.groupBox_labels->isChecked())
    return TrackSystemController::NoLabels;
  
  TrackLabelModes labels=TrackSystemController::NoLabels;
  if (m_d->ui_col.checkBox_trkLabels_p->isChecked())    labels |= TrackSystemController::P;
  if (m_d->ui_col.checkBox_trkLabels_Pt->isChecked())   labels |= TrackSystemController::Pt;
  if (m_d->ui_col.checkBox_trkLabels_direction->isChecked())   labels |= TrackSystemController::Direction;
  if (m_d->ui_col.checkBox_trkLabels_pid->isChecked())  labels |= TrackSystemController::Pid;
  if (m_d->ui_col.checkBox_trkLabels_hits->isChecked()) labels |= TrackSystemController::Hits;
  if (m_d->ui_col.checkBox_trkLabels_fitQuality->isChecked()) labels |= TrackSystemController::FitQuality;
  return labels;
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME TrackSystemController
#include "VP1Base/VP1ControllerMacros.h"
POSSIBLECHANGE_IMP(shownTrackParts)
POSSIBLECHANGE_IMP(vertexProjectionAngle)
// POSSIBLECHANGE_IMP(trackTubeRadius)
POSSIBLECHANGE_IMP(trackLabels)
POSSIBLECHANGE_IMP(trackLabelTrkOffset)
POSSIBLECHANGE_IMP(trackLabelPosOffset)
POSSIBLECHANGE_IMP(shownTSOSParts)
POSSIBLECHANGE_IMP(customColouredTSOSParts)
POSSIBLECHANGE_IMP(useShortTRTMeasurements)
POSSIBLECHANGE_IMP(useShortMDTMeasurements)
POSSIBLECHANGE_IMP(drawMeasGlobalPositions)
POSSIBLECHANGE_IMP(measurementsShorttubesScale)
POSSIBLECHANGE_IMP(nStdDevForParamErrors)
POSSIBLECHANGE_IMP(parTubeErrorsDrawCylinders)
POSSIBLECHANGE_IMP(numberOfPointsOnCircles)
POSSIBLECHANGE_IMP(materialEffectsOnTrackScale)
POSSIBLECHANGE_IMP(propagator)
POSSIBLECHANGE_IMP(propagationOptions)
POSSIBLECHANGE_IMP(propMaxRadius)
POSSIBLECHANGE_IMP(trackFitter)
POSSIBLECHANGE_IMP(selectionMode)
POSSIBLECHANGE_IMP(showTruthAscObjs)
POSSIBLECHANGE_IMP(assocObjDetailLevel)
POSSIBLECHANGE_IMP(cutAllowedPt)
POSSIBLECHANGE_IMP(cutAllowedEta)
POSSIBLECHANGE_IMP(cutAllowedPhi)
POSSIBLECHANGE_IMP(cutRequiredNHits)
POSSIBLECHANGE_IMP(cutTruthFromIROnly)
POSSIBLECHANGE_IMP(cutExcludeBarcodeZero)
POSSIBLECHANGE_IMP(cutTruthExcludeNeutrals)
POSSIBLECHANGE_IMP(cutOnlyVertexAssocTracks)
POSSIBLECHANGE_IMP(showTotMomentumOnMultiTrackSelection)
