/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTUncompressedHitCollection_hh
#define TRTUncompressedHitCollection_hh


#include "InDetSimEvent/TRTUncompressedHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"// for back compatibility

#include "AthenaKernel/CLASS_DEF.h"
typedef AthenaHitsVector<TRTUncompressedHit> TRTOldUncompressedHitCollection;

typedef AtlasHitsVector<TRTUncompressedHit> TRTUncompressedHitCollection;
typedef AtlasHitsVector<TRTUncompressedHit>::iterator TRTUncompressedHitIter;
typedef AtlasHitsVector<TRTUncompressedHit>::const_iterator TRTUncompressedHitConstIter;
#ifndef __CINT__
CLASS_DEF(TRTUncompressedHitCollection, 947021836, 1)
CLASS_DEF(TRTOldUncompressedHitCollection, 1163170609, 1)
#endif

#endif // TRTUncompressedHitCollection_hh
