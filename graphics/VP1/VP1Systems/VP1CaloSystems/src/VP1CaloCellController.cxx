/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define VP1IMPVARNAME m_d

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
#include "GaudiKernel/SystemOfUnits.h"
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
		  m_d(new Imp),
		  m_tile_hw_id(0),
		  m_tile_info(0),
		  m_tile_cabling(0)
{

	messageDebug("VP1CaloCellController() constructor...");

	m_d->theclass = this;
	m_d->ui.setupUi(this);

	// Setup dialogs
	initDialog(m_d->ui_interact, m_d->ui.pushButton_settings_interactions);
	initDialog(m_d->ui_cuts, m_d->ui.pushButton_settings_cuts);
	initDialog(m_d->ui_badchans, m_d->ui.pushButton_settings_badchannels);
	initDialog(m_d->ui_visopts, m_d->ui.pushButton_settings_visoptions);
	initDialog(m_d->ui_threshmbts,m_d->ui.pushButton_settings_thresholds_mbts);
	initDialog(m_d->ui_threshtile,m_d->ui.pushButton_settings_thresholds_tile);
	initDialog(m_d->ui_threshlarsimp,m_d->ui.pushButton_settings_thresholds_larsimple,m_d->ui.rbtnSimple);
	initDialog(m_d->ui_threshlarexp,m_d->ui.pushButton_settings_thresholds_larexpert,m_d->ui.rbtnExpert);

	m_d->ui_cuts.wdgEtaPhiCut->setEtaCutEnabled(false);

	m_d->ui_visopts.wdgDrawOptions->setLineWidths(2.0);
	m_d->ui_visopts.wdgDrawOptions->setPointSizesDisabled();
	m_d->ui_visopts.wdgDrawOptions->setBaseLightingDisabled();
	m_d->ui_visopts.wdgDrawOptions->setComplexityDisabled();

	// Initialize flags
	m_d->singlePlotHasData = false;
	m_d->doublePlotHasData = false;

	// ------------------------ SLOTS ----------------------

	// ----------------- Selection Intervals ------------------
	addUpdateSlot(SLOT(possibleChange_selectionIntervals()));

	// Enable/disable checkboxes
	connectToLastUpdateSlot(m_d->ui.chbxEMB);
	connectToLastUpdateSlot(m_d->ui.chbxEMEC);
	connectToLastUpdateSlot(m_d->ui.chbxHEC);
	connectToLastUpdateSlot(m_d->ui.chbxFCAL);
	connectToLastUpdateSlot(m_d->ui.chbxEMB_S0);
	connectToLastUpdateSlot(m_d->ui.chbxEMB_S1);
	connectToLastUpdateSlot(m_d->ui.chbxEMB_S2);
	connectToLastUpdateSlot(m_d->ui.chbxEMB_S3);
	connectToLastUpdateSlot(m_d->ui.chbxEMEC_S0);
	connectToLastUpdateSlot(m_d->ui.chbxEMEC_S1);
	connectToLastUpdateSlot(m_d->ui.chbxEMEC_S2);
	connectToLastUpdateSlot(m_d->ui.chbxEMEC_S3);
	connectToLastUpdateSlot(m_d->ui.chbxHEC_S0);
	connectToLastUpdateSlot(m_d->ui.chbxHEC_S1);
	connectToLastUpdateSlot(m_d->ui.chbxHEC_S2);
	connectToLastUpdateSlot(m_d->ui.chbxHEC_S3);
	connectToLastUpdateSlot(m_d->ui.chbxFCAL_M1);
	connectToLastUpdateSlot(m_d->ui.chbxFCAL_M2);
	connectToLastUpdateSlot(m_d->ui.chbxFCAL_M3);
	connectToLastUpdateSlot(m_d->ui.chbxTILB);
	connectToLastUpdateSlot(m_d->ui.chbxTILEC);
	connectToLastUpdateSlot(m_d->ui.chbxTILCR);
	// Negative checkboxes
	connectToLastUpdateSlot(m_d->ui.chbxLArNegative);
	connectToLastUpdateSlot(m_d->ui.chbxEMB_Neg_Expert);
	connectToLastUpdateSlot(m_d->ui.chbxEMEC_Neg_Expert);
	connectToLastUpdateSlot(m_d->ui.chbxHEC_Neg_Expert);
	connectToLastUpdateSlot(m_d->ui.chbxFCAL_Neg_Expert);
	connectToLastUpdateSlot(m_d->ui.chbxTileNegative);

	// Disable upper thresholds checkboxes
	connectToLastUpdateSlot(m_d->ui_threshlarsimp.chbxNoLArUpper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S0Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S3Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S0Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S3Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	connectToLastUpdateSlot(m_d->ui_threshtile.chbxNoTileUpper);

	// Lower threhold spinboxes
	connectToLastUpdateSlot(m_d->ui_threshlarsimp.dspbxLArLowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S0LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S1LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S2LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S3LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S0LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S1LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S2LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S3LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S0LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S1LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S2LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S3LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxFCAL_M1LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxFCAL_M2LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxFCAL_M3LowerThresh);
	connectToLastUpdateSlot(m_d->ui_threshtile.dspbxTileLowerThresh);

	// Upper threhold spinboxes
	connectToLastUpdateSlot(m_d->ui_threshlarsimp.dspbxLArUpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S0UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S1UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S2UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMB_S3UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S0UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S1UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S2UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxEMEC_S3UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S0UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S1UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S2UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxHEC_S3UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxFCAL_M1UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxFCAL_M2UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.dspbxFCAL_M3UpperThresh);
	connectToLastUpdateSlot(m_d->ui_threshtile.dspbxTileUpperThresh);

	// LAr mode change radio button
	connectToLastUpdateSlot(m_d->ui.rbtnSimple);

	// ----------------- Selection Intervals ------------------

	// ----------------- Global cuts ------------------
	addUpdateSlot(SLOT(possibleChange_globalCuts()));
	connectToLastUpdateSlot(m_d->ui_cuts.chbxSideA);
	connectToLastUpdateSlot(m_d->ui_cuts.chbxSideC);
	connectToLastUpdateSlot(m_d->ui_cuts.wdgEtaPhiCut,SIGNAL(allowedEtaChanged(const VP1Interval&)));
	connectToLastUpdateSlot(m_d->ui_cuts.wdgEtaPhiCut,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));
	// ----------------- Global cuts ------------------

	// -------------- Scale ------------
	addUpdateSlot(SLOT(possibleChange_scale()));
	connectToLastUpdateSlot(m_d->ui_visopts.dspbxScale);
	connectToLastUpdateSlot(m_d->ui_visopts.chbxLogscale);
	// -------------- Scale ------------

	// ---------------- Enable Upper Threshold ----------------
	addUpdateSlot(SLOT(enableUpperThreshold()));
	connectToLastUpdateSlot(m_d->ui_threshlarsimp.chbxNoLArUpper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S0Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMB_S3Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S0Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoHEC_S3Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	connectToLastUpdateSlot(m_d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	connectToLastUpdateSlot(m_d->ui_threshtile.chbxNoTileUpper);

	// ----------------- LAr mode ------------------
	addUpdateSlot(SLOT(changeMode()));
	connectToLastUpdateSlot(m_d->ui.rbtnSimple);

	// ----------------- MBTS ---------------------
	addUpdateSlot(SLOT(possibleChange_selectionMbts()));
	connectToLastUpdateSlot(m_d->ui.chbxMbts);
	connectToLastUpdateSlot(m_d->ui_threshmbts.dspbxMbtsThresh);

	// ------------- Digits checkbox ----------------
	addUpdateSlot(SLOT(possibleChange_showDigits()));
	connectToLastUpdateSlot(m_d->ui_interact.chbxDigits);
	// ------------- Digits checkbox ----------------

	// ------------- Volume OutLines -------------
	addUpdateSlot(SLOT(possibleChange_showVolumeOutLines()));
	connectToLastUpdateSlot(m_d->ui_visopts.chbxShowOutlines);
	// ------------- Volume OutLines -------------

	// ------------- Energy Mode -------------
	addUpdateSlot(SLOT(possibleChange_energyModeEt()));
	connectToLastUpdateSlot(m_d->ui.rbtnModeEt);
	// ------------- Energy Mode -------------

	initLastVars();
	// ------------------------ SLOTS ----------------------

	// ---------------------- Material Buttons --------------------
	m_d->ui_visopts.matbutLArEMBColorPos->setMaterial(VP1MaterialButton::createMaterial(0.21,0.8,0.51));
	m_d->ui_visopts.matbutLArEMECColorPos->setMaterial(VP1MaterialButton::createMaterial(0.26,0.8,0.2));
	m_d->ui_visopts.matbutLArHECColorPos->setMaterial(VP1MaterialButton::createMaterial(0.25,0.8,0.76));
	m_d->ui_visopts.matbutLArFCALColorPos->setMaterial(VP1MaterialButton::createMaterial(0.09,0.98,0.02));
	m_d->ui_visopts.matbutLArEMBColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	m_d->ui_visopts.matbutLArEMECColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	m_d->ui_visopts.matbutLArHECColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	m_d->ui_visopts.matbutLArFCALColorNeg->setMaterial(VP1MaterialButton::createMaterial(0.21,0.51,0.8));
	m_d->ui_visopts.matbutTileColorPosPMT1->setMaterial(VP1MaterialButton::createMaterial(1.,1.,0.));
	m_d->ui_visopts.matbutTileColorPosPMT0->setMaterial(VP1MaterialButton::createMaterial(1.,0.78,0.));
	m_d->ui_visopts.matbutTileColorPosNeg->setMaterial(VP1MaterialButton::createMaterial(1.,0.,0.));
	m_d->ui_visopts.matbutTileColorNegPMT1->setMaterial(VP1MaterialButton::createMaterial(1.,0.,1.));
	m_d->ui_visopts.matbutTileColorNegPMT0->setMaterial(VP1MaterialButton::createMaterial(0.78,0.,1.));
	m_d->ui_visopts.matbutTileColorNegPos->setMaterial(VP1MaterialButton::createMaterial(1.,0.,0.));
	m_d->ui_visopts.matbutMbts->setMaterial(VP1MaterialButton::createMaterial(1.,1.,0.));

	m_d->matButtonMap[VP1CC_SepLArEMBPos] = m_d->ui_visopts.matbutLArEMBColorPos;
	m_d->matButtonMap[VP1CC_SepLArEMECPos] = m_d->ui_visopts.matbutLArEMECColorPos;
	m_d->matButtonMap[VP1CC_SepLArHECPos] = m_d->ui_visopts.matbutLArHECColorPos;
	m_d->matButtonMap[VP1CC_SepLArFCALPos] = m_d->ui_visopts.matbutLArFCALColorPos;
	m_d->matButtonMap[VP1CC_SepLArEMBNeg] = m_d->ui_visopts.matbutLArEMBColorNeg;
	m_d->matButtonMap[VP1CC_SepLArEMECNeg] = m_d->ui_visopts.matbutLArEMECColorNeg;
	m_d->matButtonMap[VP1CC_SepLArHECNeg] = m_d->ui_visopts.matbutLArHECColorNeg;
	m_d->matButtonMap[VP1CC_SepLArFCALNeg] = m_d->ui_visopts.matbutLArFCALColorNeg;
	m_d->matButtonMap[VP1CC_SepTilePositiveUp] = m_d->ui_visopts.matbutTileColorPosPMT1;
	m_d->matButtonMap[VP1CC_SepTilePositiveDown] = m_d->ui_visopts.matbutTileColorPosPMT0;
	m_d->matButtonMap[VP1CC_SepTilePositiveNeg] = m_d->ui_visopts.matbutTileColorPosNeg;
	m_d->matButtonMap[VP1CC_SepTileNegativeUp] = m_d->ui_visopts.matbutTileColorNegPMT1;
	m_d->matButtonMap[VP1CC_SepTileNegativeDown] = m_d->ui_visopts.matbutTileColorNegPMT0;
	m_d->matButtonMap[VP1CC_SepTileNegativePos] = m_d->ui_visopts.matbutTileColorNegPos;
	m_d->matButtonMap[VP1CC_SepMBTS] = m_d->ui_visopts.matbutMbts;
	// ---------------------- Material Buttons --------------------

	// --------------------- Digits Display -------------------------
	messageDebug("Digits Display...");
	m_d->digit_form_single = new QWidget(0,Qt::WindowStaysOnTopHint);
	m_d->digit_form_single->setVisible(false);

	m_d->digit_form_double = new QWidget(0,Qt::WindowStaysOnTopHint);
	m_d->digit_form_double->setVisible(false);

//	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	messageDebug("setting up digits box (s)");
//	m_d->UiDigitsSingle = new Ui::frmCaloDigitsSingle();
//	m_d->UiDigitsSingle->setupUi(m_d->digit_form_single);
//	m_d->UiDigitsSingle->plotDigits->setAxisTitle(QwtPlot::xBottom,"Bunch crossing");
//	m_d->UiDigitsSingle->plotDigits->setAxisTitle(QwtPlot::yLeft,"ADC counts");
//
	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	messageDebug("setting up digits box (m_d)");
//	m_d->UiDigitsDouble = new Ui::frmCaloDigitsDouble();
//	m_d->UiDigitsDouble->setupUi(m_d->digit_form_double);
//	m_d->UiDigitsDouble->plotDigits_1->setAxisTitle(QwtPlot::xBottom,"Bunch crossing");
//	m_d->UiDigitsDouble->plotDigits_1->setAxisTitle(QwtPlot::yLeft,"ADC counts");
//	m_d->UiDigitsDouble->plotDigits_2->setAxisTitle(QwtPlot::xBottom,"Bunch crossing");
//	m_d->UiDigitsDouble->plotDigits_2->setAxisTitle(QwtPlot::yLeft,"ADC counts");
	// --------------------- Digits Display -------------------------

	// Fill Sel2Gui maps
	VP1CCUi2Manager* ui2manager;
	// EMB
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMB;
	ui2manager->showNegativeCB = m_d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarsimp.dspbxLArUpperThresh;

	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMB0] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMB1] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMB2] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMB3] = ui2manager;

	// EMEC
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMEC;
	ui2manager->showNegativeCB = m_d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarsimp.dspbxLArUpperThresh;

	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMEC0] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMEC1] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMEC2] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeEMEC3] = ui2manager;

	// HEC
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxHEC;
	ui2manager->showNegativeCB = m_d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarsimp.dspbxLArUpperThresh;

	(m_d->sel2GuiSimple)[VP1CC_SelTypeHEC0] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeHEC1] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeHEC2] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeHEC3] = ui2manager;

	// FCAL
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxFCAL;
	ui2manager->showNegativeCB = m_d->ui.chbxLArNegative;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarsimp.chbxNoLArUpper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarsimp.dspbxLArLowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarsimp.dspbxLArUpperThresh;

	(m_d->sel2GuiSimple)[VP1CC_SelTypeFCAL1] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeFCAL2] = ui2manager;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeFCAL3] = ui2manager;

	// ---VP1CC_SelTypeEMB0 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMB_S0;
	ui2manager->showNegativeCB = m_d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMB_S0Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S0LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S0UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMB0] = ui2manager;

	// ---VP1CC_SelTypeEMB1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMB_S1;
	ui2manager->showNegativeCB = m_d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMB_S1Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S1LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S1UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMB1] = ui2manager;

	// --- VP1CC_SelTypeEMB2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMB_S2;
	ui2manager->showNegativeCB = m_d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMB_S2Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S2LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S2UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMB2] = ui2manager;

	// --- VP1CC_SelTypeEMB3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMB_S3;
	ui2manager->showNegativeCB = m_d->ui.chbxEMB_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMB_S3Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S3LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMB_S3UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMB3] = ui2manager;

	// --- VP1CC_SelTypeEMEC0 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMEC_S0;
	ui2manager->showNegativeCB = m_d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMEC_S0Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S0LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S0UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMEC0] = ui2manager;

	// --- VP1CC_SelTypeEMEC1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMEC_S1;
	ui2manager->showNegativeCB = m_d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMEC_S1Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S1LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S1UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMEC1] = ui2manager;

	// --- VP1CC_SelTypeEMEC2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMEC_S2;
	ui2manager->showNegativeCB = m_d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMEC_S2Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S2LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S2UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMEC2] = ui2manager;

	// --- VP1CC_SelTypeEMEC3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxEMEC_S3;
	ui2manager->showNegativeCB = m_d->ui.chbxEMEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoEMEC_S3Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S3LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxEMEC_S3UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeEMEC3] = ui2manager;

	// --- VP1CC_SelTypeHEC0 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxHEC_S0;
	ui2manager->showNegativeCB = m_d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoHEC_S0Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S0LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S0UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeHEC0] = ui2manager;

	// --- VP1CC_SelTypeHEC1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxHEC_S1;
	ui2manager->showNegativeCB = m_d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoHEC_S1Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S1LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S1UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeHEC1] = ui2manager;

	// --- VP1CC_SelTypeHEC2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxHEC_S2;
	ui2manager->showNegativeCB = m_d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoHEC_S2Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S2LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S2UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeHEC2] = ui2manager;

	// --- VP1CC_SelTypeHEC3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxHEC_S3;
	ui2manager->showNegativeCB = m_d->ui.chbxHEC_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoHEC_S3Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S3LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxHEC_S3UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeHEC3] = ui2manager;

	// --- VP1CC_SelTypeFCAL1 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxFCAL_M1;
	ui2manager->showNegativeCB = m_d->ui.chbxFCAL_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoFCAL_M1Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxFCAL_M1LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxFCAL_M1UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeFCAL1] = ui2manager;

	// --- VP1CC_SelTypeFCAL2 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxFCAL_M2;
	ui2manager->showNegativeCB = m_d->ui.chbxFCAL_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoFCAL_M2Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxFCAL_M2LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxFCAL_M2UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeFCAL2] = ui2manager;

	// --- VP1CC_SelTypeFCAL3 ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxFCAL_M3;
	ui2manager->showNegativeCB = m_d->ui.chbxFCAL_Neg_Expert;
	ui2manager->upperThreshOffCB = m_d->ui_threshlarexp.chbxNoFCAL_M3Upper;
	ui2manager->lowerThresholdSB = m_d->ui_threshlarexp.dspbxFCAL_M3LowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshlarexp.dspbxFCAL_M3UpperThresh;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeFCAL3] = ui2manager;

	// --- VP1CC_SelTypeTileB ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxTILB;
	ui2manager->showNegativeCB = m_d->ui.chbxTileNegative;
	ui2manager->upperThreshOffCB = m_d->ui_threshtile.chbxNoTileUpper;
	ui2manager->lowerThresholdSB = m_d->ui_threshtile.dspbxTileLowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshtile.dspbxTileUpperThresh;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeTileB] = ui2manager;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeTileB] = ui2manager;

	// --- VP1CC_SelTypeTileEC ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxTILEC;
	ui2manager->showNegativeCB = m_d->ui.chbxTileNegative;
	ui2manager->upperThreshOffCB = m_d->ui_threshtile.chbxNoTileUpper;
	ui2manager->lowerThresholdSB = m_d->ui_threshtile.dspbxTileLowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshtile.dspbxTileUpperThresh;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeTileEC] = ui2manager;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeTileEC] = ui2manager;

	// --- VP1CC_SelTypeTileCrack ---
	ui2manager = new VP1CCUi2Manager;
	ui2manager->globalEnableCB = m_d->ui.chbxTILCR;
	ui2manager->showNegativeCB = m_d->ui.chbxTileNegative;
	ui2manager->upperThreshOffCB = m_d->ui_threshtile.chbxNoTileUpper;
	ui2manager->lowerThresholdSB = m_d->ui_threshtile.dspbxTileLowerThresh;
	ui2manager->upperThresholdSB = m_d->ui_threshtile.dspbxTileUpperThresh;
	(m_d->sel2GuiSimple)[VP1CC_SelTypeTileCrack] = ui2manager;
	(m_d->sel2GuiExpert)[VP1CC_SelTypeTileCrack] = ui2manager;
}

