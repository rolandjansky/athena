/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class GeoSysController                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1GeometrySystems/GeoSysController.h"
#include "VP1GeometrySystems/ZappedVolumeListModel.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Settings.h"
#include "VP1Base/IVP1System.h"
#include "VP1Utils/VP1JobConfigInfo.h"

#include "ui_geometrysystemcontroller.h"
#include "ui_settings_display_form.h"
#include "ui_settings_iconisedvols_form.h"
#include "ui_settings_interactions_form.h"
#include "ui_settings_misc_form.h"
#include "ui_settings_muonchamber_form.h"
#include "ui_settings_treebrowser_form.h"

#include <Inventor/nodes/SoPickStyle.h>

#include <QFileDialog>

#include <map>


//____________________________________________________________________
class GeoSysController::Imp {
public:
  GeoSysController * theclass;
  Ui::GeometrySystemControllerForm ui;
  Ui::GeoSysSettingsDisplayForm ui_disp;
  Ui::GeoSysSettingsIconisedVolumesForm ui_iconisedvols;
  Ui::GeoSysSettingsInteractionsForm ui_int;
  Ui::GeoSysSettingsMiscForm ui_misc;
  Ui::GeoSysSettingsMuonChamberForm ui_muon;
  Ui::GeoSysSettingsTreeBrowserForm ui_treebrowser;
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*> subSysCheckBoxMap;
  float last_transparency;
  bool last_showVolumeOutLines;
  VP1GeoFlags::MuonChamberAdaptionStyleFlags last_muonChamberAdaptionStyle;
  int last_labels; //!< needed for POSSIBLECHANGE_IMP macro.
  QList<int> last_labelPosOffset; //!< needed for  POSSIBLECHANGE_IMP macro.
  SoPickStyle * pickStyle;
  VolumeHandle * lastSelectedVolHandle;
  ZappedVolumeListModel * zappedVolumeListModel;

  QString lastSaveMaterialsFile;
  QString lastLoadMaterialsFile;
  
//  std::map<QString, QList<QCheckBox*> > labelProvidingSystems; //!< First is name of system, second is list of types of information provided (system stores actual information)
};


