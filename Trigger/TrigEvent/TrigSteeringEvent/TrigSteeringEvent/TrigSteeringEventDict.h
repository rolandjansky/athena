// meacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTEERINGEVENT_TRIGSTEERINGEVENTDICT_H
#define TRIGSTEERINGEVENT_TRIGSTEERINGEVENTDICT_H
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/TrigSuperRoi.h"
#include "TrigSteeringEvent/TrigSuperRoiCollection.h"
#include "TrigSteeringEvent/HLTExtraData.h"
#include "TrigSteeringEvent/GenericResult.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteeringEvent/Lvl1Item.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEvent/TrigPassFlagsCollection.h"
#include "TrigSteeringEvent/StringSerializer.h"

void dummy_function_for_TrigSteeringEvent_which_forces_dictionaries_generation ( 
   TrigRoiDescriptorCollection   troi,   std::vector<TrigRoiDescriptor*> troi_vec,   std::vector<const TrigRoiDescriptor*> troi_const_vec,
   TrigSuperRoiCollection        tsroi,  std::vector<TrigSuperRoi*>      tsroi_vec,  std::vector<const TrigSuperRoi*>      tsroi_const_vec,
   TrigOperationalInfoCollection toi,    std::vector<LVL1CTP::Lvl1Item*> test_vec,   std::vector<const LVL1CTP::Lvl1Item*> test_const_vec,
   TrigPassBitsCollection        tbits,  std::vector<TrigPassBits*>      tbits_vec,  std::vector<const TrigPassBits*>      tbits_const_vec,
   TrigPassFlagsCollection       tflags, std::vector<TrigPassFlags*>     tflags_vec, std::vector<const TrigPassFlags*>     tflags_const_vec  ) 
{

  TrigRoiDescriptorCollection aatroi = troi; 
  std::vector<TrigRoiDescriptor*> aatroi_vec = troi_vec; 
  std::vector<const TrigRoiDescriptor*> aatroi_const_vec = troi_const_vec; 

  TrigSuperRoiCollection aatsroi = tsroi; 
  std::vector<TrigSuperRoi*> aatsroi_vec = tsroi_vec; 
  std::vector<const TrigSuperRoi*> aatsroi_const_vec = tsroi_const_vec; 


  TrigOperationalInfoCollection aatoi = toi; 
  std::vector<LVL1CTP::Lvl1Item*> test1 = test_vec;
  std::vector<const LVL1CTP::Lvl1Item*> test2 = test_const_vec;

  
  TrigPassBitsCollection aatbits = tbits;
  std::vector<TrigPassBits*> aatbits_vec= tbits_vec;
  std::vector<const TrigPassBits*> aatbits_const_vec = tbits_const_vec;
  
  TrigPassFlagsCollection aatflags = tflags;
  std::vector<TrigPassFlags*> aatflags_vec= tflags_vec;
  std::vector<const TrigPassFlags*> aatflags_const_vec = tflags_const_vec;
  
  //HLT::RegisterType<TrigRoiDescriptor, DataVector<TrigRoiDescriptor> >::instan();
  //  HLT::RegisterType<TrigOperationalInfo, DataVector<TrigOperationalInfo> >::instan();
  //  std::string key;
  //  nav.attachFeature(0,(TrigRoiDescriptor*)0, HLT::Navigation::ObjectInStoreGate, key);
  //  nav.attachFeature(0,(TrigOperationalInfo*)0, HLT::Navigation::ObjectInStoreGate, key);
  //  nav.registerObject<TrigRoiDescriptor, DataVector<TrigRoiDescriptor> >();
  //  nav.registerObject<TrigOperationalInfo, DataVector<TrigOperationalInfo> >(); 
}



#endif
