/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderToolCore.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOLCORE_H
#define MUONCSC_CNVTOOLS_CSC_RAWDATAPROVIDERTOOLCORE_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCSC_CnvTools/ICSC_ROD_Decoder.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "CSCcabling/CSCcablingSvc.h"
#include "CSC_Hid2RESrcID.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class IROBDataProviderSvc;

namespace MuonGM
{
    class MuonDetectorManager;
}

namespace Muon
{

class CSC_RawDataProviderToolCore : public AthAlgTool
{
public:
    CSC_RawDataProviderToolCore(const std::string& t, const std::string& n, const IInterface* p);

    /** default destructor */
    virtual ~CSC_RawDataProviderToolCore();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;

protected:
    
    /** function to decode the passed ROB fragments into the passed container */
    StatusCode convertIntoContainer(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                    const EventContext& ctx,
                                    CscRawDataContainer& container) const;


  /** member variables for algorithm properties: */
  ToolHandle<ICSC_ROD_Decoder>        m_decoder;

  const MuonGM::MuonDetectorManager*  m_muonMgr;
  
  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};

  SG::WriteHandleKey<CscRawDataContainer> m_containerKey{
     this, "RdoLocation", "CSCRDO", "Name of the CSCRDO produced by RawDataProvider"};
  SG::ReadHandleKey<xAOD::EventInfo>  m_eventInfoKey
  { this, "EventInfoKey", "EventInfo", "" };
  CSC_Hid2RESrcID                     m_hid2re;

  ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
  ServiceHandle<CSCcablingSvc>        m_cabling;


};
} // end of namespace

#endif
