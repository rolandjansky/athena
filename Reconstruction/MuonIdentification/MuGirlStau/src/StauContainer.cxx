/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlStau/StauContainer.h"
#include "MuGirlInterfaces/RpcSegmentInfo.h"
#include <algorithm>
#include <iterator>

//================ Constructor =================================================

MuGirlNS::StauContainer::StauContainer() :
                m_rp(StauContainer::RP_ID),
                m_hasRpcHits(false),
                m_hasTileCells(false),
                m_beta(-100.),
                m_pIdTrack(NULL),
				m_pMuonTrack(NULL),
				m_pStauTrack(NULL),
                m_ann(-100.),
                m_mfP(0, 0, 0),
                m_muonP(0, 0, 0),
                m_stauP(0, 0, 0),
                m_rpcHitsByStation(NULL)
{
}

MuGirlNS::StauContainer::~StauContainer()
{
    if (m_rpcHitsByStation != NULL)
    {
        for (auto pHitsList : *m_rpcHitsByStation)
        {
            for (auto pHit : *pHitsList)
                delete pHit;
            pHitsList->clear();
            delete pHitsList;
        }
        m_rpcHitsByStation->clear();
        delete m_rpcHitsByStation;
    }
    for (Consistency* c : m_consistencyList)
      delete c;
}

std::ostream& operator<<(std::ostream& ostr, const std::map<MuGirlNS::StauTechnology, MuGirlNS::BetaAvg>& m)
{
    for (auto entry : m)
    {
        ostr << "    " << MuGirlNS::technologyName(entry.first) <<
                " Beta: " << entry.second.beta <<
                ", RMS: " << entry.second.rms << std::endl;
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const std::map<MuGirlNS::StauTechnology, MuGirlNS::BetaChi2>& m)
{
    for (auto entry : m)
    {
        ostr << "    " << MuGirlNS::technologyName(entry.first) <<
                " Beta: " << entry.second.beta <<
                ", Chi2: " << entry.second.chi2 <<
                ", DOF: " << entry.second.dof << std::endl;
    }
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Amg::Vector3D& v)
{
  ostr << "(" << v[Amg::px] << "," << v[Amg::py] << "," << v[Amg::pz] << ")";
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const MuGirlNS::StauRpcHit& hit)
{
    ostr << "measuredTime: " << hit.measuredTime <<
            ", propagationTime: " << hit.propagationTime <<
            ", muonToF: " << hit.muonToF <<
            ", distance: " << hit.distance <<
            ", x: " << hit.x <<
            ", y: " << hit.y <<
            ", z: " << hit.z <<
            ", error: " << hit.error <<
            ", isEta: " << (hit.isEta ? "Yes" : "No");
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const MuGirlNS::StauHit& hit)
{
    ostr << MuGirlNS::hitTechnologyName(hit.eTech) <<
            " mTof: " << hit.mToF <<
            ", x: " << hit.x <<
            ", y: " << hit.y <<
            ", z: " << hit.z <<
            ", e: " << hit.e <<
            ", error: " << hit.error <<
            ", shift: " << hit.shift <<
            ", measuresEta: " << (hit.measuresEta ? "Yes" : "No") << 
            ", propagationTime: " << hit.propagationTime;
    return ostr;
}

std::string MuGirlNS::StauContainer::toString() const
{
    std::ostringstream ostr;
    ostr << "StauContainer:" << std::endl;
    ostr << "  ReconstructionParameters: " << reconstructionParameters2Str(m_rp) << std::endl;
    ostr << "  HasRpcHits: " << (m_hasRpcHits ? "Yes" : "No") << std::endl;
    ostr << "  HasTileCells: " << (m_hasTileCells ? "Yes" : "No") << std::endl;
    ostr << "  consistencyList (" << m_consistencyList.size() << "):" << std::endl;
    for (auto consistency : m_consistencyList)
        ostr << "    ConsistencyParameters: " << consistencyParameters2Str(consistency->params) <<
                ", betaALL: " << consistency->betaAll <<
                ", betaALLT: " << consistency->betaAllt << std::endl;
    ostr << "  Beta: " << m_beta << std::endl;
    ostr << "  Ann: " << m_ann << std::endl;
    ostr << "  MuonP: " << m_muonP << std::endl;
    ostr << "  StauP: " << m_stauP << std::endl;
    if (m_rpcHitsByStation != NULL)
    {
        ostr << "  RPC hits:" << std::endl;
        for (auto pHitList : *m_rpcHitsByStation)
            for (auto pHit: *pHitList)
                ostr << "    " << *pHit << std::endl;
    }
    ostr << "  TechBetaAvg:" << std::endl; ostr << m_techBetaAvg;
    ostr << "  TechBetaChi2:" << std::endl; ostr << m_techBetaChi2;
    ostr << "  TechContribution2Chi2:" << std::endl; ostr << m_techContribution2Chi2;
    ostr << "  Hits (" << m_hits.size() << "):" << std::endl;
    for (auto hit : m_hits)
        ostr << "    " << hit << std::endl;
    return ostr.str();
}

void MuGirlNS::StauContainer::setRpcHitsByStation(StauRpcHitsLists* rpcHitsByStation)
{
    m_rpcHitsByStation = new StauRpcHitsLists();
    for (auto pHitsList : *rpcHitsByStation)
    {
        auto pCopyList = new StauRpcHitsList();
        m_rpcHitsByStation->push_back(pCopyList);
        for (auto pHit : *pHitsList)
            pCopyList->push_back(new StauRpcHit(*pHit));
    }
}

void MuGirlNS::StauContainer::setTileCells(StauTileCalCells* pCaloCells)
{
    if (pCaloCells == NULL) return;
    for (auto i_cell : *pCaloCells)
        m_caloCells.push_back(*i_cell);
}

void MuGirlNS::StauContainer::addHits(StauHits hits)
{
    std::copy(hits.begin(), hits.end(), std::back_inserter(m_hits));
}
