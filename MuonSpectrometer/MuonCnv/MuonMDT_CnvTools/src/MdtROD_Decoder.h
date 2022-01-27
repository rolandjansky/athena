/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTROD_DECODER_H
#define MUONBYTESTREAM_MDTROD_DECODER_H

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamData/RawEvent.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MDT_Hid2RESrcID.h"
#include "MdtAmtReadOut.h"
#include "MdtCsmReadOut.h"
#include "MdtHptdcReadOut.h"
#include "MdtRODReadOut.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRDO/MdtAmtHit.h"
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Version.h"

typedef std::map<uint16_t, MdtAmtHit*, std::less<uint16_t> > leading_amt_map;

class MdtROD_Decoder : public AthAlgTool {
public:
    /** constructor
     */
    MdtROD_Decoder(const std::string& type, const std::string& name, const IInterface* parent);

    /** destructor
     */
    virtual ~MdtROD_Decoder() = default;

    /** AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /** TODO Add documentation. What is a MDT_Hid2RESrcID?*/
    MDT_Hid2RESrcID* getHid2RE() const { return m_hid2re.get(); }

    StatusCode fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, MdtCsmContainer& rdoIDC) const;

    int specialROBNumber() const { return m_specialROBNumber; }

    std::pair<IdentifierHash, Identifier> getHash(Identifier ident) const;

private:
    std::unique_ptr<MDT_Hid2RESrcID> m_hid2re{};
    SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    /** TMP special ROB number for sector13 runs*/
    int m_specialROBNumber{-1};

    bool m_BMEpresent{false};
    bool m_BMGpresent{false};
    int m_BMEid{-1};
    int m_BMGid{-1};

    // variables to count how often the caching kicks in
    // Mutable as this is just to count calls of const function
    mutable std::atomic_uint m_nCache ATLAS_THREAD_SAFE = 0;
    mutable std::atomic_uint m_nNotCache ATLAS_THREAD_SAFE = 0;
};

#endif
