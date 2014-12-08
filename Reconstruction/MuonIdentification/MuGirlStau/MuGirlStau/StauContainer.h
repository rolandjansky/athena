/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSSTAUCONTAINER_H
#define MUGIRLNSSTAUCONTAINER_H

#include "GaudiKernel/MsgStream.h"
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include "MuGirlInterfaces/CandidateSummary.h"
#include "TrkTrack/Track.h"
#include "MuGirlStau/StauToolDefinitions.h"

namespace MuGirlNS
{
    class StauContainer
    {
    public:
        StauContainer();

        /** default destructor */
        ~StauContainer();

        std::string toString() const;

        enum ReconstructionParameters
        {
            RP_ID, RP_MF
        };

        enum ConsistencyParameters
        {
            CP_WRPC, CP_NORPC
        };

        struct Consistency
        {
            ConsistencyParameters params;
            double betaAll;
            double betaAllt;

            Consistency() :
                    params(CP_WRPC), betaAll(StauBetaDefault), betaAllt(StauBetaDefault)
            {
            }
        };

        inline std::string reconstructionParameters2Str(const enum ReconstructionParameters& p) const
        {
            switch (p)
            {
            case RP_ID: return "RP_ID";
            case RP_MF: return "RP_MF";
            default: return "N/A";
            }
        };

        inline std::string consistencyParameters2Str(const enum ConsistencyParameters& p) const
        {
            switch (p)
            {
            case CP_WRPC: return "CP_WRPC";
            case CP_NORPC: return "CP_NORPC";
            default: return "N/A";
            }
        };

        inline void setTechBetaAvg(std::map<StauTechnology, BetaAvg> techBetaAvg)
        {
            m_techBetaAvg = techBetaAvg;
        }
        inline void setTechBetaChi2(std::map<StauTechnology, BetaChi2> techBetaChi2)
        {
            m_techBetaChi2 = techBetaChi2;
        }
        inline void setTechContribution2Chi2(
                std::map<StauTechnology, BetaChi2> techContribution2Chi2)
        {
            m_techContribution2Chi2 = techContribution2Chi2;
        }
        inline void setReconstructionParameters(ReconstructionParameters rp)
        {
            m_rp = rp;
        }
        inline void setHasRpcHits(bool hasRpcHits)
        {
            m_hasRpcHits = hasRpcHits;
        }
        inline void setHasTileCells(bool hasTileCells)
        {
            m_hasTileCells = hasTileCells;
        }
        inline void setBeta(double beta)
        {
            m_beta = beta;
        }
        inline void setIdTrk(const Trk::Track* idTrk)
        {
            m_pIdTrack = idTrk;
        }
        inline void setMuonTrk(const Trk::Track* muonTrk)
        {
            m_pMuonTrack = muonTrk;
        }
        inline void setStauTrk(const Trk::Track* stauTrk)
        {
            m_pStauTrack = stauTrk;
        }
        inline void setAnn(double ann)
        {
            m_ann = ann;
        }
        void setRpcHitsByStation(StauRpcHitsLists* rpcHitsByStation); // { m_rpcHitsByStation = rpcHitsByStation; }
        void setTileCells(StauTileCalCells* pCaloCells);
        inline void setMfP(Amg::Vector3D p)
        {
            m_mfP = p;
        }
        inline void setMuonP(Amg::Vector3D p)
        {
            m_muonP = p;
        }
        inline void setStauP(Amg::Vector3D p)
        {
            m_stauP = p;
        }
        void addHits(StauHits hits);
        inline StauHits getHits() const
        {
            return m_hits;
        }

        inline bool hasRpcHits() const
        {
            return m_hasRpcHits;
        }
        inline bool hasTileCells() const
        {
            return m_hasTileCells;
        }
        inline std::vector<Consistency*>& consistencyList()
        {
            return m_consistencyList;
        }
        inline double beta() const
        {
            return m_beta;
        }
        inline Amg::Vector3D mfP() const
        {
            return m_mfP;
        }
        inline Amg::Vector3D muonP() const
        {
            return m_muonP;
        }
        inline Amg::Vector3D stauP() const
        {
            return m_stauP;
        }
        inline double ann() const
        {
            return m_ann;
        }
        inline std::map<StauTechnology, BetaAvg> techBetaAvg() const
        {
            return m_techBetaAvg;
        }
        inline std::map<StauTechnology, BetaChi2> techBetaChi2() const
        {
            return m_techBetaChi2;
        }
        inline std::map<StauTechnology, BetaChi2> techContribution2Chi2() const
        {
            return m_techContribution2Chi2;
        }

        inline const Trk::Track* idTrack() const
        {
            return m_pIdTrack;
        }
        inline const Trk::Track* muonTrack() const
        {
            return m_pMuonTrack;
        }
        inline const Trk::Track* stauTrack() const
        {
            return m_pStauTrack;
        }

        inline StauRpcHitsLists* rpcHitsByStation()
        {
            return m_rpcHitsByStation;
        }
        inline const std::vector<StauTileCalCell> caloCells() const
        {
            return m_caloCells;
        }
    private:
        ReconstructionParameters m_rp;
        bool m_hasRpcHits;
        bool m_hasTileCells;
        std::vector<Consistency*> m_consistencyList;
        double m_beta;
        const Trk::Track* m_pIdTrack;
        const Trk::Track* m_pMuonTrack;
        const Trk::Track* m_pStauTrack;
        double m_ann;
        Amg::Vector3D m_mfP;
        Amg::Vector3D m_muonP;
        Amg::Vector3D m_stauP;

        StauRpcHitsLists* m_rpcHitsByStation;
        std::vector<StauTileCalCell> m_caloCells;
        std::map<StauTechnology, BetaAvg> m_techBetaAvg;
        std::map<StauTechnology, BetaChi2> m_techBetaChi2;
        std::map<StauTechnology, BetaChi2> m_techContribution2Chi2;
        StauHits m_hits;
    };
} // end of namespace

CLASS_DEF( MuGirlNS::StauContainer , 1221873760 , 1 )
CLASS_DEF( DataVector<MuGirlNS::StauContainer> , 1194689055 , 1 )

#endif
