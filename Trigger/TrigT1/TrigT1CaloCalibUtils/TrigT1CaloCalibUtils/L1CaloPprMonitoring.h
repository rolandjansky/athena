/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************
// Name: L1CaloPprMonitoring.h
//
// Authors: Rohin T Narayan (narayan@mail.cern.ch)
//          Patrick Rieck -  rieck@physik.hu-berlin.de
//
// algorithm class for monitoring of trigger towers
//
// This class handles different kinds of monitoring, to be chosen
// with the corresponding properties declared. It relies on the plotter
// classes defined in TrigT1CaloCalibTools.
//
//
#ifndef TRIGT1CALOCALIBUTILS_L1CALOPPRMONITORING
#define TRIGT1CALOCALIBUTILS_L1CALOPPRMONITORING

#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CLHEP/Units/SystemOfUnits.h"    
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class CondAttrListCollection;
class EventInfo;
class ITHistSvc;
class L1CaloPprFineTimePlotManager;
class L1CaloPprPedestalPlotManager;
class L1CaloPprPedestalCorrectionPlotManager;
class L1CaloPprEtCorrelationPlotManager;
namespace LVL1 {
  class IL1CaloOfflineTriggerTowerTools;
  class IL1TriggerTowerTool;
}

class L1CaloPprMonitoring : public AthAlgorithm
{
    public:

        L1CaloPprMonitoring(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~L1CaloPprMonitoring();

        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

    protected:

        StatusCode loadInTools();
        StatusCode loadContainers();

    private:

        const EventInfo* m_eventInfo;
        const CondAttrListCollection* m_dbPpmDeadChannels;
        const CondAttrListCollection* m_dbPpmDisabledTowers;
	const CondAttrListCollection* m_dbFineTimeRefsTowers;
        const xAOD::TriggerTowerContainer* m_triggerTowers;
        ServiceHandle<ITHistSvc> m_histoSvc;
	
	// monitoring plotters
	L1CaloPprFineTimePlotManager* m_fineTimePlotManager;
	L1CaloPprPedestalPlotManager* m_pedestalPlotManager;
	L1CaloPprPedestalCorrectionPlotManager* m_pedestalCorrectionPlotManager;
	L1CaloPprEtCorrelationPlotManager* m_etCorrelationPlotManager;
 
        ToolHandle<LVL1::IL1CaloOfflineTriggerTowerTools> m_offlineTowerTools;
	ToolHandle<LVL1::IL1TriggerTowerTool> m_towerTools;

        std::string m_triggerTowersLocation;
        std::string m_dbPpmDeadChannelsFolder;
        std::string m_dbPpmDisabledTowersFolder;
        std::string m_dbFineTimeRefsFolder;

        unsigned int m_PpmAdcMinValue;  // cut on ADC value, used for fine time monitoring
        unsigned int m_PpmAdcMaxValue;  // cut on ADC value, used for fine time monitoring 
        unsigned int m_lumiBlockMax;    // max. number of lumi blocks plotted in histograms
	
	// decide which plots to do
	bool m_doFineTimePlots;       // do plots of ADC peak sampling jitter (fine time)
	bool m_doPedestalPlots;       // do plots of non signal behaviour (pedestals)
	bool m_doPedestalCorrectionPlots;  // do plots of the pedestal correction
	bool m_doEtCorrelationPlots;  // do plots of correlation between Et as given by L1Calo
	                              // and Et reconstructed using calorimeter cells
	
        double m_fineTimeCut;      // plotting fine time only for absolute values
	                           // smaller than cut value
        bool m_doCaloQualCut;	   // handle to switch on and off the calo quality cut (and
				   // usage of the Calo Containers via job options
	double m_pedestalMaxWidth; // plotting pedestal only if for every slice
	                           // the deviation from the mean pedestal
	                           // value is less
	std::string m_caloCellContainerName; // name of calo cell container used in
	                                     // Et correlation monitoring
	double m_EtMinForEtCorrelation;      // threshold of L1Calo and calo cell energies
	                                     // in Et correlation monitoring
};

#endif
