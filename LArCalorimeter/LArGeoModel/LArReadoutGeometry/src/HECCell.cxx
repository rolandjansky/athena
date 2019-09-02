/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVDescriptor.h"

HECCell::~HECCell()
{
}


unsigned int HECCell::getNumSubgaps() const {
  if (m_subgap.size()==0) initHV();
  return m_subgap.size();
}

const HECHVSubgap& HECCell::getSubgap (unsigned int i) const {
  if (m_subgap.size()==0) initHV();
  return *(m_subgap[i]);
}

void HECCell::initHV() const {

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
    m_subgap.push_back(&hvElec);
  }

} 


