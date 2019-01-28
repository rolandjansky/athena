/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDUTILS_LARHVTOOLMC_H
#define LARCONDUTILS_LARHVTOOLMC_H

/**
 @class LArHVToolMC 
 @brief concrete Tool to provide HV information for readout cells for MC

 Environment:
     Software developed for the ATLAS Detector at the CERN LHC

 @author 
     Guillaume Unal <unal@lal.in2p3.fr>

*/


// Includes for Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "LArElecCalib/ILArHVTool.h" 
#include "StoreGate/DataHandle.h" 
#include "Identifier/HWIdentifier.h"

// forward declaration
class LArEM_ID;
class CaloIdManager;

class LArHVToolMC: public AthAlgTool, virtual public ILArHVTool
{
 
  public: 
  LArHVToolMC(const std::string& type, const std::string& name,
                      const IInterface* parent);

  virtual ~LArHVToolMC();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}


  // Given a Offline Readout ID, return values of HV and Weight 
  virtual StatusCode getHV(const Identifier& id,
         std::vector< HV_t > & v  ) const override ;

  // Given a Offline Readout ID, return values of Current and Weight
  virtual StatusCode getCurrent(const Identifier& id,
         std::vector< CURRENT_t > & v  ) const override ;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) override;

  virtual const std::vector<HWIdentifier>& getUpdatedElectrodes() override;

  private:

  bool m_readASCII;

  double m_hv[2][1024][7][2];

  const LArEM_ID*   m_larem_id;
  const DataHandle<CaloIdManager> m_caloIdMgr;

  void InitHV();

  std::vector<HWIdentifier> m_updatedElectrodes;
};


inline 
const std::vector<HWIdentifier>&  LArHVToolMC::getUpdatedElectrodes() { 
  return m_updatedElectrodes;
}
#endif
