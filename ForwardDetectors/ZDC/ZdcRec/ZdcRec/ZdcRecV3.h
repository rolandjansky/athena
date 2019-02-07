/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZdcRec.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ZDCRECV3_H
#define ZDCRECV3_H

#include <string>
#include <map>



//which one ???
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"



//#include "ZdcRec/ZdcRecNoiseTool.h"
//#include "ZdcRec/ZdcRecChannelTool.h"

class ISvcLocator;
class StatusCode;
class Identifier;
class StoreGateSvc;

class ZdcRecChannelToolV2;
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ZdcModuleAuxContainer.h"
#include "ZdcAnalysis/ZdcAnalysisTool.h"

/** @class ZdcRec

    Class definition for the ZDC Reconstruction class

    @author  Brian Cole and Peter Steinberg, bcole@cern.ch, peter.steinberg@bnl.gov
             and M. Leite leite@cern.ch
*/


class ZdcRecV3 : public AthAlgorithm
{

public:

	ZdcRecV3(const std::string& name, ISvcLocator* pSvcLocator);
	virtual ~ZdcRecV3();

	virtual StatusCode initialize() override;
	virtual StatusCode execute() override;
	virtual StatusCode finalize() override;

private:
	/** Digits data container name */
        SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_ttContainerName
        { this, "DigitsContainerName", "ZdcTriggerTowers", "" };

	/** Raw data object name */
        SG::WriteHandleKey<xAOD::ZdcModuleContainer> m_zdcModuleContainerName
        { this, "ZdcModuleContainerName", "ZdcModules", "" };


	//Include here all tools to do the job. They will be called by the algorithm execute method
	//Another option is to use ToolHandleArray<IZdcRecTool>, where IZdcRecTool is the factory for
	//the tools
	ToolHandle<ZdcRecChannelToolV2> m_ChannelTool
        { this, "ChannelTool", "ZdcRecChannelToolV2", "" };
	ToolHandle<ZDC::IZdcAnalysisTool> m_zdcTool
        { this, "ZdcAnalysisTool", "ZDC::ZdcAnalysisTool/ZdcAnalysisTool", "" };
};

#endif
