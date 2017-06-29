/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class PRDSystemController               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1PRDSystems/PRDSystemController.h"
#include "ui_vp1prdcontrollerform.h"
#include "ui_prd_settings_cuts_form.h"
#include "ui_prd_settings_display_form.h"
#include "ui_prd_settings_interactions_form.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1CollectionWidget.h"
#include <Inventor/nodes/SoMaterial.h>
#include <QBuffer>
#include <QTimer>
#include <QSet>

//____________________________________________________________________
class PRDSystemController::Imp {
public:
  PRDSystemController * theclass;
  Ui::VP1PrdControllerForm ui;
  Ui::PRDSysSettingsInteractionsForm ui_int;
  Ui::PRDSysSettingsDisplayForm ui_disp;
  Ui::PRDSysSettingsCutsForm ui_cuts;
  VP1CollectionWidget * collWidget;

  SoMaterial * highlightmaterial;

  PRDCollHandleBase::COLOURMETHOD last_colourMethod;
  bool last_drawErrors;
  bool last_drawRDOs;
  bool last_highLightOutliers;
  double last_highLightMaterialWeight;
  bool last_highLightTRTHitsByHighThreshold;
  bool last_highLightMDTHitsByMask;
  int last_highLightMDTHitsByUpperADCBound;
  bool last_projectPixelHits;
  bool last_projectSCTHits;
  bool last_projectTRTHits;
  bool last_projectMDTHits;
  bool last_projectRPCHits;
  bool last_projectCSCHits;
  bool last_projectTGCHits;
  // bool last_projectSTGCHits;
  //Cuts:
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;//All off: empty list. All on: list with one entry: ]-inf,inf[
  PRDCommonFlags::InDetPartsFlags last_inDetPartsFlags;
  unsigned last_pixelMinNRDOPerCluster;
  bool last_sctExcludeIsolatedClusters;
  unsigned last_trtMinToT;
  unsigned last_trtMaxToT;
  unsigned last_trtMinLE;
  unsigned last_trtMaxLE;
  bool last_trtRequireHT;
  unsigned last_mdtMinNHitsPerStation;
  VP1Interval last_mdtAllowedADCValues;
  bool last_mdtExcludeMaskedHits;
  QString last_mdt_cutMdtDriftCircleStatus;
  std::set<PRDDetType::Type> last_shownCollectionTypes;
  bool last_limitToActiveChambers;
  bool last_selectionModeMultiple;
  bool last_showSelectionLine;
  //Used ID parts:
  InDetProjFlags::DetTypeFlags last_inDetPartsUsingProjections;

  QSet<PRDDetType::Type> shownCollectionTypes() const;
};

