/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGCALOVALIDATION_TRIGCALOVALIDATION_H
#define TRIGCALOVALIDATION_TRIGCALOVALIDATION_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"


class StoreGateSvc;
class MsgStream;
class IAlgToolEFCalo;

class TrigCaloValidation : public Algorithm
{

public:
	TrigCaloValidation(const std::string& name,
		ISvcLocator* pSvcLocator);
	~TrigCaloValidation();

	StatusCode initialize();
	StatusCode finalize();
	StatusCode execute();

private:
	StoreGateSvc* m_storeGate;
	MsgStream* m_log;

	/** EF Calorimeter tools for calorimeter data unpacking. */
	ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;


};

#endif
