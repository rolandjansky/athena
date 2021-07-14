/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class GeoSysController                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: October 2008                             //
//                                                            //
////////////////////////////////////////////////////////////////

#define VP1IMPVARNAME m_d

#include "VP1GeometrySystems/GeoSysController.h"
#include "VP1GeometrySystems/ZappedVolumeListModel.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Settings.h"
#include "VP1Base/IVP1System.h"
#ifndef BUILDVP1LIGHT
    #include "VP1Utils/VP1JobConfigInfo.h"
#endif

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
  GeoSysController * theclass = nullptr;
  Ui::GeometrySystemControllerForm ui{};
  Ui::GeoSysSettingsDisplayForm ui_disp{};
  Ui::GeoSysSettingsIconisedVolumesForm ui_iconisedvols{};
  Ui::GeoSysSettingsInteractionsForm ui_int{};
  Ui::GeoSysSettingsMiscForm ui_misc{};
  Ui::GeoSysSettingsMuonChamberForm ui_muon{};
  Ui::GeoSysSettingsTreeBrowserForm ui_treebrowser{};
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*> subSysCheckBoxMap;
  float last_transparency = 0.0F;
  bool last_showVolumeOutLines = false;
  VP1GeoFlags::MuonChamberAdaptionStyleFlags last_muonChamberAdaptionStyle;
  int last_labels = 0; //!< needed for POSSIBLECHANGE_IMP macro.
  QList<int> last_labelPosOffset; //!< needed for  POSSIBLECHANGE_IMP macro.
  SoPickStyle * pickStyle = nullptr;
  VolumeHandle * lastSelectedVolHandle = nullptr;
  ZappedVolumeListModel * zappedVolumeListModel = nullptr;

  QString lastSaveMaterialsFile;
  QString lastLoadMaterialsFile;
  
//  std::map<QString, QList<QCheckBox*> > labelProvidingSystems; //!< First is name of system, second is list of types of information provided (system stores actual information)
};


