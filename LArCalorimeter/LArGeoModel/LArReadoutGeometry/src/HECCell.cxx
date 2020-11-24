/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVDescriptor.h"

HECCell::~HECCell()
{
}


unsigned int HECCell::getNumSubgaps() const {
  return getSubgaps().size();
}

const HECHVSubgap& HECCell::getSubgap (unsigned int i) const {
  return *getSubgaps()[i];
}


const std::vector<const HECHVSubgap*>&
HECCell::getSubgaps() const
{
  if (!m_subgaps.isValid()) {
    std::vector<const HECHVSubgap*> subgaps;
    initHV (subgaps);
    m_subgaps.set (std::move (subgaps));
  }
  return *m_subgaps.ptr();
}


void HECCell::initHV (std::vector<const HECHVSubgap*>& subgaps) const
{
  const HECHVManager& hvManager=getDescriptor()->getManager()->getHVManager();


  unsigned int iSide=getEndcapIndex();

  double phiUpper = getPhiMaxNominal();
  double phiLower = getPhiMinNominal();
  double phi=fabs(phiUpper+phiLower)/2.0;
  const CellBinning & phiBinning=hvManager.getDescriptor().getPhiBinning();
  unsigned int iPhi = int((phi - phiBinning.getStart())/phiBinning.getDelta()) + phiBinning.getFirstDivisionNumber();

  unsigned int iSampling=getSamplingIndex();

  const HECHVModule& hvMod = hvManager.getHVModule(iSide,iPhi,iSampling);
  for (unsigned int iSubgap=0;iSubgap<4;iSubgap++) {
    const HECHVSubgap& hvElec = hvMod.getSubgap(iSubgap);
    subgaps.push_back(&hvElec);
  }
} 