//____________________________________________________________________
PRDSystemController::PRDSystemController(IVP1System * sys)
  : VP1Controller(sys,"PRDSystemController"),d(new Imp)
{
  d->theclass = this;
  d->ui.setupUi(this);

  //FIXME: Dialogs and collwidget!
  d->collWidget = new VP1CollectionWidget;
  //   d->trackcollwidget = new TrackCollWidget;
  setupCollWidgetInScrollArea(d->ui.collWidgetScrollArea,d->collWidget);
  initDialog(d->ui_cuts, d->ui.pushButton_settings_cuts);
  initDialog(d->ui_disp, d->ui.pushButton_settings_display);
  initDialog(d->ui_int, d->ui.pushButton_settings_interactions);

  d->highlightmaterial = 0;

  d->ui_int.matButton_multiselline->setMaterial(VP1MaterialButton::createMaterial(1,1,0));;

  d->ui_cuts.groupBox_cuts_tracksegmentassociation->setVisible(false);//Since it is not used at the moment!
  if (!VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_ENABLEREFIT"))
    d->ui_int.groupBox_selectionMode->setVisible(false);

  d->ui_disp.widget_drawOptions_PixelSCT->setComplexityDisabled();
  d->ui_disp.widget_drawOptions_Muons->setPointSizesDisabled();
  d->ui_disp.widget_drawOptions_PixelSCT->setPointSizes(3.0);
  d->ui_disp.widget_drawOptions_TRT->setPointSizes(2.0);

  d->ui_cuts.etaPhiCutWidget->setEtaCutEnabled(false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  addUpdateSlot(SLOT(possibleChange_colourMethod()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_colourByTrack);
  connectToLastUpdateSlot(d->ui_disp.checkBox_colourBySegment);
  
  addUpdateSlot(SLOT(possibleChange_drawErrors()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_drawErrors);
  
  addUpdateSlot(SLOT(possibleChange_drawRDOs()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_drawRDOs);
  
  addUpdateSlot(SLOT(possibleChange_highLightOutliers()));
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightTRTHitsByHighThreshold()));
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightMDTHitsByMask()));
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightMDTHitsByUpperADCBound()));
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightMaterialWeight()));
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightOutliers()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_highlight_trackoutliers);
  connectToLastUpdateSlot(d->ui_disp.checkBox_colourByTrack);
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightTRTHitsByHighThreshold()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_highlight_trackoutliers);
  connectToLastUpdateSlot(d->ui_disp.checkBox_highlight_trtht);
  connectToLastUpdateSlot(d->ui_disp.checkBox_colourByTrack);
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightMDTHitsByMask()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_highlight_trackoutliers);
  connectToLastUpdateSlot(d->ui_disp.checkBox_highlight_maskedmdts);
  connectToLastUpdateSlot(d->ui_disp.checkBox_colourByTrack);
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightMDTHitsByUpperADCBound()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_highlight_trackoutliers);
  connectToLastUpdateSlot(d->ui_disp.checkBox_colourByTrack);
  connectToLastUpdateSlot(d->ui_disp.horizontalSlider_highlights_intensity);
  connectToLastUpdateSlot(d->ui_disp.checkBox_highlight_mdt_ADC_below);
  connectToLastUpdateSlot(d->ui_disp.spinBox_highlight_mdt_ADC_below);

  addUpdateSlot(SLOT(possibleChange_projectPixelHits()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showpixel_projected);

  addUpdateSlot(SLOT(possibleChange_projectSCTHits()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showsct_projected);

  addUpdateSlot(SLOT(possibleChange_projectTRTHits()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showtrt_projected);

  addUpdateSlot(SLOT(possibleChange_projectMDTHits()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showmdt_projected);

  addUpdateSlot(SLOT(possibleChange_projectRPCHits()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showrpc_projected);

  addUpdateSlot(SLOT(possibleChange_projectCSCHits()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showcsc_projected);

  addUpdateSlot(SLOT(possibleChange_projectTGCHits()));
  connectToLastUpdateSlot(d->ui_disp.checkBox_showtgc_projected);

  // addUpdateSlot(SLOT(possibleChange_projectSTGCHits()));
  // connectToLastUpdateSlot(d->ui_disp.checkBox_showstgc_projected);

  addUpdateSlot(SLOT(possibleChange_cutAllowedEta()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  addUpdateSlot(SLOT(possibleChange_cutAllowedPhi()));
  connectToLastUpdateSlot(d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  addUpdateSlot(SLOT(possibleChange_inDetPartsFlags()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_barrelA);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_barrelC);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_endcapA);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_indet_endcapC);

  addUpdateSlot(SLOT(possibleChange_pixelMinNRDOPerCluster()));
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_pixel_minnrdo);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_pixel_minnrdo);

  addUpdateSlot(SLOT(possibleChange_sctExcludeIsolatedClusters()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters);

  addUpdateSlot(SLOT(possibleChange_trtRequireHT()));
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_needHT);

  addUpdateSlot(SLOT(possibleChange_trtMinToT()));
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_TotMin);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_trt_ToTMin);

  addUpdateSlot(SLOT(possibleChange_trtMaxToT()));
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_TotMax);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_trt_ToTMax);

  addUpdateSlot(SLOT(possibleChange_trtMinLE()));
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_LeMin);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_trt_LEMin);

  addUpdateSlot(SLOT(possibleChange_trtMaxLE()));
  connectToLastUpdateSlot(d->ui_cuts.checkbox_cut_trt_LeMax);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_trt_LEMax);

  addUpdateSlot(SLOT(possibleChange_mdtMinNHitsPerStation()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_mdt_excludeisolateddrifttubes);

  addUpdateSlot(SLOT(possibleChange_mdtAllowedADCValues()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_mdt_ADClowerbound);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_mdt_ADClowerbound);
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_mdt_ADCupperbound);
  connectToLastUpdateSlot(d->ui_cuts.spinBox_cut_mdt_ADCupperbound);

  addUpdateSlot(SLOT(possibleChange_mdtExcludeMaskedHits()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_mdt_excludemaskedhits);
  
  addUpdateSlot(SLOT(possibleChange_mdt_cutMdtDriftCircleStatus()));
  connectToLastUpdateSlot(d->ui_cuts.comboBox_mdt_cutMdtDriftCircleStatus);
  
  addUpdateSlot(SLOT(possibleChange_limitToActiveChambers()));
  connectToLastUpdateSlot(d->ui_cuts.checkBox_cut_mdt_limitToActiveChambers);

  addUpdateSlot(SLOT(possibleChange_inDetPartsUsingProjections()));
  connectToLastUpdateSlot(this,SIGNAL(projectPixelHitsChanged(bool)));
  connectToLastUpdateSlot(this,SIGNAL(projectSCTHitsChanged(bool)));
  connectToLastUpdateSlot(this,SIGNAL(projectTRTHitsChanged(bool)));
  connectToLastUpdateSlot(this,SIGNAL(inDetPartsFlagsChanged(PRDCommonFlags::InDetPartsFlags)));
  connectToLastUpdateSlot(collWidget(),SIGNAL(visibleStdCollectionTypesChanged(const QList<qint32>&)));

  addUpdateSlot(SLOT(possibleChange_selectionModeMultiple()));
  connectToLastUpdateSlot(d->ui_int.checkBox_selModeMultiple);

  addUpdateSlot(SLOT(possibleChange_showSelectionLine()));
  connectToLastUpdateSlot(d->ui_int.checkBox_showSelectionLine);

  addUpdateSlot(SLOT(possibleChange_shownCollectionTypes()));
  connectToLastUpdateSlot(collWidget(),SIGNAL(visibleStdCollectionTypesChanged(const QList<qint32>&)));

  addUpdateSlot(SLOT(updateHighlightGui()));
  connectToLastUpdateSlot(this,SIGNAL(highLightOutliersChanged(bool)));

  //Stateless:
  connect(d->ui_int.pushButton_clearSelection,SIGNAL(clicked()),this,SLOT(emitClearSelection()));

  initLastVars();

}

