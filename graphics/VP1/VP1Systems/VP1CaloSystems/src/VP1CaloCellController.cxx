/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1CaloSystems/VP1CaloCellController.h"
#include "VP1CaloSystems/VP1CaloCells.h"
#include "ui_calocellcontrollerform.h"
//#include "ui_calodigitssingle.h" // FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//#include "ui_calodigitsdouble.h" // FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
#include "ui_calocell_badchannels_form.h"
#include "ui_calocell_mbtsthresholds_form.h"
#include "ui_calocell_tilethresholds_form.h"
#include "ui_calocell_larsimpthresholds_form.h"
#include "ui_calocell_larexpthresholds_form.h"
#include "ui_calocell_interactions_form.h"
#include "ui_calocell_cuts_form.h"
#include "ui_calocell_visoptions_form.h"

#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Serialise.h"

#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/Exception.h"

/*
 * FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_text.h>
*/

#include <QVector>

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>

// -------------------- Imp -------------------------------
class VP1CaloCellController::Imp
{
public:
	// ---------- Data members ------------
	VP1CaloCellController* theclass;
	Ui::frmCaloCellController ui;
	Ui::frmCaloCellVisOptions ui_visopts;
	Ui::frmCaloCellBadChannels ui_badchans;
	Ui::frmCaloCellInteractions ui_interact;
	Ui::frmCaloCellCuts ui_cuts;
	Ui::frmCaloCellThresholdsMbts ui_threshmbts;
	Ui::frmCaloCellThresholdsTile ui_threshtile;
	Ui::frmCaloCellThresholdsLArSimple ui_threshlarsimp;
	Ui::frmCaloCellThresholdsLArExpert ui_threshlarexp;

	// This is needed by VP1Controller base class
	VP1CCIntervalMap last_selectionIntervals;
	QPair<bool,double> last_scale;
	VP1Interval last_selectionMbts;
	bool last_showDigits;
	bool last_showVolumeOutLines;
	bool last_energyModeEt;
	VP1CC_GlobalCuts last_globalCuts;

	// Map of gui object structures by selection type (simple/expert modes)
	VP1CCSelectionType2GuiMap sel2GuiSimple;
	VP1CCSelectionType2GuiMap sel2GuiExpert;

	// Material button map
	typedef std::map<VP1CC_SeparatorTypes, VP1MaterialButton* > MatButtonMap;
	MatButtonMap matButtonMap;

	// Digits display
	QWidget*                   digit_form_single;
	QWidget*                   digit_form_double;

//	Ui::frmCaloDigitsSingle*   UiDigitsSingle; // FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	Ui::frmCaloDigitsDouble*   UiDigitsDouble; // FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...

	// at the beginning of every event reset these flags to FALSE
	bool singlePlotHasData;
	bool doublePlotHasData;
};

