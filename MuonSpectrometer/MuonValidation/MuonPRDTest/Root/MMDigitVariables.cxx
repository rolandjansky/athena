/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDTest/MMDigitVariables.h"

#include "MuonReadoutGeometry/MMReadoutElement.h"

namespace MuonPRDTest {
    MMDigitVariables::MMDigitVariables(MuonTesterTree& tree, const std::string& container_name, MSG::Level msglvl) :
        PrdTesterModule(tree, "Digits_MM", true, msglvl), m_key{container_name} {}

    bool MMDigitVariables::declare_keys() { return declare_dependency(m_key); }
    bool MMDigitVariables::fill(const EventContext& ctx) {
        ATH_MSG_DEBUG("do fillMMDigitHitVariables()");
        const MuonGM::MuonDetectorManager* MuonDetMgr = getDetMgr(ctx);
        if (!MuonDetMgr) { return false; }
        SG::ReadHandle<MmDigitContainer> MMDigitContainer{m_key, ctx};
        if (!MMDigitContainer.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve digit container " << m_key.fullKey());
            return false;
        }

        ATH_MSG_DEBUG("retrieved MM Digit Container with size " << MMDigitContainer->digit_size());

        if (MMDigitContainer->size() == 0) ATH_MSG_DEBUG(" MM Digit Container empty ");
        unsigned int n_digits{0};
        for (const MmDigitCollection* coll : *MMDigitContainer) {
            ATH_MSG_DEBUG("processing collection with size " << coll->size());
            for (unsigned int digitNum = 0; digitNum < coll->size(); digitNum++) {
                const MmDigit* digit = coll->at(digitNum);
                Identifier Id = digit->identify();

                const MuonGM::MMReadoutElement* rdoEl = MuonDetMgr->getMMReadoutElement(Id);
                if (!rdoEl) {
                    ATH_MSG_ERROR("MMDigitVariables::fillVariables() - Failed to retrieve MMReadoutElement for "<<idHelperSvc()->mmIdHelper().print_to_string(Id).c_str());
                    return false;
                }

                m_NSWMM_dig_id.push_back(Id);
                m_NSWMM_dig_time.push_back(digit->chipResponseTime());
                m_NSWMM_dig_charge.push_back(digit->chipResponseCharge());
                m_NSWMM_dig_stripPosition.push_back(digit->chipResponsePosition());

                m_NSWMM_dig_sr_time.push_back(digit->stripResponseTime());
                m_NSWMM_dig_sr_charge.push_back(digit->stripResponseCharge());
                m_NSWMM_dig_sr_stripPosition.push_back(digit->stripResponsePosition());

                m_NSWMM_dig_time_trigger.push_back(digit->stripTimeForTrigger());
                m_NSWMM_dig_charge_trigger.push_back(digit->stripChargeForTrigger());
                m_NSWMM_dig_position_trigger.push_back(digit->stripPositionForTrigger());
                m_NSWMM_dig_MMFE_VMM_id_trigger.push_back(digit->MMFE_VMM_idForTrigger());
                m_NSWMM_dig_VMM_id_trigger.push_back(digit->VMM_idForTrigger());

                if ( digit->chipResponsePosition().size() != digit->stripResponsePosition().size() )
                    ATH_MSG_DEBUG("MicroMegas digitization: number of strip out from the strip response different from that out from the chip response");

                bool isValid;
                // Geant4 hit converted into digit might have fired more than one strip:
                // iterating on all strips associated to that digit (from chip response)
                for (unsigned int i=0;i<digit->chipResponsePosition().size();i++) {
                    // take strip index form chip information
                    int cr_strip = digit->chipResponsePosition().at(i);
                    //retrieve the detailed identifier for the strip form the chip response
                    Identifier cr_id = idHelperSvc()->mmIdHelper().channelID(idHelperSvc()->mmIdHelper().stationName(Id),idHelperSvc()->mmIdHelper().stationEta(Id),
                                       idHelperSvc()->mmIdHelper().stationPhi(Id),idHelperSvc()->mmIdHelper().multilayer(Id),idHelperSvc()->mmIdHelper().gasGap(Id),cr_strip,isValid);
                    if (!isValid) {
                        ATH_MSG_WARNING("MMDigitVariables: failed to create a valid ID for (chip response) strip n. " << cr_strip
                                            << "; associated positions will be set to 0.0.");
                        continue;
                    }

                    // asking the detector element to get local position of strip
                    Amg::Vector2D cr_strip_pos(0., 0.);
                    if ( !rdoEl->stripPosition(cr_id,cr_strip_pos) ) {
                        ATH_MSG_WARNING("MMDigitVariables: failed to associate a valid local position for (chip response) strip n. " << cr_strip
                                        << "; associated positions will be set to 0.0.");
                        continue;
                    }
                    m_NSWMM_dig_stripLposX.push_back(cr_strip_pos.x());
                    m_NSWMM_dig_stripLposY.push_back(cr_strip_pos.y());

                    // asking the detector element to transform this local to the global position
                    Amg::Vector3D cr_strip_gpos(0., 0., 0.);
                    rdoEl->surface(cr_id).localToGlobal(cr_strip_pos, Amg::Vector3D(0., 0., 0.), cr_strip_gpos);
                    m_NSWMM_dig_stripGpos.push_back(cr_strip_gpos);

                    // check if local and global position are congruent with the transform
                    Amg::Vector3D lpos = rdoEl->transform(cr_id).inverse() * cr_strip_gpos;
                    double dx = cr_strip_pos.x() - lpos.x();
                    double dy = cr_strip_pos.y() - lpos.y();

                    if ( std::abs(dx)>0.1 || std::abs(dy)>0.1 ) {
                        ATH_MSG_WARNING("MicroMegas digitization: inconsistency between local/global position and transform:");
                        ATH_MSG_WARNING("                         X from stripPosition: " << std::setw(10) << std::setprecision(3) << cr_strip_pos.x()
                                        << ", X from global*transform(id).inverse(): " << lpos.x() );
                        ATH_MSG_WARNING("                         Y from stripPosition: " << std::setw(10) << std::setprecision(3) << cr_strip_pos.y()
                                        << ", Y from global*transform(Id).inverse(): " << lpos.y() );
                    }
                }

                // Geant4 hit converted into digit might have fired more than one strip:
                // iterating on all strips associated to that digit (from strip response)
                for (unsigned int i=0;i<digit->stripResponsePosition().size();i++) {
                    int sr_strip = digit->stripResponsePosition().at(i);

                    //retrieve the detailed identifier for the strip form the strip response
                    Identifier sr_id = idHelperSvc()->mmIdHelper().channelID(idHelperSvc()->mmIdHelper().stationName(Id),idHelperSvc()->mmIdHelper().stationEta(Id),
                                       idHelperSvc()->mmIdHelper().stationPhi(Id),idHelperSvc()->mmIdHelper().multilayer(Id),idHelperSvc()->mmIdHelper().gasGap(Id),sr_strip,isValid);
                    if (!isValid) {
                        ATH_MSG_WARNING("MMDigitVariables: failed to create a valid ID for (chip response) strip n. " << sr_strip
                                            << "; associated positions will be set to 0.0.");
                        continue;
                    }

                    // asking the detector element to transform this local to the global position
                    Amg::Vector2D sr_strip_pos(0., 0.);
                    if ( !rdoEl->stripPosition(sr_id,sr_strip_pos) ) {
                        ATH_MSG_WARNING("MMDigitVariables: failed to associate a valid local position for (chip response) strip n. " << sr_strip
                                        << "; associated positions will be set to 0.0.");
                        continue;
                    }
                    m_NSWMM_dig_sr_stripLposX.push_back(sr_strip_pos.x());
                    m_NSWMM_dig_sr_stripLposY.push_back(sr_strip_pos.y());

                    // asking the detector element to transform this local to the global position
                    Amg::Vector3D sr_strip_gpos(0., 0., 0.);
                    rdoEl->surface(sr_id).localToGlobal(sr_strip_pos, Amg::Vector3D(0., 0., 0.), sr_strip_gpos);
                    m_NSWMM_dig_stripGpos.push_back(sr_strip_gpos);

                    // check if local and global position are congruent with the transform
                    Amg::Vector3D sr_lpos = rdoEl->transform(sr_id).inverse() * sr_strip_gpos;
                    double dx = sr_strip_pos.x() - sr_lpos.x();
                    double dy = sr_strip_pos.y() - sr_lpos.y();

                    if ( std::abs(dx)>0.1 || std::abs(dy)>0.1 ) {
                        ATH_MSG_WARNING("MicroMegas digitization: inconsistency between local/global position and transform:");
                        ATH_MSG_WARNING("                         X from Center(): " << std::setw(10) << std::setprecision(3) << sr_strip_pos.x()
                                        << ", X from local*transform: " << sr_lpos.x() );
                        ATH_MSG_WARNING("                         Y from Center(): " << std::setw(10) << std::setprecision(3) << sr_strip_pos.y()
                                        << ", Y from local*transform: " << sr_lpos.y() );
                    }

                    unsigned int cl = (digit->chipResponseTime().size()<=digit->stripResponseTime().size()) ? digit->chipResponseTime().size() : digit->stripResponseTime().size();

                    for (unsigned int i=0;i<cl;i++) {
                        ATH_MSG_DEBUG("MicroMegas Digit, chip response time    :" << std::setw(6) << std::setprecision(2) << digit->chipResponseTime().at(i)         
                                                 << ",  strip response time    :" << std::setw(6) << std::setprecision(2) << digit->stripResponseTime().at(i));
                        ATH_MSG_DEBUG("MicroMegas Digit, chip response charge  :" << std::setw(6) << std::setprecision(2) << digit->chipResponseCharge().at(i)   
                                                 << ",  strip response charge  :" << std::setw(6) << std::setprecision(2) << digit->stripResponseCharge().at(i));
                        ATH_MSG_DEBUG("MicroMegas Digit, chip response position:" << std::setw(6) << std::setprecision(2) << digit->chipResponsePosition().at(i)
                                                 << ",  strip response position:" << std::setw(6) << std::setprecision(2) << digit->stripResponsePosition().at(i));
                  }

                }
                ++n_digits;
            }
        }
        m_NSWMM_nDigits = n_digits;
        ATH_MSG_DEBUG(" finished fillMMDigitVariables()");
        return true;
    }
}