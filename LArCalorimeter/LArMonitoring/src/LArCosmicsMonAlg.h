/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCosmicsMonAlg
 * @author Margherita Spalla <margherita.spalla@cern.ch> (migrated from LArMonTools by Jessica Leveque <jleveque@in2p3.fr>)
 *
 */

#ifndef LARMONITORING_LARCOSMICSMONALG_H
#define LARMONITORING_LARCOSMICSMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "Identifier/HWIdentifier.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArCabling/LArOnOffIdMapping.h"

class LArEM_ID;
class LArOnlineID;




class LArCosmicsMonAlg: public AthMonitorAlgorithm
{
 public:
  LArCosmicsMonAlg( const std::string& name, ISvcLocator* pSvcLocator );
  
  /** @brief Default destructor */
  virtual ~LArCosmicsMonAlg();

  /** @brief Overwrite dummy method from AlgTool */
  StatusCode initialize() override;


  /** Called each event */
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;


 private:

  // services
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID*    m_LArEM_IDHelper;
  const LArHEC_ID*   m_LArHEC_IDHelper;
  const LArFCAL_ID*  m_LArFCAL_IDHelper;


  /** Handle to digits */
  SG::ReadHandleKey<LArDigitContainer> m_LArDigitContainerKey{this,"LArDigitContainerKey","FREE","SG key of LArDigitContainer read from Bytestream"};

  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_larPedestalKey{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};

  //** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "CablingKey", "LArOnOffIdMap","Cabling key"};

  /** Handle to bad-channel mask */
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;

  //properties
  Gaudi::Property<float>         m_muonADCthreshold_EM_barrel  {this, "MuonADCthreshold_EM_barrel", 30};
  Gaudi::Property<float>         m_muonADCthreshold_EM_endcap  {this, "MuonADCthreshold_EM_endcap", 40};
  Gaudi::Property<float>         m_muonADCthreshold_HEC  {this, "MuonADCthreshold_HEC", 40};
  Gaudi::Property<float>         m_muonADCthreshold_FCAL  {this, "MuonADCthreshold_FCAL", 40};

  Gaudi::Property<std::string> m_CosmicsMonGroupName {this,"CosmicsMonGroupName","CosmicsMon"};
  Gaudi::Property< std::vector<std::string> > m_SubDet {this, "SubDet", {"EM","HEC","FCal"}};

};

#endif