//____________________________________________________________________
PRDSystemController::~PRDSystemController()
{
  if (d->highlightmaterial)
    d->highlightmaterial->unref();
  delete d;
}

//____________________________________________________________________
VP1CollectionWidget * PRDSystemController::collWidget() const
{
  return d->collWidget;
}

//____________________________________________________________________
SoGroup * PRDSystemController::drawOptions(PRDDetType::Type t) const
{
  //FIXME: SET APPROPRIATE DEFAULTS!!!!
  switch (t) {
  case PRDDetType::TRT:
    return d->ui_disp.widget_drawOptions_TRT->drawOptionsGroup();
  case PRDDetType::Pixel:
  case PRDDetType::SCT:
  case PRDDetType::SpacePoints:
    return d->ui_disp.widget_drawOptions_PixelSCT->drawOptionsGroup();
  default:
    return d->ui_disp.widget_drawOptions_Muons->drawOptionsGroup();
  }
}

//____________________________________________________________________
QSet<PRDDetType::Type> PRDSystemController::Imp::shownCollectionTypes() const
{
  QSet<PRDDetType::Type> s;
  foreach (qint32 i,collWidget->visibleStdCollectionTypes()) {
    bool ok;
    PRDDetType::Type t = PRDDetType::intToType(i,ok);
    if (ok)
      s.insert(t);
  }
  return s;
}

//____________________________________________________________________
SoMaterial * PRDSystemController::getMultiSelectionLineMaterial() const
{
  QList<SoMaterial*> mats = d->ui_int.matButton_multiselline->handledMaterials();
  if (mats.count()!=1) {
    message("ERROR: No material in button!");
    return 0;//probably giving a crash...
  }
  return mats.at(0);
}

//____________________________________________________________________
void PRDSystemController::emitClearSelection()
{
  messageVerbose("Emitting clearSelection()");
  emit clearSelection();
}

//____________________________________________________________________
SoMaterial * PRDSystemController::getHighLightMaterial()
{
  if (!d->highlightmaterial) {
    d->highlightmaterial = new SoMaterial;
    d->highlightmaterial->ref();
    d->highlightmaterial->diffuseColor.setValue( 1.0f, 0.0f, 0.0f );
    d->ui_disp.materialbutton_highlights_material->copyValuesFromMaterial(d->highlightmaterial);
    d->ui_disp.materialbutton_highlights_material->handleMaterial(d->highlightmaterial);
  }
  return d->highlightmaterial;
}

//____________________________________________________________________
bool PRDSystemController::printInfoOnClick() const
{
  return d->ui_int.checkBox_printinfo->isChecked();
}

//____________________________________________________________________
bool PRDSystemController::zoomOnClick() const
{
  return d->ui_int.checkBox_zoom->isChecked();
}

//____________________________________________________________________
bool PRDSystemController::muonOrientToChambersOnClick() const
{
  return zoomOnClick() && d->ui_int.checkBox_zoom_orientToMuonChamber->isChecked();
}

