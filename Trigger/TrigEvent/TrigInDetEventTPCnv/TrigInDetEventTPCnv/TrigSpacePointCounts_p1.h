/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @class  : TrigSpacePointCounts_p1
 *
 * @brief persistent partner for TrigSpacePointCounts
 *
 * @author Regina Kwee at cern dot ch                  - CERN/HUB
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P1_H
#define TRIGINDETEVENTTPCNV_TRIGSPACEPOINTCOUNTS_P1_H

class TrigSpacePointCounts_p1 
{
  friend class TrigSpacePointCountsCnv_p1;
  friend class TrigSpacePointCounts_p1_old_cnv;

 public:
  
  TrigSpacePointCounts_p1() = default;
  virtual ~TrigSpacePointCounts_p1() = default;
  
  int m_pixSPcnt{0};
  int m_pixCL1cnt{0};
  int m_pixCL2cnt{0};
  int m_pixCLmin3cnt{0};
  int m_SPpixBarr_cnt{0};
  int m_SPpixECAcnt{0};
  int m_SPpixECCcnt{0};

  int m_sctSPcnt{0};
  int m_SPsctBarr_cnt{0};
  int m_SPsctECAcnt{0};
  int m_SPsctECCcnt{0};

};

#endif
