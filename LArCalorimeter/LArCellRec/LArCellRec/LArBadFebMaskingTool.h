/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolHandle.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/ReadHandleKey.h"

class LArCablingService;
class StoreGateSvc;
class CaloCell_ID;
class LArOnlineID;
class ILArBadChanTool;
class LArFebErrorSummary;

class LArBadFebMaskingTool: public AthAlgTool,
	             virtual public ICaloCellMakerTool 

{
 
public:    
  
  LArBadFebMaskingTool(const std::string& type, 
		const std::string& name, 
				 const IInterface* parent) ;


  /** initialize the tool
  */
  virtual StatusCode initialize() ; 

  /** finalize   the tool
  */
  virtual StatusCode finalize() ; 

  /** update theCellContainer, masking Feb with errors
  */
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);



 private:

  /** handle to bad channel tool (to get problematic Feb into)
  */
  ToolHandle<ILArBadChanTool> m_badChannelTool;
  /** handle to LAr cabling service
  */
  ToolHandle<LArCablingService> m_cablingService;
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

  /** compute bit mask of errors to mask
  */
  uint16_t m_errorToMask;

  /** pointers to identifier helpers
  */
  const CaloCell_ID* m_calo_id;
  const LArOnlineID* m_onlineID;

  /** Number of events processed
  */
  int m_evt;

  /** Number of Feb masked
  */
  int m_mask;

};

#endif

