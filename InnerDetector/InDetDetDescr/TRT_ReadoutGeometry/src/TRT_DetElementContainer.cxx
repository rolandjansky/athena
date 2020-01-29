
/*                                                                                                                                          
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"

namespace InDetDD{

  TRT_DetElementContainer::TRT_DetElementContainer():
    m_trtcoll(nullptr),
    m_trtnum(nullptr)
  {

    for (unsigned int ec=0;ec<2;ec++) {
      for (unsigned int mod=0;mod<NMODMAX;mod++) {
	for (unsigned int phi=0; phi<NPHIMAX;phi++) {
	  for (unsigned int sLay=0;sLay<NSTRAWLAYMAXBR;sLay++) {
	    m_baArray[ec][mod][phi][sLay]=nullptr;
	  }
	}
      }
    }
    for (unsigned int ec=0;ec<2;ec++) {
      for (unsigned int whe=0;whe<NWHEELMAX;whe++) {
	for (unsigned int sLay=0;sLay<NSTRAWLAYMAXEC;sLay++) {
	  for(unsigned int phi=0;phi<NPHIMAX;phi++) {
	    m_ecArray[ec][whe][sLay][phi]=nullptr;
	  }
	}
      }
    }
  }


  TRT_DetElementContainer::~TRT_DetElementContainer()
  {
  }


  void TRT_DetElementContainer::setDetElementCollection(TRT_DetElementCollection* mytrtcoll) {
    m_trtcoll=mytrtcoll;
  }

  void TRT_DetElementContainer::setNumerology(const TRT_Numerology* mynum) {
    m_trtnum=mynum;
  }

  const TRT_DetElementCollection* TRT_DetElementContainer::getElements() const
  { 
    return m_trtcoll;
  }

  const TRT_Numerology* TRT_DetElementContainer::getTRTNumerology() const
  {
    return m_trtnum;
  }

  const TRT_BarrelElement* TRT_DetElementContainer::getBarrelDetElement(unsigned int positive,
								 unsigned int moduleIndex,
								 unsigned int phiIndex,
								 unsigned int strawLayerIndex) const {
    if ( positive >= 2 || moduleIndex >= NMODMAX
	 || phiIndex>=NPHIMAX || strawLayerIndex >= NSTRAWLAYMAXBR) return nullptr;

    return m_baArray[positive][moduleIndex][phiIndex][strawLayerIndex];
  }

  const TRT_EndcapElement* TRT_DetElementContainer::getEndcapDetElement(unsigned int positive,
								 unsigned int wheelIndex,
								 unsigned int strawLayerIndex,
								 unsigned int phiIndex) const {

    if ( positive >= 2 || wheelIndex >= NWHEELMAX
	 || phiIndex>=NPHIMAX || strawLayerIndex >= NSTRAWLAYMAXEC) return nullptr;

    return m_ecArray[positive][wheelIndex][strawLayerIndex][phiIndex];
  }

  void TRT_DetElementContainer::addBarrelElement(TRT_BarrelElement *barrel) 
  {

    m_baArray
      [barrel->getCode().isPosZ()]
      [barrel->getCode().getModuleIndex()]
      [barrel->getCode().getPhiIndex()]
      [barrel->getCode().getStrawLayerIndex()]
      =barrel;
  }

  void TRT_DetElementContainer::addEndcapElement(TRT_EndcapElement *endcap)
  {

    m_ecArray
      [endcap->getCode().isPosZ()]
      [endcap->getCode().getWheelIndex()]
      [endcap->getCode().getStrawLayerIndex()]
      [endcap->getCode().getPhiIndex()]
      =endcap;
  }
     
}