//____________________________________________________________________
GeoSysController::GeoSysController(IVP1System * sys)
  : VP1Controller(sys,"GeoSysController"), d(new Imp)
{
  d->theclass = this;
  d->ui.setupUi(this);

  initDialog(d->ui_disp, d->ui.pushButton_settings_display);
  initDialog(d->ui_iconisedvols, d->ui.pushButton_settings_iconisedvols);
  initDialog(d->ui_int, d->ui.pushButton_settings_interactions);
  initDialog(d->ui_misc, d->ui.pushButton_settings_misc);
  initDialog(d->ui_muon, d->ui.pushButton_settings_muonchambers);
  initDialog(d->ui_treebrowser, d->ui.pushButton_settings_treebrowser);

  //  d->ui_disp.widget_drawOptions->setLineWidthsDisabled();
  d->ui_disp.widget_drawOptions->setPointSizesDisabled();
//   d->ui_disp.widget_drawOptions->setBaseLightingDisabled();
  d->ui_disp.widget_drawOptions->setComplexity(0.6);

  d->pickStyle = new SoPickStyle;
  d->pickStyle->ref();

  //Possibly hide parts of gui, depending on job configuration:
  if (!VP1JobConfigInfo::hasGeoModelExperiment()) {
    d->ui.groupBox_innerdetector->setVisible(false);
    d->ui.groupBox_calorimeters->setVisible(false);
    d->ui.groupBox_muonsystems->setVisible(false);
    d->ui.pushButton_settings_muonchambers->setEnabled(false);
    d->ui_misc.groupBox_pixelactivemodules->setVisible(false);
    d->ui_misc.groupBox_sctactivemodules->setVisible(false);
    d->ui.groupBox_misc->setVisible(false);
    d->ui_muon.groupBox_muonchamberconfig->setVisible(false);
    d->ui_int.checkBox_AutomaticMuonChamberEndViews->setVisible(false);
    message("GeoModel not properly initialised.");
  } else {
    d->ui.groupBox_innerdetector->setVisible(VP1JobConfigInfo::hasPixelGeometry()
					      || VP1JobConfigInfo::hasSCTGeometry()
					      || VP1JobConfigInfo::hasTRTGeometry()
					      || VP1JobConfigInfo::hasInDetServiceMaterialGeometry());
    d->ui_misc.groupBox_pixelactivemodules->setVisible(VP1JobConfigInfo::hasPixelGeometry());
    d->ui_misc.groupBox_sctactivemodules->setVisible(VP1JobConfigInfo::hasSCTGeometry());
    d->ui.groupBox_calorimeters->setVisible(VP1JobConfigInfo::hasLArGeometry()
					     ||VP1JobConfigInfo::hasTileGeometry());
    d->ui.groupBox_muonsystems->setVisible(VP1JobConfigInfo::hasMuonGeometry());
    d->ui.pushButton_settings_muonchambers->setEnabled(VP1JobConfigInfo::hasMuonGeometry());
    d->ui_int.checkBox_print_muonstationinfo->setVisible(VP1JobConfigInfo::hasMuonGeometry());
    d->ui_muon.groupBox_muonchamberconfig->setVisible(VP1JobConfigInfo::hasMuonGeometry());
    d->ui_int.checkBox_AutomaticMuonChamberEndViews->setVisible(VP1JobConfigInfo::hasMuonGeometry());
  }


  d->zappedVolumeListModel = new ZappedVolumeListModel(d->ui_iconisedvols.listView_iconisedvolumes);
  d->ui_iconisedvols.listView_iconisedvolumes->setUniformItemSizes(true);
  d->ui_iconisedvols.listView_iconisedvolumes->setSelectionMode(QAbstractItemView::ExtendedSelection);
  d->ui_iconisedvols.listView_iconisedvolumes->setModel(d->zappedVolumeListModel);

  connect(d->ui_iconisedvols.listView_iconisedvolumes,SIGNAL(activated(const QModelIndex&)),
	  d->zappedVolumeListModel,SLOT(activated(const QModelIndex&)));

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  addUpdateSlot(SLOT(updatePickStyle()));
  connectToLastUpdateSlot(d->ui_int.checkBox_geomselectable);

  addUpdateSlot(SLOT(possibleChange_showVolumeOutLines()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showVolumeOutLines);

  addUpdateSlot(SLOT(possibleChange_transparency()));
  connectToLastUpdateSlot(d->ui_disp.spinBox_transp);

  addUpdateSlot(SLOT(possibleChange_muonChamberAdaptionStyle()));
  connectToLastUpdateSlot(d->ui_muon.checkBox_muonadapt_openmdtchambers);
  connectToLastUpdateSlot(d->ui_muon.checkBox_muonadapt_hidetubes);
  connectToLastUpdateSlot(d->ui_muon.checkBox_muonadapt_hiderpcvolumes);
  connectToLastUpdateSlot(d->ui_muon.checkBox_muonadapt_opencscchambers);
  connectToLastUpdateSlot(d->ui_muon.checkBox_muonadapt_opentgcchambers);

  initLastVars();

  //Connections for state-less signals:

  connect(d->ui_disp.pushButton_saveChangedMaterials,SIGNAL(clicked()),this,SLOT(saveMaterialsRequested()));
  connect(d->ui_disp.pushButton_loadMaterials,SIGNAL(clicked()),this,SLOT(loadMaterialsRequested()));

  connect(d->ui_misc.pushButton_nonStandardShapes_Iconify,SIGNAL(clicked()),
	  this,SLOT(emit_actionOnAllNonStandardVolumes()));
  connect(d->ui_misc.pushButton_nonStandardShapes_Expand,SIGNAL(clicked()),
	  this,SLOT(emit_actionOnAllNonStandardVolumes()));

  connect(d->ui_misc.lineEdit_expand_vols_matname,SIGNAL(returnPressed()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));
  connect(d->ui_misc.pushButton_expand_vols_matname,SIGNAL(clicked()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));
  connect(d->ui_misc.lineEdit_expand_vols_volname,SIGNAL(returnPressed()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));
  connect(d->ui_misc.pushButton_expand_vols_volname,SIGNAL(clicked()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));

  connect(d->ui_muon.pushButton_muonadapt_adapttoevtdata,SIGNAL(clicked()),this,SLOT(emit_adaptMuonChambersToEventData()));

  connect(d->ui_misc.toolButton_pixelmod_adapt,SIGNAL(clicked(bool)),this,SLOT(emit_autoAdaptPixelsOrSCT()));
  connect(d->ui_misc.toolButton_sctmod_adapt,SIGNAL(clicked(bool)),this,SLOT(emit_autoAdaptPixelsOrSCT()));

  connect(d->ui_misc.toolButton_pixelmod_reset,SIGNAL(clicked(bool)),this,SLOT(emit_resetSubSystems()));
  connect(d->ui_misc.toolButton_sctmod_reset,SIGNAL(clicked(bool)),this,SLOT(emit_resetSubSystems()));
  connect(d->ui_muon.pushButton_muonadapt_resetToAllChambers,SIGNAL(clicked()),this,SLOT(emit_resetSubSystems()));

  setLastSelectedVolume(0);

  // INNER DETECTOR
  d->subSysCheckBoxMap[VP1GeoFlags::Pixel] = d->ui.checkBox_Pixel;
  d->subSysCheckBoxMap[VP1GeoFlags::SCT] = d->ui.checkBox_SCT;
  d->subSysCheckBoxMap[VP1GeoFlags::TRT] = d->ui.checkBox_TRT;
  d->subSysCheckBoxMap[VP1GeoFlags::InDetServMat] = d->ui.checkBox_InDetServMat;

  // CALO
  d->subSysCheckBoxMap[VP1GeoFlags::LAr] = d->ui.checkBox_LAr;
  d->subSysCheckBoxMap[VP1GeoFlags::Tile] = d->ui.checkBox_Tile;

  // MUON TOROID
  d->subSysCheckBoxMap[VP1GeoFlags::BarrelToroid] = d->ui.checkBox_MuonBarrelToroid;
  d->subSysCheckBoxMap[VP1GeoFlags::ToroidECA] = d->ui.checkBox_MuonECAToroid;
  // d->subSysCheckBoxMap[VP1GeoFlags::ToroidECC] = d->ui.checkBox_MuonECCToroid;
  // MUON MISC
  d->subSysCheckBoxMap[VP1GeoFlags::MuonFeet] = d->ui.checkBox_MuonFeet;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonShielding] = d->ui.checkBox_MuonShielding;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonToroidsEtc] = d->ui.checkBox_MuonEtc; // this is the "Services" checkbox in the GUI
  // MUON CHAMBERS
  d->subSysCheckBoxMap[VP1GeoFlags::MuonEndcapStationCSC] = d->ui.checkBox_MuonEndcapStationCSC;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonEndcapStationTGC] = d->ui.checkBox_MuonEndcapStationTGC;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonEndcapStationMDT] = d->ui.checkBox_MuonEndcapStationMDT;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonNSW] = d->ui.checkBox_NSW;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonBarrelStationInner] = d->ui.checkBox_MuonBarrelStationInner;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonBarrelStationMiddle] = d->ui.checkBox_MuonBarrelStationMiddle;
  d->subSysCheckBoxMap[VP1GeoFlags::MuonBarrelStationOuter] = d->ui.checkBox_MuonBarrelStationOuter;

  // MISCELLANEOUS
  d->subSysCheckBoxMap[VP1GeoFlags::CavernInfra] = d->ui.checkBox_CavernInfra;
  d->subSysCheckBoxMap[VP1GeoFlags::BeamPipe] = d->ui.checkBox_BeamPipe;
  d->subSysCheckBoxMap[VP1GeoFlags::LUCID] = d->ui.checkBox_LUCID;
  d->subSysCheckBoxMap[VP1GeoFlags::ZDC] = d->ui.checkBox_ZDC;
  d->subSysCheckBoxMap[VP1GeoFlags::ALFA] = d->ui.checkBox_ALFA;
  d->subSysCheckBoxMap[VP1GeoFlags::ForwardRegion] = d->ui.checkBox_ForwardRegion;
  // OTHER
  d->subSysCheckBoxMap[VP1GeoFlags::AllUnrecognisedVolumes] = d->ui.checkBox_other;

  // -> labels
  addUpdateSlot(SLOT(possibleChange_labels()));
  connectToLastUpdateSlot(d->ui_disp.groupBox_labels);
  connectToLastUpdateSlot(d->ui_disp.checkBox_labels_names);
  connectToLastUpdateSlot(d->ui_disp.checkBox_labels_mooret0s);
  connectToLastUpdateSlot(d->ui_disp.checkBox_labels_mboyt0s);
  connectToLastUpdateSlot(d->ui_disp.checkBox_labels_hits);
  
  addUpdateSlot(SLOT(possibleChange_labelPosOffset()));
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_labels_xOffset);
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_labels_yOffset);
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_labels_zOffset);
  
  d->last_labels=0;
}

