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
  : VP1Controller(sys,"TrackSystemController"), d(new Imp)
{
  d->theclass = this;
  d->lastUpdatedAvailableExtrapolators = QStringList("<dummy>");//special.
  d->lastUpdatedAvailableFitters = QStringList("<dummy>");//special.

  //Stuff with tools waits until ::initTools() is called:
  d->toolaccesshelper = 0;
  d->toolhelper_extrapolators = 0;
  d->toolhelper_fitters = 0;
  d->matmixer = 0;

  d->ui.setupUi(this);
  d->trackcollwidget = new TrackCollWidget;
  setupCollWidgetInScrollArea(d->ui.collWidgetScrollArea,d->trackcollwidget);

  initDialog(d->ui_col, d->ui.pushButton_settings_colouring);
  initDialog(d->ui_extrap, d->ui.pushButton_settings_extrapolation);
  initDialog(d->ui_int, d->ui.pushButton_settings_interactions);
  initDialog(d->ui_proj, d->ui.pushButton_settings_projections);
  initDialog(d->ui_cuts, d->ui.pushButton_settings_cuts);
  initDialog(d->ui_ascobjs, d->ui.pushButton_settings_details);
  initDialog(d->ui_objBrowser, d->ui.pushButton_ObjectBrowser);

  //init:
  d->numberOfSelectedPRDs=1;//anything != 0
  setNumberOfSelectedPRDsAndTracks(0,0);
  
  d->initMaterials();

  d->materialFallback = new SoMaterial;
  d->materialFallback->ref();

  d->ui_cuts.etaPhiCutWidget->setSystemBasePointer(systemBase());

  //Draw Styles / Complexity:
  // VP1QtInventorUtils::setLimitsLineWidthSlider(d->ui_col.horizontalSlider_trackWidth);
  // VP1QtInventorUtils::setValueLineWidthSlider(d->ui_col.horizontalSlider_trackWidth,1.0);
  // VP1QtInventorUtils::setLimitsLineWidthSlider(d->ui_ascobjs.horizontalSlider_linewidths);
  // VP1QtInventorUtils::setValueLineWidthSlider(d->ui_ascobjs.horizontalSlider_linewidths,1.0);
  // VP1QtInventorUtils::setLimitsPointSizeSlider(d->ui_ascobjs.horizontalSlider_pointsizes);
  // VP1QtInventorUtils::setValuePointSizeSlider(d->ui_ascobjs.horizontalSlider_pointsizes,3.0);

  // d->trackDrawStyle = new SoDrawStyle;
  // d->trackDrawStyle->setName("TrackDrawStyle");
  // d->trackDrawStyle->ref();
  //  updateTrackDrawStyle();

  // addUpdateSlot(SLOT(updateTrackDrawStyle()));
  // connectToLastUpdateSlot(d->ui_col.horizontalSlider_trackWidth);

  d->ascObjDrawStyle = new SoDrawStyle;
  d->ascObjDrawStyle->setName("AscObjDrawStyle");
  d->ascObjDrawStyle->ref();
  addUpdateSlot(SLOT(updateAscObjDrawStyle()));
  connectToLastUpdateSlot(d->ui_ascobjs.horizontalSlider_linewidths);
  connectToLastUpdateSlot(d->ui_ascobjs.horizontalSlider_pointsizes);

  d->ascObjComplexity = new SoComplexity;
  d->ascObjComplexity->setName("AscObjComplexity");
  d->ascObjComplexity->ref();
  addUpdateSlot(SLOT(updateAscObjComplexity()));
  connectToLastUpdateSlot(d->ui_ascobjs.horizontalSlider_complexity);

  // d->trackLightModel = new SoLightModel;
  // d->trackLightModel->setName("TrackLightModel");
  // d->trackLightModel->ref();
  // addUpdateSlot(SLOT(updateTrackLightModel()));
  // connectToLastUpdateSlot(d->ui_col.checkBox_tracksUseBaseLightModel);

  //Refit ui is dependent on env variable:
  d->ui_int.radioButton_selmode_trackfits->setVisible(VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_ENABLEREFIT"));
  d->ui_int.groupBox_refitting->setEnabled(VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_ENABLEREFIT"));
  connect(d->ui_int.comboBox_fitterMode,SIGNAL(currentIndexChanged(int)),this,SLOT(updateFitPRDButtonState()));
  
  //d->ui_ascobjs.checkBox_usecolour_meas_outliers->setEnabled(false);
  //d->ui_ascobjs.matButton_meas_outliers->setEnabled(false);
  //d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale->setEnabled(false);
  //d->ui_ascobjs.label_mateffects_scale->setEnabled(false);
  // d->ui_ascobjs.checkBox_materialeffectsontrack_forceposontrack->setEnabled(false);
  // d->ui_extrap.groupBox_otheroptions->setEnabled(false);
  // d->ui_extrap.radioButton_helical->setEnabled(false);


  //Disable elements based on job configuration:

  if (!VP1JobConfigInfo::hasMuonGeometry()) {
    d->ui_proj.checkBox_projections_muonchambers->setChecked(false);
    d->ui_proj.checkBox_projections_muonchambers->setEnabled(false);
  }
  if (!(VP1JobConfigInfo::hasPixelGeometry()
        &&VP1JobConfigInfo::hasSCTGeometry()
        &&VP1JobConfigInfo::hasTRTGeometry())) {
    d->ui_proj.checkBox_projections_indet->setChecked(false);
    d->ui_proj.checkBox_projections_indet->setEnabled(false);
  }

  //These we init explicitly since we might otherwise trigger a change
  //before initlastvars (fixme: is this true and the right fix??)
  d->last_propagator = 0;
  d->last_trackFitter = 0;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // -> shownTrackParts
  addUpdateSlot(SLOT(possibleChange_shownTrackParts()));
  // connectToLastUpdateSlot(d->ui_col.checkBox_hideactualpaths);
  connectToLastUpdateSlot(d->ui_proj.checkBox_projections_muonchambers);
  connectToLastUpdateSlot(d->ui_proj.checkBox_projections_indet);
  connectToLastUpdateSlot(d->ui_proj.groupBox_projections_vertex);

  addUpdateSlot(SLOT(possibleChange_vertexProjectionAngle()));
  connectToLastUpdateSlot(d->ui_proj.spinBox_projections_vertex);


  // // -> trackTubeRadius
  // addUpdateSlot(SLOT(possibleChange_trackTubeRadius()));
  // connectToLastUpdateSlot(d->ui_col.checkBox_trackTubes);
  // connectToLastUpdateSlot(d->ui_col.doubleSpinBox_trackTubesRadiusMM);

  // -> track labels
  addUpdateSlot(SLOT(possibleChange_trackLabels()));
  connectToLastUpdateSlot(d->ui_col.checkBox_trkLabels_Pt);
  connectToLastUpdateSlot(d->ui_col.checkBox_trkLabels_p);
  connectToLastUpdateSlot(d->ui_col.checkBox_trkLabels_direction);
  connectToLastUpdateSlot(d->ui_col.checkBox_trkLabels_hits);
  connectToLastUpdateSlot(d->ui_col.checkBox_trkLabels_pid);
  connectToLastUpdateSlot(d->ui_col.checkBox_trkLabels_fitQuality);
  connectToLastUpdateSlot(d->ui_col.groupBox_labels);
  
  addUpdateSlot(SLOT(possibleChange_trackLabelTrkOffset()));
  connectToLastUpdateSlot(d->ui_col.horizontalSlider_labels_trkOffset);

  addUpdateSlot(SLOT(possibleChange_trackLabelPosOffset()));
  connectToLastUpdateSlot(d->ui_col.horizontalSlider_labels_xOffset);
  connectToLastUpdateSlot(d->ui_col.horizontalSlider_labels_yOffset);
  connectToLastUpdateSlot(d->ui_col.horizontalSlider_labels_zOffset);

  
  // -> shownTSOSParts
  addUpdateSlot(SLOT(possibleChange_shownTSOSParts()));
  connectToLastUpdateSlot(d->ui_ascobjs.groupBox_measurements);
  connectToLastUpdateSlot(d->ui_ascobjs.groupBox_parameters);
  connectToLastUpdateSlot(d->ui_ascobjs.groupBox_errors);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_parametererrors);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_measurementerrors);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_parerror_hideperigeeerrors);
  connectToLastUpdateSlot(d->ui_ascobjs.groupBox_materialeffectsontrack);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE);
  connectToLastUpdateSlot(d->ui_ascobjs.groupBox_surfaces);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces);
  
  addUpdateSlot(SLOT(possibleChange_drawMeasGlobalPositions()));
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_measurements_drawGP);

  // -> customColouredTSOSParts
  addUpdateSlot(SLOT(possibleChange_customColouredTSOSParts()));

  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_usecolour_measurements);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_usecolour_meas_outliers);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_usecolour_parameters);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_useHoleColour_parameters);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_usecolour_parametererrors);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack);
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_usecolour_surfaces);

  // -> useShortTRTMeasurements
  addUpdateSlot(SLOT(possibleChange_useShortTRTMeasurements()));
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_measurements_shorttubes_trt);

  // -> useShortMDTMeasurements
  addUpdateSlot(SLOT(possibleChange_useShortMDTMeasurements()));
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_measurements_shorttubes_mdt);

  // -> useShortMDTMeasurements
  addUpdateSlot(SLOT(possibleChange_measurementsShorttubesScale()));
  connectToLastUpdateSlot(d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale);

  // -> nStdDevForParamErrors
  addUpdateSlot(SLOT(possibleChange_nStdDevForParamErrors()));
  connectToLastUpdateSlot(d->ui_ascobjs.doubleSpinBox_parerror_stddev);

  // -> parTubeErrorsDrawCylinders
  addUpdateSlot(SLOT(possibleChange_parTubeErrorsDrawCylinders()));
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_parerror_drawcylinder);

  // -> numberOfPointsOnCircles
  addUpdateSlot(SLOT(possibleChange_numberOfPointsOnCircles()));
  connectToLastUpdateSlot(d->ui_ascobjs.horizontalSlider_complexity);

  // -> materialEffectsOnTrackScale
  addUpdateSlot(SLOT(possibleChange_materialEffectsOnTrackScale()));
  connectToLastUpdateSlot(d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale);

  // -> selectionMode
  addUpdateSlot(SLOT(possibleChange_selectionMode()));
  connectToLastUpdateSlot(d->ui_int.radioButton_selmode_single);
  connectToLastUpdateSlot(d->ui_int.radioButton_selmode_multitracks);
  connectToLastUpdateSlot(d->ui_int.radioButton_selmode_trackfits);

  // -> showTruthAscObjs
  addUpdateSlot(SLOT(possibleChange_showTruthAscObjs()));
  connectToLastUpdateSlot(d->ui_ascobjs.checkBox_truthtracks_display_points);

  // -> assocObjDetailLevel
  addUpdateSlot(SLOT(possibleChange_assocObjDetailLevel()));
  connectToLastUpdateSlot(d->ui_ascobjs.comboBox_assocobj_detaillevel);

  // -> cutAllowedP/Pt
  addUpdateSlot(SLOT(possibleChange_cutAllowedPt()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_minpt);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_maxpt);
  connectToLastUpdateSlot(d->ui_cuts.doubleSpinBox_cut_minpt_gev);
  connectToLastUpdateSlot(d->ui_cuts.doubleSpinBox_cut_maxpt_gev);
  connectToLastUpdateSlot(d->ui_cuts.comboBox_momtype);

  // -> cutAllowedEta
  addUpdateSlot(SLOT(possibleChange_cutAllowedEta()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  // -> cutAllowedPhi
  addUpdateSlot(SLOT(possibleChange_cutAllowedPhi()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  // -> cutRequiredNHits();
  addUpdateSlot(SLOT(possibleChange_cutRequiredNHits()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_nhits_pixel);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_nhits_sct);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_nhits_trt);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_nhits_muon);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_nhits_pixel);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_nhits_sct);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_nhits_trt);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_nhits_muon);

  // -> cutTruthFromIROnly
  addUpdateSlot(SLOT(possibleChange_cutTruthFromIROnly()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR);

  // -> cutExcludeBarcodeZero
  addUpdateSlot(SLOT(possibleChange_cutExcludeBarcodeZero()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0);

  // -> cutTruthExcludeNeutrals
  addUpdateSlot(SLOT(possibleChange_cutTruthExcludeNeutrals()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals);

  // -> cutOnlyVertexAssocTracks
  addUpdateSlot(SLOT(possibleChange_cutOnlyVertexAssocTracks()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_vertexAssociated);

  // -> showTotMomentumOnMultiTrackSelection
  addUpdateSlot(SLOT(possibleChange_showTotMomentumOnMultiTrackSelection()));
  connectToLastUpdateSlot(d->ui_int.checkBox_sel_showtotmom);

  // Special: -> rerandomise
  connect(d->ui_col.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));

  // Special: -> refit
  connect(d->ui_int.pushButton_refit,SIGNAL(clicked()),this,SLOT(emitRefit()));

  // TrackObjBrowser
  messageVerbose("Enabling object browser");
  d->objBrowserWidget = d->ui_objBrowser.treeWidget;
  d->objBrowserWidget->setSortingEnabled(false);
  QStringList l; 
  l<<"Object"<<"Information";
  d->objBrowserWidget->setHeaderLabels(l);
  connect(d->objBrowserWidget,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(objectBrowserClicked(QTreeWidgetItem *, int)));
    
  // Special alignment tests
  if (VP1QtUtils::environmentVariableIsOn("VP1_TRKSYS_SHIFTCHAMBERS")) {
    messageVerbose("Enabling shifting of Muon Chambers");
    QWidget *widget = new QWidget;
    d->ui_shiftmuonchambers.setupUi(widget);
    messageVerbose("Making connection");
    widget->show();
    connect(d->ui_shiftmuonchambers.pushButton_Update,SIGNAL(clicked()),sys,SLOT(updateAlignment()));
  }

  // we want "Print information" on single track selection turned ON by default
  d->ui_int.checkBox_selsingle_printinfo->setChecked(true);
  
  // Since TrkVolumesSvc isn't working anymore, hardcode values. (Remove when we move to new extrapolator)
  d->calorimeterEntryLayer      = new Trk::Volume(0, new Trk::CylinderVolumeBounds(1100.0, 3200.0));
  d->muonSpectrometerEntryLayer = new Trk::Volume(0, new Trk::CylinderVolumeBounds(4250.0, 6779.0));
  d->muonSpectrometerExitLayer  = new Trk::Volume(0, new Trk::CylinderVolumeBounds(15000.0, 21000.0)); // FIXME! Put in correct values. EJWM
  initLastVars();
}

//____________________________________________________________________
void TrackSystemController::initTools()
{
  d->toolaccesshelper = new VP1ToolAccessHelper(systemBase());

  if (VP1JobConfigInfo::hasPixelGeometry()||VP1JobConfigInfo::hasSCTGeometry()
      ||VP1JobConfigInfo::hasTRTGeometry()||VP1JobConfigInfo::hasMuonGeometry()) {

    //Ensure special VP1 extrapolator and fitters are created:
    Imp::ensureExtrapolatorsCreated(systemBase());
    Imp::ensureFittersCreated(systemBase());

    messageVerbose("Setting up tool helper to monitor extrapolators - start");
    d->toolhelper_extrapolators = new VP1AvailableToolsHelper(systemBase(),this);
    //Watch for extrapolators (and Fatras extrapolators are no-go!)
    d->toolhelper_extrapolators->addMonitoredType("Trk::Extrapolator",(QStringList() << "*Fatras*"));
    connect(d->toolhelper_extrapolators,SIGNAL(availableToolsChanged(const QStringList&)),
	    this,SLOT(availableExtrapolatorsChanged(const QStringList&)));
    availableExtrapolatorsChanged(d->toolhelper_extrapolators->availableTools());
    messageVerbose("Setting up tool helper to monitor extrapolators - end");

    messageVerbose("Setting up tool helper to monitor fitters - start");
    d->toolhelper_fitters = new VP1AvailableToolsHelper(systemBase(),this);
    QStringList fittertypes;
    fittertypes << "Trk::KalmanFitter"
                << "Trk::GlobalChi2Fitter"
                << "Trk::GaussianSumFilter"
                << "Trk::DistributedKalmanFilter"
                << "Trk::DeterministicAnnealingFilter";
    d->toolhelper_fitters->addMonitoredTypes(fittertypes);
    connect(d->toolhelper_fitters,SIGNAL(availableToolsChanged(const QStringList&)),
	    this,SLOT(availableFittersChanged(const QStringList&)));
    availableFittersChanged(d->toolhelper_fitters->availableTools());
    messageVerbose("Setting up tool helper to monitor fitters - end");
  } else {
    d->toolhelper_extrapolators = 0;
    d->toolhelper_fitters = 0;
    d->ui_extrap.comboBox_propagator->clear();
    d->ui_extrap.comboBox_propagator->addItem(Imp::noneAvailString);
    d->ui_extrap.comboBox_propagator->setEnabled(false);
    d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(false);
    d->ui_extrap.radioButton_none->setChecked(true);
    d->ui_int.comboBox_fitters->clear();
    d->ui_int.comboBox_fitters->addItem(Imp::noneAvailString);
    d->ui_int.comboBox_fitters->setEnabled(false);
    //FIXME: Disable radioButton_selmode_trackfits, comboBox_fitters, lineEdit_fittedTrackCollName, pushButton_refit
  }

  // -> propagator
  //
  addUpdateSlot(SLOT(possibleChange_propagator()));
  connectToLastUpdateSlot(d->ui_extrap.radioButton_none);
  connectToLastUpdateSlot(d->ui_extrap.radioButton_athenaExtrapolator);
  connectToLastUpdateSlot(d->ui_extrap.comboBox_propagator);

  addUpdateSlot(SLOT(possibleChange_propagationOptions()));
  connectToLastUpdateSlot(d->ui_extrap.checkBox_ignoreMEoT); 
  connectToLastUpdateSlot(d->ui_extrap.checkBox_extendAllInDetTracks); 
  // connectToLastUpdateSlot(d->ui_extrap.comboBox_extendAllInDetTracksToHere); 
  d->last_propagationOptions=TrackSystemController::NoPropOptions;
  if (d->ui_extrap.checkBox_ignoreMEoT->isChecked())    d->last_propagationOptions |= TrackSystemController::IgnoreMEOT; //!< Really needed here??? FIXME
  if (d->ui_extrap.checkBox_extendAllInDetTracks->isChecked())    d->last_propagationOptions |= TrackSystemController::ExtendTrack; //!< Really needed here??? FIXME

  addUpdateSlot(SLOT(possibleChange_propMaxRadius()));
  connectToLastUpdateSlot(d->ui_extrap.checkBox_maxRadius); 
  connectToLastUpdateSlot(d->ui_extrap.spinBox_maxRadiusValue); 

  connect(d->ui_extrap.comboBox_extendAllInDetTracksToHere, SIGNAL(currentIndexChanged(int)),this, SLOT(emitExtrapolateToHereChanged(int)));
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
  delete d->toolaccesshelper;
  delete d->matmixer;
  d->materialFallback->unref();
  // d->trackDrawStyle->unref();
  // d->trackLightModel->unref();
  d->ascObjDrawStyle->unref();
  d->ascObjComplexity->unref();
  
  delete d->calorimeterEntryLayer     ;
  delete d->muonSpectrometerEntryLayer;
  delete d->muonSpectrometerExitLayer ;
    
    // delete d->objBrowserWidget;
  delete d;
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
  // s.save(VP1QtInventorUtils::getValueLineWidthSlider(d->ui_col.horizontalSlider_trackWidth));//Version 1+ GONE WITH VERSION 17  
  // s.widgetHandled(d->ui_col.horizontalSlider_trackWidth);

  //Tracks base light model:
  // s.save(d->ui_col.checkBox_tracksUseBaseLightModel);//version 4+ GONE WITH VERSION 17 

	// --- Projections options ---
	s.save(QString("Projections options"));
	s.save(d->ui_proj.checkBox_projections_indet);
	s.save(d->ui_proj.checkBox_projections_muonchambers);
	s.save(d->ui_proj.groupBox_projections_vertex); //v15
	s.save(d->ui_proj.spinBox_projections_vertex);//v15
	s.save(d->ui_proj.horizontalSlider_projections_vertex);//v15
	// s.save(d->ui_col.checkBox_hideactualpaths); GONE WITH VERSION 17
	// ---------------------------


  //Display options - track tubes
  // s.save(d->ui_col.checkBox_trackTubes);//version 8+ GONE WITH VERSION 17 
  // s.save(d->ui_col.doubleSpinBox_trackTubesRadiusMM);//version 8+ GONE WITH VERSION 17 

  //Version <= 3 had bool here


  // ----- Propagation options -----
  s.save(QString("Propagation options"));
  s.save(d->ui_extrap.radioButton_none,
		 d->ui_extrap.radioButton_helical,
		 d->ui_extrap.radioButton_athenaExtrapolator);//version 4+
  s.save(d->ui_extrap.comboBox_propagator);//NB: We restore this in a slightly special way
  // -----------------------------------




  // ----- Interactions options -----
  s.save(QString("Interactions options"));
  s.save(d->ui_int.lineEdit_fittedTrackCollName);//version 6+
  s.save(d->ui_int.comboBox_fitters);//version 6+
  // -----------------------------------



  //Version <=2 had bool here

  //version 0 output an integer followed by two bools here

  //Display options - Trk::Tracks
  //Version <=2 had bool here
  s.save(d->ui_ascobjs.comboBox_assocobj_detaillevel);
  //Version <=2 had two bool here

  //Display options - Truth tracks:
  s.save(d->ui_ascobjs.checkBox_truthtracks_display_points);

  // --- Display options - Colours ---
  s.save(QString("Colour options"));
  //Display options - Colour by pdg:
  s.save(d->ui_col.matButton_electrons);
  s.save(d->ui_col.matButton_muons);
  s.save(d->ui_col.matButton_pions);
  s.save(d->ui_col.matButton_protons);
  s.save(d->ui_col.matButton_chargedkaons);
  s.save(d->ui_col.matButton_othercharged);
  s.save(d->ui_col.matButton_neutrons);
  s.save(d->ui_col.matButton_photons);
  s.save(d->ui_col.matButton_neutrinos);
  s.save(d->ui_col.matButton_otherneutrals);

  //Display options - Colour by charge:
  s.save(d->ui_col.matButton_charge_neg);//version 2+
  s.save(d->ui_col.matButton_charge_pos);//version 2+
  s.save(d->ui_col.matButton_charge_neutral);//version 2+

  //Display options - Colour by momentum:
  s.save(d->ui_col.matButton_0GeV);//version 2+
  s.save(d->ui_col.matButton_15GeV);//version 2+
  // -----------------------------------








  // --- Cuts options ---
  s.save(QString("Cuts options"));
  //Cuts - general:
  s.save(d->ui_cuts.checkBox_cut_minpt);
  s.save(d->ui_cuts.doubleSpinBox_cut_minpt_gev);
  s.save(d->ui_cuts.checkBox_cut_maxpt);
  s.save(d->ui_cuts.doubleSpinBox_cut_maxpt_gev);
  s.save(d->ui_cuts.comboBox_momtype);// version 10+

  s.save(d->ui_cuts.etaPhiCutWidget);//Version 7+
  //Versions <=6 had 2*bool, 2*double, 1*bool, 1*obsoletephisectionstate here.

  //Cuts - number of hits:
  s.save(d->ui_cuts.checkBox_cut_nhits_pixel);//Version 9+
  s.save(d->ui_cuts.checkBox_cut_nhits_sct);//Version 9+
  s.save(d->ui_cuts.checkBox_cut_nhits_trt);//Version 9+
  s.save(d->ui_cuts.checkBox_cut_nhits_muon);//Version 9+
  s.save(d->ui_cuts.spinBox_cut_nhits_pixel);//Version 9+
  s.save(d->ui_cuts.spinBox_cut_nhits_sct);//Version 9+
  s.save(d->ui_cuts.spinBox_cut_nhits_trt);//Version 9+
  s.save(d->ui_cuts.spinBox_cut_nhits_muon);//Version 9+

  //Cuts - truth:
  s.save(d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR);
  s.save(d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals);
  s.save(d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0);

  s.save(d->ui_cuts.checkBox_vertexAssociated);//Version 14+
  // -----------------------------------






  // --- Interactions options ---
    s.save(QString("Interactions options"));
  //Interactions - selection mode:
  s.save(d->ui_int.radioButton_selmode_single,
		 d->ui_int.radioButton_selmode_multitracks,
		 d->ui_int.radioButton_selmode_trackfits);

  s.save(d->ui_int.checkBox_selsingle_printinfo);
  s.save(d->ui_int.checkBox_selsingle_printinfo_verbose);
  s.save(d->ui_int.checkBox_selsingle_orientzoom);
  s.save(d->ui_int.checkBox_sel_printtotmom);
  s.save(d->ui_int.checkBox_sel_showtotmom);
  s.save(d->ui_int.comboBox_fitterMode); // Version 12+
  s.save(d->ui_int.checkBox_removeOutliers);// Version 12+
  s.save(d->ui_int.comboBox_particleHypo);// Version 12+
  // -----------------------------------




  // --- AscObjs options ---
    s.save(QString("AscObjs options"));
  //AscObjs - TSOS:
  s.save(d->ui_ascobjs.checkBox_materialeffectsontrack_forceposontrack);
  s.save(d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE);
  s.save(d->ui_ascobjs.checkBox_measurements_shorttubes_mdt);
  s.save(d->ui_ascobjs.checkBox_measurements_shorttubes_trt);
  s.save(d->ui_ascobjs.checkBox_measurements_drawGP); // Version 11
  s.save(d->ui_ascobjs.checkBox_parerror_drawcylinder);
  s.save(d->ui_ascobjs.checkBox_parerror_hideperigeeerrors);
  s.save(d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces);
  s.save(d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces); // Version 11
  s.save(d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack);
  s.save(d->ui_ascobjs.checkBox_usecolour_meas_outliers);
  s.save(d->ui_ascobjs.checkBox_usecolour_measurements);
  s.save(d->ui_ascobjs.checkBox_usecolour_parametererrors);
  s.save(d->ui_ascobjs.checkBox_usecolour_parameters);
  s.save(d->ui_ascobjs.checkBox_useHoleColour_parameters);// Version 14
  s.save(d->ui_ascobjs.checkBox_usecolour_surfaces);
  
  s.save(d->ui_ascobjs.doubleSpinBox_parerror_stddev);
  s.save(d->ui_ascobjs.groupBox_materialeffectsontrack);
  s.save(d->ui_ascobjs.groupBox_measurements);
  s.save(d->ui_ascobjs.groupBox_errors);// Version 13
  s.save(d->ui_ascobjs.checkBox_parametererrors);// Version 13
  s.save(d->ui_ascobjs.checkBox_measurementerrors);// Version 13
  s.save(d->ui_ascobjs.groupBox_parameters);
  s.save(d->ui_ascobjs.groupBox_surfaces);
  s.save(d->ui_ascobjs.horizontalSlider_complexity);
  s.save(d->ui_ascobjs.horizontalSlider_linewidths);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.save(d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale);
  s.save(d->ui_ascobjs.horizontalSlider_pointsizes);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.save(d->ui_ascobjs.matButton_materialeffectsontrack);
  s.save(d->ui_ascobjs.matButton_meas_outliers);
  s.save(d->ui_ascobjs.matButton_measurements);
  s.save(d->ui_ascobjs.matButton_parameters);
  s.save(d->ui_ascobjs.matButton_holeParameters);// Version 14
  s.save(d->ui_ascobjs.matButton_parerrors);
  s.save(d->ui_ascobjs.matButton_surfaces);
  s.save(d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale);//Version 5+
  // -----------------------------------




  // ----- Colouring options -----
  s.save(QString("Colouring options"));
  // Version 12
  s.save(d->ui_col.groupBox_labels);
  s.save(d->ui_col.horizontalSlider_labels_trkOffset);
  s.save(d->ui_col.horizontalSlider_labels_xOffset);
  s.save(d->ui_col.horizontalSlider_labels_yOffset);
  s.save(d->ui_col.horizontalSlider_labels_zOffset);
  s.save(d->ui_col.checkBox_trkLabels_p);
  s.save(d->ui_col.checkBox_trkLabels_Pt);
  s.save(d->ui_col.checkBox_trkLabels_pid);
  s.save(d->ui_col.checkBox_trkLabels_hits);
  s.save(d->ui_col.checkBox_trkLabels_fitQuality);
  s.save(d->ui_col.checkBox_trkLabels_direction);
  // --------------------------






  // ----- Extrap options -----
  s.save(QString("Extrapolator options"));
  s.save(d->ui_extrap.checkBox_ignoreMEoT); // Version 14
  s.save(d->ui_extrap.checkBox_extendAllInDetTracks);
  s.save(d->ui_extrap.comboBox_extendAllInDetTracksToHere);
  s.save(d->ui_extrap.horizontalSlider_granularity);
  s.save(d->ui_extrap.checkBox_maxRadius); // Version 15
  s.save(d->ui_extrap.spinBox_maxRadiusValue);
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
  //   VP1QtInventorUtils::setValueLineWidthSlider(d->ui_col.horizontalSlider_trackWidth,s.restoreDouble());
  //   s.widgetHandled(d->ui_col.horizontalSlider_trackWidth);
  // }
  if (s.version()<17)
     s.ignoreInt(); //d->ui_col.horizontalSlider_trackWidth

  //Tracks base light model:
  // if (s.version()>=4)
  //   s.restore(d->ui_col.checkBox_tracksUseBaseLightModel);
  if (s.version()<17)
    s.ignoreBool(); //d->ui_col.checkBox_tracksUseBaseLightModel
  




  // --- Projections options ---
  if (s.version()>=17 &&  s.restoreString() != "Projections options") messageDebug("\n\nERROR! --> 'Projections options'");
  s.restore(d->ui_proj.checkBox_projections_indet);
  s.restore(d->ui_proj.checkBox_projections_muonchambers);
  if (s.version()>=15){
    s.restore(d->ui_proj.groupBox_projections_vertex); 
    s.restore(d->ui_proj.spinBox_projections_vertex);
    s.restore(d->ui_proj.horizontalSlider_projections_vertex);
  }
  // s.restore(d->ui_col.checkBox_hideactualpaths);
  if (s.version()<17)
    s.ignoreBool(); //d->ui_col.checkBox_hideactualpaths  
  // --------------------------------------




  //Display options - track tubes
  // if (s.version()>=8) {
  //   s.restore(d->ui_col.checkBox_trackTubes);
  //   s.restore(d->ui_col.doubleSpinBox_trackTubesRadiusMM);
  // }
  if (s.version()<17){
    s.ignoreBool(); //d->ui_col.checkBox_trackTubes  
    s.ignoreDouble(); //d->ui_col.doubleSpinBox_trackTubesRadiusMM  
  }

  if (s.version()<=3)
    s.ignoreBool();




  // ----- Propagation options -----
  if (s.version()>=17 && s.restoreString() != "Propagation options") messageDebug("\n\nERROR! --> 'Propagation options'");
  if (s.version()>=4)
    s.restore(d->ui_extrap.radioButton_none,
	      d->ui_extrap.radioButton_helical,
	      d->ui_extrap.radioButton_athenaExtrapolator);
  d->restoredLastPropagator = s.restoreString();
  s.widgetHandled(d->ui_extrap.comboBox_propagator);
  // -----------------------------------




  // ----- Interactions options -----
  if (s.version()>=17 && s.restoreString() != "Interactions options") messageDebug("\n\nERROR! --> 'Interactions options'");
  if (s.version()>=6) {
    s.restore(d->ui_int.lineEdit_fittedTrackCollName);
    d->restoredLastFitter = s.restoreString();
    s.widgetHandled(d->ui_int.comboBox_fitters);
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
  s.restore(d->ui_ascobjs.comboBox_assocobj_detaillevel);
  if (s.version()<=2) {
    s.ignoreBool();
    s.ignoreBool();
  }

  //Display options - Truth tracks:
  s.restore(d->ui_ascobjs.checkBox_truthtracks_display_points);


  // --- Display options - Colours ---
  if (s.version()>=17 && s.restoreString() != "Colour options") messageDebug("\n\nERROR! --> 'Colour options'");
  //Display options - Colour by pdg:
  s.restore(d->ui_col.matButton_electrons);
  s.restore(d->ui_col.matButton_muons);
  s.restore(d->ui_col.matButton_pions);
  s.restore(d->ui_col.matButton_protons);
  s.restore(d->ui_col.matButton_chargedkaons);
  s.restore(d->ui_col.matButton_othercharged);
  s.restore(d->ui_col.matButton_neutrons);
  s.restore(d->ui_col.matButton_photons);
  s.restore(d->ui_col.matButton_neutrinos);
  s.restore(d->ui_col.matButton_otherneutrals);
  if (s.version()>=2) {
  //Display options - Colour by charge:
    s.restore(d->ui_col.matButton_charge_neg);
    s.restore(d->ui_col.matButton_charge_pos);
    s.restore(d->ui_col.matButton_charge_neutral);

    //Display options - Colour by momentum:
    s.restore(d->ui_col.matButton_0GeV);
    s.restore(d->ui_col.matButton_15GeV);
  }
  // -----------------------------------





  // --- Cuts options ---
  if (s.version()>=17 && s.restoreString() != "Cuts options") messageDebug("\n\nERROR! --> 'Cuts options'");
  //Cuts - general:
  s.restore(d->ui_cuts.checkBox_cut_minpt);
  s.restore(d->ui_cuts.doubleSpinBox_cut_minpt_gev);
  s.restore(d->ui_cuts.checkBox_cut_maxpt);
  s.restore(d->ui_cuts.doubleSpinBox_cut_maxpt_gev);

  if (s.version()>=10) {
    s.restore(d->ui_cuts.comboBox_momtype);
  }

  if (s.version()>=7) {
    s.restore(d->ui_cuts.etaPhiCutWidget);
  } else {
    s.ignoreBool();
    s.ignoreBool();
    s.ignoreDouble();
    s.ignoreDouble();
    s.ignoreBool();
    s.ignoreObsoletePhiSectionWidgetState();
  }

  if (s.version()>=9) {
    s.restore(d->ui_cuts.checkBox_cut_nhits_pixel);
    s.restore(d->ui_cuts.checkBox_cut_nhits_sct);
    s.restore(d->ui_cuts.checkBox_cut_nhits_trt);
    s.restore(d->ui_cuts.checkBox_cut_nhits_muon);
    s.restore(d->ui_cuts.spinBox_cut_nhits_pixel);
    s.restore(d->ui_cuts.spinBox_cut_nhits_sct);
    s.restore(d->ui_cuts.spinBox_cut_nhits_trt);
    s.restore(d->ui_cuts.spinBox_cut_nhits_muon);
  }

  //Cuts - truth:
  s.restore(d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR);
  s.restore(d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals);
  s.restore(d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0);

  if (s.version()>=14) s.restore(d->ui_cuts.checkBox_vertexAssociated);
  // -----------------------------------





  // --- Interactions options ---
  if (s.version()>=17 && s.restoreString() != "Interactions options") messageDebug("\n\nERROR! --> 'Interactions options'");
  //Interactions - selection mode:
  s.restore(d->ui_int.radioButton_selmode_single,
		d->ui_int.radioButton_selmode_multitracks,
		d->ui_int.radioButton_selmode_trackfits);

  s.restore(d->ui_int.checkBox_selsingle_printinfo);
  s.restore(d->ui_int.checkBox_selsingle_printinfo_verbose);
  s.restore(d->ui_int.checkBox_selsingle_orientzoom);
  s.restore(d->ui_int.checkBox_sel_printtotmom);
  s.restore(d->ui_int.checkBox_sel_showtotmom);
  if (s.version()>=12){
    s.restore(d->ui_int.comboBox_fitterMode); 
    s.restore(d->ui_int.checkBox_removeOutliers);
    s.restore(d->ui_int.comboBox_particleHypo);
  }
  // -----------------------------------






  
  // --- AscObjs options ---
  if (s.version()>=17 && s.restoreString() != "AscObjs options") messageDebug("\n\nERROR! --> 'AscObjs options'");
  //AscObjs - TSOS:
  s.restore(d->ui_ascobjs.checkBox_materialeffectsontrack_forceposontrack);
  s.restore(d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE);
  s.restore(d->ui_ascobjs.checkBox_measurements_shorttubes_mdt);
  s.restore(d->ui_ascobjs.checkBox_measurements_shorttubes_trt);
  if (s.version()>=11) s.restore(d->ui_ascobjs.checkBox_measurements_drawGP);
  s.restore(d->ui_ascobjs.checkBox_parerror_drawcylinder);
  s.restore(d->ui_ascobjs.checkBox_parerror_hideperigeeerrors);
  s.restore(d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces);
  if (s.version()>=11) s.restore(d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces);
  s.restore(d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack);
  s.restore(d->ui_ascobjs.checkBox_usecolour_meas_outliers);
  s.restore(d->ui_ascobjs.checkBox_usecolour_measurements);
  if (s.version()<12) s.restore(d->ui_ascobjs.checkBox_parametererrors); // was groupBox_parametererrors before...
  s.restore(d->ui_ascobjs.checkBox_usecolour_parametererrors);
  s.restore(d->ui_ascobjs.checkBox_usecolour_parameters);
  if (s.version()>=14)
    s.restore(d->ui_ascobjs.checkBox_useHoleColour_parameters);  
  s.restore(d->ui_ascobjs.checkBox_usecolour_surfaces);
  s.restore(d->ui_ascobjs.doubleSpinBox_parerror_stddev);
  s.restore(d->ui_ascobjs.groupBox_materialeffectsontrack);
  s.restore(d->ui_ascobjs.groupBox_measurements);
  if (s.version()>=12) {
    s.restore(d->ui_ascobjs.groupBox_errors);
    s.restore(d->ui_ascobjs.checkBox_parametererrors);
    s.restore(d->ui_ascobjs.checkBox_measurementerrors);
  }
  s.restore(d->ui_ascobjs.groupBox_parameters);
  s.restore(d->ui_ascobjs.groupBox_surfaces);
  s.restore(d->ui_ascobjs.horizontalSlider_complexity);
  s.restore(d->ui_ascobjs.horizontalSlider_linewidths);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.restore(d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale);
  s.restore(d->ui_ascobjs.horizontalSlider_pointsizes);//FIXME!!! SAVE AS ABOVE INSTEAD!!
  s.restore(d->ui_ascobjs.matButton_materialeffectsontrack);
  s.restore(d->ui_ascobjs.matButton_meas_outliers);
  s.restore(d->ui_ascobjs.matButton_measurements);
  s.restore(d->ui_ascobjs.matButton_parameters);
  if (s.version()>=14)
   s.restore(d->ui_ascobjs.matButton_holeParameters);
  s.restore(d->ui_ascobjs.matButton_parerrors);
  s.restore(d->ui_ascobjs.matButton_surfaces);
  if (s.version()>=5)
    s.restore(d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale);
  // -----------------------------------






  //Interactions - track fits:
  


  // ----- Colouring options -----
  if (s.version()>=17 && s.restoreString() != "Colouring options") messageDebug("\n\nERROR! --> 'Colouring options'");
  if (s.version()>=12){
    s.restore(d->ui_col.groupBox_labels);
    s.restore(d->ui_col.horizontalSlider_labels_trkOffset);
    s.restore(d->ui_col.horizontalSlider_labels_xOffset);
    s.restore(d->ui_col.horizontalSlider_labels_yOffset);
    s.restore(d->ui_col.horizontalSlider_labels_zOffset);
    s.restore(d->ui_col.checkBox_trkLabels_p);
    s.restore(d->ui_col.checkBox_trkLabels_Pt);
    s.restore(d->ui_col.checkBox_trkLabels_pid);
    s.restore(d->ui_col.checkBox_trkLabels_hits);
    s.restore(d->ui_col.checkBox_trkLabels_fitQuality);
  }
  if (s.version()>=16)
    s.restore(d->ui_col.checkBox_trkLabels_direction);
  
  



  // ----- Extrap options -----
  // Version 14
  if (s.version()>=17 && s.restoreString() != "Extrapolator options") messageDebug("\n\nERROR! --> 'Extrapolator options'");
  if (s.version()>=14){
    s.restore(d->ui_extrap.checkBox_ignoreMEoT);
    s.restore(d->ui_extrap.checkBox_extendAllInDetTracks);
    s.restore(d->ui_extrap.comboBox_extendAllInDetTracksToHere); 
    s.restore(d->ui_extrap.horizontalSlider_granularity);
  }
  // version 15
  if (s.version()>=15){
    s.restore(d->ui_extrap.checkBox_maxRadius);
    s.restore(d->ui_extrap.spinBox_maxRadiusValue);  
  }
  // ------------------------------



  messageDebug("TrackSystemController::actualRestoreSettings() - DONE.");

}

//____________________________________________________________________
TrackCollWidget * TrackSystemController::collWidget() const
{
  return d->trackcollwidget;
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
  case 211:  matbutton = d->ui_col.matButton_pions; break;
  case 11:   matbutton = d->ui_col.matButton_electrons; break;
  case 22:   matbutton = d->ui_col.matButton_photons; break;
  case 13:   matbutton = d->ui_col.matButton_muons; break;
  case 2212: matbutton = d->ui_col.matButton_protons; break;
  case 2112: matbutton = d->ui_col.matButton_neutrons; break;
  case 2: return d->materialFallback;
    //Only use CHARGED kaons here!!
    //   case 130://k-long
    //   case 310://k-short
    //   case 311://k0
  case 321://k+
    //   case 313://k*0
  case 323://k*+
    //Fixme: More (charged) kaon states???
    matbutton = d->ui_col.matButton_chargedkaons; break;
  case 12://nu_e
  case 14://nu_mu
  case 16://nu_tau
  case 18://fourth gen. neutrino:
    matbutton = d->ui_col.matButton_neutrinos; break;
  default:
    bool ok;
    double charge;
    charge = VP1ParticleData:: particleCharge(abspdg,ok);
    if (!ok) {
      message("WARNING: Could not look up charge of pdg code "+str(abspdg)+". Assuming charged.");
      charge = 1.0;
    }
    if (charge==0.0)
      matbutton = d->ui_col.matButton_otherneutrals;
    else
      matbutton = d->ui_col.matButton_othercharged;
    break;
  }
  return d->getMat(matbutton);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::getMaterialForCharge(const double& charge) const
{
  if (charge>0)
    return d->getMat(d->ui_col.matButton_charge_pos);
  else if (charge<0)
    return d->getMat(d->ui_col.matButton_charge_neg);
  return d->getMat(d->ui_col.matButton_charge_neutral);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::getMaterialForMomentum(const double& absmom) const
{
  static const double low=0*CLHEP::GeV;
  static const double high=15*CLHEP::GeV;
  if (absmom<=low)
    return d->getMat(d->ui_col.matButton_0GeV);
  else if (absmom>=high)
    return d->getMat(d->ui_col.matButton_15GeV);

  if (!d->matmixer)
    d->matmixer = new VP1SoMaterialMixer(systemBase());

  const double x = (absmom-low)/(high-low);
  return d->matmixer->getMixedMaterial( d->getMat(d->ui_col.matButton_0GeV), 1-x,
					d->getMat(d->ui_col.matButton_15GeV), x );
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatMeasurements() const
{
  return d->getMat(d->ui_ascobjs.matButton_measurements);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatMeasurementsOutliers() const
{
  return d->getMat(d->ui_ascobjs.matButton_meas_outliers);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatParameters() const
{
  return d->getMat(d->ui_ascobjs.matButton_parameters);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatHoleParameters() const
{
  return d->getMat(d->ui_ascobjs.matButton_holeParameters);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatParameterErrors() const
{
  return d->getMat(d->ui_ascobjs.matButton_parerrors);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatMaterialEffects() const
{
  return d->getMat(d->ui_ascobjs.matButton_materialeffectsontrack);
}

//____________________________________________________________________
SoMaterial * TrackSystemController::customMatSurfaces() const
{
  return d->getMat(d->ui_ascobjs.matButton_surfaces);
}

// //____________________________________________________________________
// void TrackSystemController::updateTrackDrawStyle()
// {
//   double val = VP1QtInventorUtils::getValueLineWidthSlider(d->ui_col.horizontalSlider_trackWidth);
//   if (d->trackDrawStyle->lineWidth.getValue()!=val)
//     d->trackDrawStyle->lineWidth = val;
// }

// //____________________________________________________________________
// void TrackSystemController::updateTrackLightModel()
// {
//   bool base = d->ui_col.checkBox_tracksUseBaseLightModel->isChecked();
//   if (d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
//     messageVerbose("TrackLightModel changed (base = "+str(base));
//     if (base)
//       d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
//     else
//       d->trackLightModel->model.setValue(SoLightModel::PHONG);
//   }
// }

//____________________________________________________________________
void TrackSystemController::updateAscObjDrawStyle()
{
  double val_lw = VP1QtInventorUtils::getValueLineWidthSlider(d->ui_ascobjs.horizontalSlider_linewidths);
  double val_ps = VP1QtInventorUtils::getValuePointSizeSlider(d->ui_ascobjs.horizontalSlider_pointsizes);
  if (d->ascObjDrawStyle->lineWidth.getValue()!=val_lw)
    d->ascObjDrawStyle->lineWidth = val_lw;
  if (d->ascObjDrawStyle->pointSize.getValue()!=val_ps)
    d->ascObjDrawStyle->pointSize = val_ps;
}


//____________________________________________________________________
void TrackSystemController::updateAscObjComplexity()
{
  //choose complexity in interval [0.01,1.0]
  const double val = std::min<double>(1.0,std::max<double>(0.0,0.01+0.991*
        (d->ui_ascobjs.horizontalSlider_complexity->value()
	-d->ui_ascobjs.horizontalSlider_complexity->minimum())/
        (d->ui_ascobjs.horizontalSlider_complexity->maximum()*1.0)));
  if (d->ascObjComplexity->value.getValue()!=val)
    d->ascObjComplexity->value.setValue(val);
}

// //____________________________________________________________________
// SoDrawStyle * TrackSystemController::trackDrawStyle() const
// {
//   return d->trackDrawStyle;
// }

// //____________________________________________________________________
// SoLightModel * TrackSystemController::trackLightModel() const
// {
//   return d->trackLightModel;
// }

//____________________________________________________________________
SoDrawStyle * TrackSystemController::ascObjDrawStyle() const
{
  return d->ascObjDrawStyle;
}

//____________________________________________________________________
SoComplexity * TrackSystemController::ascObjComplexity() const
{
  return d->ascObjComplexity;
}

//____________________________________________________________________
QString TrackSystemController::nameOfNewlyFittedCollections() const
{
  QString name = d->ui_int.lineEdit_fittedTrackCollName->text().simplified();
  return name.isEmpty() ? "<noname>" : name;
}

//____________________________________________________________________
TrackCommonFlags::TrackPartsFlags TrackSystemController::shownTrackParts() const
{
  TrackCommonFlags::TrackPartsFlags parts = TrackCommonFlags::NoParts;
  // if (!d->ui_col.checkBox_hideactualpaths->isChecked()) parts |= TrackCommonFlags::ActualPath;
  parts |= TrackCommonFlags::ActualPath; // sensible default.
  if (d->ui_proj.checkBox_projections_indet->isChecked()) parts |= TrackCommonFlags::InDetProjections;
  if (d->ui_proj.groupBox_projections_vertex->isChecked()) parts |= TrackCommonFlags::VertexProjections;
  if (VP1JobConfigInfo::hasMuonGeometry()&&d->ui_proj.checkBox_projections_muonchambers->isChecked()) parts |= TrackCommonFlags::MuonProjections;
  return parts;
}

int TrackSystemController::vertexProjectionAngle() const{
  messageVerbose("angleForVertexPlane"+str(d->ui_proj.spinBox_projections_vertex->value()));
  
  if (!d->ui_proj.groupBox_projections_vertex->isChecked()) return -1;
  return d->ui_proj.spinBox_projections_vertex->value();
}

// //____________________________________________________________________
// double TrackSystemController::trackTubeRadius() const
// {
//   return d->ui_col.checkBox_trackTubes->isChecked() ?
//     d->ui_col.doubleSpinBox_trackTubesRadiusMM->value()*CLHEP::mm : 0.0;
// }

//____________________________________________________________________
TrackCommonFlags::TSOSPartsFlags TrackSystemController::shownTSOSParts() const
{
  TrackCommonFlags::TSOSPartsFlags f(TrackCommonFlags::TSOS_NoObjects);
  if (d->ui_ascobjs.groupBox_parameters->isChecked())
    f |= TrackCommonFlags::TSOS_TrackPars;
  if (d->ui_ascobjs.groupBox_errors->isChecked()&&d->ui_ascobjs.checkBox_parametererrors->isChecked()) {
    f |= TrackCommonFlags::TSOS_TrackParsErrorsNotPerigee;
    if (!d->ui_ascobjs.checkBox_parerror_hideperigeeerrors->isChecked())
      f |= TrackCommonFlags::TSOS_TrackParsErrorsPerigee;
  }
  if (d->ui_ascobjs.groupBox_errors->isChecked()&&d->ui_ascobjs.checkBox_measurementerrors->isChecked()) {
    f |= TrackCommonFlags::TSOS_MeasError;
  }
  if (d->ui_ascobjs.groupBox_measurements->isChecked()) {
    f |= TrackCommonFlags::TSOS_AnyMeasRioOnTrack;
    f |= TrackCommonFlags::TSOS_AnyMeasCompetingRioOnTrack;
  }
  if (d->ui_ascobjs.groupBox_surfaces->isChecked()) {
    f |= TrackCommonFlags::TSOS_SurfacesDetElem;
    if (!d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces->isChecked())
      f |= TrackCommonFlags::TSOS_SurfacesCustom;
    if (!d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces->isChecked())
      f |= TrackCommonFlags::TSOS_TubeSurfaces;
  }
  if (d->ui_ascobjs.groupBox_materialeffectsontrack->isChecked()) {
    f |= TrackCommonFlags::TSOS_MaterialEffects;
    if (!d->ui_ascobjs.checkBox_materialeffectsontrack_hideNoDE->isChecked())
      f |= TrackCommonFlags::TSOS_MaterialEffectsWithNoDeltaE;
  }

  return f;
}

//____________________________________________________________________
TrackCommonFlags::TSOSPartsFlags TrackSystemController::customColouredTSOSParts() const
{
  TrackCommonFlags::TSOSPartsFlags f(TrackCommonFlags::TSOS_NoObjects);
  if (d->ui_ascobjs.checkBox_usecolour_measurements->isChecked())
    f |= TrackCommonFlags::TSOS_AnyMeasurementNotOutlier;
  if (d->ui_ascobjs.checkBox_usecolour_meas_outliers->isChecked())
    f |= TrackCommonFlags::TSOS_AnyMeasurementOutlier;
  if (d->ui_ascobjs.checkBox_usecolour_parameters->isChecked())
    f |= TrackCommonFlags::TSOS_TrackPars;
  if (d->ui_ascobjs.checkBox_useHoleColour_parameters->isChecked())
    f |= TrackCommonFlags::TSOS_Hole;
  if (d->ui_ascobjs.checkBox_usecolour_parametererrors->isChecked())
    f |= TrackCommonFlags::TSOS_AnyParsErrors;
  if (d->ui_ascobjs.checkBox_usecolour_materialeffectsontrack->isChecked())
    f |= TrackCommonFlags::TSOS_AnyMaterialEffects;
  if (d->ui_ascobjs.checkBox_usecolour_surfaces->isChecked())
    f |= TrackCommonFlags::TSOS_AnySurface;
  
  return f;
}

//____________________________________________________________________
bool TrackSystemController::useShortTRTMeasurements() const
{
  return d->ui_ascobjs.checkBox_measurements_shorttubes_trt->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::useShortMDTMeasurements() const
{
  return d->ui_ascobjs.checkBox_measurements_shorttubes_mdt->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::hideTubeSurfaces() const
{
  return d->ui_ascobjs.checkBox_surfaces_hidetubesurfaces->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::hideCustomSurfaces() const
{
  return d->ui_ascobjs.checkBox_surfaces_hidecustomsurfaces->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::drawMeasGlobalPositions() const
{
  return d->ui_ascobjs.checkBox_measurements_drawGP->isChecked();
}


//____________________________________________________________________
double TrackSystemController::measurementsShorttubesScale() const
{
  return std::max(0.1*CLHEP::mm,d->ui_ascobjs.doubleSpinBox_measurements_shorttubes_scale->value()*CLHEP::cm);
}

//____________________________________________________________________
double TrackSystemController::nStdDevForParamErrors() const
{
  return std::min<double>(1.0e3,std::max<double>(1.0e-3,d->ui_ascobjs.doubleSpinBox_parerror_stddev->value()));
}

//____________________________________________________________________
bool TrackSystemController::parTubeErrorsDrawCylinders() const
{
  return d->ui_ascobjs.checkBox_parerror_drawcylinder->isChecked();
}

//____________________________________________________________________
int TrackSystemController::numberOfPointsOnCircles() const
{
  const int val(d->ui_ascobjs.horizontalSlider_complexity->value());
  const int max(d->ui_ascobjs.horizontalSlider_complexity->maximum());
  const int min(d->ui_ascobjs.horizontalSlider_complexity->minimum());
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
  return d->ui_ascobjs.horizontalSlider_materialeffectsontrack_scale->value()/5.0;
}

//____________________________________________________________________
Trk::IExtrapolator * TrackSystemController::propagator() const
{
  if (!d->toolaccesshelper
      ||!d->ui_extrap.radioButton_athenaExtrapolator->isChecked()
      ||d->ui_extrap.comboBox_propagator->count()==0)
    return 0;

  QString key = d->ui_extrap.comboBox_propagator->currentText();
  if (key==Imp::noneAvailString)
    return 0;
  return key.isEmpty() ? 0 : d->toolaccesshelper->getToolPointer<Trk::IExtrapolator>(key);
}

TrackSystemController::PropagationOptionFlags TrackSystemController::propagationOptions() const{
  PropagationOptionFlags options=TrackSystemController::NoPropOptions;
  if (d->ui_extrap.checkBox_ignoreMEoT->isChecked())    options |= TrackSystemController::IgnoreMEOT;
  if (d->ui_extrap.checkBox_extendAllInDetTracks->isChecked())    options |= TrackSystemController::ExtendTrack;
  return options;
}

float TrackSystemController::propMaxRadius() const{
  if (!d->ui_extrap.checkBox_maxRadius->isChecked())   return -1.0;
  return d->ui_extrap.spinBox_maxRadiusValue->value(); 
}

void TrackSystemController::emitExtrapolateToHereChanged(int /**index*/){
  emit extrapolateToThisVolumeChanged();
}

bool TrackSystemController::ignoreMeasurementEffectsOnTrackInProp() 
{
  return d->ui_extrap.checkBox_ignoreMEoT->isChecked();
}

//____________________________________________________________________
Trk::ITrackFitter * TrackSystemController::trackFitter() const
{
  if (!d->toolaccesshelper
      ||d->ui_int.comboBox_fitters->count()==0)
  return 0;

  QString key = d->ui_int.comboBox_fitters->currentText();
  if (key==Imp::noneAvailString)
    return 0;
  return key.isEmpty() ? 0 : d->toolaccesshelper->getToolPointer<Trk::ITrackFitter>(key);
}

Muon::MuonEDMPrinterTool * TrackSystemController::muonEDMPrinterTool() const
{
  QString key = "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool";
  return d->toolaccesshelper->getToolPointer<Muon::MuonEDMPrinterTool>(key);
}

// ITrackingVolumesSvc * TrackSystemController::trackingVolumeSvc() const
// {
//   return m_trackingVolumesSvc.operator->();
// }

const Trk::Volume * TrackSystemController::extrapolateToThisVolume() const
{
  if (d->ui_extrap.comboBox_extendAllInDetTracksToHere->currentText()=="Calorimeter")
    return d->calorimeterEntryLayer;
  if (d->ui_extrap.comboBox_extendAllInDetTracksToHere->currentText()=="Muon Entrance")
    return d->muonSpectrometerEntryLayer;
  if (d->ui_extrap.comboBox_extendAllInDetTracksToHere->currentText()=="Muon Exit")
    return d->muonSpectrometerExitLayer;
  return 0;
}

//____________________________________________________________________
TrackCommonFlags::SELECTIONMODE TrackSystemController::selectionMode() const
{
  if (d->ui_int.radioButton_selmode_single->isChecked())
    return TrackCommonFlags::SINGLEOBJECT;
  else if (d->ui_int.radioButton_selmode_multitracks->isChecked())
    return TrackCommonFlags::MULTITRACK;
  else if (d->ui_int.radioButton_selmode_trackfits->isChecked())
    return TrackCommonFlags::TRACKFIT;
  message("selectionMode ERROR: Inconsistency detected.");
  return TrackCommonFlags::SINGLEOBJECT;
}

TrackCommonFlags::FITTERMODE TrackSystemController::fitterMode() const
{  
  if (d->ui_int.comboBox_fitterMode->currentText()=="Fit PRDs")
    return TrackCommonFlags::FROMPRDS;
  else if (d->ui_int.comboBox_fitterMode->currentText()=="Refit Track")
    return TrackCommonFlags::REFITSINGLETRACK;
  else if (d->ui_int.comboBox_fitterMode->currentText()=="Extend Track with PRDs")
    return TrackCommonFlags::EXTENDTRACKWITHPRDS;
  else if (d->ui_int.comboBox_fitterMode->currentText()=="Combine Two Tracks")
    return TrackCommonFlags::COMBINETWOTRACKS;
  
  message("fitterMode ERROR: Inconsistency detected. Mode not known.");
  
  return TrackCommonFlags::FROMPRDS; // FIXME!
}

bool TrackSystemController::fitterRemoveOutliers() const
{
  return d->ui_int.checkBox_removeOutliers->isChecked();
}

Trk::ParticleHypothesis TrackSystemController::fitterParticleHypthesis() const
{
  if (d->ui_int.comboBox_particleHypo->currentText()=="Pion")
     return Trk::pion;
   else if (d->ui_int.comboBox_particleHypo->currentText()=="Non Interacting")
     return Trk::nonInteracting;
   else if (d->ui_int.comboBox_particleHypo->currentText()=="Electron")
     return Trk::electron;
   else if (d->ui_int.comboBox_particleHypo->currentText()=="Muon")
     return Trk::muon;
   else if (d->ui_int.comboBox_particleHypo->currentText()=="Kaon")
     return Trk::kaon;
   else if (d->ui_int.comboBox_particleHypo->currentText()=="Proton")
     return Trk::proton;
   else if (d->ui_int.comboBox_particleHypo->currentText()=="Photon")
     return Trk::photon;
     
   message("fitterMode ERROR: Inconsistency detected. Mode not known.");
   return Trk::undefined; // FIXME!
}

//____________________________________________________________________
bool TrackSystemController::showTruthAscObjs() const
{
  return d->ui_ascobjs.checkBox_truthtracks_display_points->isChecked();
}

//____________________________________________________________________
TrackCommonFlags::DETAILLEVEL TrackSystemController::assocObjDetailLevel() const
{
  if (d->ui_ascobjs.comboBox_assocobj_detaillevel->currentText()=="Auto")
    return TrackCommonFlags::AUTO;
  else if (d->ui_ascobjs.comboBox_assocobj_detaillevel->currentText()=="Simple")
    return TrackCommonFlags::SIMPLE;
  else
    return TrackCommonFlags::DETAILED;
}

void TrackSystemController::vertexCutsAllowed(bool b){
  if (d->ui_cuts.checkBox_vertexAssociated->isEnabled()!=b) {
    d->ui_cuts.checkBox_vertexAssociated->setEnabled(b);
    emit cutOnlyVertexAssocTracksChanged(d->ui_cuts.checkBox_vertexAssociated->isChecked());
  }
}

bool TrackSystemController::cutOnlyVertexAssocTracks() const {
  if (!d->ui_cuts.checkBox_vertexAssociated->isEnabled()) return false;
  return d->ui_cuts.checkBox_vertexAssociated->isChecked();
}


//____________________________________________________________________
VP1Interval TrackSystemController::cutAllowedPt() const
{
  if (!d->ui_cuts.checkBox_cut_minpt)
    return VP1Interval();

  // will set range to negative if we have momcut=P
  // if minCut unset then min=-inf
  // if minCut set, and Pt selected, then min=-minCut
  // if minCut set, and P selected, then min=-maxCut
  // etc
  bool isPCut = d->ui_cuts.comboBox_momtype->currentText()=="P";
  
  const double minFromInterface=d->ui_cuts.doubleSpinBox_cut_minpt_gev->value()*CLHEP::GeV;
  const double maxFromInterface=d->ui_cuts.doubleSpinBox_cut_maxpt_gev->value()*CLHEP::GeV;
  
  double min=0.0,max=0.0;
  if (!isPCut) {
    //Pt cut
    min = (d->ui_cuts.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
    max = (d->ui_cuts.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
  } else {
    min = (d->ui_cuts.checkBox_cut_maxpt->isChecked() ? -maxFromInterface : -std::numeric_limits<double>::infinity());
    max = (d->ui_cuts.checkBox_cut_minpt->isChecked() ? -minFromInterface : std::numeric_limits<double>::infinity());
  }
  
  //message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));
  
  if (max<min)
    return VP1Interval();
    
  return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval TrackSystemController::cutAllowedEta() const
{
  return d->ui_cuts.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> TrackSystemController::cutAllowedPhi() const
{
  return d->ui_cuts.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
QList<unsigned> TrackSystemController::cutRequiredNHits() const
{
  unsigned npixel = d->ui_cuts.checkBox_cut_nhits_pixel->isChecked() ? d->ui_cuts.spinBox_cut_nhits_pixel->value() : 0;
  unsigned nsct = d->ui_cuts.checkBox_cut_nhits_sct->isChecked() ? d->ui_cuts.spinBox_cut_nhits_sct->value() : 0;
  unsigned ntrt = d->ui_cuts.checkBox_cut_nhits_trt->isChecked() ? d->ui_cuts.spinBox_cut_nhits_trt->value() : 0;
  unsigned nmuon = d->ui_cuts.checkBox_cut_nhits_muon->isChecked() ? d->ui_cuts.spinBox_cut_nhits_muon->value() : 0;
  QList<unsigned> l;
  if (!npixel&&!nsct&&!ntrt&&!nmuon)
    return l;
  l << npixel << nsct << ntrt << nmuon;
  return l;
}

//____________________________________________________________________
bool TrackSystemController::cutTruthFromIROnly() const
{
  return d->ui_cuts.checkBox_cut_truthtracks_creationvertexinIR->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::cutExcludeBarcodeZero() const
{
  return d->ui_cuts.checkBox_cut_truthtracks_excludebarcode0->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::cutTruthExcludeNeutrals() const
{
  return d->ui_cuts.checkBox_cut_truthtracks_excludeneutrals->isChecked();
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
    //d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(true);
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
  if (d->lastUpdatedAvailableFitters==af)
    return;
  d->lastUpdatedAvailableFitters=af;
  d->updateComboBoxContents(d->ui_int.comboBox_fitters,af,d->restoredLastFitter);//remember return val.
  d->updateFitPRDButtonState();

  possibleChange_trackFitter();
}

//____________________________________________________________________
void TrackSystemController::availableExtrapolatorsChanged(const QStringList& ae)
{
  if (d->lastUpdatedAvailableExtrapolators==ae)
    return;
  d->lastUpdatedAvailableExtrapolators=ae;

  if (!d->updateComboBoxContents(d->ui_extrap.comboBox_propagator,ae,d->restoredLastPropagator)) {
    d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(false);
    bool save1 = d->ui_extrap.radioButton_none->blockSignals(true);
    bool save2 = d->ui_extrap.radioButton_helical->blockSignals(true);
    bool save3 = d->ui_extrap.radioButton_athenaExtrapolator->blockSignals(true);
    d->ui_extrap.radioButton_none->setChecked(true);//Fixme: fall back to the helical instead!
    d->ui_extrap.radioButton_helical->setChecked(false);
    d->ui_extrap.radioButton_athenaExtrapolator->setChecked(false);
    if (!save1) d->ui_extrap.radioButton_none->blockSignals(false);
    if (!save2) d->ui_extrap.radioButton_helical->blockSignals(false);
    if (!save3) d->ui_extrap.radioButton_athenaExtrapolator->blockSignals(false);
  } else {
    d->ui_extrap.radioButton_athenaExtrapolator->setEnabled(true);
  }

  possibleChange_propagator();
}

//____________________________________________________________________
bool TrackSystemController::orientAndZoomOnSingleSelection() const
{
  return d->ui_int.checkBox_selsingle_orientzoom->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::printInfoOnSingleSelection() const
{
  return d->ui_int.checkBox_selsingle_printinfo->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::printVerboseInfoOnSingleSelection() const
{
  return printInfoOnSingleSelection() && d->ui_int.checkBox_selsingle_printinfo_verbose->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::printTotMomentumOnMultiTrackSelection() const
{
  return d->ui_int.checkBox_sel_printtotmom->isChecked();
}

//____________________________________________________________________
bool TrackSystemController::showTotMomentumOnMultiTrackSelection() const
{
  return d->ui_int.checkBox_sel_showtotmom->isChecked();
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
  if (d->numberOfSelectedPRDs==prds && d->numberOfSelectedTracks==trks)
    return;
  d->numberOfSelectedPRDs=prds;
  d->numberOfSelectedTracks=trks;
  
  switch (fitterMode()) {
    case TrackCommonFlags::FROMPRDS:            d->ui_int.pushButton_refit->setText("Fit track from "+str(prds)+" PRDs");break;
    case TrackCommonFlags::REFITSINGLETRACK:    d->ui_int.pushButton_refit->setText("Fit track");break;
    case TrackCommonFlags::EXTENDTRACKWITHPRDS: d->ui_int.pushButton_refit->setText("Extend track with "+str(prds)+" PRDs");break;
    case TrackCommonFlags::COMBINETWOTRACKS:    d->ui_int.pushButton_refit->setText("Combine");break;
  }
  
  
  // FIXME Need to redo this method so that it can handle all fitter modes
  
  d->updateFitPRDButtonState();
}

void TrackSystemController::updateFitPRDButtonState(){
  messageVerbose("updateFitPRDButtonState");
  
  switch (fitterMode()) {
    case TrackCommonFlags::FROMPRDS:            d->ui_int.pushButton_refit->setText("Fit track from "+str(d->numberOfSelectedPRDs)+" PRDs");break;
    case TrackCommonFlags::REFITSINGLETRACK:    d->ui_int.pushButton_refit->setText("Fit track");break;
    case TrackCommonFlags::EXTENDTRACKWITHPRDS: d->ui_int.pushButton_refit->setText("Extend track with "+str(d->numberOfSelectedPRDs)+" PRDs");break;
    case TrackCommonFlags::COMBINETWOTRACKS:    d->ui_int.pushButton_refit->setText("Combine");break;
  }
  
  d->updateFitPRDButtonState();
}

QTreeWidget* TrackSystemController::trackObjBrowser() const
{
    return d->objBrowserWidget;
}

TrackSysCommonData * TrackSystemController::common() const {
  return d->common;
}
void TrackSystemController::setCommonData(TrackSysCommonData * common){
  d->common=common;
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
  values[0]=d->ui_shiftmuonchambers.doubleSpinBox_tra_s->value();
  values[1]=d->ui_shiftmuonchambers.doubleSpinBox_tra_z->value();
  values[2]=d->ui_shiftmuonchambers.doubleSpinBox_tra_t->value();
  values[3]=d->ui_shiftmuonchambers.doubleSpinBox_rot_s->value();
  values[4]=d->ui_shiftmuonchambers.doubleSpinBox_rot_z->value();
  values[5]=d->ui_shiftmuonchambers.doubleSpinBox_rot_t->value();
  return values;
}
int TrackSystemController::alignmentShiftLevel()
{
  if (d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 0")
     return 0;
   else if (d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 1")
     return 1;
   else if (d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 2")
     return 2;
   else if (d->ui_shiftmuonchambers.comboBox_level->currentText()=="Level 3")
     return 3;
   return 0;
}

bool TrackSystemController::doTrackLabels() 
{
  return d->ui_col.groupBox_labels->isChecked();
}

float TrackSystemController::trackLabelTrkOffset() 
{
  return static_cast<float>(d->ui_col.horizontalSlider_labels_trkOffset->value())/static_cast<float>(d->ui_col.horizontalSlider_labels_trkOffset->maximum());
}

QList<int> TrackSystemController::trackLabelPosOffset() 
{
  QList<int> values;
  values << d->ui_col.horizontalSlider_labels_xOffset->value();
  values << d->ui_col.horizontalSlider_labels_yOffset->value();
  values << d->ui_col.horizontalSlider_labels_zOffset->value();
  return values;
}

int TrackSystemController::labelXOffset() 
{
  return d->ui_col.horizontalSlider_labels_xOffset->value();
}

int TrackSystemController::labelYOffset() 
{
  return d->ui_col.horizontalSlider_labels_yOffset->value();
}

int TrackSystemController::labelZOffset() 
{
  return d->ui_col.horizontalSlider_labels_zOffset->value();
}

TrackSystemController::TrackLabelModes TrackSystemController::trackLabels(){
  if (!d->ui_col.groupBox_labels->isChecked())
    return TrackSystemController::NoLabels;
  
  TrackLabelModes labels=TrackSystemController::NoLabels;
  if (d->ui_col.checkBox_trkLabels_p->isChecked())    labels |= TrackSystemController::P;
  if (d->ui_col.checkBox_trkLabels_Pt->isChecked())   labels |= TrackSystemController::Pt;
  if (d->ui_col.checkBox_trkLabels_direction->isChecked())   labels |= TrackSystemController::Direction;
  if (d->ui_col.checkBox_trkLabels_pid->isChecked())  labels |= TrackSystemController::Pid;
  if (d->ui_col.checkBox_trkLabels_hits->isChecked()) labels |= TrackSystemController::Hits;
  if (d->ui_col.checkBox_trkLabels_fitQuality->isChecked()) labels |= TrackSystemController::FitQuality;
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
