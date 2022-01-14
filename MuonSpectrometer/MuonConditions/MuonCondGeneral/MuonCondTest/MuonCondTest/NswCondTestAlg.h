/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NswCondTestAlg_H
#define NswCondTestAlg_H 

//STL
#include <string>
#include <chrono>

//Athena
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonCondData/NswCalibDbTimeChargeData.h"
#include "MuonCondData/NswCalibDbThresholdData.h"


//Forward declarations
class ISvcLocator;
class StatusCode;


class NswCondTestAlg : public AthReentrantAlgorithm {

public:
	NswCondTestAlg(const std::string &name, ISvcLocator *pSvcLocator) ;
	virtual ~NswCondTestAlg() override;
	
	virtual StatusCode initialize() override;
	virtual StatusCode execute(const EventContext&) const override;
	virtual StatusCode finalize() override;
   
private:
	StatusCode retrieveTdoPdo(const EventContext&, const std::string&, const std::string&, const std::string&, std::chrono::duration<double>&) const;
	StatusCode retrieveVmm(const EventContext&, const std::string&, const std::string&, std::chrono::duration<double>&) const;
	std::string timestamp() const; 

 	SG::ReadCondHandleKey<NswCalibDbTimeChargeData> m_readKey_tdopdo{this, "ReadKey_tdopdo", "NswCalibDbTimeChargeData", "Key of NswCalibDbTimeChargeData object containing calibration data (TDOs and PDOs)"  };
 	SG::ReadCondHandleKey<NswCalibDbThresholdData > m_readKey_vmm   {this, "ReadKey_vmm"   , "NswCalibDbThresholdData" , "Key of NswCalibDbThresholdData object containing calibration data (VMM thresholds)"  };


}; //end of class

#endif
