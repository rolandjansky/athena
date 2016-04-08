/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrack.h"

void TrigInDetTrack::fillSiHitInfo() 
{
  eraseRdoList();
  if(m_siSpacePoints==NULL) return;
  m_NPixelSpacePoints=0;m_NSCT_SpacePoints=0;m_HitPattern=0;
  for(std::vector<const TrigSiSpacePoint*>::iterator it =  m_siSpacePoints->begin();
      it!= m_siSpacePoints->end();++it)
    {
      
      std::pair < const InDet::SiCluster*, const InDet::SiCluster* > cls = (*it)->clusters();
      if(cls.second==NULL) {//i.e pixel 
	m_NPixelSpacePoints++;
      }
      else {
	m_NSCT_SpacePoints++;
      }

      long layer = (*it)->layer();
      long mask = 1 << layer;
      m_HitPattern |= mask;
    }
}

void  TrigInDetTrack::fillRdoList() {
  if(m_siSpacePoints==NULL) return;
  m_rdoList.clear();
  for(std::vector<const TrigSiSpacePoint*>::iterator it =  m_siSpacePoints->begin();
      it!= m_siSpacePoints->end();++it)
    {
      std::pair < const InDet::SiCluster*, const InDet::SiCluster* > cls = (*it)->clusters();
      if(cls.first!=NULL) {
	std::copy(cls.first->rdoList().begin(),cls.first->rdoList().end(),std::back_inserter(m_rdoList));
      }
      if(cls.second!=NULL) {
	std::copy(cls.second->rdoList().begin(),cls.second->rdoList().end(),std::back_inserter(m_rdoList));
      }
    }
}

std::string str( const TrigInDetTrack& t ) //<! printing helper
{
  std::stringstream ss;
  const TrigInDetTrackFitPar* p = t.param();
  
  ss << "AlgorithmID: "<<t.algorithmId()
     << "d0: "<<p->a0()
     <<" z0: "<<p->z0()
     <<" phi0: "<<p->phi0()
     <<" eta: "<<p->eta()
     <<" pT: "<<p->pT()
     <<" chi2: "<<t.chi2()
     <<" NpixSPs: "<<t.NPixelSpacePoints()
     <<" NsctSPs: "<<t.NSCT_SpacePoints()
     <<" NstrawHits: "<<t.NStrawHits()
     <<" HitPattern: "<<t.HitPattern();
  return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigInDetTrack& t ) //<! printing helper (wraps above)
{
  m << str(t);
  return m;
}

bool operator== ( const TrigInDetTrack& a, const TrigInDetTrack& b )
{
  const double epsilon = 1e-8;

  const TrigInDetTrackFitPar* pA = a.param();
  const TrigInDetTrackFitPar* pB = b.param();

  if(a.algorithmId() != b.algorithmId()) return false;
  if(a.HitPattern() != b.HitPattern()) return false;
  if(a.NStrawHits() != b.NStrawHits()) return false;
  if(fabs(a.chi2()-b.chi2())>epsilon) return false;
  if(fabs(pA->a0()-pB->a0())>epsilon) return false;
  if(fabs(pA->z0()-pB->z0())>epsilon) return false;
  if(fabs(pA->phi0()-pB->phi0())>epsilon) return false;
  if(fabs(pA->eta()-pB->eta())>epsilon) return false;
  if(fabs(pA->pT()-pB->pT())>epsilon) return false;
  return true;
}

void diff( const TrigInDetTrack& a, const TrigInDetTrack& b, std::map<std::string, double>& variableChange )
{
  const TrigInDetTrackFitPar* pA = a.param();
  const TrigInDetTrackFitPar* pB = b.param();

  variableChange["d0"] = pA->a0() - pB->a0();
  variableChange["z0"] = pA->z0() - pB->z0();
  variableChange["phi0"] = pA->phi0() - pB->phi0();
  variableChange["eta"] = pA->eta() - pB->eta();
  variableChange["pT"] = pA->pT() - pB->pT();
  variableChange["chi2"] = a.chi2() - b.chi2();
}

