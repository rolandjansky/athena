/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFexByteStreamTool  -  description
//                              -------------------
//     begin                : 11 05 2022
//     email                : will@cern.ch
//  ***************************************************************************/

#include "eFexByteStreamTool.h"
#include "CxxUtils/span.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Status.h"

#include "xAODTrigL1Calo/eFexTowerAuxContainer.h"
#include "xAODTrigger/eFexEMRoIAuxContainer.h"
#include "xAODTrigger/eFexTauRoIAuxContainer.h"
#include "bytestreamDecoder/L1CaloRdoEfexTob.h"
#include "bytestreamDecoder/L1CaloRdoEfexTower.h"
#include "bytestreamDecoder/L1CaloBsDecoderUtil.h"
#include "bytestreamDecoder/L1CaloRdoRodInfo.h"
#include "bytestreamDecoder/L1CaloBsDecoderRun3.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;

namespace {
    // Helper method for creating the containers
    template<typename T, typename W>
    StatusCode addContainer(
            std::map <std::pair<L1CaloRdoFexTob::TobSource, bool>, SG::WriteHandle<T>> &map,
            L1CaloRdoFexTob::TobSource source, bool inTime, const SG::WriteHandleKey <T> &handle,
            const EventContext &ctx) {
        if (!handle.empty()) {
            return map.emplace(std::make_pair(source, inTime),
                               SG::WriteHandle<T>(handle, ctx)).first->second.record(
                    std::make_unique<T>(), std::make_unique<W>());
        }
        return StatusCode::SUCCESS;
    }
}

eFexByteStreamTool::eFexByteStreamTool(const std::string& type,
        const std::string& name,
        const IInterface* parent)
    : base_class(type, name, parent) {}