//____________________________________________________________________
GeoSysController::GeoSysController(IVP1System * sys)
  : VP1Controller(sys,"GeoSysController"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->ui.setupUi(this);

  initDialog(m_d->ui_disp, m_d->ui.pushButton_settings_display);
  initDialog(m_d->ui_iconisedvols, m_d->ui.pushButton_settings_iconisedvols);
  initDialog(m_d->ui_int, m_d->ui.pushButton_settings_interactions);
  initDialog(m_d->ui_misc, m_d->ui.pushButton_settings_misc);
  initDialog(m_d->ui_muon, m_d->ui.pushButton_settings_muonchambers);
  initDialog(m_d->ui_treebrowser, m_d->ui.pushButton_settings_treebrowser);

  //  m_d->ui_disp.widget_drawOptions->setLineWidthsDisabled();
  m_d->ui_disp.widget_drawOptions->setPointSizesDisabled();
//   m_d->ui_disp.widget_drawOptions->setBaseLightingDisabled();
  m_d->ui_disp.widget_drawOptions->setComplexity(0.6);

  m_d->pickStyle = new SoPickStyle;
  m_d->pickStyle->ref();

  #ifndef BUILDVP1LIGHT
  //Possibly hide parts of gui, depending on job configuration:
  if (!VP1JobConfigInfo::hasGeoModelExperiment()) {
    m_d->ui.groupBox_innerdetector->setVisible(false);
    m_d->ui.groupBox_calorimeters->setVisible(false);
    m_d->ui.groupBox_muonsystems->setVisible(false);
    m_d->ui.pushButton_settings_muonchambers->setEnabled(false);
    m_d->ui_misc.groupBox_pixelactivemodules->setVisible(false);
    m_d->ui_misc.groupBox_sctactivemodules->setVisible(false);
    m_d->ui.groupBox_misc->setVisible(false);
    m_d->ui_muon.groupBox_muonchamberconfig->setVisible(false);
    m_d->ui_misc.groupBox_show_NSW_chambers->setVisible(false);
    m_d->ui_int.checkBox_AutomaticMuonChamberEndViews->setVisible(false);
    message("GeoModel not properly initialised.");
  } else {
    m_d->ui.groupBox_innerdetector->setVisible(VP1JobConfigInfo::hasPixelGeometry()
					      || VP1JobConfigInfo::hasSCTGeometry()
					      || VP1JobConfigInfo::hasTRTGeometry()
					      || VP1JobConfigInfo::hasInDetServiceMaterialGeometry());
    m_d->ui_misc.groupBox_pixelactivemodules->setVisible(VP1JobConfigInfo::hasPixelGeometry());
    m_d->ui_misc.groupBox_sctactivemodules->setVisible(VP1JobConfigInfo::hasSCTGeometry());
    m_d->ui.groupBox_calorimeters->setVisible(VP1JobConfigInfo::hasLArGeometry()
					     ||VP1JobConfigInfo::hasTileGeometry());
    m_d->ui.groupBox_muonsystems->setVisible(VP1JobConfigInfo::hasMuonGeometry());
    m_d->ui.pushButton_settings_muonchambers->setEnabled(VP1JobConfigInfo::hasMuonGeometry());
    m_d->ui_int.checkBox_print_muonstationinfo->setVisible(VP1JobConfigInfo::hasMuonGeometry());
    m_d->ui_muon.groupBox_muonchamberconfig->setVisible(VP1JobConfigInfo::hasMuonGeometry());
    m_d->ui_misc.groupBox_show_NSW_chambers->setVisible(VP1JobConfigInfo::hasMuonNSWGeometry());
    m_d->ui_int.checkBox_AutomaticMuonChamberEndViews->setVisible(VP1JobConfigInfo::hasMuonGeometry());
  }
  #endif


  m_d->zappedVolumeListModel = new ZappedVolumeListModel(m_d->ui_iconisedvols.listView_iconisedvolumes);
  m_d->ui_iconisedvols.listView_iconisedvolumes->setUniformItemSizes(true);
  m_d->ui_iconisedvols.listView_iconisedvolumes->setSelectionMode(QAbstractItemView::ExtendedSelection);
  m_d->ui_iconisedvols.listView_iconisedvolumes->setModel(m_d->zappedVolumeListModel);

  connect(m_d->ui_iconisedvols.listView_iconisedvolumes,SIGNAL(activated(const QModelIndex&)),
	  m_d->zappedVolumeListModel,SLOT(activated(const QModelIndex&)));

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  addUpdateSlot(SLOT(updatePickStyle()));
  connectToLastUpdateSlot(m_d->ui_int.checkBox_geomselectable);

  addUpdateSlot(SLOT(possibleChange_showVolumeOutLines()));
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_showVolumeOutLines);

  addUpdateSlot(SLOT(possibleChange_transparency()));
  connectToLastUpdateSlot(m_d->ui_disp.spinBox_transp);

  addUpdateSlot(SLOT(possibleChange_muonChamberAdaptionStyle()));
  connectToLastUpdateSlot(m_d->ui_muon.checkBox_muonadapt_openmdtchambers);
  connectToLastUpdateSlot(m_d->ui_muon.checkBox_muonadapt_hidetubes);
  connectToLastUpdateSlot(m_d->ui_muon.checkBox_muonadapt_hiderpcvolumes);
  connectToLastUpdateSlot(m_d->ui_muon.checkBox_muonadapt_opencscchambers);
  connectToLastUpdateSlot(m_d->ui_muon.checkBox_muonadapt_opentgcchambers);

  initLastVars();

  //Connections for state-less signals:

  connect(m_d->ui_disp.pushButton_saveChangedMaterials,SIGNAL(clicked()),this,SLOT(saveMaterialsRequested()));
  connect(m_d->ui_disp.pushButton_loadMaterials,SIGNAL(clicked()),this,SLOT(loadMaterialsRequested()));

  connect(m_d->ui_misc.pushButton_nonStandardShapes_Iconify,SIGNAL(clicked()),
	  this,SLOT(emit_actionOnAllNonStandardVolumes()));
  connect(m_d->ui_misc.pushButton_nonStandardShapes_Expand,SIGNAL(clicked()),
	  this,SLOT(emit_actionOnAllNonStandardVolumes()));

  connect(m_d->ui_misc.lineEdit_expand_vols_matname,SIGNAL(returnPressed()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));
  connect(m_d->ui_misc.pushButton_expand_vols_matname,SIGNAL(clicked()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));
  connect(m_d->ui_misc.lineEdit_expand_vols_volname,SIGNAL(returnPressed()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));
  connect(m_d->ui_misc.pushButton_expand_vols_volname,SIGNAL(clicked()),this,SLOT(emit_autoExpandByVolumeOrMaterialName()));

  connect(m_d->ui_muon.pushButton_muonadapt_adapttoevtdata,SIGNAL(clicked()),this,SLOT(emit_adaptMuonChambersToEventData()));

  connect(m_d->ui_misc.toolButton_pixelmod_adapt,SIGNAL(clicked(bool)),this,SLOT(emit_autoAdaptPixelsOrSCT()));
  connect(m_d->ui_misc.toolButton_sctmod_adapt,SIGNAL(clicked(bool)),this,SLOT(emit_autoAdaptPixelsOrSCT()));
  
  connect(m_d->ui_misc.toolButton_nswchambers_adapt,SIGNAL(clicked(bool)),this,SLOT(emit_autoAdaptMuonNSW()));
  connect(m_d->ui_misc.toolButton_nswchambers_reset,SIGNAL(clicked(bool)),this,SLOT(emit_autoAdaptMuonNSW()));

  connect(m_d->ui_misc.toolButton_pixelmod_reset,SIGNAL(clicked(bool)),this,SLOT(emit_resetSubSystems()));
  connect(m_d->ui_misc.toolButton_sctmod_reset,SIGNAL(clicked(bool)),this,SLOT(emit_resetSubSystems()));
  connect(m_d->ui_muon.pushButton_muonadapt_resetToAllChambers,SIGNAL(clicked()),this,SLOT(emit_resetSubSystems()));

  setLastSelectedVolume(0);

  // INNER DETECTOR
  m_d->subSysCheckBoxMap[VP1GeoFlags::Pixel] = m_d->ui.checkBox_Pixel;
  m_d->subSysCheckBoxMap[VP1GeoFlags::SCT] = m_d->ui.checkBox_SCT;
  m_d->subSysCheckBoxMap[VP1GeoFlags::TRT] = m_d->ui.checkBox_TRT;
  m_d->subSysCheckBoxMap[VP1GeoFlags::InDetServMat] = m_d->ui.checkBox_InDetServMat;

  // CALO
  m_d->subSysCheckBoxMap[VP1GeoFlags::LAr] = m_d->ui.checkBox_LAr;
  m_d->subSysCheckBoxMap[VP1GeoFlags::Tile] = m_d->ui.checkBox_Tile;

  // MUON TOROID
  m_d->subSysCheckBoxMap[VP1GeoFlags::BarrelToroid] = m_d->ui.checkBox_MuonBarrelToroid;
  m_d->subSysCheckBoxMap[VP1GeoFlags::ToroidECA] = m_d->ui.checkBox_MuonECAToroid;
  // m_d->subSysCheckBoxMap[VP1GeoFlags::ToroidECC] = m_d->ui.checkBox_MuonECCToroid;
  // MUON MISC
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonFeet] = m_d->ui.checkBox_MuonFeet;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonShielding] = m_d->ui.checkBox_MuonShielding;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonToroidsEtc] = m_d->ui.checkBox_MuonEtc; // this is the "Services" checkbox in the GUI
  // MUON CHAMBERS
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonEndcapStationCSC] = m_d->ui.checkBox_MuonEndcapStationCSC;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonEndcapStationTGC] = m_d->ui.checkBox_MuonEndcapStationTGC;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonEndcapStationMDT] = m_d->ui.checkBox_MuonEndcapStationMDT;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonEndcapStationNSW] = m_d->ui.checkBox_NSW;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonBarrelStationInner] = m_d->ui.checkBox_MuonBarrelStationInner;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonBarrelStationMiddle] = m_d->ui.checkBox_MuonBarrelStationMiddle;
  m_d->subSysCheckBoxMap[VP1GeoFlags::MuonBarrelStationOuter] = m_d->ui.checkBox_MuonBarrelStationOuter;

  // MISCELLANEOUS
  m_d->subSysCheckBoxMap[VP1GeoFlags::CavernInfra] = m_d->ui.checkBox_CavernInfra;
  m_d->subSysCheckBoxMap[VP1GeoFlags::BeamPipe] = m_d->ui.checkBox_BeamPipe;
  m_d->subSysCheckBoxMap[VP1GeoFlags::LUCID] = m_d->ui.checkBox_LUCID;
  m_d->subSysCheckBoxMap[VP1GeoFlags::ZDC] = m_d->ui.checkBox_ZDC;
  m_d->subSysCheckBoxMap[VP1GeoFlags::ALFA] = m_d->ui.checkBox_ALFA;
  m_d->subSysCheckBoxMap[VP1GeoFlags::ForwardRegion] = m_d->ui.checkBox_ForwardRegion;
  // OTHER
  m_d->subSysCheckBoxMap[VP1GeoFlags::AllUnrecognisedVolumes] = m_d->ui.checkBox_other;

  // -> labels
  addUpdateSlot(SLOT(possibleChange_labels()));
  connectToLastUpdateSlot(m_d->ui_disp.groupBox_labels);
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_labels_names);
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_labels_mooret0s);
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_labels_mboyt0s);
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_labels_hits);
  
  addUpdateSlot(SLOT(possibleChange_labelPosOffset()));
  connectToLastUpdateSlot(m_d->ui_disp.horizontalSlider_labels_xOffset);
  connectToLastUpdateSlot(m_d->ui_disp.horizontalSlider_labels_yOffset);
  connectToLastUpdateSlot(m_d->ui_disp.horizontalSlider_labels_zOffset);
  
  m_d->last_labels=0;
}