// -------------------- Controller -------------------------------
VP1CaloCellController::VP1CaloCellController(IVP1System* sys):
		  VP1Controller(sys,"CaloCellController"),
		  d(new Imp),
		  tile_hw_id(0),
		  tile_info(0),
		  tile_cabling(0)
{

	messageDebug("VP1CaloCellController() constructor...");

	d->theclass = this;
	d->ui.setupUi(this);

	// Setup dialogs
	initDialog(d->ui_interact, d->ui.pushButton_settings_interactions);
	initDialog(d->ui_cuts, d->ui.pushButton_settings_cuts);
	initDialog(d->ui_badchans, d->ui.pushButton_settings_badchannels);
	initDialog(d->ui_visopts, d->ui.pushButton_settings_visoptions);
	initDialog(d->ui_threshmbts,d->ui.pushButton_settings_thresholds_mbts);
	initDialog(d->ui_threshtile,d->ui.pushButton_settings_thresholds_tile);
	initDialog(d->ui_threshlarsimp,d->ui.pushButton_settings_thresholds_larsimple,d->ui.rbtnSimple);
	initDialog(d->ui_threshlarexp,d->ui.pushButton_settings_thresholds_larexpert,d->ui.rbtnExpert);

	d->ui_cuts.wdgEtaPhiCut->setEtaCutEnabled(false);

	d->ui_visopts.wdgDrawOptions->setLineWidths(2.0);
	d->ui_visopts.wdgDrawOptions->setPointSizesDisabled();
	d->ui_visopts.wdgDrawOptions->setBaseLightingDisabled();
	d->ui_visopts.wdgDrawOptions->setComplexityDisabled();

	// Initialize flags
	d->singlePlotHasData = false;
	d->doublePlotHasData = false;

	// ------------------------ SLOTS ----------------------

	// ----------------- Selection Intervals ------------------
	addUpdateSlot(SLOT(possibleChange_selectionIntervals()));

	// Enable/disable checkboxes
	connectToLastUpdateSlot(d->ui.chbxEMB);
	connectToLastUpdateSlot(d->ui.chbxEMEC);
	connectToLastUpdateSlot(d->ui.chbxHEC);
	connectToLastUpdateSlot(d->ui.chbxFCAL);
	connectToLastUpdateSlot(d->ui.chbxEMB_S0);
	connectToLastUpdateSlot(d->ui.chbxEMB_S1);
	connectToLastUpdateSlot(d->ui.chbxEMB_S2);
	connectToLastUpdateSlot(d->ui.chbxEMB_S3);
	connectToLastUpdateSlot(d->ui.chbxEMEC_S0);
	connectToLastUpdateSlot(d->ui.chbxEMEC_S1);
	connectToLastUpdateSlot(d->ui.chbxEMEC_S2);
	connectToLastUpdateSlot(d->ui.chbxEMEC_S3);
	connectToLastUpdateSlot(d->ui.chbxHEC_S0);
	connectToLastUpdateSlot(d->ui.chbxHEC_S1);
	connectToLastUpdateSlot(d->ui.chbxHEC_S2);
	connectToLastUpdateSlot(d->ui.chbxHEC_S3);
	connectToLastUpdateSlot(d->ui.chbxFCAL_M1);
	connectToLastUpdateSlot(d->ui.chbxFCAL_M2);
	connectToLastUpdateSlot(d->ui.chbxFCAL_M3);
	connectToLastUpdateSlot(d->ui.chbxTILB);
	connectToLastUpdateSlot(d->ui.chbxTILEC);
	connectToLastUpdateSlot(d->ui.chbxTILCR);
	// Negative checkboxes
	connectToLastUpdateSlot(d->ui.chbxLArNegative);
	connectToLastUpdateSlot(d->ui.chbxEMB_Neg_Expert);
	connectToLastUpdateSlot(d->ui.chbxEMEC_Neg_Expert);
	connectToLastUpdateSlot(d->ui.chbxHEC_Neg_Expert);
	connectToLastUpdateSlot(d->ui.chbxFCAL_Neg_Expert);
	connectToLastUpdateSlot(d->ui.chbxTileNegative);

	// Disable upper thresholds checkboxes
	connectToLastUpdateSlot(d->ui_threshlarsimp.chbxNoLArUpper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S0Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S3Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S0Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S3Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	connectToLastUpdateSlot(d->ui_threshtile.chbxNoTileUpper);

	// Lower threhold spinboxes
	connectToLastUpdateSlot(d->ui_threshlarsimp.dspbxLArLowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S0LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S1LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S2LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S3LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S0LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S1LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S2LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S3LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S0LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S1LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S2LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S3LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxFCAL_M1LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxFCAL_M2LowerThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxFCAL_M3LowerThresh);
	connectToLastUpdateSlot(d->ui_threshtile.dspbxTileLowerThresh);

	// Upper threhold spinboxes
	connectToLastUpdateSlot(d->ui_threshlarsimp.dspbxLArUpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S0UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S1UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S2UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMB_S3UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S0UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S1UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S2UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxEMEC_S3UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S0UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S1UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S2UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxHEC_S3UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxFCAL_M1UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxFCAL_M2UpperThresh);
	connectToLastUpdateSlot(d->ui_threshlarexp.dspbxFCAL_M3UpperThresh);
	connectToLastUpdateSlot(d->ui_threshtile.dspbxTileUpperThresh);

	// LAr mode change radio button
	connectToLastUpdateSlot(d->ui.rbtnSimple);

	// ----------------- Selection Intervals ------------------

	// ----------------- Global cuts ------------------
	addUpdateSlot(SLOT(possibleChange_globalCuts()));
	connectToLastUpdateSlot(d->ui_cuts.chbxSideA);
	connectToLastUpdateSlot(d->ui_cuts.chbxSideC);
	connectToLastUpdateSlot(d->ui_cuts.wdgEtaPhiCut,SIGNAL(allowedEtaChanged(const VP1Interval&)));
	connectToLastUpdateSlot(d->ui_cuts.wdgEtaPhiCut,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));
	// ----------------- Global cuts ------------------

	// -------------- Scale ------------
	addUpdateSlot(SLOT(possibleChange_scale()));
	connectToLastUpdateSlot(d->ui_visopts.dspbxScale);
	connectToLastUpdateSlot(d->ui_visopts.chbxLogscale);
	// -------------- Scale ------------

	// ---------------- Enable Upper Threshold ----------------
	addUpdateSlot(SLOT(enableUpperThreshold()));
	connectToLastUpdateSlot(d->ui_threshlarsimp.chbxNoLArUpper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S0Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMB_S3Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S0Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoHEC_S3Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	connectToLastUpdateSlot(d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	connectToLastUpdateSlot(d->ui_threshtile.chbxNoTileUpper);

	// ----------------- LAr mode ------------------
	addUpdateSlot(SLOT(changeMode()));
	connectToLastUpdateSlot(d->ui.rbtnSimple);

	// ----------------- MBTS ---------------------
	addUpdateSlot(SLOT(possibleChange_selectionMbts()));
	connectToLastUpdateSlot(d->ui.chbxMbts);
	connectToLastUpdateSlot(d->ui_threshmbts.dspbxMbtsThresh);

	// ------------- Digits checkbox ----------------
	addUpdateSlot(SLOT(possibleChange_showDigits()));
	connectToLastUpdateSlot(d->ui_interact.chbxDigits);
	// ------------- Digits checkbox ----------------

	// ------------- Volume OutLines -------------
	addUpdateSlot(SLOT(possibleChange_showVolumeOutLines()));
	connectToLastUpdateSlot(d->ui_visopts.chbxShowOutlines);
	// ------------- Volume OutLines -------------

	// ------------- Energy Mode -------------
	addUpdateSlot(SLOT(possibleChange_energyModeEt()));
	connectToLastUpdateSlot(d->ui.rbtnModeEt);
	// ------------- Energy Mode -------------

	initLastVars();
	// ------------------------ SLOTS ----------------------

	// ---------------------- Material Buttons --------------------
	d->ui_visopts.matbutLArEMBColorPos->setMaterial(VP1MaterialButton::createMaterial(0.21,0.8,0.51));
	d->ui_visopts.matbutLArEMECColorPos->setMaterial(VP1MaterialButton::createMaterial(0.26,0.8,0.2));
	d->ui_visopts.matbutLArHECColorPos->setMaterial(VP1MaterialButton::createMaterial(0.25,0.8,0.76));
	d->ui_visopts.matbutLArFCALColorPos->setMaterial(VP1MaterialButton::createMaterial(0.09,0.98,0.02));
	d->ui_visopts.matbutLArEMBColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	d->ui_visopts.matbutLArEMECColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	d->ui_visopts.matbutLArHECColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	d->ui_visopts.matbutLArFCALColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	d->ui_visopts.matbutTileColorPosPMT1->setMaterial(VP1MaterialButton::createMaterial(1.,1.,0.));
	d->ui_visopts.matbutTileColorPosPMT0->setMaterial(VP1MaterialButton::createMaterial(1.,0.78,0.));
	d->ui_visopts.matbutTileColorPosNeg->setMaterial(VP1MaterialButton::createMaterial(1.,0.,0.));
	d->ui_visopts.matbutTileColorNegPMT1->setMaterial(VP1MaterialButton::createMaterial(1.,0.,1.));
	d->ui_visopts.matbutTileColorNegPMT0->setMaterial(VP1MaterialButton::createMaterial(0.78,0.,1.));
	d->ui_visopts.matbutTileColorNegPos->setMaterial(VP1MaterialButton::createMaterial(1.,0.,0.));
	d->ui_visopts.matbutMbts->setMaterial(VP1MaterialButton::createMaterial(1.,1.,0.));

	d->matButtonMap[VP1CC_SepLArEMBPos] = d->ui_visopts.matbutLArEMBColorPos;
	d->matButtonMap[VP1CC_SepLArEMECPos] = d->ui_visopts.matbutLArEMECColorPos;
	d->matButtonMap[VP1CC_SepLArHECPos] = d->ui_visopts.matbutLArHECColorPos;
	d->matButtonMap[VP1CC_SepLArFCALPos] = d->ui_visopts.matbutLArFCALColorPos;
	d->matButtonMap[VP1CC_SepLArEMBNeg] = d->ui_visopts.matbutLArEMBColorNeg;
	d->matButtonMap[VP1CC_SepLArEMECNeg] = d->ui_visopts.matbutLArEMECColorNeg;
	d->matButtonMap[VP1CC_SepLArHECNeg] = d->ui_visopts.matbutLArHECColorNeg;
	d->matButtonMap[VP1CC_SepLArFCALNeg] = d->ui_visopts.matbutLArFCALColorNeg;
	d->matButtonMap[VP1CC_SepTilePositiveUp] = d->ui_visopts.matbutTileColorPosPMT1;
	d->matButtonMap[VP1CC_SepTilePositiveDown] = d->ui_visopts.matbutTileColorPosPMT0;
	d->matButtonMap[VP1CC_SepTilePositiveNeg] = d->ui_visopts.matbutTileColorPosNeg;
	d->matButtonMap[VP1CC_SepTileNegativeUp] = d->ui_visopts.matbutTileColorNegPMT1;
	d->matButtonMap[VP1CC_SepTileNegativeDown] = d->ui_visopts.matbutTileColorNegPMT0;
	d->matButtonMap[VP1CC_SepTileNegativePos] = d->ui_visopts.matbutTileColorNegPos;
	d->matButtonMap[VP1CC_SepMBTS] = d->ui_visopts.matbutMbts;
	// ---------------------- Material Buttons --------------------

	// --------------------- Digits Display -------------------------
	messageDebug("Digits Display...");
	d->digit_form_single = new QWidget(0,Qt::WindowStaysOnTopHint);
	d->digit_form_single->setVisible(false);

	d->digit_form_double = new QWidget(0,Qt::WindowStaysOnTopHint);
	d->digit_form_double->setVisible(false);

//	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	messageDebug("setting up digits box (s)");
//	d->UiDigitsSingle = new Ui::frmCaloDigitsSingle();
//	d->UiDigitsSingle->setupUi(d->digit_form_single);
//	d->UiDigitsSingle->plotDigits->setAxisTitle(QwtPlot::xBottom,"Bunch crossing");
//	d->UiDigitsSingle->plotDigits->setAxisTitle(QwtPlot::yLeft,"ADC counts");
//
	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	messageDebug("setting up digits box (d)");
//	d->UiDigitsDouble = new Ui::frmCaloDigitsDouble();
//	d->UiDigitsDouble->setupUi(d->digit_form_double);
//	d->UiDigitsDouble->plotDigits_1->setAxisTitle(QwtPlot::xBottom,"Bunch crossing");
//	d->UiDigitsDouble->plotDigits_1->setAxisTitle(QwtPlot::yLeft,"ADC counts");
//	d->UiDigitsDouble->plotDigits_2->setAxisTitle(QwtPlot::xBottom,"Bunch crossing");
//	d->UiDigitsDouble->plotDigits_2->setAxisTitle(QwtPlot::yLeft,"ADC counts");
	// --------------------- Digits Display -------------------------

	// Fill Sel2Gui maps
	VP1CCUi2Manager* ui2manager;
	// EMB
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMB;
	ui2manager->showNegativeCB = d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarsimp.dspbxLArUpperThresh;

	(d->sel2GuiSimple)[VP1CC_SelTypeEMB0] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeEMB1] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeEMB2] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeEMB3] = ui2manager;

	// EMEC
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMEC;
	ui2manager->showNegativeCB = d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarsimp.dspbxLArUpperThresh;

	(d->sel2GuiSimple)[VP1CC_SelTypeEMEC0] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeEMEC1] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeEMEC2] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeEMEC3] = ui2manager;

	// HEC
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxHEC;
	ui2manager->showNegativeCB = d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarsimp.dspbxLArUpperThresh;

	(d->sel2GuiSimple)[VP1CC_SelTypeHEC0] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeHEC1] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeHEC2] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeHEC3] = ui2manager;

	// FCAL
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxFCAL;
	ui2manager->showNegativeCB = d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarsimp.dspbxLArUpperThresh;

	(d->sel2GuiSimple)[VP1CC_SelTypeFCAL1] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeFCAL2] = ui2manager;
	(d->sel2GuiSimple)[VP1CC_SelTypeFCAL3] = ui2manager;

	// ---VP1CC_SelTypeEMB0 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMB_S0;
	ui2manager->showNegativeCB = d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMB_S0Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMB_S0LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMB_S0UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMB0] = ui2manager;

	// ---VP1CC_SelTypeEMB1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMB_S1;
	ui2manager->showNegativeCB = d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMB_S1Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMB_S1LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMB_S1UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMB1] = ui2manager;

	// --- VP1CC_SelTypeEMB2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMB_S2;
	ui2manager->showNegativeCB = d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMB_S2Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMB_S2LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMB_S2UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMB2] = ui2manager;

	// --- VP1CC_SelTypeEMB3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMB_S3;
	ui2manager->showNegativeCB = d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMB_S3Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMB_S3LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMB_S3UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMB3] = ui2manager;

	// --- VP1CC_SelTypeEMEC0 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMEC_S0;
	ui2manager->showNegativeCB = d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMEC_S0Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMEC_S0LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMEC_S0UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMEC0] = ui2manager;

	// --- VP1CC_SelTypeEMEC1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMEC_S1;
	ui2manager->showNegativeCB = d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMEC_S1Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMEC_S1LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMEC_S1UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMEC1] = ui2manager;

	// --- VP1CC_SelTypeEMEC2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMEC_S2;
	ui2manager->showNegativeCB = d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMEC_S2Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMEC_S2LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMEC_S2UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMEC2] = ui2manager;

	// --- VP1CC_SelTypeEMEC3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxEMEC_S3;
	ui2manager->showNegativeCB = d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoEMEC_S3Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxEMEC_S3LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxEMEC_S3UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeEMEC3] = ui2manager;

	// --- VP1CC_SelTypeHEC0 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxHEC_S0;
	ui2manager->showNegativeCB = d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoHEC_S0Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxHEC_S0LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxHEC_S0UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeHEC0] = ui2manager;

	// --- VP1CC_SelTypeHEC1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxHEC_S1;
	ui2manager->showNegativeCB = d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoHEC_S1Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxHEC_S1LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxHEC_S1UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeHEC1] = ui2manager;

	// --- VP1CC_SelTypeHEC2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxHEC_S2;
	ui2manager->showNegativeCB = d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoHEC_S2Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxHEC_S2LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxHEC_S2UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeHEC2] = ui2manager;

	// --- VP1CC_SelTypeHEC3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxHEC_S3;
	ui2manager->showNegativeCB = d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoHEC_S3Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxHEC_S3LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxHEC_S3UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeHEC3] = ui2manager;

	// --- VP1CC_SelTypeFCAL1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxFCAL_M1;
	ui2manager->showNegativeCB = d->ui.chbxFCAL_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoFCAL_M1Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxFCAL_M1LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxFCAL_M1UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeFCAL1] = ui2manager;

	// --- VP1CC_SelTypeFCAL2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxFCAL_M2;
	ui2manager->showNegativeCB = d->ui.chbxFCAL_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoFCAL_M2Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxFCAL_M2LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxFCAL_M2UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeFCAL2] = ui2manager;

	// --- VP1CC_SelTypeFCAL3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxFCAL_M3;
	ui2manager->showNegativeCB = d->ui.chbxFCAL_Neg_Expert;
	ui2manager->upperThreshOffCB = d->ui_threshlarexp.chbxNoFCAL_M3Upper;
	ui2manager->lowerThresholdSB = d->ui_threshlarexp.dspbxFCAL_M3LowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshlarexp.dspbxFCAL_M3UpperThresh;
	(d->sel2GuiExpert)[VP1CC_SelTypeFCAL3] = ui2manager;

	// --- VP1CC_SelTypeTileB ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxTILB;
	ui2manager->showNegativeCB = d->ui.chbxTileNegative;
	ui2manager->upperThreshOffCB = d->ui_threshtile.chbxNoTileUpper;
	ui2manager->lowerThresholdSB = d->ui_threshtile.dspbxTileLowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshtile.dspbxTileUpperThresh;
	(d->sel2GuiSimple)[VP1CC_SelTypeTileB] = ui2manager;
	(d->sel2GuiExpert)[VP1CC_SelTypeTileB] = ui2manager;

	// --- VP1CC_SelTypeTileEC ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxTILEC;
	ui2manager->showNegativeCB = d->ui.chbxTileNegative;
	ui2manager->upperThreshOffCB = d->ui_threshtile.chbxNoTileUpper;
	ui2manager->lowerThresholdSB = d->ui_threshtile.dspbxTileLowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshtile.dspbxTileUpperThresh;
	(d->sel2GuiSimple)[VP1CC_SelTypeTileEC] = ui2manager;
	(d->sel2GuiExpert)[VP1CC_SelTypeTileEC] = ui2manager;

	// --- VP1CC_SelTypeTileCrack ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = d->ui.chbxTILCR;
	ui2manager->showNegativeCB = d->ui.chbxTileNegative;
	ui2manager->upperThreshOffCB = d->ui_threshtile.chbxNoTileUpper;
	ui2manager->lowerThresholdSB = d->ui_threshtile.dspbxTileLowerThresh;
	ui2manager->upperThresholdSB = d->ui_threshtile.dspbxTileUpperThresh;
	(d->sel2GuiSimple)[VP1CC_SelTypeTileCrack] = ui2manager;
	(d->sel2GuiExpert)[VP1CC_SelTypeTileCrack] = ui2manager;
}

