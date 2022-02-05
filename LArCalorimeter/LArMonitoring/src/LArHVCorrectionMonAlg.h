/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArHVCorrectionMonAlg
 * @author Pavol Strizenec <pavol@mail.cern.ch>
 * based on Tool created by Jessica Leveque <jleveque@in2p3.fr>
 *
 */

#ifndef LARMONITORING_LARHVCORRECTIONMONALG_H
#define LARMONITORING_LARHVCORRECTIONMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"

#include <string>
#include <map>
#include <mutex>
#include <set>

class LArOnlineID;
class CaloDetDescrElement;


class LArHVCorrectionMonAlg: public AthMonitorAlgorithm
{
 public:
  LArHVCorrectionMonAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Default destructor */
  virtual ~LArHVCorrectionMonAlg() = default;

  /** @brief Overwrite dummy method from AlgTool */
  virtual StatusCode initialize() override final;

  /** Called each event */
  virtual StatusCode fillHistograms(const EventContext& ctx)  const override final;

 private:

  // Properties
  Gaudi::Property<std::string> m_MonGroupName {this, "HVCorrMonGroupName","LArHVCorrMonGroup"};
  Gaudi::Property<float> m_threshold{this, "ErrorThreshold", 0.02};
  Gaudi::Property<float> m_delta_eta{this, "EtaGranularity", 0.01}; 
  Gaudi::Property<float> m_delta_phi{this, "PhiGranularity", 0.01};
  Gaudi::Property<int> m_nlumi_blocks{this, "NumberOfLBs", 1500};

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping CDO"};
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineScaleCorrKey { this, "OnlineLArHVScaleCorr", "LArHVScaleCorr", "" };
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this, "CaloDetDescrManager", "CaloDetDescrManager", "SG Key for CaloDetDescrManager in the Condition Store" };

  // To retrieve bad channel DB keywords 
  int DBflag(HWIdentifier onID);

  // services
  const LArOnlineID* m_LArOnlineIDHelper;
  const CaloIdManager*       m_caloIdMgr;

  // Other things
  // FIXME: How to find new LB, and fill only once per LB ?
  mutable std::set<unsigned int> m_filledLB ATLAS_THREAD_SAFE;
  mutable std::mutex m_mut ATLAS_THREAD_SAFE;

};

#endif