StatusCode eFexByteStreamTool::initialize() {

    // Initialise eEM data handle keys
    ATH_CHECK(m_eEMReadKey.initialize(!m_eEMReadKey.empty()));
    ATH_CHECK(m_eEMWriteKey.initialize(!m_eEMWriteKey.empty()));

    // Initialise eTAU data handle keys
    ATH_CHECK(m_eTAUReadKey.initialize(!m_eTAUReadKey.empty()));
    ATH_CHECK(m_eTAUWriteKey.initialize(!m_eTAUWriteKey.empty()));

    // write keys for xTOBs
    ATH_CHECK(m_eEMxWriteKey.initialize(!m_eEMxWriteKey.empty()));
    ATH_CHECK(m_eTAUxWriteKey.initialize(!m_eTAUxWriteKey.empty()));

    // multislice TOBs
    ATH_CHECK(m_eEMSliceWriteKey.initialize(!m_eEMSliceWriteKey.empty()));
    ATH_CHECK(m_eTAUSliceWriteKey.initialize(!m_eTAUSliceWriteKey.empty()));

    // Initialize eTower handle key
    ATH_CHECK(m_eTowerWriteKey.initialize(!m_eTowerWriteKey.empty()));

    m_decoder = std::make_unique<L1CaloBsDecoderRun3>();

    return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode eFexByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf, const EventContext& ctx) const {

    // indices are source (tob or xtob) and bool for in/out of time
    std::map<std::pair<L1CaloRdoFexTob::TobSource,bool>,SG::WriteHandle<xAOD::eFexEMRoIContainer>> eContainers;
    std::map<std::pair<L1CaloRdoFexTob::TobSource,bool>,SG::WriteHandle<xAOD::eFexTauRoIContainer>> tContainers;

    bool multislice=false;

    if (!m_eEMWriteKey.empty()) {
        ATH_CHECK(StatusCode(addContainer<xAOD::eFexEMRoIContainer, xAOD::eFexEMRoIAuxContainer>(
                eContainers, L1CaloRdoFexTob::TobSource::EfexTob, false, m_eEMWriteKey, ctx)));
    }
    if (!m_eTAUWriteKey.empty()) {
        ATH_CHECK(StatusCode(addContainer<xAOD::eFexTauRoIContainer, xAOD::eFexTauRoIAuxContainer>(
                tContainers, L1CaloRdoFexTob::TobSource::EfexTob, false, m_eTAUWriteKey, ctx)));
    }
    if (!m_eEMxWriteKey.empty()) {
        ATH_CHECK( StatusCode(addContainer<xAOD::eFexEMRoIContainer,xAOD::eFexEMRoIAuxContainer>(
                eContainers,L1CaloRdoFexTob::TobSource::EfexXtob,false,m_eEMxWriteKey,ctx)) );
    }
    if (!m_eTAUxWriteKey.empty()) {
        ATH_CHECK( StatusCode(addContainer<xAOD::eFexTauRoIContainer,xAOD::eFexTauRoIAuxContainer>(
                tContainers,L1CaloRdoFexTob::TobSource::EfexXtob,false,m_eTAUxWriteKey,ctx)) );
    }
    if (!m_eEMSliceWriteKey.empty()) {
        ATH_CHECK( StatusCode(addContainer<xAOD::eFexEMRoIContainer,xAOD::eFexEMRoIAuxContainer>(
                eContainers,L1CaloRdoFexTob::TobSource::EfexTob,true,m_eEMSliceWriteKey,ctx)) );
        multislice=true;
    }
    if (!m_eTAUSliceWriteKey.empty()) {
        ATH_CHECK( StatusCode(addContainer<xAOD::eFexTauRoIContainer,xAOD::eFexTauRoIAuxContainer>(
                tContainers,L1CaloRdoFexTob::TobSource::EfexTob,true,m_eTAUSliceWriteKey,ctx)) );
        multislice=true;
    }


    SG::WriteHandle<xAOD::eFexTowerContainer> eTowers;
    if(!m_eTowerWriteKey.empty()) {
        eTowers = SG::WriteHandle<xAOD::eFexTowerContainer>(m_eTowerWriteKey,ctx);
        ATH_CHECK( eTowers.record(std::make_unique<xAOD::eFexTowerContainer>(),std::make_unique<xAOD::eFexTowerAuxContainer>()) );
    }

    std::list<L1CaloRdoRodInfo> rodInfos;
    std::list<L1CaloRdoEfexTob> efexTobs;
    std::list<L1CaloRdoEfexTower> efexTowers;
    L1CaloBsDecoderRun3& decoder = *m_decoder;
    std::map<std::tuple<int,int,int,int,int>,size_t> towerMap; // maps {crate,module,fpga,eta,phi} onto index in vector
    for (const ROBF* rob : vrobf) {
        // Iterate over ROD words and decode
        ATH_MSG_DEBUG("Decoding " << rob->rod_ndata() << " ROD words from ROB 0x" << std::hex << rob->rob_source_id() << std::dec);
        L1CaloBsDecoderUtil::decodeRodInfo( rob, rodInfos );

        if(rob->rod_ndata()==0) continue;

        CxxUtils::span data{rob->rod_data(), rob->rod_ndata()};
        auto lastRod = rodInfos.end(); lastRod--;

        if ( (rob->rob_source_id() >> 16) == eformat::TDAQ_CALO_FEAT_EXTRACT_DAQ && !m_eTowerWriteKey.empty() ) {
            // decoding raw input data
            // tower et counts should be in order: PS, L1, L2, L3, Had
            // towers with all et counts 0 will still be zero-suppressed
            efexTowers.clear();
            decoder.decodeEfexData(data.begin(), data.end(), efexTowers, lastRod );
            for(auto& t : efexTowers) {
                if (t.getLayer() != 0) continue; // do hadronic in next loop
                towerMap[std::make_tuple(t.getCrate(),t.getModule(),t.getFpgaNumber(),t.getRegion().getEtaIndex(),t.getRegion().getPhiIndex())] = eTowers->size();
                eTowers->push_back( std::make_unique<xAOD::eFexTower>() );
                // in bytestream cell orders are L2,PS,L1,L3 so reorder to usual PS,L1,L2,L3 order
                std::vector<uint16_t> counts(11,0); // defaults hadronic to 0
                counts[0] = t.getSupercells().at(4);
                for(size_t idx = 0;idx<4;idx++) {
                    counts[idx+1] = t.getSupercells().at(idx+5); // L1
                    counts[idx+5] = t.getSupercells().at(idx); // L2
                }
                counts[9] = t.getSupercells().at(9);

                eTowers->back()->initialize(t.getRegion().getEtaIndex()*0.1,2.*ROOT::Math::Pi()*(t.getRegion().getPhiIndex() - 64*(t.getRegion().getPhiIndex()>=32))/64,
                                        counts,
                                        t.getModule() + t.getCrate()*12,
                                        t.getFpgaNumber(),
                                        t.getFlag(),0 /* hadronic status flag */);
            }

            for(auto& t : efexTowers) {
                if (t.getLayer() == 0) continue; // do hadronic towers now ... add to existing towers
                size_t idx = eTowers->size();
                if(auto itr = towerMap.find({t.getCrate(), t.getModule(), t.getFpgaNumber(),t.getRegion().getEtaIndex(),t.getRegion().getPhiIndex()}); itr != towerMap.end()) {
                    idx = itr->second;
                } else {
                    // possible that ecal tower was zero-suppressed but hcal has energy, so create such a tower
                    towerMap[std::make_tuple(t.getCrate(),t.getModule(),t.getFpgaNumber(),t.getRegion().getEtaIndex(),t.getRegion().getPhiIndex())] = eTowers->size();
                    eTowers->push_back( std::make_unique<xAOD::eFexTower>() );
                    eTowers->back()->initialize(t.getRegion().getEtaIndex()*0.1,2.*ROOT::Math::Pi()*(t.getRegion().getPhiIndex() - 64*(t.getRegion().getPhiIndex()>=32))/64,
                                                std::vector<uint16_t>(11,0),
                                                t.getModule() + t.getCrate()*12,t.getFpgaNumber(),0,t.getFlag());
                }
                auto tower = eTowers->at(idx);
                tower->setHad_status(t.getFlag());
                auto et_count = tower->et_count();
                et_count.at(10) = t.getValue();
                tower->setEt_count(et_count);
            }



        } else if ( (rob->rob_source_id() >> 16) == eformat::TDAQ_CALO_FEAT_EXTRACT_ROI ) {
            // decoding tobs
            efexTobs.clear();
            decoder.decodeEfexTobs(data.begin(), data.end(), efexTobs, lastRod);

            for (const L1CaloRdoEfexTob &tob: efexTobs) {
                // loop over slices ... create separate tobs for each, where there's a word defined
                for (size_t slice = 0; slice < tob.numSlices(); slice++) {
                    if (!multislice && int(slice) != tob.getL1aPos())
                        continue; // ignore out-of-time slices if multiSlice option = false
                    if (tob.getWord0(slice) == 0) continue; // this tob isn't in this slice
                    if (tob.getTobType() == L1CaloRdoFexTob::TobType::EM) {
                        auto cont = eContainers.find({tob.getTobSource(), int(slice) != tob.getL1aPos()});
                        if (cont == eContainers.end()) continue; // not writing this tob collection
                        cont->second->push_back(std::make_unique<xAOD::eFexEMRoI>());
                        if (tob.getTobSource() == L1CaloRdoFexTob::TobSource::EfexTob) {
                            cont->second->back()->initialize(tob.getModule(), tob.getCrate(), tob.getWord0(slice));
                        } else {
                            cont->second->back()->initialize(tob.getWord0(slice), tob.getWord1(slice));
                        }
                    } else if (tob.getTobType() == L1CaloRdoFexTob::TobType::Tau) {
                        auto cont = tContainers.find({tob.getTobSource(), int(slice) != tob.getL1aPos()});
                        if (cont == tContainers.end()) continue; // not writing this tob collection
                        cont->second->push_back(std::make_unique<xAOD::eFexTauRoI>());
                        if (tob.getTobSource() == L1CaloRdoFexTob::TobSource::EfexTob) {
                            cont->second->back()->initialize(tob.getModule(), tob.getCrate(), tob.getWord0(slice));
                        } else {
                            cont->second->back()->initialize(tob.getWord0(slice), tob.getWord1(slice));
                        }
                    }
                } // timeslice loop
            } // tob loop
        } // tob data if block
    } // fragment loop

    if(msgLevel(MSG::DEBUG)) {
        std::stringstream msg;
        for (auto&[k, v]: eContainers) {
            msg << v->size() << " " << (k.second ? "out-of-time " : "in-time ") << "eg" <<
                (k.first == L1CaloRdoFexTob::TobSource::EfexTob ? "" : "x") << "TOB, ";
        }
        for (auto&[k, v]: tContainers) {
            msg << v->size() << " " << (k.second ? "out-of-time " : "in-time ") << "tau" <<
                (k.first == L1CaloRdoFexTob::TobSource::EfexTob ? "" : "x") << "TOB, ";
        }
        ATH_MSG_DEBUG("Decoded: " << msg.str());
        if (!m_eTowerWriteKey.empty()) {
            ATH_MSG_DEBUG("Decoded: " << eTowers->size() << " eTowers");
        }
    }
    // this is how to print the in-time em xTOBs:
//    for(const auto& tob : *(eContainers[{L1CaloRdoFexTob::TobSource::EfexXtob,false}])) {
//        std::cout << tob->eFexNumber() << "." << tob->fpga() << ": " << tob->fpgaEta() << " " << tob->fpgaPhi() << " " << tob->et() << " " << tob->etXTOB() << " " << tob->etTOB() << " " << std::endl;
//    }

    return StatusCode::SUCCESS;
}

StatusCode eFexByteStreamTool::convertToBS(std::vector<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment*>& , const EventContext& ) {
    // not supported yet
    return StatusCode::FAILURE;
}