//____________________________________________________________________
GeoSysController::~GeoSysController()
{
  d->pickStyle->unref();
  delete d;
}

//____________________________________________________________________
void GeoSysController::setGeometrySelectable(bool b)
{
  d->ui_int.checkBox_geomselectable->setChecked(b);
  updatePickStyle();
}

//____________________________________________________________________
void GeoSysController::setComplexity(double c)
{
  d->ui_disp.widget_drawOptions->setComplexity(c);
}

//____________________________________________________________________
void GeoSysController::setZoomToVolumeOnClick(bool b)
{
  d->ui_int.checkBox_zoomToVolumes->setChecked(b);
}

//____________________________________________________________________
void GeoSysController::setOrientViewToMuonChambersOnClick(bool b)
{
  d->ui_int.checkBox_AutomaticMuonChamberEndViews->setChecked(b);
}

//____________________________________________________________________
void GeoSysController::setAutoAdaptMuonChambersToEventData(bool b)
{
  d->ui_muon.checkBox_muonadapt_autoadapt->setChecked(b);
}

//____________________________________________________________________
void GeoSysController::setLastSelectedVolume(VolumeHandle*vh)
{
  d->lastSelectedVolHandle = vh;
  d->ui_disp.matButton_lastSel->setEnabled(vh!=0);
  if (vh) {
    QList<SoMaterial*> mats = d->ui_disp.matButton_lastSel->handledMaterials();
    if (mats.isEmpty()||mats.at(0)!=vh->material()) {
      d->ui_disp.matButton_lastSel->clearHandledMaterials();
      d->ui_disp.matButton_lastSel->setMaterial(vh->material());
    }
  } else {
    d->ui_disp.matButton_lastSel->clearHandledMaterials();
  }
}

