/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloRec/CaloProtoCluster.h"
#include "CaloEvent/CaloPrefetch.h"
#include <cmath>
//#include "xAODCaloEvent/CaloClusterKineHelper.h"

CaloProtoCluster::CaloProtoCluster(const CaloCellContainer* cellCont) :
  m_cellLinks(new CaloClusterCellLink(cellCont)),
  m_haveKine(false),
  m_e(0.),
  m_et(0.)
{}

CaloProtoCluster::CaloProtoCluster(const DataLink<CaloCellContainer>& cellCont) :
  m_cellLinks(new CaloClusterCellLink(cellCont)),
  m_haveKine(false),
  m_e(0.),
  m_et(0.)
{}

CaloProtoCluster::CaloProtoCluster(const CaloClusterCellLink* cellLink) :
  m_cellLinks(new CaloClusterCellLink(*cellLink)),
  m_haveKine(false),
  m_e(0.),
  m_et(0.) 
{}


void CaloProtoCluster::getKine() {

  //Using sinTh saves only 2sec on 1500 events 

  m_e=0;
  double absEnergySum=0;
  double etaSum=0;
  //double sinThSum=0;
  const CaloClusterCellLink* lnk=m_cellLinks.get();
  CaloClusterCellLink::const_iterator it=lnk->begin();
  CaloClusterCellLink::const_iterator it_e=lnk->end();

  for (;it!=it_e;++it) 
    CxxUtils::prefetchObj(*it);

  it=lnk->begin();
  for (;it!=it_e;++it) {

    CaloPrefetch::nextDDE(it,it_e);

    const CaloCell* cell=*it;
    double cellEta=cell->eta();
    //double cellSinTh=cell->sinTh();
    double cellE=cell->e();
    double cellW=it.weight();
    double absEW=cellW*std::fabs(cellE);
    


    m_e+=cellE*cellW;
    absEnergySum+=absEW;
    etaSum += absEW * cellEta;
    //sinThSum+= absEW * cellSinTh;


  }
  double eta=etaSum/absEnergySum;
  //double sinTh=sinThSum/absEnergySum;

  //Check difference (numerical precision)
  //std::cout << "eta=" << eta << ", sinTh=" << sinTh << ", 1/std::cosh(std::fabs(eta))=" << 1/std::cosh(std::fabs(eta))
  //	    << ", diff=" << sinTh- 1/std::cosh(std::fabs(eta)) <<  std::endl; 

  double sinTh = 1.0 / std::cosh( eta );
  //m_et=m_e/std::cosh(std::fabs(eta));
  m_et=m_e*sinTh;
  m_haveKine=true;
}

