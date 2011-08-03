/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGCALOVALIDATION_TRIGFEBVALIDATION_H
#define TRIGCALOVALIDATION_TRIGFEBVALIDATION_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRecEvent/LArFebEnergyCollection.h"


class StoreGateSvc;
class MsgStream;
class ITrigDataAccess;
class LArOnlineID;
class LArCablingService;
class ICaloNoiseTool;
//class LArFebEnergyCollection;
class IAlgToolEFCalo;
class HWIdentifier;
class LArFebEnergy;

class TrigFEBValidation : public Algorithm
{

public:
	TrigFEBValidation(const std::string& name,
		ISvcLocator* pSvcLocator);
	~TrigFEBValidation();

	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private:
	StoreGateSvc* m_storeGate;
	MsgStream* m_log;

	/** TrigDataAccess for calorimeter data unpacking. */
	//ToolHandle<ITrigDataAccess>     m_data;
        ITrigDataAccess *m_data;
	const LArOnlineID * m_onlineHelper;
	ToolHandle<LArCablingService>     m_cablingSvc;
	/** Noise tool for offline cell by cell check */
	ToolHandle<ICaloNoiseTool> m_noisetool;

	/** energy threshold used at the DSP */
	float m_threshold;
	/** number of sigma to check that a cell has signal (absolute threshold above used if sigma==0 */
	float m_nsigma;

        bool m_useloadfullcoll;

        float m_difflimit;

	LArFebEnergyCollection m_larfebcol;
	LArFebEnergyCollection m_larfebcol_hec;
	LArFebEnergyCollection m_larfebcol_em;
	LArFebEnergyCollection m_larfebcol_fcal;
	LArFebEnergyCollection::const_iterator tbegin;
	LArFebEnergyCollection::const_iterator tend;
	 /** EF Calorimeter tools for calorimeter data unpacking. */
        ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;


        void doComparison(LArFebEnergyCollection *febcoll, std::map<HWIdentifier,LArFebEnergy> *larfebmap,int &count,int &count_not_found,int &countp,double &total,double &totalx,double &totaly,double &totalz,double &x,double &y,double &z);
};

#endif