//____________________________________________________________________
VP1Interval PRDSystemController::cutAllowedEta() const
{
  return d->ui_cuts.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> PRDSystemController::cutAllowedPhi() const
{
  return d->ui_cuts.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
PRDCommonFlags::InDetPartsFlags PRDSystemController::inDetPartsFlags() const
{
  PRDCommonFlags::InDetPartsFlags flag = PRDCommonFlags::None;
  if (d->ui_cuts.checkBox_cut_indet_barrelA->isChecked()) flag |= PRDCommonFlags::BarrelPositive;
  if (d->ui_cuts.checkBox_cut_indet_barrelC->isChecked()) flag |= PRDCommonFlags::BarrelNegative;
  if (d->ui_cuts.checkBox_cut_indet_endcapA->isChecked()) flag |= PRDCommonFlags::EndCapPositive;
  if (d->ui_cuts.checkBox_cut_indet_endcapC->isChecked()) flag |= PRDCommonFlags::EndCapNegative;
  return flag;
}

//____________________________________________________________________
unsigned PRDSystemController::trtMinToT() const
{
  return d->ui_cuts.checkbox_cut_trt_TotMin->isChecked() ? d->ui_cuts.spinBox_cut_trt_ToTMin->value() : 0;
}

//____________________________________________________________________
unsigned PRDSystemController::trtMaxToT() const
{
  return d->ui_cuts.checkbox_cut_trt_TotMax->isChecked() ? d->ui_cuts.spinBox_cut_trt_ToTMax->value() : 24;
}

//____________________________________________________________________
unsigned PRDSystemController::trtMinLE() const
{
  return d->ui_cuts.checkbox_cut_trt_LeMin->isChecked() ? d->ui_cuts.spinBox_cut_trt_LEMin->value() : 0;
}

//____________________________________________________________________
unsigned PRDSystemController::trtMaxLE() const
{
  return d->ui_cuts.checkbox_cut_trt_LeMax->isChecked() ? d->ui_cuts.spinBox_cut_trt_LEMax->value() : 24;
}

//____________________________________________________________________
bool PRDSystemController::trtRequireHT() const
{
  return d->ui_cuts.checkbox_cut_trt_needHT->isChecked();
}


//____________________________________________________________________
unsigned PRDSystemController::pixelMinNRDOPerCluster() const
{
  return d->ui_cuts.checkBox_cut_pixel_minnrdo->isChecked() ? d->ui_cuts.spinBox_cut_pixel_minnrdo->value() : 0;
}



//____________________________________________________________________
bool PRDSystemController::sctExcludeIsolatedClusters() const
{
  return d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters->isChecked();
}

//____________________________________________________________________
unsigned PRDSystemController::mdtMinNHitsPerStation() const
{
  return d->ui_cuts.checkBox_cut_mdt_excludeisolateddrifttubes->isChecked() ? 2 : 0;
}

//____________________________________________________________________
VP1Interval PRDSystemController::mdtAllowedADCValues() const
{
  double lower = d->ui_cuts.checkBox_cut_mdt_ADClowerbound->isChecked() ?
    d->ui_cuts.spinBox_cut_mdt_ADClowerbound->value()+0.5 : - VP1Interval::inf();
  double upper = d->ui_cuts.checkBox_cut_mdt_ADCupperbound->isChecked() ?
    d->ui_cuts.spinBox_cut_mdt_ADCupperbound->value()-0.5 : VP1Interval::inf();
  return upper>lower? VP1Interval(lower,upper) : VP1Interval();
}


//____________________________________________________________________
bool PRDSystemController::mdtExcludeMaskedHits() const
{
  return d->ui_cuts.checkBox_cut_mdt_excludemaskedhits->isChecked();
}

//____________________________________________________________________
QString PRDSystemController::mdt_cutMdtDriftCircleStatus() const
{
  return d->ui_cuts.comboBox_mdt_cutMdtDriftCircleStatus->currentText();
}
                
//____________________________________________________________________
bool PRDSystemController::limitToActiveChambers() const
{
  return d->ui_cuts.checkBox_cut_mdt_limitToActiveChambers->isChecked();
}

//____________________________________________________________________
PRDCollHandleBase::COLOURMETHOD PRDSystemController::colourMethod() const
{
  if (d->ui_disp.checkBox_colourByTrack->isChecked()) {
    if (d->ui_disp.checkBox_colourBySegment->isChecked())
      return PRDCollHandleBase::BySegmentAndTrack;
    else
      return PRDCollHandleBase::ByTrack;
  } else {
    if (d->ui_disp.checkBox_colourBySegment->isChecked())
      return PRDCollHandleBase::BySegment;
    else
      return PRDCollHandleBase::ByTechOnly;
  }
}

//____________________________________________________________________
bool PRDSystemController::drawErrors() const
{
  if (d->ui_disp.checkBox_drawErrors->isChecked()) return true;
  return false;
}

//____________________________________________________________________
bool PRDSystemController::drawRDOs() const
{
  if (d->ui_disp.checkBox_drawRDOs->isChecked()) return true;
  return false;
}


//____________________________________________________________________
bool PRDSystemController::highLightOutliers() const
{
  return highLightMaterialWeight() != 0.0
    && d->ui_disp.checkBox_highlight_trackoutliers->isChecked() && d->ui_disp.checkBox_colourByTrack->isChecked();
}

//____________________________________________________________________
void PRDSystemController::updateHighlightGui()
{
  const bool outlierHLEnabledInGui(d->ui_disp.checkBox_highlight_trackoutliers->isChecked() && d->ui_disp.checkBox_colourByTrack->isChecked());
  d->ui_disp.checkBox_highlight_trtht->setEnabled(!outlierHLEnabledInGui);
  d->ui_disp.checkBox_highlight_maskedmdts->setEnabled(!outlierHLEnabledInGui);
  d->ui_disp.checkBox_highlight_mdt_ADC_below->setEnabled(!outlierHLEnabledInGui);
  d->ui_disp.spinBox_highlight_mdt_ADC_below->setEnabled(!outlierHLEnabledInGui);
}

//____________________________________________________________________
bool PRDSystemController::highLightTRTHitsByHighThreshold()
{
  return highLightMaterialWeight() != 0.0
    && !highLightOutliers() && d->ui_disp.checkBox_highlight_trtht->isChecked();
}


//____________________________________________________________________
bool PRDSystemController::highLightMDTHitsByMask()
{
  return highLightMaterialWeight() != 0.0
    && !highLightOutliers() && d->ui_disp.checkBox_highlight_maskedmdts->isChecked();
}


//____________________________________________________________________
int PRDSystemController::highLightMDTHitsByUpperADCBound()
{
  return (highLightMaterialWeight() > 0.0
	  && !highLightOutliers() && d->ui_disp.checkBox_highlight_mdt_ADC_below->isChecked() )
    ? d->ui_disp.spinBox_highlight_mdt_ADC_below->value() : -1;
}


//____________________________________________________________________
double PRDSystemController::highLightMaterialWeight() const
{
  int val = d->ui_disp.horizontalSlider_highlights_intensity->value();
  if (val<=1) return 0.0;
  if (val==2) return 1.0;
  if (val==3) return 1.9;
  if (val==4) return 2.8;
  if (val==5) return 5.0;
  return 99999.0;
}

//____________________________________________________________________
bool PRDSystemController::projectPixelHits() const { return d->ui_disp.checkBox_showpixel_projected->isChecked(); }
bool PRDSystemController::projectSCTHits() const { return d->ui_disp.checkBox_showsct_projected->isChecked(); }
bool PRDSystemController::projectTRTHits() const { return d->ui_disp.checkBox_showtrt_projected->isChecked(); }
bool PRDSystemController::projectMDTHits() const { return d->ui_disp.checkBox_showmdt_projected->isChecked(); }
bool PRDSystemController::projectRPCHits() const { return d->ui_disp.checkBox_showrpc_projected->isChecked(); }
bool PRDSystemController::projectCSCHits() const { return d->ui_disp.checkBox_showcsc_projected->isChecked(); }
bool PRDSystemController::projectTGCHits() const { return d->ui_disp.checkBox_showtgc_projected->isChecked(); }
// bool PRDSystemController::projectSTGCHits() const { return d->ui_disp.checkBox_showstgc_projected->isChecked(); }

//____________________________________________________________________
InDetProjFlags::DetTypeFlags PRDSystemController::inDetPartsUsingProjections() const
{
  std::set<PRDDetType::Type> showncolltypes =  shownCollectionTypes();
  PRDCommonFlags::InDetPartsFlags indetparts = inDetPartsFlags();

  InDetProjFlags::DetTypeFlags flag = InDetProjFlags::NoDet;

  if ( projectPixelHits() && showncolltypes.find(PRDDetType::Pixel)!=showncolltypes.end() ) {
    if (indetparts & PRDCommonFlags::BarrelPositive)
      flag |= InDetProjFlags::Pixel_brlpos;
    if (indetparts & PRDCommonFlags::BarrelNegative)
      flag |= InDetProjFlags::Pixel_brlneg;
    if (indetparts & PRDCommonFlags::EndCapPositive)
      flag |= InDetProjFlags::Pixel_ecpos;
    if (indetparts & PRDCommonFlags::EndCapNegative)
      flag |= InDetProjFlags::Pixel_ecneg;
  }

  if ( projectSCTHits() && showncolltypes.find(PRDDetType::SCT)!=showncolltypes.end() ) {
    if (indetparts & PRDCommonFlags::BarrelPositive)
      flag |= InDetProjFlags::SCT_brlpos;
    if (indetparts & PRDCommonFlags::BarrelNegative)
      flag |= InDetProjFlags::SCT_brlneg;
    if (indetparts & PRDCommonFlags::EndCapPositive)
      flag |= InDetProjFlags::SCT_ecpos;
    if (indetparts & PRDCommonFlags::EndCapNegative)
      flag |= InDetProjFlags::SCT_ecneg;
  }

  if ( projectTRTHits() && showncolltypes.find(PRDDetType::TRT)!=showncolltypes.end() ) {
     if (indetparts & PRDCommonFlags::BarrelPositive)
      flag |= InDetProjFlags::TRT_brlpos;
    if (indetparts & PRDCommonFlags::BarrelNegative)
      flag |= InDetProjFlags::TRT_brlneg;
    if (indetparts & PRDCommonFlags::EndCapPositive)
      flag |= InDetProjFlags::TRT_ecpos;
    if (indetparts & PRDCommonFlags::EndCapNegative)
      flag |= InDetProjFlags::TRT_ecneg;
  }

  return flag;
}

//____________________________________________________________________
bool PRDSystemController::selectionModeMultiple() const
{
  return d->ui_int.checkBox_selModeMultiple->isChecked();
}

//____________________________________________________________________
bool PRDSystemController::showSelectionLine() const
{
  return d->ui_int.checkBox_showSelectionLine->isChecked();
}

//____________________________________________________________________
std::set<PRDDetType::Type> PRDSystemController::shownCollectionTypes() const
{
  std::set<PRDDetType::Type> s;
  foreach(qint32 i, collWidget()->visibleStdCollectionTypes()) {
    bool ok;
    PRDDetType::Type t = PRDDetType::intToType(i, ok);
    if (ok)
      s.insert(t);
    else
      message("shownCollectionTypes ERROR: Could not decode collection type");
  }
  return s;
}


//____________________________________________________________________
int PRDSystemController::currentSettingsVersion() const
{
  return 3;
}

//____________________________________________________________________
void PRDSystemController::actualSaveSettings(VP1Serialise&s) const
{
  s.save(d->ui_disp.checkBox_colourByTrack);
  s.save(d->ui_disp.checkBox_colourBySegment);
  s.save(d->ui_disp.checkBox_highlight_trackoutliers);
  s.save(d->ui_disp.checkBox_highlight_trtht);
  s.save(d->ui_disp.checkBox_highlight_maskedmdts);
  s.save(d->ui_cuts.checkBox_cut_indet_barrelA);
  s.save(d->ui_cuts.checkBox_cut_indet_barrelC);
  s.save(d->ui_cuts.checkBox_cut_indet_endcapA);
  s.save(d->ui_cuts.checkBox_cut_indet_endcapC);
  s.save(d->ui_cuts.checkBox_cut_pixel_minnrdo);
  s.save(d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters);
  s.save(d->ui_cuts.checkbox_cut_trt_needHT);
  s.save(d->ui_cuts.checkbox_cut_trt_TotMin);
  s.save(d->ui_cuts.checkbox_cut_trt_TotMax);
  s.save(d->ui_cuts.checkBox_cut_mdt_excludeisolateddrifttubes);
  s.save(d->ui_disp.checkBox_highlight_mdt_ADC_below);
  s.save(d->ui_cuts.checkBox_cut_mdt_excludemaskedhits);
  s.save(d->ui_cuts.checkBox_cut_mdt_ADClowerbound);
  s.save(d->ui_cuts.checkBox_cut_mdt_ADCupperbound);
  s.save(d->ui_disp.checkBox_showpixel_projected);
  s.save(d->ui_disp.checkBox_showsct_projected);
  s.save(d->ui_disp.checkBox_showtrt_projected);
  s.save(d->ui_disp.checkBox_showmdt_projected);
  s.save(d->ui_disp.checkBox_showrpc_projected);
  s.save(d->ui_disp.checkBox_showcsc_projected);
  s.save(d->ui_disp.checkBox_showtgc_projected);
  // s.save(d->ui_disp.checkBox_showstgc_projected);
  s.save(d->ui_disp.horizontalSlider_highlights_intensity);
  s.save(d->ui_cuts.spinBox_cut_pixel_minnrdo);
  s.save(d->ui_cuts.spinBox_cut_trt_ToTMin);
  s.save(d->ui_cuts.spinBox_cut_trt_ToTMax);
  s.save(d->ui_disp.spinBox_highlight_mdt_ADC_below);
  s.save(d->ui_cuts.spinBox_cut_mdt_ADClowerbound);
  s.save(d->ui_cuts.spinBox_cut_mdt_ADCupperbound);
  s.save(d->ui_disp.materialbutton_highlights_material);
  s.save(d->ui_int.checkBox_selModeMultiple);
  s.save(d->ui_int.checkBox_showSelectionLine);
  s.save(d->ui_int.matButton_multiselline);
  s.save(d->ui_cuts.etaPhiCutWidget);
  s.save(d->ui_disp.widget_drawOptions_PixelSCT);
  s.save(d->ui_disp.widget_drawOptions_TRT);
  s.save(d->ui_disp.widget_drawOptions_Muons);
  s.save(d->ui_int.checkBox_printinfo);
  s.save(d->ui_int.checkBox_zoom);
  s.save(d->ui_int.checkBox_zoom_orientToMuonChamber);

  s.save(d->ui_cuts.spinBox_cut_trt_LEMin);//Version 1+
  s.save(d->ui_cuts.spinBox_cut_trt_LEMax);//Version 1+
  s.save(d->ui_cuts.checkbox_cut_trt_LeMin);//Version 1+
  s.save(d->ui_cuts.checkbox_cut_trt_LeMax);//Version 1+

  s.save(d->ui_disp.checkBox_drawErrors);//Version 2+
  s.save(d->ui_disp.checkBox_drawRDOs);//Version 2+

  s.save(d->ui_cuts.comboBox_mdt_cutMdtDriftCircleStatus);//Version 3+
  s.save(d->ui_cuts.checkBox_cut_mdt_limitToActiveChambers);//Version 3+

  //Not used yet:
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_track);
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_hits_not_on_track);
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_segments);
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_not_on_segments);

}


