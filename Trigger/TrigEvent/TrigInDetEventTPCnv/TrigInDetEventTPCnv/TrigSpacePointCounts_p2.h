/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCounts_p2
 *
 * @brief persistent partner for TrigSpacePointCounts
 *
 * @author Regina Kwee at cern dot ch                  - CERN/HUB
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCounts_p2.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P2_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P2_H

#include "TrigInDetEventTPCnv/TrigHisto2D_p1.h"

class TrigSpacePointCounts_p2 
{
  friend class TrigSpacePointCountsCnv_p2;

 public:
  
  TrigSpacePointCounts_p2() {}
  virtual ~TrigSpacePointCounts_p2(){}
  
  // private:

  TrigHisto2D_p1 m_pixelClusEndcapC;
  TrigHisto2D_p1 m_pixelClusBarrel;
  TrigHisto2D_p1 m_pixelClusEndcapA;
  unsigned int m_sctSpEndcapC;
  unsigned int m_sctSpBarrel;
  unsigned int m_sctSpEndcapA;

};

#endif