VP1CaloCellController::~VP1CaloCellController()
{
	// Clean up in Imp maps
	delete (m_d->sel2GuiSimple)[VP1CC_SelTypeEMB0];
	delete (m_d->sel2GuiSimple)[VP1CC_SelTypeEMEC0];
	delete (m_d->sel2GuiSimple)[VP1CC_SelTypeHEC0];
	delete (m_d->sel2GuiSimple)[VP1CC_SelTypeFCAL1];
	//TK: Fixme: why not just iterate and delete all here?
	m_d->sel2GuiSimple.clear();

	for(VP1CCSelectionType2GuiMap::iterator it=m_d->sel2GuiExpert.begin(); it!=m_d->sel2GuiExpert.end(); it++)
		delete it->second;
	m_d->sel2GuiExpert.clear();
	delete m_d;
}

void VP1CaloCellController::actualRestoreSettings(VP1Deserialise& s)
{
	if (s.version()<0 || s.version()>4) {
		message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
		return;
	}

	s.restore(m_d->ui_interact.chbxDigits);
	s.restore(m_d->ui_visopts.dspbxScale);
	s.restore(m_d->ui.chbxMbts);
	s.restore(m_d->ui_threshmbts.dspbxMbtsThresh);
	s.restore(m_d->ui_badchans.chbxEnableBadChannels);
	s.restore(m_d->ui_badchans.chbxMaskDead);
	s.restore(m_d->ui_badchans.chbxMaskNoisy);
	s.restore(m_d->ui_badchans.chbxMaskAffected);
	s.restore(m_d->ui.rbtnSimple,m_d->ui.rbtnExpert);
	s.restore(m_d->ui.chbxEMB);
	s.restore(m_d->ui.chbxEMEC);
	s.restore(m_d->ui.chbxHEC);
	s.restore(m_d->ui.chbxFCAL);
	s.restore(m_d->ui.chbxLArNegative);
	s.restore(m_d->ui_threshlarsimp.dspbxLArLowerThresh);
	s.restore(m_d->ui_threshlarsimp.dspbxLArUpperThresh);
	s.restore(m_d->ui_threshlarsimp.chbxNoLArUpper);
	s.restore(m_d->ui.chbxEMB_Neg_Expert);
	s.restore(m_d->ui.chbxEMB_S0);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S0LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S0UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMB_S0Upper);
	s.restore(m_d->ui.chbxEMB_S1);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S1LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S1UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMB_S1Upper);
	s.restore(m_d->ui.chbxEMB_S2);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S2LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S2UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMB_S2Upper);
	s.restore(m_d->ui.chbxEMB_S3);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S3LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMB_S3UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMB_S3Upper);
	s.restore(m_d->ui.chbxEMEC_Neg_Expert);
	s.restore(m_d->ui.chbxEMEC_S0);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S0LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S0UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	s.restore(m_d->ui.chbxEMEC_S1);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S1LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S1UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	s.restore(m_d->ui.chbxEMEC_S2);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S2LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S2UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	s.restore(m_d->ui.chbxEMEC_S3);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S3LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxEMEC_S3UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	s.restore(m_d->ui.chbxHEC_Neg_Expert);
	s.restore(m_d->ui.chbxHEC_S0);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S0LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S0UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoHEC_S0Upper);
	s.restore(m_d->ui.chbxHEC_S1);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S1LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S1UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoHEC_S1Upper);
	s.restore(m_d->ui.chbxHEC_S2);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S2LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S2UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoHEC_S2Upper);
	s.restore(m_d->ui.chbxHEC_S3);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S3LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxHEC_S3UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoHEC_S3Upper);
	s.restore(m_d->ui.chbxFCAL_Neg_Expert);
	s.restore(m_d->ui.chbxFCAL_M1);
	s.restore(m_d->ui_threshlarexp.dspbxFCAL_M1LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxFCAL_M1UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	s.restore(m_d->ui.chbxFCAL_M2);
	s.restore(m_d->ui_threshlarexp.dspbxFCAL_M2LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxFCAL_M2UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	s.restore(m_d->ui.chbxFCAL_M3);
	s.restore(m_d->ui_threshlarexp.dspbxFCAL_M3LowerThresh);
	s.restore(m_d->ui_threshlarexp.dspbxFCAL_M3UpperThresh);
	s.restore(m_d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	s.restore(m_d->ui.chbxTILB);
	s.restore(m_d->ui.chbxTILEC);
	s.restore(m_d->ui.chbxTILCR);
	s.restore(m_d->ui.chbxTileNegative);
	s.restore(m_d->ui_threshtile.dspbxTileLowerThresh);
	s.restore(m_d->ui_threshtile.dspbxTileUpperThresh);
	s.restore(m_d->ui_threshtile.chbxNoTileUpper);
	s.restore(m_d->ui_visopts.matbutLArEMBColorPos);
	s.restore(m_d->ui_visopts.matbutLArEMBColorNeg);
	s.restore(m_d->ui_visopts.matbutLArEMECColorPos);
	s.restore(m_d->ui_visopts.matbutLArEMECColorNeg);
	s.restore(m_d->ui_visopts.matbutLArHECColorPos);
	s.restore(m_d->ui_visopts.matbutLArHECColorNeg);
	s.restore(m_d->ui_visopts.matbutLArFCALColorPos);
	s.restore(m_d->ui_visopts.matbutLArFCALColorNeg);
	s.restore(m_d->ui_visopts.matbutTileColorPosPMT0);
	s.restore(m_d->ui_visopts.matbutTileColorPosPMT1);
	s.restore(m_d->ui_visopts.matbutTileColorPosNeg);
	s.restore(m_d->ui_visopts.matbutTileColorNegPMT0);
	s.restore(m_d->ui_visopts.matbutTileColorNegPMT1);
	s.restore(m_d->ui_visopts.matbutTileColorNegPos);
	s.restore(m_d->ui_visopts.matbutMbts);

	if(s.version()>0) {
		s.restore(m_d->ui_visopts.wdgDrawOptions);
		s.restore(m_d->ui_visopts.chbxShowOutlines);
	}

	if(s.version()>1) {
		s.restore(m_d->ui.rbtnModeEt,m_d->ui.rbtnModeEne);
	}

	if(s.version()>2) {
		s.restore(m_d->ui_visopts.chbxLogscale);
	}

	if(s.version()>3) {
		s.restore(m_d->ui_cuts.chbxSideA);
		s.restore(m_d->ui_cuts.chbxSideC);
		s.restore(m_d->ui_cuts.wdgEtaPhiCut);
	}
}

int VP1CaloCellController::currentSettingsVersion() const
{
	return 4;
}

void VP1CaloCellController::actualSaveSettings(VP1Serialise& s) const
{
	s.save(m_d->ui_interact.chbxDigits);
	s.save(m_d->ui_visopts.dspbxScale);
	s.save(m_d->ui.chbxMbts);
	s.save(m_d->ui_threshmbts.dspbxMbtsThresh);
	s.save(m_d->ui_badchans.chbxEnableBadChannels);
	s.save(m_d->ui_badchans.chbxMaskDead);
	s.save(m_d->ui_badchans.chbxMaskNoisy);
	s.save(m_d->ui_badchans.chbxMaskAffected);
	s.save(m_d->ui.rbtnSimple,m_d->ui.rbtnExpert);
	s.save(m_d->ui.chbxEMB);
	s.save(m_d->ui.chbxEMEC);
	s.save(m_d->ui.chbxHEC);
	s.save(m_d->ui.chbxFCAL);
	s.save(m_d->ui.chbxLArNegative);
	s.save(m_d->ui_threshlarsimp.dspbxLArLowerThresh);
	s.save(m_d->ui_threshlarsimp.dspbxLArUpperThresh);
	s.save(m_d->ui_threshlarsimp.chbxNoLArUpper);
	s.save(m_d->ui.chbxEMB_Neg_Expert);
	s.save(m_d->ui.chbxEMB_S0);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S0LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S0UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMB_S0Upper);
	s.save(m_d->ui.chbxEMB_S1);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S1LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S1UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMB_S1Upper);
	s.save(m_d->ui.chbxEMB_S2);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S2LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S2UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMB_S2Upper);
	s.save(m_d->ui.chbxEMB_S3);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S3LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMB_S3UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMB_S3Upper);
	s.save(m_d->ui.chbxEMEC_Neg_Expert);
	s.save(m_d->ui.chbxEMEC_S0);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S0LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S0UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMEC_S0Upper);
	s.save(m_d->ui.chbxEMEC_S1);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S1LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S1UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMEC_S1Upper);
	s.save(m_d->ui.chbxEMEC_S2);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S2LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S2UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMEC_S2Upper);
	s.save(m_d->ui.chbxEMEC_S3);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S3LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxEMEC_S3UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoEMEC_S3Upper);
	s.save(m_d->ui.chbxHEC_Neg_Expert);
	s.save(m_d->ui.chbxHEC_S0);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S0LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S0UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoHEC_S0Upper);
	s.save(m_d->ui.chbxHEC_S1);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S1LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S1UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoHEC_S1Upper);
	s.save(m_d->ui.chbxHEC_S2);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S2LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S2UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoHEC_S2Upper);
	s.save(m_d->ui.chbxHEC_S3);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S3LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxHEC_S3UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoHEC_S3Upper);
	s.save(m_d->ui.chbxFCAL_Neg_Expert);
	s.save(m_d->ui.chbxFCAL_M1);
	s.save(m_d->ui_threshlarexp.dspbxFCAL_M1LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxFCAL_M1UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoFCAL_M1Upper);
	s.save(m_d->ui.chbxFCAL_M2);
	s.save(m_d->ui_threshlarexp.dspbxFCAL_M2LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxFCAL_M2UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoFCAL_M2Upper);
	s.save(m_d->ui.chbxFCAL_M3);
	s.save(m_d->ui_threshlarexp.dspbxFCAL_M3LowerThresh);
	s.save(m_d->ui_threshlarexp.dspbxFCAL_M3UpperThresh);
	s.save(m_d->ui_threshlarexp.chbxNoFCAL_M3Upper);
	s.save(m_d->ui.chbxTILB);
	s.save(m_d->ui.chbxTILEC);
	s.save(m_d->ui.chbxTILCR);
	s.save(m_d->ui.chbxTileNegative);
	s.save(m_d->ui_threshtile.dspbxTileLowerThresh);
	s.save(m_d->ui_threshtile.dspbxTileUpperThresh);
	s.save(m_d->ui_threshtile.chbxNoTileUpper);
	s.save(m_d->ui_visopts.matbutLArEMBColorPos);
	s.save(m_d->ui_visopts.matbutLArEMBColorNeg);
	s.save(m_d->ui_visopts.matbutLArEMECColorPos);
	s.save(m_d->ui_visopts.matbutLArEMECColorNeg);
	s.save(m_d->ui_visopts.matbutLArHECColorPos);
	s.save(m_d->ui_visopts.matbutLArHECColorNeg);
	s.save(m_d->ui_visopts.matbutLArFCALColorPos);
	s.save(m_d->ui_visopts.matbutLArFCALColorNeg);
	s.save(m_d->ui_visopts.matbutTileColorPosPMT0);
	s.save(m_d->ui_visopts.matbutTileColorPosPMT1);
	s.save(m_d->ui_visopts.matbutTileColorPosNeg);
	s.save(m_d->ui_visopts.matbutTileColorNegPMT0);
	s.save(m_d->ui_visopts.matbutTileColorNegPMT1);
	s.save(m_d->ui_visopts.matbutTileColorNegPos);
	s.save(m_d->ui_visopts.matbutMbts);
	s.save(m_d->ui_visopts.wdgDrawOptions); // 1+
	s.save(m_d->ui_visopts.chbxShowOutlines); // 1+
	s.save(m_d->ui.rbtnModeEt,m_d->ui.rbtnModeEne); // 2+
	s.save(m_d->ui_visopts.chbxLogscale); // 3+
	s.save(m_d->ui_cuts.chbxSideA); // 4+
	s.save(m_d->ui_cuts.chbxSideC); // 4+
	s.save(m_d->ui_cuts.wdgEtaPhiCut); // 4+
}

