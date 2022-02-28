/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFAData_v1.cxx 693858 2015-09-09 10:30:15Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/ZdcModule_v1.h"

namespace xAOD {

   ZdcModule_v1::ZdcModule_v1()
      : SG::AuxElement() {

   }

  // simple types
  //AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1, uint32_t, id,setId) // remove oct 29, 2021
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,int, id,setId)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,int,side,setSide)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,int,zdcModule,setZdcModule)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,int,channel,setChannel)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,int,type,setType)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,amplitude,setAmplitude)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,energy,setEnergy)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,time,setTime)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,qual,setQual)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,amplitudeG0,setAmplitudeG0)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,energyG0,setEnergyG0)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,timeG0,setTimeG0)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,qualG0,setQualG0)

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,amplitudeG1,setAmplitudeG1)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,energyG1,setEnergyG1)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,timeG1,setTimeG1)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(ZdcModule_v1,float,qualG1,setQualG1)

  AUXSTORE_OBJECT_SETTER_AND_GETTER(ZdcModule_v1,std::vector<float>,waveformTime,setWaveformTime)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(ZdcModule_v1,std::vector<float>,waveformADC,setWaveformADC)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(ZdcModule_v1,std::vector<float>,waveformTimeG0,setWaveformTimeG0)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(ZdcModule_v1,std::vector<float>,waveformADCG0,setWaveformADCG0)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(ZdcModule_v1,std::vector<float>,waveformTimeG1,setWaveformTimeG1)
  AUXSTORE_OBJECT_SETTER_AND_GETTER(ZdcModule_v1,std::vector<float>,waveformADCG1,setWaveformADCG1)

  // LocRecEvCollection & LocRecODEvCollection
  AUXSTORE_OBJECT_SETTER_AND_GETTER( ZdcModule_v1, ElementLink< TriggerTowerContainer >,
				     TTg0d0Link, setTTg0d0Link )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( ZdcModule_v1, ElementLink< TriggerTowerContainer >,
				     TTg0d1Link, setTTg0d1Link )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( ZdcModule_v1, ElementLink< TriggerTowerContainer >,
				     TTg1d0Link, setTTg1d0Link )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( ZdcModule_v1, ElementLink< TriggerTowerContainer >,
				     TTg1d1Link, setTTg1d1Link )
} // namespace xAOD
