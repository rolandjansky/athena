/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonPRDTest/MDTDigitVariables.h"

#include "MuonDigitContainer/MdtDigit.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
MdtDigitVariables::MdtDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
    PrdTesterModule(tree, "Digits_MDT", true, msglvl), m_digitKey{container_name} {}

bool MdtDigitVariables::fill(const EventContext& ctx) {
    ATH_MSG_DEBUG("do fillMDTSimHitVariables()");
    const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
    if (!MuonDetMgr) { return false; }
    SG::ReadHandle<MdtDigitContainer> MdtDigitContainer{m_digitKey, ctx};
    if (!MdtDigitContainer.isValid()) {
        ATH_MSG_FATAL("Failed to retrive digit container " << m_digitKey.fullKey());
        return false;
    }
    ATH_MSG_DEBUG("retrieved MDT Digit Container with size " << MdtDigitContainer->digit_size());

    if (MdtDigitContainer->size() == 0) ATH_MSG_DEBUG(" MDT Digit Continer empty ");
    unsigned int n_digits{0};

    for (const MdtDigitCollection* coll : *MdtDigitContainer) {
        ATH_MSG_DEBUG("processing collection with size " << coll->size());
        for (unsigned int digitNum = 0; digitNum < coll->size(); ++digitNum) {
            const MdtDigit* digit = coll->at(digitNum);

            Identifier Id = digit->identify();
            const MuonGM::MdtReadoutElement* rdoEl = MuonDetMgr->getMdtReadoutElement(Id);
            if (!rdoEl) return false;

            const Identifier first_ml = idHelperSvc()->mdtIdHelper().channelID(
                idHelperSvc()->stationName(Id), idHelperSvc()->stationEta(Id), idHelperSvc()->stationPhi(Id), 1, 1, 1);
            const MuonGM::MdtReadoutElement* firstEl = MuonDetMgr->getMdtReadoutElement(first_ml);

            int NofMultilayers = idHelperSvc()->mdtIdHelper().numberOfMultilayers(Id);

            ATH_MSG_DEBUG("MDT Digit Offline id: " << idHelperSvc()->toString(Id));

            m_MDT_digit_id.push_back(Id);
            m_MDT_dig_time.push_back(digit->tdc());
            m_MDT_dig_charge.push_back(digit->adc());

            /// Retrieve the tube position in the global coordinate frame
            const Amg::Vector3D globalpos = rdoEl->tubePos(Id);
            /// Retrieve the tube position in the local multi layer frame
            const Amg::Vector3D localTubePos = firstEl->transform().inverse() * globalpos;

            m_MDT_dig_globalPos.push_back(globalpos);
            m_MDT_dig_localTubePos.push_back(localTubePos);
            m_MDT_dig_numberOfMultilayers.push_back(NofMultilayers);
            ++n_digits;
        }
    }
    m_MDT_nDigits = n_digits;
    ATH_MSG_DEBUG(" finished fillMdtDigitVariables()");
    return true;
}
bool MdtDigitVariables::declare_keys() { return m_digitKey.initialize().isSuccess(); }