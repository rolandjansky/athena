/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/GapCalculator.h"

namespace HI
{
  float EtaList::s_detectorMaxEta=4.9;
  EtaList::EtaList() : m_etaVals({-s_detectorMaxEta,s_detectorMaxEta}){}

  //iterator validity checking not needed
  //behavior is guaranteed by EtaList constructor
  float getEdgeGapPos(const EtaList& etaList)
  {
    auto itr=etaList.getVals().rbegin();
    itr++;
    float eta_max=(*itr);
    return getDetectorEtaMax()-eta_max;
  }
  float getEdgeGapNeg(const EtaList& etaList)
  {

    auto itr=etaList.getVals().begin();
    itr++;
    float eta_min=(*itr);
    return getDetectorEtaMax()+eta_min;
  }
  float getSumGap(const EtaList& etaList, float minDelta)
  {
    float sumGap=0;
    auto itr1=etaList.getVals().rbegin();
    auto itr2=itr1;
    itr2++;
    for(; itr2!=etaList.getVals().rend(); itr1++, itr2++)
    {
      float deltaEta=(*itr1)-(*itr2);
      if(deltaEta > minDelta) sumGap+=deltaEta;
    }
    return sumGap;
  }

  float getSumGapPos(const EtaList& etaList, float jetEtaMax, float minDelta)
  {
    float sumGapPos=0;
    auto itr1=etaList.getVals().rbegin();
    auto itr2=itr1;
    itr2++;
    for(; itr2!=etaList.getVals().rend(); itr1++, itr2++)
    {
      float deltaEta=(*itr1)-(*itr2);
      if( (*itr2) < jetEtaMax)
      {
	deltaEta=(*itr1)-jetEtaMax;
	if(deltaEta > minDelta) sumGapPos+=deltaEta;
	break;
      }
      if(deltaEta > minDelta) sumGapPos+=deltaEta;
    }
    return sumGapPos;
  }

  float getSumGapNeg(const EtaList& etaList, float jetEtaMin, float minDelta)
  {
    float sumGapNeg=0;
    auto itr1=etaList.getVals().begin();
    auto itr2=itr1;
    itr2++;
    for(; itr2!=etaList.getVals().end(); itr1++, itr2++)
    {
      float deltaEta=(*itr2)-(*itr1);
      if( (*itr2) > jetEtaMin)
      {
	deltaEta=jetEtaMin-(*itr1);
	if(deltaEta > minDelta) sumGapNeg+=deltaEta;
	break;
      }
      if(deltaEta > minDelta) sumGapNeg+=deltaEta;
    }
    return sumGapNeg;
  }


}