//____________________________________________________________________
VolumeHandle* GeoSysController::lastSelectedVolume() const
{
  return d->lastSelectedVolHandle;
}

//____________________________________________________________________
void GeoSysController::saveMaterialsRequested()
{
  const bool ctrl_isdown = (Qt::ControlModifier & QApplication::keyboardModifiers());
  const bool onlyChangedMaterials = !ctrl_isdown;

  QString filename
    = QFileDialog::getSaveFileName(this, "Select geometry material file to save",
				   (d->lastSaveMaterialsFile.isEmpty()?
				    VP1Settings::defaultFileSelectDirectory()
				    :d->lastSaveMaterialsFile),
				   "VP1 geometry material files (*.vp1geomat)",
				   0,QFileDialog::DontResolveSymlinks);

  if(filename.isEmpty())
    return;

  if (!filename.endsWith(".vp1geomat"))
    filename += ".vp1geomat";

  d->lastSaveMaterialsFile = filename;

  messageVerbose("Emitting saveMaterialsToFile(\""+filename+"\","+str(onlyChangedMaterials)+")");
  emit saveMaterialsToFile(filename,onlyChangedMaterials);

}

//____________________________________________________________________
void GeoSysController::loadMaterialsRequested()
{
  QString filename = QFileDialog::getOpenFileName(this, "Select geometry material file to load",
						  (d->lastLoadMaterialsFile.isEmpty()?
						   VP1Settings::defaultFileSelectDirectory()
						   :d->lastLoadMaterialsFile),
						  "VP1 geometry material files (*.vp1geomat)",
						  0,QFileDialog::DontResolveSymlinks);
  if(filename.isEmpty())
    return;

  d->lastLoadMaterialsFile = filename;

  messageVerbose("Emitting loadMaterialsFromFile(\""+filename+"\")");
  emit loadMaterialsFromFile(filename);
}

