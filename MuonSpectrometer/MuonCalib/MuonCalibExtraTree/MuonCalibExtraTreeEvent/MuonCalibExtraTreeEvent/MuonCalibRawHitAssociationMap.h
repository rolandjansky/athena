/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBRAWHITASSICATIONMAP_H
#define MUONCALIBRAWHITASSICATIONMAP_H

#include <map>

#include "CxxUtils/checker_macros.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"

namespace MuonCalib {

    /**
       @class MuonCalibRawHitAssociationMap
       Association map between MuonFixedId and raw hits
    */
    class MuonCalibRawHitAssociationMap {
    public:
        using MdtCalibRawHitPtr = MuonCalibRawHitCollection::MdtCalibRawHitPtr;
        using CscCalibRawHitPtr = MuonCalibRawHitCollection::CscCalibRawHitPtr;

        using RpcCalibRawHitPtr = MuonCalibRawHitCollection::RpcCalibRawHitPtr;
        using TgcCalibRawHitPtr = MuonCalibRawHitCollection::TgcCalibRawHitPtr;

        /** get raw csc hit for given Identifier, returns 0 if not on track */
        CscCalibRawHitPtr getRawCscHit(const MuonFixedId& id) const;

        /** get raw mdt hit for given Identifier, returns 0 if not on track */
        MdtCalibRawHitPtr getRawMdtHit(const MuonFixedId& id) const;

        /** get raw tgc hit for given Identifier, returns 0 if not on track */
        TgcCalibRawHitPtr getRawTgcHit(const MuonFixedId& id) const;

        /** get raw rpc hit for given Identifier, returns 0 if not on track */
        RpcCalibRawHitPtr getRawRpcHit(const MuonFixedId& id) const;

        /** add associated raw mdt hit */
        void addMdtRawHit(MdtCalibRawHitPtr hit) {
            if (!hit) return;
            m_associatedRawMdtHits[hit->identify()] = hit;
        }

        /** add associated raw csc hit */
        void addCscRawHit(CscCalibRawHitPtr hit) {
            if (!hit) return;
            m_associatedRawCscHits[hit->identify()] = hit;
        }

        /** add associated raw rpc hit */
        void addRpcRawHit(RpcCalibRawHitPtr hit) {
            if (!hit) return;
            m_associatedRawRpcHits[hit->identify()] = hit;
        }

        /** add associated raw tgc hit */
        void addTgcRawHit(TgcCalibRawHitPtr hit) {
            if (!hit) return;
            m_associatedRawTgcHits[hit->identify()] = hit;
        }

    private:
        /** maps of raw hits on track */
        std::map<MuonFixedId, CscCalibRawHitPtr> m_associatedRawCscHits;
        std::map<MuonFixedId, MdtCalibRawHitPtr> m_associatedRawMdtHits;
        std::map<MuonFixedId, RpcCalibRawHitPtr> m_associatedRawRpcHits;
        std::map<MuonFixedId, TgcCalibRawHitPtr> m_associatedRawTgcHits;
    };

    inline MuonCalibRawHitAssociationMap::CscCalibRawHitPtr MuonCalibRawHitAssociationMap::getRawCscHit(const MuonFixedId& id) const {
        std::map<MuonFixedId, CscCalibRawHitPtr>::const_iterator it = m_associatedRawCscHits.find(id);
        if (it != m_associatedRawCscHits.end()) return it->second;
        return nullptr;
    }

    inline MuonCalibRawHitAssociationMap::MdtCalibRawHitPtr MuonCalibRawHitAssociationMap::getRawMdtHit(const MuonFixedId& id) const {
        std::map<MuonFixedId, MdtCalibRawHitPtr>::const_iterator it = m_associatedRawMdtHits.find(id);
        if (it != m_associatedRawMdtHits.end()) return it->second;
        return nullptr;
    }

    inline MuonCalibRawHitAssociationMap::RpcCalibRawHitPtr MuonCalibRawHitAssociationMap::getRawRpcHit(const MuonFixedId& id) const {
        std::map<MuonFixedId, RpcCalibRawHitPtr>::const_iterator it = m_associatedRawRpcHits.find(id);
        if (it != m_associatedRawRpcHits.end()) return it->second;
        return nullptr;
    }

    inline MuonCalibRawHitAssociationMap::TgcCalibRawHitPtr MuonCalibRawHitAssociationMap::getRawTgcHit(const MuonFixedId& id) const {
        std::map<MuonFixedId, TgcCalibRawHitPtr>::const_iterator it = m_associatedRawTgcHits.find(id);
        if (it != m_associatedRawTgcHits.end()) return it->second;
        return nullptr;
    }

}  // namespace MuonCalib

#endif
