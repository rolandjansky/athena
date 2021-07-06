/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOL_H
#define MUONCSC_CNVTOOLS_CSCRDOTOCSCPREPDATATOOL_H

#include <string>

#include "CscRdoToCscPrepDataToolCore.h"
#include "CxxUtils/checker_macros.h"

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
    class ATLAS_NOT_THREAD_SAFE CscRdoToCscPrepDataTool : public extends<CscRdoToCscPrepDataToolCore, IMuonRdoToPrepDataTool> {
    public:
        CscRdoToCscPrepDataTool(const std::string& type, const std::string& name, const IInterface* parent);

        /** destructor
         */
        virtual ~CscRdoToCscPrepDataTool() = default;

        virtual StatusCode initialize() override;

        virtual StatusCode decode(std::vector<IdentifierHash>& givenIdhs, std::vector<IdentifierHash>& decodedIdhs) const override;
        virtual StatusCode decode(const std::vector<uint32_t>&) const override { return StatusCode::FAILURE; }

        virtual void printPrepData() const override;

    private:
        StatusCode decodeImpl(Muon::CscStripPrepDataContainer* outputCollection, const CscRawDataContainer* rdo, IdentifierHash givenIdh,
                              std::vector<IdentifierHash>& decodedIdhs) const;
        StatusCode decodeImpl(Muon::CscStripPrepDataContainer* outputCollection, const CscRawDataContainer* rdo,
                              std::vector<IdentifierHash>& decodedIdhs) const;

        mutable bool m_fullEventDone = false;
        mutable Muon::CscStripPrepDataContainer* m_outputCollection = nullptr;
    };
}  // namespace Muon
#endif  /// MUONCSC_CNVTOOL_CSCRDOTOCSCPREPDATA_H