//____________________________________________________________________
QCheckBox * GeoSysController::subSystemCheckBox(VP1GeoFlags::SubSystemFlag f) const
{
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*>::const_iterator it = d->subSysCheckBoxMap.find(f);
  return it==d->subSysCheckBoxMap.end() ? 0 : it->second;
}

//____________________________________________________________________
SoGroup * GeoSysController::drawOptions() const
{
  return d->ui_disp.widget_drawOptions->drawOptionsGroup();
}

//____________________________________________________________________
SoPickStyle * GeoSysController::pickStyle() const
{
  return d->pickStyle;
}

//____________________________________________________________________
VP1GeoTreeView * GeoSysController::volumeTreeBrowser() const
{
  return d->ui_treebrowser.treeView_volumebrowser;
}

//____________________________________________________________________
PhiSectionWidget * GeoSysController::phiSectionWidget() const
{
  return d->ui_disp.phisectionwidget;
}

//____________________________________________________________________
ZappedVolumeListModel * GeoSysController::zappedVolumeListModel() const
{
  return d->zappedVolumeListModel;
}

//____________________________________________________________________
bool GeoSysController::zoomToVolumeOnClick() const
{
  return d->ui_int.checkBox_zoomToVolumes->isChecked();
}

//____________________________________________________________________
bool GeoSysController::orientViewToMuonChambersOnClick() const
{
  return d->ui_int.checkBox_AutomaticMuonChamberEndViews->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Shape() const
{
  return d->ui_int.checkBox_print_shape->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Material() const
{
  return d->ui_int.checkBox_print_material->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_CopyNumber() const
{
  return d->ui_int.checkBox_print_copyno->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Transform() const
{
  return d->ui_int.checkBox_print_transform->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Tree() const
{
  return d->ui_int.checkBox_print_tree->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Mass() const
{
  return d->ui_int.checkBox_print_mass->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_MuonStationInfo() const
{
  return d->ui_int.checkBox_print_muonstationinfo->isChecked();
}

//____________________________________________________________________
bool GeoSysController::autoAdaptMuonChambersToEventData() const
{
  return d->ui_muon.checkBox_muonadapt_autoadapt->isChecked();
}

//_____________________________________________________________________________________
void GeoSysController::updatePickStyle()
{
  d->pickStyle->style = d->ui_int.checkBox_geomselectable->isChecked()
    ? SoPickStyle::SHAPE : SoPickStyle::UNPICKABLE;
}

//____________________________________________________________________
float GeoSysController::transparency() const
{
  int v(d->ui_disp.spinBox_transp->value());
  return (v>=100?1.0:(v<=0?0.0:v/100.0));
}

//____________________________________________________________________
bool GeoSysController::showVolumeOutLines() const
{
  return d->ui_disp.checkBox_showVolumeOutLines->isChecked();
}

//____________________________________________________________________
VP1GeoFlags::MuonChamberAdaptionStyleFlags GeoSysController::muonChamberAdaptionStyle() const
{
  VP1GeoFlags::MuonChamberAdaptionStyleFlags f(0);
  if (d->ui_muon.checkBox_muonadapt_openmdtchambers->isChecked())
    f |= VP1GeoFlags::OpenMDTChambers;
  if (d->ui_muon.checkBox_muonadapt_hidetubes->isChecked())
    f |= VP1GeoFlags::HideMDTTubes;
  if (d->ui_muon.checkBox_muonadapt_hiderpcvolumes->isChecked())
    f |= VP1GeoFlags::HideRPCVolumes;
  if (d->ui_muon.checkBox_muonadapt_opencscchambers->isChecked())
    f |= VP1GeoFlags::OpenCSCChambers;
  if (d->ui_muon.checkBox_muonadapt_opentgcchambers->isChecked())
    f |= VP1GeoFlags::OpenTGCChambers;
  return f;
}

//void GeoSysController::setAvailableLabels(IVP1System* sys, QStringList providedLabels){
//  messageVerbose("setAvailableLabels for "+sys->name() );
//  if (d->labelProvidingSystems.find(sys->name())!=d->labelProvidingSystems.end())
//    return;
//  QList<QCheckBox*> checkboxes;
//  foreach(QString label, providedLabels) {
//    QCheckBox *checkbox = new QCheckBox(label, this);
//    checkboxes.append(checkbox);
//  }
//  d->labelProvidingSystems[sys->name()]= checkboxes;
////  std::map<QString, QList<QCheckBox*> > labelProvidingSystems; //!< First is name of system, second is list of types of information provided (system stores actual information)
//
//}

void GeoSysController::setLabelsEnabled(bool t0s, bool hits){
  // make sure that when labels are enabled, only the systems which we have are enabled too
  if (t0s) {
    connect(d->ui_disp.groupBox_labels,SIGNAL(toggled(bool)),
                   d->ui_disp.checkBox_labels_mooret0s,SLOT(setEnabled(bool)));
    connect(d->ui_disp.groupBox_labels,SIGNAL(toggled(bool)),
            d->ui_disp.checkBox_labels_mboyt0s,SLOT(setEnabled(bool)));
  } else {
    d->ui_disp.checkBox_labels_mooret0s->setEnabled(false);
    d->ui_disp.checkBox_labels_mboyt0s->setEnabled(false);
  }
  if (hits) {
    connect(d->ui_disp.groupBox_labels,SIGNAL(toggled(bool)),
                   d->ui_disp.checkBox_labels_hits,SLOT(setEnabled(bool)));
  } else {
    d->ui_disp.checkBox_labels_hits->setEnabled(false);
  }

  messageVerbose("setLabelsEnabled() t0s="+str(t0s)+", hits="+str(hits));
}


int GeoSysController::labels() const {
  if (!d->ui_disp.groupBox_labels->isChecked()) return 0;
  int labels=0;
  if (d->ui_disp.checkBox_labels_names->isChecked())    labels|=0x1;
  if (d->ui_disp.checkBox_labels_mooret0s->isChecked()) labels|=0x2;
  if (d->ui_disp.checkBox_labels_mboyt0s->isChecked())  labels|=0x4;
  // leaving space for another t0 type, if necessary
  if (d->ui_disp.checkBox_labels_hits->isChecked())     labels|=0x10;
  return labels;
}

QList<int> GeoSysController::labelPosOffset() 
{
  QList<int> values;
  values << d->ui_disp.horizontalSlider_labels_xOffset->value();
  values << d->ui_disp.horizontalSlider_labels_yOffset->value();
  values << d->ui_disp.horizontalSlider_labels_zOffset->value();
  return values;
}

//____________________________________________________________________
void GeoSysController::emit_adaptMuonChambersToEventData()
{
  messageVerbose("Emitting adaptMuonChambersToEventData()");
  emit adaptMuonChambersToEventData();
}

//____________________________________________________________________
void GeoSysController::emit_autoAdaptPixelsOrSCT()
{
  bool pixel(sender()==d->ui_misc.toolButton_pixelmod_adapt);
  bool brl = pixel ? d->ui_misc.checkBox_pixelmod_barrel->isChecked()
    : d->ui_misc.checkBox_sctmod_barrel->isChecked();
  bool ecA = pixel ? d->ui_misc.checkBox_pixelmod_endcapA->isChecked()
    : d->ui_misc.checkBox_sctmod_endcapA->isChecked();
  bool ecC = pixel ? d->ui_misc.checkBox_pixelmod_endcapC->isChecked()
    : d->ui_misc.checkBox_sctmod_endcapC->isChecked();
  bool bcmA = pixel ? d->ui_misc.checkBox_bcmMod_Aside->isChecked() : false;
  bool bcmC = pixel ? d->ui_misc.checkBox_bcmMod_Cside->isChecked() : false;

  messageVerbose ("Emitting autoAdaptPixelsOrSCT("+str(pixel)+", "+str(brl)+","+str(ecA)+","+str(ecC)+","+str(bcmA)+","+str(bcmC)+")");
  emit autoAdaptPixelsOrSCT(pixel,brl,ecA,ecC,bcmA,bcmC);
}

//____________________________________________________________________
void GeoSysController::emit_autoExpandByVolumeOrMaterialName()
{
  bool volname(sender()==d->ui_misc.pushButton_expand_vols_volname
	       ||sender()==d->ui_misc.lineEdit_expand_vols_volname);
  QString name(volname?d->ui_misc.lineEdit_expand_vols_volname->text()
	       :d->ui_misc.lineEdit_expand_vols_matname->text());
  if (name.isEmpty())
    return;
  messageVerbose("emitting autoExpandByVolumeOrMaterialName("+str(!volname)+", "+name+")");
  emit autoExpandByVolumeOrMaterialName(!volname,name);
}

//____________________________________________________________________
void GeoSysController::emit_actionOnAllNonStandardVolumes() {
  bool zap(sender()==d->ui_misc.pushButton_nonStandardShapes_Iconify);
  messageVerbose("emitting actionOnAllNonStandardVolumes("+str(zap)+")");
  emit actionOnAllNonStandardVolumes(zap);
}

//____________________________________________________________________
void GeoSysController::emit_resetSubSystems()
{
  VP1GeoFlags::SubSystemFlags f(0);
  if (sender()==d->ui_misc.toolButton_pixelmod_reset)
    f = VP1GeoFlags::Pixel;
  else if (sender()==d->ui_misc.toolButton_sctmod_reset)
    f = VP1GeoFlags::SCT;
  else if (sender()==d->ui_muon.pushButton_muonadapt_resetToAllChambers)
    f = VP1GeoFlags::AllMuonChambers;
  messageVerbose(" Emitting resetSubSystems("+str(f)+")");
  emit resetSubSystems(f);
}

//____________________________________________________________________
int GeoSysController::currentSettingsVersion() const
{
  return 5;
}

//____________________________________________________________________
void GeoSysController::actualSaveSettings(VP1Serialise&s) const
{
  s.save(d->ui_disp.phisectionwidget);//version <=1 saved in old format
  s.save(d->ui_disp.spinBox_transp);
  s.save(d->ui_muon.checkBox_muonadapt_autoadapt);
  s.save(d->ui_muon.checkBox_muonadapt_openmdtchambers);
  s.save(d->ui_muon.checkBox_muonadapt_hidetubes);
  s.save(d->ui_muon.checkBox_muonadapt_hiderpcvolumes);
  s.save(d->ui_muon.checkBox_muonadapt_opencscchambers);
  s.save(d->ui_muon.checkBox_muonadapt_opentgcchambers);
  s.save(d->ui_misc.checkBox_pixelmod_barrel);
  s.save(d->ui_misc.checkBox_pixelmod_endcapA);
  s.save(d->ui_misc.checkBox_pixelmod_endcapC);
  s.save(d->ui_misc.checkBox_sctmod_barrel);
  s.save(d->ui_misc.checkBox_sctmod_endcapA);
  s.save(d->ui_misc.checkBox_sctmod_endcapC);
  s.save(d->ui_disp.widget_drawOptions);
  s.save(d->ui_misc.lineEdit_expand_vols_matname);
  s.save(d->ui_misc.lineEdit_expand_vols_volname);
  s.save(d->ui_int.checkBox_geomselectable);
  s.save(d->ui_int.checkBox_print_shape);
  s.save(d->ui_int.checkBox_print_material);
  s.save(d->ui_int.checkBox_print_transform);
  s.save(d->ui_int.checkBox_print_mass);
  s.save(d->ui_int.checkBox_print_copyno);
  s.save(d->ui_int.checkBox_print_tree);
  s.save(d->ui_int.checkBox_print_muonstationinfo);
  s.save(d->ui_int.checkBox_zoomToVolumes);
  s.save(d->ui_int.checkBox_AutomaticMuonChamberEndViews);
  s.save(d->ui_disp.checkBox_showVolumeOutLines);//version 1+
  s.save(d->ui_misc.checkBox_bcmMod_Aside);//version 3+
  s.save(d->ui_misc.checkBox_bcmMod_Cside);//version 3+

  // labels - version 4+
  s.save(d->ui_disp.groupBox_labels);
  s.save(d->ui_disp.horizontalSlider_labels_xOffset);
  s.save(d->ui_disp.horizontalSlider_labels_yOffset);
  s.save(d->ui_disp.horizontalSlider_labels_zOffset);
  s.save(d->ui_disp.checkBox_labels_names);
  //version 5+
  s.save(d->ui_disp.checkBox_labels_mboyt0s);
  s.save(d->ui_disp.checkBox_labels_mooret0s);
  s.save(d->ui_disp.checkBox_labels_hits); 
  
  s.ignoreWidget(d->ui_disp.matButton_lastSel);
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*>::const_iterator it,itE(d->subSysCheckBoxMap.end());
  for (it=d->subSysCheckBoxMap.begin();it!=itE;++it)
    s.ignoreWidget(it->second);
}

//____________________________________________________________________
void GeoSysController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>currentSettingsVersion()) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }

  if (s.version()<=1) {
    s.ignoreObsoletePhiSectionWidgetState();
    s.ignoreWidget(d->ui_disp.phisectionwidget);
  } else {
    s.restore(d->ui_disp.phisectionwidget);
  }
  s.restore(d->ui_disp.spinBox_transp);
  s.restore(d->ui_muon.checkBox_muonadapt_autoadapt);
  s.restore(d->ui_muon.checkBox_muonadapt_openmdtchambers);
  s.restore(d->ui_muon.checkBox_muonadapt_hidetubes);
  s.restore(d->ui_muon.checkBox_muonadapt_hiderpcvolumes);
  s.restore(d->ui_muon.checkBox_muonadapt_opencscchambers);
  s.restore(d->ui_muon.checkBox_muonadapt_opentgcchambers);
  s.restore(d->ui_misc.checkBox_pixelmod_barrel);
  s.restore(d->ui_misc.checkBox_pixelmod_endcapA);
  s.restore(d->ui_misc.checkBox_pixelmod_endcapC);
  s.restore(d->ui_misc.checkBox_sctmod_barrel);
  s.restore(d->ui_misc.checkBox_sctmod_endcapA);
  s.restore(d->ui_misc.checkBox_sctmod_endcapC);
  s.restore(d->ui_disp.widget_drawOptions);
  s.restore(d->ui_misc.lineEdit_expand_vols_matname);
  s.restore(d->ui_misc.lineEdit_expand_vols_volname);
  s.restore(d->ui_int.checkBox_geomselectable);
  s.restore(d->ui_int.checkBox_print_shape);
  s.restore(d->ui_int.checkBox_print_material);
  s.restore(d->ui_int.checkBox_print_transform);
  s.restore(d->ui_int.checkBox_print_mass);
  s.restore(d->ui_int.checkBox_print_copyno);
  s.restore(d->ui_int.checkBox_print_tree);
  s.restore(d->ui_int.checkBox_print_muonstationinfo);
  s.restore(d->ui_int.checkBox_zoomToVolumes);
  s.restore(d->ui_int.checkBox_AutomaticMuonChamberEndViews);
  if (s.version()>=1)
    s.restore(d->ui_disp.checkBox_showVolumeOutLines);

  if (s.version()>=3) {
    s.restore(d->ui_misc.checkBox_bcmMod_Aside);
    s.restore(d->ui_misc.checkBox_bcmMod_Cside);
  }

  // labels
  if (s.version()>=4){
    s.restore(d->ui_disp.groupBox_labels);
    s.restore(d->ui_disp.horizontalSlider_labels_xOffset);
    s.restore(d->ui_disp.horizontalSlider_labels_yOffset);
    s.restore(d->ui_disp.horizontalSlider_labels_zOffset);
    s.restore(d->ui_disp.checkBox_labels_names);
  }  
  if (s.version()>=5){
    s.restore(d->ui_disp.checkBox_labels_mboyt0s);
    s.restore(d->ui_disp.checkBox_labels_mooret0s);
    s.restore(d->ui_disp.checkBox_labels_hits);
  }

  s.ignoreWidget(d->ui_disp.matButton_lastSel);
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*>::const_iterator it,itE(d->subSysCheckBoxMap.end());
  for (it=d->subSysCheckBoxMap.begin();it!=itE;++it)
    s.ignoreWidget(it->second);

  d->ui_int.groupBox_printOnSelection->setEnabled(d->ui_int.checkBox_geomselectable->isChecked());
  d->ui_int.groupBox_automaticViewReorientations->setEnabled(d->ui_int.checkBox_geomselectable->isChecked());

}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME GeoSysController
#include "VP1Base/VP1ControllerMacros.h"
POSSIBLECHANGE_IMP(transparency)
POSSIBLECHANGE_IMP(showVolumeOutLines)
POSSIBLECHANGE_IMP(muonChamberAdaptionStyle)
POSSIBLECHANGE_IMP(labels)
POSSIBLECHANGE_IMP(labelPosOffset)



