/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCDIGITTOCSCRDOTOOL_H
#define CSCDIGITTOCSCRDOTOOL_H

#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CLHEP/Random/RandomEngine.h"

#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonDigitContainer/CscDigitContainer.h"

#include "MuonIdHelpers/CscIdHelper.h"
#include "CSCcabling/CSCcablingSvc.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "CscCalibTools/ICscCalibTool.h"

class ActiveStoreSvc;

/////////////////////////////////////////////////////////////////////////////

class CscDigitToCscRDOTool : virtual public IMuonDigitizationTool, public AthAlgTool  {

 public:

  CscDigitToCscRDOTool (const std::string& type, const std::string& name, const IInterface* pIID);
  ~CscDigitToCscRDOTool() {}

  virtual StatusCode initialize() override;
  virtual StatusCode digitize() override;

 private:

  std::map<uint16_t, CscRawDataCollection *> m_cscRdoMap;
  uint16_t m_samplingRate;
  uint16_t m_numSamples;
  uint16_t m_latency;
  double m_startTime;
  double m_signalWidth;
  bool m_addNoise;
  
 private:

  StatusCode fill_CSCdata();
  CscRawDataCollection * cscRdo(uint16_t subDetectorId, uint16_t rodId) ;
  uint16_t m_samplingTime;   
  uint16_t m_numberOfIntegration;

 protected:

  SG::WriteHandleKey<CscRawDataContainer> m_rdoContainerKey{this,"OutputObjectName","CSCRDO","WriteHandleKey for Output CswRawDataContainer"};
  SG::ReadHandleKey<CscDigitContainer> m_digitContainerKey{this,"InputObjectName","CSC_DIGITS","ReadHandleKey for Input CscDigitContainer"};
  const CscIdHelper   * m_cscHelper;
  ServiceHandle<CSCcablingSvc> m_cscCablingSvc;
  ToolHandle<ICscCalibTool>  m_cscCalibTool;

  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine
};

#endif
