/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCDIGITTOCSCRDOTOOL_H
#define CSCDIGITTOCSCRDOTOOL_H

#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "CLHEP/Random/RandomEngine.h"

#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonDigitContainer/CscDigitContainer.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "CSCcabling/CSCcablingSvc.h"

#include "AthenaKernel/IAthRNGSvc.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "CscCalibTools/ICscCalibTool.h"


class ActiveStoreSvc;

/////////////////////////////////////////////////////////////////////////////

class CscDigitToCscRDOTool final : public extends<AthAlgTool, IMuonDigitizationTool> {

 public:

  CscDigitToCscRDOTool (const std::string& type, const std::string& name, const IInterface* pIID);
  virtual ~CscDigitToCscRDOTool() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode digitize(const EventContext& ctx) override;

 private:

  StatusCode fill_CSCdata();
  CscRawDataCollection * cscRdo(uint16_t subDetectorId, uint16_t rodId) ;

  std::map<uint16_t, CscRawDataCollection *> m_cscRdoMap;
  uint16_t m_samplingRate{}; //!< set during initialize from cscCalibTool
  uint16_t m_numSamples{4};
  uint16_t m_latency{0};
  double m_startTime{}; //!< set during initialize from cscCalibTool
  double m_signalWidth{}; //!< set during initialize from cscCalibTool
  bool m_addNoise{true};
  uint16_t m_samplingTime; //!< set during initialize from cscCalibTool
  uint16_t m_numberOfIntegration{}; //!< set during initialize from cscCalibTool

 protected:

  SG::WriteHandleKey<CscRawDataContainer> m_rdoContainerKey{this,"OutputObjectName","CSCRDO","WriteHandleKey for Output CswRawDataContainer"};
  SG::ReadHandleKey<CscDigitContainer> m_digitContainerKey{this,"InputObjectName","CSC_DIGITS","ReadHandleKey for Input CscDigitContainer"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ServiceHandle<CSCcablingSvc> m_cscCablingSvc{this, "CSCcablingSvc", "CSCcablingSvc", ""};
  ToolHandle<ICscCalibTool> m_cscCalibTool{this, "cscCalibTool", "CscCalibTool", ""};
  ServiceHandle<IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service
};

#endif
