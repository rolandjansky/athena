/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawDataProviderTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h" 
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "StoreGate/ReadHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

/// -------------------------------------------------------
/// contructor

SCTRawDataProviderTool::SCTRawDataProviderTool
(const std::string& type, const std::string& name, const IInterface* parent)
  :  AthAlgTool(type, name, parent),
     m_bsErrSvc{"SCT_ByteStreamErrorsSvc", name},
  m_robIdSet()
     {
       declareProperty("ErrorsSvc", m_bsErrSvc);
       declareProperty("xAODEventInfoKey", m_xevtInfoKey=std::string{"EventInfo"});
       declareProperty("EventInfoKey", m_evtInfoKey=std::string{"ByteStreamEventInfo"});
       declareInterface<ISCTRawDataProviderTool>(this);   
     }

/// -------------------------------------------------------
/// destructor 

SCTRawDataProviderTool::~SCTRawDataProviderTool()
{}
 
/// -------------------------------------------------------
/// initialize

StatusCode SCTRawDataProviderTool::initialize()
{
  StatusCode sc{AlgTool::initialize()};
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Failed to init baseclass");
    return StatusCode::FAILURE;
  }
   
  /** Get ByteStreamErrorsSvc  */
  if (m_bsErrSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve service " << m_bsErrSvc);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved service " << m_bsErrSvc);
  }

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority{100};
  if (sc.isSuccess()) {
    incsvc->addListener(this, "BeginEvent", priority);
  }

  ATH_CHECK(m_xevtInfoKey.initialize());
  ATH_CHECK(m_evtInfoKey.initialize());

  return StatusCode::SUCCESS;
}

/// -------------------------------------------------------
/// finalize

StatusCode SCTRawDataProviderTool::finalize()
{
  StatusCode sc{AlgTool::finalize()}; 
  return sc;
}

/// -------------------------------------------------------
/// convert method

void SCTRawDataProviderTool::handle(const Incident& inc) {
  if (inc.type() == "BeginEvent") {
    /** reset list of known robIds */
    m_robIdSet.clear(); 
  }  
}

StatusCode SCTRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecRobs,
                                           SCT_RDO_Container& rdoIdc,
                                           InDetBSErrContainer* errs)
{
  if(vecRobs.empty()) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("SCTRawDataProviderTool::convert()");
  static int DecodeErrCount{0};

  /**  are we working on a new event ? */
  std::vector<const ROBFragment*>::const_iterator rob_it{vecRobs.begin()};
  
  StatusCode sc{StatusCode::SUCCESS};

  /** loop over the ROB fragments */

  for(; rob_it!=vecRobs.end(); ++rob_it) {
    /** get the ID of this ROB/ROD */
    uint32_t robid{(*rob_it)->rod_source_id()};
    /** check if this ROBFragment was already decoded (EF case in ROIs) */
    if (!m_robIdSet.insert(robid).second) {
      ATH_MSG_DEBUG(" ROB Fragment with ID  "
                    << std::hex<<robid << std::dec
                    << " already decoded, skip"); 
    } else {
      sc = m_decoder->fillCollection(**rob_it, rdoIdc, errs);
      if (sc==StatusCode::FAILURE) {
        if (DecodeErrCount < 100) {
          ATH_MSG_ERROR("Problem with SCT ByteStream Decoding!");
          DecodeErrCount++;
        } else if (100 == DecodeErrCount) {
          ATH_MSG_ERROR("Too many Problem with SCT Decoding messages, turning message off.  ");
          DecodeErrCount++;
        }
      }
    }
  }

  if (sc==StatusCode::FAILURE) {
    ATH_MSG_ERROR("There was a problem with SCT ByteStream conversion");
    return sc;
  }
  long unsigned int nLVL1ID{m_bsErrSvc->getErrorSet(SCT_ByteStreamErrors::LVL1IDError)->size()};
  long unsigned int nROBFragment{m_bsErrSvc->getErrorSet(SCT_ByteStreamErrors::ROBFragmentError)->size()};

  if ((nLVL1ID > 500) or (nROBFragment > 1000)) {
    //// retrieve EventInfo.  
    /// First the xAOD one
    bool setOK_xAOD{false};
    SG::ReadHandle<xAOD::EventInfo> xevtInfo_const{m_xevtInfoKey};
    if (xevtInfo_const.isValid()) {
      xAOD::EventInfo* xevtInfo{const_cast<xAOD::EventInfo*>(&*xevtInfo_const)};
      setOK_xAOD = xevtInfo->setErrorState(xAOD::EventInfo::SCT, xAOD::EventInfo::Error);
    } 

    /// Second the old-style one
    bool setOK_old{false};
    SG::ReadHandle<EventInfo> evtInfo_const{m_evtInfoKey};
    if (evtInfo_const.isValid()) {
      EventInfo* evtInfo{const_cast<EventInfo*>(&*evtInfo_const)};
      setOK_old = evtInfo->setErrorState(EventInfo::SCT, EventInfo::Error);
    }

    if ((not setOK_xAOD) and (not setOK_old)) {
      ATH_MSG_ERROR("Failed to retrieve EventInfo containers or to set error states");
      return StatusCode::RECOVERABLE;
    }

    sc = StatusCode::SUCCESS;
  } /// 500 LVL1ID errors or 1000 ROBFragment errors

  return sc;
}
