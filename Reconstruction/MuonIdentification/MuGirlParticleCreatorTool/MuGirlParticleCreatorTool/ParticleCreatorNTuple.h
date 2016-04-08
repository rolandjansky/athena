/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSPARTICLECREAQTORNTUPLE_H
#define MUGIRLNSPARTICLECREAQTORNTUPLE_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

namespace MuGirlNS
{

class CandidateSummary;
class IPerformanceTruthTool;

class ParticleCreatorNTuple
{
public:
    ParticleCreatorNTuple(MsgStream& log);
    StatusCode book(NTuple::Tuple* pNTuple);
    StatusCode fillEventInfo(unsigned nRunNumber, unsigned nEventNumber);
    StatusCode fill(const CandidateSummary* pSummary,
                    bool doTruth,
                    const ToolHandle<IPerformanceTruthTool> &pTruthTool);

private:
    void reset();

    MsgStream& m_log;

    NTuple::Item<long>    m_RunNumber;          /**< The run number */
    NTuple::Item<long>    m_EventNumber;        /**< The event number */

    NTuple::Item<long>    m_NumMdtHits;
    NTuple::Item<long>    m_NumRpcPhiHits;
    NTuple::Item<long>    m_NumRpcEtaHits;
    NTuple::Item<long>    m_NumCscPhiHits;
    NTuple::Item<long>    m_NumCscEtaHits;
    NTuple::Item<long>    m_NumMdtSegs;
    NTuple::Item<long>    m_NumRpcSegs;
    NTuple::Item<long>    m_NumMdtHitsInSeg;
    NTuple::Item<long>    m_NumRpcHitsInSeg;
    NTuple::Item<long>    m_NumTgcRpcPhiHits;
    NTuple::Item<long>    m_NumTgcRpcEtaHits;
    NTuple::Item<long>    m_NumTgcRpcSegs;
    NTuple::Item<long>    m_NumTgcRpcHitsInSeg;
    NTuple::Item<long>    m_NumTgcPhiHits;
    NTuple::Item<long>    m_NumTgcEtaHits;
    NTuple::Item<long>    m_NumTgcSegs;
    NTuple::Item<long>    m_NumTgcHitsInSeg;
    NTuple::Item<long>    m_NumCscSegs;
    NTuple::Item<float>   m_IDEta;
    NTuple::Item<float>   m_IDPhi;
    NTuple::Item<float>   m_IDPt;
    NTuple::Item<float>   m_RefitEta;
    NTuple::Item<float>   m_RefitPhi;
    NTuple::Item<float>   m_RefitPt;
    NTuple::Item<float>   m_ChiSq;
    NTuple::Item<long>    m_Dof;
    NTuple::Item<float>   m_AnnBarrel;
    NTuple::Item<float>   m_AnnEndcap;
    NTuple::Item<float>   m_InnAngle;
    NTuple::Item<float>   m_InnDelta;
    NTuple::Item<float>   m_MidAngle;
    NTuple::Item<float>   m_MidDelta;
    NTuple::Item<long>    m_PdgID;
    NTuple::Item<float>   m_PassNNsel;
    NTuple::Item<float>   m_Beta;
    NTuple::Item<float>   m_Lhr;
    NTuple::Item<long>    m_SaveAnn;
    NTuple::Item<long>    m_SaveStau;
};

}

#endif //MUGIRLNSPARTICLECREAQTORNTUPLE_H
