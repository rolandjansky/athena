/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMMDTRDODECODER_H
#define MUONBYTESTREAMMDTRDODECODER_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonDigitContainer/MdtDigit.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonMDT_CnvTools/IMDT_RDO_Decoder.h"
#include "MuonRDO/MdtAmtHit.h"
#include "StoreGate/ReadCondHandleKey.h"

namespace Muon {
    // Decoder class for conversion from MDT RDOs to MDT digits
    // Stefano Rosati
    // CERN Jan 2004

    class MdtRDO_Decoder : public AthAlgTool, virtual public IMDT_RDO_Decoder {
    public:
        MdtRDO_Decoder(const std::string& type, const std::string& name, const IInterface* parent);

        virtual StatusCode initialize();

        MdtDigit* getDigit(const MdtAmtHit* amtHit, uint16_t& subdetId, uint16_t& mrodId, uint16_t& csmId) const;

        Identifier getOfflineData(const MdtAmtHit* amtHit, uint16_t& subdetId, uint16_t& mrodId, uint16_t& csmId, int& tdc,
                                  int& width) const;

    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
        SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};
    };

}  // namespace Muon

inline MdtDigit* Muon::MdtRDO_Decoder::getDigit(const MdtAmtHit* amtHit, uint16_t& subdetId, uint16_t& mrodId, uint16_t& csmId) const {
    
    
    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
    const MuonMDT_CablingMap* readCdo{*readHandle};
    if (!readCdo) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return nullptr;
    }
    MuonMDT_CablingMap::CablingData cabling_data{};
    cabling_data.tdcId = amtHit->tdcId();
    cabling_data.channelId = amtHit->channelId();
    cabling_data.subdetectorId = subdetId;
    cabling_data.mrod = mrodId;
    cabling_data.csm = csmId;

    uint16_t coarse = amtHit->coarse();
    uint16_t fine = amtHit->fine();
    int width = (int)amtHit->width();

    MsgStream& msg(msgStream() );
    bool cab = readCdo->getOfflineId(cabling_data, msg);
    if (!cab) return nullptr;
    Identifier chanId;
    if (!readCdo->convert(cabling_data,chanId,false)) return nullptr;
    int tdcCounts = coarse * 32 + fine;
    MdtDigit* mdtDigit = new MdtDigit(chanId, tdcCounts, width, amtHit->isMasked());
    return mdtDigit;
}

inline Identifier Muon::MdtRDO_Decoder::getOfflineData(const MdtAmtHit* amtHit, uint16_t& subdetId, uint16_t& mrodId, uint16_t& csmId,
                                                       int& tdcCounts, int& width) const {
    
    
    uint16_t tdc = amtHit->tdcId();
    uint16_t chan = amtHit->channelId();
    uint16_t coarse = amtHit->coarse();
    uint16_t fine = amtHit->fine();
    width = amtHit->width();
    tdcCounts = coarse * 32 + fine;

    Identifier chanIdDefault;
    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
    const MuonMDT_CablingMap* readCdo{*readHandle};
    if (!readCdo) {
        ATH_MSG_ERROR("Null pointer to the read conditions object");
        return chanIdDefault;
    }
    MuonMDT_CablingMap::CablingData cabling_data{};
    cabling_data.subdetectorId = subdetId;
    cabling_data.csm = csmId;
    cabling_data.tdcId = tdc;
    cabling_data.channelId = chan;
    cabling_data.mrod = mrodId;

    bool cab = readCdo->getOfflineId(cabling_data, msgStream() );

    if (!cab) {
        return chanIdDefault;
    }

    Identifier chanId;
    readCdo->convert(cabling_data, chanId);    
   
    return chanId;
}

#endif
