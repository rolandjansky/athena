/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTCALOFEBTOOL_H
#define HLTCALOFEBTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRecEvent/LArFebEnergyCollection.h"

// Simple description to come

class StoreGateSvc;
class IAlgToolEFCalo;
class LArCablingLegacyService;
class LArOnlineID;
class MsgStream;
class ITrigDataAccess;
class ICaloNoiseTool;
//class LArFebEnergyCollection;
class HWIdentifier;
class LArFebEnergy;


class HLTCaloFEBTool : public IHLTMonTool {

public:
	// Constructor
	HLTCaloFEBTool (const std::string & type, const std::string & name, const IInterface* parent);
	virtual ~HLTCaloFEBTool();
	StatusCode init(); // Empty for the moment.
#ifdef ManagedMonitorToolBase_Uses_API_201401
	StatusCode book(); // called by bookHistograms
#else
	StatusCode book(bool newEventsBlock, bool newLumiBlock, bool newRun); // called by bookHistograms
#endif
	StatusCode fill(); // called by fillHistograms
#ifdef ManagedMonitorToolBase_Uses_API_201401
        StatusCode proc() { return StatusCode::SUCCESS; } // called by procHistograms
#else
	StatusCode proc(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool /*endOfRun*/) { return StatusCode::SUCCESS; } // called by procHistograms
#endif

private:
	/** EF Calorimeter tools for calorimeter data unpacking. */
	ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;
	/** use or not ntuples */
	bool m_ntuple;
	const LArOnlineID * m_onlineHelper;
        ToolHandle<LArCablingLegacyService>     m_cablingSvc;
        ITrigDataAccess *m_data;
        /** Noise tool for offline cell by cell check */
        ToolHandle<ICaloNoiseTool> m_noisetool;

        /** energy threshold used at the DSP */
        float m_threshold;
        /** number of sigma to check that a cell has signal (absolute threshold above used if sigma==0 */
        float m_nsigma;

        bool m_useloadfullcoll;

        float m_difflimit;
        std::vector<LArFebEnergy> m_larfebcol;
        std::vector<LArFebEnergy> m_larfebcol_hec;
        std::vector<LArFebEnergy> m_larfebcol_em;
        std::vector<LArFebEnergy> m_larfebcol_fcal;
        /*LArFebEnergyCollection m_larfebcol;
        LArFebEnergyCollection m_larfebcol_hec;
        LArFebEnergyCollection m_larfebcol_em;
        LArFebEnergyCollection m_larfebcol_fcal;*/
        LArFebEnergyCollection::const_iterator m_tbegin;
        LArFebEnergyCollection::const_iterator m_tend;
         /** EF Calorimeter tools for calorimeter data unpacking. */

        void doComparison(std::vector<LArFebEnergy> &febcoll, std::map<HWIdentifier,std::pair<LArFebEnergy,LArFebEnergy> > *larfebmap,int &count,int &count_not_found,int &countp,double &total,double &totalx,double &totaly,double &totalz,double &x,double &y,double &z);


};

#endif
