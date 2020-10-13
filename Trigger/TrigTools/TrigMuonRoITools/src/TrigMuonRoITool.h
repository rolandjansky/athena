/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONROITOOLS_TRIGMUONROITOOL_H
#define TRIGMUONROITOOLS_TRIGMUONROITOOL_H

#include "TrigMuonToolInterfaces/ITrigMuonRoITool.h"
#include "TrigT1Result/MuCTPIRoI.h"

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

class TrigMuonRoITool: public extends<AthAlgTool, ITrigMuonRoITool>
{
    public:

    TrigMuonRoITool(const std::string& type, 
                    const std::string& name,
                    const IInterface*  parent);

    virtual StatusCode initialize() override;

    /// Decoding the muCTPi RoIB and DAQ ROB and return in and out of time RoIs
    virtual std::unique_ptr<TrigMuonRoITool::MuonRois> decodeMuCTPi(const EventContext& ctx) const;


    private:

    SG::ReadHandleKey<MuCTPI_RDO> m_muCTPIKey{this, "MUCTPILocation", "MUCTPI_RDO", "Location of MUCTPI RDO"};

    typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
    /// Reference to the ROBDataProviderSvc service
    IIROBDataProviderSvc_t           m_robDataProviderSvc;

    /// Source identifiers for ROB fragments
    IntegerProperty                  m_daqMuCTPiROBid ;

    /// vectors with muCTPi ROB Ids to retrieve
    std::vector<uint32_t>            m_muCTPiRobIds;     

    /// Number of pt Thresholds
    static const uint32_t NUMBER_OF_PT_THRESHOLDS = 6;   // Number of pT thresholds

    /// Helper for converting a mirod DAQ data word to a muCTPi RoIB data word
    uint32_t mirodToRoIBDataWord( uint32_t data_word ) const;

    /// Helper to print contents of a muCTPi RoIB data word
    void dumpRoIBDataWord(uint32_t data_word ) const;

    /// Configurable to decide how to decode muCTPi
    bool m_decodeMuCTPiFromROB;
};

#endif