SoGroup* VP1CaloCellController::drawOptions() const
{
	return m_d->ui_visopts.wdgDrawOptions->drawOptionsGroup();
}

void VP1CaloCellController::initTilePulse(const TileHWID* tile_hw_id,
		const TileInfo* tile_info,
		const TileCablingService* tile_cabling)
{
	m_tile_hw_id = tile_hw_id;
	m_tile_info = tile_info;
	m_tile_cabling = tile_cabling;
}

void VP1CaloCellController::ClearHideDigitForms()
{
	messageDebug("ClearHideDigitForms()");
	m_d->singlePlotHasData = false;
	m_d->digit_form_single->setVisible(false);
	m_d->doublePlotHasData = false;
	m_d->digit_form_double->setVisible(false);
}

void VP1CaloCellController::DeleteDigitForms()
{
	messageDebug("DeleteDigitForms()");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
	/*
	if (m_d) {
		messageDebug("delete digit_form_single...");
		if (m_d->digit_form_single) {
			delete m_d->digit_form_single;
			m_d->digit_form_single = 0;
		}
		messageDebug("delete digit_form_double...");
		if (m_d->digit_form_double) {
			delete m_d->digit_form_double;
			m_d->digit_form_double = 0;
		}
		messageDebug("delete UiDigitsSingle...");
		if (m_d->UiDigitsSingle) {
			delete m_d->UiDigitsSingle;
			m_d->UiDigitsSingle = 0;
		}
		messageDebug("delete UiDigitsDouble...");
		if (m_d->UiDigitsDouble) {
			delete m_d->UiDigitsDouble;
			m_d->UiDigitsDouble = 0;
		}
		messageDebug("DeleteDigitForms(). End.");
	}
    */
}

