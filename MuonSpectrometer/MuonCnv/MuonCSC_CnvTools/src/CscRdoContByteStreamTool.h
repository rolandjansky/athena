/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCSC_CNVTOOLS_CSCRDOCONTRAWEVENTTOOL_H
#define MUONCSC_CNVTOOLS_CSCRDOCONTRAWEVENTTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
#include "ByteStreamData/RawEvent.h"
#include "CSC_Hid2RESrcID.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCSC_CnvTools/ICSC_RDOtoByteStreamTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class CSCcablingSvc;

namespace Muon {
    /*
      An AlgTool to provide conversion between CSC RDO <---> ByteStream,
      and fill it in RawEvent.

      @author Ketevi A. Assamagan BNL December 27 2003
    */
    class CscRdoContByteStreamTool : public extends<AthAlgTool, ICSC_RDOtoByteStreamTool> {
    public:
        /** constructor
         */
        CscRdoContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);

        virtual ~CscRdoContByteStreamTool() = default;

        virtual StatusCode initialize() override;

        /** to read the cosmic data */
        virtual bool isCosmic() const override { return m_isCosmic; }

        /** for the old cosmic data before the ROB id=ROD id fix */
        virtual bool isOldCosmic() const override { return m_isOldCosmic; }

        virtual StatusCode convert(const CscRawDataContainer* cont, MsgStream& log) const override;

    private:
        CSC_Hid2RESrcID m_hid2re;
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc{this, "ByteStreamCnvSvc", "ByteStreamCnvSvc"};

        uint16_t m_rodVersion;

        CSCcablingSvc* m_cabling;

        bool m_isCosmic;
        bool m_isOldCosmic;
    };
}  // namespace Muon

#endif