VP1CaloCellController::~VP1CaloCellController()
{
	// Clean up in Imp maps
	delete (d->sel2GuiSimple)[VP1CC_SelTypeEMB0];
	delete (d->sel2GuiSimple)[VP1CC_SelTypeEMEC0];
	delete (d->sel2GuiSimple)[VP1CC_SelTypeHEC0];
	delete (d->sel2GuiSimple)[VP1CC_SelTypeFCAL1];
	//TK: Fixme: why not just iterate and delete all here?
	d->sel2GuiSimple.clear();

	for(VP1CCSelectionType2GuiMap::iterator it=d->sel2GuiExpert.begin(); it!=d->sel2GuiExpert.end(); it++)
		delete it->second;
	d->sel2GuiExpert.clear();
	delete d;
}

void VP1CaloCellController::actualRestoreSettings(VP1Deserialise& s)
{
	if (s.version()<0 || s.version()>4) {
		message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
		return;
	}

	s.restore(d->ui_interact.chbxDigits);
	s.restore(d->ui_visopts.dspbxScale);
	s.restore(d->ui.chbxMbts);
	s.restore(d->ui_threshmbts.dspbxMbtsThresh);
	s.restore(d->ui_badchans.chbxEnableBadChannels);
	s.restore(d->ui_badchans.chbxMaskDead);
	s.restore(d->ui_badchans.chbxMaskNoisy);
	s.restore(d->ui_badchans.chbxMaskAffected);
	s.restore(d->ui.rbtnSimple,d->ui.rbtnExpert);
	s.restore(d->ui.chbxEMB);
	s.restore(d->ui.chbxEMEC);
	s.restore(d->ui.chbxHEC);
	s.restore(d->ui.chbxFCAL);
	s.restore(d->ui.chbxLArNegative);
	s.restore(d->ui_threshlarsimp.dspbxLArLowerThresh);
	s.restore(d->ui_threshlarsimp.dspbxLArUpperThresh);
	s.restore(d->ui_threshlarsimp.chbxNoLArUpper);
	s.restore(d->ui.chbxEMB_Neg_Expert);
	s.restore(d->ui.chbxEMB_S0);
	s.restore(d->ui_threshlarexp.dspbxEMB_S0LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMB_S0UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMB_S0Upper);
	s.restore(d->ui.chbxEMB_S1);
	s.restore(d->ui_threshlarexp.dspbxEMB_S1LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMB_S1UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMB_S1Upper);
	s.restore(d->ui.chbxEMB_S2);
	s.restore(d->ui_threshlarexp.dspbxEMB_S2LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMB_S2UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMB_S2Upper);
	s.restore(d->ui.chbxEMB_S3);
	s.restore(d->ui_threshlarexp.dspbxEMB_S3LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMB_S3UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMB_S3Upper);
	s.restore(d->ui.chbxEMEC_Neg_Expert);
	s.restore(d->ui.chbxEMEC_S0);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S0LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S0UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	s.restore(d->ui.chbxEMEC_S1);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S1LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S1UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	s.restore(d->ui.chbxEMEC_S2);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S2LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S2UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	s.restore(d->ui.chbxEMEC_S3);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S3LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxEMEC_S3UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	s.restore(d->ui.chbxHEC_Neg_Expert);
	s.restore(d->ui.chbxHEC_S0);
	s.restore(d->ui_threshlarexp.dspbxHEC_S0LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxHEC_S0UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoHEC_S0Upper);
	s.restore(d->ui.chbxHEC_S1);
	s.restore(d->ui_threshlarexp.dspbxHEC_S1LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxHEC_S1UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoHEC_S1Upper);
	s.restore(d->ui.chbxHEC_S2);
	s.restore(d->ui_threshlarexp.dspbxHEC_S2LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxHEC_S2UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoHEC_S2Upper);
	s.restore(d->ui.chbxHEC_S3);
	s.restore(d->ui_threshlarexp.dspbxHEC_S3LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxHEC_S3UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoHEC_S3Upper);
	s.restore(d->ui.chbxFCAL_Neg_Expert);
	s.restore(d->ui.chbxFCAL_M1);
	s.restore(d->ui_threshlarexp.dspbxFCAL_M1LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxFCAL_M1UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	s.restore(d->ui.chbxFCAL_M2);
	s.restore(d->ui_threshlarexp.dspbxFCAL_M2LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxFCAL_M2UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	s.restore(d->ui.chbxFCAL_M3);
	s.restore(d->ui_threshlarexp.dspbxFCAL_M3LowerThresh);
	s.restore(d->ui_threshlarexp.dspbxFCAL_M3UpperThresh);
	s.restore(d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	s.restore(d->ui.chbxTILB);
	s.restore(d->ui.chbxTILEC);
	s.restore(d->ui.chbxTILCR);
	s.restore(d->ui.chbxTileNegative);
	s.restore(d->ui_threshtile.dspbxTileLowerThresh);
	s.restore(d->ui_threshtile.dspbxTileUpperThresh);
	s.restore(d->ui_threshtile.chbxNoTileUpper);
	s.restore(d->ui_visopts.matbutLArEMBColorPos);
	s.restore(d->ui_visopts.matbutLArEMBColorNeg);
	s.restore(d->ui_visopts.matbutLArEMECColorPos);
	s.restore(d->ui_visopts.matbutLArEMECColorNeg);
	s.restore(d->ui_visopts.matbutLArHECColorPos);
	s.restore(d->ui_visopts.matbutLArHECColorNeg);
	s.restore(d->ui_visopts.matbutLArFCALColorPos);
	s.restore(d->ui_visopts.matbutLArFCALColorNeg);
	s.restore(d->ui_visopts.matbutTileColorPosPMT0);
	s.restore(d->ui_visopts.matbutTileColorPosPMT1);
	s.restore(d->ui_visopts.matbutTileColorPosNeg);
	s.restore(d->ui_visopts.matbutTileColorNegPMT0);
	s.restore(d->ui_visopts.matbutTileColorNegPMT1);
	s.restore(d->ui_visopts.matbutTileColorNegPos);
	s.restore(d->ui_visopts.matbutMbts);

	if(s.version()>0) {
		s.restore(d->ui_visopts.wdgDrawOptions);
		s.restore(d->ui_visopts.chbxShowOutlines);
	}

	if(s.version()>1) {
		s.restore(d->ui.rbtnModeEt,d->ui.rbtnModeEne);
	}

	if(s.version()>2) {
		s.restore(d->ui_visopts.chbxLogscale);
	}

	if(s.version()>3) {
		s.restore(d->ui_cuts.chbxSideA);
		s.restore(d->ui_cuts.chbxSideC);
		s.restore(d->ui_cuts.wdgEtaPhiCut);
	}
}

int VP1CaloCellController::currentSettingsVersion() const
{
	return 4;
}

void VP1CaloCellController::actualSaveSettings(VP1Serialise& s) const
{
	s.save(d->ui_interact.chbxDigits);
	s.save(d->ui_visopts.dspbxScale);
	s.save(d->ui.chbxMbts);
	s.save(d->ui_threshmbts.dspbxMbtsThresh);
	s.save(d->ui_badchans.chbxEnableBadChannels);
	s.save(d->ui_badchans.chbxMaskDead);
	s.save(d->ui_badchans.chbxMaskNoisy);
	s.save(d->ui_badchans.chbxMaskAffected);
	s.save(d->ui.rbtnSimple,d->ui.rbtnExpert);
	s.save(d->ui.chbxEMB);
	s.save(d->ui.chbxEMEC);
	s.save(d->ui.chbxHEC);
	s.save(d->ui.chbxFCAL);
	s.save(d->ui.chbxLArNegative);
	s.save(d->ui_threshlarsimp.dspbxLArLowerThresh);
	s.save(d->ui_threshlarsimp.dspbxLArUpperThresh);
	s.save(d->ui_threshlarsimp.chbxNoLArUpper);
	s.save(d->ui.chbxEMB_Neg_Expert);
	s.save(d->ui.chbxEMB_S0);
	s.save(d->ui_threshlarexp.dspbxEMB_S0LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMB_S0UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMB_S0Upper);
	s.save(d->ui.chbxEMB_S1);
	s.save(d->ui_threshlarexp.dspbxEMB_S1LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMB_S1UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMB_S1Upper);
	s.save(d->ui.chbxEMB_S2);
	s.save(d->ui_threshlarexp.dspbxEMB_S2LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMB_S2UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMB_S2Upper);
	s.save(d->ui.chbxEMB_S3);
	s.save(d->ui_threshlarexp.dspbxEMB_S3LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMB_S3UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMB_S3Upper);
	s.save(d->ui.chbxEMEC_Neg_Expert);
	s.save(d->ui.chbxEMEC_S0);
	s.save(d->ui_threshlarexp.dspbxEMEC_S0LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMEC_S0UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	s.save(d->ui.chbxEMEC_S1);
	s.save(d->ui_threshlarexp.dspbxEMEC_S1LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMEC_S1UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	s.save(d->ui.chbxEMEC_S2);
	s.save(d->ui_threshlarexp.dspbxEMEC_S2LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMEC_S2UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	s.save(d->ui.chbxEMEC_S3);
	s.save(d->ui_threshlarexp.dspbxEMEC_S3LowerThresh);
	s.save(d->ui_threshlarexp.dspbxEMEC_S3UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	s.save(d->ui.chbxHEC_Neg_Expert);
	s.save(d->ui.chbxHEC_S0);
	s.save(d->ui_threshlarexp.dspbxHEC_S0LowerThresh);
	s.save(d->ui_threshlarexp.dspbxHEC_S0UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoHEC_S0Upper);
	s.save(d->ui.chbxHEC_S1);
	s.save(d->ui_threshlarexp.dspbxHEC_S1LowerThresh);
	s.save(d->ui_threshlarexp.dspbxHEC_S1UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoHEC_S1Upper);
	s.save(d->ui.chbxHEC_S2);
	s.save(d->ui_threshlarexp.dspbxHEC_S2LowerThresh);
	s.save(d->ui_threshlarexp.dspbxHEC_S2UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoHEC_S2Upper);
	s.save(d->ui.chbxHEC_S3);
	s.save(d->ui_threshlarexp.dspbxHEC_S3LowerThresh);
	s.save(d->ui_threshlarexp.dspbxHEC_S3UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoHEC_S3Upper);
	s.save(d->ui.chbxFCAL_Neg_Expert);
	s.save(d->ui.chbxFCAL_M1);
	s.save(d->ui_threshlarexp.dspbxFCAL_M1LowerThresh);
	s.save(d->ui_threshlarexp.dspbxFCAL_M1UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	s.save(d->ui.chbxFCAL_M2);
	s.save(d->ui_threshlarexp.dspbxFCAL_M2LowerThresh);
	s.save(d->ui_threshlarexp.dspbxFCAL_M2UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	s.save(d->ui.chbxFCAL_M3);
	s.save(d->ui_threshlarexp.dspbxFCAL_M3LowerThresh);
	s.save(d->ui_threshlarexp.dspbxFCAL_M3UpperThresh);
	s.save(d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	s.save(d->ui.chbxTILB);
	s.save(d->ui.chbxTILEC);
	s.save(d->ui.chbxTILCR);
	s.save(d->ui.chbxTileNegative);
	s.save(d->ui_threshtile.dspbxTileLowerThresh);
	s.save(d->ui_threshtile.dspbxTileUpperThresh);
	s.save(d->ui_threshtile.chbxNoTileUpper);
	s.save(d->ui_visopts.matbutLArEMBColorPos);
	s.save(d->ui_visopts.matbutLArEMBColorNeg);
	s.save(d->ui_visopts.matbutLArEMECColorPos);
	s.save(d->ui_visopts.matbutLArEMECColorNeg);
	s.save(d->ui_visopts.matbutLArHECColorPos);
	s.save(d->ui_visopts.matbutLArHECColorNeg);
	s.save(d->ui_visopts.matbutLArFCALColorPos);
	s.save(d->ui_visopts.matbutLArFCALColorNeg);
	s.save(d->ui_visopts.matbutTileColorPosPMT0);
	s.save(d->ui_visopts.matbutTileColorPosPMT1);
	s.save(d->ui_visopts.matbutTileColorPosNeg);
	s.save(d->ui_visopts.matbutTileColorNegPMT0);
	s.save(d->ui_visopts.matbutTileColorNegPMT1);
	s.save(d->ui_visopts.matbutTileColorNegPos);
	s.save(d->ui_visopts.matbutMbts);
	s.save(d->ui_visopts.wdgDrawOptions); // 1+
	s.save(d->ui_visopts.chbxShowOutlines); // 1+
	s.save(d->ui.rbtnModeEt,d->ui.rbtnModeEne); // 2+
	s.save(d->ui_visopts.chbxLogscale); // 3+
	s.save(d->ui_cuts.chbxSideA); // 4+
	s.save(d->ui_cuts.chbxSideC); // 4+
	s.save(d->ui_cuts.wdgEtaPhiCut); // 4+
}

SoGroup* VP1CaloCellController::drawOptions() const
{
	return d->ui_visopts.wdgDrawOptions->drawOptionsGroup();
}

void VP1CaloCellController::initTilePulse(const TileHWID* _tile_hw_id,
		const TileInfo* _tile_info,
		const TileCablingService* _tile_cabling)
{
	tile_hw_id = _tile_hw_id;
	tile_info = _tile_info;
	tile_cabling = _tile_cabling;
}

void VP1CaloCellController::ClearHideDigitForms()
{
	messageDebug("ClearHideDigitForms()");
	d->singlePlotHasData = false;
	d->digit_form_single->setVisible(false);
	d->doublePlotHasData = false;
	d->digit_form_double->setVisible(false);
}

void VP1CaloCellController::DeleteDigitForms()
{
	messageDebug("DeleteDigitForms()");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
	/*
	if (d) {
		messageDebug("delete digit_form_single...");
		if (d->digit_form_single) {
			delete d->digit_form_single;
			d->digit_form_single = 0;
		}
		messageDebug("delete digit_form_double...");
		if (d->digit_form_double) {
			delete d->digit_form_double;
			d->digit_form_double = 0;
		}
		messageDebug("delete UiDigitsSingle...");
		if (d->UiDigitsSingle) {
			delete d->UiDigitsSingle;
			d->UiDigitsSingle = 0;
		}
		messageDebug("delete UiDigitsDouble...");
		if (d->UiDigitsDouble) {
			delete d->UiDigitsDouble;
			d->UiDigitsDouble = 0;
		}
		messageDebug("DeleteDigitForms(). End.");
	}
    */
}

void VP1CaloCellController::EnableDigitsCheckbox(bool enable)
{
	d->ui_interact.chbxDigits->setEnabled(enable);
}

void VP1CaloCellController::displayLArDigits(
		int /* n_samples */,
		std::vector<short> /* samples */,
		std::vector<std::string>& /* msg */)
{
	message("VP1CaloCellController::displayLArDigits()... not be ported to 'master' yet, because of failing dependencies (LCG's Qwt has to be ported to Qt5, still)");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
/*

	QwtPlot* plot = d->UiDigitsSingle->plotDigits;

	// Pop up the widget
	d->digit_form_single->setVisible(true);
	d->digit_form_double->setVisible(false);

	// Get data
	QVector<QPointF> _data;
	for(int ii=0; ii<n_samples;ii++) {
		_data.push_back(QPointF(ii,samples[ii]));
	}

	// Create symbol
	QwtSymbol* sym = new QwtSymbol();
	sym->setStyle(QwtSymbol::Star1);
//	sym->setPen(QColor(Qt::darkMagenta));
	sym->setPen(QColor(Qt::red));
	sym->setSize(5);

	// Create curve
	QwtPlotCurve* _curve = new QwtPlotCurve("ADC Counts");
	_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
	_curve->setSamples(_data); // builds QwtSeriesData objects from arrays internally
	_curve->setStyle(QwtPlotCurve::NoCurve);
	_curve->setPen(QPen(Qt::red));
	_curve->setSymbol(sym);

	// Attach the curve to the hosting plot
	//	std::cout << "plot: " << d->UiDigitsSingle->plotDigits << std::endl;
	_curve->attach(plot);

	// Set the canvas background color
	plot->setCanvasBackground( QBrush(Qt::white) );

	// Refresh the plot
//	plot->replot();
	d->UiDigitsSingle->plotDigits->replot();


	// Set values for the labels
	d->UiDigitsSingle->lblCellName->setText((msg[1]).c_str());
	d->UiDigitsSingle->lblTotalEnergy->setText((msg[3]).c_str());
	d->UiDigitsSingle->lblTotalTime->setText((msg[4]).c_str());

	// hide pmt/gain label
	d->UiDigitsSingle->lblPMTGain->setVisible(false);

	// set the flag for the plot data
	d->singlePlotHasData = true;
	d->doublePlotHasData = false;
    */

	messageDebug("display LAr digits end.");
}

void VP1CaloCellController::displayTileDigits(int n_samples,
		std::vector<float> samples1,
		std::vector<float> samples2,
		std::vector<std::string>& msg)
{

	messageDebug("VP1CaloCellController::displayTileDigits(A)...");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
	/*
	// Pop up the widget
	d->digit_form_double->setVisible(true);
	d->digit_form_single->setVisible(false);

	messageDebug("Setting text...");
	d->UiDigitsDouble->lblCellName->setText(msg[1].c_str());
	d->UiDigitsDouble->lblTotalEnergy->setText(msg[2].c_str());
	d->UiDigitsDouble->lblTotalTime->setText(msg[3].c_str());


	messageDebug("Calling DrawTileDigits()...");
	DrawTileDigits(d->UiDigitsDouble->plotDigits_1,n_samples,samples1);
	messageDebug("Called on plotDigits_1.");
	DrawTileDigits(d->UiDigitsDouble->plotDigits_2,n_samples,samples2);
	messageDebug("Called on plotDigits_2.");

	// set the flag for the plot data
	d->doublePlotHasData = true;
	d->singlePlotHasData = false;
	*/
}

void VP1CaloCellController::displayTileDigits(int n_samples,
		std::vector<float> samples,
		std::vector<std::string>& msg)
{
	messageDebug("VP1CaloCellController::displayTileDigits(B)...");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
	/*
	// Pop up the widget
	d->digit_form_single->setVisible(true);
	d->digit_form_double->setVisible(false);

	messageDebug("Setting text...");
	d->UiDigitsSingle->lblCellName->setText(msg[1].c_str());
	d->UiDigitsSingle->lblTotalEnergy->setText(msg[2].c_str());
	d->UiDigitsSingle->lblTotalTime->setText(msg[3].c_str());

	// show pmt/gain label
	messageDebug("Show pmt/gain...");
	d->UiDigitsSingle->lblPMTGain->setVisible(true);

	messageDebug("calling DrawTileDigits()...");
	DrawTileDigits(d->UiDigitsSingle->plotDigits,n_samples,samples);
	messageDebug("called DrawTileDigits().");

	// set the flag for the plot data
	d->doublePlotHasData = true;
	d->singlePlotHasData = false;
	*/
}

void VP1CaloCellController::displayTilePulse(const TileRawChannel* rawchannel1,
		const TileRawChannel* rawchannel2,
		const TileRawChannelContainer* rawchannel_cont,
		size_t digitsize)
{
	messageDebug("VP1CaloCellController::displayTilePulse(A)...");

	//	std::cout << ""
	//			<< "a:" << d->UiDigitsDouble->plotDigits_1 << " - "
	//			<< "b:" << d->UiDigitsDouble->lblPMTGain_1 << " - "
	//			<< "c:" << rawchannel1 << " - "
	//			<< "d:" << rawchannel_cont << " - "
	//			<< "e:" << d->UiDigitsDouble->plotDigits_2 << " - "
	//			<< "f:" << d->UiDigitsDouble->lblPMTGain_2 << " - "
	//			<< "g:" << rawchannel2 << " - "
	//			<< "h:" << digitsize
	//			<< std::endl;


	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	DrawTilePulse(d->UiDigitsDouble->plotDigits_1,
//			d->UiDigitsDouble->lblPMTGain_1,
//			rawchannel1,
//			rawchannel_cont,
//			digitsize);
//	std::cout << "Drawn digits_1" << std::endl;
//
//	DrawTilePulse(d->UiDigitsDouble->plotDigits_2,
//			d->UiDigitsDouble->lblPMTGain_2,
//			rawchannel2,
//			rawchannel_cont,
//			digitsize);
//	std::cout << "Drawn digits_2" << std::endl;
}

void VP1CaloCellController::displayTilePulse(const TileRawChannel* rawchannel,
		const TileRawChannelContainer* rawchannel_cont,
		size_t digitsize)
{
	messageDebug("VP1CaloCellController::displayTilePulse(B)...");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	DrawTilePulse(d->UiDigitsSingle->plotDigits,
//			d->UiDigitsSingle->lblPMTGain,
//			rawchannel,
//			rawchannel_cont,
//			digitsize);

	messageDebug("displayTilePulse(). End.");
}

SoMaterial* VP1CaloCellController::GetMaterial(VP1CC_SeparatorTypes type)
{
	Imp::MatButtonMap::const_iterator it = d->matButtonMap.find(type);
	if(it!=d->matButtonMap.end())
		return getMaterial(it->second);
	else
		return 0;
}

VP1CCIntervalMap VP1CaloCellController::selectionIntervals() const
{
	// Which sel2Gui map are we working with? This depends on the active mode (simple/expert)
	VP1CCSelectionType2GuiMap* useMap = 0;

	if(d->ui.rbtnSimple->isChecked())
		useMap = &(d->sel2GuiSimple);
	else
		useMap = &(d->sel2GuiExpert);

	VP1CCIntervalMap returnMap;

	// Construct a new map of intervals based on values of GUI widgets
	for(VP1CCSelectionType2GuiMap::iterator it=useMap->begin(); it!=useMap->end(); it++) {
		// Pointer to the current structure
		VP1CCUi2Manager* _ui = it->second;

		VP1Interval _posInterval, _negInterval;
		bool _positiveON = true;
		bool _negativeON = true;

		// If subsystem is unchecked set pos/neg OFF
		if(!_ui->globalEnableCB->isChecked()) {
			_positiveON = false;
			_negativeON = false;
		}

		// If negative is unchecked set neg OFF
		if(!_ui->showNegativeCB->isChecked())
			_negativeON = false;

		// If upper threshold is available and lower>upper set pos/neg OFF
		if(!_ui->upperThreshOffCB->isChecked() &&
				_ui->lowerThresholdSB->value() > _ui->upperThresholdSB->value()){
			_positiveON = false;
			_negativeON = false;
		}

		// Construct new interval for positive cells
		if(_positiveON){
			if(_ui->upperThreshOffCB->isChecked())
				_posInterval.setUpper(VP1Interval::inf());
			else {
				_posInterval.setUpper(_ui->upperThresholdSB->value());
				_posInterval.setOpenUpper(false);
			}
			_posInterval.setLower(_ui->lowerThresholdSB->value());
			_posInterval.setOpenLower(false);
		}

		// We use the same interval for negative cells as well if they are ON
		if(_negativeON)
			_negInterval = _posInterval;

		returnMap[it->first] = VP1CCIntervalPair(_posInterval,_negInterval);
	}

	return returnMap;
}

QPair<bool,double> VP1CaloCellController::scale() const
{
	double scl = d->ui_visopts.chbxLogscale->isChecked() ? d->ui_visopts.dspbxScale->value()*CLHEP::m/log(1+10*CLHEP::GeV) : d->ui_visopts.dspbxScale->value()*CLHEP::m/(10*CLHEP::GeV);
	return QPair<bool,double>(d->ui_visopts.chbxLogscale->isChecked(),scl);
}

VP1Interval VP1CaloCellController::selectionMbts() const
{
	VP1Interval returnInterval;
	if(d->ui.chbxMbts->isChecked()) {
		returnInterval.setUpper(VP1Interval::inf());
		returnInterval.setLower(d->ui_threshmbts.dspbxMbtsThresh->value());
		returnInterval.setOpenLower(false);
	}

	return returnInterval;
}

bool VP1CaloCellController::showDigits() const
{
	return d->ui_interact.chbxDigits->isChecked();
}

bool VP1CaloCellController::showVolumeOutLines() const
{
	return d->ui_visopts.chbxShowOutlines->isChecked();
}

bool VP1CaloCellController::energyModeEt() const
{
	return d->ui.rbtnModeEt->isChecked();
}

VP1CC_GlobalCuts VP1CaloCellController::globalCuts() const
{
	VP1CC_GlobalCuts globalCuts;
	globalCuts.sideA = d->ui_cuts.chbxSideA->isChecked();
	globalCuts.sideC = d->ui_cuts.chbxSideC->isChecked();
	globalCuts.allowedEta = d->ui_cuts.wdgEtaPhiCut->allowedEta();
	globalCuts.allowedPhi = d->ui_cuts.wdgEtaPhiCut->allowedPhi();
  globalCuts.clipRadius = 10e9; 
  // no easy way to get the 'current' value since we don't know where in the AnimationSequence we are. Need to rely on this being updated by connections.

	return globalCuts;
}

// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//void VP1CaloCellController::DrawTileDigits(QwtPlot* plot,
//		int n_samples,
//		std::vector<float> samples)
//{
//	messageDebug("VP1CaloCellController::DrawTileDigits()...");
//
//	// Get data
//	QVector<QPointF> _data;
//	for(int ii=0; ii<n_samples;ii++) {
//		_data.push_back(QPointF(ii,samples[ii]));
//	}
//
//	// Create symbol
//	QwtSymbol* sym = new QwtSymbol();
//	sym->setStyle(QwtSymbol::Star1);
//	sym->setPen(QColor(Qt::red));
//	sym->setSize(5);
//
//	// Create curve
//	QwtPlotCurve* _curve = new QwtPlotCurve("ADC Counts");
//	_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
//	_curve->setSamples(_data);
//	_curve->setStyle(QwtPlotCurve::NoCurve);
//	_curve->setPen(QPen(Qt::red));
//	_curve->setSymbol(sym);
//
//	// Attach curve to the hosting plot
//	_curve->attach(plot);
//
//	// Set the canvas background color
//	plot->setCanvasBackground( QBrush(Qt::white) );
//
//	// Refresh the plot
//	plot->replot();
//}
//


// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//void VP1CaloCellController::DrawTilePulse(QwtPlot* plot,
//		QLabel* gainlabel,
//		const TileRawChannel* rawchannel,
//		const TileRawChannelContainer* rawchannel_cont,
//		size_t digitsize)
//{
//	messageDebug("VP1CaloCellController::DrawTilePulse()...");
//
//	// convert amplitude
//	double _amplitude1 = rawchannel->amplitude();
//	HWIdentifier adc_hwid1 = rawchannel->adc_HWID();
//
//	std::ostringstream pmtgain1;
//
////	std::cout << "DEBUG - "
////			<< "_amplitude1: " <<_amplitude1 << " - "
////			<< "adc_hwid1: " << adc_hwid1 << " - "
////			<< "tile_hw_id ros: " << tile_hw_id->ros(adc_hwid1) << " - "
////			<< "tile_hw_id channel: " << tile_hw_id->channel(adc_hwid1)
////			<< std::endl;
//
//	pmtgain1 << " PMT " << tile_cabling->channel2hole(tile_hw_id->ros(adc_hwid1),tile_hw_id->channel(adc_hwid1))
//			   << "   Gain " << tile_hw_id->adc(adc_hwid1);
//
//	gainlabel->setText(pmtgain1.str().c_str());
//
//	std::cout << "DEBUG - Getting TileInfo..." << std::endl;
//	try {
//		_amplitude1 /= tile_info->ChannelCalib(adc_hwid1,
//				TileRawChannelUnit::ADCcounts,
//				rawchannel_cont->get_unit(),
//				rawchannel_cont->get_type());
//	} catch (TileCalib::InvalidRawChanUnit& e) {
//		std::cout << "ERROR!!! Exception caught from Tile! --> " << e.what() << std::endl;
//		std::cout << "re-throwing it..." << std::endl;
//		throw e;
//	}
//
//
//
//	// Get correct vector of values depending on the gain
//	std::vector<double> *xval, *yval;
//	if(tile_hw_id->adc(adc_hwid1) == 0)
//	{
//		xval = &(tile_info->getPulseShapes()->m_tlphys);
//		yval = &(tile_info->getPulseShapes()->m_ylphys);
//	}
//	else if(tile_hw_id->adc(adc_hwid1) == 1)
//	{
//		xval = &(tile_info->getPulseShapes()->m_thphys);
//		yval = &(tile_info->getPulseShapes()->m_yhphys);
//	}
//	else
//		return;
//
//	// Prepare data for plotting
//	QVector<QPointF> _data;
//	for(size_t i=0; i<xval->size(); i++) {
//		double xpoint = ((*xval)[i] + rawchannel->time())/25. + (digitsize - 1)/2.;
//		double ypoint = rawchannel->pedestal() + _amplitude1*(*yval)[i];
//
//		if(xpoint >= 0 && xpoint <= digitsize -1)
//			_data.push_back(QPointF(xpoint,ypoint));
//	}
//
//	// Create curve
//	QwtPlotCurve* _curve = new QwtPlotCurve("Pulse");
//	_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
//	_curve->setSamples(_data);
//	_curve->setStyle(QwtPlotCurve::Dots);
//	_curve->setPen(QPen(Qt::blue));
//
//	// Attach the curve to the hosting plot
//	_curve->attach(plot);
//
//	// Refresh the plot
//	plot->replot();
//}

// --------------------- SLOTS ------------------------
void VP1CaloCellController::enableUpperThreshold()
{
	// Which sel2Gui map are we working with? This depends on the active mode (simple/expert)
	VP1CCSelectionType2GuiMap* useMap = 0;

	if(d->ui.rbtnSimple->isChecked())
		useMap = &(d->sel2GuiSimple);
	else
		useMap = &(d->sel2GuiExpert);

	for(VP1CCSelectionType2GuiMap::iterator it=useMap->begin(); it!=useMap->end(); it++) {
		VP1CCUi2Manager* _ui = it->second;
		_ui->upperThresholdSB->setEnabled(!_ui->upperThreshOffCB->isChecked());
	}
}

void VP1CaloCellController::changeMode()
{
	if(d->ui.rbtnSimple->isChecked()) {
		d->ui.stackLAr->setCurrentIndex(0);
		d->ui.pushButton_settings_thresholds_larsimple->setEnabled(true);
		d->ui.pushButton_settings_thresholds_larexpert->setEnabled(false);
	}
	else {
		d->ui.stackLAr->setCurrentIndex(1);
		d->ui.pushButton_settings_thresholds_larsimple->setEnabled(false);
		d->ui.pushButton_settings_thresholds_larexpert->setEnabled(true);
	}
}

#define VP1CONTROLLERCLASSNAME VP1CaloCellController
#include "VP1Base/VP1ControllerMacros.h"

POSSIBLECHANGE_IMP(selectionIntervals)
POSSIBLECHANGE_IMP(scale)
POSSIBLECHANGE_IMP(selectionMbts)
POSSIBLECHANGE_IMP(showDigits)
POSSIBLECHANGE_IMP(showVolumeOutLines)
POSSIBLECHANGE_IMP(energyModeEt)
POSSIBLECHANGE_IMP(globalCuts)
