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

#include "xAODTrigger/eFexEMRoIAuxContainer.h"
#include "xAODTrigger/eFexTauRoIAuxContainer.h"
#include "bytestreamDecoder/L1CaloRdoEfexTob.h"
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

    std::vector<SG::VarHandleKey*> writeKeys = {
            &m_eEMTOBWriteKey, &m_eTAUTOBWriteKey, &m_eEMxTOBWriteKey, &m_eTAUxTOBWriteKey };
    std::vector<SG::VarHandleKey*> readKeys = {
            &m_eEMTOBReadKey, &m_eTAUTOBReadKey, &m_eEMxTOBReadKey, &m_eTAUxTOBReadKey };
    std::vector<SG::VarHandleKey*> sliceKeys = {
            &m_eEMTOBSliceKey, &m_eTAUTOBSliceKey, &m_eEMxTOBSliceKey, &m_eTAUxTOBSliceKey };

    auto previousMode = ConversionMode::Undefined;
    for(size_t i=0;i<writeKeys.size();i++) {
        auto mode = getConversionMode(*readKeys.at(i),*writeKeys.at(i),msg());
        ATH_CHECK(mode!=ConversionMode::Undefined);
        if(i!=0) ATH_CHECK(mode==previousMode); // if this fails, user is trying to mix encoding and decoding
        previousMode = mode;
        ATH_CHECK(writeKeys.at(i)->initialize(mode==ConversionMode::Decoding));
        ATH_CHECK(readKeys.at(i)->initialize(mode==ConversionMode::Encoding));
        if(mode==ConversionMode::Decoding && m_multiSlice) {
            ATH_CHECK(sliceKeys.at(i)->assign(!writeKeys.at(i)->empty() ? writeKeys.at(i)->key()+"OutOfTime" : ""));
        }
        ATH_CHECK(sliceKeys.at(i)->initialize(!sliceKeys.at(i)->empty()));
    }

    return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode eFexByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf, const EventContext& ctx) const {
    

    // indices are source (tob or xtob) and bool for in/out of time
    std::map<std::pair<L1CaloRdoFexTob::TobSource,bool>,SG::WriteHandle<xAOD::eFexEMRoIContainer>> eContainers;
    std::map<std::pair<L1CaloRdoFexTob::TobSource,bool>,SG::WriteHandle<xAOD::eFexTauRoIContainer>> tContainers;

    ATH_CHECK( StatusCode(addContainer<xAOD::eFexEMRoIContainer,xAOD::eFexEMRoIAuxContainer>(eContainers,L1CaloRdoFexTob::TobSource::EfexTob,false,m_eEMTOBWriteKey,ctx)) );
    ATH_CHECK( StatusCode(addContainer<xAOD::eFexEMRoIContainer,xAOD::eFexEMRoIAuxContainer>(eContainers,L1CaloRdoFexTob::TobSource::EfexXtob,false,m_eEMxTOBWriteKey,ctx)) );
    ATH_CHECK( StatusCode(addContainer<xAOD::eFexEMRoIContainer,xAOD::eFexEMRoIAuxContainer>(eContainers,L1CaloRdoFexTob::TobSource::EfexTob,true,m_eEMTOBSliceKey,ctx)) );
    ATH_CHECK( StatusCode(addContainer<xAOD::eFexEMRoIContainer,xAOD::eFexEMRoIAuxContainer>(eContainers,L1CaloRdoFexTob::TobSource::EfexXtob,true,m_eEMxTOBSliceKey,ctx)) );

    ATH_CHECK( StatusCode(addContainer<xAOD::eFexTauRoIContainer,xAOD::eFexTauRoIAuxContainer>(tContainers,L1CaloRdoFexTob::TobSource::EfexTob,false,m_eTAUTOBWriteKey,ctx)) );
    ATH_CHECK( StatusCode(addContainer<xAOD::eFexTauRoIContainer,xAOD::eFexTauRoIAuxContainer>(tContainers,L1CaloRdoFexTob::TobSource::EfexXtob,false,m_eTAUxTOBWriteKey,ctx)) );
    ATH_CHECK( StatusCode(addContainer<xAOD::eFexTauRoIContainer,xAOD::eFexTauRoIAuxContainer>(tContainers,L1CaloRdoFexTob::TobSource::EfexTob,true,m_eTAUTOBSliceKey,ctx)) );
    ATH_CHECK( StatusCode(addContainer<xAOD::eFexTauRoIContainer,xAOD::eFexTauRoIAuxContainer>(tContainers,L1CaloRdoFexTob::TobSource::EfexXtob,true,m_eTAUxTOBSliceKey,ctx)) );

    std::list<L1CaloRdoRodInfo> rodInfos;
    std::list<L1CaloRdoEfexTob> efexTobs;
    L1CaloBsDecoderRun3 decoder;
    for (const ROBF* rob : vrobf) {
        // Iterate over ROD words and decode
        ATH_MSG_DEBUG("Decoding " << rob->rod_ndata() << " ROD words from ROB 0x" << std::hex << rob->rob_source_id() << std::dec);
        L1CaloBsDecoderUtil::decodeRodInfo( rob, rodInfos );

        if(rob->rod_ndata()==0) continue;

        CxxUtils::span data{rob->rod_data(), rob->rod_ndata()};
        auto lastRod = rodInfos.end(); lastRod--;
        decoder.decodeEfexTobs( data.begin(), data.end(), efexTobs, lastRod );

        for(const L1CaloRdoEfexTob& tob : efexTobs) {
            // loop over slices ... create separate tobs for each, where there's a word defined
            for(size_t slice=0;slice < tob.numSlices(); slice++) {
                if (!m_multiSlice && int(slice) != tob.getL1aPos()) continue; // ignore out-of-time slices if multiSlice option = false
                if (tob.getWord0(slice)==0) continue; // this tob isn't in this slice
                if(tob.getTobType() == L1CaloRdoFexTob::TobType::EM) {
                    auto cont = eContainers.find({tob.getTobSource(),int(slice)!=tob.getL1aPos()});
                    if(cont == eContainers.end()) continue; // not writing this tob collection
                    cont->second->push_back( std::make_unique<xAOD::eFexEMRoI>() );
                    if(tob.getTobSource() == L1CaloRdoFexTob::TobSource::EfexTob) {
                        cont->second->back()->initialize(tob.getModule(),tob.getCrate(),tob.getWord0(slice));
                    } else {
                        cont->second->back()->initialize(tob.getWord0(slice),tob.getWord1(slice));
                    }
                } else if (tob.getTobType() == L1CaloRdoFexTob::TobType::Tau) {
                    auto cont = tContainers.find({tob.getTobSource(),int(slice)!=tob.getL1aPos()});
                    if(cont == tContainers.end()) continue; // not writing this tob collection
                    cont->second->push_back( std::make_unique<xAOD::eFexTauRoI>() );
                    if(tob.getTobSource() == L1CaloRdoFexTob::TobSource::EfexTob) {
                        cont->second->back()->initialize(tob.getModule(),tob.getCrate(),tob.getWord0(slice));
                    } else {
                        cont->second->back()->initialize(tob.getWord0(slice),tob.getWord1(slice));
                    }
                }
            } // timeslice loop
        } // tob loop
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

