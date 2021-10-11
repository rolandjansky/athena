/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOLMT_H
#define MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOLMT_H

#include <string>

#include "CscRdoToCscPrepDataToolCore.h"
#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"

////////////////////////////////////////////////////////////////////////////////////////
/// Author: Ketevi A. Assamagan
/// BNL, April 03, 2005
///
/// algorithm to decode RDO into CscStripPrepData
/// get the RDO container from Storegate
/// loop over the RDO
/// Decode RDO into PrepRawData
/// loop over the PrepRawData and build the PrepRawData container
/// store the PrepRawData container in StoreGate
////////////////////////////////////////////////////////////////////////////////////////

namespace Muon {
    class CscRdoToCscPrepDataToolMT : public extends<CscRdoToCscPrepDataToolCore, IMuonRdoToPrepDataTool> {
    public:
        CscRdoToCscPrepDataToolMT(const std::string& type, const std::string& name, const IInterface* parent);

        /** destructor
         */
        virtual ~CscRdoToCscPrepDataToolMT();

        virtual StatusCode initialize() override;

        virtual StatusCode finalize() override;

        virtual StatusCode decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs) const override;
        virtual StatusCode decode(const std::vector<uint32_t>&) const override { return StatusCode::FAILURE; }

        virtual void printPrepData() const override;

    private:
        StatusCode decodeImpl(Muon::CscStripPrepDataContainer* outputCollection, const CscRawDataContainer* rdoContainer,
                              IdentifierHash givenHashId, std::vector<IdentifierHash>& decodedIdhs) const;
        StatusCode decodeImpl(Muon::CscStripPrepDataContainer* outputCollection, const CscRawDataContainer* rdoContainer,
                              std::vector<IdentifierHash>& decodedIdhs) const;

        /// This is the key for the cache for the CSC PRD containers, can be empty
        SG::UpdateHandleKey<CscStripPrepDataCollection_Cache> m_prdContainerCacheKey;
    };
}  // namespace Muon
#endif  /// MUONCSC_CNVTOOL_CSCRDOTOCSCPREPDATA_H