//____________________________________________________________________
GeoSysController::~GeoSysController()
{
  m_d->pickStyle->unref();
  delete m_d;
}

//____________________________________________________________________
void GeoSysController::setGeometrySelectable(bool b)
{
  m_d->ui_int.checkBox_geomselectable->setChecked(b);
  updatePickStyle();
}

//____________________________________________________________________
void GeoSysController::setComplexity(double c)
{
  m_d->ui_disp.widget_drawOptions->setComplexity(c);
}

//____________________________________________________________________
void GeoSysController::setZoomToVolumeOnClick(bool b)
{
  m_d->ui_int.checkBox_zoomToVolumes->setChecked(b);
}

//____________________________________________________________________
void GeoSysController::setOrientViewToMuonChambersOnClick(bool b)
{
  m_d->ui_int.checkBox_AutomaticMuonChamberEndViews->setChecked(b);
}

//____________________________________________________________________
void GeoSysController::setAutoAdaptMuonChambersToEventData(bool b)
{
  m_d->ui_muon.checkBox_muonadapt_autoadapt->setChecked(b);
}

//____________________________________________________________________
void GeoSysController::setLastSelectedVolume(VolumeHandle*vh)
{
  m_d->lastSelectedVolHandle = vh;
  m_d->ui_disp.matButton_lastSel->setEnabled(vh!=0);
  if (vh) {
    QList<SoMaterial*> mats = m_d->ui_disp.matButton_lastSel->handledMaterials();
    if (mats.isEmpty()||mats.at(0)!=vh->material()) {
      m_d->ui_disp.matButton_lastSel->clearHandledMaterials();
      m_d->ui_disp.matButton_lastSel->setMaterial(vh->material());
    }
  } else {
    m_d->ui_disp.matButton_lastSel->clearHandledMaterials();
  }
}

