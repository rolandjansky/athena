/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************
// Name: L1CaloPprFineTimePlotManager.h
//
// Author: Patrick Rieck - rieck@physik.hu-berlin.de
//
//         Rohin T Narayan (narayan@mail.cern.ch)
//
// Special plotter for fine time monitoring
//
// This class is derived from L1CaloPprPlotManager
// and implements the monitoring of ADC peak sampling offset jitter, aka fine time.
//
/*********************************************************************************************/
// For a brief description see https://twiki.cern.ch/twiki/bin/view/Sandbox/FineTimeMonitoring
/*********************************************************************************************/
//
#ifndef TRIGT1CALOCALIBTOOLS_L1CALOPPRFINETIMEPLOTMANAGER
#define TRIGT1CALOCALIBTOOLS_L1CALOPPRFINETIMEPLOTMANAGER

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloMonitoringCaloTool.h"

#include "TrigT1CaloCalibTools/L1CaloPprPlotManager.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"


class ITHistSvc;
class ManagedMonitorToolBase;
class StoreGateSvc;
class StatusCode;

namespace LVL1 {
class IL1CaloOfflineTriggerTowerTools;
class IL1TriggerTowerTool;
class TriggerTower;
}

class L1CaloPprFineTimePlotManager : public L1CaloPprPlotManager
{
public:
	// constructor for handling offline monitoring
	L1CaloPprFineTimePlotManager(ITHistSvc* histoSvc,
	                             ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools>&offlineTT_tool,
	                             const int ADC_cut,
	                             const unsigned int lumimax,
	                             const std::string& pathInRootFile);
	// constructor for online monitoring
	L1CaloPprFineTimePlotManager(ManagedMonitorToolBase* aMonObj,
	                             ToolHandle<LVL1::IL1TriggerTowerTool>&onlineTT_tool,
	                             const int ADC_cut, const unsigned int lumimax,
	                             const std::string& pathInRootFile);
	inline ~L1CaloPprFineTimePlotManager() {};

	inline void SetFineTimeCut(double& fineTimeCut) {m_fineTimeCut = fineTimeCut;};
	inline void SetPpmAdcMaxValue(unsigned int ppmAdcMaxValue) {m_ppmAdcMaxValue = ppmAdcMaxValue;}
	//	StatusCode MakeSummary();
	// method for setting the em and had reference values
	inline void SetReferenceValue(double refValue) {m_RefValue = refValue;};
	inline void SetEmReferenceValue(double refValue) {m_emRefValue = refValue;};
	inline void SetHadReferenceValue(double refValue) {m_hadRefValue = refValue;};
	// method for setting the calibration factor
	inline void SetCalibrationFactor(double calFactor) {m_CalFactor = calFactor;};
	inline void SetHadCalibrationFactor(double calFactor) {m_hadCalFactor = calFactor;};
	inline void SetEmCalibrationFactor(double calFactor) {m_emCalFactor = calFactor;};

	// load calo cells
	StatusCode getCaloCells();
	float LArQuality(const xAOD::TriggerTower* trigTower, int layer);
	bool  badLArQuality(float qual);
	bool  badHecQuality(float qual);

	void EnableCaloQualCut(bool doCut) {m_doCaloQualCut = doCut;};


private:

	ServiceHandle<StoreGateSvc> m_storeGate;
	ToolHandle<LVL1::IL1CaloMonitoringCaloTool> m_caloTool;

	// Providing fine time for histograms
	double getMonitoringValue(const xAOD::TriggerTower* trigTower, CalLayerEnum theLayer);

	// decide whether the given value that is supposed to be monitored
	// is acctually plotted
	bool doMonitoring(double &value);

	double m_fineTimeCut;  // plot fine time only for smaller absolut value
	int m_ppmAdcMaxValue;  // ADC saturation cut

	double m_RefValue;//, m_hadRefValue; // reference value per TT (read from db by the L1CaloPPrMonitoring)
	double m_emRefValue;
	double m_hadRefValue;

	double m_CalFactor;//, m_hadCalFactor;  // calibration factor per TT (read from db by the L1CaloPPrMonitoring)
	double m_emCalFactor;
	double m_hadCalFactor;

	void loadTools();

	bool m_doCaloQualCut; //allows to switch calo quality cut (and usage of calo folders) on and off


// 	friend class LVL1::IL1CaloOfflineTriggerTowerTools;
// 	friend Identifier LVL1::IL1CaloOfflineTriggerTowerTools::hadID(double, double) const;
};

#endif
