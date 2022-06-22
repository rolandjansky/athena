/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LArBadFebMaskingTool_H
#define LARCELLREC_LArBadFebMaskingTool_H

/** 
@class LArBadFebMaskingTool
@brief Mask LAr cells in Febs with decoding errors or listed in the bad feb database

 AlgTool properties (name defined in cxx file): 
     Handle for bad channel tool name
     Switches to decide which Feb errors to mask
     Removed setting LAr errors bit in EventInfo, moved to LArFebErrorSummaryMaker

 Created April 23, 2009  G.Unal
*/



#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRawEvent/LArFebErrorSummary.h" 
#include <atomic>

class CaloCell_ID;
class LArOnlineID;
class LArFebErrorSummary;

class LArBadFebMaskingTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:    

  /**Delegate constructor
   */
  
  using base_class::base_class;


  /** initialize the tool
  */
  virtual StatusCode initialize() override;

  /** finalize   the tool
  */
  virtual StatusCode finalize() override;

  /** update theCellContainer, masking Feb with errors
  */
  virtual StatusCode process (CaloCellContainer * theCellContainer,
                              const EventContext& ctx) const override;

 private:

  /** handle to get bad febs
  */
  SG::ReadCondHandleKey<LArBadFebCont> m_badFebKey{this,"BadFebKey","LArBadFeb","Key of Bad-Feb object"};
  /** handle to LAr cabling    
  */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  /** flags to select which errors to mask
  */
  Gaudi::Property<bool> m_maskParity{this,"maskParity",true};
  Gaudi::Property<bool> m_maskSampleHeader{this,"maskSampleHeader",true};
  Gaudi::Property<bool> m_maskEVTID{this,"maskEVTID",true};
  Gaudi::Property<bool> m_maskScacStatus{this,"maskScacStatus",true};
  Gaudi::Property<bool> m_maskScaOutOfRange{this,"maskScaOutOfRange",true};
  Gaudi::Property<bool> m_maskGainMismatch{this,"maskGainMismatch",true};
  Gaudi::Property<bool> m_maskTypeMismatch{this,"maskTypeMismatch",true};
  Gaudi::Property<bool> m_maskNumOfSamples{this,"maskNumOfSamples",true};
  Gaudi::Property<bool> m_maskEmptyDataBlock{this,"maskEmptyDataBlock",true};
  Gaudi::Property<bool> m_maskDspBlockSize{this,"maskDspBlockSize",true};
  Gaudi::Property<bool> m_maskCheckSum{this,"maskCheckSum",true};
  Gaudi::Property<bool> m_maskMissingHeader{this,"maskMissingHeader",true};
  Gaudi::Property<bool> m_maskBadGain{this,"maskBadGain",true};

  SG::ReadHandleKey<LArFebErrorSummary> m_larFebErrorSummaryKey{this,"FebErrorSummaryKey","LArFebErrorSummary"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo"};

  /** compute bit mask of errors to mask
  */
  uint16_t m_errorToMask=0;

  /** pointers to identifier helpers
  */
  const CaloCell_ID* m_calo_id=nullptr;
  const LArOnlineID* m_onlineID=nullptr;

  /** Number of events processed
  */
  mutable std::atomic<int> m_evt{0};

  /** Number of Feb masked
  */
  mutable std::atomic<int> m_mask{0};
};

#endif

