/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZdcRec.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ZDCREC_H
#define ZDCREC_H

#include <string>
#include <map>



//which one ???
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"



//#include "ZdcRec/ZdcRecNoiseTool.h"
//#include "ZdcRec/ZdcRecChannelTool.h"

class ISvcLocator;
class StatusCode;
class Identifier;
class StoreGateSvc;
class ZdcDigitsCollection;
class ZdcCalibEvent;
class ZdcRawChannelCollection;
class ZdcRecChannelTool;
class ZdcRecNoiseTool;


/** @class ZdcRec

    Class definition for the ZDC Reconstruction class

    @author  Brian Cole and Peter Steinberg, bcole@cern.ch, peter.steinberg@bnl.gov
             and M. Leite leite@cern.ch
*/


class ZdcRec : public AthAlgorithm
{

public:

	ZdcRec(const std::string& name, ISvcLocator* pSvcLocator);
	~ZdcRec();

	StatusCode initialize();
	StatusCode execute();
	StatusCode finalize();

private:

  /** class member version of retrieving StoreGate */
	//StoreGateSvc* m_storeGate;
	 ServiceHandle<StoreGateSvc> m_storeGate;


	/** Does the collection own it's objects ? **/
	int m_ownPolicy;

	/** Digits data container name */
	std::string m_digitsContainerName;

	/** Raw data object name */
	std::string m_rawContainerName;

	/** Pointer to Zdc input "digits" data */
	const ZdcDigitsCollection* m_digitsCollection;

	/** Pointer to Zdc output "raw" reconstructed data */
	ZdcRawChannelCollection* m_rawCollection;

	int m_eventCount;
	bool m_complainContain;
	bool m_complainRetrieve;

	//Include here all tools to do the job. They will be called by the algorithm execute method
	//Another option is to use ToolHandleArray<IZdcRecTool>, where IZdcRecTool is the factory for
	//the tools
	ToolHandle<ZdcRecChannelTool> m_ChannelTool;
	ToolHandle<ZdcRecNoiseTool>   m_NoiseTool;
};

#endif
