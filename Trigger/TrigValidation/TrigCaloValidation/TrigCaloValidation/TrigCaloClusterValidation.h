/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGCALOCLUSTERVALIDATION_TRIGCALOCLUSTERVALIDATION_H
#define TRIGCALOCLUSTERVALIDATION_TRIGCALOCLUSTERVALIDATION_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IToolSvc.h"

class StoreGateSvc;
class MsgStream;
class IAlgToolEFCalo;

class TrigCaloClusterValidation : public Algorithm
{

public:
	TrigCaloClusterValidation(const std::string& name,
		ISvcLocator* pSvcLocator);
	~TrigCaloClusterValidation();

	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();


private:
	StoreGateSvc* m_storeGate;
	MsgStream* m_log;
        std::string m_CaloClusterOfflineContainerName;
        std::string m_CaloClusterL2OnlineContainerName;
        std::string m_CaloClusterEFOnlineContainerName;
	float m_EtCut;
	float m_DeltaRCut;
	float deltaR( float, float, float, float);
	float deltaPhi( float, float);
	/** EF Calorimeter tools for calorimeter data unpacking. */
	ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;


};

#endif
