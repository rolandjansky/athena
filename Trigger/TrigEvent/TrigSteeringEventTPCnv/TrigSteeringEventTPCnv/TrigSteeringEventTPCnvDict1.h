/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERINGEVENTTPCNV_TRIGSTEERINGEVENTTPCNVDICT1_H
#define TRIGSTEERINGEVENTTPCNV_TRIGSTEERINGEVENTTPCNVDICT1_H

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


#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollection_tlp1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollectionCnv_tlp1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfo_p1.h"

#include "TrigSteeringEventTPCnv/TrigPassBitsCollectionCnv_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBits_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCnv_p1.h"

#include "TrigSteeringEventTPCnv/TrigPassFlagsCollectionCnv_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollection_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlags_p1.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCnv_p1.h"

//#include "TrigSteeringEventTPCnv/HLTAODConfigData_p1.h"
//#include "TrigSteeringEventTPCnv/Lvl1AODConfigData_p1.h"
//#include "TrigSteeringEventTPCnv/Lvl1AODPrescaleConfigData_p1.h"



struct dummy_TrigSteeringEventTPCnvDict1
{

  std::vector<TrigRoiDescriptor_p1> 	      m_dummyTrigRoiDescriptor_p1;
  std::vector<TrigRoiDescriptor_p2> 	      m_dummyTrigRoiDescriptor_p2;


  std::vector<TrigOperationalInfo_p1>         m_dummyTrigOperationalInfo;

  std::vector<TrigPassBits_p1>                m_dummyTrigPassBits;
  std::vector<TrigPassBitsCollection_p1>                m_dummyTrigPassBitsCollection;

  std::vector<TrigPassFlags_p1>                m_dummyTrigPassFlags;
  std::vector<TrigPassFlagsCollection_p1>                m_dummyTrigPassFlagsCollection;

};

#endif