void VP1CaloCellController::EnableDigitsCheckbox(bool enable)
{
	m_d->ui_interact.chbxDigits->setEnabled(enable);
}

void VP1CaloCellController::displayLArDigits(
		int /* n_samples */,
		std::vector<short> /* samples */,
		std::vector<std::string>& /* msg */)
{
	message("VP1CaloCellController::displayLArDigits()... not be ported to 'master' yet, because of failing dependencies (LCG's Qwt has to be ported to Qt5, still)");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
/*

	QwtPlot* plot = m_d->UiDigitsSingle->plotDigits;

	// Pop up the widget
	m_d->digit_form_single->setVisible(true);
	m_d->digit_form_double->setVisible(false);

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
	//	std::cout << "plot: " << m_d->UiDigitsSingle->plotDigits << std::endl;
	_curve->attach(plot);

	// Set the canvas background color
	plot->setCanvasBackground( QBrush(Qt::white) );

	// Refresh the plot
//	plot->replot();
	m_d->UiDigitsSingle->plotDigits->replot();


	// Set values for the labels
	m_d->UiDigitsSingle->lblCellName->setText((msg[1]).c_str());
	m_d->UiDigitsSingle->lblTotalEnergy->setText((msg[3]).c_str());
	m_d->UiDigitsSingle->lblTotalTime->setText((msg[4]).c_str());

	// hide pmt/gain label
	m_d->UiDigitsSingle->lblPMTGain->setVisible(false);

	// set the flag for the plot data
	m_d->singlePlotHasData = true;
	m_d->doublePlotHasData = false;
    */

	messageDebug("display LAr digits end.");
}

