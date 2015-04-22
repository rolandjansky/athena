/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StauTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSSTAURPC_H
#define MUGIRLNSSTAURPC_H

#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauTool.h"
#include "MuGirlStau/StauCalibration.h"
#include "CLHEP/Random/RandomEngine.h"

namespace MuGirlNS
{
    class StauRPC
    {
    public:
        StauRPC(StauTool* pStau, MsgStream& log,
                CLHEP::HepRandomEngine& randEngine,
                const MuGirlNS::RIO_OnTrackLists& rpcHitsInSegments);

        /** default destructor */
        virtual ~StauRPC();

        void initStepData(RpcStepData* pRpcStepData, double beta, double tTrack = 0);
        void clearStepData(RpcStepData* pRpcStepData);
        void printStepData(RpcStepData* pRpcStepData);
        void processRpcWithBeta(double currentBeta, RpcStepData* pRpcStepData);
        void processRpcWithTTrack(double tTrack, RpcStepData* pRpcStepData, double beta = 1.0);
        unsigned int rpcMinIsAt();
        void init();
        void clear();
        StauRpcHitsLists* rpcHitsByStations()
        {
            return m_rpcHitsByStations;
        }
        void averageBeta(); //calculate the average beta and the rms
        double betaMinTime();
        void setBeta(double beta)
        {
            m_beta = beta;
        }
        double beta()
        {
            return m_beta;
        }
        double avgBeta()
        {
            return m_avgBeta;
        }
        double rmsBeta()
        {
            return m_rmsBeta;
        }
        bool hasHits()
        {
            return m_hasHits;
        }
        StauRpcHitsLists* copyStauRpcHitsLists();
        inline StauHits getHits()
        {
            return m_hits;
        }

    private:
        StauTool* m_pStau;
        MsgStream& m_log;

        /**  */
        bool m_addMuToF;

        StauRpcHitsLists* m_rpcHitsByStations;

        const MuonGM::MuonDetectorManager* m_pMuonMgr; /* the muon detector manager*/

        double rpcStationChi2(double currentBeta, StauRpcHitsList* pHitsList, int& dof,
                double tTrack = 0);
        double m_beta; //The beta of the RPC - obtained from the minimization process
        double m_avgBeta; //
        double m_rmsBeta; //
        bool m_hasHits;
        StauHits m_hits;

        StauCalibration::id_calibration_map* m_pCalibration;

        CLHEP::HepRandomEngine& m_randEngine;
    };

} // end of namespace

#endif
