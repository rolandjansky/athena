/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Author: Piotr Sarna
 */

#ifndef L1Decoder_FakeRoI_h
#define L1Decoder_FakeRoI_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigT1Result/RoIBResult.h"

//#include "AthViews/View.h"

class FakeRoI : public AthAlgorithm {
private:
	struct SingleFakeRoI {
		double eta;
		double phi;
		uint32_t word;
		std::vector<std::string> passedThresholdIDs;
	};
public:
	FakeRoI(const std::string& name, ISvcLocator* pSvcLocator);
    virtual StatusCode initialize() override;
	virtual StatusCode start() override;
	virtual StatusCode execute() override;
	virtual StatusCode finalize() override;
private:
	std::vector<std::vector<SingleFakeRoI>> parseInputFile();
	std::vector<SingleFakeRoI> parseInputFileLine(const std::string& line, unsigned lineNumber);
	SingleFakeRoI parseInputRoI(const std::string& roi, unsigned lineNumber, unsigned roiNumber);

	std::vector<TrigConf::TriggerThreshold*> m_emtauThresholds;
	

	SG::WriteHandle< TrigRoiDescriptorCollection > m_trigRoIs;
	SG::WriteHandle< DataVector<LVL1::RecEmTauRoI> > m_recEMTauRoIs;
	SG::WriteHandle< xAOD::TrigCompositeContainer > m_decisions;
	SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_decisionsAux;
	//	SG::WriteHandle< std::vector<SG::View*> > m_view;
	ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc; //!< access to trigger menu

	// can demand objects 

	std::string m_inputFilename;
	std::vector<std::vector<FakeRoI::SingleFakeRoI>> m_inputData;
	unsigned m_currentRowNumber;
    ToolHandle<GenericMonitoringTool> m_monTool;
};

#endif
