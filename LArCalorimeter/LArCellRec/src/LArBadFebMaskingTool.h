/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LArBadFebMaskingTool_H
#define LARCELLREC_LArBadFebMaskingTool_H

/** 
@class LArBadFebMaskingTool
@brief Mask LAr cells in Febs with decoding errors or listed in the bad feb database

 AlgTool properties (name defined in cxx file): 
     Handle for bad channel tool name
     Switches to decide which Feb errors to mask

 Created April 23, 2009  G.Unal
*/



#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "xAODEventInfo/EventInfo.h"
#include <atomic>

class CaloCell_ID;
class LArOnlineID;
class LArFebErrorSummary;

class LArBadFebMaskingTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:    
  
  LArBadFebMaskingTool(const std::string& type, 
		const std::string& name, 
				 const IInterface* parent) ;


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
  bool m_maskParity;
  bool m_maskSampleHeader;
  bool m_maskEVTID;
  bool m_maskScacStatus;
  bool m_maskScaOutOfRange;
  bool m_maskGainMismatch;
  bool m_maskTypeMismatch;
  bool m_maskNumOfSamples;
  bool m_maskEmptyDataBlock;
  bool m_maskDspBlockSize;
  bool m_maskCheckSum;
  bool m_maskMissingHeader;
  bool m_maskBadGain;

  /**  Minimum number of FEBs in error to trigger EventInfo::LArError 
       Defined as 1 by default/bulk, 4 in online/express in CaloCellGetter (CaloRec package)
  */
  int m_minFebsInError; 
  /** key of larFebErrorSummary in storegate
  */
  //std::string m_larFebErrorSummaryKey;
  SG::ReadHandleKey<LArFebErrorSummary> m_larFebErrorSummaryKey;
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey;

  /** compute bit mask of errors to mask
  */
  uint16_t m_errorToMask;

  /** pointers to identifier helpers
  */
  const CaloCell_ID* m_calo_id;
  const LArOnlineID* m_onlineID;

  /** Number of events processed
  */
  mutable std::atomic<int> m_evt;

  /** Number of Feb masked
  */
  mutable std::atomic<int> m_mask;
};

#endif

