/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigSpacePointCounts_p1
 *
 * @brief persistent partner for TrigSpacePointCounts
 *
 * @author Regina Kwee at cern dot ch                  - CERN/HUB
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigSpacePointCounts_p1.h,v 1.2 2009-04-01 22:08:44 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P1_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P1_H

class TrigSpacePointCounts_p1 
{
  friend class TrigSpacePointCountsCnv_p1;
  friend class TrigSpacePointCounts_p1_old_cnv;

 public:
  
  TrigSpacePointCounts_p1() {}
  virtual ~TrigSpacePointCounts_p1(){}
  
  //protected:

  int m_pixSPcnt; 
  int m_pixCL1cnt;
  int m_pixCL2cnt;
  int m_pixCLmin3cnt;
  int m_SPpixBarr_cnt;
  int m_SPpixECAcnt;
  int m_SPpixECCcnt;

  int m_sctSPcnt;
  int m_SPsctBarr_cnt;
  int m_SPsctECAcnt;
  int m_SPsctECCcnt;

};

#endif