//____________________________________________________________________
void PRDSystemController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>3) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }
  s.restore(d->ui_disp.checkBox_colourByTrack);
  s.restore(d->ui_disp.checkBox_colourBySegment);
  s.restore(d->ui_disp.checkBox_highlight_trackoutliers);
  s.restore(d->ui_disp.checkBox_highlight_trtht);
  s.restore(d->ui_disp.checkBox_highlight_maskedmdts);
  s.restore(d->ui_cuts.checkBox_cut_indet_barrelA);
  s.restore(d->ui_cuts.checkBox_cut_indet_barrelC);
  s.restore(d->ui_cuts.checkBox_cut_indet_endcapA);
  s.restore(d->ui_cuts.checkBox_cut_indet_endcapC);
  s.restore(d->ui_cuts.checkBox_cut_pixel_minnrdo);
  s.restore(d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters);
  s.restore(d->ui_cuts.checkbox_cut_trt_needHT);
  s.restore(d->ui_cuts.checkbox_cut_trt_TotMin);
  s.restore(d->ui_cuts.checkbox_cut_trt_TotMax);
  s.restore(d->ui_cuts.checkBox_cut_mdt_excludeisolateddrifttubes);
  s.restore(d->ui_disp.checkBox_highlight_mdt_ADC_below);
  s.restore(d->ui_cuts.checkBox_cut_mdt_excludemaskedhits);
  s.restore(d->ui_cuts.checkBox_cut_mdt_ADClowerbound);
  s.restore(d->ui_cuts.checkBox_cut_mdt_ADCupperbound);
  s.restore(d->ui_disp.checkBox_showpixel_projected);
  s.restore(d->ui_disp.checkBox_showsct_projected);
  s.restore(d->ui_disp.checkBox_showtrt_projected);
  s.restore(d->ui_disp.checkBox_showmdt_projected);
  s.restore(d->ui_disp.checkBox_showrpc_projected);
  s.restore(d->ui_disp.checkBox_showcsc_projected);
  s.restore(d->ui_disp.checkBox_showtgc_projected);
  // s.restore(d->ui_disp.checkBox_showstgc_projected);
  s.restore(d->ui_disp.horizontalSlider_highlights_intensity);
  s.restore(d->ui_cuts.spinBox_cut_pixel_minnrdo);
  s.restore(d->ui_cuts.spinBox_cut_trt_ToTMin);
  s.restore(d->ui_cuts.spinBox_cut_trt_ToTMax);
  s.restore(d->ui_disp.spinBox_highlight_mdt_ADC_below);
  s.restore(d->ui_cuts.spinBox_cut_mdt_ADClowerbound);
  s.restore(d->ui_cuts.spinBox_cut_mdt_ADCupperbound);
  s.restore(d->ui_disp.materialbutton_highlights_material);
  s.restore(d->ui_int.checkBox_selModeMultiple);
  s.restore(d->ui_int.checkBox_showSelectionLine);
  s.restore(d->ui_int.matButton_multiselline);
  s.restore(d->ui_cuts.etaPhiCutWidget);
  s.restore(d->ui_disp.widget_drawOptions_PixelSCT);
  s.restore(d->ui_disp.widget_drawOptions_TRT);
  s.restore(d->ui_disp.widget_drawOptions_Muons);
  s.restore(d->ui_int.checkBox_printinfo);
  s.restore(d->ui_int.checkBox_zoom);
  s.restore(d->ui_int.checkBox_zoom_orientToMuonChamber);

  if (s.version()>=1) {
    s.restore(d->ui_cuts.spinBox_cut_trt_LEMin);
    s.restore(d->ui_cuts.spinBox_cut_trt_LEMax);
    s.restore(d->ui_cuts.checkbox_cut_trt_LeMin);
    s.restore(d->ui_cuts.checkbox_cut_trt_LeMax);
  }

  if (s.version()>=2) {
    s.restore(d->ui_disp.checkBox_drawErrors);
    s.restore(d->ui_disp.checkBox_drawRDOs);
  }
  
  if (s.version()>=3) {
    s.restore(d->ui_cuts.comboBox_mdt_cutMdtDriftCircleStatus);
    s.restore(d->ui_cuts.checkBox_cut_mdt_limitToActiveChambers);
  }
  
  //Not used yet:
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_track);
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_hits_not_on_track);
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_segments);
  s.ignoreWidget(d->ui_cuts.checkBox_cuts_ts_exclude_not_on_segments);
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME PRDSystemController
#include "VP1Base/VP1ControllerMacros.h"