void VP1CaloCellController::displayTileDigits(int /*n_samples*/,
                                              std::vector<float> /*samples1*/,
                                              std::vector<float> /*samples2*/,
                                              std::vector<std::string>& /*msg*/)
{

	messageDebug("VP1CaloCellController::displayTileDigits(A)...");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
	/*
	// Pop up the widget
	m_d->digit_form_double->setVisible(true);
	m_d->digit_form_single->setVisible(false);

	messageDebug("Setting text...");
	m_d->UiDigitsDouble->lblCellName->setText(msg[1].c_str());
	m_d->UiDigitsDouble->lblTotalEnergy->setText(msg[2].c_str());
	m_d->UiDigitsDouble->lblTotalTime->setText(msg[3].c_str());


	messageDebug("Calling DrawTileDigits()...");
	DrawTileDigits(m_d->UiDigitsDouble->plotDigits_1,n_samples,samples1);
	messageDebug("Called on plotDigits_1.");
	DrawTileDigits(m_d->UiDigitsDouble->plotDigits_2,n_samples,samples2);
	messageDebug("Called on plotDigits_2.");

	// set the flag for the plot data
	m_d->doublePlotHasData = true;
	m_d->singlePlotHasData = false;
	*/
}

void VP1CaloCellController::displayTileDigits(int /*n_samples*/,
                                              std::vector<float> /*samples*/,
                                              std::vector<std::string>& /*msg*/)
{
	messageDebug("VP1CaloCellController::displayTileDigits(B)...");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
	/*
	// Pop up the widget
	m_d->digit_form_single->setVisible(true);
	m_d->digit_form_double->setVisible(false);

	messageDebug("Setting text...");
	m_d->UiDigitsSingle->lblCellName->setText(msg[1].c_str());
	m_d->UiDigitsSingle->lblTotalEnergy->setText(msg[2].c_str());
	m_d->UiDigitsSingle->lblTotalTime->setText(msg[3].c_str());

	// show pmt/gain label
	messageDebug("Show pmt/gain...");
	m_d->UiDigitsSingle->lblPMTGain->setVisible(true);

	messageDebug("calling DrawTileDigits()...");
	DrawTileDigits(m_d->UiDigitsSingle->plotDigits,n_samples,samples);
	messageDebug("called DrawTileDigits().");

	// set the flag for the plot data
	m_d->doublePlotHasData = true;
	m_d->singlePlotHasData = false;
	*/
}

