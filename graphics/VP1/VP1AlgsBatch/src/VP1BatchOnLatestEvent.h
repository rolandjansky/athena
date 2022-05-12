/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1BatchOnLatestEvent            //
//                                                         //
//  Riccardo-Maria BIANCHI <rbianchi@cern.ch>              //
//  11 Mar 2015                                            //
//                                                         //
//  This is the Athena algorithm to launch VP1 in          //
//  batch-mode, to automatically create 3D event           //
//  displays on-the-fly.                                   //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1ALGSBATCH_VP1BATCHONLATESTEVENT
#define VP1ALGSBATCH_VP1BATCHONLATESTEVENT

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include <string>

class VP1BatchOnLatestEvent final: public AthAlgorithm,
public IIncidentListener
{
public:
	VP1BatchOnLatestEvent(const std::string& name, ISvcLocator* pSvcLocator);

	virtual StatusCode initialize() override;
	virtual StatusCode execute() override;
	virtual StatusCode finalize() override;

	virtual void handle(const Incident& inc) override;


private:
	std::string getRandomConfigFile();
	void overlayATLASlogo();
	void overlayEventDetails();
	void getHumanReadableTimestamp();
	void makeEventDisplay();

	// run/event number to be used in the vp1 event file name
	//  int m_runNumber;
	//  int m_eventNumber;
	//  unsigned int m_timeStamp;
	//  std::string m_humanTimestamp;

	// properties
	std::string m_inputVP1CfgFile;
	std::string m_inputDir;
	std::string m_destinationDir;
	bool m_isGetRandomFile;

	SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey {this, "EventInfo", "EventInfo", "EventInfo Key"};

	//  int m_maxProducedFiles;
	int m_nEvent; // Internal counter for the number of processed events
	int m_indexFile;
	int m_lastIndexFile;

	uint64_t m_eventNumber = 0;
	unsigned long m_runNumber = 0;
	unsigned long m_timeStamp = 0;
	std::string m_humanTimestamp;
};

#endif
