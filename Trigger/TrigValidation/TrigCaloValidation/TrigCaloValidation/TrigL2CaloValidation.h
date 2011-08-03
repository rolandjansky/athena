/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGCALOVALIDATION_TRIGL2CALOVALIDATION_H
#define TRIGCALOVALIDATION_TRIGL2CALOVALIDATION_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloEvent/CaloCellContainer.h"

class StoreGateSvc;
class MsgStream;

class TrigL2CaloValidation : public Algorithm
{

public:
	TrigL2CaloValidation(const std::string& name,
		ISvcLocator* pSvcLocator);
	~TrigL2CaloValidation();

	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private:
	void LoopOver(const CaloCellContainer* L2Calo, const CaloCellContainer*
	 AllCalo,int& count_tcellsL, int & count_tcellspL, int& count_tcellsT,
	 int & count_tcellspT);
	StoreGateSvc* m_storeGate;
	MsgStream* m_log;
	std::string m_L2egname;
	std::string m_L2taname;
	std::string m_L2jename;

};

#endif
