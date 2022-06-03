/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/BasicCellBuilderTool.h"
#include "FastCaloSim/CellInfoContainer.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"

#include "GaudiKernel/StatusCode.h"

#include <iostream>
#include <cmath>

BasicCellBuilderTool::BasicCellBuilderTool(
                                           const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
  : base_class(type, name, parent)
{
  for(int i=CaloCell_ID_FCS::FirstSample;i<CaloCell_ID_FCS::MaxSample;++i) m_isCaloBarrel[i]=true;
  for(int i=CaloCell_ID_FCS::PreSamplerE;i<=CaloCell_ID_FCS::EME3;++i) m_isCaloBarrel[i]=false;
  m_isCaloBarrel[CaloCell_ID_FCS::TileGap3]=false;
  for(int i=CaloCell_ID_FCS::HEC0;i<=CaloCell_ID_FCS::HEC3;++i) m_isCaloBarrel[i]=false;
  for(int i=CaloCell_ID_FCS::FCAL0;i<=CaloCell_ID_FCS::FCAL2;++i) m_isCaloBarrel[i]=false;
}

double BasicCellBuilderTool::deta(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const
{
  int side=0;
  if(eta>0) side=1;

  double mineta=cont->getMinEtaSample(side,sample);
  double maxeta=cont->getMaxEtaSample(side,sample);

  if(eta<mineta) {
    return fabs(eta-mineta);
  } else if(eta>maxeta) {
    return fabs(eta-maxeta);
  } else {
    double d1=fabs(eta-mineta);
    double d2=fabs(eta-maxeta);
    if(d1<d2) return -d1;
    else return -d2;
  }
}

void BasicCellBuilderTool::minmaxeta(CaloCell_ID_FCS::CaloSample sample,double eta,double& mineta,double& maxeta, const CellInfoContainer* cont) const
{
  int side = eta>0 ? 1 : 0;

  mineta=cont->getMinEtaSample(side,sample);
  maxeta=cont->getMaxEtaSample(side,sample);
}

double BasicCellBuilderTool::rmid(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const
{
  int side = eta>0 ? 1 : 0;

  return cont->getRmidMap(side,sample).find_closest(eta)->second;
}

double BasicCellBuilderTool::zmid(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const
{
  int side = eta>0 ? 1 : 0;

  return cont->getZmidMap(side,sample).find_closest(eta)->second;
}

double BasicCellBuilderTool::rzmid(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const
{
  int side = eta>0 ? 1 : 0;

  if(isCaloBarrel(sample)) {
    return cont->getRmidMap(side,sample).find_closest(eta)->second;
  }
  else {
    return cont->getZmidMap(side,sample).find_closest(eta)->second;
  }
}

double BasicCellBuilderTool::rent(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const
{
  int side = eta>0 ? 1 : 0;

  return cont->getRentMap(side,sample).find_closest(eta)->second;
}

double BasicCellBuilderTool::zent(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const
{
  int side = eta>0 ? 1 : 0;

  return cont->getZentMap(side,sample).find_closest(eta)->second;
}

double BasicCellBuilderTool::rzent(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const
{
  int side = eta>0 ? 1 : 0;

  if(isCaloBarrel(sample)) {
    return cont->getRentMap(side,sample).find_closest(eta)->second;
  }
  else {
    return cont->getZentMap(side,sample).find_closest(eta)->second;
  }
}



StatusCode
BasicCellBuilderTool::process (CaloCellContainer* theCellContainer,
                               const EventContext& /*ctx*/) const
{
  ATH_MSG_INFO("Executing start size=" <<theCellContainer->size());

  ATH_MSG_INFO("Executing finished : end size=" <<theCellContainer->size());
  return StatusCode::SUCCESS;
}
