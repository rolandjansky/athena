/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCsmContByteStreamTool.h"

#include "ByteStreamData/RawEvent.h"
#include "MdtROD_Encoder.h"
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

// default constructor
Muon::MdtCsmContByteStreamTool::MdtCsmContByteStreamTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent) {}

StatusCode Muon::MdtCsmContByteStreamTool::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    m_hid2re = std::make_unique<MDT_Hid2RESrcID>();
    ATH_CHECK(m_hid2re->set(&m_idHelperSvc->mdtIdHelper()));

    ATH_CHECK(m_byteStreamCnvSvc.retrieve());

    return StatusCode::SUCCESS;
}

StatusCode Muon::MdtCsmContByteStreamTool::finalize() { return StatusCode::SUCCESS; }

StatusCode Muon::MdtCsmContByteStreamTool::convert(const MdtCsmContainer* cont, MsgStream& /*log*/) const {
    // Get the event assembler
    FullEventAssembler<MDT_Hid2RESrcID>* fea = nullptr;
    ATH_CHECK(m_byteStreamCnvSvc->getFullEventAssembler(fea, "MdtCsmContByteStream"));

    StatusCode status = fea->idMap().set(&m_idHelperSvc->mdtIdHelper());
    if (status.isFailure()) {
        ATH_MSG_FATAL("Could not initialize MDT mapping !");
        return StatusCode::FAILURE;
    }

    FullEventAssembler<MDT_Hid2RESrcID>::RODDATA* theROD;

    std::map<uint32_t, MdtROD_Encoder> mapEncoder;

    const MuonGM::MuonDetectorManager* mdm = nullptr;
    ATH_CHECK(detStore()->retrieve(mdm, "Muon"));
    const MdtIdHelper& mdtIdHelper = *mdm->mdtIdHelper();

    ATH_MSG_DEBUG(" number of collections " << cont->size());
    for (const MdtCsm* csm : *cont) {
        Identifier coll_id = csm->identify();
        uint32_t rodId = m_hid2re->getRodID(coll_id);
        mapEncoder.try_emplace(rodId, mdtIdHelper).first->second.add(csm);
    }

    // MdtCsm_Encoder has collected all the csm, now can fill the
    // ROD block data.

    ATH_MSG_DEBUG(" start to fill Rod ");

    for (auto& p : mapEncoder) {
        theROD = fea->getRodData(p.first);
        p.second.fillROD(*theROD);
    }

    return StatusCode::SUCCESS;
}