//The actual code for each variable:
POSSIBLECHANGE_IMP(projectPixelHits)
POSSIBLECHANGE_IMP(projectSCTHits)
POSSIBLECHANGE_IMP(projectTRTHits)
POSSIBLECHANGE_IMP(projectMDTHits)
POSSIBLECHANGE_IMP(projectRPCHits)
POSSIBLECHANGE_IMP(projectCSCHits)
POSSIBLECHANGE_IMP(projectTGCHits)
// POSSIBLECHANGE_IMP(projectSTGCHits)
POSSIBLECHANGE_IMP(highLightMaterialWeight)
POSSIBLECHANGE_IMP(inDetPartsUsingProjections)
POSSIBLECHANGE_IMP(cutAllowedEta)
POSSIBLECHANGE_IMP(cutAllowedPhi)
POSSIBLECHANGE_IMP(inDetPartsFlags)
POSSIBLECHANGE_IMP(trtMinToT)
POSSIBLECHANGE_IMP(trtMaxToT)
POSSIBLECHANGE_IMP(trtMinLE)
POSSIBLECHANGE_IMP(trtMaxLE)
POSSIBLECHANGE_IMP(trtRequireHT)
POSSIBLECHANGE_IMP(pixelMinNRDOPerCluster)
POSSIBLECHANGE_IMP(sctExcludeIsolatedClusters)
POSSIBLECHANGE_IMP(mdtMinNHitsPerStation)
POSSIBLECHANGE_IMP(mdtAllowedADCValues)
POSSIBLECHANGE_IMP(mdtExcludeMaskedHits)
POSSIBLECHANGE_IMP(mdt_cutMdtDriftCircleStatus)
POSSIBLECHANGE_IMP(shownCollectionTypes)
POSSIBLECHANGE_IMP(limitToActiveChambers)
POSSIBLECHANGE_IMP(selectionModeMultiple)
POSSIBLECHANGE_IMP(showSelectionLine)
POSSIBLECHANGE_IMP(colourMethod)
POSSIBLECHANGE_IMP(highLightOutliers)
POSSIBLECHANGE_IMP(drawErrors)
POSSIBLECHANGE_IMP(drawRDOs)
POSSIBLECHANGE_IMP(highLightTRTHitsByHighThreshold)
POSSIBLECHANGE_IMP(highLightMDTHitsByMask)
POSSIBLECHANGE_IMP(highLightMDTHitsByUpperADCBound)
