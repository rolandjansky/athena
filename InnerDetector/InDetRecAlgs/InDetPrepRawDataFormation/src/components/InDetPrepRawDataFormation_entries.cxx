/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawDataFormation/PixelClusterization.h"
#include "InDetPrepRawDataFormation/SCT_Clusterization.h"
#include "InDetPrepRawDataFormation/TRT_RIO_Maker.h"
#include "../CacheCreator.h"
#include "InDetPrepRawDataFormation/InDetToXAODClusterConversion.h"
#include "../SiDetectorElementStatusAlg.h"
#include "../SiDetectorElementStatusCondAlg.h"

DECLARE_COMPONENT( InDet::PixelClusterization )
DECLARE_COMPONENT( InDet::SCT_Clusterization )
DECLARE_COMPONENT( InDet::TRT_RIO_Maker )
DECLARE_COMPONENT( InDet::CacheCreator )
DECLARE_COMPONENT( InDet::InDetToXAODClusterConversion )
DECLARE_COMPONENT( InDet::SiDetectorElementStatusAlg )
DECLARE_COMPONENT( InDet::SiDetectorElementStatusCondAlg )

