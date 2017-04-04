/*
* Author: Piotr Sarna
*/

#ifndef AthenaMonitoring_GenericMonitoringToolTest_h
#define AthenaMonitoring_GenericMonitoringToolTest_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

class GenericMonitoringToolTest : public AthAlgorithm {
public:
	GenericMonitoringToolTest(const std::string& name, ISvcLocator* pSvcLocator);
	StatusCode initialize();
	StatusCode beginRun();
	StatusCode execute();
	StatusCode finalize();

private:
    ToolHandle<GenericMonitoringTool> m_monitoringTool;
};

#endif