//____________________________________________________________________
VolumeHandle* GeoSysController::lastSelectedVolume() const
{
  return m_d->lastSelectedVolHandle;
}

//____________________________________________________________________
void GeoSysController::saveMaterialsRequested()
{
  const bool ctrl_isdown = (Qt::ControlModifier & QApplication::keyboardModifiers());
  const bool onlyChangedMaterials = !ctrl_isdown;

  QString filename
    = QFileDialog::getSaveFileName(this, "Select geometry material file to save",
				   (m_d->lastSaveMaterialsFile.isEmpty()?
				    VP1Settings::defaultFileSelectDirectory()
				    :m_d->lastSaveMaterialsFile),
				   "VP1 geometry material files (*.vp1geomat)",
				   0,QFileDialog::DontResolveSymlinks);

  if(filename.isEmpty())
    return;

  if (!filename.endsWith(".vp1geomat"))
    filename += ".vp1geomat";

  m_d->lastSaveMaterialsFile = filename;

  messageVerbose("Emitting saveMaterialsToFile(\""+filename+"\","+str(onlyChangedMaterials)+")");
  emit saveMaterialsToFile(filename,onlyChangedMaterials);

}

//____________________________________________________________________
void GeoSysController::loadMaterialsRequested()
{
  QString filename = QFileDialog::getOpenFileName(this, "Select geometry material file to load",
						  (m_d->lastLoadMaterialsFile.isEmpty()?
						   VP1Settings::defaultFileSelectDirectory()
						   :m_d->lastLoadMaterialsFile),
						  "VP1 geometry material files (*.vp1geomat)",
						  0,QFileDialog::DontResolveSymlinks);
  if(filename.isEmpty())
    return;

  m_d->lastLoadMaterialsFile = filename;

  messageVerbose("Emitting loadMaterialsFromFile(\""+filename+"\")");
  emit loadMaterialsFromFile(filename);
}

//____________________________________________________________________
QCheckBox * GeoSysController::subSystemCheckBox(VP1GeoFlags::SubSystemFlag f) const
{
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*>::const_iterator it = m_d->subSysCheckBoxMap.find(f);
  return it==m_d->subSysCheckBoxMap.end() ? 0 : it->second;
}

//____________________________________________________________________
SoGroup * GeoSysController::drawOptions() const
{
  return m_d->ui_disp.widget_drawOptions->drawOptionsGroup();
}

