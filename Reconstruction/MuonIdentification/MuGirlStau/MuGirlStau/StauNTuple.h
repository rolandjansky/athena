/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STAUNTUPLE_H
#define STAUNTUPLE_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

class INTupleSvc;

namespace MuGirlNS
{
    class StauTool;

    class StauNTuple
    {
    public:
        StauNTuple(MsgStream& log);
        StatusCode book(NTuple::Tuple* pNTuple);
        StatusCode fillStau(StauTool* pStau);

    private:
        MsgStream& m_log;

        NTuple::Item<float> m_beta;
        NTuple::Item<float> m_mass;
        NTuple::Item<float> m_tzero;

        NTuple::Item<float> m_betaAll;
        NTuple::Item<float> m_betaAllt;

        NTuple::Item<float> m_betaAvgRpc;
        NTuple::Item<float> m_betaAvgTileCal;

        NTuple::Item<long> m_fcnType; /** 0-barrel, 1-endcap */

        int m_maxSteps; /** number of steps */
        NTuple::Item<long> m_steps;

        NTuple::Array<float> m_stepBeta; /** the beta of the step   */
        NTuple::Array<float> m_stepChi2; /** the chi2 of the step   */
        NTuple::Array<float> m_stepDoF;

        NTuple::Array<float> m_mdtStepChi2; /** the mdt contribution to the chi2   */
        NTuple::Array<float> m_mdtTotNumHits;
        NTuple::Array<float> m_mdtNumSegs;

        NTuple::Array<float> m_rpcStepChi2; /** the rpc contribution to the chi2   */
        NTuple::Array<float> m_rpcTotNumHits;

        NTuple::Array<float> m_tileCalStepChi2; /** the tile cal contribution to the chi2   */
        NTuple::Array<float> m_tileCalNumCells;

        int m_maxStationNsteps;
        NTuple::Item<long> m_mdtStationNsteps;
        NTuple::Item<long> m_rpcStationNsteps;

        NTuple::Array<float> m_mdtStepStationChi2; /** the mdt station contribution to the chi2   */
        NTuple::Array<float> m_mdtStepStationBeta;
        NTuple::Array<float> m_mdtStationNumHits;
        NTuple::Array<float> m_mdtStationNumSegs;
        NTuple::Array<float> m_mdtStationDistance;
        NTuple::Array<float> m_mdtStationSegEta;
        NTuple::Array<float> m_mdtStationSegPhi;

        NTuple::Array<float> m_rpcStepStationChi2; /** the rpc station contribution to the chi2   */
        NTuple::Array<float> m_rpcStepStationBeta;
        NTuple::Array<float> m_rpcStationNumHits;

        int m_maxRpcHits;
        NTuple::Item<long> m_rpcHits;
        NTuple::Array<float> m_rpcHitDistance;
        NTuple::Array<float> m_rpcHitMeasuredTime;
        NTuple::Array<float> m_rpcHitPropagationTime;
        NTuple::Array<float> m_rpcHitX;
        NTuple::Array<float> m_rpcHitY;
        NTuple::Array<float> m_rpcHitZ;
        NTuple::Array<float> m_rpcHitIsEta;
    };
} // namespace MuGirlNS

#endif //STAUNTUPLE_H
