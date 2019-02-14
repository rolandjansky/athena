/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HLTCALOTOOL_H
#define HLTCALOTOOL_H

// Simple description to come

class StoreGateSvc;
class IAlgToolEFCalo;
class LArCablingLegacyService;
class LArOnlineID;
class TileID;

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "GaudiKernel/ToolHandle.h"

class HLTCaloTool : public IHLTMonTool {

public:
	// Constructor
	HLTCaloTool (const std::string & type, const std::string & name, const IInterface* parent);
	virtual ~HLTCaloTool();
	StatusCode init(); // Empty for the moment.
#ifdef ManagedMonitorToolBase_Uses_API_201401
	StatusCode book(); // called by bookHistograms
#else
	StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); // called by bookHistograms
#endif
	StatusCode fill(); // called by fillHistograms
#ifdef ManagedMonitorToolBase_Uses_API_201401
        StatusCode proc() ; // called by procHistograms
#else
	StatusCode proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool /*endOfRun*/) ; // called by procHistograms
#endif

private:
	/** EF Calorimeter tools for calorimeter data unpacking. */
	ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;
	/** use or not ntuples */
	bool m_ntuple;
	const LArOnlineID * m_onlineHelper;
        ToolHandle<LArCablingLegacyService>     m_cablingSvc;
        const TileID* m_tileID;


};

#endif