//____________________________________________________________________
SoPickStyle * GeoSysController::pickStyle() const
{
  return m_d->pickStyle;
}

//____________________________________________________________________
VP1GeoTreeView * GeoSysController::volumeTreeBrowser() const
{
  return m_d->ui_treebrowser.treeView_volumebrowser;
}

//____________________________________________________________________
PhiSectionWidget * GeoSysController::phiSectionWidget() const
{
  return m_d->ui_disp.phisectionwidget;
}

//____________________________________________________________________
ZappedVolumeListModel * GeoSysController::zappedVolumeListModel() const
{
  return m_d->zappedVolumeListModel;
}

//____________________________________________________________________
bool GeoSysController::zoomToVolumeOnClick() const
{
  return m_d->ui_int.checkBox_zoomToVolumes->isChecked();
}

//____________________________________________________________________
bool GeoSysController::orientViewToMuonChambersOnClick() const
{
  return m_d->ui_int.checkBox_AutomaticMuonChamberEndViews->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Shape() const
{
  return m_d->ui_int.checkBox_print_shape->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Material() const
{
  return m_d->ui_int.checkBox_print_material->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_CopyNumber() const
{
  return m_d->ui_int.checkBox_print_copyno->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Transform() const
{
  return m_d->ui_int.checkBox_print_transform->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Tree() const
{
  return m_d->ui_int.checkBox_print_tree->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_Mass() const
{
  return m_d->ui_int.checkBox_print_mass->isChecked();
}

//____________________________________________________________________
bool GeoSysController::printInfoOnClick_MuonStationInfo() const
{
  return m_d->ui_int.checkBox_print_muonstationinfo->isChecked();
}

//____________________________________________________________________
bool GeoSysController::autoAdaptMuonChambersToEventData() const
{
  return m_d->ui_muon.checkBox_muonadapt_autoadapt->isChecked();
}

//_____________________________________________________________________________________
void GeoSysController::updatePickStyle()
{
  m_d->pickStyle->style = m_d->ui_int.checkBox_geomselectable->isChecked()
    ? SoPickStyle::SHAPE : SoPickStyle::UNPICKABLE;
}

//____________________________________________________________________
float GeoSysController::transparency() const
{
  int v(m_d->ui_disp.spinBox_transp->value());
  return (v>=100?1.0:(v<=0?0.0:v/100.0));
}

//____________________________________________________________________
bool GeoSysController::showVolumeOutLines() const
{
  return m_d->ui_disp.checkBox_showVolumeOutLines->isChecked();
}

//____________________________________________________________________
VP1GeoFlags::MuonChamberAdaptionStyleFlags GeoSysController::muonChamberAdaptionStyle() const
{
  VP1GeoFlags::MuonChamberAdaptionStyleFlags f;
  if (m_d->ui_muon.checkBox_muonadapt_openmdtchambers->isChecked())
    f |= VP1GeoFlags::OpenMDTChambers;
  if (m_d->ui_muon.checkBox_muonadapt_hidetubes->isChecked())
    f |= VP1GeoFlags::HideMDTTubes;
  if (m_d->ui_muon.checkBox_muonadapt_hiderpcvolumes->isChecked())
    f |= VP1GeoFlags::HideRPCVolumes;
  if (m_d->ui_muon.checkBox_muonadapt_opencscchambers->isChecked())
    f |= VP1GeoFlags::OpenCSCChambers;
  if (m_d->ui_muon.checkBox_muonadapt_opentgcchambers->isChecked())
    f |= VP1GeoFlags::OpenTGCChambers;
  return f;
}

//void GeoSysController::setAvailableLabels(IVP1System* sys, QStringList providedLabels){
//  messageVerbose("setAvailableLabels for "+sys->name() );
//  if (m_d->labelProvidingSystems.find(sys->name())!=m_d->labelProvidingSystems.end())
//    return;
//  QList<QCheckBox*> checkboxes;
//  foreach(QString label, providedLabels) {
//    QCheckBox *checkbox = new QCheckBox(label, this);
//    checkboxes.append(checkbox);
//  }
//  m_d->labelProvidingSystems[sys->name()]= checkboxes;
////  std::map<QString, QList<QCheckBox*> > labelProvidingSystems; //!< First is name of system, second is list of types of information provided (system stores actual information)
//
//}

void GeoSysController::setLabelsEnabled(bool t0s, bool hits){
  // make sure that when labels are enabled, only the systems which we have are enabled too
  if (t0s) {
    connect(m_d->ui_disp.groupBox_labels,SIGNAL(toggled(bool)),
                   m_d->ui_disp.checkBox_labels_mooret0s,SLOT(setEnabled(bool)));
    connect(m_d->ui_disp.groupBox_labels,SIGNAL(toggled(bool)),
            m_d->ui_disp.checkBox_labels_mboyt0s,SLOT(setEnabled(bool)));
  } else {
    m_d->ui_disp.checkBox_labels_mooret0s->setEnabled(false);
    m_d->ui_disp.checkBox_labels_mboyt0s->setEnabled(false);
  }
  if (hits) {
    connect(m_d->ui_disp.groupBox_labels,SIGNAL(toggled(bool)),
                   m_d->ui_disp.checkBox_labels_hits,SLOT(setEnabled(bool)));
  } else {
    m_d->ui_disp.checkBox_labels_hits->setEnabled(false);
  }

  messageVerbose("setLabelsEnabled() t0s="+str(t0s)+", hits="+str(hits));
}


int GeoSysController::labels() const {
  if (!m_d->ui_disp.groupBox_labels->isChecked()) return 0;
  int labels=0;
  if (m_d->ui_disp.checkBox_labels_names->isChecked())    labels|=0x1;
  if (m_d->ui_disp.checkBox_labels_mooret0s->isChecked()) labels|=0x2;
  if (m_d->ui_disp.checkBox_labels_mboyt0s->isChecked())  labels|=0x4;
  // leaving space for another t0 type, if necessary
  if (m_d->ui_disp.checkBox_labels_hits->isChecked())     labels|=0x10;
  return labels;
}

QList<int> GeoSysController::labelPosOffset() 
{
  QList<int> values;
  values << m_d->ui_disp.horizontalSlider_labels_xOffset->value();
  values << m_d->ui_disp.horizontalSlider_labels_yOffset->value();
  values << m_d->ui_disp.horizontalSlider_labels_zOffset->value();
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
  bool pixel(sender()==m_d->ui_misc.toolButton_pixelmod_adapt);
  bool brl = pixel ? m_d->ui_misc.checkBox_pixelmod_barrel->isChecked()
    : m_d->ui_misc.checkBox_sctmod_barrel->isChecked();
  bool ecA = pixel ? m_d->ui_misc.checkBox_pixelmod_endcapA->isChecked()
    : m_d->ui_misc.checkBox_sctmod_endcapA->isChecked();
  bool ecC = pixel ? m_d->ui_misc.checkBox_pixelmod_endcapC->isChecked()
    : m_d->ui_misc.checkBox_sctmod_endcapC->isChecked();
  bool bcmA = pixel ? m_d->ui_misc.checkBox_bcmMod_Aside->isChecked() : false;
  bool bcmC = pixel ? m_d->ui_misc.checkBox_bcmMod_Cside->isChecked() : false;

  messageVerbose ("Emitting autoAdaptPixelsOrSCT("+str(pixel)+", "+str(brl)+","+str(ecA)+","+str(ecC)+","+str(bcmA)+","+str(bcmC)+")");
  emit autoAdaptPixelsOrSCT(pixel,brl,ecA,ecC,bcmA,bcmC);
}


//____________________________________________________________________
void GeoSysController::emit_autoAdaptMuonNSW()
{
  bool reset = false;
  if (sender()==m_d->ui_misc.toolButton_nswchambers_reset) {
    reset = true;
  }
  bool stgc = m_d->ui_misc.checkBox_NSW_sTGC->isChecked();
  bool mm   = m_d->ui_misc.checkBox_NSW_MM->isChecked();

  messageVerbose ("Emitting autoAdaptPixelsOrSCT("+str(reset)+","+str(stgc)+","+str(mm)+")");
  emit autoAdaptMuonNSW(reset, stgc, mm);
}



//____________________________________________________________________
void GeoSysController::emit_autoExpandByVolumeOrMaterialName()
{
  bool volname(sender()==m_d->ui_misc.pushButton_expand_vols_volname
	       ||sender()==m_d->ui_misc.lineEdit_expand_vols_volname);
  QString name(volname?m_d->ui_misc.lineEdit_expand_vols_volname->text()
	       :m_d->ui_misc.lineEdit_expand_vols_matname->text());
  if (name.isEmpty())
    return;
  messageVerbose("emitting autoExpandByVolumeOrMaterialName("+str(!volname)+", "+name+")");
  emit autoExpandByVolumeOrMaterialName(!volname,name);
}

//____________________________________________________________________
void GeoSysController::emit_actionOnAllNonStandardVolumes() {
  bool zap(sender()==m_d->ui_misc.pushButton_nonStandardShapes_Iconify);
  messageVerbose("emitting actionOnAllNonStandardVolumes("+str(zap)+")");
  emit actionOnAllNonStandardVolumes(zap);
}

//____________________________________________________________________
//! Reset to the sub-system top volume.
/*!
  The method reset the view to the whole sub-system 
  by contracting all children to the mother volume.
  It does work properly only if the top sub-system 
  volume has a concrete shape. If, instead, it is a 
  mere container, then this method does not work 
  because it will hide the children of the top 
  container volume. In that case, you should implement 
  a custom method. 
  
  Note: for an example of a custom solution, 
  please refer to the 'VP1GeometrySystem::autoAdaptMuonNSW()' 
  method, triggered by the emission of the 'autoAdaptMuonNSW()'
  signal in the 'emit_autoAdaptMuonNSW()' of this class.
*/
void GeoSysController::emit_resetSubSystems()
{
  VP1GeoFlags::SubSystemFlags f;
  if (sender()==m_d->ui_misc.toolButton_pixelmod_reset)
    f = VP1GeoFlags::Pixel;
  else if (sender()==m_d->ui_misc.toolButton_sctmod_reset)
    f = VP1GeoFlags::SCT;
  else if (sender()==m_d->ui_muon.pushButton_muonadapt_resetToAllChambers)
    f = VP1GeoFlags::AllMuonChambers;
  messageVerbose(" Emitting resetSubSystems("+str(f)+")");
  emit resetSubSystems(f);
}

//____________________________________________________________________
int GeoSysController::currentSettingsVersion() const
{
  return 6;
}

//____________________________________________________________________
void GeoSysController::actualSaveSettings(VP1Serialise&s) const
{
  s.save(m_d->ui_disp.phisectionwidget);//version <=1 saved in old format
  s.save(m_d->ui_disp.spinBox_transp);
  s.save(m_d->ui_muon.checkBox_muonadapt_autoadapt);
  s.save(m_d->ui_muon.checkBox_muonadapt_openmdtchambers);
  s.save(m_d->ui_muon.checkBox_muonadapt_hidetubes);
  s.save(m_d->ui_muon.checkBox_muonadapt_hiderpcvolumes);
  s.save(m_d->ui_muon.checkBox_muonadapt_opencscchambers);
  s.save(m_d->ui_muon.checkBox_muonadapt_opentgcchambers);
  s.save(m_d->ui_misc.checkBox_pixelmod_barrel);
  s.save(m_d->ui_misc.checkBox_pixelmod_endcapA);
  s.save(m_d->ui_misc.checkBox_pixelmod_endcapC);
  s.save(m_d->ui_misc.checkBox_sctmod_barrel);
  s.save(m_d->ui_misc.checkBox_sctmod_endcapA);
  s.save(m_d->ui_misc.checkBox_sctmod_endcapC);
  s.save(m_d->ui_disp.widget_drawOptions);
  s.save(m_d->ui_misc.lineEdit_expand_vols_matname);
  s.save(m_d->ui_misc.lineEdit_expand_vols_volname);
  s.save(m_d->ui_int.checkBox_geomselectable);
  s.save(m_d->ui_int.checkBox_print_shape);
  s.save(m_d->ui_int.checkBox_print_material);
  s.save(m_d->ui_int.checkBox_print_transform);
  s.save(m_d->ui_int.checkBox_print_mass);
  s.save(m_d->ui_int.checkBox_print_copyno);
  s.save(m_d->ui_int.checkBox_print_tree);
  s.save(m_d->ui_int.checkBox_print_muonstationinfo);
  s.save(m_d->ui_int.checkBox_zoomToVolumes);
  s.save(m_d->ui_int.checkBox_AutomaticMuonChamberEndViews);
  s.save(m_d->ui_disp.checkBox_showVolumeOutLines);//version 1+
  s.save(m_d->ui_misc.checkBox_bcmMod_Aside);//version 3+
  s.save(m_d->ui_misc.checkBox_bcmMod_Cside);//version 3+

  // labels - version 4+
  s.save(m_d->ui_disp.groupBox_labels);
  s.save(m_d->ui_disp.horizontalSlider_labels_xOffset);
  s.save(m_d->ui_disp.horizontalSlider_labels_yOffset);
  s.save(m_d->ui_disp.horizontalSlider_labels_zOffset);
  s.save(m_d->ui_disp.checkBox_labels_names);
  //version 5+
  s.save(m_d->ui_disp.checkBox_labels_mboyt0s);
  s.save(m_d->ui_disp.checkBox_labels_mooret0s);
  s.save(m_d->ui_disp.checkBox_labels_hits); 
  // version >=6
  s.save(m_d->ui_misc.checkBox_NSW_MM);
  s.save(m_d->ui_misc.checkBox_NSW_sTGC);
  
  
  s.ignoreWidget(m_d->ui_disp.matButton_lastSel);
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*>::const_iterator it,itE(m_d->subSysCheckBoxMap.end());
  for (it=m_d->subSysCheckBoxMap.begin();it!=itE;++it)
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
    s.ignoreWidget(m_d->ui_disp.phisectionwidget);
  } else {
    s.restore(m_d->ui_disp.phisectionwidget);
  }
  s.restore(m_d->ui_disp.spinBox_transp);
  s.restore(m_d->ui_muon.checkBox_muonadapt_autoadapt);
  s.restore(m_d->ui_muon.checkBox_muonadapt_openmdtchambers);
  s.restore(m_d->ui_muon.checkBox_muonadapt_hidetubes);
  s.restore(m_d->ui_muon.checkBox_muonadapt_hiderpcvolumes);
  s.restore(m_d->ui_muon.checkBox_muonadapt_opencscchambers);
  s.restore(m_d->ui_muon.checkBox_muonadapt_opentgcchambers);
  s.restore(m_d->ui_misc.checkBox_pixelmod_barrel);
  s.restore(m_d->ui_misc.checkBox_pixelmod_endcapA);
  s.restore(m_d->ui_misc.checkBox_pixelmod_endcapC);
  s.restore(m_d->ui_misc.checkBox_sctmod_barrel);
  s.restore(m_d->ui_misc.checkBox_sctmod_endcapA);
  s.restore(m_d->ui_misc.checkBox_sctmod_endcapC);
  s.restore(m_d->ui_disp.widget_drawOptions);
  s.restore(m_d->ui_misc.lineEdit_expand_vols_matname);
  s.restore(m_d->ui_misc.lineEdit_expand_vols_volname);
  s.restore(m_d->ui_int.checkBox_geomselectable);
  s.restore(m_d->ui_int.checkBox_print_shape);
  s.restore(m_d->ui_int.checkBox_print_material);
  s.restore(m_d->ui_int.checkBox_print_transform);
  s.restore(m_d->ui_int.checkBox_print_mass);
  s.restore(m_d->ui_int.checkBox_print_copyno);
  s.restore(m_d->ui_int.checkBox_print_tree);
  s.restore(m_d->ui_int.checkBox_print_muonstationinfo);
  s.restore(m_d->ui_int.checkBox_zoomToVolumes);
  s.restore(m_d->ui_int.checkBox_AutomaticMuonChamberEndViews);
  if (s.version()>=1)
    s.restore(m_d->ui_disp.checkBox_showVolumeOutLines);

  if (s.version()>=3) {
    s.restore(m_d->ui_misc.checkBox_bcmMod_Aside);
    s.restore(m_d->ui_misc.checkBox_bcmMod_Cside);
  }

  // labels
  if (s.version()>=4){
    s.restore(m_d->ui_disp.groupBox_labels);
    s.restore(m_d->ui_disp.horizontalSlider_labels_xOffset);
    s.restore(m_d->ui_disp.horizontalSlider_labels_yOffset);
    s.restore(m_d->ui_disp.horizontalSlider_labels_zOffset);
    s.restore(m_d->ui_disp.checkBox_labels_names);
  }  
  if (s.version()>=5){
    s.restore(m_d->ui_disp.checkBox_labels_mboyt0s);
    s.restore(m_d->ui_disp.checkBox_labels_mooret0s);
    s.restore(m_d->ui_disp.checkBox_labels_hits);
  }
  if (s.version()>=6){
    s.restore(m_d->ui_misc.checkBox_NSW_MM);
    s.restore(m_d->ui_misc.checkBox_NSW_sTGC);
  }

  s.ignoreWidget(m_d->ui_disp.matButton_lastSel);
  std::map<VP1GeoFlags::SubSystemFlag,QCheckBox*>::const_iterator it,itE(m_d->subSysCheckBoxMap.end());
  for (it=m_d->subSysCheckBoxMap.begin();it!=itE;++it)
    s.ignoreWidget(it->second);

  m_d->ui_int.groupBox_printOnSelection->setEnabled(m_d->ui_int.checkBox_geomselectable->isChecked());
  m_d->ui_int.groupBox_automaticViewReorientations->setEnabled(m_d->ui_int.checkBox_geomselectable->isChecked());

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



