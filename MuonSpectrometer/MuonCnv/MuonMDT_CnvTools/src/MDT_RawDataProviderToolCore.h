/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOLCORE_H
#define MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOLCORE_H

#include <set>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MdtROD_Decoder.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRDO/MdtCsm_Cache.h"
#include "StoreGate/ReadCondHandleKey.h"

class MdtCsmContainer;

namespace Muon {

    /** @class MDT_RawDataProviderToolCore
        @author  Edward Moyse <Edward.Moyse@cern.ch>
        @author  Mark Owen <markowen@cern.ch>
    */

    class MDT_RawDataProviderToolCore : public AthAlgTool {
    public:
        MDT_RawDataProviderToolCore(const std::string&, const std::string&, const IInterface*);

        /** default destructor */
        virtual ~MDT_RawDataProviderToolCore() = default;

        /** standard Athena-Algorithm method */
        virtual StatusCode initialize();

        /** Convert method */
        virtual StatusCode convertIntoContainer(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
                                                MdtCsmContainer& mdtContainer) const;

    protected:
        ToolHandle<MdtROD_Decoder> m_decoder{this, "Decoder", "MdtROD_Decoder/MdtROD_Decoder"};
        SG::WriteHandleKey<MdtCsmContainer> m_rdoContainerKey{this, "RdoLocation", "MDTCSM",
                                                              "Name of the MDTCSM produced by RawDataProvider"};

        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        unsigned int m_maxhashtoUse = 0U;

        // Rob Data Provider handle
        ServiceHandle<IROBDataProviderSvc> m_robDataProvider{this, "ROBDataProviderSvc", "ROBDataProviderSvc"};

        SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};
    };
}  // namespace Muon

#endif
