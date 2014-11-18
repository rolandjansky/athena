/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigSteeringEventTPCnv/HLTResult_p1.h"
#include "TrigSteeringEventTPCnv/HLTResultCnv_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p1.h"
#include "TrigSteeringEventTPCnv/Lvl1Result_p2.h"

#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_tlp1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_tlp1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p1.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p1.h"


#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p2.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p2.h"

#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollectionCnv_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCollection_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptor_p3.h"
#include "TrigSteeringEventTPCnv/TrigRoiDescriptorCnv_p3.h"


#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollection_tlp1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollectionCnv_tlp1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfo_p1.h"

//#include "TrigSteeringEventTPCnv/HLTAODConfigData_p1.h"
//#include "TrigSteeringEventTPCnv/Lvl1AODConfigData_p1.h"
//#include "TrigSteeringEventTPCnv/Lvl1AODPrescaleConfigData_p1.h"

#include "TrigSteeringEventTPCnv/TrigPassBitsCollectionCnv_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBits_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCnv_p1.h"

#include "TrigSteeringEventTPCnv/TrigPassFlagsCollectionCnv_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlags_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCnv_p1.h"


using namespace HLT;
DECLARE_TPCNV_FACTORY(HLTResultCnv_p1,
                      HLT::HLTResult,
                      HLT::HLTResult_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigOperationalInfoCollectionCnv_tlp1,
                      TrigOperationalInfoCollection,
                      TrigOperationalInfoCollection_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigRoiDescriptorCnv_p1,
                      TrigRoiDescriptor,
                      TrigRoiDescriptor_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigRoiDescriptorCnv_p2,
                      TrigRoiDescriptor,
                      TrigRoiDescriptor_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigRoiDescriptorCnv_p3,
                      TrigRoiDescriptor,
                      TrigRoiDescriptor_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigRoiDescriptorCollectionCnv_tlp1,
                      TrigRoiDescriptorCollection,
                      TrigRoiDescriptorCollection_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigRoiDescriptorCollectionCnv_p2,
                      TrigRoiDescriptorCollection,
                      TrigRoiDescriptorCollection_p2,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigRoiDescriptorCollectionCnv_p3,
                      TrigRoiDescriptorCollection,
                      TrigRoiDescriptorCollection_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigPassBitsCnv_p1,
                      TrigPassBits,
                      TrigPassBits_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigPassBitsCollectionCnv_p1,
                      TrigPassBitsCollection,
                      TrigPassBitsCollection_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigPassFlagsCnv_p1,
                      TrigPassFlags,
                      TrigPassFlags_p1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigPassFlagsCollectionCnv_p1,
                      TrigPassFlagsCollection,
                      TrigPassFlagsCollection_p1,
                      Athena::TPCnvVers::Current)
                      