void VP1CaloCellController::displayTilePulse(const TileRawChannel* /*rawchannel1*/,
                                             const TileRawChannel* /*rawchannel2*/,
                                             const TileRawChannelContainer* /*rawchannel_cont*/,
                                             size_t /*digitsize*/)
{
	messageDebug("VP1CaloCellController::displayTilePulse(A)...");

	//	std::cout << ""
	//			<< "a:" << m_d->UiDigitsDouble->plotDigits_1 << " - "
	//			<< "b:" << m_d->UiDigitsDouble->lblPMTGain_1 << " - "
	//			<< "c:" << rawchannel1 << " - "
	//			<< "m_d:" << rawchannel_cont << " - "
	//			<< "e:" << m_d->UiDigitsDouble->plotDigits_2 << " - "
	//			<< "f:" << m_d->UiDigitsDouble->lblPMTGain_2 << " - "
	//			<< "g:" << rawchannel2 << " - "
	//			<< "h:" << digitsize
	//			<< std::endl;


	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	DrawTilePulse(m_d->UiDigitsDouble->plotDigits_1,
//			m_d->UiDigitsDouble->lblPMTGain_1,
//			rawchannel1,
//			rawchannel_cont,
//			digitsize);
//	std::cout << "Drawn digits_1" << std::endl;
//
//	DrawTilePulse(m_d->UiDigitsDouble->plotDigits_2,
//			m_d->UiDigitsDouble->lblPMTGain_2,
//			rawchannel2,
//			rawchannel_cont,
//			digitsize);
//	std::cout << "Drawn digits_2" << std::endl;
}

void VP1CaloCellController::displayTilePulse(const TileRawChannel* /*rawchannel*/,
                                             const TileRawChannelContainer* /*rawchannel_cont*/,
                                             size_t /*digitsize*/)
{
	messageDebug("VP1CaloCellController::displayTilePulse(B)...");

	// FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//	DrawTilePulse(m_d->UiDigitsSingle->plotDigits,
//			m_d->UiDigitsSingle->lblPMTGain,
//			rawchannel,
//			rawchannel_cont,
//			digitsize);

	messageDebug("displayTilePulse(). End.");
}

SoMaterial* VP1CaloCellController::GetMaterial(VP1CC_SeparatorTypes type)
{
	Imp::MatButtonMap::const_iterator it = m_d->matButtonMap.find(type);
	if(it!=m_d->matButtonMap.end())
		return getMaterial(it->second);
	else
		return 0;
}

VP1CCIntervalMap VP1CaloCellController::selectionIntervals() const
{
	// Which sel2Gui map are we working with? This depends on the active mode (simple/expert)
	VP1CCSelectionType2GuiMap* useMap = 0;

	if(m_d->ui.rbtnSimple->isChecked())
		useMap = &(m_d->sel2GuiSimple);
	else
		useMap = &(m_d->sel2GuiExpert);

	VP1CCIntervalMap returnMap;

	// Construct a new map of intervals based on values of GUI widgets
	for(VP1CCSelectionType2GuiMap::iterator it=useMap->begin(); it!=useMap->end(); it++) {
		// Pointer to the current structure
		VP1CCUi2Manager* ui = it->second;

		VP1Interval posInterval, negInterval;
		bool positiveON = true;
		bool negativeON = true;

		// If subsystem is unchecked set pos/neg OFF
		if(!ui->globalEnableCB->isChecked()) {
			positiveON = false;
			negativeON = false;
		}

		// If negative is unchecked set neg OFF
		if(!ui->showNegativeCB->isChecked())
			negativeON = false;

		// If upper threshold is available and lower>upper set pos/neg OFF
		if(!ui->upperThreshOffCB->isChecked() &&
				ui->lowerThresholdSB->value() > ui->upperThresholdSB->value()){
			positiveON = false;
			negativeON = false;
		}

		// Construct new interval for positive cells
		if(positiveON){
			if(ui->upperThreshOffCB->isChecked())
				posInterval.setUpper(VP1Interval::inf());
			else {
				posInterval.setUpper(ui->upperThresholdSB->value());
				posInterval.setOpenUpper(false);
			}
			posInterval.setLower(ui->lowerThresholdSB->value());
			posInterval.setOpenLower(false);
		}

		// We use the same interval for negative cells as well if they are ON
		if(negativeON)
			negInterval = posInterval;

		returnMap[it->first] = VP1CCIntervalPair(posInterval,negInterval);
	}

	return returnMap;
}

QPair<bool,double> VP1CaloCellController::scale() const
{
	double scl = m_d->ui_visopts.chbxLogscale->isChecked() ? m_d->ui_visopts.dspbxScale->value()*Gaudi::Units::m/log(1+10*Gaudi::Units::GeV) : m_d->ui_visopts.dspbxScale->value()*Gaudi::Units::m/(10*Gaudi::Units::GeV);
	return QPair<bool,double>(m_d->ui_visopts.chbxLogscale->isChecked(),scl);
}

VP1Interval VP1CaloCellController::selectionMbts() const
{
	VP1Interval returnInterval;
	if(m_d->ui.chbxMbts->isChecked()) {
		returnInterval.setUpper(VP1Interval::inf());
		returnInterval.setLower(m_d->ui_threshmbts.dspbxMbtsThresh->value());
		returnInterval.setOpenLower(false);
	}

	return returnInterval;
}

bool VP1CaloCellController::showDigits() const
{
	return m_d->ui_interact.chbxDigits->isChecked();
}

bool VP1CaloCellController::showVolumeOutLines() const
{
	return m_d->ui_visopts.chbxShowOutlines->isChecked();
}

bool VP1CaloCellController::energyModeEt() const
{
	return m_d->ui.rbtnModeEt->isChecked();
}

VP1CC_GlobalCuts VP1CaloCellController::globalCuts() const
{
	VP1CC_GlobalCuts globalCuts;
	globalCuts.sideA = m_d->ui_cuts.chbxSideA->isChecked();
	globalCuts.sideC = m_d->ui_cuts.chbxSideC->isChecked();
	globalCuts.allowedEta = m_d->ui_cuts.wdgEtaPhiCut->allowedEta();
	globalCuts.allowedPhi = m_d->ui_cuts.wdgEtaPhiCut->allowedPhi();
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
////			<< "m_tile_hw_id ros: " << m_tile_hw_id->ros(adc_hwid1) << " - "
////			<< "m_tile_hw_id channel: " << m_tile_hw_id->channel(adc_hwid1)
////			<< std::endl;
//
//	pmtgain1 << " PMT " << m_tile_cabling->channel2hole(m_tile_hw_id->ros(adc_hwid1),m_tile_hw_id->channel(adc_hwid1))
//			   << "   Gain " << m_tile_hw_id->adc(adc_hwid1);
//
//	gainlabel->setText(pmtgain1.str().c_str());
//
//	std::cout << "DEBUG - Getting TileInfo..." << std::endl;
//	try {
//		_amplitude1 /= m_tile_info->ChannelCalib(adc_hwid1,
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
//	if(m_tile_hw_id->adc(adc_hwid1) == 0)
//	{
//		xval = &(m_tile_info->getPulseShapes()->m_tlphys);
//		yval = &(m_tile_info->getPulseShapes()->m_ylphys);
//	}
//	else if(m_tile_hw_id->adc(adc_hwid1) == 1)
//	{
//		xval = &(m_tile_info->getPulseShapes()->m_thphys);
//		yval = &(m_tile_info->getPulseShapes()->m_yhphys);
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

	if(m_d->ui.rbtnSimple->isChecked())
		useMap = &(m_d->sel2GuiSimple);
	else
		useMap = &(m_d->sel2GuiExpert);

	for(VP1CCSelectionType2GuiMap::iterator it=useMap->begin(); it!=useMap->end(); it++) {
		VP1CCUi2Manager* ui = it->second;
		ui->upperThresholdSB->setEnabled(!ui->upperThreshOffCB->isChecked());
	}
}

void VP1CaloCellController::changeMode()
{
	if(m_d->ui.rbtnSimple->isChecked()) {
		m_d->ui.stackLAr->setCurrentIndex(0);
		m_d->ui.pushButton_settings_thresholds_larsimple->setEnabled(true);
		m_d->ui.pushButton_settings_thresholds_larexpert->setEnabled(false);
	}
	else {
		m_d->ui.stackLAr->setCurrentIndex(1);
		m_d->ui.pushButton_settings_thresholds_larsimple->setEnabled(false);
		m_d->ui.pushButton_settings_thresholds_larexpert->setEnabled(true);
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
