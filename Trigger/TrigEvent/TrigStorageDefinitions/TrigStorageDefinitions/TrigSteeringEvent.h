/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGSTEERINGEVENT
#define TRIGSTORAGEDEF_TRIGSTEERINGEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

//need to fwd declare namespace classes already here
namespace LVL1{
  struct RecJetRoI;
  struct RecJetEtRoI;
  struct RecEmTauRoI;
  struct RecEnergyRoI;
  struct RecMuonRoI;
}

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct TrigRoiDescriptor,struct TrigRoiDescriptor,struct TrigRoiDescriptorCollection)
  HLT_REGISTER_TYPE(struct TrigSuperRoi,struct TrigSuperRoi,struct TrigSuperRoiCollection)
  HLT_REGISTER_TYPE(struct TrigPassBits,struct TrigPassBits,struct  TrigPassBitsCollection)
  HLT_REGISTER_TYPE(struct TrigPassFlags,struct TrigPassFlags,struct  TrigPassFlagsCollection)
  HLT_REGISTER_TYPE(struct TrigRoiDescriptor,struct TrigRoiDescriptorCollection,struct TrigRoiDescriptorCollection)
  HLT_REGISTER_TYPE(struct TrigOperationalInfo,struct TrigOperationalInfo,struct  TrigOperationalInfoCollection)
  HLT_REGISTER_TYPE(LVL1::RecJetRoI ,LVL1::RecJetRoI, DataVector<LVL1::RecJetRoI>)
  HLT_REGISTER_TYPE(LVL1::RecJetEtRoI,LVL1::RecJetEtRoI, DataVector<LVL1::RecJetEtRoI>)
  HLT_REGISTER_TYPE(LVL1::RecEmTauRoI, LVL1::RecEmTauRoI, DataVector<LVL1::RecEmTauRoI>)
  HLT_REGISTER_TYPE(LVL1::RecEnergyRoI, LVL1::RecEnergyRoI, DataVector<LVL1::RecEnergyRoI>)
  HLT_REGISTER_TYPE(LVL1::RecMuonRoI,LVL1::RecMuonRoI, DataVector<LVL1::RecMuonRoI>)
HLT_END_TYPE_REGISTRATION(TrigSteeringEvent)

#endif
