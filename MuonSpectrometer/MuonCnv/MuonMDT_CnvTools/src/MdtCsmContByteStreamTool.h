/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CNVTOOLS_MDTCSMCONTRAWEVENTTOOL_H
#define MUONMDT_CNVTOOLS_MDTCSMCONTRAWEVENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MDT_Hid2RESrcID.h"
#include "MuonCnvToolInterfaces/IMDT_RDOtoByteStreamTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class MdtCsmContainer;

#include <map>
#include <string>

namespace Muon {
    /** An AlgTool class to provide conversion from LArRawChannelContainer
     *  to ByteStream, and fill it in RawEvent
     *  created:  Sept 25, 2002, by Hong Ma
     *
     * Adapted for Muons by Ketevi A. Assamagan
     * Jan-14-2003, BNL
     * Conversion of Rpc Pad Container to byte stream
     *
     * Adapted for MDTs ny Michela Biglietti
     * March 2003, Naples
     * Conversion of a Mdt Csm to ByteStream
     */

    class MdtCsmContByteStreamTool : public extends<AthAlgTool, IMDT_RDOtoByteStreamTool> {
    public:
        /** constructor
         */
        MdtCsmContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent);

        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

        virtual StatusCode convert(const MdtCsmContainer* cont, MsgStream& log) const override;

    private:
        std::unique_ptr<MDT_Hid2RESrcID> m_hid2re;

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        ServiceHandle<ByteStreamCnvSvc> m_byteStreamCnvSvc{this, "ByteStreamCnvSvc", "ByteStreamCnvSvc"};
    };

}  // namespace Muon

#endif
