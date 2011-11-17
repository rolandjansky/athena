/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCTPCNV_ZDCTPCNVDICT_H
#define ZDCTPCNV_ZDCTPCNVDICT_H

#include "ZdcEventTPCnv/ZdcDigits_p1.h"
#include "ZdcEventTPCnv/ZdcDigitsCnv_p1.h"
#include "ZdcEventTPCnv/ZdcDigitsCollection_p1.h"
#include "ZdcEventTPCnv/ZdcDigitsCollectionCnv_p1.h"
#include "ZdcEventTPCnv/ZdcRawChannel_p1.h"
#include "ZdcEventTPCnv/ZdcRawChannelCnv_p1.h"
#include "ZdcEventTPCnv/ZdcRawChannelCollection_p1.h"
#include "ZdcEventTPCnv/ZdcRawChannelCollectionCnv_p1.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_p1.h"
#include "ZdcEventTPCnv/ZDC_SimStripHitCnv_p1.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_Collection_p1.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_CollectionCnv_p1.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_p1.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHitCnv_p1.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_Collection_p1.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_CollectionCnv_p1.h"

/*
namespace ZDC_EventTPCnv_Dict {
  std::vector<ZDC_SimStripHit_p1>                  zdc0;
  std::vector<ZDC_SimPixelHit_p1>                  zdc1;
  std::vector<ZdcDigits_p1>                        zdc2;
  std::vector<ZdcRawChannel_p1>                    zdc3;

}
*/


struct dummy {
  T_TPCnv<ZdcDigitsCollection, ZdcDigitsCollection_p1> m_zdcDigitsCnv;
  T_TPCnv<ZdcRawChannelCollection, ZdcRawChannelCollection_p1> m_zdcRawChannelCnv;
  T_TPCnv<ZDC_SimStripHit_Collection, ZDC_SimStripHit_Collection_p1> m_zdc_SimStripHitCnv;
  T_TPCnv<ZDC_SimPixelHit_Collection, ZDC_SimPixelHit_Collection_p1> m_zdc_SimPixelHitCnv;
};

#endif 
