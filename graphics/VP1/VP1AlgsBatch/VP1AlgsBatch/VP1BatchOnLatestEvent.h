/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#ifndef VP1ALGS_VP1BatchOnLatestEvent
#define VP1ALGS_VP1BatchOnLatestEvent

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include <string>

// FWD
class EventInfo;

class VP1BatchOnLatestEvent: public AthAlgorithm,
public IIncidentListener
{
public:
	VP1BatchOnLatestEvent(const std::string& name, ISvcLocator* pSvcLocator);
	~VP1BatchOnLatestEvent();

	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

	void handle(const Incident& inc);


private:
	std::string getRandomConfigFile();
	void overlayATLASlogo();
	void overlayEventDetails();
	void getEventDetails();
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

	bool m_evtInfoDone;
	const EventInfo* m_eventInfo;

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
