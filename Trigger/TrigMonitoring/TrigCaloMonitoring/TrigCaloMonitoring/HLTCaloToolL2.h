/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HLTCALOTOOLL2_H
#define HLTCALOTOOLL2_H

// Simple description to come

class StoreGateSvc;
class LArCablingLegacyService;
class LArOnlineID;
class TileID;

#include "TrigHLTMonitoring/IHLTMonTool.h"

class HLTCaloToolL2 : public IHLTMonTool {

public:
	// Constructor
	HLTCaloToolL2 (const std::string & type, const std::string & name, const IInterface* parent);
	virtual ~HLTCaloToolL2();
	StatusCode init(); // Empty for the moment.
#ifdef ManagedMonitorToolBase_Uses_API_201401
	StatusCode book(); // called by bookHistograms
#else
	StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); // called by bookHistograms
#endif
	StatusCode fill(); // called by fillHistograms
#ifdef ManagedMonitorToolBase_Uses_API_201401
        StatusCode proc(); // called by procHistograms
#else
	StatusCode proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool /*endOfRun*/); // called by procHistograms
#endif

private:
	/** use or not ntuples */
	bool m_ntuple;
        /** List of keys to search for cell containers */
	std::vector<std::string > m_listOfCellsKeys;
        const LArOnlineID * m_onlineHelper;
        ToolHandle<LArCablingLegacyService>     m_cablingSvc;
        const TileID* m_tileID;


};

#endif
